/*
 * GnomeSword Bible Study Tool
 * sword.h - support for sword libs
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

#ifndef __SWORD_H__
#define __SWORD_H__

#ifdef __cplusplus
extern "C" {
#endif	

#include <glib-1.2/glib.h>
	

#include "main/lists.h"
	
void backend_init(void);
void backend_module_name_from_description(char * mod_name, char * mod_desc);
void backend_shutdown(int save_properties);
void backend_change_percom_module(char * modName);
const char *backend_get_sword_version(void);
void backend_save_module_key(char *mod_name, char *key);
int backend_get_mod_type(char * modName);
char *backend_get_module_description(char * modName);
char *backend_get_path_to_mods(void);
char *backend_get_mod_about_info(char * modname);
int backend_get_module_page(char *module_name, char *module_type);
int backend_has_cipher_tag(char *mod_name);
int backend_module_is_locked(char *mod_name);
char *backend_get_cipher_key(char *mod_name);
char *backend_get_module_text(char * mod_name, char * key);	
int backend_check_for_module(char * mod_name);
char *backend_get_striptext(char *mod_name, char *key);


#ifdef __cplusplus
}
#endif

#endif

