/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          main.cpp  -  description
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
//#include "gs_SWORD.h"
#include "GnomeSword.h"
#include "menustuff.h"
#include "support.h"
#include "callback.h"
#include "interface.h"
#include "filestuff.h"

extern SETTINGS *settings;
extern SETTINGS myset;	

int
main (int argc, char *argv[])
{
	GtkWidget 
		*mainwindow,
		*setup;
	gboolean 		
		newconfigs = false,	
		newbookmarks = false;
	gchar 
  		tmpbuf[80];

	
	gnome_init("GnomeSword", VERSION, argc, argv);		
	if(argc > 1) {			
		if(!strcmp(argv[1],"newconfigs")) newconfigs = true;
		if(!strcmp(argv[1],"newbookmarks")) newbookmarks = true;
		if(!strcmp(argv[1],"newfiles"))
		{
  			newconfigs = true;
  			newbookmarks = true;
		}
	}		
  	/*
  	setup = create_dialog1();
  	gnome_dialog_set_default(GNOME_DIALOG(setup), 2);
		gnome_dialog_run_and_close(GNOME_DIALOG(setup));
	*/
  	mainwindow = create_mainwindow ();
  	setDiretory();
  	if(newconfigs)
  	{
  		createsettings();
  	}
  	if(newbookmarks)
  	{
  		createFiles();
  	}
  	/* load settings into structure */
  	myset = readsettings(); 
  	/* set pointer to structure */
  	settings = &myset;       
  	initSword(mainwindow);
  	initGnomeSword(mainwindow);
  	gtk_widget_show(mainwindow);
  	/* set toggle state of buttons and menu items */
  	UpdateChecks(mainwindow);    	
  	gtk_main ();  	
  	return 0;
}
