/*
 * GnomeSword Bible Study Tool
 * gnomesword.h - glue
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

#ifndef __GNOMESWORD_H__
#define __GNOMESWORD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "settings.h"

#define MAIN_TEXT_WINDOW 0 
#define INTERLINEAR_WINDOW 1
#define COMMENTARY_WINDOW 2
#define DICTIONARY_WINDOW 3
#define BOOK_WINDOW 4
#define PERCOMM_WINDOW 5
#define STUDYPAD_WINDOW 6
    
/* module types */
#define TEXT_TYPE 0 
#define COMMENTARY_TYPE 1
#define DICTIONARY_TYPE 2
#define BOOK_TYPE 3
	
/* these strings are not seen by users */    
#define GS_DOCK_OPTIONS "dockOptions"
#define GS_DOCK_GBS "dockGBSEditor"
#define GS_DOCK_PC "dockPCEditor"
#define GS_DOCK_SP "dockSPEditor"
       
/* these strings are not seen by users */
#define TEXT_MODS "Biblical Texts"
#define COMM_MODS "Commentaries"
#define DICT_MODS "Lexicons / Dictionaries"
#define BOOK_MODS "Generic Books"

#define CIPHER_KEY_LEN 16
 
/*** function prototypes ***/

void init_gnomesword(SETTINGS *settings);
void gnomesword_shutdown(SETTINGS * s);
void UpdateChecks(SETTINGS *);
void setformatoption(GtkWidget * button);
void changepagenotebook(GtkNotebook * notebook, 
		gint page_num);
void setautosave(gboolean choice);
void percent_update(char percent, void *userData);
void set_appbar_progress(gint unit);
gint string_is_color(gchar *color);
gchar *gdouble_arr_to_hex(gdouble *color,
		gint websafe);
gdouble *hex_to_gdouble_arr(gchar *color);
void display_about_module_dialog(gchar *modname, gboolean isGBS);
gchar *get_module_key(SETTINGS *s);
gchar *get_module_name(SETTINGS *s);
gchar *get_module_name_from_description(gchar *description);
void change_module_and_key(gchar *module_name, gchar *key);
void change_verse(gchar *key);
void set_verse_style(gboolean choice);
void save_module_key(gchar * mod_name, gchar * key);
const char *get_sword_version(void);
void display_devotional(SETTINGS * s);
void display_new_font_color_and_size(SETTINGS * s);
gchar *get_module_description(gchar * mod_name);
const char *get_book_from_key(char *key);
int get_chapter_from_key(char *key);
int get_verse_from_key(char *key);
void save_properties(gboolean use_default);

#ifdef __cplusplus
}
#endif

#endif

