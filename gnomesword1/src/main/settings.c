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

#include <glib-2.0/glib.h>

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
#include "main/xml.h"

#include "backend/sword.h"


/******************************************************************************
 * defines
 */
#define GS_DIR ".gnomesword-2.0"


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
	char *sword_dir = NULL;
	char *gs_old = NULL;

	/* set program title */
	strcpy(settings.program_title, "GnomeSword");

	/* Get home dir */
	if ((settings.homedir = getenv("HOME")) == NULL) {
		g_error("$HOME is not set!");
		/* if not found in env exit */
		exit(0);
	}

	/* set gSwordDir to $home + .gnomesword-2.0 */
	settings.gSwordDir = g_new(char, strlen(settings.homedir) +
				   strlen(GS_DIR) + 2);
	sprintf(settings.gSwordDir, "%s/%s", settings.homedir, GS_DIR);

	/* if gSwordDir does not exist create it */
	if (access(settings.gSwordDir, F_OK) == -1) {
		gs_old = g_strdup_printf("%s/.gnomesword-1.0",settings.homedir);		
		if (access(gs_old, F_OK) == 0) {
			if(rename(gs_old,settings.gSwordDir) != 0) {
				printf("can not rename  .gnomesword-1.0");
			}
		} else if ((mkdir(settings.gSwordDir, S_IRWXU)) != 0) {
			printf("can not create  .gnomesword-2.0");
			/* if we can not create gSwordDir exit */
			gtk_exit(1);
		} 
		g_free(gs_old);
	}
	
	
	/* if .sword does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir, ".sword");
	if (access(sword_dir, F_OK) == -1) {
		if ((mkdir(sword_dir, S_IRWXU)) != 0) {
			printf("can not create  .sword");
		} 
	}
	g_free(sword_dir);
	/* if .sword/mods.d does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir, ".sword/mods.d");
	if (access(sword_dir, F_OK) == -1) {
		if ((mkdir(sword_dir, S_IRWXU)) != 0) {
			printf("can not create  .sword/mods.d");
		} 
	}	
	g_free(sword_dir);
	/* if .sword/modules does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir,".sword/modules");
	if (access(sword_dir, F_OK) == -1) {
		if ((mkdir(sword_dir, S_IRWXU)) != 0) {
			printf("can not create  .sword/modules");
		} 
	}
	g_free(sword_dir);
	
	
	init_bookmarks(new_bookmarks);

	/* set fnconfigure to gSwordDir and settings.xml */
	settings.fnconfigure = g_new(char, strlen(settings.gSwordDir) +
				     strlen("settings.xml") + 2);
	sprintf(settings.fnconfigure, "%s/%s", settings.gSwordDir,
		"settings.xml");
	
	/* if gSwordDir does not exist create it */
	if (access(settings.fnconfigure, F_OK) == -1) {
		/* must be first run */
		g_print("\nFirst Run: need to create settings!\n");
		xml_create_settings_file(settings.fnconfigure);
		main_init_lists();
		retval = gui_first_run();
		main_shutdown_list();
		xml_save_settings_doc(settings.fnconfigure);
		xml_free_settings_doc();
	}

	xml_parse_settings_file(settings.fnconfigure);
	load_settings_structure();
	
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

	/* if .gnomesword-2.0/bookmarks does not exist create it */
	if (access(settings.swbmDir, F_OK) == -1) {
		if ((mkdir(settings.swbmDir, S_IRWXU)) == -1) {
			g_warning("can't create bookmarks dir");
			return 0;
		}
	}
					
	/* set removed dir to settings.swbmDir + /removed */
	removed = g_new(char, strlen(settings.swbmDir) +
				 strlen("/removed") + 2);
	sprintf(removed, "%s/%s", settings.swbmDir, "removed");
	if (access(removed, F_OK) == -1) {
		if ((mkdir(removed, S_IRWXU)) == -1) {
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

	if(xml_get_value("keys", "book")) {
		settings.book_key = xml_get_value("keys", "book");
	} else {
		xml_add_new_item_to_section("keys", "book", "0");
		settings.book_key = xml_get_value("keys", "book");
	}
	if(xml_get_value("keys", "offset"))
		settings.book_offset = atol(xml_get_value("keys", "offset"));
	else {
		xml_add_new_item_to_section("keys", "offset", "0");
		settings.book_offset = 0;
	}

	settings.shortcutbar_width =
		atoi(xml_get_value("layout", "shortcutbar"));
	if(xml_get_value("layout", "vltoppaneheight"))
		settings.verselist_toppane_height =
			atoi(xml_get_value("layout", "vltoppaneheight"));
	else {
		xml_add_new_item_to_section("layout", "vltoppaneheight", "210");
		settings.verselist_toppane_height = 210;
	}
	if(xml_get_value("layout", "sidebar_notebook_hight"))
		settings.sidebar_notebook_hight =
			atoi(xml_get_value("layout", "sidebar_notebook_hight"));
	else {
		xml_add_new_item_to_section("layout", "sidebar_notebook_hight", "300");
		settings.sidebar_notebook_hight = 250;
	}
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

#ifdef USE_GTKHTML30	
	settings.showsplash = atoi(xml_get_value("misc", "splash"));
#else	
	settings.showsplash = FALSE;	
#endif
	
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
	
	
	if(xml_get_value("tabs", "browsing")) {
		buf = xml_get_value("tabs", "browsing");
		settings.browsing =  atoi(buf);
	} else {
		xml_add_new_item_to_section("tabs","browsing","1");
		settings.browsing = 1;
	}
	
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
