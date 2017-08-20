/*
 * Xiphos Bible Study Tool
 * sword.h - glue
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _SWORD_H__
#define _SWORD_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <gtk/gtk.h>
#include <glib/gi18n.h>

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
#define SB_VIEWER_TYPE 13

#define DIALOG_TEXT_TYPE 7
#define DIALOG_COMMENTARY_TYPE 8
#define DIALOG_DICTIONARY_TYPE 9
#define DIALOG_BOOK_TYPE 10
#define DIALOG_SEARCH_PREVIEW_TYPE 11
#define PRAYERLIST_TYPE 12

/* these strings are not seen by users */
/* they are returned by Sword in module->Type() */
#define TEXT_MODS "Biblical Texts"
#define COMM_MODS "Commentaries"
#define DICT_MODS "Lexicons / Dictionaries"
#define BOOK_MODS "Generic Books"

#define UTF_8 "UTF-8"
extern char *OLD_CODESET;
extern char *sword_locale;
extern int re_encode_digits;
extern gboolean companion_activity;
/*** end defs ***/

/*** function prototypes ***/

void main_add_abbreviation(const char *name,
			   const char *abbreviation);
const char *main_get_abbreviation(const char *name);
const char *main_get_name(const char *abbreviation);

void main_book_heading(char *mod_name);
void main_chapter_heading(char *mod_name);
void main_delete_note(const gchar *module_name,
		      const gchar *key_str);
void main_save_note(const gchar *module_name,
		    const gchar *key_str, const gchar *note_str);
GtkWidget *main_dictionary_drop_down_new(char *mod_name,
					 char *old_key);
char *main_get_path_to_mods(void);
const char *main_get_sword_version(void);
void main_shutdown_backend(void);
void main_set_module_unlocked(const char *mod_name, char *key);
void main_save_module_key(const char *mod_name, char *key);
char *main_getText(char *key);
char *main_getShortText(char *key);
gchar *main_update_nav_controls(const char *module_name,
				const gchar *key);
char *main_get_active_pane_key(void);
char *main_get_active_pane_module(void);
char *set_sword_locale(const char *sys_locale);
void main_init_backend(void);
int main_is_mod_rtol(const char *module_name);
char *main_module_name_from_description(char *description);
char *main_get_search_results_text(char *mod_name, char *key);
void main_display_book(const char *mod_name, const char *key); //, unsigned long offset);
void main_display_commentary(const char *mod_name,
			     const char *key);
void main_display_dictionary(const char *mod_name,
			     const char *key);
void main_display_bible(const char *mod_name, const char *key);
void main_display_devotional(void);
void main_setup_displays(void);
void main_dictionary_entry_changed(char *mod_name);
void main_dictionary_button_clicked(gint direction);
const char *main_get_module_language(const char *module_name);
int main_check_for_option(const char *mod_name, const char *key,
			  const char *option);
int main_check_for_global_option(const char *mod_name,
				 const char *option);
int main_has_cipher_tag(char *mod_name);
void main_check_unlock(const char *mod_name, gboolean conditional);
int main_is_module(char *mod_name);
int main_has_search_framework(char *mod_name);
int main_optimal_search(char *mod_name);
char *main_get_mod_config_entry(const char *module_name,
				const char *entry);
char *main_get_mod_config_file(const char *module_name,
			       const char *moddir);
char *main_get_mod_about_info(char *mod_name);
char *main_get_striptext(char *module_name, char *key);
char *main_get_striptext_from_string(char *module_name,
				     char *string);
char *main_get_rendered_text(const char *module_name,
			     const char *key);
char *main_get_raw_text(char *module_name, char *key);
int main_get_mod_type(char *mod_name);
const char *main_get_module_description(const char *module_name);
char *main_get_treekey_local_name(unsigned long offset);
char *main_get_book_key_from_offset(unsigned long offset);
void main_refresh_all(void);
char *main_format_number(int x);
int main_deformat_number(char *digitstring);
void main_flush_widgets_content(void);
gboolean main_is_Bible_key(const gchar *name, const gchar *key);
const char *main_get_osisref_from_key(const char *module,
				      const char *key);
const char *main_get_language_map(const char *language);
char **main_get_module_language_list(void);
void main_init_language_map(void);

#ifdef __cplusplus
}
#endif
/*
 * waf configure -chatty to generate tracing.
 */
#ifdef CHATTY
#define XI_message(x) g_message x
#define XI_warning(x) g_warning x
#define XI_print(x) g_print x
#define XI_error(x) g_error x
#else
#define XI_message(x)
#define XI_warning(x)
#define XI_print(x)
#define XI_error(x)
#endif
#ifdef WIN32
#define Mkdir(d, m) g_mkdir(d, m)
#define DOTSWORD "Sword"
#define access(d, m) g_access(d, m)
#else
#define Mkdir(d, m) mkdir(d, m)
#define DOTSWORD ".sword"
#endif /* WIN32 */
/* we can now use "HOME" everywhere again,
   since we aggressively set it for WIN32. */
#define HOMEVAR "HOME"
#endif
