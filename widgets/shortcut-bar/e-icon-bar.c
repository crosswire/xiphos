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
 * EIconBar is a subclass of ECanvas for displaying a vertical column of
 * icons and descriptions. It provides 2 views - large icons and small icons.
 */

#include <config.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gdk-pixbuf/gnome-canvas-pixbuf.h>
#include "e-icon-bar.h"
#include "e-icon-bar-bg-item.h"
#include "../e-text/e-text.h"
#include "e-util/e-canvas-utils.h"

/* These are the offsets of the icons & text in both views. Note that the
   shadow around icons is drawn in the space between items (as is the
   horizontal bar when dragging). */
#define E_ICON_BAR_LARGE_ICON_SPACING	8	/* Spacing between items. */
#define E_ICON_BAR_LARGE_ICON_WIDTH	48
#define E_ICON_BAR_LARGE_ICON_HEIGHT	48
#define E_ICON_BAR_LARGE_ICON_TEXT_X	4
#define E_ICON_BAR_LARGE_ICON_TEXT_Y	(E_ICON_BAR_LARGE_ICON_HEIGHT + 4)

#define E_ICON_BAR_SMALL_ICON_SPACING	4	/* Spacing between items. */
#define E_ICON_BAR_SMALL_ICON_WIDTH	24
#define E_ICON_BAR_SMALL_ICON_HEIGHT	24
#define E_ICON_BAR_SMALL_ICON_X		4
#define E_ICON_BAR_SMALL_ICON_TEXT_X	(E_ICON_BAR_SMALL_ICON_WIDTH + 6)

/* The space we leave at the top or bottom of the bar when position an item
   while it is being edited. This is used since the EIconBar may be in a
   EScrolledBar which may show buttons at the top or bottom. */
#define E_ICON_BAR_V_SPACE		22

/* The number of pixels the mouse has to be moved with the button down before
   we start a drag. */
#define E_ICON_BAR_DRAG_START_OFFSET	4

/* This is the area at the top & bottom of the bar where we auto-scroll if the
   mouse goes into during a drag-and-drop operation. */
#define E_ICON_BAR_DRAG_AUTO_SCROLL_OFFSET	16

/* This is the time between each auto-scroll, when dragging. */
#define E_ICON_BAR_SCROLL_TIMEOUT	30

/* This is the number of timeouts we skip before we start scrolling. */
#define E_ICON_BAR_SCROLL_DELAY		12


static void e_icon_bar_class_init (EIconBarClass *class);
static void e_icon_bar_init (EIconBar *icon_bar);
static void e_icon_bar_destroy (GtkObject *object);
static void e_icon_bar_size_allocate (GtkWidget *widget,
				      GtkAllocation *allocation);
static gint e_icon_bar_leave_notify_event (GtkWidget *widget,
					   GdkEventCrossing *event);
static gint e_icon_bar_focus_in (GtkWidget     *widget,
				 GdkEventFocus *event);
static gint e_icon_bar_focus_out (GtkWidget     *widget,
				  GdkEventFocus *event);
static gint e_icon_bar_drag_motion (GtkWidget      *widget,
				    GdkDragContext *context,
				    gint            x,
				    gint            y,
				    guint           time);
static void e_icon_bar_drag_leave (GtkWidget      *widget,
				   GdkDragContext *context,
				   guint           time);
static void e_icon_bar_set_dragging_before_item (EIconBar *icon_bar,
						 gint before_item);
static gboolean e_icon_bar_timeout_handler (gpointer data);

static void e_icon_bar_recalc_common_positions (EIconBar *icon_bar);
static gint e_icon_bar_recalc_item_positions (EIconBar *icon_bar);
static void e_icon_bar_reflow (ECanvas *canvas);
static gint e_icon_bar_find_item (EIconBar *icon_bar,
				  GnomeCanvasItem *text_item);
static gboolean e_icon_bar_on_item_event (GnomeCanvasItem *item,
					  GdkEvent *event,
					  EIconBar *icon_bar);

static gboolean e_icon_bar_large_icons_intersects (EIconBar *icon_bar,
						   EIconBarItem *item,
						   gint x,
						   gint y);
static gboolean e_icon_bar_large_icons_is_before (EIconBar *icon_bar,
						  EIconBarItem *item,
						  gint x,
						  gint y);
static gboolean e_icon_bar_small_icons_intersects (EIconBar *icon_bar,
						   EIconBarItem *item,
						   gint x,
						   gint y);
static gboolean e_icon_bar_small_icons_is_before (EIconBar *icon_bar,
						  EIconBarItem *item,
						  gint x,
						  gint y);
static void e_icon_bar_on_editing_started (EIconBar *icon_bar,
					   GnomeCanvasItem *item);
static void e_icon_bar_on_editing_stopped (EIconBar *icon_bar,
					   GnomeCanvasItem *item);
static void e_icon_bar_ensure_edited_item_visible (EIconBar *icon_bar);
static void e_icon_bar_update_highlight (EIconBar *icon_bar);
static void e_icon_bar_vadjustment_value_changed (GtkAdjustment *adjustment,
						  EIconBar      *icon_bar);

enum
{
  ITEM_SELECTED,
  ITEM_DRAGGED,
  LAST_SIGNAL
};

static guint e_icon_bar_signals[LAST_SIGNAL] = {0};

static ECanvasClass *parent_class;


GtkType
e_icon_bar_get_type (void)
{
	static GtkType e_icon_bar_type = 0;

	if (!e_icon_bar_type){
		GtkTypeInfo e_icon_bar_info = {
			"EIconBar",
			sizeof (EIconBar),
			sizeof (EIconBarClass),
			(GtkClassInitFunc) e_icon_bar_class_init,
			(GtkObjectInitFunc) e_icon_bar_init,
			NULL, /* reserved 1 */
			NULL, /* reserved 2 */
			(GtkClassInitFunc) NULL
		};

		parent_class = gtk_type_class (e_canvas_get_type ());
		e_icon_bar_type = gtk_type_unique (e_canvas_get_type (),
						   &e_icon_bar_info);
	}

	return e_icon_bar_type;
}


static void
e_icon_bar_class_init (EIconBarClass *class)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;
	ECanvasClass *ecanvas_class;

	object_class = (GtkObjectClass *) class;
	widget_class = (GtkWidgetClass *) class;
	ecanvas_class = E_CANVAS_CLASS( class );

	e_icon_bar_signals[ITEM_SELECTED] =
		gtk_signal_new ("item_selected",
				GTK_RUN_LAST | GTK_RUN_ACTION,
				object_class->type,
				GTK_SIGNAL_OFFSET (EIconBarClass,
						   selected_item),
				gtk_marshal_NONE__POINTER_INT,
				GTK_TYPE_NONE, 2, GTK_TYPE_GDK_EVENT,
				GTK_TYPE_INT);
	e_icon_bar_signals[ITEM_DRAGGED] =
		gtk_signal_new ("item_dragged",
				GTK_RUN_LAST | GTK_RUN_ACTION,
				object_class->type,
				GTK_SIGNAL_OFFSET (EIconBarClass,
						   dragged_item),
				gtk_marshal_NONE__POINTER_INT,
				GTK_TYPE_NONE, 2, GTK_TYPE_GDK_EVENT,
				GTK_TYPE_INT);

	gtk_object_class_add_signals (object_class, e_icon_bar_signals,
				      LAST_SIGNAL);

	/* Method override */
	object_class->destroy		 = e_icon_bar_destroy;

 	widget_class->size_allocate	 = e_icon_bar_size_allocate;
 	widget_class->leave_notify_event = e_icon_bar_leave_notify_event;
	widget_class->focus_in_event	 = e_icon_bar_focus_in;
	widget_class->focus_out_event	 = e_icon_bar_focus_out;
	widget_class->drag_motion	 = e_icon_bar_drag_motion;
	widget_class->drag_leave	 = e_icon_bar_drag_leave;

	ecanvas_class->reflow            = e_icon_bar_reflow;

	class->selected_item		 = NULL;
}


static void
e_icon_bar_init (EIconBar *icon_bar)
{
	GdkColormap *colormap;
	gboolean success[E_ICON_BAR_COLOR_LAST];
	gint nfailed;

	icon_bar->view_type = E_ICON_BAR_LARGE_ICONS;
	icon_bar->items = g_array_new (FALSE, FALSE, sizeof (EIconBarItem));
	icon_bar->pressed_item_num = -1;
	icon_bar->mouse_over_item_num = -1;
	icon_bar->editing_item_num = -1;
	icon_bar->edit_rect_item = NULL;
	icon_bar->in_drag = FALSE;
	icon_bar->dragging_before_item_num = -1;
	icon_bar->icon_x = 0;
	icon_bar->icon_w = 0;
	icon_bar->icon_h = 0;
	icon_bar->text_x = 0;
	icon_bar->text_w = 5;
	icon_bar->auto_scroll_timeout_id = 0;
	icon_bar->vadjustment_value_changed_id = 0;

	/* Create the background item in the canvas, which handles selections
	   and drag-and-drop. */
	gnome_canvas_item_new (GNOME_CANVAS_GROUP (GNOME_CANVAS (icon_bar)->root),
			       e_icon_bar_bg_item_get_type (),
			       "EIconBarBgItem::icon_bar", icon_bar,
			       NULL);

	colormap = gtk_widget_get_colormap (GTK_WIDGET (icon_bar));

	icon_bar->colors[E_ICON_BAR_COLOR_TEXT].red   = 65535;
	icon_bar->colors[E_ICON_BAR_COLOR_TEXT].green = 65535;
	icon_bar->colors[E_ICON_BAR_COLOR_TEXT].blue  = 65535;

	icon_bar->colors[E_ICON_BAR_COLOR_EDITING_TEXT].red   = 0;
	icon_bar->colors[E_ICON_BAR_COLOR_EDITING_TEXT].green = 0;
	icon_bar->colors[E_ICON_BAR_COLOR_EDITING_TEXT].blue  = 0;

	icon_bar->colors[E_ICON_BAR_COLOR_EDITING_RECT].red   = 65535;
	icon_bar->colors[E_ICON_BAR_COLOR_EDITING_RECT].green = 65535;
	icon_bar->colors[E_ICON_BAR_COLOR_EDITING_RECT].blue  = 65535;

	icon_bar->colors[E_ICON_BAR_COLOR_EDITING_RECT_OUTLINE].red   = 0;
	icon_bar->colors[E_ICON_BAR_COLOR_EDITING_RECT_OUTLINE].green = 0;
	icon_bar->colors[E_ICON_BAR_COLOR_EDITING_RECT_OUTLINE].blue  = 0;

	nfailed = gdk_colormap_alloc_colors (colormap, icon_bar->colors,
					     E_ICON_BAR_COLOR_LAST, FALSE,
					     TRUE, success);
	if (nfailed)
		g_warning ("Failed to allocate all colors");
}


/**
 * e_icon_bar_new:
 * @Returns: A new #EIconBar.
 *
 * Creates a new #EIconBar.
 **/
GtkWidget *
e_icon_bar_new (void)
{
	GtkWidget *icon_bar;

	icon_bar = GTK_WIDGET (gtk_type_new (e_icon_bar_get_type ()));

	return icon_bar;
}


static void
e_icon_bar_destroy (GtkObject *object)
{
	EIconBar *icon_bar;
	int item_num;
	EIconBarItem *item;

	icon_bar = E_ICON_BAR (object);

	for (item_num = 0; item_num < icon_bar->items->len; item_num++) {
		item = &g_array_index (icon_bar->items,
				       EIconBarItem, item_num);
		if (item->destroy)
			item->destroy (item->data);
	}

	g_array_free (icon_bar->items, TRUE);

	if (icon_bar->auto_scroll_timeout_id != 0) {
		gtk_timeout_remove (icon_bar->auto_scroll_timeout_id);
		icon_bar->auto_scroll_timeout_id = 0;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}


static void
e_icon_bar_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	EIconBar *icon_bar;
	gint canvas_width, canvas_height, height;
#if 0
	g_print ("In e_icon_bar_size_allocate\n");
#endif
	icon_bar = E_ICON_BAR (widget);

	GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);

	canvas_width = GTK_WIDGET (icon_bar)->allocation.width;
	canvas_height = GTK_WIDGET (icon_bar)->allocation.height;

	/* Reset the y position and widths of all the items to the width of
	   the canvas, and reset the button labels, so they fit. */
	e_icon_bar_recalc_common_positions (icon_bar);
	height = e_icon_bar_recalc_item_positions (icon_bar);

	gnome_canvas_set_scroll_region (GNOME_CANVAS (widget),
					0, 0, canvas_width,
					MAX (height, canvas_height - 1));

	/* If we are editing an item, make sure it is visible. */
	e_icon_bar_ensure_edited_item_visible (icon_bar);

	GTK_LAYOUT (widget)->vadjustment->step_increment = 16;
	if (icon_bar->vadjustment_value_changed_id == 0)
		icon_bar->vadjustment_value_changed_id = gtk_signal_connect (GTK_OBJECT (GTK_LAYOUT (widget)->vadjustment), "value_changed", GTK_SIGNAL_FUNC (e_icon_bar_vadjustment_value_changed), icon_bar);

	e_icon_bar_update_highlight (icon_bar);
}


/* This sets all the item positions which are the same for all items in the
   group. */
static void
e_icon_bar_recalc_common_positions (EIconBar *icon_bar)
{
	gint canvas_width;

	canvas_width = GTK_WIDGET (icon_bar)->allocation.width;

	if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS) {
		icon_bar->icon_x = (canvas_width - E_ICON_BAR_LARGE_ICON_WIDTH) / 2;
		icon_bar->icon_w = E_ICON_BAR_LARGE_ICON_WIDTH;
		icon_bar->icon_h = E_ICON_BAR_LARGE_ICON_HEIGHT;

		icon_bar->text_x = E_ICON_BAR_LARGE_ICON_TEXT_X;
		icon_bar->text_w = MAX (canvas_width - (E_ICON_BAR_LARGE_ICON_TEXT_X * 2), 5);

		icon_bar->spacing = E_ICON_BAR_LARGE_ICON_SPACING;
	} else {
		icon_bar->icon_x = E_ICON_BAR_SMALL_ICON_X;
		icon_bar->icon_w = E_ICON_BAR_SMALL_ICON_WIDTH;
		icon_bar->icon_h = E_ICON_BAR_SMALL_ICON_HEIGHT;

		icon_bar->text_x = E_ICON_BAR_SMALL_ICON_TEXT_X;
		icon_bar->text_w = MAX (canvas_width - E_ICON_BAR_SMALL_ICON_TEXT_X, 5);

		icon_bar->spacing = E_ICON_BAR_SMALL_ICON_SPACING;
	}
}


/* This recalculates the positions of all the items, according to the current
   view type and the height of the text items. */
static gint
e_icon_bar_recalc_item_positions (EIconBar *icon_bar)
{
	EIconBarItem *item;
	gint y, item_num, text_h;
	gdouble x1, y1, x2, y2, text_x;
	GtkJustification justify;
	GtkAnchorType anchor;
	gint max_lines;
	GdkFont *font;
	gboolean line_wrap;

	if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS) {
		justify = GTK_JUSTIFY_CENTER;
		anchor = GTK_ANCHOR_N;
		max_lines = 2;
		text_x = icon_bar->text_x + (icon_bar->text_w / 2);
		line_wrap = TRUE;

		/*
		 * Fixme: incorrect
		 */
		font = GTK_WIDGET (icon_bar)->style->font;
		text_h = font->ascent + font->descent;
	} else {
		justify = GTK_JUSTIFY_LEFT;
		anchor = GTK_ANCHOR_NW;
		max_lines = 1;
		text_x = icon_bar->text_x;
		font = GTK_WIDGET (icon_bar)->style->font;
		text_h = font->ascent + font->descent;
		line_wrap = FALSE;
	}

	/* Now step through the items, setting the y positions. */
	y = icon_bar->spacing;
	for (item_num = 0; item_num < icon_bar->items->len; item_num++) {
		item = &g_array_index (icon_bar->items,
				       EIconBarItem, item_num);

		if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS) {
			item->icon_y = y;
			item->text_y = y + E_ICON_BAR_LARGE_ICON_TEXT_Y;
		} else {
			item->text_height = text_h;
			item->item_height = MAX (text_h, E_ICON_BAR_SMALL_ICON_HEIGHT);
			item->icon_y = y + (item->item_height - E_ICON_BAR_SMALL_ICON_HEIGHT) / 2;
			item->text_y = y + (item->item_height - item->text_height) / 2;
		}

		gnome_canvas_item_set (item->text,
				       "clip_width", (gdouble) (icon_bar->text_w),
				       "justification", justify,
				       "anchor", anchor,
				       "max_lines", max_lines,
				       "line_wrap", line_wrap,
				       NULL);
		e_canvas_item_move_absolute(item->text,
					    text_x, item->text_y);

		/* Get the text item's height. */
		gnome_canvas_item_get_bounds (item->text, &x1, &y1, &x2, &y2);
		item->text_x = x1;
		item->text_width = x2 - x1;
		item->text_height = y2 - y1;

		if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS) {
			item->item_height = E_ICON_BAR_LARGE_ICON_TEXT_Y
				+ item->text_height;
		}

		gnome_canvas_item_set (item->image,
				       "GnomeCanvasPixbuf::x", (gdouble)icon_bar->icon_x,
				       "GnomeCanvasPixbuf::y", (gdouble)item->icon_y,
				       "GnomeCanvasPixbuf::width_set", TRUE,
				       "GnomeCanvasPixbuf::height_set", TRUE,
				       "GnomeCanvasPixbuf::width", (gdouble)icon_bar->icon_w,
				       "GnomeCanvasPixbuf::height", (gdouble)icon_bar->icon_h,
				       NULL);

		y += item->item_height + icon_bar->spacing;
	}

	return y;
}


static gint
e_icon_bar_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event)
{
	EIconBar *icon_bar;

	icon_bar = E_ICON_BAR (widget);

	GTK_WIDGET_CLASS (parent_class)->leave_notify_event (widget, event);

	/* Make sure no items are highlighted. */
	e_icon_bar_item_motion (icon_bar, -1, NULL);

	return FALSE;
}


static gint
e_icon_bar_focus_in (GtkWidget     *widget,
		     GdkEventFocus *event)
{
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (E_IS_ICON_BAR (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	GTK_WIDGET_CLASS (parent_class)->focus_in_event (widget, event);
	GTK_WIDGET_SET_FLAGS (widget, GTK_HAS_FOCUS);
	return FALSE;
}


static gint
e_icon_bar_focus_out (GtkWidget     *widget,
		      GdkEventFocus *event)
{
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (E_IS_ICON_BAR (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	GTK_WIDGET_CLASS (parent_class)->focus_out_event (widget, event);
	GTK_WIDGET_UNSET_FLAGS (widget, GTK_HAS_FOCUS);
	return FALSE;
}


/**
 * e_icon_bar_set_view_type:
 * @icon_bar: An #EIconBar.
 * @view_type: The new view type, %E_ICON_BAR_LARGE_ICONS or
 * %E_ICON_BAR_SMALL_ICONS.
 *
 * Sets the view type of the #EIconBar.
 **/
void
e_icon_bar_set_view_type (EIconBar *icon_bar,
			  EIconBarViewType view_type)
{
	g_return_if_fail (E_IS_ICON_BAR (icon_bar));

	if (icon_bar->view_type == view_type)
		return;

	icon_bar->view_type = view_type;

	/* Queue a resize of the canvas, so everything is put in the right
	   positions based on the new view type. */
	gtk_widget_queue_resize (GTK_WIDGET (icon_bar));
}

static GdkPixbuf *
flatten_alpha (GdkPixbuf *image, guint rgb)
{
	if (!image || !gdk_pixbuf_get_has_alpha (image))
		return NULL;


	return gdk_pixbuf_composite_color_simple (
		image,
		gdk_pixbuf_get_width (image),
		gdk_pixbuf_get_height (image),
		GDK_INTERP_NEAREST,
		255,
		32,
		rgb, rgb);
}

/*
 * Creates a 24-bits RGB value from a GdkColor
 */
static guint
rgb_from_gdk_color (GdkColor *color)
{
	guint a =
		(((color->red >> 8) << 16) |
		((color->green >> 8) << 8) |
		((color->blue >> 8)));
	
	return a;
}

/**
 * e_icon_bar_add_item:
 * @icon_bar: An #EIconBar.
 * @image: the new item's icon.
 * @text: the new item's text.
 * @position: the position to place the new item, or -1 to place it last.
 *
 * Adds an item to the #EIconBar at the given position.
 **/
gint
e_icon_bar_add_item (EIconBar	    *icon_bar,
		     GdkPixbuf      *image,
		     const gchar    *text,
		     gint	     position)
{
	EIconBarItem item;
	GtkJustification justify;
	GtkAnchorType anchor;
	gint max_lines, retval;
	GtkStyle *style;
	GdkFont *font;
	gdouble text_x, clip_height;
	GdkPixbuf *flattened = NULL;

	g_return_val_if_fail (E_IS_ICON_BAR (icon_bar), -1);
	g_return_val_if_fail (text != NULL, -1);
	g_return_val_if_fail (position >= -1, -1);
	g_return_val_if_fail (position <= (gint)icon_bar->items->len, -1);

	if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS) {
		justify = GTK_JUSTIFY_CENTER;
		anchor = GTK_ANCHOR_N;
		max_lines = 2;
		text_x = icon_bar->text_x + (icon_bar->text_w / 2);
	} else {
		justify = GTK_JUSTIFY_LEFT;
		anchor = GTK_ANCHOR_NW;
		max_lines = 1;
		text_x = icon_bar->text_x;
	}

	gtk_widget_ensure_style (GTK_WIDGET (icon_bar));
	style = GTK_WIDGET (icon_bar)->style;
	font = style->font;
	clip_height = max_lines * (font->ascent + font->descent);
	item.text = gnome_canvas_item_new (GNOME_CANVAS_GROUP (GNOME_CANVAS (icon_bar)->root),
					   e_text_get_type (),
					   "font_gdk", font,
					   "fill_color_gdk", &icon_bar->colors[E_ICON_BAR_COLOR_TEXT],
					   "use_ellipsis", TRUE,
					   "anchor", anchor,
					   "editable", TRUE,
					   "justification", justify,
					   "line_wrap", TRUE,
					   "max_lines", max_lines,
					   "clip", TRUE,
					   "clip_width", (gdouble) (icon_bar->text_w),
					   "clip_height", clip_height,
					   "text", text,
					   NULL);
	e_canvas_item_move_absolute(item.text,
				    text_x, 0);

	gtk_signal_connect (GTK_OBJECT (item.text), "event",
			    GTK_SIGNAL_FUNC (e_icon_bar_on_item_event),
			    icon_bar);

	flattened = flatten_alpha (image,
				   rgb_from_gdk_color (&style->bg [GTK_STATE_NORMAL]));
	
	item.image = gnome_canvas_item_new (GNOME_CANVAS_GROUP (GNOME_CANVAS (icon_bar)->root),
					    gnome_canvas_pixbuf_get_type (),
					    "GnomeCanvasPixbuf::pixbuf", flattened ? flattened : image,
					    "GnomeCanvasPixbuf::width", (gdouble) icon_bar->icon_w,
					    "GnomeCanvasPixbuf::height", (gdouble) icon_bar->icon_h,
					     NULL);


	if (flattened)
		/* the canvas item holds the reference now */
		gdk_pixbuf_unref (flattened);

	gtk_signal_connect (GTK_OBJECT (item.image), "event",
			    GTK_SIGNAL_FUNC (e_icon_bar_on_item_event),
			    icon_bar);

	item.data = NULL;
	item.destroy = NULL;

	if (position == -1) {
		g_array_append_val (icon_bar->items, item);
		retval = icon_bar->items->len - 1;
	} else {
		g_array_insert_val (icon_bar->items, position, item);
		retval = position;

		/* FIXME: Should possibly update other indices. */
		if (icon_bar->dragged_item_num >= position)
			icon_bar->dragged_item_num++;
	}

	gtk_widget_queue_resize (GTK_WIDGET (icon_bar));

	return retval;
}


/**
 * e_icon_bar_reorder_item:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item to move.
 * @new_position: The new position of the item, which is used after the item
 * has been removed from its current position. If @new_position is -1, the item
 * is placed last.
 *
 * Moves an item to a new position within the #EIconBar.
 **/
void
e_icon_bar_reorder_item		(EIconBar	  *icon_bar,
				 gint		   item_num,
				 gint		   new_position)
{
	EIconBarItem tmp_item;

	g_return_if_fail (E_IS_ICON_BAR (icon_bar));
	g_return_if_fail (item_num >= 0);
	g_return_if_fail (item_num < icon_bar->items->len);
	g_return_if_fail (new_position >= -1);
	g_return_if_fail (new_position < icon_bar->items->len);

	tmp_item = g_array_index (icon_bar->items, EIconBarItem, item_num);
	g_array_remove_index (icon_bar->items, item_num);

	if (new_position == -1)
		g_array_append_val (icon_bar->items, tmp_item);
	else
		g_array_insert_val (icon_bar->items, new_position, tmp_item);

	gtk_widget_queue_resize (GTK_WIDGET (icon_bar));
}


/**
 * e_icon_bar_remove_item:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item to remove.
 *
 * Removes an item from the #EIconBar.
 **/
void
e_icon_bar_remove_item		(EIconBar	  *icon_bar,
				 gint		   item_num)
{
	EIconBarItem *item;

	g_return_if_fail (E_IS_ICON_BAR (icon_bar));
	g_return_if_fail (item_num >= 0);
	g_return_if_fail (item_num < icon_bar->items->len);

	item = &g_array_index (icon_bar->items, EIconBarItem, item_num);

	if (item->destroy)
		item->destroy (item->data);

	gtk_object_destroy (GTK_OBJECT (item->text));
	gtk_object_destroy (GTK_OBJECT (item->image));

	g_array_remove_index (icon_bar->items, item_num);

	gtk_widget_queue_resize (GTK_WIDGET (icon_bar));
}


/**
 * e_icon_bar_get_item_image:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item.
 * @Returns: The icon of the given item.
 *
 * Returns the icon used for the given item.
 **/
GdkPixbuf *
e_icon_bar_get_item_image	(EIconBar	  *icon_bar,
				 gint		   item_num)
{
	EIconBarItem *item;
	GdkPixbuf *image = NULL;

	g_return_val_if_fail (E_IS_ICON_BAR (icon_bar), NULL);
	g_return_val_if_fail (item_num >= 0, NULL);
	g_return_val_if_fail (item_num < icon_bar->items->len, NULL);

	item = &g_array_index (icon_bar->items, EIconBarItem, item_num);
	gtk_object_get (GTK_OBJECT (item->image),
			"GnomeCanvasPixbuf::pixbuf", &image,
			NULL);
	return image;
}


/**
 * e_icon_bar_set_item_image:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item.
 * @image: The new icon to use for the given item.
 *
 * Sets the icon to use for the given item.
 **/
void
e_icon_bar_set_item_image	(EIconBar	  *icon_bar,
				 gint		   item_num,
				 GdkPixbuf	  *image)
{
	EIconBarItem *item;

	g_return_if_fail (E_IS_ICON_BAR (icon_bar));
	g_return_if_fail (item_num >= 0);
	g_return_if_fail (item_num < icon_bar->items->len);

	item = &g_array_index (icon_bar->items, EIconBarItem, item_num);
	gnome_canvas_item_set (item->image,
			       "GnomeCanvasPixbuf::pixbuf", image,
			       NULL);
}


/**
 * e_icon_bar_get_item_text:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item.
 * @Returns: The text of the given item.
 *
 * Returns the text of the given item. This should be freed when no longer
 * needed.
 **/
gchar*
e_icon_bar_get_item_text	(EIconBar	  *icon_bar,
				 gint		   item_num)
{
	EIconBarItem *item;
	gchar *text;

	g_return_val_if_fail (E_IS_ICON_BAR (icon_bar), NULL);
	g_return_val_if_fail (item_num >= 0, NULL);
	g_return_val_if_fail (item_num < icon_bar->items->len, NULL);

	item = &g_array_index (icon_bar->items, EIconBarItem, item_num);
	gtk_object_get (GTK_OBJECT (item->text),
			"EText::text", &text,
			NULL);

	return text;
}


/**
 * e_icon_bar_set_item_text:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item.
 * @text: The new text for the given item.
 *
 * Sets the text of the given item.
 **/
void
e_icon_bar_set_item_text	(EIconBar	  *icon_bar,
				 gint		   item_num,
				 gchar		  *text)
{
	EIconBarItem *item;

	g_return_if_fail (E_IS_ICON_BAR (icon_bar));
	g_return_if_fail (item_num >= 0);
	g_return_if_fail (item_num < icon_bar->items->len);

	item = &g_array_index (icon_bar->items, EIconBarItem, item_num);
	gnome_canvas_item_set (item->text,
			       "EText::text", text,
			       NULL);
}


/**
 * e_icon_bar_get_item_data:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item.
 * @Returns: The user data associated with the given item.
 *
 * Returns the user data associated with the given item.
 **/
gpointer
e_icon_bar_get_item_data	(EIconBar	  *icon_bar,
				 gint		   item_num)
{
	EIconBarItem *item;

	g_return_val_if_fail (E_IS_ICON_BAR (icon_bar), NULL);
	g_return_val_if_fail (item_num >= 0, NULL);
	g_return_val_if_fail (item_num < icon_bar->items->len, NULL);

	item = &g_array_index (icon_bar->items, EIconBarItem, item_num);
	return item->data;
}


/**
 * e_icon_bar_set_item_data:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item.
 * @data: The user data to set for the given item.
 *
 * Sets the user data of the given item.
 **/
void
e_icon_bar_set_item_data	(EIconBar	  *icon_bar,
				 gint		   item_num,
				 gpointer	   data)
{
	e_icon_bar_set_item_data_full (icon_bar, item_num, data, NULL);
}


/**
 * e_icon_bar_set_item_data_full:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item.
 * @data: The user data to set for the given item.
 * @destroy: The function to free @data when the item is destroyed.
 *
 * Sets the user data of the given item, and the function to free the data
 * when the item is destroyed.
 **/
void
e_icon_bar_set_item_data_full	(EIconBar	  *icon_bar,
				 gint		   item_num,
				 gpointer	   data,
				 GtkDestroyNotify  destroy)
{
	EIconBarItem *item;

	g_return_if_fail (E_IS_ICON_BAR (icon_bar));
	g_return_if_fail (item_num >= 0);
	g_return_if_fail (item_num < icon_bar->items->len);

	item = &g_array_index (icon_bar->items, EIconBarItem, item_num);

	if (item->destroy)
		item->destroy (item->data);

	item->data = data;
	item->destroy = destroy;
}


static void
e_icon_bar_reflow (ECanvas *canvas)
{
#if 0
	g_print ("In e_icon_bar_on_canvas_reflow\n");
	gtk_widget_queue_resize (GTK_WIDGET (canvas));
#endif
}

				       
/* This returns the index of the given item, or -1 if it isn't found. */
static gint
e_icon_bar_find_item (EIconBar *icon_bar,
		      GnomeCanvasItem *canvas_item)
{
	EIconBarItem *item;
	gint item_num;

	for (item_num = 0; item_num < icon_bar->items->len; item_num++) {
		item = &g_array_index (icon_bar->items,
				       EIconBarItem, item_num);

		if (item->text == canvas_item || item->image == canvas_item) {
			return item_num;
		}
	}

	return -1;
}


/* When an item has a grab, it will get all events, so we need to use the
   position to find the real item. */
static gboolean
e_icon_bar_on_item_event (GnomeCanvasItem *item,
			  GdkEvent *event,
			  EIconBar *icon_bar)
{
	gint item_num;

	switch (event->type) {
	case GDK_BUTTON_PRESS:
		if (event->button.button == 4 || event->button.button == 5)
			return FALSE;

		item_num = e_icon_bar_find_item_at_position (icon_bar,
							     event->button.x,
							     event->button.y,
							     NULL);
		/* If the item is not being edited, we handle the event and
		   stop the signal so the text editing isn't started. */
		if (icon_bar->editing_item_num == -1
		    || icon_bar->editing_item_num != item_num) {
			e_icon_bar_item_pressed (icon_bar, item_num, event);
			gtk_signal_emit_stop_by_name (GTK_OBJECT (item),
						      "event");
			return TRUE;
		}
		break;
	case GDK_BUTTON_RELEASE:
		item_num = e_icon_bar_find_item_at_position (icon_bar,
							     event->button.x,
							     event->button.y,
							     NULL);
		/* If the item is not being edited, we handle the event and
		   stop the signal so the text editing isn't started. */
		if (icon_bar->editing_item_num == -1
		    || icon_bar->editing_item_num != item_num) {
			e_icon_bar_item_released (icon_bar, item_num, event);
			gtk_signal_emit_stop_by_name (GTK_OBJECT (item),
						      "event");
			return TRUE;
		}
		break;
	case GDK_MOTION_NOTIFY:
		item_num = e_icon_bar_find_item_at_position (icon_bar,
							     event->motion.x,
							     event->motion.y,
							     NULL);
		e_icon_bar_item_motion (icon_bar, item_num, event);
		return TRUE;
	case GDK_FOCUS_CHANGE:
		if (event->focus_change.in)
			e_icon_bar_on_editing_started (icon_bar, item);
		else
			e_icon_bar_on_editing_stopped (icon_bar, item);

		return FALSE;
	default:
		break;
	}

	return FALSE;
}


void
e_icon_bar_item_pressed (EIconBar *icon_bar,
			 gint item_num,
			 GdkEvent *event)
{
	gint button;

	/* If we are editing an item, and a different item (or anywhere outside
	   an item) is clicked, stop the edit. If the item being edited is
	   clicked we just return, since the user may be selecting text. */
	if (icon_bar->editing_item_num != -1) {
		if (icon_bar->editing_item_num != item_num) {
			e_icon_bar_stop_editing_item (icon_bar, TRUE);
		}
		return;
	}

	button = event->button.button;

	if (button == 1 && item_num != -1) {
		icon_bar->mouse_over_item_num = item_num;
		icon_bar->pressed_item_num = item_num;
		icon_bar->pressed_x = event->button.x;
		icon_bar->pressed_y = event->button.y;
		gtk_widget_queue_draw (GTK_WIDGET (icon_bar));
	} else if (button == 3) {
		gtk_signal_emit (GTK_OBJECT (icon_bar),
				 e_icon_bar_signals[ITEM_SELECTED],
				 event, item_num);
	}
}


void
e_icon_bar_item_released (EIconBar *icon_bar,
			  gint item_num,
			  GdkEvent *event)
{
	gint button;

	/* If we are editing an item, just return. */
	if (icon_bar->editing_item_num != -1)
		return;

	button = event->button.button;

	if (button == 1) {
		if (icon_bar->pressed_item_num != -1
		    && icon_bar->pressed_item_num == icon_bar->mouse_over_item_num) {
			gtk_signal_emit (GTK_OBJECT (icon_bar),
					 e_icon_bar_signals[ITEM_SELECTED],
					 event, item_num);
		}

		icon_bar->pressed_item_num = -1;
		gtk_widget_queue_draw (GTK_WIDGET (icon_bar));
	}
}


void
e_icon_bar_item_motion (EIconBar *icon_bar,
			gint item_num,
			GdkEvent *event)
{
	gboolean need_redraw = TRUE;

	if (event && event->motion.state & GDK_BUTTON1_MASK
	    && icon_bar->pressed_item_num != -1) {
		if (abs (event->motion.x - icon_bar->pressed_x) > E_ICON_BAR_DRAG_START_OFFSET
		    || abs (event->motion.y - icon_bar->pressed_y) > E_ICON_BAR_DRAG_START_OFFSET) {
			icon_bar->dragged_item_num = icon_bar->pressed_item_num;
			gtk_signal_emit (GTK_OBJECT (icon_bar),
					 e_icon_bar_signals[ITEM_DRAGGED],
					 event, icon_bar->dragged_item_num);
			
			/* Don't show the button as pressed while dragging. */
			icon_bar->pressed_item_num = -1;
			gtk_widget_queue_draw (GTK_WIDGET (icon_bar));
		}

		return;
	}

	if (icon_bar->mouse_over_item_num == item_num)
		return;

	/* If we are editing an item, items aren't highlighted so we don't
	   need a redraw. Also if an item is pressed, we only need a redraw if
	   item_num or the old mouse_over_item_num is the pressed item. */
	if (icon_bar->editing_item_num != -1) {
		need_redraw = FALSE;
	} else if (icon_bar->pressed_item_num != -1) {
		if (icon_bar->pressed_item_num != item_num
		    && icon_bar->pressed_item_num != icon_bar->mouse_over_item_num)
			need_redraw = FALSE;
	}

	icon_bar->mouse_over_item_num = item_num;

	if (need_redraw)
		gtk_widget_queue_draw (GTK_WIDGET (icon_bar));
}


/* This returns the index of the item at the given position on the EIconBar,
   or -1 if no item is found. If before_item is not NULL, it returns the
   item which the mouse is before, or -1 (for dragging). */
gint
e_icon_bar_find_item_at_position (EIconBar *icon_bar,
				  gint x,
				  gint y,
				  gint *before_item)
{
	EIconBarItem *item = NULL;
	gint item_num;

	if (before_item)
		*before_item = -1;

	for (item_num = 0; item_num < icon_bar->items->len; item_num++) {
		item = &g_array_index (icon_bar->items,
				       EIconBarItem, item_num);

		if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS) {
			if (e_icon_bar_large_icons_intersects (icon_bar, item,
							       x, y))
				return item_num;

			if (before_item
			    && e_icon_bar_large_icons_is_before (icon_bar,
								 item, x, y)) {
				*before_item = item_num;
				return -1;
			}
		} else {
			if (e_icon_bar_small_icons_intersects (icon_bar, item,
							       x, y))
				return item_num;

			if (before_item
			    && e_icon_bar_small_icons_is_before (icon_bar,
								 item, x, y)) {
				*before_item = item_num;
				return -1;
			}

		}

	}

	/* If the mouse is below all the items, but inside the items' width,
	   and before_item is not NULL, we set it to the number of items, so
	   the dropped item would be added at the end. Note that this assumes
	   that the item variable points to the last item in the EIconBar. */
	if (before_item) {
		if (icon_bar->view_type == E_ICON_BAR_LARGE_ICONS) {
			if (x < icon_bar->text_x
			    || x >= icon_bar->text_x + icon_bar->text_w)
				return -1;

			if (item == NULL
			    || y > item->icon_y + item->item_height)
				*before_item = icon_bar->items->len;
		} else {
			if (x < icon_bar->icon_x
			    || x >= icon_bar->text_x + icon_bar->text_w)
				return -1;

			if (item == NULL) {
				*before_item = icon_bar->items->len;
			} else {
				gint max_y;
				max_y = MAX (item->icon_y + icon_bar->icon_h,
					     item->text_y + item->text_height);
				if (y > max_y)
					*before_item = icon_bar->items->len;
			}
		}
	}

	return -1;
}


static gboolean
e_icon_bar_large_icons_intersects (EIconBar *icon_bar,
				   EIconBarItem *item,
				   gint x,
				   gint y)
{
	if (y < item->icon_y || y >= item->text_y + item->text_height)
		return FALSE;

	if (y < item->icon_y + icon_bar->icon_h) {
		if (x < icon_bar->icon_x
		    || x >= icon_bar->icon_x + icon_bar->icon_w)
			return FALSE;
	} else {
		if (x < item->text_x
		    || x >= item->text_x + item->text_width)
			return FALSE;
	}

	return TRUE;
}


static gboolean
e_icon_bar_large_icons_is_before (EIconBar *icon_bar,
				  EIconBarItem *item,
				  gint x,
				  gint y)
{
	if (y < item->icon_y - icon_bar->spacing
	    || y >= item->icon_y)
		return FALSE;

	if (x < icon_bar->text_x || x >= icon_bar->text_x + icon_bar->text_w)
		return FALSE;

	return TRUE;
}


static gboolean
e_icon_bar_small_icons_intersects (EIconBar *icon_bar,
				   EIconBarItem *item,
				   gint x,
				   gint y)
{
	gint min_y, max_y;

	min_y = MIN (item->icon_y, item->text_y);
	max_y = MAX (item->icon_y + icon_bar->icon_h,
		     item->text_y + item->text_height);

	if (y < min_y || y >= max_y)
		return FALSE;

	if (x < icon_bar->icon_x || x >= item->text_x + item->text_width)
		return FALSE;

	return TRUE;
}


static gboolean
e_icon_bar_small_icons_is_before (EIconBar *icon_bar,
				  EIconBarItem *item,
				  gint x,
				  gint y)
{
	gint min_y, max_y;

	max_y = MIN (item->icon_y, item->text_y);
	min_y = max_y - icon_bar->spacing;

	if (y < min_y || y >= max_y)
		return FALSE;

	if (x < icon_bar->icon_x || x >= icon_bar->text_x + icon_bar->text_w)
		return FALSE;

	return TRUE;
}


/**
 * e_icon_bar_start_editing_item:
 * @icon_bar: An #EIconBar.
 * @item_num: The index of the item.
 *
 * Turns the item into an editable text field so the user can rename it.
 * Editing is stopped automatically when the user hits 'Return' or clicks
 * outside the item. It can also be stopped explicitly by calling
 * e_icon_bar_stop_editing_item().
 **/
void
e_icon_bar_start_editing_item (EIconBar *icon_bar,
			       gint item_num)
{
	EIconBarItem *item;

	g_return_if_fail (E_IS_ICON_BAR (icon_bar));
	g_return_if_fail (item_num >= 0);
	g_return_if_fail (item_num < icon_bar->items->len);

	item = &g_array_index (icon_bar->items,
			       EIconBarItem, item_num);
	e_canvas_item_grab_focus (item->text);
}


/**
 * e_icon_bar_stop_editing_item:
 * @icon_bar: An #EIconBar.
 * @accept: TRUE if the changes should be accepted, FALSE if the text should be
 * changed back to its state before the editing started.
 *
 * Stops the editing of the items, if any were being edited.
 **/
void
e_icon_bar_stop_editing_item (EIconBar *icon_bar,
			      gboolean  accept)
{
	EIconBarItem *item;
	GtkWidget *toplevel;

	g_return_if_fail (E_IS_ICON_BAR (icon_bar));

	if (icon_bar->editing_item_num != -1) {
		item = &g_array_index (icon_bar->items, EIconBarItem,
				       icon_bar->editing_item_num);

		toplevel = gtk_widget_get_toplevel (GTK_WIDGET (icon_bar));
		if (toplevel && GTK_IS_WINDOW (toplevel))
			gtk_window_set_focus (GTK_WINDOW (toplevel), NULL);
	}
}


static void
e_icon_bar_on_editing_started (EIconBar *icon_bar,
			       GnomeCanvasItem *item)
{
	gint item_num;
	gdouble x1, y1, x2, y2;

	item_num = e_icon_bar_find_item (icon_bar, item);
	g_return_if_fail (item_num != -1);

	/* Turn off any highlighted item. */
	e_icon_bar_item_motion (icon_bar, -1, NULL);

	icon_bar->editing_item_num = item_num;

	e_icon_bar_ensure_edited_item_visible (icon_bar);

	/* Set the fg & bg colors. */
	gnome_canvas_item_set (item,
			       "fill_color_gdk", &icon_bar->colors[E_ICON_BAR_COLOR_EDITING_TEXT],
			       NULL);

	/* Create the edit rect if necessary. */
	if (!icon_bar->edit_rect_item) {
		icon_bar->edit_rect_item =
			gnome_canvas_item_new (GNOME_CANVAS_GROUP (GNOME_CANVAS (icon_bar)->root),
					       gnome_canvas_rect_get_type(),
					       "fill_color_gdk", &icon_bar->colors[E_ICON_BAR_COLOR_EDITING_RECT],
					       "outline_color_gdk", &icon_bar->colors[E_ICON_BAR_COLOR_EDITING_RECT_OUTLINE],
					       NULL);
	}

	gnome_canvas_item_get_bounds (item, &x1, &y1, &x2, &y2);
	gnome_canvas_item_set (icon_bar->edit_rect_item,
			       "x1", x1 - 1,
			       "y1", y1 - 1,
			       "x2", x2 + 1,
			       "y2", y2 + 1,
			       NULL);
	gnome_canvas_item_show (icon_bar->edit_rect_item);

	/* Make sure the text item is on top. */
	gnome_canvas_item_raise_to_top (item);
}


static void
e_icon_bar_on_editing_stopped (EIconBar *icon_bar,
			       GnomeCanvasItem *item)
{
	gint item_num;

	item_num = e_icon_bar_find_item (icon_bar, item);
	g_return_if_fail (item_num != -1);

	icon_bar->editing_item_num = -1;

	e_icon_bar_update_highlight (icon_bar);

	/* Reset the fg & bg colors. */
	gnome_canvas_item_set (item,
			       "fill_color_gdk", &icon_bar->colors[E_ICON_BAR_COLOR_TEXT],
			       NULL);

	if (icon_bar->edit_rect_item) {
		gnome_canvas_item_hide (icon_bar->edit_rect_item);
	}
}


static void
e_icon_bar_ensure_edited_item_visible (EIconBar *icon_bar)
{
	EIconBarItem *item;
	gint scroll_x, scroll_y, min_scroll_y, max_scroll_y, new_scroll_y;

	if (icon_bar->editing_item_num == -1)
		return;

	item = &g_array_index (icon_bar->items,
			       EIconBarItem, icon_bar->editing_item_num);
	gnome_canvas_get_scroll_offsets (GNOME_CANVAS (icon_bar),
					 &scroll_x, &scroll_y);

	/* The minimum scroll y position is with the text right on the bottom
	   of the display. */
	min_scroll_y = item->text_y + item->text_height + E_ICON_BAR_V_SPACE
		- GTK_WIDGET (icon_bar)->allocation.height;
	/* The maximum scroll y position is with the text at the top. */
	max_scroll_y = item->text_y - E_ICON_BAR_V_SPACE;

	new_scroll_y = MAX (scroll_y, min_scroll_y);
	new_scroll_y = MIN (new_scroll_y, max_scroll_y);

	if (new_scroll_y != scroll_y)
		gnome_canvas_scroll_to (GNOME_CANVAS (icon_bar),
					scroll_x, new_scroll_y);
}


/* This gets the mouse position and updates the highlight if necessary.
   It is called after items are added/deleted/scrolled/edited. */
static void
e_icon_bar_update_highlight (EIconBar *icon_bar)
{
	GtkWidget *widget;
	gint x, y, item_num;

	widget = GTK_WIDGET (icon_bar);

	if (!widget->window)
		return;

	gdk_window_get_pointer (widget->window, &x, &y, NULL);

	if (x < 0 || y < 0
	    || x > widget->allocation.width || y > widget->allocation.height)
		return;

	x += icon_bar->canvas.parent.layout.hadjustment->value;
	y += icon_bar->canvas.parent.layout.vadjustment->value;
	item_num = e_icon_bar_find_item_at_position (icon_bar, x, y, NULL);
	e_icon_bar_item_motion (icon_bar, item_num, NULL);
}


static gint
e_icon_bar_drag_motion (GtkWidget      *widget,
			GdkDragContext *context,
			gint            x,
			gint            y,
			guint           time)
{
	EIconBar *icon_bar;
	gint item_num, before_item, scroll_x, scroll_y;

	g_return_val_if_fail (E_IS_ICON_BAR (widget), FALSE);

	icon_bar = E_ICON_BAR (widget);

	icon_bar->in_drag = TRUE;

	/* Check if the mouse is over or between items, and if so highlight. */
	gnome_canvas_get_scroll_offsets (GNOME_CANVAS (icon_bar),
					 &scroll_x, &scroll_y);
	item_num = e_icon_bar_find_item_at_position (icon_bar,
						     x + scroll_x,
						     y + scroll_y,
						     &before_item);
	e_icon_bar_item_motion (icon_bar, item_num, NULL);
	e_icon_bar_set_dragging_before_item (icon_bar, before_item);

	/* Check if the mouse is at the top or bottom of the bar, and if it is
	   scroll up/down. */
	if (y < E_ICON_BAR_DRAG_AUTO_SCROLL_OFFSET)
		icon_bar->scrolling_up = TRUE;
	else if (y >= widget->allocation.height - E_ICON_BAR_DRAG_AUTO_SCROLL_OFFSET)
		icon_bar->scrolling_up = FALSE;
	else {
		if (icon_bar->auto_scroll_timeout_id != 0) {
			gtk_timeout_remove (icon_bar->auto_scroll_timeout_id);
			icon_bar->auto_scroll_timeout_id = 0;
		}
		return FALSE;
	}

	if (icon_bar->auto_scroll_timeout_id == 0) {
		icon_bar->auto_scroll_timeout_id = g_timeout_add (E_ICON_BAR_SCROLL_TIMEOUT, e_icon_bar_timeout_handler, icon_bar);
		icon_bar->auto_scroll_delay = E_ICON_BAR_SCROLL_DELAY;
	}

	return FALSE;
}


static void
e_icon_bar_drag_leave (GtkWidget      *widget,
		       GdkDragContext *context,
		       guint           time)
{
	EIconBar *icon_bar;

	g_return_if_fail (E_IS_ICON_BAR (widget));

	icon_bar = E_ICON_BAR (widget);

	icon_bar->in_drag = FALSE;

	if (icon_bar->auto_scroll_timeout_id != 0) {
		gtk_timeout_remove (icon_bar->auto_scroll_timeout_id);
		icon_bar->auto_scroll_timeout_id = 0;
	}

	if (icon_bar->mouse_over_item_num != -1) {
		icon_bar->mouse_over_item_num = -1;
		gtk_widget_queue_draw (GTK_WIDGET (icon_bar));
	}
}


static void
e_icon_bar_set_dragging_before_item (EIconBar *icon_bar,
				     gint before_item)
{
	if (icon_bar->dragging_before_item_num == before_item)
		return;

	icon_bar->dragging_before_item_num = before_item;

	gtk_widget_queue_draw (GTK_WIDGET (icon_bar));
}


static gboolean
e_icon_bar_timeout_handler (gpointer data)
{
	EIconBar *icon_bar;
	gint scroll_x, scroll_y, new_scroll_y;
	GtkAdjustment *adj;

	g_return_val_if_fail (E_IS_ICON_BAR (data), FALSE);

	icon_bar = E_ICON_BAR (data);

	GDK_THREADS_ENTER ();

	if (icon_bar->auto_scroll_delay > 0) {
		icon_bar->auto_scroll_delay--;
		GDK_THREADS_LEAVE ();
		return TRUE;
	}

	gnome_canvas_get_scroll_offsets (GNOME_CANVAS (icon_bar),
					 &scroll_x, &scroll_y);

	adj = GTK_LAYOUT (icon_bar)->vadjustment;

	if (icon_bar->scrolling_up)
		new_scroll_y = MAX (scroll_y - adj->step_increment, 0);
	else
		new_scroll_y = MIN (scroll_y + adj->step_increment,
				    adj->upper - adj->page_size);

	if (new_scroll_y != scroll_y)
		gnome_canvas_scroll_to (GNOME_CANVAS (icon_bar),
					scroll_x, new_scroll_y);

	GDK_THREADS_LEAVE ();
	return TRUE;
}


static void
e_icon_bar_vadjustment_value_changed (GtkAdjustment *adjustment,
				      EIconBar      *icon_bar)
{
	e_icon_bar_update_highlight (icon_bar);
}

