/*
 * Xiphos Bible Study Tool
 * gtktextview_editor.h - editor using GtkTextView
 *
 * Copyright (C) 2005-2025 Xiphos Developer Team
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

#ifndef _GTKTEXTVIEW_EDITOR_H
#define _GTKTEXTVIEW_EDITOR_H

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_GTKTVeditor

#include <gtk/gtk.h>
#include "editor/slib-editor.h"

#include "main/navbar_versekey.h"

struct _find_dialog
{
	GtkWidget *window;
	GtkWidget *find_entry;
	GtkWidget *replace_entry;
	GtkWidget *box_replace;
	GtkWidget *button_replace;
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

/* toolbar callbacks */
void action_bold_activate_cb(GtkWidget *widget, EDITOR *e);
void action_italic_activate_cb(GtkWidget *widget, EDITOR *e);
void action_underline_activate_cb(GtkWidget *widget, EDITOR *e);
void action_strikethrough_activate_cb(GtkWidget *widget, EDITOR *e);
void action_undo_activate_cb(GtkWidget *widget, EDITOR *e);
void action_redo_activate_cb(GtkWidget *widget, EDITOR *e);
void action_cut_activate_cb(GtkWidget *widget, EDITOR *e);
void action_copy_activate_cb(GtkWidget *widget, EDITOR *e);
void action_paste_activate_cb(GtkWidget *widget, EDITOR *e);
void action_delete_activate_cb(GtkWidget *widget, EDITOR *e);
void action_delete_item_activate_cb(GtkWidget *widget, EDITOR *e);
void action_find_activate_cb(GtkWidget *widget, EDITOR *e);
void action_replace_activate_cb(GtkWidget *widget, EDITOR *e);
void action_new_activate_cb(GtkWidget *widget, EDITOR *e);
void action_open_activate_cb(GtkWidget *widget, EDITOR *e);
void action_save_activate_cb(GtkWidget *widget, EDITOR *e);
void action_save_as_activate_cb(GtkWidget *widget, EDITOR *e);
void action_quit_activate_cb(GtkWidget *widget, EDITOR *e);
void action_print_cb(GtkAction *action, EDITOR *e);
void action_insert_sword_link_activate_cb(GtkWidget *widget, gpointer data);
void action_insert_link_activate_cb(GtkWidget *widget, EDITOR *e);
void colorbutton1_color_set_cb(GtkColorButton *widget, EDITOR *e);
void combo_box_changed_cb(GtkComboBox *widget, EDITOR *e);
void find_replace_response_cb(GtkDialog *dialog, gint response_id, EDITOR *e);
void set_button_state(BUTTONS_STATE state, EDITOR *e);

int delete_event(GtkWidget *widget, GdkEvent *event, EDITOR *e);
gint ask_about_saving(EDITOR *e);

gboolean _on_event(GtkTextTag *tag, GObject *event_object,
                   GdkEvent *event, const GtkTextIter *iter,
                   EDITOR *e);

#endif /* USE_GTKTVeditor */

#ifdef __cplusplus
}
#endif
#endif /* _GTKTEXTVIEW_EDITOR_H */
