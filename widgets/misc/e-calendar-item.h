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

#define	E_CALENDAR_ITEM_YPAD_ABOVE_MONTH_NAME	1
#define	E_CALENDAR_ITEM_YPAD_BELOW_MONTH_NAME	1

/* Used to mark days as bold in e_calendar_item_mark_day(). */
#define E_CALENDAR_ITEM_MARK_BOLD	1


/* These index our colors array. */
typedef enum
{
	E_CALENDAR_ITEM_COLOR_TODAY_BOX,
	E_CALENDAR_ITEM_COLOR_SELECTION_FG,
	E_CALENDAR_ITEM_COLOR_SELECTION_BG,
	E_CALENDAR_ITEM_COLOR_PREV_OR_NEXT_MONTH_FG,
	
	E_CALENDAR_ITEM_COLOR_LAST
} ECalendarItemColors;

typedef struct _ECalendarItem       ECalendarItem;
typedef struct _ECalendarItemClass  ECalendarItemClass;

typedef void (*ECalendarItemStyleCallback)   (ECalendarItem	*calitem,
					      gint		 year,
					      gint		 month,
					      gint		 day,
					      gint		 day_style,
					      gboolean		 today,
					      gboolean		 prev_or_next_month,
					      gboolean		 selected,
					      gboolean		 has_focus,
					      gboolean		 drop_target,
					      GdkColor	       **bg_color,
					      GdkColor	       **fg_color,
					      GdkColor	       **box_color,
					      gboolean		*bold,
					      gpointer		 data);


#define E_CALENDAR_ITEM(obj)     (GTK_CHECK_CAST((obj), \
        e_calendar_item_get_type (), ECalendarItem))
#define E_CALENDAR_ITEM_CLASS(k) (GTK_CHECK_CLASS_CAST ((k),\
	e_calendar_item_get_type ()))
#define E_IS_CALENDAR_ITEM(o)    (GTK_CHECK_TYPE((o), \
	e_calendar_item_get_type ()))

struct _ECalendarItem
{
	GnomeCanvasItem canvas_item;

	/* The year & month of the first calendar being displayed. */
	gint year;
	gint month;	/* 0 to 11 */

	/* Points to an array of styles, one char for each day. We use 32
	   chars for each month, with n + 2 months, where n is the number of
	   complete months shown (since we show some days before the first
	   month and after the last month grayes out).
	   A value of 0 is the default, and 1 is bold. */
	guint8 *styles;

	/*
	 * Options.
	 */

	/* The minimum & maximum number of rows & columns of months.
	   If the maximum values are -1 then there is no maximum.
	   The minimum valies default to 1. The maximum values to -1. */
	gint min_rows;
	gint min_cols;
	gint max_rows;
	gint max_cols;

	/* The actual number of rows & columns of months. */
	gint rows;
	gint cols;

	/* Whether we show week nubers. */
	gboolean show_week_numbers;

	/* The first day of the week, 0 (Monday) to 6 (Sunday). */
	gint week_start_day;

	/* Whether the cells expand to fill extra space. */
	gboolean expand;

	/* The maximum number of days that can be selected. Defaults to 1. */
	gint max_days_selected;

	/* The number of days selected before we switch to selecting whole
	   weeks, or -1 if we never switch. Defaults to -1. */
	gint days_to_start_week_selection;

	/* Whether the selection is moved when we move back/forward one month.
	   Used for things like the EDateEdit which only want the selection to
	   be changed when the user explicitly selects a day. */
	gboolean move_selection_when_moving;

	/* Whether the selection is rounded down to the nearest week when we
	   move back/forward one month. Used for the week view. */
	gboolean round_selection_when_moving;

	/*
	 * Internal stuff.
	 */

	/* Bounds of item. */
	gdouble x1, y1, x2, y2;

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
	   top-left calendar month view. Note that -1 is used for the last days
	   from the previous month. The days are real month days. */
	gboolean selecting;
	gboolean selection_dragging_end;
	gboolean selection_from_full_week;
	gboolean selection_set;
	gint selection_start_month_offset;
	gint selection_start_day;
	gint selection_end_month_offset;
	gint selection_end_day;
	gint selection_real_start_month_offset;
	gint selection_real_start_day;

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

	ECalendarItemStyleCallback style_callback;
	gpointer style_callback_data;
	GtkDestroyNotify style_callback_destroy;

	/* Colors for drawing. */
	GdkColor colors[E_CALENDAR_ITEM_COLOR_LAST];

	/* Our idle handler for emitting signals. */
	gint signal_emission_idle_id;

	/* A flag to indicate that the selection or date range has changed.
	   When set the idle function will emit the signal and reset it to
	   FALSE. This is so we don't emit it several times when args are set
	   etc. */
	gboolean selection_changed;
	gboolean date_range_changed;
};

struct _ECalendarItemClass
{
	GnomeCanvasItemClass parent_class;

	void (* date_range_changed)	(ECalendarItem *calitem);
	void (* selection_changed)	(ECalendarItem *calitem);
};


GtkType  e_calendar_item_get_type		(void);

/* FIXME: months are 0-11 throughout, but 1-12 may be better. */

void	 e_calendar_item_get_first_month	(ECalendarItem	*calitem,
						 gint		*year,
						 gint		*month);
void	 e_calendar_item_set_first_month	(ECalendarItem	*calitem,
						 gint		 year,
						 gint		 month);

/* Returns the range of dates actually shown. Months are 0 to 11.
   This also includes the last days of the previous month and the first days
   of the following month, which are normally shown in gray. */
void	 e_calendar_item_get_date_range		(ECalendarItem	*calitem,
						 gint		*start_year,
						 gint		*start_month,
						 gint		*start_day,
						 gint		*end_year,
						 gint		*end_month,
						 gint		*end_day);

/* Returns the selected date range. It returns FALSE if no days are currently
   selected. */
gboolean e_calendar_item_get_selection		(ECalendarItem	*calitem,
						 GDate		*start_date,
						 GDate		*end_date);
/* Sets the selected date range, and changes the date range shown so at least
   the start of the selection is shown. If start_date is NULL it clears the
   selection. */
void	 e_calendar_item_set_selection		(ECalendarItem	*calitem,
						 GDate		*start_date,
						 GDate		*end_date);

/* Marks a particular day. Passing E_CALENDAR_ITEM_MARK_BOLD as the day style
   will result in the day being shown as bold by default. The style callback
   could support more day_styles, or the style callback could determine the
   colors itself, without needing to mark days. */
void	 e_calendar_item_clear_marks		(ECalendarItem	*calitem);
void	 e_calendar_item_mark_day		(ECalendarItem	*calitem,
						 gint		 year,
						 gint		 month,
						 gint		 day,
						 guint8		 day_style);

/* Mark a range of days. Any days outside the currently shown range are
   ignored. */
void	 e_calendar_item_mark_days		(ECalendarItem	*calitem,
						 gint		 start_year,
						 gint		 start_month,
						 gint		 start_day,
						 gint		 end_year,
						 gint		 end_month,
						 gint		 end_day,
						 guint8		 day_style);

/* Sets the function to call to get the colors to use for a particular day. */
void	 e_calendar_item_set_style_callback	(ECalendarItem	*calitem,
						 ECalendarItemStyleCallback cb,
						 gpointer	 data,
						 GtkDestroyNotify destroy);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_CALENDAR_ITEM_H_ */
