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
 * test-calendar - tests the ECalendar widget.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>

#include "e-calendar.h"

/* Drag and Drop stuff. */
enum {
	TARGET_SHORTCUT
};
static GtkTargetEntry target_table[] = {
	{ "E-SHORTCUT",     0, TARGET_SHORTCUT }
};
static guint n_targets = sizeof(target_table) / sizeof(target_table[0]);


static void on_date_range_changed	(ECalendarItem *calitem);
static void on_selection_changed	(ECalendarItem *calitem);
static void get_day_style		(ECalendarItem	*calitem,
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
					 gboolean	*bold,
					 gpointer	 data);

static void
delete_event_cb (GtkWidget *widget,
		 GdkEventAny *event,
		 gpointer data)
{
	gtk_main_quit ();
}

int
main (int argc, char **argv)
{
	GtkWidget *app;
	GtkWidget *cal;
	GtkWidget *vbox;
	ECalendarItem *calitem;

	gnome_init ("test-calendar", "0.0", argc, argv);

	app = gnome_app_new ("Test", "Test");
	gtk_window_set_default_size (GTK_WINDOW (app), 400, 400);
	gtk_window_set_policy (GTK_WINDOW (app), FALSE, TRUE, FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (app), 8);

	gtk_signal_connect (GTK_OBJECT (app), "delete_event",
			    GTK_SIGNAL_FUNC (delete_event_cb), NULL);

	cal = e_calendar_new ();
	e_calendar_set_minimum_size (E_CALENDAR (cal), 1, 1);
	calitem = E_CALENDAR (cal)->calitem;
	gtk_widget_show (cal);
	e_calendar_item_set_style_callback (calitem, get_day_style,
					    NULL, NULL);

	gtk_signal_connect (GTK_OBJECT (calitem), "date_range_changed",
			    GTK_SIGNAL_FUNC (on_date_range_changed), NULL);
	gtk_signal_connect (GTK_OBJECT (calitem), "selection_changed",
			    GTK_SIGNAL_FUNC (on_selection_changed), NULL);


	gtk_drag_dest_set (cal,
			   GTK_DEST_DEFAULT_ALL,
			   target_table, n_targets,
			   GDK_ACTION_COPY | GDK_ACTION_MOVE);


	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), cal, TRUE, TRUE, 0);
	gtk_widget_show (vbox);

	gnome_app_set_contents (GNOME_APP (app), vbox);
	gtk_widget_show (app);

	gtk_main ();

	return 0;
}


static void
on_date_range_changed (ECalendarItem *calitem)
{
	gint start_year, start_month, start_day;
	gint end_year, end_month, end_day;

	e_calendar_item_get_date_range	(calitem,
					 &start_year, &start_month, &start_day,
					 &end_year, &end_month, &end_day);

	g_print ("Date range changed (D/M/Y): %i/%i/%i - %i/%i/%i\n",
		 start_day, start_month + 1, start_year,
		 end_day, end_month + 1, end_year);

	/* These days should appear bold. Remember month is 0 to 11. */
	e_calendar_item_mark_day (calitem, 2000, 7, 26, /* 26th Aug 2000. */
				  E_CALENDAR_ITEM_MARK_BOLD);
	e_calendar_item_mark_day (calitem, 2000, 8, 13, /* 13th Sep 2000. */
				  E_CALENDAR_ITEM_MARK_BOLD);
}


static void
on_selection_changed (ECalendarItem *calitem)
{
	GDate start_date, end_date;

	e_calendar_item_get_selection	(calitem, &start_date, &end_date);

	g_print ("Selection changed (D/M/Y): %i/%i/%i - %i/%i/%i\n",
		 g_date_day (&start_date),
		 g_date_month (&start_date),
		 g_date_year (&start_date),
		 g_date_day (&end_date),
		 g_date_month (&end_date),
		 g_date_year (&end_date));
}


static void
get_day_style		(ECalendarItem	*calitem,
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
			 gboolean	*bold,
			 gpointer	 data)
{
	*bg_color = NULL;
	*fg_color = NULL;
	*box_color = NULL;
	*bold = FALSE;

#if 1

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

#else

	if (day_style == 1)
		*bold = TRUE;

	if (today)
		*box_color = &calitem->colors[E_CALENDAR_ITEM_COLOR_PREV_OR_NEXT_MONTH_FG];

	if (prev_or_next_month)
		*fg_color = &calitem->colors[E_CALENDAR_ITEM_COLOR_TODAY_BOX];

	if (selected) {
		*fg_color = &calitem->colors[E_CALENDAR_ITEM_COLOR_SELECTION_FG];
		*bg_color = &calitem->colors[E_CALENDAR_ITEM_COLOR_PREV_OR_NEXT_MONTH_FG];
	}

#endif
}
