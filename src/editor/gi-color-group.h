/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * color-group.h - Utility to keep a shered memory of custom colors
 * between arbitrary widgets.
 * Copyright 2000, Michael Levy
 * Copyright 2001, Almer S. Tigelaar
 *
 * Authors:
 *   Michael Levy (mlevy@genoscope.cns.fr)
 * Revised and polished by:
 *   Almer S. Tigelaar <almer@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License, version 2, as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef GNOME_APP_LIB__COLOR_GROUP_H
#define GNOME_APP_LIB__COLOR_GROUP_H

#include <glib-object.h>
#include <glib.h>
#include <gdk/gdktypes.h>

G_BEGIN_DECLS

typedef gboolean (* CbCustomColors) (GdkColor const * const color, gpointer data);

typedef struct _ColorGroup {
	GObject  parent;

        gchar     *name;
	gpointer   context;

        GPtrArray *history;         /* The custom color history */
	gint       history_size;    /* length of color_history */
} ColorGroup;

typedef struct {
	GObjectClass parent_class;
	
	/* Signals emited by this object */
	void (*custom_color_add) (ColorGroup *color_group, GdkColor const * const color);
} ColorGroupClass;

#define COLOR_GROUP_TYPE     (color_group_get_type ())
#define COLOR_GROUP(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), COLOR_GROUP_TYPE, ColorGroup))
#define COLOR_GROUP_CLASS(k) (G_TYPE_CHECK_CLASS_CAST (k), COLOR_GROUP_TYPE)
#define IS_COLOR_GROUP(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), COLOR_GROUP_TYPE))

GType           color_group_get_type (void);
ColorGroup     *color_group_fetch (const gchar *name, gpointer context);
ColorGroup     *color_group_get   (const gchar *name, gpointer context);

void            color_group_set_history_size (ColorGroup *cg, gint size);
gint            color_group_get_history_size (ColorGroup *cg);

void            color_group_get_custom_colors (ColorGroup *cg, CbCustomColors callback,
					       gpointer user_data);
void            color_group_add_color	      (ColorGroup *cg, GdkColor const * const color);

G_END_DECLS

#endif /* GNOME_APP_LIB__COLOR_GROUP_H */
