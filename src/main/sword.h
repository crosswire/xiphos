/*
 * GnomeSword Bible Study Tool
 * sword.h - glue
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

#ifndef _SWORD_H__
#define _SWORD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

/*** function prototypes ***/

void search_percent_update(char percent, void *userData);
int get_mod_type(char * mod_name);
char *get_module_font_name(char *mod_name);
char *get_module_font_size(char *mod_name);
void module_name_from_description(gchar *mod_name, gchar *description);
void save_module_key(gchar * mod_name, gchar * key);
const char *get_sword_version(void);
gchar *get_module_description(gchar * mod_name);
const char *get_book_from_key(char *key);
int get_chapter_from_key(char *key);
int get_verse_from_key(char *key);
void save_properties(gboolean use_default);
GList *do_search(gpointer *usr_data);
GList *get_verse_list(gchar* module_name, gchar *verse_list);
int module_is_locked(char * mod_name);
char *get_valid_key(char *key);
int get_module_number(char *module_name, char *module_type);
char *get_module_text(char * mod_name, char * key);
char *get_search_results_text(char * mod_name, char * key);
int create_properties_from_setup(void);
gchar *get_path_to_mods(void);
GList *get_list_of_mods_by_type(char *mod_type);
gchar *get_mod_about_info(char * mod_name);

#ifdef __cplusplus
}
#endif

#endif

