/*
 * GnomeSword Bible Study Tool
 * display_info.h - display information (ie strongs) in a dialog
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

#ifndef ___DISPLAY_INFO_H__
#define ___DISPLAY_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

extern GtkWidget *dialog_display_info;
	
void gui_display_text_information(gchar * information);	
void gui_display_mod_and_key(gchar * mod_name, gchar * key);
GtkWidget *gui_create_display_informtion_dialog(void);

#ifdef __cplusplus
}
#endif

#endif
