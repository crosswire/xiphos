/*
 * GnomeSword Bible Study Tool
 * search.h - glue
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

#ifndef _SEARCH_H__
#define _SEARCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <glib-1.2/glib.h>
  
typedef struct _sword_search SEARCH_SWORD;
struct  _sword_search {
	gchar
	    *module_name;
	
};


typedef struct _custom_list CUSTOM_LIST;
struct  _custom_list{
	gchar *label;
	gchar *value;
};

void search_dialog_appbar_update(char percent, void *userData);
void search_percent_update(char percent, void *userData);

#ifdef __cplusplus
}
#endif

#endif

