/*
 * Xiphos Bible Study Tool
 * find_dialog.h -
 *
 * Copyright (C) 2000-2025 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _FIND_DIALOG_H_
#define _FIND_DIALOG_H_

#include <gtk/gtk.h>

void gui_find_dlg(GtkWidget *htmlwidget, gchar *mod_name,
		  gboolean regular, gchar *text);

#endif
