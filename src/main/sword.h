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
	
#include "main/key.h"	
#include "main/configs.h"	
/*** these defs need to be seen by gui and backend ***/
	
/* module types */
#define TEXT_TYPE 0 
#define COMMENTARY_TYPE 1 
#define DICTIONARY_TYPE 2
#define BOOK_TYPE 3
#define PERCOM_TYPE 4
       
/* these strings are not seen by users */
#define TEXT_MODS "Biblical Texts"
#define COMM_MODS "Commentaries"
#define DICT_MODS "Lexicons / Dictionaries"
#define BOOK_MODS "Generic Books"

#define OLD_CODESET "iso8859-1"
#define UTF_8 "UTF-8" 
/*** end defs ***/


/*** function prototypes ***/

//void set_global_option(int manager, char * option, gboolean choice);
//void delete_module_mgr(void);
gchar *main_update_nav_controls(const gchar * key);
char *main_get_active_pane_key(void);
char *main_get_active_pane_module(void);
const char *get_sword_version(void);
char *get_path_to_mods(void);
void init_sword(void);
void shutdown_backend(void);
int main_is_mod_rtol(const char * module_name);
//int set_module(int manager, char * module_name);
char *main_module_name_from_description(char *description);
char *main_get_crossref(char * mod_name, char * key, 
						char * note_number);
char *main_get_footnote_body(char * mod_name, char * key, 
						char * note_number);
char *main_get_search_results_text(char * mod_name, char * key);
void main_locked_module_display(GtkWidget * html_widget,
				  gchar * mod_name, gchar * cipher_key);
void main_entry_display(GtkWidget * html_widget, gchar * mod_name,
		   gchar * text, gchar * key, gboolean show_key);
void main_setup_displays(void);
void main_display_book(const char * mod_name, char * key);
void main_display_commentary(const char * mod_name, const char * key);
void main_display_dictionary(char * mod_name, char * key);
void main_display_bible(const char * mod_name, const char * key);
void main_change_verse(const char * bible, const char * commentary, 
						const char * key);
void main_setup_new_displays(void);
void main_dictionary_entery_changed(char * mod_name);
const char *main_get_module_language(const char *module_name);
int main_check_for_global_option(char * mod_name, char * option);
int main_has_cipher_tag(char *mod_name);
int main_is_module(char * mod_name);
const char *main_get_mod_config_entry(const char * module_name, const char * entry);
char *main_get_mod_about_info(char * mod_name);
char *main_get_striptext(char *module_name, char *key);
char *main_get_rendered_text(char *module_name, char *key);
int main_get_mod_type(char * mod_name);
char *main_get_module_description(char * module_name);


#ifdef __cplusplus
}
#endif

#endif
