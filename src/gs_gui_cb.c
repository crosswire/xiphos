/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_gui_cb.c
    * -------------------
    * Mon May 8 2000
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

#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>

#include "gs_gui_cb.h"
#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_popup_cb.h"
#include "sw_gnomesword.h"
#include "gs_viewdict_dlg.h"
#include "gs_history.h"
#include "support.h"
#include "gs_file.h"
//#include "gs_listeditor.h"
#include "gs_abouts.h"


/******************************************************************************
 * globals
******************************************************************************/
gboolean 
	firstsearch = TRUE,	/* search dialog is not running when TRUE */
	firstLE = TRUE;	  /* ListEditor in not running when TRUE */
GtkWidget 
	*listeditor;
GtkWidget *toolbarComments;
GtkWidget *toolbarBooks;
GtkWidget *toolbarStudypad;
	
/******************************************************************************
 * externals
******************************************************************************/
extern gchar 
	current_verse[80];
extern SETTINGS 
	*settings;	/* pointer to settings structure - (declared in gs_gnomesword.c) */
extern gboolean 
	ApplyChange,	/* to keep form looping when book combobox is changed */
	noteModified;	/* personal comments window changed */
extern GtkWidget 
	*htmlComments,
	*NEtext;	/* menu check item (declared in gs_gnomesword.c) */
extern gint 
	answer;		/* do we save file on exit */

/*********************************************************************************
**********************************************************************************
 *callbacks settings->app
**********************************************************************************
*********************************************************************************/

void
on_mainwindow_size_allocate(GtkWidget *widget,
                                        GtkAllocation   *allocation,
                                        gpointer  user_data)
{
	settings->gs_width = allocation->width;
	settings->gs_hight = allocation->height;
//	g_warning("gs hight = %d\ngs width = %d", settings->gs_hight,settings->gs_width);
}

//----------------------------------------------------------------------------------------------
void
on_btnStrongs_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GTK_CHECK_MENU_ITEM(settings->strongsnum)->active = togglebutton->active;	//-- change menu check item to match button
	globaloptionsSWORD("Strong's Numbers", 0, togglebutton->active,TRUE);	//-- trun strongs numbers on and off(gs_sword.cpp)	
}
//----------------------------------------------------------------------------------------------
void
on_btnMorphs_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GTK_CHECK_MENU_ITEM(settings->morphs)->active = togglebutton->active;	//-- change menu check item to match button
	globaloptionsSWORD("Morphological Tags", 0, togglebutton->active,TRUE);	//-- trun strongs numbers on and off(gs_sword.cpp)	
}
//----------------------------------------------------------------------------------------------
void
on_btnFootnotes_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GTK_CHECK_MENU_ITEM(settings->footnotes)->active = togglebutton->active;	//-- change menu check item to match button
	globaloptionsSWORD("Footnotes", 0, togglebutton->active,TRUE);	//-- trun strongs numbers on and off(gs_sword.cpp)	
}

//----------------------------------------------------------------------------------------------
void on_mainwindow_destroy(GtkObject * object, gpointer user_data)
{
	shutdownSWORD();
}

//----------------------------------------------------------------------------------------------
void on_btnSearch_clicked(GtkButton * button, gpointer user_data)
{
	SETTINGS *s;
	
	s = (SETTINGS *)user_data;
	showSBGroup(s, s->searchbargroup);
}

//----------------------------------------------------------------------------------------------
void on_cbeBook_changed(GtkEditable * editable, gpointer user_data)
{
	if (ApplyChange) {
		bookSWORD();
	}
}

//----------------------------------------------------------------------------------------------
gboolean
on_spbChapter_button_release_event(GtkWidget * widget,
				   GdkEventButton * event,
				   gpointer user_data)
{
	if (ApplyChange) {
		verseSWORD();
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------------
gboolean
on_spbVerse_button_release_event(GtkWidget * widget,
				 GdkEventButton * event,
				 gpointer user_data)
{
	if (ApplyChange) {
		verseSWORD();
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------------
void on_btnLookup_clicked(GtkButton * button, gpointer user_data)
{
	btnlookupSWORD();
}

//----------------------------------------------------------------------------------------------
gboolean
on_cbeFreeformLookup_key_press_event(GtkWidget * widget,
				     GdkEventKey * event,
				     gpointer user_data)
{
	freeformlookupSWORD(event);
	return TRUE;
}


//----------------------------------------------------------------------------------------------
void on_btnExit_clicked(GtkButton * button, gpointer user_data)
{
	shutdownSWORD();
}

//----------------------------------------------------------------------------------------------
void /* commentary notebook page changed */
on_nbTextMods_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	GtkLabel *label;	//-- pointer to page label
	
	label = (GtkLabel *) page->tab_label;	//-- get label
	changecurModSWORD((char *) label->label,TRUE ); 
}


//----------------------------------------------------------------------------------------------
void /* commentary notebook page changed */
on_notebook1_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	GtkLabel *label;	//-- pointer to page label
	static gboolean firsttime = TRUE;
	if (!firsttime) {
		label = (GtkLabel *) page->tab_label;	//-- get label
		changcurcomModSWORD((char *) label->label, TRUE);	//-- pass label text and page number
	}								//-- to function to do the work - gs_sword.cpp
	firsttime = FALSE;
}

//----------------------------------------------------------------------------------------------
void on_textComments_changed(GtkEditable * editable, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
gboolean
on_textComments_button_release_event(GtkWidget * widget,
				     GdkEventButton * event,
				     gpointer user_data)
{

	return FALSE;
}

//----------------------------------------------------------------------------------------------
void
on_btnEditNote_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{	
	gtk_html_set_editable(GTK_HTML(htmlComments), GTK_TOGGLE_BUTTON(togglebutton)->active);
	if(GTK_TOGGLE_BUTTON(togglebutton)->active){
		gtk_widget_show(settings->toolbarComments);
	}
	
	else {
		gtk_widget_hide(settings->toolbarComments);
	}
}

//----------------------------------------------------------------------------------------------
void on_btnSaveNote_clicked(GtkButton * button, gpointer user_data)
{
	if (GTK_TOGGLE_BUTTON
	    (lookup_widget(GTK_WIDGET(button), "btnEditNote"))->active) {
		//savenoteSWORD(TRUE);
	}
}

//----------------------------------------------------------------------------------------------
void on_btnDeleteNote_clicked(GtkButton * button, gpointer user_data)
{
	if (GTK_TOGGLE_BUTTON
	    (lookup_widget(GTK_WIDGET(button), "btnEditNote"))->active) {
		deletenoteSWORD();
	}
}


//----------------------------------------------------------------------------------------------
void
//-- someone changed dict & lex notebook page---------------------------------------------------
on_notebook4_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	GtkLabel *label;	//-- pointer to tab label
	gchar *keyText;		//-- string for verse key text
	gchar *entryText;	//-- pointer to dict key
	static gboolean firsttime = TRUE; //-- dont do anything if this is the first time here, but remember we were here - set firsttime to FALSE
	if (!firsttime) {
		label = (GtkLabel *) page->tab_label;	//-- set label to tab label
		entryText =
		    gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget
					(GTK_WIDGET(notebook), "dictionarySearchText")));	//-- get key from entry
		if (!strcmp(label->label, "Torrey"))
			entryText = "";	//-- if Torrey module then set text to null else segfault?
		keyText = entryText;	//-- put entryText into string keyText
		changcurdictModSWORD((char *) label->label, keyText);	/* sent to changcurdictModSWORD() function in
															 gs_sword.cpp */
	}
	firsttime = FALSE;	//-- no longer the first time
}

//----------------------------------------------------------------------------------------------
void
on_dictionarySearchText_changed(GtkEditable * editable, gpointer user_data)
{
	gchar *mytext;

	mytext = gtk_entry_get_text(GTK_ENTRY(editable));
	dictSearchTextChangedSWORD(mytext);

}

//----------------------------------------------------------------------------------------------
void
on_list1_select_row(GtkCList * clist,
		    gint row,
		    gint column, GdkEvent * event, gpointer user_data)
{
	gchar *text, *buf;

	gtk_clist_get_text(GTK_CLIST
			   (lookup_widget(GTK_WIDGET(clist), "list1")),
			   row, column, &text);
	buf =
	    gtk_entry_get_text(GTK_ENTRY
			       (lookup_widget
				(GTK_WIDGET(clist),
				 "dictionarySearchText")));
	gtk_entry_set_text(GTK_ENTRY
			   (lookup_widget
			    (GTK_WIDGET(clist), "dictionarySearchText")),
			   text);
}


//----------------------------------------------------------------------------------------------
void on_btnKeyPrev_clicked(GtkButton * button, gpointer user_data)
{
	dictchangekeySWORD(0);
}

//----------------------------------------------------------------------------------------------
void on_btnKeyNext_clicked(GtkButton * button, gpointer user_data)
{
	dictchangekeySWORD(1);
}

//----------------------------------------------------------------------------------------------
void
on_notebook3_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	GnomeDockItem* item;
	static gboolean firsttime = TRUE;	//-- dont do anything if this is the first time here, but remember we were here - set firsttime to FALSE
	if (!firsttime) {
		changepagenotebook(notebook, page_num);	//-- send to changepagenotebook() function in GnomeSword.cpp
	}
	firsttime = FALSE;	//-- remember we were here
	
	gtk_widget_hide(settings->toolbarBooks);
	gtk_widget_hide(settings->toolbarComments);
	gtk_widget_hide(settings->toolbarStudypad);
	
	if(page_num == 2 && settings->editnote) {
		gtk_widget_show(settings->toolbarComments);
	}
	
	else if(page_num == 1 && settings->editgbs) {
		gtk_widget_show(settings->toolbarBooks);
	}
	
	else if(page_num == 4) {
		gtk_widget_show(settings->toolbarStudypad);	
	}		
}

//----------------------------------------------------------------------------------------------
void on_btnInfoBoxYes_clicked(GtkButton * button, gpointer user_data)
{
	answer = 0;
}

//----------------------------------------------------------------------------------------------
void on_btnInfoBoxNo_clicked(GtkButton * button, gpointer user_data)
{
	answer = 1;
}

//----------------------------------------------------------------------------------------------
void
on_tbtnFollow_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
gboolean
on_textComments_key_press_event(GtkWidget * widget,
				GdkEventKey * event, gpointer user_data)
{
//	gchar *buf;
	static gboolean needsecond = FALSE;

	if (!settings->formatpercom){	/* do we want formatting?	*/
		noteModified = TRUE;	/* note has been modified to get us here */
		return FALSE;
	}
	if (event->keyval == 65293 || event->keyval == 65421) {	/* return key */
		gtk_text_set_point(GTK_TEXT(NEtext), gtk_editable_get_position(GTK_EDITABLE(NEtext)));
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "<br>", -1); 		
		noteModified = TRUE;	//-- noteeditor.cpp
		return TRUE;
	}
	if (GTK_EDITABLE(NEtext)->has_selection){	//-- do we have a selection?	
		if (event->keyval == 65507){	//-- ctrl key pressed		
			needsecond = TRUE;
			return FALSE;
		}
	}

	noteModified = TRUE;
	if (needsecond) {
		switch (event->keyval) {
		case 98:	//-- bold
			//noteModified = noteeditor->setBOLD(NEtext);	//-- noteeditor.cpp
			break;
		case 105:	//-- italics
			//noteModified = noteeditor->setITALIC(NEtext);	//-- noteeditor.cpp
			break;
		case 114:	//-- reference
			//noteModified = noteeditor->setREFERENCE(NEtext);	//-- noteeditor.cpp
			break;
		case 108:	//-- underline -- broken using l not u - u removes line
			//noteModified = noteeditor->setUNDERLINE(NEtext);	//-- noteeditor.cpp
			break;
		case 103:	//-- greek font
			//noteModified = noteeditor->setGREEK(NEtext);	//-- noteeditor.cpp
			break;
		default:
			break;
		}
		needsecond = FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------
void on_btnBack_clicked(GtkButton * button, gpointer user_data)
{
	historynav(settings->app, 0);
}

//----------------------------------------------------------------------------------------------
void on_btnFoward_clicked(GtkButton * button, gpointer user_data)
{
	historynav(settings->app, 1);
}

//----------------------------------------------------------------------------------------------
void on_btnComPrev_clicked(GtkButton * button, gpointer user_data)
{
	navcurcomModSWORD(0);
}


void on_btnComNext_clicked(GtkButton * button, gpointer user_data)
{
	navcurcomModSWORD(1);
}

//----------------------------------------------------------------------------------------------
gboolean
    on_dictionarySearchText_key_press_event
    (GtkWidget * widget, GdkEventKey * event, gpointer user_data) {
	gchar *mytext;

	mytext = gtk_entry_get_text(GTK_ENTRY(widget));

	if (event->keyval == 65293 || event->keyval == 65421)	//-- return key
	{
		dictSearchTextChangedSWORD(mytext);
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------------
gboolean
on_list1_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
{

	return FALSE;
}

//----------------------------------------------------------------------------------------------
gboolean
on_epaned_button_release_event(GtkWidget       *widget,
                               GdkEventButton  *event,
                               gpointer         user_data)
{
	gint panesize;
	panesize = e_paned_get_position(E_PANED(lookup_widget(settings->app,(gchar *)user_data)));
	        
	if(panesize > 15 ) {
		if(!strcmp((gchar *)user_data,"epaned"))
			settings->shortcutbar_width = panesize; 
		if(!strcmp((gchar *)user_data,"vpaned1"))
			settings->upperpane_hight = panesize; 
		if(!strcmp((gchar *)user_data,"hpaned1"))
			settings->biblepane_width = panesize; 
	}	
        return TRUE;
}

//----------------------------------------------------------------------------------------------
void on_listeditor_destroy(GtkObject * object, gpointer user_data)
{
	firstLE = TRUE;
}

void on_btnCommSyn_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	changeVerseComSWORD();
}


void on_btnCommPrint_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	on_print_item_activate(NULL, "htmlCommentaries");
}


