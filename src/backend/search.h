/*
 * GnomeSword Bible Study Tool
 * search.h - search Sword modules
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
 
#ifndef __SEARCH_H__
#define __SEARCH_H__

#ifdef __cplusplus
extern "C" {
#endif	
	
char * backend_set_range(char * list)	;
GList * backend_get_element(char * entry);
void backend_save_custom_ranges(GList * ranges);
GList * backend_load_custom_ranges(void);
int backend_clear_scope(void);	
int backend_clear_search_list(void);	
int backend_clear_bounds(void);	
int backend_set_bounds_upper(char * bound);	
int backend_set_bounds_lower(char * bound);	
int backend_set_scope2bounds(void);	
int backend_set_scope2last_search(void);
void backend_set_scope2range(void);	
int backend_set_search_module(char * mod_name);	
int backend_do_dialog_search(char *search_string, int search_type, int search_params);
	
GList *backend_do_sb_search(gpointer *usr_data);
char *backend_get_search_results_text(char * mod_name, char * key);

#ifdef __cplusplus
}
#endif

#endif

