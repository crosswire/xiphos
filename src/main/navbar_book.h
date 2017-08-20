/*
 * Xiphos Bible Study Tool
 * navbar_book.h - glue between all navbar_book and sword
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

#ifndef _NAVBAR_BOOK_H
#define _NAVBAR_BOOK_H

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _navbar_book NAVBAR_BOOK;
struct _navbar_book
{
	GtkWidget *button_up;
	GtkWidget *button_down;
	GtkWidget *button_left;
	GtkWidget *button_right;
	GtkWidget *button_list;
	GtkWidget *menu;
	GtkWidget *lookup_entry;
	gchar *key;
	gchar *book_name;
};
extern NAVBAR_BOOK navbar_book;

GtkWidget *main_book_drop_down_new(void);
void main_navbar_book_parent(void);
void main_navbar_book_first_child(void);
void main_navbar_book_prev(void);
void main_navbar_book_next(void);
void main_setup_navbar_book(char *book_name, unsigned long offset);
void main_navbar_book_entry_activate(const gchar *entry_text);

#ifdef __cplusplus
}
#endif
#endif /* _NAVBAR_BOOK_H */
