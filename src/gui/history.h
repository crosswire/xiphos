/*
 * GnomeSword Bible Study Tool
 * history.h - add, remove and nav history
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

#ifndef __HISTORY_H_
#define __HISTORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

typedef struct _history HISTORY;
struct _history {
        gint itemnum;
        gchar verseref[80];
        gchar textmod[80];
        gchar commod[80];
};

void gui_clear_history(GtkWidget *app,
		GtkWidget *shortcut_bar);
void updatehistoryshortcutbar(GtkWidget *app,
                GtkWidget *shortcut_bar);
void gui_add_history_Item(GtkWidget *app,
		GtkWidget *shortcut_bar,
		gchar *ref);
void gui_navigate_history(GtkWidget *app, gint direction);
void gui_change_verse_history(gint historynum);		

#ifdef __cplusplus
}
#endif

#endif
