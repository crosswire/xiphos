/*
 * GnomeSword Bible Study Tool
 * studypad.c - studypad gui
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
#include <gal/widgets/e-unicode.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "gui/gnomesword.h"
#include "gui/studypad.h"
#include "gui/editor.h"
#include "gui/toolbar_style.h"
#include "gui/toolbar_edit.h"
#include "gui/editor_menu.h"
#include "gui/editor_spell.h"
#include "gui/html.h"
#include "gui/dialog.h"
#include "gui/fileselection.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/xml.h"

#define BUFFER_SIZE 4096

GSHTMLEditorControlData *editor_cd;

gboolean
on_studypad_delete                        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        GSHTMLEditorControlData *ecd)
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
			_("has been modified. Do you wish to save it?"));
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			if (settings.studypadfilename) {
				filename = g_strdup(settings.studypadfilename);
				save_file(filename, ecd);
			} else {
				gui_fileselection_save(ecd,TRUE);
			}
		}
		settings.modifiedSP = FALSE;
		g_free(info);
		g_string_free(str,TRUE);
	}
	return FALSE;
}

void screen_changed(GtkWidget *widget, GdkScreen *arg1, gpointer user_data)
{
	//g_warning("screen_changed");
	
}
	

/******************************************************************************
 * Name
 *   gui_studypad_can_close
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   void gui_studypad_can_close(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_studypad_can_close(void)
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
			_("has been modified. Do you wish to save it?"));
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			if (settings.studypadfilename) {
				filename = g_strdup(settings.studypadfilename);
				save_file(filename, editor_cd);
			} else {
				gui_fileselection_save(editor_cd,TRUE);
			}
		}
		settings.modifiedSP = FALSE;
		g_free(info);
		g_string_free(str,TRUE);
	}
}

/******************************************************************************
 * Name
 *   load_file
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   	gint load_file(gchar * filename, GSHTMLEditorControlData * ecd)
 *
 * Description
 *    load file into studypad
 *
 * Return value
 *   gint
 */

gint load_file(gchar * filename, GSHTMLEditorControlData * ecd)
{
	GtkHTMLStream *stream;
	char buffer[BUFFER_SIZE];
	ssize_t count;
	gboolean was_editable;
	int fd;
	GString *str;
	
	g_warning(filename);
	if(!g_file_test((const gchar*)filename, G_FILE_TEST_EXISTS)) {
		str = g_string_new(" ");
		g_string_printf(str, "file not found: %s",filename);
		ecd->changed = FALSE;
		gtk_html_load_from_string(ecd->html,"file not found",15);
		settings.studypadfilename = NULL;
		xml_set_value("GnomeSword", "studypad", "lastfile", NULL);		
		sprintf(ecd->filename, "%s", "");
		ecd->filename[0] = '\0';
		gtk_statusbar_push(GTK_STATUSBAR(ecd->statusbar), 1, str->str);
		ecd->changed = FALSE;
		g_string_free(str,TRUE);
		return 0;
	}
	
	settings.studypadfilename = filename;
	xml_set_value("GnomeSword", "studypad", "lastfile", filename);

	ecd->changed = FALSE;
	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return -1;

	was_editable = gtk_html_get_editable(ecd->html);
	if (was_editable)
		gtk_html_set_editable(ecd->html, FALSE);

	stream = gtk_html_begin(ecd->html);
	if (stream == NULL) {
		close(fd);
		if (was_editable)
			gtk_html_set_editable(ecd->html, TRUE);
		return -1;
	}

	while (1) {
		count = read(fd, buffer, BUFFER_SIZE);
		if (count > 0)
			gtk_html_write(ecd->html, stream, buffer,
				       count);
		else
			break;
	}

	close(fd);

	if (count == 0) {
		gtk_html_end(ecd->html, stream, GTK_HTML_STREAM_OK);
		if (was_editable)
			gtk_html_set_editable(ecd->html, TRUE);
		gui_update_statusbar(ecd);
		return 0;
	} else {
		gtk_html_end(ecd->html, stream, GTK_HTML_STREAM_ERROR);
		if (was_editable)
			gtk_html_set_editable(ecd->html, TRUE);
		return -1;
	}

}

/******************************************************************************
 * Name
 *   save_receiver
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   gboolean save_receiver(const HTMLEngine * engine,
 *		const char *data, unsigned int len, void *user_data)	
 *
 * Description
 *    save reviever for saving html file
 *
 * Return value
 *   gboolean
 */

static gboolean save_receiver(const HTMLEngine * engine,
			      const char *data, unsigned int len,
			      void *user_data)
{
	int fd;

	fd = GPOINTER_TO_INT(user_data);

	while (len > 0) {
		ssize_t count;

		count = write(fd, data, len);
		if (count < 0)
			return FALSE;

		len -= count;
		data += count;
	}

	return TRUE;
}

/******************************************************************************
 * Name
 *   save_file_program_end
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   gint save_file_program_end(GtkWidget * htmlwidget, gchar * filename)	
 *
 * Description
 *    save studypad file at program close
 *
 * Return value
 *   gint
 */

gint save_file_program_end(GtkWidget * htmlwidget, gchar * filename)
{
	int retval = -1;
	int fd;
	if (filename) {
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);

		if (fd == -1)
			return -1;

		if (!gtk_html_save
		    (GTK_HTML(htmlwidget),
		     (GtkHTMLSaveReceiverFn) save_receiver,
		     GINT_TO_POINTER(fd)))
			retval = -1;
		else {
			retval = 0;
		}
		close(fd);
	}
	return retval;
}

/******************************************************************************
 * Name
 *   save_file
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   gint save_file(gchar * filename, GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    save studypad file
 *
 * Return value
 *   gint
 */

gint save_file(gchar * filename, GSHTMLEditorControlData * ecd)
{
	int retval = -1;
	int fd;

	if (filename) {
		if(ecd->studypad) {
			xml_set_value("GnomeSword", "studypad", "lastfile", 
							filename);
			settings.studypadfilename = 
					xml_get_value("studypad", "lastfile");
			strcpy(ecd->filename, settings.studypadfilename);
		}
			
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);

		if (fd == -1)
			return -1;

		if (!gtk_html_export (ecd->html, "text/html",
		 (GtkHTMLSaveReceiverFn) save_receiver,
		 GINT_TO_POINTER(fd))) {		 
			retval = -1;
		 }
		else {
			retval = 0;
			ecd->changed = FALSE;
			gui_update_statusbar(ecd);
		}

		close(fd);
		g_free(filename);
	}
	return retval;
}


/******************************************************************************
 * Name
 *   save_file_plain_text
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   gint save_file_plain_text(gchar * filename, GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    save studypad file
 *
 * Return value
 *   gint
 */

gint save_file_plain_text(gchar * filename, GSHTMLEditorControlData * ecd)
{
	int retval = -1;
	int fd;

	if (filename) {
					
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);

		if (fd == -1)
			return -1;

		if (!gtk_html_export (ecd->html, "text/plain",
		 		(GtkHTMLSaveReceiverFn) save_receiver,
		 			GINT_TO_POINTER(fd))) {
			retval = -1;
		 }
		else {
			retval = 0;
			//ecd->changed = FALSE;
			//gui_update_statusbar(ecd);
		}

		close(fd);
		g_free(filename);
	}
	return retval;
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

/******************************************************************************
 * Name
 *  html_load_done
 *
 * Synopsis
 *   #include "studypad.h"
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
	gui_update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_submit
 *
 * Synopsis
 *   #include "studypad.h"
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
 *   #include "studypad.h"
 *
 *   gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    mouse button pressed in editor 
 *
 * Return value
 *   gint
 */

static gint html_button_pressed(GtkWidget * html,
				GdkEventButton * event,
				GSHTMLEditorControlData * ecd)
{
	if (ecd->personal_comments)
		settings.whichwindow = PERCOMM_WINDOW;
	if (ecd->studypad)
		settings.whichwindow = STUDYPAD_WINDOW;
	switch (event->button) {
	case 1:
		if (event->type == GDK_2BUTTON_PRESS && ecd->obj
		    && event->state & GDK_CONTROL_MASK) {
			ecd->releaseId =
			    g_signal_connect(G_OBJECT(html),
					       "button_release_event",
					       G_CALLBACK(release),
					       ecd);

		}

		break;
	case 2:
		/* 
		 * pass this for pasting 
		 */
	case 3:
		gtk_signal_emit_stop_by_name(GTK_OBJECT(html),
					     "button_press_event");
		break;
	}

	return FALSE;
}

/******************************************************************************
 * Name
 *  on_html_enter_notify_event
 *
 * Synopsis
 *   #include "studypad.h"
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
					   GSHTMLEditorControlData *
					   ecd)
{
	if (!ecd->personal_comments && !ecd->gbs)
		if (!gtk_html_get_editable(ecd->html))
			gtk_html_set_editable(ecd->html, TRUE);
	return TRUE;
}

/******************************************************************************
 * Name
 *  gui_create_studypad_control
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   GtkWidget *gui_create_studypad_control(GtkWidget *container)	
 *
 * Description
 *    create studypad control
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_studypad_control(GtkWidget * container,
				gchar * filename)
{
	GtkWidget *vbox;
	GtkWidget *vboxSP;
	GtkWidget *hboxstyle;
	GtkWidget *htmlwidget;
	GtkWidget *frame34;
	GtkWidget *scrolledwindow17;
	GtkWidget *toolbar;

	GSHTMLEditorControlData *specd =
	    gs_html_editor_control_data_new();


	specd->studypad = TRUE;
	specd->stylebar = settings.show_style_bar_sp;
	specd->editbar = settings.show_edit_bar_sp;

	vboxSP = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vboxSP);
	gtk_container_add(GTK_CONTAINER(container), vboxSP);
	
	g_signal_connect(GTK_OBJECT(container), "delete-event",
                G_CALLBACK(on_studypad_delete),
			   specd);
			   
	hboxstyle = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hboxstyle);
	gtk_box_pack_start(GTK_BOX(vboxSP), hboxstyle, TRUE, TRUE, 0);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_box_pack_start(GTK_BOX(hboxstyle), vbox, TRUE, TRUE, 0);

	htmlwidget = gtk_html_new();

	frame34 = gtk_frame_new(NULL);
	gtk_widget_show(frame34);
	gtk_box_pack_end(GTK_BOX(vbox), frame34, TRUE, TRUE, 0);

	scrolledwindow17 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow17);
	gtk_container_add(GTK_CONTAINER(frame34), scrolledwindow17);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow17),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow17,
                                             settings.shadow_type);

	specd->htmlwidget = htmlwidget;
	specd->html = GTK_HTML(specd->htmlwidget);
	gtk_widget_show(specd->htmlwidget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow17),
			  specd->htmlwidget);

	specd->statusbar = gtk_statusbar_new();
	gtk_widget_show(specd->statusbar);
	gtk_box_pack_start(GTK_BOX(vboxSP), specd->statusbar, FALSE,
			   TRUE, 0);

	specd->vbox = vboxSP;
	specd->pm = gui_create_editor_popup(specd);
	gnome_popup_menu_attach(specd->pm, specd->htmlwidget, NULL);
	

	g_signal_connect(GTK_OBJECT
			   (specd->htmlwidget),
			   "screen-changed",
			   G_CALLBACK(screen_changed), specd);
	g_signal_connect(G_OBJECT(specd->htmlwidget),"key_press_event",
			   G_CALLBACK(html_key_pressed), 
			   specd);
	g_signal_connect(G_OBJECT(specd->htmlwidget), "link_clicked", 
			G_CALLBACK(gui_link_clicked),	
			   NULL);
	g_signal_connect(G_OBJECT(specd->htmlwidget), "on_url", 
			G_CALLBACK(gui_url),	
			   NULL);
	g_signal_connect(G_OBJECT(specd->htmlwidget),"button_press_event",
			   G_CALLBACK(html_button_pressed), 
			   specd);

	/* create toolbars */
	widgets.toolbar_studypad = gui_toolbar_style(specd);
	if (settings.show_style_bar_sp)
		gtk_widget_show(widgets.toolbar_studypad);
	else
		gtk_widget_hide(widgets.toolbar_studypad);
	
	gtk_box_pack_start(GTK_BOX(vbox), widgets.toolbar_studypad,
			   FALSE, FALSE, 0);

	toolbar = gui_toolbar_edit(specd);
	if (settings.show_edit_bar_sp)
		gtk_widget_show(toolbar);
	else
		gtk_widget_hide(toolbar);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

	/* load last file */
	if (filename) 
		load_file(filename, specd);
	else 
		gtk_html_load_from_string(specd->html,"  ",2);
		
	editor_cd = specd;	
	
	gtk_html_set_editable(specd->html,TRUE);
	
	//gui_new_editor_api(specd);
	return htmlwidget;
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   	
 *
 * Description
 *    
 *
 * Return value
 *   
 */

gint gui_open_studypad(gchar * file_name)
{
/*	widgets.studypad_dialog = create_dialog();

	widgets.html_studypad =
	  	gui_create_studypad_control(widgets.studypad_dialog,
							  file_name);
*/	
	return TRUE;
}
