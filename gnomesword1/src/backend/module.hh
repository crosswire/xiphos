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
#include "main/lists.h"


const char *backend_get_module_config_entry(const char * module_name, 
					    const char * entry);
const char *backend_get_language_map(const char *language);
char *backend_get_preverse_header(char * module_name, char * key, int pvHeading);
const char *backend_get_footnote_type(char *module_name, char *key, char *note);
char *backend_get_crossref(char *module_name, char *key, char *note);
const char *backend_get_module_version(char *module_name);

char *backend_get_footnote_body(char *module_name, 
					char *key, char *note);

int backend_get_display_level(char * module_name);
int backend_is_module_rtl(char * mod_name);
char *backend_module_name_from_description(char * mod_desc);
int backend_do_module_search(char * module_name ,const char *search_string, 
		int search_type, int search_params);
char *backend_get_search_results_text(char * mod_name, char * key);

const char *backend_get_module_language(const char *module_name);
int backend_is_personal_comment_module(char * mod_name);

char *backend_get_module_description(char * module_name);
int backend_has_cipher_tag(char *mod_name);
int backend_module_is_locked(char *mod_name);
char *backend_get_cipher_key(char *mod_name);	
int backend_check_for_module(const char * mod_name);
char *backend_get_mod_about_info(char * modname);
int backend_get_module_page(const char *module_name, const char *module_type);
int backend_get_mod_type(char * mod_name);


char *backend_get_module_text(int manager, char * mod_name, char * key);
char *backend_get_striptext(int manager, char *module_name, char *key);

int backend_check_for_global_option(char * mod_name, char * option);

char *backend_nav_module(int manager, char * mod_name, int direction);
char *backend_get_chap_heading(int manager, char * mod_name, char * key);	
char *backend_get_book_heading(int manager, char * mod_name, char * key);
void backend_set_commentary_key(char * mod_name, char * key);
		
char *backend_get_commentary_key(char *mod_name);	
char *backend_get_commentary_text(char *mod_name, char *key);

char *backend_get_key_from_module(int manager, char * module_name);

int backend_set_module(int manager, char * module_name);
void backend_set_percomm_key(char * key);
void backend_save_personal_comment(char * note);
void backend_delete_personal_comment(void);
char *backend_get_percomm_text(char * key);
void backend_init_language_map(void);
void backend_get_module_lists(MOD_LISTS * mods);

#ifdef __cplusplus
}
#endif

#endif
