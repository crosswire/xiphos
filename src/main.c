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
#include "gs_menu.h"
#include "support.h"
//#include "callback.h"
#include "interface.h"
#include "gs_file.h"
#include "gs_html.h"
#include "gs_gui.h"
#include "sw_properties.h"
#include "gs_setup.h"

extern SETTINGS *settings;
SETTINGS myset;	
extern GList 	*biblemods,
	*commentarymods,
	*dictionarymods,
	*percommods;
int
main (int argc, char *argv[])
{
	GtkWidget 
		*mainwindow;
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
  	mainwindow = create_mainwindow ();
  	add_gtkhtml_widgets(mainwindow);
  	icreatefiles = setDiretory(); 
  	if(newconfigs)
  	{
  		gs_firstrunSWORD();
  	}
  	if(newbookmarks)
  	{
  		createFiles();
  	}
	//icreatefiles = 1;/* please remove me - i am for testing */
	if(icreatefiles == 1){		
		gs_firstrunSWORD();
	}
  	/* set pointer to structure */
  	settings = &myset; 
	loadconfig(); /* new */      
  	initSWORD(mainwindow);
  	initGnomeSword(mainwindow,settings,biblemods,commentarymods,dictionarymods,percommods);
  	
  	gtk_widget_show(mainwindow);
  	/* set toggle state of buttons and menu items */
  	UpdateChecks(mainwindow);    	
  	gtk_main ();  	
  	return 0;
}

