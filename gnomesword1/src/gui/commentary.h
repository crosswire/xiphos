/*
 * GnomeSword Bible Study Tool
 * commentary.h - gui for commentary modules
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#ifndef ___COMMENTARY_H_
#define ___COMMENTARY_H_

//#include "gui/editor.h"
//#include "gui/mod_global_ops.h"

void gui_set_comm_label(gchar * mod_name);
void gui_set_commentary_mod_and_key(gchar * mod_name,
				     gchar * key);
void gui_display_commentary(gchar * key);
GtkWidget *gui_create_commentary_pane(void);
//void gui_popup_pm_comm(gchar * mod_name, GdkEventButton * event);


#endif
