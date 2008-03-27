/*
 * GnomeSword Bible Study Tool
 * sword_treekey.h -  treekey stuff for book editor
 *
 * Copyright (C) 2008 GnomeSword Developer Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _SWORD_TREEKEY_HH
#define _SWORD_TREEKEY_HH

#ifdef __cplusplus
extern "C"
{
#endif

unsigned long main_treekey_remove (char *book, char *name, char * offset);
unsigned long main_treekey_append_sibling (char *book, char *name, char * offset);
unsigned long main_treekey_append_child (char *book, char *name, char * offset);
void main_treekey_set_local_name(char * book, char * name, char * offset);
//void main_prayer_list_add_child(gchar * mod_name);
int main_prayer_list_new(char * list_name);
void main_load_book_tree_in_editor (GtkTreeView * treeview, char * book);
char *main_get_book_raw_text (char *book, char * offset);
void main_treekey_save_book_text (char *book, char * offset, char * text);

#ifdef __cplusplus
}
#endif

#endif /* _SWORD_TREEKEY_HH */
