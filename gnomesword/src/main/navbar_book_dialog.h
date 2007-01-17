/*
 * GnomeSword Bible Study Tool
 * navbar_book.h - glue between all navbar_book and sword
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
 
#ifndef  _NAVBAR_BOOK_DIALOG_H
#define _NAVBAR_BOOK_DIALOG_H

#ifdef __cplusplus
extern "C"
{
#endif

GtkWidget *main_navbar_book_dialog_drop_down_new(gpointer data);
void main_navbar_book_dialog_parent(gpointer data);
void main_navbar_book_dialog_first_child(gpointer data);
void main_navbar_book_dialog_prev(gpointer data);
void main_navbar_book_dialog_next(gpointer data);
void main_setup_navbar_book_dialog(gpointer data);


#ifdef __cplusplus
}
#endif

#endif /* _NAVBAR_BOOK_DIALOG_H */
