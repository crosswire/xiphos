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
#ifndef _E_SHORTCUT_MODEL_H_
#define _E_SHORTCUT_MODEL_H_

#include <gtk/gtkobject.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * EShortcutModel keeps track of a number of groups of shortcuts.
 * Each shortcut has a URL and a short textual name.
 * It is used as the model of the EShortcutBar.
 */

typedef struct _EShortcutModel       EShortcutModel;
typedef struct _EShortcutModelClass  EShortcutModelClass;


#define E_TYPE_SHORTCUT_MODEL	       (e_shortcut_model_get_type ())
#define E_SHORTCUT_MODEL(obj)          GTK_CHECK_CAST (obj, e_shortcut_model_get_type (), EShortcutModel)
#define E_SHORTCUT_MODEL_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, e_shortcut_model_get_type (), EShortcutModelClass)
#define E_IS_SHORTCUT_MODEL(obj)       GTK_CHECK_TYPE (obj, e_shortcut_model_get_type ())


struct _EShortcutModel
{
	GtkObject object;

	/* This is a private array of EShortcutModelGroup elements. */
	GArray *groups;
};

struct _EShortcutModelClass
{
	GtkObjectClass parent_class;

	/*
	 * Signals - These act like virtual functions as well as notification
	 * signals. The class function is run first and does the actual action.
	 */
	void   (*group_added)	       (EShortcutModel *shortcut_model,
					gint	        group_num,
					const gchar    *group_name);
	void   (*group_removed)	       (EShortcutModel *shortcut_model,
					gint		group_num);

	void   (*item_added)	       (EShortcutModel *shortcut_model,
					gint		group_num,
					gint	        item_num,
					const gchar    *item_url,
					const gchar    *item_name);
	void   (*item_removed)	       (EShortcutModel *shortcut_model,
					gint		group_num,
					gint		item_num);

	/*
	 * Virtual methods.
	 */
	gint   (*get_num_groups)       (EShortcutModel *shortcut_model);
	gint   (*get_num_items)        (EShortcutModel *shortcut_model,
					gint		group_num);
	/* This returns a copy of the group name. */
	gchar* (*get_group_name)       (EShortcutModel *shortcut_model,
					gint		group_num);
	/* This returns a copy of the item url and name. */
	void   (*get_item_info)        (EShortcutModel *shortcut_model,
					gint		group_num,
					gint		item_num,
					gchar	      **item_url,
					gchar	      **item_name);
};


GtkType	   	  e_shortcut_model_get_type	(void);
EShortcutModel*	  e_shortcut_model_new		(void);

/* Adds a new group at the given position, or last if position is -1.
   It returns the index of the new group. */
gint	   	  e_shortcut_model_add_group	(EShortcutModel	 *shortcut_model,
						 gint		  position,
						 const gchar	 *group_name);
void	   	  e_shortcut_model_remove_group	(EShortcutModel	 *shortcut_model,
						 gint		  group_num);

/* Adds a new item to a group at the given position, or last if position is -1.
   It returns the index of the new item within the group. */
gint	   	  e_shortcut_model_add_item	(EShortcutModel	 *shortcut_model,
						 gint		  group_num,
						 gint		  position,
						 const gchar	 *item_url,
						 const gchar	 *item_name);

void	   	  e_shortcut_model_remove_item   (EShortcutModel *shortcut_model,
						  gint		  group_num,
						  gint		  item_num);

gint		  e_shortcut_model_get_num_groups(EShortcutModel *shortcut_model);
gint		  e_shortcut_model_get_num_items (EShortcutModel *shortcut_model,
						  gint		group_num);
/* The group_name should be freed after use. */
gchar*		  e_shortcut_model_get_group_name(EShortcutModel *shortcut_model,
						  gint		group_num);
/* The item_url and item_name should be freed after use. */
void		  e_shortcut_model_get_item_info (EShortcutModel *shortcut_model,
						  gint		group_num,
						  gint		item_num,
						  gchar	      **item_url,
						  gchar	      **item_name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _E_SHORTCUT_MODEL_H_ */
