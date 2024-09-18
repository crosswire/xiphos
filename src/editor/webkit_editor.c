/*
 * Xiphos Bible Study Tool
 * webkit_editor.c - html editor using webkit
 *
 * Copyright (C) 2005-2024 Xiphos Developer Team
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

#include <config.h>

#ifdef USE_WEBKIT_EDITOR

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include <glib.h>

#include <libintl.h>
#include <locale.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>

#include "editor/webkit_editor.h"
#include "editor/editor.h"
#include "editor/link_dialog.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/sword_treekey.h"
#include "main/url.hh"
#include "main/xml.h"

#include "gui/navbar_versekey_editor.h"
#include "gui/dialog.h"
#include "gui/widgets.h"
#include "gui/xiphos.h"
#include "gui/treekey-editor.h"
#include "gui/utilities.h"

#include "gui/debug_glib_null.h"
//TOOL_ITEMS toolitems;
MENU popup;
FIND_DIALOG find_dialog;

extern gboolean do_display;
extern BUTTONS_STATE buttons_state;

static void change_window_title(GtkWidget *window, const gchar *window_title);

static gboolean editor_is_dirty(EDITOR *e);

static void _load_file(EDITOR *e, const gchar *filename);
static void _save_file(EDITOR *e);
static void _save_note(EDITOR *e);
static void _save_book(EDITOR *e);

static GList *editors_all = NULL;

/******************************************************************************
 * Name
 *   on_about_dialog_response
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_about_dialog_response(GtkDialog * dialog, gint response_id,
 *			           gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

static void
on_about_dialog_response(GtkDialog *dialog,
			 gint response_id, gpointer user_data)
{
}

/******************************************************************************
 * Name
 *   action_about_activate_cb
 *
 * Synopsis
 *   #include "gui/about_sword.h"
 *
 *   void action_about_activate_cb(GtkWidget *widget, EDITOR *e);
 *
 * Description
 *   Create brief dialog for webkit editor.
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void
action_about_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_increase_indent_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_decrease_indent_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_insert_rule_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_insert_table_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_insert_emoticon_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_insert_image_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_insert_outline_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_justify_right_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_justify_left_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_justify_center_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_justify_full_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_bold_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_italic_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_undo_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_redo_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_underline_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_strikethrough_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void action_cut_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_copy_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_paste_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_delete_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_delete_item_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

void set_button_state(BUTTONS_STATE state, EDITOR *e)
{
}

G_MODULE_EXPORT void
colorbutton1_color_set_cb(GtkColorButton *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
colorbutton_highlight_color_set_cb(GtkColorButton *widget, EDITOR *e)
{
}


G_MODULE_EXPORT void
find_replace_response_cb(GtkDialog *dialog, gint response_id, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_find_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_replace_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void action_print_cb(GtkAction *action, EDITOR *e)
{
}

G_MODULE_EXPORT void action_print_preview_cb(GtkAction *action, EDITOR *e)
{
}

G_MODULE_EXPORT void action_quit_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void action_open_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void action_save_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void action_new_activate_cb(GtkWidget *widget, EDITOR *e)
{ /* for studypad only */
}

G_MODULE_EXPORT void
action_insert_sword_link_activate_cb(GtkWidget *widget, gpointer data)
{
}

G_MODULE_EXPORT void
action_insert_link_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void
action_save_as_activate_cb(GtkWidget *widget, EDITOR *e)
{
}

G_MODULE_EXPORT void combo_box_changed_cb(GtkComboBox *widget, EDITOR *e)
{
}

gboolean editor_is_dirty(EDITOR *e)
{
	return FALSE;
}

void editor_save_book(EDITOR *e)
{
}

/* save if needed is done in treeky-editor.c before calling editor_load_book() */
void editor_load_book(EDITOR *e)
{
}

/******************************************************************************
 * Name
 *   editor_sync_with_main
 *
 * Synopsis
 *   #include "editor/webkit_editor.h"
 *
 *   void editor_sync_with_main(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void editor_sync_with_main(void)
{
}

void
editor_load_note(EDITOR *e, const gchar *module_name, const gchar *key)
{
}

G_MODULE_EXPORT int
delete_event(GtkWidget *widget, GdkEvent *event, EDITOR *e)
{
	return FALSE;
}

/******************************************************************************
 * Name
 *   editor_create_new
 *
 * Synopsis
 *   #include "editor/html-editor.h"
 *
 *   gint editor_create_new(const gchar * filename, const gchar * key, gint note)
 *
 * Description
 *   limits editors to one note editor and one studypad open at a time
 *
 * Return value
 *   gint
 */

gint editor_create_new(const gchar *filename, const gchar *key,
		       gint editor_type)
{
	return 1;
}

void editor_maybe_save_all(void)
{
}

#endif /* USE_WEBKIT_EDITOR */
