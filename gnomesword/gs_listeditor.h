/***************************************************************************
                          listeditor.h  -  description
                             -------------------
    begin                : Wed Jun 21 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
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
void
	 on_clLElist_select_row(GtkCList * clist,
				gint row,
				gint column,
				GdkEvent * event, gpointer user_data);

	void
	 on_btnLEup_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEdown_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEleft_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEright_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEgotoverse_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEAddVerse_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEAddItem_clicked(GtkButton * button, gpointer user_data);

	void
	 on_clLElist_select_row(GtkCList * clist,
				gint row,
				gint column,
				GdkEvent * event, gpointer user_data);

	void
	 on_btnLEok_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEapply_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEapplylistchanges_clicked(GtkButton * button,
					  gpointer user_data);

	void
	 on_btnLEok_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEapply_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnLEcancel_clicked(GtkButton * button, gpointer user_data);





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
