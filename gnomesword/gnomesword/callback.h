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
#include <gnome.h>
#include <swmodule.h>


void
on_mnuHistoryitem1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ok_button1_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_button1_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_ok_button2_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_button2_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnAboutOK_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSearch1_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_resultList_select_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_dictionary_lookup1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy_verse1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_btnOptionsOK_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnOptionsCancel_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnOptionsHelp_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_mainwindow_destroy                  (GtkObject       *object,
                                        gpointer         user_data);
void
on_dlgSearch_destroy                  	(GtkObject       *object,
                                        	gpointer         user_data);
void
on_btnSearch_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnStrongs_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_add_bookmark1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_edit_bookmarks1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_john_3_1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_romans_1_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_luke_1_1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_view_item1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_footnotes_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainText_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_1st_interlinear_window1_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_2nd_interlinear_window1_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item3_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_3rd_interlinear_window1_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item4_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_verse_style1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_show_interlinear_page1_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_cbBook_button_release_event         (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

GtkDirectionType
on_cbBook_focus                        (GtkContainer    *container,
                                        GtkDirectionType direction,
                                        gpointer         user_data);

void
on_cbeBook_changed                     (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spbChapter_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spbChapter_activate                 (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_spbChapter_button_release_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_spbVerse_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spbVerse_activate                   (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_spbVerse_button_release_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_btnLookup_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_cbeFreeformLookup_key_press_event   (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

gboolean
on_arrow1_button_press_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_btnExit_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_moduleText_button_press_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_notebook1_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data);

void
on_textComments_changed                (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_textComments_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_btnEditNote_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_btnSaveNote_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnDeleteNote_clicked               (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_textComp1_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_textComp2_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_textComp3_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_btnOpenFile_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSaveFile_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSaveFileAs_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnPrint_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnCut_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnCopy_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnPaste_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_text3_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_notebook4_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data);

gboolean
on_moduleText_selection_notify_event   (GtkWidget       *widget,
                                        GdkEventSelection *event,
                                        gpointer         user_data);

void
on_moduleText_selection_received       (GtkWidget       *widget,
                                        GtkSelectionData *data,
                                        guint            time,
                                        gpointer         user_data);

void
on_dictionarySearchText_changed        (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_list1_select_row                    (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_item51_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item52_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item53_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_btnSpell_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_btbSpellOK_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSpellApply_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSpellCancel_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSearchOK_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_find1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_add_bookmark1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_edit_bookmarks1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_john_3_1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_clear1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_module1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_inter1_module1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about_gnomesword1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item3_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item4_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_strongs_numbers1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_footnotes1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy3_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_propertybox1_clicked                (GnomePropertyBox *gnomepropertybox,
                                        gint             arg1,
                                        gpointer         user_data);


void
on_fontpicker2_font_set                (GnomeFontPicker *gnomefontpicker,
                                        string        arg1,
                                        gpointer         user_data);

void
on_colorpicker5_color_set              (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data);

void
on_colorpicker1_color_set              (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data);

void
on_colorpicker2_color_set              (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data);

void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_suggestions_select_row              (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_btnSpellOK_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSpellApply_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSpellCancel_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnEBMcut_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnEBMcopy_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnEBMpaste_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_text4_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_btnEBMOK_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnEBMApply_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnEBMCancel_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_add_bookmark2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_edit_bookmarks_1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cbusebounds_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_cpfgMainwindow_color_set            (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data);

void
on_fpMainwindow_font_set               (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data);

void
on_fbCurrentverse_font_set             (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data);

void
on_fbInerlinear_font_set               (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data);

void
on_cpfgCurrentverse_color_set          (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data);

void
on_cpfgInterlinear_color_set           (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data);

void
on_cpbgMainwindow_color_set            (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data);

void
on_cpbgInterlinear_color_set           (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data);

void
on_cpbgCurrentverse_color_set          (GnomeColorPicker *gnomecolorpicker,
                                        guint            arg1,
                                        guint            arg2,
                                        guint            arg3,
                                        guint            arg4,
                                        gpointer         user_data);

void
on_about_the_sword_project1_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_btnAboutSwordOK_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_spell1_found_word                   (GnomeSpell      *gnomespell,
                                        gpointer         arg1,
                                        gpointer         user_data);

void
on_spell1_handled_word                 (GnomeSpell      *gnomespell,
                                        gpointer         arg1,
                                        gpointer         user_data);

void
on_btndlg1OK_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_btndlg1Apply_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_btndialog1Cancel_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSpellNotes_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnSpellNotes_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_fpMainwindowitalic_font_set         (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data);

void
save_changes_handler         					(int reply, gpointer data);

void
on_auto_save_notes1_activate			(GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_btnBack_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy4_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_change_module_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_fbMainwindowbold_font_set           (GnomeFontPicker *gnomefontpicker,
                                        GString        arg1,
                                        gpointer         user_data);

void
on_btnKeyPrev_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnKeyNext_clicked                  (GtkButton       *button,
                                        gpointer         user_data);
void
on_search1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
