/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
     filebar.c  20001-04-24
     
    Copyright (C) 2001 Terry Biggs

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

/* most of the code (that does anything) in this file comes form gtkhtml */
#include <config.h>
#include <gnome.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <gal/widgets/widget-color-combo.h>
#include <htmlcolor.h>
#include <htmlcolorset.h>
#include <htmlengine-edit-fontstyle.h>
#include <htmlsettings.h>
#include <htmlengine-edit-cut-and-paste.h>
//#include <htmlengine-edit-cut.h>
//#include <htmlengine-edit-paste.h>
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>

#include "filebar.h"
#include "toolbar.h"
#include "utils.h"
#include "properties.h"
#include "text.h"
#include "./src/gssword/gs_sword.h" 
#include "gs_editor.h" 

#define BUFFER_SIZE 4096

gchar filename[240];
GString *gstr;


extern gboolean noteModified;
extern gboolean file_changed;	/* set to true if text is study pad has changed - and file is not saved  */
/******************************************************************************
 * updatestatusbar - 
 ******************************************************************************/
void updatestatusbar(GtkHTMLControlData * cd)
{ 
	gint context_id2;
	gchar buf[255];

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(cd->statusbar),
					 "GnomeSword");
	gtk_statusbar_pop(GTK_STATUSBAR(cd->statusbar), context_id2);
	if(cd->html_modified)
		sprintf(buf,"%s - modified",cd->filename);
	else 
		sprintf(buf,"%s",cd->filename);
	gtk_statusbar_push(GTK_STATUSBAR(cd->statusbar), context_id2, buf);
	if(cd->note_editor)
		noteModified = cd->html_modified;
	else
		file_changed = cd->html_modified;
	
}
static gboolean
save_note_receiver  (const HTMLEngine *engine,
		const char *data,
		unsigned int len,
		void *user_data)
{
	static gboolean startgrabing = FALSE;
		
	if(!strncmp(data,"</BODY>",7)) startgrabing = FALSE;
	if(startgrabing)
		gstr = g_string_append(gstr,data);
	if(!strcmp(data,"<BODY>")) startgrabing = TRUE;
	
	return TRUE;
}

static void
file_toolbar_new_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	//gtk_html_load_empty(cd->html);
}

static void 
loadFILE(GtkWidget *filesel, GtkHTMLControlData *cd)
{
	GtkHTMLStream *stream;
	char buffer[BUFFER_SIZE];
	ssize_t count;
	gboolean was_editable = TRUE;
	int fd;		
	gint context_id2;

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(cd->statusbar),
					 "GnomeSword");
	gtk_statusbar_pop(GTK_STATUSBAR(cd->statusbar), context_id2);
	sprintf(filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION
						(filesel)));
	//g_warning(filename);
	fd = open (filename, O_RDONLY);
	if (fd == -1)	
	was_editable = gtk_html_get_editable (cd->html);
	if (was_editable)
		gtk_html_set_editable (cd->html, FALSE);

	stream = gtk_html_begin (cd->html);
	if (stream == NULL) {
		close (fd);
		if (was_editable)
			gtk_html_set_editable (cd->html, TRUE);		
	}
	while (1) {
		count = read (fd, buffer, BUFFER_SIZE);
		if (count > 0)
			gtk_html_write (cd->html, stream, buffer, count);
		else
			break;
	}
	close (fd);
	if (count == 0) {
		gtk_html_end (cd->html, stream, GTK_HTML_STREAM_OK);
		if (was_editable)
			gtk_html_set_editable (cd->html, TRUE);
		cd->filename = filename;		
		gtk_statusbar_push(GTK_STATUSBAR(cd->statusbar), context_id2, filename);
	} else {
		gtk_html_end (cd->html, stream, GTK_HTML_STREAM_ERROR);
		if (was_editable)
			gtk_html_set_editable (cd->html, TRUE);
		gtk_statusbar_push(GTK_STATUSBAR(cd->statusbar), context_id2, "error loading file");
	}	
	gtk_widget_destroy(filesel);
	cd->html_modified = FALSE;
	updatestatusbar(cd);
}

static void on_ok_button1_clicked(GtkButton * button, GtkHTMLControlData *cd)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	loadFILE(filesel,cd);
	//gtk_widget_destroy(filesel);
}

static void on_cancel_button1_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

static
GtkWidget *create_fileselection1(GtkHTMLControlData *cd)
{
	GtkWidget *fileselection1;
	GtkWidget *ok_button1;
	GtkWidget *cancel_button1;

	fileselection1 =
	    gtk_file_selection_new("GnomeSword - Open Note File");
	gtk_object_set_data(GTK_OBJECT(fileselection1), "fileselection1",
			    fileselection1);
	gtk_container_set_border_width(GTK_CONTAINER(fileselection1), 10);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION
					       (fileselection1));

	ok_button1 = GTK_FILE_SELECTION(fileselection1)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselection1), "ok_button1",
			    ok_button1);
	gtk_widget_show(ok_button1);
	GTK_WIDGET_SET_FLAGS(ok_button1, GTK_CAN_DEFAULT);

	cancel_button1 = GTK_FILE_SELECTION(fileselection1)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselection1), "cancel_button1",
			    cancel_button1);
	gtk_widget_show(cancel_button1);
	GTK_WIDGET_SET_FLAGS(cancel_button1, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(ok_button1), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button1_clicked), cd);
	gtk_signal_connect(GTK_OBJECT(cancel_button1), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button1_clicked),
			   NULL);

	return fileselection1;
}

static void
file_toolbar_open_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	GtkWidget *dlg;
	
	dlg = create_fileselection1(cd);
	gtk_widget_show(dlg);
}

static gboolean
save_receiver  (const HTMLEngine *engine,
		const char *data,
		unsigned int len,
		void *user_data)
{
	int fd;

	fd = GPOINTER_TO_INT (user_data);

	while (len > 0) {
		ssize_t count;

		count = write (fd, data, len);
		if (count < 0)
			return FALSE;

		len -= count;
	}

	return TRUE;
}

static int
save(gchar *file, GtkHTMLControlData *cd)
{
	int retval;
	int fd;
	gint context_id2;
	gchar buf[255];

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(cd->statusbar),
					 "GnomeSword");
	gtk_statusbar_pop(GTK_STATUSBAR(cd->statusbar), context_id2);
	
	fd = open (file, O_WRONLY | O_CREAT, 0600);

	if (fd == -1)
		return -1;

	if (!gtk_html_save (cd->html, (GtkHTMLSaveReceiverFn)save_receiver, GINT_TO_POINTER (fd)))
		retval = -1;
	else {
		retval = 0;
		sprintf(buf,"%s - saved",filename);
		cd->filename = filename;
		gtk_statusbar_push(GTK_STATUSBAR(cd->statusbar), context_id2, buf);
	}
	cd->html_modified = FALSE;
	updatestatusbar(cd);
	close (fd);
	return retval;
}

static
void on_ok_button2_clicked(GtkButton * button, GtkHTMLControlData *cd)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	sprintf(filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION
						(filesel)));
	gtk_widget_destroy(filesel);
	save(filename,cd);
}

static
void on_cancel_button2_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));	
	gtk_widget_destroy(filesel);
}

static
GtkWidget *create_fileselectionSave(GtkHTMLControlData *cd)
{
	GtkWidget *fileselectionSave;
	GtkWidget *ok_button2;
	GtkWidget *cancel_button2;

	fileselectionSave =
	    gtk_file_selection_new("GnomeSword - Save StudyPad File");
	gtk_object_set_data(GTK_OBJECT(fileselectionSave),
			    "fileselectionSave", fileselectionSave);
	gtk_container_set_border_width(GTK_CONTAINER(fileselectionSave),
				       10);

	ok_button2 = GTK_FILE_SELECTION(fileselectionSave)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselectionSave), "ok_button2",
			    ok_button2);
	gtk_widget_show(ok_button2);
	GTK_WIDGET_SET_FLAGS(ok_button2, GTK_CAN_DEFAULT);

	cancel_button2 =
	    GTK_FILE_SELECTION(fileselectionSave)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselectionSave),
			    "cancel_button2", cancel_button2);
	gtk_widget_show(cancel_button2);
	GTK_WIDGET_SET_FLAGS(cancel_button2, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button2_clicked), cd);
	gtk_signal_connect(GTK_OBJECT(cancel_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button2_clicked),
			   NULL);

	return fileselectionSave;
}

static void
file_toolbar_save_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if(*filename)
		save(filename, cd);
	else {
		GtkWidget *dlg;	
		dlg = create_fileselectionSave(cd);
		gtk_widget_show(dlg);		
	}
}

static void
file_toolbar_saveas_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	GtkWidget *dlg;
	
	dlg = create_fileselectionSave(cd);
	gtk_widget_show(dlg);
}

static gint page_num;
static GnomeFont *font;

static void
print_footer (GtkHTML *html, GnomePrintContext *context,
	      gdouble x, gdouble y, gdouble width, gdouble height, gpointer user_data)
{
	gchar *text = g_strdup_printf ("- %d -", page_num);
	gdouble tw = gnome_font_get_width_string (font, "text");

	if (font) {
		gnome_print_newpath     (context);
		gnome_print_setrgbcolor (context, .0, .0, .0);
		gnome_print_moveto      (context, x + (width - tw)/2, y - (height + gnome_font_get_ascender (font))/2);
		gnome_print_setfont     (context, font);
		gnome_print_show        (context, text);
	}

	g_free (text);
	page_num++;
}

static void
file_toolbar_print_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	GnomePrintMaster *print_master;
	GnomePrintContext *print_context;
	GtkWidget *preview;
	
	//html = GTK_HTML(lookup_widget(MainFrm, (gchar *) data));
	print_master = gnome_print_master_new ();
	/*  gnome_print_master_set_paper (master, gnome_paper_with_name ("A4")); */

	print_context = gnome_print_master_get_context (print_master);

	page_num = 1;
	font = gnome_font_new_closest ("Helvetica", GNOME_FONT_BOOK, FALSE, 12);
	gtk_html_print_with_header_footer (cd->html, print_context, .0, .03, NULL, print_footer, NULL);
	if (font) gtk_object_unref (GTK_OBJECT (font));

	preview = GTK_WIDGET (gnome_print_master_preview_new (print_master, "GnomeSword Print Preview"));
	gtk_widget_show (preview);

	gtk_object_unref (GTK_OBJECT (print_master));
}

static void
file_toolbar_sync_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	
}

static void
file_toolbar_savenote_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cd->code_view_button))) {
		gtk_html_set_editable(cd->html,FALSE); 
		gstr = g_string_new("");
		if (!gtk_html_save(cd->html, (GtkHTMLSaveReceiverFn)save_note_receiver, GINT_TO_POINTER (0)))
			g_warning("file not writen");		
		else
			g_warning("file writen");
		g_warning(gstr->str);
		savenoteSWORD(gstr->str,cd->html_modified);
		g_string_free(gstr,1);
		gtk_html_set_editable(cd->html,TRUE); 
	} else {
		gchar *buf;
		buf = gtk_editable_get_chars((GtkEditable *)cd->gtktext,0,-1); //-- get comments from text widget
		savenoteSWORD(buf,cd->html_modified);
	}
	cd->html_modified = FALSE;
	updatestatusbar(cd);
}

/*  add by tb 2001-04-18  */
static void
file_toolbar_code_cb(GtkWidget *widget,
			    GtkHTMLControlData *cd)
{		
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
		gtk_html_set_editable(cd->html,FALSE); 
		gstr = g_string_new("");
		if (!gtk_html_save(cd->html, (GtkHTMLSaveReceiverFn)save_note_receiver, GINT_TO_POINTER (0)))
			g_warning("file not writen");		
		else {
			//g_warning("file writen");
			//g_warning(gstr->str);
			gtk_text_set_point(GTK_TEXT(cd->gtktext), 0);
			gtk_text_forward_delete(GTK_TEXT(cd->gtktext),
							gtk_text_get_length((GTK_TEXT(cd->gtktext))));
			gtk_text_freeze(GTK_TEXT(cd->gtktext));
			gtk_text_insert(GTK_TEXT(cd->gtktext), NULL, NULL, NULL,gstr->str , -1);
			//gtk_text_set_point(GTK_TEXT(cd->gtktext), curPos);
			gtk_text_thaw(GTK_TEXT(cd->gtktext));
				//savenoteSWORD(gstr->str);
			g_string_free(gstr,1);
			gtk_html_set_editable(cd->html,TRUE); 	
			gtk_notebook_set_page(GTK_NOTEBOOK(cd->notebook),1);
		}
	}
	else {
		gchar *buf;
		GtkHTMLStream *htmlstream;
		GtkHTMLStreamStatus status1;
		gboolean was_editable;
		
		was_editable = gtk_html_get_editable (cd->html);
		if (was_editable)
			gtk_html_set_editable (cd->html, FALSE);
		htmlstream = gtk_html_begin(cd->html);
		
		buf = gtk_editable_get_chars(GTK_EDITABLE(cd->gtktext),
					0,
					gtk_text_get_length(GTK_TEXT(cd->gtktext)));
		
		if (strlen(buf)) 
			gtk_html_write(cd->html, htmlstream, buf, strlen(buf));
	
		gtk_html_end(cd->html, htmlstream, status1);
		if (was_editable)
			gtk_html_set_editable (cd->html, TRUE);
		gtk_notebook_set_page(GTK_NOTEBOOK(cd->notebook),0);	
	}
}

static void
file_toolbar_deletenote_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	deletenoteSWORD() ;
}

static GnomeUIInfo file_toolbar_file_note_uiinfo[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("Sync"), N_("Follow Main Text Window"),
				file_toolbar_sync_cb, GNOME_STOCK_PIXMAP_JUMP_TO),
	GNOMEUIINFO_ITEM_STOCK (N_("Save"), N_("Save Note"),
				file_toolbar_savenote_cb, GNOME_STOCK_PIXMAP_ADD),
	GNOMEUIINFO_ITEM_STOCK (N_("Delete"), N_("Delete Note"),
				file_toolbar_deletenote_cb, GNOME_STOCK_PIXMAP_REMOVE),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK (N_("Print"), N_("Print document"),
				file_toolbar_print_cb, GNOME_STOCK_PIXMAP_PRINT),
	GNOMEUIINFO_SEPARATOR,
	{ GNOME_APP_UI_TOGGLEITEM, N_("Code"), N_("Toggle code view"),
	  file_toolbar_code_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_PREFERENCES },
	GNOMEUIINFO_END
};

static GnomeUIInfo file_toolbar_file_uiinfo[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("New"), N_("New document"),
				file_toolbar_new_cb, GNOME_STOCK_PIXMAP_NEW),
	GNOMEUIINFO_ITEM_STOCK (N_("Open"), N_("Open document"),
				file_toolbar_open_cb, GNOME_STOCK_PIXMAP_OPEN),
	GNOMEUIINFO_ITEM_STOCK (N_("Save"), N_("Save document"),
				file_toolbar_save_cb, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK (N_("Save AS"), N_("Save document as"),
				file_toolbar_saveas_cb, GNOME_STOCK_PIXMAP_SAVE_AS),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK (N_("Print"), N_("Print document"),
				file_toolbar_print_cb, GNOME_STOCK_PIXMAP_PRINT),
	GNOMEUIINFO_SEPARATOR,
	{ GNOME_APP_UI_TOGGLEITEM, N_("Code"), N_("Toggle code view"),
	  file_toolbar_code_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_PREFERENCES },
	GNOMEUIINFO_END
};

static GtkWidget *
create_file_toolbar (GtkHTMLControlData *cd)
{
	GtkWidget *frame, *hbox;

	hbox = gtk_hbox_new (FALSE, 0);
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	cd->toolbar_file = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_container_add (GTK_CONTAINER (frame), cd->toolbar_file);
	gtk_box_pack_start (GTK_BOX (hbox), frame, FALSE, FALSE, 0);

	gtk_widget_show_all (hbox);
	if(cd->note_editor) {
		gnome_app_fill_toolbar_with_data (GTK_TOOLBAR (cd->toolbar_file), file_toolbar_file_note_uiinfo, NULL, cd);
		cd->code_view_button = file_toolbar_file_note_uiinfo[0].widget;
	} else {
		gnome_app_fill_toolbar_with_data (GTK_TOOLBAR (cd->toolbar_file), file_toolbar_file_uiinfo, NULL, cd);
		cd->code_view_button = file_toolbar_file_uiinfo[0].widget;
	}
	return hbox;
}


GtkWidget *
toolbar_file(GtkHTMLControlData *cd)
{
	g_return_val_if_fail (cd->html != NULL, NULL);
	g_return_val_if_fail (GTK_IS_HTML (cd->html), NULL);
	
	return create_file_toolbar (cd);
}

