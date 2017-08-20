/*
 * Xiphos Bible Study Tool
 * mod_mgr.h
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __GUI_MOD_MGR_H__
#define __GUI_MOD_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "main/mod_mgr.h"
#include <gtk/gtk.h>

void gui_update_install_status(glong total, glong done,
			       const gchar *message);
void gui_update_install_progressbar(gdouble fraction);
void gui_open_mod_mgr(void);
void gui_open_mod_mgr_initial_run(void);
void clear_and_hide_progress_bar(void);

#ifdef USE_GTK_3
void
on_notebook1_switch_page(GtkNotebook *notebook,
			 gpointer arg,
			 guint page_num, gpointer user_data);
#else
void
on_notebook1_switch_page(GtkNotebook *notebook,
			 GtkNotebookPage *page,
			 guint page_num, gpointer user_data);
#endif

//void on_notebook1_switch_page(GtkNotebook * notebook, guint page_num, gpointer user_data);
void on_radiobutton2_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void on_radiobutton4_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void save_sources(void);
void on_refresh_clicked(GtkButton *button, gpointer user_data);
void on_install_clicked(GtkButton *button, gpointer user_data);
void on_remove_clicked(GtkButton *button, gpointer user_data);
void on_archive_clicked(GtkButton *button, gpointer user_data);
void on_index_clicked(GtkButton *button, gpointer user_data);
void on_delete_index_clicked(GtkButton *button,
			     gpointer user_data);
void on_load_sources_clicked(GtkButton *button,
			     gpointer user_data);
void on_mod_mgr_intro_clicked(GtkButton *button,
			      gpointer user_data);
void delete_obsolete(char *module, int *counter);
void on_scan_obsolete(GtkButton *button, gpointer user_data);
void on_cancel_clicked(GtkButton *button, gpointer user_data);
void on_mod_mgr_response(GtkDialog *dialog, gint response_id,
			 gpointer user_data);
void on_button_add_local_clicked(GtkButton *button,
				 gpointer user_data);
void on_button_remove_local_clicked(GtkButton *button,
				    gpointer user_data);
void on_button_add_remote_clicked(GtkButton *button,
				  gpointer user_data);
void on_button_remove_remote_clicked(GtkButton *button,
				     gpointer user_data);
gboolean on_treeview1_button_release_event(GtkWidget *widget,
					   GdkEventButton *event,
					   gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif
