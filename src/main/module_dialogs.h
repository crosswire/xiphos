/*
 * Xiphos Bible Study Tool
 * module_dialog.h - view module in a dialog
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

#ifndef __MODULE_DIALOGS_H__
#define __MODULE_DIALOGS_H__

#include <config.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _dialog_data DIALOG_DATA;

#include "main/global_ops.hh"
#include "main/navbar_book.h"
#include "main/navbar_versekey.h"

struct _dialog_data
{
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
	GtkWidget *previewer;
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
	gint mod_type;
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

	NAVBAR_VERSEKEY navbar;
	NAVBAR_BOOK navbar_book;
};

void main_dialogs_book_heading(DIALOG_DATA *d);
void main_dialogs_chapter_heading(DIALOG_DATA *d);
void main_dialogs_information_viewer(DIALOG_DATA *d,
				     gchar *mod_name,
				     gchar *text, gchar *key,
				     gchar *action, gchar *type,
				     gchar *morph_text,
				     gchar *morph);
void main_dialogs_clear_viewer(DIALOG_DATA *d);
void main_dialogs_add_children_to_tree(GtkTreeModel *model,
				       GtkTreeIter iter,
				       unsigned long offset,
				       gboolean is_dialog,
				       DIALOG_DATA *d);
void main_dialogs_add_book_to_tree(GtkWidget *tree,
				   gchar *mod_name,
				   gboolean is_dialog,
				   DIALOG_DATA *d);
void main_dialogs_tree_selection_changed(GtkTreeModel *model,
					 GtkTreeSelection *
					     selection,
					 gboolean is_dialog,
					 DIALOG_DATA *g);

void main_dialogs_dictionary_entry_changed(DIALOG_DATA *d);
//void main_dialog_save_note(gpointer data);
//void main_dialog_delete_note(gpointer data);

//void main_dialog_update_controls(DIALOG_DATA * vt);
//void main_bible_dialog_display(DIALOG_DATA * t);
//void main_bible_dialog_passage_changed(DIALOG_DATA * t, gchar * passage);
void main_keep_bibletext_dialog_in_sync(gchar *key);
void main_dialogs_setup(void);
void main_dialogs_shutdown(void);
void main_free_on_destroy(DIALOG_DATA *vt);
void main_dialog_goto_bookmark(const gchar *module,
			       const gchar *key);
//void main_dialog_set_global_opt(gpointer backend, gboolean choice);
gint main_dialogs_url_handler(DIALOG_DATA *t, const gchar *url,
			      gboolean clicked);
DIALOG_DATA *main_dialogs_open(const gchar *mod_name,
			       const gchar *key,
			       const gboolean maximize);

#ifdef __cplusplus
}
#endif
#endif
/* end of file */
