/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* This file is part of the GtkHTML library

   Copyright (C) 2000 Helix Code, Inc.
   Authors:           Radek Doulik (rodo@helixcode.com)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHcANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _GTK_HTML_EDIT_PROPERTIES_H_
#define _GTK_HTML_EDIT_PROPERTIES_H_

#include <gnome.h>
#include "control-data.h"

typedef GtkWidget * (*GtkHTMLEditPropertyCreateFunc) (GtkHTMLControlData *cd, gpointer *own_data);
typedef void        (*GtkHTMLEditPropertyApplyFunc)  (GtkHTMLControlData *cd, gpointer  own_data);
typedef void        (*GtkHTMLEditPropertyCloseFunc)  (GtkHTMLControlData *cd, gpointer  own_data);

enum _GtkHTMLEditPropertyType {
	GTK_HTML_EDIT_PROPERTY_PARAGRAPH,
	GTK_HTML_EDIT_PROPERTY_TEXT,
	GTK_HTML_EDIT_PROPERTY_IMAGE,
	GTK_HTML_EDIT_PROPERTY_LINK,
	GTK_HTML_EDIT_PROPERTY_BODY,
	GTK_HTML_EDIT_PROPERTY_RULE,
};

GtkHTMLEditPropertiesDialog * gtk_html_edit_properties_dialog_new          (GtkHTMLControlData *cd,
									    gboolean insert,
									    gchar *title);
void                          gtk_html_edit_properties_dialog_destroy      (GtkHTMLEditPropertiesDialog *d);

void                          gtk_html_edit_properties_dialog_add_entry    (GtkHTMLEditPropertiesDialog *d,
									    GtkHTMLEditPropertyType t,
									    const gchar *name,
									    GtkHTMLEditPropertyCreateFunc create,
									    GtkHTMLEditPropertyApplyFunc apply_cb,
									    GtkHTMLEditPropertyCloseFunc close_cb);
void                          gtk_html_edit_properties_dialog_show         (GtkHTMLEditPropertiesDialog *d);
void                          gtk_html_edit_properties_dialog_close        (GtkHTMLEditPropertiesDialog *d);
void                          gtk_html_edit_properties_dialog_change       (GtkHTMLEditPropertiesDialog *d);
void                          gtk_html_edit_properties_dialog_set_page     (GtkHTMLEditPropertiesDialog *d,
									    GtkHTMLEditPropertyType t);

#endif
