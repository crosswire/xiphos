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
#include <gtkhtml/gtkhtml.h>
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
#include "editor/editor-control.h"
#include "editor/popup.h"
#include "editor/spell.h"
#include "editor/toolbar_style.h"
#include "editor/toolbar_edit.h"
#include "editor/editor_menu.h"
//#include "editor/htmlsourceview.h"


#include "main/settings.h"


#include "gui/html.h"
#include "gui/dialog.h"
#include "gui/fileselection.h"
#include "gui/widgets.h"
#include "gui/gnomesword.h"

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
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			if (settings.studypadfilename) {
				filename =
				    g_strdup(settings.studypadfilename);
				save_file(filename, ecd);
			} else {
				gui_fileselection_save(ecd, TRUE);
			}
		}
		settings.modifiedSP = FALSE;
		g_free(info);
		g_string_free(str, TRUE);
	}
	return FALSE;
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
	cd->editbar = settings.show_edit_bar_sp;

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

GSHTMLEditorControlData *editor_new_editor(GtkWidget * container,
					   int type, char *filename)
{
	if (!container)
		return NULL;
	return new_control(container, type, filename);
}
