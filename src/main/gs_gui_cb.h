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
#include "gs_gnomesword.h"

void on_mainwindow_size_allocate(GtkWidget * widget,
				 GtkAllocation * allocation,
				 gpointer user_data);
void on_mainwindow_destroy(GtkObject * object, SETTINGS * s);
void on_btn_search_clicked(GtkButton * button, gpointer user_data);
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
void on_notebook3_switch_page(GtkNotebook * notebook,
			      GtkNotebookPage * page,
			      gint page_num, gpointer user_data);
void on_btnBack_clicked(GtkButton * button, gpointer user_data);
void on_btnFoward_clicked(GtkButton * button, gpointer user_data);
gboolean on_epaned_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif				/* __GS_GUI_CB_H_ */
