/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2004 Marco Pesenti Gritti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Marco Pesenti Gritti <marco@gnome.org>
 */

#include "mozilla-config.h"
#include "config.h"

#include <stdlib.h>
#include <gconf/gconf-client.h>

#include <nsStringAPI.h>

#ifdef HAVE_GECKO_1_9
#include <gtkmozembed_glue.cpp>
#endif

#include <gtkmozembed.h>
#include <gtkmozembed_internal.h>
#include <nsCOMPtr.h>
#include <nsIPrefService.h>
#include <nsIServiceManager.h>
#include <nsServiceManagerUtils.h>

#include "gecko/gecko-services.h"
#include "gecko/gecko-utils.h"

#include "main/sword.h"

#define KEY_GNOME_DIR           "/desktop/gnome/interface"
#define KEY_GNOME_VARIABLE_FONT KEY_GNOME_DIR "/document_font_name"
#define KEY_GNOME_FIXED_FONT    KEY_GNOME_DIR "/monospace_font_name"

static nsIPrefBranch* gPrefBranch;

static GConfClient *gconf_client = NULL;
static const char *font_languages[] = {
	"x-western",
	"ar",
	"el",
	"he",
	"ja",
	"ko",
	"th",
	"tr",
	"x-armn",
	"x-baltic",
	"x-beng",
	"x-cans",
	"x-central-euro",
	"x-cyrillic",
	"x-devanagari",
	"x-ethi",
	"x-geor",
	"x-gujr",
	"x-guru",
	"x-khmr",
	"x-mlym",
	"x-tamil",
	"x-unicode",
	"zh-CN",
	"zh-HK",
	"zh-TW"
};

static gboolean
_util_split_font_string (const gchar *font_name, gchar **name, gint *size)
{
	PangoFontDescription *desc;
	PangoFontMask mask = (PangoFontMask) (PANGO_FONT_MASK_FAMILY | PANGO_FONT_MASK_SIZE);
	gboolean retval = FALSE;

	if (!font_name) return FALSE;

	desc = pango_font_description_from_string (font_name);
	if (!desc) return FALSE;

	if ((pango_font_description_get_set_fields (desc) & mask) == mask) {
		*size = PANGO_PIXELS (pango_font_description_get_size (desc));
		*name = g_strdup (pango_font_description_get_family (desc));
		retval = TRUE;
	}

	pango_font_description_free (desc);

	return retval;
}

static gboolean
gecko_prefs_set_bool (const gchar *key, gboolean value)
{
	NS_ENSURE_TRUE (gPrefBranch, FALSE);

	return NS_SUCCEEDED(gPrefBranch->SetBoolPref (key, value));
}

static gboolean
gecko_prefs_set_string (const gchar *key, const gchar *value)
{
	NS_ENSURE_TRUE (gPrefBranch, FALSE);

	return NS_SUCCEEDED(gPrefBranch->SetCharPref (key, value));
}

static gboolean
gecko_prefs_set_int (const gchar *key, gint value)
{
	NS_ENSURE_TRUE (gPrefBranch, FALSE);

	return NS_SUCCEEDED(gPrefBranch->SetIntPref (key, value));
}

extern "C" void
gecko_set_caret (gboolean value)
{
	gecko_prefs_set_bool ("accessibility.browsewithcaret", value);
}
/*
extern "C" void
gecko_set_color (GeckoColorType type, const gchar *color)
{
	gecko_prefs_set_bool ("browser.display.use_system_colors", FALSE);
	switch (type) {
	case GS_COLOR_FG:
		gecko_prefs_set_string ("browser.display.foreground_color",
					color);
		break;
	case GS_COLOR_BG:
		gecko_prefs_set_string ("browser.display.background_color",
					color);
		break;
	case GS_COLOR_ANCHOR:
		gecko_prefs_set_string ("browser.anchor_color",
					color);
		break;
	default:
		break;
	}
}
*/
extern "C" void
gecko_set_font (GeckoFontType font_type, const gchar *fontname)
{
	char *name = NULL;
	char pref[128];
	int size = 0;
	guint i;

	if (!_util_split_font_string (fontname, &name, &size)) {
		g_free (name);
		return;
	}

	gecko_prefs_set_string ("font.size.unit", "pt");

	switch (font_type) {
	case GECKO_FONT_VARIABLE:
		for (i = 0; i < G_N_ELEMENTS (font_languages); ++i) {
			g_snprintf (pref, sizeof (pref), 
				    "font.name.variable.%s",
				    font_languages[i]);
			gecko_prefs_set_string (pref, name);
			
			g_snprintf (pref, sizeof (pref), 
				    "font.size.variable.%s",
				    font_languages[i]);
			gecko_prefs_set_int (pref, size);
			
			g_snprintf (pref, sizeof (pref), 
				    "font.minimum-size.%s",
				    font_languages[i]);
			gecko_prefs_set_int (pref, 8);
		}
		break;
	case GECKO_FONT_FIXED:
		for (i = 0; i < G_N_ELEMENTS (font_languages); ++i) {
			g_snprintf (pref, sizeof (pref), 
				    "font.name.fixed.%s",
				    font_languages[i]);
			gecko_prefs_set_string (pref, name);
			
			g_snprintf (pref, sizeof (pref), 
				    "font.size.fixed.%s",
				    font_languages[i]);
			gecko_prefs_set_int (pref, size);
		}
		
		break;
	default:
		break;
	}

	g_free (name);
}		   

extern "C" gboolean
gecko_init (void)
{
    	GError *err = NULL;
	gchar *fontname = NULL;
#ifdef HAVE_GECKO_1_9
	NS_LogInit ();
#endif

	nsresult rv;
#ifdef XPCOM_GLUE
	static const GREVersionRange greVersion = {
		"1.9a", PR_TRUE,
		"2", PR_TRUE
	};
	char xpcomLocation[PATH_MAX];
	rv = GRE_GetGREPathWithProperties(&greVersion, 1, nsnull, 0, xpcomLocation, sizeof (xpcomLocation));
	NS_ENSURE_SUCCESS (rv, FALSE);

	// Startup the XPCOM Glue that links us up with XPCOM.
	rv = XPCOMGlueStartup(xpcomLocation);
	NS_ENSURE_SUCCESS (rv, FALSE);

	rv = GTKEmbedGlueStartup();
	NS_ENSURE_SUCCESS (rv, FALSE);

	rv = GTKEmbedGlueStartupInternal();
	NS_ENSURE_SUCCESS (rv, FALSE);

	char *lastSlash = strrchr(xpcomLocation, '/');
	if (lastSlash)
		*lastSlash = '\0';

	gtk_moz_embed_set_path(xpcomLocation);

#else
#ifdef HAVE_GECKO_1_9
	gtk_moz_embed_set_path (GECKO_HOME);
#else
	gtk_moz_embed_set_comp_path (GECKO_HOME);
#endif
#endif // XPCOM_GLUE

	gtk_moz_embed_push_startup ();

#ifdef USE_GTKUPRINT
	gecko_register_printing ();
#endif
	//	nsresult rv;
	nsCOMPtr<nsIPrefService> prefService (do_GetService (NS_PREFSERVICE_CONTRACTID, &rv));
	NS_ENSURE_SUCCESS (rv, FALSE);

	rv = CallQueryInterface (prefService, &gPrefBranch);
	NS_ENSURE_SUCCESS (rv, FALSE);
	
    	gconf_client = gconf_client_get_default ();
    	fontname = gconf_client_get_string (gconf_client, KEY_GNOME_VARIABLE_FONT, &err);
	if(fontname) {
		GS_message(("var fontname %s",fontname));
		gecko_set_font (GECKO_FONT_VARIABLE, fontname);
		g_free(fontname);
	}
    	fontname = gconf_client_get_string (gconf_client, KEY_GNOME_FIXED_FONT, &err);
	if(fontname) {
		GS_message(("fixed fontname %s",fontname));
		gecko_set_font (GECKO_FONT_FIXED, fontname);
		g_free(fontname);
	}
	g_object_unref(gconf_client);
	return TRUE;
}

extern "C" void
gecko_shutdown (void)
{
	NS_IF_RELEASE (gPrefBranch);
	gPrefBranch = nsnull;

	gtk_moz_embed_pop_startup ();

#ifdef HAVE_GECKO_1_9
        NS_LogTerm ();
#endif
}
