/*
 * GnomeSword Bible Study Tool
 * editor_search.h - search dialog for editors
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
 
#ifndef _GS_EDITOR_SEARCH_H_
#define _GS_EDITOR_SEARCH_H_

typedef struct _GSHTMLSearchDialog  GSHTMLSearchDialog;

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

#include "gui/editor.h"

GSHTMLSearchDialog * gs_editor_search_dialog_new(GtkHTML *html);
void gs_editor_search_dialog_destroy(GSHTMLSearchDialog *d);
void search (GSHTMLEditorControlData *ecd, 
				gboolean regular, gchar *text);
void search_next (GSHTMLEditorControlData *ecd);

#endif

