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
#include <swmodule.h>

#include "GnomeSword.h"
#include "menustuff.h"
#include "support.h"
#include "callback.h"
#include "interface.h"
#include "filestuff.h"
#include "dialogs.h"

int
main (int argc, char *argv[])
{
  GtkWidget *mainwindow;
  GtkWidget *menu1;
  GtkWidget *menu2;
  GtkWidget *menu3;
  GtkWidget *menu4;
  GtkWidget *menu5;
  bool 			newconfigs = false;
  bool			newbookmarks = false;
  gchar 		*buf,
  					tmpbuf[80];

  sprintf(tmpbuf,"%d",argc);
  cout << tmpbuf;
  if(argc > 1)
  buf = g_strdup(argv[1]);
	
  gnome_init ("GnomeSword", VERSION, argc, argv);
  if(!strcmp(buf,"newconfigs")) newconfigs = true;
  if(!strcmp(buf,"newbookmarks")) newbookmarks = true;
  if(!strcmp(buf,"newfiles"))
  {
  	newconfigs = true;
  	newbookmarks = true;
  }
  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */

  menu1 = create_menu1();
  menu2 = create_menuInt1();
  menu3 = create_menuInt2();
  menu4 = create_menuInt3();
  menu5 = create_pmEditnote();
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
  initSword(mainwindow,menu1,menu2,menu3,menu4,menu5);
  gtk_widget_show (mainwindow);
  UpdateChecks(mainwindow); //-- set toggle state of buttons and menu items
  gtk_main ();
  return 0;
}

