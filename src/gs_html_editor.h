/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
  /*
     * GnomeSword Bible Study Tool
     * gs_html_editor.h
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

#ifndef __GS_HTML_EDITOR_H_
#define __GS_HTML_EDITOR_H_

#include "gs_gnomesword.h"
#include <gtkhtml/htmlgdkpainter.h>


//typedef enum   _GtkHTMLEditPropertyType       GtkHTMLEditPropertyType;
typedef struct _GSHTMLEditorControlData            GSHTMLEditorControlData;	

struct _GSHTMLEditorControlData {
	GtkHTML    *html;
	GtkWidget  *htmlwidget;
	GtkWidget  *vbox;
	GtkWidget  *statusbar;
	
	GtkWidget  *cpicker;
	GtkWidget  *combo;
	GtkWidget  *paragraph_option;

	GList                         *properties_types;
	GSList                         *paragraph_group;
	/* search & replace dialogs */
	GtkWidget     *search_dialog;
	GtkWidget    *replace_dialog;

	/* html/plain mode settings */
	gboolean format_html;
	HTMLGdkPainter *gdk_painter;
	HTMLGdkPainter *plain_painter;

	/* object from last button press event */
	HTMLObject *obj;

	/* button release signal id */
	guint releaseId;

	/* toolbars */
	GtkWidget *toolbar_commands, *toolbar_style;

	GtkWidget *bold;
	GtkWidget *italic;
	GtkWidget *underline;
	GtkWidget *strikeout;

	GtkWidget *left_align;
	GtkWidget *center;
	GtkWidget *right_align;

	GtkWidget *indent_button;
	GtkWidget *unindent_button;

	GtkWidget *font_size_menu;
	GtkWidget *pm;

	guint font_style_changed_connection_id;
	gboolean block_font_style_change;
	gboolean changed;
	gboolean personal_comments;
	gchar filename[256];

};

GSHTMLEditorControlData * gs_html_editor_control_data_new(SETTINGS *s);
void gs_html_editor_control_data_destroy(GSHTMLEditorControlData *cd);

/*** create editor ui ***/
GtkWidget *create_editor(GtkWidget *htmlwidget, GtkWidget *vbox, SETTINGS *s, GSHTMLEditorControlData *necd);
void on_editor_destroy(GtkObject * object, GSHTMLEditorControlData *ecd);
void savenoteEDITOR(GtkWidget *html_widget);
gint save_file(gchar *filename, GSHTMLEditorControlData *ecd);
/*** load studypad file ***/
gint load_file(gchar *filename, GSHTMLEditorControlData * ecd);

GtkWidget *studypad_control(GtkWidget *notebook, SETTINGS *s);
GtkWidget *percom_control(GtkWidget *notebook, SETTINGS *s);

#endif				/* __GS_HTML_EDITOR_H_ */
