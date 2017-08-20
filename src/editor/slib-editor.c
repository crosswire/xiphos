/*
 * Xiphos Bible Study Tool
 * html-editor.c - the html editor
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

#include <config.h>

#ifndef USE_WEBKIT_EDITOR

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>

#include <libintl.h>
#include <locale.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <editor/gtkhtml-editor.h>
#include <gtkhtml/gtkhtml-stream.h>

#include "editor/slib-editor.h"
#include "editor/link_dialog.h"

#include "main/settings.h"
#include "main/sword_treekey.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"

#include "gui/navbar_versekey_editor.h"
#include "gui/dialog.h"
#include "gui/widgets.h"
#include "gui/xiphos.h"
#include "gui/treekey-editor.h"
#include "gui/utilities.h"

#include "gui/debug_glib_null.h"

extern gboolean do_display;

static gboolean editor_is_dirty(EDITOR *e);

static int app_delete_cb(GtkWidget *widget, GdkEvent *event, gpointer data);

static void _load_file(EDITOR *e, const gchar *filename);

static void _save_file(EDITOR *e);

static void _save_note(EDITOR *e);

static void _save_book(EDITOR *e);

static gint ask_about_saving(EDITOR *e);

static GList *editors_all = NULL;

static void handle_error(GError **error)
{
	if (*error != NULL) {
		g_warning("%s", (*error)->message);
		g_clear_error(error);
	}
}

static void do_exit(EDITOR *e)
{
	if (e->filename) {
		g_free(e->filename);
	}
	if (e->module) {
		g_free(e->module);
	}
	if (e->key) {
		g_free(e->key);
	}
	if (e->window)
		gtk_widget_destroy(e->window);
	g_free(e);
}

static void change_window_title(GtkWidget *window, const gchar *window_title)
{
	gtk_window_set_title(GTK_WINDOW(window), window_title);
}

#if 0
static gint
_calc_header_height(GtkHTML * html, GtkPrintOperation * operation,
		    GtkPrintContext * context)
{
	PangoContext *pango_context;
	PangoFontDescription *desc;
	PangoFontMetrics *metrics;
	gint header_height;

	pango_context = gtk_print_context_create_pango_context(context);
	desc = pango_font_description_from_string("Sans Regular 10");

	metrics =
	    pango_context_get_metrics(pango_context, desc,
				      pango_language_get_default());
	header_height =
	    pango_font_metrics_get_ascent(metrics) +
	    pango_font_metrics_get_descent(metrics);
	pango_font_metrics_unref(metrics);

	pango_font_description_free(desc);
	g_object_unref(pango_context);

	return header_height;
}

static gint
_calc_footer_height(GtkHTML * html, GtkPrintOperation * operation,
		    GtkPrintContext * context)
{
	PangoContext *pango_context;
	PangoFontDescription *desc;
	PangoFontMetrics *metrics;
	gint footer_height;

	pango_context = gtk_print_context_create_pango_context(context);
	desc = pango_font_description_from_string("Sans Regular 10");

	metrics =
	    pango_context_get_metrics(pango_context, desc,
				      pango_language_get_default());
	footer_height =
	    pango_font_metrics_get_ascent(metrics) +
	    pango_font_metrics_get_descent(metrics);
	pango_font_metrics_unref(metrics);

	pango_font_description_free(desc);
	g_object_unref(pango_context);

	return footer_height;
}

static void
_draw_header(GtkHTML * html, GtkPrintOperation * operation,
	     GtkPrintContext * context,
	     gint page_nr, PangoRectangle * rec, EDITOR * e)
{
	PangoFontDescription *desc;
	PangoLayout *layout;
	gdouble x, y;
	gchar *text;
	cairo_t *cr;

	text = g_strdup(e->filename);

	desc = pango_font_description_from_string("Sans Regular 10");
	layout = gtk_print_context_create_pango_layout(context);
	pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
	pango_layout_set_font_description(layout, desc);
	pango_layout_set_text(layout, text, -1);
	pango_layout_set_width(layout, rec->width);

	x = pango_units_to_double(rec->x);
	y = pango_units_to_double(rec->y);

	cr = gtk_print_context_get_cairo_context(context);

	cairo_save(cr);
	cairo_set_source_rgb(cr, .0, .0, .0);
	cairo_move_to(cr, x, y);
	pango_cairo_show_layout(cr, layout);
	cairo_restore(cr);

	g_object_unref(layout);
	pango_font_description_free(desc);

	g_free(text);
}


static void
_draw_footer(GtkHTML * html, GtkPrintOperation * operation,
	     GtkPrintContext * context,
	     gint page_nr, PangoRectangle * rec, EDITOR * e)
{
	PangoFontDescription *desc;
	PangoLayout *layout;
	gdouble x, y;
	gint n_pages;
	gchar *text;
	cairo_t *cr;

	g_object_get(operation, "n-pages", &n_pages, NULL);
	text = g_strdup_printf(_("Page %d of %d"), page_nr + 1, n_pages);

	desc = pango_font_description_from_string("Sans Regular 10");
	layout = gtk_print_context_create_pango_layout(context);
	pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
	pango_layout_set_font_description(layout, desc);
	pango_layout_set_text(layout, text, -1);
	pango_layout_set_width(layout, rec->width);

	x = pango_units_to_double(rec->x);
	y = pango_units_to_double(rec->y);

	cr = gtk_print_context_get_cairo_context(context);

	cairo_save(cr);
	cairo_set_source_rgb(cr, .0, .0, .0);
	cairo_move_to(cr, x, y);
	pango_cairo_show_layout(cr, layout);
	cairo_restore(cr);

	g_object_unref(layout);
	pango_font_description_free(desc);

	g_free(text);
}

static GtkPrintOperationResult
_do_print(EDITOR * e, GtkPrintOperationAction action)
{
	GtkPrintOperation *operation;
	GtkPrintSettings *psettings;
	GtkPageSetup *setup;
	GtkPrintOperationResult result;
	GError *error = NULL;

	operation = gtk_print_operation_new();
	psettings = gtk_print_settings_new();

	psettings = gtk_print_operation_get_print_settings(operation);

	setup = gtk_page_setup_new();
	gtk_page_setup_set_top_margin(setup, 30, GTK_UNIT_PIXEL);
	gtk_page_setup_set_left_margin(setup, 50, GTK_UNIT_PIXEL);

#ifdef WIN32
	gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS);
#endif
	gtk_print_operation_set_default_page_setup(operation, setup);

	result = gtk_html_print_operation_run(GTK_HTML(e->html_widget), operation, action, GTK_WINDOW(e->window), (GtkHTMLPrintCalcHeight) _calc_header_height,	/* GtkHTMLPrintCalcHeight  calc_header_height */
					      (GtkHTMLPrintCalcHeight) _calc_footer_height,	/* GtkHTMLPrintCalcHeight  calc_footer_height */
					      (GtkHTMLPrintDrawFunc) _draw_header,	/* GtkHTMLPrintDrawFunc draw_header */
					      (GtkHTMLPrintDrawFunc) _draw_footer,	/* GtkHTMLPrintDrawFunc draw_footer */
					      e,	/* gpointer user_data */
					      &error);

	g_object_unref(operation);
	handle_error(&error);

	return result;
}
#endif /* 0 */

static const gchar *file_ui =
    "<ui>\n"
    "  <menubar name='main-menu'>\n"
    "    <menu action='file-menu'>\n"
    "     <menuitem action='open'/>\n"
    "     <menuitem action='save'/>\n"
    "     <menuitem action='save-as'/>\n"
    "     <menuitem action='new'/>\n"
    "     <separator/>\n"
    "     <menuitem action='print-preview'/>\n"
    "     <menuitem action='print'/>\n"
    "     <separator/>\n"
    "     <menuitem action='quit'/>\n"
    "    </menu>\n"
    "  </menubar>\n"
    "</ui>";

static const gchar *note_file_ui =
    "<ui>\n"
    "  <menubar name='main-menu'>\n"
    "    <menu action='file-menu'>\n"
    "     <menuitem action='save'/>\n"
    "     <menuitem action='save-as'/>\n"
    "     <menuitem action='delete'/>\n"
    "     <separator/>\n"
    "     <menuitem action='print-preview'/>\n"
    "     <menuitem action='print'/>\n"
    "     <separator/>\n"
    "     <menuitem action='quit'/>\n"
    "    </menu>\n"
    "  </menubar>\n"
    "</ui>";

static const gchar *view_ui =
    "<ui>\n"
    "  <menubar name='main-menu'>\n"
    "    <menu action='view-menu'>\n"
    "     <menuitem action='view-html-output'/>\n"
    "     <menuitem action='view-html-source'/>\n"
    "     <menuitem action='view-plain-source'/>\n"
    "    </menu>\n"
    "  </menubar>\n"
    "</ui>";

static const gchar *main_ui_note =
    "<ui>\n"
    "  <toolbar name='main-toolbar'> \n"
    "    <placeholder name='pre-main-toolbar'> \n"
    "      <toolitem action='save'/> \n"
    "      <toolitem action='delete'/> \n"
    "      <separator/> \n"
    "    </placeholder> \n"
    "  </toolbar> \n"
    "</ui>";

static const gchar *main_ui_studypad =
    "<ui>\n"
    "  <toolbar name='main-toolbar'> \n"
    "    <placeholder name='pre-main-toolbar'> \n"
    "      <toolitem action='save'/> \n"
    "      <toolitem action='new'/> \n"
    "      <separator/> \n"
    "    </placeholder> \n"
    "  </toolbar> \n"
    "</ui>";

static GtkPrintOperationResult
print(GtkhtmlEditor *editor, GtkPrintOperationAction action)
{
	GtkPrintOperation *operation;
	GtkPrintOperationResult result;
	GError *error = NULL;

	operation = gtk_print_operation_new();

	result =
	    gtk_html_print_operation_run(gtkhtml_editor_get_html(editor),
					 operation, action,
					 GTK_WINDOW(editor), NULL, NULL,
					 NULL, NULL, NULL, &error);

	g_object_unref(operation);
	handle_error(&error);

	return result;
}

static gint open_dialog(EDITOR *e)
{
	GtkWidget *dialog;
	gint response;
#if 0
	const gchar *filename;
#endif

	dialog =
	    gtk_file_chooser_dialog_new(_("Open"), GTK_WINDOW(e->window),
					GTK_FILE_CHOOSER_ACTION_OPEN,
#if GTK_CHECK_VERSION(3, 10, 0)
					"_Cancel", GTK_RESPONSE_CANCEL,
					"_Open", GTK_RESPONSE_ACCEPT,
#else
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN,
					GTK_RESPONSE_ACCEPT,
#endif
					NULL);

/*gtk_file_chooser_set_do_overwrite_confirmation (
	   GTK_FILE_CHOOSER (dialog), TRUE); */

#if 0
	filename = gtkhtml_editor_get_filename(GTKHTML_EDITOR(e->window));
#endif

	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),
					    settings.studypaddir);

	response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == GTK_RESPONSE_ACCEPT) {
		gchar *new_filename;

		new_filename =
		    gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		//gtkhtml_editor_set_filename (e->window, new_filename);
		_load_file(e, new_filename);
		g_free(new_filename);
	}

	gtk_widget_destroy(dialog);

	return response;
}

static gint save_dialog(GtkhtmlEditor *editor, EDITOR *e)
{
	GtkWidget *dialog;
	const gchar *filename;
	gint response;

	dialog =
	    gtk_file_chooser_dialog_new(_("Save As"), GTK_WINDOW(editor),
					GTK_FILE_CHOOSER_ACTION_SAVE,
#if GTK_CHECK_VERSION(3, 10, 0)
					"_Cancel", GTK_RESPONSE_CANCEL,
					"_Open", GTK_RESPONSE_ACCEPT,
#else
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE,
					GTK_RESPONSE_ACCEPT,
#endif
					NULL);

	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

	filename = gtkhtml_editor_get_filename(editor);

	if (filename != NULL)
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog),
					      filename);
	else {
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),
						    settings.studypaddir);
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),
						  _("Untitled document"));
	}

	response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == GTK_RESPONSE_ACCEPT) {
		gchar *new_filename;

		new_filename =
		    gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		gtkhtml_editor_set_filename(editor, new_filename);

		if (e->filename)
			g_free(e->filename);
		e->filename = g_strdup(new_filename);

		xml_set_value("Xiphos", "studypad", "lastfile",
			      e->filename);
		settings.studypadfilename =
		    xml_get_value("studypad", "lastfile");

		change_window_title(e->window, e->filename);

		g_free(new_filename);
	}

	gtk_widget_destroy(dialog);

	return response;
}

/* Helper for view_source_dialog() */
static gboolean
view_source_dialog_receiver(HTMLEngine *engine,
			    const gchar *data,
			    guint length, GString *string)
{
	g_string_append_len(string, data, length);

	return TRUE;
}

static void
view_source_dialog(GtkhtmlEditor *editor,
		   const gchar *title,
		   const gchar *content_type, gboolean show_output)
{
	GtkWidget *dialog;
	GtkWidget *content;
	GtkWidget *scrolled_window;
	GString *string;

	dialog = gtk_dialog_new_with_buttons(title, GTK_WINDOW(editor),
					     GTK_DIALOG_DESTROY_WITH_PARENT,
#if GTK_CHECK_VERSION(3, 10, 0)
					     "_Close", GTK_RESPONSE_CLOSE,
#else
					     GTK_STOCK_CLOSE,
					     GTK_RESPONSE_CLOSE,
#endif
					     NULL);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window),
					    GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), scrolled_window, TRUE,
			   TRUE, 0);

	gtk_container_set_border_width(GTK_CONTAINER(dialog), 6);
	gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 6);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 300);

	string = g_string_sized_new(4096);

	gtk_html_export(gtkhtml_editor_get_html(editor),
			content_type, (GtkHTMLSaveReceiverFn)
			view_source_dialog_receiver,
			string);

	if (show_output) {
		GtkHTMLStream *stream;

		content = gtk_html_new();
		stream = gtk_html_begin(GTK_HTML(content));
		gtk_html_stream_write(stream, string->str, string->len);
		gtk_html_stream_close(stream, GTK_HTML_STREAM_OK);
	} else {
		GtkTextBuffer *buffer;

		content = gtk_text_view_new();
		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(content));
		gtk_text_buffer_set_text(buffer, string->str, string->len);
		gtk_text_view_set_editable(GTK_TEXT_VIEW(content), FALSE);
	}

	g_string_free(string, TRUE);

	gtk_container_add(GTK_CONTAINER(scrolled_window), content);
	gtk_widget_show_all(scrolled_window);

	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static void action_print_cb(GtkAction *action, EDITOR *e)
{
	print(GTKHTML_EDITOR(e->window),
	      GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG);
}

static void action_print_preview_cb(GtkAction *action, EDITOR *e)
{
	print(GTKHTML_EDITOR(e->window),
	      GTK_PRINT_OPERATION_ACTION_PREVIEW);
}

static void action_quit_cb(GtkAction *action, EDITOR *e)
{
	app_delete_cb(NULL, NULL, e);
}

static void action_open_cb(GtkAction *action, EDITOR *e)
{
	open_dialog(e);
}

static void action_save_cb(GtkAction *action, EDITOR *e)
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

static void action_new_cb(GtkAction *action, EDITOR *e)
{ /* for studypad only */

	if (editor_is_dirty(e))
		_save_file(e);

	_load_file(e,
		   g_strdup_printf("%s/%s", settings.gSwordDir,
				   "template.pad"));

	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(_("Untitled document"));

	xml_set_value("Xiphos", "studypad", "lastfile", e->filename);
	settings.studypadfilename = xml_get_value("studypad", "lastfile");
	change_window_title(e->window, e->filename);

	gtkhtml_editor_set_filename(GTKHTML_EDITOR(e->window), NULL);
	gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window), TRUE);
}

static void action_delete_cb(GtkAction *action, EDITOR *e)
{ /* for note only */

	gchar *buf;

	if (e->studypad)
		return;

	buf = g_strdup_printf("<span weight=\"bold\" size=\"larger\">%s %s?</span>",
			      _("Are you sure you want to delete the note for"), e->key);

	if (gui_yes_no_dialog(buf,
#if GTK_CHECK_VERSION(3, 10, 0)
			      "dialog-warning"
#else
			      GTK_STOCK_DIALOG_WARNING
#endif
			      )) {
		main_delete_note(e->module, e->key);
		gtkhtml_editor_set_text_html(GTKHTML_EDITOR(e->window),
					     "", strlen(""));
	}
	g_free(buf);
}

static void action_insert_link_cb(GtkAction *action, EDITOR *e)
{
	editor_link_dialog(e);
}

static void action_save_as_cb(GtkAction *action, EDITOR *e)
{
	const gchar *filename;
	gboolean as_html;
	GError *error = NULL;

	if (save_dialog(GTKHTML_EDITOR(e->window), e) ==
	    GTK_RESPONSE_CANCEL)
		return;

	filename = gtkhtml_editor_get_filename(GTKHTML_EDITOR(e->window));
	as_html = gtkhtml_editor_get_html_mode(GTKHTML_EDITOR(e->window));

	gtkhtml_editor_save(GTKHTML_EDITOR(e->window), filename, as_html,
			    &error);
	handle_error(&error);
}

static void
action_view_html_output(GtkAction *action, GtkhtmlEditor *editor)
{
	view_source_dialog(editor, _("HTML Output"), "text/html", TRUE);
}

static void
action_view_html_source(GtkAction *action, GtkhtmlEditor *editor)
{
	view_source_dialog(editor, _("HTML Source"), "text/html", FALSE);
}

static void
action_view_plain_source(GtkAction *action, GtkhtmlEditor *editor)
{
	view_source_dialog(editor, _("Plain Source"), "text/plain", FALSE);
}

static GtkActionEntry file_entries[] = {

    {"print",
#if GTK_CHECK_VERSION(3, 10, 0)
     "_Print...",
#else
     GTK_STOCK_PRINT,
#endif
     N_("_Print..."),
     NULL,
     NULL,
     G_CALLBACK(action_print_cb)},

    {"print-preview",
#if GTK_CHECK_VERSION(3, 10, 0)
     "Print Pre_view...",
#else
     GTK_STOCK_PRINT_PREVIEW,
#endif
     N_("Print Pre_view"),
     NULL,
     NULL,
     G_CALLBACK(action_print_preview_cb)},

    {"quit",
#if GTK_CHECK_VERSION(3, 10, 0)
     "_Quit...",
#else
     GTK_STOCK_QUIT,
#endif
     N_("_Quit"),
     NULL,
     NULL,
     G_CALLBACK(action_quit_cb)},

    {"open",
#if GTK_CHECK_VERSION(3, 10, 0)
     "_Open",
#else
     GTK_STOCK_OPEN,
#endif
     N_("_Open"),
     NULL,
     NULL,
     G_CALLBACK(action_open_cb)},

    {"save",
#if GTK_CHECK_VERSION(3, 10, 0)
     "_Save",
#else
     GTK_STOCK_SAVE,
#endif
     N_("_Save"),
     NULL,
     NULL,
     G_CALLBACK(action_save_cb)},

    {"save-as",
#if GTK_CHECK_VERSION(3, 10, 0)
     "Save _As...",
#else
     GTK_STOCK_SAVE_AS,
#endif
     N_("Save _As..."),
     NULL,
     NULL,
     G_CALLBACK(action_save_as_cb)},
    {"new",
     "gtk-new",
     N_("New"),
     NULL,
     N_("Open new document"),
     G_CALLBACK(action_new_cb)},
    {"delete",
     "gtk-delete",
     N_("Delete"),
     NULL,
     N_("Delete current note"),
     G_CALLBACK(action_delete_cb)},

    {"file-menu",
     NULL,
     N_("_File"),
     NULL,
     NULL,
     NULL}};

/*
static GtkActionEntry note_file_entries[] = {

	{ "print",
	  GTK_STOCK_PRINT,
	  N_("_Print..."),
	  NULL,
	  NULL,
	  G_CALLBACK (action_print_cb) },

	{ "print-preview",
	  GTK_STOCK_PRINT_PREVIEW,
	  N_("Print Pre_view"),
	  NULL,
	  NULL,
	  G_CALLBACK (action_print_preview_cb) },

	{ "quit",
	  GTK_STOCK_QUIT,
	  N_("_Quit"),
	  NULL,
	  NULL,
	  G_CALLBACK (action_quit_cb) },

	{ "save",
	  GTK_STOCK_SAVE,
	  N_("_Save"),
	  NULL,
	  NULL,
	  G_CALLBACK (action_save_cb) },

	{ "save-as",
	  GTK_STOCK_SAVE_AS,
	  N_("Save _As..."),
	  NULL,
	  NULL,
	  G_CALLBACK (action_save_as_cb) },

	{ "file-menu",
	  NULL,
	  N_("_File"),
	  NULL,
	  NULL,
	  NULL }
};
*/
static GtkActionEntry view_entries[] = {

    {"view-html-output",
     NULL,
     N_("HTML _Output"),
     NULL,
     NULL,
     G_CALLBACK(action_view_html_output)},

    {"view-html-source",
     NULL,
     N_("_HTML Source"),
     NULL,
     NULL,
     G_CALLBACK(action_view_html_source)},

    {"view-plain-source",
     NULL,
     N_("_Plain Source"),
     NULL,
     NULL,
     G_CALLBACK(action_view_plain_source)},

    {"view-menu",
     NULL,
     N_("_View"),
     NULL,
     NULL,
     NULL}};

static GtkActionEntry main_entries[] = {

    {"save",
     "gtk-save",
     N_("Save"),
     NULL,
     NULL,
     G_CALLBACK(action_save_cb)}};

static GtkActionEntry test_entries[] = {

    {"context-insert-link",
     "insert-link",
     N_("Insert Link"),
     NULL,
     NULL,
     G_CALLBACK(action_insert_link_cb)},

    {"insert-link",
     "insert-link",
     N_("Insert Link"),
     NULL,
     NULL,
     G_CALLBACK(action_insert_link_cb)}};

GtkWidget *editor_new(const gchar *title, EDITOR *e)
{
	GtkActionGroup *action_group;
	GtkUIManager *manager;
	GtkWidget *editor;
	GError *error = NULL;

	editor = gtkhtml_editor_new();
	e->window = editor;
	e->html_widget =
	    GTK_WIDGET(gtkhtml_editor_get_html(GTKHTML_EDITOR(editor)));
	gtk_window_set_title(GTK_WINDOW(editor), title);

	set_window_icon(GTK_WINDOW(editor));

	manager = gtkhtml_editor_get_ui_manager(GTKHTML_EDITOR(editor));
	if (e->type == STUDYPAD_EDITOR)
		gtk_ui_manager_add_ui_from_string(manager, file_ui, -1,
						  &error);
	else
		gtk_ui_manager_add_ui_from_string(manager, note_file_ui,
						  -1, &error);

	handle_error(&error);

	gtk_ui_manager_add_ui_from_string(manager, view_ui, -1, &error);
	handle_error(&error);

	if (e->type == STUDYPAD_EDITOR)
		gtk_ui_manager_add_ui_from_string(manager,
						  main_ui_studypad, -1,
						  &error);
	else
		gtk_ui_manager_add_ui_from_string(manager, main_ui_note,
						  -1, &error);

	handle_error(&error);

	action_group = gtk_action_group_new("file");
	gtk_action_group_set_translation_domain(action_group,
						GETTEXT_PACKAGE);
	gtk_action_group_add_actions(action_group, file_entries,
				     G_N_ELEMENTS(file_entries), e);
	gtk_ui_manager_insert_action_group(manager, action_group, 0);

	action_group = gtk_action_group_new("view");
	gtk_action_group_set_translation_domain(action_group,
						GETTEXT_PACKAGE);
	gtk_action_group_add_actions(action_group, view_entries,
				     G_N_ELEMENTS(view_entries), editor);
	gtk_ui_manager_insert_action_group(manager, action_group, 0);

	action_group = gtk_action_group_new("main");
	gtk_action_group_set_translation_domain(action_group,
						GETTEXT_PACKAGE);
	gtk_action_group_add_actions(action_group, main_entries,
				     G_N_ELEMENTS(main_entries), e);
	gtk_ui_manager_insert_action_group(manager, action_group, 0);

	action_group = gtk_action_group_new("context-menu");
	gtk_action_group_set_translation_domain(action_group,
						GETTEXT_PACKAGE);
	gtk_action_group_add_actions(action_group, test_entries,
				     G_N_ELEMENTS(test_entries), e);
	gtk_ui_manager_insert_action_group(manager, action_group, 0);

	gtk_ui_manager_ensure_update(manager);
	gtk_widget_show(editor);

	gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
	gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window), FALSE);

	g_signal_connect(editor, "delete-event",
			 G_CALLBACK(app_delete_cb), (EDITOR *)e);
	return editor;
}

/* Helper for _save_note() and _save_book()*/
static gboolean
_save_receiver(HTMLEngine *engine,
	       const gchar *data, guint length, GString *string)
{
	g_string_append_len(string, data, length);

	return TRUE;
}

static void _save_note(EDITOR *e)
{
	GString *string;

	string = g_string_sized_new(4096);

	gtk_html_export(gtkhtml_editor_get_html(GTKHTML_EDITOR(e->window)),
			"text/html",
			(GtkHTMLSaveReceiverFn)_save_receiver, string);
	XI_message(("\n_save_note: %s\n", string->str));
	main_save_note(e->module, e->key, string->str);

	g_string_free(string, TRUE);
	gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
	gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window), FALSE);
}

static void _save_book(EDITOR *e)
{
	GString *string;

	string = g_string_sized_new(4096);

	gtk_html_export(gtkhtml_editor_get_html(GTKHTML_EDITOR(e->window)),
			"text/html",
			(GtkHTMLSaveReceiverFn)_save_receiver, string);

	main_treekey_save_book_text(e->module, e->key, string->str);
	g_string_free(string, TRUE);
	gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
	gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window), FALSE);
}

static void _save_file(EDITOR *e)
{
	const gchar *filename;
	gboolean as_html;
	GError *error = NULL;

	if (gtkhtml_editor_get_filename(GTKHTML_EDITOR(e->window)) == NULL)
		if (save_dialog(GTKHTML_EDITOR(e->window), e) ==
		    GTK_RESPONSE_CANCEL)
			return;

	filename = gtkhtml_editor_get_filename(GTKHTML_EDITOR(e->window));
	as_html = gtkhtml_editor_get_html_mode(GTKHTML_EDITOR(e->window));

	XI_message(("\n_save_file filename: %s\n", filename));

	gtkhtml_editor_save(GTKHTML_EDITOR(e->window), filename, as_html,
			    &error);
	handle_error(&error);

	gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
	gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window), FALSE);
}

static void _load_file(EDITOR *e, const gchar *filename)
{
	//GtkWidget *choser;
	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(filename);

	XI_message(("_load_file filename: %s", filename));

	xml_set_value("Xiphos", "studypad", "lastfile", e->filename);
	settings.studypadfilename = xml_get_value("studypad", "lastfile");
	change_window_title(e->window, e->filename);

	char *contents;
	char *etag_out;
	gsize length;
	GFile *file;
	file = g_file_new_for_path(filename);
	gtkhtml_editor_set_filename(GTKHTML_EDITOR(e->window),
				    e->filename);
	if (g_file_load_contents(file, NULL, &contents, &length, &etag_out, NULL))
		gtkhtml_editor_set_text_html(GTKHTML_EDITOR(e->window),
					     contents, length);
	gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
	gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window), FALSE);
	return;
}

gboolean editor_is_dirty(EDITOR *e)
{
	return gtkhtml_editor_get_changed(GTKHTML_EDITOR(e->window));
}

void editor_save_book(EDITOR *e)
{
	if (editor_is_dirty(e)) {
		_save_book(e);
	}
}

/* save if needed is done in treeky-editor.c before calling editor_load_book() */
void editor_load_book(EDITOR *e)
{
	gchar *title = NULL;
	gchar *text = NULL;

	if (!g_ascii_isdigit(e->key[0]))
		return; /* make sure is a number (offset) */

	title = g_strdup_printf("%s", e->module);
	XI_message(("book: %s\noffset :%s", e->module, e->key));

	if (atol(e->key) != 0)
		text = main_get_book_raw_text(e->module, e->key);
	else
		text = g_strdup(e->module);

	if (strlen(text)) {
		gtkhtml_editor_set_text_html(GTKHTML_EDITOR(e->window),
					     text, strlen(text));
		gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
		gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window),
					   FALSE);
	} else {
		gtkhtml_editor_set_text_html(GTKHTML_EDITOR(e->window),
					     "", strlen(""));
		gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
		gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window),
					   FALSE);
	}

	change_window_title(e->window, title);

	if (text)
		g_free(text);
	if (title)
		g_free(title);
}

/******************************************************************************
 * Name
 *   sync_toggled
 *
 * Synopsis
 *   #include "/.h"
 *
 *   void sync_toggled(GtkToggleButton * button, DIALOG_DATA * vc)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void editor_sync_with_main(void)
{
	GList *tmp = NULL;

	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		EDITOR *e = (EDITOR *)tmp->data;

		switch (e->type) {
		case STUDYPAD_EDITOR:
		case BOOK_EDITOR:
			break;
		case NOTE_EDITOR:
			if (e->sync)
				editor_load_note(e, NULL,
						 settings.currentverse);
			break;
		}
		tmp = g_list_next(tmp);
	}
}

void
editor_load_note(EDITOR *e, const gchar *module_name, const gchar *key)
{
	gchar *title;
	gchar *text;

	if (editor_is_dirty(e))
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

	title = g_strdup_printf("%s - %s", e->module, e->key);
	text = main_get_raw_text((gchar *)e->module, (gchar *)e->key);
	if (strlen(text)) {
		gtkhtml_editor_set_text_html(GTKHTML_EDITOR(e->window),
					     text, strlen(text));
		gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
		gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window),
					   FALSE);

	} else {
		gtkhtml_editor_set_text_html(GTKHTML_EDITOR(e->window),
					     "", strlen(""));
		gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
		gtkhtml_editor_set_changed(GTKHTML_EDITOR(e->window),
					   FALSE);
	}

	change_window_title(e->window, title);
	if (e->type == NOTE_EDITOR)
		main_navbar_versekey_set(e->navbar, e->key);

	if (text)
		g_free(text);
	if (title)
		g_free(title);
}

static int
app_delete_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	if (editor_is_dirty((EDITOR *)data)) {
		switch (ask_about_saving((EDITOR *)data)) {
		case GS_YES: /* exit saving */

			break;
		case GS_NO: /* exit without saving */

			break;
		case GS_CANCEL:
			return TRUE;
			break;
		}
	}
	editors_all = g_list_remove(editors_all, (EDITOR *)data);
	do_exit((EDITOR *)data);
	return FALSE;
}

static gint ask_about_saving(EDITOR *e)
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
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;

		buf = g_strdup_printf("%s: %s", e->module, e->key);
		buf1 = _("Save the changes to document");
		buf2 = _("before closing?");
		buf3 =
		    g_strdup_printf("<span weight=\"bold\" size=\"larger\">%s %s %s</span>",
				    buf1, buf, buf2);
		info->label_top = buf3;
		info->label2 = _("If you don't save, changes will be permanently lost.");
		info->save = TRUE;
		info->cancel = TRUE;
		info->no_save = TRUE;

		test = gui_alert_dialog(info);
		retval = test;

		if (test == GS_YES) {
			if (e->type == NOTE_EDITOR) {
				/* save notes and prayer lists */
				_save_note(e);

			} else {
				/* save notes and prayer lists */
				_save_book(e);
			}
		}
		g_free(info);
		g_free(buf);
		g_free(buf3);
		break;

	case STUDYPAD_EDITOR:
		info = gui_new_dialog();
		info->stock_icon =
#if GTK_CHECK_VERSION(3, 10, 0)
		    "dialog-warning";
#else
		    GTK_STOCK_DIALOG_WARNING;
#endif
		if (settings.studypadfilename)
			buf = settings.studypadfilename;
		else
			buf = N_("File");
		buf1 = _("Save the changes to document");
		buf2 = _("before closing?");
		buf3 =
		    g_strdup_printf("<span weight=\"bold\" size=\"larger\">%s %s %s</span>",
				    buf1, buf, buf2);
		info->label_top = buf3;
		info->label2 = _("If you don't save, changes will be permanently lost.");
		info->save = TRUE;
		info->cancel = TRUE;
		info->no_save = TRUE;

		test = gui_alert_dialog(info);
		retval = test;
		if (test == GS_YES) {
			if (e->filename)
				gtkhtml_editor_save(GTKHTML_EDITOR(e->window),
						    e->filename, TRUE,
						    NULL);
			/*else
			   open_or_save_as_dialog(e,
			   OP_SAVE_THROUGH_PERSIST_FILE); */
		}
		g_free(info);
		g_free(buf3);
		break;
	}
	sync_windows();
	return retval;
}

static gint _create_new(const gchar *filename, const gchar *key,
			gint editor_type)
{
	EDITOR *editor;
	GtkWidget *vbox = NULL;
	GtkWidget *toolbar_nav = NULL;

	editor = g_new(EDITOR, 1);
	editor->html_widget = NULL;
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
		widgets.studypad_dialog =
		    editor_new(_("StudyPad"), editor);

		if (filename) {
			editor->filename = g_strdup(filename);
			_load_file(editor, g_strdup(filename));
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
		vbox = GTKHTML_EDITOR(editor->window)->vbox;
		toolbar_nav = gui_navbar_versekey_editor_new(editor);
		gtk_widget_show(toolbar_nav);
		gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(toolbar_nav),
				   FALSE, TRUE, 0);
		gtk_box_reorder_child(GTK_BOX(vbox),
				      GTK_WIDGET(toolbar_nav), 1);

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
		UI_VBOX(box, TRUE, 0);
		gtk_widget_show(box);
		GtkWidget *hpaned1 = UI_HPANE();
		gtk_widget_show(hpaned1);
		gtk_paned_pack2(GTK_PANED(hpaned1), box, TRUE, TRUE);

		GtkWidget *scrollbar = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_show(scrollbar);
		gtk_paned_pack1(GTK_PANED(hpaned1), GTK_WIDGET(scrollbar),
				TRUE, TRUE);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar),
					       GTK_POLICY_AUTOMATIC,
					       GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
						    scrollbar,
						    settings.shadow_type);

		editor->treeview = gui_create_editor_tree(editor);
		gtk_widget_show(editor->treeview);
		gtk_container_add(GTK_CONTAINER(scrollbar),
				  editor->treeview);
		gtk_paned_set_position(GTK_PANED(hpaned1), 125);
		gtk_tree_view_collapse_all((GtkTreeView *)
					   editor->treeview);
		// then we should expand on the item to which we've opened for edit.
		vbox = GTKHTML_EDITOR(editor->window)->vbox;

		gtk_widget_reparent(vbox, box);

		gtk_container_add(GTK_CONTAINER(editor->window), hpaned1);

		editor_load_book(editor);

		break;
	}
	editor->is_changed = FALSE;
	editors_all = g_list_append(editors_all, (EDITOR *)editor);
	return 1;
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
	GList *tmp = NULL;

	tmp = g_list_first(editors_all);
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
				gdk_window_raise(gtk_widget_get_parent_window(GTK_WIDGET(e->window)));

				_load_file(e, g_strdup(filename));
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
			gdk_window_raise(gtk_widget_get_parent_window(GTK_WIDGET(e->window)));

			editor_load_note(e, NULL, NULL);

			return 1;
			break;
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
			gdk_window_raise(gtk_widget_get_parent_window(GTK_WIDGET(e->window)));
			main_load_book_tree_in_editor(GTK_TREE_VIEW(e->treeview),
						      e->module);
			editor_load_book(e);

			return 1;
			break;
		}
		tmp = g_list_next(tmp);
	}
	XI_message(("filename %s, key %s",
		    (filename ? filename : "-null-"),
		    (key ? key : "-null-")));
	return _create_new(filename, key, editor_type);
}

void editor_maybe_save_all(void)
{
	GList *tmp, *tmp2;

	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		/* 2ndary list chaser:
		   elements will be removed by the saver. */
		tmp2 = g_list_next(tmp);

		app_delete_cb(NULL, NULL, (EDITOR *)tmp->data);
		tmp = tmp2;
	}
}

#endif /* !USE_WEBKIT_EDITOR */
