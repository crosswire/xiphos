/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          callback.cpp  -  description
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by Terry Biggs
    email                :  tbiggs@infinet.com
 ***************************************************************************/

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

#if USE_SHORTCUTBAR
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>
#endif /* USE_SHORTCUTBAR */

#include "callback.h"
#include "gs_gnomesword.h"
#include "gs_sword.h"
#include "gs_viewdict.h"
#include "gs_history.h"
#include "support.h"
#include "interface.h"
#include "gs_file.h"
#include "gs_listeditor.h"
#include "gs_abouts.h"
#include "printstuff.h"


/******************************************************************************
 * globals
******************************************************************************/
gboolean firstsearch = TRUE;	/* search dialog is not running when TRUE */
gboolean firstLE = TRUE;	/* ListEditor in not running when TRUE */
GtkWidget *searchDlg;		/* pointer to search dialog */
guint num1, num2, num3;
gboolean texttabs,
	dicttabs,
	comtabs,
	bar,
	applycolor = FALSE,
	showtextgroup,
	showcomgroup,
	showdictgroup,
	showhistorygroup;
GtkWidget *listeditor;		/* pointer to ListEditor */
gchar *tmpcolor;
/******************************************************************************
 * externals
******************************************************************************/
extern gchar current_verse[80];	
extern SETTINGS *settings;	/* pointer to settings structure - (declared in gs_gnomesword.c) */
extern gboolean ApplyChange;	/* to keep form looping when book combobox is changed */
extern GtkWidget *MainFrm;	/* GnomeSword widget (gnome app)(declared and set in gs_sword.cpp) */
extern GtkWidget *NEtext;
extern GtkWidget *strongsnum;	/* menu check item (declared in gs_gnomesword.c) */
extern GtkWidget *shortcut_bar;
extern gchar 	*font_mainwindow,	
		*font_interlinear,		
		*font_currentverse;
extern gboolean noteModified;	/* personal comments window changed */
extern gint answer;		/* do we save file on exit */
extern gboolean isstrongs;	/* main window selection is not storngs number (gs_gnomsword.c) */
//extern gboolean isrunningSD;    /* is the view dictionary dialog runing */
extern GtkWidget *htmlCommentaries;
extern gchar *mycolor;
extern GString *gs_clipboard; /* declared in gs_gnomesword.c, freed in gs_sword.cpp */
extern GS_LAYOUT gslayout;
extern GS_TABS	*p_tabs;

/*********************************************************************************
**********************************************************************************
 *callbacks MainFrm
**********************************************************************************
*********************************************************************************/

/**********************************************************************************
 * main menu call backs
 * popup menu call backs are located in 
 **********************************************************************************/

/******************************************************************************
 *on_mnuHistoryitem1_activate - user chose an history item
******************************************************************************/
void
on_mnuHistoryitem1_activate(GtkMenuItem * menuitem, gpointer user_data)
{       	
	//g_warning("user data = %d\n", atoi((gchar *)user_data));
	changeverseHistory(atoi((gchar *)user_data));
}

//----------------------------------------------------------------------------------------------
void
on_strongs_numbers1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	/* we change the state of strongs toogle buttn to match the menu item this will 
	    activate on_btnStrongs_toogled  which will do the work */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (lookup_widget
				      (MainFrm, "btnStrongs")),
				     GTK_CHECK_MENU_ITEM(menuitem)->active);	/* set strongs toogle button */
}

//----------------------------------------------------------------------------------------------
void
on_morphs_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	morphsSWORD(GTK_CHECK_MENU_ITEM(menuitem)->active);	
}

//----------------------------------------------------------------------------------------------
void
on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
{
	GtkWidget *dlg, *text1, *text2, *label;

	dlg = create_AboutSword();
	text1 = lookup_widget(dlg, "txtAboutSword");
	text2 = lookup_widget(dlg, "text6");
	label = lookup_widget(dlg, "label96");
	gtk_text_set_word_wrap(GTK_TEXT(text1), TRUE);
	gtk_text_set_word_wrap(GTK_TEXT(text2), TRUE);
	showinfoSWORD(text2, GTK_LABEL(label));
	gtk_widget_show(dlg);
}


//----------------------------------------------------------------------------------------------
void on_footnotes1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		footnotesSWORD(TRUE);
	else
		footnotesSWORD(FALSE);
}


//----------------------------------------------------------------------------------------------
void on_copy3_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(MainFrm, (gchar *) user_data);	
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_preferences1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	openpropertiesbox();
	texttabs = p_tabs->textwindow;
	comtabs = p_tabs->commwindow;
	dicttabs = p_tabs->dictwindow;
	bar = settings->showshortcutbar;
	showtextgroup = settings->showtextgroup;
	showcomgroup = settings->showcomgroup;
	showdictgroup = settings->showdictgroup;
}


//----------------------------------------------------------------------------------------------
void on_add_bookmark1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	addBookmark(MainFrm);
}

//----------------------------------------------------------------------------------------------
void
on_edit_bookmarks1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (firstLE) {
		listeditor = createListEditor();
		editbookmarks(listeditor);
		firstLE = FALSE;
	}
	gtk_widget_show(listeditor);
}
//----------------------------------------------------------------------------------------------
void on_search1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (firstsearch) {
		searchDlg = createSearchDlgSWORD();
		firstsearch = FALSE;
	}
	gtk_widget_show(searchDlg);
}
//----------------------------------------------------------------------------------------------
void
on_auto_save_notes1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	setautosave(GTK_CHECK_MENU_ITEM(menuitem)->active);
}


//----------------------------------------------------------------------------------------------
void on_verse_style1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		setversestyleSWORD(TRUE);
	else
		setversestyleSWORD(FALSE);
}

//----------------------------------------------------------------------------------------------
void
on_show_interlinear_page1_activate(GtkMenuItem * menuitem,
				   gpointer user_data)
{

	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		showIntPage(MainFrm, TRUE);
	else
		showIntPage(MainFrm, FALSE);
}

//----------------------------------------------------------------------------------------------
void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	shutdownSWORD();
}

//----------------------------------------------------------------------------------------------
void on_clear1_activate(GtkMenuItem * menuitem,	//-- clear history menu
			gpointer user_data)
{
	clearhistory(MainFrm,GTK_WIDGET(shortcut_bar));
}

//----------------------------------------------------------------------------------------------
void
on_about_gnomesword1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *AboutBox;

	AboutBox = create_about2();
	gtk_widget_show(AboutBox);
}

//----------------------------------------------------------------------------------------------
void on_com_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gchar *modNum;

	modNum = (gchar *) user_data;
	notebook = lookup_widget(MainFrm, "notebook1");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), atoi(modNum));	//-- set notebook page
}

//----------------------------------------------------------------------------------------------
void on_dict_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gchar *modNum;

	modNum = (gchar *) user_data;
	notebook = lookup_widget(MainFrm, "notebook4");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), atoi(modNum));	//-- set notebook page
}



//----------------------------------------------------------------------------------------------
void
on_btnStrongs_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{

	GTK_CHECK_MENU_ITEM(strongsnum)->active = togglebutton->active;	//-- change menu check item to match button
	if (togglebutton->active)
		strongsSWORD(TRUE);	//-- trun strongs numbers on (GnomeSword.cpp)
	else
		strongsSWORD(FALSE);	//-- trun strongs numbers off (GnomeSword.cpp)
}

//----------------------------------------------------------------------------------------------
void on_mainwindow_destroy(GtkObject * object, gpointer user_data)
{
	shutdownSWORD();
}


//----------------------------------------------------------------------------------------------
void on_btnSearch_clicked(GtkButton * button, gpointer user_data)
{
	if (firstsearch) {
		searchDlg = createSearchDlgSWORD();
	}
	gtk_widget_show(searchDlg);
}


//----------------------------------------------------------------------------------------------
void on_cbeBook_changed(GtkEditable * editable, gpointer user_data)
{
	gchar *bookname, ref[255];

	if (ApplyChange) {
		bookname =
		    gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget
					(GTK_WIDGET(editable),
					 "cbeBook")));
		sprintf(ref, "%s 1:1", bookname);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON
					  (lookup_widget
					   (GTK_WIDGET(editable),
					    "spbChapter")), 1);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON
					  (lookup_widget
					   (GTK_WIDGET(editable),
					    "spbVerse")), 1);
		gtk_entry_set_text(GTK_ENTRY
				   (lookup_widget
				    (GTK_WIDGET(editable),
				     "cbeFreeformLookup")), ref);
		changeVerseSWORD(ref);
	}
}

//----------------------------------------------------------------------------------------------
gboolean
on_spbChapter_button_release_event(GtkWidget * widget,
				   GdkEventButton * event,
				   gpointer user_data)
{
	chapterSWORD();
	return TRUE;
}

//----------------------------------------------------------------------------------------------
gboolean
on_spbVerse_button_release_event(GtkWidget * widget,
				 GdkEventButton * event,
				 gpointer user_data)
{
	verseSWORD();
	return TRUE;
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
gboolean
on_moduleText_button_press_event(GtkWidget * widget,
				 GdkEventButton * event,
				 gpointer user_data)
{
	gint versenum;		//-- new verse number
	//gchar buf[80];

	isstrongs = FALSE;
	if (event->button == 1){  //-- some one pressed mouse button one
		if (!GTK_EDITABLE(widget)->has_selection)
			return FALSE;	//-- we do not have a selection
		versenum = getversenumber(widget);	//-- get the new verse number
		if (versenum > 0){	//-- if not a number stop		
			if (isstrongs){	//-- if we have a storngs number look it up
				lookupStrongsSWORD(versenum);
				isstrongs = FALSE;
			} else {
				gtk_spin_button_set_value(GTK_SPIN_BUTTON
							  (lookup_widget
							   (MainFrm,
							    "spbVerse")),
							    versenum);	//-- set verse spin button to new number
				verseSWORD();	//-- change modules to the new verse
			}
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------
gboolean
on_textDict_button_press_event (GtkWidget *widget,
                                 GdkEventButton * event,
                                 gpointer user_data)
{
        gint versenum;          //-- new verse number
        //gchar buf[80];

        isstrongs = FALSE;
        if (event->button == 1){  //-- some one pressed mouse button one
                if (!GTK_EDITABLE(widget)->has_selection)
                        return FALSE;   //-- we do not have a selection
                versenum = getdictnumber(widget);      //-- get the new verse number
                if (versenum > 0){      //-- if not a number stop
                        if (isstrongs){ //-- if we have a storngs number look it up
                                lookupStrongsSWORD(versenum);
                                isstrongs = FALSE;
                        }
                }
        }
        return TRUE;
}
//----------------------------------------------------------------------------------------------
void /* commentary notebook page changed */
on_nbTextMods_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	GtkLabel *label;	//-- pointer to page label
	static gboolean firsttime = TRUE;
	if (!firsttime) {
		label = (GtkLabel *) page->tab_label;	//-- get label
		nbchangecurModSWORD((char *) label->label, page_num, TRUE);	//-- pass label text and page number
	}								//-- to function to do the work - gs_sword.cpp
	firsttime = FALSE;
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
		changcurcomModSWORD((char *) label->label, page_num, TRUE);	//-- pass label text and page number
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
	editnoteSWORD(GTK_TOGGLE_BUTTON(togglebutton)->active);
}

//----------------------------------------------------------------------------------------------
void on_btnSaveNote_clicked(GtkButton * button, gpointer user_data)
{
	if (GTK_TOGGLE_BUTTON
	    (lookup_widget(GTK_WIDGET(button), "btnEditNote"))->active) {
		savenoteSWORD(TRUE);
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
		changcurdictModSWORD((char *) label->label, keyText, page_num);	/* sent to changcurdictModSWORD() function in
															 GnomeSword.cpp */
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
void on_btbSpellOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *spell;

	spell = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(spell);
}


//----------------------------------------------------------------------------------------------
void on_btnSpellCancel_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *spell;

	spell = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(spell);
}
//----------------------------------------------------------------------------------------------
void
on_fpMainwindow_font_set(GnomeFontPicker * gnomefontpicker,
			 GString arg1, gpointer user_data)
{
	font_mainwindow = arg1.str;
}

//----------------------------------------------------------------------------------------------
void
on_fbCurrentverse_font_set(GnomeFontPicker * gnomefontpicker,
			   GString arg1, gpointer user_data)
{
	font_currentverse = arg1.str;
}

//----------------------------------------------------------------------------------------------
void
on_fbInerlinear_font_set(GnomeFontPicker * gnomefontpicker,
			 GString arg1, gpointer user_data)
{
	font_interlinear = arg1.str;
}

//----------------------------------------------------------------------------------------------
void
on_cpfgCurrentverse_color_set(GnomeColorPicker * gnomecolorpicker,
			      guint arg1,
			      guint arg2,
			      guint arg3, guint arg4, gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;
	gdouble colo[4];
        //gchar *colour;

	num1 = arg1;
	num2 = arg2;
	num3 = arg3;
	
	gnome_color_picker_get_d(gnomecolorpicker, &colo[0] , &colo[1], &colo[2], &colo[3]);
        tmpcolor = gdouble_arr_to_hex(colo, 1);
        //g_warning(colour);
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(gnomecolorpicker));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	applycolor = TRUE;
	gtk_widget_set_sensitive(btnok, TRUE);
	gtk_widget_set_sensitive(btnapply, TRUE);
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
on_fpStudypad_font_set(GnomeFontPicker * gnomefontpicker,
		       GString arg1, gpointer user_data)
{
	//arg1.str;
	gnome_property_box_changed(GNOME_PROPERTY_BOX
				   (gtk_widget_get_toplevel(GTK_WIDGET
							    (gnomefontpicker))));
}

//----------------------------------------------------------------------------------------------
void
on_notebook3_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	static gboolean firsttime = TRUE;	//-- dont do anything if this is the first time here, but remember we were here - set firsttime to FALSE
	if (!firsttime) {
		changepagenotebook(notebook, page_num);	//-- send to changepagenotebook() function in GnomeSword.cpp
	}
	firsttime = FALSE;	//-- remember we were here
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
void on_btnPropertyboxOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	if (applycolor){
		mycolor = tmpcolor;
        	strcpy(settings->currentverse_color,mycolor);        	
		setcurrentversecolor(num1, num2, num3); /* if color has changed  */
		//g_free(tmpcolor);
	}
	applycolor = FALSE;
	setformatoption(lookup_widget(GTK_WIDGET(button), "cbtnPNformat"));
	applyoptions(MainFrm, bar, texttabs, comtabs, dicttabs, showtextgroup, showcomgroup,
		     showdictgroup, showhistorygroup);
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void on_btnPropertyboxApply_clicked(GtkButton * button, gpointer user_data)
{
	if (applycolor){
		mycolor = tmpcolor;		
        	strcpy(settings->currentverse_color,mycolor);
		setcurrentversecolor(num1, num2, num3);	/* if color has changed */
		//g_free(tmpcolor);
	}
	applycolor = FALSE;
	setformatoption(lookup_widget(GTK_WIDGET(button), "cbtnPNformat"));
	applyoptions(MainFrm,bar, texttabs, comtabs, dicttabs, showtextgroup, showcomgroup,
		     showdictgroup, showhistorygroup);
}

//----------------------------------------------------------------------------------------------
void
on_btnPropertyboxCancel_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}




//==============================================================================================



//----------------------------------------------------------------------------------------------
void
on_tbtnFollow_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
void
on_cbtnPNformat_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(togglebutton));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, TRUE);
	gtk_widget_set_sensitive(btnapply, TRUE);
}

//----------------------------------------------------------------------------------------------
gboolean
on_cbeFreeformLookup_drag_drop(GtkWidget * widget,
			       GdkDragContext * drag_context,
			       gint x,
			       gint y, guint time, gpointer user_data)
{

	return FALSE;
}

//----------------------------------------------------------------------------------------------
void
on_moduleText_drag_begin(GtkWidget * widget,
			 GdkDragContext * drag_context, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_textCommentaries_drag_begin(GtkWidget * widget,
			       GdkDragContext * drag_context,
			       gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_textDict_drag_begin(GtkWidget * widget,
		       GdkDragContext * drag_context, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_textComments_drag_begin(GtkWidget * widget,
			   GdkDragContext * drag_context,
			   gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_textComments_drag_data_get(GtkWidget * widget,
			      GdkDragContext * drag_context,
			      GtkSelectionData * data,
			      guint info, guint time, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_textComments_drag_data_received(GtkWidget * widget,
				   GdkDragContext * drag_context,
				   gint x,
				   gint y,
				   GtkSelectionData * data,
				   guint info,
				   guint time, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
gboolean
on_textComments_drag_drop(GtkWidget * widget,
			  GdkDragContext * drag_context,
			  gint x, gint y, guint time, gpointer user_data)
{

	return FALSE;
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
gboolean
on_moduleText_enter_notify_event(GtkWidget * widget,
				 GdkEventCrossing * event,
				 gpointer user_data)
{

	return FALSE;
}
//----------------------------------------------------------------------------------------------
void on_btnBack_clicked(GtkButton * button, gpointer user_data)
{
	historynav(MainFrm, 0);
}

//----------------------------------------------------------------------------------------------
void on_btnFoward_clicked(GtkButton * button, gpointer user_data)
{
	historynav(MainFrm, 1);
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
void
on_cbtnShowSCB_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(togglebutton));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, TRUE);
	gtk_widget_set_sensitive(btnapply, TRUE);
	bar = togglebutton->active;
}

//----------------------------------------------------------------------------------------------
void
on_cbtnShowTXtabs_toggled(GtkToggleButton * togglebutton,
			  gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(togglebutton));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, TRUE);
	gtk_widget_set_sensitive(btnapply, TRUE);
	texttabs = togglebutton->active;
}

//----------------------------------------------------------------------------------------------
void
on_cbtnShowCOMtabs_toggled(GtkToggleButton * togglebutton,
			   gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(togglebutton));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, TRUE);
	gtk_widget_set_sensitive(btnapply, TRUE);
	comtabs = togglebutton->active;
}

//----------------------------------------------------------------------------------------------
void
on_cbtnShowDLtabs_toggled(GtkToggleButton * togglebutton,
			  gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(togglebutton));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, TRUE);
	gtk_widget_set_sensitive(btnapply, TRUE);
	dicttabs = togglebutton->active;
}


//----------------------------------------------------------------------------------------------
void
on_cbtnShowTextgroup_toggled(GtkToggleButton * togglebutton,
			     gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(togglebutton));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, TRUE);
	gtk_widget_set_sensitive(btnapply, TRUE);
	showtextgroup = togglebutton->active;
}

//----------------------------------------------------------------------------------------------
void
on_cbtnShowComGroup_toggled(GtkToggleButton * togglebutton,
			    gpointer user_data)
{
    GtkWidget	*dlg,
				*btnok,
				*btnapply;
							
	dlg = gtk_widget_get_toplevel (GTK_WIDGET (togglebutton));
	btnok = lookup_widget(dlg,"btnPropertyboxOK");
	btnapply = lookup_widget(dlg,"btnPropertyboxApply");
	gtk_widget_set_sensitive (btnok, TRUE);
	gtk_widget_set_sensitive (btnapply, TRUE);
	showcomgroup = togglebutton->active;
}

//----------------------------------------------------------------------------------------------
void
on_cbtnShowDictGroup_toggled(GtkToggleButton * togglebutton,
			     gpointer user_data)
{
    	GtkWidget	*dlg,
				*btnok,
				*btnapply;
							
	dlg = gtk_widget_get_toplevel (GTK_WIDGET (togglebutton));
	btnok = lookup_widget(dlg,"btnPropertyboxOK");
	btnapply = lookup_widget(dlg,"btnPropertyboxApply");
	gtk_widget_set_sensitive (btnok, TRUE);
	gtk_widget_set_sensitive (btnapply, TRUE);
	showdictgroup = togglebutton->active;
}

//----------------------------------------------------------------------------------------------
#if USE_SHORTCUTBAR
gboolean
on_epaned_button_release_event(GtkWidget       *widget,
                               GdkEventButton  *event,
                               gpointer         user_data)
{
        gint panesize;
	panesize = e_paned_get_position(E_PANED(lookup_widget(MainFrm,(gchar *)user_data)));
	
        if(panesize > 15 )
        {	if(!strcmp((gchar *)user_data,"epaned"))
        		gslayout.shortcutbar_width = panesize; 
		if(!strcmp((gchar *)user_data,"vpaned1"))
        		gslayout.upperpane_hight = panesize; 
		if(!strcmp((gchar *)user_data,"hpaned1"))
        		gslayout.biblepane_width = panesize; 
        }
        return TRUE;
}
#endif /* USE_SHORTCUTBAR */
//----------------------------------------------------------------------------------------------
void
on_cbtnShowHistoryGroup_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    	GtkWidget *dlg,
		  *btnok,
		  *btnapply;
							
	dlg = gtk_widget_get_toplevel (GTK_WIDGET (togglebutton));
	btnok = lookup_widget(dlg,"btnPropertyboxOK");
	btnapply = lookup_widget(dlg,"btnPropertyboxApply");
	gtk_widget_set_sensitive (btnok, TRUE);
	gtk_widget_set_sensitive (btnapply, TRUE);
	showhistorygroup = togglebutton->active;
}


//----------------------------------------------------------------------------------------------
void on_listeditor_destroy(GtkObject * object, gpointer user_data)
{
	firstLE = TRUE;
	destroyListEditorSWORD();
}

