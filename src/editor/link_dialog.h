/*
 * Xiphos Bible Study Tool
 * link_dialog.f - dialog for inserting a link
 *
 * Copyright (C) 2005-2017 Xiphos Developer Team
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

#ifndef _LINK_DIALOG_H
#define _LINK_DIALOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>
#include <glib.h>

#ifdef USE_WEBKIT_EDITOR
#include "editor/webkit_editor.h"
#include "editor/editor.h"
#else
#include "editor/slib-editor.h"
#endif

void editor_link_dialog(EDITOR *e);
void entry_verse_changed_cb(GObject *object, EDITOR *e);
void button_ok_clicked_cb(GObject *object, EDITOR *e);
void button_test_clicked_cb(GObject *object, gpointer user_data);
void button_cancel_clicked_cb(GObject *object,
			      gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif /* _LINK_DIALOG_H */
