/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          main.c  -  description
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/
 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "gs_sword.h"
#include "gs_gnomesword.h"
#include "gs_file.h"
#include "gs_html.h"
#include "gs_gui.h"
#include "sw_properties.h"
#include "gs_setup.h"
#include "gs_bookmarks.h"
#include "sw_bookmarks.h"
#include "e-splash.h"

extern SETTINGS 
	*settings;
SETTINGS 
	myset;	
extern GList 	
	*biblemods,
	*commentarymods,
	*dictionarymods,
	*percommods;	
extern gchar *swbmDir;
 
int
main (int argc, char *argv[])
{
	GtkWidget 
		*mainwindow;
	GtkWidget 
		*splash;
	gboolean 		
		newconfigs = FALSE,	
		newbookmarks = FALSE;
	gint 
		icreatefiles=0;
	
	gnome_init("GnomeSword", VERSION, argc, argv);		
	if(argc > 1) {			
		if(!strcmp(argv[1],"newconfigs")) newconfigs = TRUE;
		if(!strcmp(argv[1],"newbookmarks")) newbookmarks = TRUE;
		if(!strcmp(argv[1],"newfiles"))
		{
  			newconfigs = TRUE;
  			newbookmarks = TRUE;
		}
	}  	
  	icreatefiles = setDiretory(); /*** gs_file.c ***/  
  	if(newconfigs)
  	{
  		gs_firstrunSWORD(); /*** gs_sword.cpp ***/  
  	}
  	if(newbookmarks)
  	{
  		createbookmarksBM(swbmDir); /*** sw_bookmarks.cpp ***/ 
  	}
	//icreatefiles = 1;/* please remove me - i am for testing */
	if(icreatefiles == 1 || icreatefiles == 3 ){		
		gs_firstrunSWORD(); /*** gs_sword.cpp ***/  
	}
  	/* set pointer to structure */
  	settings = &myset; 
	loadconfig(); /* new */  /*** sw_propertiew.cpp ***/  
	/* splash screen */	
	splash = e_splash_new (); /*** e-splash.c ***/ 
	if(settings->showsplash){	
		gtk_widget_show (splash);
		gtk_object_ref (GTK_OBJECT (splash));
		while (gtk_events_pending ())
			gtk_main_iteration ();
	}  	 
	mainwindow = create_mainwindow (splash); /*** gs_gui.c ***/ 
  	add_gtkhtml_widgets(mainwindow); /*** gs_html.c ***/ 
	
	if(settings->showsplash)
		e_splash_set_icon_highlight (E_SPLASH(splash),1, TRUE);
  	initSWORD(mainwindow); /*** gs_sword.cpp ***/  
	if(settings->showsplash)
		e_splash_set_icon_highlight (E_SPLASH(splash),2, TRUE);
  	initGnomeSword(mainwindow,settings,biblemods,commentarymods,dictionarymods,percommods,splash); /*** gs_gnomesword.c ***/ 
	if(icreatefiles == 2 || icreatefiles == 3 ){		
		if(loadoldbookmarks() == 0){ /*** gs_bookmarks.c ***/ 
			g_warning("loadoldbookmarks failed!");
			createbookmarksBM(swbmDir); /*** sw_bookmarks.cpp ***/ 
		}
	}
	while (gtk_events_pending ())
		gtk_main_iteration ();
  	if(settings->showsplash)
		gtk_widget_unref (splash);
	gtk_widget_destroy (splash);
  	gtk_widget_show(mainwindow);
  	/* set toggle state of buttons and menu items */
  	UpdateChecks(mainwindow);    	
  	gtk_main ();  	
  	return 0;
}

