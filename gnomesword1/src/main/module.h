/*
 * GnomeSword Bible Study Tool
 * module.h - SHORT DESCRIPTION
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

#ifndef _MODULE_H_
#define _MODULE_H_

#ifdef __cplusplus
extern "C" {
#endif

char *get_module_key(void);
char *get_module_name(void);
char *get_footnote_body(char *note);
int get_display_level(char * module_name);	
int is_module_rtl(char * mod_name);
int get_module_number(const char *module_name, const char *module_type);
int get_mod_type(char * mod_name);
int do_module_search(char * module_name ,const char *search_string, 
		  int search_type, int search_params, int dialog);
char *get_module_description(char * module_name);
const char *get_module_language(const char *module_name);
int module_is_locked(char * mod_name);
char *get_cipher_key(char *mod_name);
int has_cipher_tag(char *mod_name);
int check_for_module(char * mod_name);
char *get_mod_about_info(char * mod_name);

char *get_module_text(int manager, char * module_name, char * key);
char *get_striptext(int manager, char *module_name, char *key);

#ifdef __cplusplus
}
#endif

#endif
