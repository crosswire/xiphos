/*
 * GnomeSword Bible Study Tool
 * hints.h - create hint_window dialog and display hints
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
#ifndef _HINTS_H
#define _HINTS_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _hint HINT;
struct  _hint {
	
	GtkWidget *hint_window;
	GtkWidget *html_widget;
	
	gboolean in_popup;
	gboolean use_hints;
};
extern HINT hint;

void gui_display_hint_in_viewer(gchar *text);
void gui_display_in_hint_window(gchar *note);
void gui_destroy_hint_window(void);
void gui_open_hint_viewer(void);

#ifdef __cplusplus
}
#endif

#endif /* _HINTS_H */
