/***************************************************************************
                          	        gs_history.h 
                             -------------------
    begin                : Thu Feb 1 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

typedef struct _history HISTORY;
struct _history {
        gint itemnum;
        gint compagenum;
        gchar verseref[80];
        gchar textmod[80];
        gchar commod[80];
};


void clearhistory(GtkWidget *app,
		GtkWidget *shortcut_bar);
void updatehistorymenu(GtkWidget *app);
void updatehistoryshortcutbar(GtkWidget *app,
                GtkWidget *shortcut_bar);
void updatehistorysidebar(GtkWidget *app);
void addHistoryItem(GtkWidget *app,
		GtkWidget *shortcut_bar,
		gchar *ref);
void historynav(GtkWidget *app, gint direction);
void changeverseHistory(gint historynum);

		

#ifdef __cplusplus
}
#endif

