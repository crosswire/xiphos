/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_editor.h
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

#ifndef __GS_EDITOR_H_
#define __GS_EDITOR_H_

#include <gnome.h>
#include <gtkhtml/htmlgdkpainter.h>
#include "gs_gnomesword.h"
#include "gs_editor_replace.h"
#include "gs_editor_search.h"


typedef GnomeDialog ** (*DialogCtor)(GtkHTML *html);

#define RUN_DIALOG(name,title) run_dialog ((GnomeDialog ***)&ecd-> name ## _dialog, ecd->html, (DialogCtor) gs_editor_ ## name ## _dialog_new, title)


//typedef enum   _GtkHTMLEditPropertyType       GtkHTMLEditPropertyType;

typedef struct _GSHTMLEditorControlData GSHTMLEditorControlData;
struct _GSHTMLEditorControlData {
	GtkHTML *html;
	GtkWidget *htmlwidget;
	GtkWidget *vbox;
	GtkWidget *statusbar;

	GtkWidget *cpicker;
	GtkWidget *combo;
	GtkWidget *paragraph_option;

	GList *properties_types;
	GSList *paragraph_group;

	/* search & replace dialogs */
	GSHTMLSearchDialog  *search_dialog;
	GtkHTMLReplaceDialog *replace_dialog;

	/* html/plain mode settings */
	gboolean format_html;
	HTMLGdkPainter *gdk_painter;
	HTMLGdkPainter *plain_painter;

	/* object from last button press event */
	HTMLObject *obj;

	/* button release signal id */
	guint releaseId;

	/* toolbars */
	 GtkWidget * toolbar_commands, *toolbar_style;
	/* toolbars buttons */
	 GtkWidget
	    *editnote,
	    *file,
	    *edit2,
	    *link,
	    * tt_button,
	    *bold_button,
	    *italic_button,
	    *underline_button,
	    *strikeout_button,
	    *left_align_button,
	    *center_button,
	    *right_align_button, *unindent_button, *indent_button;

	GtkWidget *bold;
	GtkWidget *italic;
	GtkWidget *underline;
	GtkWidget *strikeout;

	GtkWidget *left_align;
	GtkWidget *center;
	GtkWidget *right_align;

	//GtkWidget *indent;
	//GtkWidget *unindent;

	GtkWidget *font_size_menu;
	GtkWidget *pm;

	guint font_style_changed_connection_id;
	gboolean block_font_style_change;
	gboolean changed;
	gboolean personal_comments;
	gboolean gbs; /** gen book support **/
	gchar filename[256];

};

GSHTMLEditorControlData *gs_html_editor_control_data_new(SETTINGS * s);
void gs_html_editor_control_data_destroy(GSHTMLEditorControlData * cd);

void replace(GSHTMLEditorControlData * ecd);
void search(GSHTMLEditorControlData *ecd, gboolean regular, gchar *text);
void search_next(GSHTMLEditorControlData *ecd);
/*** create editor ui ***/
GtkWidget *create_editor(GtkWidget * htmlwidget, GtkWidget * vbox,
			 SETTINGS * s, GSHTMLEditorControlData * necd);
void on_editor_destroy(GtkObject * object, GSHTMLEditorControlData * ecd);
void savenoteEDITOR(GtkWidget * html_widget);
void savebookEDITOR(GtkWidget * html_widget);
gint save_file_program_end(GtkWidget * htmlwidget, gchar * filename);
gint save_file(gchar * filename, GSHTMLEditorControlData * ecd);
/*** load studypad file ***/
gint load_file(gchar * filename, GSHTMLEditorControlData * ecd);

GtkWidget *studypad_control(GtkWidget * notebook, SETTINGS * s);
GtkWidget *percom_control(GtkWidget * notebook, SETTINGS * s);
GtkWidget *gbs_control(GtkWidget * notebook, SETTINGS * s);
/*** link dialog create and call back ***/
void set_link_to_module(gchar * linkref, gchar * linkmod,
			GSHTMLEditorControlData * ecd);
void find_word_EDITOR(gchar * word, GSHTMLEditorControlData * ecd);
void load_text_from_spell_EDITOR(GtkWidget * text,
				 GSHTMLEditorControlData * ecd);
/*** load text from html to speller ***/
gboolean load_text_for_spell_EDITOR(GtkWidget * text,
				    GSHTMLEditorControlData * ecd);
/*** save studypad file ***/
void on_save_activate(GtkMenuItem * menuitem,
		      GSHTMLEditorControlData * ecd);
void run_dialog (GnomeDialog ***dialog, GtkHTML *html, DialogCtor ctor, const gchar *title);
void searchgbsGS_EDITOR(gchar *searchstring);

#endif				/* __GS_EDITOR_H_ */
