/*
 * GnomeSword Bible Study Tool
 * gs_history.c - SHORT DESCRIPTION
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

/* main */
#include "gs_gnomesword.h"
#include "sword.h"
#include "gs_history.h"
#include "gs_menu.h"
#include "support.h"
#include "settings.h"

/* frontend */
#include "main_menu.h"

#include <gal/e-paned/e-hpaned.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>

HISTORY historylist[25];
gint historyitems = 0;
gint currenthistoryitem = 0;
gboolean addhistoryitem = FALSE; /* do we need to add item to history */
gboolean firstbackclick = TRUE;

extern gint groupnum4;
extern gchar *shortcut_types[];

/*****************************************************************************
 *clearhistory - someone clicked clear history
 *app
 *shortcut_bar
*****************************************************************************/
void clearhistory(GtkWidget *app, GtkWidget *shortcut_bar)
{
        gint i;

        removemenuitems(app, _("_History/<Separator>"), historyitems+1);
        addseparator(app, _("_History/C_lear"));
        /* set sensitivity of history buttons */
        gtk_widget_set_sensitive(lookup_widget(app,"btnBack"), FALSE);
	gtk_widget_set_sensitive(lookup_widget(app,"btnFoward"), FALSE);
	
        if(settings.showhistorygroup){
        	for(i = historyitems-1; i >= 0; i--) {
        		e_shortcut_model_remove_item(E_SHORTCUT_BAR(shortcut_bar)->model,
						  groupnum4,
						  i);
		}
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

	/* check to see if item is already in list  
	     if so do nothing */
	for(i=0;i<historyitems;i++){
		if(!strcmp(historylist[i].verseref,ref)) 
			return;
	}	
	/* add item to history menu */
	if(historyitems >= 24) {
	        for(i=0;i<24;i++) { 	
	                historylist[i] = historylist[i+1];
	        }
	        historyitems = 23;
	}
	historylist[historyitems].itemnum = historyitems;	
	historylist[historyitems].compagenum = gtk_notebook_get_current_page(
	                        GTK_NOTEBOOK(settings.notebook_comm));		
	sprintf(historylist[historyitems].verseref,"%s",ref); 
	sprintf(historylist[historyitems].textmod,"%s",settings.MainWindowModule);	
	sprintf(historylist[historyitems].commod,"%s", settings.CommWindowModule);
	
	++historyitems;	
	currenthistoryitem = historyitems;
        /* set sensitivity of history buttons */
	if(currenthistoryitem > 0) gtk_widget_set_sensitive(lookup_widget(app,"btnBack"), TRUE);
	gtk_widget_set_sensitive(lookup_widget(app,"btnFoward"), FALSE);
	updatehistorymenu(app);
	firstbackclick = TRUE;
	if(settings.showhistorygroup){
                updatehistoryshortcutbar(app, shortcut_bar);
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
        } else { 
		addhistoryitem = FALSE;
	}
	/* change text mod */
	change_module_and_key(historylist[historynum].textmod, historylist[historynum].verseref);
	/* change commentary mod */
	change_module_and_key(historylist[historynum].commod, historylist[historynum].verseref);
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

        removemenuitems(app, _("_History/<Separator>"), historyitems+1);
        addseparator(app, _("_History/C_lear"));
        for(i=0;i<historyitems;i++) {
                sprintf(buf,"%d",historylist[i].itemnum);
                additemtognomemenu(app, historylist[i].verseref, buf, _("_History/<Separator>"),
					(GtkMenuCallback) on_mnuHistoryitem1_activate);
        }
}

/*
 *
 */
void updatehistoryshortcutbar(GtkWidget *app, GtkWidget *shortcut_bar)
{
        gint i;

        for(i = historyitems-2; i >= 0; i--) {
                e_shortcut_model_remove_item(E_SHORTCUT_BAR(shortcut_bar)->model,
                                                groupnum4,
						i);
        }
        for(i=0;i<historyitems;i++) {
		e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
						groupnum4, 
						-1,
						NULL,
						historylist[i].verseref,
						NULL);
        }
}
