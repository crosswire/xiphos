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
#ifndef _E_SHORTCUT_BAR_H_
#define _E_SHORTCUT_BAR_H_

#include "e-group-bar.h"
#include "e-icon-bar.h"
#include "e-shortcut-model.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * EShortcutBar displays a vertical bar with a number of Groups, each of which
 * contains any number of icons. It is used on the left of the main application
 * window so users can easily access items such as folders and files.
 */

typedef struct _EShortcutBar       EShortcutBar;
typedef struct _EShortcutBarClass  EShortcutBarClass;


typedef GdkPixbuf* (*EShortcutBarIconCallback)   (EShortcutBar *shortcut_bar,
						  const gchar  *url,
						  gpointer      data);

/* This contains information on one group. */
typedef struct _EShortcutBarGroup   EShortcutBarGroup;
struct _EShortcutBarGroup
{
	/* This is the EVScrolledBar which scrolls the group. */
	GtkWidget *vscrolled_bar;

	/* This is the icon bar containing the child items. */
	GtkWidget *icon_bar;
};


#define E_TYPE_SHORTCUT_BAR	     (e_shortcut_bar_get_type ())
#define E_SHORTCUT_BAR(obj)          GTK_CHECK_CAST (obj, e_shortcut_bar_get_type (), EShortcutBar)
#define E_SHORTCUT_BAR_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, e_shortcut_bar_get_type (), EShortcutBarClass)
#define E_IS_SHORTCUT_BAR(obj)       GTK_CHECK_TYPE (obj, e_shortcut_bar_get_type ())


struct _EShortcutBar
{
	EGroupBar group_bar;

	/* This is the underlying model. */
	EShortcutModel *model;

	/* This is an array of EShortcutBarGroup elements. */
	GArray *groups;

	/* The callback which the application sets to return the icon to use
	   for a given URL. */
	EShortcutBarIconCallback icon_callback;

	/* Closure for the callback. */
	gpointer icon_callback_data;

	gchar *dragged_url;
	gchar *dragged_name;
};

struct _EShortcutBarClass
{
	EGroupBarClass parent_class;

	void   (*selected_item)        (EShortcutBar   *shortcut_bar,
					GdkEvent       *event,
					gint		group_num,
					gint		item_num);
};


GtkType	   	  e_shortcut_bar_get_type	     (void);
GtkWidget* 	  e_shortcut_bar_new		     (void);

/* Sets the underlying model. */
void		  e_shortcut_bar_set_model	     (EShortcutBar	 *shortcut_bar,
						      EShortcutModel	 *shortcut_model);

/* Sets/gets the view type for the group. */
void              e_shortcut_bar_set_view_type	     (EShortcutBar	 *shortcut_bar,
						      gint		  group_num,
						      EIconBarViewType view_type);
EIconBarViewType  e_shortcut_bar_get_view_type	     (EShortcutBar	 *shortcut_bar,
						      gint		  group_num);

void	   	  e_shortcut_bar_start_editing_item  (EShortcutBar	 *shortcut_bar,
						      gint		  group_num,
						      gint		  item_num);

/* Sets the callback which is called to return the icon to use for a particular
   URL. This callback must be set before any items are added. If the callback
   returns NULL the default icon is used. */
void	          e_shortcut_bar_set_icon_callback   (EShortcutBar    *shortcut_bar,
						      EShortcutBarIconCallback cb,
						      gpointer         data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_SHORTCUT_BAR_H_ */
