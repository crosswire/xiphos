/*
 * Xiphos Bible Study Tool
 * commentary_dialog.h - dialog for a commentary module
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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

#ifndef __COMMENTARY_DIALOG_H_
#define __COMMENTARY_DIALOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main/module_dialogs.h"

void gui_commentary_dialog_sync_toggled(GtkToggleButton *button,
					DIALOG_DATA *c);
void gui_create_commentary_dialog(DIALOG_DATA *d,
				  gboolean do_edit);
void gui_display_commentary_with_struct(DIALOG_DATA *d,
					gchar *key);
void gui_close_comm_dialog(DIALOG_DATA *d);

#ifdef __cplusplus
}
#endif
#endif
