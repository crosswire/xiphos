/*
 * GnomeSword Bible Study Tool
 * commentary_find.h - 
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

#ifndef _COMMENTARY_FIND_H_
#define _COMMENTARY_FIND_H_

#include <gnome.h>

#include "main/find_dlg_defs.h"

typedef struct _gs_find_dialog COMMFindDialog;

#include "gui/_commentary.h"	

void comm_find_close_dialog(GtkWidget *but, COMMFindDialog *d);
void search_comm_find_dlg(COMM_DATA *c, gboolean regular, gchar *text);
			
#endif

