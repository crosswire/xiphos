/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* 
 * Author : 
 *  Damon Chaplin <damon@helixcode.com>
 *
 * Copyright 2000, Helix Code, Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */
#ifndef _E_CALENDAR_H_
#define _E_CALENDAR_H_

#include <gtk/gtkwidget.h>
#include "e-util/e-canvas.h"
#include "e-calendar-item.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * ECalendar - displays a table of monthly calendars, allowing highlighting
 * and selection of one or more days. Like GtkCalendar with more features.
 * Most of the functionality is in the ECalendarItem canvas item, though
 * we also add GnomeCanvasWidget buttons to go to the previous/next month and
 * to got to the current day.
 */

#define E_CALENDAR(obj)          GTK_CHECK_CAST (obj, e_calendar_get_type (), ECalendar)
#define E_CALENDAR_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, e_calendar_get_type (), ECalendarClass)
#define E_IS_CALENDAR(obj)       GTK_CHECK_TYPE (obj, e_calendar_get_type ())


typedef struct _ECalendar       ECalendar;
typedef struct _ECalendarClass  ECalendarClass;

struct _ECalendar
{
	ECanvas canvas;

	ECalendarItem *calitem;

	GnomeCanvasItem *prev_item, *next_item;

	gint min_rows;
	gint min_cols;

	gint max_rows;
	gint max_cols;
};

struct _ECalendarClass
{
	ECanvasClass parent_class;
};


GtkType	   e_calendar_get_type		(void);
GtkWidget* e_calendar_new		(void);

void	   e_calendar_set_minimum_size	(ECalendar	*cal,
					 gint		 rows,
					 gint		 cols);
void	   e_calendar_set_maximum_size	(ECalendar	*cal,
					 gint		 rows,
					 gint		 cols);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_CALENDAR_H_ */
