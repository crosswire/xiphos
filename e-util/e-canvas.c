/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * e-canvas.c
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

#include <gnome.h>
#include "e-canvas.h"
static void e_canvas_init           (ECanvas         *card);
static void e_canvas_destroy        (GtkObject        *object);
static void e_canvas_class_init	    (ECanvasClass    *klass);
static void e_canvas_realize        (GtkWidget        *widget);
static void e_canvas_unrealize      (GtkWidget        *widget);
static gint e_canvas_key            (GtkWidget        *widget,
				     GdkEventKey      *event);

static gint e_canvas_focus_in       (GtkWidget        *widget,
				     GdkEventFocus    *event);
static gint e_canvas_focus_out      (GtkWidget        *widget,
				     GdkEventFocus    *event);

static int emit_event (GnomeCanvas *canvas, GdkEvent *event);

static GnomeCanvasClass *parent_class = NULL;

enum {
	REFLOW,
	LAST_SIGNAL
};

static guint e_canvas_signals [LAST_SIGNAL] = { 0, };

GtkType
e_canvas_get_type (void)
{
	static GtkType canvas_type = 0;

	if (!canvas_type)
	{
		static const GtkTypeInfo canvas_info =
		{
			"ECanvas",
			sizeof (ECanvas),
			sizeof (ECanvasClass),
			(GtkClassInitFunc) e_canvas_class_init,
			(GtkObjectInitFunc) e_canvas_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		canvas_type = gtk_type_unique (gnome_canvas_get_type (), &canvas_info);
	}

	return canvas_type;
}

static void
e_canvas_class_init (ECanvasClass *klass)
{
	GtkObjectClass *object_class;
	GnomeCanvasClass *canvas_class;
	GtkWidgetClass *widget_class;

	object_class = (GtkObjectClass*) klass;
	canvas_class = (GnomeCanvasClass *) klass;
	widget_class = (GtkWidgetClass *) klass;

	parent_class = gtk_type_class (gnome_canvas_get_type ());

	object_class->destroy = e_canvas_destroy;

	widget_class->key_press_event = e_canvas_key;
	widget_class->key_release_event = e_canvas_key;
	widget_class->focus_in_event = e_canvas_focus_in;
	widget_class->focus_out_event = e_canvas_focus_out;
	widget_class->realize = e_canvas_realize;
	widget_class->unrealize = e_canvas_unrealize;

	klass->reflow = NULL;

	e_canvas_signals [REFLOW] =
		gtk_signal_new ("reflow",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (ECanvasClass, reflow),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0);

	gtk_object_class_add_signals (object_class, e_canvas_signals, LAST_SIGNAL);
}

static void
e_canvas_init (ECanvas *canvas)
{
	canvas->selection = NULL;
	canvas->cursor = NULL;
	canvas->ic = NULL;
	canvas->ic_attr = NULL;
}

static void
e_canvas_destroy (GtkObject *object)
{
	ECanvas *canvas = E_CANVAS(object);
	if (canvas->idle_id)
		g_source_remove(canvas->idle_id);
	if ((GTK_OBJECT_CLASS (parent_class))->destroy)
		(*(GTK_OBJECT_CLASS (parent_class))->destroy) (object);
}

GtkWidget *
e_canvas_new ()
{
	return GTK_WIDGET (gtk_type_new (e_canvas_get_type ()));
}


/* Returns whether the item is an inferior of or is equal to the parent. */
static int
is_descendant (GnomeCanvasItem *item, GnomeCanvasItem *parent)
{
	for (; item; item = item->parent)
		if (item == parent)
			return TRUE;

	return FALSE;
}

/* Emits an event for an item in the canvas, be it the current item, grabbed
 * item, or focused item, as appropriate.
 */
static int
emit_event (GnomeCanvas *canvas, GdkEvent *event)
{
	/*GdkEvent ev;*/
	gint finished;
	GnomeCanvasItem *item;
	GnomeCanvasItem *parent;
	guint mask;

	/* Perform checks for grabbed items */

	if (canvas->grabbed_item && !is_descendant (canvas->current_item, canvas->grabbed_item))
		return FALSE;

	if (canvas->grabbed_item) {
		switch (event->type) {
		case GDK_ENTER_NOTIFY:
			mask = GDK_ENTER_NOTIFY_MASK;
			break;

		case GDK_LEAVE_NOTIFY:
			mask = GDK_LEAVE_NOTIFY_MASK;
			break;

		case GDK_MOTION_NOTIFY:
			mask = GDK_POINTER_MOTION_MASK;
			break;

		case GDK_BUTTON_PRESS:
		case GDK_2BUTTON_PRESS:
		case GDK_3BUTTON_PRESS:
			mask = GDK_BUTTON_PRESS_MASK;
			break;

		case GDK_BUTTON_RELEASE:
			mask = GDK_BUTTON_RELEASE_MASK;
			break;

		case GDK_KEY_PRESS:
			mask = GDK_KEY_PRESS_MASK;
			break;

		case GDK_KEY_RELEASE:
			mask = GDK_KEY_RELEASE_MASK;
			break;

		default:
			mask = 0;
			break;
		}

		if (!(mask & canvas->grabbed_event_mask))
			return FALSE;
	}

	/* Convert to world coordinates -- we have two cases because of diferent
	 * offsets of the fields in the event structures.
	 */

	/*ev = *event;*/

	switch (event->type) {
	case GDK_ENTER_NOTIFY:
	case GDK_LEAVE_NOTIFY:
		gnome_canvas_window_to_world (canvas,
					      event->crossing.x, 
					      event->crossing.y,
					      &(event->crossing.x), 
					      &(event->crossing.y));
		break;

	case GDK_MOTION_NOTIFY:
	case GDK_BUTTON_PRESS:
	case GDK_2BUTTON_PRESS:
	case GDK_3BUTTON_PRESS:
	case GDK_BUTTON_RELEASE:
		gnome_canvas_window_to_world (canvas,
					      event->motion.x, 
					      event->motion.y,
					      &(event->motion.x), 
					      &(event->motion.y));
		break;

	default:
		break;
	}

	/* Choose where we send the event */

	item = canvas->current_item;

	if (canvas->focused_item
	    && ((event->type == GDK_KEY_PRESS) || (event->type == GDK_KEY_RELEASE) || (event->type == GDK_FOCUS_CHANGE)))
		item = canvas->focused_item;

	/* The event is propagated up the hierarchy (for if someone connected to
	 * a group instead of a leaf event), and emission is stopped if a
	 * handler returns TRUE, just like for GtkWidget events.
	 */

	finished = FALSE;

	while (item && !finished) {
		gtk_object_ref (GTK_OBJECT (item));

		gtk_signal_emit_by_name (GTK_OBJECT (item), "event",
					 event,
					 &finished);

		if (GTK_OBJECT_DESTROYED (item))
			finished = TRUE;

		parent = item->parent;
		gtk_object_unref (GTK_OBJECT (item));

		item = parent;
	}

	return finished;
}

/* Key event handler for the canvas */
static gint
e_canvas_key (GtkWidget *widget, GdkEventKey *event)
{
	GnomeCanvas *canvas;

	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GNOME_IS_CANVAS (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	canvas = GNOME_CANVAS (widget);

	return emit_event (canvas, (GdkEvent *) event);
}


/**
 * e_canvas_item_grab_focus:
 * @item: A canvas item.
 *
 * Makes the specified item take the keyboard focus, so all keyboard events will
 * be sent to it.  If the canvas widget itself did not have the focus, it grabs
 * it as well.
 **/
void
e_canvas_item_grab_focus (GnomeCanvasItem *item)
{
	GnomeCanvasItem *focused_item;
	GdkEvent ev;

	g_return_if_fail (item != NULL);
	g_return_if_fail (GNOME_IS_CANVAS_ITEM (item));
	g_return_if_fail (GTK_WIDGET_CAN_FOCUS (GTK_WIDGET (item->canvas)));

	focused_item = item->canvas->focused_item;

	if (focused_item) {
		ev.focus_change.type = GDK_FOCUS_CHANGE;
		ev.focus_change.window = GTK_LAYOUT (item->canvas)->bin_window;
		ev.focus_change.send_event = FALSE;
		ev.focus_change.in = FALSE;

		emit_event (item->canvas, &ev);
	}

	item->canvas->focused_item = item;

	if (!GTK_WIDGET_HAS_FOCUS (GTK_WIDGET(item->canvas))) {
		gtk_widget_grab_focus (GTK_WIDGET (item->canvas));
	}

	if (focused_item) {
		ev.focus_change.type = GDK_FOCUS_CHANGE;
		ev.focus_change.window = GTK_LAYOUT (item->canvas)->bin_window;
		ev.focus_change.send_event = FALSE;
		ev.focus_change.in = TRUE;

		emit_event (item->canvas, &ev);
	}
}

/* Focus in handler for the canvas */
static gint
e_canvas_focus_in (GtkWidget *widget, GdkEventFocus *event)
{
	GnomeCanvas *canvas;
	ECanvas *ecanvas;

	canvas = GNOME_CANVAS (widget);
	ecanvas = E_CANVAS (widget);

	GTK_WIDGET_SET_FLAGS (widget, GTK_HAS_FOCUS);

	if (ecanvas->ic)
		gdk_im_begin (ecanvas->ic, canvas->layout.bin_window);

	if (canvas->focused_item)
		return emit_event (canvas, (GdkEvent *) event);
	else
		return FALSE;
}

/* Focus out handler for the canvas */
static gint
e_canvas_focus_out (GtkWidget *widget, GdkEventFocus *event)
{
	GnomeCanvas *canvas;
	ECanvas *ecanvas;

	canvas = GNOME_CANVAS (widget);
	ecanvas = E_CANVAS (widget);

	GTK_WIDGET_UNSET_FLAGS (widget, GTK_HAS_FOCUS);

	if (ecanvas->ic)
		gdk_im_end ();

	if (canvas->focused_item)
		return emit_event (canvas, (GdkEvent *) event);
	else
		return FALSE;
}

static void
e_canvas_realize (GtkWidget *widget)
{
	ECanvas *ecanvas = E_CANVAS (widget);

	if (GTK_WIDGET_CLASS (parent_class)->realize)
		(* GTK_WIDGET_CLASS (parent_class)->realize) (widget);

	gdk_window_set_back_pixmap (GTK_LAYOUT (widget)->bin_window, NULL, FALSE);

	if (gdk_im_ready () && (ecanvas->ic_attr = gdk_ic_attr_new ()) != NULL) {
		GdkEventMask mask;
		GdkICAttr *attr = ecanvas->ic_attr;
		GdkICAttributesType attrmask = GDK_IC_ALL_REQ;
		GdkIMStyle style;
		GdkIMStyle supported_style = GDK_IM_PREEDIT_NONE |
			GDK_IM_PREEDIT_NOTHING |
			GDK_IM_STATUS_NONE |
			GDK_IM_STATUS_NOTHING;

		attr->style = style = gdk_im_decide_style (supported_style);
		attr->client_window = ecanvas->parent.layout.bin_window;

		ecanvas->ic = gdk_ic_new (attr, attrmask);
		if (ecanvas->ic != NULL) {
			mask = gdk_window_get_events (attr->client_window);
			mask |= gdk_ic_get_events (ecanvas->ic);
			gdk_window_set_events (attr->client_window, mask);

			if (GTK_WIDGET_HAS_FOCUS (widget))
				gdk_im_begin (ecanvas->ic, attr->client_window);
		} else
			g_warning ("Can't create input context.");
	}

}

static void
e_canvas_unrealize (GtkWidget *widget)
{
	ECanvas * ecanvas = E_CANVAS (widget);
	if (ecanvas->ic) {
		gdk_ic_destroy (ecanvas->ic);
		ecanvas->ic = NULL;
	}
	if (ecanvas->ic_attr) {
		gdk_ic_attr_destroy (ecanvas->ic_attr);
		ecanvas->ic_attr = NULL;
	}
	if (GTK_WIDGET_CLASS (parent_class)->unrealize)
		(* GTK_WIDGET_CLASS (parent_class)->unrealize) (widget);
}

static void
e_canvas_item_invoke_reflow (GnomeCanvasItem *item, int flags)
{
	GnomeCanvasGroup *group;
	GList *list;
	GnomeCanvasItem *child;

	if (GNOME_IS_CANVAS_GROUP (item)) {
		group = GNOME_CANVAS_GROUP (item);
		for (list = group->item_list; list; list = list->next) {
			child = GNOME_CANVAS_ITEM (list->data);
			if (child->object.flags & E_CANVAS_ITEM_DESCENDENT_NEEDS_REFLOW)
				e_canvas_item_invoke_reflow (child, flags);
		}
	}

	if (item->object.flags & E_CANVAS_ITEM_NEEDS_REFLOW) {
		ECanvasItemReflowFunc func;
		func = gtk_object_get_data (GTK_OBJECT (item),
					    "ECanvasItem::reflow_callback");
		if (func)
			func (item, flags);
	}

	item->object.flags &= ~E_CANVAS_ITEM_NEEDS_REFLOW;
	item->object.flags &= ~E_CANVAS_ITEM_DESCENDENT_NEEDS_REFLOW;
}

static void
do_reflow (ECanvas *canvas)
{
	if (GNOME_CANVAS(canvas)->root->object.flags & E_CANVAS_ITEM_DESCENDENT_NEEDS_REFLOW)
		e_canvas_item_invoke_reflow (GNOME_CANVAS(canvas)->root, 0);
}

/* Idle handler for the e-canvas.  It deals with pending reflows. */
static gint
idle_handler (gpointer data)
{
	ECanvas *canvas;

	GDK_THREADS_ENTER();

	canvas = E_CANVAS (data);
	do_reflow (canvas);

	/* Reset idle id */
	canvas->idle_id = 0;

	gtk_signal_emit (GTK_OBJECT (canvas),
			 e_canvas_signals [REFLOW]);

	GDK_THREADS_LEAVE();

	return FALSE;
}

/* Convenience function to add an idle handler to a canvas */
static void
add_idle (ECanvas *canvas)
{
	if (canvas->idle_id != 0)
		return;

	canvas->idle_id = g_idle_add_full (G_PRIORITY_HIGH_IDLE, idle_handler, (gpointer) canvas, NULL);
}

static void
e_canvas_item_descendent_needs_reflow (GnomeCanvasItem *item)
{
	if (item->object.flags & E_CANVAS_ITEM_DESCENDENT_NEEDS_REFLOW)
		return;

	item->object.flags |= E_CANVAS_ITEM_DESCENDENT_NEEDS_REFLOW;
	if (item->parent)
		e_canvas_item_descendent_needs_reflow(item->parent);
}

void
e_canvas_item_request_reflow (GnomeCanvasItem *item)
{
	if (item->object.flags & GNOME_CANVAS_ITEM_REALIZED) {
		item->object.flags |= E_CANVAS_ITEM_NEEDS_REFLOW;
		e_canvas_item_descendent_needs_reflow(item);
		add_idle(E_CANVAS(item->canvas));
	}
}

void
e_canvas_item_request_parent_reflow (GnomeCanvasItem *item)
{
	g_return_if_fail(item != NULL);
	g_return_if_fail(GNOME_IS_CANVAS_ITEM(item));
	e_canvas_item_request_reflow(item->parent);
}

void
e_canvas_item_set_reflow_callback (GnomeCanvasItem *item, ECanvasItemReflowFunc func)
{
	gtk_object_set_data(GTK_OBJECT(item), "ECanvasItem::reflow_callback", (gpointer) func);
}


void
e_canvas_item_set_selection_callback (GnomeCanvasItem *item, ECanvasItemSelectionFunc func)
{
	gtk_object_set_data(GTK_OBJECT(item), "ECanvasItem::selection_callback", (gpointer) func);
}

void
e_canvas_item_set_selection_compare_callback (GnomeCanvasItem *item, ECanvasItemSelectionCompareFunc func)
{
	gtk_object_set_data(GTK_OBJECT(item), "ECanvasItem::selection_compare_callback", (gpointer) func);
}

void
e_canvas_item_set_cursor (GnomeCanvasItem *item, gpointer id)
{
	GList *list;
	int flags;
	ECanvas *canvas;
	ECanvasSelectionInfo *info;
	ECanvasItemSelectionFunc func;

	g_return_if_fail(item != NULL);
	g_return_if_fail(GNOME_IS_CANVAS_ITEM(item));
	g_return_if_fail(item->canvas != NULL);
	g_return_if_fail(E_IS_CANVAS(item->canvas));

	canvas = E_CANVAS(item->canvas);
	flags = E_CANVAS_ITEM_SELECTION_DELETE_DATA;

	for (list = canvas->selection; list; list = g_list_next(list)) {
		info = list->data;

		func = gtk_object_get_data(GTK_OBJECT(info->item), "ECanvasItem::selection_callback");
		if (func)
			func(info->item, flags, info->id);
		g_message ("ECANVAS: free info (2): item %p, id %p",
			   info->item, info->id);
		gtk_object_unref (GTK_OBJECT (info->item));
		g_free(info);
	}
	g_list_free(canvas->selection);

	canvas->selection = NULL;

	gnome_canvas_item_grab_focus(item);

	info = g_new(ECanvasSelectionInfo, 1);
	info->item = item;
	gtk_object_ref (GTK_OBJECT (info->item));
	info->id = id;
	g_message ("ECANVAS: new info item %p, id %p", item, id);

	flags = E_CANVAS_ITEM_SELECTION_SELECT | E_CANVAS_ITEM_SELECTION_CURSOR;
	func = gtk_object_get_data(GTK_OBJECT(item), "ECanvasItem::selection_callback");
	if (func)
		func(item, flags, id);

	canvas->selection = g_list_prepend(canvas->selection, info);
	canvas->cursor = info;
}

void
e_canvas_item_set_cursor_end (GnomeCanvasItem *item, gpointer id)
{
}

void
e_canvas_item_add_selection (GnomeCanvasItem *item, gpointer id)
{
	int flags;
	ECanvas *canvas;
	ECanvasSelectionInfo *info;
	ECanvasItemSelectionFunc func;
	GList *list;

	g_return_if_fail(item != NULL);
	g_return_if_fail(GNOME_IS_CANVAS_ITEM(item));
	g_return_if_fail(item->canvas != NULL);
	g_return_if_fail(E_IS_CANVAS(item->canvas));

	flags = E_CANVAS_ITEM_SELECTION_SELECT;
	canvas = E_CANVAS(item->canvas);

	if (canvas->cursor) {
		func = gtk_object_get_data(GTK_OBJECT(canvas->cursor->item), "ECanvasItem::selection_callback");
		if (func)
			func(canvas->cursor->item, flags, canvas->cursor->id);
	}

	gnome_canvas_item_grab_focus(item);

	flags = E_CANVAS_ITEM_SELECTION_SELECT | E_CANVAS_ITEM_SELECTION_CURSOR;

	for (list = canvas->selection; list; list = g_list_next(list)) {
		ECanvasSelectionInfo *search;
		search = list->data;

		if (search->item == item) {
			ECanvasItemSelectionCompareFunc compare_func;
			compare_func = gtk_object_get_data(GTK_OBJECT(search->item), "ECanvasItem::selection_compare_callback");

			if (compare_func(search->item, search->id, id, 0) == 0) {
				canvas->cursor = search;
				func = gtk_object_get_data(GTK_OBJECT(item), "ECanvasItem::selection_callback");
				if (func)
					func(item, flags, search->id);
				return;
			}
		}
	}

	info = g_new(ECanvasSelectionInfo, 1);
	info->item = item;
	gtk_object_ref (GTK_OBJECT (info->item));
	info->id = id;
	g_message ("ECANVAS: new info (2): item %p, id %p", item, id);

	func = gtk_object_get_data(GTK_OBJECT(item), "ECanvasItem::selection_callback");
	if (func)
		func(item, flags, id);

	canvas->selection = g_list_prepend(canvas->selection, info);
	canvas->cursor = info;
}

void
e_canvas_item_remove_selection (GnomeCanvasItem *item, gpointer id)
{
	int flags;
	ECanvas *canvas;
	ECanvasSelectionInfo *info;
	GList *list;

	g_return_if_fail(item != NULL);
	g_return_if_fail(GNOME_IS_CANVAS_ITEM(item));
	g_return_if_fail(item->canvas != NULL);
	g_return_if_fail(E_IS_CANVAS(item->canvas));

	flags = E_CANVAS_ITEM_SELECTION_DELETE_DATA;
	canvas = E_CANVAS(item->canvas);

	for (list = canvas->selection; list; list = g_list_next(list)) {
		info = list->data;

		if (info->item == item) {
			ECanvasItemSelectionCompareFunc compare_func;
			compare_func = gtk_object_get_data(GTK_OBJECT(info->item), "ECanvasItem::selection_compare_callback");

			if (compare_func(info->item, info->id, id, 0) == 0) {
				ECanvasItemSelectionFunc func;
				func = gtk_object_get_data(GTK_OBJECT(info->item), "ECanvasItem::selection_callback");
				if (func)
					func(info->item, flags, info->id);
				canvas->selection = g_list_remove_link(canvas->selection, list);

				if (canvas->cursor == info)
					canvas->cursor = NULL;

				g_message ("ECANVAS: removing info: item %p, info %p",
					   info->item, info->id);
				gtk_object_unref (GTK_OBJECT (info->item));
				g_free(info);
				g_list_free_1(list);
				break;
			}
		}
	}
}
