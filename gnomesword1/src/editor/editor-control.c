/* 

    Copyright (C) 2000 Helix Code, Inc.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <config.h>
#include <libgnome/gnome-i18n.h>

#include <gnome.h>
#include <bonobo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>
#include <Editor.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/gtkhtml-properties.h>

#ifndef USE_GTKHTML38
#include "gtkhtml/gtkhtml-properties.h"
#include "gtkhtml/htmlcursor.h"
#include "gtkhtml/htmlengine.h"
#include "gtkhtml/htmlengine-edit.h"
#include "gtkhtml/htmlengine-edit-cut-and-paste.h"
#include "gtkhtml/htmlengine-edit-movement.h"
#include "gtkhtml/htmlengine-edit-selection-updater.h"
#include "gtkhtml/htmlimage.h"
#include "gtkhtml/htmlinterval.h"
#include "gtkhtml/htmlselection.h"
#include "gtkhtml/htmlfontmanager.h"
#include "gtkhtml/htmlsettings.h"
#include "gtkhtml/htmlpainter.h"
#include "gtkhtml/htmlplainpainter.h"

#include "editor/editor.h"
#include "editor/popup.h"
#include "editor/spell.h"
#include "editor/toolbar_style.h"
#include "editor/toolbar_edit.h"
#include "editor/editor_menu.h"

#include "gui/fileselection.h"
#endif


#ifndef USE_GTKHTML38

#endif

#include "editor/editor-control.h"

#include "main/settings.h"
#include "main/sword.h"


#include "gui/dialog.h"
#include "gui/html.h"
#include "gui/widgets.h"
#include "gui/gnomesword.h"



#ifdef USE_GTKHTML38

#define CONTROL_FACTORY_ID "OAFIID:GNOME_GtkHTML_Editor_Factory:" GTKHTML_API_VERSION
#define CONTROL_ID         "OAFIID:GNOME_GtkHTML_Editor:" GTKHTML_API_VERSION

struct _editor {
	GtkWidget *window;
	GtkWidget *toolbar;
	GtkWidget *html_widget;
	GtkWidget *statusbar;
	
	GNOME_GtkHTML_Editor_Engine engine;
	Bonobo_PersistFile       persist_file_interface;
	Bonobo_PersistStream     persist_stream_interface;
	
	gboolean studypad;
	gboolean is_changed;
	
	gchar *filename;
	gchar *module;
	gchar *key;
};

static GtkWidget *control;
static gint formatHTML = 1;
static GtkWidget *win;
static	GtkHTML *html;
static	BonoboWindow *app;

/* Saving/loading through PersistStream.  */


gboolean
editor_is_dirty (EDITOR *e)
{
	
	CORBA_Environment ev;
	gboolean rv;
	
	CORBA_exception_init (&ev);
	rv = e->is_changed
		|| (GNOME_GtkHTML_Editor_Engine_hasUndo (e->engine, &ev) &&
		    !GNOME_GtkHTML_Editor_Engine_runCommand (e->engine, "is-saved", &ev));
	CORBA_exception_free (&ev);

	return rv;
}



static
void change_window_title(GtkWidget * window, const gchar * window_title)
{	
	gtk_window_set_title(GTK_WINDOW(window), window_title);
}

static void
load_through_persist_stream (const gchar *text,
			     EDITOR * e)
{
	BonoboObject *smem;
	BonoboWidget *control;
	CORBA_Object interface;
	CORBA_Environment ev;	
	
	CORBA_exception_init (&ev);
	
	control = BONOBO_WIDGET(bonobo_window_get_contents(app));
	
	smem = bonobo_stream_mem_create (text, strlen(text), FALSE, TRUE);
	CORBA_exception_init (&ev);
	interface = Bonobo_Unknown_queryInterface (bonobo_widget_get_objref (control),
						   "IDL:Bonobo/PersistStream:1.0", &ev);
	if (BONOBO_EX (&ev) || interface == CORBA_OBJECT_NIL) {
		g_warning ("Couldn't find persist stream interface");
		return;
	}
	Bonobo_PersistStream_load (interface, BONOBO_OBJREF (smem), "text/html", &ev);
		
	bonobo_object_unref (BONOBO_OBJECT (smem));
	CORBA_exception_free (&ev);
}


void
editor_load_note(EDITOR * e, const gchar * module_name, const gchar * key)
{
	gchar *title = g_strdup_printf("%s - %s",module_name,key);
	gchar *text = main_get_rendered_text((gchar *)module_name, (gchar *)key);
	
	load_through_persist_stream (text, e);
	change_window_title(e->window,title);
#ifdef DEBUG
	g_message(text);
#endif
	if(text)
		g_free(text);
	if(title)
		g_free(title);
}


static void
save_through_persist_stream_cb (GtkWidget * widget,
			     gpointer data)
{
	BonoboObject *smem;
	BonoboWidget *control;
	CORBA_Environment ev;
	CORBA_Object interface;
	const char *text;
	size_t len;
	EDITOR *ed = (EDITOR*) data;
	CORBA_exception_init (&ev);
	control = BONOBO_WIDGET(bonobo_window_get_contents(BONOBO_WINDOW(ed->window))); //BONOBO_WINDOW(ed->window)));
	interface = Bonobo_Unknown_queryInterface (bonobo_widget_get_objref (control),
						   "IDL:Bonobo/PersistStream:1.0", &ev);
	if (BONOBO_EX (&ev) || interface == CORBA_OBJECT_NIL) {
		g_warning ("Couldn't find persist stream interface");
		return;
	}
	smem = bonobo_stream_mem_create (NULL, 1, FALSE, TRUE);
	Bonobo_PersistStream_save (interface, BONOBO_OBJREF (smem), "text/html", &ev);
	/* Terminate the buffer for e_text_to_html */
	bonobo_stream_client_write (BONOBO_OBJREF (smem), "", 1, &ev);

        text = bonobo_stream_mem_get_buffer (BONOBO_STREAM_MEM (smem));
	len  = bonobo_stream_mem_get_size (BONOBO_STREAM_MEM (smem));
#ifdef DEBUG
	g_message(text);
#endif
	main_save_note(ed->module, ed->key, text);
	GNOME_GtkHTML_Editor_Engine_dropUndo(ed->engine, &ev);
	bonobo_object_unref (BONOBO_OBJECT (smem));
	CORBA_exception_free (&ev);
}

static void
save_through_plain_persist_stream_cb (const gchar *filename,
			     Bonobo_PersistStream pstream)
{
	BonoboObject *stream = NULL;
	CORBA_Environment ev;

	CORBA_exception_init (&ev);

	/* FIX2 stream = bonobo_stream_open (BONOBO_IO_DRIVER_FS, filename,
				     Bonobo_Storage_WRITE |
				     Bonobo_Storage_CREATE |
				     Bonobo_Storage_FAILIFEXIST, 0); */

	if (stream == NULL) {
		g_warning ("Couldn't create `%s'\n", filename);
	} else {
		Bonobo_Stream corba_stream;

		corba_stream = bonobo_object_corba_objref (stream);
		Bonobo_Stream_truncate (corba_stream, 0, &ev);
		Bonobo_PersistStream_save (pstream, corba_stream, "text/plain", &ev);
	}

	Bonobo_Unknown_unref (pstream, &ev);
	CORBA_Object_release (pstream, &ev);

	CORBA_exception_free (&ev);
}


/* Loading/saving through PersistFile.  */

static void
load_through_persist_file (EDITOR * e, const gchar *filename,
			   Bonobo_PersistFile pfile)
{
	CORBA_Environment ev;

	CORBA_exception_init (&ev);
	
	
	Bonobo_PersistFile_load (pfile, filename, &ev);

	if (ev._major != CORBA_NO_EXCEPTION)
		g_warning ("Cannot load.");
	if(e->filename)
		g_free(e->filename);
	e->filename = g_strdup(filename);
	change_window_title(e->window,filename);
	CORBA_exception_free (&ev);
}

static gboolean
save_through_persist_file (EDITOR * e, const gchar *filename,
			   Bonobo_PersistFile pfile)
{
	CORBA_Environment ev;

	CORBA_exception_init (&ev);

	Bonobo_PersistFile_save (pfile, filename, &ev);

	if (ev._major != CORBA_NO_EXCEPTION) {
		g_warning ("Cannot save.");		
		CORBA_exception_free (&ev);
		return FALSE;
	}
	if(e->filename)
		g_free(e->filename);
	e->filename = g_strdup(filename);
	change_window_title(e->window,filename);

	CORBA_exception_free (&ev);
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

static void
file_selection_destroy_cb (GtkWidget *widget,
			   gpointer data)
{
	file_selection_info.widget = NULL;
}

static void
view_source_dialog (BonoboWindow *app, char *type, gboolean as_html)
{
	//load_through_persist_stream ("<b>bold</b><br>not so bold",NULL);
}    

static void
view_html_source_cb (GtkWidget *widget,
		     gpointer data)
{
	view_source_dialog (data, "text/html", FALSE);
}

static void
view_plain_source_cb (GtkWidget *widget,
		      gpointer data)
{
	view_source_dialog (data, "text/plain", FALSE);
}

static void
view_html_source_html_cb (GtkWidget *widget,
			  gpointer data)
{
	view_source_dialog (data, "text/html", TRUE);
}

static void
file_selection_cancel_cb (GtkWidget *widget, gpointer data)
{
	gtk_widget_destroy (GTK_WIDGET (data));
}

static void
file_selection_ok_cb (GtkWidget *widget,
		      gpointer data)
{
	CORBA_Object interface;
	const gchar *interface_name;
	CORBA_Environment ev;

	if (file_selection_info.operation == OP_SAVE_THROUGH_PERSIST_FILE
	    || file_selection_info.operation == OP_LOAD_THROUGH_PERSIST_FILE)
		interface_name = "IDL:Bonobo/PersistFile:1.0";
	else
		interface_name = "IDL:Bonobo/PersistStream:1.0";

	CORBA_exception_init (&ev);
	interface = Bonobo_Unknown_queryInterface (bonobo_widget_get_objref (file_selection_info.control),
						   interface_name, &ev);
	CORBA_exception_free (&ev);
	
	if (interface == CORBA_OBJECT_NIL) {
		g_warning ("The Control does not seem to support `%s'.", interface_name);
	} else 	 {
		const gchar *fname;

		fname = gtk_file_selection_get_filename
			(GTK_FILE_SELECTION (file_selection_info.widget));

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
			load_through_persist_file (data, fname, interface);
			break;
		case OP_SAVE_THROUGH_PERSIST_FILE:
			save_through_persist_file (data, fname, interface);
			break;
		default:
			g_assert_not_reached ();
		}
	} 

	gtk_widget_destroy (file_selection_info.widget);
}


static void
open_or_save_as_dialog (EDITOR * e,
			FileSelectionOperation op)
{
	GtkWidget    *widget;
	BonoboWidget *control;
	gchar *directory = NULL;

	control = BONOBO_WIDGET (bonobo_window_get_contents (BONOBO_WINDOW(e->window)));
	
	if (file_selection_info.widget != NULL) {
		gdk_window_show (GTK_WIDGET (file_selection_info.widget)->window);
		return;
	}

	if (op == OP_LOAD_THROUGH_PERSIST_FILE || op == OP_LOAD_THROUGH_PERSIST_STREAM) {
		widget = gtk_file_selection_new (_("Open file..."));
		if(settings.studypaddir) {
			directory = g_strdup_printf("%s/", settings.studypaddir);
			gtk_file_selection_set_filename ((GtkFileSelection *)widget, directory);
			g_free(directory);
		}
	} else {
		widget = gtk_file_selection_new (_("Save file as..."));
		if(e->filename)
			gtk_file_selection_set_filename ((GtkFileSelection *)widget, e->filename);
		else {
			directory = g_strdup_printf("%s/", settings.studypaddir);
			gtk_file_selection_set_filename ((GtkFileSelection *)widget, directory);
			g_free(directory);
		}
	}
	gtk_window_set_transient_for (GTK_WINDOW (widget),
				      GTK_WINDOW (app));

	file_selection_info.widget = widget;
	file_selection_info.control = control;
	file_selection_info.operation = op;

	g_signal_connect (GTK_FILE_SELECTION (widget)->cancel_button, "clicked", G_CALLBACK (file_selection_cancel_cb), widget);
	g_signal_connect (GTK_FILE_SELECTION (widget)->ok_button, "clicked", G_CALLBACK (file_selection_ok_cb), (EDITOR*)e);
	g_signal_connect (file_selection_info.widget, "destroy", G_CALLBACK (file_selection_destroy_cb), NULL);

	gtk_widget_show (file_selection_info.widget);
}

/* "Open through persist stream" dialog.  */
static void
open_through_persist_stream_cb (GtkWidget *widget,
				gpointer data)
{
	open_or_save_as_dialog ((EDITOR*)data, OP_LOAD_THROUGH_PERSIST_STREAM);
}

/* "Save through persist stream" dialog.  */
/*static void
save_through_persist_stream_cb (GtkWidget *widget,
				gpointer data)
{
	open_or_save_as_dialog (BONOBO_WINDOW (data), OP_SAVE_THROUGH_PERSIST_STREAM);
}
*/
/* "Save through persist stream" dialog.  */
/*
static void
save_through_plain_persist_stream_cb (GtkWidget *widget,
				gpointer data)
{
	open_or_save_as_dialog (BONOBO_WINDOW (data), OP_SAVE_THROUGH_PLAIN_PERSIST_STREAM);
}
*/

static void
open_new_document_cb (GtkWidget *widget,
			      gpointer data)
{
	gchar *template = NULL;
	EDITOR *e = (EDITOR*)data;
	
	template = g_strdup_printf("%s/template.pad",settings.gSwordDir);
	if(g_file_test(template, G_FILE_TEST_EXISTS)) {
		if(e->filename)
			g_free(e->filename);
		e->filename = g_strdup(template);
		load_through_persist_file (data, e->filename, e->persist_file_interface);
		g_free(e->filename);
		e->filename = NULL;
#ifdef DEBUG
		g_message(template);
#endif		
	}
	g_free(template);
}


/* "Open through persist file" dialog.  */
static void
open_through_persist_file_cb (GtkWidget *widget,
			      gpointer data)
{
#ifdef DEBUG
	g_message("open_through_persist_file_cb");
#endif
	
	open_or_save_as_dialog ((EDITOR*)data, OP_LOAD_THROUGH_PERSIST_FILE);
}

/* "Save through persist file" dialog.  */
static void
save_through_persist_file_cb (GtkWidget *widget,
			      gpointer data)
{
#ifdef DEBUG
	g_message("save_through_persist_file_cb");
#endif
	open_or_save_as_dialog ((EDITOR*)data, OP_SAVE_THROUGH_PERSIST_FILE);
}

static
gint ask_about_saving(EDITOR *e)
{
	CORBA_Object interface;
	CORBA_Environment ev;
	gint test;
	GS_DIALOG *info;
	GString *str;
	gchar *buf = NULL;
		
	if(!e->studypad)
		save_through_persist_stream_cb (NULL,e);
	else {
		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		if (settings.studypadfilename)
			buf = settings.studypadfilename;
		else
			buf = N_("File");
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			buf,
			_("has been modified. Do you wish to save it?"));
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			if (e->filename) {
				save_through_persist_file (e, 
						e->filename, 
						e->persist_file_interface);
				
			} else {
				open_or_save_as_dialog(e, 
						OP_SAVE_THROUGH_PERSIST_FILE);
			}
		}
		g_free(info);
		g_string_free(str,TRUE);
		
		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
	}	
}


static
void do_exit(EDITOR *e)
{
	CORBA_Environment ev;
	
	CORBA_exception_init (&ev);
	
	if (e->engine != CORBA_OBJECT_NIL) {
		Bonobo_Unknown_unref (e->engine, &ev);
		CORBA_Object_release (e->engine, &ev);
		e->engine = CORBA_OBJECT_NIL;
	}
	
	if (e->persist_stream_interface != CORBA_OBJECT_NIL) {
		Bonobo_Unknown_unref (e->persist_stream_interface, &ev);
		CORBA_Object_release (e->persist_stream_interface, &ev);
		e->persist_stream_interface = CORBA_OBJECT_NIL;
	}
	
	
	if (e->persist_file_interface != CORBA_OBJECT_NIL) {
		Bonobo_Unknown_unref (e->persist_file_interface, &ev);
		CORBA_Object_release (e->persist_file_interface, &ev);
		e->persist_file_interface = CORBA_OBJECT_NIL;
	}
	
	
#ifdef DEBUG
	g_message("app_delete_cb");
	g_message(e->filename);
	g_message(e->module);
	g_message(e->key);
#endif
	if(e->studypad)
		settings.studypad_dialog_exist = FALSE;
	if(e->filename) {
		g_free(e->filename);
	}
	if(e->module) {
		g_free(e->module);
	}
	if(e->key) {
		g_free(e->key);
	}
	gtk_widget_destroy (GTK_WIDGET (e->window));
	g_free(e);
	CORBA_exception_free (&ev);
}


static void
exit_cb (GtkWidget *widget, gpointer data)
{
	do_exit((EDITOR*)data);
}


static BonoboUIVerb verbs [] = {
	BONOBO_UI_UNSAFE_VERB ("OpenFile",   open_through_persist_file_cb),
	BONOBO_UI_UNSAFE_VERB ("SaveFile",   save_through_persist_file_cb),
	BONOBO_UI_UNSAFE_VERB ("OpenStream", open_through_persist_stream_cb),
	BONOBO_UI_UNSAFE_VERB ("SaveNote", save_through_persist_stream_cb),
	BONOBO_UI_UNSAFE_VERB ("NewDoc", open_new_document_cb),
	BONOBO_UI_UNSAFE_VERB ("SavePlainStream",save_through_plain_persist_stream_cb),
	BONOBO_UI_UNSAFE_VERB ("ViewHTMLSource", view_html_source_cb),
	BONOBO_UI_UNSAFE_VERB ("ViewHTMLSourceHTML", view_html_source_html_cb),
	BONOBO_UI_UNSAFE_VERB ("ViewPlainSource", view_plain_source_cb),
	BONOBO_UI_UNSAFE_VERB ("FileExit", exit_cb),

	BONOBO_UI_VERB_END
};

static void
menu_format_html_cb (BonoboUIComponent           *component,
		     const char                  *path,
		     Bonobo_UIComponent_EventType type,
		     const char                  *state,
		     gpointer                     user_data)

{
	if (type != Bonobo_UIComponent_STATE_CHANGED)
		return;
	formatHTML = *state == '0' ? 0 : 1;

	bonobo_widget_set_property (BONOBO_WIDGET (user_data), "FormatHTML", TC_CORBA_boolean, formatHTML, NULL);
	bonobo_widget_set_property (BONOBO_WIDGET (user_data), "HTMLTitle", TC_CORBA_string, "testing", NULL);
}
 
/* A dirty, non-translatable hack */
static char ui_studypad [] = 
"<Root>"
"	<commands>"
"	        <cmd name=\"FileExit\" _label=\"Exit\" _tip=\"Exit the program\""
"	         pixtype=\"stock\" pixname=\"Exit\" accel=\"*Control*q\"/>"
"	        <cmd name=\"FormatHTML\" _label=\"HTML mode\" type=\"toggle\" _tip=\"HTML Format switch\"/>"
"	</commands>"
"	<menu>"
"		<submenu name=\"File\" _label=\"_File\">"
"			<menuitem name=\"OpenFile\" verb=\"\" _label=\"Open (PersistFile)\" _tip=\"Open using the PersistFile interface\""
"			pixtype=\"stock\" pixname=\"Open\"/>"
"			<menuitem name=\"SaveFile\" verb=\"\" _label=\"Save (PersistFile)\" _tip=\"Save using the PersistFile interface\""
"			pixtype=\"stock\" pixname=\"Save\"/>"
"			<separator/>"
"			<menuitem name=\"NewDoc\" verb=\"\" _label=\"New Document\" _tip=\"Create a new document\""
"			pixtype=\"stock\" pixname=\"Save\"/>"
"			<separator/>"
"			<menuitem name=\"FileExit\" verb=\"\" _label=\"E_xit\"/>"
"		</submenu>"
"		<placeholder name=\"Component\"/>"
"		<submenu name=\"Format\" _label=\"For_mat\">"
"			<menuitem name=\"FormatHTML\" verb=\"\"/>"
"                       <separator/>"
"                       <placeholder name=\"FormatParagraph\"/>"
"               </submenu>"
"	</menu>"
"	<dockitem name=\"Toolbar\" behavior=\"exclusive\">"
"		<placeholder name=\"Editbar\"/>"

"	</dockitem>"
"</Root>";

/*"			<separator/>"
"			<menuitem name=\"OpenStream\" verb=\"\" _label=\"_Open (PersistStream)\" _tip=\"Open using the PersistStream interface\""
"			pixtype=\"stock\" pixname=\"Open\"/>"
"			<menuitem name=\"SaveNote\" verb=\"\" _label=\"_Save (Personal Comment)\" _tip=\"Save using the PersistStream interface\""
"			pixtype=\"stock\" pixname=\"Save\"/>"
"			<menuitem name=\"SavePlainStream\" verb=\"\" _label=\"Save _plain(PersistStream)\" _tip=\"Save using the PersistStream interface\""
"			pixtype=\"stock\" pixname=\"Save\"/>"
"			<separator/>"
"                       <menuitem name=\"ViewHTMLSource\" verb=\"\" _label=\"View HTML Source\" _tip=\"View the html source of the current document\"/>"
"                       <menuitem name=\"ViewHTMLSourceHTML\" verb=\"\" _label=\"View HTML Output\" _tip=\"View the html source of the current document as html\"/>"
"                       <menuitem name=\"ViewPlainSource\" verb=\"\" _label=\"View PLAIN Source\" _tip=\"View the plain text source of the current document\"/>"
*/

/* A dirty, non-translatable hack */
static char ui_note [] = 
"<Root>"
"	<commands>"
"	        <cmd name=\"FileExit\" _label=\"Exit\" _tip=\"Exit the program\""
"	         pixtype=\"stock\" pixname=\"Exit\" accel=\"*Control*q\"/>"
"	        <cmd name=\"FormatHTML\" _label=\"HTML mode\" type=\"toggle\" _tip=\"HTML Format switch\"/>"
"	</commands>"
"	<menu>"
"		<submenu name=\"File\" _label=\"_File\">"
"			<menuitem name=\"SaveNote\" verb=\"\" _label=\"_Save (Personal Comment)\" _tip=\"Save using the PersistStream interface\""
"			pixtype=\"stock\" pixname=\"Save\"/>"
"			<separator/>"
"			<menuitem name=\"FileExit\" verb=\"\" _label=\"E_xit\"/>"
"		</submenu>"
"		<placeholder name=\"Component\"/>"
"		<submenu name=\"Format\" _label=\"For_mat\">"
"			<menuitem name=\"FormatHTML\" verb=\"\"/>"
"                       <separator/>"
"                       <placeholder name=\"FormatParagraph\"/>"
"               </submenu>"
"	</menu>"
"	<dockitem name=\"Toolbar\" behavior=\"exclusive\">"
"		<placeholder name=\"Editbar\"/>"

"	</dockitem>"
"</Root>";

/*"			<separator/>"
"			<menuitem name=\"OpenStream\" verb=\"\" _label=\"_Open (PersistStream)\" _tip=\"Open using the PersistStream interface\""
"			pixtype=\"stock\" pixname=\"Open\"/>"
"			<menuitem name=\"SaveNote\" verb=\"\" _label=\"_Save (Personal Comment)\" _tip=\"Save using the PersistStream interface\""
"			pixtype=\"stock\" pixname=\"Save\"/>"
"			<menuitem name=\"SavePlainStream\" verb=\"\" _label=\"Save _plain(PersistStream)\" _tip=\"Save using the PersistStream interface\""
"			pixtype=\"stock\" pixname=\"Save\"/>"
"			<separator/>"
"                       <menuitem name=\"ViewHTMLSource\" verb=\"\" _label=\"View HTML Source\" _tip=\"View the html source of the current document\"/>"
"                       <menuitem name=\"ViewHTMLSourceHTML\" verb=\"\" _label=\"View HTML Output\" _tip=\"View the html source of the current document as html\"/>"
"                       <menuitem name=\"ViewPlainSource\" verb=\"\" _label=\"View PLAIN Source\" _tip=\"View the plain text source of the current document\"/>"
*/
static int
app_delete_cb (GtkWidget *widget, GdkEvent *event, gpointer data)
{	
	if(editor_is_dirty((EDITOR*)data)) {
		if(!ask_about_saving((EDITOR*)data))
			return TRUE;
	}		
	do_exit((EDITOR*)data);
	return FALSE;
}


static GtkWidget *
container_create (const gchar * window_title, EDITOR * editor)
{
	GtkWindow *window;
	GtkWidget *vbox;
	BonoboUIComponent *component;
	BonoboUIContainer *container;
	CORBA_Environment ev;
	GNOME_GtkHTML_Editor_Engine engine;
	gpointer servant;
	BonoboObject *impl;
	
	
	win = bonobo_window_new ("Editor", window_title);
	vbox = gtk_vbox_new(FALSE,6);
	
	app = BONOBO_WINDOW(win);
	window = GTK_WINDOW (win);
	editor->window = win;
	container = bonobo_window_get_ui_container (BONOBO_WINDOW (win));

	g_signal_connect (window, "delete-event", G_CALLBACK (app_delete_cb), (EDITOR*)editor);
	
	gtk_window_set_default_size (window, 600, 440);
	gtk_window_set_resizable (window, TRUE);

	component = bonobo_ui_component_new ("Editor");
	//bonobo_running_context_auto_exit_unref (BONOBO_OBJECT (component));	
	bonobo_ui_component_set_container (component, BONOBO_OBJREF (container), NULL);
	bonobo_ui_component_add_verb_list_with_data (component, verbs, editor);
	if(editor->studypad)
		bonobo_ui_component_set_translate (component, "/", ui_studypad, NULL);
	else
		bonobo_ui_component_set_translate (component, "/", ui_note, NULL);
		

	
	control = bonobo_widget_new_control (CONTROL_ID, BONOBO_OBJREF (container));
#ifdef DEBUG
	g_message(CONTROL_ID);
#endif
	if (control == NULL)
		g_error ("Cannot get `%s'.", CONTROL_ID);

	bonobo_widget_set_property (BONOBO_WIDGET (control), "FormatHTML", TC_CORBA_boolean, formatHTML, NULL);
	bonobo_ui_component_set_prop (component, "/commands/FormatHTML", "state", formatHTML ? "1" : "0", NULL);
	bonobo_ui_component_add_listener (component, "FormatHTML", menu_format_html_cb, control);


	//gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET(control), TRUE, TRUE, 0);
	
	
	bonobo_window_set_contents (BONOBO_WINDOW (win), control);

	/* FIXME: handle exceptions */
	CORBA_exception_init (&ev);
	editor->persist_file_interface
		= Bonobo_Unknown_queryInterface (bonobo_widget_get_objref (BONOBO_WIDGET (control)), "IDL:Bonobo/PersistFile:1.0", &ev);
	editor->persist_stream_interface
		= Bonobo_Unknown_queryInterface (bonobo_widget_get_objref (BONOBO_WIDGET (control)), "IDL:Bonobo/PersistStream:1.0", &ev);
	CORBA_exception_free (&ev);
	//gtk_widget_show(GTK_WIDGET (vbox));
	//gtk_box_pack_start (GTK_BOX (vbox), p->eeditor, TRUE, TRUE, 0);
	gtk_widget_show_all (GTK_WIDGET (window));

	CORBA_exception_init (&ev);
	editor->engine = (GNOME_GtkHTML_Editor_Engine) Bonobo_Unknown_queryInterface
		(bonobo_widget_get_objref (BONOBO_WIDGET (control)), "IDL:GNOME/GtkHTML/Editor/Engine:1.0", &ev);
	
	
	servant = ORBit_small_get_servant (editor->engine);
    	if (servant) {
		impl = bonobo_object (servant);
		if(impl) {
			g_message("impl");
			editor->html_widget = g_object_get_data (G_OBJECT(impl), "html-widget");
		}
	}
#ifdef DEBUG
	if (editor->html_widget) {
		g_message("we have the html_widget!!!");
	}
#endif
	GNOME_GtkHTML_Editor_Engine_runCommand (editor->engine, "grab-focus", &ev);
	//bonobo_object_release_unref (editor->engine, &ev);
	CORBA_exception_free (&ev);
/*
	editor->statusbar = gtk_statusbar_new();
	gtk_widget_show(editor->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox), editor->statusbar, TRUE, TRUE, 0);
*/	
	bonobo_widget_set_property (BONOBO_WIDGET (control),
				    "MagicSmileys", TC_CORBA_boolean, TRUE,
				    NULL);
	bonobo_widget_set_property (BONOBO_WIDGET (control),
				    "MagicLinks", TC_CORBA_boolean, TRUE,
				    NULL);
	bonobo_widget_set_property (BONOBO_WIDGET (control),
				   "InlineSpelling", TC_CORBA_boolean, TRUE,
				   NULL);
	return win;
}

gint 
editor_create_new(const gchar * filename, const gchar * key, gint note)
{
	gchar *title = NULL;
	EDITOR *editor;
	
	editor = g_new(EDITOR,1);
	editor->html_widget = NULL;
	editor->persist_file_interface   = CORBA_OBJECT_NIL;
	editor->persist_stream_interface = CORBA_OBJECT_NIL;
	//bonobo_ui_component_new_default
	if(note) {
		editor->studypad = FALSE;
		editor->filename = NULL;
		editor->module = g_strdup(filename);
		editor->key = g_strdup(key);
		container_create (_("Note Editor"),editor);
		editor_load_note(editor, filename, key);
	} else {
		editor->studypad = TRUE;
		editor->module = NULL;
		editor->key = NULL;
		editor->filename = NULL;
		widgets.studypad_dialog = container_create (_("StudyPad"),editor);
		if(filename)
			editor->filename = g_strdup(filename);
			load_file(editor);
		settings.studypad_dialog_exist = TRUE;	
	}
	editor->is_changed = FALSE;
}

gint
load_file (EDITOR * e)
{
	CORBA_Object interface;
	CORBA_Environment ev;

	BonoboWidget *control;

	control = BONOBO_WIDGET (bonobo_window_get_contents (BONOBO_WINDOW(e->window)));

	printf ("loading: %s\n", e->filename);
	CORBA_exception_init (&ev);
	interface = Bonobo_Unknown_queryInterface (bonobo_widget_get_objref (BONOBO_WIDGET (control)),
						   "IDL:Bonobo/PersistFile:1.0", &ev);
	CORBA_exception_free (&ev);
	load_through_persist_file (e, e->filename, interface);
	//change_window_title(e->window,filename);

	return FALSE;
}



#else
static GtkHTMLEditorAPI *editor_api;

GSHTMLEditorControlData *editor_cd;


static gboolean
editor_api_command(GtkHTML * html, GtkHTMLCommandType com_type,
		   gpointer data)
{
	GSHTMLEditorControlData *cd = (GSHTMLEditorControlData *) data;
	gboolean rv = TRUE;
	printf("editor_api_command\n");
	switch (com_type) {
	case GTK_HTML_COMMAND_POPUP_MENU:
		popup_show_at_cursor(cd);
		break;
	case GTK_HTML_COMMAND_PROPERTIES_DIALOG:
		property_dialog_show(cd);
		break;
	case GTK_HTML_COMMAND_TEXT_COLOR_APPLY:
		toolbar_apply_color(cd);
		break;
	default:
		rv = FALSE;
	}

	return rv;
}


static GValue *editor_api_event(GtkHTML * html,
				GtkHTMLEditorEventType event_type,
				GValue * args, gpointer data)
{
	GSHTMLEditorControlData *cd = (GSHTMLEditorControlData *) data;
	GValue *retval = NULL;
/*	if(args)
		if(G_VALUE_TYPE(args)== 64) {
			g_message("value type = %d",G_VALUE_TYPE(args));
			g_message("value type = %s",G_VALUE_TYPE_NAME(args)); 
			g_message("value  = %s",g_strdup_value_contents(args));
		}
*/
//      printf ("editor_api_event\n"); 

/*

	if (cd->editor_bonobo_engine) {
		GNOME_GnomeSword_Editor_Engine engine;
		GNOME_GnomeSword_Editor_Listener listener;
		CORBA_Environment ev;

		CORBA_exception_init (&ev);
		engine = bonobo_object_corba_objref (BONOBO_OBJECT (cd->editor_bonobo_engine));

		if (engine != CORBA_OBJECT_NIL
		    && (listener = GNOME_GnomeSword_Editor_Engine__get_listener (engine, &ev)) != CORBA_OBJECT_NIL) {

			switch (event_type) {
			case GTK_HTML_EDITOR_EVENT_COMMAND_BEFORE:
				retval = send_event_str (engine, listener, "command_before", args);
				break;
			case GTK_HTML_EDITOR_EVENT_COMMAND_AFTER:
				retval = send_event_str (engine, listener, "command_after", args);
				break;
			case GTK_HTML_EDITOR_EVENT_IMAGE_URL:
				retval = send_event_str (engine, listener, "image_url", args);
				break;
			case GTK_HTML_EDITOR_EVENT_DELETE:
				send_event_void (engine, listener, "delete");
				break;
			default:
				g_warning ("Unsupported event.\n");
			}
			CORBA_exception_free (&ev);
		}
	}
*/
	return retval;
}



static GtkWidget *editor_api_create_input_line(GtkHTML * html,
					       gpointer data)
{
	GSHTMLEditorControlData *cd = (GSHTMLEditorControlData *) data;
	GtkWidget *entry;

	entry = gtk_entry_new();
	gtk_box_pack_end(GTK_BOX(cd->vbox), entry, FALSE, FALSE, 0);
	gtk_widget_show(entry);

	return entry;
}

static void new_editor_api()
{
	editor_api = g_new(GtkHTMLEditorAPI, 1);

	editor_api->check_word = spell_check_word;
	editor_api->suggestion_request = spell_suggestion_request;
	editor_api->add_to_personal = spell_add_to_personal;
	editor_api->add_to_session = spell_add_to_session;
	editor_api->set_language = spell_set_language;
	editor_api->command = editor_api_command;
	editor_api->event = editor_api_event;
	editor_api->create_input_line = editor_api_create_input_line;
}

/******************************************************************************
 * Name
 *  release
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   gint release(GtkWidget * widget, GdkEventButton * event,
 *					GSHTMLEditorControlData * cd)	
 *
 * Description
 *    ?????
 *
 * Return value
 *   gint
 */

static gint release(GtkWidget * widget, GdkEventButton * event,
		    GSHTMLEditorControlData * cd)
{

	return FALSE;
}

static gint
html_button_pressed2(GtkWidget * html, GdkEventButton * event,
		     GSHTMLEditorControlData * cd)
{
	HTMLEngine *engine = cd->html->engine;
	guint offset;

	cd->obj =
	    html_engine_get_object_at(engine, event->x, event->y,
				      &offset, FALSE);
	switch (event->button) {
	case 1:
		if (event->type == GDK_2BUTTON_PRESS && cd->obj
		    && event->state & GDK_CONTROL_MASK) {
			cd->releaseId =
			    g_signal_connect(html,
					     "button_release_event",
					     G_CALLBACK(release), cd);
			return TRUE;
		}
		break;
	case 2:
		/* pass this for pasting */
		return FALSE;
	case 3:
		if (!html_engine_is_selection_active(engine)
		    || !html_engine_point_in_selection(engine, cd->obj,
						       offset)) {
			html_engine_disable_selection(engine);
			html_engine_jump_at(engine, event->x, event->y);
			gtk_html_update_styles(cd->html);
		}

		if (popup_show(cd, event)) {
			g_signal_stop_emission_by_name(html,
						       "button_press_event");
			return TRUE;
		}
		break;
	default:
		;
	}

	return FALSE;
}

static gint
html_button_pressed_after(GtkWidget * html, GdkEventButton * event,
			  GSHTMLEditorControlData * cd)
{
	HTMLEngine *e = cd->html->engine;
	HTMLObject *obj = e->cursor->object;

	if (event->button == 1 && event->type == GDK_BUTTON_PRESS && obj
	    && obj->parent && !html_engine_is_selection_active(e)) {
		if (html_object_is_text(obj)
		    && html_object_get_data(obj->parent,
					    "template_text")) {
			html_object_set_data_full(obj->parent,
						  "template_text", NULL,
						  NULL);
			html_cursor_jump_to_position(e->cursor, e,
						     e->cursor->
						     position -
						     e->cursor->offset);
			html_engine_set_mark(e);
			html_cursor_jump_to_position(e->cursor, e,
						     e->cursor->
						     position +
						     html_object_get_length
						     (obj));
			html_engine_select_interval(e,
						    html_interval_new_from_cursor
						    (e->mark,
						     e->cursor));
			/* printf ("delete template text\n"); */
			html_engine_delete(cd->html->engine);
		} else if (HTML_IS_IMAGE(obj)
			   && html_object_get_data(obj->parent,
						   "template_image"))
			property_dialog_show(cd);
	}
//typedef enum   _GtkHTMLEditPropertyType       GtkHTMLEditPropertyType;

	return FALSE;
}

static gboolean
html_show_popup(GtkWidget * html, GSHTMLEditorControlData * cd)
{
#ifdef DEBUG
	g_message("html_show_popup");
#endif
	popup_show_at_cursor(cd);

	return TRUE;
}

/******************************************************************************
 * Name
 *  html_key_pressed
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   gint html_key_pressed(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *   a key has been pressed - ecd->changed to true 
 *
 * Return value
 *   gint
 */

static gint html_key_pressed(GtkWidget * html, GdkEventKey * event,
			     GSHTMLEditorControlData * ecd)
{
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
	return FALSE;
}


static gboolean on_delete(GtkWidget * widget,
			  GdkEvent * event,
			  GSHTMLEditorControlData * ecd)
{
	gchar *filename = NULL;
	gchar *buf = NULL;
	gint test;
	GS_DIALOG *info;
	GString *str;
	gboolean retval = FALSE;
#ifdef DEBUG 
	g_message("on_delete");
#endif
	if (settings.modifiedSP) {
		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		if (settings.studypadfilename)
			buf = settings.studypadfilename;
		else
			buf = N_("File");
		g_string_printf(str,
				"<span weight=\"bold\">%s</span>\n\n%s",
				buf,
				_
				("has been modified. Do you wish to save it?"));
		info->label_top = str->str;

		test = gui_close_confirmation_dialog(info);
		switch(test) {
			case GS_YES:
				if (settings.studypadfilename) {
					filename =
					    g_strdup(settings.studypadfilename);
					save_file(filename, ecd);
				} else {
					gui_fileselection_save(ecd, TRUE);
				}
				retval = FALSE;
			break;
			case GS_NO:
				retval = FALSE;
			break;
			case GS_CANCEL:
				retval = TRUE;
			break;
		}
		
		settings.modifiedSP = FALSE;
		g_free(info);
		g_string_free(str, TRUE);
	}
	return retval;
}


static
GSHTMLEditorControlData *new_control(GtkWidget * container, int type,
				     char *filename)
{
	GtkWidget *toolbar_box;
	GtkWidget *frame34;
	GtkWidget *scrolledwindow17;
	GtkWidget *toolbar;
	GtkWidget *htmlwidget = gtk_html_new();
	GtkWidget *vboxSP = gtk_vbox_new(FALSE, 0);
	GSHTMLEditorControlData *cd =
	    editor_control_data_new(GTK_HTML(htmlwidget), vboxSP);

	switch (type) {
	case STUDYPAD:
		cd->personal_comments = FALSE;
		cd->studypad = TRUE;
		break;
	case PERCOM:
		cd->personal_comments = TRUE;
		cd->studypad = FALSE;
		break;
	}

#ifdef DEBUG
	g_message("gui_create_studypad_control");
#endif

//      cd->stylebar = settings.show_style_bar_sp;
	cd->editbar = TRUE; //settings.show_edit_bar_sp;

	gtk_widget_show(vboxSP);
	gtk_container_add(GTK_CONTAINER(container), vboxSP);

	g_signal_connect(GTK_OBJECT(container), "delete-event",
			 G_CALLBACK(on_delete), cd);

	toolbar_box = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(toolbar_box);
	gtk_box_pack_start(GTK_BOX(vboxSP), toolbar_box, FALSE, TRUE,
			   0);


	frame34 = gtk_frame_new(NULL);
	gtk_widget_show(frame34);
	gtk_box_pack_start(GTK_BOX(vboxSP), frame34, TRUE, TRUE, 0);

	scrolledwindow17 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow17);
	gtk_container_add(GTK_CONTAINER(frame34), scrolledwindow17);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow17),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow17,
					    settings.shadow_type);

	cd->htmlwidget = htmlwidget;
	gtk_widget_show(cd->htmlwidget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow17),
			  cd->htmlwidget);

	cd->statusbar = gtk_statusbar_new();
	gtk_widget_show(cd->statusbar);
	gtk_box_pack_start(GTK_BOX(vboxSP), cd->statusbar, FALSE,
			   TRUE, 0);

	cd->vbox = vboxSP;
//      cd->pm = gui_create_editor_popup(cd);
//      gnome_popup_menu_attach(cd->pm, cd->htmlwidget, NULL);


	g_signal_connect(G_OBJECT(cd->htmlwidget), "url_requested",
			 G_CALLBACK(url_requested), cd);

	g_signal_connect(G_OBJECT(cd->htmlwidget), "key_press_event",
			 G_CALLBACK(html_key_pressed), cd);
	g_signal_connect(G_OBJECT(cd->htmlwidget), "link_clicked",
			 G_CALLBACK(gui_link_clicked), NULL);
	g_signal_connect(G_OBJECT(cd->htmlwidget), "on_url",
			 G_CALLBACK(gui_url), NULL);
	g_signal_connect(G_OBJECT(cd->htmlwidget), "popup_menu",
			 G_CALLBACK(html_show_popup), cd);
	g_signal_connect(G_OBJECT(cd->htmlwidget), "button_press_event",
			 G_CALLBACK(html_button_pressed2), cd);
	g_signal_connect_after(G_OBJECT(cd->htmlwidget),
			       "button_press_event",
			       G_CALLBACK(html_button_pressed_after),
			       cd);

	/* create toolbars */

	widgets.toolbar_studypad = gui_toolbar_style(cd);
	gtk_widget_show(widgets.toolbar_studypad);
	gtk_box_pack_start(GTK_BOX(toolbar_box),
			   widgets.toolbar_studypad, FALSE, FALSE, 0);
	toolbar = gui_toolbar_edit(cd);
	gtk_widget_show(toolbar);
	gtk_box_pack_start(GTK_BOX(toolbar_box), toolbar, FALSE, FALSE,
			   0);

	/* load last file */
	if (filename)
		load_file(filename, cd);
	else
		gtk_html_load_from_string(cd->html, "  ", 2);

	editor_cd = cd;

	new_editor_api();
	gtk_html_set_editor_api(GTK_HTML(cd->html), editor_api, cd);
	/* GNOME Spell Dictionary */
	cd->dict = spell_new_dictionary();
	spell_set_language(cd->html, settings.spell_language, cd);
	gtk_html_set_editable(cd->html, TRUE);
	gtk_html_set_inline_spelling(cd->html, TRUE);
	return cd;
}

GSHTMLEditorControlData *editor_new(GtkWidget * container,
					   int type, char *filename)
{
	if (!container)
		return NULL;
	return new_control(container, type, filename);
}
#endif
