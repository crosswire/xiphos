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

#include <gnome.h>
	
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
 
typedef struct _search_opt SEARCH_OPT;
struct _search_opt {
	GtkWidget
	    *ckbCommentary,
	    *ckbPerCom, 
	    *ckbGBS, 
	    *rbPhraseSearch;

	gchar
	    *module_name,
	    *upper_bond, 
	    *lower_bond, 
	    *search_string;

	gint
	    search_type, 
	    search_params, 
	    found_count;

	 gboolean 
	    use_bonds, 
	    use_lastsearch_for_bonds;
};
/*** function prototypes ***/

void init_gnomesword(void);
void gnomesword_shutdown(void);
void search_percent_update(char percent, void *userData);
gint string_is_color(gchar *color);
gchar *gdouble_arr_to_hex(gdouble *color,
		gint websafe);
gdouble *hex_to_gdouble_arr(gchar *color);
gchar *get_module_key(void);
gchar *get_module_name(void);
int get_mod_type(char * mod_name);
char *get_module_font_name(char *mod_name);
char *get_module_font_size(char *mod_name);
void module_name_from_description(gchar *mod_name, gchar *description);
void change_module_and_key(gchar *module_name, gchar *key);
void change_verse(gchar *key);
void set_verse_style(gboolean choice);
void save_module_key(gchar * mod_name, gchar * key);
const char *get_sword_version(void);
void display_devotional(void);
void display_new_font_color_and_size(void);
gchar *get_module_description(gchar * mod_name);
const char *get_book_from_key(char *key);
int get_chapter_from_key(char *key);
int get_verse_from_key(char *key);
void save_properties(gboolean use_default);
GList *do_search(gpointer *usr_data);
GList *get_verse_list(gchar* module_name, gchar *verse_list);
GList *load_sb_group(gchar *filename, gchar *group_name, 
						gchar *icon_size);
void save_sb_group(gchar *file_name, gchar *group_name, gint group_num,
						     char *large_icons);
void save_sb_iconsize(gchar *file_name, char *icons);
gint get_num_shortcut_items(GtkWidget *shortcutbar_widget,
				     gint group_num); 
void get_shortcut_item_info(GtkWidget *shortcutbar_widget,
				     gint group_num,
				     gint item_num,
				     gchar ** item_url,
				     gchar ** item_name);
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

