/*
 * GnomeSword Bible Study Tool
 * gbs_find_dialog.h - 
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

#ifndef _GBS_FIND_DIALOG_H_
#define _GBS_FIND_DIALOG_H_

#include <gnome.h>

/* main */
#include "find_dlg_defs.h"
typedef struct _gs_find_dialog GBF_FIND_DIALOG;
/* main */
#include "gbs.h"	

void gbs_find_close_dialog(GtkWidget *but, GBF_FIND_DIALOG *d);
void search_gbs_find_dlg(GBS_DATA *c, gboolean regular, gchar *text);
			
#endif /* _GBS_FIND_DIALOG_H_ */
