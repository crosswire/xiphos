/*
 * GnomeSword Bible Study Tool
 * bibletext_dialog.h - view Bible text module in a dialog
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
 
#ifndef __MODULE_DIALOGS_H__
#define __MODULE_DIALOGS_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _dialog_data DIALOG_DATA;
	
#include "main/global_ops.hh"

struct _dialog_data {
	/* dialog widgets */
	GtkWidget *dialog;
	GtkWidget *toolbar_nav;
	GtkWidget *cbe_book;
	GtkWidget *spb_chapter;
	GtkWidget *spb_verse;
	GtkWidget *freeform_lookup;
	GtkWidget *statusbar;
        GtkWidget *listview;
	GtkWidget *tree; 
	/* widgets for text panes */
	GtkWidget *vbox;
	GtkWidget *html;
	GtkWidget *text;
	GtkWidget *frame;
        GtkWidget *entry;
	GtkWidget *showtabs;
	GtkWidget *module_options_menu;
	
	gchar *mod_name;
	gchar *search_string;
	gchar *key;
	gchar *cipher_key;
	gchar *cipher_old;
	
	gint mod_num;
	gint display_level;
	unsigned long offset;
	
	gboolean is_locked;
	gboolean is_percomm;
	gboolean is_dialog;
	gboolean is_rtol;
	gboolean is_leaf;
	gboolean book_heading;
	gboolean chapter_heading;
	gboolean sync;
	
	gpointer backend;
	gpointer editor;
	
        GObject *mod_selection;
	
	GLOBAL_OPS *ops;
};

void main_dialogs_add_children_to_tree(GtkTreeModel * model, GtkTreeIter iter,
	  unsigned long offset, gboolean is_dialog, DIALOG_DATA * d);
void main_dialogs_add_book_to_tree(GtkWidget * tree, gchar * mod_name, 
			     gboolean is_dialog, DIALOG_DATA * d);
void main_dialogs_tree_selection_changed(GtkTreeModel * model, 
	   GtkTreeSelection * selection, gboolean is_dialog, DIALOG_DATA * g);

void main_dialogs_dictionary_entery_changed(DIALOG_DATA * d);
void main_dialog_save_note(DIALOG_DATA * d);
void main_dialog_delete_note(DIALOG_DATA * d);

void main_dialog_update_controls(DIALOG_DATA * vt);
void main_bible_dialog_display(DIALOG_DATA * t);
void main_bible_dialog_passage_changed(DIALOG_DATA * t, gchar * passage);
void main_keep_bibletext_dialog_in_sync(gchar * key);
void main_dialogs_setup(void);
void main_dialogs_shutdown(void);
void main_free_on_destroy(DIALOG_DATA * vt);
void main_dialog_goto_bookmark(const gchar * module, const gchar * key);
void main_dialog_set_global_opt(gboolean choice);
gint main_dialogs_url_handler(DIALOG_DATA * t, const gchar * url, gboolean clicked);
void main_dialogs_open(gchar * mod_name);

#ifdef __cplusplus
}
#endif	
	
#endif


/* end of file */
