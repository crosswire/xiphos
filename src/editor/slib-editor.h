/*
 * Xiphos Bible Study Tool
 * slib-editor.h - the html editor using gtkhtml-editor (slib)
 *
 * Copyright (C) 2005-2017 Xiphos Developer Team
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

#ifndef _SLIB_EDITOR_H
#define _SLIB_EDITOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>

#ifndef USE_WEBKIT_EDITOR

#include <gtk/gtk.h>

#include "main/navbar_versekey.h"

typedef struct _editor EDITOR;
struct _editor
{
	GtkWidget *window;
	GtkWidget *toolbar;
	GtkWidget *treeview;
	GtkWidget *sync_button;
	GtkWidget *html_widget;
	GtkWidget *statusbar;

	NAVBAR_VERSEKEY navbar;

	gint type;

	gboolean studypad;
	gboolean noteeditor;
	gboolean bookeditor;
	gboolean is_changed;
	gboolean sync;

	gchar *filename;
	gchar *module;
	gchar *key;
};

enum {
	STUDYPAD_EDITOR,
	NOTE_EDITOR,
	BOOK_EDITOR
};

/*
void button_test_clicked_cb(GtkObject *object, gpointer user_data);
void button_ok_clicked_cb(GtkObject *object, gpointer user_data);
void button_cancel_clicked_cb(GtkObject *object, gpointer user_data);*/
void editor_sync_with_main(void);
void editor_load_note(EDITOR *e, const gchar *module_name,
		      const gchar *key);
void editor_load_book(EDITOR *e);
gint editor_create_new(const gchar *filename, const gchar *key,
		       gint note);
void editor_save_book(EDITOR *e);
void editor_maybe_save_all(void);
GtkWidget *editor_new(const gchar *title, EDITOR *e);

#ifdef __cplusplus
}
#endif
#endif /* !USE_WEBKIT_EDITOR */
#endif /* _SLIB_EDITOR_H */
