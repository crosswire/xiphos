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
#include <glib.h>
#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>
#include <e-util/e-util.h>
#include "e-calendar-item.h"


/* The minimum padding around the numbers in each cell/day. */
#define	E_CALENDAR_ITEM_MIN_CELL_XPAD	4
#define	E_CALENDAR_ITEM_MIN_CELL_YPAD	0


/* These are the padding sizes between various pieces of the calendar. */
/* FIXME: Use decent names eventually. */
#define	E_CALENDAR_ITEM_XPAD1	4
#define	E_CALENDAR_ITEM_XPAD2	2
#define	E_CALENDAR_ITEM_XPAD3	2
#define	E_CALENDAR_ITEM_XPAD4	4

#define	E_CALENDAR_ITEM_YPAD3	1
#define	E_CALENDAR_ITEM_YPAD4	0
#define	E_CALENDAR_ITEM_YPAD5	1
#define	E_CALENDAR_ITEM_YPAD6	2

#define	E_CALENDAR_ITEM_XPAD11	16
#define	E_CALENDAR_ITEM_XPAD12	2
#define	E_CALENDAR_ITEM_XPAD13	1
#define	E_CALENDAR_ITEM_XPAD14	1
#define	E_CALENDAR_ITEM_XPAD15	2
#define	E_CALENDAR_ITEM_XPAD16	16

/* The number of rows & columns of days in each month. */
#define E_CALENDAR_ROWS_PER_MONTH	6
#define E_CALENDAR_COLS_PER_MONTH	7


static void e_calendar_item_class_init	(ECalendarItemClass *class);
static void e_calendar_item_init	(ECalendarItem	 *calitem);

static void e_calendar_item_get_arg	(GtkObject	 *o,
					 GtkArg		 *arg,
					 guint		  arg_id);
static void e_calendar_item_set_arg	(GtkObject	 *o,
					 GtkArg		 *arg,
					 guint		  arg_id);
static void e_calendar_item_realize	(GnomeCanvasItem *item);
static void e_calendar_item_unrealize	(GnomeCanvasItem *item);
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
							 gint		*month,
							 gint		*day,
							 gboolean	*entire_week);
static void e_calendar_item_get_month_info	(ECalendarItem	*calitem,
						 gint		 row,
						 gint		 col,
						 gint		*first_valid_day,
						 gint		*last_valid_day);
static void e_calendar_item_recalc_sizes(ECalendarItem *calitem);

static gint e_calendar_item_get_week_number	(ECalendarItem *calitem,
						 gint		day,
						 gint		month,
						 gint		year);

static void e_calendar_item_get_day_style	(ECalendarItem	*calitem,
						 gint		 year,
						 gint		 month,
						 gint		 day,
						 gboolean	 today,
						 gboolean	 current_month,
						 gboolean	 selected,
						 GdkColor      **bg_color,
						 GdkColor      **fg_color,
						 GdkColor      **box_color,
						 gboolean	*bold);

static GnomeCanvasItemClass *parent_class;

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
	ARG_SHOW_WEEK_NUMBERS
};


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
				 GTK_TYPE_DOUBLE, GTK_ARG_READABLE,
				 ARG_ROW_HEIGHT);
	gtk_object_add_arg_type ("ECalendarItem::column_width",
				 GTK_TYPE_DOUBLE, GTK_ARG_READABLE,
				 ARG_COLUMN_WIDTH);
	gtk_object_add_arg_type ("ECalendarItem::mininum_rows",
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

	object_class->get_arg = e_calendar_item_get_arg;
	object_class->set_arg = e_calendar_item_set_arg;

	/* GnomeCanvasItem method overrides */
	item_class->realize     = e_calendar_item_realize;
	item_class->unrealize   = e_calendar_item_unrealize;
	item_class->update      = e_calendar_item_update;
	item_class->draw        = e_calendar_item_draw;
	item_class->point       = e_calendar_item_point;
	item_class->event       = e_calendar_item_event;
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

	calitem->min_cols = 1;
	calitem->min_rows = 1;
	calitem->show_week_numbers = FALSE;
	calitem->week_start_day = 0;
	calitem->expand = TRUE;

	calitem->x1 = 0.0;
	calitem->y1 = 0.0;
	calitem->x2 = 0.0;
	calitem->y2 = 0.0;

	calitem->selection_start_month_offset = -1;

	/* Translators: These are the first characters of each day of the
	   week, 'M' for 'Monday', 'T' for Tuesday etc. */
	calitem->days = _("MTWTFSS");
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
		GTK_VALUE_DOUBLE (*arg) = calitem->min_month_height;
		break;
	case ARG_COLUMN_WIDTH:
		e_calendar_item_recalc_sizes (calitem);
		GTK_VALUE_DOUBLE (*arg) = calitem->min_month_width;
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
	}
}


static void
e_calendar_item_set_arg (GtkObject *o, GtkArg *arg, guint arg_id)
{
	GnomeCanvasItem *item;
	ECalendarItem *calitem;
	GdkFont *font;
	gboolean need_reshape = FALSE;
	gdouble dvalue;
	gint ivalue;
	gboolean bvalue;

	item = GNOME_CANVAS_ITEM (o);
	calitem = E_CALENDAR_ITEM (o);
	
	switch (arg_id){
	case ARG_YEAR:
		ivalue = GTK_VALUE_INT (*arg);
		if (calitem->year != ivalue) {
			calitem->year = ivalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_MONTH:
		ivalue = GTK_VALUE_INT (*arg);
		if (calitem->month != ivalue) {
			calitem->month = ivalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_X1:
		dvalue = GTK_VALUE_DOUBLE (*arg);
		if (calitem->x1 != dvalue) {
			calitem->x1 = dvalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_Y1:
		dvalue = GTK_VALUE_DOUBLE (*arg);
		if (calitem->y1 != dvalue) {
			calitem->y1 = dvalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_X2:
		dvalue = GTK_VALUE_DOUBLE (*arg);
		if (calitem->x2 != dvalue) {
			calitem->x2 = dvalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_Y2:
		dvalue = GTK_VALUE_DOUBLE (*arg);
		if (calitem->y2 != dvalue) {
			calitem->y2 = dvalue;
			need_reshape = TRUE;
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
			need_reshape = TRUE;
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
			need_reshape = TRUE;
		}
		break;
	case ARG_MINIMUM_ROWS:
		ivalue = GTK_VALUE_INT (*arg);
		ivalue = MAX (1, ivalue);
		if (calitem->min_rows != ivalue) {
			calitem->min_rows = ivalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_MINIMUM_COLUMNS:
		ivalue = GTK_VALUE_INT (*arg);
		ivalue = MAX (1, ivalue);
		if (calitem->min_cols != ivalue) {
			calitem->min_cols = ivalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_MAXIMUM_ROWS:
		ivalue = GTK_VALUE_INT (*arg);
		if (calitem->max_rows != ivalue) {
			calitem->max_rows = ivalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_MAXIMUM_COLUMNS:
		ivalue = GTK_VALUE_INT (*arg);
		if (calitem->max_cols != ivalue) {
			calitem->max_cols = ivalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_WEEK_START_DAY:
		ivalue = GTK_VALUE_INT (*arg);
		if (calitem->week_start_day != ivalue) {
			calitem->week_start_day = ivalue;
			need_reshape = TRUE;
		}
		break;
	case ARG_SHOW_WEEK_NUMBERS:
		bvalue = GTK_VALUE_BOOL (*arg);
		if (calitem->show_week_numbers != ivalue) {
			calitem->show_week_numbers = bvalue;
			need_reshape = TRUE;
		}
		break;
	default:
		g_warning ("Invalid arg");
	}

	/* FIXME: finish. */
	if (need_reshape) {
		gnome_canvas_item_request_update (item);
	}
}


static void
e_calendar_item_realize		(GnomeCanvasItem *item)
{
	ECalendarItem *calitem;
	GdkColormap *colormap;
	gboolean success[E_CALENDAR_COLOR_LAST];
	gint nfailed;

	calitem = E_CALENDAR_ITEM (item);

	colormap = gtk_widget_get_colormap (GTK_WIDGET (item->canvas));

	calitem->colors[E_CALENDAR_COLOR_SELECTION].red   = 65535;
	calitem->colors[E_CALENDAR_COLOR_SELECTION].green = 65535;
	calitem->colors[E_CALENDAR_COLOR_SELECTION].blue  = 65535;

	calitem->colors[E_CALENDAR_COLOR_HIGHLIGHT].red   = 56000;
	calitem->colors[E_CALENDAR_COLOR_HIGHLIGHT].green = 57000;
	calitem->colors[E_CALENDAR_COLOR_HIGHLIGHT].blue  = 57000;

	calitem->colors[E_CALENDAR_COLOR_TODAY].red   = 65535;
	calitem->colors[E_CALENDAR_COLOR_TODAY].green = 0;
	calitem->colors[E_CALENDAR_COLOR_TODAY].blue  = 0;

	nfailed = gdk_colormap_alloc_colors (colormap, calitem->colors,
					     E_CALENDAR_COLOR_LAST, FALSE,
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

	for (i = 0; i < E_CALENDAR_COLOR_LAST; i++)
		gdk_colors_free (colormap, &calitem->colors[i].pixel, 1, 0);
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
	gint xthickness, ythickness;

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

	calitem->rows = height / calitem->min_month_height;
	calitem->rows = MAX (calitem->rows, calitem->min_rows);
	if (calitem->max_rows > 0)
		calitem->rows = MIN (calitem->rows, calitem->max_rows);
	calitem->cols = width / calitem->min_month_width;
	calitem->cols = MAX (calitem->cols, calitem->min_cols);
	if (calitem->max_cols > 0)
		calitem->cols = MIN (calitem->cols, calitem->max_cols);

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
	gint char_height, row, col, row_y, bar_height, col_x, ythickness;

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
	ythickness = style->klass->ythickness;
	base_gc = style->base_gc[GTK_STATE_NORMAL];
	bg_gc = style->bg_gc[GTK_STATE_NORMAL];

	/* Clear the entire background. */
	gdk_draw_rectangle (drawable, base_gc, TRUE,
			    calitem->x1 - x, calitem->y1 - y,
			    calitem->x2 - calitem->x1,
			    calitem->y2 - calitem->y1);

	/* Draw the shadow around the entire item.
	   FIXME: must also leave room for the 'Today' & 'None' buttons etc. */
	gtk_draw_shadow (style, drawable,
			 GTK_STATE_NORMAL, GTK_SHADOW_OUT,
			 calitem->x1 - x, calitem->y1 - y,
			 calitem->x2 - calitem->x1, calitem->y2 - calitem->y1);

	row_y = canvas_item->y1 + ythickness;
	bar_height = ythickness * 2 + E_CALENDAR_ITEM_YPAD1 + char_height
		+ E_CALENDAR_ITEM_YPAD2;

	for (row = 0; row < calitem->rows; row++) {
		/* Draw the background for the title bars and the shadow around
		   it, and the vertical lines between columns. */

		gdk_draw_rectangle (drawable, bg_gc, TRUE,
				    calitem->x1 - x, row_y - y,
				    calitem->x2 - calitem->x1, bar_height);

		gtk_draw_shadow (style, drawable,
				 GTK_STATE_NORMAL, GTK_SHADOW_OUT,
				 calitem->x1 - x, row_y - y,
				 calitem->x2 - calitem->x1, bar_height);


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
	GdkGC *fg_gc, *bg_gc;
	struct tm tmp_tm;
	GdkRectangle clip_rect;
	gint char_height, xthickness, ythickness, start_weekday;
	gint year, month, month_x, month_y, min_x, max_x, text_x, text_y;
	gint day, day_index, cells_x, cells_y, min_cell_width, text_width;
	gchar buffer[64];

#if 0
	g_print ("In e_calendar_item_draw_month: %i,%i %ix%i\n",
		 x, y, width, height);
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
	bg_gc = style->bg_gc[GTK_STATE_NORMAL];

	/* Calculate the top-left position of the entire month display. */
	month_x = item->x1 + xthickness + calitem->x_offset
		+ col * calitem->month_width - x;
	month_y = item->y1 + ythickness + row * calitem->month_height - y;

	/* Just return if the month is outside the given area. */
	if (month_x >= width || month_x + calitem->month_width <= 0
	    || month_y >= height || month_y + calitem->month_height <= 0)
		return;


	/* Draw the month name & year, with clipping. Note that the top row
	   needs extra space around it for the buttons. */
	if (row == 0 && col == 0)
		min_x = E_CALENDAR_ITEM_XPAD11;
	else
		min_x = E_CALENDAR_ITEM_XPAD14 + E_CALENDAR_ITEM_XPAD15;

	if (row == 0 && col == calitem->cols - 1)
		max_x = calitem->month_width - E_CALENDAR_ITEM_XPAD16;
	else
		max_x = calitem->month_width - E_CALENDAR_ITEM_XPAD12
			- E_CALENDAR_ITEM_XPAD13;

	text_y = month_y + style->klass->ythickness
		+ E_CALENDAR_ITEM_YPAD1;
	clip_rect.x = month_x + min_x;
	clip_rect.y = text_y;
	clip_rect.width = max_x - min_x;
	clip_rect.height = char_height;
	gdk_gc_set_clip_rectangle (fg_gc, &clip_rect);

	memset (&tmp_tm, 0, sizeof (tmp_tm));
	month = calitem->month + row * calitem->cols + col;
	year = calitem->year + month / 12;
	month %= 12;
	tmp_tm.tm_year = year - 1900;
	tmp_tm.tm_mon = month;
	tmp_tm.tm_mday = 1;
	tmp_tm.tm_isdst = -1;
	mktime (&tmp_tm);
	strftime (buffer, 64, "%B %Y", &tmp_tm);
	start_weekday = (tmp_tm.tm_wday + 6) % 7;

	/* Ideally we place the text centered in the month, but we won't go
	   to the left of the minimum x position. */
	text_width = gdk_string_width (font, buffer);
	text_x = (calitem->month_width - text_width) / 2;
	text_x = MAX (min_x, text_x);

	gdk_draw_string (drawable, font, fg_gc,
			 month_x + text_x, text_y + font->ascent, buffer);

	gdk_gc_set_clip_rectangle (fg_gc, NULL);


	/* Draw the day initials across the top of the month. */
	min_cell_width = calitem->max_digit_width * 2
		+ E_CALENDAR_ITEM_MIN_CELL_XPAD;

	cells_x = month_x + E_CALENDAR_ITEM_XPAD1 + calitem->month_lpad
		+ E_CALENDAR_ITEM_XPAD3;
	if (calitem->show_week_numbers)
		cells_x += calitem->max_week_number_digit_width * 2
			+ E_CALENDAR_ITEM_XPAD2 + 1;
	text_x = cells_x + calitem->cell_width
		- (calitem->cell_width - min_cell_width) / 2;
	text_x -= E_CALENDAR_ITEM_MIN_CELL_XPAD / 2;
	text_y = month_y + ythickness * 2 + E_CALENDAR_ITEM_YPAD1
		+ char_height + E_CALENDAR_ITEM_YPAD2 + E_CALENDAR_ITEM_YPAD3
		+  calitem->month_tpad;

	cells_y = text_y + char_height + E_CALENDAR_ITEM_YPAD4 + 1
		+ E_CALENDAR_ITEM_YPAD5;

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
		       cells_x - E_CALENDAR_ITEM_XPAD3,
		       cells_y - E_CALENDAR_ITEM_YPAD5,
		       cells_x + E_CALENDAR_COLS_PER_MONTH * calitem->cell_width - 1,
		       cells_y - E_CALENDAR_ITEM_YPAD5);

	e_calendar_item_draw_day_numbers (calitem, drawable, width, height,
					  row, col, year, month, start_weekday,
					  cells_x, cells_y);

	/* Draw the vertical line after the week number. */
	if (calitem->show_week_numbers) {
		gdk_draw_line (drawable, fg_gc,
			       cells_x - E_CALENDAR_ITEM_XPAD3,
			       cells_y,
			       cells_x - E_CALENDAR_ITEM_XPAD3,
			       cells_y + E_CALENDAR_ROWS_PER_MONTH * calitem->cell_height - 1);
	}
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
	gboolean bold, today, draw_day, finished = FALSE, selected;
	gint today_year, today_month, today_mday, month_offset, day_offset;
	gchar buffer[2];

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
	   months. Note that g_date uses 1 to 12 for months. */
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

	days_in_month[0] = g_date_days_in_month (months[0] + 1, years[0]);
	days_in_month[1] = g_date_days_in_month (months[1] + 1, years[1]);
	days_in_month[2] = g_date_days_in_month (months[2] + 1, years[2]);

	/* Mon 0 is the previous month, which we may show the end of. Mon 1 is
	   the current month, and mon 2 is the next month. */
	mon = 0;

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

	month_offset = row * calitem->cols + col;
	day_offset = 0;

	for (drow = 0; drow < 6; drow++) {
		/* Draw the week number. */
		if (calitem->show_week_numbers) {
			week_num = e_calendar_item_get_week_number (calitem,
								    day_num,
								    months[mon],
								    years[mon]);

			text_x = cells_x - E_CALENDAR_ITEM_XPAD3 - 1
				- E_CALENDAR_ITEM_XPAD2;
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

				selected = calitem->selection_start_month_offset != -1
					&& (calitem->selection_start_month_offset < month_offset
					    || (calitem->selection_start_month_offset == month_offset
						&& calitem->selection_start_day_offset <= day_offset))
					&& (calitem->selection_end_month_offset > month_offset
					    || (calitem->selection_end_month_offset == month_offset
						&& calitem->selection_end_day_offset >= day_offset));

				/* Get the colors & style to use for the day.*/
				e_calendar_item_get_day_style (calitem,
							       years[mon],
							       months[mon],
							       day_num,
							       today,
							       mon == 1,
							       selected,
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

			day_offset++;
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
   layout of the item. */
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

	/* If both fonts are the same, just return. */
	if (font == calitem->old_font
	    && wkfont == calitem->old_week_number_font)
		return;

	calitem->old_font = font;
	calitem->old_week_number_font = wkfont;

	for (day = 0; day < 7; day++)
		calitem->day_widths[day] = gdk_char_width (font,
							   calitem->days[day]);

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


	min_cell_width = max_digit_width * 2 + E_CALENDAR_ITEM_MIN_CELL_XPAD;
	min_cell_height = char_height + E_CALENDAR_ITEM_MIN_CELL_YPAD;

	calitem->min_month_width = E_CALENDAR_ITEM_XPAD1
		+ E_CALENDAR_ITEM_XPAD3 + min_cell_width * 7
		+ E_CALENDAR_ITEM_XPAD4;
	if (calitem->show_week_numbers)
		calitem->min_month_width += max_week_number_digit_width * 2
			+ E_CALENDAR_ITEM_XPAD2 + 1;

	calitem->min_month_height = style->klass->ythickness * 2
		+ E_CALENDAR_ITEM_YPAD1 + char_height
		+ E_CALENDAR_ITEM_YPAD2 + 1 + E_CALENDAR_ITEM_YPAD3
		+ char_height + E_CALENDAR_ITEM_YPAD4 + 1
		+ E_CALENDAR_ITEM_YPAD5 + min_cell_height * 6
		+ E_CALENDAR_ITEM_YPAD6;
}


static void
e_calendar_item_get_day_style		(ECalendarItem	*calitem,
					 gint		 year,
					 gint		 month,
					 gint		 day,
					 gboolean	 today,
					 gboolean	 current_month,
					 gboolean	 selected,
					 GdkColor      **bg_color,
					 GdkColor      **fg_color,
					 GdkColor      **box_color,
					 gboolean	*bold)
{
	*bg_color = NULL;
	*fg_color = NULL;
	*box_color = NULL;
	*bold = FALSE;

	if (today)
		*box_color = &calitem->colors[E_CALENDAR_COLOR_TODAY];

	if (!current_month)
		*fg_color = &calitem->colors[E_CALENDAR_COLOR_HIGHLIGHT];

	if (selected) {
		*fg_color = &calitem->colors[E_CALENDAR_COLOR_SELECTION];
		*bg_color = &calitem->colors[E_CALENDAR_COLOR_HIGHLIGHT];
	}
}



static gboolean
e_calendar_item_button_press	(ECalendarItem	*calitem,
				 GdkEvent	*event)
{
	gint month, day;
	gboolean all_week;

	g_print ("In e_calendar_item_button_press\n");

	if (e_calendar_item_convert_position_to_day (calitem,
						     event->button.x,
						     event->button.y,
						     &month, &day, &all_week)) {
		g_print ("  Pressed month: %i day: %i\n", month, day);

		calitem->selection_start_month_offset = month;
		calitem->selection_start_day_offset = day;
		calitem->selection_end_month_offset = month;
		calitem->selection_end_day_offset = day;
		calitem->selecting = TRUE;
		calitem->selection_dragging_end = TRUE;

		gnome_canvas_item_request_update (GNOME_CANVAS_ITEM (calitem));

		return TRUE;
	}

	return FALSE;
}


static gboolean
e_calendar_item_button_release	(ECalendarItem	*calitem,
				 GdkEvent	*event)
{
	gint month, day;
	gboolean all_week;

	g_print ("In e_calendar_item_button_release\n");

	calitem->selecting = FALSE;

	if (e_calendar_item_convert_position_to_day (calitem,
						     event->button.x,
						     event->button.y,
						     &month, &day, &all_week))
		g_print ("  Released month: %i day: %i\n", month, day);

	return FALSE;
}


static gboolean
e_calendar_item_motion		(ECalendarItem	*calitem,
				 GdkEvent	*event)
{
	gint month, day;
	gboolean all_week;

	if (!calitem->selecting)
		return FALSE;

	if (e_calendar_item_convert_position_to_day (calitem,
						     event->button.x,
						     event->button.y,
						     &month, &day, &all_week)) {
		if (calitem->selection_dragging_end) {
			if (calitem->selection_end_month_offset == month
			    && calitem->selection_end_day_offset == day)
				return FALSE;
			calitem->selection_end_month_offset = month;
			calitem->selection_end_day_offset = day;
		} else {
			if (calitem->selection_start_month_offset == month
			    && calitem->selection_start_day_offset == day)
				return FALSE;
			calitem->selection_start_month_offset = month;
			calitem->selection_start_day_offset = day;
		}

		if (calitem->selection_start_month_offset > calitem->selection_end_month_offset
		    || (calitem->selection_start_month_offset == calitem->selection_end_month_offset
			&& calitem->selection_start_day_offset > calitem->selection_end_day_offset)) {
			month = calitem->selection_start_month_offset;
			day = calitem->selection_start_day_offset;
			calitem->selection_start_month_offset = calitem->selection_end_month_offset;
			calitem->selection_end_month_offset = month;
			calitem->selection_start_day_offset = calitem->selection_end_day_offset;
			calitem->selection_end_day_offset = day;

			calitem->selection_dragging_end = !calitem->selection_dragging_end;
		}

		gnome_canvas_item_request_update (GNOME_CANVAS_ITEM (calitem));
	}

	return FALSE;
}




/* Converts a position within the item to a month & day.
   The month returned is 0 for the top-left month displayed.
   If the position is over a week number the first day of the week is returned
   and entire_week is set to TRUE. */
static gboolean
e_calendar_item_convert_position_to_day	(ECalendarItem	*calitem,
					 gint		 event_x,
					 gint		 event_y,
					 gint		*month,
					 gint		*day,
					 gboolean	*entire_week)
{
	GnomeCanvasItem *item;
	GtkWidget *widget;
	GtkStyle *style;
	gint xthickness, ythickness, char_height;
	gint x, y, row, col, cells_x, cells_y, day_row, day_col;
	gint first_valid_day, last_valid_day;

	item = GNOME_CANVAS_ITEM (calitem);
	widget = GTK_WIDGET (item->canvas);
	style = widget->style;
	char_height = style->font->ascent + style->font->descent;
	xthickness = style->klass->xthickness;
	ythickness = style->klass->ythickness;

	x = event_x - xthickness - calitem->x_offset;
	y = event_y - ythickness;

	if (x < 0 || y < 0)
		return FALSE;

	row = y / calitem->month_height;
	col = x / calitem->month_width;

	if (row < 0 || row >= calitem->rows
	    || col < 0 || col >= calitem->cols)
		return FALSE;

	x = x % calitem->month_width;
	y = y % calitem->month_height;

	cells_x = E_CALENDAR_ITEM_XPAD1 + calitem->month_lpad
		+ E_CALENDAR_ITEM_XPAD3;
	if (calitem->show_week_numbers)
		cells_x += calitem->max_week_number_digit_width * 2
			+ E_CALENDAR_ITEM_XPAD2 + 1;
	cells_y = ythickness * 2 + E_CALENDAR_ITEM_YPAD1
		+ char_height + E_CALENDAR_ITEM_YPAD2 + E_CALENDAR_ITEM_YPAD3
		+  calitem->month_tpad
		+ char_height + E_CALENDAR_ITEM_YPAD4 + 1
		+ E_CALENDAR_ITEM_YPAD5;

	x -= cells_x;
	y -= cells_y;

	if (x < 0 || y < 0)
		return FALSE;

	day_row = y / calitem->cell_height;
	day_col = x / calitem->cell_width;

	if (day_row < 0 || day_row >= E_CALENDAR_ROWS_PER_MONTH
	    || day_col < 0 || day_col >= E_CALENDAR_COLS_PER_MONTH)
		return FALSE;

	*month = row * calitem->cols + col;
	*day = day_row * E_CALENDAR_COLS_PER_MONTH + day_col;
	*entire_week = FALSE;

	e_calendar_item_get_month_info (calitem, row, col,
					&first_valid_day, &last_valid_day);
	if (*day < first_valid_day || *day > last_valid_day)
		return FALSE;

	return TRUE;
}


static void
e_calendar_item_get_month_info	(ECalendarItem	*calitem,
				 gint		 row,
				 gint		 col,
				 gint		*first_valid_day,
				 gint		*last_valid_day)
{
	gint year, month, days_in_month, start_weekday, first_day_of_month;
	struct tm tmp_tm = { 0 };

	month = calitem->month + row * calitem->cols + col;
	year = calitem->year + month / 12;
	month = month % 12;
	days_in_month = g_date_days_in_month (month + 1, year);

	tmp_tm.tm_year = year - 1900;
	tmp_tm.tm_mon = month;
	tmp_tm.tm_mday = 1;
	tmp_tm.tm_isdst = -1;
	mktime (&tmp_tm);

	/* Convert to 0 (Monday) to 6 (Sunday). */
	start_weekday = (tmp_tm.tm_wday + 6) % 7;

	first_day_of_month = (start_weekday + 7 - calitem->week_start_day) % 7;

	if (row == 0 && col == 0)
		*first_valid_day = 0;
	else
		*first_valid_day = first_day_of_month;

	if (row == calitem->rows - 1 && col == calitem->cols - 1)
		*last_valid_day = E_CALENDAR_ROWS_PER_MONTH * E_CALENDAR_COLS_PER_MONTH - 1;
	else
		*last_valid_day = first_day_of_month + days_in_month - 1;
}






















#if 0

static gint
e_calendar_button_press		(GtkWidget	*widget,
				 GdkEventButton	*event)
{
	ECalendar *cal;
	gint day;

	cal = E_CALENDAR (widget);

	g_print ("In e_calendar_button_press\n");

	day = e_calendar_convert_position_to_day (cal, event->x, event->y);

	cal->selection_start_day = day;
	cal->selection_end_day = day;
	cal->selection_dragging_end = TRUE;

	gtk_widget_queue_draw (widget);

	return FALSE;
}


static gint
e_calendar_button_release	(GtkWidget	*widget,
				 GdkEventButton	*event)
{
	g_print ("In e_calendar_button_release\n");

	return FALSE;
}


static gint
e_calendar_motion		(GtkWidget	*widget, 
				 GdkEventMotion	*event)
{
	ECalendar *cal;
	gint x, y, day;

	cal = E_CALENDAR (widget);

	g_print ("In e_calendar_motion\n");

	x = event->x;
	y = event->y;

	if (event->is_hint || event->window != widget->window)
		gtk_widget_get_pointer (widget, &x, &y);

	day = e_calendar_convert_position_to_day (cal, event->x, event->y);

	if (cal->selection_dragging_end)
		cal->selection_end_day = day;
	else
		cal->selection_start_day = day;

	if (cal->selection_start_day > cal->selection_end_day) {
		day = cal->selection_start_day;
		cal->selection_start_day = cal->selection_end_day;
		cal->selection_end_day = day;
		cal->selection_dragging_end = !cal->selection_dragging_end;
	}

	gtk_widget_queue_draw (widget);

	return FALSE;
}
#endif
