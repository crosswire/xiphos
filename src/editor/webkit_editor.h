/*
 * Xiphos Bible Study Tool
 * webkit_editor.h - html editor using webkit 
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

#ifndef _WEBKIT_EDITOR_H
#define _WEBKIT_EDITOR_H

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_WEBKIT_EDITOR

#include <gtk/gtk.h>

#include "main/navbar_versekey.h"

struct _tool_items
{
	GtkToggleToolButton *bold;
	GtkToggleToolButton *italic;
	GtkToggleToolButton *underline;
	GtkToggleToolButton *strike;
	GtkColorButton *color;
	GtkToolButton *newdoc;
	GtkToolButton *open;
	GtkToolButton *deletedoc;
	GtkComboBoxText *cb;
	gint outline_level;
};
typedef struct _tool_items TOOL_ITEMS;

typedef struct _editor EDITOR;
struct _editor
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *scrollwindow;
	GtkWidget *toolbar;
	GtkWidget *treeview;
	GtkWidget *sync_button;
	GtkWidget *html_widget;
	GtkWidget *statusbar;
	GtkWidget *navbar_box;
	NAVBAR_VERSEKEY navbar;
	TOOL_ITEMS toolitems;

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

struct _menu
{
	GtkWidget *menu;
	GtkWidget *cut;
	GtkWidget *copy;
	GtkWidget *paste;
	//GtkWidget *;
};
typedef struct _menu MENU;

extern MENU popup;

struct _find_dialog
{
	GtkWidget *window;
	GtkWidget *find_entry;
	GtkWidget *replace_entry;
	GtkWidget *box_replace;
	GtkWidget *button_replace;
	//GtkWidget *;
	//GtkWidget *;
};
typedef struct _find_dialog FIND_DIALOG;

extern FIND_DIALOG find_dialog;

struct _buttons_state
{
	gint bold;
	gint italic;
	gint underline;
	gint strike;
	gint style;
	gint nochange;
	gchar *color;
};
typedef struct _buttons_state BUTTONS_STATE;

void action_about_activate_cb(GtkWidget *widget, EDITOR *e);

void action_increase_indent_activate_cb(GtkWidget *widget,
					EDITOR *e);
void action_decrease_indent_activate_cb(GtkWidget *widget,
					EDITOR *e);

void action_insert_image_activate_cb(GtkWidget *widget,
				     EDITOR *e);
void action_insert_rule_activate_cb(GtkWidget *widget,
				    EDITOR *e);
void action_insert_table_activate_cb(GtkWidget *widget,
				     EDITOR *e);
void action_insert_emoticon_activate_cb(GtkWidget *widget,
					EDITOR *e);
void action_insert_sword_link_activate_cb(GtkWidget *widget,
					  gpointer data);
void action_insert_link_activate_cb(GtkWidget *widget,
				    EDITOR *e);

void action_insert_outline_activate_cb(GtkWidget *widget,
				       EDITOR *e);

void action_justify_right_activate_cb(GtkWidget *widget,
				      EDITOR *e);
void action_justify_left_activate_cb(GtkWidget *widget,
				     EDITOR *e);
void action_justify_center_activate_cb(GtkWidget *widget,
				       EDITOR *e);
void action_justify_full_activate_cb(GtkWidget *widget,
				     EDITOR *e);

void action_bold_activate_cb(GtkWidget *widget, EDITOR *e);
void action_italic_activate_cb(GtkWidget *widget, EDITOR *e);
void action_underline_activate_cb(GtkWidget *widget, EDITOR *e);
void action_strikethrough_activate_cb(GtkWidget *widget,
				      EDITOR *e);

void action_undo_activate_cb(GtkWidget *widget, EDITOR *e);
void action_redo_activate_cb(GtkWidget *widget, EDITOR *e);

void action_cut_activate_cb(GtkWidget *widget, EDITOR *e);
void action_copy_activate_cb(GtkWidget *widget, EDITOR *e);
void action_paste_activate_cb(GtkWidget *widget, EDITOR *e);
void action_delete_activate_cb(GtkWidget *widget, EDITOR *e);
void action_delete_item_activate_cb(GtkWidget *widget,
				    EDITOR *e);

void set_button_state(BUTTONS_STATE state, EDITOR *e);
void action_fontcolor_activate_cb(GtkWidget *widget, EDITOR *e);
void action_font_activate_cb(GtkWidget *widget, EDITOR *e);
void find_replace_response_cb(GtkDialog *dialog, gint response_id,
			      EDITOR *e);
void action_replace_activate_cb(GtkWidget *widget, EDITOR *e);
void action_find_activate_cb(GtkWidget *widget, EDITOR *e);
int delete_event(GtkWidget *widget, GdkEvent *event, EDITOR *e);
void action_quit_activate_cb(GtkWidget *widget, EDITOR *e);
void action_new_activate_cb(GtkWidget *widget, EDITOR *e);
void action_open_activate_cb(GtkWidget *widget, EDITOR *e);
void action_print_cb(GtkAction *action, EDITOR *e);
void action_print_preview_cb(GtkAction *action, EDITOR *e);
void action_save_activate_cb(GtkWidget *widget, EDITOR *e);
void action_save_as_activate_cb(GtkWidget *widget, EDITOR *e);

void editor_sync_with_main(void);
void editor_load_note(EDITOR *e, const gchar *module_name,
		      const gchar *key);
void editor_load_book(EDITOR *e);
gint editor_create_new(const gchar *filename, const gchar *key,
		       gint note);
void editor_save_book(EDITOR *e);
void editor_maybe_save_all(void);
gint ask_about_saving(EDITOR *e);
void colorbutton1_color_set_cb(GtkColorButton *widget,
			       EDITOR *e);
void colorbutton_highlight_color_set_cb(GtkColorButton *widget,
					EDITOR *e);
void combo_box_changed_cb(GtkComboBox *widget, EDITOR *e);

#ifdef __cplusplus
}
#endif /* USE_WEBKIT_EDITOR */
#endif
#endif /* _WEBKIT_EDITOR_H */
