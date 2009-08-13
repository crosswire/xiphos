/*
 * Xiphos Bible Study Tool
 * html-editor.c - the html editor
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

#include <config.h>
#include <libgnome/gnome-i18n.h>

#ifndef USE_GTKHTML3_14_23

#include <gnome.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>

#include <gtkhtml/gtkhtml.h>
#include <bonobo.h>
#include <Editor.h>

#include "editor/bonobo-editor.h"

#include "gui/navbar_versekey_editor.h"
#include "gui/bookmark_dialog.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/sword_treekey.h"
#include "main/xml.h"

#include "gui/dialog.h"
#include "gui/html.h"
#include "gui/widgets.h"
#include "gui/xiphos.h"
#include "gui/treekey-editor.h"
#include "gui/utilities.h"

#include "gui/debug_glib_null.h"


#define CONTROL_ID         "OAFIID:GNOME_GtkHTML_Editor:" GTKHTML_API_VERSION

extern gboolean do_display;

static gboolean editor_is_dirty(EDITOR *e);

static 
gboolean save_through_persist_file(EDITOR * e, const gchar * filename);

static 
int app_delete_cb(GtkWidget * widget, GdkEvent * event, gpointer data);


static void
_load_file (EDITOR * e, const gchar * filename);

static
gint ask_about_saving(EDITOR * e);


static GList *editors_all = NULL;
static gint formatHTML = 1;
static GtkWidget *win;

 
#ifdef USE_GTKHTML3_14_23
static void
handle_error (GError **error)
{
	if (*error != NULL) {
		g_warning ("%s", (*error)->message);
		g_clear_error (error);
	}
}
#endif

static
void do_exit(EDITOR * e)
{
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

	GS_message(("do_exit"));
	GS_message((e->filename ? e->filename : "no filename"));
	GS_message((e->module ? e->module : "no module"));
	GS_message((e->key ? e->key : "no key"));

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
	if(e->window)	
		gtk_widget_destroy(e->window);
		//GS_message(("e->widow still exist!"));
	g_free(e);
}

static
void change_window_title(GtkWidget * window, const gchar * window_title)
{
	gtk_window_set_title(GTK_WINDOW(window), window_title);
}


#ifdef USE_GTKHTML3_14_23
static gint
_calc_header_height (GtkHTML *html, GtkPrintOperation *operation,
                         GtkPrintContext *context)
{
	PangoContext *pango_context;
	PangoFontDescription *desc;
	PangoFontMetrics *metrics;
	gint header_height;

	pango_context = gtk_print_context_create_pango_context (context);
	desc = pango_font_description_from_string ("Sans Regular 10");

	metrics = pango_context_get_metrics (
		pango_context, desc, pango_language_get_default ());
	header_height =
		pango_font_metrics_get_ascent (metrics) +
		pango_font_metrics_get_descent (metrics);
	pango_font_metrics_unref (metrics);

	pango_font_description_free (desc);
	g_object_unref (pango_context);

	return header_height;
}

static gint
_calc_footer_height (GtkHTML *html, GtkPrintOperation *operation,
                         GtkPrintContext *context)
{
	PangoContext *pango_context;
	PangoFontDescription *desc;
	PangoFontMetrics *metrics;
	gint footer_height;

	pango_context = gtk_print_context_create_pango_context (context);
	desc = pango_font_description_from_string ("Sans Regular 10");

	metrics = pango_context_get_metrics (
		pango_context, desc, pango_language_get_default ());
	footer_height =
		pango_font_metrics_get_ascent (metrics) +
		pango_font_metrics_get_descent (metrics);
	pango_font_metrics_unref (metrics);

	pango_font_description_free (desc);
	g_object_unref (pango_context);

	return footer_height;
}
static void
_draw_header (GtkHTML *html, GtkPrintOperation *operation,
                  GtkPrintContext *context,
                  gint page_nr,
                  PangoRectangle *rec,
		  EDITOR * e)
{
	PangoFontDescription *desc;
	PangoLayout *layout;
	gdouble x, y;
	gchar *text;
	cairo_t *cr;

	text = g_strdup (e->filename);

	desc = pango_font_description_from_string ("Sans Regular 10");
	layout = gtk_print_context_create_pango_layout (context);
	pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
	pango_layout_set_font_description (layout, desc);
	pango_layout_set_text (layout, text, -1);
	pango_layout_set_width (layout, rec->width);

	x = pango_units_to_double (rec->x);
	y = pango_units_to_double (rec->y);

	cr = gtk_print_context_get_cairo_context (context);

	cairo_save (cr);
	cairo_set_source_rgb (cr, .0, .0, .0);
	cairo_move_to (cr, x, y);
	pango_cairo_show_layout (cr, layout);
	cairo_restore (cr);

	g_object_unref (layout);
	pango_font_description_free (desc);

	g_free (text);
}


static void
_draw_footer (GtkHTML *html, GtkPrintOperation *operation,
                  GtkPrintContext *context,
                  gint page_nr,
                  PangoRectangle *rec,
		  EDITOR * e)
{
	PangoFontDescription *desc;
	PangoLayout *layout;
	gdouble x, y;
	gint n_pages;
	gchar *text;
	cairo_t *cr;

	g_object_get (operation, "n-pages", &n_pages, NULL);
	text = g_strdup_printf (_("Page %d of %d"), page_nr + 1, n_pages);

	desc = pango_font_description_from_string ("Sans Regular 10");
	layout = gtk_print_context_create_pango_layout (context);
	pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
	pango_layout_set_font_description (layout, desc);
	pango_layout_set_text (layout, text, -1);
	pango_layout_set_width (layout, rec->width);

	x = pango_units_to_double (rec->x);
	y = pango_units_to_double (rec->y);

	cr = gtk_print_context_get_cairo_context (context);

	cairo_save (cr);
	cairo_set_source_rgb (cr, .0, .0, .0);
	cairo_move_to (cr, x, y);
	pango_cairo_show_layout (cr, layout);
	cairo_restore (cr);

	g_object_unref (layout);
	pango_font_description_free (desc);

	g_free (text);
}

static GtkPrintOperationResult
_do_print (EDITOR * e,
       GtkPrintOperationAction action)
{
	GtkPrintOperation *operation;
	GtkPrintSettings *psettings;
	GtkPageSetup *setup;
	GtkPrintOperationResult result;
	GError *error = NULL;

	operation = gtk_print_operation_new ();	
	psettings = gtk_print_settings_new ();
	
	psettings = gtk_print_operation_get_print_settings (operation);	
	
	setup = gtk_page_setup_new ();
	gtk_page_setup_set_top_margin (setup,30,GTK_UNIT_PIXEL);
	gtk_page_setup_set_left_margin (setup,50,GTK_UNIT_PIXEL);
	
#ifdef WIN32
	gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS);
#endif	
	gtk_print_operation_set_default_page_setup (operation, setup);
	
	result = gtk_html_print_operation_run (GTK_HTML(e->html_widget), 
					       operation, 
					       action,
					       GTK_WINDOW (e->window), 
					       (GtkHTMLPrintCalcHeight) _calc_header_height, /* GtkHTMLPrintCalcHeight  calc_header_height*/
					       (GtkHTMLPrintCalcHeight) _calc_footer_height, /* GtkHTMLPrintCalcHeight  calc_footer_height*/ 
					       (GtkHTMLPrintDrawFunc) _draw_header, /* GtkHTMLPrintDrawFunc draw_header */ 
					       (GtkHTMLPrintDrawFunc) _draw_footer, /* GtkHTMLPrintDrawFunc draw_footer */
					       e, /* gpointer user_data */ 
					       &error);

	g_object_unref (operation);
	handle_error (&error);

	return result;
}
#endif


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
	gchar *buf;
	EDITOR *e = (EDITOR *) data;
	
	if (e->studypad)
		return;
	
	buf = g_strdup_printf
	    ("<span weight=\"bold\" size=\"larger\">%s %s?</span>",
	    _("Are you sure you want to delete the note for") , e->key);
	/* info->label2 =  N_("It will be lost permanently!"); */
	if (gui_yes_no_dialog(buf, GTK_STOCK_DIALOG_WARNING)) {
		main_delete_note(e->module, e->key);
		load_through_persist_stream("", e);
	}
	g_free(buf);
}

static void
save_through_persist_stream_cb(GtkWidget * widget, gpointer data)
{
	BonoboObject *smem;
	CORBA_Environment ev;
	const gchar *text;
	gchar *buf;
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
		xml_set_value("Xiphos", "studypad", "lastfile",
			      e->filename);
		settings.studypadfilename =
		    xml_get_value("studypad", "lastfile");
		change_window_title(e->window, filename);
	}
	e->is_changed = FALSE;
	//g_free((gchar *) filename);
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

static void
open_or_save_as_dialog(EDITOR * e, FileSelectionOperation op)
{
	
	GtkWidget *dialog;
	gchar *studypad_dir;
	
	studypad_dir = g_strdup_printf("%s/", settings.studypaddir);
	if(op == OP_SAVE_THROUGH_PERSIST_FILE) {		
		dialog = gtk_file_chooser_dialog_new("Save File As",
						NULL,
						GTK_FILE_CHOOSER_ACTION_SAVE,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_SAVE_AS, GTK_RESPONSE_ACCEPT,
						NULL);
		
	} else {
		dialog = gtk_file_chooser_dialog_new("Open File",
						NULL,
						GTK_FILE_CHOOSER_ACTION_OPEN,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						NULL);
	}
	gtk_file_chooser_set_current_folder((GtkFileChooser*)dialog, studypad_dir);
	
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		GS_message(("filename: %s",filename));
		if(op == OP_SAVE_THROUGH_PERSIST_FILE) 
			save_through_persist_file(e, filename);
		else
			_load_file (e, filename);
		
		g_free (filename);
	}
	
	//this actually causes a segfault; sorry! :FIXED 
	gtk_widget_destroy (dialog);	
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
	}
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
#ifdef USE_GTKHTML3_14_23
	_do_print(e, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG);
#else
	gui_html_print(e->html_widget, FALSE, "");
#endif
}

static
void print_preview_message_cb(GtkWidget * widget, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
#ifdef USE_GTKHTML3_14_23
	_do_print(e, GTK_PRINT_OPERATION_ACTION_PREVIEW);
#else
	gui_html_print(e->html_widget, TRUE, "");
#endif
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


static void
_load_file (EDITOR * e, const gchar * filename)
{
	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(filename);

	GS_message(("_load_file filename: %s",filename));
	
	xml_set_value("Xiphos", "studypad", "lastfile",
		      e->filename);
	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	change_window_title(e->window, e->filename);
	

	CORBA_Environment ev;

	CORBA_exception_init(&ev);


	Bonobo_PersistFile_load(e->persist_file_interface, filename, &ev);

	if (ev._major != CORBA_NO_EXCEPTION)
		g_warning("Cannot load.");
	GNOME_GtkHTML_Editor_Engine_dropUndo(e->engine, &ev);
	CORBA_exception_free(&ev);
	e->is_changed = FALSE;
}

gboolean editor_is_dirty(EDITOR * e)
{
	CORBA_Environment ev;
	gboolean rv;

	CORBA_exception_init(&ev);
	
	rv = GNOME_GtkHTML_Editor_Engine_hasUndo(e->engine, &ev);
	
	CORBA_exception_free(&ev);
	return rv;
}


void editor_save_book(EDITOR * e)
{
	if (editor_is_dirty(e)) {
		GS_message(("editor_is_dirty"));
		save_through_persist_stream_cb(NULL, e);
	}
}

/* save if needed is done in treeky-editor.c before calling editor_load_book() */
void editor_load_book(EDITOR * e)
{
	gchar *title = NULL;
	gchar *text = NULL;	

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
	if (button->active) {
		if (editor_is_dirty(e))
			save_through_persist_stream_cb(NULL, (EDITOR*)e);
		editor_load_note(e, NULL,settings.currentverse);
		e->sync = TRUE;
	}
	else		
		e->sync = FALSE;
}


void editor_sync_with_main(void)
{
	GList *tmp = NULL;
	EDITOR * e;
	
	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		e = (EDITOR*)tmp->data;
		switch(e->type) {
			case STUDYPAD_EDITOR:
			case BOOK_EDITOR:
				break;			
			case NOTE_EDITOR:
			editor_sync_toggled(GTK_TOGGLE_BUTTON(e->navbar.button_sync), e);
			break;
		}
		tmp = g_list_next(tmp);
	}
}

void
editor_load_note(EDITOR * e, const gchar * module_name,
		 const gchar * key)
{
	gchar *title;
	gchar *text;

	
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
		main_navbar_versekey_set(e->navbar, e->key);
	} 
	
	if (text)
		g_free(text);
	if (title)
		g_free(title);
}


gboolean editor_close_all(void)
{
	GList *tmp = NULL;
	EDITOR *e = NULL;
	tmp = g_list_first(editors_all);
	GS_message(("number of editors = %d",g_list_length(tmp)));
	while (tmp != NULL) {
		e = (EDITOR*)tmp->data;
		if (e->window)
			do_exit(e);			
		tmp = g_list_next(tmp);
	}
	g_list_free(editors_all);
        return 1;
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


static
gint ask_about_saving(EDITOR * e)
{
	
	gint test;
	GS_DIALOG *info;
	gchar *buf = NULL;
	gchar *buf1 = NULL;
	gchar *buf2 = NULL;
	gchar *buf3 = NULL;
	gint retval = FALSE;	

	switch(e->type) {
		case BOOK_EDITOR:
		case NOTE_EDITOR:
			info = gui_new_dialog();
			info->stock_icon = GTK_STOCK_DIALOG_WARNING;
			
			buf = g_strdup_printf("%s: %s",e->module, e->key);
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
				if (e->type == NOTE_EDITOR)
					save_through_persist_stream_cb(NULL, e);
				else 
					save_through_persist_stream_cb(NULL, e);
			}
			g_free(info);
			g_free(buf3);
		break;
		
		case STUDYPAD_EDITOR:
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
				if (e->filename) 
					save_through_persist_file(e,
							g_strdup(e->filename));
				else 
					open_or_save_as_dialog(e,
						     OP_SAVE_THROUGH_PERSIST_FILE);
			}
			g_free(info);
			g_free(buf3);
			break;
	}
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
	return retval;
}


static GtkWidget *container_create(const gchar * window_title,
				   EDITOR * editor)
{
	GtkWindow *window;
	GtkWidget *vbox;
	GtkWidget *paned;
	GtkWidget *scrollbar;
	GtkWidget *toolbar_nav = NULL;
	BonoboUIComponent *component;
	BonoboUIContainer *container;
	CORBA_Environment ev;
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
			toolbar_nav = gui_navbar_versekey_editor_new(editor);
			gtk_widget_show(toolbar_nav);
			gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(toolbar_nav), FALSE, TRUE, 0);

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
			gtk_tree_view_collapse_all((GtkTreeView *)editor->treeview);
			// then we should expand on the item to which we've opened for edit.
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
}


static
gint _create_new(const gchar * filename, const gchar * key, gint editor_type)
{
	EDITOR *editor;
	
	
	editor = g_new(EDITOR, 1);
	editor->html_widget = NULL;
	
	editor->persist_file_interface = CORBA_OBJECT_NIL;
	editor->persist_stream_interface = CORBA_OBJECT_NIL;
	
	editor->sync = FALSE;
	editor->type = editor_type;
	
	switch(editor_type) {
		case STUDYPAD_EDITOR:	
			editor->studypad = TRUE;
			editor->module = NULL;
			editor->key = NULL;
			editor->filename = NULL;
			widgets.studypad_dialog =
				container_create(_("StudyPad"), editor);
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
			container_create(_("Note Editor"), editor);
			
			editor_load_note(editor, NULL, NULL);
		break;
	case BOOK_EDITOR:
			editor->studypad = FALSE;
			editor->filename = NULL;
			editor->module = g_strdup(filename);
			editor->key = g_strdup(key);
			container_create(_("Prayer List/Journal Editor"), editor);
			
			editor_load_book(editor);
			
		break;
	
	}
	if (editor->html_widget) {
		g_signal_connect(editor->html_widget,
				 "key_release_event",
				 G_CALLBACK(on_key_release_event),
				 (EDITOR *) editor);
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
						save_through_persist_file(e, 
								g_strdup(e->filename));
					if (e->filename)
						g_free(e->filename);
					e->filename = g_strdup(filename);
					gtk_widget_show(e->window);
					gdk_window_raise(GTK_WIDGET(e->window)->window);
					
					_load_file(e, g_strdup(filename));
					return 1;
				}
			break;
			case NOTE_EDITOR:
			if (!e->studypad) {
				if (editor_is_dirty(e))
					save_through_persist_stream_cb(NULL, 
							(EDITOR*)e);
				
				if (e->module)
					g_free(e->module);
				e->module = g_strdup(filename);
				if (e->key)
					g_free(e->key);
				e->key = g_strdup(key);
				gtk_widget_show(e->window);
				gdk_window_raise(GTK_WIDGET(e->window)->window);
				
				editor_load_note(e, NULL, NULL);
				return 1;
			}
			case BOOK_EDITOR:
				if (!e->studypad) {
					if (editor_is_dirty(e))
						save_through_persist_stream_cb(NULL, 
								(EDITOR*)e);
					if (e->module)
						g_free(e->module);
					e->module = g_strdup(filename);
					if (e->key)
						g_free(e->key);
					e->key = g_strdup(key);
					gtk_widget_show(e->window);
					gdk_window_raise(GTK_WIDGET(e->window)->window);
					
					editor_load_book(e);
					
					return 1;
				}
			break;
		}
		tmp = g_list_next(tmp);
	}
    	GS_message (("filename %s, key %s",filename, key));
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

		app_delete_cb(NULL, NULL, (EDITOR*)tmp->data);
		tmp = tmp2;
	}
}

#endif /* USE_GTKHTML3_14_23 */
