/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_popup_cb.c
    * -------------------
    * Thu Feb 8 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
 */
 
 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <libgnomeprint/gnome-printer.h>
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-printer-dialog.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>
#include <libgnomeprint/gnome-print-preview.h>

#include "gs_popup_cb.h"
#include "gs_html.h"
#include "gs_editor.h"
#include "gs_unlockkey_dlg.h"
#include "gs_gnomesword.h"
#include "sword.h"
#include "gs_viewdict_dlg.h"
#include "gs_viewcomm_dlg.h"
#include "gs_viewtext_dlg.h"
#include "gs_bookmarks.h"
#include "gs_detach_int.h"
#include "support.h"


/******************************************************************************
 * externals
******************************************************************************/
extern SETTINGS 
	*settings;	/* pointer to settings structure - (declared in gs_gnomesword.c) */
	
extern GtkWidget 
	*NEtext,
	*htmlComments,
	*htmlCommentaries;
	
extern gboolean 
	autoscroll,  /* is commentary window set to scroll with text window */
	isrunningSD,    /* is the view dictionary dialog runing */
	isrunningVC,    /* is the view commentary dialog runing */
	isrunningVT,    /* is the view text dialog runing */
	noteModified;	/* personal comments window changed */
	
extern gchar 
	current_verse[80];

extern GList
	*options;
	
/*** undock/dock interlinear page ***/
void on_undockInt_activate(GtkMenuItem * menuitem,  SETTINGS *s)
{
	if(s->dockedInt) {
		s->dockedInt = FALSE;
		undock_interlinear_page(s); /* gs_interlinear_dlg.c */
		
	}
	else {
		s->dockedInt = TRUE;
		on_btnDockInt_clicked(NULL,s); /* gs_interlinear_dlg.c */
	}	
}


//----------------------------------------------------------------------------------------------
void on_edit_book_activate(GtkMenuItem * menuitem,  gchar *user_data)
{
	set_html_edit(lookup_widget(settings->app,user_data));
}


//----------------------------------------------------------------------------------------------
void on_save_book_activate(GtkMenuItem * menuitem, gchar *user_data)
{
	savebookEDITOR(lookup_widget(settings->app,user_data));
}

	

//----------------------------------------------------------------------------------------------
void on_unlock_key_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg;
	
	dlg = create_dlgUnlockKey(GPOINTER_TO_INT(user_data),settings);
	gtk_widget_show(dlg);
}


//----------------------------------------------------------------------------------------------
void on_print_item_activate(GtkMenuItem * menuitem, gchar * html)
{
	html_print(lookup_widget(settings->app,html));		
}


//----------------------------------------------------------------------------------------------
void on_goto_reference_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *buf;

	if (!GTK_EDITABLE(NEtext)->has_selection)
		return;		//-- do we have a selection?
	buf =
	    gtk_editable_get_chars(GTK_EDITABLE(NEtext),
				   GTK_EDITABLE(NEtext)->selection_start_pos,
				   GTK_EDITABLE(NEtext)->selection_end_pos);
	//changeVerseSWORD(buf);
		
}

//----------------------------------------------------------------------------------------------
void
on_goto_reference2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*	GtkWidget *text;
	gchar *buf;

	text = lookup_widget(settings->app, "textCommentaries");
	if (!GTK_EDITABLE(text)->has_selection)
		return;		//-- do we have a selection?
	buf =
	    gtk_editable_get_chars(GTK_EDITABLE(text),
				   GTK_EDITABLE(text)->selection_start_pos,
				   GTK_EDITABLE(text)->selection_end_pos);

	
	changeVerseSWORD(buf);*/
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module5_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	
	//display_about_module_dialog(getmodnameSWORD(2),FALSE);
}


//----------------------------------------------------------------------------------------------
void
on_lookup_selection_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(settings->app, "dictionarySearchText");
	text = lookup_widget(settings->app, "moduleText");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		dictSearchTextChangedSWORD(buf);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
	}*/
}

//----------------------------------------------------------------------------------------------
void
on_lookup_selection2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(settings->app, "dictionarySearchText");
	text = lookup_widget(settings->app, "textCommentaries");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
		dictSearchTextChangedSWORD(buf);
	}*/
}


//----------------------------------------------------------------------------------------------
void on_john_3_1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	changeVerseSWORD((gchar *) user_data);
}

//----------------------------------------------------------------------------------------------
void on_mainText_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings->app, "nbTextMods");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page	
}
/*
//----------------------------------------------------------------------------------------------
void on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	if (GTK_TOGGLE_BUTTON(lookup_widget(settings->app, "btnEditNote"))->
	    active) {
		text = lookup_widget(settings->app, "textComments");
		gtk_editable_cut_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
	}
}
*/
//----------------------------------------------------------------------------------------------
void on_copy4_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(settings->app, "textComments");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}
/*
//----------------------------------------------------------------------------------------------
void on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;

	text = lookup_widget(settings->app, "textComments");
	gtk_text_insert(GTK_TEXT(text), NULL, //&gtkText->style->black
			NULL, NULL,
			gs_clipboard->str, -1);
}
*/
//----------------------------------------------------------------------------------------------
void on_kjv1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	display_about_module_dialog((char *) user_data,FALSE);
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if(!strcmp((gchar *)user_data,"Bible")) 
		settings->whichwindow = MAIN_TEXT_WINDOW;
	else if (!strcmp((gchar *)user_data,"commentary")) 
		settings->whichwindow = COMMENTARY_WINDOW;
	else if (!strcmp((gchar *)user_data,"dictionary")) 
		settings->whichwindow = DICTIONARY_WINDOW;
	
	display_about_module_dialog(get_module_name(settings),FALSE);	
}

/*******************************************************************************
 *
 *******************************************************************************/	
void on_auto_scroll1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *toolbar;

	toolbar = lookup_widget(settings->app, "handlebox17");

	autoscroll = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (autoscroll)
		gtk_widget_hide(toolbar);
	else
		gtk_widget_show(toolbar);
}
		


/*******************************************************************************
 * show hide notebook tabs - popup menu choice
 *******************************************************************************/	
void on_show_tabs_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		gtk_widget_show(lookup_widget(settings->app, (gchar *)user_data));		
	else
		gtk_widget_hide(lookup_widget(settings->app, (gchar *)user_data));
	if(!strcmp((gchar *)user_data, "nbTextMods")) 
		settings->text_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if(!strcmp((gchar *)user_data, "notebook4")) 
		settings->dict_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
}
	
/*******************************************************************************
 *
 *******************************************************************************/
void on_change_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	backend_change_percom_module((gchar *) user_data);
}		

/*******************************************************************************
 *
 *******************************************************************************/		
void on_changeint1mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
//	g_warning(modName);
	sprintf(settings->Interlinear1Module,"%s",modName);
	//changeVerseSWORD(current_verse);
	if(settings->dockedInt)
		updateinterlinearpage();
	else
		updateIntDlg(settings);
}	
		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint2mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
//	g_warning(modName);
	sprintf(settings->Interlinear2Module,"%s",modName);
	//changeVerseSWORD(current_verse);
	if(settings->dockedInt)
		updateinterlinearpage();
	else
		updateIntDlg(settings);
}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint3mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{	
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
//	g_warning(modName);
	sprintf(settings->Interlinear3Module,"%s",modName);
	//changeVerseSWORD(current_verse);
	if(settings->dockedInt)
		updateinterlinearpage();
	else
		updateIntDlg(settings);
}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint4mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
//	g_warning(modName);
	sprintf(settings->Interlinear4Module,"%s",modName);
	//changeVerseSWORD(current_verse);
	if(settings->dockedInt)
		updateinterlinearpage();
	else
		updateIntDlg(settings);
}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint5mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
//	g_warning(modName);
	sprintf(settings->Interlinear5Module,"%s",modName);
	//changeVerseSWORD(current_verse);
	if(settings->dockedInt)
		updateinterlinearpage();
	else
		updateIntDlg(settings);
}


/*** toogle global options in interlinear window ***/
void on_int_global_options_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	globaloptionsSWORD((gchar *)user_data, INTERLINEAR_WINDOW, GTK_CHECK_MENU_ITEM(menuitem)->active, FALSE);	
}

void
on_viewtext_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	static GtkWidget *dlg;
	
	if(!isrunningVT) {
		dlg = create_dlgViewText(options);
	}
	gtk_widget_show(dlg);
}

/***  ***/
void on_dict_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings->app, "notebook4");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page
}

/***  ***/
void on_com_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings->app, "notebook1");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page
*/
}

