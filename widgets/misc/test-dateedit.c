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
 * test-dateedit - tests the EDateEdit widget.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>

#include "e-dateedit.h"

static void delete_event_cb		(GtkWidget	*widget,
					 GdkEventAny	*event,
					 GtkWidget	*app);
static void on_get_date_clicked		(GtkWidget	*button,
					 EDateEdit	*dedit);
static void on_toggle_24_hour_clicked	(GtkWidget	*button,
					 EDateEdit	*dedit);
static void on_changed			(EDateEdit	*dedit,
					 gchar		*name);
#if 0
static void on_date_changed		(EDateEdit	*dedit,
					 gchar		*name);
static void on_time_changed		(EDateEdit	*dedit,
					 gchar		*name);
#endif

int
main (int argc, char **argv)
{
	GtkWidget *app;
	EDateEdit *dedit;
	GtkWidget *table, *button;

	gnome_init ("test-dateedit", "0.0", argc, argv);

	app = gnome_app_new ("Test", "Test");
	gtk_window_set_default_size (GTK_WINDOW (app), 300, 200);
	gtk_window_set_policy (GTK_WINDOW (app), FALSE, TRUE, TRUE);
	gtk_container_set_border_width (GTK_CONTAINER (app), 8);

	gtk_signal_connect (GTK_OBJECT (app), "delete_event",
			    GTK_SIGNAL_FUNC (delete_event_cb), app);

	table = gtk_table_new (3, 3, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (table), 4);
	gtk_table_set_col_spacings (GTK_TABLE (table), 4);
	gtk_widget_show (table);
	gnome_app_set_contents (GNOME_APP (app), table);

	/* EDateEdit 1. */
	dedit = E_DATE_EDIT (e_date_edit_new ());
	gtk_table_attach (GTK_TABLE (table), (GtkWidget*) dedit,
			  0, 1, 0, 1, GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_widget_show ((GtkWidget*) (dedit));

#if 0
	gtk_signal_connect (GTK_OBJECT (dedit), "date_changed",
			    GTK_SIGNAL_FUNC (on_date_changed), "1");
	gtk_signal_connect (GTK_OBJECT (dedit), "time_changed",
			    GTK_SIGNAL_FUNC (on_time_changed), "1");
#else
	gtk_signal_connect (GTK_OBJECT (dedit), "changed",
			    GTK_SIGNAL_FUNC (on_changed), "1");
#endif

	button = gtk_button_new_with_label ("Print Date");
	gtk_table_attach (GTK_TABLE (table), button,
			  1, 2, 0, 1, 0, 0, 0, 0);
	gtk_widget_show (button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_get_date_clicked), dedit);

	/* EDateEdit 2. */
	dedit = E_DATE_EDIT (e_date_edit_new ());
	gtk_table_attach (GTK_TABLE (table), (GtkWidget*) dedit,
			  0, 1, 1, 2, GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_widget_show ((GtkWidget*) (dedit));
	e_date_edit_set_week_start_day (dedit, 1);
	e_date_edit_set_show_week_numbers (dedit, TRUE);
	e_date_edit_set_use_24_hour_format (dedit, FALSE);
	e_date_edit_set_time_popup_range (dedit, 8, 18);
	e_date_edit_set_show_time (dedit, FALSE);

#if 0
	gtk_signal_connect (GTK_OBJECT (dedit), "date_changed",
			    GTK_SIGNAL_FUNC (on_date_changed), "2");
	gtk_signal_connect (GTK_OBJECT (dedit), "time_changed",
			    GTK_SIGNAL_FUNC (on_time_changed), "2");
#else
	gtk_signal_connect (GTK_OBJECT (dedit), "changed",
			    GTK_SIGNAL_FUNC (on_changed), "2");
#endif

	button = gtk_button_new_with_label ("Print Date");
	gtk_table_attach (GTK_TABLE (table), button,
			  1, 2, 1, 2, 0, 0, 0, 0);
	gtk_widget_show (button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_get_date_clicked), dedit);

	/* EDateEdit 3. */
	dedit = E_DATE_EDIT (e_date_edit_new ());
	gtk_table_attach (GTK_TABLE (table), (GtkWidget*) dedit,
			  0, 1, 2, 3, GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_widget_show ((GtkWidget*) (dedit));
	e_date_edit_set_week_start_day (dedit, 1);
	e_date_edit_set_show_week_numbers (dedit, TRUE);
	e_date_edit_set_use_24_hour_format (dedit, FALSE);
	e_date_edit_set_time_popup_range (dedit, 8, 18);
	e_date_edit_set_allow_no_date_set (dedit, TRUE);

#if 0
	gtk_signal_connect (GTK_OBJECT (dedit), "date_changed",
			    GTK_SIGNAL_FUNC (on_date_changed), "3");
	gtk_signal_connect (GTK_OBJECT (dedit), "time_changed",
			    GTK_SIGNAL_FUNC (on_time_changed), "3");
#else
	gtk_signal_connect (GTK_OBJECT (dedit), "changed",
			    GTK_SIGNAL_FUNC (on_changed), "3");
#endif

	button = gtk_button_new_with_label ("Print Date");
	gtk_table_attach (GTK_TABLE (table), button,
			  1, 2, 2, 3, 0, 0, 0, 0);
	gtk_widget_show (button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_get_date_clicked), dedit);

	button = gtk_button_new_with_label ("Toggle 24-hour");
	gtk_table_attach (GTK_TABLE (table), button,
			  2, 3, 2, 3, 0, 0, 0, 0);
	gtk_widget_show (button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_toggle_24_hour_clicked),
			    dedit);

	gtk_widget_show (app);

	gtk_main ();

	return 0;
}


static void
delete_event_cb		(GtkWidget	*widget,
			 GdkEventAny	*event,
			 GtkWidget	*app)
{
	gtk_widget_destroy (app);

	gtk_main_quit ();
}


static void
on_get_date_clicked	(GtkWidget	*button,
			 EDateEdit	*dedit)
{
	time_t t;

	t = e_date_edit_get_time (dedit);
	if (t == -1)
		g_print ("Time: None\n");
	else
		g_print ("Time: %s", ctime (&t));

	if (!e_date_edit_date_is_valid (dedit))
		g_print ("  Date invalid\n");

	if (!e_date_edit_time_is_valid (dedit))
		g_print ("  Time invalid\n");
}


static void
on_toggle_24_hour_clicked	(GtkWidget	*button,
				 EDateEdit	*dedit)
{
	e_date_edit_set_use_24_hour_format (dedit, !e_date_edit_get_use_24_hour_format (dedit));
}


#if 0
static void
on_date_changed		(EDateEdit	*dedit,
			 gchar		*name)
{
	gint year, month, day;

	if (e_date_edit_date_is_valid (dedit)) {
		if (e_date_edit_get_date (dedit, &year, &month, &day)) {
			g_print ("Date %s changed to: %i/%i/%i (M/D/Y)\n",
				 name, month, day, year);
		} else {
			g_print ("Date %s changed to: None\n", name);
		}
	} else {
		g_print ("Date %s changed to: Not Valid\n", name);
	}
}


static void
on_time_changed		(EDateEdit	*dedit,
			 gchar		*name)
{
	gint hour, minute;

	if (e_date_edit_time_is_valid (dedit)) {
		if (e_date_edit_get_time_of_day (dedit, &hour, &minute)) {
			g_print ("Time %s changed to: %02i:%02i\n", name,
				 hour, minute);
		} else {
			g_print ("Time %s changed to: None\n", name);
		}
	} else {
		g_print ("Time %s changed to: Not Valid\n", name);
	}
}
#endif


static void
on_changed		(EDateEdit	*dedit,
			 gchar		*name)
{
	gint year, month, day, hour, minute;

	g_print ("Date %s changed ", name);

	if (e_date_edit_date_is_valid (dedit)) {
		if (e_date_edit_get_date (dedit, &year, &month, &day)) {
			g_print ("M/D/Y: %i/%i/%i", month, day, year);
		} else {
			g_print ("None");
		}
	} else {
		g_print ("Date Invalid");
	}

	if (e_date_edit_time_is_valid (dedit)) {
		if (e_date_edit_get_time_of_day (dedit, &hour, &minute)) {
			g_print (" %02i:%02i\n", hour, minute);
		} else {
			g_print (" None\n");
		}
	} else {
		g_print (" Time Invalid\n");
	}
}


