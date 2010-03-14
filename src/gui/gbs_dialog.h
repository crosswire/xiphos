/*
 * Xiphos Bible Study Tool
 * gbs_dialog.h - dialog for displaying a gbs module
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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

#ifndef __GBS_DIALOG_H_
#define __GBS_DIALOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main/module_dialogs.h"

void gui_create_gbs_dialog(DIALOG_DATA * dlg);
void gui_close_gbs_dialog(DIALOG_DATA * dlg);

#ifdef __cplusplus
}
#endif

#endif
