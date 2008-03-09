/*
 * GnomeSword Bible Study Tool
 * sword_treekey.h -  treekey stuff for book editor
 *
 * Copyright (C) 2007 GnomeSword Developer Team
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

void main_prayer_list_add_child(gchar * mod_name);
//void main_fill_prayer_list_tree(gchar * mod_name);
gint main_prayer_list_new(char * list_name);


#ifdef __cplusplus
}
#endif

#endif /* _SWORD_TREEKEY_HH */
