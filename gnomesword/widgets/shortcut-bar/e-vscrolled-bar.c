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
 * EVScrolledBar is like GtkScrolledWindow but only scrolls the child widget
 * vertically. It is intended for scrolling narrow vertical bars.
 */

#include <config.h>
#include <gtk/gtkarrow.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtksignal.h>
#include "e-vscrolled-bar.h"

/* These are the offsets of the up & down buttons from the right and top/bottom
   of the widget. */
#define E_VSCROLLED_BAR_BUTTON_X_OFFSET	2
#define E_VSCROLLED_BAR_BUTTON_Y_OFFSET	2

/* This is the time between scrolls. */
#define E_VSCROLLED_BAR_SCROLL_TIMEOUT 20

static void e_vscrolled_bar_class_init (EVScrolledBarClass *class);
static void e_vscrolled_bar_init (EVScrolledBar *vscrolled_bar);
static void e_vscrolled_bar_destroy (GtkObject *object);
static void e_vscrolled_bar_finalize (GtkObject *object);
static void e_vscrolled_bar_map (GtkWidget *widget);
static void e_vscrolled_bar_unmap (GtkWidget *widget);
static void e_vscrolled_bar_size_request (GtkWidget      *widget,
					  GtkRequisition *requisition);
static void e_vscrolled_bar_size_allocate (GtkWidget *widget,
					   GtkAllocation *allocation);
static void e_vscrolled_bar_draw (GtkWidget    *widget,
				  GdkRectangle *area);
static gint e_vscrolled_bar_button_press   (GtkWidget        *widget,
					    GdkEventButton   *event);
static void e_vscrolled_bar_add (GtkContainer *container,
				 GtkWidget    *child);
static void e_vscrolled_bar_remove (GtkContainer *container,
				    GtkWidget    *child);
static void e_vscrolled_bar_forall (GtkContainer *container,
				    gboolean	  include_internals,
				    GtkCallback   callback,
				    gpointer      callback_data);
static void e_vscrolled_bar_adjustment_changed (GtkAdjustment *adjustment,
						gpointer       data);
static void e_vscrolled_bar_button_pressed (GtkWidget *button,
					    EVScrolledBar *vscrolled_bar);
static void e_vscrolled_bar_button_released (GtkWidget *button,
					     EVScrolledBar *vscrolled_bar);
static void e_vscrolled_bar_button_clicked (GtkWidget *button,
					    EVScrolledBar *vscrolled_bar);
static gboolean e_vscrolled_bar_timeout_handler (gpointer data);


static GtkBinClass *parent_class;


GtkType
e_vscrolled_bar_get_type (void)
{
	static GtkType e_vscrolled_bar_type = 0;

	if (!e_vscrolled_bar_type) {
		GtkTypeInfo e_vscrolled_bar_info = {
			"EVScrolledBar",
			sizeof (EVScrolledBar),
			sizeof (EVScrolledBarClass),
			(GtkClassInitFunc) e_vscrolled_bar_class_init,
			(GtkObjectInitFunc) e_vscrolled_bar_init,
			NULL, /* reserved 1 */
			NULL, /* reserved 2 */
			(GtkClassInitFunc) NULL
		};

		parent_class = gtk_type_class (GTK_TYPE_BIN);
		e_vscrolled_bar_type = gtk_type_unique (GTK_TYPE_BIN,
							&e_vscrolled_bar_info);
	}

	return e_vscrolled_bar_type;
}


static void
e_vscrolled_bar_class_init (EVScrolledBarClass *class)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;
	GtkContainerClass *container_class;

	object_class = (GtkObjectClass *) class;
	widget_class = (GtkWidgetClass *) class;
	container_class = (GtkContainerClass *) class;

	/* Method override */
	object_class->destroy		= e_vscrolled_bar_destroy;
	object_class->finalize		= e_vscrolled_bar_finalize;

	widget_class->map		= e_vscrolled_bar_map;
	widget_class->unmap		= e_vscrolled_bar_unmap;
	widget_class->size_request	= e_vscrolled_bar_size_request;
 	widget_class->size_allocate	= e_vscrolled_bar_size_allocate;
	widget_class->draw		= e_vscrolled_bar_draw;
	widget_class->button_press_event   = e_vscrolled_bar_button_press;

	container_class->add		= e_vscrolled_bar_add;
	container_class->remove		= e_vscrolled_bar_remove;
	container_class->forall		= e_vscrolled_bar_forall;
}


static void
e_vscrolled_bar_init (EVScrolledBar *vscrolled_bar)
{
	GtkWidget *arrow;

	GTK_WIDGET_SET_FLAGS (vscrolled_bar, GTK_NO_WINDOW);

	gtk_container_set_resize_mode (GTK_CONTAINER (vscrolled_bar),
				       GTK_RESIZE_QUEUE);

	gtk_widget_push_composite_child ();

	vscrolled_bar->up_button = gtk_button_new ();
	gtk_widget_set_composite_name (vscrolled_bar->up_button,
				       "up_button");
	gtk_widget_set_parent (vscrolled_bar->up_button,
			       GTK_WIDGET (vscrolled_bar));
	arrow = gtk_arrow_new (GTK_ARROW_UP, GTK_SHADOW_OUT);
	gtk_misc_set_padding (GTK_MISC (arrow), 1, 1);
	gtk_widget_show (arrow);
	gtk_container_add (GTK_CONTAINER (vscrolled_bar->up_button), arrow);
	gtk_widget_show (vscrolled_bar->up_button);
	gtk_signal_connect_after (GTK_OBJECT (vscrolled_bar->up_button), "pressed", GTK_SIGNAL_FUNC (e_vscrolled_bar_button_pressed), vscrolled_bar);
	gtk_signal_connect_after (GTK_OBJECT (vscrolled_bar->up_button), "released", GTK_SIGNAL_FUNC (e_vscrolled_bar_button_released), vscrolled_bar);
	gtk_signal_connect (GTK_OBJECT (vscrolled_bar->up_button), "clicked", GTK_SIGNAL_FUNC (e_vscrolled_bar_button_clicked), vscrolled_bar);

	vscrolled_bar->down_button = gtk_button_new ();
	gtk_widget_set_composite_name (vscrolled_bar->up_button,
				       "down_button");
	gtk_widget_set_parent (vscrolled_bar->down_button,
			       GTK_WIDGET (vscrolled_bar));
	arrow = gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_OUT);
	gtk_misc_set_padding (GTK_MISC (arrow), 1, 1);
	gtk_widget_show (arrow);
	gtk_container_add (GTK_CONTAINER (vscrolled_bar->down_button), arrow);
	gtk_widget_show (vscrolled_bar->down_button);
	gtk_signal_connect_after (GTK_OBJECT (vscrolled_bar->down_button), "pressed", GTK_SIGNAL_FUNC (e_vscrolled_bar_button_pressed), vscrolled_bar);
	gtk_signal_connect_after (GTK_OBJECT (vscrolled_bar->down_button), "released", GTK_SIGNAL_FUNC (e_vscrolled_bar_button_released), vscrolled_bar);
	gtk_signal_connect (GTK_OBJECT (vscrolled_bar->down_button), "clicked", GTK_SIGNAL_FUNC (e_vscrolled_bar_button_clicked), vscrolled_bar);

	gtk_widget_pop_composite_child ();

	vscrolled_bar->adjustment = NULL;
	vscrolled_bar->timeout_id = 0;
	vscrolled_bar->scrolling_up = FALSE;
	vscrolled_bar->min_distance = -1.0;
	vscrolled_bar->button_pressed = FALSE;
}


/**
 * e_vscrolled_bar_new:
 *
 * @adjustment: The #GtkAdjustment to use for scrolling, or NULL.
 * @Return: A new #EVScrolledBar.
 *
 * Creates a new #EVScrolledBar with the given adjustment.
 **/
GtkWidget *
e_vscrolled_bar_new (GtkAdjustment *adjustment)
{
	GtkWidget *vscrolled_bar;

	vscrolled_bar = GTK_WIDGET (gtk_type_new (e_vscrolled_bar_get_type ()));
	e_vscrolled_bar_set_adjustment (E_VSCROLLED_BAR (vscrolled_bar),
					adjustment);

	return vscrolled_bar;
}


static void
e_vscrolled_bar_destroy (GtkObject *object)
{
	EVScrolledBar *vscrolled_bar;

	vscrolled_bar = E_VSCROLLED_BAR (object);

	if (vscrolled_bar->timeout_id) {
		g_source_remove (vscrolled_bar->timeout_id);
		vscrolled_bar->timeout_id = 0;
	}

	gtk_widget_unparent (vscrolled_bar->up_button);
	gtk_widget_unparent (vscrolled_bar->down_button);

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}


static void
e_vscrolled_bar_finalize (GtkObject *object)
{
	EVScrolledBar *vscrolled_bar;

	vscrolled_bar = E_VSCROLLED_BAR (object);

	gtk_object_unref (GTK_OBJECT (vscrolled_bar->adjustment));

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}


static void
e_vscrolled_bar_map (GtkWidget *widget)
{
	EVScrolledBar *vscrolled_bar;
	GtkAdjustment *adjustment;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (E_IS_VSCROLLED_BAR (widget));

	vscrolled_bar = E_VSCROLLED_BAR (widget);

	/* chain parent class handler to map self and child */
	GTK_WIDGET_CLASS (parent_class)->map (widget);
  
	adjustment = vscrolled_bar->adjustment;

	if (GTK_WIDGET_VISIBLE (vscrolled_bar->up_button)
	    && adjustment->value != adjustment->lower
	    && !GTK_WIDGET_MAPPED (vscrolled_bar->up_button))
		gtk_widget_map (vscrolled_bar->up_button);
  
	if (GTK_WIDGET_VISIBLE (vscrolled_bar->down_button)
	    && adjustment->value < adjustment->upper - adjustment->page_size
	    && !GTK_WIDGET_MAPPED (vscrolled_bar->down_button))
		gtk_widget_map (vscrolled_bar->down_button);
}


static void
e_vscrolled_bar_unmap (GtkWidget *widget)
{
	EVScrolledBar *vscrolled_bar;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (E_IS_VSCROLLED_BAR (widget));

	vscrolled_bar = E_VSCROLLED_BAR (widget);

	/* chain parent class handler to unmap self and child */
	GTK_WIDGET_CLASS (parent_class)->unmap (widget);
  
	if (GTK_WIDGET_MAPPED (vscrolled_bar->up_button))
		gtk_widget_unmap (vscrolled_bar->up_button);
  
	if (GTK_WIDGET_MAPPED (vscrolled_bar->down_button))
		gtk_widget_unmap (vscrolled_bar->down_button);
}


static void
e_vscrolled_bar_size_request (GtkWidget      *widget,
			      GtkRequisition *requisition)
{
	EVScrolledBar *vscrolled_bar;
	GtkBin *bin;
	GtkRequisition child_requisition;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (E_IS_VSCROLLED_BAR (widget));
	g_return_if_fail (requisition != NULL);

	vscrolled_bar = E_VSCROLLED_BAR (widget);
	bin = GTK_BIN (widget);

	requisition->width = 0;
	requisition->height = 0;

	/* We just return the requisition of the child widget, plus the
	   border width. */
	if (bin->child && GTK_WIDGET_VISIBLE (bin->child)) {
		gtk_widget_size_request (bin->child, &child_requisition);
		*requisition = child_requisition;
	}

	/* We remember the requested heights of the up & down buttons. */
	gtk_widget_size_request (vscrolled_bar->up_button,
				 &child_requisition);
	vscrolled_bar->up_button_width = child_requisition.width;
	vscrolled_bar->up_button_height = child_requisition.height;

	gtk_widget_size_request (vscrolled_bar->down_button,
				 &child_requisition);
	vscrolled_bar->down_button_width = child_requisition.width;
	vscrolled_bar->down_button_height = child_requisition.height;

	/* Add on the standard container border widths. */
	requisition->width += GTK_CONTAINER (widget)->border_width * 2;
	requisition->height += GTK_CONTAINER (widget)->border_width * 2;
}


static void
e_vscrolled_bar_size_allocate (GtkWidget *widget,
			       GtkAllocation *allocation)
{
	EVScrolledBar *vscrolled_bar;
	GtkBin *bin;
	GtkAllocation button_allocation, child_allocation;
	gint border_width;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (E_IS_VSCROLLED_BAR (widget));
	g_return_if_fail (allocation != NULL);

	vscrolled_bar = E_VSCROLLED_BAR (widget);
	bin = GTK_BIN (widget);

	widget->allocation = *allocation;

	border_width = GTK_CONTAINER (widget)->border_width;

	child_allocation.x = border_width;
	child_allocation.y = border_width;
	child_allocation.width = allocation->width - 2 * border_width;
	child_allocation.height = allocation->height - 2 * border_width;
	gtk_widget_size_allocate (bin->child, &child_allocation);

	button_allocation.x = allocation->width - border_width
		- vscrolled_bar->up_button_width
		- E_VSCROLLED_BAR_BUTTON_X_OFFSET;
	button_allocation.y = border_width + E_VSCROLLED_BAR_BUTTON_Y_OFFSET;
	button_allocation.width = vscrolled_bar->up_button_width;
	button_allocation.height = vscrolled_bar->up_button_height;
	gtk_widget_size_allocate (vscrolled_bar->up_button,
				  &button_allocation);

	button_allocation.x = allocation->width - border_width
		- vscrolled_bar->down_button_width
		- E_VSCROLLED_BAR_BUTTON_X_OFFSET;
	button_allocation.y = allocation->height - border_width
		- vscrolled_bar->down_button_height
		- E_VSCROLLED_BAR_BUTTON_Y_OFFSET;
	button_allocation.width = vscrolled_bar->down_button_width;
	button_allocation.height = vscrolled_bar->down_button_height;
	gtk_widget_size_allocate (vscrolled_bar->down_button,
				  &button_allocation);
}


static void
e_vscrolled_bar_draw (GtkWidget    *widget,
		      GdkRectangle *area)
{
	EVScrolledBar *vscrolled_bar;
	GtkBin *bin;
	GdkRectangle child_area;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (E_IS_VSCROLLED_BAR (widget));
	g_return_if_fail (area != NULL);
  
	vscrolled_bar = E_VSCROLLED_BAR (widget);
	bin = GTK_BIN (widget);

	if (bin->child && GTK_WIDGET_VISIBLE (bin->child) &&
	    gtk_widget_intersect (bin->child, area, &child_area))
		gtk_widget_draw (bin->child, &child_area);
  
	if (GTK_WIDGET_DRAWABLE (vscrolled_bar->up_button) &&
	    gtk_widget_intersect (vscrolled_bar->up_button, area, &child_area))
		gtk_widget_draw (vscrolled_bar->up_button, &child_area);
  
	if (GTK_WIDGET_DRAWABLE (vscrolled_bar->down_button) &&
	    gtk_widget_intersect (vscrolled_bar->down_button, area, &child_area))
		gtk_widget_draw (vscrolled_bar->down_button, &child_area);
}


static gint
e_vscrolled_bar_button_press (GtkWidget      *widget,
			      GdkEventButton *event)
{
	EVScrolledBar *vscrolled_bar;
	GtkAdjustment *adjustment;
	gfloat new_value, step;

	vscrolled_bar = E_VSCROLLED_BAR (widget);
	adjustment = vscrolled_bar->adjustment;

	step = adjustment->page_size;

	if (event->button == 4) {
		new_value = adjustment->value - step;
		if (new_value <= adjustment->lower) {
			new_value = adjustment->lower;
		}
	} else if (event->button == 5) {
		new_value = adjustment->value + step;
		if (new_value >= adjustment->upper - adjustment->page_size) {
			new_value = adjustment->upper - adjustment->page_size;
		}
	} else
		return FALSE;

	if (adjustment->value != new_value) {
		adjustment->value = new_value;
		gtk_signal_emit_by_name (GTK_OBJECT (adjustment),
					 "value_changed");
		return TRUE;
	}

	return FALSE;
}


static void
e_vscrolled_bar_add (GtkContainer *container,
		     GtkWidget    *child)
{
	EVScrolledBar *vscrolled_bar;
	GtkBin *bin;

	g_return_if_fail (container != NULL);
	g_return_if_fail (E_IS_VSCROLLED_BAR (container));
  
	vscrolled_bar = E_VSCROLLED_BAR (container);
	bin = GTK_BIN (container);

	g_return_if_fail (bin->child == NULL);

	bin->child = child;
	gtk_widget_set_parent (child, GTK_WIDGET (bin));

	gtk_widget_set_scroll_adjustments (child, NULL,
					   vscrolled_bar->adjustment);

	if (GTK_WIDGET_REALIZED (child->parent))
		gtk_widget_realize (child);

	if (GTK_WIDGET_VISIBLE (child->parent) && GTK_WIDGET_VISIBLE (child)) {
		if (GTK_WIDGET_MAPPED (child->parent))
			gtk_widget_map (child);

		gtk_widget_queue_resize (child);
	}
}


static void
e_vscrolled_bar_remove (GtkContainer *container,
			GtkWidget    *child)
{
	g_return_if_fail (container != NULL);
	g_return_if_fail (E_IS_VSCROLLED_BAR (container));
	g_return_if_fail (child != NULL);
	g_return_if_fail (GTK_BIN (container)->child == child);

	gtk_widget_set_scroll_adjustments (child, NULL, NULL);

	/* chain parent class handler to remove child */
	GTK_CONTAINER_CLASS (parent_class)->remove (container, child);
}


static void
e_vscrolled_bar_forall (GtkContainer *container,
			gboolean      include_internals,
			GtkCallback   callback,
			gpointer      callback_data)
{
	g_return_if_fail (container != NULL);
	g_return_if_fail (E_IS_VSCROLLED_BAR (container));
	g_return_if_fail (callback != NULL);

	GTK_CONTAINER_CLASS (parent_class)->forall (container,
						    include_internals,
						    callback,
						    callback_data);
	if (include_internals) {
		EVScrolledBar *vscrolled_bar;

		vscrolled_bar = E_VSCROLLED_BAR (container);
      
		if (vscrolled_bar->up_button)
			callback (vscrolled_bar->up_button, callback_data);
		if (vscrolled_bar->down_button)
			callback (vscrolled_bar->down_button, callback_data);
	}
}


/**
 * e_vscrolled_bar_get_adjustment:
 *
 * @vscrolled_bar: An #EVScrolledBar.
 * @Return: The #GtkAdjustment used for scrolling @vscrolled_bar.
 *
 * Returns the #GtkAdjustment used for scrolling the #EVscrolledBar.
 **/
GtkAdjustment*
e_vscrolled_bar_get_adjustment (EVScrolledBar *vscrolled_bar)
{
	g_return_val_if_fail (vscrolled_bar != NULL, NULL);
	g_return_val_if_fail (E_IS_VSCROLLED_BAR (vscrolled_bar), NULL);

	return vscrolled_bar->adjustment;
}


/**
 * e_vscrolled_bar_set_adjustment:
 *
 * @vscrolled_bar: An #EVScrolledBar.
 * @adjustment: The #GtkAdjustment to use for scrolling @vscrolled_bar.
 *
 * Sets the #GtkAdjustment to use for scrolling the #EVscrolledBar.
 **/
void
e_vscrolled_bar_set_adjustment (EVScrolledBar     *vscrolled_bar,
				GtkAdjustment     *adjustment)
{
	g_return_if_fail (vscrolled_bar != NULL);
	g_return_if_fail (E_IS_VSCROLLED_BAR (vscrolled_bar));

	if (adjustment)
		g_return_if_fail (GTK_IS_ADJUSTMENT (adjustment));
	else
		adjustment = (GtkAdjustment*) gtk_object_new (GTK_TYPE_ADJUSTMENT, NULL);

	if (vscrolled_bar->adjustment == adjustment)
		return;

	if (vscrolled_bar->adjustment) {
		gtk_signal_disconnect_by_func (GTK_OBJECT (vscrolled_bar->adjustment),
					       GTK_SIGNAL_FUNC (e_vscrolled_bar_adjustment_changed),
					       vscrolled_bar);
		gtk_object_unref (GTK_OBJECT (vscrolled_bar->adjustment));
	}

	vscrolled_bar->adjustment = adjustment;
	gtk_object_ref (GTK_OBJECT (vscrolled_bar->adjustment));
	gtk_object_sink (GTK_OBJECT (vscrolled_bar->adjustment));

	/* I've used connect_after here to avoid a problem when using a
	   GnomeCanvas as the child widget. When just using connect it would
	   leave a blank space when one of the buttons is hidden. We want
	   the GtkLayout to handle the scrolling before we hide any buttons. */
	gtk_signal_connect_after (GTK_OBJECT (adjustment), "changed",
				  GTK_SIGNAL_FUNC (e_vscrolled_bar_adjustment_changed),
				  vscrolled_bar);
	gtk_signal_connect_after (GTK_OBJECT (adjustment), "value_changed",
				  GTK_SIGNAL_FUNC (e_vscrolled_bar_adjustment_changed),
				  vscrolled_bar);

	e_vscrolled_bar_adjustment_changed (adjustment, vscrolled_bar);

	if (GTK_BIN (vscrolled_bar)->child)
		gtk_widget_set_scroll_adjustments (GTK_BIN (vscrolled_bar)->child, NULL, adjustment);
}


static void
e_vscrolled_bar_adjustment_changed (GtkAdjustment *adjustment,
				    gpointer       data)
{
	EVScrolledBar *vscrolled_bar;

	g_return_if_fail (adjustment != NULL);
	g_return_if_fail (data != NULL);
#if 0
	g_print ("Adjustment changed to: %g\n", adjustment->value);
#endif
	vscrolled_bar = E_VSCROLLED_BAR (data);

	if (!GTK_WIDGET_MAPPED (vscrolled_bar))
		return;

	/* If the adjustment value is not 0, show the up button. */
	if (adjustment->value != adjustment->lower) {
		gtk_widget_map (vscrolled_bar->up_button);
		gdk_window_raise (vscrolled_bar->up_button->window);
	} else {
		gtk_widget_unmap (vscrolled_bar->up_button);
	}

	/* If the adjustment value is less than the maximum value, show the
	   down button. */
	if (adjustment->value < adjustment->upper - adjustment->page_size) {
		gtk_widget_map (vscrolled_bar->down_button);
		gdk_window_raise (vscrolled_bar->down_button->window);
	} else {
		gtk_widget_unmap (vscrolled_bar->down_button);
	}
}


static void
e_vscrolled_bar_button_pressed (GtkWidget *button,
				EVScrolledBar *vscrolled_bar)
{
	if (vscrolled_bar->timeout_id != 0)
		g_source_remove (vscrolled_bar->timeout_id);

	vscrolled_bar->timeout_id = g_timeout_add (E_VSCROLLED_BAR_SCROLL_TIMEOUT, e_vscrolled_bar_timeout_handler, vscrolled_bar);
	vscrolled_bar->scrolling_up = (button == vscrolled_bar->up_button) ? TRUE : FALSE;
	vscrolled_bar->min_distance = vscrolled_bar->adjustment->page_size / 4;
	vscrolled_bar->button_pressed = TRUE;

	e_vscrolled_bar_timeout_handler (vscrolled_bar);
}


static void
e_vscrolled_bar_button_released (GtkWidget *button,
				 EVScrolledBar *vscrolled_bar)
{
	vscrolled_bar->button_pressed = FALSE;
}


/* This will be called when the user hits the space key to activate the button.
   It will also be called just before button_released() is called, but since
   we already handle that we simply return if the button is pressed. */
static void
e_vscrolled_bar_button_clicked (GtkWidget *button,
				EVScrolledBar *vscrolled_bar)
{
	if (vscrolled_bar->button_pressed)
		return;

	/* We act as if the button is pressed and released immediately. */
	e_vscrolled_bar_button_pressed (button, vscrolled_bar);
	vscrolled_bar->button_pressed = FALSE;
}


static gboolean
e_vscrolled_bar_timeout_handler (gpointer data)
{
	EVScrolledBar *vscrolled_bar;
	GtkAdjustment *adjustment;
	gfloat new_value;
	gboolean retval = TRUE;

	vscrolled_bar = E_VSCROLLED_BAR (data);
	adjustment = vscrolled_bar->adjustment;

	GDK_THREADS_ENTER ();

	/* Check if the user has released the button and we have already
	   scrolled the minimum distance. */
	if (vscrolled_bar->button_pressed == FALSE
	    && vscrolled_bar->min_distance <= 0) {
		GDK_THREADS_LEAVE ();
		return FALSE;
	}

	vscrolled_bar->min_distance -= adjustment->step_increment;

	if (vscrolled_bar->scrolling_up) {
		new_value = adjustment->value - adjustment->step_increment;
		if (new_value <= adjustment->lower) {
			new_value = adjustment->lower;
			retval = FALSE;
		}
	} else {
		new_value = adjustment->value + adjustment->step_increment;
		if (new_value >= adjustment->upper - adjustment->page_size) {
			new_value = adjustment->upper - adjustment->page_size;
			retval = FALSE;
		}
	}

	if (adjustment->value != new_value) {
		adjustment->value = new_value;
		gtk_signal_emit_by_name (GTK_OBJECT (adjustment),
					 "value_changed");
	}

	GDK_THREADS_LEAVE ();
	return retval;
}
