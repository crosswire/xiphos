/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 * Author :
 *  Damon Chaplin <damon@helixcode.com>
 *
 * Copyright 2000, Helix Code, Inc.
 *
 * Based on the GnomeDateEdit, part of the Gnome Library.
 * Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation
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
 * EDateEdit - a widget based on GnomeDateEdit to provide a date & optional
 * time field with popups for entering a date.
 */

/* We need this for strptime. */
#define _XOPEN_SOURCE 4

#include <config.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtkmain.h>
#include "e-dateedit.h"
#include "e-calendar.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkoptionmenu.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkarrow.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkwindow.h>


struct _EDateEditPrivate {
	GtkWidget *date_entry;
	GtkWidget *date_button;
	
	GtkWidget *space;

	GtkWidget *time_combo;

	GtkWidget *cal_popup;
	GtkWidget *calendar;
	GtkWidget *now_button;
	GtkWidget *today_button;
	GtkWidget *none_button;		/* This will only be visible if a
					   'None' date/time is permitted. */

	gboolean show_date;
	gboolean show_time;
	gboolean use_24_hour_format;

	/* This is the range of hours we show in the time popup. */
	gint lower_hour;
	gint upper_hour;

	/* This indicates whether the last date committed was invalid.
	   (A date is committed by hitting Return, moving the keyboard focus,
	   or selecting a date in the popup). Note that this only indicates
	   that the date couldn't be parsed. A date set to 'None' is valid
	   here, though e_date_edit_date_is_valid() will return FALSE if an
	   empty date isn't actually permitted. */
	gboolean date_is_valid;

	/* This is the last valid date which was set. If the date was set to
	   'None' or empty, date_set_to_none will be TRUE and the other fields
	   are undefined, so don't use them. */
	gboolean date_set_to_none;
	gint year;
	gint month;
	gint day;

	/* This indicates whether the last time committed was invalid.
	   (A time is committed by hitting Return, moving the keyboard focus,
	   or selecting a time in the popup). Note that this only indicates
	   that the time couldn't be parsed. An empty/None time is valid
	   here, though e_date_edit_time_is_valid() will return FALSE if an
	   empty time isn't actually permitted. */
	gboolean time_is_valid;

	/* This is the last valid time which was set. If the time was set to
	   'None' or empty, time_set_to_none will be TRUE and the other fields
	   are undefined, so don't use them. */
	gboolean time_set_to_none;
	gint hour;
	gint minute;
};

enum {
	CHANGED,
	LAST_SIGNAL
};


static gint date_edit_signals [LAST_SIGNAL] = { 0 };


static void e_date_edit_class_init		(EDateEditClass	*class);
static void e_date_edit_init			(EDateEdit	*dedit);
static void create_children			(EDateEdit	*dedit);
static void e_date_edit_destroy			(GtkObject	*object);
static void e_date_edit_forall			(GtkContainer   *container,
						 gboolean	 include_internals,
						 GtkCallback     callback,
						 gpointer	 callback_data);

static gint on_date_entry_key_press		(GtkWidget	*widget,
						 GdkEventKey	*event,
						 EDateEdit	*dedit);
static void on_date_button_clicked		(GtkWidget	*widget,
						 EDateEdit	*dedit);
static void e_date_edit_show_date_popup		(EDateEdit	*dedit);
static void position_date_popup			(EDateEdit	*dedit);
static void on_date_popup_none_button_clicked	(GtkWidget	*button,
						 EDateEdit	*dedit);
static void on_date_popup_today_button_clicked	(GtkWidget	*button,
						 EDateEdit	*dedit);
static void on_date_popup_now_button_clicked	(GtkWidget	*button,
						 EDateEdit	*dedit);
static gint on_date_popup_delete_event		(GtkWidget	*widget,
						 EDateEdit	*dedit);
static gint on_date_popup_key_press		(GtkWidget	*widget,
						 GdkEventKey	*event,
						 EDateEdit	*dedit);
static gint on_date_popup_button_press		(GtkWidget	*widget,
						 GdkEventButton *event,
						 gpointer	 data);
static void on_date_popup_date_selected		(ECalendarItem	*calitem,
						 EDateEdit	*dedit);
static void hide_date_popup			(EDateEdit	*dedit);
static void rebuild_time_popup			(EDateEdit	*dedit);
static gboolean field_set_to_none		(char		*text);
static gboolean e_date_edit_parse_date		(EDateEdit	*dedit,
						 char		*date_text,
						 struct tm	*date_tm);
static gboolean e_date_edit_parse_time		(EDateEdit	*dedit,
						 gchar		*time_text,
						 struct tm	*time_tm);
static void on_date_edit_time_selected		(GtkList	*list,
						 EDateEdit	*dedit);
static gint on_time_entry_key_press		(GtkWidget	*widget,
						 GdkEventKey	*event,
						 EDateEdit	*dedit);
static gint on_date_entry_focus_out		(GtkEntry	*entry,
						 GdkEventFocus  *event,
						 EDateEdit	*dedit);
static gint on_time_entry_focus_out		(GtkEntry	*entry,
						 GdkEventFocus  *event,
						 EDateEdit	*dedit);
static void e_date_edit_update_date_entry	(EDateEdit	*dedit);
static void e_date_edit_update_time_entry	(EDateEdit	*dedit);
static void e_date_edit_update_time_combo_state	(EDateEdit	*dedit);
static void e_date_edit_check_date_changed	(EDateEdit	*dedit);
static void e_date_edit_check_time_changed	(EDateEdit	*dedit);
static gboolean e_date_edit_set_date_internal	(EDateEdit	*dedit,
						 gboolean	 valid,
						 gboolean	 none,
						 gint		 year,
						 gint		 month,
						 gint		 day);
static gboolean e_date_edit_set_time_internal	(EDateEdit	*dedit,
						 gboolean	 valid,
						 gboolean	 none,
						 gint		 hour,
						 gint		 minute);


static GtkHBoxClass *parent_class;

/**
 * e_date_edit_get_type:
 *
 * Returns the GtkType for the EDateEdit widget
 */
guint
e_date_edit_get_type		(void)
{
	static guint date_edit_type = 0;

	if (!date_edit_type){
		GtkTypeInfo date_edit_info = {
			"EDateEdit",
			sizeof (EDateEdit),
			sizeof (EDateEditClass),
			(GtkClassInitFunc) e_date_edit_class_init,
			(GtkObjectInitFunc) e_date_edit_init,
			NULL,
			NULL,
		};

		date_edit_type = gtk_type_unique (gtk_hbox_get_type (), &date_edit_info);
	}
	
	return date_edit_type;
}


static void
e_date_edit_class_init		(EDateEditClass	*class)
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkContainerClass *container_class = (GtkContainerClass *) class;

	object_class = (GtkObjectClass*) class;

	parent_class = gtk_type_class (gtk_hbox_get_type ());

	date_edit_signals [CHANGED] =
		gtk_signal_new ("changed",
				GTK_RUN_FIRST, object_class->type,
				GTK_SIGNAL_OFFSET (EDateEditClass,
						   changed),
				gtk_signal_default_marshaller,
				GTK_TYPE_NONE, 0);
	
	gtk_object_class_add_signals (object_class, date_edit_signals,
				      LAST_SIGNAL);

	object_class->destroy		= e_date_edit_destroy;

	container_class->forall		= e_date_edit_forall;

	class->changed = NULL;
}


static void
e_date_edit_init		(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;

	dedit->priv = priv = g_new0 (EDateEditPrivate, 1);

	priv->show_date = TRUE;
	priv->show_time = TRUE;
	priv->use_24_hour_format = TRUE;

	priv->lower_hour = 0;
	priv->upper_hour = 24;

	priv->date_is_valid = TRUE;
	priv->date_set_to_none = TRUE;
	priv->time_is_valid = TRUE;
	priv->time_set_to_none = TRUE;

	create_children (dedit);

	/* Set it to the current time. */
	e_date_edit_set_time (dedit, 0);
}


/**
 * e_date_edit_new:
 *
 * Description: Creates a new #EDateEdit widget which can be used
 * to provide an easy to use way for entering dates and times.
 * 
 * Returns: a new #EDateEdit widget.
 */
GtkWidget *
e_date_edit_new			(void)
{
	EDateEdit *dedit;

	dedit = gtk_type_new (e_date_edit_get_type ());

	return GTK_WIDGET (dedit);
}


static void
create_children			(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	ECalendar *calendar;
	GtkWidget *frame, *arrow;
	GtkWidget *vbox, *bbox;

	priv = dedit->priv;

	priv->date_entry  = gtk_entry_new ();
	gtk_widget_set_usize (priv->date_entry, 90, 0);
	gtk_box_pack_start (GTK_BOX (dedit), priv->date_entry, FALSE, TRUE, 0);
	
	gtk_signal_connect (GTK_OBJECT (priv->date_entry), "key_press_event",
			    (GtkSignalFunc) on_date_entry_key_press,
			    dedit);
	gtk_signal_connect_after (GTK_OBJECT (priv->date_entry),
				  "focus_out_event",
				  (GtkSignalFunc) on_date_entry_focus_out,
				  dedit);

	priv->date_button = gtk_button_new ();
	gtk_signal_connect (GTK_OBJECT (priv->date_button), "clicked",
			    GTK_SIGNAL_FUNC (on_date_button_clicked), dedit);
	gtk_box_pack_start (GTK_BOX (dedit), priv->date_button,
			    FALSE, FALSE, 0);

	arrow = gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_OUT);
	gtk_container_add (GTK_CONTAINER (priv->date_button), arrow);
	gtk_widget_show (arrow);

	if (priv->show_date) {
		gtk_widget_show (priv->date_entry);
		gtk_widget_show (priv->date_button);
	}

	/* This is just to create a space between the date & time parts. */
	priv->space = gtk_drawing_area_new ();
	gtk_box_pack_start (GTK_BOX (dedit), priv->space, FALSE, FALSE, 2);


	priv->time_combo = gtk_combo_new ();
	gtk_widget_set_usize (GTK_COMBO (priv->time_combo)->entry, 90, 0);
	gtk_box_pack_start (GTK_BOX (dedit), priv->time_combo, FALSE, TRUE, 0);
	rebuild_time_popup (dedit);

	gtk_signal_connect (GTK_OBJECT (GTK_COMBO (priv->time_combo)->entry),
			    "key_press_event",
			    (GtkSignalFunc) on_time_entry_key_press,
			    dedit);
	gtk_signal_connect_after (GTK_OBJECT (GTK_COMBO (priv->time_combo)->entry),
				  "focus_out_event",
				  (GtkSignalFunc) on_time_entry_focus_out,
				  dedit);
	gtk_signal_connect_after (GTK_OBJECT (GTK_COMBO (priv->time_combo)->list),
				  "selection_changed",
				  (GtkSignalFunc) on_date_edit_time_selected,
				  dedit);

	if (priv->show_time)
		gtk_widget_show (priv->time_combo);

	if (priv->show_date && priv->show_time)
		gtk_widget_show (priv->space);

	priv->cal_popup = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_widget_set_events (priv->cal_popup,
			       gtk_widget_get_events (priv->cal_popup)
			       | GDK_KEY_PRESS_MASK);
	gtk_signal_connect (GTK_OBJECT (priv->cal_popup), "delete_event",
			    (GtkSignalFunc) on_date_popup_delete_event,
			    dedit);
	gtk_signal_connect (GTK_OBJECT (priv->cal_popup), "key_press_event",
			    (GtkSignalFunc) on_date_popup_key_press,
			    dedit);
	gtk_signal_connect (GTK_OBJECT (priv->cal_popup), "button_press_event",
			    (GtkSignalFunc) on_date_popup_button_press,
			    dedit);
	gtk_window_set_policy (GTK_WINDOW (priv->cal_popup),
			       FALSE, FALSE, TRUE);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	gtk_container_add (GTK_CONTAINER (priv->cal_popup), frame);
	gtk_widget_show (frame);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), vbox);
        gtk_widget_show (vbox);

	priv->calendar = e_calendar_new ();
	calendar = E_CALENDAR (priv->calendar);
	gnome_canvas_item_set (GNOME_CANVAS_ITEM (calendar->calitem),
			       "maximum_days_selected", 1,
			       "move_selection_when_moving", FALSE,
			       NULL);

	gtk_signal_connect (GTK_OBJECT (calendar->calitem),
			    "selection_changed",
			    GTK_SIGNAL_FUNC (on_date_popup_date_selected), dedit);

	gtk_box_pack_start (GTK_BOX (vbox), priv->calendar, FALSE, FALSE, 0);
        gtk_widget_show (priv->calendar);

	bbox = gtk_hbutton_box_new ();
	gtk_container_set_border_width (GTK_CONTAINER (bbox), 4);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (bbox), 2);
	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (bbox), 2, 0);
	gtk_button_box_set_child_size (GTK_BUTTON_BOX (bbox), 0, 0);
	gtk_box_pack_start (GTK_BOX (vbox), bbox, FALSE, FALSE, 0);
        gtk_widget_show (bbox);

	priv->now_button = gtk_button_new_with_label (_("Now"));
	gtk_container_add (GTK_CONTAINER (bbox), priv->now_button);
        gtk_widget_show (priv->now_button);
	gtk_signal_connect (GTK_OBJECT (priv->now_button), "clicked",
			    GTK_SIGNAL_FUNC (on_date_popup_now_button_clicked), dedit);

	priv->today_button = gtk_button_new_with_label (_("Today"));
	gtk_container_add (GTK_CONTAINER (bbox), priv->today_button);
        gtk_widget_show (priv->today_button);
	gtk_signal_connect (GTK_OBJECT (priv->today_button), "clicked",
			    GTK_SIGNAL_FUNC (on_date_popup_today_button_clicked), dedit);

	/* Note that we don't show this here, since by default a 'None' date
	   is not permitted. */
	priv->none_button = gtk_button_new_with_label (_("None"));
	gtk_container_add (GTK_CONTAINER (bbox), priv->none_button);
	gtk_signal_connect (GTK_OBJECT (priv->none_button), "clicked",
			    GTK_SIGNAL_FUNC (on_date_popup_none_button_clicked), dedit);
}


static void
e_date_edit_destroy		(GtkObject	*object)
{
	EDateEdit *dedit;

	g_return_if_fail (E_IS_DATE_EDIT (object));

	dedit = E_DATE_EDIT (object);

	gtk_widget_destroy (dedit->priv->cal_popup);
	dedit->priv->cal_popup = NULL;

	g_free (dedit->priv);
	dedit->priv = NULL;

	if (GTK_OBJECT_CLASS (parent_class)->destroy)
		(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}


static void
e_date_edit_forall		(GtkContainer	*container,
				 gboolean	 include_internals,
				 GtkCallback	 callback,
				 gpointer	 callback_data)
{
	g_return_if_fail (E_IS_DATE_EDIT (container));
	g_return_if_fail (callback != NULL);

	/* Let GtkBox handle the internal widgets if needed. */
	if (include_internals) {
		if (GTK_CONTAINER_CLASS (parent_class)->forall)
			(* GTK_CONTAINER_CLASS (parent_class)->forall)
				(container, include_internals,
				 callback, callback_data);
	}
}


/**
 * e_date_edit_get_time:
 * @dedit: an #EDateEdit widget.
 * @the_time: returns the last valid time entered.
 * @Returns: the last valid time entered, or -1 if the time is not set.
 *
 * Returns the last valid time entered. If empty times are valid, by calling
 * e_date_edit_set_allow_no_date_set(), then it may return -1.
 *
 * Note that the last time entered may actually have been invalid. You can
 * check this with e_date_edit_time_is_valid().
 */
time_t
e_date_edit_get_time		(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	struct tm tmp_tm = { 0 };

	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), -1);
	
	priv = dedit->priv;

	/* Try to parse any new value now. */
	e_date_edit_check_date_changed (dedit);
	e_date_edit_check_time_changed (dedit);

	if (priv->date_set_to_none)
		return -1;

	tmp_tm.tm_year = priv->year;
	tmp_tm.tm_mon = priv->month;
	tmp_tm.tm_mday = priv->day;

	if (!priv->show_time || priv->time_set_to_none) {
		tmp_tm.tm_hour = 0;
		tmp_tm.tm_min = 0;
	} else {
		tmp_tm.tm_hour = priv->hour;
		tmp_tm.tm_min = priv->minute;
	}
	tmp_tm.tm_sec = 0;
	tmp_tm.tm_isdst = -1;

	return mktime (&tmp_tm);
}


/**
 * e_date_edit_set_time:
 * @dedit: the EDateEdit widget
 * @the_time: The time and date that should be set on the widget
 *
 * Description:  Changes the displayed date and time in the EDateEdit
 * widget to be the one represented by @the_time.  If @the_time is 0
 * then current time is used. If it is -1, then the date is set to None.
 */
void
e_date_edit_set_time		(EDateEdit	*dedit,
				 time_t		 the_time)
{
	EDateEditPrivate *priv;
	struct tm *tmp_tm;
	gboolean date_changed = FALSE, time_changed = FALSE;

	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	priv = dedit->priv;

	if (the_time == -1) {
		date_changed = e_date_edit_set_date_internal (dedit, TRUE,
							      TRUE, 0, 0, 0);
		time_changed = e_date_edit_set_time_internal (dedit, TRUE,
							      TRUE, 0, 0);
	} else {
		if (the_time == 0)
			the_time = time (NULL);

		tmp_tm = localtime (&the_time);

		date_changed = e_date_edit_set_date_internal (dedit, TRUE,
							      FALSE,
							      tmp_tm->tm_year,
							      tmp_tm->tm_mon,
							      tmp_tm->tm_mday);
		time_changed = e_date_edit_set_time_internal (dedit, TRUE,
							      FALSE,
							      tmp_tm->tm_hour,
							      tmp_tm->tm_min);
	}

	e_date_edit_update_date_entry (dedit);
	e_date_edit_update_time_entry (dedit);
	e_date_edit_update_time_combo_state (dedit);

	/* Emit the signals if the date and/or time has actually changed. */
	if (date_changed || time_changed)
		gtk_signal_emit (GTK_OBJECT (dedit),
				 date_edit_signals [CHANGED]);
}


/**
 * e_date_edit_get_date:
 * @dedit: an #EDateEdit widget.
 * @year: returns the year set.
 * @month: returns the month set (1 - 12).
 * @day: returns the day set (1 - 31).
 * @Returns: TRUE if a time was set, or FALSE if the field is empty or 'None'.
 *
 * Returns the last valid date entered into the date field.
 */
gboolean
e_date_edit_get_date		(EDateEdit	*dedit,
				 gint		*year,
				 gint		*month,
				 gint		*day)
{
	EDateEditPrivate *priv;

	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), FALSE);

	priv = dedit->priv;

	/* Try to parse any new value now. */
	e_date_edit_check_date_changed (dedit);

	if (priv->date_set_to_none)
		return FALSE;

	*year = priv->year + 1900;
	*month = priv->month + 1;
	*day = priv->day;

	return TRUE;
}


/**
 * e_date_edit_set_date:
 * @dedit: an #EDateEdit widget.
 * @year: the year to set.
 * @month: the month to set (1 - 12).
 * @day: the day to set (1 - 31).
 *
 * Sets the date in the date field.
 */
void
e_date_edit_set_date		(EDateEdit	*dedit,
				 gint		 year,
				 gint		 month,
				 gint		 day)
{
	EDateEditPrivate *priv;
	gboolean date_changed = FALSE;

	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	priv = dedit->priv;

	date_changed = e_date_edit_set_date_internal (dedit, TRUE, FALSE,
						      year - 1900, month - 1,
						      day);

	e_date_edit_update_date_entry (dedit);
	e_date_edit_update_time_combo_state (dedit);

	/* Emit the signals if the date has actually changed. */
	if (date_changed)
		gtk_signal_emit (GTK_OBJECT (dedit),
				 date_edit_signals [CHANGED]);
}


/**
 * e_date_edit_get_time_of_day:
 * @dedit: an #EDateEdit widget.
 * @hour: returns the hour set.
 * @minute: returns the minute set.
 * @Returns: TRUE if a time was set, or FALSE if the field is empty or 'None'.
 *
 * Returns the last valid time entered into the time field.
 */
gboolean
e_date_edit_get_time_of_day		(EDateEdit	*dedit,
					 gint		*hour,
					 gint		*minute)
{
	EDateEditPrivate *priv;

	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), FALSE);

	priv = dedit->priv;

	/* Try to parse any new value now. */
	e_date_edit_check_time_changed (dedit);

	if (priv->time_set_to_none)
		return FALSE;

	*hour = priv->hour;
	*minute = priv->minute;

	return TRUE;
}


/**
 * e_date_edit_set_time_of_day:
 * @dedit: an #EDateEdit widget.
 * @hour: the hour to set.
 * @minute: the minute to set.
 *
 * Description: Sets the time in the time field.
 */
void
e_date_edit_set_time_of_day		(EDateEdit	*dedit,
					 gint		 hour,
					 gint		 minute)
{
	EDateEditPrivate *priv;
	gboolean time_changed = FALSE;

	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	priv = dedit->priv;

	if (priv->time_set_to_none
	    || priv->hour != hour
	    || priv->minute != minute) {
		priv->time_set_to_none = FALSE;
		priv->hour = hour;
		priv->minute = minute;
		time_changed = TRUE;
	}

	e_date_edit_update_time_entry (dedit);

	if (time_changed)
		gtk_signal_emit (GTK_OBJECT (dedit),
				 date_edit_signals [CHANGED]);
}


/**
 * e_date_edit_get_show_date:
 * @dedit: an #EDateEdit widget.
 * @Returns: Whether the date field is shown.
 *
 * Description: Returns TRUE if the date field is currently shown.
 */
gboolean
e_date_edit_get_show_date		(EDateEdit	*dedit)
{
	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), TRUE);

	return dedit->priv->show_date;
}


/**
 * e_date_edit_set_show_date:
 * @dedit: an #EDateEdit widget.
 * @show_time: TRUE if the date field should be shown.
 *
 * Description: Specifies whether the date field should be shown. The date
 * field would be hidden if only a time needed to be entered.
 */
void
e_date_edit_set_show_date		(EDateEdit	*dedit,
					 gboolean	 show_date)
{
	EDateEditPrivate *priv;

	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	priv = dedit->priv;

	if (priv->show_date == show_date)
		return;

	priv->show_date = show_date;

	if (show_date) {
		gtk_widget_show (priv->date_entry);
		gtk_widget_show (priv->date_button);
	} else {
		gtk_widget_hide (priv->date_entry);
		gtk_widget_hide (priv->date_button);
	}

	e_date_edit_update_time_combo_state (dedit);

	if (priv->show_date && priv->show_time)
		gtk_widget_show (priv->space);
	else
		gtk_widget_hide (priv->space);
}


/**
 * e_date_edit_get_show_time:
 * @dedit: an #EDateEdit widget
 * @Returns: Whether the time field is shown.
 *
 * Description: Returns TRUE if the time field is currently shown.
 */
gboolean
e_date_edit_get_show_time		(EDateEdit	*dedit)
{
	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), TRUE);

	return dedit->priv->show_time;
}


/**
 * e_date_edit_set_show_time:
 * @dedit: an #EDateEdit widget
 * @show_time: TRUE if the time field should be shown.
 *
 * Description: Specifies whether the time field should be shown. The time
 * field would be hidden if only a date needed to be entered.
 */
void
e_date_edit_set_show_time		(EDateEdit	*dedit,
					 gboolean	 show_time)
{
	EDateEditPrivate *priv;

	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	priv = dedit->priv;

	if (priv->show_time == show_time)
		return;

	priv->show_time = show_time;

	if (show_time) {
		gtk_widget_show (priv->time_combo);
	} else {
		gtk_widget_hide (priv->time_combo);
		gtk_widget_hide (priv->now_button);
	}

	if (priv->show_date && priv->show_time)
		gtk_widget_show (priv->space);
	else
		gtk_widget_hide (priv->space);
}


/**
 * e_date_edit_get_week_start_day:
 * @dedit: an #EDateEdit widget
 * @Returns: the week start day, from 0 (Sunday) to 6 (Saturday).
 *
 * Description: Returns the week start day currently used in the calendar
 * popup.
 */
gint
e_date_edit_get_week_start_day		(EDateEdit	*dedit)
{
	gint week_start_day;

	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), 1);

	gtk_object_get (GTK_OBJECT (E_CALENDAR (dedit->priv->calendar)->calitem),
			"week_start_day", &week_start_day,
			NULL);

	return week_start_day;
}


/**
 * e_date_edit_set_week_start_day:
 * @dedit: an #EDateEdit widget
 * @week_start_day: the week start day, from 0 (Sunday) to 6 (Saturday).
 *
 * Description: Sets the week start day to use in the calendar popup.
 */
void
e_date_edit_set_week_start_day		(EDateEdit	*dedit,
					 gint		 week_start_day)
{
	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	gnome_canvas_item_set (GNOME_CANVAS_ITEM (E_CALENDAR (dedit->priv->calendar)->calitem),
			"week_start_day", week_start_day,
			NULL);
}


/* Whether we show week numbers in the date popup. */
gboolean
e_date_edit_get_show_week_numbers	(EDateEdit	*dedit)
{
	gboolean show_week_numbers;

	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), FALSE);

	gtk_object_get (GTK_OBJECT (E_CALENDAR (dedit->priv->calendar)->calitem),
			"show_week_numbers", &show_week_numbers,
			NULL);

	return show_week_numbers;
}


void
e_date_edit_set_show_week_numbers	(EDateEdit	*dedit,
					 gboolean	 show_week_numbers)
{
	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	gnome_canvas_item_set (GNOME_CANVAS_ITEM (E_CALENDAR (dedit->priv->calendar)->calitem),
			"show_week_numbers", show_week_numbers,
			NULL);
}


/* Whether we use 24 hour format in the time field & popup. */
gboolean
e_date_edit_get_use_24_hour_format	(EDateEdit	*dedit)
{
	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), TRUE);

	return dedit->priv->use_24_hour_format;
}


void
e_date_edit_set_use_24_hour_format	(EDateEdit	*dedit,
					 gboolean	 use_24_hour_format)
{
	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	if (dedit->priv->use_24_hour_format == use_24_hour_format)
		return;

	dedit->priv->use_24_hour_format = use_24_hour_format;

	rebuild_time_popup (dedit);

	e_date_edit_update_time_entry (dedit);
}


/* Whether we allow the date to be set to 'None'. e_date_edit_get_time() will
   return (time_t) -1 in this case. */
gboolean
e_date_edit_get_allow_no_date_set	(EDateEdit	*dedit)
{
	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), FALSE);

	return GTK_WIDGET_VISIBLE (dedit->priv->none_button);
}


void
e_date_edit_set_allow_no_date_set	(EDateEdit	*dedit,
					 gboolean	 allow_no_date_set)
{
	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	if (allow_no_date_set) {
		gtk_widget_show (dedit->priv->none_button);
	} else {
		gtk_widget_hide (dedit->priv->none_button);

		/* If the date is showing, we make sure it isn't 'None' (we
		   don't really mind if the time is empty), else if just the
		   time is showing we make sure it isn't 'None'. */
		if (dedit->priv->show_date) {
			if (dedit->priv->date_set_to_none)
				e_date_edit_set_time (dedit, time (NULL));
		} else {
			if (dedit->priv->time_set_to_none)
				e_date_edit_set_time (dedit, time (NULL));
		}
	}
}


/* The range of time to show in the time combo popup. */
void
e_date_edit_get_time_popup_range	(EDateEdit	*dedit,
					 gint		*lower_hour,
					 gint		*upper_hour)
{
	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	*lower_hour = dedit->priv->lower_hour;
	*upper_hour = dedit->priv->upper_hour;
}


void
e_date_edit_set_time_popup_range	(EDateEdit	*dedit,
					 gint		 lower_hour,
					 gint		 upper_hour)
{
	EDateEditPrivate *priv;

	g_return_if_fail (E_IS_DATE_EDIT (dedit));

	priv = dedit->priv;

	if (priv->lower_hour == lower_hour
	    && priv->upper_hour == upper_hour)
		return;

	priv->lower_hour = lower_hour;
	priv->upper_hour = upper_hour;

	rebuild_time_popup (dedit);

	/* Setting the combo list items seems to mess up the time entry, so
	   we set it again. We have to reset it to its last valid time. */
	priv->time_is_valid = TRUE;
	e_date_edit_update_time_entry (dedit);
}


/* The arrow button beside the date field has been clicked, so we show the
   popup with the ECalendar in. */
static void
on_date_button_clicked		(GtkWidget	*widget,
				 EDateEdit	*dedit)
{
	e_date_edit_show_date_popup (dedit);
}


static void
e_date_edit_show_date_popup	(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	ECalendar *calendar;
	struct tm mtm;
	gchar *date_text;
	GDate selected_day;
	gboolean clear_selection = FALSE;

	priv = dedit->priv;
	calendar = E_CALENDAR (priv->calendar);

	date_text = gtk_entry_get_text (GTK_ENTRY (priv->date_entry));
	if (field_set_to_none (date_text)
	    || !e_date_edit_parse_date (dedit, date_text, &mtm))
		clear_selection = TRUE;

	if (clear_selection) {
		e_calendar_item_set_selection (calendar->calitem, NULL, NULL);
	} else {
		g_date_clear (&selected_day, 1);
		g_date_set_dmy (&selected_day, mtm.tm_mday, mtm.tm_mon + 1,
				mtm.tm_year + 1900);
		e_calendar_item_set_selection (calendar->calitem,
					       &selected_day, NULL);
	}

	/* FIXME: Hack. Change ECalendarItem so it doesn't queue signal
	   emissions. */
	calendar->calitem->selection_changed = FALSE;

        position_date_popup (dedit);
	gtk_widget_show (priv->cal_popup);
	gtk_widget_grab_focus (priv->cal_popup);
	gtk_grab_add (priv->cal_popup);
	gdk_pointer_grab (priv->cal_popup->window, TRUE,
			  (GDK_BUTTON_PRESS_MASK
			   | GDK_BUTTON_RELEASE_MASK
			   | GDK_POINTER_MOTION_MASK),
			  NULL, NULL, GDK_CURRENT_TIME);
}


/* This positions the date popup below and to the left of the arrow button,
   just before it is shown. */
static void
position_date_popup		(EDateEdit	*dedit)
{
	gint x, y;
	gint bwidth, bheight;
	GtkRequisition req;
	gint screen_width, screen_height;

	gtk_widget_size_request (dedit->priv->cal_popup, &req);

	gdk_window_get_origin (dedit->priv->date_button->window, &x, &y);
	gdk_window_get_size (dedit->priv->date_button->window,
			     &bwidth, &bheight);

      	screen_width = gdk_screen_width ();
	screen_height = gdk_screen_height ();

	x += bwidth - req.width;
	y += bheight;

	x = CLAMP (x, 0, MAX (0, screen_width - req.width));
	y = CLAMP (y, 0, MAX (0, screen_height - req.height));

	gtk_widget_set_uposition (dedit->priv->cal_popup, x, y);
}


/* A date has been selected in the date popup, so we set the date field
   and hide the popup. */
static void
on_date_popup_date_selected	(ECalendarItem	*calitem,
				 EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	GDate start_date, end_date;

	priv = dedit->priv;

	hide_date_popup (dedit);

	if (!e_calendar_item_get_selection (calitem, &start_date, &end_date))
		return;

	e_date_edit_set_date (dedit, g_date_year (&start_date),
			      g_date_month (&start_date),
			      g_date_day (&start_date));
}


static void
on_date_popup_now_button_clicked	(GtkWidget	*button,
					 EDateEdit	*dedit)
{
	hide_date_popup (dedit);
	e_date_edit_set_time (dedit, time (NULL));
}


static void
on_date_popup_today_button_clicked	(GtkWidget	*button,
					 EDateEdit	*dedit)
{
	struct tm *tmp_tm;
	time_t t;

	hide_date_popup (dedit);

	t = time (NULL);
	tmp_tm = localtime (&t);

	e_date_edit_set_date (dedit, tmp_tm->tm_year + 1900,
			      tmp_tm->tm_mon + 1, tmp_tm->tm_mday);
}


static void
on_date_popup_none_button_clicked	(GtkWidget	*button,
					 EDateEdit	*dedit)
{
	hide_date_popup (dedit);
	e_date_edit_set_time (dedit, -1);
}


/* A key has been pressed while the date popup is showing. If it is the Escape
   key we hide the popup. */
static gint
on_date_popup_key_press			(GtkWidget	*widget,
					 GdkEventKey	*event,
					 EDateEdit	*dedit)
{
	if (event->keyval != GDK_Escape)
		return FALSE;

	gtk_signal_emit_stop_by_name (GTK_OBJECT (widget), "key_press_event");
	hide_date_popup (dedit);

	return TRUE;
}


/* A mouse button has been pressed while the date popup is showing.
   Any button press events used to select days etc. in the popup will have
   have been handled elsewhere, so here we just hide the popup.
   (This function is yanked from gtkcombo.c) */
static gint
on_date_popup_button_press	(GtkWidget	*widget,
				 GdkEventButton *event,
				 gpointer	 data)
{
	EDateEdit *dedit;
	GtkWidget *child;

	dedit = data;

	child = gtk_get_event_widget ((GdkEvent *) event);

	/* We don't ask for button press events on the grab widget, so
	 *  if an event is reported directly to the grab widget, it must
	 *  be on a window outside the application (and thus we remove
	 *  the popup window). Otherwise, we check if the widget is a child
	 *  of the grab widget, and only remove the popup window if it
	 *  is not.
	 */
	if (child != widget) {
		while (child) {
			if (child == widget)
				return FALSE;
			child = child->parent;
		}
	}

	hide_date_popup (dedit);

	return TRUE;
}


/* A delete event has been received for the date popup, so we hide it and
   return TRUE so it doesn't get destroyed. */
static gint
on_date_popup_delete_event	(GtkWidget	*widget,
				 EDateEdit	*dedit)
{
	hide_date_popup (dedit);
	return TRUE;
}


/* Hides the date popup, removing any grabs. */
static void
hide_date_popup			(EDateEdit	*dedit)
{
	gtk_widget_hide (dedit->priv->cal_popup);
	gtk_grab_remove (dedit->priv->cal_popup);
	gdk_pointer_ungrab (GDK_CURRENT_TIME);
}


/* Clears the time popup and rebuilds it using the lower_hour, upper_hour
   and use_24_hour_format settings. */
static void
rebuild_time_popup			(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	GtkList *list;
	GtkWidget *listitem;
	char buffer[40], *format;
	struct tm tmp_tm;
	gint hour, min;

	priv = dedit->priv;

	list = GTK_LIST (GTK_COMBO (priv->time_combo)->list);

	gtk_list_clear_items (list, 0, -1);

	/* Fill the struct tm with some sane values. */
	tmp_tm.tm_year = 2000;
	tmp_tm.tm_mon = 0;
	tmp_tm.tm_mday = 1;
	tmp_tm.tm_sec  = 0;
	tmp_tm.tm_isdst = 0;

	for (hour = priv->lower_hour; hour <= priv->upper_hour; hour++) {

		/* We don't want to display midnight at the end, since that is
		   really in the next day. */
		if (hour == 24)
			break;

		/* We want to finish on upper_hour, with min == 0. */
		for (min = 0;
		     min == 0 || (min < 60 && hour != priv->upper_hour);
		     min += 30) {
			tmp_tm.tm_hour = hour;
			tmp_tm.tm_min  = min;

			if (priv->use_24_hour_format)
				/* This is a strftime() format. %H = hour (0-23), %M = minute. */
				format = _("%H:%M");
			else
				/* This is a strftime() format. %I = hour (1-12), %M = minute, %p = am/pm string. */
				format = _("%I:%M %p");

			strftime (buffer, sizeof (buffer), format, &tmp_tm);

			listitem = gtk_list_item_new_with_label (buffer);
			gtk_widget_show (listitem);
			gtk_container_add (GTK_CONTAINER (list), listitem);
		}
	}
}


static gboolean
e_date_edit_parse_date (EDateEdit *dedit,
			gchar	  *date_text,
			struct tm *date_tm)
{
	struct tm *tmp_tm;
	time_t t;

	/* This is a strptime() format for a short date. %m = month,
	   %d = day of month, %Y = year (all digits). */
	if (!strptime (date_text, _("%m/%d/%Y"), date_tm))
		return FALSE;

	/* If the user entered a 2-digit year we use the current century. */
	if (date_tm->tm_year < 0) {
		t = time (NULL);
		tmp_tm = localtime (&t);

		/* This should convert it into a value from 0 to 99. */
		date_tm->tm_year += 1900;

		/* Now add on the century. */
		date_tm->tm_year += tmp_tm->tm_year - (tmp_tm->tm_year % 100);
	}

	return TRUE;
}


static gboolean
e_date_edit_parse_time	(EDateEdit	*dedit,
			 gchar		*time_text,
			 struct tm	*time_tm)
{
	gchar *format;

	if (field_set_to_none (time_text)) {
		time_tm->tm_hour = 0;
		time_tm->tm_min = 0;
		return TRUE;
	}

	if (dedit->priv->use_24_hour_format)
		/* This is a strptime() format. %H = hour (0-23), %M = minute. */
		format = _("%H:%M");
	else
		/* This is a strptime() format. %I = hour (1-12), %M = minute, %p = am/pm string. */
		format = _("%I:%M %p");

	if (!strptime (time_text, format, time_tm))
		return FALSE;

	return TRUE;
}


/* Returns TRUE if the string is empty or is "None" in the current locale.
   It ignores whitespace. */
static gboolean
field_set_to_none		(char		*text)
{
	char *pos, *none_string;

	pos = text;
	while (isspace (*pos))
		pos++;

	none_string = _("None");

	if (*pos == '\0' || !strncmp (pos, none_string, strlen (none_string)))
		return TRUE;
	return FALSE;
}


static void
on_date_edit_time_selected	(GtkList	*list,
				 EDateEdit	*dedit)
{
	GtkWidget *toplevel;

	/* We only want to emit signals when an item is selected explicitly,
	   not when it is selected by the silly combo update thing. */
	if (!list->selection)
		return;

	toplevel = gtk_widget_get_toplevel (GTK_WIDGET (list));
	if (!GTK_WIDGET_MAPPED (toplevel))
		return;

	e_date_edit_check_time_changed (dedit);
}


static gint
on_date_entry_key_press			(GtkWidget	*widget,
					 GdkEventKey	*event,
					 EDateEdit	*dedit)
{
	if (event->state & GDK_MOD1_MASK
	    && (event->keyval == GDK_Up || event->keyval == GDK_Down
		|| event->keyval == GDK_Return)) {
		gtk_signal_emit_stop_by_name (GTK_OBJECT (widget),
					      "key_press_event");
		e_date_edit_show_date_popup (dedit);
		return TRUE;
	}

	/* If the user hits the return key emit a "date_changed" signal if
	   needed. But let the signal carry on. */
	if (event->keyval == GDK_Return) {
		e_date_edit_check_date_changed (dedit);
		return FALSE;
	}

	return FALSE;
}


static gint
on_time_entry_key_press			(GtkWidget	*widget,
					 GdkEventKey	*event,
					 EDateEdit	*dedit)
{
	/* I'd like to use Alt+Up/Down for popping up the list, like Win32,
	   but the combo steals any Up/Down keys, so we use Alt+Return. */
#if 0	   
	if (event->state & GDK_MOD1_MASK
	    && (event->keyval == GDK_Up || event->keyval == GDK_Down)) {
#else
	if (event->state & GDK_MOD1_MASK && event->keyval == GDK_Return) {
#endif
		gtk_signal_emit_stop_by_name (GTK_OBJECT (widget),
					      "key_press_event");
		gtk_signal_emit_by_name (GTK_OBJECT (GTK_COMBO (dedit->priv->time_combo)->entry), "activate");
		return TRUE;
	}

	/* Stop the return key from emitting the activate signal, and check
	   if we need to emit a "time_changed" signal. */
	if (event->keyval == GDK_Return) {
		gtk_signal_emit_stop_by_name (GTK_OBJECT (widget),
					      "key_press_event");
		e_date_edit_check_time_changed (dedit);
		return TRUE;
	}

	return FALSE;
}


static gint
on_date_entry_focus_out			(GtkEntry	*entry,
					 GdkEventFocus  *event,
					 EDateEdit	*dedit)
{
	e_date_edit_check_date_changed (dedit);
	return FALSE;
}


static gint
on_time_entry_focus_out			(GtkEntry	*entry,
					 GdkEventFocus  *event,
					 EDateEdit	*dedit)
{
	e_date_edit_check_time_changed (dedit);
	return FALSE;
}


/* This sets the text in the date entry according to the current settings. */
static void
e_date_edit_update_date_entry		(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	char buffer[40];
	struct tm tmp_tm = { 0 };

	priv = dedit->priv;

	if (priv->date_set_to_none || !priv->date_is_valid) {
		gtk_entry_set_text (GTK_ENTRY (priv->date_entry), _("None"));
	} else {
		tmp_tm.tm_year = priv->year;
		tmp_tm.tm_mon = priv->month;
		tmp_tm.tm_mday = priv->day;
		tmp_tm.tm_isdst = -1;

		/* This is a strftime() format for a short date. %m = month,
		   %d = day of month, %Y = year (all digits). */
		strftime (buffer, sizeof (buffer), _("%m/%d/%Y"), &tmp_tm);
		gtk_entry_set_text (GTK_ENTRY (priv->date_entry), buffer);
	}
}


/* This sets the text in the time entry according to the current settings. */
static void
e_date_edit_update_time_entry		(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	char buffer[40], *format;
	struct tm tmp_tm = { 0 };

	priv = dedit->priv;

	if (priv->time_set_to_none || !priv->time_is_valid) {
		gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (priv->time_combo)->entry), "");
	} else {
		/* Set these to reasonable values just in case. */
		tmp_tm.tm_year = 2000;
		tmp_tm.tm_mon = 0;
		tmp_tm.tm_mday = 1;

		tmp_tm.tm_hour = priv->hour;
		tmp_tm.tm_min = priv->minute;

		tmp_tm.tm_sec = 0;
		tmp_tm.tm_isdst = -1;

		if (priv->use_24_hour_format)
			/* This is a strftime() format. %H = hour (0-23), %M = minute. */
			format = _("%H:%M");
		else
			/* This is a strftime() format. %I = hour (1-12), %M = minute, %p = am/pm string. */
			format = _("%I:%M %p");

		strftime (buffer, sizeof (buffer), format, &tmp_tm);
		gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (priv->time_combo)->entry),
				    buffer);
	}
}


static void
e_date_edit_update_time_combo_state	(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	gchar *text;

	priv = dedit->priv;

	/* If the date entry is currently shown, and it is set to None,
	   clear the time entry and disable the time combo, else enable it. */
	if (priv->show_date && priv->date_set_to_none) {
		text = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (priv->time_combo)->entry));
		if (text[0])
			gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (priv->time_combo)->entry), "");
		gtk_widget_set_sensitive (priv->time_combo, FALSE);
	} else {
		gtk_widget_set_sensitive (priv->time_combo, TRUE);
	}
}


/* Parses the date, and if it is different from the current settings it
   updates the settings and emits a "date_changed" signal. */
static void
e_date_edit_check_date_changed		(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	gchar *date_text;
	struct tm tmp_tm;
	gboolean none = FALSE, valid = TRUE, date_changed;

	priv = dedit->priv;

	date_text = gtk_entry_get_text (GTK_ENTRY (priv->date_entry));
	if (field_set_to_none (date_text))
		none = TRUE;
	else if (!e_date_edit_parse_date (dedit, date_text, &tmp_tm))
		valid = FALSE;

	date_changed = e_date_edit_set_date_internal (dedit, valid, none,
						      tmp_tm.tm_year,
						      tmp_tm.tm_mon,
						      tmp_tm.tm_mday);

	if (date_changed)
		gtk_signal_emit (GTK_OBJECT (dedit),
				 date_edit_signals [CHANGED]);
}


/* Parses the time, and if it is different from the current settings it
   updates the settings and emits a "time_changed" signal. */
static void
e_date_edit_check_time_changed		(EDateEdit	*dedit)
{
	EDateEditPrivate *priv;
	gchar *time_text;
	struct tm tmp_tm;
	gboolean none = FALSE, valid = TRUE, time_changed;

	priv = dedit->priv;

	time_text = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (priv->time_combo)->entry));
	if (field_set_to_none (time_text))
		none = TRUE;
	else if (!e_date_edit_parse_time (dedit, time_text, &tmp_tm))
		valid = FALSE;

	time_changed = e_date_edit_set_time_internal (dedit, valid, none,
						      tmp_tm.tm_hour,
						      tmp_tm.tm_min);

	if (time_changed)
		gtk_signal_emit (GTK_OBJECT (dedit),
				 date_edit_signals [CHANGED]);
}


/**
 * e_date_edit_date_is_valid:
 * @dedit: an #EDateEdit widget.
 * @Returns: TRUE if the last date entered was valid.
 *
 * Returns TRUE if the last date entered was valid.
 *
 * Note that if this returns FALSE, you can still use e_date_edit_get_time()
 * or e_date_edit_get_date() to get the last time or date entered which was
 * valid.
 */
gboolean
e_date_edit_date_is_valid	(EDateEdit	*dedit)
{
	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), FALSE);

	if (!dedit->priv->date_is_valid)
		return FALSE;

	/* If the date is empty/None and that isn't permitted, return FALSE. */
	if (dedit->priv->date_set_to_none
	    && !e_date_edit_get_allow_no_date_set (dedit))
		return FALSE;

	return TRUE;
}


/**
 * e_date_edit_time_is_valid:
 * @dedit: an #EDateEdit widget.
 * @Returns: TRUE if the last time entered was valid.
 *
 * Returns TRUE if the last time entered was valid.
 *
 * Note that if this returns FALSE, you can still use e_date_edit_get_time()
 * or e_date_edit_get_time_of_day() to get the last time or time of the day
 * entered which was valid.
 */
gboolean
e_date_edit_time_is_valid	(EDateEdit	*dedit)
{
	g_return_val_if_fail (E_IS_DATE_EDIT (dedit), FALSE);

	if (!dedit->priv->time_is_valid)
		return FALSE;

	/* If the time is empty and that isn't permitted, return FALSE.
	   Note that we don't mind an empty time if the date field is shown
	   - in that case we just assume 0:00. */
	if (dedit->priv->time_set_to_none && !dedit->priv->show_date
	    && !e_date_edit_get_allow_no_date_set (dedit))
		return FALSE;

	return TRUE;
}


static gboolean
e_date_edit_set_date_internal	(EDateEdit	*dedit,
				 gboolean	 valid,
				 gboolean	 none,
				 gint		 year,
				 gint		 month,
				 gint		 day)
{
	EDateEditPrivate *priv;
	gboolean date_changed = FALSE;

	priv = dedit->priv;

	if (!valid) {
		/* Date is invalid. */
		if (priv->date_is_valid) {
			priv->date_is_valid = FALSE;
			date_changed = TRUE;
		}
	} else if (none) {
		/* Date has been set to 'None'. */
		if (!priv->date_is_valid
		    || !priv->date_set_to_none) {
			priv->date_is_valid = TRUE;
			priv->date_set_to_none = TRUE;
			date_changed = TRUE;
		}
	} else {
		/* Date has been set to a specific date. */
		if (!priv->date_is_valid
		    || priv->date_set_to_none
		    || priv->year != year
		    || priv->month != month
		    || priv->day != day) {
			priv->date_is_valid = TRUE;
			priv->date_set_to_none = FALSE;
			priv->year = year;
			priv->month = month;
			priv->day = day;
			date_changed = TRUE;
		}
	}

	return date_changed;
}


static gboolean
e_date_edit_set_time_internal	(EDateEdit	*dedit,
				 gboolean	 valid,
				 gboolean	 none,
				 gint		 hour,
				 gint		 minute)
{
	EDateEditPrivate *priv;
	gboolean time_changed = FALSE;

	priv = dedit->priv;

	if (!valid) {
		/* Time is invalid. */
		if (priv->time_is_valid) {
			priv->time_is_valid = FALSE;
			time_changed = TRUE;
		}
	} else if (none) {
		/* Time has been set to empty/'None'. */
		if (!priv->time_is_valid
		    || !priv->time_set_to_none) {
			priv->time_is_valid = TRUE;
			priv->time_set_to_none = TRUE;
			time_changed = TRUE;
		}
	} else {
		/* Time has been set to a specific time. */
		if (!priv->time_is_valid
		    || priv->time_set_to_none
		    || priv->hour != hour
		    || priv->minute != minute) {
			priv->time_is_valid = TRUE;
			priv->time_set_to_none = FALSE;
			priv->hour = hour;
			priv->minute = minute;
			time_changed = TRUE;
		}
	}

	return time_changed;
}
