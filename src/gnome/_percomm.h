/*
 * GnomeSword Bible Study Tool
 * _percom.c - personal comments gui
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

#ifndef ___PERCOMM_H_
#define ___PERCOMM_H_

#include <gnome.h>
#include "settings.h"
#include "percomm.h"

void gui_set_percomm_frame_label(GtkWidget * frame, gchar *mod_name);
void tabs(gboolean choice);
void on_notebook_percomm_switch_page(GtkNotebook * notebook,
		GtkNotebookPage * page, gint page_num, GList * pcl);
GSHTMLEditorControlData *gui_percomm_control(SETTINGS * s, 
					gchar *mod_name, gint page_num);
#endif	/* ___PERCOMM_H_ */
