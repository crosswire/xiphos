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
	
	GList * modules;
	gchar
	    *module_name,
	    *upper_bond, 
	    *lower_bond, 
	    *search_string;

	gint
	    search_type, 
	    search_params, 
	    found_count;

	 gboolean 
	    use_bonds, 
	    use_lastsearch_for_bonds;
};

  
typedef struct _custom_range CUSTOM_RANGE;
struct  _custom_range{
	gchar *label;
	gchar *range;
};

GList * get_element(char * entry);
char * set_range(char * list);
void save_custom_ranges(GList * ranges);
GList * load_custom_ranges(void);
void search_dialog_appbar_update(char percent, void *userData);
void search_percent_update(char percent, void *userData);
GList *do_search(gpointer *usr_data);
int clear_scope(void);	
int clear_search_list(void);	
int clear_bounds(void);	
int set_bounds_upper(char * bound);	
int set_bounds_lower(char * bound);	
int set_scope2bounds(void);	
int set_scope2last_search(void);
void set_scope2range(void);	
int set_search_module(char * module);	
int do_dialog_search(char *search_string, int search_type, 
						int search_params);
void add_to_found_list(const char * result_text, const char * module);

#ifdef __cplusplus
}
#endif

#endif

