/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * gtk-combo-box.h - a customizable combobox
 * Copyright 2000, 2001, Ximian, Inc.
 *
 * Authors:
 *   Miguel de Icaza <miguel@ximian.com>
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

#ifndef GI_COMBO_BOX_H
#define GI_COMBO_BOX_H

#include <gtk/gtkhbox.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GAL_COMBO_BOX_TYPE          (gal_combo_box_get_type())
#define GAL_COMBO_BOX(obj)	    G_TYPE_CHECK_INSTANCE_CAST (obj, gal_combo_box_get_type (), GalComboBox)
#define GAL_COMBO_BOX_CLASS(klass)  G_TYPE_CHECK_CLASS_CAST (klass, gal_combo_box_get_type (), GalComboBoxClass)
#define GAL_IS_COMBO_BOX(obj)       G_TYPE_CHECK_INSTANCE_TYPE (obj, gal_combo_box_get_type ())

typedef struct _GalComboBox	   GalComboBox;
typedef struct _GalComboBoxPrivate GalComboBoxPrivate;
typedef struct _GalComboBoxClass   GalComboBoxClass;

struct _GalComboBox {
	GtkHBox hbox;
	GalComboBoxPrivate *priv;
};

struct _GalComboBoxClass {
	GtkHBoxClass parent_class;

	GtkWidget *(*pop_down_widget) (GalComboBox *cbox);

	/*
	 * invoked when the popup has been hidden, if the signal
	 * returns TRUE, it means it should be killed from the
	 */ 
	gboolean  *(*pop_down_done)   (GalComboBox *cbox, GtkWidget *);

	/*
	 * Notification signals.
	 */
	void      (*pre_pop_down)     (GalComboBox *cbox);
	void      (*post_pop_hide)    (GalComboBox *cbox);
};

GtkType    gal_combo_box_get_type    (void);
void       gal_combo_box_construct   (GalComboBox *combo_box,
				      GtkWidget   *display_widget,
				      GtkWidget   *optional_pop_down_widget);
void       gal_combo_box_get_pos     (GalComboBox *combo_box, int *x, int *y);

GtkWidget *gal_combo_box_new         (GtkWidget *display_widget,
				      GtkWidget *optional_pop_down_widget);
void       gal_combo_box_popup_hide  (GalComboBox *combo_box);

void       gal_combo_box_set_display (GalComboBox *combo_box,
				      GtkWidget *display_widget);

void       gal_combo_box_set_title   (GalComboBox *combo,
				      const gchar *title);

void       gal_combo_box_set_tearable        (GalComboBox *combo,
					      gboolean tearable);
void       gal_combo_box_set_arrow_sensitive (GalComboBox *combo,
					      gboolean sensitive);
void       gal_combo_box_set_arrow_relief    (GalComboBox *cc,
					      GtkReliefStyle relief);
#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
