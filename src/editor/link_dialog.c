/*
 * Xiphos Bible Study Tool
 * link_dialog.c - dialog for inserting a link
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

#include <config.h>

#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef USE_WEBKIT_EDITOR
#include "editor/webkit_editor.h"
#include "editor/editor.h"
#elif defined(USE_GTKTVeditor)
#include "editor/gtktextview_editor.h"
#else
#include <editor/gtkhtml-editor.h>
#include <gtkhtml/gtkhtml-stream.h>
#include "editor/slib-editor.h"
#endif

#include "editor/link_dialog.h"

#include "main/url.hh"
#include "main/sword.h"

#include "gui/utilities.h"
#include "gui/debug_glib_null.h"

/************* begin link dialog ****************/
static GtkWidget *window;
static GtkWidget *entry_module;
static GtkWidget *entry_verse;
static GtkWidget *entry_text;
static GtkWidget *linkage_verse_list;

G_MODULE_EXPORT void entry_verse_changed_cb(GObject *object, EDITOR *e)
{
	const gchar *verse_str = NULL;

	verse_str = gtk_entry_get_text(GTK_ENTRY(object));
	gtk_entry_set_text(GTK_ENTRY(entry_text), verse_str);
}

G_MODULE_EXPORT void button_ok_clicked_cb(GObject *object, EDITOR *e)
{
	const gchar *mod_str = NULL;
	const gchar *verse_str = NULL;
	const gchar *text_str = NULL;
	const gchar *encoded_mod = NULL;
	const gchar *encoded_verse = NULL;
	GString *str = g_string_new(NULL);
	gint type = 0;

	mod_str = gtk_entry_get_text(GTK_ENTRY(entry_module));
	verse_str = gtk_entry_get_text(GTK_ENTRY(entry_verse));
	text_str = gtk_entry_get_text(GTK_ENTRY(entry_text));

	type = main_get_mod_type((gchar *)mod_str);

	if (mod_str)
		encoded_mod = main_url_encode(mod_str);
	if (verse_str)
		encoded_verse = main_url_encode(verse_str);

	g_string_printf(str,
			(((gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(linkage_verse_list))) && ((type == -1) || (type == TEXT_TYPE) || (type == COMMENTARY_TYPE)))
			     ? "<a href=\"passagestudy.jsp?action=showRef&type=scripRef&module=%s&value=%s\">%s</a>"
			     : "<a href=\"sword://%s/%s\">%s</a>"),
			(encoded_mod ? encoded_mod : ""),
			(encoded_verse ? encoded_verse : ""),
			(text_str ? text_str : ""));

	XI_message(("link: %s", str->str));

#ifdef USE_WEBKIT_EDITOR
	editor_insert_html(str->str, e);
#elif defined(USE_GTKTVeditor)
{
	GtkTextBuffer *buffer =
		gtk_text_view_get_buffer(GTK_TEXT_VIEW(e->text_widget));
	GtkTextIter cursor;
	gtk_text_buffer_get_iter_at_mark(buffer, &cursor,
					 gtk_text_buffer_get_insert(buffer));

	/* construire l'URI directement depuis les champs */
	gchar *uri = NULL;
	if ((gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(linkage_verse_list))) &&
	    ((type == -1) || (type == TEXT_TYPE) || (type == COMMENTARY_TYPE)))
		uri = g_strdup_printf("passagestudy.jsp?action=showRef&type=scripRef&module=%s&value=%s",
				      encoded_mod ? encoded_mod : "",
				      encoded_verse ? encoded_verse : "");
	else
		uri = g_strdup_printf("sword://%s/%s",
				      encoded_mod ? encoded_mod : "",
				      encoded_verse ? encoded_verse : "");

	gchar *tag_name = g_strdup_printf("sword_link_%s", uri);
	GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
	GtkTextTag *tag = gtk_text_tag_table_lookup(table, tag_name);
	if (!tag) {
		tag = gtk_text_buffer_create_tag(buffer, tag_name,
						 "foreground", "#0000EE",
						 "underline", PANGO_UNDERLINE_SINGLE,
						 NULL);
		g_object_set_data_full(G_OBJECT(tag), "uri",
				       g_strdup(uri), g_free);
		g_signal_connect(tag, "event",
				 G_CALLBACK(_on_event), e);
	}
	gtk_text_buffer_insert_with_tags(buffer, &cursor,
					 text_str ? text_str : uri, -1, tag, NULL);
	g_free(tag_name);
	g_free(uri);
}

#else
	gtkhtml_editor_insert_html(GTKHTML_EDITOR(e->window), str->str);
#endif

	g_string_free(str, TRUE);
	g_free((gchar *)encoded_mod);
	g_free((gchar *)encoded_verse);

	gtk_widget_destroy(GTK_WIDGET(window));
}

G_MODULE_EXPORT
void button_test_clicked_cb(GObject *object, gpointer user_data)
{
	const gchar *mod_str = NULL;
	const gchar *verse_str = NULL;
	const gchar *encoded_mod = NULL;
	const gchar *encoded_verse = NULL;
	GString *str = g_string_new(NULL);

	mod_str = gtk_entry_get_text(GTK_ENTRY(entry_module));
	verse_str = gtk_entry_get_text(GTK_ENTRY(entry_verse));

	if (mod_str)
		encoded_mod = main_url_encode(mod_str);
	if (verse_str)
		encoded_verse = main_url_encode(verse_str);

	g_string_printf(str,
			"passagestudy.jsp?action=showRef&type=scripRef&module=%s&value=%s",
			(encoded_mod ? encoded_mod : ""),
			(encoded_verse ? encoded_verse : ""));
	XI_message(("link: %s", str->str));
	main_url_handler(str->str, TRUE);
	g_string_free(str, TRUE);
	g_free((gchar *)encoded_mod);
	g_free((gchar *)encoded_verse);
}

G_MODULE_EXPORT
void button_cancel_clicked_cb(GObject *object, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(window));
}

void editor_link_dialog(EDITOR *e)
{
	GtkBuilder *builder;
	gchar *gbuilder_file;

#ifndef USE_WEBKIT_EDITOR
#ifndef USE_GTKTVeditor
	GtkHTML *html = gtkhtml_editor_get_html(GTKHTML_EDITOR(e->window));
	if (html->pointer_url) /* are we in a link */
		return;	/* if so don't do anything */
#endif
#endif

	gbuilder_file =
	    gui_general_user_file("editor_link_dialog" UI_SUFFIX, FALSE);

#if GTK_CHECK_VERSION(3, 10, 0)
	builder = gtk_builder_new_from_file(gbuilder_file);
#else
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, gbuilder_file, NULL);
#endif

	window = GTK_WIDGET(gtk_builder_get_object(builder, "dialog1"));
	set_window_icon(GTK_WINDOW(window));
	gtk_builder_connect_signals(builder, (EDITOR *)e);

	entry_module =
	    GTK_WIDGET(gtk_builder_get_object(builder, "entry_module"));
	entry_verse =
	    GTK_WIDGET(gtk_builder_get_object(builder, "entry_verse"));
	entry_text =
	    GTK_WIDGET(gtk_builder_get_object(builder, "entry_text"));
	linkage_verse_list =
	    GTK_WIDGET(gtk_builder_get_object(builder, "radio_verse_list"));

	g_object_unref(G_OBJECT(builder));
	gtk_widget_show(window);
}

/************* end link dialog ****************/
