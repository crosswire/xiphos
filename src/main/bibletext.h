/*
 * GnomeSword Bible Study Tool
 * bibletext.h - support for commentary modules
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

#ifndef __BIBLETEXT_H_
#define __BIBLETEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gs_gnomesword.h"
#include "settings.h"

typedef struct _text_find_dialog Text_Find_Dialog;
struct _text_find_dialog {
	GnomeDialog *dialog;
	GtkWidget *htmlwidget;
	GtkWidget *entry;
	GtkWidget *backward;
	GtkWidget *case_sensitive;
	gboolean regular;
};

typedef struct _text_data TEXT_DATA;
struct _text_data {
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *frame_toolbar;
	GtkWidget *showtabs;
	GtkWidget *showtoolbar;
	GtkWidget *t_btn_strongs;
	GtkWidget *t_btn_morphs;
	GtkWidget *t_btn_footnotes;
	GtkWidget *t_btn_accents;
	GtkWidget *t_btn_lemmas;
	GtkWidget *t_btn_scripturerefs;
	GtkWidget *t_btn_points;
	GtkWidget *t_btn_cant;
	GtkWidget *t_btn_headings;
	GtkWidget *t_btn_primary;
	GtkWidget *t_btn_secondary;
	GtkWidget *t_btn_all;
	gchar *mod_name;
	//gchar *mod_description;
	gchar *search_string;
	gchar *key;
	gint mod_num;
	gboolean is_locked;
	gboolean
	    gbfstrongs,
	    thmlstrongs,
	    gbfmorphs,
	    thmlmorphs,
	    gbffootnotes,
	    thmlfootnotes,
	    greekaccents,
	    lemmas,
	    scripturerefs,
	    hebrewpoints, 
	    hebrewcant, 
	    headings, 
	    variants;
	Text_Find_Dialog *find_dialog;
};

const char* text_get_description(int mod_num);
void text_page_changed(gint page_num, TEXT_DATA *t);
void set_text_page_and_key(gint page_num, gchar * key);
void set_text_module_global_option(gchar * option,
				   gboolean choice);
void set_text_variant_global_option(gchar * option,
				    gchar * choice);
void set_options_on_page_change(TEXT_DATA * t);
void setup_text(SETTINGS * s, GList *mods);
void shutdown_text(void);
void display_text(gchar * key);

	
const char *get_book_viewtext(void);	
int get_chapter_viewtext(void);
int get_verse_viewtext(void);
void goto_verse_viewtext(char * verse);
void load_module_viewtext(char * module_name);
void shutdown_viewtext(void);	
void set_global_options_viewtext(gchar *option, gboolean choice);
void setup_viewtext(GtkWidget * text);

#ifdef __cplusplus
}
#endif
#endif				/* __BIBLETEXT_H_ */
