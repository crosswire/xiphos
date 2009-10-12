/*
 * Xiphos Bible Study Tool
 * gui.c - The heart of the gui.
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>

/* ----------------------------------------------- */
/* do not #include "gui/debug_glib_null.h" in this */
/* file: here we define the replacement functions, */
/* so we need access to the real glib versions.    */
/* ----------------------------------------------- */

#include "gui/gui.h"
#include "gui/session.h"
#include "gui/dialog.h"
#include "gui/ipc.h"

static IpcObject* ipc;

void gui_init(int argc, char *argv[])
{
	static int gui_init_done = FALSE;

	if (gui_init_done) return;
	gui_init_done = TRUE;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
#ifdef WIN32
	gchar *locale_dir = g_win32_get_package_installation_directory_of_module(NULL);
	locale_dir = g_strconcat(locale_dir, "\0", NULL);
	locale_dir = g_build_filename (locale_dir, "share", "locale", NULL);
	bindtextdomain(GETTEXT_PACKAGE, locale_dir);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
	g_free (locale_dir);
#endif
	gnome_program_init (PACKAGE, VERSION, LIBGNOMEUI_MODULE,
			    argc, argv,
			    GNOME_PROGRAM_STANDARD_PROPERTIES,
			    NULL);
#ifndef WIN32
	gs_session_init(argv[0]);
	gconf_setup();
	ipc = ipc_init_dbus_connection(ipc);
#endif
}

void gui_main(void)
{
	gtk_main();
}


/******************************************************************************
 * Name
 *    gconf_setup
 *
 * Synopsis
 *   #include "main/settings.h"
 *
 *   void gconf_setup()
 *
 * Description
 *   verifies and initializes the GConf subsystem, so that "sword://" and
 *   similar can be handled by url-comprehending programs such as browsers.
 *   dialogs for permission/success/failure => conditional on debug build.
 *
 * Return value
 *   void
 */

#define	GS_GCONF_PERMISSION	_("URL references using \"sword://\" (similar to web page\nreferences) can be used by programs to look up\nscripture references. Your system currently has no\nprogram set to handle these references.\n\nWould you like Xiphos to set itself as the\nprogram to handle these references?")

#define	GS_GCONF_SUCCESS	_("Xiphos has successfully set itself\nas the handler of sword:// and bible:// URLs.\n\nYou may wish to run the program \"gconf-editor\"\nto examine keys under /desktop/gnome/url-handlers,\nif you need to change these.")

char *gconf_keys[GS_GCONF_MAX][2] = {
    { "/desktop/gnome/url-handlers/bible/command",        "xiphos \"%s\"" },
    { "/desktop/gnome/url-handlers/bible/enabled",        (char *) 1 },
    { "/desktop/gnome/url-handlers/bible/needs_terminal", (char *) 0 },
    { "/desktop/gnome/url-handlers/sword/command",        "xiphos \"%s\"" },
    { "/desktop/gnome/url-handlers/sword/enabled",        (char *) 1 },
    { "/desktop/gnome/url-handlers/sword/needs_terminal", (char *) 0 }
};

void gconf_setup()
{
	int i;
	gchar *str;
	gboolean retval;
	GConfClient* client = gconf_client_get_default();

	if (client == NULL)
		return;		/* we're not running under GConf */

	/*
	 * This is deliberately somewhat simple-minded, at least for now.
	 * We care about one thing: Is anything set to handle "bible://"?
	 *
	 * Unfortunate consequence of changing xiphos2 => xiphos:
	 * We must fix broken keys.
	 */
	if ((((str = gconf_client_get_string(client, gconf_keys[0][0],
					    NULL)) == NULL) ||
	     (strncmp(str, "gnomesword", 10) == 0))
#ifdef DEBUG
	    && gui_yes_no_dialog(GS_GCONF_PERMISSION, NULL)
#endif /* DEBUG */
	    ) {
		/*
		 * Mechanical as can be, one after another.
		 */
		for (i = 0; i < GS_GCONF_MAX; ++i) {
			retval = (((i % 3) == 0)	/* contrived hack */
				  ? gconf_client_set_string
					(client,
					 gconf_keys[i][0],
					 gconf_keys[i][1],
					 NULL)
				  : gconf_client_set_bool
					(client,
					 gconf_keys[i][0],
					 (gconf_keys[i][1] ? TRUE : FALSE),
					 NULL));
#ifdef DEBUG
			if (!retval) {
				char msg[256];
				sprintf(msg, _("Xiphos failed to complete handler init for key #%d:\n%s"),
					i, gconf_keys[i][0]);
				gui_generic_warning(msg);
				return;
			}
#endif /* DEBUG */
		}
#ifdef DEBUG
		gui_generic_warning(GS_GCONF_SUCCESS);
#endif /* DEBUG */
	}
}


#ifdef DEBUG

/* NOTE: these routines are here only and exactly because there is no other */
/* code in this file that needs the real glib versions.  if there is ever a */
/* reason for using g_str{dup,ing}_printf in this file, these functions     */
/* will need to be moved to some other file which does not need them.  this */
/* is because the file implementing replacements must not #include the .h   */
/* that induces access to the replacements via #define.                     */

/* this is a total mind game: we redefine the standard use of glib functions */
/* so as to see our internal versions.  our internal versions scan for mis-  */
/* use of %s, i.e. (char*)NULL that should (by rights) cause crashes anyhow. */

/* GIVE US THE CRASHES, PLEASE!  MYSTERY BUGS ARE EVIL!  glibc.helpfulness-- */

gchar*
XI_g_strdup_printf(const char *filename,
		   int linenumber,
		   const gchar *format,
		   ...)
{
	gchar *buffer, *next, *s;
	va_list args;

	va_start(args, format);
	for (s = strchr(format, '%'); s; s = strchr(++s, '%'))
	{
		next = va_arg(args, gchar*);
		if ((next == (gchar*) NULL) && (*(s+1) == 's'))
		{
			gchar *msg = g_strdup_printf
			    ("%s\n%s\n\n%s:%d \"%s\"",
			     _("BUG! Xiphos is about to crash due to a \"STRDUP\" error."),
			     _("Please report this error to the Xiphos team with:"),
			     filename, linenumber, format);
			gui_generic_warning(msg);
			g_free(msg);
			abort();
		}
	}
	va_end(args);

	/* real g_strdup_printf content */
	va_start (args, format);
	buffer = g_strdup_vprintf (format, args);
	va_end (args);
	return buffer;
}

void
XI_g_string_printf(const char *filename,
		   int linenumber,
		   GString *string,
		   const gchar *format,
		   ...)
{
	gchar *next, *s;
	va_list args;

	va_start(args, format);
	for (s = strchr(format, '%'); s; s = strchr(++s, '%'))
	{
		next = va_arg(args, gchar*);
		if ((next == (gchar*) NULL) && (*(s+1) == 's'))
		{
			gchar *msg = g_strdup_printf
			    ("%s\n%s\n\n%s:%d \"%s\"",
			     _("BUG! Xiphos is about to crash due to a \"STRING\" error."),
			     _("Please report this error to the Xiphos team with:"),
			     filename, linenumber, format);
			gui_generic_warning(msg);
			g_free(msg);
			abort();
		}
	}
	va_end(args);

	/* real g_string_printf content */
	g_string_truncate (string, 0);
	va_start (args, format);
	g_string_append_vprintf (string, format, args);
	va_end (args);
}

#endif /* DEBUG */
