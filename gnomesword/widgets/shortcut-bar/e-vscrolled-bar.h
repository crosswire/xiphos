/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* 
 * Author : 
 *  Damon Chaplin <damon@helixcode.com>
 *
 * Copyright 1999, Helix Code, Inc.
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
#ifndef _E_VSCROLLED_BAR_H_
#define _E_VSCROLLED_BAR_H_

#include <gtk/gtkbin.h>
#include <gtk/gtkadjustment.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * EVScrolledBar is like GtkScrolledWindow but only scrolls the child widget
 * vertically. It is intended for scrolling narrow vertical bars.
 */


#define E_VSCROLLED_BAR(obj)          GTK_CHECK_CAST (obj, e_vscrolled_bar_get_type (), EVScrolledBar)
#define E_VSCROLLED_BAR_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, e_vscrolled_bar_get_type (), EVScrolledBarClass)
#define E_IS_VSCROLLED_BAR(obj)       GTK_CHECK_TYPE (obj, e_vscrolled_bar_get_type ())


typedef struct _EVScrolledBar       EVScrolledBar;
typedef struct _EVScrolledBarClass  EVScrolledBarClass;

struct _EVScrolledBar
{
	GtkBin bin;

	GtkWidget *up_button;
	GtkWidget *down_button;

	GtkAdjustment *adjustment;

	gint up_button_width;
	gint up_button_height;
	gint down_button_width;
	gint down_button_height;

	/* The GTK+ event source ID of our timeout handler. */
	gint timeout_id;

	/* TRUE if we are scrolling up, FALSE if scrolling down. */
	gboolean scrolling_up;

	/* The minimum distance left to scroll. If the user just clicks a
	   button we scroll a minimum amount. This is reduced after each
	   scroll. */
	gfloat min_distance;

	/* TRUE if the button is still pressed. When the up/down button is
	   released, this gets set to FALSE, and we scroll until the minimum
	   distance falls below 0. */
	gboolean button_pressed;
};

struct _EVScrolledBarClass
{
	GtkBinClass parent_class;
};


GtkType	       e_vscrolled_bar_get_type		(void);
GtkWidget*     e_vscrolled_bar_new		(GtkAdjustment *adjustment);

GtkAdjustment* e_vscrolled_bar_get_adjustment   (EVScrolledBar *vscrolled_bar);
void           e_vscrolled_bar_set_adjustment   (EVScrolledBar *vscrolled_bar,
						 GtkAdjustment *adjustment);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_VSCROLLED_BAR_H_ */
