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
#include <glib/gi18n.h>
//#include <glib-2.0/glib.h>
	
#include "main/configs.h"	
	
/*** these defs need to be seen by gui and backend ***/
	
/* module types */
#define TEXT_TYPE 0 
#define COMMENTARY_TYPE 1 
#define DICTIONARY_TYPE 2
#define BOOK_TYPE 3
#define PERCOM_TYPE 4
	
#define PARALLEL_TYPE 5
#define VIEWER_TYPE 6
	
#define DIALOG_TEXT_TYPE 7
#define DIALOG_COMMENTARY_TYPE 8 
#define DIALOG_DICTIONARY_TYPE 9
#define DIALOG_BOOK_TYPE 10

#define DIALOG_SEARCH_PREVIEW_TYPE 11
       
/* these strings are not seen by users */
#define TEXT_MODS "Biblical Texts"
#define COMM_MODS "Commentaries"
#define DICT_MODS "Lexicons / Dictionaries"
#define BOOK_MODS "Generic Books"

#define UTF_8 "UTF-8" 
extern char *OLD_CODESET;
extern char *sword_locale;
/*** end defs ***/

/*** function prototypes ***/

void main_book_heading(char * mod_name);
void main_chapter_heading(char * mod_name);
void main_display_prev_verse(const char * mod_name, char * current_verse);
void main_display_next_verse(const char * mod_name, char * current_verse);
void main_delete_note(	const gchar * module_name, 
								const gchar * key_str);
void main_save_note(const gchar * module_name, const gchar * key_str, 
				               const gchar * note_str );
GtkWidget *main_dictionary_drop_down_new(char * mod_name, char * old_key);
char *main_get_path_to_mods(void);
const char *main_get_sword_version(void);
void main_shutdown_backend(void);
void main_set_module_unlocked(char * mod_name, char * key);
void main_save_module_key(char * mod_name, char * key);
gchar *main_update_nav_controls(const gchar * key);
char *main_get_active_pane_key(void);
char *main_get_active_pane_module(void);
void main_init_backend(void);
int main_is_mod_rtol(const char * module_name);
char *main_module_name_from_description(char *description);
char *main_get_search_results_text(char * mod_name, char * key);
void main_locked_module_display(gpointer data,
				  char * mod_name, char * cipher_key);
void main_display_book(const char * mod_name, char * key);
void main_display_commentary(const char * mod_name, const char * key);
void main_display_dictionary(char * mod_name, char * key);
void main_display_bible(const char * mod_name, const char * key);
void main_display_devotional(void);
void main_setup_displays(void);
void main_dictionary_entry_changed(char * mod_name);
void main_dictionary_button_clicked(gint direction);
const char *main_get_module_language(const char *module_name);
int main_check_for_global_option(char * mod_name, char * option);
int main_has_cipher_tag(char *mod_name);
int main_is_module(char * mod_name);
const char *main_get_mod_config_entry(const char * module_name, 
					const char * entry);
char *main_get_mod_about_info(char * mod_name);
char *main_get_striptext(char *module_name, char *key);
char *main_get_striptext_from_string(char *module_name, char *string);
char *main_get_rendered_text(char *module_name, char *key);
char *main_get_raw_text(char *module_name, char *key);
int main_get_mod_type(char * mod_name);
char *main_get_module_description(char * module_name);
char *main_get_treekey_local_name(unsigned long offset);

#ifdef __cplusplus
}
#endif

#endif
