/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * e-canvas-vbox.c
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

#include <config.h>
#include <gnome.h>
#include <math.h>
#include "e-canvas-vbox.h"
#include <e-util/e-canvas-utils.h>
#include <e-util/e-canvas.h>
#include <e-util/e-util.h>

static void e_canvas_vbox_init		(ECanvasVbox		 *CanvasVbox);
static void e_canvas_vbox_class_init	(ECanvasVboxClass	 *klass);
static void e_canvas_vbox_set_arg (GtkObject *o, GtkArg *arg, guint arg_id);
static void e_canvas_vbox_get_arg (GtkObject *object, GtkArg *arg, guint arg_id);
static void e_canvas_vbox_destroy (GtkObject *object);

static gint e_canvas_vbox_event   (GnomeCanvasItem *item, GdkEvent *event);
static void e_canvas_vbox_realize (GnomeCanvasItem *item);

static void e_canvas_vbox_reflow (GnomeCanvasItem *item, int flags);

static void e_canvas_vbox_real_add_item(ECanvasVbox *e_canvas_vbox, GnomeCanvasItem *item);
static void e_canvas_vbox_resize_children (GnomeCanvasItem *item);

static GnomeCanvasGroupClass *parent_class = NULL;

/* The arguments we take */
enum {
	ARG_0,
	ARG_WIDTH,
	ARG_HEIGHT,
	ARG_SPACING,
};

GtkType
e_canvas_vbox_get_type (void)
{
  static GtkType type = 0;

  if (!type)
    {
      static const GtkTypeInfo info =
      {
        "ECanvasVbox",
        sizeof (ECanvasVbox),
        sizeof (ECanvasVboxClass),
        (GtkClassInitFunc) e_canvas_vbox_class_init,
        (GtkObjectInitFunc) e_canvas_vbox_init,
        /* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      type = gtk_type_unique (gnome_canvas_group_get_type (), &info);
    }

  return type;
}

static void
e_canvas_vbox_class_init (ECanvasVboxClass *klass)
{
	GtkObjectClass *object_class;
	GnomeCanvasItemClass *item_class;

	object_class = (GtkObjectClass*) klass;
	item_class = (GnomeCanvasItemClass *) klass;

	parent_class = gtk_type_class (gnome_canvas_group_get_type ());
  
	gtk_object_add_arg_type ("ECanvasVbox::width", GTK_TYPE_DOUBLE, 
				 GTK_ARG_READWRITE, ARG_WIDTH); 
	gtk_object_add_arg_type ("ECanvasVbox::height", GTK_TYPE_DOUBLE, 
				 GTK_ARG_READABLE, ARG_HEIGHT);
	gtk_object_add_arg_type ("ECanvasVbox::spacing", GTK_TYPE_DOUBLE, 
				 GTK_ARG_READWRITE, ARG_SPACING);

	klass->add_item       = e_canvas_vbox_real_add_item;
 
	object_class->set_arg   = e_canvas_vbox_set_arg;
	object_class->get_arg   = e_canvas_vbox_get_arg;
	object_class->destroy   = e_canvas_vbox_destroy;
  
	/* GnomeCanvasItem method overrides */
	item_class->event       = e_canvas_vbox_event;
	item_class->realize     = e_canvas_vbox_realize;
}

static void
e_canvas_vbox_init (ECanvasVbox *vbox)
{
	vbox->items = NULL;

	vbox->width = 10;
	vbox->height = 10;
	vbox->spacing = 0;

	e_canvas_item_set_reflow_callback(GNOME_CANVAS_ITEM(vbox), e_canvas_vbox_reflow);
}

static void
e_canvas_vbox_set_arg (GtkObject *o, GtkArg *arg, guint arg_id)
{
	GnomeCanvasItem *item;
	ECanvasVbox *e_canvas_vbox;

	item = GNOME_CANVAS_ITEM (o);
	e_canvas_vbox = E_CANVAS_VBOX (o);
	
	switch (arg_id){
	case ARG_WIDTH:
		e_canvas_vbox->width = GTK_VALUE_DOUBLE (*arg);
		e_canvas_vbox_resize_children(item);
		e_canvas_item_request_reflow(item);
		break;
	case ARG_SPACING:
		e_canvas_vbox->spacing = GTK_VALUE_DOUBLE (*arg);
		e_canvas_item_request_reflow(item);
		break;
	}
}

static void
e_canvas_vbox_get_arg (GtkObject *object, GtkArg *arg, guint arg_id)
{
	ECanvasVbox *e_canvas_vbox;

	e_canvas_vbox = E_CANVAS_VBOX (object);

	switch (arg_id) {
	case ARG_WIDTH:
		GTK_VALUE_DOUBLE (*arg) = e_canvas_vbox->width;
		break;
	case ARG_HEIGHT:
		GTK_VALUE_DOUBLE (*arg) = e_canvas_vbox->height;
		break;
	case ARG_SPACING:
		GTK_VALUE_DOUBLE (*arg) = e_canvas_vbox->spacing;
		break;
	default:
		arg->type = GTK_TYPE_INVALID;
		break;
	}
}

static void
e_canvas_vbox_destroy (GtkObject *object)
{
	ECanvasVbox *vbox = E_CANVAS_VBOX(object);

	g_list_foreach(vbox->items, (GFunc) gtk_object_unref, NULL);
	g_list_free(vbox->items);
	vbox->items = NULL;
	
	GTK_OBJECT_CLASS(parent_class)->destroy (object);
}

static gint
e_canvas_vbox_event (GnomeCanvasItem *item, GdkEvent *event)
{
	gint return_val = TRUE;

	switch (event->type) {
	case GDK_KEY_PRESS:
		switch (event->key.keyval) {
		case GDK_Tab:
		case GDK_KP_Tab:
		case GDK_ISO_Left_Tab:
		case GDK_Left:
		case GDK_KP_Left:
		case GDK_Right:
		case GDK_KP_Right:
		case GDK_Down:
		case GDK_KP_Down:
		case GDK_Up:
		case GDK_KP_Up:
		case GDK_Return:
		case GDK_KP_Enter:
			return_val = TRUE;
			break;
		default:
			return_val = FALSE;
			break;
		}
		break;
	default:
		return_val = FALSE;
		break;
	}
	if (!return_val) {
		if (GNOME_CANVAS_ITEM_CLASS(parent_class)->event)
			return GNOME_CANVAS_ITEM_CLASS (parent_class)->event (item, event);
	}
	return return_val;
	
}

static void
e_canvas_vbox_realize (GnomeCanvasItem *item)
{
	if (GNOME_CANVAS_ITEM_CLASS(parent_class)->realize)
		(* GNOME_CANVAS_ITEM_CLASS(parent_class)->realize) (item);
	
	e_canvas_vbox_resize_children(item);
	e_canvas_item_request_reflow(item);
}

static void
e_canvas_vbox_remove_item (GnomeCanvasItem *item, ECanvasVbox *vbox)
{
	vbox->items = g_list_remove(vbox->items, item);
	gtk_object_unref(GTK_OBJECT(vbox));
}

static void
e_canvas_vbox_real_add_item(ECanvasVbox *e_canvas_vbox, GnomeCanvasItem *item)
{
	e_canvas_vbox->items = g_list_append(e_canvas_vbox->items, item);
	gtk_object_ref(GTK_OBJECT(item));
	gtk_object_ref(GTK_OBJECT(e_canvas_vbox));
	gtk_signal_connect(GTK_OBJECT(item), "destroy",
			   GTK_SIGNAL_FUNC(e_canvas_vbox_remove_item), e_canvas_vbox);
	if ( GTK_OBJECT_FLAGS( e_canvas_vbox ) & GNOME_CANVAS_ITEM_REALIZED ) {
		gnome_canvas_item_set(item,
				      "width", (double) e_canvas_vbox->width,
				      NULL);
		e_canvas_item_request_reflow(item);
	}

}

static void
e_canvas_vbox_resize_children (GnomeCanvasItem *item)
{
	GList *list;
	ECanvasVbox *e_canvas_vbox;

	e_canvas_vbox = E_CANVAS_VBOX (item);
	for ( list = e_canvas_vbox->items; list; list = list->next ) {
		GnomeCanvasItem *child = GNOME_CANVAS_ITEM(list->data);
		gnome_canvas_item_set(child,
				      "width", (double) e_canvas_vbox->width,
				      NULL);
	}
}

static void
e_canvas_vbox_reflow( GnomeCanvasItem *item, int flags )
{
	ECanvasVbox *e_canvas_vbox = E_CANVAS_VBOX(item);
	if ( GTK_OBJECT_FLAGS( e_canvas_vbox ) & GNOME_CANVAS_ITEM_REALIZED ) {

		gdouble old_height;
		gdouble running_height;

		old_height = e_canvas_vbox->height;
		
		running_height = 0;

		if (e_canvas_vbox->items == NULL) {
		} else {
			GList *list;
			gdouble item_height;

			list = e_canvas_vbox->items;
			gtk_object_get (GTK_OBJECT(list->data),
					"height", &item_height,
					NULL);
			e_canvas_item_move_absolute(GNOME_CANVAS_ITEM(list->data),
						    (double) 0,
						    (double) running_height);
			running_height += item_height;
			list = g_list_next(list);
			
			for( ; list; list = g_list_next(list)) {
				running_height += e_canvas_vbox->spacing;

				gtk_object_get (GTK_OBJECT(list->data),
						"height", &item_height,
						NULL);

				e_canvas_item_move_absolute(GNOME_CANVAS_ITEM(list->data),
							    (double) 0,
							    (double) running_height);

				running_height += item_height;
			}
				 
		}
		e_canvas_vbox->height = running_height;
		if (old_height != e_canvas_vbox->height)
			e_canvas_item_request_parent_reflow(item);
	}
}

void
e_canvas_vbox_add_item(ECanvasVbox *e_canvas_vbox, GnomeCanvasItem *item)
{
	if (E_CANVAS_VBOX_CLASS(GTK_OBJECT(e_canvas_vbox)->klass)->add_item)
		(E_CANVAS_VBOX_CLASS(GTK_OBJECT(e_canvas_vbox)->klass)->add_item) (e_canvas_vbox, item);
}
