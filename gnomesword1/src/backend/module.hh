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

//char *backend_get_book_key(void);
//int backend_module_has_testament(const char * module_name,  int testament);
//const char *backend_get_module_version(char *module_name);
//int backend_get_display_level(char * module_name);
//int backend_is_module_rtl(char * mod_name);
	
char *backend_module_name_from_description(char * mod_desc);
	
//int backend_do_module_search(char * module_name ,const char *search_string, int search_type, int search_params);

#ifdef __cplusplus
}
#endif

#endif
