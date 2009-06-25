/*
 * Xiphos Bible Study Tool
 * settings.c - Guess what?
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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
#include "gui/mod_mgr.h"
#include "gui/utilities.h"

#include "main/lists.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include <glib/gstdio.h>


/******************************************************************************
 * defines
 */
#define GS_DIR ".xiphos"

#define GS_NET_PERMISSION	_("Welcome to Xiphos.\n\nThere are no Bible modules installed. In order to initialize, Xiphos needs you to install at least one Bible.\n\nWith your permission, Xiphos will invoke the Module Manager so that you may install from Crosswire:\n1. Configure remote install.\n2. Connect.\n3. Select a Bible text of your language preference.\n4. Click `install'.\nClose the Module Manager when you are done.\n\nWarning: If you live in a persecuted country, use with care.\n\nMay Xiphos invoke the Module Manager so that you may install a Bible?")

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

int settings_init(int new_configs, int new_bookmarks)
{
	int retval = 0;
	char *sword_dir = NULL;
	char *gs_old = NULL;
	char *tmp = NULL;
	char *env;

	settings.first_run = FALSE;
	/* set program title */
	strcpy(settings.program_title, "Xiphos");

	/* find out what kind of peculiar language environment we have */
	re_encode_digits = FALSE;
	if (((env = (char*) g_getenv("LC_ALL")) != NULL) &&
	    // for now, farsi-sensitive only
	    (strncmp (env, "fa", 2) == 0)) {
		re_encode_digits = TRUE;
	}

	/* Get home dir */
	if ((settings.homedir = (char*) g_getenv(HOMEVAR)) == NULL) {
		gui_generic_warning(_("$HOME is not set!"));
		/* if not found in env exit */
		exit(0);
	}

	/* set gSwordDir to $home + .xiphos */
	settings.gSwordDir = g_new(char, strlen(settings.homedir) +
				   strlen(GS_DIR) + 2);
	sprintf(settings.gSwordDir, "%s/%s", settings.homedir, GS_DIR);

	/* if gSwordDir does not exist, create it. */
	if (access(settings.gSwordDir, F_OK) == -1) {
		gs_old = g_strdup_printf("%s/.xiphos-2.0", settings.homedir);
		if (access(gs_old, F_OK) == 0) {
			if (rename(gs_old, settings.gSwordDir) == 0)
				gui_generic_warning(_("Xiphos has renamed .xiphos-2.0 to .xiphos"));
			else {
				char msg[300];
				sprintf(msg, _("Xiphos can not rename  .xiphos-2.0 to .xiphos:\n%s\n\nXiphos cannot continue."),
					strerror(errno));
				gui_generic_warning(msg);
				/* necessarily giving up. */
				gtk_exit(1);
			}
		} else if ((Mkdir(settings.gSwordDir, S_IRWXU)) != 0) {
			char msg[300];
			sprintf(msg, _("Xiphos can not create  .xiphos:\n%s\n\nXiphos cannot continue."),
				strerror(errno));
			gui_generic_warning(msg);
			/* if we can not create gSwordDir exit */
			gtk_exit(1);
		}
		g_free(gs_old);
	}

	/* if .sword does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir, DOTSWORD);
	if (access(sword_dir, F_OK) == -1) {
		if ((Mkdir(sword_dir, S_IRWXU)) != 0) {
			gui_generic_warning(_("can not create " DOTSWORD));
		} 
	}
	g_free(sword_dir);
	/* if .sword/mods.d does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir, DOTSWORD "/mods.d");
	if (access(sword_dir, F_OK) == -1) {
		if ((Mkdir(sword_dir, S_IRWXU)) != 0) {
			gui_generic_warning(_("can not create " DOTSWORD "/mods.d"));
		} 
	}	
	g_free(sword_dir);
	/* if .sword/modules does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir, DOTSWORD "/modules");
	if (access(sword_dir, F_OK) == -1) {
		if ((Mkdir(sword_dir, S_IRWXU)) != 0) {
			gui_generic_warning(_("can not create " DOTSWORD "/modules"));
		} 
	}
	g_free(sword_dir);
	
	/* moved here from crud locations in backend. */
	main_init_language_map();

	language_init();

	init_bookmarks(new_bookmarks);

	/* set fnconfigure to gSwordDir and settings.xml */
	settings.fnconfigure = g_new(char, strlen(settings.gSwordDir) +
				     strlen("settings.xml") + 2);
	sprintf(settings.fnconfigure, "%s/%s", settings.gSwordDir,
		"settings.xml");
	
	/* if settings.xml does not exist create it */
	if ((access(settings.fnconfigure, F_OK) == -1) || new_configs) {
		/* must be first run */
		GS_print(("\nFirst Run: need to create settings!\n"));
		settings.first_run = TRUE;

		main_init_lists();
		if (settings.havebible == 0) {
			if (gui_yes_no_dialog(GS_NET_PERMISSION, NULL)) {
				main_shutdown_list();
				gui_open_mod_mgr_initial_run();
				main_init_lists();
				if (settings.havebible == 0) {
					gui_generic_warning
					    (_("There are still no Bibles installed.\nEvidently, you declined to install any.\n\nWithout any Bible modules to display,\nXiphos cannot proceed,\nand will now exit."));
					exit(1);
				} else
					gui_generic_warning
					    (_("Bible module installation complete."));
			} else {
				gui_generic_warning
				    (_("Without any Bible modules to display,\nXiphos cannot proceed,\nand will now exit."));
				exit(1);
			}
		}
		main_shutdown_list();

		main_init_lists();
		xml_create_settings_file(settings.fnconfigure);
		//retval = gui_first_run();
		main_shutdown_list();
		xml_save_settings_doc(settings.fnconfigure);
		xml_free_settings_doc();
	}
	
	/* check for template.pad file for studypad */
	tmp = g_new(char, strlen(settings.gSwordDir) +
				     strlen("template.pad") + 2);
	sprintf(tmp, "%s/%s", settings.gSwordDir,
		"template.pad");
	
	/* if template.pad does not exist create it */
	if ((access(tmp, F_OK) == -1)) {
		g_file_set_contents(tmp, " ", strlen(" "), NULL);
	}
	
	/* check for template.pad file for studypad */
	/* set fnconfigure to gSwordDir and settings.xml */
	tmp = g_new(char, strlen(settings.gSwordDir) +
				     strlen("template.pad") + 2);
	sprintf(tmp, "%s/%s", settings.gSwordDir,
		"template.pad");
	
	/* if template.pad does not exist create it */
	if ((access(tmp, F_OK) == -1)) {
		g_file_set_contents(tmp, " ", strlen(" "), NULL);
	}
    
	xml_parse_settings_file(settings.fnconfigure);
	load_settings_structure();

	gconf_setup();

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
	char *removed = NULL;
	//int load_old = FALSE;

	settings.load_xml_bookmarks = FALSE;

	/* set bookmarks dir to settings.gSwordDir + /bookmarks */
	settings.swbmDir = g_new(char, strlen(settings.gSwordDir) +
				 strlen("/bookmarks") + 2);
	sprintf(settings.swbmDir, "%s/%s", settings.gSwordDir, "bookmarks");

	/* if .xiphos-2.0/bookmarks does not exist create it */
	if (access(settings.swbmDir, F_OK) == -1) {
		if ((Mkdir(settings.swbmDir, S_IRWXU)) == -1) {
			g_warning("can't create bookmarks dir");
			return 0;
		}
	}
					
	/* set removed dir to settings.swbmDir + /removed */
	removed = g_new(char, strlen(settings.swbmDir) +
				 strlen("/removed") + 2);
	sprintf(removed, "%s/%s", settings.swbmDir, "removed");
	if (access(removed, F_OK) == -1) {
		if ((Mkdir(removed, S_IRWXU)) == -1) {
			g_warning("can't create removed dir");
			//return 0;
		}
	}	
	
	if (new_bookmarks) 
		xml_new_bookmark_file();
		
	/* check for xml bookmarks */
	file_buf = g_new(char, strlen(settings.swbmDir) +
			 strlen("/bookmarks.xml") + 2);
	sprintf(file_buf, "%s/bookmarks.xml", settings.swbmDir);

	if (access(file_buf, F_OK) == 0) {
		settings.load_xml_bookmarks = TRUE;
	}
	g_free(file_buf);

	if (!settings.load_xml_bookmarks) {
		xml_new_bookmark_file();
		settings.load_xml_bookmarks = TRUE;
	}

	/* check for Bibletime bookmarks */
	settings.have_bibletime = FALSE;
	file_buf = g_new(char, strlen(settings.homedir) +
			 strlen
			 ("/.kde/share/apps/bibletime/bookmarks.xml") +
			 2);
	sprintf(file_buf, "%s/.kde/share/apps/bibletime/bookmarks.xml",
		settings.homedir);
	if (access(file_buf, F_OK) == 0) {
		settings.have_bibletime = TRUE;
	}
	
	/* clean up */
	if (file_buf)
		g_free(file_buf);
	if (removed)
		g_free(removed);
	
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
	char *buf = NULL;
	
	settings.gs_version = VERSION;
	settings.MainWindowModule = xml_get_value("modules", "bible");
	settings.CommWindowModule = xml_get_value("modules", "comm");
	settings.DictWindowModule = xml_get_value("modules", "dict");
	settings.parallel1Module = xml_get_value("modules", "int1");
	settings.parallel2Module = xml_get_value("modules", "int2");
	settings.parallel3Module = xml_get_value("modules", "int3");
	settings.parallel4Module = xml_get_value("modules", "int4");
	settings.parallel5Module = xml_get_value("modules", "int5");
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

	settings.DefaultDict = xml_get_value("lexicons", "defaultdictionary");
	if (!settings.DefaultDict) {
		settings.DefaultDict = g_strdup("WebstersDict");
		xml_set_value("Xiphos", "lexicons", "defaultdictionary",
			      settings.DefaultDict);
	}

	settings.useDefaultDict = atoi(xml_get_value("lexicons","usedefaultdict"));

	/* mod mgr stuff */ 
	if (xml_get_value("modmgr", "mod_mgr_source"))
		settings.mod_mgr_source = atoi(xml_get_value("modmgr", "mod_mgr_source"));
	else {
		xml_add_new_section_to_settings_doc("modmgr");
		xml_add_new_item_to_section("modmgr", "mod_mgr_source", "1");
		settings.mod_mgr_source = 1;
	}	 
	if (xml_get_value("modmgr", "mod_mgr_destination"))
		settings.mod_mgr_destination = atoi(xml_get_value("modmgr", "mod_mgr_destination"));
	else {
		xml_add_new_item_to_section("modmgr", "mod_mgr_destination", "0");
		settings.mod_mgr_destination = 0;
	}		
	if (xml_get_value("modmgr", "mod_mgr_local_source_index"))
		settings.mod_mgr_local_source_index = 
			atoi(xml_get_value("modmgr", "mod_mgr_local_source_index"));
	else {
		xml_add_new_item_to_section("modmgr", "mod_mgr_local_source_index", "0");
		settings.mod_mgr_local_source_index = 0;
	}		
	if (xml_get_value("modmgr", "mod_mgr_remote_source_index"))
		settings.mod_mgr_remote_source_index =
			atoi(xml_get_value("modmgr", "mod_mgr_remote_source_index"));
	else {
		xml_add_new_item_to_section("modmgr", "mod_mgr_remote_source_index", "0");
		settings.mod_mgr_remote_source_index = 0;
	}	
	
	/* end mod mgr stuff */
	
	
	settings.currentverse = xml_get_value("keys", "verse");
	settings.dictkey = xml_get_value("keys", "dictionary");

	if (xml_get_value("keys", "book")) {
		settings.book_key = xml_get_value("keys", "book");
	} else {
		xml_add_new_item_to_section("keys", "book", "0");
		settings.book_key = xml_get_value("keys", "book");
	}
	if (xml_get_value("keys", "offset"))
		settings.book_offset = atol(xml_get_value("keys", "offset"));
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

	settings.sidebar_width =
		atoi(xml_get_value("layout", "shortcutbar"));
	if (xml_get_value("layout", "vltoppaneheight"))
		settings.verselist_toppane_height =
			atoi(xml_get_value("layout", "vltoppaneheight"));
	else {
		xml_add_new_item_to_section("layout", "vltoppaneheight", "210");
		settings.verselist_toppane_height = 210;
	}
	if (xml_get_value("layout", "sidebar_notebook_hight"))
		settings.sidebar_notebook_hight =
			atoi(xml_get_value("layout", "sidebar_notebook_hight"));
	else {
		xml_add_new_item_to_section("layout", "sidebar_notebook_hight", "300");
		settings.sidebar_notebook_hight = 250;
	}
	settings.gs_hight = atoi(xml_get_value("layout", "hight"));
	settings.gs_width = atoi(xml_get_value("layout", "width"));
	
	if (xml_get_value("layout", "app_x"))
		settings.app_x = atoi(xml_get_value("layout", "app_x"));
	else {
		xml_add_new_item_to_section("layout", "app_x", "40");
		settings.app_x = 40;
	}
	if (xml_get_value("layout", "app_y"))
		settings.app_y = atoi(xml_get_value("layout", "app_y"));
	else{
		xml_add_new_item_to_section("layout", "app_y", "40");
		settings.app_y = 40;
	}
	
	settings.biblepane_width =
	    atoi(xml_get_value("layout", "textpane"));
	settings.upperpane_hight =
	    atoi(xml_get_value("layout", "uperpane"));

	if (xml_get_value("layout", "biblehight"))
		settings.biblepane_hight = atoi(xml_get_value("layout", "biblehight"));
	else {
		xml_add_new_item_to_section("layout", "biblehight", "340");
		settings.biblepane_hight = 250;
	}

	if (xml_get_value("layout", "commentaryhight"))
		settings.commpane_hight = atoi(xml_get_value("layout", "commentaryhight"));
	else {
		xml_add_new_item_to_section("layout", "commentaryhight", "240");
		settings.commpane_hight = 240;
	}
	
	settings.verse_num_font_size_str = xml_get_value("fontsize", "versenum");
	settings.verse_num_font_size = atoi(settings.verse_num_font_size_str);

	if ((buf = xml_get_value("fontsize", "basefontsize"))) {
		settings.base_font_size_str = g_strdup(buf);
		settings.base_font_size = atoi(settings.base_font_size_str);
	} else {
#ifndef WIN32
		xml_add_new_item_to_section("fontsize", "basefontsize", "+0");
		settings.base_font_size_str = g_strdup("+0");
		settings.base_font_size = 0;
#else
		xml_add_new_item_to_section("fontsize", "basefontsize", "+1");
		settings.base_font_size_str = g_strdup("+1");
		settings.base_font_size = 1;
#endif /* WIN32 */
	}
 
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
		xml_add_new_item_to_section("HTMLcolors", "highlight_fg", "#FFFF00");
		settings.highlight_fg = g_strdup("#FFFF00");
	}
	if ((buf = xml_get_value("HTMLcolors", "highlight_bg")))
		settings.highlight_bg = g_strdup(buf);
	else {
		xml_add_new_item_to_section("HTMLcolors", "highlight_bg", "#060680");
		settings.highlight_bg = g_strdup("#060680");
	}
	
	/*  parallel ops  */
	
	if ((buf = xml_get_value("parallel", "Strong_s_Numbers")))
		settings.parallel_strongs = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Strong_s_Numbers", "0");
		settings.parallel_strongs = 0;
	}
	
	if ((buf = xml_get_value("parallel", "Morphological_Tags")))
		settings.parallel_morphs = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Morphological_Tags", "0");
		settings.parallel_morphs = 0;
	}
	
	if ((buf = xml_get_value("parallel", "Hebrew_Vowel_Points")))
		settings.parallel_hebrewpoints = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Hebrew_Vowel_Points", "1");
		settings.parallel_hebrewpoints = 1;
	}
	
	if ((buf = xml_get_value("parallel", "Hebrew_Cantillation")))
		settings.parallel_cantillationmarks = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Hebrew_Cantillation", "0");
		settings.parallel_cantillationmarks = 0;
	}
	
	if ((buf = xml_get_value("parallel", "Footnotes")))
		settings.parallel_footnotes = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Footnotes", "0");
		settings.parallel_footnotes = 0;
	}	

	if ((buf = xml_get_value("parallel", "Cross-references")))
		settings.parallel_crossref = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Cross-references", "1");
		settings.parallel_crossref = 1;
	}		

	if ((buf = xml_get_value("parallel", "Transliteration")))
		settings.parallel_transliteration = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Transliteration", "0");
		settings.parallel_transliteration = 0;
	}
	
	if ((buf = xml_get_value("parallel", "Words_of_Christ_in_Red")))
		settings.parallel_red_words = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Words_of_Christ_in_Red", "0");
		settings.parallel_red_words = 0;
	}
	
	if ((buf = xml_get_value("parallel", "Morpheme_Segmentation")))
		settings.parallel_segmentation = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Morpheme_Segmentation", "0");
		settings.parallel_segmentation = 0;
	}
	
	if ((buf = xml_get_value("parallel", "Headings")))
		settings.parallel_headings = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Headings", "0");
		settings.parallel_headings = 0;
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
		xml_add_new_item_to_section("parallel", "Primary_Reading", "1");
		settings.parallel_variants_primary = 1;
	}	
	
	if ((buf = xml_get_value("parallel", "Secondary_Reading")))
		settings.parallel_variants_secondary = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Secondary_Reading", "0");
		settings.parallel_variants_secondary = 0;
	}
	
	if ((buf = xml_get_value("parallel", "All_Readings")))
		settings.parallel_variants_all = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "All_Readings", "0");
		settings.parallel_variants_all = 0;
	}
	
	if ((buf = xml_get_value("parallel", "Greek_Accents")))
		settings.parallel_greekaccents = atoi(buf);
	else {
		xml_add_new_item_to_section("parallel", "Greek_Accents", "1");
		settings.parallel_greekaccents = 1;
	}
	
	buf = xml_get_value("parallel", "parallel");
	if (buf)
	settings.parallelpage = atoi(buf);

	
	/*  Misc stuff  */
	
	settings.showtexts = atoi(xml_get_value("misc", "showtexts"));
	settings.showcomms = atoi(xml_get_value("misc", "showcomms"));
	settings.showdicts = atoi(xml_get_value("misc", "showdicts"));
	if (xml_get_value("misc", "showpreview"))
		settings.showpreview = atol(xml_get_value("misc", "showpreview"));
	else {
		xml_add_new_item_to_section("misc", "showpreview", "1");
		settings.showpreview = 1;
	}

	if (xml_get_value("misc", "show_side_preview"))
		settings.show_previewer_in_sidebar = atol(xml_get_value("misc", "show_side_preview"));
	else {
		xml_add_new_item_to_section("misc", "show_side_preview", "0");
		settings.show_previewer_in_sidebar = 0;
	}
    
	if (xml_get_value("misc", "showparatab"))
		settings.showparatab = atol(xml_get_value("misc", "showparatab"));
	else {
		xml_add_new_item_to_section("misc", "showparatab", "0");
		settings.showparatab = 0;
	}	
	
	settings.showsplash = atoi(xml_get_value("misc", "splash"));
	
	settings.showdevotional =
	    atoi(xml_get_value("misc", "dailydevotional"));
	settings.versestyle = atoi(xml_get_value("misc", "versestyle"));

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

	if ((buf = xml_get_value("misc", "versehighlight")))
		settings.versehighlight = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "versehighlight", "0");
		settings.versehighlight = 0;
	}

	if ((buf = xml_get_value("misc", "doublespace")))
		settings.doublespace = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "doublespace", "0");
		settings.doublespace = 0;
	}

	if ((buf = xml_get_value("misc", "chapter-scroll"))) {
		settings.chapter_scroll = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc","chapter-scroll","0");
		settings.chapter_scroll = 0;
	}

	if ((buf = xml_get_value("misc", "imageresize"))) {
		settings.imageresize = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc","imageresize","1");
		settings.imageresize = 1;
	}

	if ((buf = xml_get_value("editor", "spell_language")))
		settings.spell_language = g_strdup(buf);
	else {
		xml_add_new_item_to_section("editor","spell_language","unknown");
		settings.spell_language = "unknown";
	}

	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	settings.studypaddir = xml_get_value("studypad", "directory");

	if (xml_get_value("misc", "show_sidebar")) {
		buf = xml_get_value("misc", "show_sidebar");
		settings.showshortcutbar = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc","show_sidebar","1");
		settings.showshortcutbar = 1;
	}		
	if (xml_get_value("misc", "sidebar_docked")) {
		buf = xml_get_value("misc", "sidebar_docked");
		settings.docked = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc","sidebar_docked","1");
		settings.docked = 1;
	}
	
	if (xml_get_value("misc", "prayerlist")) {
		buf = xml_get_value("misc", "prayerlist");
		settings.prayerlist =  atoi(buf);
	} else {
		xml_add_new_item_to_section("misc","prayerlist","1");
		settings.prayerlist = 1;
	}

	
	if (xml_get_value("tabs", "browsing")) {
		buf = xml_get_value("tabs", "browsing");
		settings.browsing =  atoi(buf);
	} else {
		xml_add_new_item_to_section("tabs","browsing","1");
		settings.browsing = 1;
	}
	/*
	   settings. = xml_get_value("", "");
	   settings. = xml_get_value("", "");
	   settings. = xml_get_value("", "");
	   settings. = xml_get_value("", "");
	   settings. = xml_get_value("", "");
	 */
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
