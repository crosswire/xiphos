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
#ifndef _E_CLIPPED_LABEL_H_
#define _E_CLIPPED_LABEL_H_

#include <gtk/gtkmisc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define E_CLIPPED_LABEL(obj)          GTK_CHECK_CAST (obj, e_clipped_label_get_type (), EClippedLabel)
#define E_CLIPPED_LABEL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, e_clipped_label_get_type (), EClippedLabelClass)
#define E_IS_CLIPPED_LABEL(obj)       GTK_CHECK_TYPE (obj, e_clipped_label_get_type ())


typedef struct _EClippedLabel       EClippedLabel;
typedef struct _EClippedLabelClass  EClippedLabelClass;

struct _EClippedLabel
{
	GtkMisc misc;

	gchar *label;
	GdkWChar *label_wc;

	/* This is the number of wide characters in the label. */
	gint wc_len;

	/* This is the width of the entire label string, in pixels. */
	gint label_width;

	/* This is the number of characters we can fit in, or
	   E_CLIPPED_LABEL_NEED_RECALC if it needs to be recalculated, or
	   E_CLIPPED_LABEL_SHOW_ENTIRE_LABEL to show the entire label. */
	gint chars_displayed;

	/* This is the x position to display the ellipsis string, e.g. '...',
	   relative to the start of the label. */
	gint ellipsis_x;
};

struct _EClippedLabelClass
{
	GtkMiscClass parent_class;
};


GtkType	   e_clipped_label_get_type	(void);
GtkWidget* e_clipped_label_new		(const gchar	*text);

gchar*	   e_clipped_label_get_text	(EClippedLabel	*label);
void	   e_clipped_label_set_text	(EClippedLabel	*label,
					 const gchar	*text);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_CLIPPED_LABEL_H_ */
