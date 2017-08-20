/*
 * Xiphos Bible Study Tool
 * bibletext_dialog.h - view Bible text module in a dialog
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

#ifndef __BIBLETEXT_DIALOG_H__
#define __BIBLETEXT_DIALOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main/module_dialogs.h"

void gui_bible_dialog_sync_toggled(GtkToggleButton *button,
				   DIALOG_DATA *c);
void gui_create_bibletext_dialog(DIALOG_DATA *vt);
void gui_text_dialog_create_menu(DIALOG_DATA *d);

#ifdef __cplusplus
}
#endif
#endif
