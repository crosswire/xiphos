/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_gui_cb.h
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

#ifndef __GS_GUI_CB_H_
#define __GS_GUI_CB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

	void on_mainwindow_size_allocate(GtkWidget * widget,
					 GtkAllocation * allocation,
					 gpointer user_data);
	void on_nbTextMods_switch_page(GtkNotebook * notebook,
				       GtkNotebookPage * page,
				       gint page_num, gpointer user_data);
	void on_ok_button1_clicked(GtkButton * button, gpointer user_data);
	void on_cancel_button1_clicked(GtkButton * button,
				       gpointer user_data);
	void on_ok_button2_clicked(GtkButton * button, gpointer user_data);
	void on_cancel_button2_clicked(GtkButton * button,
				       gpointer user_data);
	void on_btnSearch1_clicked(GtkButton * button, gpointer user_data);
	void on_resultList_select_row(GtkCList * clist, gint row,
				      gint column, GdkEvent * event,
				      gpointer user_data);
	void on_mainwindow_destroy(GtkObject * object, gpointer user_data);
	void on_dlgSearch_destroy(GtkObject * object, gpointer user_data);
	void on_btnSearch_clicked(GtkButton * button, gpointer user_data);
	void on_btnStrongs_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data);
	void on_btnMorphs_toggled(GtkToggleButton * togglebutton,
				  gpointer user_data);
	void on_btnFootnotes_toggled(GtkToggleButton * togglebutton,
				     gpointer user_data);
	void on_cbeBook_changed(GtkEditable * editable,
				gpointer user_data);
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
	void on_notebook1_switch_page(GtkNotebook * notebook,
				      GtkNotebookPage * page,
				      gint page_num, gpointer user_data);
	void on_textComments_changed(GtkEditable * editable,
				     gpointer user_data);
	gboolean on_textComments_button_release_event(GtkWidget * widget,
						      GdkEventButton *
						      event,
						      gpointer user_data);
	void on_btnEditNote_toggled(GtkToggleButton * togglebutton,
				    gpointer user_data);
	void on_btnSaveNote_clicked(GtkButton * button,
				    gpointer user_data);
	void on_btnDeleteNote_clicked(GtkButton * button,
				      gpointer user_data);
	void on_btnSpell_clicked(GtkButton * button, gpointer user_data);
	void on_btbSpellOK_clicked(GtkButton * button, gpointer user_data);
	void on_btnSpellCancel_clicked(GtkButton * button,
				       gpointer user_data);
	void on_btnSearchOK_clicked(GtkButton * button,
				    gpointer user_data);
	void on_btnSpellCancel_clicked(GtkButton * button,
				       gpointer user_data);
	void on_spell1_found_word(GnomeSpell * gnomespell, gpointer arg1,
				  gpointer user_data);
	void on_spell1_handled_word(GnomeSpell * gnomespell, gpointer arg1,
				    gpointer user_data);
	void on_btndlg1OK_clicked(GtkButton * button, gpointer user_data);
	void on_btndlg1Apply_clicked(GtkButton * button,
				     gpointer user_data);
	void on_btndialog1Cancel_clicked(GtkButton * button,
					 gpointer user_data);
	void on_btnBack_clicked(GtkButton * button, gpointer user_data);
	void on_btnSearchSaveList_clicked(GtkButton * button,
					  gpointer user_data);
	void on_notebook3_switch_page(GtkNotebook * notebook,
				      GtkNotebookPage * page,
				      gint page_num, gpointer user_data);
	void on_btnInfoBoxYes_clicked(GtkButton * button,
				      gpointer user_data);
	void on_btnInfoBoxNo_clicked(GtkButton * button,
				     gpointer user_data);
	void on_tbtnFollow_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data);
	gboolean on_textComments_key_press_event(GtkWidget * widget,
						 GdkEventKey * event,
						 gpointer user_data);
	void on_btnBack_clicked(GtkButton * button, gpointer user_data);
	void on_btnFoward_clicked(GtkButton * button, gpointer user_data);
	gboolean on_epaned_button_release_event(GtkWidget * widget,
						GdkEventButton * event,
						gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif				/* __GS_GUI_CB_H_ */
