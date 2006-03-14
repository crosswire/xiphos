/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*  This file is part of the GtkHTML library.

    Copyright (C) 2000,2001,2002 Ximian, Inc.
    Authors:  Radek Doulik (rodo@ximian.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <config.h>
#ifndef USE_GTKHTML38
#include <libgnome/gnome-i18n.h>
#include <string.h>
#include <glade/glade.h>

#include "gi-color-combo.h"
#include "gtkhtml/htmlcolor.h"
#include "gtkhtml/htmlcolorset.h"
#include "gtkhtml/htmlengine-edit.h"
#include "gtkhtml/htmlengine-edit-cut-and-paste.h"
#include "gtkhtml/htmlengine-edit-fontstyle.h"
#include "gtkhtml/htmlengine-save.h"
#include "gtkhtml/htmlselection.h"
#include "gtkhtml/htmlsettings.h"
#include "gtkhtml/htmltext.h"

#include "text.h"
#include "properties.h"
#include "utils.h"

struct _GtkHTMLEditTextProperties
{

	GSHTMLEditorControlData *cd;

	GtkWidget *combo_color;
	GtkWidget *option_size;
	GtkWidget *check_bold;
	GtkWidget *check_italic;
	GtkWidget *check_underline;
	GtkWidget *check_strikeout;

	gboolean disable_change;
};
typedef struct _GtkHTMLEditTextProperties GtkHTMLEditTextProperties;

static void
color_changed (GtkWidget *w, GdkColor *color, gboolean custom, gboolean by_user, gboolean is_default,
	       GtkHTMLEditTextProperties *d)
{
	HTMLColor *html_color;

	if (d->disable_change)
		return;

	html_color = html_color_new_from_gdk_color (color);
	gtk_html_set_color (d->cd->html, html_color);
	html_color_unref (html_color);
}

static void
set_style (GtkHTMLFontStyle mask, GtkHTMLFontStyle style, GtkHTMLEditTextProperties *d)
{
	if (d->disable_change)
		return;

	gtk_html_set_font_style (d->cd->html, mask, style);
}

static void
size_changed (GtkWidget *w, GtkHTMLEditTextProperties *d)
{
	GtkWidget *menu = gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_size));


	set_style (~GTK_HTML_FONT_STYLE_SIZE_MASK,
		   g_list_index (GTK_MENU_SHELL (menu)->children, gtk_menu_get_active (GTK_MENU (menu))) + GTK_HTML_FONT_STYLE_SIZE_1,
		   d);
}

static void
bold_changed (GtkWidget *w, GtkHTMLEditTextProperties *d)
{
	set_style (~GTK_HTML_FONT_STYLE_BOLD, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)) ? GTK_HTML_FONT_STYLE_BOLD : 0, d);
}

static void
italic_changed (GtkWidget *w, GtkHTMLEditTextProperties *d)
{
	set_style (~GTK_HTML_FONT_STYLE_ITALIC, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)) ? GTK_HTML_FONT_STYLE_ITALIC : 0, d);
}

static void
underline_changed (GtkWidget *w, GtkHTMLEditTextProperties *d)
{
	set_style (~GTK_HTML_FONT_STYLE_UNDERLINE, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)) ? GTK_HTML_FONT_STYLE_UNDERLINE : 0, d);
}

static void
strikeout_changed (GtkWidget *w, GtkHTMLEditTextProperties *d)
{
	set_style (~GTK_HTML_FONT_STYLE_STRIKEOUT, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)) ? GTK_HTML_FONT_STYLE_STRIKEOUT : 0, d);
}

static gint
get_size (GtkHTMLFontStyle s)
{
	return (s & GTK_HTML_FONT_STYLE_SIZE_MASK)
		? (s & GTK_HTML_FONT_STYLE_SIZE_MASK) - GTK_HTML_FONT_STYLE_SIZE_1
		: 2;
}

static void
set_ui (GtkHTMLEditTextProperties *d)
{
	HTMLEngine *e = d->cd->html->engine;
	HTMLColor *color = html_engine_get_color (e);

	d->disable_change = TRUE;

	if (color)
		color_combo_set_color (COLOR_COMBO (d->combo_color), &color->color);
	else
		color_combo_set_color (COLOR_COMBO (d->combo_color), NULL);

	gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_size), get_size (html_engine_get_font_style (e)));

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_bold), (html_engine_get_font_style (e) & GTK_HTML_FONT_STYLE_BOLD) != 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_italic), (html_engine_get_font_style (e) & GTK_HTML_FONT_STYLE_ITALIC) != 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_underline), (html_engine_get_font_style (e) & GTK_HTML_FONT_STYLE_UNDERLINE) != 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_strikeout), (html_engine_get_font_style (e) & GTK_HTML_FONT_STYLE_STRIKEOUT) != 0);

	d->disable_change = FALSE;
}

GtkWidget *
text_properties (GSHTMLEditorControlData *cd, gpointer *set_data)
{
	GtkHTMLEditTextProperties *d = g_new (GtkHTMLEditTextProperties, 1);
	GtkWidget *text_page;
	GladeXML *xml;

	d->cd = cd;
	*set_data = d;
	xml = glade_xml_new (GTKHTML_DATA_DIR"/gtkhtml-editor-properties.glade", "text_page", GETTEXT_PACKAGE);
	if (!xml)
		g_error (_("Could not load glade file."));

	text_page = glade_xml_get_widget (xml, "text_page");
	d->combo_color = color_combo_new (NULL, _("Automatic"), &html_colorset_get_color (cd->html->engine->defaultSettings->color_set, HTMLTextColor)->color,
					     color_group_fetch ("text_color", d->cd));
        color_combo_box_set_preview_relief (COLOR_COMBO (d->combo_color), GTK_RELIEF_NORMAL);
        g_signal_connect (d->combo_color, "color_changed", G_CALLBACK (color_changed), d);
	gtk_box_pack_start (GTK_BOX (glade_xml_get_widget (xml, "text_color_hbox")), d->combo_color, FALSE, FALSE, 0);

	d->check_bold = glade_xml_get_widget (xml, "check_bold");
	g_signal_connect (d->check_bold, "toggled", G_CALLBACK (bold_changed), d);

	d->check_italic = glade_xml_get_widget (xml, "check_italic");
	g_signal_connect (d->check_italic, "toggled", G_CALLBACK (italic_changed), d);

	d->check_underline = glade_xml_get_widget (xml, "check_underline");
	g_signal_connect (d->check_underline, "toggled", G_CALLBACK (underline_changed), d);

	d->check_strikeout = glade_xml_get_widget (xml, "check_strikeout");
	g_signal_connect (d->check_strikeout, "toggled", G_CALLBACK (strikeout_changed), d);

	d->option_size = glade_xml_get_widget (xml, "option_size");
	g_signal_connect (gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_size)), "selection-done",
			  G_CALLBACK (size_changed), d);

	gtk_widget_show_all (text_page);

	set_ui (d);

	return text_page;
}

void
text_close_cb (GSHTMLEditorControlData *cd, gpointer get_data)
{
	GtkHTMLEditTextProperties *data = (GtkHTMLEditTextProperties *) get_data;

	g_free (data);
}
#endif
