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

#include "callback.h"
#include "GnomeSword.h"
#include "support.h"
#include "interface.h"
#include "filestuff.h"
#include "display.h"
#include "dialogs.h"

#ifdef  USE_GNOMEPRINT
#include "printstuff.h"
#endif /* USE_GNOMEPRINT */

#ifdef USE_ASPELL
#include "spellcheck.h"
#endif /* USE_ASPELL */


bool firstsearch = TRUE;
GtkWidget *searchDlg;
extern bool ApplyChange;
extern bool file_changed;
extern gchar *current_filename;
extern GtkWidget *MainFrm;
extern GtkWidget *bookmark_mnu;
extern gchar	*font_mainwindow,
							*font_interlinear,
							*font_currentverse;
extern GdkColor myGreen;  //-- current verse color for main text window - declared in display.cpp
extern bool noteModified;
extern bool waitonmessage;
//extern gboolean saveChanges;
extern gboolean autoSave; //-- auto save personal comments when verse changes -- declared in GnomeSword.cpp
extern gint answer;    //-- do we save file on exit
//-------------------------------------------------------------------------------------------
void
on_mnuHistoryitem1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	changeVerse(user_data);
}

//----------------------------------------------------------------------------------------------
void
on_ok_button1_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
 	GtkWidget 	*filesel;

	filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
	loadFile (filesel);
	
	gtk_widget_destroy(filesel);

}


//----------------------------------------------------------------------------------------------
void
on_cancel_button1_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel (GTK_WIDGET (button)));
}

//----------------------------------------------------------------------------------------------
void
on_ok_button2_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *filesel;
  gchar filename[255];

  filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
  sprintf(filename,"%s", gtk_file_selection_get_filename (GTK_FILE_SELECTION (filesel)));
  gtk_widget_destroy(filesel);
  saveFile(filename);
}

//----------------------------------------------------------------------------------------------
void
on_cancel_button2_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel (GTK_WIDGET (button)));
}

//----------------------------------------------------------------------------------------------
void
on_btnAboutOK_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy(gtk_widget_get_toplevel (GTK_WIDGET (button)));
}

//----------------------------------------------------------------------------------------------
void
on_btnSearch1_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget	*searchFrm;
	searchFrm = gtk_widget_get_toplevel(GTK_WIDGET(button));
	searchSWORD(searchFrm);
}

//----------------------------------------------------------------------------------------------
void
on_resultList_select_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	GtkWidget	*searchFrm;
	searchFrm = gtk_widget_get_toplevel(GTK_WIDGET(clist)); 
	resultsListSWORD(searchFrm, row, column);
}

//----------------------------------------------------------------------------------------------
void
on_dictionary_lookup1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_copy_verse1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *text;
	text = lookup_widget(MainFrm,"moduleText");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void
on_item1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnOptionsOK_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnOptionsCancel_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnOptionsHelp_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_mainwindow_destroy                  (GtkObject       *object,
                                        gpointer         user_data)
{
 	ShutItDown();
}

//----------------------------------------------------------------------------------------------
void
on_dlgSearch_destroy                  (GtkObject       *object,
                                        gpointer         user_data)
{
 	firstsearch=true;
}

//----------------------------------------------------------------------------------------------
void
on_btnSearch_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	if(firstsearch)
	{
		searchDlg = create_dlgSearch();
		setupSearchDlg(searchDlg);
		firstsearch = FALSE;
		gtk_signal_connect (GTK_OBJECT (searchDlg), "destroy",
                      GTK_SIGNAL_FUNC (on_dlgSearch_destroy),
                      NULL);
	}
	gtk_widget_show (searchDlg); 	
}

//----------------------------------------------------------------------------------------------
void
on_btnStrongs_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	if(togglebutton->active) 
		strongsSWORD(TRUE);
	else 
		strongsSWORD(FALSE);
}

//----------------------------------------------------------------------------------------------
void
on_add_bookmark1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
 	addBookmark();
}

//----------------------------------------------------------------------------------------------
void
on_edit_bookmarks1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *BMEditor;
	
	BMEditor = create_wdwEditBookmarks();
	editbookmarksLoad(BMEditor);
	gtk_widget_show(BMEditor);
}

//----------------------------------------------------------------------------------------------
void
on_john_3_1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
 	changeVerse((gchar*)user_data);
}

//----------------------------------------------------------------------------------------------
void
on_romans_1_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  	changeVerse(user_data);
}

//----------------------------------------------------------------------------------------------
void
on_luke_1_1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
 	changeVerse(user_data);
}

//----------------------------------------------------------------------------------------------
void
on_footnotes_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(GTK_CHECK_MENU_ITEM(menuitem)->active) 
		footnotesSWORD(TRUE);
	else 
		footnotesSWORD(FALSE);
}

//----------------------------------------------------------------------------------------------
void
on_view_item1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}
//----------------------------------------------------------------------------------------------
void
on_mainText_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gchar *modName;

	modName = (gchar *)user_data;
	changecurModSWORD(modName);
}

//----------------------------------------------------------------------------------------------
void
on_1st_interlinear_window1_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gchar *modName;

	modName = 	(gchar *)user_data;
	changecomp1ModSWORD(modName);
}

//----------------------------------------------------------------------------------------------
void
on_item2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_2nd_interlinear_window1_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gchar *modName;

	modName = 	(gchar *)user_data;
	changecomp2ModSWORD(modName);
}

//----------------------------------------------------------------------------------------------
void
on_3rd_interlinear_window1_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gchar *modName;

	modName = 	(gchar *)user_data;
	changecomp3ModSWORD(modName);
}

//----------------------------------------------------------------------------------------------
void
on_item4_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_verse_style1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	
	if(GTK_CHECK_MENU_ITEM(menuitem)->active)
		setversestyleSWORD(TRUE);
	else
		setversestyleSWORD(FALSE);
}

//----------------------------------------------------------------------------------------------
void
on_show_interlinear_page1_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

	if(GTK_CHECK_MENU_ITEM(menuitem)->active)
		showIntPage(TRUE);
	else
		showIntPage(FALSE);
}

//----------------------------------------------------------------------------------------------
void
on_about2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
gboolean
on_cbBook_button_release_event         (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}

//----------------------------------------------------------------------------------------------
GtkDirectionType
on_cbBook_focus                        (GtkContainer    *container,
                                        GtkDirectionType direction,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cbeBook_changed                     (GtkEditable     *editable,
                                        gpointer         user_data)
{
	gchar    *bookname,
				ref[255];

	if(ApplyChange)
	{
		bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(GTK_WIDGET(editable),"cbeBook")));
		sprintf(ref,"%s 1:1",bookname);
		gtk_spin_button_set_value( GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(editable),"spbChapter")), gint(1));
		gtk_spin_button_set_value( GTK_SPIN_BUTTON(lookup_widget(GTK_WIDGET(editable),"spbVerse") ), 1);
		gtk_entry_set_text(GTK_ENTRY(lookup_widget(GTK_WIDGET(editable),"cbeFreeformLookup")),ref);
		changeVerse(ref);	
	}	
}

//----------------------------------------------------------------------------------------------
void
on_spbChapter_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_spbChapter_activate                 (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
gboolean
on_spbChapter_button_release_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	chapterSWORD();
    return TRUE;
}

//----------------------------------------------------------------------------------------------
void
on_spbVerse_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_spbVerse_activate                   (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
gboolean
on_spbVerse_button_release_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	verseSWORD();
    return TRUE;
}

//----------------------------------------------------------------------------------------------
void
on_btnLookup_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	btnlookupSWORD();
}

//----------------------------------------------------------------------------------------------
gboolean
on_cbeFreeformLookup_key_press_event   (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
	freeformlookupSWORD(event);
  	return TRUE;
}

//----------------------------------------------------------------------------------------------
gboolean
on_arrow1_button_press_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}


//----------------------------------------------------------------------------------------------
void
on_btnExit_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{	
	ShutItDown();
}

//----------------------------------------------------------------------------------------------
gboolean
on_moduleText_button_press_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{	
	if(event->button == 1)
	{		
		gtk_entry_set_text(GTK_ENTRY(lookup_widget(widget,"dictionarySearchText")), "");		
	 	return TRUE;  
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------------
void                  //-- commentary notebook page changed
on_notebook1_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data)
{
	GtkLabel *label; //-- pointer to page label
  static bool firsttime = true;
  if(!firsttime)
  {
		label = (GtkLabel *)page->tab_label; //-- get label
		changcurcomModSWORD((char *)label->label,page_num); //-- pass label text and page number to function to do the work - GnomeSword.cpp
	}
	firsttime = false;
}

//----------------------------------------------------------------------------------------------
void
on_textComments_changed                (GtkEditable     *editable,
                                        gpointer         user_data)
{
	noteModified = TRUE;
}

//----------------------------------------------------------------------------------------------
gboolean
on_textComments_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}

//----------------------------------------------------------------------------------------------
void
on_btnEditNote_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	editnoteSWORD(GTK_TOGGLE_BUTTON(togglebutton)->active);
/*	if(
	{
		gtk_text_set_editable (GTK_TEXT (lookup_widget(GTK_WIDGET(togglebutton),"textComments")), TRUE);	
		noteModified = FALSE;
		
	}
	else
	{
		gtk_text_set_editable (GTK_TEXT (lookup_widget(GTK_WIDGET(togglebutton),"textComments")), FALSE);
		//if(noteModified) ;
	}
*/
	//setsensitive(togglebutton->active);
}

//----------------------------------------------------------------------------------------------
void
on_btnSaveNote_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{	
	if(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button),"btnEditNote"))->active)
	{
		//saveChanges = true; //-- we know we want to save the changes - that's how we got here
		savenoteSWORD(noteModified);
	}	
	//noteModified = false;
}

//----------------------------------------------------------------------------------------------
void
on_btnDeleteNote_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	if(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button),"btnEditNote"))->active)
	{
		deletenoteSWORD();
	}
}

//----------------------------------------------------------------------------------------------
gboolean
on_textComp1_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}

//----------------------------------------------------------------------------------------------
gboolean
on_textComp2_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}

//----------------------------------------------------------------------------------------------
gboolean
on_textComp3_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}

//----------------------------------------------------------------------------------------------
void
on_btnOpenFile_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *openFile;

	openFile = create_fileselection1();
	gtk_file_selection_set_filename( GTK_FILE_SELECTION(openFile),
                                            "/home/tb/BibleStudy/*.pad" );
	gtk_widget_show (openFile);
}

//----------------------------------------------------------------------------------------------
void
on_btnSaveFile_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *savemyFile;
	if(current_filename)
	{
		saveFile(current_filename);	
		return;	
	}
	else
	{	
		savemyFile = create_fileselectionSave();
		gtk_file_selection_set_filename( GTK_FILE_SELECTION(savemyFile),
                                             "/home/tb/BibleStudy");
		gtk_widget_show (savemyFile);
	}
}

//----------------------------------------------------------------------------------------------
void
on_btnSaveFileAs_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *savemyFile;

	savemyFile = create_fileselectionSave();
	gtk_file_selection_set_filename( GTK_FILE_SELECTION(savemyFile),
                                            "/home/tb/BibleStudy/" );
	gtk_widget_show (savemyFile);
}

//----------------------------------------------------------------------------------------------
void
on_btnPrint_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef  USE_GNOMEPRINT
	file_print(GTK_WIDGET(button),current_filename,1); //-- pass studypad text widget to print
#endif  /*USE_GNOMEPRINT*/
}

//----------------------------------------------------------------------------------------------
void
on_btnCut_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
 	GtkWidget *text;
	text = lookup_widget(GTK_WIDGET(button),"text3");
	gtk_editable_cut_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void
on_btnCopy_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
 	GtkWidget *text;
	text = lookup_widget(GTK_WIDGET(button),"text3");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void
on_btnPaste_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *text;
	text = lookup_widget(GTK_WIDGET(button),"text3");
	gtk_editable_paste_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void
on_text3_changed                       (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkWidget *statusbar;
	gchar charbuf[255];

	statusbar = lookup_widget(GTK_WIDGET(editable),"statusbar2");
	if(current_filename)
	{
		sprintf(charbuf,"%s - modified.",current_filename);
		gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, charbuf);	
	}
	else
	{
		gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, "modified");		
	}
	file_changed = true;
}

//----------------------------------------------------------------------------------------------
void                             //-- someone changed dict & lex notebook page
on_notebook4_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data)
{
	GtkLabel *label;     //-- pointer to tab label
	string keyText;      //-- string for verse key text
	gchar *entryText;    //-- pointer to dict key
	static bool firsttime = true;    //-- don't do anything if this is the first time here, but remember we were here - set firsttime to false
  if(!firsttime)
  {
		label = (GtkLabel *)page->tab_label;	//-- set label to tab label
		entryText = gtk_entry_get_text(GTK_ENTRY(lookup_widget(GTK_WIDGET(notebook),"dictionarySearchText"))); //-- get key from entry
		if(!strcmp(label->label,"Torrey")) entryText = ""; //-- if Torrey module then set text to null else segfault?
		keyText = entryText;  //-- put entryText into string keyText
		changcurdictModSWORD((char *)label->label,keyText, page_num);  //-- sent to changcurdictModSWORD() function in GnomeSword.cpp
	}
	firsttime = false; //-- no longer the first time
}

//----------------------------------------------------------------------------------------------
gboolean
on_moduleText_selection_notify_event   (GtkWidget       *widget,
                                        GdkEventSelection *event,
                                        gpointer         user_data)
{

  return FALSE;
}

//----------------------------------------------------------------------------------------------
void
on_moduleText_selection_received       (GtkWidget       *widget,
                                        GtkSelectionData *data,
                                        guint            time,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_dictionarySearchText_changed        (GtkEditable     *editable,
                                        gpointer         user_data)
{
	gchar *mytext;

	mytext = gtk_entry_get_text(GTK_ENTRY(editable));
	dictSearchTextChangedSWORD(mytext);
}

//----------------------------------------------------------------------------------------------
void
on_list1_select_row                    (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gchar *text;

	gtk_clist_get_text(GTK_CLIST(lookup_widget(GTK_WIDGET(clist),"list1")), row, column, &text);	
	gtk_entry_set_text(GTK_ENTRY(lookup_widget(GTK_WIDGET(clist),"dictionarySearchText")), text);
}

//----------------------------------------------------------------------------------------------
void
on_item51_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_item52_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_item53_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnSpell_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{ 	
#ifdef USE_ASPELL
	GtkWidget *speller;
		
	speller = spellcheck(0);
#endif /* USE_ASPELL */
}


//----------------------------------------------------------------------------------------------
void
on_btbSpellOK_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget	*spell;

	spell = gtk_widget_get_toplevel (GTK_WIDGET (button));	
	gtk_widget_destroy(spell);
}


//----------------------------------------------------------------------------------------------
void
on_btnSpellApply_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


//----------------------------------------------------------------------------------------------
void
on_btnSpellCancel_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget* spell;

	spell = gtk_widget_get_toplevel (GTK_WIDGET (button));
	gtk_widget_destroy(spell);
}


//----------------------------------------------------------------------------------------------
void
on_btnSearchOK_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	
	gtk_widget_hide(searchDlg);
}


//----------------------------------------------------------------------------------------------
void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	ShutItDown();
}


//----------------------------------------------------------------------------------------------
void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


//----------------------------------------------------------------------------------------------
void
on_copy2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_find1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


//----------------------------------------------------------------------------------------------
void
on_inter1_module1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_clear1_activate               (GtkMenuItem     *menuitem,    //-- clear history menu
                                        gpointer         user_data)
{
   clearhistory();
}

//----------------------------------------------------------------------------------------------
void
on_about_gnomesword1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
 	GtkWidget *AboutBox;

	AboutBox = create_about2();
	gtk_widget_show (AboutBox);
}


//----------------------------------------------------------------------------------------------
void
on_item3_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


//----------------------------------------------------------------------------------------------
void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


//----------------------------------------------------------------------------------------------
void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


//----------------------------------------------------------------------------------------------
void
on_strongs_numbers1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(GTK_CHECK_MENU_ITEM(menuitem)->active) 
		strongsSWORD(TRUE);
	else 
		strongsSWORD(FALSE);
}


//----------------------------------------------------------------------------------------------
void
on_footnotes1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(GTK_CHECK_MENU_ITEM(menuitem)->active) 
		footnotesSWORD(TRUE);
	else 
		footnotesSWORD(FALSE);
}


//----------------------------------------------------------------------------------------------
void
on_copy3_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *text;
	text = lookup_widget(MainFrm,"moduleText");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void
on_propertybox1_clicked                (GnomePropertyBox *gnomepropertybox,
                                        gint             arg1,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_fontpicker2_font_set                (GnomeFontPicker *gnomefontpicker,
                                        string        arg1,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_colorpicker5_color_set              (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data)
{
/*	myGreen.red = arg1;
	myGreen.green = arg2;
	myGreen.blue = arg3;
*/
//	curMod->Display();
}

//----------------------------------------------------------------------------------------------
void
on_colorpicker1_color_set              (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data)
{
/*	BGcolor.red = arg1;
	BGcolor.green = arg2;
	BGcolor.blue = arg3;
*/
//	curMod->Display();
}

//----------------------------------------------------------------------------------------------
void
on_colorpicker2_color_set              (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	openpropertiesbox();
}

//----------------------------------------------------------------------------------------------
void
on_suggestions_select_row              (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnSpellOK_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnEBMcut_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnEBMcopy_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnEBMpaste_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_text4_changed                       (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnEBMOK_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget	*dlg;

	dlg = gtk_widget_get_toplevel (GTK_WIDGET (button));
	editbookmarksSave(dlg);	  //-- in filestuff.cpp
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void
on_btnEBMApply_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget	*dlg;

	dlg = gtk_widget_get_toplevel (GTK_WIDGET (button));
	editbookmarksSave(dlg);	 //-- in filestuff.cpp
}

//----------------------------------------------------------------------------------------------
void
on_btnEBMCancel_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget	*dlg;

	dlg = gtk_widget_get_toplevel (GTK_WIDGET (button));	
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void
on_add_bookmark2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_edit_bookmarks_1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cbusebounds_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cpfgMainwindow_color_set            (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_fpMainwindow_font_set               (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data)
{
	font_mainwindow = arg1.str;
	gnome_property_box_changed(GNOME_PROPERTY_BOX(gtk_widget_get_toplevel(GTK_WIDGET(gnomefontpicker))));


}

//----------------------------------------------------------------------------------------------
void
on_fbCurrentverse_font_set             (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data)
{
  font_currentverse = arg1.str;
  gnome_property_box_changed(GNOME_PROPERTY_BOX(gtk_widget_get_toplevel(GTK_WIDGET(gnomefontpicker))));
}

//----------------------------------------------------------------------------------------------
void
on_fbInerlinear_font_set               (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data)
{
  font_interlinear = arg1.str;
  gnome_property_box_changed(GNOME_PROPERTY_BOX(gtk_widget_get_toplevel(GTK_WIDGET(gnomefontpicker))));
}

//----------------------------------------------------------------------------------------------
void
on_cpfgCurrentverse_color_set          (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data)
{
	setcurrentversecolor(arg1,arg2,arg3);
	gnome_property_box_changed(GNOME_PROPERTY_BOX(gtk_widget_get_toplevel(GTK_WIDGET(gnomecolorpicker))));
}

//----------------------------------------------------------------------------------------------
void
on_cpfgInterlinear_color_set           (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cpbgMainwindow_color_set            (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cpbgInterlinear_color_set           (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data)
{


}

//----------------------------------------------------------------------------------------------
void
on_cpbgCurrentverse_color_set          (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_about_the_sword_project1_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *dlg,
						*text1,
						*text2;
	
 	dlg = create_AboutSword();
 	text1 = lookup_widget(dlg,"txtAboutSword");
 	text2 = lookup_widget(dlg,"text6");
 	gtk_text_set_word_wrap(GTK_TEXT(text1), TRUE);
 	gtk_text_set_word_wrap(GTK_TEXT(text2), TRUE);
 	gtk_widget_show(dlg);
}

//----------------------------------------------------------------------------------------------
void
on_btnAboutSwordOK_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget	*dlg;

	dlg = gtk_widget_get_toplevel (GTK_WIDGET (button));	
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void
on_btnSpellNotes_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef USE_ASPELL
	GtkWidget *speller;
	if(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button),"btnEditNote"))->active)
  speller = spellcheck(1);
#endif /* USE_ASPELL */
}


//----------------------------------------------------------------------------------------------
void
on_fpMainwindowitalic_font_set         (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data)
{

}
//----------------------------------------------------------------------------------------------
void
save_changes_handler         (int reply, gpointer data)
{
  /*if(reply == 0) saveChanges = true;
  else saveChanges = false;
  waitonmessage = false;
*/
}

//----------------------------------------------------------------------------------------------
void
on_auto_save_notes1_activate			(GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	setautosave(GTK_CHECK_MENU_ITEM(menuitem)->active);
}

//----------------------------------------------------------------------------------------------
void
on_btnBack_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_copy4_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


//----------------------------------------------------------------------------------------------
void
on_change_module_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  changepercomModSWORD((gchar*)user_data);
}

//----------------------------------------------------------------------------------------------
void
on_fbMainwindowbold_font_set           (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnKeyPrev_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
   dictchangekeySWORD(0);
}

//----------------------------------------------------------------------------------------------
void
on_btnKeyNext_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
   dictchangekeySWORD(1);
}

//----------------------------------------------------------------------------------------------
void
on_search1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(firstsearch)
	{
		searchDlg = create_dlgSearch();
		setupSearchDlg(searchDlg);
		firstsearch = FALSE;
		gtk_signal_connect (GTK_OBJECT (searchDlg), "destroy",
                      GTK_SIGNAL_FUNC (on_dlgSearch_destroy),
                      NULL);
	}
	gtk_widget_show (searchDlg); 	
}

//----------------------------------------------------------------------------------------------
void
on_fpStudypad_font_set                 (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data)
{
  //arg1.str;
  gnome_property_box_changed(GNOME_PROPERTY_BOX(gtk_widget_get_toplevel(GTK_WIDGET(gnomefontpicker))));
}

//----------------------------------------------------------------------------------------------
void
on_fpSPPrinter_font_set                (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnSearchSaveList_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *list, //-- pointer to resultList
    					*savedlg;//-- pointer to save file dialog


    list = lookup_widget(GTK_WIDGET(button),"resultList"); //-- set list to resultlist in search dialog
    savelistinfo(list); //-- send list widget to savelistinfo function in GnomeSword.cpp
}

//----------------------------------------------------------------------------------------------
void
on_btnVerseListNew_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnVerseListOpen_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnVerseListClose_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnVerseListPrint_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_verse_list1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{ 	
	GtkWidget *dlg;
	dlg = create_dlgVerseList();
	gtk_widget_show(dlg);
}


//----------------------------------------------------------------------------------------------
void
on_btnVerseListSave_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}


//----------------------------------------------------------------------------------------------
void
on_btnVerseListCopy_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnVerseListPaste_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_cbContext_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_verselist_select_row                (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_notebook3_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data)
{  	
	static bool firsttime = true;    //-- don't do anything if this is the first time here, but remember we were here - set firsttime to false
  if(!firsttime)
  {
    changepagenotebook(notebook,page_num); //-- send to changepagenotebook() function in GnomeSword.cpp
  }
  firsttime = false; //-- remember we were here
}

//----------------------------------------------------------------------------------------------
void
on_btnInfoBoxYes_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    answer = 0;
}

//----------------------------------------------------------------------------------------------
void
on_btnInfoBoxNo_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
    answer = 1;
}

//----------------------------------------------------------------------------------------------
void
on_kjv1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    showmoduleinfoSWORD((char*)user_data);
}

//----------------------------------------------------------------------------------------------
void
on_btnAboutModuleOK_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
 	GtkWidget	*dlg;

	dlg = gtk_widget_get_toplevel (GTK_WIDGET (button));	
	gtk_widget_destroy(dlg);
}

//----------------------------------------------------------------------------------------------
void
on_tree1_select_child                  (GtkTree         *tree,
                                        GtkWidget       *widget,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_tree1_selection_changed             (GtkTree         *tree,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_tree1_unselect_child                (GtkTree         *tree,
                                        GtkWidget       *widget,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEAddItem_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEDelete_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEsave_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEup_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEdown_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEleft_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEright_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEok_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEapply_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------
void
on_btnBMEcancel_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

}

//----------------------------------------------------------------------------------------------