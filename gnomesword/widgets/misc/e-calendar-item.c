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

/*
 * ECalendarItem - canvas item displaying a calendar.
 */

#include <config.h>
#include <time.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtksignal.h>
#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>
#include <gal/util/e-util.h>
#include "e-calendar-item.h"


/*
 * These are the padding sizes between various pieces of the calendar.
 */

/* The minimum padding around the numbers in each cell/day. */
#define	E_CALENDAR_ITEM_MIN_CELL_XPAD	4
#define	E_CALENDAR_ITEM_MIN_CELL_YPAD	0

/* Vertical padding. */
#define	E_CALENDAR_ITEM_YPAD_ABOVE_DAY_LETTERS		1
#define	E_CALENDAR_ITEM_YPAD_BELOW_DAY_LETTERS		0
#define	E_CALENDAR_ITEM_YPAD_ABOVE_CELLS		1
#define	E_CALENDAR_ITEM_YPAD_BELOW_CELLS		2

/* Horizontal padding in the heading bars. */
#define	E_CALENDAR_ITEM_XPAD_BEFORE_MONTH_NAME_WITH_BUTTON	16
#define	E_CALENDAR_ITEM_XPAD_BEFORE_MONTH_NAME			3
#define	E_CALENDAR_ITEM_XPAD_AFTER_MONTH_NAME			3
#define	E_CALENDAR_ITEM_XPAD_AFTER_MONTH_NAME_WITH_BUTTON	16

/* Horizontal padding in the month displays. */
#define	E_CALENDAR_ITEM_XPAD_BEFORE_WEEK_NUMBERS	4
#define	E_CALENDAR_ITEM_XPAD_AFTER_WEEK_NUMBERS		2
#define	E_CALENDAR_ITEM_XPAD_BEFORE_CELLS		1
#define	E_CALENDAR_ITEM_XPAD_AFTER_CELLS		4

/* The number of rows & columns of days in each month. */
#define E_CALENDAR_ROWS_PER_MONTH	6
#define E_CALENDAR_COLS_PER_MONTH	7

static const int e_calendar_item_days_in_month[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

#define DAYS_IN_MONTH(year, month) \
  e_calendar_item_days_in_month[month] + (((month) == 1 \
  && ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))) ? 1 : 0)


static void e_calendar_item_class_init	(ECalendarItemClass *class);
static void e_calendar_item_init	(ECalendarItem	 *calitem);
static void e_calendar_item_destroy	(GtkObject	 *o);
static void e_calendar_item_get_arg	(GtkObject	 *o,
					 GtkArg		 *arg,
					 guint		  arg_id);
static void e_calendar_item_set_arg	(GtkObject	 *o,
					 GtkArg		 *arg,
					 guint		  arg_id);
static void e_calendar_item_realize	(GnomeCanvasItem *item);
static void e_calendar_item_unrealize	(GnomeCanvasItem *item);
static void e_calendar_item_unmap	(GnomeCanvasItem *item);
static void e_calendar_item_update	(GnomeCanvasItem *item,
					 double		 *affine,
					 ArtSVP		 *clip_path,
					 int		  flags);
static void e_calendar_item_draw	(GnomeCanvasItem *item,
					 GdkDrawable	 *drawable,
					 int		  x,
					 int		  y,
					 int		  width,
					 int		  height);
static void e_calendar_item_draw_month	(ECalendarItem   *calitem,
					 GdkDrawable	 *drawable,
					 int		  x,
					 int		  y,
					 int		  width,
					 int		  height,
					 int		  row,
					 int		  col);
static void e_calendar_item_draw_day_numbers (ECalendarItem	*calitem,
					      GdkDrawable	*drawable,
					      int		 width,
					      int		 height,
					      int		 row,
					      int		 col,
					      int		 year,
					      int		 month,
					      int		 start_weekday,
					      gint		 cells_x,
					      gint		 cells_y);
static double e_calendar_item_point	(GnomeCanvasItem *item,
					 double		  x,
					 double		  y,
					 int		  cx,
					 int		  cy,
					 GnomeCanvasItem **actual_item);
static gint e_calendar_item_event	(GnomeCanvasItem *item,
					 GdkEvent	 *event);
static gboolean e_calendar_item_button_press	(ECalendarItem	*calitem,
						 GdkEvent	*event);
static gboolean e_calendar_item_button_release	(ECalendarItem	*calitem,
						 GdkEvent	*event);
static gboolean e_calendar_item_motion		(ECalendarItem	*calitem,
						 GdkEvent	*event);

static gboolean e_calendar_item_convert_position_to_day	(ECalendarItem	*calitem,
							 gint		 x,
							 gint		 y,
							 gboolean	 round_empty_positions,
							 gint		*month_offset,
							 gint		*day,
							 gboolean	*entire_week);
static void e_calendar_item_get_month_info	(ECalendarItem	*calitem,
						 gint		 row,
						 gint		 col,
						 gint		*first_day_offset,
						 gint		*days_in_month,
						 gint		*days_in_prev_month);
static void e_calendar_item_recalc_sizes(ECalendarItem *calitem);

static gint e_calendar_item_get_week_number	(ECalendarItem *calitem,
						 gint		day,
						 gint		month,
						 gint		year);

static void e_calendar_item_get_day_style	(ECalendarItem	*calitem,
						 gint		 year,
						 gint		 month,
						 gint		 day,
						 gint		 day_style,
						 gboolean	 today,
						 gboolean	 prev_or_next_month,
						 gboolean	 selected,
						 gboolean	 has_focus,
						 gboolean	 drop_target,
						 GdkColor      **bg_color,
						 GdkColor      **fg_color,
						 GdkColor      **box_color,
						 gboolean	*bold);
static void e_calendar_item_check_selection_end	(ECalendarItem	*calitem,
						 gint		 start_month,
						 gint		 start_day,
						 gint		*end_month,
						 gint		*end_day);
static void e_calendar_item_check_selection_start(ECalendarItem	*calitem,
						  gint		*start_month,
						  gint		*start_day,
						  gint		 end_month,
						  gint		 end_day);
static void e_calendar_item_normalize_date	(ECalendarItem	*calitem,
						 gint		*year,
						 gint		*month);
static void e_calendar_item_add_days_to_selection(ECalendarItem	*calitem,
						  gint		 days);
static void e_calendar_item_round_up_selection	(ECalendarItem	*calitem,
						 gint		*month_offset,
						 gint		*day);
static void e_calendar_item_round_down_selection (ECalendarItem	*calitem,
						  gint		*month_offset,
						  gint		*day);
static gint e_calendar_item_get_inclusive_days	(ECalendarItem	*calitem,
						 gint		 start_month_offset,
						 gint		 start_day,
						 gint		 end_month_offset,
						 gint		 end_day);
static void e_calendar_item_ensure_valid_day	(ECalendarItem	*calitem,
						 gint		*month_offset,
						 gint		*day);
static gboolean e_calendar_item_ensure_days_visible (ECalendarItem	*calitem,
						     gint		 start_year,
						     gint		 start_month,
						     gint		 start_day,
						     gint		 end_year,
						     gint		 end_month,
						     gint		 end_day);
static void e_calendar_item_show_popup_menu	(ECalendarItem	*calitem,
						 GdkEventButton	*event,
						 gint		 month_offset);
static void e_calendar_item_on_menu_item_activate(GtkWidget	*menuitem,
						  ECalendarItem	*calitem);
static void e_calendar_item_position_menu	(GtkMenu            *menu,
						 gint               *x,
						 gint               *y,
						 gpointer            user_data);
static void e_calendar_item_date_range_changed	(ECalendarItem	*calitem);
static void e_calendar_item_queue_signal_emission	(ECalendarItem	*calitem);
static gboolean e_calendar_item_signal_emission_idle_cb	(gpointer data);

/* Our arguments. */
enum {
	ARG_0,
	ARG_YEAR,
	ARG_MONTH,
	ARG_X1,
	ARG_Y1,
	ARG_X2,
	ARG_Y2,
	ARG_FONT,
	ARG_WEEK_NUMBER_FONT,
	ARG_ROW_HEIGHT,
	ARG_COLUMN_WIDTH,
	ARG_MINIMUM_ROWS,
	ARG_MINIMUM_COLUMNS,
	ARG_MAXIMUM_ROWS,
	ARG_MAXIMUM_COLUMNS,
	ARG_WEEK_START_DAY,
	ARG_SHOW_WEEK_NUMBERS,
	ARG_MAXIMUM_DAYS_SELECTED,
	ARG_DAYS_TO_START_WEEK_SELECTION,
	ARG_MOVE_SELECTION_WHEN_MOVING,
	ARG_ROUND_SELECTION_WHEN_MOVING
};

enum {
  DATE_RANGE_CHANGED,
  SELECTION_CHANGED,
  LAST_SIGNAL
};


static GnomeCanvasItemClass *parent_class;
static guint e_calendar_item_signals[LAST_SIGNAL] = { 0 };


E_MAKE_TYPE (e_calendar_item, "ECalendarItem", ECalendarItem,
	     e_calendar_item_class_init, e_calendar_item_init,
	     GNOME_TYPE_CANVAS_ITEM)


static void
e_calendar_item_class_init (ECalendarItemClass *class)
{
	GtkObjectClass  *object_class;
	GnomeCanvasItemClass *item_class;

	parent_class = gtk_type_class (gnome_canvas_item_get_type());

	object_class = (GtkObjectClass *) class;
	item_class = (GnomeCanvasItemClass *) class;

	gtk_object_add_arg_type ("ECalendarItem::year",
				 GTK_TYPE_INT, GTK_ARG_READWRITE,
				 ARG_YEAR);
	gtk_object_add_arg_type ("ECalendarItem::month",
				 GTK_TYPE_INT, GTK_ARG_READWRITE,
				 ARG_MONTH);
	gtk_object_add_arg_type ("ECalendarItem::x1",
				 GTK_TYPE_DOUBLE, GTK_ARG_READWRITE,
				 ARG_X1);
	gtk_object_add_arg_type ("ECalendarItem::y1",
				 GTK_TYPE_DOUBLE, GTK_ARG_READWRITE,
				 ARG_Y1);
	gtk_object_add_arg_type ("ECalendarItem::x2",
				 GTK_TYPE_DOUBLE, GTK_ARG_READWRITE,
				 ARG_X2);
	gtk_object_add_arg_type ("ECalendarItem::y2",
				 GTK_TYPE_DOUBLE, GTK_ARG_READWRITE,
				 ARG_Y2);
	gtk_object_add_arg_type ("ECalendarItem::font",
				 GTK_TYPE_GDK_FONT, GTK_ARG_READWRITE,
				 ARG_FONT);
	gtk_object_add_arg_type ("ECalendarItem::week_number_font",
				 GTK_TYPE_GDK_FONT, GTK_ARG_READWRITE,
				 ARG_WEEK_NUMBER_FONT);
	gtk_object_add_arg_type ("ECalendarItem::row_height",
				 GTK_TYPE_INT, GTK_ARG_READABLE,
				 ARG_ROW_HEIGHT);
	gtk_object_add_arg_type ("ECalendarItem::column_width",
				 GTK_TYPE_INT, GTK_ARG_READABLE,
				 ARG_COLUMN_WIDTH);
	gtk_object_add_arg_type ("ECalendarItem::minimum_rows",
				 GTK_TYPE_INT, GTK_ARG_READWRITE,
				 ARG_MINIMUM_ROWS);
	gtk_object_add_arg_type ("ECalendarItem::minimum_columns",
				 GTK_TYPE_INT, GTK_ARG_READWRITE,
				 ARG_MINIMUM_COLUMNS);
	gtk_object_add_arg_type ("ECalendarItem::maximum_rows",
				 GTK_TYPE_INT, GTK_ARG_READWRITE,
				 ARG_MAXIMUM_ROWS);
	gtk_object_add_arg_type ("ECalendarItem::maximum_columns",
				 GTK_TYPE_INT, GTK_ARG_READWRITE,
				 ARG_MAXIMUM_COLUMNS);
	gtk_object_add_arg_type ("ECalendarItem::week_start_day",
				 GTK_TYPE_INT, GTK_ARG_READWRITE,
				 ARG_WEEK_START_DAY);
	gtk_object_add_arg_type ("ECalendarItem::show_week_numbers",
				 GTK_TYPE_BOOL, GTK_ARG_READWRITE,
				 ARG_SHOW_WEEK_NUMBERS);
	gtk_object_add_arg_type ("ECalendarItem::maximum_days_selected",
				 GTK_TYPE_INT, GTK_ARG_READWRITE,
				 ARG_MAXIMUM_DAYS_SELECTED);
	gtk_object_add_arg_type ("ECalendarItem::days_to_start_week_selection",
				 GTK_TYPE_INT, GTK_ARG_READWRITE,
				 ARG_DAYS_TO_START_WEEK_SELECTION);
	gtk_object_add_arg_type ("ECalendarItem::move_selection_when_moving",
				 GTK_TYPE_BOOL, GTK_ARG_READWRITE,
				 ARG_MOVE_SELECTION_WHEN_MOVING);
	gtk_object_add_arg_type ("ECalendarItem::round_selection_when_moving",
				 GTK_TYPE_BOOL, GTK_ARG_READWRITE,
				 ARG_ROUND_SELECTION_WHEN_MOVING);

	e_calendar_item_signals[DATE_RANGE_CHANGED] =
		gtk_signal_new ("date_range_changed",
				GTK_RUN_FIRST,
				object_class->type,
				GTK_SIGNAL_OFFSET (ECalendarItemClass, date_range_changed),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0);
	e_calendar_item_signals[SELECTION_CHANGED] =
		gtk_signal_new ("selection_changed",
				GTK_RUN_FIRST,
				object_class->type,
				GTK_SIGNAL_OFFSET (ECalendarItemClass, selection_changed),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0);

	gtk_object_class_add_signals (object_class, e_calendar_item_signals,
				      LAST_SIGNAL);


	object_class->destroy = e_calendar_item_destroy;
	object_class->get_arg = e_calendar_item_get_arg;
	object_class->set_arg = e_calendar_item_set_arg;

	/* GnomeCanvasItem method overrides */
	item_class->realize     = e_calendar_item_realize;
	item_class->unrealize   = e_calendar_item_unrealize;
	item_class->unmap	= e_calendar_item_unmap;
	item_class->update      = e_calendar_item_update;
	item_class->draw        = e_calendar_item_draw;
	item_class->point       = e_calendar_item_point;
	item_class->event       = e_calendar_item_event;

	class->date_range_changed	= NULL;
	class->selection_changed	= NULL;
}


static void
e_calendar_item_init (ECalendarItem *calitem)
{
	struct tm *tmp_tm;
	time_t t;

	/* Set the default time to the current month. */
	t = time (NULL);
	tmp_tm = localtime (&t);
	calitem->year = tmp_tm->tm_year + 1900;
	calitem->month = tmp_tm->tm_mon;

	calitem->styles = NULL;

	calitem->min_cols = 1;
	calitem->min_rows = 1;
	calitem->max_cols = -1;
	calitem->max_rows = -1;

	calitem->rows = 0;
	calitem->cols = 0;

	calitem->show_week_numbers = FALSE;
	calitem->week_start_day = 0;
	calitem->expand = TRUE;
	calitem->max_days_selected = 42;
	calitem->days_to_start_week_selection = 9;
	calitem->move_selection_when_moving = TRUE;
	calitem->round_selection_when_moving = FALSE;

	calitem->x1 = 0.0;
	calitem->y1 = 0.0;
	calitem->x2 = 0.0;
	calitem->y2 = 0.0;

	calitem->selection_set = FALSE;

	calitem->selection_changed = FALSE;
	calitem->date_range_changed = FALSE;

	calitem->style_callback = NULL;
	calitem->style_callback_destroy = NULL;

	/* Translators: These are the first characters of each day of the
	   week, 'M' for 'Monday', 'T' for Tuesday etc. */
	calitem->days = _("MTWTFSS");

	calitem->signal_emission_idle_id = 0;
}


static void
e_calendar_item_destroy		(GtkObject *o)
{
	ECalendarItem *calitem;

	calitem = E_CALENDAR_ITEM (o);

	e_calendar_item_set_style_callback (calitem, NULL, NULL, NULL);

	g_free (calitem->styles);

	if (calitem->signal_emission_idle_id != 0) {
		g_source_remove (calitem->signal_emission_idle_id);
		calitem->signal_emission_idle_id = 0;
	}

	if (calitem->old_font)
		gdk_font_unref (calitem->old_font);
	if (calitem->old_week_number_font)
		gdk_font_unref (calitem->old_week_number_font);
}


static void
e_calendar_item_get_arg (GtkObject *o, GtkArg *arg, guint arg_id)
{
	GnomeCanvasItem *item;
	ECalendarItem *calitem;

	item = GNOME_CANVAS_ITEM (o);
	calitem = E_CALENDAR_ITEM (o);

	switch (arg_id) {
	case ARG_YEAR:
		GTK_VALUE_INT (*arg) = calitem->year;
		break;
	case ARG_MONTH:
		GTK_VALUE_INT (*arg) = calitem->month;
		break;
	case ARG_X1:
		GTK_VALUE_DOUBLE (*arg) = calitem->x1;
		break;
	case ARG_Y1:
		GTK_VALUE_DOUBLE (*arg) = calitem->y1;
		break;
	case ARG_X2:
		GTK_VALUE_DOUBLE (*arg) = calitem->x2;
		break;
	case ARG_Y2:
		GTK_VALUE_DOUBLE (*arg) = calitem->y2;
		break;
	case ARG_FONT:
		GTK_VALUE_BOXED (*arg) = calitem->font;
		break;
	case ARG_WEEK_NUMBER_FONT:
		GTK_VALUE_BOXED (*arg) = calitem->week_number_font;
		break;
	case ARG_ROW_HEIGHT:
		e_calendar_item_recalc_sizes (calitem);
		GTK_VALUE_INT (*arg) = calitem->min_month_height;
		break;
	case ARG_COLUMN_WIDTH:
		e_calendar_item_recalc_sizes (calitem);
		GTK_VALUE_INT (*arg) = calitem->min_month_width;
		break;
	case ARG_MINIMUM_ROWS:
		GTK_VALUE_INT (*arg) = calitem->min_rows;
		break;
	case ARG_MINIMUM_COLUMNS:
		GTK_VALUE_INT (*arg) = calitem->min_cols;
		break;
	case ARG_MAXIMUM_ROWS:
		GTK_VALUE_INT (*arg) = calitem->max_rows;
		break;
	case ARG_MAXIMUM_COLUMNS:
		GTK_VALUE_INT (*arg) = calitem->max_cols;
		break;
	case ARG_WEEK_START_DAY:
		GTK_VALUE_INT (*arg) = calitem->week_start_day;
		break;
	case ARG_SHOW_WEEK_NUMBERS:
		GTK_VALUE_BOOL (*arg) = calitem->show_week_numbers;
		break;
	case ARG_MAXIMUM_DAYS_SELECTED:
		GTK_VALUE_INT (*arg) = calitem->max_days_selected;
		break;
	case ARG_DAYS_TO_START_WEEK_SELECTION:
		GTK_VALUE_INT (*arg) = calitem->days_to_start_week_selection;
		break;
	case ARG_MOVE_SELECTION_WHEN_MOVING:
		GTK_VALUE_BOOL (*arg) = calitem->move_selection_when_moving;
		break;
	case ARG_ROUND_SELECTION_WHEN_MOVING:
		GTK_VALUE_BOOL (*arg) = calitem->round_selection_when_moving;
		break;
	}
}


static void
e_calendar_item_set_arg (GtkObject *o, GtkArg *arg, guint arg_id)
{
	GnomeCanvasItem *item;
	ECalendarItem *calitem;
	GdkFont *font;
	gboolean need_update = FALSE;
	gdouble dvalue;
	gint ivalue;
	gboolean bvalue;

	item = GNOME_CANVAS_ITEM (o);
	calitem = E_CALENDAR_ITEM (o);
	
	switch (arg_id){
	case ARG_YEAR:
		ivalue = GTK_VALUE_INT (*arg);
		e_calendar_item_set_first_month (calitem, ivalue,
						 calitem->month);
		break;
	case ARG_MONTH:
		ivalue = GTK_VALUE_INT (*arg);
		e_calendar_item_set_first_month (calitem, calitem->year,
						 ivalue);
		break;
	case ARG_X1:
		dvalue = GTK_VALUE_DOUBLE (*arg);
		if (calitem->x1 != dvalue) {
			calitem->x1 = dvalue;
			need_update = TRUE;
		}
		break;
	case ARG_Y1:
		dvalue = GTK_VALUE_DOUBLE (*arg);
		if (calitem->y1 != dvalue) {
			calitem->y1 = dvalue;
			need_update = TRUE;
		}
		break;
	case ARG_X2:
		dvalue = GTK_VALUE_DOUBLE (*arg);
		if (calitem->x2 != dvalue) {
			calitem->x2 = dvalue;
			need_update = TRUE;
		}
		break;
	case ARG_Y2:
		dvalue = GTK_VALUE_DOUBLE (*arg);
		if (calitem->y2 != dvalue) {
			calitem->y2 = dvalue;
			need_update = TRUE;
		}
		break;
	case ARG_FONT:
		font = GTK_VALUE_BOXED (*arg);
		if (calitem->font != font) {
			if (calitem->font)
				gdk_font_unref (calitem->font);
			calitem->font = font;
			if (font)
				gdk_font_ref (font);
			need_update = TRUE;
		}
		break;
	case ARG_WEEK_NUMBER_FONT:
		font = GTK_VALUE_BOXED (*arg);
		if (calitem->week_number_font != font) {
			if (calitem->week_number_font)
				gdk_font_unref (calitem->week_number_font);
			calitem->week_number_font = font;
			if (font)
				gdk_font_ref (font);
			need_update = TRUE;
		}
		break;
	case ARG_MINIMUM_ROWS:
		ivalue = GTK_VALUE_INT (*arg);
		ivalue = MAX (1, ivalue);
		if (calitem->min_rows != ivalue) {
			calitem->min_rows = ivalue;
			need_update = TRUE;
		}
		break;
	case ARG_MINIMUM_COLUMNS:
		ivalue = GTK_VALUE_INT (*arg);
		ivalue = MAX (1, ivalue);
		if (calitem->min_cols != ivalue) {
			calitem->min_cols = ivalue;
			need_update = TRUE;
		}
		break;
	case ARG_MAXIMUM_ROWS:
		ivalue = GTK_VALUE_INT (*arg);
		if (calitem->max_rows != ivalue) {
			calitem->max_rows = ivalue;
			need_update = TRUE;
		}
		break;
	case ARG_MAXIMUM_COLUMNS:
		ivalue = GTK_VALUE_INT (*arg);
		if (calitem->max_cols != ivalue) {
			calitem->max_cols = ivalue;
			need_update = TRUE;
		}
		break;
	case ARG_WEEK_START_DAY:
		ivalue = GTK_VALUE_INT (*arg);
		if (calitem->week_start_day != ivalue) {
			calitem->week_start_day = ivalue;
			need_update = TRUE;
		}
		break;
	case ARG_SHOW_WEEK_NUMBERS:
		bvalue = GTK_VALUE_BOOL (*arg);
		if (calitem->show_week_numbers != bvalue) {
			calitem->show_week_numbers = bvalue;
			need_update = TRUE;
		}
		break;
	case ARG_MAXIMUM_DAYS_SELECTED:
		ivalue = GTK_VALUE_INT (*arg);
		ivalue = MAX (1, ivalue);
		calitem->max_days_selected = ivalue;
		break;
	case ARG_DAYS_TO_START_WEEK_SELECTION:
		ivalue = GTK_VALUE_INT (*arg);
		calitem->days_to_start_week_selection = ivalue;
		break;
	case ARG_MOVE_SELECTION_WHEN_MOVING:
		bvalue = GTK_VALUE_BOOL (*arg);
		calitem->move_selection_when_moving = bvalue;
		break;
	case ARG_ROUND_SELECTION_WHEN_MOVING:
		bvalue = GTK_VALUE_BOOL (*arg);
		calitem->round_selection_when_moving = bvalue;
		break;
	default:
		g_warning ("Invalid arg");
	}

	if (need_update) {
		gnome_canvas_item_request_update (item);
	}
}


static void
e_calendar_item_realize		(GnomeCanvasItem *item)
{
	ECalendarItem *calitem;
	GdkColormap *colormap;
	gboolean success[E_CALENDAR_ITEM_COLOR_LAST];
	gint nfailed;

	if (GNOME_CANVAS_ITEM_CLASS (parent_class)->realize)
		(* GNOME_CANVAS_ITEM_CLASS (parent_class)->realize) (item);

	calitem = E_CALENDAR_ITEM (item);

	colormap = gtk_widget_get_colormap (GTK_WIDGET (item->canvas));

	calitem->colors[E_CALENDAR_ITEM_COLOR_TODAY_BOX].red   = 65535;
	calitem->colors[E_CALENDAR_ITEM_COLOR_TODAY_BOX].green = 0;
	calitem->colors[E_CALENDAR_ITEM_COLOR_TODAY_BOX].blue  = 0;

	calitem->colors[E_CALENDAR_ITEM_COLOR_SELECTION_FG].red   = 65535;
	calitem->colors[E_CALENDAR_ITEM_COLOR_SELECTION_FG].green = 65535;
	calitem->colors[E_CALENDAR_ITEM_COLOR_SELECTION_FG].blue  = 65535;

	calitem->colors[E_CALENDAR_ITEM_COLOR_SELECTION_BG].red   = 47000;
	calitem->colors[E_CALENDAR_ITEM_COLOR_SELECTION_BG].green = 47000;
	calitem->colors[E_CALENDAR_ITEM_COLOR_SELECTION_BG].blue  = 48000;

	calitem->colors[E_CALENDAR_ITEM_COLOR_PREV_OR_NEXT_MONTH_FG].red   = 47000;
	calitem->colors[E_CALENDAR_ITEM_COLOR_PREV_OR_NEXT_MONTH_FG].green = 47000;
	calitem->colors[E_CALENDAR_ITEM_COLOR_PREV_OR_NEXT_MONTH_FG].blue  = 48000;

	nfailed = gdk_colormap_alloc_colors (colormap, calitem->colors,
					     E_CALENDAR_ITEM_COLOR_LAST, FALSE,
					     TRUE, success);
	if (nfailed)
		g_warning ("Failed to allocate all colors");
}


static void
e_calendar_item_unrealize	(GnomeCanvasItem *item)
{
	ECalendarItem *calitem;
	GdkColormap *colormap;
	gint i;

	calitem = E_CALENDAR_ITEM (item);

	colormap = gtk_widget_get_colormap (GTK_WIDGET (item->canvas));

	for (i = 0; i < E_CALENDAR_ITEM_COLOR_LAST; i++)
		gdk_colors_free (colormap, &calitem->colors[i].pixel, 1, 0);

	if (GNOME_CANVAS_ITEM_CLASS (parent_class)->unrealize)
		(* GNOME_CANVAS_ITEM_CLASS (parent_class)->unrealize) (item);
}


static void
e_calendar_item_unmap		(GnomeCanvasItem *item)
{
	ECalendarItem *calitem;

	calitem = E_CALENDAR_ITEM (item);

	if (calitem->selecting) {
		gnome_canvas_item_ungrab (item, GDK_CURRENT_TIME);
		calitem->selecting = FALSE;
	}

	if (GNOME_CANVAS_ITEM_CLASS (parent_class)->unmap)
		(* GNOME_CANVAS_ITEM_CLASS (parent_class)->unmap) (item);
}


static void
e_calendar_item_update		(GnomeCanvasItem *item,
				 double		 *affine,
				 ArtSVP		 *clip_path,
				 int		  flags)
{
	ECalendarItem *calitem;
	GtkStyle *style;
	GdkFont *font;
	gint char_height, width, height, space, space_per_cal, space_per_cell;
	gint rows, cols, xthickness, ythickness;

	if (GNOME_CANVAS_ITEM_CLASS (parent_class)->update)
		(* GNOME_CANVAS_ITEM_CLASS (parent_class)->update) (item, affine, clip_path, flags);

	calitem = E_CALENDAR_ITEM (item);
	style = GTK_WIDGET (item->canvas)->style;
	xthickness = style->klass->xthickness;
	ythickness = style->klass->ythickness;

	item->x1 = calitem->x1;
	item->y1 = calitem->y1;
	item->x2 = calitem->x2 >= calitem->x1 ? calitem->x2 : calitem->x1;
	item->y2 = calitem->y2 >= calitem->y1 ? calitem->y2 : calitem->y1;

	/*
	 * Calculate the new layout of the calendar.
	 */

	/* Make sure the minimum row width & cell height and the widths of
	   all the digits and characters are up to date. */
	e_calendar_item_recalc_sizes (calitem);

	/* Calculate how many rows & cols we can fit in. */
	width = item->x2 - item->x1;
	height = item->y2 - item->y1;

	width -= xthickness * 2;
	height -= ythickness * 2;

	rows = height / calitem->min_month_height;
	rows = MAX (rows, calitem->min_rows);
	if (calitem->max_rows > 0)
		rows = MIN (rows, calitem->max_rows);
	cols = width / calitem->min_month_width;
	cols = MAX (cols, calitem->min_cols);
	if (calitem->max_cols > 0)
		cols = MIN (cols, calitem->max_cols);

	if (rows != calitem->rows || cols != calitem->cols)
		e_calendar_item_date_range_changed (calitem);

	calitem->rows = rows;
	calitem->cols = cols;

	/* Split up the empty space according to the configuration.
	   If the calendar is set to expand, we divide the space between the
	   cells and the spaces around the calendar, otherwise we place the
	   calendars in the center of the available area. */

	font = calitem->font;
	if (!font)
		font = style->font;
	char_height = font->ascent + font->descent;

	calitem->month_width = calitem->min_month_width;
	calitem->month_height = calitem->min_month_height;
	calitem->cell_width = calitem->max_digit_width * 2
		+ E_CALENDAR_ITEM_MIN_CELL_XPAD;
	calitem->cell_height = char_height
		+ E_CALENDAR_ITEM_MIN_CELL_YPAD;
	calitem->month_tpad = 0;
	calitem->month_bpad = 0;
	calitem->month_lpad = 0;
	calitem->month_rpad = 0;

	space = height - calitem->rows * calitem->month_height;
	if (space > 0) {
		space_per_cal = space / calitem->rows;
		calitem->month_height += space_per_cal;

		if (calitem->expand) {
			space_per_cell = space_per_cal / E_CALENDAR_ROWS_PER_MONTH;
			calitem->cell_height += space_per_cell;
			space_per_cal -= space_per_cell * E_CALENDAR_ROWS_PER_MONTH;
		}

		calitem->month_tpad = space_per_cal / 2;
		calitem->month_bpad = space_per_cal - calitem->month_tpad;
	}

	space = width - calitem->cols * calitem->month_width;
	if (space > 0) {
		space_per_cal = space / calitem->cols;
		calitem->month_width += space_per_cal;
		space -= space_per_cal * calitem->cols;

		if (calitem->expand) {
			space_per_cell = space_per_cal / E_CALENDAR_COLS_PER_MONTH;
			calitem->cell_width += space_per_cell;
			space_per_cal -= space_per_cell * E_CALENDAR_COLS_PER_MONTH;
		}

		calitem->month_lpad = space_per_cal / 2;
		calitem->month_rpad = space_per_cal - calitem->month_lpad;
	}

	space = MAX (0, space);
	calitem->x_offset = space / 2;

	gnome_canvas_request_redraw (item->canvas, item->x1, item->y1,
				     item->x2, item->y2);
}


/*
 * DRAWING ROUTINES - functions to paint the canvas item.
 */

static void
e_calendar_item_draw		(GnomeCanvasItem *canvas_item,
				 GdkDrawable	 *drawable,
				 int		  x,
				 int		  y,
				 int		  width,
				 int		  height)
{
	ECalendarItem *calitem;
	GtkStyle *style;
	GdkFont *font;
	GdkGC *base_gc, *bg_gc;
	gint char_height, row, col, row_y, bar_height, col_x;
	gint xthickness, ythickness;

#if 0
	g_print ("In e_calendar_item_draw %i,%i %ix%i\n",
		 x, y, width, height);
#endif
	calitem = E_CALENDAR_ITEM (canvas_item);
	style = GTK_WIDGET (canvas_item->canvas)->style;
	font = calitem->font;
	if (!font)
		font = style->font;
	char_height = font->ascent + font->descent;
	xthickness = style->klass->xthickness;
	ythickness = style->klass->ythickness;
	base_gc = style->base_gc[GTK_STATE_NORMAL];
	bg_gc = style->bg_gc[GTK_STATE_NORMAL];

	/* Clear the entire background. */
	gdk_draw_rectangle (drawable, base_gc, TRUE,
			    calitem->x1 - x, calitem->y1 - y,
			    calitem->x2 - calitem->x1 + 1,
			    calitem->y2 - calitem->y1 + 1);

	/* Draw the shadow around the entire item. */
	gtk_draw_shadow (style, drawable,
			 GTK_STATE_NORMAL, GTK_SHADOW_IN,
			 calitem->x1 - x, calitem->y1 - y,
			 calitem->x2 - calitem->x1 + 1,
			 calitem->y2 - calitem->y1 + 1);

	row_y = canvas_item->y1 + ythickness;
	bar_height = ythickness * 2
		+ E_CALENDAR_ITEM_YPAD_ABOVE_MONTH_NAME + char_height
		+ E_CALENDAR_ITEM_YPAD_BELOW_MONTH_NAME;

	for (row = 0; row < calitem->rows; row++) {
		/* Draw the background for the title bars and the shadow around
		   it, and the vertical lines between columns. */

		gdk_draw_rectangle (drawable, bg_gc, TRUE,
				    calitem->x1 + xthickness - x, row_y - y,
				    calitem->x2 - calitem->x1 + 1
				    - xthickness * 2,
				    bar_height);

		gtk_draw_shadow (style, drawable,
				 GTK_STATE_NORMAL, GTK_SHADOW_OUT,
				 calitem->x1 + xthickness - x, row_y - y,
				 calitem->x2 - calitem->x1 + 1
				 - xthickness * 2,
				 bar_height);


		for (col = 0; col < calitem->cols; col++) {
			if (col != 0) {
				col_x = calitem->x1 + calitem->x_offset
					+ calitem->month_width * col;
				gtk_draw_vline (style, drawable,
						GTK_STATE_NORMAL,
						row_y + ythickness + 1 - y,
						row_y + bar_height
						- ythickness - 2 - y,
						col_x - 1 - x);
			}


			e_calendar_item_draw_month (calitem, drawable, x, y,
						    width, height, row, col);
		}

		row_y += calitem->month_height;
	}
}


static void
e_calendar_item_draw_month	(ECalendarItem   *calitem,
				 GdkDrawable	 *drawable,
				 int		  x,
				 int		  y,
				 int		  width,
				 int		  height,
				 int		  row,
				 int		  col)
{
	GnomeCanvasItem *item;
	GtkWidget *widget;
	GtkStyle *style;
	GdkFont *font;
	GdkGC *fg_gc;
	struct tm tmp_tm;
	GdkRectangle clip_rect;
	gint char_height, xthickness, ythickness, start_weekday;
	gint year, month;
	gint month_x, month_y, month_w, month_h;
	gint min_x, max_x, text_x, text_y;
	gint day, day_index, cells_x, cells_y, min_cell_width, text_width;
	gint clip_width, clip_height;
	gchar buffer[64];

#if 0
	g_print ("In e_calendar_item_draw_month: %i,%i %ix%i row:%i col:%i\n",
		 x, y, width, height, row, col);
#endif
	item = GNOME_CANVAS_ITEM (calitem);
	widget = GTK_WIDGET (item->canvas);
	style = widget->style;
	font = calitem->font;
	if (!font)
		font = style->font;
	char_height = font->ascent + font->descent;
	xthickness = style->klass->xthickness;
	ythickness = style->klass->ythickness;
	fg_gc = style->fg_gc[GTK_STATE_NORMAL];

	/* Calculate the top-left position of the entire month display. */
	month_x = item->x1 + xthickness + calitem->x_offset
		+ col * calitem->month_width - x;
	month_w = item->x2 - item->x1 - xthickness * 2;
	month_w = MIN (month_w, calitem->month_width);
	month_y = item->y1 + ythickness + row * calitem->month_height - y;
	month_h = item->y2 - item->y1 - ythickness * 2;
	month_h = MIN (month_h, calitem->month_height);

	/* Just return if the month is outside the given area. */
	if (month_x >= width || month_x + calitem->month_width <= 0
	    || month_y >= height || month_y + calitem->month_height <= 0)
		return;

	month = calitem->month + row * calitem->cols + col;
	year = calitem->year + month / 12;
	month %= 12;

	/* Draw the month name & year, with clipping. Note that the top row
	   needs extra space around it for the buttons. */
	if (row == 0 && col == 0)
		min_x = E_CALENDAR_ITEM_XPAD_BEFORE_MONTH_NAME_WITH_BUTTON;
	else
		min_x = E_CALENDAR_ITEM_XPAD_BEFORE_MONTH_NAME;

	max_x = month_w;
	if (row == 0 && col == calitem->cols - 1)
		max_x -= E_CALENDAR_ITEM_XPAD_AFTER_MONTH_NAME_WITH_BUTTON;
	else
		max_x -= E_CALENDAR_ITEM_XPAD_AFTER_MONTH_NAME;

	text_y = month_y + style->klass->ythickness
		+ E_CALENDAR_ITEM_YPAD_ABOVE_MONTH_NAME;
	clip_rect.x = month_x + min_x;
	clip_rect.x = MAX (0, clip_rect.x);
	clip_rect.y = MAX (0, text_y);

	memset (&tmp_tm, 0, sizeof (tmp_tm));
	tmp_tm.tm_year = year - 1900;
	tmp_tm.tm_mon = month;
	tmp_tm.tm_mday = 1;
	tmp_tm.tm_isdst = -1;
	mktime (&tmp_tm);
	start_weekday = (tmp_tm.tm_wday + 6) % 7;

	if (month_x + max_x - clip_rect.x > 0) {
		clip_rect.width = month_x + max_x - clip_rect.x;
		clip_rect.height = text_y + char_height - clip_rect.y;
		gdk_gc_set_clip_rectangle (fg_gc, &clip_rect);

		/* This is a strftime() format. %B = Month name, %Y = Year. */
		strftime (buffer, sizeof (buffer), _("%B %Y"), &tmp_tm);

		/* Ideally we place the text centered in the month, but we
		   won't go to the left of the minimum x position. */
		text_width = gdk_string_width (font, buffer);
		text_x = (calitem->month_width - text_width) / 2;
		text_x = MAX (min_x, text_x);

		gdk_draw_string (drawable, font, fg_gc,
				 month_x + text_x, text_y + font->ascent, buffer);
	}

	/* Set the clip rectangle for the main month display. */
	clip_rect.x = MAX (0, month_x);
	clip_rect.y = MAX (0, month_y);
	clip_width = month_x + month_w - clip_rect.x;
	clip_height = month_y + month_h - clip_rect.y;

	if (clip_width <= 0 || clip_height <= 0)
		return;

	clip_rect.width = clip_width;
	clip_rect.height = clip_height;

	gdk_gc_set_clip_rectangle (fg_gc, &clip_rect);


	/* Draw the day initials across the top of the month. */
	min_cell_width = calitem->max_digit_width * 2
		+ E_CALENDAR_ITEM_MIN_CELL_XPAD;

	cells_x = month_x + E_CALENDAR_ITEM_XPAD_BEFORE_WEEK_NUMBERS + calitem->month_lpad
		+ E_CALENDAR_ITEM_XPAD_BEFORE_CELLS;
	if (calitem->show_week_numbers)
		cells_x += calitem->max_week_number_digit_width * 2
			+ E_CALENDAR_ITEM_XPAD_AFTER_WEEK_NUMBERS + 1;
	text_x = cells_x + calitem->cell_width
		- (calitem->cell_width - min_cell_width) / 2;
	text_x -= E_CALENDAR_ITEM_MIN_CELL_XPAD / 2;
	text_y = month_y + ythickness * 2
		+ E_CALENDAR_ITEM_YPAD_ABOVE_MONTH_NAME
		+ char_height + E_CALENDAR_ITEM_YPAD_BELOW_MONTH_NAME
		+ E_CALENDAR_ITEM_YPAD_ABOVE_DAY_LETTERS + calitem->month_tpad;

	cells_y = text_y + char_height
		+ E_CALENDAR_ITEM_YPAD_BELOW_DAY_LETTERS + 1
		+ E_CALENDAR_ITEM_YPAD_ABOVE_CELLS;

	text_y += font->ascent;
	day_index = calitem->week_start_day;
	for (day = 0; day < 7; day++) {
		gdk_draw_text (drawable, font, fg_gc,
			       text_x - calitem->day_widths[day_index], text_y,
			       &calitem->days[day_index], 1);
		text_x += calitem->cell_width;
		day_index++;
		if (day_index == 7)
			day_index = 0;
	}


	/* Draw the horizontal line beneath the day initials. */
	gdk_draw_line (drawable, fg_gc,
		       cells_x - E_CALENDAR_ITEM_XPAD_BEFORE_CELLS,
		       cells_y - E_CALENDAR_ITEM_YPAD_ABOVE_CELLS - 1,
		       cells_x + E_CALENDAR_COLS_PER_MONTH * calitem->cell_width - 1,
		       cells_y - E_CALENDAR_ITEM_YPAD_ABOVE_CELLS - 1);

	e_calendar_item_draw_day_numbers (calitem, drawable, width, height,
					  row, col, year, month, start_weekday,
					  cells_x, cells_y);

	/* Draw the vertical line after the week number. */
	if (calitem->show_week_numbers) {
		gdk_draw_line (drawable, fg_gc,
			       cells_x - E_CALENDAR_ITEM_XPAD_BEFORE_CELLS - 1,
			       cells_y - E_CALENDAR_ITEM_YPAD_ABOVE_CELLS - 1,
			       cells_x - E_CALENDAR_ITEM_XPAD_BEFORE_CELLS - 1,
			       cells_y + E_CALENDAR_ROWS_PER_MONTH * calitem->cell_height - 1);
	}

	gdk_gc_set_clip_rectangle (fg_gc, NULL);
}


static void
e_calendar_item_draw_day_numbers (ECalendarItem	*calitem,
				  GdkDrawable	*drawable,
				  int		 width,
				  int		 height,
				  int		 row,
				  int		 col,
				  int		 year,
				  int		 month,
				  int		 start_weekday,
				  gint		 cells_x,
				  gint		 cells_y)
{
	GnomeCanvasItem *item;
	GtkWidget *widget;
	GtkStyle *style;
	GdkFont *font, *wkfont;
	GdkGC *fg_gc;
	GdkColor *bg_color, *fg_color, *box_color;
	struct tm *today_tm;
	time_t t;
	gint char_height, min_cell_width, min_cell_height;
	gint day_num, drow, dcol, day_x, day_y;
	gint text_x, text_y;
	gint num_chars, digit;
	gint week_num, mon, days_from_week_start;
	gint years[3], months[3], days_in_month[3];
	gboolean today, selected, has_focus = FALSE, drop_target = FALSE;
	gboolean bold, draw_day, finished = FALSE;
	gint today_year, today_month, today_mday, month_offset;
	gchar buffer[2];
	gint day_style = 0;

	item = GNOME_CANVAS_ITEM (calitem);
	widget = GTK_WIDGET (item->canvas);
	style = widget->style;
	font = calitem->font;
	if (!font)
		font = style->font;
	wkfont = calitem->week_number_font;
	if (!wkfont)
		wkfont = font;
	fg_gc = style->fg_gc[GTK_STATE_NORMAL];
	char_height = font->ascent + font->descent;

	min_cell_width = calitem->max_digit_width * 2
		+ E_CALENDAR_ITEM_MIN_CELL_XPAD;
	min_cell_height = char_height + E_CALENDAR_ITEM_MIN_CELL_YPAD;

	/* Calculate the number of days in the previous, current, and next
	   months. */
	years[0] = years[1] = years[2] = year;
	months[0] = month - 1;
	months[1] = month;
	months[2] = month + 1;
	if (months[0] == -1) {
		months[0] = 11;
		years[0]--;
	}
	if (months[2] == 12) {
		months[2] = 0;
		years[2]++;
	}

	days_in_month[0] = DAYS_IN_MONTH (years[0], months[0]);
	days_in_month[1] = DAYS_IN_MONTH (years[1], months[1]);
	days_in_month[2] = DAYS_IN_MONTH (years[2], months[2]);

	/* Mon 0 is the previous month, which we may show the end of. Mon 1 is
	   the current month, and mon 2 is the next month. */
	mon = 0;

	month_offset = row * calitem->cols + col - 1;
	day_num = days_in_month[0];
	days_from_week_start = (start_weekday + 7 - calitem->week_start_day)
		% 7;
	/* For the top-left month we show the end of the previous month, and
	   if the new month starts on the first day of the week we show a
	   complete week from the previous month. */
	if (days_from_week_start == 0) {
		if (row == 0 && col == 0) {
			day_num -= 6;
		} else {
			mon++;
			month_offset++;
			day_num = 1;
		}
	} else {
		day_num -= days_from_week_start - 1;
	}

	/* Get today's date, so we can highlight it. */
	t = time (NULL);
	today_tm = localtime (&t);
	today_year = today_tm->tm_year + 1900;
	today_month = today_tm->tm_mon;
	today_mday = today_tm->tm_mday;

	/* We usually skip the last days of the previous month (mon = 0),
	   except for the top-left month displayed. */
	draw_day = (mon == 1 || (row == 0 && col == 0));

	for (drow = 0; drow < 6; drow++) {
		/* Draw the week number. */
		if (calitem->show_week_numbers) {
			week_num = e_calendar_item_get_week_number (calitem,
								    day_num,
								    months[mon],
								    years[mon]);

			text_x = cells_x - E_CALENDAR_ITEM_XPAD_BEFORE_CELLS - 1
				- E_CALENDAR_ITEM_XPAD_AFTER_WEEK_NUMBERS;
			text_y = cells_y + drow * calitem->cell_height +
				+ (calitem->cell_height - min_cell_height + 1) / 2;

			num_chars = 0;
			if (week_num >= 10) {
				digit = week_num / 10;
				text_x -= calitem->week_number_digit_widths[digit];
				buffer[num_chars++] = digit + '0';
			}

			digit = week_num % 10;
			text_x -= calitem->week_number_digit_widths[digit];
			buffer[num_chars++] = digit + '0';

			gdk_gc_set_foreground (fg_gc,
					       &style->fg[GTK_STATE_NORMAL]);
			gdk_draw_text (drawable, wkfont, fg_gc,
				       text_x, text_y + font->ascent,
				       buffer, num_chars);
		}

		for (dcol = 0; dcol < 7; dcol++) {
			if (draw_day) {
				day_x = cells_x + dcol * calitem->cell_width;
				day_y = cells_y + drow * calitem->cell_height;

				today = years[mon] == today_year
					&& months[mon] == today_month
					&& day_num == today_mday;

				selected = calitem->selection_set
					&& (calitem->selection_start_month_offset < month_offset
					    || (calitem->selection_start_month_offset == month_offset
						&& calitem->selection_start_day <= day_num))
					&& (calitem->selection_end_month_offset > month_offset
					    || (calitem->selection_end_month_offset == month_offset
						&& calitem->selection_end_day >= day_num));

				if (calitem->styles)
					day_style = calitem->styles[(month_offset + 1) * 32 + day_num];

				/* Get the colors & style to use for the day.*/
				if (calitem->style_callback)
					(*calitem->style_callback)
						(calitem,
						 years[mon],
						 months[mon],
						 day_num,
						 day_style,
						 today,
						 mon != 1,
						 selected,
						 has_focus,
						 drop_target,
						 &bg_color,
						 &fg_color,
						 &box_color,
						 &bold,
						 calitem->style_callback_data);
				else
					e_calendar_item_get_day_style
						(calitem,
						 years[mon],
						 months[mon],
						 day_num,
						 day_style,
						 today,
						 mon != 1,
						 selected,
						 has_focus,
						 drop_target,
						 &bg_color,
						 &fg_color,
						 &box_color,
						 &bold);

				/* Draw the background, if set. */
				if (bg_color) {
					gdk_gc_set_foreground (fg_gc, bg_color);
					gdk_draw_rectangle (drawable, fg_gc,
							    TRUE,
							    day_x, day_y,
							    calitem->cell_width,
							    calitem->cell_height);
				}

				/* Draw the box, if set. */
				if (box_color) {
					gdk_gc_set_foreground (fg_gc, box_color);
					gdk_draw_rectangle (drawable, fg_gc,
							    FALSE,
							    day_x, day_y,
							    calitem->cell_width - 1,
							    calitem->cell_height - 1);
				}

				/* Draw the 1- or 2-digit day number. */
				day_x += calitem->cell_width - (calitem->cell_width - min_cell_width) / 2;
				day_x -= E_CALENDAR_ITEM_MIN_CELL_XPAD / 2;
				day_y += (calitem->cell_height - min_cell_height + 1) / 2;
				day_y += E_CALENDAR_ITEM_MIN_CELL_YPAD / 2;

				num_chars = 0;
				if (day_num >= 10) {
					digit = day_num / 10;
					day_x -= calitem->digit_widths[digit];
					buffer[num_chars++] = digit + '0';
				}

				digit = day_num % 10;
				day_x -= calitem->digit_widths[digit];
				buffer[num_chars++] = digit + '0';

				if (fg_color) {
					gdk_gc_set_foreground (fg_gc,
							       fg_color);
				} else {
					gdk_gc_set_foreground (fg_gc,
							       &style->fg[GTK_STATE_NORMAL]);
				}

				gdk_draw_text (drawable, font, fg_gc,
					       day_x,
					       day_y + font->ascent,
					       buffer, num_chars);
				/* We use a stupid technique for bold. Just
				   draw it again 1 pixel to the left. */
				if (bold)
					gdk_draw_text (drawable, font, fg_gc,
						       day_x - 1,
						       day_y + font->ascent,
						       buffer, num_chars);
			}

			/* See if we've reached the end of a month. */
			if (day_num == days_in_month[mon]) {
				month_offset++;
				mon++;
				/* We only draw the start of the next month
				   for the bottom-right month displayed. */
				if (mon == 2 && (row != calitem->rows - 1
						 || col != calitem->cols - 1)) {
					/* Set a flag so we exit the loop. */
					finished = TRUE;
					break;
				}
				day_num = 1;
				draw_day = TRUE;
			} else {
				day_num++;
			}
		}

		/* Exit the loop if the flag is set. */
		if (finished)
			break;
	}

	/* Reset the foreground color. */
	gdk_gc_set_foreground (fg_gc, &style->fg[GTK_STATE_NORMAL]);
}


static gint
e_calendar_item_get_week_number	(ECalendarItem *calitem,
				 gint		day,
				 gint		month,
				 gint		year)
{
	GDate tmp_date;
	gint weekday, yearday, offset, week_num;

	/* FIXME: check what happens at year boundaries. */

	g_date_clear (&tmp_date, 1);
	g_date_set_dmy (&tmp_date, day, month + 1, year);

	/* This results in a value of 0 (Monday) - 6 (Sunday). */
	weekday = g_date_weekday (&tmp_date) - 1;

	/* Calculate the offset from the start of the week. */
	offset = (calitem->week_start_day + 7 - weekday) % 7;

	/* Calculate the day of the year, from 0 to 365. */
	yearday = g_date_day_of_year (&tmp_date) - 1;

	/* If the week starts on or after 29th December, it is week 1 of the
	   next year, since there are 4 days in the next year. */
	g_date_subtract_days (&tmp_date, offset);
	if (g_date_month (&tmp_date) == 12 && g_date_day (&tmp_date) >= 29)
		return 1;

	/* Calculate the week number, from 0. */
	week_num = (yearday - offset) / 7;

	/* If the first week starts on or after Jan 5th, then we need to add
	   1 since the previous week will really be the first week. */
	if ((yearday - offset) % 7 >= 4)
		week_num++;

	/* Add 1 so week numbers are from 1 to 53. */
	return week_num + 1;
}



/* This is supposed to return the nearest item the the point and the distance.
   Since we are the only item we just return ourself and 0 for the distance.
   This is needed so that we get button/motion events. */
static double
e_calendar_item_point (GnomeCanvasItem *item, double x, double y,
			   int cx, int cy,
			   GnomeCanvasItem **actual_item)
{
	*actual_item = item;
	return 0.0;
}


static gint
e_calendar_item_event (GnomeCanvasItem *item, GdkEvent *event)
{
	ECalendarItem *calitem;

	calitem = E_CALENDAR_ITEM (item);

	switch (event->type) {
	case GDK_BUTTON_PRESS:
		return e_calendar_item_button_press (calitem, event);
	case GDK_BUTTON_RELEASE:
		return e_calendar_item_button_release (calitem, event);
	case GDK_MOTION_NOTIFY:
		return e_calendar_item_motion (calitem, event);
	default:
		break;
	}

	return FALSE;
}



/* This checks if any fonts have changed, and if so it recalculates the
   text sizes and the minimum month size. */
static void
e_calendar_item_recalc_sizes		(ECalendarItem *calitem)
{
	GnomeCanvasItem *canvas_item;
	GtkStyle *style;
	GdkFont *font, *wkfont;
	gchar *digits = "0123456789";
	gint day, digit, max_digit_width, max_week_number_digit_width;
	gint char_height, width, min_cell_width, min_cell_height;

	canvas_item = GNOME_CANVAS_ITEM (calitem);
	style = GTK_WIDGET (canvas_item->canvas)->style;

	font = calitem->font;
	if (!font)
		font = style->font;
	wkfont = calitem->week_number_font;
	if (!wkfont)
		wkfont = font;
	char_height = font->ascent + font->descent;

	g_return_if_fail (font != NULL);
	g_return_if_fail (wkfont != NULL);

	/* If both fonts are the same, just return. */
	if (font != calitem->old_font
	    || wkfont != calitem->old_week_number_font) {
		if (calitem->old_font)
			gdk_font_unref (calitem->old_font);
		calitem->old_font = font;
		gdk_font_ref (font);

		if (calitem->old_week_number_font)
			gdk_font_unref (calitem->old_week_number_font);
		calitem->old_week_number_font = wkfont;
		gdk_font_ref (wkfont);

		for (day = 0; day < 7; day++)
			calitem->day_widths[day] = gdk_char_width (font, calitem->days[day]);

		max_digit_width = 0;
		max_week_number_digit_width = 0;
		for (digit = 0; digit < 10; digit++) {
			width = gdk_char_width (font, digits[digit]);
			calitem->digit_widths[digit] = width;
			max_digit_width = MAX (max_digit_width, width);

			if (wkfont) {
				width = gdk_char_width (wkfont, digits[digit]);
				calitem->week_number_digit_widths[digit] = width;
				max_week_number_digit_width = MAX (max_week_number_digit_width, width);
			} else {
				calitem->week_number_digit_widths[digit] = width;
				max_week_number_digit_width = max_digit_width;
			}
		}
		calitem->max_digit_width = max_digit_width;
		calitem->max_week_number_digit_width = max_week_number_digit_width;
	}

	min_cell_width = calitem->max_digit_width * 2
		+ E_CALENDAR_ITEM_MIN_CELL_XPAD;
	min_cell_height = char_height + E_CALENDAR_ITEM_MIN_CELL_YPAD;

	calitem->min_month_width = E_CALENDAR_ITEM_XPAD_BEFORE_WEEK_NUMBERS
		+ E_CALENDAR_ITEM_XPAD_BEFORE_CELLS + min_cell_width * 7
		+ E_CALENDAR_ITEM_XPAD_AFTER_CELLS;
	if (calitem->show_week_numbers) {
		calitem->min_month_width += calitem->max_week_number_digit_width * 2
			+ E_CALENDAR_ITEM_XPAD_AFTER_WEEK_NUMBERS + 1;
	}

	calitem->min_month_height = style->klass->ythickness * 2
		+ E_CALENDAR_ITEM_YPAD_ABOVE_MONTH_NAME + char_height
		+ E_CALENDAR_ITEM_YPAD_BELOW_MONTH_NAME + 1
		+ E_CALENDAR_ITEM_YPAD_ABOVE_DAY_LETTERS
		+ char_height + E_CALENDAR_ITEM_YPAD_BELOW_DAY_LETTERS + 1
		+ E_CALENDAR_ITEM_YPAD_ABOVE_CELLS + min_cell_height * 6
		+ E_CALENDAR_ITEM_YPAD_BELOW_CELLS;
}


static void
e_calendar_item_get_day_style		(ECalendarItem	*calitem,
					 gint		 year,
					 gint		 month,
					 gint		 day,
					 gint		 day_style,
					 gboolean	 today,
					 gboolean	 prev_or_next_month,
					 gboolean	 selected,
					 gboolean	 has_focus,
					 gboolean	 drop_target,
					 GdkColor      **bg_color,
					 GdkColor      **fg_color,
					 GdkColor      **box_color,
					 gboolean	*bold)
{
	*bg_color = NULL;
	*fg_color = NULL;
	*box_color = NULL;
	*bold = FALSE;

	if (day_style == 1)
		*bold = TRUE;

	if (today)
		*box_color = &calitem->colors[E_CALENDAR_ITEM_COLOR_TODAY_BOX];

	if (prev_or_next_month)
		*fg_color = &calitem->colors[E_CALENDAR_ITEM_COLOR_PREV_OR_NEXT_MONTH_FG];

	if (selected) {
		*fg_color = &calitem->colors[E_CALENDAR_ITEM_COLOR_SELECTION_FG];
		*bg_color = &calitem->colors[E_CALENDAR_ITEM_COLOR_SELECTION_BG];
	}
}



static gboolean
e_calendar_item_button_press	(ECalendarItem	*calitem,
				 GdkEvent	*event)
{
	gint month_offset, day;
	gboolean all_week, round_up_end = FALSE, round_down_start = FALSE;

	if (event->button.button == 4)
		e_calendar_item_set_first_month (calitem, calitem->year,
						 calitem->month - 1);
	else if (event->button.button == 5)
		e_calendar_item_set_first_month (calitem, calitem->year,
						 calitem->month + 1);

	if (!e_calendar_item_convert_position_to_day (calitem,
						      event->button.x,
						      event->button.y,
						      TRUE,
						      &month_offset, &day,
						      &all_week))
		return FALSE;

	if (event->button.button == 3 && day == -1) {
		e_calendar_item_show_popup_menu (calitem,
						 (GdkEventButton*) event,
						 month_offset);
		return TRUE;
	}

	if (event->button.button != 1 || day == -1)
		return FALSE;

	if (gnome_canvas_item_grab (GNOME_CANVAS_ITEM (calitem),
				    GDK_POINTER_MOTION_MASK
				    | GDK_BUTTON_RELEASE_MASK,
				    NULL, event->button.time) != 0)
		return FALSE;

	calitem->selection_set = TRUE;
	calitem->selection_start_month_offset = month_offset;
	calitem->selection_start_day = day;
	calitem->selection_end_month_offset = month_offset;
	calitem->selection_end_day = day;

	calitem->selection_real_start_month_offset = month_offset;
	calitem->selection_real_start_day = day;

	calitem->selection_from_full_week = FALSE;
	calitem->selecting = TRUE;
	calitem->selection_dragging_end = TRUE;

	if (all_week) {
		calitem->selection_from_full_week = TRUE;
		round_up_end = TRUE;
	}

	if (calitem->days_to_start_week_selection == 1) {
		round_down_start = TRUE;
		round_up_end = TRUE;
	}

	/* Don't round up or down if we can't select a week or more. */
	if (calitem->max_days_selected < 7) {
		round_down_start = FALSE;
		round_up_end = FALSE;
	}

	if (round_up_end)
		e_calendar_item_round_up_selection (calitem, &calitem->selection_end_month_offset, &calitem->selection_end_day);

	if (round_down_start)
		e_calendar_item_round_down_selection (calitem, &calitem->selection_start_month_offset, &calitem->selection_start_day);

	gnome_canvas_item_request_update (GNOME_CANVAS_ITEM (calitem));

	return TRUE;
}


static gboolean
e_calendar_item_button_release	(ECalendarItem	*calitem,
				 GdkEvent	*event)
{
	if (!calitem->selecting)
		return FALSE;

	gnome_canvas_item_ungrab (GNOME_CANVAS_ITEM (calitem),
				  event->button.time);

	calitem->selecting = FALSE;

	/* If the user selects the grayed dates before the first month or
	   after the last month, we move backwards or forwards one month.
	   The set_month() call should take care of updating the selection. */
	if (calitem->selection_end_month_offset == -1)
		e_calendar_item_set_first_month (calitem, calitem->year,
						 calitem->month - 1);
	else if (calitem->selection_start_month_offset == calitem->rows * calitem->cols)
		e_calendar_item_set_first_month (calitem, calitem->year,
						 calitem->month + 1);

	calitem->selection_changed = TRUE;
	e_calendar_item_queue_signal_emission (calitem);
	gnome_canvas_item_request_update (GNOME_CANVAS_ITEM (calitem));

	return FALSE;
}


static gboolean
e_calendar_item_motion		(ECalendarItem	*calitem,
				 GdkEvent	*event)
{
	gint start_month, start_day, end_month, end_day, month_offset, day;
	gint tmp_month, tmp_day, days_in_selection;
	gboolean all_week, round_up_end = FALSE, round_down_start = FALSE;

	if (!calitem->selecting)
		return FALSE;

	if (!e_calendar_item_convert_position_to_day (calitem,
						      event->button.x,
						      event->button.y,
						      TRUE,
						      &month_offset, &day,
						      &all_week))
		return FALSE;

	if (day == -1)
		return FALSE;

	if (calitem->selection_dragging_end) {
		start_month = calitem->selection_real_start_month_offset;
		start_day = calitem->selection_real_start_day;
		end_month = month_offset;
		end_day = day;
	} else {
		start_month = month_offset;
		start_day = day;
		end_month = calitem->selection_real_start_month_offset;
		end_day = calitem->selection_real_start_day;
	}

	if (start_month > end_month || (start_month == end_month
					&& start_day > end_day)) {
		tmp_month = start_month;
		tmp_day = start_day;
		start_month = end_month;
		start_day = end_day;
		end_month = tmp_month;
		end_day = tmp_day;

		calitem->selection_dragging_end = !calitem->selection_dragging_end;
	}

	if (calitem->days_to_start_week_selection > 0) {
		days_in_selection = e_calendar_item_get_inclusive_days (calitem, start_month, start_day, end_month, end_day);
		if (days_in_selection >= calitem->days_to_start_week_selection) {
			round_down_start = TRUE;
			round_up_end = TRUE;
		}
	}

	/* If we are over a week number and we are dragging the end of the
	   selection, we round up to the end of this week. */
	if (all_week && calitem->selection_dragging_end)
		round_up_end = TRUE;

	/* If the selection was started from a week number and we are dragging
	   the start of the selection, we need to round up the end to include
	   all of the original week selected. */
	if (calitem->selection_from_full_week
	    && !calitem->selection_dragging_end)
			round_up_end = TRUE;

	/* Don't round up or down if we can't select a week or more. */
	if (calitem->max_days_selected < 7) {
		round_down_start = FALSE;
		round_up_end = FALSE;
	}

	if (round_up_end)
		e_calendar_item_round_up_selection (calitem, &end_month,
						    &end_day);
	if (round_down_start)
		e_calendar_item_round_down_selection (calitem, &start_month,
						      &start_day);


	/* Check we don't go over the maximum number of days to select. */
	if (calitem->selection_dragging_end) {
		e_calendar_item_check_selection_end (calitem,
						     start_month,
						     start_day,
						     &end_month,
						     &end_day);
	} else {
		e_calendar_item_check_selection_start (calitem,
						       &start_month,
						       &start_day,
						       end_month,
						       end_day);
	}

	if (start_month == calitem->selection_start_month_offset
	    && start_day == calitem->selection_start_day
	    && end_month == calitem->selection_end_month_offset
	    && end_day == calitem->selection_end_day)
		return FALSE;

	calitem->selection_start_month_offset = start_month;
	calitem->selection_start_day = start_day;
	calitem->selection_end_month_offset = end_month;
	calitem->selection_end_day = end_day;

	gnome_canvas_item_request_update (GNOME_CANVAS_ITEM (calitem));

	return TRUE;
}


static void
e_calendar_item_check_selection_end	(ECalendarItem	*calitem,
					 gint		 start_month,
					 gint		 start_day,
					 gint		*end_month,
					 gint		*end_day)
{
	gint year, month, max_month, max_day, days_in_month;

	if (calitem->max_days_selected <= 0)
		return;

	year = calitem->year;
	month = calitem->month + start_month;
	e_calendar_item_normalize_date (calitem, &year, &month);

	max_month = start_month;
	max_day = start_day + calitem->max_days_selected - 1;

	for (;;) {
		days_in_month = DAYS_IN_MONTH (year, month);
		if (max_day <= days_in_month)
			break;
		max_month++;
		month++;
		if (month == 12) {
			year++;
			month = 0;
		}
		max_day -= days_in_month;
	}

	if (*end_month > max_month) {
		*end_month = max_month;
		*end_day = max_day;
	} else if (*end_month == max_month && *end_day > max_day) {
		*end_day = max_day;
	}
}


static void
e_calendar_item_check_selection_start	(ECalendarItem	*calitem,
					 gint		*start_month,
					 gint		*start_day,
					 gint		 end_month,
					 gint		 end_day)
{
	gint year, month, min_month, min_day, days_in_month;

	if (calitem->max_days_selected <= 0)
		return;

	year = calitem->year;
	month = calitem->month + end_month;
	e_calendar_item_normalize_date (calitem, &year, &month);

	min_month = end_month;
	min_day = end_day - calitem->max_days_selected + 1;

	while (min_day <= 0) {
		min_month--;
		month--;
		if (month == -1) {
			year--;
			month = 11;
		}
		days_in_month = DAYS_IN_MONTH (year, month);
		min_day += days_in_month;
	}

	if (*start_month < min_month) {
		*start_month = min_month;
		*start_day = min_day;
	} else if (*start_month == min_month && *start_day < min_day) {
		*start_day = min_day;
	}
}


/* Converts a position within the item to a month & day.
   The month returned is 0 for the top-left month displayed.
   If the position is over the month heading -1 is returned for the day.
   If the position is over a week number the first day of the week is returned
   and entire_week is set to TRUE.
   It returns FALSE if the position is completely outside all months. */
static gboolean
e_calendar_item_convert_position_to_day	(ECalendarItem	*calitem,
					 gint		 event_x,
					 gint		 event_y,
					 gboolean	 round_empty_positions,
					 gint		*month_offset,
					 gint		*day,
					 gboolean	*entire_week)
{
	GnomeCanvasItem *item;
	GtkWidget *widget;
	GtkStyle *style;
	gint xthickness, ythickness, char_height;
	gint x, y, row, col, cells_x, cells_y, day_row, day_col;
	gint first_day_offset, days_in_month, days_in_prev_month;
	gint week_num_x1, week_num_x2;

	item = GNOME_CANVAS_ITEM (calitem);
	widget = GTK_WIDGET (item->canvas);
	style = widget->style;
	char_height = style->font->ascent + style->font->descent;
	xthickness = style->klass->xthickness;
	ythickness = style->klass->ythickness;

	*entire_week = FALSE;

	x = event_x - xthickness - calitem->x_offset;
	y = event_y - ythickness;

	if (x < 0 || y < 0)
		return FALSE;

	row = y / calitem->month_height;
	col = x / calitem->month_width;

	if (row >= calitem->rows || col >= calitem->cols)
		return FALSE;

	*month_offset = row * calitem->cols + col;

	x = x % calitem->month_width;
	y = y % calitem->month_height;

	if (y < ythickness * 2 + E_CALENDAR_ITEM_YPAD_ABOVE_MONTH_NAME
	    + char_height + E_CALENDAR_ITEM_YPAD_BELOW_MONTH_NAME) {
		*day = -1;
		return TRUE;
	}

	cells_y = ythickness * 2 + E_CALENDAR_ITEM_YPAD_ABOVE_MONTH_NAME
		+ char_height + E_CALENDAR_ITEM_YPAD_BELOW_MONTH_NAME
		+ E_CALENDAR_ITEM_YPAD_ABOVE_DAY_LETTERS + calitem->month_tpad
		+ char_height + E_CALENDAR_ITEM_YPAD_BELOW_DAY_LETTERS + 1
		+ E_CALENDAR_ITEM_YPAD_ABOVE_CELLS;
	y -= cells_y;
	if (y < 0)
		return FALSE;
	day_row = y / calitem->cell_height;
	if (day_row >= E_CALENDAR_ROWS_PER_MONTH)
		return FALSE;

	week_num_x1 = E_CALENDAR_ITEM_XPAD_BEFORE_WEEK_NUMBERS + calitem->month_lpad;

	if (calitem->show_week_numbers) {
		week_num_x2 = week_num_x1
			+ calitem->max_week_number_digit_width * 2;
		if (x >= week_num_x1 && x < week_num_x2)
			*entire_week = TRUE;
		cells_x = week_num_x2 + E_CALENDAR_ITEM_XPAD_AFTER_WEEK_NUMBERS + 1;
	} else {
		cells_x = week_num_x1;
	}

	if (*entire_week) {
		day_col = 0;
	} else {
		cells_x += E_CALENDAR_ITEM_XPAD_BEFORE_CELLS;
		x -= cells_x;
		if (x < 0)
			return FALSE;
		day_col = x / calitem->cell_width;
		if (day_col >= E_CALENDAR_COLS_PER_MONTH)
			return FALSE;
	}

	*day = day_row * E_CALENDAR_COLS_PER_MONTH + day_col;

	e_calendar_item_get_month_info (calitem, row, col, &first_day_offset,
					&days_in_month, &days_in_prev_month);
	if (*day < first_day_offset) {
		if (*entire_week || (row == 0 && col == 0)) {
			(*month_offset)--;
			*day = days_in_prev_month + 1 - first_day_offset
				+ *day;
			return TRUE;
		} else if (round_empty_positions) {
			*day = first_day_offset;
		} else {
			return FALSE;
		}
	}

	*day -= first_day_offset - 1;

	if (*day > days_in_month) {
		if (row == calitem->rows - 1 && col == calitem->cols - 1) {
			(*month_offset)++;
			*day -= days_in_month;
			return TRUE;
		} else if (round_empty_positions) {
			*day = days_in_month;
		} else {
			return FALSE;
		}
	}

	return TRUE;
}


static void
e_calendar_item_get_month_info	(ECalendarItem	*calitem,
				 gint		 row,
				 gint		 col,
				 gint		*first_day_offset,
				 gint		*days_in_month,
				 gint		*days_in_prev_month)
{
	gint year, month, start_weekday, first_day_of_month;
	struct tm tmp_tm = { 0 };

	month = calitem->month + row * calitem->cols + col;
	year = calitem->year + month / 12;
	month = month % 12;

	*days_in_month = DAYS_IN_MONTH (year, month);
	if (month == 0)
		*days_in_prev_month = DAYS_IN_MONTH (year - 1, 11);
	else
		*days_in_prev_month = DAYS_IN_MONTH (year, month - 1);

	tmp_tm.tm_year = year - 1900;
	tmp_tm.tm_mon = month;
	tmp_tm.tm_mday = 1;
	tmp_tm.tm_isdst = -1;
	mktime (&tmp_tm);

	/* Convert to 0 (Monday) to 6 (Sunday). */
	start_weekday = (tmp_tm.tm_wday + 6) % 7;

	first_day_of_month = (start_weekday + 7 - calitem->week_start_day) % 7;

	if (row == 0 && col == 0 && first_day_of_month == 0)
		*first_day_offset = 7;
	else
		*first_day_offset = first_day_of_month;
}


void
e_calendar_item_get_first_month(ECalendarItem	*calitem,
				gint		*year,
				gint		*month)
{
	*year = calitem->year;
	*month = calitem->month;
}


/* This also handles values of month < 0 or > 11 by updating the year. */
void
e_calendar_item_set_first_month(ECalendarItem	*calitem,
				gint		 year,
				gint		 month)
{
	gint new_year, new_month, months_diff, num_months;
	gint old_days_in_selection, new_days_in_selection;

	new_year = year;
	new_month = month;
	e_calendar_item_normalize_date	(calitem, &new_year, &new_month);

	if (calitem->year == new_year && calitem->month == new_month)
		return;

	/* Update the selection. */
	num_months = calitem->rows * calitem->cols;
	months_diff = (new_year - calitem->year) * 12
		+ new_month - calitem->month;

	if (calitem->selection_set) {
		if (!calitem->move_selection_when_moving
		    || (calitem->selection_start_month_offset - months_diff >= 0
			&& calitem->selection_end_month_offset - months_diff < num_months)) {
			calitem->selection_start_month_offset -= months_diff;
			calitem->selection_end_month_offset -= months_diff;
			calitem->selection_real_start_month_offset -= months_diff;

			calitem->year = new_year;
			calitem->month = new_month;
		} else {
			old_days_in_selection = e_calendar_item_get_inclusive_days (calitem, calitem->selection_start_month_offset, calitem->selection_start_day, calitem->selection_end_month_offset, calitem->selection_end_day);

			/* Make sure the selection will be displayed. */
			if (calitem->selection_start_month_offset < 0
			    || calitem->selection_start_month_offset >= num_months) {
				calitem->selection_end_month_offset -= calitem->selection_start_month_offset;
				calitem->selection_start_month_offset = 0;
			}

			/* We want to ensure that the same number of days are
			   selected after we have moved the selection. */
			calitem->year = new_year;
			calitem->month = new_month;

			e_calendar_item_ensure_valid_day (calitem, &calitem->selection_start_month_offset, &calitem->selection_start_day);
			e_calendar_item_ensure_valid_day (calitem, &calitem->selection_end_month_offset, &calitem->selection_end_day);

			if (calitem->round_selection_when_moving) {
				e_calendar_item_round_down_selection (calitem, &calitem->selection_start_month_offset, &calitem->selection_start_day);
			}

			new_days_in_selection = e_calendar_item_get_inclusive_days (calitem, calitem->selection_start_month_offset, calitem->selection_start_day, calitem->selection_end_month_offset, calitem->selection_end_day);

			if (old_days_in_selection != new_days_in_selection)
				e_calendar_item_add_days_to_selection (calitem, old_days_in_selection - new_days_in_selection);

			/* Flag that we need to emit the "selection_changed"
			   signal. We don't want to emit it here since setting
			   the "year" and "month" args would result in 2
			   signals emitted. */
			calitem->selection_changed = TRUE;
		}
	} else {
		calitem->year = new_year;
		calitem->month = new_month;
	}

	e_calendar_item_date_range_changed (calitem);
	gnome_canvas_item_request_update (GNOME_CANVAS_ITEM (calitem));
}


/* This will make sure that the given year & month are valid, i.e. if month
   is < 0 or > 11 the year and month will be updated accordingly. */
static void
e_calendar_item_normalize_date	(ECalendarItem	*calitem,
				 gint		*year,
				 gint		*month)
{
	if (*month >= 0) {
		*year += *month / 12;
		*month = *month % 12;
	} else {
		*year += *month / 12 - 1;
		*month = *month % 12;
		if (*month != 0)
			*month += 12;
	}
}


/* Adds or subtracts days from the selection. It is used when we switch months
   and the selection extends past the end of a month but we want to keep the
   number of days selected the same. days should not be more than 30. */
static void
e_calendar_item_add_days_to_selection	(ECalendarItem	*calitem,
					 gint		 days)
{
	gint year, month, days_in_month;

	year = calitem->year;
	month = calitem->month + calitem->selection_end_month_offset;
	e_calendar_item_normalize_date (calitem, &year,	&month);

	calitem->selection_end_day += days;
	if (calitem->selection_end_day <= 0) {
		month--;
		e_calendar_item_normalize_date (calitem, &year,	&month);
		calitem->selection_end_month_offset--;
		calitem->selection_end_day += DAYS_IN_MONTH (year, month);
	} else {
		days_in_month = DAYS_IN_MONTH (year, month);
		if (calitem->selection_end_day > days_in_month) {
			calitem->selection_end_month_offset++;
			calitem->selection_end_day -= days_in_month;
		}
	}
}


/* Returns the range of dates actually shown. Months are 0 to 11. */
void
e_calendar_item_get_date_range	(ECalendarItem	*calitem,
				 gint		*start_year,
				 gint		*start_month,
				 gint		*start_day,
				 gint		*end_year,
				 gint		*end_month,
				 gint		*end_day)
{
	gint first_day_offset, days_in_month, days_in_prev_month;

	/* Calculate the first day shown. This will be one of the greyed-out
	   days before the first full month begins. */
	e_calendar_item_get_month_info (calitem, 0, 0, &first_day_offset,
					&days_in_month, &days_in_prev_month);
	*start_year = calitem->year;
	*start_month = calitem->month - 1;
	if (*start_month == -1) {
		(*start_year)--;
		*start_month = 11;
	}
	*start_day = days_in_prev_month + 1 - first_day_offset;


	/* Calculate the last day shown. This will be one of the greyed-out
	   days after the last full month ends. */
	e_calendar_item_get_month_info (calitem, calitem->rows - 1,
					calitem->cols - 1, &first_day_offset,
					&days_in_month, &days_in_prev_month);
	*end_month = calitem->month + calitem->rows * calitem->cols;
	*end_year = calitem->year + *end_month / 12;
	*end_month %= 12;
	*end_day = E_CALENDAR_ROWS_PER_MONTH * E_CALENDAR_COLS_PER_MONTH
		- first_day_offset - days_in_month;
}


/* Simple way to mark days so they appear bold.
   A more flexible interface may be added later. */
void
e_calendar_item_clear_marks	(ECalendarItem	*calitem)
{
	GnomeCanvasItem *item;

	item = GNOME_CANVAS_ITEM (calitem);

	g_free (calitem->styles);
	calitem->styles = NULL;

	gnome_canvas_request_redraw (item->canvas, item->x1, item->y1,
				     item->x2, item->y2);
}


void
e_calendar_item_mark_day	(ECalendarItem	*calitem,
				 gint		 year,
				 gint		 month,
				 gint		 day,
				 guint8		 day_style)
{
	gint month_offset;

	month_offset = (year - calitem->year) * 12 + month - calitem->month;
	if (month_offset < -1 || month_offset > calitem->rows * calitem->cols)
		return;

	if (!calitem->styles)
		calitem->styles = g_new0 (guint8, (calitem->rows * calitem->cols + 2) * 32);

	calitem->styles[(month_offset + 1) * 32 + day] = day_style;
}


void
e_calendar_item_mark_days	(ECalendarItem	*calitem,
				 gint		 start_year,
				 gint		 start_month,
				 gint		 start_day,
				 gint		 end_year,
				 gint		 end_month,
				 gint		 end_day,
				 guint8		 day_style)
{
	gint month_offset, end_month_offset, day;

	month_offset = (start_year - calitem->year) * 12 + start_month
		- calitem->month;
	day = start_day;
	if (month_offset > calitem->rows * calitem->cols)
		return;
	if (month_offset < -1) {
		month_offset = -1;
		day = 1;
	}

	end_month_offset = (end_year - calitem->year) * 12 + end_month
		- calitem->month;
	if (end_month_offset < -1)
		return;
	if (end_month_offset > calitem->rows * calitem->cols) {
		end_month_offset = calitem->rows * calitem->cols;
		end_day = 31;
	}

	if (month_offset > end_month_offset)
		return;

	if (!calitem->styles)
		calitem->styles = g_new0 (guint8, (calitem->rows * calitem->cols + 2) * 32);

	for (;;) {
		if (month_offset == end_month_offset && day > end_day)
			break;

		calitem->styles[(month_offset + 1) * 32 + day] = day_style;

		day++;
		if (day == 32) {
			month_offset++;
			day = 1;
			if (month_offset > end_month_offset)
				break;
		}
	}
}


/* Rounds up the given day to the end of the week. */
static void
e_calendar_item_round_up_selection	(ECalendarItem	*calitem,
					 gint		*month_offset,
					 gint		*day)
{
	gint year, month, weekday, days, days_in_month;
	struct tm tmp_tm = { 0 };

	year = calitem->year;
	month = calitem->month + *month_offset;
	e_calendar_item_normalize_date (calitem, &year, &month);

	tmp_tm.tm_year = year - 1900;
	tmp_tm.tm_mon = month;
	tmp_tm.tm_mday = *day;
	tmp_tm.tm_isdst = -1;
	mktime (&tmp_tm);

	/* Convert to 0 (Monday) to 6 (Sunday). */
	weekday = (tmp_tm.tm_wday + 6) % 7;

	/* Calculate how many days to the end of the row. */
	days = (calitem->week_start_day + 6 - weekday) % 7;

	*day += days;
	days_in_month = DAYS_IN_MONTH (year, month);
	if (*day > days_in_month) {
		(*month_offset)++;
		*day -= days_in_month;
	}
}


/* Rounds down the given day to the start of the week. */
static void
e_calendar_item_round_down_selection	(ECalendarItem	*calitem,
					 gint		*month_offset,
					 gint		*day)
{
	gint year, month, weekday, days, days_in_month;
	struct tm tmp_tm = { 0 };

	year = calitem->year;
	month = calitem->month + *month_offset;
	e_calendar_item_normalize_date (calitem, &year, &month);

	tmp_tm.tm_year = year - 1900;
	tmp_tm.tm_mon = month;
	tmp_tm.tm_mday = *day;
	tmp_tm.tm_isdst = -1;
	mktime (&tmp_tm);

	/* Convert to 0 (Monday) to 6 (Sunday). */
	weekday = (tmp_tm.tm_wday + 6) % 7;

	/* Calculate how many days to the start of the row. */
	days = (weekday + 7 - calitem->week_start_day) % 7;

	*day -= days;
	if (*day <= 0) {
		month--;
		if (month == -1) {
			year--;
			month = 11;
		}
		days_in_month = DAYS_IN_MONTH (year, month);
		(*month_offset)--;
		*day += days_in_month;
	}
}


static gint
e_calendar_item_get_inclusive_days	(ECalendarItem	*calitem,
					 gint		 start_month_offset,
					 gint		 start_day,
					 gint		 end_month_offset,
					 gint		 end_day)
{
	gint start_year, start_month, end_year, end_month, days = 0;

	start_year = calitem->year;
	start_month = calitem->month + start_month_offset;
	e_calendar_item_normalize_date (calitem, &start_year, &start_month);

	end_year = calitem->year;
	end_month = calitem->month + end_month_offset;
	e_calendar_item_normalize_date (calitem, &end_year, &end_month);

	while (start_year < end_year || start_month < end_month) {
		days += DAYS_IN_MONTH (start_year, start_month);
		start_month++;
		if (start_month == 12) {
			start_year++;
			start_month = 0;
		}
	}

	days += end_day - start_day + 1;

	return days;
}


/* If the day is off the end of the month it is set to the last day of the
   month. */
static void
e_calendar_item_ensure_valid_day	(ECalendarItem	*calitem,
					 gint		*month_offset,
					 gint		*day)
{
	gint year, month, days_in_month;

	year = calitem->year;
	month = calitem->month + *month_offset;
	e_calendar_item_normalize_date (calitem, &year, &month);

	days_in_month = DAYS_IN_MONTH (year, month);
	if (*day > days_in_month)
		*day = days_in_month;
}


gboolean
e_calendar_item_get_selection		(ECalendarItem	*calitem,
					 GDate		*start_date,
					 GDate		*end_date)
{
	gint start_year, start_month, start_day;
	gint end_year, end_month, end_day;

	g_date_clear (start_date, 1);
	g_date_clear (end_date, 1);

	if (!calitem->selection_set)
		return FALSE;

	start_year = calitem->year;
	start_month = calitem->month + calitem->selection_start_month_offset;
	e_calendar_item_normalize_date (calitem, &start_year, &start_month);
	start_day = calitem->selection_start_day;

	end_year = calitem->year;
	end_month = calitem->month + calitem->selection_end_month_offset;
	e_calendar_item_normalize_date (calitem, &end_year, &end_month);
	end_day = calitem->selection_end_day;

	g_date_set_dmy (start_date, start_day, start_month + 1, start_year);
	g_date_set_dmy (end_date, end_day, end_month + 1, end_year);

	return TRUE;
}


void
e_calendar_item_set_selection	(ECalendarItem	*calitem,
				 GDate		*start_date,
				 GDate		*end_date)
{
	gint start_year, start_month, start_day;
	gint end_year, end_month, end_day;
	gint new_start_month_offset, new_start_day;
	gint new_end_month_offset, new_end_day;
	gboolean need_update;

	g_return_if_fail (E_IS_CALENDAR_ITEM (calitem));

	/* If the user is in the middle of a selection, we must abort it. */
	if (calitem->selecting) {
		gnome_canvas_item_ungrab (GNOME_CANVAS_ITEM (calitem),
					  GDK_CURRENT_TIME);
		calitem->selecting = FALSE;
	}

	/* If start_date is NULL, we clear the selection without changing the
	   month shown. */
	if (start_date == NULL) {
		calitem->selection_set = FALSE;
		calitem->selection_changed = TRUE;
		e_calendar_item_queue_signal_emission (calitem);
		gnome_canvas_item_request_update (GNOME_CANVAS_ITEM (calitem));
		return;
	}

	if (end_date == NULL)
		end_date = start_date;
		
	g_return_if_fail (g_date_compare (start_date, end_date) <= 0);

	start_year = g_date_year (start_date);
	start_month = g_date_month (start_date) - 1;
	start_day = g_date_day (start_date);
	end_year = g_date_year (end_date);
	end_month = g_date_month (end_date) - 1;
	end_day = g_date_day (end_date);

	need_update = e_calendar_item_ensure_days_visible (calitem,
							   start_year,
							   start_month,
							   start_day,
							   end_year,
							   end_month,
							   end_day);

	new_start_month_offset = (start_year - calitem->year) * 12
		+ start_month - calitem->month;
	new_start_day = start_day;

	/* This may go outside the visible months, but we don't care. */
	new_end_month_offset = (end_year - calitem->year) * 12
		+ end_month - calitem->month;
	new_end_day = end_day;


	if (!calitem->selection_set
	    || calitem->selection_start_month_offset != new_start_month_offset
	    || calitem->selection_start_day != new_start_day
	    || calitem->selection_end_month_offset != new_end_month_offset
	    || calitem->selection_end_day != new_end_day) {
		need_update = TRUE;
		calitem->selection_changed = TRUE;
		e_calendar_item_queue_signal_emission (calitem);
		calitem->selection_set = TRUE;
		calitem->selection_start_month_offset = new_start_month_offset;
		calitem->selection_start_day = new_start_day;
		calitem->selection_end_month_offset = new_end_month_offset;
		calitem->selection_end_day = new_end_day;

		calitem->selection_real_start_month_offset = new_start_month_offset;
		calitem->selection_real_start_day = new_start_day;
		calitem->selection_from_full_week = FALSE;
	}

	if (need_update)
		gnome_canvas_item_request_update (GNOME_CANVAS_ITEM (calitem));
}


/* This tries to ensure that the given time range is visible. If the range
   given is longer than we can show, only the start of it will be visible.
   Note that this will not update the selection. That should be done somewhere
   else. It returns TRUE if the visible range has been changed. */
static gboolean
e_calendar_item_ensure_days_visible	(ECalendarItem	*calitem,
					 gint		 start_year,
					 gint		 start_month,
					 gint		 start_day,
					 gint		 end_year,
					 gint		 end_month,
					 gint		 end_day)
{
	gint current_end_year, current_end_month;
	gint months_shown, months;
	gint first_day_offset, days_in_month, days_in_prev_month;
	gboolean need_update = FALSE;

	months_shown = calitem->rows * calitem->cols;
	months = (end_year - start_year) * 12 + end_month - start_month;

	/* Calculate the range of months currently displayed. */
	current_end_year = calitem->year;
	current_end_month = calitem->month + months_shown - 1;
	e_calendar_item_normalize_date (calitem, &current_end_year,
					&current_end_month);

	/* Try to ensure that the end month is shown. */
	if (end_year > current_end_year
	    || (end_year == current_end_year
		&& end_month > current_end_month)) {
		need_update = TRUE;
		calitem->year = end_year;
		calitem->month = end_month - months_shown + 1;
		e_calendar_item_normalize_date (calitem, &calitem->year,
						&calitem->month);
	}

	/* Now try to ensure that the start month is shown. We do this after
	   the end month so that the start month will always be shown. */
	if (start_year < calitem->year
	    || (start_year == calitem->year
		&& start_month < calitem->month)) {
		need_update = TRUE;

		/* First we see if the start of the selection will fit in the
		   leftover days of the month before the first one shown. */
		calitem->year = start_year;
		calitem->month = start_month + 1;
		e_calendar_item_normalize_date (calitem, &calitem->year,
						&calitem->month);

		e_calendar_item_get_month_info (calitem, 0, 0,
						&first_day_offset,
						&days_in_month,
						&days_in_prev_month);

		if (start_day <= days_in_prev_month - first_day_offset) {
			calitem->year = start_year;
			calitem->month = start_month;
		}
	}

	if (need_update)
		e_calendar_item_date_range_changed (calitem);

	return need_update;
}


static void
e_calendar_item_show_popup_menu		(ECalendarItem	*calitem,
					 GdkEventButton	*event,
					 gint		 month_offset)
{
	GtkWidget *menu, *submenu, *menuitem;
	gint year, month;
	gchar buffer[64];
	struct tm tmp_tm;

	menu = gtk_menu_new ();

	for (year = calitem->year - 2; year <= calitem->year + 2; year++) {
		g_snprintf (buffer, 64, "%i", year);
		menuitem = gtk_menu_item_new_with_label (buffer);
		gtk_widget_show (menuitem);
		gtk_container_add (GTK_CONTAINER (menu), menuitem);

		submenu = gtk_menu_new ();
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);

		gtk_object_set_data (GTK_OBJECT (submenu), "year",
				     GINT_TO_POINTER (year));
		gtk_object_set_data (GTK_OBJECT (submenu), "month_offset",
				     GINT_TO_POINTER (month_offset));

		for (month = 0; month < 12; month++) {
			memset (&tmp_tm, 0, sizeof (tmp_tm));
			tmp_tm.tm_year = year - 1900;
			tmp_tm.tm_mon = month;
			tmp_tm.tm_mday = 1;
			tmp_tm.tm_isdst = -1;
			mktime (&tmp_tm);
			/* This is a strftime() format. %B = Month name, %Y = Year. */
			strftime (buffer, sizeof (buffer), _("%B %Y"), &tmp_tm);

			menuitem = gtk_menu_item_new_with_label (buffer);
			gtk_widget_show (menuitem);
			gtk_container_add (GTK_CONTAINER (submenu), menuitem);

			gtk_object_set_data (GTK_OBJECT (menuitem), "month",
					     GINT_TO_POINTER (month));

			gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
					    GTK_SIGNAL_FUNC (e_calendar_item_on_menu_item_activate), calitem);
		}
	}

	/* Run the menu modal so we can destroy it after. */
	gtk_signal_connect (GTK_OBJECT (menu), "deactivate",
			    GTK_SIGNAL_FUNC (gtk_main_quit), NULL);
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL,
			e_calendar_item_position_menu, calitem,
			event->button, event->time);
	gtk_grab_add (menu);
	gtk_main ();
	gtk_grab_remove (menu);
	gtk_widget_destroy (menu);
}


static void
e_calendar_item_on_menu_item_activate	(GtkWidget	*menuitem,
					 ECalendarItem	*calitem)
{
	gint year, month_offset, month;

	year = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menuitem->parent), "year"));
	month_offset = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menuitem->parent), "month_offset"));
	month = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menuitem), "month"));

	month -= month_offset;
	e_calendar_item_normalize_date (calitem, &year, &month);
	e_calendar_item_set_first_month (calitem, year, month);
}


static void
e_calendar_item_position_menu	(GtkMenu            *menu,
				 gint               *x,
				 gint               *y,
				 gpointer            user_data)
{
	GtkRequisition requisition;
	gint screen_width, screen_height;

	gtk_widget_get_child_requisition (GTK_WIDGET (menu), &requisition);

	*x -= 2;
	*y -= requisition.height / 2;

	screen_width = gdk_screen_width ();
	screen_height = gdk_screen_height ();

	*x = CLAMP (*x, 0, screen_width - requisition.width);
	*y = CLAMP (*y, 0, screen_height - requisition.height);
}


/* Sets the function to call to get the colors to use for a particular day. */
void
e_calendar_item_set_style_callback	(ECalendarItem	*calitem,
					 ECalendarItemStyleCallback cb,
					 gpointer	 data,
					 GtkDestroyNotify destroy)
{
	g_return_if_fail (E_IS_CALENDAR_ITEM (calitem));

	if (calitem->style_callback_data)
		(*calitem->style_callback_destroy) (calitem->style_callback_data);

	calitem->style_callback = cb;
	calitem->style_callback_data = data;
	calitem->style_callback_destroy = destroy;
}


static void
e_calendar_item_date_range_changed	(ECalendarItem	*calitem)
{
	g_free (calitem->styles);
	calitem->styles = NULL;
	calitem->date_range_changed = TRUE;
	e_calendar_item_queue_signal_emission (calitem);
}


static void
e_calendar_item_queue_signal_emission	(ECalendarItem	*calitem)
{
	if (calitem->signal_emission_idle_id == 0) {
		calitem->signal_emission_idle_id = g_idle_add_full (G_PRIORITY_HIGH, e_calendar_item_signal_emission_idle_cb, calitem, NULL);
	}
}


static gboolean
e_calendar_item_signal_emission_idle_cb	(gpointer data)
{
	ECalendarItem *calitem;

	g_return_val_if_fail (E_IS_CALENDAR_ITEM (data), FALSE);

	GDK_THREADS_ENTER ();

	calitem = E_CALENDAR_ITEM (data);

	calitem->signal_emission_idle_id = 0;

	if (calitem->date_range_changed) {
		gtk_signal_emit (GTK_OBJECT (calitem),
				 e_calendar_item_signals[DATE_RANGE_CHANGED]);
		calitem->date_range_changed = FALSE;
	}

	if (calitem->selection_changed) {
		gtk_signal_emit (GTK_OBJECT (calitem),
				 e_calendar_item_signals[SELECTION_CHANGED]);
		calitem->selection_changed = FALSE;
	}

	GDK_THREADS_LEAVE ();
	return FALSE;
}

