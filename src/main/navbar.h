/*
 * Xiphos Bible Study Tool
 * navbar.h - glue between all navbars and sword
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

#ifndef _NAVBAR_H
#define _NAVBAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>

typedef struct _navbar NAVBAR;
struct _navbar
{
	GtkWidget *button_back;
	GtkWidget *button_forward;
	GtkWidget *comboboxentry_book;
	//      SPINBUTTONS book;
	GtkWidget *comboboxentry_chapter;
	//      SPINBUTTONS chapter;
	GtkWidget *comboboxentry_verse;
	//      SPINBUTTONS verse;
	GtkWidget *lookup_entry;
	gchar *key;
	gchar *module_name;
	gboolean is_dialog;
	int testaments;
};
extern NAVBAR navbar_main;

void main_navbar_set(NAVBAR navbar, const char *key);
void main_navbar_fill_book_combo(NAVBAR navbar);

#ifdef __cplusplus
}
#endif
#endif /* _NAVBAR_H */
