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
#ifndef _E_GROUP_BAR_H_
#define _E_GROUP_BAR_H_

#include <gtk/gtkcontainer.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * EGroupBar displays a vertical bar with a number of Groups, which are viewed
 * one at a time by selecting the Group's button. When a different Group is
 * selected, it slides into view, and the old Group slides out.
 * It is typically used on the left of the main application window so users
 * can easily access particular features.
 *
 * It is implemented like GtkNotebook, i.e. the main widgets are the children
 * of the EGroupBar and the button widgets are treated specially like the
 * GtkNotebook tab labels.
 */

/* This contains information on one item. */
typedef struct _EGroupBarChild   EGroupBarChild;
struct _EGroupBarChild
{
	/* This is the button used to select the group, and the window we use
	   to move it around easily. */
	GtkWidget *button;
	GdkWindow *button_window;
	gint button_height;

	/* This is the child widget, which can be any widget added by the
	   application, and the window we use to move it around easily. */
	GtkWidget *child;
	GdkWindow *child_window;

	/* These are TRUE if we are currently animating the windows. */
	gboolean button_window_in_animation;
	gboolean child_window_in_animation;

	/* These are the target y positions that the windows should eventually
	   move to, used for animation. If we get a size_allocate we just
	   update these and the animation can continue as normal.
	   When a child window reaches its target position, it is unmapped if
	   if it is not the current group (i.e. it has slid off screen). */
	gint button_window_target_y;
	gint child_window_target_y;
};


#define E_GROUP_BAR(obj)          GTK_CHECK_CAST (obj, e_group_bar_get_type (), EGroupBar)
#define E_GROUP_BAR_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, e_group_bar_get_type (), EGroupBarClass)
#define E_IS_GROUP_BAR(obj)       GTK_CHECK_TYPE (obj, e_group_bar_get_type ())


typedef struct _EGroupBar       EGroupBar;
typedef struct _EGroupBarClass  EGroupBarClass;

struct _EGroupBar
{
	GtkContainer container;

	/* This is an array of EGroupBarChild elements. */
	GArray *children;

	/* This is the group currently shown. */
	gint current_group_num;

	/* This is TRUE if all the buttons are allocated the same height. */
	gboolean buttons_homogeneous;

	/* This is the biggest requested height of all the buttons, which we
	   use for all buttons when buttons_homogeneous is set. */
	gint max_button_height;

	/* This is the height of all the child windows & widgets. */
	gint child_height;

	/* The id of the source function for animation timeouts. If this is
	   not 0 then we are in the middle of an animation. */
	guint animation_timeout_id;

	/* The id of the source function for automatically showing groups when
	   the user drags over the group button, and the group to show. */
	guint auto_show_timeout_id;
	gint auto_show_group_num;
};

struct _EGroupBarClass
{
	GtkContainerClass parent_class;
};


GtkType	   e_group_bar_get_type			(void);
GtkWidget* e_group_bar_new			(void);

/*
 * Insertion, reordering and deletion of items.
 */

/* Adds a new group at the given position. If position is -1 it adds it as
   the last group. It returns the group number. */
gint	   e_group_bar_add_group		(EGroupBar	*group_bar,
						 GtkWidget	*child,
						 GtkWidget	*button,
						 gint		 position);
void	   e_group_bar_reorder_group		(EGroupBar	*group_bar,
						 gint		 group_num,
						 gint		 new_position);
void	   e_group_bar_remove_group		(EGroupBar	*group_bar,
						 gint		 group_num);

/*
 * Getting & setting the current group.
 */
gint	   e_group_bar_get_current_group_num	(EGroupBar	*group_bar);
void	   e_group_bar_set_current_group_num	(EGroupBar	*group_bar,
						 gint		 group_num,
						 gboolean	 animate);

/*
 * Getting groups and group numbers.
 */
GtkWidget* e_group_bar_get_nth_group		(EGroupBar	*group_bar,
						 gint		 group_num);
gint	   e_group_bar_get_group_num		(EGroupBar	*group_bar,
						 GtkWidget	*child);

/*
 * Setting the group button label.
 */
void	   e_group_bar_set_group_button_label	(EGroupBar	*group_bar,
						 gint		 group_num,
						 GtkWidget	*label);

/*
 * Getting & setting the EGroupBar options.
 */
gboolean   e_group_bar_get_buttons_homogeneous	(EGroupBar	*group_bar);
void	   e_group_bar_set_buttons_homogeneous	(EGroupBar	*group_bar,
						 gboolean	 homogeneous);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_GROUP_BAR_H_ */
