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
#ifndef _E_ICON_BAR_H_
#define _E_ICON_BAR_H_

#include <gdk_imlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "../../e-util/e-canvas.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * EIconBar is a subclass of GnomeCanvas for displaying a vertical column of
 * icons and descriptions. It provides 2 views - large icons and small icons.
 */


/* This contains information on one item. */
typedef struct _EIconBarItem   EIconBarItem;
struct _EIconBarItem
{
	GnomeCanvasItem *text;
	GnomeCanvasItem *image;

	/* This is user data attached to the item, e.g. a URL. */
	gpointer data;
	GtkDestroyNotify destroy;

	/* This is the height of the item. */
	gint item_height;

	/* This is the actual x, width and height of the text, rather than
	   the maximum allowed area. */
	gint text_x;
	gint text_width;
	gint text_height;

	gint icon_y, text_y;
};


/* These are the view types. Defaults to LARGE_ICONS. */
typedef enum
{
	E_ICON_BAR_LARGE_ICONS,
	E_ICON_BAR_SMALL_ICONS
} EIconBarViewType;


/* These index our colors array. */
typedef enum
{
	E_ICON_BAR_COLOR_TEXT,
	E_ICON_BAR_COLOR_EDITING_TEXT,
	E_ICON_BAR_COLOR_EDITING_RECT,
	E_ICON_BAR_COLOR_EDITING_RECT_OUTLINE,
	
	E_ICON_BAR_COLOR_LAST
} EIconBarColors;


#define E_ICON_BAR(obj)          GTK_CHECK_CAST (obj, e_icon_bar_get_type (), EIconBar)
#define E_ICON_BAR_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, e_icon_bar_get_type (), EIconBarClass)
#define E_IS_ICON_BAR(obj)       GTK_CHECK_TYPE (obj, e_icon_bar_get_type ())


typedef struct _EIconBar       EIconBar;
typedef struct _EIconBarClass  EIconBarClass;

struct _EIconBar
{
	ECanvas canvas;

	/* This specifies if we are using large icons or small icons. */
	EIconBarViewType view_type;

	/* This is an array of EIconBarItem elements. */
	GArray *items;

	/* This is the index of the item which has been pressed, or -1.
	   It will be shown as pressed in while the mouse is over it. */
	gint pressed_item_num;

	/* This is the coordinates of where the button was pressed. If the
	   mouse moves a certain distance with the button still pressed, we
	   start a drag. */
	gint pressed_x;
	gint pressed_y;

	/* This is the index of the item the mouse is currently over, or -1.
	   It will be highlighted unless one of the items is pressed. */
	gint mouse_over_item_num;

	/* This is the item that we are currently editing, or -1. */
	gint editing_item_num;

	/* This is a GnomeCanvasRect which is placed around the edited item. */
	GnomeCanvasItem *edit_rect_item;

	/* This is the index of the item which is being dragged, or -1.
	   If the drag results in a move it will be deleted. */
	gint dragged_item_num;

	/* This is TRUE if we are dragging over this EIconBar. */
	gboolean in_drag;

	/* This is used in drag-and-drop to indicate the item which the mouse
	   is currently before, e.g. if it is 1 then a dropped item would be
	   inserted between items 0 and 1. It ranges from 0 to the number of
	   items, or is -1 when the mouse is not dragging between items. */
	gint dragging_before_item_num;

	/* These are the common positions of all the items in the EIconBar. */
	gint icon_x, icon_w, icon_h, text_x, text_w, spacing;

	/* This is the source id of our auto-scroll timeout handler, used when
	   in the middle of drag-and-drop operations. */
	gint auto_scroll_timeout_id;
	gint auto_scroll_delay;
	gboolean scrolling_up;

	/* Colors for drawing. */
	GdkColor colors[E_ICON_BAR_COLOR_LAST];

	/* Signal connection id for "value_changed" signal of vadjustment. */
	guint vadjustment_value_changed_id;
};

struct _EIconBarClass
{
	ECanvasClass parent_class;

	void   (*selected_item)        (EIconBar       *icon_bar,
					GdkEvent       *event,
					gint		item_num);
	void   (*dragged_item)         (EIconBar       *icon_bar,
					GdkEvent       *event,
					gint		item_num);
};


GtkType	       e_icon_bar_get_type		(void);
GtkWidget*     e_icon_bar_new			(void);

/* Sets the view type. */
void	       e_icon_bar_set_view_type		(EIconBar	  *icon_bar,
						 EIconBarViewType  view_type);

/* Adds a new item to a group at the given position. If position is -1 it is
   added at the end. It returns the index of the item. */
gint	       e_icon_bar_add_item		(EIconBar	  *icon_bar,
						 GdkPixbuf	  *image,
						 const gchar	  *text,
						 gint		   position);

/* Reorders an item. Note that position refers to the new position to add the
   item after removing it from its current position. If position is -1 it is
   moved to the end of the bar. */
void	       e_icon_bar_reorder_item		(EIconBar	  *icon_bar,
						 gint		   item_num,
						 gint		   new_position);
void	       e_icon_bar_remove_item		(EIconBar	  *icon_bar,
						 gint		   item_num);

GdkPixbuf    * e_icon_bar_get_item_image	(EIconBar	  *icon_bar,
						 gint		   item_num);
void	       e_icon_bar_set_item_image	(EIconBar	  *icon_bar,
						 gint		   item_num,
						 GdkPixbuf	  *image);

gchar*	       e_icon_bar_get_item_text		(EIconBar	  *icon_bar,
						 gint		   item_num);
void	       e_icon_bar_set_item_text		(EIconBar	  *icon_bar,
						 gint		   item_num,
						 gchar		  *text);

gpointer       e_icon_bar_get_item_data		(EIconBar	  *icon_bar,
						 gint		   item_num);
void	       e_icon_bar_set_item_data		(EIconBar	  *icon_bar,
						 gint		   item_num,
						 gpointer	   data);
void	       e_icon_bar_set_item_data_full	(EIconBar	  *icon_bar,
						 gint		   item_num,
						 gpointer	   data,
						 GtkDestroyNotify  destroy);

void	       e_icon_bar_start_editing_item	(EIconBar	  *icon_bar,
						 gint		   item_num);
void	       e_icon_bar_stop_editing_item	(EIconBar	  *icon_bar,
						 gboolean	   accept);



/*
 * INTERNAL FUNCTIONS - for use by EIconBarBgItem.
 */

/* This returns the index of the item at the given position on the EIconBar,
   or -1 if no item is found. If before_item is not NULL, it returns the
   item which the mouse is before, or -1 (this is used for dragging). */
gint	       e_icon_bar_find_item_at_position	(EIconBar	   *icon_bar,
						 gint		    x,
						 gint		    y,
						 gint		   *before_item);

void	       e_icon_bar_item_pressed		(EIconBar	   *icon_bar,
						 gint		    item_num,
						 GdkEvent	   *event);
void	       e_icon_bar_item_released		(EIconBar	   *icon_bar,
						 gint		    item_num,
						 GdkEvent	   *event);
void	       e_icon_bar_item_motion		(EIconBar	   *icon_bar,
						 gint		    item_num,
						 GdkEvent	   *event);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_ICON_BAR_H_ */
