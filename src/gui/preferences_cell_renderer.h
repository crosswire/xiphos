/*
 * GnomeSword Bible Study Tool
 *
 * preferences_cell_renderer.h
 *
 * Copyright (C) 2002 Dave Camp
 * taken form Anjuta-1.1.98
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef PREFS_CELL_RENDERER_H
#define PREFS_CELL_RENDERER_H

#include <gtk/gtkcellrenderer.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkcellrendererpixbuf.h>

G_BEGIN_DECLS

#define PREFS_TYPE_CELL_RENDERER_CAPTIONED_IMAGE		(preferences_cell_renderer_captioned_image_get_type ())
#define PREFS_CELL_RENDERER_CAPTIONED_IMAGE(obj)		(GTK_CHECK_CAST ((obj), PREFS_TYPE_CELL_RENDERER_CAPTIONED_IMAGE, CellRendererCaptionedImage))
#define PREFS_CELL_RENDERER_CAPTIONED_IMAGE_CLASS(klass)	(GTK_CHECK_CLASS_CAST ((klass), PREFS_TYPE_CELL_RENDERER_CAPTIONED_IMAGE, CellRendererCaptionedImageClass))
#define PREFS_IS_CELL_RENDERER_CAPTIONED_IMAGE(obj)		(GTK_CHECK_TYPE ((obj), PREFS_TYPE_CELL_RENDERER_CAPTIONED_IMAGE))
#define PREFS_IS_CELL_RENDERER_CAPTIONED_IMAGE_CLASS(klass)	(GTK_CHECK_CLASS_TYPE ((klass), PREFS_TYPE_CELL_RENDERER_CAPTIONED_IMAGE))
#define PREFS_CELL_RENDERER_CAPTIONED_IMAGE_GET_CLASS(obj)   (GTK_CHECK_GET_CLASS ((obj), PREFS_TYPE_CELL_RENDERER_CAPTIONED_IMAGE, CellRendererCaptionedImageClass))

typedef struct _CellRendererCaptionedImage        CellRendererCaptionedImage;
typedef struct _CellRendererCaptionedImageClass   CellRendererCaptionedImageClass;

struct _CellRendererCaptionedImage {
	GtkCellRenderer parent;

	GtkCellRenderer *image;
	GtkCellRenderer *caption;
};

struct _CellRendererCaptionedImageClass {
	GtkCellRendererClass parent_class;
};

GType            preferences_cell_renderer_captioned_image_get_type (void);
GtkCellRenderer *gui_preferences_cell_renderer_captioned_image_new      (void);

G_END_DECLS

#endif
