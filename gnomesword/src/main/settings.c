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

#include "main/lists.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

//#include "backend/sword.h"


/******************************************************************************
 * defines
 */
#define GS_DIR ".gnomesword"

#define GS_NET_PERMISSION	"Welcome to GnomeSword.\n\nThere are no Bible modules installed. In order to initialize, GnomeSword needs you to install at least one Bible.\n\nWith your permission, GnomeSword will invoke the Module Manager so that you may install from Crosswire:\n1. Configure remote install.\n2. Connect.\n3. Select a Bible text of your language preference.\n4. Click `install'.\nClose the Module Manager when you are done.\n\nWarning: If you live in a persecuted country, use with care.\n\nMay GnomeSword invoke the Module Manager so that you may install a Bible?"

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
	char *tabs = NULL;

	settings.first_run = FALSE;
	/* set program title */
	strcpy(settings.program_title, "GnomeSword");

	/* Get home dir */
	if ((settings.homedir = getenv("HOME")) == NULL) {
		gui_generic_warning(_("$HOME is not set!"));
		/* if not found in env exit */
		exit(0);
	}

	/* set gSwordDir to $home + .gnomesword */
	settings.gSwordDir = g_new(char, strlen(settings.homedir) +
				   strlen(GS_DIR) + 2);
	sprintf(settings.gSwordDir, "%s/%s", settings.homedir, GS_DIR);

	/* if gSwordDir does not exist, create it. */
	if (access(settings.gSwordDir, F_OK) == -1) {
		gs_old = g_strdup_printf("%s/.gnomesword-2.0", settings.homedir);
		if (access(gs_old, F_OK) == 0) {
			if (rename(gs_old, settings.gSwordDir) == 0)
				gui_generic_warning("GnomeSword has renamed .gnomesword-2.0 to .gnomesword");
			else {
				char msg[300];
				sprintf(msg, "GnomeSword can not rename  .gnomesword-2.0 to .gnomesword:\n%s\n\nGnomeSword cannot continue.",
					strerror(errno));
				gui_generic_warning(msg);
				/* necessarily giving up. */
				gtk_exit(1);
			}
		} else if ((mkdir(settings.gSwordDir, S_IRWXU)) != 0) {
			char msg[300];
			sprintf(msg, "GnomeSword can not create  .gnomesword:\n%s\n\nGnomeSword cannot continue.",
				strerror(errno));
			gui_generic_warning(msg);
			/* if we can not create gSwordDir exit */
			gtk_exit(1);
		}
		g_free(gs_old);
	}

	/* check for tabs dir -- if directory does not exist create it */
/*	tabs = g_strdup_printf("%s/tabs",settings.gSwordDir);
	if (access(tabs, F_OK) == -1)
		gui_save_tabs(NULL);
	g_free(tabs);
*/

	/* if .sword does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir, ".sword");
	if (access(sword_dir, F_OK) == -1) {
		if ((mkdir(sword_dir, S_IRWXU)) != 0) {
			gui_generic_warning(_("can not create  .sword"));
		} 
	}
	g_free(sword_dir);
	/* if .sword/mods.d does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir, ".sword/mods.d");
	if (access(sword_dir, F_OK) == -1) {
		if ((mkdir(sword_dir, S_IRWXU)) != 0) {
			gui_generic_warning(_("can not create  .sword/mods.d"));
		} 
	}	
	g_free(sword_dir);
	/* if .sword/modules does not exist create it */
	sword_dir = g_strdup_printf("%s/%s", settings.homedir,".sword/modules");
	if (access(sword_dir, F_OK) == -1) {
		if ((mkdir(sword_dir, S_IRWXU)) != 0) {
			gui_generic_warning(_("can not create  .sword/modules"));
		} 
	}
	g_free(sword_dir);
	
	
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
			if (gui_yes_no_dialog(GS_NET_PERMISSION)) {
				main_shutdown_list();
				gui_open_mod_mgr_initial_run();
				main_init_lists();
				if (settings.havebible == 0) {
					gui_generic_warning
					    ("There are still no Bibles installed.\nEvidently, you declined to install any.\n\nWithout any Bible modules to display, GnomeSword cannot proceed, and will now exit.");
					exit(1);
				} else
					gui_generic_warning
					    ("Bible module installation complete.");
			} else {
				gui_generic_warning
				    ("Without any Bible modules to display, GnomeSword cannot proceed, and will now exit.");
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
	settings.lex_greek_viewer = xml_get_value("lexicons", "greekviewer");
	settings.lex_hebrew_viewer = xml_get_value("lexicons", "hebrewviewer");
	settings.DefaultDict = xml_get_value("lexicons", "defaultdictionary");
//	settings.inViewer = atoi(xml_get_value("lexicons", "inviewer"));
//	settings.inDictpane = atoi(xml_get_value("lexicons", "indictpane"));
	settings.useDefaultDict = atoi(xml_get_value("lexicons","usedefaultdict"));


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
		xml_add_new_item_to_section("keys", "offset", "");
		settings.book_offset = 0;
	}

	if(xml_get_value("tab", "page"))
		settings.tab_page = atol(xml_get_value("tab", "page"));
	else {
		xml_add_new_item_to_section("tab", "page", "0");
		settings.tab_page = 0;
	}

	settings.sidebar_width =
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

	if(xml_get_value("layout", "biblehight"))
		settings.biblepane_hight = atoi(xml_get_value("layout", "biblehight"));
	else {
		xml_add_new_item_to_section("layout", "biblehight", "340");
		settings.biblepane_hight = 250;
	}

	if(xml_get_value("layout", "commentaryhight"))
		settings.commpane_hight = atoi(xml_get_value("layout", "commentaryhight"));
	else {
		xml_add_new_item_to_section("layout", "commentaryhight", "240");
		settings.commpane_hight = 240;
	}
	
	settings.verse_num_font_size_str = xml_get_value("fontsize", "versenum");
	settings.verse_num_font_size = atoi(settings.verse_num_font_size_str);

	if (buf = xml_get_value("fontsize", "basefontsize")) {
		settings.base_font_size_str = g_strdup(buf);
		settings.base_font_size = atoi(settings.base_font_size_str);
	} else {
		xml_add_new_item_to_section("fontsize", "basefontsize", "+0");
		settings.base_font_size_str = g_strdup("+0");
		settings.base_font_size = 0;
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

	if (buf = xml_get_value("HTMLcolors", "highlight_fg"))
		settings.highlight_fg = g_strdup(buf);
	else {
		xml_add_new_item_to_section("HTMLcolors", "highlight_fg", "#FFFF00");
		settings.highlight_fg = g_strdup("#FFFF00");
	}
	if (buf = xml_get_value("HTMLcolors", "highlight_bg"))
		settings.highlight_bg = g_strdup(buf);
	else {
		xml_add_new_item_to_section("HTMLcolors", "highlight_bg", "#060680");
		settings.highlight_bg = g_strdup("#060680");
	}

	buf = xml_get_value("parallel", "strongs");
	if(buf)
		settings.strongsint = atoi(buf);
	
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
	if(xml_get_value("misc", "showpreview"))
		settings.showpreview = atol(xml_get_value("misc", "showpreview"));
	else {
		xml_add_new_item_to_section("misc", "showpreview", "1");
		settings.showpreview = 1;
	}

//#ifdef USE_GTKHTML30	
	settings.showsplash = atoi(xml_get_value("misc", "splash"));
//#else	
	//settings.showsplash = FALSE;	
//#endif
	
	settings.showdevotional =
	    atoi(xml_get_value("misc", "dailydevotional"));
	settings.versestyle = atoi(xml_get_value("misc", "versestyle"));

	if (buf = xml_get_value("misc", "pinnedtabs"))
		settings.pinnedtabs = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "pinnedtabs", "0");
		settings.pinnedtabs = 0;
	}

	if (buf = xml_get_value("misc", "readaloud"))
		settings.readaloud = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "readaloud", "0");
		settings.readaloud = 0;
	}

	if (buf = xml_get_value("misc", "showversenum"))
		settings.showversenum = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "showversenum", "1");
		settings.showversenum = 1;
	}

	if (buf = xml_get_value("misc", "versehighlight"))
		settings.versehighlight = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "versehighlight", "0");
		settings.versehighlight = 0;
	}

	buf = xml_get_value("misc", "doublespace");
	if (buf)
		settings.doublespace = atoi(buf);
	else {
		xml_add_new_item_to_section("misc", "doublespace", "0");
		settings.doublespace = 0;
	}

	settings.usedefault = atoi(xml_get_value("misc", "usedefault"));
	
	if (buf = xml_get_value("misc", "chapter-scroll")) {
		settings.chapter_scroll = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc","chapter-scroll","0");
		settings.chapter_scroll = 0;
	}

	if (buf = xml_get_value("misc", "imageresize")) {
		settings.imageresize = atoi(buf);
	} else {
		xml_add_new_item_to_section("misc","imageresize","0");
		settings.imageresize = 0;
	}

/*	settings.use_studypad =
	    atoi(xml_get_value("editor", "UseStudyPad"));
	settings.use_studypad_dialog =
	    atoi(xml_get_value("editor", "UseStudypadDialog"));
	settings.use_percomm_dialog =
	    atoi(xml_get_value("editor", "UsePercommDialog"));*/

	if (xml_get_value("editor", "spell_language"))
		settings.spell_language = xml_get_value("editor","spell_language");
	else {
		xml_add_new_item_to_section("editor","spell_language","unknown");
		settings.spell_language = "unknown";
	}


	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	settings.studypaddir = xml_get_value("studypad", "directory");
/*
	if (buf = xml_get_value("shortcutbar", "shortcutbar"))
		settings.showshortcutbar = atoi(buf);
	
	if (buf = xml_get_value("shortcutbar", "docked"))
		settings.docked =  atoi(buf);
	
*/		
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
 *
 * Return value
 *   void
 */

#define	GS_GCONF_PERMISSION	"There is currently no program set as your\nhandler for \"sword://\" and similar URLs.\nWould you like GnomeSword to install itself\nas the program to handle these URLs?"

#define	GS_GCONF_SUCCESS	"GnomeSword has successfully set itself\nas the handler of sword:// and bible:// URLs.\n\nYou may wish to run the program \"gconf-editor\"\nto examine keys under /desktop/gnome/url-handlers,\nif you need to change these."

char *gconf_keys[GS_GCONF_MAX][2] = {
    { "/desktop/gnome/url-handlers/bible/command",        "gnomesword2 \"%s\"" },
    { "/desktop/gnome/url-handlers/bible/enabled",        (char *) 1 },
    { "/desktop/gnome/url-handlers/bible/needs_terminal", (char *) 0 },
    { "/desktop/gnome/url-handlers/sword/command",        "gnomesword2 \"%s\"" },
    { "/desktop/gnome/url-handlers/sword/enabled",        (char *) 1 },
    { "/desktop/gnome/url-handlers/sword/needs_terminal", (char *) 0 }
};

void gconf_setup()
{
	int i;
	char msg[256];
	gchar *str;
	gboolean retval;
	GConfClient* client = gconf_client_get_default();

	if (client == NULL)
		return;		/* we're not running under GConf */

	/*
	 * This is deliberately somewhat simple-minded, at least for now.
	 * We care about one thing: Is anything set to handle "bible://"?
	 */
	if (((str = gconf_client_get_string(client, gconf_keys[0][0],
					    NULL)) == NULL) &&
	    gui_yes_no_dialog(GS_GCONF_PERMISSION)) {
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
			if (!retval) {
				sprintf(msg, "GnomeSword failed to complete handler init for key #%d:\n%s",
					i, gconf_keys[i][0]);
				gui_generic_warning(msg);
				return;
			}
		}
		gui_generic_warning(GS_GCONF_SUCCESS);
	}
}
