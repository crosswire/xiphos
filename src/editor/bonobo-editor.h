/*
 * Xiphos Bible Study Tool
 * html-editor.h - the html editor
 *
 * Copyright (C) 2005-2009 Xiphos Developer Team
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
 
#ifndef _BONOBO_EDITOR_H
#define _BONOBO_EDITOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <config.h>
#ifndef USE_GTKHTML3_14_23
	
#include <bonobo.h>
#include "editor/Editor.h"
	
	
#include "main/navbar_versekey.h"

typedef struct _editor EDITOR;
struct _editor {
	GtkWidget *window;
	GtkWidget *toolbar;
    	GtkWidget *treeview;
	GtkWidget *sync_button;
	GtkWidget *html_widget;
	GtkWidget *statusbar;

	NAVBAR_VERSEKEY navbar;
	
	BonoboWidget *control;
	GNOME_GtkHTML_Editor_Engine engine;
	Bonobo_PersistFile persist_file_interface;
	Bonobo_PersistStream persist_stream_interface;
	
	gint type;

	gboolean studypad;
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

void editor_sync_toggled(GtkToggleButton * button, EDITOR * e);
void editor_sync_with_main(void);
gboolean editor_close_all(void);
void editor_load_note(EDITOR * e, const gchar * module_name, const gchar * key);
void editor_load_book(EDITOR * e);
gint editor_create_new(const gchar * filename, const gchar * key, gint note);
void editor_save_book(EDITOR * e);
void editor_maybe_save_all(void);

#endif /* USE_GTKHTML3_14_23 */

#ifdef __cplusplus
}
#endif

#endif /* _BONOBO_EDITOR_H */
