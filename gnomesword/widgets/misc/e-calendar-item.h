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
#ifndef _E_CALENDAR_ITEM_H_
#define _E_CALENDAR_ITEM_H_

#include <libgnomeui/gnome-canvas.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * ECalendarItem - canvas item displaying a calendar.
 */

#define	E_CALENDAR_ITEM_YPAD1	1
#define	E_CALENDAR_ITEM_YPAD2	1

/* These index our colors array. */
typedef enum
{
	E_CALENDAR_COLOR_SELECTION,
	E_CALENDAR_COLOR_HIGHLIGHT,
	E_CALENDAR_COLOR_TODAY,
	
	E_CALENDAR_COLOR_LAST
} ECalendarColors;


#define E_CALENDAR_ITEM(obj)     (GTK_CHECK_CAST((obj), \
        e_calendar_item_get_type (), ECalendarItem))
#define E_CALENDAR_ITEM_CLASS(k) (GTK_CHECK_CLASS_CAST ((k),\
	e_calendar_item_get_type ()))
#define E_IS_CALENDAR_ITEM(o)    (GTK_CHECK_TYPE((o), \
	e_calendar_item_get_type ()))

typedef struct {
	GnomeCanvasItem canvas_item;

	/* The year & month of the first calendar being displayed. */
	gint year;
	gint month;	/* 0 to 11 */

	/* Bounds of item. */
	gdouble x1, y1, x2, y2;

	/* The minimum & maximum number of rows & columns of months. */
	gint min_rows;
	gint min_cols;
	gint max_rows;
	gint max_cols;

	/* The number of rows & columns of months. */
	gint rows;
	gint cols;

	/* Whether we show week nubers. */
	gboolean show_week_numbers;

	/* The first day of the week, 0 (Monday) to 6 (Sunday). */
	gint week_start_day;

	/* Whether the cells expand to fill extra space. */
	gboolean expand;

	/* The minimum size of each month, based on the fonts used. */
	gint min_month_width;
	gint min_month_height;

	/* The actual size of each month, after dividing extra space. */
	gint month_width;
	gint month_height;

	/* The offset to the left edge of the first calendar. */
	gint x_offset;

	/* The padding around each calendar month. */
	gint month_lpad, month_rpad;
	gint month_tpad, month_bpad;

	/* The size of each cell. */
	gint cell_width;
	gint cell_height;


	/* The current selection. The month offsets are from 0, which is the
	   top-left calendar month view. The day offsets are from 0, which is
	   the top-left cell in the month view (which may be empty). */
	gint selection_start_month_offset;
	gint selection_start_day_offset;
	gint selection_end_month_offset;
	gint selection_end_day_offset;
	gboolean selecting;
	gboolean selection_dragging_end;

	/* The first character of each day of the week, e.g. 'MTWTFSS'. */
	gchar *days;

	/* Widths of the day characters. */
	gint day_widths[7];

	/* Widths of the digits, '0' .. '9'. */
	gint digit_widths[10];
	gint max_digit_width;
	gint week_number_digit_widths[10];
	gint max_week_number_digit_width;

	/* Fonts for drawing text. If font isn't set it uses the font from the
	   canvas widget. If week_number_font isn't set it uses font. */
	GdkFont *font, *old_font;
	GdkFont *week_number_font, *old_week_number_font;

	/* Colors for drawing. */
	GdkColor colors[E_CALENDAR_COLOR_LAST];
} ECalendarItem;

typedef struct {
	GnomeCanvasItemClass parent_class;

} ECalendarItemClass;


GtkType  e_calendar_item_get_type	(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_CALENDAR_ITEM_H_ */
