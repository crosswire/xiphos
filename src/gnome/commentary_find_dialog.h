/*
 * GnomeSword Bible Study Tool
 * commentary_find_dialog.h - 
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

#ifndef _COMMENTARY_FIND_DIALOG_H_
#define _COMMENTARY_FIND_DIALOG_H_

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

#include "commentary.h"	

typedef GnomeDialog ** (*DialogCtor)(GtkWidget *htmlwidget);

#define FIND_DIALOG(name,title) comm_find_dialog ((GnomeDialog ***)&c-> name ## _dialog, c->html, (DialogCtor) comm_ ## name ## _dialog_new, title)

void comm_find_close_dialog(GtkWidget *but, COMMFindDialog *d);
COMMFindDialog * comm_find_dialog_new(GtkWidget *htmlwidget);
void comm_find_dialog_destroy(GtkWidget *dialog, COMMFindDialog *d);
void search_comm_find_dlg(COMM_DATA *c, gboolean regular, gchar *text);
void comm_find_dialog(GnomeDialog ***dialog, 
			GtkWidget *html, 
			DialogCtor ctor,
			const gchar *title); 

#endif /* _COMMENTARY_FIND_DIALOG_H_ */
