/***************************************************************************
                          callback.h  -  description
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
#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

void on_help_contents_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_nbTextMods_switch_page(GtkNotebook * notebook,	
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data);
void on_mnuHistoryitem1_activate(GtkMenuItem * menuitem, 
		gpointer user_data);
void on_ok_button1_clicked(GtkButton * button, 
		gpointer user_data);
void on_cancel_button1_clicked(GtkButton * button, 
		gpointer user_data);
void on_ok_button2_clicked(GtkButton * button, 
		gpointer user_data);
void on_cancel_button2_clicked(GtkButton * button, gpointer user_data);
void on_btnSearch1_clicked(GtkButton * button, gpointer user_data);
void on_resultList_select_row(GtkCList * clist, gint row,
		gint column, GdkEvent * event,
		gpointer user_data);
void on_mainwindow_destroy(GtkObject * object, gpointer user_data);
void on_listeditor_destroy(GtkObject * object, gpointer user_data);
void on_dlgSearch_destroy(GtkObject * object, gpointer user_data);
void on_btnSearch_clicked(GtkButton * button, gpointer user_data);
void on_btnStrongs_toggled(GtkToggleButton * togglebutton,
		gpointer user_data);
void on_add_bookmark1_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_edit_bookmarks1_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_verse_style1_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_show_interlinear_page1_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_cbeBook_changed(GtkEditable * editable, gpointer user_data);
gboolean on_spbChapter_button_release_event(GtkWidget * widget,
					       GdkEventButton * event,
					       gpointer user_data);
gboolean on_spbVerse_button_release_event(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer user_data);
void on_btnLookup_clicked(GtkButton * button, gpointer user_data);
gboolean on_cbeFreeformLookup_key_press_event(GtkWidget * widget,
						 GdkEventKey * event,
						 gpointer user_data);
void on_btnExit_clicked(GtkButton * button, gpointer user_data);
void on_notebook1_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, gpointer user_data);
void on_textComments_changed(GtkEditable * editable,
				 gpointer user_data);
gboolean on_textComments_button_release_event(GtkWidget * widget,
						 GdkEventButton * event,
						 gpointer user_data);
void on_btnEditNote_toggled(GtkToggleButton * togglebutton,
				gpointer user_data);
void on_btnSaveNote_clicked(GtkButton * button, gpointer user_data);
void on_btnDeleteNote_clicked(GtkButton * button, gpointer user_data);
void on_notebook4_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, gpointer user_data);
void on_dictionarySearchText_changed(GtkEditable * editable,
					 gpointer user_data);
void on_list1_select_row(GtkCList * clist,
			     gint row,
			     gint column,
			     GdkEvent * event, gpointer user_data);
void on_btnSpell_clicked(GtkButton * button, gpointer user_data);
void on_btbSpellOK_clicked(GtkButton * button, gpointer user_data);
void on_btnSpellCancel_clicked(GtkButton * button, gpointer user_data);
void on_btnSearchOK_clicked(GtkButton * button, gpointer user_data);
void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_edit_bookmarks1_activate(GtkMenuItem * menuitem,
				     gpointer user_data);
void on_clear1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_main_module1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);
void on_item1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_about_gnomesword1_activate(GtkMenuItem * menuitem,
				       gpointer user_data);
void on_item1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_strongs_numbers1_activate(GtkMenuItem * menuitem,
				      gpointer user_data);
void on_footnotes1_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_item1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_copy2_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_copy3_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_preferences1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);
void on_btnSpellCancel_clicked(GtkButton * button, gpointer user_data);
void on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
					      gpointer user_data);
void on_spell1_found_word(GnomeSpell * gnomespell,
			      gpointer arg1, gpointer user_data);
void on_spell1_handled_word(GnomeSpell * gnomespell,
				gpointer arg1, gpointer user_data);
void on_btndlg1OK_clicked(GtkButton * button, gpointer user_data);
void on_btndlg1Apply_clicked(GtkButton * button, gpointer user_data);
void on_btndialog1Cancel_clicked(GtkButton * button,
				     gpointer user_data);
void on_btnSpellNotes_clicked(GtkButton * button, gpointer user_data);
void on_btnSpellNotes_clicked(GtkButton * button, gpointer user_data);
void on_auto_save_notes1_activate(GtkMenuItem * menuitem,
				      gpointer user_data);
void on_btnBack_clicked(GtkButton * button, gpointer user_data);
void on_change_module_activate(GtkMenuItem * menuitem,
				   gpointer user_data);
void on_btnKeyPrev_clicked(GtkButton * button, gpointer user_data);
void on_btnKeyNext_clicked(GtkButton * button, gpointer user_data);
void on_search1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_btnSearchSaveList_clicked(GtkButton * button,
				      gpointer user_data);
void on_notebook3_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, gpointer user_data);
void on_btnInfoBoxYes_clicked(GtkButton * button, gpointer user_data);
void on_btnInfoBoxNo_clicked(GtkButton * button, gpointer user_data);
//==============================================================================================
void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_tbtnFollow_toggled(GtkToggleButton * togglebutton,
			       gpointer user_data);

gboolean on_textComments_key_press_event(GtkWidget * widget,
					    GdkEventKey * event,
					    gpointer user_data);
gboolean on_moduleText_enter_notify_event(GtkWidget * widget,
					     GdkEventCrossing * event,
					     gpointer user_data);
void on_btnBack_clicked(GtkButton * button, gpointer user_data);
void on_btnFoward_clicked(GtkButton * button, gpointer user_data);
void on_btnComPrev_clicked(GtkButton * button, gpointer user_data);
void on_btnComNext_clicked(GtkButton * button, gpointer user_data);
gboolean on_dictionarySearchText_key_press_event
	    (GtkWidget * widget, GdkEventKey * event, gpointer user_data);
gboolean on_list1_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer user_data);
void on_com_select_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_dict_select_activate(GtkMenuItem * menuitem,
				 gpointer user_data);
gboolean on_epaned_button_release_event(GtkWidget *widget,
		GdkEventButton  *event,
		gpointer  user_data);   
void on_morphs_activate(GtkMenuItem * menuitem, gpointer user_data);


#ifdef __cplusplus
}
#endif

