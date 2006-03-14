/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * e-colors.c - General color allocation utilities
 * Copyright 2000, 2001, Ximian, Inc.
 *
 * Authors:
 *  Miguel de Icaza (miguel@kernel.org)
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

/* We keep our own color context, as the color allocation might take
 * place before things are realized.
 */

#include <config.h>
#include <gdk/gdkrgb.h>
#include <gtk/gtkwidget.h>
#include "gi-colors.h"

GdkColor e_white, e_dark_gray, e_black;

gulong
e_color_alloc (gushort red, gushort green, gushort blue)
{
	e_color_init ();

	red >>= 8;
	green >>= 8;
	blue >>= 8;
	return gdk_rgb_xpixel_from_rgb (
		((red & 0xff) << 16) | ((green & 0xff) << 8) |
		(blue & 0xff));
}

void
e_color_alloc_gdk (GtkWidget *widget, GdkColor *c)
{
	GdkColormap *map;

	e_color_init ();

	if (widget)
		map = gtk_widget_get_colormap (widget);
	else /* FIXME: multi depth broken ? */
		map = gtk_widget_get_default_colormap ();

	gdk_rgb_find_color (map, c);
}

void
e_color_alloc_name (GtkWidget *widget, const char *name, GdkColor *c)
{
	GdkColormap *map;

	e_color_init ();

	gdk_color_parse (name, c);

	if (widget)
		map = gtk_widget_get_colormap (widget);
	else /* FIXME: multi depth broken ? */
		map = gtk_widget_get_default_colormap ();

	gdk_rgb_find_color (map, c);
}

void
e_color_init (void)
{
	static gboolean e_color_inited = FALSE;

	/* It's surprisingly easy to end up calling this twice.  Survive.  */
	if (e_color_inited)
		return;

	e_color_inited = TRUE;

	/* Allocate the default colors */
	e_white.red   = 65535;
	e_white.green = 65535;
	e_white.blue  = 65535;
	e_color_alloc_gdk (NULL, &e_white);

	e_black.red   = 0;
	e_black.green = 0;
	e_black.blue  = 0;
	e_color_alloc_gdk (NULL, &e_black);

	e_color_alloc_name (NULL, "gray20",  &e_dark_gray);
}

