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

#include "gs_editor.h"
#include "fileselection.h"

#define BUFFER_SIZE 8192	/* input buffer size */

/******************************************************************************
 * Name
 *   open_ok
 *
 * Synopsis
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
	gtk_widget_destroy(filesel);
	save_file(ecd->filename, ecd);
}

/******************************************************************************
 * Name
 *   save_cancel
 *
 * Synopsis
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
 *   #include "fileselection.h"
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
		gtk_file_selection_new(_("GnomeSword - Open Note File"));
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
 *   gui_fileselection_save
 *
 * Synopsis
 *   #include "fileselection.h"
 *   
 *   void gui_fileselection_save(GSHTMLEditorControlData *ecd);
 *
 * Description
 *   Fileselection dialog
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_fileselection_save(GSHTMLEditorControlData *ecd)
{
	GtkWidget *fileselection;
	GtkWidget *ok;
	GtkWidget *cancel;

	fileselection =
		gtk_file_selection_new(_("GomeSword - Save Note File"));
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

	gtk_signal_connect(GTK_OBJECT(ok), "clicked",
			GTK_SIGNAL_FUNC(save_ok), ecd);
	gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
		       GTK_SIGNAL_FUNC(save_cancel), NULL);

	return fileselection;
}
