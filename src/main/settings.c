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

#include "main/settings.h"
#include "main/search.h"

#include "backend/properties.h"
#include "backend/bookmarks.h"
#include "backend/shortcutbar.h"


/******************************************************************************
 * defines
 */
#define GS_DIR ".gnomesword-1.0"


/******************************************************************************
 * globals
 */
SETTINGS settings;


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

gint settings_init(void)
{
	gint retval = 0;
	gchar bmFile[300];
	gchar genbookdir[300];
	gchar *old_prefs  = NULL;
	gchar *old_dir    = NULL;
	gchar *old_bm     = NULL;
	gchar *old_sb     = NULL;
	gchar *old_fav    = NULL;
	gchar *old_text   = NULL;
	gchar *old_comm   = NULL;
	gchar *old_dict   = NULL;
	gchar *old_gbs    = NULL;
	gchar *file_buf   = NULL;
	gboolean have_old = FALSE;

	/* set program title */
	strcpy(settings.program_title, "GnomeSword");

	/* Get home dir */
	if ((settings.homedir = getenv("HOME")) == NULL) {
		g_error("$HOME is not set!");
		/* if not found in env exit */
		exit(0);
	}


	/* set gSwordDir to $home + .GnomeSword */
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
	}


	/* set old_dir to $home + .GnomeSword */
	old_dir = g_new(char, strlen(settings.homedir) +
			strlen(".GnomeSword") + 2);
	sprintf(old_dir, "%s/%s", settings.homedir, ".GnomeSword");

	if (access(old_dir, F_OK) == 0)
		have_old = TRUE;

	/* set bookmarks dir to settings.gSwordDir + /bookmarks */
	settings.swbmDir = g_new(char, strlen(settings.gSwordDir) +
				 strlen("/bookmarks") + 2);
	sprintf(settings.swbmDir, "%s/%s", settings.gSwordDir,
		"bookmarks");

	/* set bookmarks dir to settings.gSwordDir + /bookmarks */
	/*
	settings.xml_bookmark_dir =
	    g_new(char,
		  strlen(settings.gSwordDir) +
		  strlen("/xml_bookmarks") + 2);
	sprintf(settings.xml_bookmark_dir, "%s/%s", settings.gSwordDir,
		"xml_bookmarks");
	*/
	
	if (have_old) {

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
	}

	/* if .gnomesword-1.0/bookmarks does not exist create it */
	if (access(settings.swbmDir, F_OK) == -1) {
		if ((mkdir(settings.swbmDir, S_IRWXU)) == 0) {
			/* if we have old style bookmarks from a 
			   previous version of gnomesword load them */			
			GNode *bookmark_tree;
			bookmark_tree =
			    backend_load_old_bookmarks(old_bm);
			gui_save_gnode_to_xml_bookmarks(bookmark_tree);
		} else
			g_warning("can't create bookmarks dir");
	}
	/* check for xml bookmarks */
	settings.load_xml_bookmarks = FALSE;
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
		if (access(file_buf, F_OK) == 0) {
			GNode *bookmark_tree;
			bookmark_tree =
			    backend_load_bookmarks(settings.swbmDir);
			gui_save_gnode_to_xml_bookmarks(bookmark_tree);
			settings.load_xml_bookmarks = TRUE;
		}
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

	/* shortcutbar dir */
	settings.shortcutbarDir =
	    g_new(char,
		  strlen(settings.gSwordDir) + strlen("shortcutbar") +
		  2);
	sprintf(settings.shortcutbarDir, "%s/%s", settings.gSwordDir,
		"shortcutbar");

	/* if shortcutbar dir does not exist create it */
	if (access(settings.shortcutbarDir, F_OK) == -1) {
		if ((mkdir(settings.shortcutbarDir, S_IRWXU)) == 0) {
			if (have_old) {
				gchar group_name[256], icon_size[10];
				if (access(old_fav, F_OK) == 0) {

					GList *glist =
					    backend_load_sb_group
					    (old_fav, group_name,
					     icon_size);
					backend_save_sb_group(glist,
						      "Favorites.conf",
						      group_name,
						      icon_size);
				}
				if (access(old_text, F_OK) == 0) {
					GList *glist =
					    backend_load_sb_group
					    (old_text, group_name,
					     icon_size);
					backend_save_sb_group(glist,
						      "BibleText.conf",
						      group_name,
						      icon_size);
				}
				if (access(old_comm, F_OK) == 0) {
					GList *glist =
					    backend_load_sb_group
					    (old_comm, group_name,
					     icon_size);
					backend_save_sb_group(glist,
						      "Commentaries.conf",
						      group_name,
						      icon_size);
				}
				if (access(old_dict, F_OK) == 0) {
					GList *glist =
					    backend_load_sb_group
					    (old_dict, group_name,
					     icon_size);
					backend_save_sb_group(glist,
						      "Dictionaries.conf",
						      group_name,
						      icon_size);
				}
				if (access(old_gbs, F_OK) == 0) {
					GList *glist =
					    backend_load_sb_group
					    (old_gbs, group_name,
					     icon_size);
					backend_save_sb_group(glist,
						      "Books.conf",
						      group_name,
						      icon_size);
				}
			}
		} else
			printf("can't create shortcutbar dir");
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


	if (access(settings.fnconfigure, F_OK) == -1) {
		retval = 1;
	}


	/*  find or create dir for local gbs support */
	sprintf(genbookdir, "%s/%s", settings.homedir, ".sword/");

	if (access(genbookdir, F_OK) == -1) {
		if ((mkdir(genbookdir, S_IRWXU)) != 0) {
			g_warning("can't create .sword dir");
		}
	}

	/* for local module support */
	sprintf(genbookdir, "%s/%s", settings.homedir, ".sword/mods.d");

	if (access(genbookdir, F_OK) == -1) {
		if ((mkdir(genbookdir, S_IRWXU)) != 0) {
			g_warning("can't create .sword/mods.d dir");
		}
	}

	/* for local gbs support */
	sprintf(genbookdir, "%s/%s", settings.homedir,
		".sword/modules");

	if (access(genbookdir, F_OK) == -1) {
		if ((mkdir(genbookdir, S_IRWXU)) != 0) {
			g_warning("can't create .sword/modules dir");
		}
	}

	/* for local gbs support */
	sprintf(genbookdir, "%s/%s", settings.homedir,
		".sword/modules/genbook");


	if (access(genbookdir, F_OK) == -1) {
		if ((mkdir(genbookdir, S_IRWXU)) != 0) {
			g_warning
			    ("can't create .sword/modules/genbook dir");
		}
	}

	/* for local gbs support */
	sprintf(genbookdir, "%s/%s", settings.homedir,
		".sword/modules/genbook/rawgenbook");


	if (access(genbookdir, F_OK) == -1) {
		if ((mkdir(genbookdir, S_IRWXU)) != 0) {
			g_warning
			    ("can't create .sword/modules/genbook/rawgenbook"
			     " dir");
		}
	}

	sprintf(bmFile, "%s/%s", settings.swbmDir, "bookmarks.xml");
	if (access(bmFile, F_OK) == -1) {
		if (retval == 1) {
			retval = 3;
		} else {
			retval = 2;
		}
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

	if(file_buf)	
		g_free(file_buf);
	g_free(old_dir);
	return retval;
}
