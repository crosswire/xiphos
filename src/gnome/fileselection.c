/*
 * GnomeSword Bible Study Tool
 * fileselection.c - SHORT DESCRIPTION
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
#include <stdio.h>

#include "gui/editor.h"
#include "gui/fileselection.h"
#include "gui/studypad.h"

#include "main/settings.h"
#include "main/xml.h"

#define BUFFER_SIZE 8192	/* input buffer size */

 
/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include "gui/fileselection.h"
 *   
 *   void dialog_destroy(GtkObject * object, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_destroy(GtkObject * object, gpointer user_data)
{
	gtk_main_quit();
}


/******************************************************************************
 * Name
 *   open_ok
 *
 * Synopsis
 *   #include "gui/fileselection.h"
 *   
 *   void open_ok(GtkButton *button, GSHTMLEditorControlData ecd);
 *
 * Description
 *   OK button for open fileselection was clicked.
 *
 * Return value
 *   void
 */

static void open_ok(GtkButton *button, GSHTMLEditorControlData *ecd)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	sprintf(ecd->filename, "%s",
			gtk_file_selection_get_filename(
				GTK_FILE_SELECTION(filesel)));
	load_file(ecd->filename, ecd);
	gtk_widget_destroy(filesel);
}


/******************************************************************************
 * Name
 *   open_cancel
 *
 * Synopsis
 *   #include "gui/fileselection.h"
 *   
 *   void open_cancel(GtkButton *button, GSHTMLEditorControlData ecd);
 *
 * Description
 *   Cancel button for open fileselection was clicked.
 *
 * Return value
 *   void
 */

static void open_cancel(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


/******************************************************************************
 * Name
 *   save_ok
 *
 * Synopsis
 *   #include "gui/fileselection.h"
 *   
 *   void save_ok(GtkButton *button, GSHTMLEditorControlData ecd);
 *
 * Description
 *   OK button for save fileselection was clicked.
 *
 * Return value
 *   void
 */

static void save_ok(GtkButton * button, GSHTMLEditorControlData * ecd)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	sprintf(ecd->filename, "%s", gtk_file_selection_get_filename(
				GTK_FILE_SELECTION(filesel)));
	settings.studypadfilename = ecd->filename;
	xml_set_value("GnomeSword", "studypad", "lastfile", 
							ecd->filename);
	gtk_widget_destroy(filesel);
	save_file(ecd->filename, ecd);
}


/******************************************************************************
 * Name
 *   save_cancel
 *
 * Synopsis
 *   #include "gui/fileselection.h"
 *   
 *   void save_cancel(GtkButton *button, GSHTMLEditorControlData ecd);
 *
 * Description
 *   Cancel button for save fileselection was clicked.
 *
 * Return value
 *   void
 */

static void save_cancel(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


/******************************************************************************
 * Name
 *   gui_fileselection_open
 *
 * Synopsis
 *   #include "gui/fileselection.h"
 *   
 *   void gui_fileselection_open(GSHTMLEditorControlData *ecd);
 *
 * Description
 *   Fileselection dialog
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_fileselection_open(GSHTMLEditorControlData * ecd)
{
	GtkWidget *fileselection;
	GtkWidget *ok;
	GtkWidget *cancel;

	fileselection =
		gtk_file_selection_new(_("Open StudyPad File"));
	gtk_object_set_data(GTK_OBJECT(fileselection), "fileselection",
			fileselection);
	gtk_container_set_border_width(GTK_CONTAINER(fileselection), 10);
	gtk_file_selection_hide_fileop_buttons(
			GTK_FILE_SELECTION(fileselection));

	ok = GTK_FILE_SELECTION(fileselection)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselection), "ok", ok);
	gtk_widget_show(ok);
	GTK_WIDGET_SET_FLAGS(ok, GTK_CAN_DEFAULT);

	cancel = GTK_FILE_SELECTION(fileselection)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselection), "cancel", cancel);
	gtk_widget_show(cancel);
	GTK_WIDGET_SET_FLAGS(cancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(ok), "clicked",
			GTK_SIGNAL_FUNC(open_ok), ecd);
	gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
			GTK_SIGNAL_FUNC(open_cancel), NULL);

	return fileselection;
}


/******************************************************************************
 * Name
 *   create_fileselection_save
 *
 * Synopsis
 *   #include "gui/fileselection.h"
 *   
 *   void create_fileselection_save(GSHTMLEditorControlData *ecd);
 *
 * Description
 *   create Fileselection dialog
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_fileselection_save(
					GSHTMLEditorControlData *ecd)
{
	GtkWidget *fileselection;
	GtkWidget *ok;
	GtkWidget *cancel;

	fileselection =
		gtk_file_selection_new(_("GomeSword - Save StudyPad File"));
	gtk_object_set_data(GTK_OBJECT(fileselection), "fileselection",
			fileselection);
	gtk_container_set_border_width(GTK_CONTAINER(fileselection), 10);

	ok = GTK_FILE_SELECTION(fileselection)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselection), "ok", ok);
	gtk_widget_show(ok);
	GTK_WIDGET_SET_FLAGS(ok, GTK_CAN_DEFAULT);

	cancel = GTK_FILE_SELECTION(fileselection)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselection), "cancel", cancel);
	gtk_widget_show(cancel);
	GTK_WIDGET_SET_FLAGS(cancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(fileselection), "destroy",
			   GTK_SIGNAL_FUNC(dialog_destroy), NULL);
	gtk_signal_connect(GTK_OBJECT(ok), "clicked",
			GTK_SIGNAL_FUNC(save_ok), ecd);
	gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
		       GTK_SIGNAL_FUNC(save_cancel), NULL);

	gtk_widget_show_all(fileselection);
	return fileselection;
}


/******************************************************************************
 * Name
 *   gui_fileselection_save
 *
 * Synopsis
 *   #include "gui/fileselection.h"
 *   
 *   gint gui_fileselection_save(GSHTMLEditorControlData *ecd);
 *
 * Description
 *   run Fileselection dialog
 *
 * Return value
 *   gint
 */

gint gui_fileselection_save(GSHTMLEditorControlData *ecd)
{
	GtkWidget *window;
	static gboolean is_running = FALSE;
	gint retval;
	
	if(!is_running) {
		gchar buf[256];
		window = create_fileselection_save(ecd);
		sprintf(buf, "%s/.pad", settings.homedir);
		gtk_file_selection_set_filename(GTK_FILE_SELECTION
						(window), buf);
		retval = 4;
		is_running = TRUE;
		gtk_main();
//		g_warning("fileselection dialog closed");
		is_running = FALSE;
		return retval;
	}
	return 4;
	
	
}
