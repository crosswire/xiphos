/*
 * GnomeSword Bible Study Tool
 * configs.h - 
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
 
#ifndef __CONFIGS_H__
#define __CONFIGS_H__

#ifdef __cplusplus
extern "C" {
#endif	
#include <glib-1.2/glib.h>
typedef struct _mod_font MOD_FONT;
struct  _mod_font {
	const char *mod_name;
	char *old_font;
	char *old_gdk_font;
	const char *new_font;
	const char *new_gdk_font;
	char *old_font_size;
	const char *new_font_size;
	int no_font;
};

void save_module_key(char * mod_name, char * key);
int open_config_file(char *file);
void erase_config_section(char *section);
int close_config_file(void);
void add_to_config_file(char * section, 
			char * label, char * value);
const char *get_config_value(char * section, char * label);
int set_config_to_get_labels(char * section);
const char *get_next_config_label(void);
	
char *get_module_font_name(char *mod_name);
char *get_conf_file_item(const char * file, const char * mod_name, const char * item);
void save_conf_file_item(const char * file, const char * mod_name, const char * item,
			const char * value);
char *get_module_font_size(char *mod_name);
#ifdef __cplusplus
}
#endif

#endif
