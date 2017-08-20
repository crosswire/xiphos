/*
 * Xiphos Bible Study Tool
 * bibletext.h - gui for Bible text modules
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

#ifndef ___BIBLETEXT_H_
#define ___BIBLETEXT_H_

#include <gtk/gtk.h>

void gui_setup_bibletext(void);
gboolean gui_text_button_release_event(GtkWidget *widget,
				       GdkEventButton *event,
				       gpointer data);
void gui_popup_pm_text(void);
GtkWidget *gui_create_bible_pane(void);

#endif
