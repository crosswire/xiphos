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

#include <glib-1.2/glib.h>
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

#include "backend/properties.h"
#include "backend/bookmarks.h"
#include "backend/shortcutbar.h"
#include "backend/sword.h"
#include "backend/mgr.hh"


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
static void old_2_new_shortcut_file(gchar * old_file, gchar * new_file);
static int init_old(void);
static int init_bookmarks(int new_bookmarks, int have_old);

static 	gchar *old_prefs  = NULL;
static 	gchar *old_dir    = NULL;
static 	gchar *old_bm     = NULL;
static 	gchar *old_sb     = NULL;
static 	gchar *old_fav    = NULL;
static 	gchar *old_text   = NULL;
static 	gchar *old_comm   = NULL;
static 	gchar *old_dict   = NULL;
static 	gchar *old_gbs    = NULL;


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

gint settings_init(int new_configs, int new_bookmarks)
{
	gint retval = 0;
	gboolean have_old = FALSE;
	gboolean need_old = FALSE;

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
		} 
		else 
			need_old = TRUE;
	}

	/* shortcutbar dir */
	settings.shortcutbarDir =
	    g_new(char,
		  strlen(settings.gSwordDir) + strlen("shortcutbar") +
		  2);
	sprintf(settings.shortcutbarDir, "%s/%s", settings.gSwordDir,
							"shortcutbar");
	

	if(need_old) {
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


	/* set fnconfigure to gSwordDir and preferences.conf */
	settings.fnconfigure = g_new(char, strlen(settings.gSwordDir) +
				     strlen("preferences-1.0.conf") +
				     2);
	sprintf(settings.fnconfigure, "%s/%s", settings.gSwordDir,
						"preferences-1.0.conf");

	/* if gSwordDir does not exist create it */
	if (access(settings.fnconfigure, F_OK) == -1) {
		/* must be first run */
		g_print("\nFirst Run: need to create settings!\n");
		if (access(old_prefs, F_OK) == 0) {
			g_print
			    ("\nFirst Run: have old settings will load them\n");
			backend_load_properties(old_prefs);
			backend_save_properties(TRUE);
		}
	}


	if ((access(settings.fnconfigure, F_OK) == -1) || new_configs) {
		backend_init_main_mgr();
		init_lists();
		retval = gui_first_run();
		shutdown_list();
		backend_delete_main_mgr();
	}
	
	
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

	
	if(old_dir)
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
	sprintf(old_prefs, "%s/%s", old_dir,
		"preferences.conf");
	
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
	} 
	else {
		printf("can't create shortcutbar dir");
		return 0;
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

int init_bookmarks(int new_bookmarks, int have_old)
{
	GNode *bookmark_tree = NULL;
	gchar *file_buf = NULL;
	gint load_old = FALSE;
	
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
	
	if(new_bookmarks) {
		gui_new_xml_bookmark_file();
	}
	else if(load_old) {
		if(have_old) {			
			backend_init_main_mgr();	
			bookmark_tree =
			    backend_load_bookmarks(old_bm);
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
	
	if(!settings.load_xml_bookmarks) {
		file_buf = g_new(char, strlen(settings.swbmDir) +
			 strlen("/personal.conf") + 2);
		sprintf(file_buf, "%s/personal.conf", settings.swbmDir);
		backend_init_main_mgr();
		if (access(file_buf, F_OK) == 0) {
			bookmark_tree =
			    backend_load_bookmarks(settings.swbmDir);
			gui_save_old_bookmarks_to_new(bookmark_tree);
		}
		else
			gui_new_xml_bookmark_file();
				
		backend_delete_main_mgr();	
		settings.load_xml_bookmarks = TRUE;
		g_free(file_buf);
	}
	
	/* check for Bibletime bookmarks */
	settings.have_bibletime = FALSE;
	file_buf = g_new(char, strlen(settings.homedir) + 
		 strlen("/.kde/share/apps/bibletime/bookmarks.xml") + 2);
	sprintf(file_buf, "%s/.kde/share/apps/bibletime/bookmarks.xml", 
					settings.homedir);
	if (access(file_buf, F_OK) == 0) {
		settings.have_bibletime = TRUE;
	}
	if(file_buf)	
		g_free(file_buf);
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

	glist = backend_load_sb_group(old_file, group_name, icon_size);
	backend_save_sb_group(glist, new_file, group_name, icon_size);
	g_list_free(glist);
}
