/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* e-canvas.h
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
#ifndef __E_CANVAS_H__
#define __E_CANVAS_H__

#include <gnome.h>

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

/* ECanvas - A class derived from canvas for the purpose of adding
 * evolution specific canvas hacks.
 */

#define E_CANVAS_TYPE			(e_canvas_get_type ())
#define E_CANVAS(obj)			(GTK_CHECK_CAST ((obj), E_CANVAS_TYPE, ECanvas))
#define E_CANVAS_CLASS(klass)		(GTK_CHECK_CLASS_CAST ((klass), E_CANVAS_TYPE, ECanvasClass))
#define E_IS_CANVAS(obj)		(GTK_CHECK_TYPE ((obj), E_CANVAS_TYPE))
#define E_IS_CANVAS_CLASS(klass)	(GTK_CHECK_CLASS_TYPE ((obj), E_CANVAS_TYPE))

typedef void		(*ECanvasItemReflowFunc)		(GnomeCanvasItem *item,
								 gint   	  flags);

typedef void            (*ECanvasItemSelectionFunc)             (GnomeCanvasItem *item,
								 gint             flags,
								 gpointer         user_data);
/* Returns the same as strcmp does. */
typedef gint            (*ECanvasItemSelectionCompareFunc)      (GnomeCanvasItem *item,
								 gpointer         data1,
								 gpointer         data2,
								 gint             flags);


typedef struct _ECanvas       ECanvas;
typedef struct _ECanvasClass  ECanvasClass;

/* Object flags for items */
enum {
	E_CANVAS_ITEM_NEEDS_REFLOW            = 1 << 13,
	E_CANVAS_ITEM_DESCENDENT_NEEDS_REFLOW = 1 << 14
};

enum {
	E_CANVAS_ITEM_SELECTION_SELECT        = 1 << 0, /* TRUE = select.  FALSE = unselect. */
	E_CANVAS_ITEM_SELECTION_CURSOR        = 1 << 1, /* TRUE = has become cursor.  FALSE = not cursor. */
	E_CANVAS_ITEM_SELECTION_DELETE_DATA   = 1 << 2,
};

typedef struct {
	GnomeCanvasItem *item;
	gpointer         id;
} ECanvasSelectionInfo;

struct _ECanvas
{
	GnomeCanvas parent;
		             
	int                   idle_id;
	GList                *selection;
	ECanvasSelectionInfo *cursor;

	/* Input context for dead key support */
	GdkIC     *ic;
	GdkICAttr *ic_attr;
};

struct _ECanvasClass
{
	GnomeCanvasClass parent_class;
	void (* reflow) (ECanvas *canvas);
};


GtkType    e_canvas_get_type (void);
GtkWidget *e_canvas_new      (void);

/* Used to send all of the keystroke events to a specific item as well as
 * GDK_FOCUS_CHANGE events.
 */
void e_canvas_item_grab_focus (GnomeCanvasItem *item);

void e_canvas_item_request_reflow (GnomeCanvasItem *item);
void e_canvas_item_request_parent_reflow (GnomeCanvasItem *item);
void e_canvas_item_set_reflow_callback (GnomeCanvasItem *item, ECanvasItemReflowFunc func);

void e_canvas_item_set_selection_callback (GnomeCanvasItem *item, ECanvasItemSelectionFunc func);
void e_canvas_item_set_selection_compare_callback (GnomeCanvasItem *item, ECanvasItemSelectionCompareFunc func);

void e_canvas_item_set_cursor (GnomeCanvasItem *item, gpointer id);
void e_canvas_item_add_selection (GnomeCanvasItem *item, gpointer id);
void e_canvas_item_remove_selection (GnomeCanvasItem *item, gpointer id);

/* Not implemented yet. */
void e_canvas_item_set_cursor_end (GnomeCanvasItem *item, gpointer id);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __E_CANVAS_H__ */
