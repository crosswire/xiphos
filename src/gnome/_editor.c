/*
 * GnomeSword Bible Study Tool
 * _editor.c - the html editor
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/gtkhtmlfontstyle.h>
#include <gtkhtml/htmlform.h>
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
#include <gal/widgets/e-unicode.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef USE_SPELL
#include "spell.h"
#include "spell_gui.h"
#endif /* USE_SPELL */

#include "gs_gnomesword.h"
#include "gs_html.h"
#include "gs_editor.h"
#include "gs_info_box.h"
#include "settings.h"

/* gnome */
#include "editor_toolbar.h"
#include "editor_menu.h"
#include "link_dialog.h"
#include "fileselection.h"

/* main */
#include "percomm.h"

/******************************************************************************
 * much this code taken form GtkHTML
 */ 
 
/******************************************************************************
 * Name
 *  release
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   gint release(GtkWidget * widget, GdkEventButton * event,
					GSHTMLEditorControlData * cd)	
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

/******************************************************************************
 * Name
 *  html_key_pressed
 *
 * Synopsis
 *   #include "_editor.h"
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
 
static gint html_key_pressed(GtkWidget * html, GdkEventButton * event,
					GSHTMLEditorControlData * ecd)
{
	ecd->changed = TRUE;
	//file_changed = TRUE;
	update_statusbar(ecd);
	return 1;
}

/******************************************************************************
 * Name
 *  html_load_done
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void html_load_done(GtkWidget * html,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *   ???? 
 *
 * Return value
 *   void
 */
 
static void html_load_done(GtkWidget * html,
					GSHTMLEditorControlData * ecd)
{
	update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_submit
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_submit(GtkHTML * html, const gchar * method,
 *		      const gchar * url, const gchar * encoding,
 *		      GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    ?????
 *
 * Return value
 *   void
 */
 
static void on_submit(GtkHTML * html, const gchar * method,
		      const gchar * url, const gchar * encoding,
		      GSHTMLEditorControlData * ecd)
{
	/*
	   GList *l;

	   l = NULL;

	   l=html->engine->form->elements;
	   while (l != NULL) {  
	   g_warning((gchar *) l->data);
	   l = g_list_next(l);
	   }
	   g_list_free(l);
	 */
	g_warning(method);
	g_warning(url);
	g_warning(encoding);

}

/******************************************************************************
 * Name
 *  html_button_pressed
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * cd)	
 *
 * Description
 *    mouse button pressed in editor 
 *
 * Return value
 *   gint
 */ 

static gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
					GSHTMLEditorControlData * cd)
{
	//HTMLEngine *engine = cd->html->engine;

	switch (event->button) {
	case 1:
		if (event->type == GDK_2BUTTON_PRESS && cd->obj
		    && event->state & GDK_CONTROL_MASK) {
			cd->releaseId =
			    gtk_signal_connect(GTK_OBJECT(html),
					       "button_release_event",
					       GTK_SIGNAL_FUNC(release),
					       cd);

		}

		else
			return TRUE;
		break;
	case 2:
		/* 
		 * pass this for pasting 
		 */
		return TRUE;
	case 3:
		gtk_signal_emit_stop_by_name(GTK_OBJECT(html),
					     "button_press_event");
		break;
	default:
	}

	return FALSE;
}

/******************************************************************************
 * Name
 *  on_html_enter_notify_event
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   gboolean on_html_enter_notify_event(GtkWidget * widget,
 *			   GdkEventCrossing * event,
 *			   GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    mouse moved into editor - sets studypad html widget to edit mode
 *
 * Return value
 *   gboolean
 */
 
static gboolean on_html_enter_notify_event(GtkWidget * widget,
			   GdkEventCrossing * event,
			   GSHTMLEditorControlData * ecd)
{
	if (!ecd->personal_comments && !ecd->gbs)
		if (!gtk_html_get_editable(ecd->html))
			gtk_html_set_editable(ecd->html, TRUE);
	return TRUE;
}

/******************************************************************************
 * Name
 *  on_btn_save_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_save_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    save contents of editor
 *
 * Return value
 *   void
 */
 
static void on_btn_save_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)
{
	if(ecd->studypad) {
		GtkWidget *savemyFile;
		gchar buf[255];
		
		if (ecd->filename) {
			save_file(ecd->filename, ecd);
			return;
		} else {
			sprintf(buf, "%s/.pad", settings.homedir);
			savemyFile = gui_fileselection_save(ecd);
			gtk_file_selection_set_filename(GTK_FILE_SELECTION
							(savemyFile), buf);
			gtk_widget_show(savemyFile);
		}
	}else if(ecd->personal_comments) {
		editor_save_note(ecd->htmlwidget);
		update_statusbar(ecd);
	}
		
}


/******************************************************************************
 * Name
 *  on_btn_delete_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_delete_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    delete personal commnet
 *
 * Return value
 *   void
 */
 
static void on_btn_delete_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)
{
	if(ecd->personal_comments) {
		delete_percomm_note();
		ecd->changed = FALSE;
		update_statusbar(ecd);
	}
}

/******************************************************************************
 * Name
 *  on_btn_open_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_open_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open file dialog 
 *
 * Return value
 *   void
 */
 
static void on_btn_open_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)
{
	GtkWidget *openFile;
	gchar *msg, buf[255];
	GtkWidget *msgbox;
	gint answer = 0;
	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		msg =
		    g_strdup_printf(_
				    ("``%s'' has been modified.  Do you wish to save it?"),
				    ecd->filename);
		msgbox = create_InfoBox();
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer =
		    gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
		g_free(msg);
		switch (answer) {
		case 0:
			save_file(ecd->filename, ecd);
			break;
		default:
			break;
		}
	}
	sprintf(buf, "%s/*.pad", settings.homedir);
	openFile = gui_fileselection_open(ecd);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(openFile),
					buf);
	gtk_widget_show(openFile);
}

/******************************************************************************
 * Name
 *  on_btn_print_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_print_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    print the editor text
 *
 * Return value
 *   void
 */
 
static void on_btn_print_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	html_print(ecd->htmlwidget);
}

/******************************************************************************
 * Name
 *  on_btn_cut_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_cut_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    cut selected text to clipboard
 *
 * Return value
 *   void
 */
 
static void on_btn_cut_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gtk_html_cut(ecd->html);
	ecd->changed = TRUE;
	update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_btn_copy_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_copy_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    copy selected text to clipboard
 *
 * Return value
 *   void
 */
 
static void on_btn_copy_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gtk_html_copy(ecd->html);
}

/******************************************************************************
 * Name
 *  on_btn_paste_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_paste_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    paste contents of clipboard into editor
 *
 * Return value
 *   
 */
 
static void on_btn_paste_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gtk_html_paste(ecd->html);
	ecd->changed = TRUE;
	update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_btn_undo_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_undo_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    undo changes in the editor
 *
 * Return value
 *   void
 */ 

static void on_btn_undo_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gtk_html_undo(ecd->html);
	ecd->changed = TRUE;
	update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_btn_Find_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_Find_clicked(GtkButton *button, 
 *					GSHTMLEditorControlData *ecd)	
 *
 * Description
 *    open find dialog
 *
 * Return value
 *   void
 */ 

static void on_btn_Find_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	search(ecd, FALSE, NULL);
}


/******************************************************************************
 * Name
 *  on_btn_replace_clicked
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void on_btn_replace_clicked(GtkButton * button,
		       GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open find and replace dialog
 *
 * Return value
 *   void
 */
 
static void on_btn_replace_clicked(GtkButton * button,
		       GSHTMLEditorControlData * ecd)
{
	replace(ecd);
}

/******************************************************************************
 * Name
 *  gui_create_html_editor
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   GtkWidget *gui_create_html_editor(GtkWidget * htmlwidget, GtkWidget * vbox,
			 SETTINGS * s, GSHTMLEditorControlData * necd)	
 *
 * Description
 *    create html editor gui
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_html_editor(GtkWidget * htmlwidget, GtkWidget * vbox,
			 SETTINGS * s, GSHTMLEditorControlData * necd)
{
	GtkWidget *frame34;
	GtkWidget *scrolledwindow17;
	GtkWidget *toolbar;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator19;
	GtkWidget *vseparator20;
	GtkWidget *vseparator21;

	necd->frame_toolbar = gtk_frame_new(NULL);
	gtk_widget_ref(necd->frame_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "necd->frame_toolbar",
				 necd->frame_toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	if (necd->studypad)
		gtk_widget_show(necd->frame_toolbar);
	gtk_box_pack_start(GTK_BOX(vbox), necd->frame_toolbar, FALSE,
			   TRUE, 0);

	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbar", toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_container_add(GTK_CONTAINER(necd->frame_toolbar), toolbar);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);
	
	if(necd->studypad) {	
		tmp_toolbar_icon =
		    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_OPEN);
		necd->btn_open =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_BUTTON, NULL,
					       _("Open"), _("Open File"), NULL,
					       tmp_toolbar_icon, NULL, NULL);
		gtk_widget_ref(necd->btn_open);
		gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_open",
					 necd->btn_open,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(necd->btn_open);
	
		gtk_signal_connect(GTK_OBJECT(necd->btn_open), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_open_clicked),
			   necd);	
	}	
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_SAVE);
	necd->btn_save =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save"), _("Save Note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(necd->btn_save);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_save",
				 necd->btn_save,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->btn_save);
	
	if(!necd->studypad) {	
		tmp_toolbar_icon =
		    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_TRASH);
		necd->btn_delete =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_BUTTON, NULL,
					       _("Delete"), _("Delete Note"),
					       NULL, tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_ref(necd->btn_delete);
		gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_delete",
					 necd->btn_delete,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(necd->btn_delete);
	
		gtk_signal_connect(GTK_OBJECT(necd->btn_delete), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_delete_clicked),
			   necd);
	}

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_PRINT);
	necd->btn_print =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"), _("Print Note"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(necd->btn_print);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_print",
				 necd->btn_print,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->btn_print);

	vseparator19 = gtk_vseparator_new();
	gtk_widget_ref(vseparator19);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vseparator19",
				 vseparator19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator19);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator19,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator19, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_CUT);
	necd->btn_cut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Cut"), _("Cut "), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(necd->btn_cut);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_cut",
				 necd->btn_cut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->btn_cut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_COPY);
	necd->btn_copy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Copy"), _("Copy"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(necd->btn_copy);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_copy",
				 necd->btn_copy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->btn_copy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_PASTE);
	necd->btn_paste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Paste"), _("Paste"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(necd->btn_paste);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_paste",
				 necd->btn_paste,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->btn_paste);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_UNDO);
	necd->btn_undo =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Undo"), _("Undo"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(necd->btn_undo);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_undo",
				 necd->btn_undo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->btn_undo);

	vseparator20 = gtk_vseparator_new();
	gtk_widget_ref(vseparator20);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vseparator20",
				 vseparator20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator20);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator20,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator20, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_SEARCH);
	necd->btn_Find =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Find"),
				       _("Find in this note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(necd->btn_Find);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_Find",
				 necd->btn_Find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->btn_Find);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_SRCHRPL);
	necd->btn_replace =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Replace"),
				       _("Find and Replace"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(necd->btn_replace);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "necd->btn_replace", necd->btn_replace,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->btn_replace);

	vseparator21 = gtk_vseparator_new();
	gtk_widget_ref(vseparator21);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vseparator21",
				 vseparator21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator21);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator21,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator21, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	necd->btn_spell =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Spell"),
				       _("Spell check note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(necd->btn_spell);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->btn_spell",
				 necd->btn_spell,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->btn_spell);

	frame34 = gtk_frame_new(NULL);
	gtk_widget_ref(frame34);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame34", frame34,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame34);
	gtk_box_pack_start(GTK_BOX(vbox), frame34, TRUE, TRUE, 0);

	scrolledwindow17 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow17);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "scrolledwindow17", scrolledwindow17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow17);
	gtk_container_add(GTK_CONTAINER(frame34), scrolledwindow17);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow17),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	necd->htmlwidget = htmlwidget;
	necd->html = GTK_HTML(necd->htmlwidget);
	gtk_widget_ref(necd->htmlwidget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "necd->htmlwidget", necd->htmlwidget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->htmlwidget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow17),
			  necd->htmlwidget);
	gtk_html_load_empty(necd->html);

	necd->statusbar = gtk_statusbar_new();
	gtk_widget_ref(necd->statusbar);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "necd->statusbar",
				 necd->statusbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(necd->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox), necd->statusbar, FALSE, TRUE,
			   0);
			   
	necd->vbox = vbox;
	necd->pm = gui_create_editor_popup(necd);
	gnome_popup_menu_attach(necd->pm, necd->htmlwidget, NULL);
	
	gtk_signal_connect(GTK_OBJECT(necd->html), "submit",
			   GTK_SIGNAL_FUNC(on_submit), necd);
	gtk_signal_connect(GTK_OBJECT
			   (necd->htmlwidget),
			   "load_done",
			   GTK_SIGNAL_FUNC(html_load_done), necd);
	gtk_signal_connect(GTK_OBJECT
			   (necd->htmlwidget),
			   "key_press_event",
			   GTK_SIGNAL_FUNC(html_key_pressed), necd);
	gtk_signal_connect(GTK_OBJECT
			   (necd->htmlwidget),
			   "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), necd);
	gtk_signal_connect(GTK_OBJECT(necd->htmlwidget),
			   "enter_notify_event",
			   GTK_SIGNAL_FUNC(on_html_enter_notify_event),
			   necd);
	gtk_signal_connect(GTK_OBJECT(necd->htmlwidget), "link_clicked", GTK_SIGNAL_FUNC(on_link_clicked),	/* gs_html.c */
			   NULL);
	gtk_signal_connect(GTK_OBJECT(necd->htmlwidget), "on_url", GTK_SIGNAL_FUNC(on_url),	/* gs_html.c */
			   NULL);

	gtk_signal_connect(GTK_OBJECT(necd->btn_save), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_save_clicked), necd);
	gtk_signal_connect(GTK_OBJECT(necd->btn_print), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_print_clicked), necd);
	gtk_signal_connect(GTK_OBJECT(necd->btn_cut), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_cut_clicked), necd);
	gtk_signal_connect(GTK_OBJECT(necd->btn_copy), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_copy_clicked), necd);
	gtk_signal_connect(GTK_OBJECT(necd->btn_paste), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_paste_clicked), necd);
	gtk_signal_connect(GTK_OBJECT(necd->btn_undo), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_undo_clicked), necd);
	gtk_signal_connect(GTK_OBJECT(necd->btn_Find), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_Find_clicked), necd);
	gtk_signal_connect(GTK_OBJECT(necd->btn_replace), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_replace_clicked),
			   necd);
	gtk_signal_connect(GTK_OBJECT(necd->btn_spell), "clicked",
			   GTK_SIGNAL_FUNC(spell_check_cb), necd);

	return necd->htmlwidget;
}

/******   end of file   ******/
