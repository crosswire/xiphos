/*
 * GnomeSword Bible Study Tool
 * editor.c - html editor for personal comments and studypad 
 *               (generic book support - to be added)
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
#endif

#include "gs_gnomesword.h"
#include "fileselection.h"
#include "gs_html.h"
#include "gs_editor.h"
#include "gs_info_box.h"
#include "support.h"

/* gnome */
#include "_editor.h"
#include "editor_toolbar.h"
#include "editor_menu.h"
#include "link_dialog.h"

/* main */
#include "percomm.h"
#include "settings.h"


GSHTMLEditorControlData *gs_html_editor_control_data_new(SETTINGS * s)
{
	GSHTMLEditorControlData *necd =
	    g_new0(GSHTMLEditorControlData, 1);

	necd->paragraph_option = NULL;
	necd->properties_types = NULL;
	necd->block_font_style_change = FALSE;
	necd->gdk_painter = NULL;
	necd->plain_painter = NULL;
	necd->format_html = FALSE;
	necd->changed = FALSE;
	necd->gbs = FALSE;
	necd->personal_comments = FALSE;
	necd->studypad = FALSE;
	sprintf(necd->filename, "%s", s->studypadfilename);
	return necd;
}


void gs_html_editor_control_data_destroy(GSHTMLEditorControlData * ecd)
{
	g_assert(ecd);

	if (ecd->plain_painter)
		gtk_object_unref(GTK_OBJECT(ecd->plain_painter));

	if (ecd->gdk_painter)
		gtk_object_unref(GTK_OBJECT(ecd->gdk_painter));

	g_free(ecd);
}




void update_statusbar(GSHTMLEditorControlData * ecd)
{
	gint context_id2;
	gchar buf[255];
	gchar *buf2;

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(ecd->statusbar),
					 "GnomeSword");
	gtk_statusbar_pop(GTK_STATUSBAR(ecd->statusbar), context_id2);

	if (ecd->personal_comments)
		buf2 = settings.percomverse;
			
	else
		buf2 = ecd->filename;

	if (ecd->changed) {
		sprintf(buf, "%s - modified", buf2);
		if(!ecd->personal_comments && !ecd->gbs)
			settings.modifiedSP = TRUE;
		if(ecd->personal_comments)
			settings.modifiedPC = TRUE;
		if(ecd->gbs)
			settings.modifiedGBS = TRUE;
	} else {
		sprintf(buf, "%s", buf2);
		if(!ecd->personal_comments && !ecd->gbs)
			settings.modifiedSP = FALSE;
		if(ecd->personal_comments)
			settings.modifiedPC = FALSE;
		if(ecd->gbs)
			settings.modifiedGBS = FALSE;
	} 

	gtk_statusbar_push(GTK_STATUSBAR(ecd->statusbar), context_id2,
			   buf);
}


/******************************************************************* html file stuff - load and save */
#define BUFFER_SIZE 4096
/*** load studypad file ***/
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

GString *gstr;
static gboolean
save_note_receiver(const HTMLEngine * engine,
		   const char *data, unsigned int len, void *user_data)
{
	static gboolean startgrabing = FALSE;
	if (!strncmp(data, "</BODY>", 7))
		startgrabing = FALSE;
	if (startgrabing) {
		gstr = g_string_append(gstr, data);
		//g_warning(gstr->str);
	}
	if (strstr(data, "<BODY") != NULL)
		startgrabing = TRUE;

	return TRUE;
}

/******************************************************************************
 * save studypad file
******************************************************************************/
static gboolean
save_receiver(const HTMLEngine * engine,
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

void editor_save_note(GtkWidget * html_widget)
{
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	gtk_html_set_editable(html, FALSE);
	gstr = g_string_new("");
	if (!gtk_html_save
	    (html, (GtkHTMLSaveReceiverFn) save_note_receiver,
	     GINT_TO_POINTER(0))) {
		g_warning("file not writen");
	} else {
		save_percomm_note(gstr->str);
		g_print("\nfile writen\n");
	}
	g_string_free(gstr, 1);
	gtk_html_set_editable(html, TRUE);
}

void savebookEDITOR(GtkWidget * html_widget)
{
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	gtk_html_set_editable(html, FALSE);
	gstr = g_string_new("");
	if (!gtk_html_save
	    (html, (GtkHTMLSaveReceiverFn) save_note_receiver,
	     GINT_TO_POINTER(0))) {
		g_warning("file not writen");
	} else {
		g_print("file writen");
		//savebookSW_GBS(gstr->str);
	}
	g_string_free(gstr, 1);
	gtk_html_set_editable(html, TRUE);
}


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

/************************************************************************************ spell checking stuff */
#ifdef USE_SPELL
void find_word_EDITOR(gchar * word, GSHTMLEditorControlData * ecd)
{
	html_engine_search(ecd->html->engine, word, 0,	/* not case sen */
			   1,	/* foward search */
			   0);	/* not regex */
}

void load_text_from_spell_EDITOR(GtkWidget * text,
				 GSHTMLEditorControlData * ecd)
{
	gchar *buf, *utf8str;
	GtkHTMLStream *htmlstream;
	GtkHTMLStreamStatus status1 = 0;
	gboolean was_editable;

	buf =
	    gtk_editable_get_chars(GTK_EDITABLE(GTK_TEXT(text)), 0, -1);

	was_editable = gtk_html_get_editable(ecd->html);
	utf8str = e_utf8_from_gtk_string(ecd->htmlwidget, buf);
	if (was_editable)
		gtk_html_set_editable(ecd->html, FALSE);

	htmlstream = gtk_html_begin_content(ecd->html,
					    "text/html; charset=utf-8");
	gtk_html_write(ecd->html, htmlstream, utf8str, strlen(utf8str));
	gtk_html_end(ecd->html, htmlstream, status1);

	gtk_html_set_editable(ecd->html, was_editable);
}


gboolean load_text_for_spell_EDITOR(GtkWidget * text,
				    GSHTMLEditorControlData * ecd)
{
	gtk_html_set_editable(ecd->html, FALSE);
	gstr = g_string_new("");
	if (!gtk_html_save
	    (ecd->html, (GtkHTMLSaveReceiverFn) save_note_receiver,
	     GINT_TO_POINTER(0)))
		g_warning("file not writen");
	else
		g_warning("file writen");
	//g_warning(gstr->str);

	gtk_text_insert(GTK_TEXT(text), NULL,
			&text->style->black, NULL, gstr->str, -1);
	g_string_free(gstr, 1);
	gtk_html_set_editable(ecd->html, TRUE);
	return TRUE;
}
#endif				/* USE_SPELL */

void on_editor_destroy(GtkObject * object,
		       GSHTMLEditorControlData * ecd)
{
	gs_html_editor_control_data_destroy(ecd);
}

/************************************************************************************ studypad editor control */
/*** create studypad editor control ***/
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

/************************************************************************************ studypad editor control */
/*** create studypad editor control ***/
GtkWidget *gbs_control(GtkWidget * notebook, SETTINGS * s)
{
	GtkWidget *frame12;
	GtkWidget *vbox6;
	GtkWidget *vboxSP;
	GtkWidget *htmlwidget;
	GSHTMLEditorControlData *gbsecd =
		gs_html_editor_control_data_new(s);

	
	gbsecd->gbs = TRUE;

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
	gui_create_html_editor(htmlwidget, vboxSP, s, gbsecd);

	s->toolbarBooks = toolbar_style(gbsecd);
	gtk_widget_hide(s->toolbarBooks);

	return htmlwidget;
}

void
run_dialog(GnomeDialog *** dialog, GtkHTML * html, DialogCtor ctor,
	   const gchar * title)
{
	if (*dialog) {
		gtk_window_set_title(GTK_WINDOW(**dialog), title);
		gtk_widget_show(GTK_WIDGET(**dialog));
		gdk_window_raise(GTK_WIDGET(**dialog)->window);
	} else {
		*dialog = ctor(html);
		gtk_window_set_title(GTK_WINDOW(**dialog), title);
		gtk_widget_show(GTK_WIDGET(**dialog));
	}
}
