/*
 * GnomeSword Bible Study Tool
 * main.c - In the beginning... ;o)
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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


#include <string.h>

#include "gui/gui.h"
#include "gui/main_window.h"
#include "gui/splash.h"
#include "gui/gnomesword.h"
//#include "gui/widgets.h"

#include "main/url.hh"

//#include "backend/sword.h"
 
/******************************************************************************
 * Name
 *   main
 *
 * Synopsis
 *   
 *   int main(int argc, char *argv[])
 *
 * Description
 *   Starting point of GnomeSword.
 *
 * Return value
 *   int
 */

int main(int argc, char *argv[])
{
	int newconfigs = FALSE;
	int newbookmarks = FALSE;
	int have_sword_url = FALSE;

	gui_init(argc, argv);
	
	if (argc > 2) {
		gui_generic_warning("GnomeSword does not understand more than one argument.");
		exit(1);
	}

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
		/*
		 * this arg is a sword uri
		 */
		if ((!strncmp(argv[1], "sword:/", 7)) ||
		    (!strncmp(argv[1], "bible:/", 7)) ||
		    (strstr(argv[1], "studypad"))) {
			if (!strncmp(argv[1], "bible:/", 7))
				memcpy(argv[1], "sword", 5); /* equivalent */
			have_sword_url = TRUE;
		}
	}
	/* 
	 * check for directories and files
	 */   
    	settings_init(newconfigs,newbookmarks);
	
	gui_splash_init();

	gui_splash_step1();
	
	create_mainwindow();

	gui_splash_step2();
		
	main_init_backend();
	
	gui_splash_step3();

	frontend_init();
	
	gui_splash_step4();
	
	frontend_display();
	
	gui_splash_done();
	
	if(have_sword_url)
		main_url_handler(argv[1], TRUE);

	/*gtk_notebook_set_current_page(GTK_NOTEBOOK
					(widgets.notebook_comm_book), 0);*/
	gui_main();

	return 0;
}
