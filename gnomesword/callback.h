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
#include  <widgets/shortcut-bar/e-shortcut-bar.h>

GtkWidget *create_fileselection1(void);
GtkWidget *create_fileselectionSave(void);
GtkWidget *create_menu1(void);
GtkWidget *create_mainwindow(void);
GtkWidget *create_about2(void);
GtkWidget *create_wdwEditBookmarks(void);
GtkWidget *create_InfoBox(void);
GtkWidget *create_AboutSword(void);
GtkWidget *create_aboutmodules(void);
GtkWidget *create_dlgSettings(void);
GtkWidget *create_dialog1(void);
GtkWidget *create_listeditor(void);

void on_mnuHistoryitem1_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_ok_button1_clicked(GtkButton * button, gpointer user_data);
void on_cancel_button1_clicked(GtkButton * button, gpointer user_data);
void on_ok_button2_clicked(GtkButton * button, gpointer user_data);

void
on_ok_button4_clicked(GtkButton * button, gpointer user_data);

void
on_cancel_button2_clicked(GtkButton * button, gpointer user_data);

void
 on_btnAboutOK_clicked(GtkButton * button, gpointer user_data);

void on_btnSearch1_clicked(GtkButton * button, gpointer user_data);
void on_resultList_select_row(GtkCList * clist, gint row,
				      gint column, GdkEvent * event,
				      gpointer user_data);

void
 on_copy_verse1_activate(GtkMenuItem * menuitem,
				 gpointer user_data);

void
 on_mainwindow_destroy(GtkObject * object, gpointer user_data);
void
 on_listeditor_destroy(GtkObject * object, gpointer user_data);
void
 on_dlgSearch_destroy(GtkObject * object, gpointer user_data);
void
 on_btnSearch_clicked(GtkButton * button, gpointer user_data);

void
 on_btnStrongs_toggled(GtkToggleButton * togglebutton,
			       gpointer user_data);

void
 on_add_bookmark1_activate(GtkMenuItem * menuitem,
				   gpointer user_data);

void
 on_edit_bookmarks1_activate(GtkMenuItem * menuitem,
				     gpointer user_data);

void
 on_john_3_1_activate(GtkMenuItem * menuitem, gpointer user_data);

void
 on_footnotes_activate(GtkMenuItem * menuitem, gpointer user_data);

void
 on_mainText_activate(GtkMenuItem * menuitem, gpointer user_data);

void
 on_1st_interlinear_window1_activate(GtkMenuItem * menuitem,
					     gpointer user_data);

void
 on_2nd_interlinear_window1_activate(GtkMenuItem * menuitem,
					     gpointer user_data);

void
 on_3rd_interlinear_window1_activate(GtkMenuItem * menuitem,
					     gpointer user_data);

void
 on_verse_style1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);

void
 on_show_interlinear_page1_activate(GtkMenuItem * menuitem,
					    gpointer user_data);

void
 on_cbeBook_changed(GtkEditable * editable, gpointer user_data);

gboolean
 on_spbChapter_button_release_event(GtkWidget * widget,
					       GdkEventButton * event,
					       gpointer user_data);

gboolean
 on_spbVerse_button_release_event(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer user_data);

void
 on_btnLookup_clicked(GtkButton * button, gpointer user_data);

gboolean
 on_cbeFreeformLookup_key_press_event(GtkWidget * widget,
						 GdkEventKey * event,
						 gpointer user_data);

void
 on_btnExit_clicked(GtkButton * button, gpointer user_data);

gboolean
 on_moduleText_button_press_event(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer user_data);

gboolean
on_textDict_button_press_event(GtkWidget      *widget,
					GdkEventButton *event,
                                        gpointer        user_data);

void
 on_notebook1_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, gpointer user_data);

void
 on_textComments_changed(GtkEditable * editable,
				 gpointer user_data);

gboolean
on_textComments_button_release_event(GtkWidget * widget,
						 GdkEventButton * event,
						 gpointer user_data);

void
 on_btnEditNote_toggled(GtkToggleButton * togglebutton,
				gpointer user_data);

void
 on_btnSaveNote_clicked(GtkButton * button, gpointer user_data);

void
 on_btnDeleteNote_clicked(GtkButton * button, gpointer user_data);

gboolean
 on_textComp1_button_press_event(GtkWidget * widget,
					    GdkEventButton * event,
					    gpointer user_data);

gboolean
 on_textComp2_button_press_event(GtkWidget * widget,
					    GdkEventButton * event,
					    gpointer user_data);

gboolean
 on_textComp3_button_press_event(GtkWidget * widget,
					    GdkEventButton * event,
					    gpointer user_data);

void
 on_btnOpenFile_clicked(GtkButton * button, gpointer user_data);

void
 on_btnSaveFile_clicked(GtkButton * button, gpointer user_data);

void
 on_btnSaveFileAs_clicked(GtkButton * button, gpointer user_data);

void
 on_btnPrint_clicked(GtkButton * button, gpointer user_data);

void
 on_btnCut_clicked(GtkButton * button, gpointer user_data);

void
 on_btnCopy_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnPaste_clicked(GtkButton * button, gpointer user_data);

	void
	 on_text3_changed(GtkEditable * editable, gpointer user_data);

	void
	 on_notebook4_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, gpointer user_data);

	 gboolean
	    on_moduleText_selection_notify_event(GtkWidget * widget,
						 GdkEventSelection * event,
						 gpointer user_data);

	void
	 on_moduleText_selection_received(GtkWidget * widget,
					  GtkSelectionData * data,
					  guint time, gpointer user_data);

	void
	 on_dictionarySearchText_changed(GtkEditable * editable,
					 gpointer user_data);

	void
	 on_list1_select_row(GtkCList * clist,
			     gint row,
			     gint column,
			     GdkEvent * event, gpointer user_data);


	void
	 on_btnSpell_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btbSpellOK_clicked(GtkButton * button, gpointer user_data);
	void
	 on_btnSpellCancel_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnSearchOK_clicked(GtkButton * button, gpointer user_data);

	void
	 on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_add_bookmark1_activate(GtkMenuItem * menuitem,
				   gpointer user_data);

	void
	 on_edit_bookmarks1_activate(GtkMenuItem * menuitem,
				     gpointer user_data);

	void
	 on_john_3_1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_clear1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_main_module1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);

	void
	 on_item1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_about_gnomesword1_activate(GtkMenuItem * menuitem,
				       gpointer user_data);

	void
	 on_item1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_save1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_save_as1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_strongs_numbers1_activate(GtkMenuItem * menuitem,
				      gpointer user_data);

	void
	 on_footnotes1_activate(GtkMenuItem * menuitem,
				gpointer user_data);

	void
	 on_item1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_copy2_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_copy3_activate(GtkMenuItem * menuitem, gpointer user_data);


	void
	 on_fontpicker2_font_set(GnomeFontPicker * gnomefontpicker,
				 GString arg1, gpointer user_data);

	void
	 on_colorpicker5_color_set(GnomeColorPicker * gnomecolorpicker,
				   guint arg1,
				   guint arg2,
				   guint arg3,
				   guint arg4, gpointer user_data);

	void
	 on_colorpicker1_color_set(GnomeColorPicker * gnomecolorpicker,
				   guint arg1,
				   guint arg2,
				   guint arg3,
				   guint arg4, gpointer user_data);

	void
	 on_colorpicker2_color_set(GnomeColorPicker * gnomecolorpicker,
				   guint arg1,
				   guint arg2,
				   guint arg3,
				   guint arg4, gpointer user_data);

	void
	 on_preferences1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);

	void
	 on_suggestions_select_row(GtkCList * clist,
				   gint row,
				   gint column,
				   GdkEvent * event, gpointer user_data);

	void
	 on_btnSpellOK_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnSpellCancel_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnEBMcut_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnEBMcopy_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnEBMpaste_clicked(GtkButton * button, gpointer user_data);

	void
	 on_text4_changed(GtkEditable * editable, gpointer user_data);

	void
	 on_btnEBMOK_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnEBMApply_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnEBMCancel_clicked(GtkButton * button, gpointer user_data);

	void
	 on_add_bookmark2_activate(GtkMenuItem * menuitem,
				   gpointer user_data);

	void
	 on_edit_bookmarks_1_activate(GtkMenuItem * menuitem,
				      gpointer user_data);

	void
	 on_cbusebounds_toggled(GtkToggleButton * togglebutton,
				gpointer user_data);

	void
	 on_cpfgMainwindow_color_set(GnomeColorPicker * gnomecolorpicker,
				     guint arg1,
				     guint arg2,
				     guint arg3,
				     guint arg4, gpointer user_data);

	void
	 on_fpMainwindow_font_set(GnomeFontPicker * gnomefontpicker,
				  GString arg1, gpointer user_data);

	void
	 on_fbCurrentverse_font_set(GnomeFontPicker * gnomefontpicker,
				    GString arg1, gpointer user_data);

	void
	 on_fbInerlinear_font_set(GnomeFontPicker * gnomefontpicker,
				  GString arg1, gpointer user_data);

	void
	 on_cpfgCurrentverse_color_set(GnomeColorPicker * gnomecolorpicker,
				       guint arg1,
				       guint arg2,
				       guint arg3,
				       guint arg4, gpointer user_data);

	void
	 on_cpfgInterlinear_color_set(GnomeColorPicker * gnomecolorpicker,
				      guint arg1,
				      guint arg2,
				      guint arg3,
				      guint arg4, gpointer user_data);

	void
	 on_cpbgMainwindow_color_set(GnomeColorPicker * gnomecolorpicker,
				     guint arg1,
				     guint arg2,
				     guint arg3,
				     guint arg4, gpointer user_data);

	void
	 on_cpbgInterlinear_color_set(GnomeColorPicker * gnomecolorpicker,
				      guint arg1,
				      guint arg2,
				      guint arg3,
				      guint arg4, gpointer user_data);

	void
	 on_cpbgCurrentverse_color_set(GnomeColorPicker * gnomecolorpicker,
				       guint arg1,
				       guint arg2,
				       guint arg3,
				       guint arg4, gpointer user_data);

	void
	 on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
					      gpointer user_data);

	void
	 on_btnAboutSwordOK_clicked(GtkButton * button,
				    gpointer user_data);

	void
	 on_spell1_found_word(GnomeSpell * gnomespell,
			      gpointer arg1, gpointer user_data);

	void
	 on_spell1_handled_word(GnomeSpell * gnomespell,
				gpointer arg1, gpointer user_data);

	void
	 on_btndlg1OK_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btndlg1Apply_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btndialog1Cancel_clicked(GtkButton * button,
				     gpointer user_data);

	void
	 on_btnSpellNotes_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnSpellNotes_clicked(GtkButton * button, gpointer user_data);

	void
	 on_fpMainwindowitalic_font_set(GnomeFontPicker * gnomefontpicker,
					GString arg1, gpointer user_data);

	void
	 save_changes_handler(int reply, gpointer data);

	void
	 on_auto_save_notes1_activate(GtkMenuItem * menuitem,
				      gpointer user_data);
	void
	 on_btnBack_clicked(GtkButton * button, gpointer user_data);

	void
	 on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_copy4_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_change_module_activate(GtkMenuItem * menuitem,
				   gpointer user_data);
	void
	 on_fbMainwindowbold_font_set(GnomeFontPicker * gnomefontpicker,
				      GString arg1, gpointer user_data);

	void
	 on_btnKeyPrev_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnKeyNext_clicked(GtkButton * button, gpointer user_data);
	void
	 on_search1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_fpStudypad_font_set(GnomeFontPicker * gnomefontpicker,
				GString arg1, gpointer user_data);

	void
	 on_fpSPPrinter_font_set(GnomeFontPicker * gnomefontpicker,
				 GString arg1, gpointer user_data);

	void
	 on_btnSearchSaveList_clicked(GtkButton * button,
				      gpointer user_data);

	void
	 on_btnVerseListNew_clicked(GtkButton * button,
				    gpointer user_data);

	void
	 on_btnVerseListOpen_clicked(GtkButton * button,
				     gpointer user_data);

	void
	 on_btnVerseListSave_clicked(GtkButton * button,
				     gpointer user_data);

	void
	 on_btnVerseListPrint_clicked(GtkButton * button,
				      gpointer user_data);

	void
	 on_btnVerseListClose_clicked(GtkButton * button,
				      gpointer user_data);

	void
	 on_verse_list1_activate(GtkMenuItem * menuitem,
				 gpointer user_data);

	void
	 on_btnVerseListOpen_clicked(GtkButton * button,
				     gpointer user_data);

	void
	 on_btnVerseListSave_clicked(GtkButton * button,
				     gpointer user_data);

	void
	 on_btnVerseListPrint_clicked(GtkButton * button,
				      gpointer user_data);

	void
	 on_btnVerseListCopy_clicked(GtkButton * button,
				     gpointer user_data);

	void
	 on_btnVerseListPaste_clicked(GtkButton * button,
				      gpointer user_data);

	void
	 on_cbContext_toggled(GtkToggleButton * togglebutton,
			      gpointer user_data);

	void
	 on_verselist_select_row(GtkCList * clist,
				 gint row,
				 gint column,
				 GdkEvent * event, gpointer user_data);

	void
	 on_notebook3_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, gpointer user_data);

	void
	 on_btnInfoBoxYes_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnInfoBoxNo_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnAboutSwordOK_clicked(GtkButton * button,
				    gpointer user_data);

	void
	 on_kjv1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_btnAboutModuleOK_clicked(GtkButton * button,
				     gpointer user_data);

	void
	 on_btnAboutModuleOK_clicked(GtkButton * button,
				     gpointer user_data);

	void
	 on_tree1_select_child(GtkTree * tree,
			       GtkWidget * widget, gpointer user_data);

	void
	 on_tree1_selection_changed(GtkTree * tree, gpointer user_data);

	void
	 on_tree1_unselect_child(GtkTree * tree,
				 GtkWidget * widget, gpointer user_data);

	void
	 on_btnBMEAddItem_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnBMEDelete_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnBMEsave_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnBMEup_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnBMEdown_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnBMEleft_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnBMEright_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnBMEok_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnBMEapply_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnBMEcancel_clicked(GtkButton * button, gpointer user_data);

	void
	 on_about_this_module1_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_about_this_module2_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_about_this_module3_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_about_this_module4_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_btnPropertyboxOK_clicked(GtkButton * button,
				     gpointer user_data);

	void
	 on_btnPropertyboxApply_clicked(GtkButton * button,
					gpointer user_data);

	void
	 on_btnPropertyboxCancel_clicked(GtkButton * button,
					 gpointer user_data);

	void
	 on_about_this_module1_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_about_this_module2_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_about_this_module3_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_about_this_module4_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_clLElist_select_row(GtkCList * clist,
				gint row,
				gint column,
				GdkEvent * event, gpointer user_data);

	void
	 on_btnLEup_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEdown_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEleft_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEright_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEgotoverse_clicked(GtkButton * button, gpointer user_data);

	void
	 on_cbLEBook_changed(GtkEditable * editable, gpointer user_data);

	void
	 on_spLEChapter_changed(GtkEditable * editable,
				gpointer user_data);

	void
	 on_spLEVerse_changed(GtkEditable * editable, gpointer user_data);

	void
	 on_btnLEAddVerse_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEAddItem_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEDelete_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEClose_clicked(GtkButton * button, gpointer user_data);

	void
	 on_clLElist_select_row(GtkCList * clist,
				gint row,
				gint column,
				GdkEvent * event, gpointer user_data);

	void
	 on_btnLEmakesub_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEok_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEapply_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);

	void
	 on_entryPreitem_changed(GtkEditable * editable,
				 gpointer user_data);

	void
	 on_btnLEapplylistchanges_clicked(GtkButton * button,
					  gpointer user_data);

	void
	 on_btnLEcancelistchanges_clicked(GtkButton * button,
					  gpointer user_data);

	void
	 on_btnLEok_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEapply_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);

//==============================================================================================


	void
	 on_new1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_open1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_save1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_save_as1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_close1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_btnLEup1_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEdown1_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEaddmenu_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEdelete_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEok_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEapply_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEnew_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEopen_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEclose_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEup1_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEdown1_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEup2_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEdown2_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEup3_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEdown3_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEok_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEapply_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEnew_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEopen_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEclose_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEprev_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEnext_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEprev_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEnext_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEprev_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEnext_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEclose_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEprev_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEnext_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEclose_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEprev_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEnext_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEclose_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLErefresh_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEsave_clicked(GtkButton * button, gpointer user_data);

	void
	 on_tbtnFollow_toggled(GtkToggleButton * togglebutton,
			       gpointer user_data);

	void
	 on_btnSPnew_clicked(GtkButton * button, gpointer user_data);


	void
	 on_cbtnPNformat_toggled(GtkToggleButton * togglebutton,
				 gpointer user_data);


	 gboolean
	    on_cbeFreeformLookup_drag_drop(GtkWidget * widget,
					   GdkDragContext * drag_context,
					   gint x,
					   gint y,
					   guint time, gpointer user_data);

	void
	 on_moduleText_drag_begin(GtkWidget * widget,
				  GdkDragContext * drag_context,
				  gpointer user_data);

	void
	 on_textCommentaries_drag_begin(GtkWidget * widget,
					GdkDragContext * drag_context,
					gpointer user_data);

	void
	 on_textDict_drag_begin(GtkWidget * widget,
				GdkDragContext * drag_context,
				gpointer user_data);


	void
	 on_boldNE_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_italicNE_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_referenceNE_activate(GtkMenuItem * menuitem,
				 gpointer user_data);

	void
	 on_underlineNE_activate(GtkMenuItem * menuitem,
				 gpointer user_data);

	void
	 on_greekNE_activate(GtkMenuItem * menuitem, gpointer user_data);

	void
	 on_goto_reference_activate(GtkMenuItem * menuitem,
				    gpointer user_data);


	void
	 on_goto_reference2_activate(GtkMenuItem * menuitem,
				     gpointer user_data);


	void
	 on_textComments_drag_begin(GtkWidget * widget,
				    GdkDragContext * drag_context,
				    gpointer user_data);

	void
	 on_textComments_drag_data_get(GtkWidget * widget,
				       GdkDragContext * drag_context,
				       GtkSelectionData * data,
				       guint info,
				       guint time, gpointer user_data);

	void
	 on_textComments_drag_data_received(GtkWidget * widget,
					    GdkDragContext * drag_context,
					    gint x,
					    gint y,
					    GtkSelectionData * data,
					    guint info,
					    guint time,
					    gpointer user_data);

	 gboolean
	    on_textComments_drag_drop(GtkWidget * widget,
				      GdkDragContext * drag_context,
				      gint x,
				      gint y,
				      guint time, gpointer user_data);

	 gboolean
	    on_textComments_key_press_event(GtkWidget * widget,
					    GdkEventKey * event,
					    gpointer user_data);

	void
	 on_lookup_selection2_activate(GtkMenuItem * menuitem,
				       gpointer user_data);

	void
	 on_about_this_module6_activate(GtkMenuItem * menuitem,
					gpointer user_data);


	 gboolean
	    on_moduleText_enter_notify_event(GtkWidget * widget,
					     GdkEventCrossing * event,
					     gpointer user_data);

	void
	 on_lookup_word_1_activate(GtkMenuItem * menuitem,
				   gpointer user_data);

	void
	 on_about_this_module5_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_lookup_word1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);

	void
	 on_btnBack_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnFoward_clicked(GtkButton * button, gpointer user_data);


	void
	 on_lookup_selection_activate(GtkMenuItem * menuitem,
				      gpointer user_data);


	void
	 on_about_this_module6_activate(GtkMenuItem * menuitem,
					gpointer user_data);

	void
	 on_btnComPrev_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnComNext_clicked(GtkButton * button, gpointer user_data);

	void
	 on_auto_scroll1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);

	void
	 on_lookup_selection4_activate(GtkMenuItem * menuitem,
				       gpointer user_data);

	void
	 on_goto_reference3_activate(GtkMenuItem * menuitem,
				     gpointer user_data);

	 gboolean
	    on_dictionarySearchText_key_press_event
	    (GtkWidget * widget, GdkEventKey * event, gpointer user_data);

	 gboolean
	    on_list1_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer user_data);

	void
	 on_btnSB_clicked(GtkButton * button, gpointer user_data);

	void
	 on_cbtnShowSCB_toggled(GtkToggleButton * togglebutton,
				gpointer user_data);

	void
	 on_cbtnShowCOMtabs_toggled(GtkToggleButton * togglebutton,
				    gpointer user_data);

	void
	 on_cbtnShowDLtabs_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data);

	void
	 on_show_tabs1_activate(GtkMenuItem * menuitem,
				gpointer user_data);

	void
	 on_show_tabs2_activate(GtkMenuItem * menuitem,
				gpointer user_data);


	void
	 on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
				       GdkEvent * event,
				       gint group_num, gint item_num);

	void
	 on_com_select_activate(GtkMenuItem * menuitem,
				gpointer user_data);

	void
	 on_dict_select_activate(GtkMenuItem * menuitem,
				 gpointer user_data);


	void
	 on_cbtnShowTextgroup_toggled(GtkToggleButton * togglebutton,
				      gpointer user_data);

	void
	 on_cbtnShowComGroup_toggled(GtkToggleButton * togglebutton,
				     gpointer user_data);

	void
	 on_cbtnShowDictGroup_toggled(GtkToggleButton * togglebutton,
				      gpointer user_data);
	
	
        gboolean
        on_epaned_button_release_event(GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);
                   
void
on_cbtnShowHistoryGroup_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

#ifdef __cplusplus
}
#endif
