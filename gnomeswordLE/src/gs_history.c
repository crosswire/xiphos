/***************************************************************************
                          gs_history.cpp  -  description
                             -------------------
    begin                : Thu Feb 1 2001
    copyright            : (C) 2001 by Terry Biggs
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

#include "gs_gnomesword.h"
#include "gs_sword.h"
#include "gs_history.h"
#include "callback.h"
#include "gs_menu.h"
#include "support.h"

HISTORY historylist[25];
gint historyitems = 0;
gint currenthistoryitem = 0;
gboolean addhistoryitem = FALSE; /* do we need to add item to history */
gboolean firstbackclick = TRUE;
extern SETTINGS *settings;
extern gint groupnum4;

/*****************************************************************************
 *clearhistory - someone clicked clear history
 *app
 *shortcut_bar
*****************************************************************************/
void clearhistory(GtkWidget *app, GtkWidget *shortcut_bar)
{
        GtkWidget *toolbar30,
		  *button;

        removemenuitems(app, "_History/<Separator>", historyitems+1);
        addseparator(app, "_History/C_lear");
        /* set sensitivity of history buttons */
        gtk_widget_set_sensitive(lookup_widget(app,"btnBack"), FALSE);
	gtk_widget_set_sensitive(lookup_widget(app,"btnFoward"), FALSE);
	
        if(settings->showhistorygroup){
        	gtk_widget_destroy(lookup_widget(app,"toolbar30"));

        	toolbar30 = gtk_toolbar_new (GTK_ORIENTATION_VERTICAL, GTK_TOOLBAR_TEXT);
  		gtk_widget_ref (toolbar30);
  		gtk_object_set_data_full (GTK_OBJECT (app), "toolbar30", toolbar30,
                            (GtkDestroyNotify) gtk_widget_unref);
  		gtk_widget_show (toolbar30);
  		gtk_container_add (GTK_CONTAINER (lookup_widget(app,"viewport4")), toolbar30);
  		gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar30), GTK_RELIEF_NONE);
  	
  		button = gtk_toolbar_append_element (GTK_TOOLBAR (lookup_widget(app,"toolbar30")),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Clear History",
                                "Clear History",
                                NULL,
                                NULL, NULL, NULL);
  		gtk_widget_ref (button);
  		gtk_object_set_data_full (GTK_OBJECT (app), "button", button,
                            	(GtkDestroyNotify) gtk_widget_unref);
  		gtk_widget_show (button);
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      		GTK_SIGNAL_FUNC (on_btnClearHistory_clicked),
                     	 	NULL);
        }
        historyitems = 0;
        currenthistoryitem = 0;
        firstbackclick = TRUE;
}

/*****************************************************************************
 * addHistoryItem - add an item to the history menu
 * ref
*****************************************************************************/
void addHistoryItem(GtkWidget *app, GtkWidget *shortcut_bar, gchar *ref)
{
        gint i;

	/* add item to history menu */
	if(historyitems >= 24) {
	        for(i=0;i<24;i++) { 	
	                historylist[i] = historylist[i+1];
	        }
	        historyitems = 23;
	}
	historylist[historyitems].itemnum = historyitems;	
	historylist[historyitems].compagenum = gtk_notebook_get_current_page(
	                        GTK_NOTEBOOK(lookup_widget(app,"notebook1")));	
	sprintf(historylist[historyitems].verseref,"%s",ref); 	
	//g_warning("savemod = %s", getcommodSWORD());
	sprintf(historylist[historyitems].textmod,"%s",gettextmodSWORD());	
	sprintf(historylist[historyitems].commod,"%s", getcommodSWORD());
	
	++historyitems;	
	currenthistoryitem = historyitems;
        /* set sensitivity of history buttons */
	if(currenthistoryitem > 0) gtk_widget_set_sensitive(lookup_widget(app,"btnBack"), TRUE);
	gtk_widget_set_sensitive(lookup_widget(app,"btnFoward"), FALSE);
	
	updatehistorymenu(app);
	firstbackclick = TRUE;
	if(settings->showhistorygroup){
		updatehistorysidebar(app);				
	}
}

/*
 *
 */
void changeverseHistory(gint historynum)
{
        currenthistoryitem = historynum;
        if(firstbackclick){
                 addhistoryitem = TRUE;                 
        } else   addhistoryitem = FALSE;
        //g_warning("change mod = %s\n",historylist[historynum].textmod);
        changecurModSWORD(historylist[historynum].textmod,FALSE);
        changcurcomModSWORD(historylist[historynum].commod,historylist[historynum].compagenum, FALSE);
        changeVerseSWORD(historylist[historynum].verseref);
	if(firstbackclick){
                --currenthistoryitem;
		--currenthistoryitem;
                firstbackclick = FALSE;
        } 
}

/*
 *
 */
void historynav(GtkWidget *app, gint direction)
{
        addhistoryitem = FALSE;
        if(direction){
                if(currenthistoryitem < historyitems-1){
                        ++currenthistoryitem;
                        changeverseHistory(currenthistoryitem);
                }
                /* set sensitivity of history buttons */
	        if(currenthistoryitem >= historyitems-1) gtk_widget_set_sensitive(
	                                lookup_widget(app,"btnFoward"), FALSE);
	        if(currenthistoryitem >= 0) gtk_widget_set_sensitive(
					lookup_widget(app,"btnBack"), TRUE);
        } else {
                if(currenthistoryitem > 0){
                        --currenthistoryitem;
			if(firstbackclick) addhistoryitem = TRUE;
                        changeverseHistory(currenthistoryitem);
                        firstbackclick = FALSE;
                }
                /* set sensitivity of history buttons */
	        if(currenthistoryitem < 1) gtk_widget_set_sensitive(
	                                lookup_widget(app,"btnBack"), FALSE);
	        if(currenthistoryitem < historyitems) gtk_widget_set_sensitive(
	                                lookup_widget(app,"btnFoward"), TRUE);
        }
}

/*
 *
 */
void updatehistorymenu(GtkWidget *app)
{
        gint i;
        gchar buf[80];

        removemenuitems(app, "_History/<Separator>", historyitems+1);
        addseparator(app, "_History/C_lear");
        for(i=0;i<historyitems;i++) {
                sprintf(buf,"%d",historylist[i].itemnum);
                additemtognomemenu(app, historylist[i].verseref, buf, "_History/<Separator>",(GtkMenuCallback) on_mnuHistoryitem1_activate);
        }
}

/*
 *
 */
void updatehistorysidebar(GtkWidget *app)
{
        GtkWidget *button,
                *toolbar30;
        gint i;

        gtk_widget_destroy(lookup_widget(app,"toolbar30"));

        toolbar30 = gtk_toolbar_new (GTK_ORIENTATION_VERTICAL, GTK_TOOLBAR_TEXT);
        gtk_widget_ref (toolbar30);
        gtk_object_set_data_full (GTK_OBJECT (app), "toolbar30", toolbar30,
                            (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (toolbar30);
        gtk_container_add (GTK_CONTAINER (lookup_widget(app,"viewport4")), toolbar30);
        gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar30), GTK_RELIEF_NONE);
  	
        button = gtk_toolbar_append_element (GTK_TOOLBAR (lookup_widget(app,"toolbar30")),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Clear History",
                                "Clear History",
                                NULL,
                                NULL, NULL, NULL);
        gtk_widget_ref (button);
        gtk_object_set_data_full (GTK_OBJECT (app), "button", button,
                            	(GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      		GTK_SIGNAL_FUNC (on_btnClearHistory_clicked),
                     	 	NULL);
                     	 	
        for(i=0;i<historyitems;i++) {
                button = gtk_toolbar_append_element (GTK_TOOLBAR (lookup_widget(app,"toolbar30")),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                historylist[i].verseref,
                                historylist[i].verseref, NULL,
                                NULL, NULL, NULL);
  	        gtk_widget_ref (button);
  	        gtk_object_set_data_full (GTK_OBJECT (app), "button", button,
                            	(GtkDestroyNotify) gtk_widget_unref);
  	        gtk_widget_show (button);
	        gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      		GTK_SIGNAL_FUNC (on_historybutton_clicked),
                     	 	GINT_TO_POINTER(historylist[i].itemnum));
         }	

}

