/*
 * GnomeSword Bible Study Tool
 * tree.hh - sword lib support for treekey
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
  
#ifndef __TREE_HH_
#define __TREE_HH_

#ifdef __cplusplus
extern "C" {
#endif

int backend_get_parent(unsigned long offset);
int backend_treekey_next_sibling(unsigned long offset);
int backend_gbs_treekey_has_children(unsigned long offset);
char *backend_gbs_get_treekey_local_name(unsigned long offset);
unsigned long backend_gbs_get_treekey_offset(void);
int backend_gbs_treekey_first_child(unsigned long offset);
char * backend_get_text_from_offset(char * module_name, unsigned long offset);
void backend_set_treekey(char * module_name, unsigned long offset);
void backend_setup_treekey(void);
void backend_shutdown_treekey(void);


#ifdef __cplusplus
}
#endif

#endif
