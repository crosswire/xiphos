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

/*
 * EIconBarBgItem - A GnomeCanvasItem which covers the entire EIconBar.
 * It paints the rectangles around items when the mouse moves over them, and
 * the lines between items when dragging.
 */

#include <config.h>
#include "e-icon-bar-bg-item.h"
#include "e-icon-bar.h"

/* This is the size of the border around the icons, for the shadow. */
#define E_ICON_BAR_LARGE_ICON_SHADOW_BORDER	2
#define E_ICON_BAR_SMALL_ICON_SHADOW_BORDER	2

/* These are for the horzontal bar when dragging. */
#define	E_ICON_BAR_BG_ITEM_BAR_HEIGHT		1
#define	E_ICON_BAR_BG_ITEM_BAR_OFFSET		2
#define E_ICON_BAR_BG_ITEM_LARGE_ARROW_HEIGHT	8
#define E_ICON_BAR_BG_ITEM_SMALL_ARROW_HEIGHT	4

static void e_icon_bar_bg_item_class_init (EIconBarBgItemClass *class);
static void e_icon_bar_bg_item_init (EIconBarBgItem *ibitem);

static void e_icon_bar_bg_item_set_arg (GtkObject *o, GtkArg *arg,
					guint arg_id);
static void e_icon_bar_bg_item_update (GnomeCanvasItem *item,
				       double *affine,
				       ArtSVP *clip_path, int flags);
static void e_icon_bar_bg_item_draw (GnomeCanvasItem *item,
				     GdkDrawable *drawable,
				     int x, int y,
				     int width, int height);
static double e_icon_bar_bg_item_point (GnomeCanvasItem *item,
					double x, double y,
					int cx, int cy,
					GnomeCanvasItem **actual_item);
static gint e_icon_bar_bg_item_event (GnomeCanvasItem *item,
				      GdkEvent *event);
static gint e_icon_bar_bg_item_button_press (EIconBarBgItem *ibitem,
					     GdkEvent *event);
static gint e_icon_bar_bg_item_button_release (EIconBarBgItem *ibitem,
					       GdkEvent *event);
static gint e_icon_bar_bg_item_motion_notify (EIconBarBgItem *ibitem,
					      GdkEvent *event);

static GnomeCanvasItemClass *parent_class;

/* The arguments we take */
enum {
	ARG_0,
	ARG_ICON_BAR
};


GtkType
e_icon_bar_bg_item_get_type (void)
{
	static GtkType e_icon_bar_bg_item_type = 0;

	if (!e_icon_bar_bg_item_type) {
		GtkTypeInfo e_icon_bar_bg_item_info = {
			"EIconBarBgItem",
			sizeof (EIconBarBgItem),
			sizeof (EIconBarBgItemClass),
			(GtkClassInitFunc) e_icon_bar_bg_item_class_init,
			(GtkObjectInitFunc) e_icon_bar_bg_item_init,
			NULL, /* reserved_1 */
			NULL, /* reserved_2 */
			(GtkClassInitFunc) NULL
		};

		e_icon_bar_bg_item_type = gtk_type_unique (gnome_canvas_item_get_type (), &e_icon_bar_bg_item_info);
	}

	return e_icon_bar_bg_item_type;
}


static void
e_icon_bar_bg_item_class_init (EIconBarBgItemClass *class)
{
	GtkObjectClass  *object_class;
	GnomeCanvasItemClass *item_class;

	parent_class = gtk_type_class (gnome_canvas_item_get_type());

	object_class = (GtkObjectClass *) class;
	item_class = (GnomeCanvasItemClass *) class;

	gtk_object_add_arg_type ("EIconBarBgItem::icon_bar",
				 GTK_TYPE_POINTER, GTK_ARG_WRITABLE,
				 ARG_ICON_BAR);

	object_class->set_arg = e_icon_bar_bg_item_set_arg;

	/* GnomeCanvasItem method overrides */
	item_class->update      = e_icon_bar_bg_item_update;
	item_class->draw        = e_icon_bar_bg_item_draw;
	item_class->point       = e_icon_bar_bg_item_point;
	item_class->event       = e_icon_bar_bg_item_event;
}


static void
e_icon_bar_bg_item_init (EIconBarBgItem *ibitem)
{
	GnomeCanvasItem *item = GNOME_CANVAS_ITEM (ibitem);
	
	ibitem->icon_bar = NULL;

	item->x1 = 0;
	item->y1 = 0;
	item->x2 = 0;
	item->y2 = 0;
}


static void
e_icon_bar_bg_item_set_arg (GtkObject *o, GtkArg *arg, guint arg_id)
{
	GnomeCanvasItem *item;
	EIconBarBgItem *ibitem;

	item = GNOME_CANVAS_ITEM (o);
	ibitem = E_ICON_BAR_BG_ITEM (o);
	
	switch (arg_id){
	case ARG_ICON_BAR:
		ibitem->icon_bar = GTK_VALUE_POINTER (*arg);
		break;
	}
}


static void
e_icon_bar_bg_item_update (GnomeCanvasItem *item,
			   double *affine,
			   ArtSVP *clip_path,
			   int flags)
{
	if (GNOME_CANVAS_ITEM_CLASS (parent_class)->update)
		(* GNOME_CANVAS_ITEM_CLASS (parent_class)->update) (item, affine, clip_path, flags);

	/* The grid covers the entire canvas area. */
	item->x1 = 0;
	item->y1 = 0;
	item->x2 = INT_MAX;
	item->y2 = INT_MAX;
}


/*
 * DRAWING ROUTINES - functions to paint the canvas item.
 */

static void
e_icon_bar_bg_item_draw (GnomeCanvasItem *canvas_item, GdkDrawable *drawable,
			 int x, int y, int width, int height)
{
	EIconBar *icon_bar;
	EIconBarItem *item;
	EIconBarBgItem *ibitem;
	GtkStyle *style;
	GdkGC *gc;
	GtkShadowType shadow;
	gint item_num, border, bar_x, bar_y, bar_w, i, arrow_height;

	ibitem = E_ICON_BAR_BG_ITEM (canvas_item);
	icon_bar = ibitem->icon_bar;
	g_return_if_fail (icon_bar != NULL);
	style = GTK_WIDGET (icon_bar)->style;
	shadow = GTK_SHADOW_NONE;

	/* Draw the highlight around the current highlight item. */
	item_num = -1;
	if (icon_bar->editing_item_num == -1) {
		if (icon_bar->pressed_item_num != -1) {
			item_num = icon_bar->pressed_item_num;
			if (icon_bar->pressed_item_num == icon_bar->mouse_over_item_num)
				shadow = GTK_SHADOW_IN;
			else
				shadow = GTK_SHADOW_OUT;
		} else if (icon_bar->mouse_over_item_num != -1) {
			item_num = icon_bar->mouse_over_item_num;
			shadow = GTK_SHADOW_OUT;
		}
	}

	if (item_num != -1) {
		item = &g_array_index (icon_bar->items, EIconBarItem,
				       item_num);

		if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS)
			border = E_ICON_BAR_LARGE_ICON_SHADOW_BORDER;
		else
			border = E_ICON_BAR_SMALL_ICON_SHADOW_BORDER;

		gtk_draw_shadow (style, drawable, GTK_STATE_NORMAL, shadow,
				 icon_bar->icon_x - border - x,
				 item->icon_y - border - y,
				 icon_bar->icon_w + border * 2 - 1,
				 icon_bar->icon_h + border * 2 - 1);
	}

	/* Draw the bar between items when dragging, if needed. */
	if (icon_bar->in_drag && icon_bar->dragging_before_item_num != -1) {
		if (icon_bar->dragging_before_item_num < icon_bar->items->len) {
			item = &g_array_index (icon_bar->items, EIconBarItem,
					       icon_bar->dragging_before_item_num);
			bar_y = 0;
		} else {
			/* We need to draw the bar after the last item. */
			item = &g_array_index (icon_bar->items, EIconBarItem,
					       icon_bar->items->len - 1);
			bar_y = item->item_height + icon_bar->spacing;
		}

		if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS) {
			bar_y += item->icon_y;
		} else {
			bar_y += MIN (item->icon_y, item->text_y);
		}
		bar_y -= y + icon_bar->spacing / 2;

		bar_x = E_ICON_BAR_BG_ITEM_BAR_OFFSET - x;
		bar_w = GTK_WIDGET (icon_bar)->allocation.width - 2 * E_ICON_BAR_BG_ITEM_BAR_OFFSET - 1;

		gc = GTK_WIDGET (icon_bar)->style->fg_gc[GTK_STATE_NORMAL];

		/* Draw the horizontal bar. */
		gdk_draw_rectangle (drawable, gc, TRUE,
				    bar_x, bar_y,
				    bar_w, E_ICON_BAR_BG_ITEM_BAR_HEIGHT);

		if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS)
			arrow_height = E_ICON_BAR_BG_ITEM_LARGE_ARROW_HEIGHT / 2;
		else
			arrow_height = E_ICON_BAR_BG_ITEM_SMALL_ARROW_HEIGHT / 2;

		/* Draw the arrows at the end of the lines. We use
		   gdk_draw_line() to draw a series of vertical lines, since
		   gdk_draw_polygon() produces odd results. */
		i = 0;
		while (arrow_height > 0) {
			gdk_draw_line (drawable, gc,
				       bar_x + i,
				       bar_y - arrow_height,
				       bar_x + i,
				       bar_y + arrow_height);
			gdk_draw_line (drawable, gc,
				       bar_x + bar_w - i - 1,
				       bar_y - arrow_height,
				       bar_x + bar_w - i - 1,
				       bar_y + arrow_height);
			arrow_height--;
			i++;
		}
	}
}


/* This is supposed to return the nearest item the the point and the distance.
   Since we are the only item we just return ourself and 0 for the distance.
   This is needed so that we get button/motion events. */
static double
e_icon_bar_bg_item_point (GnomeCanvasItem *item, double x, double y,
			  int cx, int cy,
			  GnomeCanvasItem **actual_item)
{
	*actual_item = item;
	return 0.0;
}


static gint
e_icon_bar_bg_item_event (GnomeCanvasItem *item, GdkEvent *event)
{
	EIconBarBgItem *ibitem;

	ibitem = E_ICON_BAR_BG_ITEM (item);

	switch (event->type) {
	case GDK_BUTTON_PRESS:
		return e_icon_bar_bg_item_button_press (ibitem, event);
	case GDK_BUTTON_RELEASE:
		return e_icon_bar_bg_item_button_release (ibitem, event);
	case GDK_MOTION_NOTIFY:
		return e_icon_bar_bg_item_motion_notify (ibitem, event);
	default:
		break;
	}

	return FALSE;
}


static gint
e_icon_bar_bg_item_button_press (EIconBarBgItem *ibitem,
				 GdkEvent *event)
{
	gint item_num;

	if (event->button.button == 4 || event->button.button == 5)
		return FALSE;

	item_num = e_icon_bar_find_item_at_position (ibitem->icon_bar,
						     event->button.x,
						     event->button.y,
						     NULL);
	e_icon_bar_item_pressed (ibitem->icon_bar, item_num, event);
	return TRUE;
}


static gint
e_icon_bar_bg_item_button_release (EIconBarBgItem *ibitem,
				   GdkEvent *event)
{
	gint item_num;

	item_num = e_icon_bar_find_item_at_position (ibitem->icon_bar,
						     event->button.x,
						     event->button.y,
						     NULL);
	e_icon_bar_item_released (ibitem->icon_bar, item_num, event);
	return TRUE;
}


static gint
e_icon_bar_bg_item_motion_notify (EIconBarBgItem *ibitem,
				  GdkEvent *event)
{
	gint item_num;

	item_num = e_icon_bar_find_item_at_position (ibitem->icon_bar,
						     event->motion.x,
						     event->motion.y,
						     NULL);
	e_icon_bar_item_motion (ibitem->icon_bar, item_num, event);
	return TRUE;
}
