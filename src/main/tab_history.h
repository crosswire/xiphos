/*
 * GnomeSword Bible Study Tool
 * tab_history.h - add, remove and navigate history
 *
 * Copyright (C) 2005 GnomeSword Developer Team
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

#ifndef __TAB_HISTORY_H_
#define __TAB_HISTORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

typedef struct _tab_history TAB_HISTORY;
struct _tab_history {
        gint itemnum;
        gchar verseref[80];
        gchar textmod[80];
        gchar commod[80];
};

void main_clear_tab_history(void);
void main_add_tab_history_item(gpointer data);
void main_navigate_tab_history(gint direction);
void main_update_tab_history_menu(gpointer data);		
void main_change_verse_tab_history(gint historynum);

#ifdef __cplusplus
}
#endif

#endif
