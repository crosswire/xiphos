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

#ifdef USE_SPELL
//#include "main/spell.h"
//#include "main/spell_gui.h"
#endif

#include "gui/gnomesword.h"
#include "gui/studypad.h"
#include "gui/_editor.h"
#include "gui/editor_toolbar.h"
#include "gui/editor_menu.h"
#include "gui/editor_spell.h"
#include "gui/html.h"
#include "gui/dialog.h"

#include "gui/fileselection.h"
#include "main/settings.h"

#define BUFFER_SIZE 4096

static void on_btn_save_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd);
					
static GSHTMLEditorControlData *editor_cd;


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
	if (settings.modifiedSP) {
		
		gint test;
		GS_DIALOG *info;
	
		info = gui_new_dialog();
		if (strlen(settings.studypadfilename) > 0 )
			info->label_top = settings.studypadfilename;
		else
			info->label_top = N_("File");
		info->label_middle = N_("has been modified. ");
		info->label_bottom = N_("Do you wish to save it?");
		info->yes = TRUE;
		info->no = TRUE;
	
		test = gui_gs_dialog(info);
		if (test == GS_YES){
			on_btn_save_clicked(NULL,
					editor_cd);
		}
		
		g_free(info);
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
	char buffer[BUFFER_SIZE], *base_name = NULL;
	ssize_t count;
	gboolean was_editable;
	int fd;
	
	sprintf(settings.studypadfilename, "%s", filename);
	
	base_name = g_basename(filename);
	if(base_name)
		gtk_frame_set_label(GTK_FRAME(ecd->frame), base_name);
	else
		gtk_frame_set_label(GTK_FRAME(ecd->frame), NULL);
	
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
		update_statusbar(ecd);
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
		const char *data, unsigned int len, void *user_data)
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
	gchar *base_name;
	
	if (filename) {
		sprintf(settings.studypadfilename, "%s", filename);

		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);

		if (fd == -1)
			return -1;

		if (!gtk_html_save
		    (ecd->html, (GtkHTMLSaveReceiverFn) save_receiver,
		     GINT_TO_POINTER(fd)))
			retval = -1;
		else {
			retval = 0;
			ecd->changed = FALSE;
			update_statusbar(ecd);
			base_name = g_basename(filename);
			gtk_frame_set_label(GTK_FRAME(ecd->frame), base_name);
		}

		close(fd);
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
	update_statusbar(ecd);
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

static gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
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
			    gtk_signal_connect(GTK_OBJECT(html),
					       "button_release_event",
					       GTK_SIGNAL_FUNC(release),
					       ecd);

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
 *   #include "studypad.h"
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
	if(strlen(settings.studypadfilename) > 0)  {
		save_file(ecd->filename, ecd);
	} else {
		gui_fileselection_save(ecd);
	}
		
}

/******************************************************************************
 * Name
 *  on_btn_open_clicked
 *
 * Synopsis
 *   #include "studypad.h"
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
	gchar buf[255];
	
	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		gint test;
		GS_DIALOG *info;
	
		info = gui_new_dialog();
		if (strlen(settings.studypadfilename) > 0 )
			info->label_top = settings.studypadfilename;
		else
			info->label_top = N_("File");
		info->label_middle = N_("has been modified. ");
		info->label_bottom = N_("Do you wish to save it?");
		info->yes = TRUE;
		info->no = TRUE;
	
		test = gui_gs_dialog(info);
		if (test == GS_YES){
			on_btn_save_clicked(NULL,
					editor_cd);
		}		
		g_free(info);
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
 *   #include "studypad.h"
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
	gui_html_print(ecd->htmlwidget);
}

/******************************************************************************
 * Name
 *  on_btn_cut_clicked
 *
 * Synopsis
 *   #include "studypad.h"
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
 *   #include "studypad.h"
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
 *   #include "studypad.h"
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
 *   #include "studypad.h"
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
 *   #include "studypad.h"
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
 *   #include "studypad.h"
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
 *  gui_create_studypad_control
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   GtkWidget *gui_create_studypad_control(GtkWidget *notebook)	
 *
 * Description
 *    create studypad control
 *
 * Return value
 *   GtkWidget *
 */ 

GtkWidget *gui_create_studypad_control(GtkWidget *notebook)
{
	GtkWidget *vbox6;
	GtkWidget *vboxSP;
	GtkWidget *htmlwidget;	
	GtkWidget *frame34;
	GtkWidget *scrolledwindow17;
	GtkWidget *toolbar;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator19;
	GtkWidget *vseparator20;
	GtkWidget *vseparator21;
	GSHTMLEditorControlData *specd =
		gs_html_editor_control_data_new();
	
	
	specd->studypad = TRUE;

	specd->frame = gtk_frame_new(NULL);
	gtk_widget_ref(specd->frame);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->frame",
				 specd->frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->frame);
	gtk_container_add(GTK_CONTAINER(notebook), specd->frame);

	vbox6 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox6);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vbox6", vbox6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox6);
	gtk_container_add(GTK_CONTAINER(specd->frame), vbox6);

	vboxSP = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxSP);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vboxSP", vboxSP,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vboxSP);
	gtk_box_pack_start(GTK_BOX(vbox6), vboxSP, TRUE, TRUE, 0);

	htmlwidget = gtk_html_new();
	
	specd->frame_toolbar = gtk_frame_new(NULL);
	gtk_widget_ref(specd->frame_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "specd->frame_toolbar",
				 specd->frame_toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	if (specd->studypad)
		gtk_widget_show(specd->frame_toolbar);
	gtk_box_pack_start(GTK_BOX(vboxSP), specd->frame_toolbar, FALSE,
			   TRUE, 0);

	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "toolbar", toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_container_add(GTK_CONTAINER(specd->frame_toolbar), toolbar);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_OPEN);
	specd->btn_open =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Open"), _("Open File"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(specd->btn_open);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->btn_open",
				 specd->btn_open,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_open);

	gtk_signal_connect(GTK_OBJECT(specd->btn_open), "clicked",
		   GTK_SIGNAL_FUNC(on_btn_open_clicked),
		   specd);		
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_SAVE);
	specd->btn_save =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save"), _("Save Note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(specd->btn_save);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->btn_save",
				 specd->btn_save,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_save);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_PRINT);
	specd->btn_print =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"), _("Print Note"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(specd->btn_print);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->btn_print",
				 specd->btn_print,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_print);

	vseparator19 = gtk_vseparator_new();
	gtk_widget_ref(vseparator19);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vseparator19",
				 vseparator19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator19);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator19,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator19, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_CUT);
	specd->btn_cut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Cut"), _("Cut "), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(specd->btn_cut);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->btn_cut",
				 specd->btn_cut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_cut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_COPY);
	specd->btn_copy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Copy"), _("Copy"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(specd->btn_copy);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->btn_copy",
				 specd->btn_copy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_copy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_PASTE);
	specd->btn_paste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Paste"), _("Paste"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(specd->btn_paste);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->btn_paste",
				 specd->btn_paste,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_paste);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_UNDO);
	specd->btn_undo =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Undo"), _("Undo"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(specd->btn_undo);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->btn_undo",
				 specd->btn_undo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_undo);

	vseparator20 = gtk_vseparator_new();
	gtk_widget_ref(vseparator20);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vseparator20",
				 vseparator20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator20);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator20,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator20, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_SEARCH);
	specd->btn_Find =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Find"),
				       _("Find in this note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(specd->btn_Find);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->btn_Find",
				 specd->btn_Find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_Find);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_SRCHRPL);
	specd->btn_replace =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Replace"),
				       _("Find and Replace"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(specd->btn_replace);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "specd->btn_replace", specd->btn_replace,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_replace);

	vseparator21 = gtk_vseparator_new();
	gtk_widget_ref(vseparator21);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vseparator21",
				 vseparator21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator21);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator21,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator21, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	specd->btn_spell =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Spell"),
				       _("Spell check note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(specd->btn_spell);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->btn_spell",
				 specd->btn_spell,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->btn_spell);
	
#ifdef USE_SPELL	
	gtk_widget_set_sensitive(specd->btn_spell, 1);
#else
	gtk_widget_set_sensitive(specd->btn_spell, 0);
#endif

	frame34 = gtk_frame_new(NULL);
	gtk_widget_ref(frame34);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "frame34", frame34,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame34);
	gtk_box_pack_start(GTK_BOX(vboxSP), frame34, TRUE, TRUE, 0);

	scrolledwindow17 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow17);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "scrolledwindow17", scrolledwindow17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow17);
	gtk_container_add(GTK_CONTAINER(frame34), scrolledwindow17);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow17),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	specd->htmlwidget = htmlwidget;
	specd->html = GTK_HTML(specd->htmlwidget);
	gtk_widget_ref(specd->htmlwidget);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "specd->htmlwidget", specd->htmlwidget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->htmlwidget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow17),
			  specd->htmlwidget);
	gtk_html_load_empty(specd->html);
	
	
	
	
	
	specd->statusbar = gtk_statusbar_new();
	gtk_widget_ref(specd->statusbar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "specd->statusbar",
				 specd->statusbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(specd->statusbar);
	gtk_box_pack_start(GTK_BOX(vboxSP), specd->statusbar, FALSE, TRUE,
			   0);
			   
	specd->vbox = vboxSP;
	specd->pm = gui_create_editor_popup(specd);
	gnome_popup_menu_attach(specd->pm, specd->htmlwidget, NULL);
	
	gtk_signal_connect(GTK_OBJECT(specd->html), "submit",
			   GTK_SIGNAL_FUNC(on_submit), specd);
	gtk_signal_connect(GTK_OBJECT
			   (specd->htmlwidget),
			   "load_done",
			   GTK_SIGNAL_FUNC(html_load_done), specd);
	gtk_signal_connect(GTK_OBJECT
			   (specd->htmlwidget),
			   "key_press_event",
			   GTK_SIGNAL_FUNC(html_key_pressed), specd);
	gtk_signal_connect(GTK_OBJECT
			   (specd->htmlwidget),
			   "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), specd);
	gtk_signal_connect(GTK_OBJECT(specd->htmlwidget),
			   "enter_notify_event",
			   GTK_SIGNAL_FUNC(on_html_enter_notify_event),
			   specd);
	gtk_signal_connect(GTK_OBJECT(specd->htmlwidget), "link_clicked", GTK_SIGNAL_FUNC(gui_link_clicked),	/* gs_html.c */
			   NULL);
	gtk_signal_connect(GTK_OBJECT(specd->htmlwidget), "on_url", GTK_SIGNAL_FUNC(gui_url),	/* gs_html.c */
			   NULL);

	gtk_signal_connect(GTK_OBJECT(specd->btn_save), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_save_clicked), specd);
	gtk_signal_connect(GTK_OBJECT(specd->btn_print), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_print_clicked), specd);
	gtk_signal_connect(GTK_OBJECT(specd->btn_cut), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_cut_clicked), specd);
	gtk_signal_connect(GTK_OBJECT(specd->btn_copy), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_copy_clicked), specd);
	gtk_signal_connect(GTK_OBJECT(specd->btn_paste), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_paste_clicked), specd);
	gtk_signal_connect(GTK_OBJECT(specd->btn_undo), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_undo_clicked), specd);
	gtk_signal_connect(GTK_OBJECT(specd->btn_Find), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_Find_clicked), specd);
	gtk_signal_connect(GTK_OBJECT(specd->btn_replace), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_replace_clicked),
			   specd);
#ifdef USE_SPELL
	gtk_signal_connect(GTK_OBJECT(specd->btn_spell), "clicked",
			   GTK_SIGNAL_FUNC(spell_check_cb), specd);
#endif
	
	

	widgets.toolbar_studypad = toolbar_style(specd);
	gtk_widget_hide(widgets.toolbar_studypad);

	if (settings.studypadfilename) {
		load_file(settings.studypadfilename, specd);
	}
	editor_cd = specd;	
	return htmlwidget;
}


