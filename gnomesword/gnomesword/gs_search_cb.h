/***************************************************************************
                          gs_search_cb.h  -  description
                             -------------------
    begin                : Mon Mar 12 2001
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
	
void on_btnSearchOK_clicked(GtkButton * button, gpointer user_data);
void on_btnSearchSaveList_clicked(GtkButton * button, gpointer user_data);
void on_btnSearch1_clicked(GtkButton * button, gpointer user_data);
void on_resultList_select_row(GtkCList * clist, gint row, gint column, GdkEvent * event, gpointer user_data);
void on_dlgSearch_destroy(GtkObject * object, gpointer user_data);

#ifdef __cplusplus
}
#endif

