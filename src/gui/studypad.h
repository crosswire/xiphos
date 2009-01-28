/*
 * Xiphos Bible Study Tool
 * studypad.h - studypad gui
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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

#ifndef __STUDYPAD_H_
#define __STUDYPAD_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#include <config.h>
#ifndef USE_GTKHTML38
#include <gnome.h>
#include "editor/editor.h"

extern GSHTMLEditorControlData *editor_cd;

void gui_studypad_can_close(void);
gint save_file_program_end(GtkWidget *htmlwidget, gchar *filename);
gint save_file(gchar *filename, GSHTMLEditorControlData *ecd);
gint load_file(gchar *filename, GSHTMLEditorControlData *ecd);
gint gui_open_studypad(gchar * file_name);

#endif
#ifdef __cplusplus
}
#endif	
#endif
