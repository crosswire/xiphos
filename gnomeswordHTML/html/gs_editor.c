/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*                                                gs_editor.c  2001-04-17

    Copyright 2001, Terry Biggs

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
#include <gnome.h>
#include <bonobo.h>
#include <stdio.h>
#include <glib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gal/widgets/e-scroll-frame.h>


#include <gtkhtml.h>
#include <gtkhtml-properties.h>
#include <htmlcursor.h>
#include <htmlengine.h>
#include <htmlengine-edit.h>
#include <htmlengine-edit-selection-updater.h>
#include <htmlselection.h>
#include <htmlfontmanager.h>
#include <htmlsettings.h>
#include <htmlpainter.h>
#include <htmlplainpainter.h>

//#include "engine.h"
//#include "menubar.h"
//#include "persist-file-impl.h"
//#include "persist-stream-impl.h"
#include "./src/gssword/gs_sword.h"
#include "gs_editor.h"
#include "control-data.h"
#include "popup.h"
#include "toolbar.h"
#include "filebar.h"
#include "insertbar.h"
#include "editbar.h"
#include "properties.h"
#include "text.h"
#include "paragraph.h"
#include "link.h"
#include "body.h"
#include "filebar.h"
//#include "spell.h"
//#include "resolver-progressive-impl.h"


//#include "editor-control-factory.h"
//#include "gtkhtmldebug.h"



GtkWidget *statusbar;
GtkWidget *notebookEDITOR;



/******************************************************************************
 * this code taken form GtkHTML
 ******************************************************************************/
static gint
release(GtkWidget * widget, GdkEventButton * event,
	GtkHTMLControlData * cd)
{
	HTMLEngine *e = cd->html->engine;
	GtkHTMLEditPropertyType start = GTK_HTML_EDIT_PROPERTY_BODY;
	gboolean run_dialog = FALSE;

	if (cd->obj) {
		switch (HTML_OBJECT_TYPE(cd->obj)) {
		case HTML_TYPE_IMAGE:
		case HTML_TYPE_LINKTEXT:
		case HTML_TYPE_TEXT:
		case HTML_TYPE_RULE:
			run_dialog = TRUE;
			break;
		default:
		}
		if (run_dialog) {
			cd->properties_dialog =
			    gtk_html_edit_properties_dialog_new(cd, FALSE,
								_
								("Properties"));

			html_cursor_jump_to(e->cursor, e, cd->obj, 0);
			html_engine_disable_selection(e);
			html_engine_set_mark(e);
			html_cursor_jump_to(e->cursor, e, cd->obj,
					    html_object_get_length
					    (cd->obj));
			html_engine_edit_selection_updater_update_now
			    (e->selection_updater);

			switch (HTML_OBJECT_TYPE(cd->obj)) {
			case HTML_TYPE_IMAGE:
				gtk_html_edit_properties_dialog_add_entry
				    (cd->properties_dialog,
				     GTK_HTML_EDIT_PROPERTY_IMAGE,
				     _("Image"), image_properties,
				     image_apply_cb, image_close_cb);
				start = GTK_HTML_EDIT_PROPERTY_IMAGE;
				break;
			case HTML_TYPE_LINKTEXT:
			case HTML_TYPE_TEXT:
				gtk_html_edit_properties_dialog_add_entry
				    (cd->properties_dialog,
				     GTK_HTML_EDIT_PROPERTY_TEXT,
				     _("Text"), text_properties,
				     text_apply_cb, text_close_cb);
				start =
				    (HTML_OBJECT_TYPE(cd->obj) ==
				     HTML_TYPE_TEXT) ?
				    GTK_HTML_EDIT_PROPERTY_TEXT :
				    GTK_HTML_EDIT_PROPERTY_LINK;
 
				break;
			case HTML_TYPE_RULE:
				gtk_html_edit_properties_dialog_add_entry
				    (cd->properties_dialog,
				     GTK_HTML_EDIT_PROPERTY_RULE,
				     _("Rule"), rule_properties,
				     rule_apply_cb, rule_close_cb);
				start = GTK_HTML_EDIT_PROPERTY_RULE;
				break;
			default:
			}
			gtk_html_edit_properties_dialog_add_entry
			    (cd->properties_dialog,
			     GTK_HTML_EDIT_PROPERTY_PARAGRAPH,
			     _("Paragraph"), paragraph_properties,
			     paragraph_apply_cb, paragraph_close_cb);
			gtk_html_edit_properties_dialog_add_entry
			    (cd->properties_dialog,
			     GTK_HTML_EDIT_PROPERTY_LINK, _("Link"),
			     link_properties, link_apply_cb,
			     link_close_cb);
			gtk_html_edit_properties_dialog_add_entry
			    (cd->properties_dialog,
			     GTK_HTML_EDIT_PROPERTY_BODY, _("Page"),
			     body_properties, body_apply_cb,
			     body_close_cb);
			gtk_html_edit_properties_dialog_show
			    (cd->properties_dialog);
			gtk_html_edit_properties_dialog_set_page
			    (cd->properties_dialog, start);
		}
	}
	gtk_signal_disconnect(GTK_OBJECT(widget), cd->releaseId);
	return FALSE;
}
/******************************************************************************
 * 
 ******************************************************************************/
static gint
html_key_pressed(GtkWidget * html, GdkEventButton * event,
		    GtkHTMLControlData * cd)
{
	extern gchar sbNoteEditorText[];
	
	if(cd->note_editor) cd->filename = sbNoteEditorText;
	updatestatusbar(cd);
	cd->html_modified = TRUE;
	updatestatusbar(cd);
	return 1;
}


/******************************************************************************
 * this code taken form GtkHTML
 ******************************************************************************/
static gint
html_button_pressed(GtkWidget * html, GdkEventButton * event,
		    GtkHTMLControlData * cd)
{
	HTMLEngine *engine = cd->html->engine;

	cd->obj = html_engine_get_object_at(engine,
					    event->x + engine->x_offset,
					    event->y + engine->y_offset,
					    NULL, FALSE);
	switch (event->button) {
	case 1:
		if (event->type == GDK_2BUTTON_PRESS && cd->obj
		    && event->state & GDK_CONTROL_MASK)
			cd->releaseId =
			    gtk_signal_connect(GTK_OBJECT(html),
					       "button_release_event",
					       GTK_SIGNAL_FUNC(release),
					       cd);
		else
			return TRUE;
		break;
	case 2:
		/* pass this for pasting */
		return TRUE;
	case 3:
		if (popup_show(cd, event))
			gtk_signal_emit_stop_by_name(GTK_OBJECT(html),
						     "button_press_event");
		break;
	default:
	}

	return FALSE;
}



/******************************************************************************
 * create_editor - create html editor
 ******************************************************************************/
GtkWidget *create_editor(GtkWidget *app1 ,EDITOR ed_widgets)
{ 	
	GtkHTMLControlData *cd;
	EDITOR *ed;
	GtkWidget *vbEditor;
	GtkWidget *vbox3;
	GtkWidget *handlebox1;
	GtkWidget *FormatBar;
	GtkWidget *hbox1;
	GtkWidget *handlebox2;
	GtkWidget *EditBar;
	GtkWidget *handlebox3;
	GtkWidget *InsertBar;
	GtkWidget *handlebox4;
	GtkWidget *FileBar;
	GtkWidget *vbox2;
	GtkWidget *frame1;
	GtkWidget *scrolledwindowHTML;
	//GtkWidget *html_widget;
	GtkWidget *label1;
	GtkWidget *scrolledwindowTEXT;
	//GtkWidget *gtktext;
	GtkWidget *label2;
	ed = &ed_widgets;


	vbEditor = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbEditor);
	gtk_object_set_data_full(GTK_OBJECT(app1), "vbEditor", vbEditor,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbEditor);
	gtk_container_add (GTK_CONTAINER (ed->vbox), vbEditor);

	handlebox1 = gtk_handle_box_new();
	gtk_widget_ref(handlebox1);
	gtk_object_set_data_full(GTK_OBJECT(app1), "handlebox1",
				 handlebox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handlebox1);
	gtk_box_pack_start(GTK_BOX(vbEditor), handlebox1, FALSE, FALSE, 0);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_object_set_data_full(GTK_OBJECT(app1), "vbox3", vbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(handlebox1), vbox3);

	//ed->htmlwidget = gtk_html_new();
	gtk_widget_ref(ed->htmlwidget );
	gtk_object_set_data_full(GTK_OBJECT(app1),
				 "ed->htmlwidget", ed->htmlwidget,
				 (GtkDestroyNotify) gtk_widget_unref);
	
	
	/*   gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	   gtk_widget_ref(FormatBar);
	   gtk_object_set_data_full(GTK_OBJECT(app1), "FormatBar", FormatBar,
	   (GtkDestroyNotify) gtk_widget_unref);
	   gtk_widget_show(FormatBar);
	   gtk_box_pack_start(GTK_BOX(vbox3), FormatBar, FALSE, FALSE, 0); */


	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox1);
	gtk_object_set_data_full(GTK_OBJECT(app1), "hbox1", hbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox1);
	gtk_box_pack_start(GTK_BOX(vbEditor), hbox1, FALSE, FALSE, 0);

	handlebox2 = gtk_handle_box_new();
	gtk_widget_ref(handlebox2);
	gtk_object_set_data_full(GTK_OBJECT(app1), "handlebox2",
				 handlebox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handlebox2);
	gtk_box_pack_start(GTK_BOX(hbox1), handlebox2, FALSE, FALSE, 0);
	/* edit  buttons */

	handlebox3 = gtk_handle_box_new();
	gtk_widget_ref(handlebox3);
	gtk_object_set_data_full(GTK_OBJECT(app1), "handlebox3",
				 handlebox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handlebox3);
	gtk_box_pack_start(GTK_BOX(hbox1), handlebox3, FALSE, FALSE, 0);
	/* insert  buttons */
	handlebox4 = gtk_handle_box_new();
	gtk_widget_ref(handlebox4);
	gtk_object_set_data_full(GTK_OBJECT(app1), "handlebox4",
				 handlebox4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handlebox4);
	gtk_box_pack_start(GTK_BOX(hbox1), handlebox4, FALSE, FALSE, 0);
	/* file buttons */


	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(app1), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_box_pack_start(GTK_BOX(vbEditor), vbox2, TRUE, TRUE, 0);

	//notebookEDITOR = gtk_notebook_new();
	
	gtk_widget_ref(ed->notebook);
	gtk_object_set_data_full(GTK_OBJECT(app1), "ed->notebook",
				 ed->notebook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ed->notebook);
	gtk_box_pack_start(GTK_BOX(vbox2), ed->notebook, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(ed->notebook, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ed->notebook), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(ed->notebook), FALSE);

	frame1 = gtk_frame_new(NULL);
	gtk_widget_ref(frame1);
	gtk_object_set_data_full(GTK_OBJECT(app1), "frame1", frame1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame1);
	gtk_container_add(GTK_CONTAINER(ed->notebook), frame1);

	scrolledwindowHTML = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowHTML);
	gtk_object_set_data_full(GTK_OBJECT(app1), "scrolledwindowHTML",
				 scrolledwindowHTML,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindowHTML);
	gtk_container_add(GTK_CONTAINER(frame1), scrolledwindowHTML);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowHTML),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);	
	
	gtk_widget_show(ed->htmlwidget);
	gtk_container_add(GTK_CONTAINER(scrolledwindowHTML), ed->htmlwidget);
	gtk_html_load_empty(GTK_HTML(ed->htmlwidget));	
	gtk_html_set_editable (GTK_HTML(ed->htmlwidget), TRUE );
	
	cd =
	    gtk_html_control_data_new(GTK_HTML
				      (ed->htmlwidget), vbox3);
	
	cd->notebook = ed->notebook;
	cd->statusbar = ed->statusbar;
	cd->note_editor = ed->note_editor;
	cd->gtktext = ed->text;
	
	FormatBar = toolbar_style(cd);
	gtk_box_pack_start(GTK_BOX(cd->vbox), FormatBar, FALSE, FALSE,
			   0);
	EditBar = toolbar_edit(cd);
	gtk_container_add(GTK_CONTAINER(handlebox3), EditBar); 
	
	FileBar = toolbar_file(cd);
	gtk_container_add(GTK_CONTAINER(handlebox2), FileBar); 
	
	InsertBar = toolbar_insert(cd);
	gtk_container_add(GTK_CONTAINER(handlebox4), InsertBar);
	
	label1 = gtk_label_new(_("label1"));
	gtk_widget_ref(label1);
	gtk_object_set_data_full(GTK_OBJECT(app1), "label1", label1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label1);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(ed->notebook),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (ed->notebook),
							     0), label1);

	scrolledwindowTEXT = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowTEXT);
	gtk_object_set_data_full(GTK_OBJECT(app1), "scrolledwindowTEXT",
				 scrolledwindowTEXT,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindowTEXT);
	gtk_container_add(GTK_CONTAINER(ed->notebook),
			  scrolledwindowTEXT);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowTEXT),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	//gtktext = gtk_text_new(NULL, NULL);
	gtk_widget_ref(ed->text);
	gtk_object_set_data_full(GTK_OBJECT(app1), "ed->text", ed->text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ed->text);
	gtk_container_add(GTK_CONTAINER(scrolledwindowTEXT), ed->text);
	gtk_text_set_word_wrap(GTK_TEXT (ed->text) , TRUE );
	gtk_text_set_editable(GTK_TEXT (ed->text) , TRUE);
	
	label2 = gtk_label_new(_("label2"));
	gtk_widget_ref(label2);
	gtk_object_set_data_full(GTK_OBJECT(app1), "label2", label2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label2);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(ed->notebook),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (ed->notebook),
							     1), label2);

	gtk_widget_ref(ed->statusbar);
	gtk_object_set_data_full(GTK_OBJECT(app1), "ed->statusbar",
				 ed->statusbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ed->statusbar);
	gtk_box_pack_start(GTK_BOX(vbEditor), ed->statusbar, FALSE, FALSE, 0);

	gtk_signal_connect(GTK_OBJECT
			   (ed->htmlwidget),
			   "key_press_event",
			   GTK_SIGNAL_FUNC(html_key_pressed), cd);
	gtk_signal_connect(GTK_OBJECT
			   (ed->htmlwidget),
			   "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), cd);
			   
	return ed->htmlwidget;
}

