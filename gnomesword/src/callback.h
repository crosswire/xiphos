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
	
#if USE_SHORTCUTBAR
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#endif /* USE_SHORTCUTBAR */
	
GtkWidget *create_menu1(void);
GtkWidget *create_mainwindow(void);
GtkWidget *create_wdwEditBookmarks(void);
GtkWidget *create_InfoBox(void);
GtkWidget *create_dlgSettings(void);
GtkWidget *create_dialog1(void);
	
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
gboolean on_moduleText_button_press_event(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer user_data);
gboolean on_textDict_button_press_event(GtkWidget      *widget,
					GdkEventButton *event,
                                        gpointer        user_data);
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
void on_fpMainwindow_font_set(GnomeFontPicker * gnomefontpicker,
				  GString arg1, gpointer user_data);
void on_fbCurrentverse_font_set(GnomeFontPicker * gnomefontpicker,
				    GString arg1, gpointer user_data);
void on_fbInerlinear_font_set(GnomeFontPicker * gnomefontpicker,
				  GString arg1, gpointer user_data);
void on_cpfgCurrentverse_color_set(GnomeColorPicker * gnomecolorpicker,
				       guint arg1,
				       guint arg2,
				       guint arg3,
				       guint arg4, gpointer user_data);
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
void on_fpStudypad_font_set(GnomeFontPicker * gnomefontpicker,
				GString arg1, gpointer user_data);
void on_btnSearchSaveList_clicked(GtkButton * button,
				      gpointer user_data);
void on_notebook3_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, gpointer user_data);
void on_btnInfoBoxYes_clicked(GtkButton * button, gpointer user_data);
void on_btnInfoBoxNo_clicked(GtkButton * button, gpointer user_data);
void on_btnPropertyboxOK_clicked(GtkButton * button,
				     gpointer user_data);
void on_btnPropertyboxApply_clicked(GtkButton * button,
					gpointer user_data);
void on_btnPropertyboxCancel_clicked(GtkButton * button,
					 gpointer user_data);
//==============================================================================================
void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_btnLEdelete_clicked(GtkButton * button, gpointer user_data);
void on_btnLEok_clicked(GtkButton * button, gpointer user_data);
void on_btnLEapply_clicked(GtkButton * button, gpointer user_data);
void on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);
void on_btnLEopen_clicked(GtkButton * button, gpointer user_data);
void on_btnLEclose_clicked(GtkButton * button, gpointer user_data);
void on_btnLEok_clicked(GtkButton * button, gpointer user_data);
void on_btnLEapply_clicked(GtkButton * button, gpointer user_data);
void on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);
void on_btnLEnew_clicked(GtkButton * button, gpointer user_data);
void on_btnLEopen_clicked(GtkButton * button, gpointer user_data);
void on_btnLErefresh_clicked(GtkButton * button, gpointer user_data);
void on_btnLEsave_clicked(GtkButton * button, gpointer user_data);
void on_tbtnFollow_toggled(GtkToggleButton * togglebutton,
			       gpointer user_data);
void on_cbtnPNformat_toggled(GtkToggleButton * togglebutton,
				 gpointer user_data);
gboolean on_cbeFreeformLookup_drag_drop(GtkWidget * widget,
					   GdkDragContext * drag_context,
					   gint x,
					   gint y,
					   guint time, gpointer user_data);
void on_moduleText_drag_begin(GtkWidget * widget,
				  GdkDragContext * drag_context,
				  gpointer user_data);
void on_textCommentaries_drag_begin(GtkWidget * widget,
					GdkDragContext * drag_context,
					gpointer user_data);
void on_textDict_drag_begin(GtkWidget * widget,
				GdkDragContext * drag_context,
				gpointer user_data);
void on_textComments_drag_begin(GtkWidget * widget,
				    GdkDragContext * drag_context,
				    gpointer user_data);
void on_textComments_drag_data_get(GtkWidget * widget,
				       GdkDragContext * drag_context,
				       GtkSelectionData * data,
				       guint info,
				       guint time, gpointer user_data);
void on_textComments_drag_data_received(GtkWidget * widget,
					    GdkDragContext * drag_context,
					    gint x,
					    gint y,
					    GtkSelectionData * data,
					    guint info,
					    guint time,
					    gpointer user_data);
gboolean on_textComments_drag_drop(GtkWidget * widget,
				      GdkDragContext * drag_context,
				      gint x,
				      gint y,
				      guint time, gpointer user_data);
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
void on_btnSB_clicked(GtkButton * button, gpointer user_data);
void on_cbtnShowSCB_toggled(GtkToggleButton * togglebutton,
				gpointer user_data);
void on_cbtnShowCOMtabs_toggled(GtkToggleButton * togglebutton,
				    gpointer user_data);
void on_cbtnShowDLtabs_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data);
#if USE_SHORTCUTBAR
void on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
				       GdkEvent * event,
				       gint group_num, gint item_num);
#endif /* USE_SHORTCUTBAR */
void on_com_select_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_dict_select_activate(GtkMenuItem * menuitem,
				 gpointer user_data);
void on_cbtnShowTextgroup_toggled(GtkToggleButton * togglebutton,
				      gpointer user_data);
void on_cbtnShowComGroup_toggled(GtkToggleButton * togglebutton,
				     gpointer user_data);
void on_cbtnShowDictGroup_toggled(GtkToggleButton * togglebutton,
				      gpointer user_data);
gboolean on_epaned_button_release_event(GtkWidget *widget,
		GdkEventButton  *event,
		gpointer  user_data);                   
void on_cbtnShowHistoryGroup_toggled(GtkToggleButton *togglebutton,
		gpointer user_data);
void on_btsText_clicked(GtkButton *button, gpointer user_data);
void on_btsComms_clicked                    (GtkButton       *button,
                                        gpointer         user_data);
void on_btsDicts_clicked                    (GtkButton       *button,
                                        gpointer         user_data);
void on_btsBookmarks_clicked                (GtkButton       *button,
                                        gpointer         user_data);
void on_btsHistory_clicked                  (GtkButton       *button,
                                        gpointer         user_data);
void on_btnClearHistory_clicked             (GtkButton       *button,
                                        gpointer         user_data);
void on_btsComms2_clicked                   (GtkButton       *button,
                                        gpointer         user_data);
void on_historybutton_clicked                     (GtkButton       *button,
                                        gpointer         user_data);
void on_btsDicts2_clicked                   (GtkButton       *button,
                                        gpointer         user_data);
void on_btsBookmarks2_clicked               (GtkButton       *button,
                                        gpointer         user_data);
void on_btsHistory2_clicked                 (GtkButton       *button,
                 			gpointer         user_data);
void on_textbutton_clicked                     (GtkButton       *button,
                                        gpointer         user_data);
void on_combutton_clicked                     (GtkButton       *button,
                                        gpointer         user_data);
void on_dictbutton_clicked                     (GtkButton       *button,
                                        gpointer         user_data);
void on_cbtnShowBookmarksGroup_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
void on_morphs_activate(GtkMenuItem * menuitem, gpointer user_data);


#ifdef __cplusplus
}
#endif

