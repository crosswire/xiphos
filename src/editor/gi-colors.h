/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * e-colors.h
 * Copyright 2000, 2001, Ximian, Inc.
 *
 * Authors:
 *   Miguel de Icaza (miguel@kernel.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License, version 2, as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef GNOME_APP_LIBS_COLOR_H
#define GNOME_APP_LIBS_COLOR_H

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

void     e_color_init       (void);

/* Return the pixel value for the given red, green and blue */
gulong   e_color_alloc      (gushort red, gushort green, gushort blue);
void     e_color_alloc_name (GtkWidget *widget, const char *name, GdkColor *color);
void     e_color_alloc_gdk  (GtkWidget *widget, GdkColor *color);

extern GdkColor e_white, e_dark_gray, e_black;

G_END_DECLS

#endif /* GNOME_APP_LIBS_COLOR_H */
