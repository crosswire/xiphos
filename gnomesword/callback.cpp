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
#include <swmodule.h>
#include <swmgr.h>
#include <versekey.h>
#include <regex.h>
#include  <widgets/shortcut-bar/e-shortcut-bar.h>
#include  <widgets/e-paned/e-paned.h>
#include  <widgets/e-paned/e-hpaned.h>
#include  <widgets/e-paned/e-vpaned.h>

#include "callback.h"
#include "GnomeSword.h"
#include "support.h"
#include "interface.h"
#include "filestuff.h"
#include "display.h"
#include "listeditor.h"
#include "noteeditor.h"
#include "searchstuff.h"

#ifdef  USE_GNOMEPRINT
#include "printstuff.h"
#endif				/* USE_GNOMEPRINT */

#ifdef USE_ASPELL
#include "spellcheck.h"
#endif				/* USE_ASPELL */

LISTITEM listitem;		//-- structure for ListEditor items (verse lists and bookmarks)
gint listrow;			//-- current row in ListEditor clist widget
gboolean firstsearch = true;	//-- search dialog is not running when true
gboolean firstLE = true;		//-- ListEditor in not running when true
GtkWidget *searchDlg;		//-- pointer to search dialog
GtkWidget *listeditor;		//-- pointer to ListEditor
extern SETTINGS *settings;	//-- pointer to settings structure - (declared in GnomeSword.cpp)
extern gboolean ApplyChange;	//-- to keep form looping when book combobox is changed - do we still need this ???
extern gboolean file_changed;	//-- ???
extern gchar *current_filename;	//-- file in studypad
extern GtkWidget *MainFrm;	//-- GnomeSword widget (gnome app)(declared and set in GnomeSword.cpp)
extern GtkWidget *NEtext;
extern GtkWidget *bookmark_mnu;	//-- ???
extern GtkWidget *strongsnum;	//-- menu check item (declared in GnomeSword.cpp)
extern GtkWidget *shortcut_bar;
extern gchar *font_mainwindow,	//--
*font_interlinear,		//--
*font_currentverse;		//--
extern GdkColor myGreen;	//-- current verse color for main text window - declared in display.cpp
extern gboolean noteModified;	//-- personal comments window changed

extern gboolean autoSave;	//-- auto save personal comments when verse changes -- declared in GnomeSword.cpp
extern gint answer;		//-- do we save file on exit
extern SWModule *curMod;	//-- module for main text window (GnomeSword.cpp)
extern SWModule *comp1Mod;	//-- module for first interlinear window (GnomeSword.cpp)
extern SWModule *comp2Mod;	//-- module for second interlinear window (GnomeSword.cpp)   
extern SWModule *comp3Mod;	//-- module for third interlinear window (GnomeSword.cpp)
extern SWModule *curcomMod;	//-- module for commentary text window (GnomeSword.cpp)
extern SWModule *curdictMod;	//-- module for dict/lex text window (GnomeSword.cpp)

extern gint ibookmarks;		//-- number of bookmark menu items
extern NoteEditor *noteeditor;
extern gboolean autoscroll;
extern gboolean isstrongs;	//-- main window selection is not storngs number (GnomeSword.cpp)
extern char *homedir;
guint num1, num2, num3;
gboolean 	buttonpressed = false;
gboolean 	dicttabs,
	comtabs,
	bar,
	applycolor = false,
	showtextgroup,
	showcomgroup,
	showdictgroup,
	showhistorygroup;

//-------------------------------------------------------------------------------------------
void
on_mnuHistoryitem1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *buf;
	buf = (gchar *) user_data;
	changeVerse(buf);
}

//----------------------------------------------------------------------------------------------
void on_ok_button1_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	loadFile(filesel);
	gtk_widget_destroy(filesel);
}


//----------------------------------------------------------------------------------------------
void on_cancel_button1_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

//----------------------------------------------------------------------------------------------
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

 //----------------------------------------------------------------------------------------------
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
	savelist(filename, (GtkWidget *) user_data);	//-- send filename and list widget to savelist function (filestuff.cpp)
}

//----------------------------------------------------------------------------------------------
void
//-- fileselection dialog cancel button clicked
on_cancel_button2_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));	//--destroy fileselection dialog
}

//----------------------------------------------------------------------------------------------
void on_btnAboutOK_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

//----------------------------------------------------------------------------------------------
void on_copy_verse1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(MainFrm, "moduleText");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_mainwindow_destroy(GtkObject * object, gpointer user_data)
{
	ShutItDown();
}

//----------------------------------------------------------------------------------------------
void on_listeditor_destroy(GtkObject * object, gpointer user_data)
{
	firstLE = true;
	destroyListEditor();
}

//----------------------------------------------------------------------------------------------
void on_dlgSearch_destroy(GtkObject * object, gpointer user_data)
{
	firstsearch = true;
}

//----------------------------------------------------------------------------------------------
void on_btnSearch1_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *searchFrm;
	searchFrm = gtk_widget_get_toplevel(GTK_WIDGET(button));
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

//----------------------------------------------------------------------------------------------
void on_btnSearch_clicked(GtkButton * button, gpointer user_data)
{
	if (firstsearch) {
		searchDlg = create_dlgSearch();
		searchWindow->initsearchWindow(searchDlg);
		firstsearch = false;
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
		strongsSWORD(true);	//-- trun strongs numbers on (GnomeSword.cpp)
	else
		strongsSWORD(false);	//-- trun strongs numbers off (GnomeSword.cpp)
}

//----------------------------------------------------------------------------------------------
void on_add_bookmark1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	addBookmark();
}

//----------------------------------------------------------------------------------------------
void
on_edit_bookmarks1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (firstLE) {
		listeditor = createListEditor();
		editbookmarks(listeditor);
		firstLE = false;
	}
	gtk_widget_show(listeditor);
}

//----------------------------------------------------------------------------------------------
void on_john_3_1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	changeVerse((gchar *) user_data);
}

//----------------------------------------------------------------------------------------------
void on_footnotes_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		footnotesSWORD(true);
	else
		footnotesSWORD(false);
}

//----------------------------------------------------------------------------------------------
void on_mainText_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *modName;

	modName = (gchar *) user_data;
	changecurModSWORD(modName);
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
		setversestyleSWORD(true);
	else
		setversestyleSWORD(false);
}

//----------------------------------------------------------------------------------------------
void
on_show_interlinear_page1_activate(GtkMenuItem * menuitem,
				   gpointer user_data)
{

	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		showIntPage(true);
	else
		showIntPage(false);
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
		changeVerse(ref);
	}
}

//----------------------------------------------------------------------------------------------
gboolean
on_spbChapter_button_release_event(GtkWidget * widget,
				   GdkEventButton * event,
				   gpointer user_data)
{
	chapterSWORD();
	return true;
}

//----------------------------------------------------------------------------------------------
gboolean
on_spbVerse_button_release_event(GtkWidget * widget,
				 GdkEventButton * event,
				 gpointer user_data)
{
	verseSWORD();
	return true;
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
	return true;
}


//----------------------------------------------------------------------------------------------
void on_btnExit_clicked(GtkButton * button, gpointer user_data)
{
	ShutItDown();
}

//----------------------------------------------------------------------------------------------
gboolean
on_moduleText_button_press_event(GtkWidget * widget,
				 GdkEventButton * event,
				 gpointer user_data)
{
	gint versenum;		//-- new verse number
	//gchar buf[80];

	isstrongs = false;
	if (event->button == 1){  //-- some one pressed mouse button one
		if (!GTK_EDITABLE(widget)->has_selection)
			return false;	//-- we do not have a selection
		versenum = getversenumber(widget);	//-- get the new verse number
		if (versenum > 0){	//-- if not a number stop		
			if (isstrongs){	//-- if we have a storngs number look it up
				lookupStrongsSWORD(versenum);
				isstrongs = false;
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
	return true;
}

//----------------------------------------------------------------------------------------------
gboolean
on_textDict_button_press_event (GtkWidget *widget,
                                 GdkEventButton * event,
                                 gpointer user_data)
{
        gint versenum;          //-- new verse number
        //gchar buf[80];

        isstrongs = false;
        if (event->button == 1){  //-- some one pressed mouse button one
                if (!GTK_EDITABLE(widget)->has_selection)
                        return false;   //-- we do not have a selection
                versenum = getdictnumber(widget);      //-- get the new verse number
                if (versenum > 0){      //-- if not a number stop
                        if (isstrongs){ //-- if we have a storngs number look it up
                                lookupStrongsSWORD(versenum);
                                isstrongs = false;
                        }
                }
        }
        return true;
}


//----------------------------------------------------------------------------------------------
void /* commentary notebook page changed */
on_notebook1_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	GtkLabel *label;	//-- pointer to page label
	static gboolean firsttime = true;
	if (!firsttime) {
		label = (GtkLabel *) page->tab_label;	//-- get label
		changcurcomModSWORD((char *) label->label, page_num);	//-- pass label text and page number 
	}								//-- to function to do the work - GnomeSword.cpp
	firsttime = false;
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

	return false;
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
		//saveChanges = true; //-- we know we want to save the changes - thats how we got here
		savenoteSWORD(noteModified);
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
gboolean
on_textComp1_button_press_event(GtkWidget * widget,
				GdkEventButton * event, gpointer user_data)
{

	return false;
}

//----------------------------------------------------------------------------------------------
gboolean
on_textComp2_button_press_event(GtkWidget * widget,
				GdkEventButton * event, gpointer user_data)
{

	return false;
}

//----------------------------------------------------------------------------------------------
gboolean
on_textComp3_button_press_event(GtkWidget * widget,
				GdkEventButton * event, gpointer user_data)
{

	return false;
}

//----------------------------------------------------------------------------------------------
void on_btnOpenFile_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *openFile;
	gchar buf[255];

	sprintf(buf,"%s/BibleStudy/*.pad",homedir);
	openFile = create_fileselection1();
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(openFile),
					buf);
	gtk_widget_show(openFile);
}

//----------------------------------------------------------------------------------------------
void on_btnSaveFile_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *savemyFile, *ok_button2;
	gchar buf[255];

	sprintf(buf,"%s/BibleStudy/.pad",homedir);	
	if (current_filename) {
		saveFile(current_filename);
		return;
	} else {
		savemyFile = create_fileselectionSave();
		gtk_file_selection_set_filename(GTK_FILE_SELECTION
						(savemyFile),
						buf);
		ok_button2 = lookup_widget(savemyFile, "ok_button2");
		gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
				   GTK_SIGNAL_FUNC(on_ok_button2_clicked),
				   NULL);
		gtk_widget_show(savemyFile);
	}
}

//----------------------------------------------------------------------------------------------
void on_btnSaveFileAs_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *savemyFile, *ok_button2;
        gchar buf[255];

	sprintf(buf,"%s/BibleStudy/.pad",homedir);
	savemyFile = create_fileselectionSave();
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(savemyFile),
					buf);
	ok_button2 = lookup_widget(savemyFile, "ok_button2");
	gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button2_clicked), NULL);
	gtk_widget_show(savemyFile);
}

//----------------------------------------------------------------------------------------------
void on_btnPrint_clicked(GtkButton * button, gpointer user_data)
{
#ifdef  USE_GNOMEPRINT
	file_print(GTK_WIDGET(button), current_filename, 1);	//-- pass studypad text widget to print
#endif				/*USE_GNOMEPRINT */
}

//----------------------------------------------------------------------------------------------
void on_btnCut_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(GTK_WIDGET(button), "text3");
	gtk_editable_cut_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_btnCopy_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(GTK_WIDGET(button), "text3");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_btnPaste_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(GTK_WIDGET(button), "text3");
	gtk_editable_paste_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_text3_changed(GtkEditable * editable, gpointer user_data)
{
	GtkWidget *statusbar;
	gchar charbuf[255];

	statusbar = lookup_widget(GTK_WIDGET(editable), "statusbar2");
	if (current_filename) {
		sprintf(charbuf, "%s - modified.", current_filename);
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1, charbuf);
	} else {
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1,
				   "modified");
	}
	file_changed = true;
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
	static gboolean firsttime = true;	//-- dont do anything if this is the first time here, but remember we were here - set firsttime to false
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
	firsttime = false;	//-- no longer the first time
}

//----------------------------------------------------------------------------------------------
gboolean
on_moduleText_selection_notify_event(GtkWidget * widget,
				     GdkEventSelection * event,
				     gpointer user_data)
{

	return false;
}

//----------------------------------------------------------------------------------------------
void
on_moduleText_selection_received(GtkWidget * widget,
				 GtkSelectionData * data,
				 guint time, gpointer user_data)
{

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
void on_btnSpell_clicked(GtkButton * button, gpointer user_data)
{
#ifdef USE_ASPELL
	GtkWidget *speller;

	speller = spellcheck(0);
#endif				/* USE_ASPELL */
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
	ShutItDown();
}

//----------------------------------------------------------------------------------------------
void on_clear1_activate(GtkMenuItem * menuitem,	//-- clear history menu
			gpointer user_data)
{
	clearhistory();
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
void on_save1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


//----------------------------------------------------------------------------------------------
void on_save_as1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

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
		footnotesSWORD(true);
	else
		footnotesSWORD(false);
}


//----------------------------------------------------------------------------------------------
void on_copy3_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(MainFrm, (gchar *) user_data);
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void
on_fontpicker2_font_set(GnomeFontPicker * gnomefontpicker,
			GString arg1, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_colorpicker5_color_set(GnomeColorPicker * gnomecolorpicker,
			  guint arg1,
			  guint arg2,
			  guint arg3, guint arg4, gpointer user_data)
{
/*	myGreen.red = arg1;
	myGreen.green = arg2;
	myGreen.blue = arg3;
*/
//      curMod->Display();
}

//----------------------------------------------------------------------------------------------
void
on_colorpicker1_color_set(GnomeColorPicker * gnomecolorpicker,
			  guint arg1,
			  guint arg2,
			  guint arg3, guint arg4, gpointer user_data)
{
/*	BGcolor.red = arg1;
	BGcolor.green = arg2;
	BGcolor.blue = arg3;
*/
//      curMod->Display();
}

//----------------------------------------------------------------------------------------------
void
on_colorpicker2_color_set(GnomeColorPicker * gnomecolorpicker,
			  guint arg1,
			  guint arg2,
			  guint arg3, guint arg4, gpointer user_data)
{

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
on_suggestions_select_row(GtkCList * clist,
			  gint row,
			  gint column,
			  GdkEvent * event, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnSpellOK_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnEBMcut_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnEBMcopy_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnEBMpaste_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_text4_changed(GtkEditable * editable, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnEBMOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	editbookmarksSave(dlg);	//-- in filestuff.cpp
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void on_btnEBMApply_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	editbookmarksSave(dlg);	//-- in filestuff.cpp
}

//----------------------------------------------------------------------------------------------
void on_btnEBMCancel_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void on_add_bookmark2_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_edit_bookmarks_1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cbusebounds_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cpfgMainwindow_color_set(GnomeColorPicker * gnomecolorpicker,
			    guint arg1,
			    guint arg2,
			    guint arg3, guint arg4, gpointer user_data)
{

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
	applycolor = true;
	gtk_widget_set_sensitive(btnok, true);
	gtk_widget_set_sensitive(btnapply, true);
}

//----------------------------------------------------------------------------------------------
void
on_cpfgInterlinear_color_set(GnomeColorPicker * gnomecolorpicker,
			     guint arg1,
			     guint arg2,
			     guint arg3, guint arg4, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cpbgMainwindow_color_set(GnomeColorPicker * gnomecolorpicker,
			    guint arg1,
			    guint arg2,
			    guint arg3, guint arg4, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cpbgInterlinear_color_set(GnomeColorPicker * gnomecolorpicker,
			     guint arg1,
			     guint arg2,
			     guint arg3, guint arg4, gpointer user_data)
{


}

//----------------------------------------------------------------------------------------------
void
on_cpbgCurrentverse_color_set(GnomeColorPicker * gnomecolorpicker,
			      guint arg1,
			      guint arg2,
			      guint arg3, guint arg4, gpointer user_data)
{

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
	gtk_text_set_word_wrap(GTK_TEXT(text1), true);
	gtk_text_set_word_wrap(GTK_TEXT(text2), true);
	showinfoSWORD(text2, GTK_LABEL(label));
	gtk_widget_show(dlg);
}

//----------------------------------------------------------------------------------------------
void on_btnAboutSwordOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void on_btnSpellNotes_clicked(GtkButton * button, gpointer user_data)
{
#ifdef USE_ASPELL
	GtkWidget *speller;
	if (GTK_TOGGLE_BUTTON
	    (lookup_widget(GTK_WIDGET(button), "btnEditNote"))->active)
		speller = spellcheck(1);
#endif				/* USE_ASPELL */
}


//----------------------------------------------------------------------------------------------
void
on_fpMainwindowitalic_font_set(GnomeFontPicker * gnomefontpicker,
			       GString arg1, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void save_changes_handler(int reply, gpointer data)
{
	/*if(reply == 0) saveChanges = true;
	   else saveChanges = false;
	   waitonmessage = false;
	 */
}

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
void
on_fbMainwindowbold_font_set(GnomeFontPicker * gnomefontpicker,
			     GString arg1, gpointer user_data)
{

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
		firstsearch = false;
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
void
on_fpSPPrinter_font_set(GnomeFontPicker * gnomefontpicker,
			GString arg1, gpointer user_data)
{

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
void on_btnVerseListNew_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnVerseListOpen_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnVerseListClose_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnVerseListPrint_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_verse_list1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


//----------------------------------------------------------------------------------------------
void on_btnVerseListSave_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	editbookmarksSave(dlg);
}


//----------------------------------------------------------------------------------------------
void on_btnVerseListCopy_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnVerseListPaste_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cbContext_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_verselist_select_row(GtkCList * clist,
			gint row,
			gint column, GdkEvent * event, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_notebook3_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	static gboolean firsttime = true;	//-- dont do anything if this is the first time here, but remember we were here - set firsttime to false
	if (!firsttime) {
		changepagenotebook(notebook, page_num);	//-- send to changepagenotebook() function in GnomeSword.cpp
	}
	firsttime = false;	//-- remember we were here
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
void on_btnAboutModuleOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void
on_tree1_select_child(GtkTree * tree,
		      GtkWidget * widget, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_tree1_selection_changed(GtkTree * tree, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_tree1_unselect_child(GtkTree * tree,
			GtkWidget * widget, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnBMEAddItem_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnBMEDelete_clicked(GtkButton * button, gpointer user_data)
{

}


//----------------------------------------------------------------------------------------------
void on_btnBMEup_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnBMEdown_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnBMEleft_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnBMEright_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnBMEok_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnBMEapply_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnBMEcancel_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_about_this_module1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(curMod->Name());
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(comp1Mod->Name());
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module3_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(comp2Mod->Name());
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module4_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(comp3Mod->Name());
}

//----------------------------------------------------------------------------------------------
void on_btnPropertyboxOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	if (applycolor)
		setcurrentversecolor(num1, num2, num3);
	applycolor = false;

	setformatoption(lookup_widget(GTK_WIDGET(button), "cbtnPNformat"));
	applyoptions(bar, comtabs, dicttabs, showtextgroup, showcomgroup,
		     showdictgroup, showhistorygroup);
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void on_btnPropertyboxApply_clicked(GtkButton * button, gpointer user_data)
{
	if (applycolor)
		setcurrentversecolor(num1, num2, num3);	//-- if color has changed
	applycolor = false;
	setformatoption(lookup_widget(GTK_WIDGET(button), "cbtnPNformat"));
	applyoptions(bar, comtabs, dicttabs, showtextgroup, showcomgroup,
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

//----------------------------------------------------------------------------------------------
void
on_clLElist_select_row(GtkCList * clist,
		       gint row,
		       gint column, GdkEvent * event, gpointer user_data)
{
	listrow = row;
	selectrow(GTK_WIDGET(clist), listrow, column);
}

//----------------------------------------------------------------------------------------------
void on_btnLEup_clicked(GtkButton * button, gpointer user_data)
{
	movelistitem(GTK_WIDGET(button), 0, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEdown_clicked(GtkButton * button, gpointer user_data)
{
	movelistitem(GTK_WIDGET(button), 1, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEleft_clicked(GtkButton * button, gpointer user_data)
{
	movelistitem(GTK_WIDGET(button), 2, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEright_clicked(GtkButton * button, gpointer user_data)
{
	movelistitem(GTK_WIDGET(button), 3, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEgotoverse_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *entry;
	gchar *buf;
	entry = lookup_widget(GTK_WIDGET(button), "entryVerseLookup");
	buf = gtk_entry_get_text(GTK_ENTRY(entry));
	changeLEverse(buf);
}

//----------------------------------------------------------------------------------------------
void on_cbLEBook_changed(GtkEditable * editable, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_spLEChapter_changed(GtkEditable * editable, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_spLEVerse_changed(GtkEditable * editable, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEAddVerse_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list, *entry;
	gchar *item;

	list = lookup_widget(GTK_WIDGET(button), "clLElist");
	entry = lookup_widget(GTK_WIDGET(button), "entryVerseLookup");
	item = gtk_entry_get_text(GTK_ENTRY(entry));
	addverse(list, listrow, item);	//-- function to add item to list (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEAddItem_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clLElist");
	addsubitme(list, listrow);
}

//----------------------------------------------------------------------------------------------
void on_btnLEDelete_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEClose_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEmakesub_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEok_clicked(GtkButton * button, gpointer user_data)
{
	applychanges(GTK_WIDGET(button));
	gtk_widget_hide(listeditor);
}

//----------------------------------------------------------------------------------------------
void on_btnLEapply_clicked(GtkButton * button, gpointer user_data)
{
	applychanges(GTK_WIDGET(button));
}

//----------------------------------------------------------------------------------------------
void on_btnLEcancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(listeditor);
}

//----------------------------------------------------------------------------------------------
void on_entryPreitem_changed(GtkEditable * editable, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnLEapplylistchanges_clicked(GtkButton * button, gpointer user_data)
{
	applylistchanges(GTK_WIDGET(button), listrow);
}

//----------------------------------------------------------------------------------------------
void
on_btnLEcancelistchanges_clicked(GtkButton * button, gpointer user_data)
{

}

//==============================================================================================
//----------------------------------------------------------------------------------------------
void on_new1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_open1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_close1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEup1_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clist1");
	movelistitem(list, 0, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEdown1_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clist1");
	movelistitem(list, 1, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEup2_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clist2");
	movelistitem(list, 0, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEdown2_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clist2");
	movelistitem(list, 1, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEup3_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clist3");
	movelistitem(list, 0, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEdown3_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clist3");
	movelistitem(list, 1, listrow);	//-- send to movelistitem function for processing (listeditor.cpp)
}

//----------------------------------------------------------------------------------------------
void on_btnLEaddmenu_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEdelete_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clLElist");
	deleteitem(list);
}

//----------------------------------------------------------------------------------------------
void on_btnLEnew_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEopen_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEclose_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEprev_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLEnext_clicked(GtkButton * button, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnLErefresh_clicked(GtkButton * button, gpointer user_data)
{

}


//----------------------------------------------------------------------------------------------
void on_btnLEsave_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *listeditor;

	listeditor = gtk_widget_get_toplevel(GTK_WIDGET(button));
	editbookmarksSave(listeditor);
}

//----------------------------------------------------------------------------------------------
void
on_tbtnFollow_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void on_btnSPnew_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *text;

	text = lookup_widget(GTK_WIDGET(button), "text3");
	newSP(text);		//-- send text widget to GnomeSword.cpp to start new file
}

//----------------------------------------------------------------------------------------------
void
on_cbtnPNformat_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(togglebutton));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, true);
	gtk_widget_set_sensitive(btnapply, true);
}

//----------------------------------------------------------------------------------------------
gboolean
on_cbeFreeformLookup_drag_drop(GtkWidget * widget,
			       GdkDragContext * drag_context,
			       gint x,
			       gint y, guint time, gpointer user_data)
{

	return false;
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

	changeVerse(buf);	
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

	changeVerse(buf);
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

	return false;
}

//----------------------------------------------------------------------------------------------
gboolean
on_textComments_key_press_event(GtkWidget * widget,
				GdkEventKey * event, gpointer user_data)
{
	gchar *buf;
	static gboolean needsecond = false;

	if (!settings->formatpercom){	//-- do we want formatting?	
		noteModified = true;	//-- note has been modified to get us here
		return false;
	}
	if (event->keyval == 65293 || event->keyval == 65421){	//-- return key	
		noteModified = noteeditor->setNEWLINE(NEtext);	//-- noteeditor.cpp
		return true;
	}
	if (GTK_EDITABLE(NEtext)->has_selection){	//-- do we have a selection?	
		if (event->keyval == 65507){	//-- ctrl key pressed		
			needsecond = true;
			return false;
		}
	}

	noteModified = true;
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
		needsecond = false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------
gboolean
on_moduleText_enter_notify_event(GtkWidget * widget,
				 GdkEventCrossing * event,
				 gpointer user_data)
{

	return false;
}

//----------------------------------------------------------------------------------------------
void on_lookup_word_1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_about_this_module5_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(curdictMod->Name());
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
	if (curcomMod) {
		(*curcomMod)--;
		curcomMod->Display();
	}
}

//----------------------------------------------------------------------------------------------
void on_btnFoward_clicked(GtkButton * button, gpointer user_data)
{
	if (curcomMod) {
		(*curcomMod)++;
		curcomMod->Display();
	}
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
	showmoduleinfoSWORD(curcomMod->Name());
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

	changeVerse(buf);
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
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------------
gboolean
on_list1_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
{

	return false;
}

//----------------------------------------------------------------------------------------------
void
on_cbtnShowSCB_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(togglebutton));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, true);
	gtk_widget_set_sensitive(btnapply, true);
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
	gtk_widget_set_sensitive(btnok, true);
	gtk_widget_set_sensitive(btnapply, true);
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
	gtk_widget_set_sensitive(btnok, true);
	gtk_widget_set_sensitive(btnapply, true);
	dicttabs = togglebutton->active;
}

//----------------------------------------------------------------------------------------------
void
//-- show hide commentary notebook tabs - popup menu choice------------------------------------- to falsetoogled which will do the work
on_show_tabs1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		gtk_widget_show(lookup_widget(MainFrm, "notebook1"));
	else
		gtk_widget_hide(lookup_widget(MainFrm, "notebook1"));
}

//----------------------------------------------------------------------------------------------
void
//-- show hide dict/lex notebook tabs - popup menu choice--------------------------------------- to falsetoogled which will do the work
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
	if (settings->showshortcutbar) {
		settings->showshortcutbar = false;
		e_paned_set_position (E_PANED(lookup_widget(MainFrm,"epaned")), 0);
	} else {
		settings->showshortcutbar = true;
		e_paned_set_position (E_PANED(lookup_widget(MainFrm,"epaned")), settings->shortcutbarsize);
	}
}

//----------------------------------------------------------------------------------------------
void
on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
			      GdkEvent * event,
			      gint group_num, gint item_num)
{
	sbchangeModSword(group_num, item_num);
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
on_cbtnShowTextgroup_toggled(GtkToggleButton * togglebutton,
			     gpointer user_data)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(togglebutton));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, true);
	gtk_widget_set_sensitive(btnapply, true);
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
	gtk_widget_set_sensitive (btnok, true);
	gtk_widget_set_sensitive (btnapply, true);
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
	gtk_widget_set_sensitive (btnok, true);
	gtk_widget_set_sensitive (btnapply, true);
	showdictgroup = togglebutton->active;
}

//----------------------------------------------------------------------------------------------
gboolean
on_epaned_button_release_event(GtkWidget       *widget,
                               GdkEventButton  *event,
                               gpointer         user_data)
{
        //gchar buf[80];
        
        settings->shortcutbarsize = e_paned_get_position(E_PANED(lookup_widget(MainFrm,"epaned")));
        //sprintf(buf,"%d\n",settings->shortcutbarsize);
        //cout << buf;
        if(settings->shortcutbarsize) return true;
        return false;
}

void
on_cbtnShowHistoryGroup_toggled        (GtkToggleButton *togglebutton,
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
	showhistorygroup = togglebutton->active;
}
