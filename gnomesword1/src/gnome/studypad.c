/*
 * GnomeSword Bible Study Tool
 * editor.c - studypad gui
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
#include "spell.h"
#include "spell_gui.h"
#endif

/* gnome */
#include "studypad.h"
#include "_editor.h"
#include "editor_toolbar.h"
#include "editor_menu.h"
#include "link_dialog.h"

/* main */
#include "gs_gnomesword.h"
#include "fileselection.h"
#include "support.h"
#include "settings.h"


#define BUFFER_SIZE 4096

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

	sprintf(settings.studypadfilename, "%s", filename);
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
		}

		close(fd);
	}
	return retval;
}

/******************************************************************************
 * Name
 *  studypad_control
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   GtkWidget *studypad_control(GtkWidget * notebook, SETTINGS * s)	
 *
 * Description
 *    create studypad control
 *
 * Return value
 *   GtkWidget *
 */ 

GtkWidget *studypad_control(GtkWidget * notebook, SETTINGS * s)
{
	GtkWidget *frame12;
	GtkWidget *vbox6;
	GtkWidget *vboxSP;
	GtkWidget *htmlwidget;
	GSHTMLEditorControlData *specd =
		gs_html_editor_control_data_new(s);
	
	specd->studypad = TRUE;

	frame12 = gtk_frame_new(NULL);
	gtk_widget_ref(frame12);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame12",
				 frame12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame12);
	gtk_container_add(GTK_CONTAINER(notebook), frame12);

	vbox6 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox6);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox6", vbox6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox6);
	gtk_container_add(GTK_CONTAINER(frame12), vbox6);

	vboxSP = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxSP);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vboxSP", vboxSP,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vboxSP);
	gtk_box_pack_start(GTK_BOX(vbox6), vboxSP, TRUE, TRUE, 0);

	htmlwidget = gtk_html_new();
	gui_create_html_editor(htmlwidget, vboxSP, s, specd);

	s->toolbarStudypad = toolbar_style(specd);
	gtk_widget_hide(s->toolbarStudypad);
	if (settings.studypadfilename)
		load_file(settings.studypadfilename, specd);

	return htmlwidget;
}


