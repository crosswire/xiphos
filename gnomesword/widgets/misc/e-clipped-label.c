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
 * This is similar to GtkLabel but clips itself and displays '...' if it
 * can't fit inside its allocated area. The intended use is for inside buttons
 * that are a fixed size. The GtkLabel would normally display only the middle
 * part of the text, which doesn't look very good. This only supports one line
 * of text (so no wrapping/justification), without underlined characters.
 */

#include <math.h>

#include <gdk/gdki18n.h>
#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>

#include "e-clipped-label.h"


static void e_clipped_label_class_init (EClippedLabelClass *class);
static void e_clipped_label_init (EClippedLabel *label);
static void e_clipped_label_size_request (GtkWidget      *widget,
					  GtkRequisition *requisition);
static void e_clipped_label_size_allocate (GtkWidget *widget,
					   GtkAllocation *allocation);
static gint e_clipped_label_expose (GtkWidget      *widget,
				    GdkEventExpose *event);
static void e_clipped_label_recalc_chars_displayed (EClippedLabel *label);
static void e_clipped_label_destroy                (GtkObject *object);


static GtkMiscClass *parent_class;

/* This is the string to draw when the label is clipped, e.g. '...'. */
static gchar *e_clipped_label_ellipsis;

/* Flags used in chars_displayed field. Must be negative. */
#define E_CLIPPED_LABEL_NEED_RECALC		-1
#define E_CLIPPED_LABEL_SHOW_ENTIRE_LABEL	-2


GtkType
e_clipped_label_get_type (void)
{
	static GtkType e_clipped_label_type = 0;

	if (!e_clipped_label_type){
		GtkTypeInfo e_clipped_label_info = {
			"EClippedLabel",
			sizeof (EClippedLabel),
			sizeof (EClippedLabelClass),
			(GtkClassInitFunc) e_clipped_label_class_init,
			(GtkObjectInitFunc) e_clipped_label_init,
			NULL, /* reserved 1 */
			NULL, /* reserved 2 */
			(GtkClassInitFunc) NULL
		};

		parent_class = gtk_type_class (GTK_TYPE_MISC);
		e_clipped_label_type = gtk_type_unique (GTK_TYPE_MISC,
							&e_clipped_label_info);
	}

	return e_clipped_label_type;
}


static void
e_clipped_label_class_init (EClippedLabelClass *class)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;

	object_class = (GtkObjectClass *) class;
	widget_class = (GtkWidgetClass *) class;

	/* Method override */
	widget_class->size_request	= e_clipped_label_size_request;
 	widget_class->size_allocate	= e_clipped_label_size_allocate;
	widget_class->expose_event	= e_clipped_label_expose;

	object_class->destroy           = e_clipped_label_destroy;

	e_clipped_label_ellipsis = _("...");
}


static void
e_clipped_label_init (EClippedLabel *label)
{
	GTK_WIDGET_SET_FLAGS (label, GTK_NO_WINDOW);

	label->label = NULL;
	label->label_wc = NULL;
	label->chars_displayed = E_CLIPPED_LABEL_NEED_RECALC;
}


/**
 * e_clipped_label_new:
 *
 * @text: The label text.
 * @Returns: A new #EClippedLabel.
 *
 * Creates a new #EClippedLabel with the given text.
 **/
GtkWidget *
e_clipped_label_new (const gchar *text)
{
	GtkWidget *label;

	label = GTK_WIDGET (gtk_type_new (e_clipped_label_get_type ()));

	if (text && *text)
		e_clipped_label_set_text (E_CLIPPED_LABEL (label), text);

	return label;
}


static void
e_clipped_label_size_request (GtkWidget      *widget,
			      GtkRequisition *requisition)
{
	EClippedLabel *label;
	GdkFont *font;
  
	g_return_if_fail (E_IS_CLIPPED_LABEL (widget));
	g_return_if_fail (requisition != NULL);
  
	label = E_CLIPPED_LABEL (widget);
	font = widget->style->font;

	requisition->width = 0;
	requisition->height = font->ascent + font->descent
		+ 2 * GTK_MISC (widget)->ypad;
}


static void
e_clipped_label_size_allocate (GtkWidget *widget,
			       GtkAllocation *allocation)
{
	EClippedLabel *label;

	label = E_CLIPPED_LABEL (widget);

	widget->allocation = *allocation;

	/* Flag that we need to recalculate how many characters to display. */
	label->chars_displayed = E_CLIPPED_LABEL_NEED_RECALC;
}


static gint
e_clipped_label_expose (GtkWidget      *widget,
			GdkEventExpose *event)
{
	EClippedLabel *label;
	GtkMisc *misc;
	gint x, y;
	GdkFont *font;
	gchar *tmp_str, tmp_ch;

	g_return_val_if_fail (E_IS_CLIPPED_LABEL (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
  
	label = E_CLIPPED_LABEL (widget);
	misc = GTK_MISC (widget);
	font = widget->style->font;

	/* If the label isn't visible or has no text, just return. */
	if (!GTK_WIDGET_VISIBLE (widget) || !GTK_WIDGET_MAPPED (widget)
	    || !label->label || (*label->label == '\0'))
		return TRUE;

	/* Recalculate the number of characters displayed, if necessary. */
	if (label->chars_displayed == E_CLIPPED_LABEL_NEED_RECALC)
		e_clipped_label_recalc_chars_displayed (label);

	/*
	 * GC Clipping
	 */
	gdk_gc_set_clip_rectangle (widget->style->white_gc,
				   &event->area);
	gdk_gc_set_clip_rectangle (widget->style->fg_gc[widget->state],
				   &event->area);
      
	y = floor (widget->allocation.y + (gint)misc->ypad 
		   + (((gint)widget->allocation.height - 2 * (gint)misc->ypad
		       - (gint)font->ascent - font->descent)
		      * misc->yalign) + 0.5) + font->ascent;

	if (label->chars_displayed == E_CLIPPED_LABEL_SHOW_ENTIRE_LABEL) {
		x = floor (widget->allocation.x + (gint)misc->xpad
			   + (((gint)widget->allocation.width - 
			       (gint)label->label_width - 2 * (gint)misc->xpad)
			      * misc->xalign) + 0.5);

		gtk_paint_string (widget->style, widget->window, widget->state,
				  &event->area, widget, "label", 
				  x, y, label->label);
	} else {
		x = widget->allocation.x + (gint)misc->xpad;

		tmp_ch = label->label_wc[label->chars_displayed];
		label->label_wc[label->chars_displayed] = '\0';
		tmp_str = gdk_wcstombs (label->label_wc);
		if (tmp_str) {
			gtk_paint_string (widget->style, widget->window,
					  widget->state, &event->area,
					  widget, "label", 
					  x, y, tmp_str);
			g_free (tmp_str);
		}
		label->label_wc[label->chars_displayed] = tmp_ch;

		x = widget->allocation.x + (gint)misc->xpad
			+ label->ellipsis_x;
		gtk_paint_string (widget->style, widget->window, widget->state,
				  &event->area, widget, "label", 
				  x, y, e_clipped_label_ellipsis);
	}

	gdk_gc_set_clip_mask (widget->style->white_gc, NULL);
	gdk_gc_set_clip_mask (widget->style->fg_gc[widget->state], NULL);

	return TRUE;
}


static void
e_clipped_label_destroy (GtkObject *object)
{
	EClippedLabel *label;

	g_return_if_fail (E_IS_CLIPPED_LABEL (object));

	label = E_CLIPPED_LABEL(object);

	g_free (label->label);
	g_free (label->label_wc);
}


/**
 * e_clipped_label_get_text:
 *
 * @label: An #EClippedLabel.
 * @Return: The label text.
 *
 * Returns the label text, or NULL.
 **/
gchar*
e_clipped_label_get_text (EClippedLabel	*label)
{
	g_return_val_if_fail (E_IS_CLIPPED_LABEL (label), NULL);

	return label->label;
}


/**
 * e_clipped_label_set_text:
 *
 * @label: An #EClippedLabel.
 * @text: The new label text.
 *
 * Sets the label text.
 **/
void
e_clipped_label_set_text (EClippedLabel *label,
			  const gchar *text)
{
	gint len;

	g_return_if_fail (E_IS_CLIPPED_LABEL (label));

	if (label->label != text || !label->label || !text
	    || strcmp (label->label, text)) {
		g_free (label->label);
		g_free (label->label_wc);
		label->label = NULL;
		label->label_wc = NULL;

		if (text) {
			label->label = g_strdup (text);
			len = strlen (text);
			label->label_wc = g_new (GdkWChar, len + 1);
			label->wc_len = gdk_mbstowcs (label->label_wc,
						      label->label, len + 1);
			label->label_wc[label->wc_len] = '\0';
		}

		/* Reset the number of characters displayed, so it is
		   recalculated when needed. */
		label->chars_displayed = E_CLIPPED_LABEL_NEED_RECALC;

		/* We don't queue a resize, since the label should not affect
		   the widget size, but we queue a draw. */
		gtk_widget_queue_draw (GTK_WIDGET (label));
	}
}


static void
e_clipped_label_recalc_chars_displayed (EClippedLabel *label)
{
	GdkFont *font;
	gint max_width, width, ch, last_width;

	font = GTK_WIDGET (label)->style->font;

	max_width = GTK_WIDGET (label)->allocation.width
		- 2 * GTK_MISC (label)->xpad;

	if (!label->label) {
		label->chars_displayed = 0;
		return;
	}

	/* See if the entire label fits in the allocated width. */
	label->label_width = gdk_string_width (font, label->label);
	if (label->label_width <= max_width) {
		label->chars_displayed = E_CLIPPED_LABEL_SHOW_ENTIRE_LABEL;
		return;
	}

	/* Calculate the width of the ellipsis string. */
	max_width -= gdk_string_measure (font, e_clipped_label_ellipsis);

	if (max_width <= 0) {
		label->chars_displayed = 0;
		label->ellipsis_x = 0;
		return;
	}

	/* Step through the wide-char label, adding on the widths of the
	   characters, until we can't fit any more in. */
	width = last_width = 0;
	for (ch = 0; ch < label->wc_len; ch++) {
		width += gdk_char_width_wc (font, label->label_wc[ch]);

		if (width > max_width) {
			label->chars_displayed = ch;
			label->ellipsis_x = last_width;
			return;
		}

		last_width = width;
	}

	g_warning ("Clipped label width not exceeded as expected");
	label->chars_displayed = E_CLIPPED_LABEL_SHOW_ENTIRE_LABEL;
}

