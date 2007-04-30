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

#ifdef OLD_NAVBAR
#include "main/navbar.h"
#else
#include "gui/navbar_versekey_editor.h"
#endif

#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "gui/dialog.h"
#include "gui/html.h"
#include "gui/widgets.h"
#include "gui/gnomesword.h"
#include "gui/utilities.h"


#define CONTROL_ID         "OAFIID:GNOME_GtkHTML_Editor:" GTKHTML_API_VERSION

extern gboolean do_display;

static 
gboolean save_through_persist_file(EDITOR * e, const gchar * filename);
//static 


static GList *editors_all = NULL;
static gint formatHTML = 1;
static GtkWidget *win;
static GtkHTML *html;



gboolean editor_is_dirty(EDITOR * e)
{

	CORBA_Environment ev;
	gboolean rv;

	CORBA_exception_init(&ev);
/*	rv = e->is_changed
	    || (GNOME_GtkHTML_Editor_Engine_hasUndo(e->engine, &ev) &&
		!GNOME_GtkHTML_Editor_Engine_runCommand(e->engine,
							"is-saved",
							&ev)); */
	rv = GNOME_GtkHTML_Editor_Engine_hasUndo(e->engine, &ev);
	
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
		info->label2 =  _("It will be lost permanently!");
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
	text = ncr_to_utf8(buf);
	g_free(buf);
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

#ifdef DEBUG	
	g_message(filename);
#endif

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
	GNOME_GtkHTML_Editor_Engine_dropUndo(e->engine, &ev);
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
			buf = _("File");
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
	g_message("do_exit");
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
	g_free(e);
	CORBA_exception_free(&ev);
}


gboolean editor_close_all(void)
{
	GList *tmp = NULL;
	EDITOR *e = NULL;
	
	tmp = g_list_first(editors_all);
#ifdef DEBUG
	g_message("number of editors = %d",g_list_length(tmp));
#endif
	while (tmp != NULL) {
		e = (EDITOR*)tmp->data;
		if (e->window)
			do_exit(e);			
		tmp = g_list_next(tmp);
	}
	g_list_free(editors_all);
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
	text = main_get_raw_text((gchar *) e->module,
				   (gchar *) e->key);
	if(strlen(text)) 
		load_through_persist_stream(text, e);
	else
		load_through_persist_stream("", e); /* clear editor if no text */
	
	change_window_title(e->window, title);
#ifdef OLD_NAVBAR
	main_navbar_set(e->navbar, e->key);
#else	
	main_navbar_versekey_set(e->navbar, e->key);
#endif
	if (text)
		g_free(text);
	if (title)
		g_free(title);
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

static void sync_toggled(GtkToggleButton * button, EDITOR * e)
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
		if (!e->studypad) 
			sync_toggled(GTK_TOGGLE_BUTTON(e->sync_button), e);
		tmp = g_list_next(tmp);
	}
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
			 "toggled", G_CALLBACK(sync_toggled), editor);

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
	GtkWindow *window;
	GtkWidget *vbox;
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
	if (!editor->studypad) {
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
	}

	window = GTK_WINDOW(win);
	editor->window = win;
	container = bonobo_window_get_ui_container(BONOBO_WINDOW(win));

	g_signal_connect(window, "delete-event",
			 G_CALLBACK(app_delete_cb), (EDITOR *) editor);

	gtk_window_set_default_size(window, 600, 440);
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
#ifdef DEBUG
	g_message(CONTROL_ID);
#endif
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
	gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(editor->control), TRUE, TRUE, 0);
	
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
gint _create_new(const gchar * filename, const gchar * key, gint note)
{
	gchar *title = NULL;
	EDITOR *editor;
	
	editor = g_new(EDITOR, 1);
	editor->html_widget = NULL;
	editor->persist_file_interface = CORBA_OBJECT_NIL;
	editor->persist_stream_interface = CORBA_OBJECT_NIL;
	editor->sync = FALSE;
	
	if (note) {
		editor->studypad = FALSE;
		editor->filename = NULL;
		editor->module = g_strdup(filename);
		editor->key = g_strdup(key);
		editor->navbar.key = NULL;
		container_create(_("Note Editor"), editor);
		editor_load_note(editor, NULL, NULL);
	} else {
		editor->studypad = TRUE;
		editor->module = NULL;
		editor->key = NULL;
		editor->filename = NULL;
		widgets.studypad_dialog =
		    container_create(_("StudyPad"), editor);
		if (filename) {
			editor->filename = g_strdup(filename);
			load_through_persist_file(editor, g_strdup(filename));
		}
		settings.studypad_dialog_exist = TRUE;
	}
	if (editor->html_widget) {
		g_signal_connect(editor->html_widget,
				 "key_release_event",
				 G_CALLBACK(on_key_release_event),
				 (EDITOR *) editor);
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

gint editor_create_new(const gchar * filename, const gchar * key, gint note)
{
	GList *tmp = NULL;
	EDITOR *e;
	
	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		e = (EDITOR*)tmp->data;
		if (note && !e->studypad) {
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
		if (!note && e->studypad) {
			if (editor_is_dirty(e))
				save_through_persist_file(e, 
						g_strdup(e->filename));
				
			if (e->filename)
				g_free(e->filename);
			e->filename = g_strdup(filename);
			gtk_widget_show(e->window);
			gdk_window_raise(GTK_WIDGET(e->window)->window);
			load_through_persist_file(e, g_strdup(filename));
			return 1;
		}
		tmp = g_list_next(tmp);
	}
	return _create_new(filename, key, note);
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
