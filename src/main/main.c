/*
 * Xiphos Bible Study Tool
 * main.c - In the beginning... ;o)
 *
 * Copyright (C) 2000-2026 Xiphos Developer Team
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
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#include <glib/gi18n.h>
#include <locale.h>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

#include "gui/dialog.h"
#include "gui/gui.h"
#include "gui/main_window.h"
#include "gui/splash.h"
#include "gui/tabbed_browser.h"
#include "gui/xiphos.h"
#include "gui/utilities.h"

#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"
#include "xiphos_html/xiphos_html.h"

#include "gui/debug_glib_null.h"

#include <glib/gstdio.h>

/*
 * global initialization flag.
 * initially false. when we are about to call gui_main(), we set it.
 */
gboolean initialized = FALSE;

#ifdef WIN32

#ifdef HAVE_DBUS
GPid dbus_pid;
#endif

/*
 * this is, we hope, a temporary fix for the webkit image display problem.
 * we instantiate a local web server (!) so as to self-serve image files
 * whose size we have already otherwise determined using local path.
 */

#include <gio/gio.h>
#include <libsoup/soup.h>

static GHashTable *local_media_files;
static gboolean local_media_server_ready;

gchar *main_local_media_url(const gchar *path)
{
	gchar *canonical_path;
	gchar *token;
	gchar *basename;
	gchar *escaped_basename;
	gchar *url;

	if (!local_media_server_ready || !path)
		return NULL;
	canonical_path = g_canonicalize_filename(path, NULL);
	if (!g_file_test(canonical_path, G_FILE_TEST_IS_REGULAR)) {
		g_free(canonical_path);
		return NULL;
	}

	if (!local_media_files)
		local_media_files =
		    g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	token = g_compute_checksum_for_string(G_CHECKSUM_SHA256,
					      canonical_path, -1);
	g_hash_table_replace(local_media_files, g_strdup(token), canonical_path);

	basename = g_path_get_basename(canonical_path);
	escaped_basename = g_uri_escape_string(basename, NULL, FALSE);
	url = g_strdup_printf("http://127.0.0.1:7878/%s/%s", token,
			      escaped_basename);

	g_free(escaped_basename);
	g_free(basename);
	g_free(token);
	return url;
}

static void
server_callback_media(SoupServer *server,
		      SoupMessage *msg,
		      const char *path,
		      GHashTable *query,
		      SoupClientContext *client, gpointer user_data)
{
	char *contents;
	gsize length;
	GError *error = NULL;
	gchar token[65];
	const gchar *real_path;
	const gchar *requested_name;
	gchar *registered_name;
	gchar *content_type;
	gchar *mime_type;

	if (g_strcmp0(msg->method, SOUP_METHOD_GET)) {
		soup_message_set_status(msg, SOUP_STATUS_METHOD_NOT_ALLOWED);
		return;
	}

	/* Only serve paths explicitly registered by Xiphos itself. */
	if (!path || path[0] != '/' || strlen(path) < 66 || path[65] != '/') {
		soup_message_set_status(msg, SOUP_STATUS_NOT_FOUND);
		return;
	}
	memcpy(token, path + 1, 64);
	token[64] = '\0';
	requested_name = path + 66;
	real_path = local_media_files
		? (const gchar *)g_hash_table_lookup(local_media_files, token)
		: NULL;
	registered_name = real_path ? g_path_get_basename(real_path) : NULL;
	if (!real_path || !requested_name[0] || strchr(requested_name, '/') ||
	    strchr(requested_name, '\\') ||
	    g_strcmp0(requested_name, registered_name) ||
	    !g_file_test(real_path, G_FILE_TEST_IS_REGULAR)) {
		g_free(registered_name);
		soup_message_set_status(msg, SOUP_STATUS_NOT_FOUND);
		return;
	}
	g_free(registered_name);

	if (!g_file_get_contents(real_path, &contents, &length, &error)) {
		g_warning("Unable to read local media: %s",
			  error ? error->message : "unknown error");
		g_clear_error(&error);
		soup_message_set_status(msg, SOUP_STATUS_NOT_FOUND);
		return;
	}

	content_type = g_content_type_guess(real_path,
					    (const guchar *)contents,
					    length, NULL);
	mime_type = content_type
		? g_content_type_get_mime_type(content_type)
		: NULL;
	if (mime_type)
		soup_message_headers_set_content_type(msg->response_headers,
					      mime_type, NULL);
	g_free(mime_type);
	g_free(content_type);

	soup_message_body_append(msg->response_body, SOUP_MEMORY_TAKE,
				 contents, length);
	soup_message_set_status(msg, SOUP_STATUS_OK);
}
#endif /* WIN32 */

/******************************************************************************
 * Name
 *   main
 *
 * Synopsis
 *
 *   int main(int argc, char *argv[])
 *
 * Description
 *   Starting point of Xiphos.
 *
 * Return value
 *   int
 */

int main(int argc, char *argv[])
{
	int newconfigs = FALSE;
	int newbookmarks = FALSE;
	int have_sword_url = FALSE;
	int have_tab_list = FALSE;
	gint base_step = 0; //needed for splash
#ifdef WIN32
	/* see comments on function immediately preceding. */
	SoupServer *server = NULL;
	SoupAddress *loopback = NULL;
	guint server_status;
#endif
#ifdef CHATTY
	GTimer *total;
	double d;
#endif

	// ---------------------------------------------------------
	// **** LOUD OBNOXIOUS COMMENT TO GET ATTENTION ****
	// **** This a sick hack to get around a WK regression.
	// **** https://bugs.webkit.org/show_bug.cgi?id=228268
	// **** If WK ever gets this fixed, this should be removed.
	/*
	 * set env.var that causes WK not to screw with nvidia badly.
	 */
	g_setenv("WEBKIT_DISABLE_COMPOSITING_MODE", "1", TRUE);
	// **** end of LOUD OBNOXIOUS COMMENT TO GET ATTENTION ****
	// ---------------------------------------------------------

#if !GLIB_CHECK_VERSION(2, 35, 0)
	g_type_init(); // g_type_init has been deprecated since version 2.36
		       // Since GLib 2.36, the type system is initialised
		       // automatically and this function does nothing.
#endif

#ifdef CHATTY
	total = g_timer_new();
#endif

	/* get this error check out of the way before risking leaking win32 resources */
	if (argc > 2) {
		gui_generic_warning_modal(_("Xiphos does not understand more than one argument."));
		exit(1);
	}
#ifdef WIN32
	/*
	 * We have learned that Windows scaling is not respected by
	 * GTK3 titlebar controls. But there is an env.var to tell
	 * GTK3 not to worry abott it, let Windows do its thing.
	 */
	g_setenv("GTK_CSD", "0", TRUE);

	/*
	 * WIN32 perversity: We very much want to be *in* the
	 * the directory where Xiphos exists, to keep gconf and
	 * gspawn happy.  Glib provides this nice function for us
	 * to determine where we are (we can't depend on argv).
	 */
	gchar *bin_dir = xiphos_win32_get_subdir("bin");
	g_chdir(bin_dir);

	/* add this directory to $PATH for other stuff, e.g. zip */
	g_setenv("PATH", g_strdup_printf("%s;%s", bin_dir,
					 g_getenv("PATH")),
		 TRUE);

	/* This will not overwrite SWORD_PATH in case it has
	   been set by some other program or set manually. In the case that
	   it hasn't been set, it is set to the localized equivalent of
	   C:/Documents and Settings/All Users/Application Data/Sword */
	/*
	 * NOTE: SWORD_PATH is deliberately computed WITHOUT calling
	 * g_get_system_data_dirs(). That function caches its result on
	 * first call for the lifetime of the process (documented GLib
	 * behavior), and GTK's internal icon theme lookup relies on the
	 * same cached call. Calling it here — before or after setting
	 * XDG_DATA_DIRS below — would poison that cache and make our
	 * XDG_DATA_DIRS override invisible to GTK's icon search, which
	 * is what caused icons to go missing again after a previous fix
	 * to this same block. ALLUSERSPROFILE/PROGRAMDATA give the same
	 * directory g_get_system_data_dirs() would have on Windows by
	 * default, without touching the cache.
	 */
	const gchar *programdata = g_getenv("PROGRAMDATA");
	if (!programdata)
		programdata = g_getenv("ALLUSERSPROFILE");
	g_setenv("SWORD_PATH", g_strdup_printf("%s\\Sword", programdata),
		 FALSE);
	/*
	 * GTK3 needs to find its compiled GSettings schemas (used
	 * internally e.g. by GtkFileChooserButton in the Preferences
	 * dialog) and its icon theme (hicolor/Adwaita). Neither is
	 * discovered automatically in this MinGW cross-compiled build,
	 * so point GLib/GTK at them explicitly. Safe to do here since
	 * nothing above has called g_get_system_data_dirs() yet.
	 */
	gchar *share_dir = xiphos_win32_get_subdir("share");
	g_setenv("GSETTINGS_SCHEMA_DIR",
		g_build_filename(share_dir, "glib-2.0", "schemas", NULL),
		TRUE);
	const gchar *existing_xdg_data_dirs = g_getenv("XDG_DATA_DIRS");
	gchar *new_xdg_data_dirs = existing_xdg_data_dirs
		? g_strdup_printf("%s;%s", share_dir, existing_xdg_data_dirs)
		: share_dir;
	g_setenv("XDG_DATA_DIRS", new_xdg_data_dirs, TRUE);

	/*it is necessary to explicitly set LANG, because we depend on that
	   variable to set the SWORD locale */
	g_setenv("LANG", g_win32_getlocale(), FALSE);

	/* we need an idea of $HOME that's convenient. */
	/* this gives us linux-equivalent semantics. */
	g_setenv("HOME", g_getenv("APPDATA"), TRUE);

	/* see comments near top of file. */
	loopback = soup_address_new("127.0.0.1", 7878);
	if (loopback) {
		server_status = soup_address_resolve_sync(loopback, NULL);
		if (server_status == SOUP_STATUS_OK)
			server = soup_server_new(SOUP_SERVER_INTERFACE,
						 loopback, NULL);
		else
			g_warning("Unable to resolve local media address: %s",
				  soup_status_get_phrase(server_status));
		g_object_unref(loopback);
	}
	if (server) {
		soup_server_add_handler(server, "/", server_callback_media,
					NULL, NULL);
		soup_server_run_async(server);
		if (soup_server_get_listener(server))
			local_media_server_ready = TRUE;
		else {
			g_warning("Unable to listen for local media requests");
			g_object_unref(server);
			server = NULL;
		}
	} else {
		g_warning("Unable to create local media server");
	}

#ifdef HAVE_DBUS
	/* we must start dbus-daemon on our own as well */
	/* on windows, we will ship dbus along with Xiphos */
	gchar *dbus_args[4];

	gchar *dbus_com =
	    g_win32_get_package_installation_directory_of_module(NULL);
	dbus_com = g_strconcat(dbus_com, "\0", NULL);

	gchar *dbus_conf =
	    g_build_filename(dbus_com, "etc\\dbus-session.conf\0", NULL);
	dbus_conf = g_build_filename("--config-file=\0", dbus_conf, NULL);

	dbus_com = g_build_filename(dbus_com, "dbus.exe\0", NULL);

	dbus_args[0] = dbus_com;
	dbus_args[1] = dbus_conf;
	dbus_args[2] = g_strdup("--fork");
	dbus_args[3] = NULL;
	g_spawn_async(NULL,
		      dbus_args,
		      NULL,
		      G_SPAWN_SEARCH_PATH, NULL, NULL, &dbus_pid, NULL);
	Sleep(2); // give dbus a moment to init.
#endif		  /* HAVE_DBUS */

#endif /* WIN32 */

	if (argc > 1) {
		/*
		 * these args are for broken configs or bookmarks -
		 * ie xiphos will not start
		 */
		if (!strcmp(argv[1], "newconfigs")) {
			newconfigs = TRUE;
		}
		if (!strcmp(argv[1], "newbookmarks")) {
			newbookmarks = TRUE;
		}
		if (!strcmp(argv[1], "newfiles")) {
			newconfigs = TRUE;
			newbookmarks = TRUE;
		}

		/* this arg is a tab list */
		if (g_access(argv[1], F_OK) == 0) {
			have_tab_list = TRUE;
		}

		/* this arg is a sword uri */
		if ((!strncmp(argv[1], "sword:/", 7)) ||
		    (!strncmp(argv[1], "bible:/", 7)) ||
		    (strstr(argv[1], "studypad"))) {
			if (!strncmp(argv[1], "bible:/", 7))
				memcpy(argv[1], "sword", 5); /* equivalent */
			have_sword_url = TRUE;
		}
	}

	/*
	 * Initialize the locale and gettext as early as possible, before
	 * any code that generates translatable content (e.g.
	 * settings_init() -> init_bookmarks(), which writes the default
	 * bookmarks.xml using _()-wrapped strings). Previously this only
	 * happened later, implicitly via gtk_init_with_args() inside
	 * gui_init(), so the default bookmarks were always generated
	 * while the process was still in the "C" locale (English),
	 * regardless of the user's actual language. setlocale() and the
	 * later bindtextdomain/textdomain calls in gui_init() remain
	 * harmless to repeat (idempotent).
	 */
	setlocale(LC_ALL, "");
#ifdef ENABLE_NLS
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
#endif
#ifdef WIN32
	{
		gchar *locale_dir =
		    g_win32_get_package_installation_directory_of_module(NULL);
		locale_dir = g_strconcat(locale_dir, "\0", NULL);
		locale_dir = g_build_filename(locale_dir, "share", "locale", NULL);
		bindtextdomain(GETTEXT_PACKAGE, locale_dir);
		bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
		textdomain(GETTEXT_PACKAGE);
		g_free(locale_dir);
	}
#endif
	/*
	 * check for directories and files
	 */
	settings_init(argc, argv, newconfigs, newbookmarks);

	/* Phase 2: backup bookmarks.xml once on first run after upgrade.
	 * The backup is skipped if it already exists. */
	{
		gchar *bm   = g_strdup_printf("%s/bookmarks/bookmarks.xml",
		                               settings.gSwordDir);
		gchar *bmbak = g_strdup_printf("%s/bookmarks/bookmarks.xml.bak",
		                               settings.gSwordDir);
		if (g_file_test(bm, G_FILE_TEST_EXISTS) &&
		    !g_file_test(bmbak, G_FILE_TEST_EXISTS)) {
			GError *err = NULL;
			GFile *src_f = g_file_new_for_path(bm);
			GFile *dst_f = g_file_new_for_path(bmbak);
			g_file_copy(src_f, dst_f,
			            G_FILE_COPY_NONE, NULL, NULL, NULL, &err);
			if (err) g_error_free(err);
			g_object_unref(src_f);
			g_object_unref(dst_f);
		}
		g_free(bm);
		g_free(bmbak);
	}

	gui_init(argc, argv);

	gui_splash_init();

	gui_splash_step(_("Initiating HTML"), 0.0, 0);
	XIPHOS_HTML_INITIALIZE();

	base_step = 1;

	gui_splash_step(_("Building Interface"), 0.2, 0 + base_step);
	create_mainwindow();

	gui_splash_step(_("Starting Sword"), 0.5, 1 + base_step);
	main_init_backend();

	gui_splash_step(_("Loading Settings"), 0.8, 2 + base_step);
	frontend_init();

	gui_splash_step(_("Displaying Xiphos"), 1.0, 3 + base_step);

	/* need to get rid of wrongly-formatted annotation labels. */
	xml_convert_to_osisref();

	frontend_display(have_tab_list ? argv[1] : NULL);

	if (have_sword_url) {
		if (!strncmp(argv[1], "sword:/", 7)) {
			char *key = strchr(argv[1] + 8, '/');
			if (key && *(key + 1) != '\0')
				key++;
			else
				key = "Gen 1:1";
			gui_open_passage_in_new_tab(key);
		}
		main_url_handler(argv[1], TRUE);
	}
#ifdef CHATTY
	g_timer_stop(total);
	d = g_timer_elapsed(total, NULL);
	g_timer_destroy(total);
	printf("total time is %f\n", d);
#endif
	g_idle_add((GSourceFunc)gui_splash_done, NULL);

	gui_recompute_shows(FALSE);

	initialized = TRUE;

	gui_main();
	return 0;
}
