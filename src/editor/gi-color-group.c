/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * color-group.c - Utility to keep a shered memory of custom colors
 *                 between arbitrary widgets.
 * Copyright 2000, Michael Levy
 * Copyright 2001, Almer S. Tigelaar
 *
 * Authors:
 * 	Michael Levy (mlevy@genoscope.cns.fr)
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

#include <string.h>
#include <glib-object.h>
#include <gdk/gdkcolor.h>
#include "gi-color-group.h"
#include "gi-utils.h"

#define PARENT_TYPE	G_TYPE_OBJECT

enum {
        CUSTOM_COLOR_ADD,
        LAST_SIGNAL
};

static GObjectClass *parent_class;

static GQuark color_group_signals [LAST_SIGNAL] = { 0 };

static void color_group_finalize (GObject *obj);

static void
color_group_class_init (ColorGroupClass *klass)
{
	GObjectClass *object_class;

	object_class = (GObjectClass*) klass;
	
	object_class->finalize = &color_group_finalize;
	parent_class = g_type_class_peek (PARENT_TYPE);

	color_group_signals [CUSTOM_COLOR_ADD] =
		g_signal_new ("custom_color_add",
			COLOR_GROUP_TYPE,
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET (ColorGroupClass, custom_color_add),
			(GSignalAccumulator) NULL, NULL,
			g_cclosure_marshal_VOID__POINTER,
			G_TYPE_NONE,
			1, G_TYPE_POINTER);
}

static void
color_group_init (ColorGroup *cg)
{
	cg->name = NULL;
	cg->history = NULL;
	cg->history_size = 0;
}

TMP_GAL_MAKE_TYPE(color_group,
		  "ColorGroup",
		  ColorGroup,
		  color_group_class_init,
		  color_group_init,
		  PARENT_TYPE)

/* Hash table used to ensure unicity in newly created names*/
static GHashTable *group_names = NULL;

static guint
cg_hash (gconstpointer	key)
{
	/* Do NOT use smart type checking it will not work for the tmp_key */
	return g_str_hash (((ColorGroup *)key)->name);
}

static gint
cg_cmp (gconstpointer a, gconstpointer	b)
{
	/* Do NOT use smart type checking it will not work for the tmp_key */
	ColorGroup const *cg_a = (ColorGroup *)a;
	ColorGroup const *cg_b = (ColorGroup *)b;
	if (cg_a == cg_b)
		return TRUE;
	if (cg_a->context != cg_b->context)
		return FALSE;
	return g_str_equal (cg_a->name, cg_b->name);
}

static void
initialize_group_names (void)
{
	g_assert (group_names == NULL);
	group_names = g_hash_table_new (cg_hash, cg_cmp);
}

/**
 * color_group_get :
 * @name :
 * @context : 
 *
 * Look up the name/context specific color-group.  Return NULL if it is not found.
 * No reference is added if it is found.
 */
ColorGroup *
color_group_get (const gchar * name, gpointer context)
{
	ColorGroup tmp_key;
	gpointer res;

	g_assert(group_names);

	g_return_val_if_fail(name != NULL, NULL);

	tmp_key.name = (char *)name;
	tmp_key.context = context;
	res = g_hash_table_lookup (group_names, &tmp_key);

	if (res != NULL)
		return COLOR_GROUP (res);
	else
		return NULL;
}

static gchar *
create_unique_name (gpointer context)
{
	const gchar *prefix = "__cg_autogen_name__";
	static gint latest_suff = 0;
	gchar *new_name;

	for(;;latest_suff++) {
		new_name = g_strdup_printf("%s%i", prefix, latest_suff);
		if (color_group_get (new_name, context) == NULL)
			return new_name;
		else
			g_free(new_name);
	}
	g_assert_not_reached();
}

static void
color_group_finalize (GObject *obj)
{
	ColorGroup *cg;

	g_return_if_fail(obj != NULL);
	g_return_if_fail(IS_COLOR_GROUP(obj));
	g_assert(group_names != NULL);

	cg = COLOR_GROUP (obj);

	/* make this name available */
	if (cg->name) {
		g_hash_table_remove (group_names, cg);
		g_free (cg->name);
		cg->name = NULL;
	}

	if (cg->history) {
		/* Free the whole colour history */
		while ((int) cg->history->len > 0)
			gdk_color_free ((GdkColor *)
					g_ptr_array_remove_index (cg->history, 0));
		g_ptr_array_free (cg->history, TRUE);
		cg->history = NULL;
	}
	
	if (parent_class->finalize)
		(parent_class->finalize) (obj);
}

/*
 * color_group_get_history_size:
 * Get the size of the custom color history
 */
gint
color_group_get_history_size (ColorGroup *cg)
{
	g_return_val_if_fail (cg != NULL, 0);
	
	return cg->history_size;
}

/*
 * Change the size of the custom color history.
 */
void
color_group_set_history_size (ColorGroup *cg, gint size)
{
	g_return_if_fail(cg != NULL);
	g_return_if_fail(size >= 0);

	/* Remove excess elements (begin with kicking out the oldest) */
	while ((int) cg->history->len > size)
		gdk_color_free ((GdkColor *) g_ptr_array_remove_index (cg->history, 0));
}

/*
 * color_group_fetch :
 * @name :
 * @context :
 *
 * if name is NULL or a name not currently in use by another group
 * then a new group is created and returned. If name was NULL
 * then the new group is given a unique name prefixed by "__cg_autogen_name__"
 * (thereby insuring namespace separation).
 * If name was already used by a group then the reference count is
 * incremented and a pointer to the group is returned.
 */
ColorGroup *
color_group_fetch (const gchar *name, gpointer context)
{
	ColorGroup *cg;
	gchar *new_name;

	if (group_names == NULL)
		initialize_group_names();

	if (name == NULL)
		new_name = create_unique_name (context);
	else
		new_name = g_strdup (name);

	cg = color_group_get (new_name, context);
	if (cg != NULL) {
		g_free (new_name);
		g_object_ref (G_OBJECT (cg));
		return cg;
	}

	/* Take care of creating the new object */
	cg = g_object_new (color_group_get_type (), NULL);
	g_return_val_if_fail(cg != NULL, NULL);

	cg->name = new_name;
	cg->context = context;

	/* Create history */
	cg->history = g_ptr_array_new ();

	/* FIXME: Why not 8? We never use more then 8 on the palette,
	 * maybe we can't free colors while they are still on the palette and
	 * need to be sure they are not on it when we free them and thus we
	 * make the upper limit twice the size of the number of displayed items
	 * (2 * 8) ?
	 */
	cg->history_size = 16; 

	/* lastly register this name */
	g_hash_table_insert (group_names, cg, cg);

	return cg;
}

/*
 * color_group_get_custom_colors:
 * Retrieve all custom colors currently in the history using a callback
 * mechanism. The custom colors will be passed from the oldest to the newest.
 */
void
color_group_get_custom_colors (ColorGroup *cg, CbCustomColors callback, gpointer user_data)
{
	int i;

	g_return_if_fail (cg != NULL);

	/* Invoke the callback for our full history */
	for (i = 0; i < (int) cg->history->len; i++) {
		GdkColor const * const color = g_ptr_array_index (cg->history, i);

		callback (color, user_data);
	}
}

/*
 * color_group_add_color:
 * Changes the colors. The color to be set should always be a custom
 * color! It has no use adding a color which is already in the default
 * palette.
 */
void
color_group_add_color (ColorGroup *cg, GdkColor const * const color)
{
	int i;
	
	g_return_if_fail(cg != NULL);
	g_return_if_fail(color != NULL); /* Can't be NULL */

	/* Let's be smart and see if it's already in our history, no need to add it again*/
	for (i = 0; i < (int) cg->history->len; i++) {
		GdkColor *current = g_ptr_array_index (cg->history, i);
		
		if (gdk_color_equal (color, current))
			return;
	}

	/*
	 * We make our own private copy of the color passed and put
	 * it in the history, this is freed later.
	 */
	if (cg->history_size > 0)
		g_ptr_array_add (cg->history, gdk_color_copy (color));

	/* Shift out the oldest item if we grow beyond our set size */
	if ((int) cg->history->len > cg->history_size)
		gdk_color_free ((GdkColor *) g_ptr_array_remove_index (cg->history, 0));

	/* Tell color-palette's that use this group that
	 * a new custom color was added.
	 */
	g_signal_emit (G_OBJECT(cg),
		color_group_signals [CUSTOM_COLOR_ADD],
		0,
		color);
}
