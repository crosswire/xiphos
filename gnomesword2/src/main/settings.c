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
#include <settings.h>

/* input buffer size */
#define BUFFER_SIZE 8192

/******************************************************************************
 * globals
 *****************************************************************************/

/* file pointer to configuration file - options */
FILE * configfile;

SETTINGS settings;

/*****************************************************************************
 * check for GnomeSword dir
 * if not found - create it and
 * the files needed to run program
 *****************************************************************************/

gint settings_init(void)
{
	gint retval = 0;
	/* Bookmarks directory for gnomesword-0.7.0 and up */
	gchar *gsbmDir;
	gchar bmFile[300];
	gchar genbookdir[300];

	/* Get home dir */
	if ((settings.homedir = getenv("HOME")) == NULL) {
		g_error("$HOME is not set!");
		/* if not found in env exit */
		exit(0);
	}
	
	/* set gSwordDir to $home + .GnomeSword */
	settings.gSwordDir = g_new(char, strlen(settings.homedir) +
			strlen(".GnomeSword") + 2);
	sprintf(settings.gSwordDir, "%s/%s", settings.homedir, ".GnomeSword");

	/* set bookmarks dir to homedir + .GnomeSword/bookmarks */
	gsbmDir = g_new(char, strlen(settings.homedir) +
			strlen(".GnomeSword/bookmarks") + 2);
	sprintf(gsbmDir, "%s/%s", settings.homedir,
			".GnomeSword/bookmarks/");

	/* shortcutbar dir */
	settings.shortcutbarDir = g_new(char, strlen(settings.gSwordDir) +
			strlen("shortcutbar") + 2);
	sprintf(settings.shortcutbarDir, "%s/%s", settings.gSwordDir,
			"shortcutbar");

	/* if shortcutbar dir does not exist create it */
	if (access(settings.shortcutbarDir, F_OK) == -1) {
		if ((mkdir(settings.shortcutbarDir, S_IRWXU)) != 0) {
			printf("can't create shortcutbar dir");
		}
	}


	/* set fnconfigure to gSwordDir and preferences.conf */
	settings.fnconfigure = g_new(char, strlen(settings.gSwordDir) +
			strlen("preferences.conf") + 2);
	sprintf(settings.fnconfigure, "%s/%s", settings.gSwordDir,
			"preferences.conf");

	/* if gSwordDir does not exist create it */
	if (access(settings.gSwordDir, F_OK) == -1) {
		if ((mkdir(settings.gSwordDir, S_IRWXU)) != 0) {
			printf(".GnomeSword does not exist");
			/* if we can not create gSwordDir exit */
			exit(1);
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
	sprintf(genbookdir, "%s/%s", settings.homedir,
			".sword/mods.d");

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
	    		g_warning("can't create .sword/modules/genbook dir");
		}
	}

	/* for local gbs support */
	sprintf(genbookdir, "%s/%s", settings.homedir,
			".sword/modules/genbook/rawgenbook");


	if (access(genbookdir, F_OK) == -1) {
		if ((mkdir(genbookdir, S_IRWXU)) != 0) {
		    g_warning("can't create .sword/modules/genbook/rawgenbook"
				    " dir");
		}
	}

	/* set swbmDir to gsbmDir */
	settings.swbmDir = g_new(char, strlen(gsbmDir) + 2);
	sprintf(settings.swbmDir, "%s", gsbmDir);
	g_free(gsbmDir);

	/* if .GnomeSword/bookmarks does not exist create it */
	if (access(settings.swbmDir, F_OK) == -1) {
		if ((mkdir(settings.swbmDir, S_IRWXU)) != 0) {
			g_warning("can't create bookmarks dir");
		}
	}

	sprintf(bmFile, "%s/%s", settings.swbmDir, "personal.conf");
	if (access(bmFile, F_OK) == -1) {
		if (retval == 1) {
			retval = 3;
		}
		else {
			retval = 2;
		}
	}

	return retval;
}
