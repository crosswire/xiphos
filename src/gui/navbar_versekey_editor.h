/*
 * Xiphos Bible Study Tool
 * navbar_book_dialog.h - navigation bar for genbook dialog modules
 *
 * Copyright (C) 2007-2008 Xiphos Developer Team
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

#ifndef ___NAVBAR_VERSEKEY_EDITOR_H_
#define ___NAVBAR_VERSEKEY_EDITOR_H_

#include <config.h>

#ifdef USE_GTKHTML3_14_23
#include "editor/slib-editor.h"
#else
#include "editor/bonobo-editor.h"
#endif

GtkWidget *gui_navbar_versekey_editor_new(EDITOR * editor);

#endif
