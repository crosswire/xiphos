/*
 * GnomeSword Bible Study Tool
 * bibletext.h - gui for Bible text modules
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

#ifndef ___BIBLETEXT_H_
#define ___BIBLETEXT_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "gui/mod_global_ops.h"
	

typedef struct _text_data TEXT_DATA;
struct _text_data {
	/* dialog widgets */
	GtkWidget *dialog;
	GtkWidget *toolbar_nav;
	GtkWidget *cbe_book;
	GtkWidget *spb_chapter;
	GtkWidget *spb_verse;
	GtkWidget *freeform_lookup;
	GtkWidget *statusbar;
	/* widgets for text panes */
	GtkWidget *vbox;
	GtkWidget *html;
	GtkWidget *text;
	GtkWidget *frame;
	GtkWidget *showtabs;
	GtkWidget *module_options_menu;
	
	gchar *mod_name;
	gchar *search_string;
	gchar *key;
	gchar *cipher_key;
	gchar *cipher_old;
	
	gint mod_num;
	
	gboolean is_locked;
	gboolean is_dialog;
	gboolean is_rtol;
	gboolean sync;
	
	GLOBAL_OPS *ops;
};
extern TEXT_DATA *cur_t;

void gui_update_text_global_ops(gchar * option, gboolean choice);
void gui_lookup_bibletext_selection(GtkMenuItem * menuitem,
				 gchar * dict_mod_description);
void gui_set_text_mod_and_key(gchar *mod, gchar *key);
void gui_unlock_bibletext(GtkMenuItem * menuitem, TEXT_DATA * t);
void gui_set_text_frame_label(TEXT_DATA * t);
void gui_set_text_page_and_key(gint page_num, gchar * key);
void gui_display_text(gchar * key);
void gui_add_new_text_pane(TEXT_DATA * t);
void gui_setup_text(GList *mods);
void gui_shutdown_text(void);
	
#ifdef __cplusplus
}
#endif

#endif
