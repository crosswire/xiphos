/*
 * GnomeSword Bible Study Tool
 * gbs.h - generic book support - the glue
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

#ifndef __GBS_H_
#define __GBS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main/settings.h"

void display_gbs(int page_num, char * key);
int treekey_next_sibling(char * mod_name, unsigned long offset);
int gbs_treekey_has_children(char * mod_name, 
					unsigned long offset);
char * gbs_get_treekey_local_name(char * mod_name, unsigned long offset);
unsigned long gbs_get_treekey_offset(char *mod_name);
int gbs_treekey_first_child(char * bookname, unsigned long offset);
char *get_book_key(int book_num);
gboolean display_row_gbs(gint book_num, gchar *offset);
void new_gbs_display(GtkWidget * html, char * mod_name);
#ifdef __cplusplus
}
#endif

#endif

