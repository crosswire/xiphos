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

#ifndef _GTK_HTML_SEARCH_H_
#define _GTK_HTML_SEARCH_H_

typedef struct _GtkHTMLSearchDialog  GtkHTMLSearchDialog;

#include <gnome.h>
#include "gtkhtml.h"
#include "control-data.h"

GtkHTMLSearchDialog * gtk_html_search_dialog_new     (GtkHTML *html, gboolean regular);
void                  gtk_html_search_dialog_destroy (GtkHTMLSearchDialog *d);

void                  search                         (GtkHTMLControlData *cd, gboolean regular);
void                  search_next                    (GtkHTMLControlData *cd);

#endif
