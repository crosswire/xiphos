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
#include <string.h>


#ifdef USE_GTKHTML1
#include <gconf/gconf.h>
#endif	

#include "gui/gui.h"
#include "gui/main_window.h"
#include "gui/splash.h"
#include "gui/gnomesword.h"

#include "backend/sword.h"

#include "main/settings.h"
 
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
	gint first_run = 0;
	gboolean newconfigs = FALSE;
	gboolean newbookmarks = FALSE;

#ifdef USE_GTKHTML1
	GError *gconf_error = NULL;
#endif
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
	
#ifdef USE_GTKHTML1
    /* 
     * This is needed for gtkhtml-1.1
     */

    if (!gconf_init(argc, argv, &gconf_error)) {
	g_assert(gconf_error != NULL);
	g_error("GConf init failed:\n  %s", gconf_error->message);
	return FALSE;
    }
#endif
	/* 
	 * check for directories and files
	 */   
    	first_run = settings_init(newconfigs,newbookmarks);
    
//	backend_load_properties(settings.fnconfigure); //settings.fnconfigure);
	
	gui_splash_init();

	gui_splash_step1();
    
	backend_init();

	gui_splash_step2();
		
	create_mainwindow();

	gui_splash_step3();

	init_gnomesword();

	gui_splash_step4();
	
	gui_splash_done();

	gui_main();

	return 0;
}
