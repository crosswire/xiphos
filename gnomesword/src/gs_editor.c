/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_html_editor.c
     * -------------------
     * Mon Dec 10 2001
     * copyright (C) 2001 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
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
#endif				/* USE_SPELL */

#include "gs_gnomesword.h"
#include "gs_file.h"
#include "gs_html.h"
#include "gs_editor.h"
#include "gs_editor_toolbar.h"
#include "gs_info_box.h"
#include "support.h"
#include "sw_gnomesword.h"
#include "sw_gbs.h"

GSHTMLEditorControlData *specd;
GSHTMLEditorControlData *gbsecd;

extern SETTINGS *settings;
extern char *homedir;
extern gchar * current_filename;	/* filename for open file in study pad window  */
extern GtkWidget *htmlComments;
extern GtkWidget *toolbarComments;
extern GtkWidget *toolbarBooks;
extern GtkWidget *toolbarStudypad;

static GtkWidget *create_pmEditor(GSHTMLEditorControlData * ecd);
static GtkWidget *create_dlgSearch(GSHTMLEditorControlData * ecd);
static GtkWidget *create_dlgLink(GSHTMLEditorControlData * ecd);


GSHTMLEditorControlData *gs_html_editor_control_data_new(SETTINGS * s)
{
	GSHTMLEditorControlData *necd = g_new0(GSHTMLEditorControlData, 1);

	necd->paragraph_option = NULL;
	necd->properties_types = NULL;
	necd->block_font_style_change = FALSE;
	necd->gdk_painter = NULL;
	necd->plain_painter = NULL;
	necd->format_html = FALSE;
	necd->changed = FALSE;
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

/******************************************************************************
 * updatestatusbar - 
 ******************************************************************************/
static void updatestatusbar(GSHTMLEditorControlData * ecd)
{
	gint context_id2;
	gchar buf[255];
	gchar *buf2;

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(ecd->statusbar),
					 "GnomeSword");
	gtk_statusbar_pop(GTK_STATUSBAR(ecd->statusbar), context_id2);

	if (ecd->personal_comments)
		buf2 = settings->percomverse;
			
	else
		buf2 = ecd->filename;

	if (ecd->changed){
		sprintf(buf, "%s - modified", buf2);
		if(!ecd->personal_comments && !ecd->gbs)
			settings->modifiedSP = TRUE;
		if(ecd->personal_comments)
			settings->modifiedPC = TRUE;
		if(ecd->gbs)
			settings->modifiedGBS = TRUE;
	} else {
		sprintf(buf, "%s", buf2);
		if(!ecd->personal_comments && !ecd->gbs)
			settings->modifiedSP = FALSE;
		if(ecd->personal_comments)
			settings->modifiedPC = FALSE;
		if(ecd->gbs)
			settings->modifiedGBS = FALSE;
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

	current_filename = filename;
	sprintf(settings->studypadfilename,"%s",filename);	
	//g_warning("filename = %s\n",current_filename);
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
			gtk_html_write(ecd->html, stream, buffer, count);
		else
			break;
	}

	close(fd);

	if (count == 0) {
		gtk_html_end(ecd->html, stream, GTK_HTML_STREAM_OK);
		if (was_editable)
			gtk_html_set_editable(ecd->html, TRUE);
		updatestatusbar(ecd);
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

void savenoteEDITOR(GtkWidget * html_widget)
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
		savenoteSWORD(gstr->str);
		g_print("file writen");
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
		savebookSW_GBS(gstr->str);
	}
	g_string_free(gstr, 1);
	gtk_html_set_editable(html, TRUE);
}


gint save_file_program_end(GtkWidget *htmlwidget, gchar * filename)
{
	int retval = -1;
	int fd;
	if (filename) {
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);

		if (fd == -1)
			return -1;

		if (!gtk_html_save
		    (GTK_HTML(htmlwidget), (GtkHTMLSaveReceiverFn) save_receiver,
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
		sprintf(settings->studypadfilename, "%s", filename);

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
			updatestatusbar(ecd);
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

	buf = gtk_editable_get_chars(GTK_EDITABLE(GTK_TEXT(text)), 0, -1);

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

void on_editor_destroy(GtkObject * object, GSHTMLEditorControlData * ecd)
{
	gs_html_editor_control_data_destroy(ecd);
}

/********************************************************************************** create editor and call backs */
/******************************************************************************
 * this code taken form GtkHTML
 ******************************************************************************/
static gint
release(GtkWidget * widget, GdkEventButton * event,
	GSHTMLEditorControlData * cd)
{

	return FALSE;
}

/******************************************************************************
 * 
 ******************************************************************************/
static gint
html_key_pressed(GtkWidget * html, GdkEventButton * event,
		 GSHTMLEditorControlData * ecd)
{
	ecd->changed = TRUE;
	//file_changed = TRUE;
	updatestatusbar(ecd);
	return 1;
}

static void html_load_done(GtkWidget * html, GSHTMLEditorControlData * ecd)
{
	updatestatusbar(ecd);
}

static void on_submit(GtkHTML * html, const gchar *method, const gchar *url, const gchar *encoding, GSHTMLEditorControlData * ecd)
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
 * this code taken form GtkHTML
 ******************************************************************************/
static gint
html_button_pressed(GtkWidget * html, GdkEventButton * event,
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
		/* pass this for pasting */
		return TRUE;
	case 3:
		gtk_signal_emit_stop_by_name(GTK_OBJECT(html),
					     "button_press_event");
		break;
	default:
	}

	return FALSE;
}

static gboolean
on_html_enter_notify_event(GtkWidget * widget,
			   GdkEventCrossing * event,
			   GSHTMLEditorControlData * ecd)
{
	if (!ecd->personal_comments && !ecd->gbs)
		if (!gtk_html_get_editable(ecd->html))
			gtk_html_set_editable(ecd->html, TRUE);
	return TRUE;
}

/*** create editor ui ***/
GtkWidget *create_editor(GtkWidget * htmlwidget, GtkWidget * vbox, 
			 SETTINGS * s, GSHTMLEditorControlData * necd)
{
	GtkWidget 
		* frame34, 
		*scrolledwindow17;
	
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
	gtk_box_pack_start(GTK_BOX(vbox), necd->statusbar, FALSE, TRUE, 0);

	 
	 
	necd->vbox = vbox;
	necd->pm = create_pmEditor(necd);
	gnome_popup_menu_attach(necd->pm, necd->htmlwidget, NULL);
	gtk_signal_connect(GTK_OBJECT(necd->html), "submit",
			   GTK_SIGNAL_FUNC(on_submit), 
			   necd);
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
	gtk_signal_connect(GTK_OBJECT(necd->htmlwidget), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), /* gs_html.c */
			   NULL);
	gtk_signal_connect(GTK_OBJECT(necd->htmlwidget), "on_url",
			   GTK_SIGNAL_FUNC(on_url),  /* gs_html.c */
			   NULL);
			   
	if(necd->personal_comments){
		s->toolbarComments = toolbar_style(necd);
		gtk_widget_hide(s->toolbarComments);
	}
			   
	else if(necd->gbs){
		s->toolbarBooks = toolbar_style(necd);
		gtk_widget_hide(s->toolbarBooks);
	}
	
	else {
		s->toolbarStudypad = toolbar_style(necd);
		gtk_widget_hide(s->toolbarStudypad);
		if(settings->studypadfilename)
			load_file(settings->studypadfilename,necd);
	}
		
	return necd->htmlwidget;
}


/*************************************************************************** editor popup menu and call backs */
static void
on_new_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{
	GtkWidget *msgbox;
	gchar *msg;
	gint answer = 0;

	/* if study pad file has changed let's ask about saving it */
	if (ecd->changed) {
		msg =
		    g_strdup_printf(_
				    ("``%s'' has been modified.  Do you wish to save it?"),
				    ecd->filename);
		msgbox = create_InfoBox();
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
		g_free(msg);
		switch (answer) {
		case 0:
			save_file(ecd->filename, ecd);
			break;
		default:
			break;
		}
	}
	sprintf(ecd->filename, "%s", "");
	gtk_html_select_all(ecd->html);
	gtk_html_cut(ecd->html);
	gtk_statusbar_push(GTK_STATUSBAR(ecd->statusbar), 1,
			   _("-untitled-"));

	ecd->changed = FALSE;
}

static void on_open_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	GtkWidget *openFile;
	gchar *msg, buf[255];
	GtkWidget *msgbox;
	gint answer = 0;
	/* if study pad file has changed let's ask about saving it */
	if (ecd->changed) {
		msg =
		    g_strdup_printf(_
				    ("``%s'' has been modified.  Do you wish to save it?"),
				    ecd->filename);
		msgbox = create_InfoBox();
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
		g_free(msg);
		switch (answer) {
		case 0:
			save_file(ecd->filename, ecd);
			break;
		default:
			break;
		}
	}
	sprintf(buf, "%s/*.pad", homedir);
	openFile = create_fileselection1(ecd);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(openFile), buf);
	gtk_widget_show(openFile);
}

static void
on_savenote_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{
	if(ecd->personal_comments)
		savenoteEDITOR(ecd->htmlwidget);
	else
		savebookEDITOR(ecd->htmlwidget);
	ecd->changed = FALSE;
	updatestatusbar(ecd);
}

static void
on_deletenote_activate(GtkMenuItem * menuitem,
		       GSHTMLEditorControlData * ecd)
{
	deletenoteSWORD();
	ecd->changed = FALSE;
	updatestatusbar(ecd);
}


void on_save_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	GtkWidget *savemyFile;
	gchar buf[255];

	if (ecd->filename) {
		save_file(ecd->filename, ecd);
		return;
	} else {
		sprintf(buf, "%s/.pad", homedir);
		savemyFile = create_fileselectionSave(ecd);
		gtk_file_selection_set_filename(GTK_FILE_SELECTION
						(savemyFile), buf);
		gtk_widget_show(savemyFile);
	}
}


static void on_save_as_activate(GtkMenuItem * menuitem,
				GSHTMLEditorControlData * ecd)
{
	GtkWidget *savemyFile;
	gchar buf[255];

	sprintf(buf, "%s/.pad", homedir);
	savemyFile = create_fileselectionSave(ecd);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(savemyFile),
					buf);
	gtk_widget_show(savemyFile);
}


static void
on_print_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{	
	html_print(ecd->htmlwidget); /* gs_html.c */
}


static void
on_cut_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{
	gtk_html_cut(ecd->html);
	ecd->changed = TRUE;
	updatestatusbar(ecd);
}


static void
on_copy_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{
	gtk_html_copy(ecd->html);
}


static void
on_paste_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{
	gtk_html_paste(ecd->html);
	ecd->changed = TRUE;
	updatestatusbar(ecd);
}


static void
on_undo_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{
	gtk_html_undo(ecd->html);
	ecd->changed = TRUE;
	updatestatusbar(ecd);
}


static void
on_find_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{
	GtkWidget *dlg;

	dlg = create_dlgSearch(ecd);
	gtk_widget_show(dlg);
}

static void
on_find_again_activate(GtkMenuItem * menuitem,
		       GSHTMLEditorControlData * ecd)
{
	html_engine_search_next(ecd->html->engine);
}


static void
on_replace_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{
	replace(ecd);
}

static void
on_link_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{
	GtkWidget *dlg;

	dlg = create_dlgLink(ecd);
	gtk_widget_show(dlg);
}

static void
on_autoscroll_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{	
	settings->notefollow = GTK_CHECK_MENU_ITEM(menuitem)->active;
}

static void
on_editnote_activate(GtkMenuItem * menuitem, GSHTMLEditorControlData * ecd)
{	
	if(ecd->personal_comments) {
		settings->editnote = GTK_CHECK_MENU_ITEM(menuitem)->active;
		
		if(GTK_CHECK_MENU_ITEM(menuitem)->active){		
			gtk_widget_show(settings->toolbarComments);
		}
	
		else {
			gtk_widget_hide(settings->toolbarComments);
		}
	}
	
	if(ecd->gbs) {
		settings->editgbs = GTK_CHECK_MENU_ITEM(menuitem)->active;
		
		if(GTK_CHECK_MENU_ITEM(menuitem)->active){		
			gtk_widget_show(settings->toolbarBooks);
		}
	
		else {
			gtk_widget_hide(settings->toolbarBooks);
		}
	}
	gtk_html_set_editable(GTK_HTML(ecd->html), GTK_CHECK_MENU_ITEM(menuitem)->active);

}

GtkWidget *create_pmEditor(GSHTMLEditorControlData * ecd)
{
	GtkWidget *pmEditor;
	GtkAccelGroup *pmEditor_accels;
	guint tmp_key;
	GtkWidget *autoscroll = NULL;
	GtkWidget *separator;
	//GtkWidget *file;
	GtkWidget *file_menu;
	GtkAccelGroup *file_menu_accels;
	GtkWidget *save_note = NULL;
	GtkWidget *delete_note = NULL;
	GtkWidget *new = NULL;
	GtkWidget *open = NULL;
	GtkWidget *save = NULL;
	GtkWidget *saveas = NULL;
	GtkWidget *print;
	//GtkWidget *edit2;
	GtkWidget *edit2_menu;
	GtkAccelGroup *edit2_menu_accels;
	GtkWidget *cut;
	GtkWidget *copy;
	GtkWidget *paste;

#ifdef USE_SPELL		/* USE_SPELL */
	GtkWidget *spell;
#endif				/* USE_SPELL */

	GtkWidget *undo;
	GtkWidget *find;
	GtkWidget *find_again;
	GtkWidget *replace;
	//GtkWidget *link;

	ecd->editnote = NULL;
	
	pmEditor = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmEditor), "pmEditor", pmEditor);
	pmEditor_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(pmEditor));
	    
	if (ecd->personal_comments) {
		autoscroll = gtk_check_menu_item_new_with_label("Auto Scroll");
		gtk_widget_ref(autoscroll);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "autoscroll",
				 autoscroll,
				 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(autoscroll);
		gtk_container_add(GTK_CONTAINER(pmEditor), autoscroll);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(autoscroll),
				       TRUE);
				       
		ecd->editnote = gtk_check_menu_item_new_with_label("Edit Note");
		gtk_widget_ref(ecd->editnote);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "ecd->editnote",
				 ecd->editnote,
				 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(ecd->editnote);
		gtk_container_add(GTK_CONTAINER(pmEditor), ecd->editnote);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ecd->editnote),
				       FALSE);	
				       
		separator = gtk_menu_item_new ();
		gtk_widget_ref (separator);
		gtk_object_set_data_full (GTK_OBJECT (pmEditor), "separator", separator,
                            (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show (separator);
		gtk_container_add (GTK_CONTAINER (pmEditor), separator);
		gtk_widget_set_sensitive (separator, FALSE);		    
	}
	    
	if (ecd->gbs) {
				       
		ecd->editnote = gtk_check_menu_item_new_with_label("Edit");
		gtk_widget_ref(ecd->editnote);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "ecd->editnote",
				 ecd->editnote,
				 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(ecd->editnote);
		gtk_container_add(GTK_CONTAINER(pmEditor), ecd->editnote);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ecd->editnote),
				       FALSE);	
		gtk_widget_set_sensitive(GTK_WIDGET(ecd->editnote),
				 FALSE);
				       
		separator = gtk_menu_item_new ();
		gtk_widget_ref (separator);
		gtk_object_set_data_full (GTK_OBJECT (pmEditor), "separator", separator,
                            (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show (separator);
		gtk_container_add (GTK_CONTAINER (pmEditor), separator);
		gtk_widget_set_sensitive (separator, FALSE);		    
	}
				  			       
	ecd->file = gtk_menu_item_new_with_label("");
	tmp_key = gtk_label_parse_uline(GTK_LABEL(GTK_BIN(ecd->file)->child),
					_("File"));
	gtk_widget_add_accelerator(ecd->file, "activate_item", pmEditor_accels,
				   tmp_key, 0, 0);
	gtk_widget_ref(ecd->file);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "ecd->file", ecd->file,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->file);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->file);
	
	if (ecd->gbs)		
		gtk_widget_set_sensitive(GTK_WIDGET(ecd->file),
				 FALSE);
	
	file_menu = gtk_menu_new();
	gtk_widget_ref(file_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "file_menu",
				 file_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ecd->file), file_menu);
	file_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(file_menu));

	if (ecd->personal_comments) {
		save_note = gtk_menu_item_new_with_label(_("Save Note"));
		gtk_widget_ref(save_note);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "save_note",
					 save_note,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(save_note);
		gtk_container_add(GTK_CONTAINER(file_menu), save_note);

		delete_note =
		    gtk_menu_item_new_with_label(_("Delete Note"));
		gtk_widget_ref(delete_note);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor),
					 "delete_note", delete_note,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(delete_note);
		gtk_container_add(GTK_CONTAINER(file_menu), delete_note);
	}
	
	else if (ecd->gbs) {

		open = gtk_menu_item_new_with_label(_("Open File"));
		gtk_widget_ref(open);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "open",
					 open,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(open);
		gtk_container_add(GTK_CONTAINER(file_menu), open);

		save_note = gtk_menu_item_new_with_label(_("Save Entry"));
		gtk_widget_ref(save_note);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "save_note",
					 save_note,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(save_note);
		gtk_container_add(GTK_CONTAINER(file_menu), save_note);

		delete_note =
		    gtk_menu_item_new_with_label(_("Delete Entry"));
		gtk_widget_ref(delete_note);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor),
					 "delete_note", delete_note,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(delete_note);
		gtk_container_add(GTK_CONTAINER(file_menu), delete_note);
	}

	else {
		new = gtk_menu_item_new_with_label(_("New"));
		gtk_widget_ref(new);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "new", new,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(new);
		gtk_container_add(GTK_CONTAINER(file_menu), new);

		open = gtk_menu_item_new_with_label(_("Open"));
		gtk_widget_ref(open);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "open",
					 open,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(open);
		gtk_container_add(GTK_CONTAINER(file_menu), open);

		save = gtk_menu_item_new_with_label(_("Save"));
		gtk_widget_ref(save);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "save",
					 save,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(save);
		gtk_container_add(GTK_CONTAINER(file_menu), save);


		saveas = gtk_menu_item_new_with_label(_("Save AS"));
		gtk_widget_ref(saveas);
		gtk_object_set_data_full(GTK_OBJECT(pmEditor), "saveas",
					 saveas,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(saveas);
		gtk_container_add(GTK_CONTAINER(file_menu), saveas);
	}

	print = gtk_menu_item_new_with_label(_("Print"));
	gtk_widget_ref(print);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "print", print,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(print);
	gtk_container_add(GTK_CONTAINER(file_menu), print);


	ecd->edit2 = gtk_menu_item_new_with_label("");
	tmp_key = gtk_label_parse_uline(GTK_LABEL(GTK_BIN(ecd->edit2)->child),
					_("_Edit"));
	gtk_widget_add_accelerator(ecd->edit2, "activate_item", pmEditor_accels,
				   tmp_key, 0, 0);
	gtk_widget_ref(ecd->edit2);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "ecd->edit2", ecd->edit2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->edit2);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->edit2);
	
	if (ecd->gbs)		
		gtk_widget_set_sensitive(GTK_WIDGET(ecd->edit2),
				 FALSE);
	
	edit2_menu = gtk_menu_new();
	gtk_widget_ref(edit2_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "edit2_menu",
				 edit2_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ecd->edit2), edit2_menu);
	edit2_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(edit2_menu));

	cut = gtk_menu_item_new_with_label(_("Cut"));
	gtk_widget_ref(cut);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "cut", cut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cut);
	gtk_container_add(GTK_CONTAINER(edit2_menu), cut);

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_ref(copy);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "copy", copy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(edit2_menu), copy);

	paste = gtk_menu_item_new_with_label(_("Paste"));
	gtk_widget_ref(paste);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "paste", paste,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(paste);
	gtk_container_add(GTK_CONTAINER(edit2_menu), paste);

#ifdef USE_SPELL
	spell = gtk_menu_item_new_with_label(_("Spell Check"));
	gtk_widget_ref(spell);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "spell", spell,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spell);
	gtk_container_add(GTK_CONTAINER(edit2_menu), spell);
#endif				/* USE_SPELL */

	undo = gtk_menu_item_new_with_label(_("Undo"));
	gtk_widget_ref(undo);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "undo", undo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(undo);
	gtk_container_add(GTK_CONTAINER(edit2_menu), undo);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_ref(find);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "find", find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(edit2_menu), find);

	find_again = gtk_menu_item_new_with_label(_("Find Next"));
	gtk_widget_ref(find_again);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "find_again",
				 find_again,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(find_again);
	gtk_container_add(GTK_CONTAINER(edit2_menu), find_again);

	replace = gtk_menu_item_new_with_label(_("Replace"));
	gtk_widget_ref(replace);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "replace",
				 replace,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(replace);
	gtk_container_add(GTK_CONTAINER(edit2_menu), replace);

	ecd->link = gtk_menu_item_new_with_label(_("Link..."));
	gtk_widget_ref(ecd->link);
	gtk_object_set_data_full(GTK_OBJECT(pmEditor), "ecd->link",
				 ecd->link,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->link);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->link);
	
	if (ecd->gbs)		
		gtk_widget_set_sensitive(GTK_WIDGET(ecd->link),
				 FALSE);
	
	if (ecd->personal_comments) {
		gtk_signal_connect(GTK_OBJECT(save_note), "activate",
				   GTK_SIGNAL_FUNC(on_savenote_activate),
				   ecd);
		gtk_signal_connect(GTK_OBJECT(delete_note), "activate",
				   GTK_SIGNAL_FUNC(on_deletenote_activate),
				   ecd);
		gtk_signal_connect(GTK_OBJECT(autoscroll), "activate",
			   GTK_SIGNAL_FUNC(on_autoscroll_activate), 
				   ecd);
		gtk_signal_connect(GTK_OBJECT(ecd->editnote), "activate",
			   GTK_SIGNAL_FUNC(on_editnote_activate), 
				   ecd);
	} else if (ecd->gbs) {
		gtk_signal_connect(GTK_OBJECT(save_note), "activate",
				   GTK_SIGNAL_FUNC(on_savenote_activate),
				   ecd);
		gtk_signal_connect(GTK_OBJECT(delete_note), "activate",
				   GTK_SIGNAL_FUNC(on_deletenote_activate),
				   ecd);
		gtk_signal_connect(GTK_OBJECT(ecd->editnote), "activate",
			   GTK_SIGNAL_FUNC(on_editnote_activate), 
				   ecd);
		gtk_signal_connect(GTK_OBJECT(open), "activate",
				   GTK_SIGNAL_FUNC(on_open_activate), 
				  ecd);
	} else {
		gtk_signal_connect(GTK_OBJECT(new), "activate",
				   GTK_SIGNAL_FUNC(on_new_activate), ecd);
		gtk_signal_connect(GTK_OBJECT(open), "activate",
				   GTK_SIGNAL_FUNC(on_open_activate), ecd);
		gtk_signal_connect(GTK_OBJECT(save), "activate",
				   GTK_SIGNAL_FUNC(on_save_activate), ecd);
		gtk_signal_connect(GTK_OBJECT(saveas), "activate",
				   GTK_SIGNAL_FUNC(on_save_as_activate),
				   ecd);
	}
	gtk_signal_connect(GTK_OBJECT(print), "activate",
			   GTK_SIGNAL_FUNC(on_print_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(cut), "activate",
			   GTK_SIGNAL_FUNC(on_cut_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(copy), "activate",
			   GTK_SIGNAL_FUNC(on_copy_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(paste), "activate",
			   GTK_SIGNAL_FUNC(on_paste_activate), ecd);
#ifdef USE_SPELL
	gtk_signal_connect(GTK_OBJECT(spell), "activate",
			   GTK_SIGNAL_FUNC(spell_check_cb), ecd);
#endif				/* USE_SPELL */
	gtk_signal_connect(GTK_OBJECT(undo), "activate",
			   GTK_SIGNAL_FUNC(on_undo_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(find), "activate",
			   GTK_SIGNAL_FUNC(on_find_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(find_again), "activate",
			   GTK_SIGNAL_FUNC(on_find_again_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(replace), "activate",
			   GTK_SIGNAL_FUNC(on_replace_activate), ecd);
	
	gtk_signal_connect(GTK_OBJECT(ecd->link), "activate",
			   GTK_SIGNAL_FUNC(on_link_activate), ecd);
			  
	return pmEditor;
}

/*************************************************************************** search dialog and call backs */
static gboolean
on_entry12_key_press_event(GtkWidget * widget,
			   GdkEventKey * event,
			   GSHTMLEditorControlData * ecd)
{

	return FALSE;
}

static void
on_btnFindOK_clicked(GtkButton * button, GSHTMLEditorControlData * ecd)
{
	GtkWidget *dlg;
	gchar *buf;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	buf =
	    e_utf8_gtk_entry_get_text(GTK_ENTRY
				      (lookup_widget
				       (GTK_WIDGET(button), "entry12")));
	html_engine_search(ecd->html->engine, buf, 0,	/* not case sen */
			   1,	/* foward search */
			   0);	/* not regex */
	g_free(buf);
	gtk_widget_destroy(dlg);
}


static void
on_cancel_clicked(GtkButton * button, GSHTMLEditorControlData * ecd)
{
	GtkWidget *dlg;
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}


GtkWidget *create_dlgSearch(GSHTMLEditorControlData * ecd)
{
	GtkWidget *dlgSearch;
	GtkWidget *dialog_vbox16;
	GtkWidget *vbox45;
	GtkWidget *label180;
	GtkWidget *entry12;
	GtkWidget *hbox66;
	GtkWidget *cbtnSearchCase;
	GtkWidget *cbtnSearchDir;
	GtkWidget *cbtnSearchRegex;
	GtkWidget *dialog_action_area16;
	GtkWidget *btnFindOK;
	GtkWidget *btnFindCancel;
			
	dlgSearch = gnome_dialog_new(_("GnomeSWORD Find"), NULL);
	gtk_object_set_data(GTK_OBJECT(dlgSearch), "dlgSearch", dlgSearch);
	gtk_window_set_default_size(GTK_WINDOW(dlgSearch), 350, -1);

	dialog_vbox16 = GNOME_DIALOG(dlgSearch)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgSearch), "dialog_vbox16",
			    dialog_vbox16);
	gtk_widget_show(dialog_vbox16);

	vbox45 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox45);
	gtk_object_set_data_full(GTK_OBJECT(dlgSearch), "vbox45", vbox45,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox45);
	gtk_box_pack_start(GTK_BOX(dialog_vbox16), vbox45, TRUE, TRUE, 0);

	label180 = gtk_label_new(_("Enter Word or Phrase"));
	gtk_widget_ref(label180);
	gtk_object_set_data_full(GTK_OBJECT(dlgSearch), "label180",
				 label180,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label180);
	gtk_box_pack_start(GTK_BOX(vbox45), label180, FALSE, FALSE, 0);

	entry12 = gtk_entry_new();
	gtk_widget_ref(entry12);
	gtk_object_set_data_full(GTK_OBJECT(dlgSearch), "entry12", entry12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry12);
	gtk_box_pack_start(GTK_BOX(vbox45), entry12, FALSE, FALSE, 0);
//	gtk_widget_set_usize(entry12, 291, -2);
	
	hbox66 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox66);
	gtk_object_set_data_full(GTK_OBJECT(dlgSearch), "hbox66", hbox66,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox66);
	gtk_box_pack_start(GTK_BOX(vbox45), hbox66, TRUE, TRUE, 0);

	cbtnSearchCase =
	    gtk_check_button_new_with_label(_("Case Sensitive"));
	gtk_widget_ref(cbtnSearchCase);
	gtk_object_set_data_full(GTK_OBJECT(dlgSearch), "cbtnSearchCase",
				 cbtnSearchCase,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnSearchCase);
	gtk_box_pack_start(GTK_BOX(hbox66), cbtnSearchCase, FALSE, FALSE,
			   0);

	cbtnSearchDir =
	    gtk_check_button_new_with_label(_("Foward Search"));
	gtk_widget_ref(cbtnSearchDir);
	gtk_object_set_data_full(GTK_OBJECT(dlgSearch), "cbtnSearchDir",
				 cbtnSearchDir,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnSearchDir);
	gtk_box_pack_start(GTK_BOX(hbox66), cbtnSearchDir, FALSE, FALSE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnSearchDir),
				     TRUE);

	cbtnSearchRegex =
	    gtk_check_button_new_with_label(_("Regular Expresion"));
	gtk_widget_ref(cbtnSearchRegex);
	gtk_object_set_data_full(GTK_OBJECT(dlgSearch), "cbtnSearchRegex",
				 cbtnSearchRegex,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnSearchRegex);
	gtk_box_pack_start(GTK_BOX(hbox66), cbtnSearchRegex, FALSE, FALSE,
			   0);

	dialog_action_area16 = GNOME_DIALOG(dlgSearch)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgSearch), "dialog_action_area16",
			    dialog_action_area16);
	gtk_widget_show(dialog_action_area16);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area16),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area16),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSearch),
				   GNOME_STOCK_BUTTON_OK);
	btnFindOK =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgSearch)->buttons)->
		       data);
	gtk_widget_ref(btnFindOK);
	gtk_object_set_data_full(GTK_OBJECT(dlgSearch), "btnFindOK",
				 btnFindOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnFindOK);
	GTK_WIDGET_SET_FLAGS(btnFindOK, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSearch),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnFindCancel =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgSearch)->buttons)->
		       data);
	gtk_widget_ref(btnFindCancel);
	gtk_object_set_data_full(GTK_OBJECT(dlgSearch), "btnFindCancel",
				 btnFindCancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnFindCancel);
	GTK_WIDGET_SET_FLAGS(btnFindCancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(entry12), "key_press_event",
			   GTK_SIGNAL_FUNC(on_entry12_key_press_event),
			   ecd);
	gtk_signal_connect(GTK_OBJECT(btnFindOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnFindOK_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(btnFindCancel), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_clicked), ecd);

	return dlgSearch;
}

/*************************************************************************** link dialog create and call backs */
/*** set link module and key ***/
void
set_link_to_module(gchar * linkref, gchar * linkmod,
		   GSHTMLEditorControlData * ecd)
{
	gchar buf[256], *target = "";
	HTMLEngine *e = ecd->html->engine;

	if (strlen(linkmod))
		sprintf(buf, "version=%s passage=%s", linkmod, linkref);
	else
		sprintf(buf, "passage=%s", linkref);
	html_engine_selection_push(e);
	html_engine_insert_link(e, buf, target);
	html_engine_selection_pop(e);
}

static void
on_btnLinkOK_clicked(GtkButton * button, GSHTMLEditorControlData * ecd)
{
	gchar *linkreference, *linkmodule;
	GtkWidget *dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));

	linkreference =
	    gtk_editable_get_chars(GTK_EDITABLE
				   (lookup_widget
				    (GTK_WIDGET(button), "entryLinkRef")),
				   0, -1);
	linkmodule =
	    gtk_editable_get_chars(GTK_EDITABLE
				   (lookup_widget
				    (GTK_WIDGET(button), "entryLinkMod")),
				   0, -1);
	set_link_to_module(linkreference, linkmodule, ecd);

	gtk_widget_destroy(dlg);
	ecd->changed = TRUE;
	updatestatusbar(ecd);
}

/*** create dialog for setting up links in text ***/
GtkWidget *create_dlgLink(GSHTMLEditorControlData * ecd)
{
	GtkWidget *dlgLink;
	GtkWidget *dialog_vbox17;
	GtkWidget *vbox46;
	GtkWidget *label182;
	GtkWidget *entryLinkRef;
	GtkWidget *label183;
	GtkWidget *entryLinkMod;
	GtkWidget *dialog_action_area17;
	GtkWidget *btnLinkOK;
	GtkWidget *btnLinkCancel;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();

	dlgLink =
	    gnome_dialog_new(_("GnomeSWORD Add Reference Link"), NULL);
	gtk_object_set_data(GTK_OBJECT(dlgLink), "dlgLink", dlgLink);
	gtk_window_set_default_size(GTK_WINDOW(dlgLink), 465, -1);

	dialog_vbox17 = GNOME_DIALOG(dlgLink)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgLink), "dialog_vbox17",
			    dialog_vbox17);
	gtk_widget_show(dialog_vbox17);

	vbox46 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox46);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "vbox46", vbox46,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox46);
	gtk_box_pack_start(GTK_BOX(dialog_vbox17), vbox46, TRUE, TRUE, 0);

	label182 = gtk_label_new(_("Reference"));
	gtk_widget_ref(label182);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "label182", label182,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label182);
	gtk_box_pack_start(GTK_BOX(vbox46), label182, FALSE, FALSE, 0);

	entryLinkRef = gtk_entry_new();
	gtk_widget_ref(entryLinkRef);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "entryLinkRef",
				 entryLinkRef,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryLinkRef);
	gtk_box_pack_start(GTK_BOX(vbox46), entryLinkRef, FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, entryLinkRef,
			     _
			     ("Bible Reference or Module key to display when link is clicked"),
			     NULL);

  /*** put selected text in label entry ***/
	gtk_html_copy(ecd->html);
	gtk_editable_paste_clipboard(GTK_EDITABLE
				     (GTK_ENTRY(entryLinkRef)));

	label183 = gtk_label_new(_("Module"));
	gtk_widget_ref(label183);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "label183", label183,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label183);
	gtk_box_pack_start(GTK_BOX(vbox46), label183, FALSE, FALSE, 0);

	entryLinkMod = gtk_entry_new();
	gtk_widget_ref(entryLinkMod);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "entryLinkMod",
				 entryLinkMod,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryLinkMod);
	gtk_box_pack_start(GTK_BOX(vbox46), entryLinkMod, FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, entryLinkMod,
			     _
			     ("Name of module to show when link is clicked"),
			     NULL);

	dialog_action_area17 = GNOME_DIALOG(dlgLink)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgLink), "dialog_action_area17",
			    dialog_action_area17);
	gtk_widget_show(dialog_action_area17);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area17),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area17),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgLink),
				   GNOME_STOCK_BUTTON_OK);
	btnLinkOK =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgLink)->buttons)->data);
	gtk_widget_ref(btnLinkOK);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "btnLinkOK",
				 btnLinkOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLinkOK);
	GTK_WIDGET_SET_FLAGS(btnLinkOK, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgLink),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnLinkCancel =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgLink)->buttons)->data);
	gtk_widget_ref(btnLinkCancel);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "btnLinkCancel",
				 btnLinkCancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLinkCancel);
	GTK_WIDGET_SET_FLAGS(btnLinkCancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(btnLinkOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLinkOK_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(btnLinkCancel), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_clicked), ecd);

	gtk_object_set_data(GTK_OBJECT(dlgLink), "tooltips", tooltips);

	return dlgLink;
}

/************************************************************************************ studypad editor control */
/*** create studypad editor control ***/
GtkWidget *studypad_control(GtkWidget * notebook, SETTINGS * s)
{
	GtkWidget *frame12;
	GtkWidget *vbox6;
	//GSHTMLEditorControlData *specd =
	   // gs_html_editor_control_data_new(s);
	GtkWidget *vboxSP;
	GtkWidget *htmlwidget;
	
	specd = gs_html_editor_control_data_new(s);
	specd->personal_comments = FALSE;
	specd->gbs = FALSE;
	
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
	create_editor(htmlwidget, vboxSP, s, specd);

	return htmlwidget;
}

/************************************************************************** personal commentary editor control */
/*** create personal commentary editor control ***/
GtkWidget *percom_control(GtkWidget * vbox, SETTINGS * s)
{
	GtkWidget *frame12;
	GtkWidget *vbox6;
	GSHTMLEditorControlData *pcecd =
	    gs_html_editor_control_data_new(s);
	GtkWidget *vboxPC;

	pcecd->personal_comments = TRUE;
	pcecd->gbs = FALSE;

	frame12 = gtk_frame_new(NULL);
	gtk_widget_ref(frame12);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame12",
				 frame12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame12);
	gtk_container_add(GTK_CONTAINER(vbox), frame12);

	vbox6 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox6);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox6", vbox6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox6);
	gtk_container_add(GTK_CONTAINER(frame12), vbox6);

	vboxPC = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxPC);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vboxPC", vboxPC,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vboxPC);
	gtk_box_pack_start(GTK_BOX(vbox6), vboxPC, TRUE, TRUE, 0);

	htmlComments = gtk_html_new();
	
	create_editor(htmlComments, vboxPC, s, pcecd);
	return htmlComments;
}

/************************************************************************************ studypad editor control */
/*** create studypad editor control ***/
GtkWidget *gbs_control(GtkWidget * notebook, SETTINGS * s)
{
	GtkWidget *frame12;
	GtkWidget *vbox6;
	//GSHTMLEditorControlData *gbsecd =
	   // gs_html_editor_control_data_new(s);
	GtkWidget *vboxSP;
	GtkWidget *htmlwidget;
	
	gbsecd = gs_html_editor_control_data_new(s);
	gbsecd->gbs = TRUE;
	gbsecd->personal_comments = FALSE;
	
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
	create_editor(htmlwidget, vboxSP, s, gbsecd);

	return htmlwidget;
}
