/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
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

/*
 * Modified by the GTK+ Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

/* EScrollFrame based on GtkScrolledWindow.
 *
 * Modifications by Federico Mena <federico@helixcode.com>
 */

#ifndef __E_SCROLL_FRAME_H__
#define __E_SCROLL_FRAME_H__


#include <gdk/gdk.h>
#include <gtk/gtkbin.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define E_TYPE_SCROLL_FRAME            (e_scroll_frame_get_type ())
#define E_SCROLL_FRAME(obj)            (GTK_CHECK_CAST ((obj), E_TYPE_SCROLL_FRAME, EScrollFrame))
#define E_SCROLL_FRAME_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), E_TYPE_SCROLL_FRAME,	\
					EScrollFrameClass))
#define E_IS_SCROLL_FRAME(obj)         (GTK_CHECK_TYPE ((obj), E_TYPE_SCROLL_FRAME))
#define E_IS_SCROLL_FRAME_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), E_TYPE_SCROLL_FRAME))


typedef struct _EScrollFrame       EScrollFrame;
typedef struct _EScrollFrameClass  EScrollFrameClass;

struct _EScrollFrame
{
	GtkBin bin;

	/* Private data */
	gpointer priv;
};

struct _EScrollFrameClass
{
	GtkBinClass parent_class;
};


GtkType e_scroll_frame_get_type (void);
GtkWidget *e_scroll_frame_new (GtkAdjustment *hadj, GtkAdjustment *vadj);

void e_scroll_frame_set_hadjustment (EScrollFrame *sf, GtkAdjustment *adj);
void e_scroll_frame_set_vadjustment (EScrollFrame *sf, GtkAdjustment *adj);

GtkAdjustment *e_scroll_frame_get_hadjustment (EScrollFrame *sf);
GtkAdjustment *e_scroll_frame_get_vadjustment (EScrollFrame *sf);

void e_scroll_frame_set_policy (EScrollFrame *sf,
				GtkPolicyType hsb_policy,
				GtkPolicyType vsb_policy);

void e_scroll_frame_set_placement (EScrollFrame *sf, GtkCornerType frame_placement);
void e_scroll_frame_set_shadow_type (EScrollFrame *sf, GtkShadowType shadow_type);
void e_scroll_frame_set_scrollbar_spacing (EScrollFrame *sf, guint spacing);

void e_scroll_frame_add_with_viewport (EScrollFrame *sf, GtkWidget *child);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __E_SCROLL_FRAME_H__ */
