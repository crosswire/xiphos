/*
 * Xiphos Bible Study Tool
 * gtktextview_editor.c - editor using GtkTextView
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

#ifdef USE_GTKTVeditor

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include "editor/gtktextview_editor.h"
#include "editor/link_dialog.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/sword_treekey.h"
#include "main/url.hh"
#include "main/xml.h"
#include <libxml/HTMLparser.h>
#include <libxml/tree.h>

#include "gui/navbar_versekey_editor.h"
#include "gui/dialog.h"
#include "gui/widgets.h"
#include "gui/xiphos.h"
#include "gui/treekey-editor.h"
#include "gui/utilities.h"

#include "gui/debug_glib_null.h"

FIND_DIALOG find_dialog;
BUTTONS_STATE buttons_state;

static GList *editors_all = NULL;

/* forward declarations */
static void _load_file(EDITOR *e, const gchar *filename);
static void _save_file(EDITOR *e);
static void _save_note(EDITOR *e);
static void _save_book(EDITOR *e);
static gboolean editor_is_dirty(EDITOR *e);
static void do_exit(EDITOR *e);
static void change_window_title(GtkWidget *window, const gchar *title);
GtkWidget *editor_new(const gchar *title, EDITOR *e);
static void _setup_text_tags(GtkTextBuffer *buffer);
static gboolean _on_key_press(GtkWidget *widget, GdkEventKey *event, EDITOR *e);

/* ============================================================
 * TextTag names - used throughout for formatting
 * ============================================================ */
#define TAG_BOLD        "bold"
#define TAG_ITALIC      "italic"
#define TAG_UNDERLINE   "underline"
#define TAG_STRIKE      "strikethrough"
#define TAG_SWORD_LINK  "sword-link"

/* ============================================================
 * Helpers
 * ============================================================ */

static GtkTextBuffer *
_get_buffer(EDITOR *e)
{
	return gtk_text_view_get_buffer(GTK_TEXT_VIEW(e->text_widget));
}

static void
change_window_title(GtkWidget *window, const gchar *title)
{
	gtk_window_set_title(GTK_WINDOW(window), title);
}

static gboolean
editor_is_dirty(EDITOR *e)
{
	return e->is_changed;
}

/* ============================================================
 * Text tag setup
 * ============================================================ */

static void
_setup_text_tags(GtkTextBuffer *buffer)
{
	gtk_text_buffer_create_tag(buffer, TAG_BOLD,
				   "weight", PANGO_WEIGHT_BOLD,
				   NULL);
	gtk_text_buffer_create_tag(buffer, TAG_ITALIC,
				   "style", PANGO_STYLE_ITALIC,
				   NULL);
	gtk_text_buffer_create_tag(buffer, TAG_UNDERLINE,
				   "underline", PANGO_UNDERLINE_SINGLE,
				   NULL);
	gtk_text_buffer_create_tag(buffer, TAG_STRIKE,
				   "strikethrough", TRUE,
				   NULL);
	/* sword:// links - styled like hyperlinks */
	gtk_text_buffer_create_tag(buffer, TAG_SWORD_LINK,
				   "foreground", "#0000EE",
				   "underline", PANGO_UNDERLINE_SINGLE,
				   NULL);
				   gtk_text_buffer_create_tag(buffer, "h1", "weight", PANGO_WEIGHT_BOLD, "scale", 2.0, NULL);
	gtk_text_buffer_create_tag(buffer, "h2", "weight", PANGO_WEIGHT_BOLD, "scale", 1.7, NULL);
	gtk_text_buffer_create_tag(buffer, "h3", "weight", PANGO_WEIGHT_BOLD, "scale", 1.4, NULL);
	gtk_text_buffer_create_tag(buffer, "h4", "weight", PANGO_WEIGHT_BOLD, "scale", 1.2, NULL);
	gtk_text_buffer_create_tag(buffer, "h5", "weight", PANGO_WEIGHT_BOLD, "scale", 1.0, NULL);
	gtk_text_buffer_create_tag(buffer, "h6", "weight", PANGO_WEIGHT_BOLD, "scale", 0.8, NULL);
	gtk_text_buffer_create_tag(buffer, "address",
		"style", PANGO_STYLE_ITALIC,
		"left-margin", 20,
		"right-margin", 20,
		NULL);
	gtk_text_buffer_create_tag(buffer, "pre",
		"family", "Monospace",
		"wrap-mode", GTK_WRAP_NONE,
		NULL);
		/* list styles */
	gtk_text_buffer_create_tag(buffer, "list-bullet",
				"left-margin", 20,
		NULL);
	gtk_text_buffer_create_tag(buffer, "list-roman",
				"left-margin", 20,
				NULL);
	gtk_text_buffer_create_tag(buffer, "list-numbered",
				"left-margin", 20,
			NULL);
	gtk_text_buffer_create_tag(buffer, "list-alpha",
				"left-margin", 20,
				NULL);
}

/* ============================================================
 * Apply / remove a tag on the current selection
 * ============================================================ */

static void
_toggle_tag(EDITOR *e, const gchar *tag_name)
{
	GtkTextBuffer *buffer = _get_buffer(e);
	GtkTextIter start, end;

	if (!gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
		/* no selection - use current line */
		gtk_text_buffer_get_iter_at_mark(buffer, &start,
						 gtk_text_buffer_get_insert(buffer));
		end = start;
		gtk_text_iter_set_line_offset(&start, 0);
		gtk_text_iter_forward_to_line_end(&end);
	}

	GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
	GtkTextTag *tag = gtk_text_tag_table_lookup(table, tag_name);
	if (!tag)
		return;

	/* if entire selection already has tag, remove it; otherwise apply */
	if (gtk_text_iter_has_tag(&start, tag))
		gtk_text_buffer_remove_tag(buffer, tag, &start, &end);
	else
		gtk_text_buffer_apply_tag(buffer, tag, &start, &end);

	e->is_changed = TRUE;
}

/* ============================================================
 * Serialize buffer content to a string (plain text for now;
 * format to be confirmed by Karl - see GitHub discussion)
 * ============================================================ */

static gchar *
_serialize_buffer(EDITOR *e)
{
    GtkTextBuffer *buffer = _get_buffer(e);
    GtkTextIter iter, end;
    GString *html = g_string_new("");

    gtk_text_buffer_get_start_iter(buffer, &iter);
    gtk_text_buffer_get_end_iter(buffer, &end);

    while (!gtk_text_iter_equal(&iter, &end)) {
        /* tags ouvrants */
        GSList *tags = gtk_text_iter_get_toggled_tags(&iter, TRUE);
        for (GSList *t = tags; t; t = t->next) {
            GtkTextTag *tag = t->data;
            gchar *name = NULL;
            g_object_get(tag, "name", &name, NULL);
            if (!g_strcmp0(name, TAG_BOLD))
                g_string_append(html, "<b>");
            else if (!g_strcmp0(name, TAG_ITALIC))
                g_string_append(html, "<i>");
            else if (!g_strcmp0(name, TAG_UNDERLINE))
                g_string_append(html, "<u>");
            else if (!g_strcmp0(name, TAG_STRIKE))
                g_string_append(html, "<s>");
            else if (!g_strcmp0(name, "h1"))
                g_string_append(html, "<h1>");
            else if (!g_strcmp0(name, "h2"))
                g_string_append(html, "<h2>");
            else if (!g_strcmp0(name, "h3"))
                g_string_append(html, "<h3>");
            else if (!g_strcmp0(name, "h4"))
                g_string_append(html, "<h4>");
            else if (!g_strcmp0(name, "h5"))
                g_string_append(html, "<h5>");
            else if (!g_strcmp0(name, "h6"))
                g_string_append(html, "<h6>");
			else if (!g_strcmp0(name, "list-bullet"))
				g_string_append(html, "<ul><li>");
			else if (!g_strcmp0(name, "list-roman"))
				g_string_append(html, "<ol type=\"i\"><li>");
			else if (!g_strcmp0(name, "list-numbered"))
				g_string_append(html, "<ol><li>");
			else if (!g_strcmp0(name, "list-alpha"))
				g_string_append(html, "<ol type=\"a\"><li>");
			else if (g_str_has_prefix(name, "fg_"))
				g_string_append_printf(html, "<span style=\"color:%s\">", name + 3);
			else if (g_str_has_prefix(name, "bg_"))
				g_string_append_printf(html, "<span style=\"background-color:%s\">", name + 3);
			else if (g_str_has_prefix(name, "sword_link_")) {
				gchar *uri = g_object_get_data(G_OBJECT(tag), "uri");
				g_string_append_printf(html, "<a href=\"%s\">", uri ? uri : "");
			}
			else if (!g_strcmp0(name, "address"))
				g_string_append(html, "<address>");
			else if (!g_strcmp0(name, "pre"))
				g_string_append(html, "<pre>");
			g_free(name);
        }
        g_slist_free(tags);

        /* closing tags */
        tags = gtk_text_iter_get_toggled_tags(&iter, FALSE);
        for (GSList *t = tags; t; t = t->next) {
            GtkTextTag *tag = t->data;
            gchar *name = NULL;
            g_object_get(tag, "name", &name, NULL);
            if (!g_strcmp0(name, TAG_BOLD))
                g_string_append(html, "</b>");
            else if (!g_strcmp0(name, TAG_ITALIC))
                g_string_append(html, "</i>");
            else if (!g_strcmp0(name, TAG_UNDERLINE))
                g_string_append(html, "</u>");
            else if (!g_strcmp0(name, TAG_STRIKE))
                g_string_append(html, "</s>");
            else if (!g_strcmp0(name, "h1"))
                g_string_append(html, "</h1>");
            else if (!g_strcmp0(name, "h2"))
                g_string_append(html, "</h2>");
            else if (!g_strcmp0(name, "h3"))
                g_string_append(html, "</h3>");
            else if (!g_strcmp0(name, "h4"))
                g_string_append(html, "</h4>");
            else if (!g_strcmp0(name, "h5"))
                g_string_append(html, "</h5>");
            else if (!g_strcmp0(name, "h6"))
                g_string_append(html, "</h6>");
			else if (!g_strcmp0(name, "list-bullet"))
				g_string_append(html, "</li></ul>");
			else if (!g_strcmp0(name, "list-roman") ||
				!g_strcmp0(name, "list-numbered") ||
				!g_strcmp0(name, "list-alpha"))
			g_string_append(html, "</li></ol>");
            else if (g_str_has_prefix(name, "fg_") ||
                     g_str_has_prefix(name, "bg_"))
                g_string_append(html, "</span>");
            else if (g_str_has_prefix(name, "sword_link_"))
                g_string_append(html, "</a>");
            else if (!g_strcmp0(name, "address"))
				g_string_append(html, "</address>");
			else if (!g_strcmp0(name, "pre"))
					g_string_append(html, "</pre>");
            g_free(name);
        }
        g_slist_free(tags);

        /* caractère courant */
        if (gtk_text_iter_get_pixbuf(&iter)) {
            /* image — on ne peut pas sérialiser le pixbuf facilement,
             * on insère un placeholder pour l'instant */
            g_string_append(html, "<img src=\"placeholder\"/>");
        } else {
            gunichar c = gtk_text_iter_get_char(&iter);
            if (c == '<')
                g_string_append(html, "&lt;");
            else if (c == '>')
                g_string_append(html, "&gt;");
            else if (c == '&')
                g_string_append(html, "&amp;");
            else if (c == '\n')
                g_string_append(html, "<br/>");
            else {
                gchar buf[7];
                gint len = g_unichar_to_utf8(c, buf);
                buf[len] = '\0';
                g_string_append(html, buf);
            }
        }

        gtk_text_iter_forward_char(&iter);
    }

	/* no wrapper tags - store raw HTML for compatibility */
    return g_string_free(html, FALSE);
}

/* ============================================================
 * Load content into the buffer
 * ============================================================ */

static void
_parse_html_node(GtkTextBuffer *buffer, GtkTextIter *iter,
		 xmlNodePtr node, EDITOR *e)
{
	for (xmlNodePtr n = node; n; n = n->next) {
		if (n->type == XML_TEXT_NODE) {
			/* remplacer les entités HTML */
			gchar *content = (gchar *)n->content;
			if (content)
				gtk_text_buffer_insert(buffer, iter, content, -1);
		} else if (n->type == XML_ELEMENT_NODE) {
			GtkTextMark *mark_start =
				gtk_text_buffer_create_mark(buffer, NULL, iter, TRUE);

			if (n->children)
				_parse_html_node(buffer, iter, n->children, e);

			GtkTextIter start;
			gtk_text_buffer_get_iter_at_mark(buffer, &start, mark_start);
			gtk_text_buffer_delete_mark(buffer, mark_start);

			const gchar *tag = NULL;
			if (!g_ascii_strcasecmp((gchar *)n->name, "b"))
				tag = TAG_BOLD;
			else if (!g_ascii_strcasecmp((gchar *)n->name, "i"))
				tag = TAG_ITALIC;
			else if (!g_ascii_strcasecmp((gchar *)n->name, "u"))
				tag = TAG_UNDERLINE;
			else if (!g_ascii_strcasecmp((gchar *)n->name, "s"))
				tag = TAG_STRIKE;
			else if (!g_ascii_strcasecmp((gchar *)n->name, "h1"))
				tag = "h1";
			else if (!g_ascii_strcasecmp((gchar *)n->name, "h2"))
				tag = "h2";
			else if (!g_ascii_strcasecmp((gchar *)n->name, "h3"))
				tag = "h3";
			else if (!g_ascii_strcasecmp((gchar *)n->name, "h4"))
				tag = "h4";
			else if (!g_ascii_strcasecmp((gchar *)n->name, "h5"))
				tag = "h5";
			else if (!g_ascii_strcasecmp((gchar *)n->name, "h6"))
				tag = "h6";
			else if (!g_ascii_strcasecmp((gchar *)n->name, "address"))
				tag = "address";
			else if (!g_ascii_strcasecmp((gchar *)n->name, "ul"))
				tag = "list-bullet";
			else if (!g_ascii_strcasecmp((gchar *)n->name, "ol")) {
				xmlChar *type = xmlGetProp(n, (xmlChar *)"type");
				if (type && !g_ascii_strcasecmp((gchar *)type, "i"))
					tag = "list-roman";
				else if (type && !g_ascii_strcasecmp((gchar *)type, "a"))
					tag = "list-alpha";
				else
					tag = "list-numbered";
				if (type) xmlFree(type);
			}
			else if (!g_ascii_strcasecmp((gchar *)n->name, "pre"))
				tag = "pre";
			else if (!g_ascii_strcasecmp((gchar *)n->name, "br"))
				gtk_text_buffer_insert(buffer, iter, "\n", -1);
			else if (!g_ascii_strcasecmp((gchar *)n->name, "a")) {
				xmlChar *href = xmlGetProp(n, (xmlChar *)"href");
				if (href) {
					gchar *tag_name = g_strdup_printf("sword_link_%s",
									  (gchar *)href);
					GtkTextTagTable *table =
						gtk_text_buffer_get_tag_table(buffer);
					GtkTextTag *atag =
						gtk_text_tag_table_lookup(table, tag_name);
					if (!atag) {
						atag = gtk_text_buffer_create_tag(
							buffer, tag_name,
							"foreground", "#0000EE",
							"underline", PANGO_UNDERLINE_SINGLE,
							NULL);
						g_object_set_data_full(G_OBJECT(atag), "uri",
								       g_strdup((gchar *)href),
								       g_free);
						g_signal_connect(atag, "event",
								 G_CALLBACK(_on_event), e);
					}
					gtk_text_buffer_apply_tag_by_name(buffer, tag_name,
									  &start, iter);
					g_free(tag_name);
					xmlFree(href);
				}
			} else if (!g_ascii_strcasecmp((gchar *)n->name, "span")) {
				xmlChar *style = xmlGetProp(n, (xmlChar *)"style");
				if (style) {
					gchar *style_str = (gchar *)style;
					GtkTextTagTable *table =
						gtk_text_buffer_get_tag_table(buffer);
					if (g_str_has_prefix(style_str, "color:")) {
						gchar *color = style_str + 6;
						gchar *tag_name = g_strdup_printf("fg_%s", color);
						if (!gtk_text_tag_table_lookup(table, tag_name))
							gtk_text_buffer_create_tag(buffer, tag_name,
										   "foreground", color,
										   NULL);
						gtk_text_buffer_apply_tag_by_name(buffer, tag_name,
										  &start, iter);
						g_free(tag_name);
					} else if (g_str_has_prefix(style_str, "background-color:")) {
						gchar *color = style_str + 17;
						gchar *tag_name = g_strdup_printf("bg_%s", color);
						if (!gtk_text_tag_table_lookup(table, tag_name))
							gtk_text_buffer_create_tag(buffer, tag_name,
										   "background", color,
										   NULL);
						gtk_text_buffer_apply_tag_by_name(buffer, tag_name,
										  &start, iter);
						g_free(tag_name);
					}
					xmlFree(style);
				}
			}

			if (tag)
				gtk_text_buffer_apply_tag_by_name(buffer, tag,
								  &start, iter);
		}
	}
}

static void
_load_text_into_buffer(EDITOR *e, const gchar *text)
{
	GtkTextBuffer *buffer = _get_buffer(e);
	GtkTextIter iter;
	gtk_text_buffer_get_start_iter(buffer, &iter);

/* wrap in html/body for libxml2 parser */
gchar *wrapped = g_strdup_printf("<html><body>%s</body></html>", text);
htmlDocPtr doc = htmlReadMemory(wrapped, strlen(wrapped), NULL, "UTF-8",
				HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
g_free(wrapped);
if (!doc) {
	gtk_text_buffer_set_text(buffer, text, -1);
	e->is_changed = FALSE;
	return;
}

xmlNodePtr body = NULL;
xmlNodePtr root = xmlDocGetRootElement(doc);
for (xmlNodePtr n = root->children; n; n = n->next) {
	if (!g_ascii_strcasecmp((gchar *)n->name, "body")) {
		body = n;
		break;
	}
}

if (body)
	_parse_html_node(buffer, &iter, body->children, e);

	xmlFreeDoc(doc);
	e->is_changed = FALSE;
}

/* ============================================================
 * File I/O
 * ============================================================ */

static void
_load_file(EDITOR *e, const gchar *filename)
{
	GtkRecentManager *rm = NULL;
	gchar *text = NULL;
	GError *error = NULL;

	rm = gtk_recent_manager_get_default();
	gtk_recent_manager_add_item(rm, filename);

	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(filename);

	xml_set_value("Xiphos", "studypad", "lastfile", e->filename);
	settings.studypadfilename = xml_get_value("studypad", "lastfile");
	change_window_title(e->window, e->filename);

	g_file_get_contents(!strncmp(filename, "file:", 5)
				? filename + 5
				: filename,
			    &text, NULL, &error);
	if (error) {
		XI_message(("_load_file error: %s", error->message));
		g_error_free(error);
	}

	_load_text_into_buffer(e, text);
	if (text)
		g_free(text);
}

static void
_save_file(EDITOR *e)
{
	GtkRecentManager *rm = NULL;
	gchar *text = _serialize_buffer(e);

	if (!e->filename ||
	    (0 == g_strcmp0(_("Untitled document"), e->filename))) {

		GtkWidget *dialog =
		    gtk_file_chooser_dialog_new(_("Save as"),
						GTK_WINDOW(e->window),
						GTK_FILE_CHOOSER_ACTION_SAVE,
						"_Cancel", GTK_RESPONSE_CANCEL,
						"_OK", GTK_RESPONSE_OK,
						NULL);
		gtk_file_chooser_set_do_overwrite_confirmation(
		    GTK_FILE_CHOOSER(dialog), TRUE);
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),
						    settings.studypaddir);

		if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
			gchar *filename =
			    gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			if (e->filename)
				g_free(e->filename);
			e->filename = g_strdup(filename);
			GFile *gfile = g_file_parse_name(filename);
			g_file_replace_contents(gfile, text, strlen(text),
						NULL, TRUE,
						G_FILE_CREATE_NONE,
						NULL, NULL, NULL);
			g_free(filename);
			g_object_unref(gfile);
		}
		change_window_title(e->window, e->filename);
		gtk_widget_destroy(dialog);

	} else {
		GFile *gfile = g_file_parse_name(e->filename);
		g_file_replace_contents(gfile, text, strlen(text),
					NULL, TRUE,
					G_FILE_CREATE_NONE,
					NULL, NULL, NULL);
		g_object_unref(gfile);
	}

	rm = gtk_recent_manager_get_default();
	gtk_recent_manager_add_item(rm, e->filename);

	e->is_changed = FALSE;
	g_free(text);
}

static void
_save_note(EDITOR *e)
{
	gchar *text = _serialize_buffer(e);
	main_save_note(e->module, e->key, text);
	e->is_changed = FALSE;
	g_free(text);
}

static void
_save_book(EDITOR *e)
{
	gchar *text = _serialize_buffer(e);

	main_treekey_save_book_text(e->module, e->key, text);
	e->is_changed = FALSE;
	g_free(text);
}

/* ============================================================
 * Cleanup
 * ============================================================ */

static void
do_exit(EDITOR *e)
{
	if (e->filename)
		g_free(e->filename);
	if (e->module)
		g_free(e->module);
	if (e->key)
		g_free(e->key);
	if (e->window)
		gtk_widget_destroy(e->window);
	g_free(e);
}

/* ============================================================
 * Ask about saving dialog
 * ============================================================ */

gint
ask_about_saving(EDITOR *e)
{
	gint test;
	GS_DIALOG *info;
	gchar *buf = NULL;
	gchar *buf1 = NULL;
	gchar *buf2 = NULL;
	gchar *buf3 = NULL;
	gint retval = FALSE;

	switch (e->type) {
	case BOOK_EDITOR:
	case NOTE_EDITOR:
		info = gui_new_dialog();
		info->stock_icon = "dialog-warning";
		buf = g_strdup_printf("%s: %s", e->module, e->key);
		buf1 = _("Save the changes to document");
		buf2 = _("before closing?");
		buf3 = g_strdup_printf(
		    "<span weight=\"bold\" size=\"larger\">%s %s %s</span>",
		    buf1, buf, buf2);
		info->label_top = buf3;
		info->label2 = _("If you don't save, changes will be permanently lost.");
		info->save = TRUE;
		info->cancel = TRUE;
		info->no_save = TRUE;
		test = gui_alert_dialog(info);
		retval = test;
		if (test == GS_YES) {
			if (e->type == NOTE_EDITOR)
				_save_note(e);
			else
				_save_book(e);
		}
		g_free(info);
		g_free(buf);
		g_free(buf3);
		break;

	case STUDYPAD_EDITOR:
		info = gui_new_dialog();
		info->stock_icon = "dialog-warning";
		buf = settings.studypadfilename
			  ? settings.studypadfilename
			  : N_("File");
		buf1 = _("Save the changes to document");
		buf2 = _("before closing?");
		buf3 = g_strdup_printf(
		    "<span weight=\"bold\" size=\"larger\">%s %s %s</span>",
		    buf1, buf, buf2);
		info->label_top = buf3;
		info->label2 = _("If you don't save, changes will be permanently lost.");
		info->save = TRUE;
		info->cancel = TRUE;
		info->no_save = TRUE;
		test = gui_alert_dialog(info);
		retval = test;
		if (test == GS_YES)
			_save_file(e);
		g_free(info);
		g_free(buf3);
		break;
	}
	sync_windows();
	return retval;
}

/* ============================================================
 * Toolbar callbacks
 * ============================================================ */

G_MODULE_EXPORT void
action_bold_activate_cb(GtkWidget *widget, EDITOR *e)
{
    if (buttons_state.nochange)
        return;
    _toggle_tag(e, TAG_BOLD);
}

G_MODULE_EXPORT void
action_italic_activate_cb(GtkWidget *widget, EDITOR *e)
{
	if (buttons_state.nochange)
		return;
	_toggle_tag(e, TAG_ITALIC);
}

G_MODULE_EXPORT void
action_underline_activate_cb(GtkWidget *widget, EDITOR *e)
{
	if (buttons_state.nochange)
		return;
	_toggle_tag(e, TAG_UNDERLINE);
}

G_MODULE_EXPORT void
action_strikethrough_activate_cb(GtkWidget *widget, EDITOR *e)
{
	if (buttons_state.nochange)
		return;
	_toggle_tag(e, TAG_STRIKE);
}

G_MODULE_EXPORT void
action_undo_activate_cb(GtkWidget *widget, EDITOR *e)
{
	/* TODO: GtkTextBuffer does not have built-in undo before GTK 3.96.
	 * For GTK < 4, use a third-party undo manager or implement manually. */
	XI_message(("%s", "undo: not yet implemented"));
}

G_MODULE_EXPORT void
action_redo_activate_cb(GtkWidget *widget, EDITOR *e)
{
	XI_message(("%s", "redo: not yet implemented"));
}

G_MODULE_EXPORT void
action_cut_activate_cb(GtkWidget *widget, EDITOR *e)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_cut_clipboard(_get_buffer(e), clipboard, TRUE);
}

G_MODULE_EXPORT void
action_copy_activate_cb(GtkWidget *widget, EDITOR *e)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_copy_clipboard(_get_buffer(e), clipboard);
}

G_MODULE_EXPORT void
action_paste_activate_cb(GtkWidget *widget, EDITOR *e)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_paste_clipboard(_get_buffer(e), clipboard, NULL, TRUE);
}

G_MODULE_EXPORT void
action_delete_activate_cb(GtkWidget *widget, EDITOR *e)
{
	GtkTextBuffer *buffer = _get_buffer(e);
	GtkTextIter start, end;

	if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end))
		gtk_text_buffer_delete(buffer, &start, &end);
}

G_MODULE_EXPORT void
action_delete_item_activate_cb(GtkWidget *widget, EDITOR *e)
{
	if (e->studypad)
		return;

	gchar *buf = g_strdup_printf(
	    "<span weight=\"bold\" size=\"larger\">%s %s?</span>",
	    _("Are you sure you want to delete the note for"), e->key);

	if (gui_yes_no_dialog(buf, "dialog-warning")) {
		main_delete_note(e->module, e->key);
		_load_text_into_buffer(e, "");
	}
	g_free(buf);
	e->is_changed = FALSE;
}

G_MODULE_EXPORT void
action_new_activate_cb(GtkWidget *widget, EDITOR *e)
{
	if (e->is_changed)
		ask_about_saving(e);

	_load_text_into_buffer(e, "");

	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(_("Untitled document"));

	xml_set_value("Xiphos", "studypad", "lastfile", e->filename);
	settings.studypadfilename = xml_get_value("studypad", "lastfile");
	change_window_title(e->window, e->filename);
	e->is_changed = TRUE;
}

G_MODULE_EXPORT void
action_open_activate_cb(GtkWidget *widget, EDITOR *e)
{
	GtkWidget *dialog =
	    gtk_file_chooser_dialog_new(_("Open"), GTK_WINDOW(e->window),
					GTK_FILE_CHOOSER_ACTION_OPEN,
					"_Cancel", GTK_RESPONSE_CANCEL,
					"_Open", GTK_RESPONSE_ACCEPT,
					NULL);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),
					    settings.studypaddir);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		gchar *filename =
		    gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		_load_file(e, filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
}

G_MODULE_EXPORT void
action_save_activate_cb(GtkWidget *widget, EDITOR *e)
{
	switch (e->type) {
	case STUDYPAD_EDITOR:
		_save_file(e);
		break;
	case NOTE_EDITOR:
		_save_note(e);
		break;
	case BOOK_EDITOR:
		_save_book(e);
		break;
	default:
		XI_message(("\naction_save_cb oops!\n"));
		break;
	}
}

G_MODULE_EXPORT void
action_save_as_activate_cb(GtkWidget *widget, EDITOR *e)
{
	if (e->filename)
		g_free(e->filename);
	e->filename = NULL;
	_save_file(e);
}

G_MODULE_EXPORT void
action_quit_activate_cb(GtkWidget *widget, EDITOR *e)
{
	delete_event(NULL, NULL, e);
}

G_MODULE_EXPORT void
action_print_cb(GtkAction *action, EDITOR *e)
{
	/* TODO: implement printing via GtkPrintOperation */
	XI_message(("%s", "print: not yet implemented"));
}

G_MODULE_EXPORT void
action_insert_sword_link_activate_cb(GtkWidget *widget, gpointer data)
{
	editor_link_dialog((EDITOR *)data);
}

G_MODULE_EXPORT void
action_insert_link_activate_cb(GtkWidget *widget, EDITOR *e)
{
	editor_link_dialog(e);
}

G_MODULE_EXPORT void
colorbutton1_color_set_cb(GtkColorButton *widget, EDITOR *e)
{
	GtkTextBuffer *buffer = _get_buffer(e);
	GtkTextIter start, end;

	if (!gtk_text_buffer_get_selection_bounds(buffer, &start, &end))
		return;

	GdkRGBA color;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget), &color);
	gchar *color_str = gdk_rgba_to_string(&color);

	/* créer un tag unique pour cette couleur */
	gchar *tag_name = g_strdup_printf("fg_%s", color_str);
	GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
	if (!gtk_text_tag_table_lookup(table, tag_name))
		gtk_text_buffer_create_tag(buffer, tag_name,
		                           "foreground", color_str, NULL);
	gtk_text_buffer_apply_tag_by_name(buffer, tag_name, &start, &end);

	g_free(color_str);
	g_free(tag_name);
	e->is_changed = TRUE;
}

G_MODULE_EXPORT void
colorbutton_highlight_color_set_cb(GtkColorButton *widget, EDITOR *e)
{
    GtkTextBuffer *buffer = _get_buffer(e);
    GtkTextIter start, end;

    if (!gtk_text_buffer_get_selection_bounds(buffer, &start, &end))
        return;

    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget), &color);
    gchar *color_str = gdk_rgba_to_string(&color);

    gchar *tag_name = g_strdup_printf("bg_%s", color_str);
    GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
    if (!gtk_text_tag_table_lookup(table, tag_name))
        gtk_text_buffer_create_tag(buffer, tag_name,
                                   "background", color_str, NULL);
    gtk_text_buffer_apply_tag_by_name(buffer, tag_name, &start, &end);

    g_free(color_str);
    g_free(tag_name);
    e->is_changed = TRUE;
}

G_MODULE_EXPORT void
combo_box_changed_cb(GtkComboBox *widget, EDITOR *e)
{
	if (buttons_state.nochange)
		return;

	GtkTextBuffer *buffer = _get_buffer(e);
	GtkTextIter start, end;

if (!gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
		/* no selection - use current line */
		gtk_text_buffer_get_iter_at_mark(buffer, &start,
						 gtk_text_buffer_get_insert(buffer));
		end = start;
		gtk_text_iter_set_line_offset(&start, 0);
		gtk_text_iter_forward_to_line_end(&end);
	}
	gint choice = gtk_combo_box_get_active(widget);

	/* detect previous list tag BEFORE removing tags */
	const gchar *prev_tags[] = {
		"list-bullet", "list-roman", "list-numbered", "list-alpha", NULL
	};
	const gchar *prefixes[] = { "\u2022 ", "i. ", "1. ", "a. " };
	gint prev_choice = -1;
	for (gint i = 0; prev_tags[i]; i++) {
		if (gtk_text_iter_has_tag(&start,
			gtk_text_tag_table_lookup(
				gtk_text_buffer_get_tag_table(buffer),
				prev_tags[i]))) {
			prev_choice = i;
			break;
		}
	}

	/* remove all existing style tags */
	const gchar *style_tags[] = {
		"h1", "h2", "h3", "h4", "h5", "h6",
		"address", "pre",
		"list-bullet", "list-roman", "list-numbered", "list-alpha", NULL
	};
	for (gint i = 0; style_tags[i]; i++)
		gtk_text_buffer_remove_tag_by_name(buffer, style_tags[i], &start, &end);

	/* if switching back to Normal, remove list prefix */
	if (prev_choice >= 0 && choice == 0) {
		GtkTextIter line_start = start;
		gtk_text_iter_set_line_offset(&line_start, 0);
		GtkTextIter prefix_end = line_start;
		const gchar *prefix = prefixes[prev_choice];
		gtk_text_iter_forward_chars(&prefix_end, g_utf8_strlen(prefix, -1));
		gchar *line_text = gtk_text_buffer_get_text(buffer,
							    &line_start, &prefix_end, FALSE);
		if (!g_strcmp0(line_text, prefix))
			gtk_text_buffer_delete(buffer, &line_start, &prefix_end);
		g_free(line_text);
	}

	const gchar *tag = NULL;
	switch (choice) {
	case 1:  tag = "h1";           break;
	case 2:  tag = "h2";           break;
	case 3:  tag = "h3";           break;
	case 4:  tag = "h4";           break;
	case 5:  tag = "h5";           break;
	case 6:  tag = "h6";           break;
	case 7:  tag = "address";      break;
	case 8:  tag = "pre";          break;
	case 9:  tag = "list-bullet";  break;
	case 10: /* list-roman - not yet implemented */   break;
	case 11: /* list-numbered - not yet implemented */break;
	case 12: /* list-alpha - not yet implemented */   break;
	default: break;
	}

	if (tag) {
		gtk_text_buffer_apply_tag_by_name(buffer, tag, &start, &end);

		/* insert list prefix at beginning of line */
		const gchar *prefix = NULL;
		if (!g_strcmp0(tag, "list-bullet"))    prefix = "\u2022 ";
		if (!g_strcmp0(tag, "list-roman"))     prefix = "i. ";
		if (!g_strcmp0(tag, "list-numbered"))  prefix = "1. ";
		if (!g_strcmp0(tag, "list-alpha"))     prefix = "a. ";
		if (prefix) {
			GtkTextIter line_start = start;
			gtk_text_iter_set_line_offset(&line_start, 0);
			gtk_text_buffer_insert(buffer, &line_start, prefix, -1);
		}
	}

	e->is_changed = TRUE;
}

G_MODULE_EXPORT void
find_replace_response_cb(GtkDialog *dialog, gint response_id, EDITOR *e)
{
	GtkTextBuffer *buffer = _get_buffer(e);
	GtkTextIter start, end, match_start, match_end;
	const gchar *needle;
	gboolean found;

	switch (response_id) {
	case GTK_RESPONSE_CANCEL:
		gtk_widget_hide(find_dialog.window);
		break;
	case 1: /* Find */
		needle = gtk_entry_get_text(GTK_ENTRY(find_dialog.find_entry));
		gtk_text_buffer_get_start_iter(buffer, &start);
		found = gtk_text_iter_forward_search(&start, needle,
						     GTK_TEXT_SEARCH_CASE_INSENSITIVE,
						     &match_start, &match_end,
						     NULL);
		if (found) {
			gtk_text_buffer_select_range(buffer,
						     &match_start, &match_end);
			gtk_text_view_scroll_to_iter(
			    GTK_TEXT_VIEW(e->text_widget),
			    &match_start, 0.0, FALSE, 0.0, 0.0);
		}
		break;
	case 2: /* Replace */
		needle = gtk_entry_get_text(GTK_ENTRY(find_dialog.find_entry));
		const gchar *replacement =
		    gtk_entry_get_text(GTK_ENTRY(find_dialog.replace_entry));
		if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
			gtk_text_buffer_delete(buffer, &start, &end);
			gtk_text_buffer_insert(buffer, &start,
					       replacement, -1);
		}
		break;
	default:
		gtk_widget_hide(find_dialog.window);
		break;
	}
}

G_MODULE_EXPORT void
action_find_activate_cb(GtkWidget *widget, EDITOR *e)
{
	gtk_widget_show(find_dialog.window);
	gtk_widget_hide(find_dialog.box_replace);
	gtk_widget_hide(find_dialog.button_replace);
}

G_MODULE_EXPORT void
action_replace_activate_cb(GtkWidget *widget, EDITOR *e)
{
	gtk_widget_show(find_dialog.window);
	gtk_widget_show(find_dialog.box_replace);
	gtk_widget_show(find_dialog.button_replace);
}

void
set_button_state(BUTTONS_STATE state, EDITOR *e)
{
	gtk_toggle_tool_button_set_active(e->toolitems.bold, state.bold);
	gtk_toggle_tool_button_set_active(e->toolitems.italic, state.italic);
	gtk_toggle_tool_button_set_active(e->toolitems.underline, state.underline);
	gtk_toggle_tool_button_set_active(e->toolitems.strike, state.strike);
	gtk_combo_box_set_active((GtkComboBox *)e->toolitems.cb, state.style);
}

/* ============================================================
 * delete_event / window close
 * ============================================================ */

G_MODULE_EXPORT int
delete_event(GtkWidget *widget, GdkEvent *event, EDITOR *e)
{
	if (e->is_changed) {
		switch (ask_about_saving(e)) {
		case GS_YES:
		case GS_NO:
			break;
		case GS_CANCEL:
			return TRUE;
		}
	}
	editors_all = g_list_remove(editors_all, e);
	do_exit(e);
	return FALSE;
}

/* ============================================================
 * Signals
 * ============================================================ */

static void
_on_buffer_changed(GtkTextBuffer *buffer, EDITOR *e)
{
	e->is_changed = TRUE;
}

/* handle sword:// link clicks */
gboolean
_on_event(GtkTextTag *tag, GObject *event_object,
	  GdkEvent *event, const GtkTextIter *iter,
	  EDITOR *e)
{
	if (event->type == GDK_BUTTON_PRESS) {
		GtkTextBuffer *buffer = _get_buffer(e);
		GtkTextIter start = *iter, end = *iter;

		gtk_text_iter_backward_to_tag_toggle(&start, tag);
		gtk_text_iter_forward_to_tag_toggle(&end, tag);

		gchar *uri = g_object_get_data(G_OBJECT(tag), "uri");
		if (uri) {
			XI_message(("sword link clicked: %s", uri));
			main_url_handler(uri, TRUE);
		}

		return TRUE;
	}
	return FALSE;
}


/* ============================================================
 * Create on_key_press
 * ============================================================ */
 
 static gboolean
_on_key_press(GtkWidget *widget, GdkEventKey *event, EDITOR *e)

{
	if (event->keyval != GDK_KEY_Return)
		return FALSE;

	GtkTextBuffer *buffer = _get_buffer(e);
	GtkTextIter iter;
	gtk_text_buffer_get_iter_at_mark(buffer, &iter,
					 gtk_text_buffer_get_insert(buffer));

	GtkTextIter line_start = iter;
	gtk_text_iter_set_line_offset(&line_start, 0);
		
	const gchar *active_tag = NULL;
	GtkTextIter scan = line_start;
	GtkTextIter line_end = line_start;
	gtk_text_iter_forward_to_line_end(&line_end);
	while (!gtk_text_iter_equal(&scan, &line_end)) {
		gtk_text_iter_forward_char(&scan);
		GtkTextTag *tag = gtk_text_tag_table_lookup(
			gtk_text_buffer_get_tag_table(buffer), "list-bullet");
		if (gtk_text_iter_has_tag(&scan, tag)) {
			active_tag = "list-bullet";
			break;
		}
	}

	if (!active_tag){
		return FALSE;
	}

	/* count previous lines with same tag */
	gint count = 0;
	scan = line_start;
	while (gtk_text_iter_backward_line(&scan)) {
		GtkTextTag *tag = gtk_text_tag_table_lookup(
			gtk_text_buffer_get_tag_table(buffer), active_tag);
		if (gtk_text_iter_has_tag(&scan, tag))
			count++;
		else
			break;
	}

	/* build prefix */
	gchar *prefix = NULL;
	if (!g_strcmp0(active_tag, "list-bullet"))
		prefix = g_strdup("\u2022 ");

	if (prefix) {
		gchar *text = g_strdup_printf("\n%s", prefix);
		gtk_text_buffer_insert_with_tags_by_name(buffer, &iter,
							 text, -1, active_tag, NULL);
		g_free(text);
		g_free(prefix);
		return TRUE;
	}

	return FALSE;
}

/* ============================================================
 * Create editor widget
 * ============================================================ */

static void
create_editor_window(GtkWidget *scrollwindow, EDITOR *e)
{
	GtkWidget *textview = gtk_text_view_new();
	GtkTextBuffer *buffer =
	    gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));

	e->text_widget = textview;

	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), TRUE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD_CHAR);
	gtk_widget_show(textview);

	_setup_text_tags(buffer);

	/* wire sword link click handler */
	GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
	GtkTextTag *link_tag = gtk_text_tag_table_lookup(table, TAG_SWORD_LINK);
	g_signal_connect(link_tag, "event",
			 G_CALLBACK(_on_event), e);

	/* track modifications */
	g_signal_connect(buffer, "changed",
			 G_CALLBACK(_on_buffer_changed), e);

	gtk_container_add(GTK_CONTAINER(scrollwindow), textview);
	e->is_changed = FALSE;
	buttons_state.nochange = 0;
	g_signal_connect(textview, "key-press-event",
			 G_CALLBACK(_on_key_press), e);
}

/* ============================================================
 * editor_new - build the editor window from .ui file
 * ============================================================ */

GtkWidget *
editor_new(const gchar *title, EDITOR *e)
{
	GtkWidget *window;
	GtkWidget *scrollwindow;
	GtkBuilder *builder;
	gchar *gbuilder_file;
	GError *error = NULL;
	GtkMenuItem *item;
	GtkWidget *recent_item;

	buttons_state.nochange = 1;

	/* reuse the existing webkit editor .ui file for now;
	 * html_widget references will need renaming once a dedicated
	 * gtk_tvedit.ui is created */
	gbuilder_file = gui_general_user_file("gtk_webedit.ui", FALSE);
	builder = gtk_builder_new();

	if (!gtk_builder_add_from_file(builder, gbuilder_file, &error)) {
		g_warning("Couldn't load builder file: %s", error->message);
		g_error_free(error);
	}

	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	e->window = window;
	gtk_window_set_title(GTK_WINDOW(window), title);

	e->toolitems.bold = GTK_TOGGLE_TOOL_BUTTON(
	    gtk_builder_get_object(builder, "toolbutton_bold"));
	e->toolitems.italic = GTK_TOGGLE_TOOL_BUTTON(
	    gtk_builder_get_object(builder, "toolbutton_italic"));
	e->toolitems.underline = GTK_TOGGLE_TOOL_BUTTON(
	    gtk_builder_get_object(builder, "toolbuttonunderline"));
	e->toolitems.strike = GTK_TOGGLE_TOOL_BUTTON(
	    gtk_builder_get_object(builder, "toolbutton_strikethrough"));
	e->toolitems.open = GTK_TOOL_BUTTON(
	    gtk_builder_get_object(builder, "toolbutton_open"));
	e->toolitems.newdoc = GTK_TOOL_BUTTON(
	    gtk_builder_get_object(builder, "toolbutton_new"));
	e->toolitems.deletedoc = GTK_TOOL_BUTTON(
	    gtk_builder_get_object(builder, "toolbutton_delete"));
	e->toolitems.color = GTK_COLOR_BUTTON(
	    gtk_builder_get_object(builder, "colorbutton1"));
	e->toolitems.cb = GTK_COMBO_BOX_TEXT(
	    gtk_builder_get_object(builder, "comboboxtext1"));

	gtk_combo_box_set_active((GtkComboBox *)e->toolitems.cb, 0);

	item = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menuitem_recent"));

	switch (e->type) {
	case STUDYPAD_EDITOR:
		gtk_widget_hide(GTK_WIDGET(e->toolitems.deletedoc));
		recent_item = gtk_recent_chooser_menu_new();
		gtk_menu_item_set_submenu(item, recent_item);
		break;
	case NOTE_EDITOR:
		if (e->toolitems.open)
			gtk_widget_hide(GTK_WIDGET(e->toolitems.open));
		if (e->toolitems.newdoc)
			gtk_widget_hide(GTK_WIDGET(e->toolitems.newdoc));
		if (item)
			gtk_widget_hide(GTK_WIDGET(item));
		break;
	case BOOK_EDITOR:
		if (e->toolitems.open)
			gtk_widget_hide(GTK_WIDGET(e->toolitems.open));
		if (e->toolitems.newdoc)
			gtk_widget_hide(GTK_WIDGET(e->toolitems.newdoc));
		if (item)
			gtk_widget_hide(GTK_WIDGET(item));
		break;
	}

	e->navbar_box = GTK_WIDGET(gtk_builder_get_object(builder, "box_navbar"));
	e->box = GTK_WIDGET(gtk_builder_get_object(builder, "vbox1"));

	scrollwindow = GTK_WIDGET(
	    gtk_builder_get_object(builder, "scrolledwindow1"));
	create_editor_window(scrollwindow, e);
	e->is_changed = FALSE;

	gtk_builder_connect_signals(builder, (EDITOR *)e);

	find_dialog.window = GTK_WIDGET(
	    gtk_builder_get_object(builder, "dialog_find_replace"));
	find_dialog.find_entry = GTK_WIDGET(
	    gtk_builder_get_object(builder, "entry1"));
	find_dialog.replace_entry = GTK_WIDGET(
	    gtk_builder_get_object(builder, "entry2"));
	find_dialog.box_replace = GTK_WIDGET(
	    gtk_builder_get_object(builder, "box4"));
	find_dialog.button_replace = GTK_WIDGET(
	    gtk_builder_get_object(builder, "button_replace"));

	g_object_unref(builder);
	return window;
}

/* ============================================================
 * Public API
 * ============================================================ */

void
editor_load_note(EDITOR *e, const gchar *module_name, const gchar *key)
{
	gchar *title = NULL, *text = NULL;

	if (e->is_changed)
		_save_note(e);

	if (module_name) {
		if (e->module)
			g_free(e->module);
		e->module = g_strdup(module_name);
	}
	if (key) {
		if (e->key)
			g_free(e->key);
		e->key = g_strdup(key);
	}

	text = main_get_raw_text((gchar *)e->module, (gchar *)e->key);
	_load_text_into_buffer(e, text);
	if (text)
		g_free(text);

	if (e->type == NOTE_EDITOR) {
		e->navbar.valid_key = TRUE;
		main_navbar_versekey_set(e->navbar, e->key);
	}

	title = g_strdup_printf("%s - %s", e->module, e->key);
	change_window_title(e->window, title);
	g_free(title);
}

void
editor_load_book(EDITOR *e)
{
	gchar *title = NULL, *text = NULL;

	if (!g_ascii_isdigit(e->key[0]))
		return;

	if (atol(e->key) != 0)
		text = main_get_book_raw_text(e->module, e->key);
	else
		text = g_strdup(e->module);

	_load_text_into_buffer(e, text ? text : "");
	if (text)
		g_free(text);

	title = g_strdup_printf("%s", e->module);
	change_window_title(e->window, title);
	g_free(title);
	e->is_changed = FALSE;
}

void
editor_save_book(EDITOR *e)
{
	if (editor_is_dirty(e))
		_save_book(e);
}

void
editor_sync_with_main(void)
{
	GList *tmp = g_list_first(editors_all);

	while (tmp != NULL) {
		EDITOR *e = (EDITOR *)tmp->data;
		if (e->type == NOTE_EDITOR && e->sync)
			editor_load_note(e, NULL, settings.currentverse);
		tmp = g_list_next(tmp);
	}
}

void
editor_maybe_save_all(void)
{
	GList *tmp, *tmp2;

	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		tmp2 = g_list_next(tmp);
		delete_event(NULL, NULL, (EDITOR *)tmp->data);
		tmp = tmp2;
	}
}

static gint
_create_new(const gchar *filename, const gchar *key, gint editor_type)
{
	EDITOR *editor;
	GtkWidget *toolbar_nav = NULL;

	editor = g_new(EDITOR, 1);
	editor->text_widget = NULL;
	editor->sync = FALSE;
	editor->type = editor_type;

	switch (editor_type) {
	case STUDYPAD_EDITOR:
		editor->studypad = TRUE;
		editor->bookeditor = FALSE;
		editor->noteeditor = FALSE;
		editor->module = NULL;
		editor->key = NULL;
		editor->filename = NULL;
		widgets.studypad_dialog = editor_new(_("StudyPad"), editor);
		if (filename) {
			editor->filename = g_strdup(filename);
			_load_file(editor, filename);
		}
		break;

	case NOTE_EDITOR:
		editor->noteeditor = TRUE;
		editor->bookeditor = FALSE;
		editor->studypad = FALSE;
		editor->filename = NULL;
		editor->module = g_strdup(filename);
		editor->key = g_strdup(key);
		editor->navbar.key = NULL;
		editor_new(_("Note Editor"), editor);

		toolbar_nav = gui_navbar_versekey_editor_new(editor);
		gtk_widget_show(toolbar_nav);
		gtk_box_pack_start(GTK_BOX(editor->navbar_box),
				   GTK_WIDGET(toolbar_nav), FALSE, TRUE, 0);

		editor_load_note(editor, NULL, NULL);
		break;

	case BOOK_EDITOR:
		editor->bookeditor = TRUE;
		editor->noteeditor = FALSE;
		editor->studypad = FALSE;
		editor->filename = NULL;
		editor->module = g_strdup(filename);
		editor->key = g_strdup(key);
		editor_new(_("Prayer List/Journal Editor"), editor);

		GtkWidget *box;
		UI_VBOX(box, FALSE, 0);
		gtk_widget_show(box);
		GtkWidget *hpaned1 = UI_HPANE();
		gtk_widget_show(hpaned1);
		gtk_paned_pack2(GTK_PANED(hpaned1), box, TRUE, TRUE);

		GtkWidget *scrollbar = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_show(scrollbar);
		gtk_paned_pack1(GTK_PANED(hpaned1), GTK_WIDGET(scrollbar),
				TRUE, TRUE);
		gtk_scrolled_window_set_policy(
		    GTK_SCROLLED_WINDOW(scrollbar),
		    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_set_shadow_type(
		    (GtkScrolledWindow *)scrollbar, settings.shadow_type);

		editor->treeview = gui_create_editor_tree(editor);
		gtk_widget_show(editor->treeview);
		gtk_container_add(GTK_CONTAINER(scrollbar), editor->treeview);
		gtk_paned_set_position(GTK_PANED(hpaned1), 125);
		gtk_tree_view_expand_all((GtkTreeView *)editor->treeview);

		gtk_container_add(GTK_CONTAINER(editor->box), box);
		gtk_container_add(GTK_CONTAINER(editor->window), hpaned1);

		editor_load_book(editor);
		break;
	}

	editor->is_changed = FALSE;
	editors_all = g_list_append(editors_all, (EDITOR *)editor);
	return 1;
}

gint
editor_create_new(const gchar *filename, const gchar *key,
		  gint editor_type)
{
	GList *tmp = g_list_first(editors_all);

	while (tmp != NULL) {
		EDITOR *e = (EDITOR *)tmp->data;
		switch (editor_type) {
		case STUDYPAD_EDITOR:
			if (e->studypad) {
				if (editor_is_dirty(e))
					_save_file(e);
				if (e->filename)
					g_free(e->filename);
				e->filename = g_strdup(filename);
				gtk_widget_show(e->window);
				gdk_window_raise(
				    gtk_widget_get_parent_window(
					GTK_WIDGET(e->window)));
				_load_file(e, filename);
				return 1;
			}
			break;
		case NOTE_EDITOR:
			if (!e->noteeditor)
				break;
			if (editor_is_dirty(e))
				_save_note(e);
			if (e->module)
				g_free(e->module);
			e->module = g_strdup(filename);
			if (e->key)
				g_free(e->key);
			e->key = g_strdup(key);
			gtk_widget_show(e->window);
			gdk_window_raise(
			    gtk_widget_get_parent_window(GTK_WIDGET(e->window)));
			editor_load_note(e, NULL, NULL);
			return 1;
		case BOOK_EDITOR:
			if (!e->bookeditor)
				break;
			if (editor_is_dirty(e))
				_save_book(e);
			if (e->module)
				g_free(e->module);
			e->module = g_strdup(filename);
			if (e->key)
				g_free(e->key);
			e->key = g_strdup(key);
			gtk_widget_show(e->window);
			gdk_window_raise(
			    gtk_widget_get_parent_window(GTK_WIDGET(e->window)));
			main_load_book_tree_in_editor(
			    GTK_TREE_VIEW(e->treeview), e->module);
			editor_load_book(e);
			return 1;
		}
		tmp = g_list_next(tmp);
	}

	XI_message(("filename %s, key %s",
		    filename ? filename : "-null-",
		    key ? key : "-null-"));
	return _create_new(filename, key, editor_type);
}

G_MODULE_EXPORT void
action_about_activate_cb(GtkWidget *widget, EDITOR *e) {}

G_MODULE_EXPORT void
action_increase_indent_activate_cb(GtkWidget *widget, EDITOR *e) {}

G_MODULE_EXPORT void
action_decrease_indent_activate_cb(GtkWidget *widget, EDITOR *e) {}

G_MODULE_EXPORT void
action_insert_rule_activate_cb(GtkWidget *widget, EDITOR *e) {}

G_MODULE_EXPORT void
action_insert_table_activate_cb(GtkWidget *widget, EDITOR *e) {}

G_MODULE_EXPORT void
action_insert_emoticon_activate_cb(GtkWidget *widget, EDITOR *e) {}

G_MODULE_EXPORT void
action_insert_image_activate_cb(GtkWidget *widget, EDITOR *e)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        _("Select an image file"), GTK_WINDOW(e->window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_OK", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(
            GTK_FILE_CHOOSER(dialog));
        GtkTextBuffer *buffer = _get_buffer(e);
        GtkTextIter cursor;
        gtk_text_buffer_get_iter_at_mark(buffer, &cursor,
            gtk_text_buffer_get_insert(buffer));
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        if (pixbuf) {
            gtk_text_buffer_insert_pixbuf(buffer, &cursor, pixbuf);
            g_object_unref(pixbuf);
            e->is_changed = TRUE;
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

G_MODULE_EXPORT void
action_insert_outline_activate_cb(GtkWidget *widget, EDITOR *e) {}

G_MODULE_EXPORT void
action_justify_left_activate_cb(GtkWidget *widget, EDITOR *e)
{
    gtk_text_view_set_justification(GTK_TEXT_VIEW(e->text_widget),
                                    GTK_JUSTIFY_LEFT);
}

G_MODULE_EXPORT void
action_justify_right_activate_cb(GtkWidget *widget, EDITOR *e)
{
    gtk_text_view_set_justification(GTK_TEXT_VIEW(e->text_widget),
                                    GTK_JUSTIFY_RIGHT);
}

G_MODULE_EXPORT void
action_justify_center_activate_cb(GtkWidget *widget, EDITOR *e)
{
    gtk_text_view_set_justification(GTK_TEXT_VIEW(e->text_widget),
                                    GTK_JUSTIFY_CENTER);
}

G_MODULE_EXPORT void
action_justify_full_activate_cb(GtkWidget *widget, EDITOR *e)
{
    gtk_text_view_set_justification(GTK_TEXT_VIEW(e->text_widget),
                                    GTK_JUSTIFY_FILL);
}

G_MODULE_EXPORT void
action_print_preview_cb(GtkAction *action, EDITOR *e) {}

G_MODULE_EXPORT void
action_font_activate_cb(GtkWidget *widget, EDITOR *e) {}

#endif /* USE_GTKTVeditor */
