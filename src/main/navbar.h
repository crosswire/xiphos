/*
 * GnomeSword Bible Study Tool
 * navbar.h - glue between all navbars and sword
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
 
#ifndef _NAVBAR_H
#define _NAVBAR_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _navbar NAVBAR;
struct _navbar {	
	GtkWidget *button_back;
	GtkWidget *button_forward;
	GtkWidget *comboboxentry_book;
	GtkWidget *comboboxentry_chapter;
	GtkWidget *comboboxentry_verse;
	GtkWidget *lookup_entry;
	gchar *key;
	gchar *module_name;
	gboolean is_dialog;
};
extern NAVBAR navbar_main;

void main_navbar_set(NAVBAR navbar, const char * key);
void main_navbar_fill_book_combo(NAVBAR navbar);

#ifdef __cplusplus
}
#endif

#endif /* _NAVBAR_H */
