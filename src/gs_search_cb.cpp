/***************************************************************************
                          gs_search.cpp  -  description
                             -------------------
    begin                : Wed Jul 26 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
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
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>
#include <plainhtml.h>
#include <gbfhtml.h>
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>
#include "gs_search_cb.h"
#include "gs_search.h"
#include "sw_gnomesword.h"
#include "gs_gnomesword.h"
#include "gs_file.h"
#include "callback.h"
#include "support.h"

extern gboolean firstsearch;

//----------------------------------------------------------------------------------------------
void on_btnSearchOK_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(searchWindow->dlgSearch);
}

//----------------------------------------------------------------------------------------------
void on_btnSearchSaveList_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list,	//-- pointer to resultList
	*savedlg,		//-- pointer to save file dialog
	*ok_button2;		//-- pointer to fileselection ok button
	//list = lookup_widget(GTK_WIDGET(button), "resultList");	//-- set list to resultlist in search dialog
	savedlg = create_fileselectionSave();
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(savedlg),
					"/home/tb/BibleStudy/");
	ok_button2 = lookup_widget(savedlg, "ok_button2");
	gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button4_clicked), searchWindow->resultList);
	gtk_widget_show(savedlg);
}

//----------------------------------------------------------------------------------------------
void on_dlgSearch_destroy(GtkObject * object, gpointer user_data)
{
	firstsearch = TRUE;
}

//----------------------------------------------------------------------------------------------
void on_btnSearch1_clicked(GtkButton * button, gpointer user_data)
{
	gtk_clist_clear(GTK_CLIST(searchWindow->resultList));
	searchWindow->searchSWORD(searchWindow->dlgSearch);	
}

//----------------------------------------------------------------------------------------------
void on_resultList_select_row(GtkCList * clist,
			 gint row,
			 gint column, GdkEvent * event, gpointer user_data)
{
	gchar           *text;		//-- pointer to resultlist key text
 
	gtk_clist_get_text (GTK_CLIST (searchWindow->resultList), row, column, &text);	//-- get key text from resultlist
	if (searchWindow->searchMod) {	/* make sure module is not null */	  
		  searchWindow->searchMod->SetKey (text);	//-- set module to verse key text
		  searchWindow->searchMod->Display ();	//-- show verse or commentary
	}
	if (GTK_TOGGLE_BUTTON (searchWindow->cbContext)->active) {	/* check state of context check box */        
		  /* if true  show selection in main window */
		changeVerseSWORD(text);	
	}
}






