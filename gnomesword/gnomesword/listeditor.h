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
addsubitme										(GtkWidget *list,
															gint row);

void
addverse											(GtkWidget *list,
															gint row,
															gchar *item);

void
changeLEverse									(gchar *verse);

GtkWidget *
createListEditor							(void);

void
destroyListEditor							(void);

void
applylistchanges							(GtkWidget *widget,
															gint row);

void
selectrow											(GtkWidget *widget,
															gint row,
															gint column);

void
movelistitem									(GtkWidget *widget,
															gint direction,
															gint listrow);

GtkWidget*
create_ListEditor   					(void);

#endif /* __LISTEDITOR_H__ */

