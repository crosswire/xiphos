/*
 * GnomeSword Bible Study Tool
 * config.hh - search Sword modules
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
 
#ifndef __CONFIG_HH__
#define __CONFIG_HH__

#ifdef __cplusplus
extern "C" {
#endif	
#include "main/sword.h"

int backend_open_config_file(char *file);
void backend_erase_config_section(char *section);
int backend_close_config_file(void);
void backend_add_to_config_file(char * section, 
				char * label, char * value);
void backend_save_value_to_config_file(char * section, 
				char * label, char * value);
const char *backend_get_config_value(char * section, char * label);
int backend_set_config_to_get_labels(char * section);
const char *backend_get_next_config_label(void);
const char *backend_get_next_config_value(void);
void backend_save_module_key(char *mod_name, char *key);
	
char *backend_get_module_font_name(char *mod_name, char * dir);
char *backend_get_module_font_size(char *mod_name, char * dir);	
void backend_load_font_info(MOD_FONT * mf, char * dir);
void backend_save_font_info(MOD_FONT * mf, char * dir);


int backend_load_module_options(char * modName, char * option, 
					char * dir, char * conf);
int backend_save_module_options(char * modName, char * option, 
			char * value, char * dir, char * conf);

GNode * backend_load_bookmarks(char *dir);
#ifdef __cplusplus
}
#endif

#endif
