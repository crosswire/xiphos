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

#ifndef _E_ICON_BAR_BG_ITEM_H_
#define _E_ICON_BAR_BG_ITEM_H_

#include <libgnomeui/gnome-canvas.h>

#include "e-icon-bar.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define E_ICON_BAR_BG_ITEM(obj)          (GTK_CHECK_CAST((obj), e_icon_bar_bg_item_get_type (), EIconBarBgItem))
#define E_ICON_BAR_BG_ITEM_CLASS(k)      (GTK_CHECK_CLASS_CAST ((k), e_icon_bar_bg_item_get_type (), EIconBarBgItemClass))
#define E_IS_ICON_BAR_BG_ITEM(o)         (GTK_CHECK_TYPE((o), e_icon_bar_bg_item_get_type ()))


typedef struct _EIconBarBgItem       EIconBarBgItem;
typedef struct _EIconBarBgItemClass  EIconBarBgItemClass;

struct _EIconBarBgItem
{
	GnomeCanvasItem canvas_item;

	/* The parent EIconBar widget. */
	EIconBar *icon_bar;
};


struct _EIconBarBgItemClass
{
	GnomeCanvasItemClass parent_class;
};

GtkType e_icon_bar_bg_item_get_type (void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_ICON_BAR_BG_ITEM_H_ */
