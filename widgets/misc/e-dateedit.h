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
 *
 * It emits a "changed" signal when the date and/or time has changed.
 * You can check if the last date or time entered was invalid by
 * calling e_date_edit_date_is_valid() and e_date_edit_time_is_valid().
 *
 * Note that when the user types in a date or time, it will only emit the
 * signals when the user presses the return key or switches the keyboard
 * focus to another widget, or you call one of the _get_time/date functions.
 */

#ifndef __E_DATE_EDIT_H_
#define __E_DATE_EDIT_H_ 

#include <time.h>
#include <gtk/gtkhbox.h>
#include <libgnome/gnome-defs.h>
 
BEGIN_GNOME_DECLS


#define E_TYPE_DATE_EDIT            (e_date_edit_get_type ())
#define E_DATE_EDIT(obj)            (GTK_CHECK_CAST ((obj), E_TYPE_DATE_EDIT, EDateEdit))
#define E_DATE_EDIT_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), E_TYPE_DATE_EDIT, EDateEditClass))
#define E_IS_DATE_EDIT(obj)         (GTK_CHECK_TYPE ((obj), E_TYPE_DATE_EDIT))
#define E_IS_DATE_EDIT_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), E_TYPE_DATE_EDIT))


typedef struct _EDateEdit        EDateEdit;
typedef struct _EDateEditPrivate EDateEditPrivate;
typedef struct _EDateEditClass   EDateEditClass;

struct _EDateEdit {
	GtkHBox hbox;

	/*< private >*/
	EDateEditPrivate *priv;
};

struct _EDateEditClass {
	GtkHBoxClass parent_class;

	void (* changed) (EDateEdit *dedit);
};

guint      e_date_edit_get_type			(void);
GtkWidget* e_date_edit_new			(void);

/* Returns TRUE if the last date and time set were valid. The date and time
   are only set when the user hits Return or switches keyboard focus, or
   selects a date or time from the popup. */
gboolean   e_date_edit_date_is_valid		(EDateEdit	*dedit);
gboolean   e_date_edit_time_is_valid		(EDateEdit	*dedit);

/* Returns the last valid date & time set, or -1 if the date & time was set to
   'None' and this is permitted via e_date_edit_set_allow_no_date_set. */
time_t	   e_date_edit_get_time			(EDateEdit	*dedit);
void       e_date_edit_set_time			(EDateEdit	*dedit,
						 time_t		 the_time);

/* This returns the last valid date set, without the time. It returns TRUE
   if a date is set, or FALSE if the date is set to 'None' and this is
   permitted via e_date_edit_set_allow_no_date_set. */
gboolean   e_date_edit_get_date			(EDateEdit	*dedit,
						 gint		*year,
						 gint		*month,
						 gint		*day);
void	   e_date_edit_set_date			(EDateEdit	*dedit,
						 gint		 year,
						 gint		 month,
						 gint		 day);

/* This returns the last valid time set, without the date. It returns TRUE
   if a time is set, or FALSE if the time is set to 'None' and this is
   permitted via e_date_edit_set_allow_no_date_set. */
gboolean   e_date_edit_get_time_of_day		(EDateEdit	*dedit,
						 gint		*hour,
						 gint		*minute);
void       e_date_edit_set_time_of_day		(EDateEdit	*dedit,
						 gint		 hour,
						 gint		 minute);

/* Whether we show the date field. */
gboolean   e_date_edit_get_show_date		(EDateEdit	*dedit);
void       e_date_edit_set_show_date		(EDateEdit	*dedit,
						 gboolean	 show_date);

/* Whether we show the time field. */
gboolean   e_date_edit_get_show_time		(EDateEdit	*dedit);
void       e_date_edit_set_show_time		(EDateEdit	*dedit,
						 gboolean	 show_time);

/* The week start day, used in the date popup. 0 (Mon) to 6 (Sun). */
gint	   e_date_edit_get_week_start_day	(EDateEdit	*dedit);
void       e_date_edit_set_week_start_day	(EDateEdit	*dedit,
						 gint		 week_start_day);

/* Whether we show week numbers in the date popup. */
gboolean   e_date_edit_get_show_week_numbers	(EDateEdit	*dedit);
void       e_date_edit_set_show_week_numbers	(EDateEdit	*dedit,
						 gboolean	 show_week_numbers);

/* Whether we use 24 hour format in the time field & popup. */
gboolean   e_date_edit_get_use_24_hour_format	(EDateEdit	*dedit);
void       e_date_edit_set_use_24_hour_format	(EDateEdit	*dedit,
						 gboolean	 use_24_hour_format);

/* Whether we allow the date to be set to 'None'. e_date_edit_get_time() will
   return (time_t) -1 in this case. */
gboolean   e_date_edit_get_allow_no_date_set	(EDateEdit	*dedit);
void       e_date_edit_set_allow_no_date_set	(EDateEdit	*dedit,
						 gboolean	 allow_no_date_set);

/* The range of time to show in the time combo popup. */
void	   e_date_edit_get_time_popup_range	(EDateEdit	*dedit,
						 gint		*lower_hour,
						 gint		*upper_hour);
void       e_date_edit_set_time_popup_range	(EDateEdit	*dedit,
						 gint		 lower_hour,
						 gint		 upper_hour);

END_GNOME_DECLS

#endif
