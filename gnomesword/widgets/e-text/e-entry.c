/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * E-table.c: A graphical view of a Table.
 *
 * Author:
 *   Miguel de Icaza (miguel@helixcode.com)
 *   Chris Lahey (clahey@helixcode.com)
 *
 * Copyright 1999, Helix Code, Inc
 */
#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <stdio.h>
#include <libgnomeui/gnome-canvas.h>
#include <gtk/gtksignal.h>
#include <gnome-xml/parser.h>
#include "e-util/e-util.h"
#include "e-util/e-canvas.h"
#include "e-entry.h"
#include "e-util/e-canvas-utils.h"

#define MIN_ENTRY_WIDTH  150
#define INNER_BORDER     2

#define PARENT_TYPE gtk_table_get_type ()

static GtkObjectClass *parent_class;

enum {
	E_ENTRY_CHANGED,
	E_ENTRY_ACTIVATE,
	E_ENTRY_LAST_SIGNAL
};

static guint e_entry_signals[E_ENTRY_LAST_SIGNAL] = { 0 };

/* Object argument IDs */
enum {
	ARG_0,
	ARG_MODEL,
	ARG_EVENT_PROCESSOR,
	ARG_TEXT,
	ARG_FONT,
        ARG_FONTSET,
	ARG_FONT_GDK,
	ARG_ANCHOR,
	ARG_JUSTIFICATION,
	ARG_X_OFFSET,
	ARG_Y_OFFSET,
	ARG_FILL_COLOR,
	ARG_FILL_COLOR_GDK,
	ARG_FILL_COLOR_RGBA,
	ARG_FILL_STIPPLE,
	ARG_EDITABLE,
	ARG_USE_ELLIPSIS,
	ARG_ELLIPSIS,
	ARG_LINE_WRAP,
	ARG_BREAK_CHARACTERS,
	ARG_MAX_LINES,
	ARG_WIDTH,
	ARG_HEIGHT
};

static void
canvas_size_allocate (GtkWidget *widget, GtkAllocation *alloc,
		      EEntry *e_entry)
{
	gnome_canvas_set_scroll_region (
		e_entry->canvas,
		0, 0, alloc->width, alloc->height);
	gtk_object_set (GTK_OBJECT (e_entry->item),
			"clip_width", (double) alloc->width,
			"clip_height", (double) alloc->height,
			NULL);
}

static void
canvas_size_request (GtkWidget *widget, GtkRequisition *requisition,
		     EEntry *e_entry)
{
	g_return_if_fail (widget != NULL);
	g_return_if_fail (GNOME_IS_CANVAS (widget));
	g_return_if_fail (requisition != NULL);

	requisition->width = MIN_ENTRY_WIDTH + (widget->style->klass->xthickness + INNER_BORDER) * 2;
	requisition->height = (widget->style->font->ascent +
			       widget->style->font->descent +
			       (widget->style->klass->ythickness + INNER_BORDER) * 2);
}

static void
e_entry_init (GtkObject *object)
{
	EEntry *e_entry = E_ENTRY (object);
	GtkTable *gtk_table = GTK_TABLE (object);
	
	e_entry->canvas = GNOME_CANVAS(e_canvas_new());
	gtk_signal_connect(GTK_OBJECT(e_entry->canvas), "size_allocate",
			   GTK_SIGNAL_FUNC(canvas_size_allocate), e_entry);
	gtk_signal_connect(GTK_OBJECT(e_entry->canvas), "size_request",
			   GTK_SIGNAL_FUNC(canvas_size_request), e_entry);
	e_entry->item = E_TEXT(gnome_canvas_item_new(gnome_canvas_root(e_entry->canvas),
						     e_text_get_type(),
						     "clip", TRUE,
						     "fill_clip_rectangle", TRUE,
						     "anchor", GTK_ANCHOR_NW,
						     "draw_borders", TRUE,
						     NULL));

	gtk_table_attach_defaults(gtk_table, GTK_WIDGET(e_entry->canvas),
				  0, 1, 0, 1);
	gtk_widget_show(GTK_WIDGET(e_entry->canvas));
}

EEntry *
e_entry_construct (EEntry *e_entry)
{
	return e_entry;
}

GtkWidget *
e_entry_new (void)
{
	EEntry *e_entry;
	e_entry = gtk_type_new (e_entry_get_type ());
	e_entry = e_entry_construct (e_entry);

	return GTK_WIDGET (e_entry);
}

static void
et_get_arg (GtkObject *o, GtkArg *arg, guint arg_id)
{
	EEntry *ee = E_ENTRY (o);

	switch (arg_id){
	case ARG_MODEL:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "model", &GTK_VALUE_OBJECT (*arg),
			       NULL);
		break;

	case ARG_EVENT_PROCESSOR:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "event_processor", &GTK_VALUE_OBJECT (*arg),
			       NULL);
		break;

	case ARG_TEXT:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "text", &GTK_VALUE_STRING (*arg),
			       NULL);
		break;

	case ARG_FONT_GDK:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "font_gdk", &GTK_VALUE_BOXED (*arg),
			       NULL);
		break;

	case ARG_JUSTIFICATION:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "justification", &GTK_VALUE_ENUM (*arg),
			       NULL);
		break;

	case ARG_FILL_COLOR_GDK:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "fill_color_gdk", &GTK_VALUE_BOXED (*arg),
			       NULL);
		break;

	case ARG_FILL_COLOR_RGBA:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "fill_color_rgba", &GTK_VALUE_UINT (*arg),
			       NULL);
		break;

	case ARG_FILL_STIPPLE:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "fill_stiple", &GTK_VALUE_BOXED (*arg),
			       NULL);
		break;

	case ARG_EDITABLE:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "editable", &GTK_VALUE_BOOL (*arg),
			       NULL);
		break;

	case ARG_USE_ELLIPSIS:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "use_ellipsis", &GTK_VALUE_BOOL (*arg),
			       NULL);
		break;

	case ARG_ELLIPSIS:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "ellipsis", &GTK_VALUE_STRING (*arg),
			       NULL);
		break;

	case ARG_LINE_WRAP:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "line_wrap", &GTK_VALUE_BOOL (*arg),
			       NULL);
		break;
		
	case ARG_BREAK_CHARACTERS:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "break_characters", &GTK_VALUE_STRING (*arg),
			       NULL);
		break;

	case ARG_MAX_LINES:
		gtk_object_get(GTK_OBJECT(ee->item),
			       "max_lines", &GTK_VALUE_INT (*arg),
			       NULL);
		break;
	default:
		arg->type = GTK_TYPE_INVALID;
		break;
	}
}

static void
et_set_arg (GtkObject *o, GtkArg *arg, guint arg_id)
{
	EEntry *ee = E_ENTRY (o);
	
	switch (arg_id){
	case ARG_MODEL:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "model", GTK_VALUE_OBJECT (*arg),
			       NULL);
		break;

	case ARG_EVENT_PROCESSOR:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "event_processor", GTK_VALUE_OBJECT (*arg),
			       NULL);
		break;

	case ARG_TEXT:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "text", GTK_VALUE_STRING (*arg),
			       NULL);
		break;

	case ARG_FONT:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "font", GTK_VALUE_STRING (*arg),
			       NULL);
		break;

	case ARG_FONTSET:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "fontset", GTK_VALUE_STRING (*arg),
			       NULL);
		break;

	case ARG_FONT_GDK:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "font_gdk", GTK_VALUE_BOXED (*arg),
			       NULL);
		break;

	case ARG_JUSTIFICATION:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "justification", GTK_VALUE_ENUM (*arg),
			       NULL);
		break;

	case ARG_FILL_COLOR:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "fill_color", GTK_VALUE_STRING (*arg),
			       NULL);
		break;

	case ARG_FILL_COLOR_GDK:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "fill_color_gdk", GTK_VALUE_BOXED (*arg),
			       NULL);
		break;

	case ARG_FILL_COLOR_RGBA:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "fill_color_rgba", GTK_VALUE_UINT (*arg),
			       NULL);
		break;

	case ARG_FILL_STIPPLE:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "fill_stiple", GTK_VALUE_BOXED (*arg),
			       NULL);
		break;

	case ARG_EDITABLE:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "editable", GTK_VALUE_BOOL (*arg),
			       NULL);
		break;

	case ARG_USE_ELLIPSIS:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "use_ellipsis", GTK_VALUE_BOOL (*arg),
			       NULL);
		break;

	case ARG_ELLIPSIS:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "ellipsis", GTK_VALUE_STRING (*arg),
			       NULL);
		break;

	case ARG_LINE_WRAP:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "line_wrap", GTK_VALUE_BOOL (*arg),
			       NULL);
		break;
		
	case ARG_BREAK_CHARACTERS:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "break_characters", GTK_VALUE_STRING (*arg),
			       NULL);
		break;

	case ARG_MAX_LINES:
		gtk_object_set(GTK_OBJECT(ee->item),
			       "max_lines", GTK_VALUE_INT (*arg),
			       NULL);
		break;
	}
}

static void
e_entry_class_init (GtkObjectClass *object_class)
{
	EEntryClass *klass = E_ENTRY_CLASS(object_class);
	parent_class = gtk_type_class (PARENT_TYPE);

	object_class->set_arg = et_set_arg;
	object_class->get_arg = et_get_arg;

	klass->changed = NULL;
	klass->activate = NULL;

	e_entry_signals[E_ENTRY_CHANGED] =
		gtk_signal_new ("changed",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (EEntryClass, changed),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0);

	e_entry_signals[E_ENTRY_ACTIVATE] =
		gtk_signal_new ("activate",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (EEntryClass, activate),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0);


	gtk_object_class_add_signals (object_class, e_entry_signals, E_ENTRY_LAST_SIGNAL);

	gtk_object_add_arg_type ("EEntry::model",
				 GTK_TYPE_OBJECT, GTK_ARG_READWRITE, ARG_MODEL);  
	gtk_object_add_arg_type ("EEntry::event_processor",
				 GTK_TYPE_OBJECT, GTK_ARG_READWRITE, ARG_EVENT_PROCESSOR);
	gtk_object_add_arg_type ("EEntry::text",
				 GTK_TYPE_STRING, GTK_ARG_READWRITE, ARG_TEXT);
	gtk_object_add_arg_type ("EEntry::font",
				 GTK_TYPE_STRING, GTK_ARG_WRITABLE, ARG_FONT);
	gtk_object_add_arg_type ("EEntry::fontset",
				 GTK_TYPE_STRING, GTK_ARG_WRITABLE, ARG_FONTSET);
	gtk_object_add_arg_type ("EEntry::font_gdk",
				 GTK_TYPE_GDK_FONT, GTK_ARG_READWRITE, ARG_FONT_GDK);
	gtk_object_add_arg_type ("EEntry::justification",
				 GTK_TYPE_JUSTIFICATION, GTK_ARG_READWRITE, ARG_JUSTIFICATION);
	gtk_object_add_arg_type ("EEntry::fill_color",
				 GTK_TYPE_STRING, GTK_ARG_WRITABLE, ARG_FILL_COLOR);
	gtk_object_add_arg_type ("EEntry::fill_color_gdk",
				 GTK_TYPE_GDK_COLOR, GTK_ARG_READWRITE, ARG_FILL_COLOR_GDK);
	gtk_object_add_arg_type ("EEntry::fill_color_rgba",
				 GTK_TYPE_UINT, GTK_ARG_READWRITE, ARG_FILL_COLOR_RGBA);
	gtk_object_add_arg_type ("EEntry::fill_stipple",
				 GTK_TYPE_GDK_WINDOW, GTK_ARG_READWRITE, ARG_FILL_STIPPLE);
	gtk_object_add_arg_type ("EEntry::editable",
				 GTK_TYPE_BOOL, GTK_ARG_READWRITE, ARG_EDITABLE);
	gtk_object_add_arg_type ("EEntry::use_ellipsis",
				 GTK_TYPE_BOOL, GTK_ARG_READWRITE, ARG_USE_ELLIPSIS);
	gtk_object_add_arg_type ("EEntry::ellipsis",
				 GTK_TYPE_STRING, GTK_ARG_READWRITE, ARG_ELLIPSIS);
	gtk_object_add_arg_type ("EEntry::line_wrap",
				 GTK_TYPE_BOOL, GTK_ARG_READWRITE, ARG_LINE_WRAP);
	gtk_object_add_arg_type ("EEntry::break_characters",
				 GTK_TYPE_STRING, GTK_ARG_READWRITE, ARG_BREAK_CHARACTERS);
	gtk_object_add_arg_type ("EEntry::max_lines",
				 GTK_TYPE_INT, GTK_ARG_READWRITE, ARG_MAX_LINES);
}

E_MAKE_TYPE(e_entry, "EEntry", EEntry, e_entry_class_init, e_entry_init, PARENT_TYPE);
