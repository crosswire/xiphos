/*
 * GnomeSword Bible Study Tool
 * _gbs.h - generic book support - the gui
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef ___GBS_H_
#define ___GBS_H_
	
#include "main/gbs.h"

void gui_set_book_page_and_key(gint page_num, gchar * key);
void gui_set_gbs_frame_label(GBS_DATA *g);
void on_notebook_gbs_switch_page(GtkNotebook *notebook, 
		GtkNotebookPage *page, gint page_num, GList *data_gbs);
GtkWidget *gui_create_pm_gbs(GBS_DATA *gbs);
void gui_create_gbs_pane(gchar *modName, gint count, GBS_DATA *p_gbs);
void gui_setup_gbs(GList *mods);
void gui_shutdown_gbs(void);

#endif

