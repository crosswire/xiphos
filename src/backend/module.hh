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

int backend_get_display_level(char * module_name);
int backend_is_module_rtl(char * mod_name);
void backend_module_name_from_description(char * mod_name, 
						char * mod_desc);
int backend_do_module_search(char * module_name ,char *search_string, 
		int search_type, int search_params);
char *backend_get_search_results_text(char * mod_name, char * key);
void backend_set_module_iterators(void);
NAME_TYPE *backend_get_next_module_name(void);
NAME_TYPE *backend_get_next_module_description(void);

int backend_is_personal_comment_module(char * mod_name);
char *backend_get_next_percom_name(void);
char *backend_get_next_devotion_name(void);
char *backend_get_module_description(char * module_name);
int backend_has_cipher_tag(char *mod_name);
int backend_module_is_locked(char *mod_name);
char *backend_get_cipher_key(char *mod_name);	
int backend_check_for_module(char * mod_name);
char *backend_get_mod_about_info(char * modname);
int backend_get_module_page(char *module_name, char *module_type);
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

#ifdef __cplusplus
}
#endif

#endif
