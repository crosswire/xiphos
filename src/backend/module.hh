/*
 * GnomeSword Bible Study Tool
 * module.hh - SHORT DESCRIPTION
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

#ifndef _MODULE_HH_
#define _MODULE_HH_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _name_type NAME_TYPE;
struct _name_type {
	char *name;
	int type;
};


int backend_do_module_search(char * module_name ,char *search_string, 
		int search_type, int search_params);
char *backend_get_search_results_text(char * mod_name, char * key);
void backend_set_module_iterators(void);
NAME_TYPE *backend_get_next_module_name(void);
NAME_TYPE *backend_get_next_module_description(void);
char *backend_get_next_percom_name(void);
char *backend_get_next_devotion_name(void);
	
#ifdef __cplusplus
}
#endif

#endif

