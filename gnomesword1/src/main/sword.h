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

#include <glib-1.2/glib.h>
	
/*** these defs need to be seen by gui and backend ***/
	
/* module types */
#define TEXT_TYPE 0 
#define COMMENTARY_TYPE 1
#define DICTIONARY_TYPE 2
#define BOOK_TYPE 3
       
/* these strings are not seen by users */
#define TEXT_MODS "Biblical Texts"
#define COMM_MODS "Commentaries"
#define DICT_MODS "Lexicons / Dictionaries"
#define BOOK_MODS "Generic Books"

/*** end defs ***/

//#include "gui/font_dialog.h"

typedef struct _mod_font MOD_FONT;
struct  _mod_font {
	gchar *mod_name;
	gchar *old_font;
	gchar *old_gdk_font;
	gchar *new_font;
	gchar *new_gdk_font;
	gchar *old_font_size;
	gchar *new_font_size;
	gint no_font;
};

/*** function prototypes ***/

int get_mod_type(char * mod_name);
char *get_module_font_name(char *mod_name);
void get_font_info(MOD_FONT *mf);
void save_font_info(MOD_FONT *mf);
char *get_module_font_size(char *mod_name);
void module_name_from_description(char *mod_name, char *description);
void save_module_key(char * mod_name, char * key);
const char *get_sword_version(void);
char *get_module_description(char * mod_name);
const char *get_book_from_key(char *key);
int get_chapter_from_key(char *key);
int get_verse_from_key(char *key);
void save_properties(gboolean use_default);
GList *get_verse_list(char* module_name, char *verse_list);
int module_is_locked(char * mod_name);
char *get_cipher_key(char *mod_name);
int has_cipher_tag(char *mod_name);
char *get_valid_key(char *key);
int get_module_number(char *module_name, char *module_type);
char *get_module_text(char * mod_name, char * key);
char *get_search_results_text(char * mod_name, char * key);
int create_properties_from_setup(void);
char *get_path_to_mods(void);
GList *get_list_of_mods_by_type(char *mod_type);
char *get_mod_about_info(char * mod_name);
int check_for_module(char * mod_name);
void init_sword(void);
void shutdown_sword(void);
char *get_striptext(char *mod_name, char *key);
#ifdef __cplusplus
}
#endif

#endif

