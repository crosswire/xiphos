/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* e-splash.h
 *
 * Copyright (C) 2000  Helix Code, Inc.
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

#ifndef _E_SPLASH_H_
#define _E_SPLASH_H_

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtkwindow.h>

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

#define E_TYPE_SPLASH			(e_splash_get_type ())
#define E_SPLASH(obj)			(GTK_CHECK_CAST ((obj), E_TYPE_SPLASH, ESplash))
#define E_SPLASH_CLASS(klass)		(GTK_CHECK_CLASS_CAST ((klass), E_TYPE_SPLASH, ESplashClass))
#define E_IS_SPLASH(obj)			(GTK_CHECK_TYPE ((obj), E_TYPE_SPLASH))
#define E_IS_SPLASH_CLASS(klass)		(GTK_CHECK_CLASS_TYPE ((obj), E_TYPE_SPLASH))


typedef struct _ESplash        ESplash;
typedef struct _ESplashPrivate ESplashPrivate;
typedef struct _ESplashClass   ESplashClass;

struct _ESplash {
	GtkWindow parent;

	ESplashPrivate *priv;
};

struct _ESplashClass {
	GtkWindowClass parent_class;
};


GtkType    e_splash_get_type            (void);
void       e_splash_construct           (ESplash   *splash,
					 GdkPixbuf *splash_image);
GtkWidget *e_splash_new                 (void);

int        e_splash_add_icon            (ESplash   *splash,
					 GdkPixbuf *icon);
void       e_splash_set_icon_highlight  (ESplash   *splash,
					 int        num,
					 gboolean   highlight);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_SPLASH_H_ */
