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
 * ECalendar - displays a table of monthly calendars, allowing highlighting
 * and selection of one or more days. Like GtkCalendar with more features.
 * Most of the functionality is in the ECalendarItem canvas item, though
 * we also add GnomeCanvasWidget buttons to go to the previous/next month and
 * to got to the current day.
 */

#include <config.h>
#include <e-util/e-util.h>
#include "e-calendar.h"

#define E_CALENDAR_SMALL_FONT	\
	"-adobe-utopia-regular-r-normal-*-*-100-*-*-p-*-iso8859-*"
#define E_CALENDAR_SMALL_FONT_FALLBACK	\
	"-adobe-helvetica-medium-r-normal-*-*-80-*-*-p-*-iso8859-*"

#define E_CALENDAR_BUTTON_X_PAD	2
#define E_CALENDAR_BUTTON_Y_PAD	0

static void e_calendar_class_init	(ECalendarClass *class);
static void e_calendar_init		(ECalendar	*cal);
static void e_calendar_destroy		(GtkObject	*object);
static void e_calendar_realize		(GtkWidget	*widget);
static void e_calendar_style_set	(GtkWidget	*widget,
					 GtkStyle	*previous_style);
static void e_calendar_size_request	(GtkWidget      *widget,
					 GtkRequisition *requisition);
static void e_calendar_size_allocate	(GtkWidget	*widget,
					 GtkAllocation	*allocation);
static gint e_calendar_expose		(GtkWidget      *widget,
					 GdkEventExpose *event);
static void e_calendar_draw		(GtkWidget	*widget,
					 GdkRectangle	*area);
static gint e_calendar_button_press	(GtkWidget	*widget,
					 GdkEventButton	*event);
static gint e_calendar_button_release	(GtkWidget	*widget,
					 GdkEventButton	*event);
static gint e_calendar_focus_in		(GtkWidget	*widget,
					 GdkEventFocus	*event);
static gint e_calendar_focus_out	(GtkWidget	*widget,
					 GdkEventFocus	*event);
static gint e_calendar_key_press	(GtkWidget	*widget,
					 GdkEventKey	*event);

static void e_calendar_paint		(ECalendar	*cal,
					 GdkRectangle	*area);

static void e_calendar_on_prev_clicked	(ECalendar	*cal);
static void e_calendar_on_next_clicked	(ECalendar	*cal);

static GnomeCanvasClass *parent_class;
static GtkLayoutClass *grandparent_class;

E_MAKE_TYPE (e_calendar, "ECalendar", ECalendar,
	     e_calendar_class_init, e_calendar_init, E_CANVAS_TYPE)


static void
e_calendar_class_init (ECalendarClass *class)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;

	object_class = (GtkObjectClass *) class;
	widget_class = (GtkWidgetClass *) class;

	parent_class = gtk_type_class (E_CANVAS_TYPE);
	grandparent_class = gtk_type_class (GTK_TYPE_LAYOUT);

	object_class->destroy = e_calendar_destroy;

	widget_class->realize		   = e_calendar_realize;
	widget_class->style_set		   = e_calendar_style_set;
 	widget_class->size_request	   = e_calendar_size_request;
 	widget_class->size_allocate	   = e_calendar_size_allocate;
	widget_class->expose_event	   = e_calendar_expose;
	widget_class->draw		   = e_calendar_draw;
#if 0
	widget_class->button_press_event   = e_calendar_button_press;
	widget_class->button_release_event = e_calendar_button_release;
#endif
	widget_class->focus_in_event	   = e_calendar_focus_in;
	widget_class->focus_out_event	   = e_calendar_focus_out;
	widget_class->key_press_event	   = e_calendar_key_press;
}


static void
e_calendar_init (ECalendar *cal)
{
	GnomeCanvasGroup *canvas_group;
	GdkFont *small_font;
	GtkWidget *button, *arrow;

	/* Create the small font. */
	small_font = gdk_font_load (E_CALENDAR_SMALL_FONT);
	if (!small_font)
		small_font = gdk_font_load (E_CALENDAR_SMALL_FONT_FALLBACK);
	if (!small_font)
		g_warning ("Couldn't load font");

	canvas_group = GNOME_CANVAS_GROUP (GNOME_CANVAS (cal)->root);

	cal->calitem = E_CALENDAR_ITEM (gnome_canvas_item_new (canvas_group,
							       e_calendar_item_get_type (),
							       "week_number_font", small_font,
							       "week_start_day", 6,
							       NULL));

#if 0
							       "show_week_numbers", TRUE,
							       "minimum_columns", 5,
							       "maximum_columns", 5,
#endif

	if (small_font)
		gdk_font_unref (small_font);


	button = gtk_button_new ();
	/* FIXME: The buttons doesn't display properly if we do this. */
	/*gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);*/
	gtk_widget_show (button);
	gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
				   GTK_SIGNAL_FUNC (e_calendar_on_prev_clicked),
				   GTK_OBJECT (cal));
	arrow = gtk_arrow_new (GTK_ARROW_LEFT, GTK_SHADOW_OUT);
	gtk_widget_show (arrow);
	gtk_container_add (GTK_CONTAINER (button), arrow);
	cal->prev_item = gnome_canvas_item_new (canvas_group,
						gnome_canvas_widget_get_type (),
						"widget", button,
						NULL);

	button = gtk_button_new ();
	/*gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);*/
	gtk_widget_show (button);
	gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
				   GTK_SIGNAL_FUNC (e_calendar_on_next_clicked),
				   GTK_OBJECT (cal));
	arrow = gtk_arrow_new (GTK_ARROW_RIGHT, GTK_SHADOW_OUT);
	gtk_widget_show (arrow);
	gtk_container_add (GTK_CONTAINER (button), arrow);
	cal->next_item = gnome_canvas_item_new (canvas_group,
						gnome_canvas_widget_get_type (),
						"widget", button,
						NULL);

	cal->min_rows = 1;
	cal->min_cols = 1;
	cal->max_rows = -1;
	cal->max_cols = -1;
}


/**
 * e_calendar_new:
 * @Returns: a new #ECalendar.
 *
 * Creates a new #ECalendar.
 **/
GtkWidget *
e_calendar_new			(void)
{
	GtkWidget *cal;

	cal = gtk_type_new (e_calendar_get_type ());

	return cal;
}


static void
e_calendar_destroy		(GtkObject *object)
{
	ECalendar *cal;

	g_return_if_fail (object != NULL);
	g_return_if_fail (E_IS_CALENDAR (object));

	cal = E_CALENDAR (object);



}


static void
e_calendar_realize (GtkWidget *widget)
{
	(*GTK_WIDGET_CLASS (parent_class)->realize) (widget);
}


static void
e_calendar_style_set		(GtkWidget	*widget,
				 GtkStyle	*previous_style)
{
}


static void
e_calendar_size_request		(GtkWidget      *widget,
				 GtkRequisition *requisition)
{
	ECalendar *cal;
	GtkStyle *style;
	gdouble col_width, row_height;
	gint width, height;

	cal = E_CALENDAR (widget);
	style = GTK_WIDGET (cal)->style;

	gtk_object_get (GTK_OBJECT (cal->calitem),
			"row_height", &row_height,
			"column_width", &col_width,
			NULL);

	height = row_height * cal->min_rows;
	width = col_width * cal->min_cols;

	/* FIXME: Add on space for line & button if shown. */

	requisition->width = width + style->klass->xthickness * 2;
	requisition->height = height + style->klass->ythickness * 2;
}


static void
e_calendar_size_allocate	(GtkWidget	*widget,
				 GtkAllocation	*allocation)
{
	ECalendar *cal;
	GdkFont *font;
	gdouble old_x2, old_y2, new_x2, new_y2;
	gdouble xthickness, ythickness, button_size;

	cal = E_CALENDAR (widget);
	font = widget->style->font;
	xthickness = widget->style->klass->xthickness;
	ythickness = widget->style->klass->ythickness;

	(*GTK_WIDGET_CLASS (parent_class)->size_allocate) (widget, allocation);

	/* Set the scroll region to its allocated size, if changed. */
	gnome_canvas_get_scroll_region (GNOME_CANVAS (cal),
					NULL, NULL, &old_x2, &old_y2);
	new_x2 = widget->allocation.width - 1;
	new_y2 = widget->allocation.height - 1;
	if (old_x2 != new_x2 || old_y2 != new_y2)
		gnome_canvas_set_scroll_region (GNOME_CANVAS (cal),
						0, 0, new_x2, new_y2);

	/* FIXME: Take off space for line & buttons if shown. */
	gnome_canvas_item_set (GNOME_CANVAS_ITEM (cal->calitem),
			       "x1", 0.0,
			       "y1", 0.0,
			       "x2", new_x2,
			       "y2", new_y2,
			       NULL);

	button_size = font->ascent + font->descent + E_CALENDAR_ITEM_YPAD1
		+ E_CALENDAR_ITEM_YPAD2 - E_CALENDAR_BUTTON_Y_PAD * 2;

	gnome_canvas_item_set (cal->prev_item,
			       "x", xthickness * 2 + E_CALENDAR_BUTTON_X_PAD,
			       "y", ythickness * 2 + E_CALENDAR_BUTTON_Y_PAD,
			       "width", button_size,
			       "height", button_size,
			       NULL);

	gnome_canvas_item_set (cal->next_item,
			       "x", new_x2 + 1 - xthickness * 2
			       - E_CALENDAR_BUTTON_X_PAD - button_size,
			       "y", ythickness * 2 + E_CALENDAR_BUTTON_Y_PAD,
			       "width", button_size,
			       "height", button_size,
			       NULL);
}


static gint
e_calendar_expose		(GtkWidget      *widget,
				 GdkEventExpose *event)
{
	ECalendar *cal;

	cal = E_CALENDAR (widget);

	(*GTK_WIDGET_CLASS (parent_class)->expose_event) (widget, event);

	e_calendar_paint (cal, &event->area);

	return FALSE;
}


static void
e_calendar_draw			(GtkWidget	*widget,
				 GdkRectangle	*area)
{
	ECalendar *cal;

	cal = E_CALENDAR (widget);

	(*GTK_WIDGET_CLASS (parent_class)->draw) (widget, area);

	(*GTK_WIDGET_CLASS (grandparent_class)->draw) (widget, area);

	e_calendar_paint (cal, area);
}


static void
e_calendar_paint		(ECalendar	*cal,
				 GdkRectangle	*area)
{
#if 0
	g_print ("In e_calendar_paint: %i,%i %ix%i\n", area->x, area->y,
		 area->width, area->height);
#endif
}



static gint
e_calendar_button_press		(GtkWidget	*widget,
				 GdkEventButton	*event)
{
	ECalendar *cal;

	cal = E_CALENDAR (widget);

	g_print ("In e_calendar_button_press\n");

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
e_calendar_focus_in		(GtkWidget	*widget,
				 GdkEventFocus	*event)
{
	return FALSE;
}


static gint
e_calendar_focus_out		(GtkWidget	*widget,
				 GdkEventFocus	*event)
{
	return FALSE;
}


static gint
e_calendar_key_press		(GtkWidget	*widget,
				 GdkEventKey	*event)
{
	return FALSE;
}


void
e_calendar_set_minimum_size	(ECalendar	*cal,
				 gint		 rows,
				 gint		 cols)
{
	g_return_if_fail (E_IS_CALENDAR (cal));

	cal->min_rows = rows;
	cal->min_cols = cols;

	gnome_canvas_item_set (GNOME_CANVAS_ITEM (cal->calitem),
			       "minimum_rows", rows,
			       "minimum_columns", cols,
			       NULL);

	gtk_widget_queue_resize (GTK_WIDGET (cal));
}


void
e_calendar_set_maximum_size	(ECalendar	*cal,
				 gint		 rows,
				 gint		 cols)
{
	g_return_if_fail (E_IS_CALENDAR (cal));

	cal->max_rows = rows;
	cal->max_cols = cols;

	gnome_canvas_item_set (GNOME_CANVAS_ITEM (cal->calitem),
			       "maximum_rows", rows,
			       "maximum_columns", cols,
			       NULL);

	gtk_widget_queue_resize (GTK_WIDGET (cal));
}


static void
e_calendar_on_prev_clicked	(ECalendar	*cal)
{
	gint year, month;

	gtk_object_get (GTK_OBJECT (cal->calitem),
			"year", &year,
			"month", &month,
			NULL);

	month--;
	if (month == -1) {
		year--;
		month = 11;
	}

	gnome_canvas_item_set (GNOME_CANVAS_ITEM (cal->calitem),
			       "year", year,
			       "month", month,
			       NULL);
}


static void
e_calendar_on_next_clicked	(ECalendar	*cal)
{
	gint year, month;

	gtk_object_get (GTK_OBJECT (cal->calitem),
			"year", &year,
			"month", &month,
			NULL);

	month++;
	if (month == 12) {
		year++;
		month = 0;
	}

	gnome_canvas_item_set (GNOME_CANVAS_ITEM (cal->calitem),
			       "year", year,
			       "month", month,
			       NULL);
}

