/*
 * GnomeSword Bible Study Tool
 * export_dialog.c - 
 *
 * Copyright (C) 2008 GnomeSword Developer Team
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
#ifdef USE_EXPORTER

#include <gnome.h>
#include <glade/glade-xml.h>

#include "gui/export_dialog.h"
#include "gui/utilities.h"


#include "main/export_passage.h"
#include "main/settings.h"
#include "main/sword.h"


static GtkWidget *dialog;
static gchar *filename;


typedef struct _export_dialog EXPORT_DIALOG;
struct _export_dialog {
	GtkWidget *rb_book;
	GtkWidget *rb_chapter;
	GtkWidget *rb_verse;
	GtkWidget *rb_html;
	GtkWidget *rb_plain;
	GtkWidget *lb_version;
	GtkWidget *lb_key;
	gint format;
	gint passage_type;
};

EXPORT_DIALOG d;

static void _read_buttons(void)
{
	
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_book));
	
}

static
void on_filechooserdialog_response(GtkDialog * fdialog,
					    gint response_id,
					    GtkFileChooser * filesel)
{
	switch (response_id) {
	case GTK_RESPONSE_OK:
		filename = g_strdup(gtk_file_chooser_get_filename(filesel));
		
		if(d.format)
			main_export_html(filename, d.passage_type);
		else
			main_export_plain(filename, d.passage_type);
		break;

	case GTK_RESPONSE_CANCEL:
		break;

	}
	gtk_widget_destroy(GTK_WIDGET(fdialog));
}

void _get_export_filename(void)
{
	gchar *glade_file;
	GladeXML *gxml;
	GtkWidget *fdialog;
	filename = NULL;
	
	glade_file = gui_general_user_file("export-dialog.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
	GS_message((glade_file));

	/* build the widget */
	gxml = glade_xml_new(glade_file, "filechooserdialog1", NULL);
	
	fdialog =  glade_xml_get_widget (gxml, "filechooserdialog1");	
	g_signal_connect(fdialog, "response",
			 G_CALLBACK(on_filechooserdialog_response), (GtkFileChooser *)fdialog);	
	
}

/******************************************************************************
 * Name
 *   on_mod_mgr_response
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_mod_mgr_response(GtkDialog * dialog, gint response_id, gpointer user_data)
 *
 * Description
 *   these are local defines at the top of this file.
 *
 * Return value
 *   void
 */

void on_dialog_export_passage_response(GtkDialog * dialog,
			 gint response_id,
			 gpointer user_data)
{
	gchar *filename = NULL;
	
	switch (response_id) {
	case GTK_RESPONSE_CANCEL:		
		gtk_widget_destroy(GTK_WIDGET(dialog));	
		break;
	case GTK_RESPONSE_OK:
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_html)))
			d.format = 1;
		else
			d.format = 0;
			
		d.passage_type = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_book)) ? BOOK
				 : gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_chapter)) ? CHAPTER
				 : VERSE;
		
		_get_export_filename();
		gtk_widget_destroy(GTK_WIDGET(dialog));
		break;
	}
}


/******************************************************************************
 * Name
 *   _connect_signals
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void _connect_signals(NAVBAR_VERSEKEY navbar)
 *
 * Description
 *   
 *
 * Return value
 *  void 
 */

static
void _connect_signals(NAVBAR_VERSEKEY navbar)
{
		

}

/******************************************************************************
 * Name
 *  gui_navbar_versekey_new
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *  GtkWidget *gui_navbar_book_new(void)
 *
 * Description
 *   create a new Bible navigation toolbar and return it
 *
 * Return value
 *   GtkWidget *
 */

void gui_export_dialog(void)
{

	gchar *glade_file;
	GladeXML *gxml;
	
	glade_file = gui_general_user_file("export-dialog.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
	GS_message((glade_file));

	/* build the widget */
	gxml = glade_xml_new(glade_file, "dialog_export_passage", NULL);
	
	dialog =  glade_xml_get_widget (gxml, "dialog_export_passage");	
	g_signal_connect(dialog, "response",
			 G_CALLBACK(on_dialog_export_passage_response), NULL);
	
	d.rb_book = glade_xml_get_widget(gxml, "radiobutton1");
	d.rb_chapter = glade_xml_get_widget(gxml, "radiobutton2");
	d.rb_verse = glade_xml_get_widget(gxml, "radiobutton3");
	d.rb_html = glade_xml_get_widget(gxml, "radiobutton4");
	d.rb_plain = glade_xml_get_widget(gxml, "radiobutton5");
	d.lb_version = glade_xml_get_widget(gxml, "label3");
	d.lb_key = glade_xml_get_widget(gxml, "label4");
	
	gtk_label_set_text (GTK_LABEL(d.lb_version), settings.MainWindowModule);	
	gtk_label_set_text (GTK_LABEL(d.lb_key), settings.currentverse);

	
}
#endif
