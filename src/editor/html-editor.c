/*
 * GnomeSword Bible Study Tool
 * html-editor.c - the html editor
 *
 * Copyright (C) 2005 GnomeSword Developer Team
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

#include <config.h>
#include <libgnome/gnome-i18n.h>

#include <gnome.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>

#ifdef USE_GTKHTML3_14_23
#include <libintl.h>
#include <locale.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <editor/gtkhtml-editor.h>
#include <gtkhtml/gtkhtml-stream.h>	
#else
#include <bonobo.h>
#include <Editor.h>

#endif
#include "editor/html-editor.h"

#ifdef OLD_NAVBAR
#include "main/navbar.h"
#else
#include "gui/navbar_versekey_editor.h"
#endif

#include "main/settings.h"
#include "main/sword.h"
#include "main/sword_treekey.h"
#include "main/xml.h"

#include "gui/dialog.h"
#include "gui/widgets.h"
#include "gui/gnomesword.h"
#include "gui/treekey-editor.h"
#include "gui/utilities.h"


#ifdef USE_GTKHTML3_14_23
	
#else
#define CONTROL_ID         "OAFIID:GNOME_GtkHTML_Editor:" GTKHTML_API_VERSION
#endif
extern gboolean do_display;

static 
gboolean save_through_persist_file(EDITOR * e, const gchar * filename);

static 
int app_delete_cb(GtkWidget * widget, GdkEvent * event, gpointer data);

static 
void _load_file(EDITOR * e, const gchar * filename);

static
gint ask_about_saving(EDITOR * e);


static GList *editors_all = NULL;
static gint formatHTML = 1;
static GtkWidget *win;
//static GtkHTML *html;

 

static
void do_exit(EDITOR * e)
{
#ifdef USE_GTKHTML3_14_23
	
#else
	CORBA_Environment ev;

	CORBA_exception_init(&ev);
	if (e->engine != CORBA_OBJECT_NIL) {
		Bonobo_Unknown_unref(e->engine, &ev);
		CORBA_Object_release(e->engine, &ev);
		e->engine = CORBA_OBJECT_NIL;
	}

	if (e->persist_stream_interface != CORBA_OBJECT_NIL) {
		Bonobo_Unknown_unref(e->persist_stream_interface, &ev);
		CORBA_Object_release(e->persist_stream_interface, &ev);
		e->persist_stream_interface = CORBA_OBJECT_NIL;
	}


	if (e->persist_file_interface != CORBA_OBJECT_NIL) {
		Bonobo_Unknown_unref(e->persist_file_interface, &ev);
		CORBA_Object_release(e->persist_file_interface, &ev);
		e->persist_file_interface = CORBA_OBJECT_NIL;
	}
	CORBA_exception_free(&ev);
#endif
	GS_message(("do_exit"));
	GS_message((e->filename));
	GS_message((e->module));
	GS_message((e->key));

	if (e->studypad)
		settings.studypad_dialog_exist = FALSE;
	if (e->filename) {
		g_free(e->filename);
	}
	if (e->module) {
		g_free(e->module);
	}
	if (e->key) {
		g_free(e->key);
	}
	g_free(e);
}

static
void change_window_title(GtkWidget * window, const gchar * window_title)
{
	gtk_window_set_title(GTK_WINDOW(window), window_title);
}


#ifdef USE_GTKHTML3_14_23


static const gchar *file_ui =
"<ui>\n"
"  <menubar name='main-menu'>\n"
"    <menu action='file-menu'>\n"
"     <menuitem action='open'/>\n"
"     <menuitem action='save'/>\n"
"     <menuitem action='save-as'/>\n"
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

static void
handle_error (GError **error)
{
	if (*error != NULL) {
		g_warning ("%s", (*error)->message);
		g_clear_error (error);
	}
}

static GtkPrintOperationResult
print (GtkhtmlEditor *editor,
       GtkPrintOperationAction action)
{
	GtkPrintOperation *operation;
	GtkPrintOperationResult result;
	GError *error = NULL;

	operation = gtk_print_operation_new ();

	result = gtk_html_print_operation_run (
		gtkhtml_editor_get_html (editor), operation, action,
		GTK_WINDOW (editor), NULL, NULL, NULL, NULL, NULL, &error);

	g_object_unref (operation);
	handle_error (&error);

	return result;
}
static gint
open_dialog (EDITOR * e)
{
	GtkWidget *dialog;
	const gchar *filename;
	gint response;

	dialog = gtk_file_chooser_dialog_new (
		_("Save As"), GTK_WINDOW (e->window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);

	/*gtk_file_chooser_set_do_overwrite_confirmation (
		GTK_FILE_CHOOSER (dialog), TRUE);*/

	filename = gtkhtml_editor_get_filename (GTKHTML_EDITOR(e->window));

	if (filename != NULL)
		gtk_file_chooser_set_filename (
			GTK_FILE_CHOOSER (dialog), filename);
	else {
		gtk_file_chooser_set_current_folder (
			GTK_FILE_CHOOSER (dialog), g_get_home_dir ());
		gtk_file_chooser_set_current_name (
			GTK_FILE_CHOOSER (dialog), _("Untitled document"));
	}

	response = gtk_dialog_run (GTK_DIALOG (dialog));

	if (response == GTK_RESPONSE_ACCEPT) {
		gchar *new_filename;

		new_filename = gtk_file_chooser_get_filename (
			GTK_FILE_CHOOSER (dialog));
		//gtkhtml_editor_set_filename (e->window, new_filename);
		_load_file(e, new_filename);
		g_free (new_filename);
	}

	gtk_widget_destroy (dialog);

	return response;
}


static gint
save_dialog (GtkhtmlEditor *editor)
{
	GtkWidget *dialog;
	const gchar *filename;
	gint response;

	dialog = gtk_file_chooser_dialog_new (
		_("Save As"), GTK_WINDOW (editor),
		GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_file_chooser_set_do_overwrite_confirmation (
		GTK_FILE_CHOOSER (dialog), TRUE);

	filename = gtkhtml_editor_get_filename (editor);

	if (filename != NULL)
		gtk_file_chooser_set_filename (
			GTK_FILE_CHOOSER (dialog), filename);
	else {
		gtk_file_chooser_set_current_folder (
			GTK_FILE_CHOOSER (dialog), g_get_home_dir ());
		gtk_file_chooser_set_current_name (
			GTK_FILE_CHOOSER (dialog), _("Untitled document"));
	}

	response = gtk_dialog_run (GTK_DIALOG (dialog));

	if (response == GTK_RESPONSE_ACCEPT) {
		gchar *new_filename;

		new_filename = gtk_file_chooser_get_filename (
			GTK_FILE_CHOOSER (dialog));
		gtkhtml_editor_set_filename (editor, new_filename);
		g_free (new_filename);
	}

	gtk_widget_destroy (dialog);

	return response;
}

/* Helper for view_source_dialog() */
static gboolean
view_source_dialog_receiver (HTMLEngine *engine,
                             const gchar *data,
                             guint length,
                             GString *string)
{
	g_string_append_len (string, data, length);

	return TRUE;
}

static void
view_source_dialog (GtkhtmlEditor *editor,
                    const gchar *title,
                    const gchar *content_type,
                    gboolean show_output)
{
	GtkWidget *dialog;
	GtkWidget *content;
	GtkWidget *scrolled_window;
	GString *string;

	dialog = gtk_dialog_new_with_buttons (
		title, GTK_WINDOW (editor),
		GTK_DIALOG_DESTROY_WITH_PARENT |
		GTK_DIALOG_NO_SEPARATOR,
		GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
		NULL);

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (
		GTK_SCROLLED_WINDOW (scrolled_window),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (
		GTK_SCROLLED_WINDOW (scrolled_window), GTK_SHADOW_IN);
	gtk_box_pack_start (
		GTK_BOX (GTK_DIALOG (dialog)->vbox),
		scrolled_window, TRUE, TRUE, 0);

	gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 6);
	gtk_window_set_default_size (GTK_WINDOW (dialog), 400, 300);

	string = g_string_sized_new (4096);

	gtk_html_export (
		gtkhtml_editor_get_html (editor),
		content_type, (GtkHTMLSaveReceiverFn)
		view_source_dialog_receiver, string);

	if (show_output) {
		GtkHTMLStream *stream;

		content = gtk_html_new ();
		stream = gtk_html_begin (GTK_HTML (content));
		gtk_html_stream_write (stream, string->str, string->len);
		gtk_html_stream_close (stream, GTK_HTML_STREAM_OK);
	} else {
		GtkTextBuffer *buffer;

		content = gtk_text_view_new ();
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (content));
		gtk_text_buffer_set_text (buffer, string->str, string->len);
		gtk_text_view_set_editable (GTK_TEXT_VIEW (content), FALSE);
	}

	g_string_free (string, TRUE);

	gtk_container_add (GTK_CONTAINER (scrolled_window), content);
	gtk_widget_show_all (scrolled_window);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void
action_print_cb (GtkAction *action,
                 EDITOR *e)
{
	print (GTKHTML_EDITOR(e->window), GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG);
}

static void
action_print_preview_cb (GtkAction *action,
                         EDITOR *e)
{
	print (GTKHTML_EDITOR(e->window), GTK_PRINT_OPERATION_ACTION_PREVIEW);
}

static void
action_quit_cb (GtkAction *action,
                EDITOR *e)
{
	//gtk_widget_destroy(GTK_WIDGET(editor));
	//gtk_main_quit ();
}


static void
action_open_cb (GtkAction * action, EDITOR * e)
{
	open_dialog(e);
}

static void
action_save_cb (GtkAction *action,
                EDITOR *e)
{
	const gchar *filename;
	gboolean as_html;
	GError *error = NULL;

	if (gtkhtml_editor_get_filename (GTKHTML_EDITOR(e->window)) == NULL)
		if (save_dialog (GTKHTML_EDITOR(e->window)) == GTK_RESPONSE_CANCEL)
			return;

	filename = gtkhtml_editor_get_filename (GTKHTML_EDITOR(e->window));
	as_html = gtkhtml_editor_get_html_mode (GTKHTML_EDITOR(e->window));

	gtkhtml_editor_save (GTKHTML_EDITOR(e->window), filename, as_html, &error);
	handle_error (&error);
}

static void
action_save_as_cb (GtkAction *action,
                   EDITOR *e)
{
	const gchar *filename;
	gboolean as_html;
	GError *error = NULL;

	if (save_dialog (GTKHTML_EDITOR(e->window)) == GTK_RESPONSE_CANCEL)
		return;

	filename = gtkhtml_editor_get_filename (GTKHTML_EDITOR(e->window));
	as_html = gtkhtml_editor_get_html_mode (GTKHTML_EDITOR(e->window));

	gtkhtml_editor_save (GTKHTML_EDITOR(e->window), filename, as_html, &error);
	handle_error (&error);
}

static void
action_view_html_output (GtkAction *action,
                         GtkhtmlEditor *editor)
{
	view_source_dialog (editor, _("HTML Output"), "text/html", TRUE);
}

static void
action_view_html_source (GtkAction *action,
                         GtkhtmlEditor *editor)
{
	view_source_dialog (editor, _("HTML Source"), "text/html", FALSE);
}

static void
action_view_plain_source (GtkAction *action,
                          GtkhtmlEditor *editor)
{
	view_source_dialog (editor, _("Plain Source"), "text/plain", FALSE);
}

static GtkActionEntry file_entries[] = {

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

	{ "open",
	  GTK_STOCK_OPEN,
	  N_("_Open"),
	  NULL,
	  NULL,
	  G_CALLBACK (action_open_cb) },

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

static GtkActionEntry view_entries[] = {

	{ "view-html-output",
	  NULL,
	  N_("HTML _Output"),
	  NULL,
	  NULL,
	  G_CALLBACK (action_view_html_output) },

	{ "view-html-source",
	  NULL,
	  N_("_HTML Source"),
	  NULL,
	  NULL,
	  G_CALLBACK (action_view_html_source) },

	{ "view-plain-source",
	  NULL,
	  N_("_Plain Source"),
	  NULL,
	  NULL,
	  G_CALLBACK (action_view_plain_source) },

	{ "view-menu",
	  NULL,
	  N_("_View"),
	  NULL,
	  NULL,
	  NULL }
};

GtkWidget *
editor_new (const gchar * title, EDITOR *e)
{
	GtkActionGroup *action_group;
	GtkUIManager *manager;
	GtkWidget *editor;
	GError *error = NULL;
	//GtkWindow *window;
/*
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
*/
//	g_thread_init (NULL);

//	gtk_init (&argc, &argv);

	editor = gtkhtml_editor_new ();
	e->window = editor;
	gtk_window_set_title(GTK_WINDOW(editor),title);
	manager = gtkhtml_editor_get_ui_manager (GTKHTML_EDITOR (editor));

	gtk_ui_manager_add_ui_from_string (manager, file_ui, -1, &error);
	handle_error (&error);

	gtk_ui_manager_add_ui_from_string (manager, view_ui, -1, &error);
	handle_error (&error);

	action_group = gtk_action_group_new ("file");
	gtk_action_group_set_translation_domain (
		action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (
		action_group, file_entries,
		G_N_ELEMENTS (file_entries), e);
	gtk_ui_manager_insert_action_group (manager, action_group, 0);

	action_group = gtk_action_group_new ("view");
	gtk_action_group_set_translation_domain (
		action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (
		action_group, view_entries,
		G_N_ELEMENTS (view_entries), editor);
	gtk_ui_manager_insert_action_group (manager, action_group, 0);

	gtk_ui_manager_ensure_update (manager);
	gtk_widget_show (editor);

	g_signal_connect(editor, "delete-event",
			 G_CALLBACK(app_delete_cb), (EDITOR *) e);
	/*g_signal_connect (
		editor, "destroy",
		G_CALLBACK (gtk_main_quit), NULL);*/

	//gtk_main ();

	return editor;

}	
#else


static void load_through_persist_stream(const gchar * text, EDITOR * e)
{
	BonoboObject *smem;
	CORBA_Environment ev;

	CORBA_exception_init(&ev);

	smem =
	    bonobo_stream_mem_create(text, strlen(text), FALSE, TRUE);
	CORBA_exception_init(&ev);
	Bonobo_PersistStream_load(e->persist_stream_interface,
				  BONOBO_OBJREF(smem), "text/html",
				  &ev);
	bonobo_object_unref(BONOBO_OBJECT(smem));
	e->is_changed = FALSE;
	CORBA_exception_free(&ev);
}


static void
delete_note_cb(GtkWidget * widget, gpointer data)
{	
	gint test;
	GS_DIALOG *info;
	gchar *buf = NULL;
	EDITOR *e = (EDITOR *) data;
	
	if (e->studypad)
		return;
	
	info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		buf = g_strdup_printf
		    ("<span weight=\"bold\" size=\"larger\">%s %s?</span>",
		    _("Are you sure you want to delete the note for") , e->key);
		info->label_top = buf;
		info->label2 =  N_("It will be lost permanently!");
		info->yes = TRUE;
		info->cancel = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			main_delete_note(e->module, e->key);
			load_through_persist_stream("", e);
		}
		g_free(info);
		g_free(buf);
}

static void
save_through_persist_stream_cb(GtkWidget * widget, gpointer data)
{
	BonoboObject *smem;
	CORBA_Environment ev;
	const gchar *text;
	gchar *buf;
	size_t len;
	EDITOR *ed = (EDITOR *) data;

	CORBA_exception_init(&ev);
	smem = bonobo_stream_mem_create(NULL, 1, FALSE, TRUE);
	Bonobo_PersistStream_save(ed->persist_stream_interface,
				  BONOBO_OBJREF(smem), "text/html",
				  &ev);
	bonobo_stream_client_write(BONOBO_OBJREF(smem), "", 1, &ev);

	text = bonobo_stream_mem_get_buffer(BONOBO_STREAM_MEM(smem));
	buf = strdup(text);
	// converts encoding from ncr to utf8
	if(g_strstr_len(buf, strlen(buf), "&#") != NULL)
		text = ncr_to_utf8(buf);
	g_free(buf);
	
	GS_message((text));
	
	if(ed->type == NOTE_EDITOR)
		main_save_note(ed->module, ed->key, text);
	else {
		main_treekey_save_book_text(ed->module, ed->key, (gchar*)text); 
		
	}
	
	GNOME_GtkHTML_Editor_Engine_dropUndo(ed->engine, &ev);
	ed->is_changed = FALSE;
	bonobo_object_unref(BONOBO_OBJECT(smem));
	CORBA_exception_free(&ev);
	
}

static void
save_through_plain_persist_stream_cb(GtkWidget * widget, gpointer data)
{
	BonoboObject *smem;
	CORBA_Environment ev;
	const char *text;
	size_t len;
	EDITOR *ed = (EDITOR *) data;

	CORBA_exception_init(&ev);
	smem = bonobo_stream_mem_create(NULL, 1, FALSE, TRUE);
	Bonobo_PersistStream_save(ed->persist_stream_interface,
				  BONOBO_OBJREF(smem), "text/plain",
				  &ev);
	bonobo_stream_client_write(BONOBO_OBJREF(smem), "", 1, &ev);

	text = bonobo_stream_mem_get_buffer(BONOBO_STREAM_MEM(smem));
	GS_message((text));
	//main_save_note(ed->module, ed->key, text);
	GNOME_GtkHTML_Editor_Engine_dropUndo(ed->engine, &ev);
	ed->is_changed = FALSE;
	bonobo_object_unref(BONOBO_OBJECT(smem));
	CORBA_exception_free(&ev);	
}

/* Loading/saving through PersistFile.  */

static gboolean
save_through_persist_file(EDITOR * e, const gchar * filename)
{
	CORBA_Environment ev;

	CORBA_exception_init(&ev);

	Bonobo_PersistFile_save(e->persist_file_interface, filename,
				&ev);

	if (ev._major != CORBA_NO_EXCEPTION) {
		g_warning("Cannot save.");
		CORBA_exception_free(&ev);
		return FALSE;
	}
	if (!e->filename || strcmp(e->filename, filename)) {
		if (e->filename)
			g_free(e->filename);
		e->filename = g_strdup(filename);
		xml_set_value("GnomeSword", "studypad", "lastfile",
			      e->filename);
		settings.studypadfilename =
		    xml_get_value("studypad", "lastfile");
		change_window_title(e->window, filename);
	}
	e->is_changed = FALSE;
	g_free((gchar *) filename);
	GNOME_GtkHTML_Editor_Engine_dropUndo(e->engine, &ev);
	CORBA_exception_free(&ev);
	return TRUE;
}


/* Common file selection widget.  We make sure there is only one open at a
   given time.  */

enum _FileSelectionOperation {
	OP_NONE,
	OP_SAVE_THROUGH_PERSIST_STREAM,
	OP_SAVE_THROUGH_PLAIN_PERSIST_STREAM,
	OP_LOAD_THROUGH_PERSIST_STREAM,
	OP_SAVE_THROUGH_PERSIST_FILE,
	OP_LOAD_THROUGH_PERSIST_FILE
};
typedef enum _FileSelectionOperation FileSelectionOperation;

struct _FileSelectionInfo {
	BonoboWidget *control;
	GtkWidget *widget;

	FileSelectionOperation operation;
};
typedef struct _FileSelectionInfo FileSelectionInfo;

static FileSelectionInfo file_selection_info = {
	NULL,
	NULL,
	OP_NONE
};

static void file_selection_destroy_cb(GtkWidget * widget, gpointer data)
{
	file_selection_info.widget = NULL;
}

static void
view_source_dialog(BonoboWindow * app, char *type, gboolean as_html)
{
	//load_through_persist_stream ("<b>bold</b><br>not so bold",NULL);
}

static void view_html_source_cb(GtkWidget * widget, gpointer data)
{
	view_source_dialog(data, "text/html", FALSE);
}

static void view_plain_source_cb(GtkWidget * widget, gpointer data)
{
	view_source_dialog(data, "text/plain", FALSE);
}

static void view_html_source_html_cb(GtkWidget * widget, gpointer data)
{
	view_source_dialog(data, "text/html", TRUE);
}

static void file_selection_cancel_cb(GtkWidget * widget, gpointer data)
{
	gtk_widget_destroy(GTK_WIDGET(data));
}

static void file_selection_ok_cb(GtkWidget * widget, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
	const gchar *fname;

	fname = gtk_file_selection_get_filename
	    (GTK_FILE_SELECTION(file_selection_info.widget));

	switch (file_selection_info.operation) {
	case OP_LOAD_THROUGH_PERSIST_STREAM:
		//load_through_persist_stream (fname, interface);
		break;
	case OP_SAVE_THROUGH_PERSIST_STREAM:
		//save_through_persist_stream (fname, interface);
		break;
	case OP_SAVE_THROUGH_PLAIN_PERSIST_STREAM:
		//save_through_plain_persist_stream (fname, interface);
		break;
	case OP_LOAD_THROUGH_PERSIST_FILE:
		_load_file(e, g_strdup(fname));
		break;
	case OP_SAVE_THROUGH_PERSIST_FILE:
		save_through_persist_file(e, g_strdup(fname));
		break;
	default:
		g_assert_not_reached();
	}
	gtk_widget_destroy(file_selection_info.widget);
}


static void
open_or_save_as_dialog(EDITOR * e, FileSelectionOperation op)
{
	GtkWidget *widget;
	BonoboWidget *control;
	gchar *directory = NULL;

	control = e->control;
	
	if (file_selection_info.widget != NULL) {
		gdk_window_show(GTK_WIDGET(file_selection_info.widget)->
				window);
		return;
	}

	if (op == OP_LOAD_THROUGH_PERSIST_FILE
	    || op == OP_LOAD_THROUGH_PERSIST_STREAM) {
		widget = gtk_file_selection_new(_("Open file..."));
		if (settings.studypaddir) {
			directory =
			    g_strdup_printf("%s/",
					    settings.studypaddir);
			gtk_file_selection_set_filename(
						(GtkFileSelection *) widget, 
						directory);
			g_free(directory);
		}
	} else {
		widget = gtk_file_selection_new(_("Save file as..."));
		if (e->filename)
			gtk_file_selection_set_filename(
						(GtkFileSelection *) widget, 
						e->filename);
		else {
			directory =
			    g_strdup_printf("%s/",
					    settings.studypaddir);
			gtk_file_selection_set_filename(
						(GtkFileSelection *) widget, 
						directory);
			g_free(directory);
		}
	}
	gtk_window_set_transient_for(GTK_WINDOW(widget),
				     GTK_WINDOW(e->window));

	file_selection_info.widget = widget;
	file_selection_info.control = control;
	file_selection_info.operation = op;

	g_signal_connect(GTK_FILE_SELECTION(widget)->cancel_button,
			 "clicked",
			 G_CALLBACK(file_selection_cancel_cb), widget);
	g_signal_connect(GTK_FILE_SELECTION(widget)->ok_button,
			 "clicked", G_CALLBACK(file_selection_ok_cb),
			 (EDITOR *) e);
	g_signal_connect(file_selection_info.widget, "destroy",
			 G_CALLBACK(file_selection_destroy_cb), NULL);

	gtk_widget_show(file_selection_info.widget);
}

/* "Open through persist stream" dialog.  */
static void
open_through_persist_stream_cb(GtkWidget * widget, gpointer data)
{
	open_or_save_as_dialog((EDITOR *) data,
			       OP_LOAD_THROUGH_PERSIST_STREAM);
}


static void open_new_document_cb(GtkWidget * widget, gpointer data)
{
	gchar *template = NULL;
	EDITOR *e = (EDITOR *) data;

	template =
	    g_strdup_printf("%s/template.pad", settings.gSwordDir);
	
	if (g_file_test(template, G_FILE_TEST_EXISTS)) {
		if (e->filename)
			g_free(e->filename);
		e->filename = g_strdup(template);
		_load_file(e, g_strdup(e->filename));
		g_free(e->filename);
		e->filename = NULL;
		GS_message((template));
	} 
	g_free(template);
	e->is_changed = TRUE;
}

/* "Open through persist file" dialog.  */
static void
add_bookmark_cb(GtkWidget * widget, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
	gchar *label = NULL;
	
	if (e->studypad) {
		label = g_strdup_printf("%s, %s",e->filename, "studypad");
		gui_bookmark_dialog(label, "studypad", e->filename);
		g_free(label);
	} /* else {
		label = g_strdup_printf("%s, %s",e->key, ->filename);
		gui_bookmark_dialog(label, e->filename, e->key);
		g_free(label);	
	}
*/
}


/* "Open through persist file" dialog.  */
static void
open_through_persist_file_cb(GtkWidget * widget, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
	open_or_save_as_dialog(e, OP_LOAD_THROUGH_PERSIST_FILE);
	e->is_changed = FALSE;
}

/* "Save through persist file" dialog.  */
static void
save_through_persist_file_cb(GtkWidget * widget, gpointer data)
{
	gchar *filename = NULL;
	EDITOR *e = (EDITOR *) data;
	if (e->filename) {
		filename = g_strdup(e->filename);
		save_through_persist_file(e, filename);
	}
	else
		open_or_save_as_dialog(e, OP_SAVE_THROUGH_PERSIST_FILE);
}

/* "Save through persist file" dialog.  */
static void
save_as_through_persist_file_cb(GtkWidget * widget, gpointer data)
{
	gchar *filename = NULL;
	EDITOR *e = (EDITOR *) data;
	open_or_save_as_dialog(e, OP_SAVE_THROUGH_PERSIST_FILE);
}

static void exit_cb(GtkWidget * widget, gpointer data)
{
	if (editor_is_dirty((EDITOR *) data)) {
		switch (ask_about_saving((EDITOR *) data)) {
		case GS_YES:	// * exit saving *

			break;
		case GS_NO:	// * exit without saving *

			break;
		case GS_CANCEL:	// * don't exit *
			return;
			break;
		}
	}
	editors_all = g_list_remove(editors_all, (EDITOR*) data);
	gtk_widget_destroy(((EDITOR*) data)->window);
	do_exit((EDITOR *) data);
}

static
void print_message_cb(GtkWidget * widget, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
	//gui_html_print(e->html_widget, FALSE);
}

static
void print_preview_message_cb(GtkWidget * widget, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
	//gui_html_print(e->html_widget, TRUE);
}


static BonoboUIVerb verbs[] = {
	BONOBO_UI_UNSAFE_VERB("OpenFile", open_through_persist_file_cb),
	BONOBO_UI_UNSAFE_VERB("SaveFile", save_through_persist_file_cb),
	BONOBO_UI_UNSAFE_VERB("SaveFileAs", save_as_through_persist_file_cb),
	BONOBO_UI_UNSAFE_VERB("BookMark",add_bookmark_cb),
	BONOBO_UI_UNSAFE_VERB("OpenStream",open_through_persist_stream_cb),
	BONOBO_UI_UNSAFE_VERB("SaveNote",save_through_persist_stream_cb),
	BONOBO_UI_UNSAFE_VERB("DeleteNote",delete_note_cb),
	BONOBO_UI_UNSAFE_VERB("NewDoc", open_new_document_cb),
	BONOBO_UI_UNSAFE_VERB("SavePlainStream",save_through_plain_persist_stream_cb),
	BONOBO_UI_UNSAFE_VERB("PrintMessage", print_message_cb),
	BONOBO_UI_UNSAFE_VERB("PrintPreviewMessage",print_preview_message_cb),
	BONOBO_UI_UNSAFE_VERB("ViewHTMLSource", view_html_source_cb),
	BONOBO_UI_UNSAFE_VERB("ViewHTMLSourceHTML",view_html_source_html_cb),
	BONOBO_UI_UNSAFE_VERB("ViewPlainSource", view_plain_source_cb),
	
	
	BONOBO_UI_UNSAFE_VERB("FileExit", exit_cb),

	BONOBO_UI_VERB_END
};

static void
menu_format_html_cb(BonoboUIComponent * component,
		    const char *path,
		    Bonobo_UIComponent_EventType type,
		    const char *state, gpointer user_data)
{
	if (type != Bonobo_UIComponent_STATE_CHANGED)
		return;
	formatHTML = *state == '0' ? 0 : 1;

	bonobo_widget_set_property(BONOBO_WIDGET(user_data),
				   "FormatHTML", TC_CORBA_boolean,
				   formatHTML, NULL);
	bonobo_widget_set_property(BONOBO_WIDGET(user_data),
				   "HTMLTitle", TC_CORBA_string,
				   "testing", NULL);
}
#endif


static void
_load_file(EDITOR * e, const gchar * filename)
{
	GtkWidget *choser;
	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(filename);

	GS_message((filename));
	
	xml_set_value("GnomeSword", "studypad", "lastfile",
		      e->filename);
	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	change_window_title(e->window, e->filename);
	
#ifdef USE_GTKHTML3_14_23
	char *contents;
	char *etag_out;
	gsize length; 
	GFile *file;
	file = g_file_new_for_path(filename);
	gtkhtml_editor_set_filename(GTKHTML_EDITOR(e->window),e->filename);
	if(g_file_load_contents (file,
                              NULL,
                              &contents,
                              &length,
                              &etag_out,
                              NULL))
		gtkhtml_editor_set_text_html (GTKHTML_EDITOR(e->window),
					      contents,
					      length);
	gtkhtml_editor_drop_undo(GTKHTML_EDITOR(e->window));
	gtkhtml_editor_set_changed (GTKHTML_EDITOR(e->window), FALSE);
	return;
#else

	CORBA_Environment ev;

	CORBA_exception_init(&ev);


	Bonobo_PersistFile_load(e->persist_file_interface, filename, &ev);

	if (ev._major != CORBA_NO_EXCEPTION)
		g_warning("Cannot load.");
	GNOME_GtkHTML_Editor_Engine_dropUndo(e->engine, &ev);
	CORBA_exception_free(&ev);
	g_free((gchar *) filename);
	e->is_changed = FALSE;
#endif
}

gboolean editor_is_dirty(EDITOR * e)
{
#ifdef USE_GTKHTML3_14_23
	return gtkhtml_editor_get_changed(GTKHTML_EDITOR(e->window));
#else

	CORBA_Environment ev;
	gboolean rv;

	CORBA_exception_init(&ev);
	/* rv = e->is_changed
	    || (GNOME_GtkHTML_Editor_Engine_hasUndo(e->engine, &ev) &&
		!GNOME_GtkHTML_Editor_Engine_runCommand(e->engine,
							"is-saved",
							&ev)); */
	rv = GNOME_GtkHTML_Editor_Engine_hasUndo(e->engine, &ev);
	
	CORBA_exception_free(&ev);
	return rv;
#endif
}


void editor_save_book(EDITOR * e)
{
#ifdef USE_GTKHTML3_14_23
	
#else
	if (editor_is_dirty(e)) {
		GS_message(("editor_is_dirty"));
		save_through_persist_stream_cb(NULL, e);
	}
#endif
}

void editor_load_book(EDITOR * e)
{
	gchar *title = NULL;
	gchar *text = NULL;	

#ifdef USE_GTKHTML3_14_23
	
#else	
		
	if(!g_ascii_isdigit(e->key[0])) return; /* make sure is a number (offset) */
	
	title = g_strdup_printf("%s", e->module);
	GS_message (("book: %s\noffset :%s", e->module, e->key));
	
	if(atol(e->key) != 0)
		text = main_get_book_raw_text (e->module, e->key);
	else
		text = g_strdup(e->module);
	
	if(strlen(text)) 
		load_through_persist_stream(text, e);
	else
		load_through_persist_stream("", e); /* clear editor if no text */
	
	change_window_title(e->window, title);
	
	if (text)
		g_free(text);
	if (title)
		g_free(title);
#endif
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

void editor_sync_toggled(GtkToggleButton * button, EDITOR * e)
{
#ifdef USE_GTKHTML3_14_23
	
#else
	if (button->active) {
		if (editor_is_dirty(e))
			save_through_persist_stream_cb(NULL, (EDITOR*)e);
		editor_load_note(e, NULL,settings.currentverse);
		e->sync = TRUE;
	}
	else		
		e->sync = FALSE;
#endif
}


void editor_sync_with_main(void)
{
	GList *tmp = NULL;
	EDITOR * e;
	
	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		e = (EDITOR*)tmp->data;
		if (!e->studypad) 
			editor_sync_toggled(GTK_TOGGLE_BUTTON(e->sync_button), e);
		tmp = g_list_next(tmp);
	}
}

void
editor_load_note(EDITOR * e, const gchar * module_name,
		 const gchar * key)
{
	gchar *title;
	gchar *text;

#ifdef USE_GTKHTML3_14_23
	
#else	
	if (editor_is_dirty(e))
		save_through_persist_stream_cb(NULL, e);
	
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
	text = main_get_raw_text((gchar *) e->module, (gchar *) e->key);
	if(strlen(text)) 
		load_through_persist_stream(text, e);
	else
		load_through_persist_stream("", e); /* clear editor if no text */
	
	change_window_title(e->window, title);
    	if(e->type == NOTE_EDITOR) {
#ifdef OLD_NAVBAR
		main_navbar_set(e->navbar, e->key);
#else	
		main_navbar_versekey_set(e->navbar, e->key);
#endif
	} 
	
	if (text)
		g_free(text);
	if (title)
		g_free(title);
#endif
}


gboolean editor_close_all(void)
{
	GList *tmp = NULL;
	EDITOR *e = NULL;

#ifdef USE_GTKHTML3_14_23
	
#else	
	tmp = g_list_first(editors_all);
	GS_message(("number of editors = %d",g_list_length(tmp)));
	while (tmp != NULL) {
		e = (EDITOR*)tmp->data;
		if (e->window)
			do_exit(e);			
		tmp = g_list_next(tmp);
	}
	g_list_free(editors_all);
#endif
}

static int
app_delete_cb(GtkWidget * widget, GdkEvent * event, gpointer data)
{
	if (editor_is_dirty((EDITOR *) data)) {
		switch (ask_about_saving((EDITOR *) data)) {
		case GS_YES:	/* exit saving */

			break;
		case GS_NO:	/* exit without saving */

			break;
		case GS_CANCEL:
			return TRUE;
			break;
		}
	}
	editors_all = g_list_remove(editors_all, (EDITOR*) data);
	do_exit((EDITOR *) data);
	return FALSE;
}
/*
static void size_changed(GtkHTML * html, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
	GS_message(("size_changed"));
	e->is_changed = TRUE;
}
*/
#ifndef USE_GTKHTML3_14_23
static gboolean on_key_release_event(GtkWidget * widget,
				     GdkEventKey * event, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
	switch (event->hardware_keycode) {
	case 37:
	case 50:
	case 64:
	case 66:
	case 77:
	case 97:
	case 98:
	case 100:
	case 102:
	case 103:
	case 104:
	case 109:
	case 113:
	case 115:
	case 116:
		return FALSE;
		break;
	default:
		e->is_changed = TRUE;
		break;
	}
	return FALSE;
}



#ifdef OLD_NAVBAR
static void on_comboboxentry4_changed(GtkComboBox * combobox, EDITOR * e)
{
	gchar *book = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	if (!do_display)
		return;
	
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &book, -1);

	buf = g_strdup_printf("%s 1:1", book);
	editor_load_note(e, NULL, buf);
	
	main_navbar_set(e->navbar, buf);
	g_free(book);
	g_free(buf);
}


static void on_comboboxentry5_changed(GtkComboBox * combobox, EDITOR * e)
{
	gchar *book = NULL;
	gchar *chapter = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	GtkTreeModel *book_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (e->navbar.comboboxentry_book));
	if (!do_display)
		return;
	
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (e->navbar.comboboxentry_book),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter, 0, &book,
			   -1);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			   0, &chapter, -1);

	buf = g_strdup_printf("%s %s:1", book, chapter);
	editor_load_note(e, NULL, buf);
	main_navbar_set(e->navbar, buf);

	g_free(book);
	g_free(chapter);
	g_free(buf);
}


static void on_comboboxentry6_changed(GtkComboBox * combobox, EDITOR * e)
{
	gchar *book = NULL;
	gchar *chapter = NULL;
	gchar *verse = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	GtkTreeModel *book_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (e->navbar.comboboxentry_book));
	GtkTreeModel *chapter_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (e->navbar.comboboxentry_chapter));


	if (!do_display)
		return;
	
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (e->navbar.comboboxentry_book),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter, 0, &book,
			   -1);

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (e->navbar.comboboxentry_chapter),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(chapter_model), &iter, 0,
			   &chapter, -1);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &verse, -1);

	buf = g_strdup_printf("%s %s:%s", book, chapter, verse);
	editor_load_note(e, NULL, buf);
	main_navbar_set(e->navbar, buf);

	g_free(book);
	g_free(chapter);
	g_free(verse);
	g_free(buf);
}
#endif
/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "/.h"
 *
 *   void on_entry_activate(GtkEntry * entry, EDITOR * e)	
 *
 * Description
 *   this is called when user hits enter key in the lookup entry
 *
 * Return value
 *   void
 */

static void on_entry_activate(GtkEntry * entry, EDITOR * e)
{
	const gchar *buf = gtk_entry_get_text(entry);
	if (e->navbar.key)
		g_free(e->navbar.key);
#ifdef OLD_NAVBAR
	e->navbar.key = g_strdup(buf);
#else
	e->navbar.key = g_string_assign(e->navbar.key,buf);
#endif
	editor_load_note(e, NULL, buf);
}


#endif

static
gint ask_about_saving(EDITOR * e)
{
#ifdef USE_GTKHTML3_14_23
	
#else
	CORBA_Object interface;
	CORBA_Environment ev;
#endif
	gint test;
	GS_DIALOG *info;
	gchar *buf = NULL;
	gchar *buf1 = NULL;
	gchar *buf2 = NULL;
	gchar *buf3 = NULL;
	gint retval = FALSE;

	if (!e->studypad) {
#ifdef USE_GTKHTML3_14_23
		/* save notes and prayer lists */
#else
		save_through_persist_stream_cb(NULL, e);
		retval = GS_YES;
#endif
	} else {
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		if (settings.studypadfilename)
			buf = settings.studypadfilename;
		else
			buf = N_("File");
		buf1 = _("Save the changes to document");
		buf2 = _("before closing?");
		buf3 =
		    g_strdup_printf
		    ("<span weight=\"bold\" size=\"larger\">%s %s %s</span>",
		     buf1, buf, buf2);
		info->label_top = buf3;
		info->label2 =
		    _("If you don't save, changes will be permanently lost.");
		info->save = TRUE;
		info->cancel = TRUE;
		info->no_save = TRUE;

		test = gui_alert_dialog(info);
		retval = test;
		if (test == GS_YES) {
			if (e->filename) {
#ifdef USE_GTKHTML3_14_23
				gtkhtml_editor_save (GTKHTML_EDITOR(e->window),
						 e->filename,
						 TRUE,
						 NULL);
#else
				save_through_persist_file(e,
						g_strdup(e->filename));
#endif
			} else {
#ifdef USE_GTKHTML3_14_23
	
#else
				open_or_save_as_dialog(e,
					     OP_SAVE_THROUGH_PERSIST_FILE);
#endif
			}
		}
		g_free(info);
		g_free(buf3);
		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
	}
	return retval;
}

static GtkWidget *navebar_create(EDITOR * editor)
{
#ifdef OLD_NAVBAR
	GtkWidget *hbox3;
	GtkWidget *image;
	GtkWidget *separatortoolitem;
	GtkListStore *store;
	GtkCellRenderer *renderer;

	hbox3 = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbox3);
	gtk_container_set_border_width(GTK_CONTAINER(hbox3), 3);

	editor->sync_button = gtk_toggle_button_new();
	gtk_widget_show(editor->sync_button);
	gtk_box_pack_start(GTK_BOX(hbox3), editor->sync_button, FALSE, FALSE,
			   0);
	gtk_button_set_relief(GTK_BUTTON(editor->sync_button), GTK_RELIEF_NONE);

	image =
	    gtk_image_new_from_stock("gtk-refresh",
				     GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image);
	gtk_container_add(GTK_CONTAINER(editor->sync_button), image);

	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);

	store = gtk_list_store_new(1, G_TYPE_STRING);

	editor->navbar.comboboxentry_book =
	    gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	gtk_widget_show(editor->navbar.comboboxentry_book);
	gtk_box_pack_start(GTK_BOX(hbox3), editor->navbar.comboboxentry_book,
			   TRUE, TRUE, 0);
	gtk_widget_set_size_request(editor->navbar.comboboxentry_book, -1,
				    6);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (editor->navbar.comboboxentry_book),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (editor->navbar.comboboxentry_book),
				       renderer, "text", 0, NULL);


	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM
					 (separatortoolitem), FALSE);

	store = gtk_list_store_new(1, G_TYPE_STRING);

	editor->navbar.comboboxentry_chapter = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));	//gtk_combo_box_entry_new();
	gtk_widget_show(editor->navbar.comboboxentry_chapter);
	gtk_box_pack_start(GTK_BOX(hbox3),
			   editor->navbar.comboboxentry_chapter, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(editor->navbar.comboboxentry_chapter, 61,
				    -1);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (editor->navbar.comboboxentry_chapter),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (editor->navbar.
					comboboxentry_chapter),
				       renderer, "text", 0, NULL);

	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM
					 (separatortoolitem), FALSE);

	store = gtk_list_store_new(1, G_TYPE_STRING);
	editor->navbar.comboboxentry_verse = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));	//gtk_combo_box_entry_new();
	gtk_widget_show(editor->navbar.comboboxentry_verse);
	gtk_box_pack_start(GTK_BOX(hbox3),
			   editor->navbar.comboboxentry_verse, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(editor->navbar.comboboxentry_verse, 61,
				    -1);


	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (editor->navbar.comboboxentry_verse),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (editor->navbar.comboboxentry_verse),
				       renderer, "text", 0, NULL);


	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);

	editor->navbar.lookup_entry = gtk_entry_new();
	gtk_widget_show(editor->navbar.lookup_entry);
	gtk_box_pack_start(GTK_BOX(hbox3), editor->navbar.lookup_entry, TRUE,
			   TRUE, 0);
	
	g_signal_connect(GTK_OBJECT(editor->sync_button),
			 "toggled", G_CALLBACK(editor_sync_toggled), editor);

	g_signal_connect((gpointer) editor->navbar.comboboxentry_book,
			 "changed",
			 G_CALLBACK(on_comboboxentry4_changed),editor);
	g_signal_connect((gpointer) editor->navbar.comboboxentry_chapter,
			 "changed",
			 G_CALLBACK(on_comboboxentry5_changed),editor);
	g_signal_connect((gpointer) editor->navbar.comboboxentry_verse,
			 "changed",
			 G_CALLBACK(on_comboboxentry6_changed),editor);
	g_signal_connect((gpointer) editor->navbar.lookup_entry, "activate",
			 G_CALLBACK(on_entry_activate), editor);
	return hbox3;
#endif	
}


static GtkWidget *container_create(const gchar * window_title,
				   EDITOR * editor)
{
#ifdef USE_GTKHTML3_14_23
	return NULL;
#else
	GtkWindow *window;
	GtkWidget *vbox;
	GtkWidget *paned;
	GtkWidget *scrollbar;
	GtkWidget *toolbar_nav = NULL;
	BonoboUIComponent *component;
	BonoboUIContainer *container;
	CORBA_Environment ev;
	GNOME_GtkHTML_Editor_Engine engine;
	gpointer servant;
	BonoboObject *impl;


	win = bonobo_window_new("Editor", window_title);
	
	vbox = gtk_vbox_new(FALSE, 6);
	gtk_widget_show(vbox);

	window = GTK_WINDOW(win);
	editor->window = win;
	container = bonobo_window_get_ui_container(BONOBO_WINDOW(win));

	g_signal_connect(window, "delete-event",
			 G_CALLBACK(app_delete_cb), (EDITOR *) editor);

	gtk_window_set_default_size(window, 640, 440);
	gtk_window_set_resizable(window, TRUE);

	component = bonobo_ui_component_new("Editor");
	
	bonobo_ui_component_set_container(component,
					  BONOBO_OBJREF(container),
					  NULL);
	bonobo_ui_component_add_verb_list_with_data(component, verbs,
						    editor);

	CORBA_exception_init(&ev);
	if (editor->studypad) {
		bonobo_ui_util_set_ui(component,
				      INSTALL_PREFIX,
				      gui_general_user_file
				      ("/editor_studypad.xml", FALSE),
				      "editor", &ev);
	} else {
		bonobo_ui_util_set_ui(component,
				      INSTALL_PREFIX,
				      gui_general_user_file
				      ("/editor_note.xml", FALSE),
				      "editor", &ev);
	}
	CORBA_exception_free(&ev);

	editor->control =
	    BONOBO_WIDGET(bonobo_widget_new_control(CONTROL_ID,
				      BONOBO_OBJREF(container)));
	GS_message((CONTROL_ID));
	if (editor->control == NULL)
		g_error("Cannot get `%s'.", CONTROL_ID);

	bonobo_widget_set_property(BONOBO_WIDGET(editor->control), "FormatHTML",
				   TC_CORBA_boolean, formatHTML, NULL);
	bonobo_ui_component_set_prop(component, "/commands/FormatHTML",
				     "state", formatHTML ? "1" : "0",
				     NULL);
	bonobo_ui_component_add_listener(component, "FormatHTML",
					 menu_format_html_cb, editor->control);
	
	
	/* add editor control to vbox */
	switch(editor->type){
		case NOTE_EDITOR:
#ifdef OLD_NAVBAR
			toolbar_nav = navebar_create(editor);
			gtk_widget_show(toolbar_nav);
			gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(toolbar_nav), FALSE, TRUE, 0);
			editor->navbar.module_name = editor->module;
			if (editor->navbar.key == NULL)
				editor->navbar.key = g_strdup(editor->key);
			main_navbar_fill_book_combo(editor->navbar);
#else
			toolbar_nav = gui_navbar_versekey_editor_new(editor);
			gtk_widget_show(toolbar_nav);
			gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(toolbar_nav), FALSE, TRUE, 0);
#endif
			gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(editor->control), TRUE, TRUE, 0);
		break;
		case BOOK_EDITOR:
			paned = gtk_hpaned_new();
			gtk_widget_show(paned);
			gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(paned), 
					    	TRUE, TRUE, 0);
	    		scrollbar = gtk_scrolled_window_new(NULL, NULL);
			gtk_widget_show(scrollbar);
			gtk_paned_pack1(GTK_PANED(paned), GTK_WIDGET(scrollbar),
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
			gtk_paned_set_position(GTK_PANED(paned),
				       125);
			gtk_tree_view_expand_all((GtkTreeView *)editor->treeview);
    			gtk_paned_pack2(GTK_PANED(paned), GTK_WIDGET(editor->control),
						TRUE, TRUE);
		break;
		case STUDYPAD_EDITOR:
			gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(editor->control), TRUE, TRUE, 0);
		break;
	}
	
	/* add a statusbar */
	editor->statusbar = gtk_statusbar_new();
	gtk_widget_show(editor->statusbar);
	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(editor->statusbar), FALSE, TRUE, 0);
	
	bonobo_window_set_contents(BONOBO_WINDOW(win), GTK_WIDGET(vbox));

	/* FIXME: handle exceptions */
	CORBA_exception_init(&ev);
	editor->persist_file_interface
	    =
	    Bonobo_Unknown_queryInterface(bonobo_widget_get_objref
					  (BONOBO_WIDGET(editor->control)),
					  "IDL:Bonobo/PersistFile:1.0",
					  &ev);
	editor->persist_stream_interface =
	    Bonobo_Unknown_queryInterface(bonobo_widget_get_objref
					  (BONOBO_WIDGET(editor->control)),
					  "IDL:Bonobo/PersistStream:1.0",
					  &ev);
	CORBA_exception_free(&ev);

	gtk_widget_show_all(GTK_WIDGET(window));
    
#ifndef OLD_NAVBAR
	if(editor->type == NOTE_EDITOR) {
		gtk_widget_hide(editor->navbar.button_history_back);
		gtk_widget_hide(editor->navbar.button_history_next);
		gtk_widget_hide(editor->navbar.button_history_menu);
		gtk_widget_hide(editor->navbar.arrow_book_up_box);
		gtk_widget_hide(editor->navbar.arrow_book_down_box);
		gtk_widget_hide(editor->navbar.arrow_chapter_up_box);
		gtk_widget_hide(editor->navbar.arrow_chapter_down_box);
		gtk_widget_hide(editor->navbar.arrow_verse_up_box);
		gtk_widget_hide(editor->navbar.arrow_verse_down_box);
	}
#endif    /* OLD_NAVBAR */
	CORBA_exception_init(&ev);
	editor->engine =
	    (GNOME_GtkHTML_Editor_Engine)
	    Bonobo_Unknown_queryInterface(bonobo_widget_get_objref
					(BONOBO_WIDGET(editor->control)),
					"IDL:GNOME/GtkHTML/Editor/Engine:1.0",
					&ev);


	servant = ORBit_small_get_servant(editor->engine);
	if (servant) {
		impl = bonobo_object(servant);
		if (impl) {
			editor->html_widget =
			    g_object_get_data(G_OBJECT(impl),
					      "html-widget");
		}
	}
	GNOME_GtkHTML_Editor_Engine_runCommand(editor->engine,
					       "grab-focus", &ev);
	CORBA_exception_free(&ev);

	bonobo_widget_set_property(BONOBO_WIDGET(editor->control),
				   "MagicSmileys", TC_CORBA_boolean,
				   TRUE, NULL);
	bonobo_widget_set_property(BONOBO_WIDGET(editor->control), "MagicLinks",
				   TC_CORBA_boolean, 
				   TRUE, NULL);
	bonobo_widget_set_property(BONOBO_WIDGET(editor->control),
				   "InlineSpelling", TC_CORBA_boolean,
				   TRUE, NULL);
	return win;
#endif
}


static
gint _create_new(const gchar * filename, const gchar * key, gint editor_type)
{
	gchar *title = NULL;
	EDITOR *editor;
	
#ifdef USE_GTKHTML3_14_23 /* remove when note support is added */
	if(editor_type != STUDYPAD_EDITOR)
		return 0;
#endif
	
	editor = g_new(EDITOR, 1);
	editor->html_widget = NULL;
#ifdef USE_GTKHTML3_14_23
	
	
#else
	editor->persist_file_interface = CORBA_OBJECT_NIL;
	editor->persist_stream_interface = CORBA_OBJECT_NIL;
#endif	
	editor->sync = FALSE;
	editor->type = editor_type;
	
	switch(editor_type) {
		case STUDYPAD_EDITOR:	
			editor->studypad = TRUE;
			editor->module = NULL;
			editor->key = NULL;
			editor->filename = NULL;
#ifdef USE_GTKHTML3_14_23	
			widgets.studypad_dialog = 
				editor_new(_("StudyPad"), editor);
#else
			widgets.studypad_dialog =
				container_create(_("StudyPad"), editor);
#endif
			if (filename) {
				editor->filename = g_strdup(filename);
				_load_file(editor, g_strdup(filename));
			}
			settings.studypad_dialog_exist = TRUE;
		break;
	case NOTE_EDITOR:
			editor->studypad = FALSE;
			editor->filename = NULL;
			editor->module = g_strdup(filename);
			editor->key = g_strdup(key);
			editor->navbar.key = NULL;
#ifdef USE_GTKHTML3_14_23
			editor_new(_("Note Editor"), editor);
	
	
#else
			container_create(_("Note Editor"), editor);
			editor_load_note(editor, NULL, NULL);
#endif
		break;
	case BOOK_EDITOR:
			editor->studypad = FALSE;
			editor->filename = NULL;
			editor->module = g_strdup(filename);
			editor->key = g_strdup(key);
#ifdef USE_GTKHTML3_14_23
			editor_new(_("Prayer List Editor"), editor);
	
	
#else
			container_create(_("Prayer List Editor"), editor);
			editor_load_book(editor);
#endif
			
		break;
	
	}
	if (editor->html_widget) {
#ifdef USE_GTKHTML3_14_23
	
	
#else
		g_signal_connect(editor->html_widget,
				 "key_release_event",
				 G_CALLBACK(on_key_release_event),
				 (EDITOR *) editor);
#endif
	/*	g_signal_connect(editor->html_widget, "size-changed",
				 G_CALLBACK(size_changed),
				 (EDITOR *) editor);*/
	}
	editor->is_changed = FALSE;		
	editors_all = g_list_append(editors_all,(EDITOR*) editor);
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
 
gint editor_create_new(const gchar * filename, const gchar * key, gint editor_type)
{
	GList *tmp = NULL;
	EDITOR *e;
	
	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		e = (EDITOR*)tmp->data;
		switch(editor_type) {
			case STUDYPAD_EDITOR:
				if (e->studypad) {
					if (editor_is_dirty(e))
#ifdef USE_GTKHTML3_14_23
						
#else
						save_through_persist_file(e, 
								g_strdup(e->filename));

#endif
					if (e->filename)
						g_free(e->filename);
					e->filename = g_strdup(filename);
					gtk_widget_show(e->window);
					gdk_window_raise(GTK_WIDGET(e->window)->window);
#ifdef USE_GTKHTML3_14_23
						
#else
					_load_file(e, g_strdup(filename));
#endif
					return 1;
				}
			break;
			case NOTE_EDITOR:
			if (!e->studypad) {
				if (editor_is_dirty(e))
#ifdef USE_GTKHTML3_14_23	
#else
					save_through_persist_stream_cb(NULL, 
							(EDITOR*)e);
#endif
				if (e->module)
					g_free(e->module);
				e->module = g_strdup(filename);
				if (e->key)
					g_free(e->key);
				e->key = g_strdup(key);
				gtk_widget_show(e->window);
				gdk_window_raise(GTK_WIDGET(e->window)->window);
				
#ifdef USE_GTKHTML3_14_23
						
#else
				editor_load_note(e, NULL, NULL);
#endif
				return 1;
			}
			case BOOK_EDITOR:
				if (!e->studypad) {
					if (editor_is_dirty(e))
#ifdef USE_GTKHTML3_14_23
						
#else
						save_through_persist_stream_cb(NULL, 
								(EDITOR*)e);
#endif
					if (e->module)
						g_free(e->module);
					e->module = g_strdup(filename);
					if (e->key)
						g_free(e->key);
					e->key = g_strdup(key);
					gtk_widget_show(e->window);
					gdk_window_raise(GTK_WIDGET(e->window)->window);
					
#ifdef USE_GTKHTML3_14_23
						
#else
					editor_load_book(e);
#endif
					return 1;
				}
			break;
		}
		tmp = g_list_next(tmp);
	}
	return _create_new(filename, key, editor_type);
}
/*
gint load_file(EDITOR * e)
{
#ifdef USE_GTKHTML3_14_23
	return FALSE;
#else
	GS_message(("loading: %s\n", e->filename));
	_load_file(e, g_strdup(e->filename));
	return FALSE;
#endif
}
*/
