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


#ifdef USE_GTKHTML38

#include <gnome.h>
#include <bonobo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>
#include <Editor.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/gtkhtml-properties.h>

#include "editor/html-editor.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"


#include "gui/dialog.h"
#include "gui/html.h"
#include "gui/widgets.h"
#include "gui/gnomesword.h"
#include "gui/utilities.h"



//#define CONTROL_FACTORY_ID "OAFIID:GNOME_GtkHTML_Editor_Factory:" GTKHTML_API_VERSION
#define CONTROL_ID         "OAFIID:GNOME_GtkHTML_Editor:" GTKHTML_API_VERSION

struct _editor {
	GtkWidget *window;
	GtkWidget *toolbar;
	GtkWidget *html_widget;
	GtkWidget *statusbar;

	GNOME_GtkHTML_Editor_Engine engine;
	Bonobo_PersistFile persist_file_interface;
	Bonobo_PersistStream persist_stream_interface;

	gboolean studypad;
	gboolean is_changed;

	gchar *filename;
	gchar *module;
	gchar *key;
};

static GtkWidget *control;
static gint formatHTML = 1;
static GtkWidget *win;
static GtkHTML *html;
static BonoboWindow *app;

/* Saving/loading through PersistStream.  */


gboolean editor_is_dirty(EDITOR * e)
{

	CORBA_Environment ev;
	gboolean rv;

	CORBA_exception_init(&ev);
	rv = e->is_changed
	    || (GNOME_GtkHTML_Editor_Engine_hasUndo(e->engine, &ev) &&
		!GNOME_GtkHTML_Editor_Engine_runCommand(e->engine,
							"is-saved",
							&ev));
	CORBA_exception_free(&ev);

	return rv;
}



static
void change_window_title(GtkWidget * window, const gchar * window_title)
{
	gtk_window_set_title(GTK_WINDOW(window), window_title);
}

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


void
editor_load_note(EDITOR * e, const gchar * module_name,
		 const gchar * key)
{
	gchar *title = g_strdup_printf("%s - %s", module_name, key);
	gchar *text =
	    main_get_rendered_text((gchar *) module_name,
				   (gchar *) key);

	load_through_persist_stream(text, e);
	change_window_title(e->window, title);
#ifdef DEBUG
	g_message(text);
#endif
	if (text)
		g_free(text);
	if (title)
		g_free(title);
}

static void
delete_note_cb(GtkWidget * widget, gpointer data)
{	
	gint test;
	GS_DIALOG *info;
	gchar *buf = NULL;
	EDITOR *e = (EDITOR *) data;
	
	if(e->studypad)
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
		}
		g_free(info);
		g_free(buf);
}

static void
save_through_persist_stream_cb(GtkWidget * widget, gpointer data)
{
	BonoboObject *smem;
	CORBA_Environment ev;
	const char *text;
	size_t len;
	EDITOR *ed = (EDITOR *) data;

	CORBA_exception_init(&ev);
	smem = bonobo_stream_mem_create(NULL, 1, FALSE, TRUE);
	Bonobo_PersistStream_save(ed->persist_stream_interface,
				  BONOBO_OBJREF(smem), "text/html",
				  &ev);
	bonobo_stream_client_write(BONOBO_OBJREF(smem), "", 1, &ev);

	text = bonobo_stream_mem_get_buffer(BONOBO_STREAM_MEM(smem));
#ifdef DEBUG
	g_message(text);
#endif
	main_save_note(ed->module, ed->key, text);
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
#ifdef DEBUG
	g_message(text);
#endif
	//main_save_note(ed->module, ed->key, text);
	GNOME_GtkHTML_Editor_Engine_dropUndo(ed->engine, &ev);
	ed->is_changed = FALSE;
	bonobo_object_unref(BONOBO_OBJECT(smem));
	CORBA_exception_free(&ev);	
}

/* Loading/saving through PersistFile.  */

static void
load_through_persist_file(EDITOR * e, const gchar * filename)
{
	CORBA_Environment ev;

	CORBA_exception_init(&ev);

	Bonobo_PersistFile_load(e->persist_file_interface, filename, &ev);

	if (ev._major != CORBA_NO_EXCEPTION)
		g_warning("Cannot load.");
	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(filename);

	xml_set_value("GnomeSword", "studypad", "lastfile",
		      e->filename);
	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	change_window_title(e->window, e->filename);
	CORBA_exception_free(&ev);
	g_free((gchar *) filename);
	e->is_changed = FALSE;
}

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
	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(filename);
	xml_set_value("GnomeSword", "studypad", "lastfile",
		      e->filename);
	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	change_window_title(e->window, filename);
	e->is_changed = FALSE;
	g_free((gchar *) filename);

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
		load_through_persist_file(e, g_strdup(fname));
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

	control =
	    BONOBO_WIDGET(bonobo_window_get_contents
			  (BONOBO_WINDOW(e->window)));

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
				     GTK_WINDOW(app));

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

/* "Save through persist stream" dialog.  */
/*static void
save_through_persist_stream_cb (GtkWidget *widget,
				gpointer data)
{
	open_or_save_as_dialog (BONOBO_WINDOW (data), 
					      OP_SAVE_THROUGH_PERSIST_STREAM);
}
*/
/* "Save through persist stream" dialog.  */
/*
static void
save_through_plain_persist_stream_cb (GtkWidget *widget,
				gpointer data)
{
	open_or_save_as_dialog (BONOBO_WINDOW (data), 
					OP_SAVE_THROUGH_PLAIN_PERSIST_STREAM);
}
*/

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
		load_through_persist_file(e, g_strdup(e->filename));
		g_free(e->filename);
		e->filename = NULL;
#ifdef DEBUG
		g_message(template);
#endif
	}
	g_free(template);
	e->is_changed = TRUE;
}


/* "Open through persist file" dialog.  */
static void
open_through_persist_file_cb(GtkWidget * widget, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
#ifdef DEBUG
	g_message("open_through_persist_file_cb");
#endif
	open_or_save_as_dialog(e, OP_LOAD_THROUGH_PERSIST_FILE);
	e->is_changed = FALSE;
}

/* "Save through persist file" dialog.  */
static void
save_through_persist_file_cb(GtkWidget * widget, gpointer data)
{
#ifdef DEBUG
	g_message("save_through_persist_file_cb");
#endif
	open_or_save_as_dialog((EDITOR *) data,
			       OP_SAVE_THROUGH_PERSIST_FILE);
}

static
gint ask_about_saving(EDITOR * e)
{
	CORBA_Object interface;
	CORBA_Environment ev;
	gint test;
	GS_DIALOG *info;
	gchar *buf = NULL;
	gchar *buf1 = NULL;
	gchar *buf2 = NULL;
	gchar *buf3 = NULL;
	gint retval = FALSE;

	if (!e->studypad) {
		save_through_persist_stream_cb(NULL, e);
		retval = GS_YES;
	} else {
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		if (settings.studypadfilename)
			buf = settings.studypadfilename;
		else
			buf = N_("File");
		buf1 = N_("Save the changes to document");
		buf2 = N_("before closing?");
		buf3 =
		    g_strdup_printf
		    ("<span weight=\"bold\" size=\"larger\">%s %s %s</span>",
		     buf1, buf, buf2);
		info->label_top = buf3;
		info->label2 =
		    N_
		    ("If you don't save, changes will be permanently lost.");
		info->save = TRUE;
		info->cancel = TRUE;
		info->no_save = TRUE;

		test = gui_alert_dialog(info);
		retval = test;
		if (test == GS_YES) {
			if (e->filename) {
				save_through_persist_file(e,
						g_strdup(e->filename));
			} else {
				open_or_save_as_dialog(e,
					     OP_SAVE_THROUGH_PERSIST_FILE);
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

#ifdef DEBUG
	g_message("app_delete_cb");
	g_message(e->filename);
	g_message(e->module);
	g_message(e->key);
#endif
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
	gtk_widget_destroy(GTK_WIDGET(e->window));
	g_free(e);
	CORBA_exception_free(&ev);
}


static void exit_cb(GtkWidget * widget, gpointer data)
{
	if (editor_is_dirty((EDITOR *) data)) {
		switch (ask_about_saving((EDITOR *) data)) {
		case GS_YES:	/* exit saving */

			break;
		case GS_NO:	/* exit without saving */

			break;
		case GS_CANCEL:	/* don't exit */
			return;
			break;
		}
	}
	do_exit((EDITOR *) data);
}

static
void print_message_cb(GtkWidget * widget, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
	gui_html_print(e->html_widget, FALSE);
}

static
void print_preview_message_cb(GtkWidget * widget, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
	gui_html_print(e->html_widget, TRUE);
}


static BonoboUIVerb verbs[] = {
	BONOBO_UI_UNSAFE_VERB("OpenFile", open_through_persist_file_cb),
	BONOBO_UI_UNSAFE_VERB("SaveFile", save_through_persist_file_cb),
	BONOBO_UI_UNSAFE_VERB("OpenStream",open_through_persist_stream_cb),
	BONOBO_UI_UNSAFE_VERB("SaveNote",save_through_persist_stream_cb),
	BONOBO_UI_UNSAFE_VERB("DeleteNote",delete_note_cb),
	BONOBO_UI_UNSAFE_VERB("NewDoc", open_new_document_cb),
	BONOBO_UI_UNSAFE_VERB("SavePlainStream",
			      save_through_plain_persist_stream_cb),
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
	do_exit((EDITOR *) data);
	return FALSE;
}

static void size_changed(GtkHTML * html, gpointer data)
{
	EDITOR *e = (EDITOR *) data;
#ifdef DEBUG
	g_message("size_changed");
#endif
	e->is_changed = TRUE;
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


static GtkWidget *container_create(const gchar * window_title,
				   EDITOR * editor)
{
	GtkWindow *window;
	GtkWidget *vbox;
	BonoboUIComponent *component;
	BonoboUIContainer *container;
	CORBA_Environment ev;
	GNOME_GtkHTML_Editor_Engine engine;
	gpointer servant;
	BonoboObject *impl;


	win = bonobo_window_new("Editor", window_title);
	vbox = gtk_vbox_new(FALSE, 6);

	app = BONOBO_WINDOW(win);
	window = GTK_WINDOW(win);
	editor->window = win;
	container = bonobo_window_get_ui_container(BONOBO_WINDOW(win));

	g_signal_connect(window, "delete-event",
			 G_CALLBACK(app_delete_cb), (EDITOR *) editor);

	gtk_window_set_default_size(window, 600, 440);
	gtk_window_set_resizable(window, TRUE);

	component = bonobo_ui_component_new("Editor");
	//bonobo_running_context_auto_exit_unref (BONOBO_OBJECT (component));   
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

	control =
	    bonobo_widget_new_control(CONTROL_ID,
				      BONOBO_OBJREF(container));
#ifdef DEBUG
	g_message(CONTROL_ID);
#endif
	if (control == NULL)
		g_error("Cannot get `%s'.", CONTROL_ID);

	bonobo_widget_set_property(BONOBO_WIDGET(control), "FormatHTML",
				   TC_CORBA_boolean, formatHTML, NULL);
	bonobo_ui_component_set_prop(component, "/commands/FormatHTML",
				     "state", formatHTML ? "1" : "0",
				     NULL);
	bonobo_ui_component_add_listener(component, "FormatHTML",
					 menu_format_html_cb, control);

	bonobo_window_set_contents(BONOBO_WINDOW(win), control);

	/* FIXME: handle exceptions */
	CORBA_exception_init(&ev);
	editor->persist_file_interface
	    =
	    Bonobo_Unknown_queryInterface(bonobo_widget_get_objref
					  (BONOBO_WIDGET(control)),
					  "IDL:Bonobo/PersistFile:1.0",
					  &ev);
	editor->persist_stream_interface =
	    Bonobo_Unknown_queryInterface(bonobo_widget_get_objref
					  (BONOBO_WIDGET(control)),
					  "IDL:Bonobo/PersistStream:1.0",
					  &ev);
	CORBA_exception_free(&ev);

	gtk_widget_show_all(GTK_WIDGET(window));

	CORBA_exception_init(&ev);
	editor->engine =
	    (GNOME_GtkHTML_Editor_Engine)
	    Bonobo_Unknown_queryInterface(bonobo_widget_get_objref
					(BONOBO_WIDGET(control)),
					"IDL:GNOME/GtkHTML/Editor/Engine:1.0",
					&ev);


	servant = ORBit_small_get_servant(editor->engine);
	if (servant) {
		impl = bonobo_object(servant);
		if (impl) {
			g_message("impl");
			editor->html_widget =
			    g_object_get_data(G_OBJECT(impl),
					      "html-widget");
		}
	}
	if (editor->html_widget) {
		g_signal_connect(editor->html_widget, "size-changed",
				 G_CALLBACK(size_changed),
				 (EDITOR *) editor);
		g_signal_connect(editor->html_widget,
				 "key_release_event",
				 G_CALLBACK(on_key_release_event),
				 (EDITOR *) editor);
#ifdef DEBUG
		g_message("we have the html_widget!!!");
#endif
	}
	GNOME_GtkHTML_Editor_Engine_runCommand(editor->engine,
					       "grab-focus", &ev);
	CORBA_exception_free(&ev);

	bonobo_widget_set_property(BONOBO_WIDGET(control),
				   "MagicSmileys", TC_CORBA_boolean,
				   TRUE, NULL);
	bonobo_widget_set_property(BONOBO_WIDGET(control), "MagicLinks",
				   TC_CORBA_boolean, 
				   TRUE, NULL);
	bonobo_widget_set_property(BONOBO_WIDGET(control),
				   "InlineSpelling", TC_CORBA_boolean,
				   TRUE, NULL);
	return win;
}

gint
editor_create_new(const gchar * filename, const gchar * key, gint note)
{
	gchar *title = NULL;
	EDITOR *editor;

	editor = g_new(EDITOR, 1);
	editor->html_widget = NULL;
	editor->persist_file_interface = CORBA_OBJECT_NIL;
	editor->persist_stream_interface = CORBA_OBJECT_NIL;

	if (note) {
		editor->studypad = FALSE;
		editor->filename = NULL;
		editor->module = g_strdup(filename);
		editor->key = g_strdup(key);
		container_create(_("Note Editor"), editor);
		editor_load_note(editor, filename, key);
	} else {
		editor->studypad = TRUE;
		editor->module = NULL;
		editor->key = NULL;
		editor->filename = NULL;
		widgets.studypad_dialog =
		    container_create(_("StudyPad"), editor);
		if (filename)
#ifdef DEBUG
			g_message("filename = %s", filename);
#endif
		editor->filename = g_strdup(filename);
		load_through_persist_file(editor, g_strdup(filename));
		settings.studypad_dialog_exist = TRUE;
	}
	editor->is_changed = FALSE;
}

gint load_file(EDITOR * e)
{
#ifdef DEBUG
	g_message("loading: %s\n", e->filename);
#endif
	load_through_persist_file(e, g_strdup(e->filename));
	return FALSE;
}
#endif				/* USE_GTKHTML38 */
