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

#include <gnome.h>
#include <gtk/gtk.h>

#ifdef GTKHTML_HAVE_GCONF
#include <gconf/gconf.h>
#endif

#include "splash.h"
#include "sword.h"
#include "gs_gnomesword.h"
#include "gs_file.h"
#include "gs_html.h"
#include "gs_gui.h"
#include "properties.h"
#include "gs_setup.h"
#include "gs_bookmarks.h"
#include "bookmarks.h"

extern SETTINGS *settings;
extern GList *biblemods;
extern GList *commentarymods;
extern GList *dictionarymods;
extern GList *percommods;
extern gchar *swbmDir;

SETTINGS myset;

int main(int argc, char *argv[])
{
	GtkWidget *mainwindow;
	gint icreatefiles = 0;
	gboolean newconfigs = FALSE;
	gboolean newbookmarks = FALSE;

#ifdef GTKHTML_HAVE_GCONF
	GError *gconf_error = NULL;
#endif

#if ENABLE_NLS
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain(PACKAGE);
#endif

	gnome_init("GnomeSword", VERSION, argc, argv);
	
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
	
#ifdef GTKHTML_HAVE_GCONF
	/* 
	 * This is needed for gtkhtml.
	 */

	if (!gconf_init(argc, argv, &gconf_error)) {
		g_assert(gconf_error != NULL);
		g_error("GConf init failed:\n  %s", gconf_error->message);
		return FALSE;
	}
#endif
	
	/* 
	 * start swmgrs so they can be used by setup druid
	 */
	backend_firstInitSWORD(); /* light-up swmgrs */
	
	/* 
	 * check for directories and files
	 */    
	icreatefiles = setDiretory();

	if (newconfigs) {
		gui_firstRunSETUP(); //gs_firstrunSWORD();
	}
	
	if (newbookmarks) {
		createbookmarksBM(swbmDir);
	}

	/*icreatefiles = 1;*/ /* please comment me - i am for testing */
	if (icreatefiles == 1 || icreatefiles == 3) {
		gui_firstRunSETUP();  //gs_firstrunSWORD();
	}

	/*
	 * Set pointer to structure.
	 */
	settings = &myset;
	loadconfig(settings);

	/*
	 * If we have a new version run setup druid FIXME: do we need this?
	 */
	if (strcmp(VERSION, settings->gs_version)) {
		gui_firstRunSETUP(); 
	}

	gui_splash_init();

	gui_splash_step1();

	mainwindow = create_mainwindow(settings);
	add_gtkhtml_widgets(mainwindow);/*FIXME: add these widgets to create_mainwindow() */

	gui_splash_step2();
	
	initSWORD(settings);

	gui_splash_step3();

	initGnomeSword(settings, biblemods, commentarymods, dictionarymods,
		   percommods);

	gui_splash_step4();
	
	if (icreatefiles == 2 || icreatefiles == 3) {
		createbookmarksBM(swbmDir);
	}

	gui_splash_done();

	/*
	 * Set the main window size.
	 */
	gtk_widget_set_usize(settings->app, settings->gs_width,
			 settings->gs_hight);

	/*
	 * Set toggle state of buttons and menu items.
	 */
	UpdateChecks(settings);
	
	/* showing the devotional must come after the the app is shown or
	   it will mess up the shortcut bar display */
	/* FIXME: maybe we need to move the devotional ? */
	if (settings->showdevotional) {
		displayDevotional();
	}
	
	gtk_main();

	return 0;
}
