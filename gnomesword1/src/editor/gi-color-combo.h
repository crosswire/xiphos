/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * widget-color-combo.h - A color selector combo box
 * Copyright 2000, 2001, Ximian, Inc.
 *
 * Authors:
 *   Miguel de Icaza (miguel@kernel.org)
 *   Dom Lachowicz (dominicl@seas.upenn.edu)
 *
 * Reworked and split up into a separate ColorPalette object:
 *   Michael Levy (mlevy@genoscope.cns.fr)
 *
 * And later revised and polished by:
 *   Almer S. Tigelaar (almer@gnome.org)
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

#ifndef GI_COLOR_COMBO_H
#define GI_COLOR_COMBO_H

#include <gtk/gtkwidget.h>
#include "gi-combo-box.h"
#include "gi-color-palette.h"

G_BEGIN_DECLS

typedef struct _ColorCombo {
	GalComboBox     combo_box;

	/*
	 * Canvas where we display
	 */
	GtkWidget       *preview_button;
	GnomeCanvas     *preview_canvas;
	GnomeCanvasItem *preview_color_item;
	ColorPalette    *palette;

        GdkColor *default_color;
	gboolean  trigger;
} ColorCombo;

typedef struct {
	GalComboBoxClass parent_class;

	/* Signals emited by this widget */
	void (* color_changed) (ColorCombo *color_combo, GdkColor *color,
				gboolean custom, gboolean by_user, gboolean is_default);
} ColorComboClass;

#define COLOR_COMBO_TYPE     (color_combo_get_type ())
#define COLOR_COMBO(obj)     (G_TYPE_CHECK_INSTANCE_CAST((obj), COLOR_COMBO_TYPE, ColorCombo))
#define COLOR_COMBO_CLASS(k) (G_TYPE_CHECK_CLASS_CAST(k), COLOR_COMBO_TYPE)
#define IS_COLOR_COMBO(obj)  (G_TYPE_CHECK_INSTANCE_TYPE((obj), COLOR_COMBO_TYPE))

GtkType    color_combo_get_type   (void);
GtkWidget *color_combo_new        (GdkPixbuf   *icon,
				   char  const *no_color_label,
				   GdkColor    *default_color,
				   ColorGroup  *color_group);
void       color_combo_set_color  (ColorCombo  *cc,
				   GdkColor    *color);
void       color_combo_set_color_to_default (ColorCombo *cc);
GdkColor  *color_combo_get_color  (ColorCombo  *cc, gboolean *is_default);

void       color_combo_box_set_preview_relief (ColorCombo *cc, GtkReliefStyle relief);

G_END_DECLS

#endif
