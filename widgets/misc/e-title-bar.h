/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* e-title-bar.h
 *
 * Copyright (C) 2000 Helix Code, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Ettore Perazzoli
 */

#ifndef __E_TITLE_BAR_H__
#define __E_TITLE_BAR_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtkframe.h>

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

#define E_TYPE_TITLE_BAR			(e_title_bar_get_type ())
#define E_TITLE_BAR(obj)			(GTK_CHECK_CAST ((obj), E_TYPE_TITLE_BAR, ETitleBar))
#define E_TITLE_BAR_CLASS(klass)		(GTK_CHECK_CLASS_CAST ((klass), E_TYPE_TITLE_BAR, ETitleBarClass))
#define E_IS_TITLE_BAR(obj)			(GTK_CHECK_TYPE ((obj), E_TYPE_TITLE_BAR))
#define E_IS_TITLE_BAR_CLASS(klass)		(GTK_CHECK_CLASS_TYPE ((obj), E_TYPE_TITLE_BAR))


typedef struct _ETitleBar        ETitleBar;
typedef struct _ETitleBarPrivate ETitleBarPrivate;
typedef struct _ETitleBarClass   ETitleBarClass;

struct _ETitleBar {
	GtkFrame parent;

	ETitleBarPrivate *priv;
};

struct _ETitleBarClass {
	GtkFrameClass parent_class;

	/* Signals.  */

	void (* title_button_press_event) (ETitleBar *title_bar, GdkEventButton *event);
	void (* close_button_clicked)     (ETitleBar *title_bar);
};


GtkType    e_title_bar_get_type   (void);
void       e_title_bar_construct  (ETitleBar  *title_bar,
				   const char *title);
GtkWidget *e_title_bar_new        (const char *title);

void  e_title_bar_set_title          (ETitleBar  *title_bar,
				      const char *title);
void  e_title_bar_show_close_button  (ETitleBar  *title_bar,
				      gboolean    show);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __E_TITLE_BAR_H__ */
