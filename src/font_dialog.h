/*
 * GnomeSword Bible Study Tool
 * font_dialog.h - dialog to set module font
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

#ifndef _FONT_DIALOG_
#define _FONT_DIALOG_

#ifdef __cplusplus
extern "C" {
#endif
	
typedef struct _mod_font MOD_FONT;
struct  _mod_font {
	gchar *mod_name;
	gchar *old_font;
	gchar *new_font;
	gchar *old_font_size;
	gchar *new_font_size;
	gint no_font;
};

void gui_set_module_font(gchar * mod_name);

#ifdef __cplusplus
}
#endif
#endif
