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

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "editor/editor.h"
#include "editor/popup.h"
#include "editor/spell.h"
#include "editor/editor-control.h"
//#include "editor/toolbar_edit.h"
#include "gui/gnomesword.h"
#include "gui/studypad.h"
#include "editor/editor_menu.h"
#include "gui/html.h"
#include "gui/dialog.h"
#include "gui/fileselection.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/xml.h"

#define BUFFER_SIZE 4096


static GtkHTMLEditorAPI *editor_api;

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
	
//	g_warning(filename);
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
	gui_studypad_can_close();
	//settings.studypadfilename = filename;
	xml_set_value("GnomeSword", "studypad", "lastfile", filename);
	settings.studypadfilename = xml_get_value("studypad", "lastfile");
	
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
 *   dialog_destroy
 *
 * Synopsis
 *   #include "studypad_dialog.h"
 *
 *   void dialog_destroy(GtkObject *object, DL_DIALOG * dlg)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static gboolean dialog_destroy (GtkObject *object, gpointer data)
{	
	widgets.studypad_dialog = NULL;
	settings.studypad_dialog_exist = FALSE;	
	return TRUE;
}


/******************************************************************************
 * Name
 *  create_dialog
 *
 * Synopsis
 *   #include "studypad_dialog.h"
 *
 *   GtkWidget *create_dialog(void)	
 *
 * Description
 *    
 *
 * Return value
 *   
 */

static GtkWidget *create_dialog(void)
{
	GtkWidget *dialog;
	dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(dialog),"dialog",dialog);
	gtk_widget_set_size_request(dialog, 600, 300);
	gtk_window_set_resizable(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_title((GtkWindow *)dialog,
                                (const gchar*)N_("StudyPad"));
	gtk_widget_show(dialog);
	
	g_signal_connect(G_OBJECT(dialog), "destroy",
                      G_CALLBACK(dialog_destroy),
			   NULL);	 
	
	return dialog;	
}

/******************************************************************************
 * Name
 *  gui_open_studypad
 *
 * Synopsis
 *   #include "studypad_dialog.h"
 *
 *   gint gui_open_studypad(gchar * file_name)	
 *
 * Description
 *    
 *
 * Return value
 *   
 */

gint gui_open_studypad(gchar * file_name)
{

	widgets.studypad_dialog = create_dialog();
	
	editor_new(widgets.studypad_dialog, 
					STUDYPAD, 
					file_name); 
}
