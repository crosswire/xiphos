/*
 * GnomeSword Bible Study Tool
 * main.c - In the beginning... ;o)
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

#include <glib-1.2/glib.h>

#include "gui.h"
#include "splash.h"
#include "sword.h"
#include "bookmarks.h"
#include "setup.h"
#include "properties.h"
#include "gs_gui.h"
#include "gs_gnomesword.h"
#include "settings.h"

/******************************************************************************
 * Name
 *   main
 *
 * Synopsis
 *   none
 *
 * Description
 *   Starting point of GnomeSword.
 *
 * Return value
 *   int
 */

int main(int argc, char *argv[])
{
	gint icreatefiles = 0;
	gboolean newconfigs = FALSE;
	gboolean newbookmarks = FALSE;

	gui_init(argc, argv);
	
	if (argc > 1) {
		/*
		 * these args are for broken configs or bookmarks -
		 * ie gnomesword will not start
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
	}

	/* 
	 * start swmgrs so they can be used by setup druid
	 */
	backend_first_init(&settings); /* light-up swmgrs */
	
	/* 
	 * check for directories and files
	 */    
	icreatefiles = settings_init();

	if (icreatefiles == 2 || icreatefiles == 3 || newbookmarks) {
		createbookmarksBM(settings.swbmDir);
	}

	if (icreatefiles == 1 || icreatefiles == 3 || newconfigs) {
		first_run();
	}

	backend_load_properties(&settings);

	gui_splash_init();

	gui_splash_step1();

	create_mainwindow(&settings);

	gui_splash_step2();
	
	backend_init_sword(&settings);

	gui_splash_step3();

	init_gnomesword(&settings);

	gui_splash_step4();
	
	gui_splash_done();

	gui_main();

	return 0;
}
