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

#ifndef __LISTEDITOR_H__
#define __LISTEDITOR_H__

void
editbookmarksSave							(GtkWidget *editdlg);

void
loadbookmarkslevel						(GtkWidget *list,
															gint level,
															gchar *submenu);

void
addsubitme										(GtkWidget *list,
															gint row);

void
addverse											(GtkWidget *list,
															gint row,
															gchar *item);

void
deleteitem										(GtkWidget *list);

void
changeLEverse									(gchar *verse);

GtkWidget *
createListEditor							(void);

void
editbookmarks									(GtkWidget *editdlg);

void
destroyListEditor							(void);

void
applychanges									(GtkWidget *widget);

void
applylistchanges							(GtkWidget *widget,
															gint row);

void
selectrow											(GtkWidget *widget,
															gint row,
															gint column);

void
movelistitem									(GtkWidget *list,
															gint direction,
															gint listrow);

void
getrow												(GtkWidget *list,
															gint listrow,
															gchar *buf[6]);
															
void
getcolumn											(GtkWidget *list,
															gint row,
															gint column,
															gchar *buf);
/*
GtkWidget*
create_ListEditor   					(void);
*/
#endif /* __LISTEDITOR_H__ */

