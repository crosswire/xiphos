/*
 * GnomeSword Bible Study Tool
 * gbs_.c - sword lib support for general book modules
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
  
#ifndef __GBS__H_
#define __GBS__H_

#ifdef __cplusplus
extern "C" {
#endif
	
char *backend_get_book_key(char * book_name);
char * backend_display_gbs(char * book_name, char * key);
int backend_treekey_next_sibling(unsigned long offset);
int backend_gbs_treekey_has_children(unsigned long offset);
char *backend_gbs_get_treekey_local_name(unsigned long offset);
unsigned long backend_gbs_get_treekey_offset(void);
int backend_gbs_treekey_first_child(unsigned long offset);
char * backend_display_row_gbs(char * book_name, char *offset);
void backend_setup_books(void);
void backend_shutdown_books(void);
void backend_change_book(char * mod_name, unsigned long offset);


#ifdef __cplusplus
}
#endif

#endif

