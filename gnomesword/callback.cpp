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
#include "noteeditor.h"
#include "searchstuff.h"
#include "printstuff.h"


/******************************************************************************
 * globals
******************************************************************************/
gboolean firstsearch = TRUE;	/* search dialog is not running when TRUE */
gboolean firstLE = TRUE;	/* ListEditor in not running when TRUE */
GtkWidget *searchDlg;		/* pointer to search dialog */
guint num1, num2, num3;
gboolean dicttabs,
	comtabs,
	bar,
	applycolor = FALSE,
	showtextgroup,
	showcomgroup,
	showdictgroup,
	showhistorygroup;
GtkWidget *listeditor;		/* pointer to ListEditor */

/******************************************************************************
 * externals
******************************************************************************/
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
extern NoteEditor *noteeditor;
extern gboolean autoscroll;
extern gboolean isstrongs;	/* main window selection is not storngs number (gs_gnomsword.c) */
extern gboolean isrunningSD;    /* is the view dictionary dialog runing */
extern GtkWidget *htmlCommentaries;
/******************************************************************************
*******************************************************************************
 *callbacks fileselection dialogs
*******************************************************************************
******************************************************************************/

/******************************************************************************
 *on_ok_button1_clicked - fileselection dialog
******************************************************************************/
void on_ok_button1_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	loadFile(filesel);
	gtk_widget_destroy(filesel);
}


//----------------------------------------------
void on_cancel_button1_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

//----------------------------------------------
void on_ok_button2_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *filesel;
	gchar filename[255];

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	sprintf(filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION
						(filesel)));
	gtk_widget_destroy(filesel);
	saveFile(filename);
}

 //------------------------------------------
 //-- save verse list - fileselection dialog ok button clicke
 //-- user data - the list widget dialog ok button clicked
void on_ok_button4_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *filesel;	//-- pointer to fileselection dialog
	gchar filename[255];	//-- string to store filename from fileselection dialog

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));	//-- get fileselection dialog
	sprintf(filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel)));	//-- get filename
	gtk_widget_destroy(filesel);	//-- destroy fileselection dialog
	savelist(filename, (GtkWidget *) user_data);	//-- send filename and list widget to savelist function (gs_file.c)
}

/* fileselection dialog cancel button clicked */
void
on_cancel_button2_clicked(GtkButton * button, gpointer user_data)
{	//--destroy fileselection dialog
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


/*********************************************************************************
**********************************************************************************
 *callbacks About dialogs
**********************************************************************************
*********************************************************************************/

//-------------------------------------------------
void on_btnAboutModuleOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

//------------------------------------------------
void on_btnAboutSwordOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}


/*********************************************************************************
**********************************************************************************
 *callbacks MainFrm
**********************************************************************************
*********************************************************************************/

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
void on_mainwindow_destroy(GtkObject * object, gpointer user_data)
{
	shutdownSWORD();
}


//----------------------------------------------------------------------------------------------
void on_btnSearch_clicked(GtkButton * button, gpointer user_data)
{
	if (firstsearch) {
		searchDlg = create_dlgSearch();
		searchWindow->initsearchWindow(searchDlg);
		firstsearch = FALSE;
		gtk_signal_connect(GTK_OBJECT(searchDlg), "destroy",
				   GTK_SIGNAL_FUNC(on_dlgSearch_destroy),
				   NULL);
	}
	gtk_widget_show(searchDlg);
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
void on_john_3_1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	changeVerseSWORD((gchar *) user_data);
}

//----------------------------------------------------------------------------------------------
void on_mainText_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *modName;

	modName = (gchar *) user_data;
	changecurModSWORD(modName,TRUE);
}

//----------------------------------------------------------------------------------------------
void
on_1st_interlinear_window1_activate(GtkMenuItem * menuitem,
				    gpointer user_data)
{
	gchar *modName;

	modName = (gchar *) user_data;
	changecomp1ModSWORD(modName);
}

//----------------------------------------------------------------------------------------------
void
on_2nd_interlinear_window1_activate(GtkMenuItem * menuitem,
				    gpointer user_data)
{
	gchar *modName;

	modName = (gchar *) user_data;
	changecomp2ModSWORD(modName);
}

//----------------------------------------------------------------------------------------------
void
on_3rd_interlinear_window1_activate(GtkMenuItem * menuitem,
				    gpointer user_data)
{
	gchar *modName;

	modName = (gchar *) user_data;
	changecomp3ModSWORD(modName);
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
					    "spbChapter")), gint(1));
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
	static gboolean firsttime = TRUE;	//-- dont do anything if this is the first time here, but remember we were here - set firsttime to FALSE
	if (!firsttime) {
		label = (GtkLabel *) page->tab_label;	//-- set label to tab label
		entryText =
		    gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget
					(GTK_WIDGET(notebook), "dictionarySearchText")));	//-- get key from entry
		if (!strcmp(label->label, "Torrey"))
			entryText = "";	//-- if Torrey module then set text to null else segfault?
		keyText = entryText;	//-- put entryText into string keyText
		changcurdictModSWORD((char *) label->label, keyText, page_num);	//-- sent to changcurdictModSWORD() function in GnomeSword.cpp
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
void on_btnSearchOK_clicked(GtkButton * button, gpointer user_data)
{

	gtk_widget_hide(searchDlg);
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
void
on_strongs_numbers1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	//-- we change the state of strongs toogle buttn to match the menu item this will activate on_btnStrongs_toogled
	//-- which will do the work
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (lookup_widget
				      (MainFrm, "btnStrongs")),
				     GTK_CHECK_MENU_ITEM(menuitem)->active);	//-- set strongs toogle button
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
	/* g_warning((gchar *) user_data);
	if(!strcmp((gchar *) user_data,"htmlCommentaries")) gtk_html_copy(GTK_HTML(htmlCommentaries));
	else */
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_preferences1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	openpropertiesbox();
	comtabs = settings->showcomtabs;
	dicttabs = settings->showdicttabs;
	bar = settings->showshortcutbar;
	showtextgroup = settings->showtextgroup;
	showcomgroup = settings->showcomgroup;
	showdictgroup = settings->showdictgroup;
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

	num1 = arg1;
	num2 = arg2;
	num3 = arg3;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(gnomecolorpicker));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	applycolor = TRUE;
	gtk_widget_set_sensitive(btnok, TRUE);
	gtk_widget_set_sensitive(btnapply, TRUE);
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
/*
//----------------------------------------------------------------------------------------------
void on_btnSpellNotes_clicked(GtkButton * button, gpointer user_data)
{
#ifdef USE_ASPELL
	GtkWidget *speller;
	if (GTK_TOGGLE_BUTTON
	    (lookup_widget(GTK_WIDGET(button), "btnEditNote"))->active) {};
		//speller = spellcheck(1);
#endif				
}
*/
//----------------------------------------------------------------------------------------------
void
on_auto_save_notes1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	setautosave(GTK_CHECK_MENU_ITEM(menuitem)->active);
}

//----------------------------------------------------------------------------------------------
void on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	if (GTK_TOGGLE_BUTTON(lookup_widget(MainFrm, "btnEditNote"))->
	    active) {
		text = lookup_widget(MainFrm, "textComments");
		gtk_editable_cut_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
	}
}

//----------------------------------------------------------------------------------------------
void on_copy4_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(MainFrm, "textComments");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;

	text = lookup_widget(MainFrm, "textComments");
	gtk_editable_paste_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}


//----------------------------------------------------------------------------------------------
void on_change_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	changepercomModSWORD((gchar *) user_data);
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
void on_search1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (firstsearch) {
		searchDlg = create_dlgSearch();
		searchWindow->initsearchWindow(searchDlg);
		firstsearch = FALSE;
		gtk_signal_connect(GTK_OBJECT(searchDlg), "destroy",
				   GTK_SIGNAL_FUNC(on_dlgSearch_destroy),
				   NULL);
	}
	gtk_widget_show(searchDlg);
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
void on_btnSearchSaveList_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list,	//-- pointer to resultList
	*savedlg,		//-- pointer to save file dialog
	*ok_button2;		//-- pointer to fileselection ok button
	list = lookup_widget(GTK_WIDGET(button), "resultList");	//-- set list to resultlist in search dialog
	savedlg = create_fileselectionSave();
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(savedlg),
					"/home/tb/BibleStudy/");
	ok_button2 = lookup_widget(savedlg, "ok_button2");
	gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button4_clicked), list);
	gtk_widget_show(savedlg);
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
void on_kjv1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD((char *) user_data);
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(0));
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(3));
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module3_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(4));
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module4_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(5));
}

//----------------------------------------------------------------------------------------------
void on_btnPropertyboxOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	if (applycolor)
		setcurrentversecolor(num1, num2, num3);
	applycolor = FALSE;

	setformatoption(lookup_widget(GTK_WIDGET(button), "cbtnPNformat"));
	applyoptions(MainFrm, bar, comtabs, dicttabs, showtextgroup, showcomgroup,
		     showdictgroup, showhistorygroup);
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void on_btnPropertyboxApply_clicked(GtkButton * button, gpointer user_data)
{
	if (applycolor)
		setcurrentversecolor(num1, num2, num3);	//-- if color has changed
	applycolor = FALSE;
	setformatoption(lookup_widget(GTK_WIDGET(button), "cbtnPNformat"));
	applyoptions(MainFrm,bar, comtabs, dicttabs, showtextgroup, showcomgroup,
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
void on_boldNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?
	noteModified = noteeditor->setBOLD(NEtext);	//-- noteeditor.cpp
}

//----------------------------------------------------------------------------------------------
void on_italicNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?
	noteModified = noteeditor->setITALIC(NEtext);	//-- noteeditor.cpp
}

//----------------------------------------------------------------------------------------------
void on_referenceNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?
	noteModified = noteeditor->setREFERENCE(NEtext);	//-- noteeditor.cpp
}

//----------------------------------------------------------------------------------------------
void on_underlineNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?
	noteModified = noteeditor->setUNDERLINE(NEtext);	//-- noteeditor.cpp
}

//----------------------------------------------------------------------------------------------
void on_greekNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?
	noteModified = noteeditor->setGREEK(NEtext);	//-- noteeditor.cpp
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
	changeVerseSWORD(buf);
		
}

//----------------------------------------------------------------------------------------------
void
on_goto_reference2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	gchar *buf;

	text = lookup_widget(MainFrm, "textCommentaries");
	if (!GTK_EDITABLE(text)->has_selection)
		return;		//-- do we have a selection?
	buf =
	    gtk_editable_get_chars(GTK_EDITABLE(text),
				   GTK_EDITABLE(text)->selection_start_pos,
				   GTK_EDITABLE(text)->selection_end_pos);

	changeVerseSWORD(buf);
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
	gchar *buf;
	static gboolean needsecond = FALSE;

	if (!settings->formatpercom){	//-- do we want formatting?	
		noteModified = TRUE;	//-- note has been modified to get us here
		return FALSE;
	}
	if (event->keyval == 65293 || event->keyval == 65421){	//-- return key	
		noteModified = noteeditor->setNEWLINE(NEtext);	//-- noteeditor.cpp
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
			noteModified = noteeditor->setBOLD(NEtext);	//-- noteeditor.cpp
			break;
		case 105:	//-- italics
			noteModified = noteeditor->setITALIC(NEtext);	//-- noteeditor.cpp
			break;
		case 114:	//-- reference
			noteModified = noteeditor->setREFERENCE(NEtext);	//-- noteeditor.cpp
			break;
		case 108:	//-- underline -- broken using l not u - u removes line
			noteModified = noteeditor->setUNDERLINE(NEtext);	//-- noteeditor.cpp
			break;
		case 103:	//-- greek font
			noteModified = noteeditor->setGREEK(NEtext);	//-- noteeditor.cpp
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
void
on_about_this_module5_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(2));
}

//----------------------------------------------------------------------------------------------
void on_lookup_word1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(MainFrm, "dictionarySearchText");
	text = lookup_widget(MainFrm, "textDict");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
		dictSearchTextChangedSWORD(buf);
	}
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
void
on_lookup_selection_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(MainFrm, "dictionarySearchText");
	text = lookup_widget(MainFrm, "moduleText");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		dictSearchTextChangedSWORD(buf);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
	}
}

//----------------------------------------------------------------------------------------------
void
on_lookup_selection2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(MainFrm, "dictionarySearchText");
	text = lookup_widget(MainFrm, "textCommentaries");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
		dictSearchTextChangedSWORD(buf);
	}
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module6_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(1));
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
void on_auto_scroll1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *toolbar;

	toolbar = lookup_widget(MainFrm, "handlebox17");

	autoscroll = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (autoscroll)
		gtk_widget_hide(toolbar);
	else
		gtk_widget_show(toolbar);

}

//----------------------------------------------------------------------------------------------
void
on_lookup_selection4_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(MainFrm, "dictionarySearchText");
	text = lookup_widget(MainFrm, "textComments");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
		dictSearchTextChangedSWORD(buf);
	}
}

//----------------------------------------------------------------------------------------------
void
on_goto_reference3_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	gchar *buf;

	text = lookup_widget(MainFrm, "textDict");
	if (!GTK_EDITABLE(text)->has_selection)
		return;		//-- do we have a selection?
	buf =
	    gtk_editable_get_chars(GTK_EDITABLE(text),
				   GTK_EDITABLE(text)->selection_start_pos,
				   GTK_EDITABLE(text)->selection_end_pos);
	
	changeVerseSWORD(buf);
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
//-- show hide commentary notebook tabs - popup menu choice
on_show_tabs1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		gtk_widget_show(lookup_widget(MainFrm, "notebook1"));
	else
		gtk_widget_hide(lookup_widget(MainFrm, "notebook1"));
}

//----------------------------------------------------------------------------------------------
void
//-- show hide dict/lex notebook tabs - popup menu choice
on_show_tabs2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		gtk_widget_show(lookup_widget(MainFrm, "notebook4"));
	else
		gtk_widget_hide(lookup_widget(MainFrm, "notebook4"));
}

//----------------------------------------------------------------------------------------------
void on_btnSB_clicked(GtkButton * button, gpointer user_data)
{
#if USE_SHORTCUTBAR
	if (settings->showshortcutbar) {
		settings->showshortcutbar = FALSE;
		e_paned_set_position (E_PANED(lookup_widget(MainFrm,"epaned")), 0);
	} else {
		settings->showshortcutbar = TRUE;
		e_paned_set_position (E_PANED(lookup_widget(MainFrm,"epaned")), settings->shortcutbarsize);
	}
#else
        if (settings->showshortcutbar) {
		settings->showshortcutbar = false;
		gtk_paned_set_position(GTK_PANED(lookup_widget(MainFrm,"hpaned2")), 0);
	} else {
		settings->showshortcutbar = true;
		gtk_paned_set_position(GTK_PANED(lookup_widget(MainFrm,"hpaned2")), 106); //settings->shortcutbarsize);
	} 	
#endif /* USE_SHORTCUTBAR */
}

//----------------------------------------------------------------------------------------------
#if USE_SHORTCUTBAR
void
on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
			      GdkEvent * event,
			      gint group_num, gint item_num)
{
	sbchangeModSword(MainFrm, GTK_WIDGET(shortcut_bar), group_num, item_num);
}
#endif /* USE_SHORTCUTBAR */
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

        panesize = e_paned_get_position(E_PANED(lookup_widget(MainFrm,"epaned")));
        if(panesize > 15 )
        {
        	settings->shortcutbarsize = panesize;        	
        }
        return TRUE;
}
#else
/*******************************************************************************
 *
 *******************************************************************************/
void
on_btsText_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sidebar;
	
	sidebar = lookup_widget(GTK_WIDGET(button), "nbSidebar");
	gtk_notebook_set_page(GTK_NOTEBOOK(sidebar),0);
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsComms"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsDicts"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsBookmarks"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsHistory"));
	if(settings->showcomgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsComms2"));
	if(settings->showdictgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsDicts2"));
	if(settings->showbookmarksgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsBookmarks2"));
	if(settings->showhistorygroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsHistory2"));
	
	
	
}


void
on_btsComms_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sidebar;
	
	sidebar = lookup_widget(GTK_WIDGET(button), "nbSidebar");
	gtk_notebook_set_page(GTK_NOTEBOOK(sidebar),1);
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsDicts"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsBookmarks"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsHistory"));
	if(settings->showdictgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsDicts2"));
	if(settings->showbookmarksgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsBookmarks2"));
	if(settings->showhistorygroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsHistory2"));	
}


void
on_btsDicts_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sidebar;
	
	sidebar = lookup_widget(GTK_WIDGET(button), "nbSidebar");
	gtk_notebook_set_page(GTK_NOTEBOOK(sidebar),2);
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsBookmarks"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsHistory"));
	if(settings->showbookmarksgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsBookmarks2"));
	if(settings->showhistorygroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsHistory2"));	
}


void
on_btsBookmarks_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sidebar;
	
	sidebar = lookup_widget(GTK_WIDGET(button), "nbSidebar");
	gtk_notebook_set_page(GTK_NOTEBOOK(sidebar),3);
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsHistory"));
	if(settings->showhistorygroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsHistory2"));
}


void
on_btsHistory_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sidebar;
	
	sidebar = lookup_widget(GTK_WIDGET(button), "nbSidebar");
	gtk_notebook_set_page(GTK_NOTEBOOK(sidebar),4);
}
/*
 *
*/
void
on_btnClearHistory_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	clearhistory(MainFrm,MainFrm);
}


void
on_btsComms2_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sidebar;
	
	sidebar = lookup_widget(GTK_WIDGET(button), "nbSidebar");
	gtk_notebook_set_page(GTK_NOTEBOOK(sidebar),1);
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsComms2"));
	gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsComms"));
}


void
on_btsDicts2_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sidebar;
	
	sidebar = lookup_widget(GTK_WIDGET(button), "nbSidebar");
	gtk_notebook_set_page(GTK_NOTEBOOK(sidebar),2);
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsComms2"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsDicts2"));
	if(settings->showcomgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsComms"));
	gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsDicts"));
}


void
on_btsBookmarks2_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sidebar;
	
	sidebar = lookup_widget(GTK_WIDGET(button), "nbSidebar");
	gtk_notebook_set_page(GTK_NOTEBOOK(sidebar),3);
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsComms2"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsDicts2"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsBookmarks2"));
	if(settings->showcomgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsComms"));
	if(settings->showdictgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsDicts"));
	gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsBookmarks"));
}


void
on_btsHistory2_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sidebar;
	
	sidebar = lookup_widget(GTK_WIDGET(button), "nbSidebar");
	gtk_notebook_set_page(GTK_NOTEBOOK(sidebar),4);
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsComms2"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsDicts2"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsBookmarks2"));
	gtk_widget_hide(lookup_widget(GTK_WIDGET(button),"btsHistory2"));
	
	if(settings->showcomgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsComms"));
	if(settings->showdictgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsDicts"));
	if(settings->showbookmarksgroup) gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsBookmarks"));
	gtk_widget_show(lookup_widget(GTK_WIDGET(button),"btsHistory"));
}
/*******************************************************************************
 *
 *******************************************************************************/
void
on_cbtnShowBookmarksGroup_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    GtkWidget	*dlg,
				*btnok,
				*btnapply;
							
	dlg = gtk_widget_get_toplevel (GTK_WIDGET (togglebutton));
	btnok = lookup_widget(dlg,"btnPropertyboxOK");
	btnapply = lookup_widget(dlg,"btnPropertyboxApply");
	gtk_widget_set_sensitive (btnok, true);
	gtk_widget_set_sensitive (btnapply, true);
	
//	showbookmarksgroup = togglebutton->active;
	
}

/*******************************************************************************
 *
 *******************************************************************************/
void
on_textbutton_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	changecurModSWORD((char *) user_data,TRUE);
}

/*******************************************************************************
 *
 *******************************************************************************/
void
on_combutton_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	sbchangeModSword(MainFrm,MainFrm,1,gint(user_data));
}

/*******************************************************************************
 *
 *******************************************************************************/
void
on_dictbutton_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	sbchangeModSword(MainFrm,MainFrm,2,gint(user_data));
}

/*******************************************************************************
 *
 *******************************************************************************/
void
on_historybutton_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	changeverseHistory((gint)user_data);
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

//----------------------------------------------------------------------------------------------
void on_dlgSearch_destroy(GtkObject * object, gpointer user_data)
{
	firstsearch = TRUE;
}

//----------------------------------------------------------------------------------------------
void on_btnSearch1_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *searchFrm;
	searchFrm = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_clist_clear(GTK_CLIST(searchWindow->resultList));
	searchWindow->searchSWORD(searchFrm);
}

//----------------------------------------------------------------------------------------------
void
on_resultList_select_row(GtkCList * clist,
			 gint row,
			 gint column, GdkEvent * event, gpointer user_data)
{
	GtkWidget *searchFrm;
	searchFrm = gtk_widget_get_toplevel(GTK_WIDGET(clist));
	searchWindow->resultsListSWORD(searchFrm, row, column);
}

/*******************************************************************************
 *
 *******************************************************************************/
void on_view_in_new_window_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	extern GtkWidget *frameShowDict;
	static GtkWidget *dlg;
	gchar *modName;
        GdkCursor *cursor;	
	gtk_widget_show(MainFrm);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(MainFrm->window,cursor);
	
	if(!isrunningSD) {
		dlg = create_dlgShowDict();
		modName = getdictmodSWORD();
		gtk_frame_set_label(GTK_FRAME(frameShowDict),modName);  /* set frame label to current Module name  */				
		initSD(modName);
		isrunningSD = true;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(MainFrm);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(MainFrm->window,cursor);
}		
		
		
