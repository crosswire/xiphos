/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*  This file is part of the GtkHTML library.

    Copyright (C) 2000 Helix Code, Inc.

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

    Author: Radek Doulik <rodo@helixcode.com>
*/

#ifndef _CONTROL_DATA_H
#define _CONTROL_DATA_H


typedef struct _GtkHTMLControlData            GtkHTMLControlData;
typedef enum   _GtkHTMLEditPropertyType       GtkHTMLEditPropertyType;
typedef struct _GtkHTMLEditPropertiesDialog   GtkHTMLEditPropertiesDialog;

#include <gtkhtml/gtkhtml.h>
//#include <bonobo.h>
#include <htmlgdkpainter.h>
#include "search.h"
#include "replace.h"
#include "image.h"
#include "link.h"
#include "rule.h"
//#include "engine.h"
//#include "Spell.h"

struct _GtkHTMLControlData {
	GtkHTML   *html;
	GtkWidget 	*vbox, 
				*cpicker, 
				*combo, 
				*notebook,
				*gtktext,
				*statusbar;

	GtkHTMLEditPropertiesDialog   *properties_dialog;
	GList                         *properties_types;

	/* search & replace dialogs */
	GtkHTMLSearchDialog     *search_dialog;
	GtkHTMLReplaceDialog    *replace_dialog;

	/* html/plain mode settings */
	gboolean 	format_html,
				html_modified,
				note_editor;
	HTMLGdkPainter *gdk_painter;
	HTMLGdkPainter *plain_painter;

	/* object from last button press event */
	HTMLObject *obj;

	/* button release signal id */
	guint releaseId;

	/* filename */
	gchar *filename;
	
	/* toolbars */
	GtkWidget *toolbar_commands, 
			*toolbar_style,
			*toolbar_edit,
			*toolbar_file,
			*toolbar_insert;

	GtkWidget *tt_button;
	GtkWidget *bold_button;
	GtkWidget *italic_button;
	GtkWidget *underline_button;
	GtkWidget *strikeout_button;
	
	GtkWidget *code_view_button;
	
	GtkWidget *left_align_button;
	GtkWidget *center_button;
	GtkWidget *right_align_button;

	GtkWidget *font_size_menu;

	guint font_style_changed_connection_id;
	gboolean block_font_style_change;

//	GNOME_Spell_Dictionary  dict;
//	BonoboObjectClient     *dict_client;
//	EditorEngine       *editor_bonobo_engine;
};

GtkHTMLControlData * gtk_html_control_data_new       (GtkHTML *html, GtkWidget *vbox);
void                 gtk_html_control_data_destroy   (GtkHTMLControlData *cd);

#endif
