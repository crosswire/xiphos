/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
  /*
    * GnomeSword Bible Study Tool
    * gs_listeditor.h
    * -------------------
    * Wed Jun 21 2000
    * copyright (C) 2000 by Terry Biggs
    * tbiggs@users.sourceforge.net
    *
 */
 
 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef __LISTEDITOR_H__
#define __LISTEDITOR_H__


typedef struct _listitem LISTITEM;
struct _listitem {
	gint type;
	gint level;
	gchar item[80];
	gchar preitem[80];
	gchar subitem[80];
	gchar menu[80];
};



/*********************************************************************************
**********************************************************************************
 *callbacks liseditor
**********************************************************************************
*********************************************************************************/
void on_clLElist_select_row(GtkCList * clist,
				gint row,	
				gint column,
				GdkEvent * event,
				gpointer user_data);
void on_btnLEup_clicked(GtkButton * button, gpointer user_data);
void on_btnLEdown_clicked(GtkButton * button, gpointer user_data);
void on_btnLEleft_clicked(GtkButton * button, gpointer user_data);
void on_btnLEright_clicked(GtkButton * button, gpointer user_data);
void on_btnLEAddItem_clicked(GtkButton * button, gpointer user_data);
void on_clLElist_select_row(GtkCList * clist,
				gint row,
				gint column,
				GdkEvent * event, gpointer user_data);
void on_btnLEok_clicked(GtkButton * button, gpointer user_data);
void on_btnLEapply_clicked(GtkButton * button, gpointer user_data);
void on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);
void on_btnLEapplylistchanges_clicked(GtkButton * button,
				gpointer user_data);
void on_btnLEok_clicked(GtkButton * button, gpointer user_data);
void on_btnLEapply_clicked(GtkButton * button, gpointer user_data);
void on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);
void on_btnLEdelete_clicked(GtkButton * button, gpointer user_data);
void on_btnLEnew_clicked(GtkButton * button, gpointer user_data);
void on_btnLEopen_clicked(GtkButton * button, gpointer user_data);
void on_btnLEclose_clicked(GtkButton * button, gpointer user_data);
void on_btnLErefresh_clicked(GtkButton * button, gpointer user_data);
void on_btnLEsave_clicked(GtkButton * button, gpointer user_data);



/*********************************************************************************
**********************************************************************************
 *functions liseditor
**********************************************************************************
*********************************************************************************/
void editbookmarksSave(GtkWidget *editdlg);
void loadbookmarkslevel(GtkWidget *list, gint level, gchar *submenu);
void addsubitme(GtkWidget *list, gint row);
void addverse(GtkWidget *list, gint row, gchar *item);
void deleteitem(GtkWidget *list);
GtkWidget *createListEditor(void);
void editbookmarks(GtkWidget *editdlg);
void applychanges(GtkWidget *widget);
void applylistchanges(GtkWidget *widget, gint row);
void selectrow(GtkWidget *widget, gint row, gint column);
void movelistitem(GtkWidget *list, gint direction, gint listrow);
void getrow(GtkWidget *list, gint listrow, gchar *buf[6]);															
void getcolumn(GtkWidget *list, gint row, gint column, gchar *buf);
GtkWidget *create_listeditor(void);

#endif /* __LISTEDITOR_H__ */

#ifdef __cplusplus
}
#endif

