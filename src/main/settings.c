/*
 * Xiphos Bible Study Tool
 * settings.c - Guess what?
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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

#include <gtk/gtk.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "gui/bookmarks.h"
#include "gui/dialog.h"
#include "gui/gui.h"
#include "gui/mod_mgr.h"
#include "gui/utilities.h"
#include "gui/preferences_dialog.h"

#include "main/lists.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "editor/template.h"

#include <glib/gstdio.h>

#include "gui/debug_glib_null.h"
#include <locale.h>

/******************************************************************************
 * defines
 */
#define XI_DIR ".xiphos"

/******************************************************************************
 * globals
 */
SETTINGS settings;

/******************************************************************************
 * static
 */
static int init_bookmarks(int new_bookmarks);

/******************************************************************************
 * Name
 *   settings_init
 *
 * Synopsis
 *   #include "main/settings.h"
 *
 *   gint settings_init(void)
 *
 * Description
 *   check for Xiphos dir
 *   if not found - create it and
 *   the files needed to run program
 *
 * Return value
 *   gint
 */

int settings_init(int argc, char **argv, int new_configs,
		  int new_bookmarks)
{
	int retval = 0;
	char *sword_dir = NULL;
	char *tmp = NULL;
	char *env;
	struct stat buf;
	gboolean buf_says_empty = FALSE;

	settings.first_run = FALSE;
	/* set program title */
	strcpy(settings.program_title, "Xiphos");

	/* Get home dir */
	if ((settings.homedir = (char *)g_getenv(HOMEVAR)) == NULL) {
		gui_init(argc, argv);
		gui_generic_warning_modal(_("$HOME is not set!"));
		/* if not found in env exit */
		exit(0);
	}

	/* set gSwordDir to $home + .xiphos */
	settings.gSwordDir =
	    g_build_filename(settings.homedir, XI_DIR, NULL);

	/* if gSwordDir does not exist, create it. */
	if (g_access(settings.gSwordDir, F_OK) == -1) {
		if ((Mkdir(settings.gSwordDir, S_IRWXU)) != 0) {
			char msg[300];
			gui_init(argc, argv);
			sprintf(msg, _("Xiphos can not create directory " XI_DIR
				       ":\n%s\n\nXiphos cannot continue."),
				strerror(errno));
			gui_generic_warning_modal(msg);
			/* if we can not create gSwordDir exit */
			exit(1);
		}
	}

	/* if .sword does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir, DOTSWORD);
	if (g_access(sword_dir, F_OK) == -1) {
		if ((Mkdir(sword_dir, S_IRWXU)) != 0) {
			gui_init(argc, argv);
			gui_generic_warning_modal(_("can not create " DOTSWORD));
		}
	}
	g_free(sword_dir);
	/* if .sword/mods.d does not exist create it */
	sword_dir =
	    g_strdup_printf("%s/%s", settings.homedir, DOTSWORD "/mods.d");
	if (g_access(sword_dir, F_OK) == -1) {
		if ((Mkdir(sword_dir, S_IRWXU)) != 0) {
			gui_init(argc, argv);
			gui_generic_warning_modal(_("can not create " DOTSWORD "/mods.d"));
		}
	}
	g_free(sword_dir);
	/* if .sword/modules does not exist create it */
	sword_dir =
	    g_strdup_printf("%s/%s", settings.homedir,
			    DOTSWORD "/modules");
	if (g_access(sword_dir, F_OK) == -1) {
		if ((Mkdir(sword_dir, S_IRWXU)) != 0) {
			gui_init(argc, argv);
			gui_generic_warning_modal(_("can not create " DOTSWORD "/modules"));
		}
	}
	g_free(sword_dir);

	/* moved here from crud locations in backend. */
	main_init_language_map();

	language_init();

	init_bookmarks(new_bookmarks);

	/* get the first cut at the set of modules available. */
	/* maybe insufficient, contain no bibles: ok for now. */
	main_init_lists();

	/* set fnconfigure to gSwordDir and settings.xml */
	settings.fnconfigure =
	    g_strdup_printf("%s/settings.xml", settings.gSwordDir);

	/* check for a bad settings file. */
	if ((stat(settings.fnconfigure, &buf) == 0) && /* exists... */
	    (buf.st_size == 0)) {		       /* ...but empty? */
		char *backup_name =
		    g_strdup_printf("%s.SAVE", settings.fnconfigure);

		/* toss out the bad one and look for recovery. */
		unlink(settings.fnconfigure);
		if (stat(backup_name, &buf) == 0) {
			buf_says_empty = (buf.st_size == 0);
			rename(backup_name, settings.fnconfigure);
			gui_init(argc, argv);
			gui_generic_warning_modal(_("Empty settings file -- backup recovery attempted.\n"
						    "Some information may have been lost."));
		} else {
			gui_generic_warning_modal(_("Empty settings file -- no backup?!? Information lost!"));
		}
		g_free(backup_name);
	}

	/* if settings.xml does not exist create it */
	if (buf_says_empty ||
	    (g_access(settings.fnconfigure, F_OK) == -1) || new_configs) {
		/* must be first run */
		XI_print((buf_says_empty
			      ? "\nSETTINGS FILE EXISTS BUT IS EMPTY - RECREATING.\n"
			      : "\nFirst Run: need to create settings!\n"));
		settings.first_run = TRUE;

		xml_create_settings_file(settings.fnconfigure);
		xml_save_settings_doc(settings.fnconfigure);
		xml_free_settings_doc();
	}
	xml_parse_settings_file(settings.fnconfigure);

	/* ensure that the user has a bible with which to work */
	if (settings.havebible == 0) {
		gui_init(argc, argv);
		main_shutdown_list();
		gui_open_mod_mgr_initial_run();
		main_init_lists();
		if (settings.havebible == 0) {
			gui_generic_warning_modal(_("There are no Bibles installed.\n"
						    "Evidently, you declined to install any.\n\n"
						    "Without any Bible modules to display,\n"
						    "Xiphos cannot proceed,\nand will now exit."));
			exit(1);
		}
		gui_generic_warning_modal(_("Bible module installation complete."));
	}

	/* check for template.pad file for studypad */
	tmp = g_strdup_printf("%s/%s", settings.gSwordDir, "template.pad");
	if ((g_access(tmp, F_OK) == -1)) {
		g_file_set_contents(tmp, " ", 1, NULL);
	}
	g_free(tmp);

	/* check for studypad.spt template file for studypad */
	tmp = g_build_filename(settings.gSwordDir, "studypad.spt", NULL);
	if (g_access(tmp, F_OK) == -1) {
		g_file_set_contents(tmp, studypad_template, -1, NULL);
	}
	g_free(tmp);

	load_settings_structure();

	/* BibleSync */
	settings.bs_mode = 0;
	settings.bs_privacy = FALSE;
	settings.bs_navdirect = TRUE;
	settings.bs_debug = FALSE;
	settings.bs_presence = FALSE;
	settings.bs_mismatch = FALSE;
	settings.bs_group_tab = TRUE;
	settings.bs_receiving = FALSE;
	settings.bs_listen_set = 0; // selective
	settings.bs_passphrase = g_strdup("BibleSync");

	/* if the user had forced a locale, we must set it now. */
	if (settings.special_locale && strcmp(settings.special_locale, NONE)) {
		g_setenv("LANG", settings.special_locale, TRUE);
		gchar *test = setlocale(LC_ALL, settings.special_locale);
		if (test == NULL) {
			gchar lfix[32];
			sprintf(lfix, "%s.UTF-8", settings.special_locale); //for Ubuntu
			test = setlocale(LC_ALL, lfix);
		}
		if (test != NULL) {
			g_setenv("LC_ALL", test, TRUE);
			XI_message(("set locale to %s",
				    settings.special_locale));
		}
	}

	/* find out what kind of peculiar language environment we have */
	re_encode_digits = FALSE;
	if (((env = (char *)g_getenv("LANG")) != NULL) &&
	    // for now, farsi-sensitive only
	    (strncmp(env, "fa", 2) == 0)) {
		re_encode_digits = TRUE;
	}

	return retval;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "main/settings.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

int init_bookmarks(int new_bookmarks)
{
	//GNode *bookmark_tree = NULL;
	char *file_buf = NULL;

	settings.load_xml_bookmarks = FALSE;

	/* set bookmarks dir to settings.gSwordDir + /bookmarks */
	settings.swbmDir =
	    g_strdup_printf("%s/%s", settings.gSwordDir, "bookmarks");

	/* if .xiphos/bookmarks does not exist create it */
	if (g_access(settings.swbmDir, F_OK) == -1) {
		if ((Mkdir(settings.swbmDir, S_IRWXU)) == -1) {
			g_warning("can't create bookmarks dir");
			return 0;
		}
	}

	if (new_bookmarks)
		xml_new_bookmark_file();

	/* check for xml bookmarks */
	file_buf = g_strdup_printf("%s/bookmarks.xml", settings.swbmDir);

	if (g_access(file_buf, F_OK) == 0) {
		settings.load_xml_bookmarks = TRUE;
	}
	g_free(file_buf);

	if (!settings.load_xml_bookmarks) {
		xml_new_bookmark_file();
		settings.load_xml_bookmarks = TRUE;
	}

	return 1;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "main/settings.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

void load_settings_structure(void)
{
	char *buf = NULL, *parallels;

	settings.gs_version = VERSION;
	if ((settings.MainWindowModule =
		 xml_get_value("modules", "bible")) == NULL) {
		/* by the time we are here, we *must* have at least 1 bible */
		settings.MainWindowModule =
		    g_strdup(get_list(TEXT_LIST)->data);
	}
	settings.CommWindowModule = xml_get_value("modules", "comm");
	settings.DictWindowModule = xml_get_value("modules", "dict");

	parallels = xml_get_value("modules", "parallels");
	/* if no parallels are known, convert old fixed parallel set */
	if (!parallels || (*parallels == '\0')) {
		char intN[] = "intN", i, *newhold;

		g_free(parallels); /* in case it was real but empty */
		parallels = g_strdup("");
		for (i = '1'; i <= '5'; ++i) { /* the old set */
			intN[3] = i;
			char *oldparallel = xml_get_value("modules", intN);
			if (oldparallel && *oldparallel) {
				newhold =
				    g_strconcat(parallels, oldparallel,
						",", NULL);
				g_free(parallels);
				parallels = newhold;
				g_free(oldparallel);
				xml_remove_node("modules", intN, NULL); /* ? */
			}
		}

		/* trim off the last comma */
		if (*parallels) {
			*(parallels + strlen(parallels) - 1) = '\0';
			xml_set_new_element("modules", "parallels",
					    parallels);
		}
	}
	if (parallels && *parallels)
		settings.parallel_list = g_strsplit(parallels, ",", -1);
	else
		settings.parallel_list = NULL;
	g_free(parallels);

	settings.personalcommentsmod = xml_get_value("modules", "percomm");
	settings.devotionalmod = xml_get_value("modules", "devotional");
	settings.book_mod = xml_get_value("modules", "book");

	/* we will assume reasonable module names, if not otherwise set */
	settings.lex_greek = xml_get_value("lexicons", "greek");
	settings.lex_hebrew = xml_get_value("lexicons", "hebrew");
	if (!settings.lex_greek) {
		settings.lex_greek = g_strdup("StrongsGreek");
		xml_set_value("Xiphos", "lexicons", "greek",
			      settings.lex_greek);
	}
	if (!settings.lex_hebrew) {
		settings.lex_hebrew = g_strdup("StrongsHebrew");
		xml_set_value("Xiphos", "lexicons", "hebrew",
			      settings.lex_hebrew);
	}

	settings.DefaultDict =
	    xml_get_value("lexicons", "defaultdictionary");
	if (!settings.DefaultDict) {
		settings.DefaultDict = g_strdup("WebstersDict");
		xml_set_value("Xiphos", "lexicons", "defaultdictionary",
			      settings.DefaultDict);
	}
	settings.useDefaultDict =
	    atoi((buf = xml_get_value("lexicons", "usedefaultdict"))
		     ? buf
		     : "0");

	/* unusual locale setting */
	if ((buf = xml_get_value("locale", "special")))
		settings.special_locale = g_strdup(buf);
	else {
		xml_add_new_section_to_settings_doc("locale");
		xml_add_new_item_to_section("locale", "special", NONE);
		settings.special_locale = g_strdup(NONE);
	}

	/* mod mgr stuff */
	if ((buf = xml_get_value("modmgr", "mod_mgr_source")))
		settings.mod_mgr_source = atoi(buf);
	else {
		xml_add_new_section_to_settings_doc("modmgr");
		xml_add_new_item_to_section("modmgr", "mod_mgr_source",
					    "1");
		settings.mod_mgr_source = 1;
	}
	if ((buf = xml_get_value("modmgr", "mod_mgr_destination")))
		settings.mod_mgr_destination = atoi(buf);
	else {
		xml_add_new_item_to_section("modmgr",
					    "mod_mgr_destination", "0");
		settings.mod_mgr_destination = 0;
	}
	if ((buf = xml_get_value("modmgr", "mod_mgr_local_source_index")))
		settings.mod_mgr_local_source_index = atoi(buf);
	else {
		xml_add_new_item_to_section("modmgr",
					    "mod_mgr_local_source_index",
					    "0");
		settings.mod_mgr_local_source_index = 0;
	}
	if ((buf = xml_get_value("modmgr", "mod_mgr_remote_source_index")))
		settings.mod_mgr_remote_source_index = atoi(buf);
	else {
		xml_add_new_item_to_section("modmgr",
					    "mod_mgr_remote_source_index",
					    "0");
		settings.mod_mgr_remote_source_index = 0;
	}

	/* current verse & keys */
	settings.currentverse = xml_get_value("keys", "verse");
	settings.dictkey = xml_get_value("keys", "dictionary");

	if (xml_get_value("keys", "book")) {
		settings.book_key = xml_get_value("keys", "book");
	} else {
		xml_add_new_item_to_section("keys", "book", "0");
		settings.book_key = xml_get_value("keys", "book");
	}
	if (xml_get_value("keys", "offset"))
		settings.book_offset =
		    atol(xml_get_value("keys", "offset"));
	else {
		xml_add_new_item_to_section("keys", "offset", "");
		settings.book_offset = 0;
	}

	if (xml_get_value("tab", "page"))
		settings.tab_page = atol(xml_get_value("tab", "page"));
	else {
		xml_add_new_item_to_section("tab", "page", "0");
		settings.tab_page = 0;
	}

	/* layout */
	settings.sidebar_width =
	    atoi((buf = xml_get_value("layout", "shortcutbar"))
		     ? buf
		     : "100");
	if ((buf = xml_get_value("layout", "vltoppaneheight")))
		settings.verselist_toppane_height = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "vltoppaneheight",
					    "210");
		settings.verselist_toppane_height = 210;
	}
	if ((buf = xml_get_value("layout", "sidebar_notebook_height")))
		settings.sidebar_notebook_height = atoi(buf);
	else {
		if ((buf = xml_get_value("layout", "sidebar_notebook_hight"))) { /* backward compatible */
			settings.sidebar_notebook_height = atoi(buf);
			xml_remove_node("layout", "sidebar_notebook_hight",
					0);
		} else
			settings.sidebar_notebook_height = 250;
		buf =
		    g_strdup_printf("%d",
				    settings.sidebar_notebook_height);
		xml_add_new_item_to_section("layout",
					    "sidebar_notebook_height",
					    buf);
		g_free(buf);
	}

	/* whether to open saved windows at startup */
	if ((buf = xml_get_value("layout", "parallelopen")))
		settings.display_parallel = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "parallelopen", "0");
		settings.display_parallel = 0;
	}

	if ((buf = xml_get_value("layout", "modmgropen")))
		settings.display_modmgr = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "modmgropen", "0");
		settings.display_modmgr = 0;
	}

	if ((buf = xml_get_value("layout", "advsearchopen")))
		settings.display_advsearch = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "advsearchopen",
					    "0");
		settings.display_advsearch = 0;
	}

	if ((buf = xml_get_value("layout", "prefsopen")))
		settings.display_prefs = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "prefsopen", "0");
		settings.display_prefs = 0;
	}

	/* main window geometry */
	if ((buf = xml_get_value("layout", "height")))
		settings.gs_height = atoi(buf);
	else {
		if ((buf = xml_get_value("layout", "hight"))) { /* backward compatible */
			settings.gs_height = atoi(buf);
			xml_remove_node("layout", "hight", 0);
		} else
			settings.gs_height = 500;
		buf = g_strdup_printf("%d", settings.gs_height);
		xml_add_new_item_to_section("layout", "height", buf);
		g_free(buf);
	}
	if ((buf = xml_get_value("layout", "width")))
		settings.gs_width = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "width", "500");
		settings.gs_width = 500;
	}

	/* other panes' sizes */
	/* some of these sizes come out of the glade/gtkbuilder files */
	/* must maintain them in parallel */
	if ((buf = xml_get_value("layout", "parallel_height")))
		settings.parallel_height = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "parallel_height",
					    "361");
		settings.parallel_height = 361;
	}
	if ((buf = xml_get_value("layout", "parallel_width")))
		settings.parallel_width = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "parallel_width",
					    "657");
		settings.parallel_width = 657;
	}

	if ((buf = xml_get_value("layout", "modmgr_height")))
		settings.modmgr_height = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "modmgr_height",
					    "450");
		settings.modmgr_height = 450;
	}
	if ((buf = xml_get_value("layout", "modmgr_width")))
		settings.modmgr_width = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "modmgr_width",
					    "710");
		settings.modmgr_width = 710;
	}

	if ((buf = xml_get_value("layout", "advsearch_height")))
		settings.advsearch_height = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "advsearch_height",
					    "465");
		settings.advsearch_height = 465;
	}
	if ((buf = xml_get_value("layout", "advsearch_width")))
		settings.advsearch_width = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "advsearch_width",
					    "665");
		settings.advsearch_width = 665;
	}

	if ((buf = xml_get_value("layout", "prefs_height")))
		settings.prefs_height = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "prefs_height",
					    "300");
		settings.prefs_height = 300;
	}
	if ((buf = xml_get_value("layout", "prefs_width")))
		settings.prefs_width = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "prefs_width",
					    "590");
		settings.prefs_width = 590;
	}

	/* main window placement */
	if ((buf = xml_get_value("layout", "maximized")))
		settings.app_x = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "maximized", "0");
		settings.maximized = 0;
	}
	if ((buf = xml_get_value("layout", "app_x")))
		settings.app_x = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "app_x", "40");
		settings.app_x = 40;
	}
	if ((buf = xml_get_value("layout", "app_y")))
		settings.app_y = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "app_y", "40");
		settings.app_y = 40;
	}

	/* other windows' placement */
	if ((buf = xml_get_value("layout", "parallel_x")))
		settings.parallel_x = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "parallel_x", "40");
		settings.parallel_x = 40;
	}
	if ((buf = xml_get_value("layout", "parallel_y")))
		settings.parallel_y = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "parallel_y", "40");
		settings.parallel_y = 40;
	}

	if ((buf = xml_get_value("layout", "modmgr_x")))
		settings.modmgr_x = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "modmgr_x", "40");
		settings.modmgr_x = 40;
	}
	if ((buf = xml_get_value("layout", "modmgr_y")))
		settings.modmgr_y = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "modmgr_y", "40");
		settings.modmgr_y = 40;
	}

	if ((buf = xml_get_value("layout", "advsearch_x")))
		settings.advsearch_x = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "advsearch_x", "40");
		settings.advsearch_x = 40;
	}
	if ((buf = xml_get_value("layout", "advsearch_y")))
		settings.advsearch_y = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "advsearch_y", "40");
		settings.advsearch_y = 40;
	}

	if ((buf = xml_get_value("layout", "prefs_x")))
		settings.prefs_x = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "prefs_x", "40");
		settings.prefs_x = 40;
	}
	if ((buf = xml_get_value("layout", "prefs_y")))
		settings.prefs_y = atoi(buf);
	else {
		xml_add_new_item_to_section("layout", "prefs_y", "40");
		settings.prefs_y = 40;
	}

	/* random geometry stuff */
	settings.biblepane_width =
	    atoi((buf = xml_get_value("layout", "textpane"))
		     ? buf
		     : "0");

	if ((buf = xml_get_value("layout", "bibleheight")))
		settings.biblepane_height = atoi(buf);
	else {
		if ((buf = xml_get_value("layout", "biblehight"))) { /* backward compatible */
			settings.biblepane_height = atoi(buf);
			xml_remove_node("layout", "biblehight", 0);
		} else
			settings.biblepane_height = 250;
		buf = g_strdup_printf("%d", settings.biblepane_height);
		xml_add_new_item_to_section("layout", "bibleheight", buf);
		g_free(buf);
	}

	if ((buf = xml_get_value("layout", "commentaryheight")))
		settings.commpane_height = atoi(buf);
	else {
		if ((buf = xml_get_value("layout", "commentaryhight"))) { /* backward compatible */
			settings.commpane_height = atoi(buf);
			xml_remove_node("layout", "commentaryhight", 0);
		} else
			settings.commpane_height = 240;
		buf = g_strdup_printf("%d", settings.commpane_height);
		xml_add_new_item_to_section("layout", "commentaryheight",
					    buf);
		g_free(buf);
	}

	/* font sizes */
	if ((buf = xml_get_value("fontsize", "versenum"))) {
		settings.verse_num_font_size_str = buf;
		settings.verse_num_font_size = atoi(buf);
	} else {
		settings.verse_num_font_size_str = g_strdup("-1");
		settings.verse_num_font_size = -1;
	}

	if ((buf = xml_get_value("fontsize", "basefontsize"))) {
		settings.base_font_size_str = buf;
		settings.base_font_size =
		    atoi(settings.base_font_size_str);
	} else {
#ifndef WIN32
		xml_add_new_item_to_section("fontsize", "basefontsize",
					    "+0");
		settings.base_font_size_str = g_strdup("+0");
		settings.base_font_size = 0;
#else
		xml_add_new_item_to_section("fontsize", "basefontsize",
					    "+1");
		settings.base_font_size_str = g_strdup("+1");
		settings.base_font_size = 1;
#endif /* WIN32 */
	}

	/* colors */
	settings.bible_text_color = xml_get_value("HTMLcolors", "text_fg");
	settings.bible_bg_color =
	    xml_get_value("HTMLcolors", "background");
	settings.currentverse_color =
	    xml_get_value("HTMLcolors", "currentverse");
	settings.link_color = xml_get_value("HTMLcolors", "link");
	settings.bible_verse_num_color =
	    xml_get_value("HTMLcolors", "versenum");
	settings.found_color = xml_get_value("HTMLcolors", "found");

	if ((buf = xml_get_value("HTMLcolors", "highlight_fg")))
		settings.highlight_fg = g_strdup(buf);
	else {
		xml_add_new_item_to_section("HTMLcolors", "highlight_fg",
					    "#FFFF00");
		settings.highlight_fg = g_strdup("#FFFF00");
	}
	if ((buf = xml_get_value("HTMLcolors", "highlight_bg")))
		settings.highlight_bg = g_strdup(buf);
	else {
		xml_add_new_item_to_section("HTMLcolors", "highlight_bg",
					    "#060680");
		settings.highlight_bg = g_strdup("#060680");
	}

	/*  parallel ops  */
	if ((buf = xml_get_value("parallel", "Strong_s_Numbers")))
		settings.parallel_strongs = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Strong_s_Numbers",
					    "0");
		settings.parallel_strongs = 0;
	}

	if ((buf = xml_get_value("parallel", "Morphological_Tags")))
		settings.parallel_morphs = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel",
					    "Morphological_Tags", "0");
		settings.parallel_morphs = 0;
	}

	if ((buf = xml_get_value("parallel", "Hebrew_Vowel_Points")))
		settings.parallel_hebrewpoints = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel",
					    "Hebrew_Vowel_Points", "1");
		settings.parallel_hebrewpoints = 1;
	}

	if ((buf = xml_get_value("parallel", "Hebrew_Cantillation")))
		settings.parallel_cantillationmarks = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel",
					    "Hebrew_Cantillation", "0");
		settings.parallel_cantillationmarks = 0;
	}

	if ((buf = xml_get_value("parallel", "Footnotes")))
		settings.parallel_footnotes = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Footnotes", "1");
		settings.parallel_footnotes = 1;
	}

	if ((buf = xml_get_value("parallel", "Cross-references")))
		settings.parallel_crossref = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Cross-references",
					    "1");
		settings.parallel_crossref = 1;
	}

	if ((buf = xml_get_value("parallel", "Transliteration")))
		settings.parallel_transliteration = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Transliteration",
					    "0");
		settings.parallel_transliteration = 0;
	}

	if ((buf = xml_get_value("parallel", "Words_of_Christ_in_Red")))
		settings.parallel_red_words = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel",
					    "Words_of_Christ_in_Red", "1");
		settings.parallel_red_words = 1;
	}

	if ((buf = xml_get_value("parallel", "Morpheme_Segmentation")))
		settings.parallel_segmentation = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel",
					    "Morpheme_Segmentation", "0");
		settings.parallel_segmentation = 0;
	}

	if ((buf = xml_get_value("parallel", "Headings")))
		settings.parallel_headings = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Headings", "1");
		settings.parallel_headings = 1;
	}

	if ((buf = xml_get_value("parallel", "Lemmas")))
		settings.parallel_lemmas = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Lemmas", "0");
		settings.parallel_lemmas = 0;
	}

	if ((buf = xml_get_value("parallel", "Primary_Reading")))
		settings.parallel_variants_primary = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Primary_Reading",
					    "1");
		settings.parallel_variants_primary = 1;
	}

	if ((buf = xml_get_value("parallel", "Secondary_Reading")))
		settings.parallel_variants_secondary = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel",
					    "Secondary_Reading", "0");
		settings.parallel_variants_secondary = 0;
	}

	if ((buf = xml_get_value("parallel", "All_Readings")))
		settings.parallel_variants_all = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "All_Readings",
					    "0");
		settings.parallel_variants_all = 0;
	}

	if ((buf = xml_get_value("parallel", "Greek_Accents")))
		settings.parallel_greekaccents = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Greek_Accents",
					    "1");
		settings.parallel_greekaccents = 1;
	}

	if ((buf = xml_get_value("parallel", "Transliterated_Forms")))
		settings.parallel_xlit = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel",
					    "Transliterated_Forms", "0");
		settings.parallel_xlit = 0;
	}

	if ((buf = xml_get_value("parallel", "Enumerations")))
		settings.parallel_enumerated = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Enumerations",
					    "0");
		settings.parallel_enumerated = 0;
	}

	if ((buf = xml_get_value("parallel", "Glosses")))
		settings.parallel_glosses = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Glosses", "0");
		settings.parallel_glosses = 0;
	}

	if ((buf = xml_get_value("parallel", "Morpheme_Segmentation")))
		settings.parallel_morphseg = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel",
					    "Morpheme_Segmentation", "0");
		settings.parallel_morphseg = 0;
	}

	/*  Misc stuff  */

	settings.showtexts =
	    atoi((buf = xml_get_value("misc", "showtexts")) ? buf : "1");
	settings.showcomms =
	    atoi((buf = xml_get_value("misc", "showcomms")) ? buf : "1");
	settings.showdicts =
	    atoi((buf = xml_get_value("misc", "showdicts")) ? buf : "1");
	if (xml_get_value("misc", "showpreview"))
		settings.showpreview =
		    atol(xml_get_value("misc", "showpreview"));
	else {
		xml_add_new_item_to_section("misc", "showpreview", "1");
		settings.showpreview = 1;
	}

	if (xml_get_value("misc", "show_side_preview"))
		settings.show_previewer_in_sidebar =
		    atol(xml_get_value("misc", "show_side_preview"));
	else {
		xml_add_new_item_to_section("misc", "show_side_preview",
					    "1");
		settings.show_previewer_in_sidebar = 1;
	}

	if (xml_get_value("misc", "showparatab"))
		settings.showparatab =
		    atol(xml_get_value("misc", "showparatab"));
	else {
		xml_add_new_item_to_section("misc", "showparatab", "0");
		settings.showparatab = 0;
	}

	settings.showsplash =
	    atoi((buf = xml_get_value("misc", "splash")) ? buf : "1");

	settings.showdevotional =
	    atoi((buf =
		      xml_get_value("misc", "dailydevotional"))
		     ? buf
		     : "1");
	settings.versestyle =
	    atoi((buf = xml_get_value("misc", "versestyle")) ? buf : "1");

	if ((buf = xml_get_value("misc", "pinnedtabs")))
		settings.linkedtabs = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "pinnedtabs", "0");
		settings.linkedtabs = 0;
	}

	if ((buf = xml_get_value("misc", "readaloud")))
		settings.readaloud = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "readaloud", "0");
		settings.readaloud = 0;
	}

	if ((buf = xml_get_value("misc", "showversenum")))
		settings.showversenum = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "showversenum", "1");
		settings.showversenum = 1;
	}

	if ((buf = xml_get_value("misc", "verse_num_bold")))
		settings.verse_num_bold = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "verse_num_bold", "0");
		settings.verse_num_bold = 0;
	}

	if ((buf = xml_get_value("misc", "verse_num_bracket")))
		settings.verse_num_bracket = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "verse_num_bracket",
					    "0");
		settings.verse_num_bracket = 0;
	}

	if ((buf = xml_get_value("misc", "verse_num_superscript")))
		settings.verse_num_superscript = atoi(buf);
	else {
		xml_add_new_item_to_section("misc",
					    "verse_num_superscript", "0");
		settings.verse_num_superscript = 0;
	}

	if ((buf = xml_get_value("misc", "versehighlight")))
		settings.versehighlight = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "versehighlight", "0");
		settings.versehighlight = 0;
	}

	if ((buf = xml_get_value("misc", "annotatehighlight")))
		settings.annotate_highlight = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "annotatehighlight",
					    "1");
		settings.annotate_highlight = 1;
	}

	if ((buf = xml_get_value("misc", "xrefsinverselist")))
		settings.xrefs_in_verse_list = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "xrefsinverselist",
					    "1");
		settings.xrefs_in_verse_list = 1;
	}

	if ((buf = xml_get_value("misc", "chapter-scroll"))) {
		settings.chapter_scroll = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc", "chapter-scroll", "0");
		settings.chapter_scroll = 0;
	}

	if ((buf = xml_get_value("misc", "imageresize"))) {
		settings.imageresize = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc", "imageresize", "1");
		settings.imageresize = 1;
	}
#ifdef WIN32
	/* due to hackery caused by webkit "no images" bug */
	settings.imageresize = 1;
#endif

	if ((buf = xml_get_value("editor", "spell_language")))
		settings.spell_language = g_strdup(buf);
	else {
		xml_add_new_item_to_section("editor", "spell_language",
					    "unknown");
		settings.spell_language = "unknown";
	}

	settings.studypadfilename = xml_get_value("studypad", "lastfile");
	settings.studypaddir = xml_get_value("studypad", "directory");

	if ((buf = xml_get_value("misc", "show_sidebar"))) {
		settings.showshortcutbar = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc", "show_sidebar", "1");
		settings.showshortcutbar = 1;
	}
	if ((buf = xml_get_value("misc", "sidebar_docked"))) {
		settings.docked = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc", "sidebar_docked", "1");
		settings.docked = 1;
	}

	if ((buf = xml_get_value("misc", "prayerlist"))) {
		settings.prayerlist = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc", "prayerlist", "1");
		settings.prayerlist = 1;
	}

	if ((buf = xml_get_value("misc", "statusbar"))) {
		settings.statusbar = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc", "statusbar", "1");
		settings.statusbar = 1;
	}

	if ((buf = xml_get_value("misc", "alternation"))) {
		settings.alternation = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc", "alternation", "1");
		settings.alternation = 1;
	}

#if 1
	settings.browsing = 1; /* unconditional - no longer toggle-able. */
#else
	if ((buf = xml_get_value("tabs", "browsing"))) {
		settings.browsing = atoi(buf);
	} else {
		xml_add_new_item_to_section("tabs", "browsing", "1");
		settings.browsing = 1;
	}
#endif
}
