/*
 * GnomeSword Bible Study Tool
 * settings.c - Guess what?
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "gui/bookmarks.h"
#include "gui/setup_druid.h"

#include "main/settings.h"
#include "main/search.h"
#include "main/shortcutbar.h"
#include "main/xml.h"

#include "backend/sword.h"


/******************************************************************************
 * defines
 */
#define GS_DIR ".gnomesword-1.0"


/******************************************************************************
 * globals
 */
SETTINGS settings;


/******************************************************************************
 * static
 */
static void old_2_new_shortcut_file(char * old_file, char * new_file);
static int init_old(void);
static int init_bookmarks(int new_bookmarks, int have_old);

static char *old_prefs = NULL;
static char *old_dir = NULL;
static char *old_bm = NULL;
static char *old_sb = NULL;
static char *old_fav = NULL;
static char *old_text = NULL;
static char *old_comm = NULL;
static char *old_dict = NULL;
static char *old_gbs = NULL;


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
 *   check for GnomeSword dir
 *   if not found - create it and
 *   the files needed to run program
 *
 * Return value
 *   gint
 */

int settings_init(int new_configs, int new_bookmarks)
{
	int retval = 0;
	int have_old = FALSE;
	int need_old = FALSE;

	/* set program title */
	strcpy(settings.program_title, "GnomeSword");

	/* Get home dir */
	if ((settings.homedir = getenv("HOME")) == NULL) {
		g_error("$HOME is not set!");
		/* if not found in env exit */
		exit(0);
	}

	/* set gSwordDir to $home + .gnomesword-1.0 */
	settings.gSwordDir = g_new(char, strlen(settings.homedir) +
				   strlen(GS_DIR) + 2);
	sprintf(settings.gSwordDir, "%s/%s", settings.homedir, GS_DIR);

	/* if gSwordDir does not exist create it */
	if (access(settings.gSwordDir, F_OK) == -1) {
		if ((mkdir(settings.gSwordDir, S_IRWXU)) != 0) {
			printf("can not create  .gnomesword-1.0");
			/* if we can not create gSwordDir exit */
			gtk_exit(1);
		} else
			need_old = TRUE;
	}

	/* shortcutbar dir */
	settings.shortcutbarDir =
	    g_new(char,
		  strlen(settings.gSwordDir) + strlen("shortcutbar") +
		  2);
	sprintf(settings.shortcutbarDir, "%s/%s", settings.gSwordDir,
		"shortcutbar");


	if (need_old) {
		have_old = init_old();
	}

	init_bookmarks(new_bookmarks, have_old);

	/* if shortcutbar dir does not exist create it */
	if (access(settings.shortcutbarDir, F_OK) == -1) {
		if ((mkdir(settings.shortcutbarDir, S_IRWXU)) == -1) {
			printf("can't create shortcutbar dir");
			gtk_exit(1);
		}
	}


	/* set fnconfigure to gSwordDir and settings.xml */
	settings.fnconfigure = g_new(char, strlen(settings.gSwordDir) +
				     strlen("settings.xml") + 2);
	sprintf(settings.fnconfigure, "%s/%s", settings.gSwordDir,
		"settings.xml");
/*
	if(new_configs){
		// user ask for new configs 
		g_print("\nCreate new configs: need to create settings!\n");
		xml_create_settings_file(settings.fnconfigure);
		backend_init_main_mgr();
		init_lists();
		retval = gui_first_run();
		shutdown_list();
		backend_delete_main_mgr();
		xml_save_settings_doc(settings.fnconfigure);
		xml_free_settings_doc();
	}
	*/
	/* if gSwordDir does not exist create it */
	if (access(settings.fnconfigure, F_OK) == -1) {
		/* must be first run */
		g_print("\nFirst Run: need to create settings!\n");
		xml_create_settings_file(settings.fnconfigure);
		backend_init_main_mgr();
		init_lists();
		retval = gui_first_run();
		shutdown_list();
		backend_delete_main_mgr();
		xml_save_settings_doc(settings.fnconfigure);
		xml_free_settings_doc();
	}

	xml_parse_settings_file(settings.fnconfigure);
	load_settings_structure();

	if (have_old) {
		g_free(old_prefs);
		g_free(old_bm);
		g_free(old_sb);
		g_free(old_fav);
		g_free(old_text);
		g_free(old_comm);
		g_free(old_dict);
		g_free(old_gbs);
	}

	if (old_dir)
		g_free(old_dir);
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

int init_old(void)
{
	/* set old_dir to $home + .GnomeSword */
	old_dir = g_new(char, strlen(settings.homedir) +
			strlen(".GnomeSword") + 2);
	sprintf(old_dir, "%s/%s", settings.homedir, ".GnomeSword");

	if (access(old_dir, F_OK) == -1)
		return 0;


	old_bm = g_new(char, strlen(old_dir) +
		       strlen("/bookmarks") + 2);
	sprintf(old_bm, "%s/%s", old_dir, "bookmarks");

	old_sb = g_new(char, strlen(old_dir) +
		       strlen("/shortcutbar") + 2);
	sprintf(old_sb, "%s/%s", old_dir, "shortcutbar");

	old_prefs = g_new(char, strlen(old_dir) +
			  strlen("preferences.conf") + 2);
	sprintf(old_prefs, "%s/%s", old_dir, "preferences.conf");

	old_fav = g_new(char, strlen(old_sb) +
			strlen("Favorites.conf") + 2);
	sprintf(old_fav, "%s/%s", old_sb, "Favorites.conf");

	old_text = g_new(char, strlen(old_sb) +
			 strlen("BibleText.conf") + 3);
	sprintf(old_text, "%s/%s", old_sb, "BibleText.conf");

	old_comm = g_new(char, strlen(old_sb) +
			 strlen("Commentaries.conf") + 3);
	sprintf(old_comm, "%s/%s", old_sb, "Commentaries.conf");

	old_dict = g_new(char, strlen(old_sb) +
			 strlen("Dictionaries.conf") + 3);
	sprintf(old_dict, "%s/%s", old_sb, "Dictionaries.conf");

	old_gbs = g_new(char, strlen(old_sb) +
			strlen("Books.conf") + 3);
	sprintf(old_gbs, "%s/%s", old_sb, "Books.conf");
/*
	if (access(settings.shortcutbarDir, F_OK) == -1) {
		if ((mkdir(settings.shortcutbarDir, S_IRWXU)) == 0) {
			if (access(old_fav, F_OK) == 0)
				old_2_new_shortcut_file(old_fav,
						"Favorites.conf");
			if (access(old_text, F_OK) == 0)
				old_2_new_shortcut_file(old_text,
						"BibleText.conf");
			if (access(old_comm, F_OK) == 0)
				old_2_new_shortcut_file(old_comm,
						"Commentaries.conf");
			if (access(old_dict, F_OK) == 0)
				old_2_new_shortcut_file(old_dict,
						"Dictionaries.conf");
			if (access(old_gbs, F_OK) == 0)
				old_2_new_shortcut_file(old_gbs,
						"Books.conf");

		}
	} else {
		printf("can't create shortcutbar dir");
		return 0;
	}
*/
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

int init_bookmarks(int new_bookmarks, int have_old)
{
	GNode *bookmark_tree = NULL;
	char *file_buf = NULL;
	char *removed = NULL;
	int load_old = FALSE;

	settings.load_xml_bookmarks = FALSE;

	/* set bookmarks dir to settings.gSwordDir + /bookmarks */
	settings.swbmDir = g_new(char, strlen(settings.gSwordDir) +
				 strlen("/bookmarks") + 2);
	sprintf(settings.swbmDir, "%s/%s", settings.gSwordDir,
		"bookmarks");

	/* if .gnomesword-1.0/bookmarks does not exist create it */
	if (access(settings.swbmDir, F_OK) == -1) {
		if ((mkdir(settings.swbmDir, S_IRWXU)) == 0) {
			/* if we have old style bookmarks from a 
			   previous version of gnomesword load them */
			load_old = TRUE;

		} else {
			g_warning("can't create bookmarks dir");
			return 0;
		}
	}
					
	/* set removed dir to settings.swbmDir + /removed */
	removed = g_new(char, strlen(settings.swbmDir) +
				 strlen("/removed") + 2);
	sprintf(removed, "%s/%s", settings.swbmDir,
		"removed");
	if (access(removed, F_OK) == -1) {
		if ((mkdir(removed, S_IRWXU)) == 0) {
			/* this directory is used to save removed
			   bookmark folders to be restored later */
		} else {
			g_warning("can't create removed dir");
			//return 0;
		}
	}	
	
	if (new_bookmarks) {
		xml_new_bookmark_file();
	} else if (load_old) {
		if (have_old) {
			backend_init_main_mgr();
			bookmark_tree = backend_load_bookmarks(old_bm);
			gui_save_old_bookmarks_to_new(bookmark_tree);
			backend_delete_main_mgr();
		}
	}
	/* check for xml bookmarks */
	file_buf = g_new(char, strlen(settings.swbmDir) +
			 strlen("/bookmarks.xml") + 2);
	sprintf(file_buf, "%s/bookmarks.xml", settings.swbmDir);

	if (access(file_buf, F_OK) == 0) {
		settings.load_xml_bookmarks = TRUE;
	}
	g_free(file_buf);

	if (!settings.load_xml_bookmarks) {
		file_buf = g_new(char, strlen(settings.swbmDir) +
				 strlen("/personal.conf") + 2);
		sprintf(file_buf, "%s/personal.conf", settings.swbmDir);
		backend_init_main_mgr();
		if (access(file_buf, F_OK) == 0) {
			bookmark_tree =
			    backend_load_bookmarks(settings.swbmDir);
			gui_save_old_bookmarks_to_new(bookmark_tree);
		} else
			xml_new_bookmark_file();

		backend_delete_main_mgr();
		settings.load_xml_bookmarks = TRUE;
		g_free(file_buf);
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

void old_2_new_shortcut_file(gchar * old_file, gchar * new_file)
{
	gchar group_name[256], icon_size[10];
	GList *glist = NULL;

	glist = load_sb_group(old_file, group_name, icon_size);
	save_sb_group(glist, new_file, group_name, icon_size);
	g_list_free(glist);
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
	settings.BookWindowModule = xml_get_value("modules", "book");
	settings.parallel1Module = xml_get_value("modules", "int1");
	settings.parallel2Module = xml_get_value("modules", "int2");
	settings.parallel3Module = xml_get_value("modules", "int3");
	settings.parallel4Module = xml_get_value("modules", "int4");
	settings.parallel5Module = xml_get_value("modules", "int5");
	settings.personalcommentsmod =
	    xml_get_value("modules", "percomm");
	settings.devotionalmod = xml_get_value("modules", "devotional");


	settings.lex_greek = xml_get_value("lexicons", "greek");
	settings.lex_hebrew = xml_get_value("lexicons", "hebrew");
	settings.lex_greek_viewer =
	    xml_get_value("lexicons", "greekviewer");
	settings.lex_hebrew_viewer =
	    xml_get_value("lexicons", "hebrewviewer");
	settings.DefaultDict =
	    xml_get_value("lexicons", "defaultdictionary");
	settings.inViewer = atoi(xml_get_value("lexicons", "inviewer"));
	settings.inDictpane =
	    atoi(xml_get_value("lexicons", "indictpane"));
	settings.useDefaultDict =
	    atoi(xml_get_value("lexicons", "usedefaultdict"));


	settings.currentverse = xml_get_value("keys", "verse");
	settings.dictkey = xml_get_value("keys", "dictionary");
	settings.book_key = xml_get_value("keys", "book");


	settings.shortcutbar_width =
	    atoi(xml_get_value("layout", "shortcutbar"));
	settings.gs_hight = atoi(xml_get_value("layout", "hight"));
	settings.gs_width = atoi(xml_get_value("layout", "width"));
	
	if(xml_get_value("layout", "app_x"))
		settings.app_x = atoi(xml_get_value("layout", "app_x"));
	else {
		xml_add_new_item_to_section("layout", "app_x", "40");
		settings.app_x = 40;
	}
	if(xml_get_value("layout", "app_y"))
		settings.app_y = atoi(xml_get_value("layout", "app_y"));
	else{
		xml_add_new_item_to_section("layout", "app_y", "40");
		settings.app_y = 40;
	}
	
	settings.biblepane_width =
	    atoi(xml_get_value("layout", "textpane"));
	settings.upperpane_hight =
	    atoi(xml_get_value("layout", "uperpane"));


	settings.verse_num_font_size =
	    xml_get_value("fontsize", "versenum");


	settings.bible_text_color = xml_get_value("HTMLcolors", "text_fg");
	settings.bible_bg_color =
	    xml_get_value("HTMLcolors", "background");
	settings.currentverse_color =
	    xml_get_value("HTMLcolors", "currentverse");
	settings.link_color = xml_get_value("HTMLcolors", "link");
	settings.bible_verse_num_color =
	    xml_get_value("HTMLcolors", "versenum");
	settings.found_color = xml_get_value("HTMLcolors", "found");


	buf = xml_get_value("parallel", "strongs");
	if(buf)
		settings.strongsint =atoi(buf);
	
	buf = xml_get_value("parallel", "morphs");
	if(buf)
		settings.morphsint = atoi(buf);
	
	buf = xml_get_value("parallel", "points");
	if(buf)
		settings.hebrewpointsint = atoi(buf);
	
	buf = xml_get_value("parallel", "cantillation");
	if(buf)
		settings.cantillationmarksint = atoi(buf);
	
	buf = xml_get_value("parallel", "footnotes");
	if(buf)
		settings.footnotesint = atoi(buf);	
	
	buf = xml_get_value("parallel", "parallel");
	if(buf)
	settings.parallelpage = atoi(buf);


	settings.showtexts = atoi(xml_get_value("misc", "showtexts"));
	settings.showcomms = atoi(xml_get_value("misc", "showcomms"));
	settings.showdicts = atoi(xml_get_value("misc", "showdicts"));
	settings.showsplash = atoi(xml_get_value("misc", "splash"));
	settings.showdevotional =
	    atoi(xml_get_value("misc", "dailydevotional"));
	settings.versestyle = atoi(xml_get_value("misc", "versestyle"));
	settings.usedefault = atoi(xml_get_value("misc", "usedefault"));
	
		
	if(xml_get_value("misc", "setup_canceled"))
		settings.setup_canceled = atoi(xml_get_value("misc", "setup_canceled"));
	else {
		xml_add_new_item_to_section("misc","setup_canceled","0");
		settings.setup_canceled = atoi(xml_get_value("misc", "setup_canceled"));
	}

	settings.use_studypad =
	    atoi(xml_get_value("editor", "UseStudyPad"));
	settings.use_studypad_dialog =
	    atoi(xml_get_value("editor", "UseStudypadDialog"));
	settings.use_percomm_dialog =
	    atoi(xml_get_value("editor", "UsePercommDialog"));
	if(xml_get_value("editor", "spell_language"))
	     settings.spell_language = xml_get_value("editor","spell_language");
	else {
		xml_add_new_item_to_section("editor","spell_language","unknown");
		settings.spell_language = "unknown";
	}


	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	settings.studypaddir = xml_get_value("studypad", "directory");
	
	buf = xml_get_value("studypad", "stylebar");
	if(buf)
		settings.show_style_bar_sp =  atoi(buf);
	
	buf = xml_get_value("studypad", "editbar");
	if(buf)
		settings.show_edit_bar_sp =  atoi(buf);


	buf = xml_get_value("shortcutbar", "shortcutbar");
	if(buf)
		settings.showshortcutbar = atoi(buf);
	
	buf = xml_get_value("shortcutbar", "favo");
	if(buf)
		settings.showfavoritesgroup = atoi(buf);
	
	buf = xml_get_value("shortcutbar", "bible");
	if(buf)
		settings.showtextgroup = atoi(buf);
	
	buf = xml_get_value("shortcutbar", "comm");
	if(buf)
		settings.showcomgroup = atoi(buf);
	
	buf = xml_get_value("shortcutbar", "dict");
	if(buf)
		settings.showdictgroup = atoi(buf);
	
	buf = xml_get_value("shortcutbar", "book");
	if(buf)
		settings.showbookgroup = atoi(buf);
	
	buf = xml_get_value("shortcutbar", "bookmark");
	if(buf)
		settings.showbookmarksgroup = atoi(buf);
	
	buf = xml_get_value("shortcutbar", "history");
	if(buf)
		settings.showhistorygroup = atoi(buf);
	
	buf = xml_get_value("shortcutbar", "docked");
	if(buf)
		settings.docked =  atoi(buf);
	
	
	buf = xml_get_value("tabs", "bible");
	if(buf)
		settings.text_tabs =  atoi(buf);
	
	buf = xml_get_value("tabs", "comm");
	if(buf)
		settings.comm_tabs =  atoi(buf);
	
	buf = xml_get_value("tabs", "dict");
	if(buf)
		settings.dict_tabs =  atoi(buf);
	
	buf = xml_get_value("tabs", "book");
	if(buf)
		settings.book_tabs =  atoi(buf);
	
	buf = xml_get_value("tabs", "percomm");
	if(buf)
		settings.percomm_tabs =  atoi(buf);
	

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
