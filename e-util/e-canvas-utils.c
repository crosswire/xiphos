/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * e-canvas-utils.c
 * Copyright (C) 2000  Helix Code, Inc.
 * Author: Chris Lahey <clahey@helixcode.com>
 *
 * This library is free software; you can redistribute it and/or
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
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "e-canvas-utils.h"

void
e_canvas_item_move_absolute (GnomeCanvasItem *item, double dx, double dy)
{
	double translate[6];

	g_return_if_fail (item != NULL);
	g_return_if_fail (GNOME_IS_CANVAS_ITEM (item));

	art_affine_translate (translate, dx, dy);

	gnome_canvas_item_affine_absolute (item, translate);
}

void
e_canvas_item_show_area (GnomeCanvasItem *item, double x1, double y1, double x2, double y2)
{
	GtkAdjustment *h, *v;
	double dx = 0, dy = 0;
	
	g_return_if_fail (item != NULL);
	g_return_if_fail (GNOME_IS_CANVAS_ITEM (item));
	
	gnome_canvas_item_i2w(item, &x1, &y1);
	gnome_canvas_item_i2w(item, &x2, &y2);

	h = gtk_layout_get_hadjustment(GTK_LAYOUT(item->canvas));
	v = gtk_layout_get_vadjustment(GTK_LAYOUT(item->canvas));

	if (x2 > h->value + h->page_size)
		dx = (x2 - (h->value + h->page_size));
	if (y2 > v->value + v->page_size)
		dy = (y2 - (v->value + v->page_size));

	if (x1 < h->value + dx)
		dx = (x1 - h->value);
	if (y1 < v->value + dy)
		dy = (y1 - v->value);

	if (dx)
		gtk_adjustment_set_value(h, h->value + dx);
	if (dy)
		gtk_adjustment_set_value(v, v->value + dy);
}
