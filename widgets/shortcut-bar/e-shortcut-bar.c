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
 * EShortcutBar displays a vertical bar with a number of Groups, each of which
 * contains any number of icons. It is used on the left of the main application
 * window so users can easily access items such as folders and files.
 *
 * The architecture is a bit complicated. EShortcutBar is a sublass of
 * EGroupBar (which supports a number of groups with buttons to slide them
 * into view). EShortcutBar places an EIconBar widget in each group page,
 * which displays an icon and name for each shortcut.
 */

#include <config.h>
#include <string.h>
#include <e-util/e-util.h>
#include "e-shortcut-bar.h"
#include "e-clipped-label.h"
#include "e-vscrolled-bar.h"

/* Drag and Drop stuff. */
enum {
	TARGET_SHORTCUT
};
static GtkTargetEntry target_table[] = {
	{ "E-SHORTCUT",     0, TARGET_SHORTCUT }
};
static guint n_targets = sizeof(target_table) / sizeof(target_table[0]);

gboolean   e_shortcut_bar_default_icon_loaded   = FALSE;
GdkPixbuf *e_shortcut_bar_default_icon		= NULL;
gchar	  *e_shortcut_bar_default_icon_filename = "gnome-folder.png";

static void e_shortcut_bar_class_init		(EShortcutBarClass *class);
static void e_shortcut_bar_init			(EShortcutBar	*shortcut_bar);
static void e_shortcut_bar_destroy		(GtkObject	*object);

static void e_shortcut_bar_disconnect_model	(EShortcutBar	*shortcut_bar);

static void e_shortcut_bar_on_model_destroyed	(EShortcutModel	*model,
						 EShortcutBar	*shortcut_bar);
static void e_shortcut_bar_on_group_added	(EShortcutModel	*model,
						 gint		 group_num,
						 gchar		*group_name,
						 EShortcutBar	*shortcut_bar);
static void e_shortcut_bar_on_group_removed	(EShortcutModel	*model,
						 gint		 group_num,
						 EShortcutBar	*shortcut_bar);
static void e_shortcut_bar_on_item_added	(EShortcutModel *model,
						 gint		 group_num,
						 gint		 item_num,
						 gchar		*item_url,
						 gchar		*item_name,
						 EShortcutBar	*shortcut_bar);
static void e_shortcut_bar_on_item_removed	(EShortcutModel *model,
						 gint		 group_num,
						 gint		 item_num,
						 EShortcutBar	*shortcut_bar);

static gint e_shortcut_bar_add_group		(EShortcutBar	*shortcut_bar,
						 gint		 position,
						 const gchar	*group_name);
static void e_shortcut_bar_remove_group		(EShortcutBar	*shortcut_bar,
						 gint		 group_num);
static gint e_shortcut_bar_add_item		(EShortcutBar	*shortcut_bar,
						 gint		 group_num,
						 gint		 position,
						 const gchar	*item_url,
						 const gchar	*item_name);
static void e_shortcut_bar_remove_item		(EShortcutBar	*shortcut_bar,
						 gint		 group_num,
						 gint		 item_num);


static void e_shortcut_bar_set_canvas_style	(EShortcutBar	*shortcut_bar,
						 GtkWidget	*canvas);
static void e_shortcut_bar_item_selected	(EIconBar	*icon_bar,
						 GdkEvent	*event,
						 gint		 item_num,
						 EShortcutBar	*shortcut_bar);
static void e_shortcut_bar_item_dragged		(EIconBar	*icon_bar,
						 GdkEvent	*event,
						 gint		 item_num,
						 EShortcutBar	*shortcut_bar);
static void e_shortcut_bar_on_drag_data_get	(GtkWidget      *widget,
						 GdkDragContext *context,
						 GtkSelectionData *selection_data,
						 guint           info,
						 guint           time,
						 EShortcutBar	*shortcut_bar);
static void e_shortcut_bar_on_drag_data_received(GtkWidget      *widget,
						 GdkDragContext *context,
						 gint            x,
						 gint            y,
						 GtkSelectionData *data,
						 guint           info,
						 guint           time,
						 EShortcutBar	*shortcut_bar);
static void e_shortcut_bar_on_drag_data_delete	(GtkWidget      *widget,
						 GdkDragContext *context,
						 EShortcutBar   *shortcut_bar);
static void e_shortcut_bar_on_drag_end		(GtkWidget      *widget,
						 GdkDragContext *context,
						 EShortcutBar   *shortcut_bar);
static void e_shortcut_bar_stop_editing		(GtkWidget	*button,
						 EShortcutBar	*shortcut_bar);
static GdkPixbuf* e_shortcut_bar_get_image_from_url (EShortcutBar *shortcut_bar,
						     const gchar *item_url);
static GdkPixbuf* e_shortcut_bar_load_image	(const gchar	*filename);


enum
{
  ITEM_SELECTED,
  LAST_SIGNAL
};

static guint e_shortcut_bar_signals[LAST_SIGNAL] = {0};

static EGroupBarClass *parent_class;


E_MAKE_TYPE(e_shortcut_bar, "EShortcutBar", EShortcutBar,
	    e_shortcut_bar_class_init, e_shortcut_bar_init,
	    e_group_bar_get_type())


static void
e_shortcut_bar_class_init (EShortcutBarClass *class)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;

	parent_class = gtk_type_class (e_group_bar_get_type ());

	object_class = (GtkObjectClass *) class;
	widget_class = (GtkWidgetClass *) class;

	e_shortcut_bar_signals[ITEM_SELECTED] =
		gtk_signal_new ("item_selected",
				GTK_RUN_LAST | GTK_RUN_ACTION,
				object_class->type,
				GTK_SIGNAL_OFFSET (EShortcutBarClass,
						   selected_item),
				gtk_marshal_NONE__POINTER_INT_INT,
				GTK_TYPE_NONE, 3, GTK_TYPE_GDK_EVENT,
				GTK_TYPE_INT, GTK_TYPE_INT);

	gtk_object_class_add_signals (object_class, e_shortcut_bar_signals,
				      LAST_SIGNAL);

	/* Method override */
	object_class->destroy		= e_shortcut_bar_destroy;
}


static void
e_shortcut_bar_init (EShortcutBar *shortcut_bar)
{
	shortcut_bar->groups = g_array_new (FALSE, FALSE,
					    sizeof (EShortcutBarGroup));

	shortcut_bar->dragged_url = NULL;
	shortcut_bar->dragged_name = NULL;
}


GtkWidget *
e_shortcut_bar_new (void)
{
	GtkWidget *shortcut_bar;

	shortcut_bar = GTK_WIDGET (gtk_type_new (e_shortcut_bar_get_type ()));

	return shortcut_bar;
}


static void
e_shortcut_bar_destroy (GtkObject *object)
{
	EShortcutBar *shortcut_bar;

	shortcut_bar = E_SHORTCUT_BAR (object);

	g_array_free (shortcut_bar->groups, TRUE);

	g_free (shortcut_bar->dragged_url);
	g_free (shortcut_bar->dragged_name);

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}


void
e_shortcut_bar_set_model (EShortcutBar *shortcut_bar,
			  EShortcutModel *model)
{
	gint num_groups, group_num, num_items, item_num;
	gchar *group_name, *item_url, *item_name;

	/* Disconnect any existing model. */
	e_shortcut_bar_disconnect_model (shortcut_bar);

	shortcut_bar->model = model;

	if (!model)
		return;

	gtk_signal_connect (GTK_OBJECT (model), "destroy",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_on_model_destroyed),
			    shortcut_bar);
	gtk_signal_connect (GTK_OBJECT (model), "group_added",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_on_group_added),
			    shortcut_bar);
	gtk_signal_connect (GTK_OBJECT (model), "group_removed",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_on_group_removed),
			    shortcut_bar);
	gtk_signal_connect (GTK_OBJECT (model), "item_added",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_on_item_added),
			    shortcut_bar);
	gtk_signal_connect (GTK_OBJECT (model), "item_removed",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_on_item_removed),
			    shortcut_bar);

	/* Add any items already in the model. */
	num_groups = e_shortcut_model_get_num_groups (model);
	for (group_num = 0; group_num < num_groups; group_num++) {
		group_name = e_shortcut_model_get_group_name (model,
							      group_num);
		e_shortcut_bar_add_group (shortcut_bar, group_num, group_name);
		g_free (group_name);

		num_items = e_shortcut_model_get_num_items (model, group_num);
		for (item_num = 0; item_num < num_items; item_num++) {
			e_shortcut_model_get_item_info (model, group_num,
							item_num, &item_url,
							&item_name);
			e_shortcut_bar_add_item (shortcut_bar, group_num,
						 item_num, item_url,
						 item_name);
			g_free (item_url);
			g_free (item_name);
		}
	}
}


static void
e_shortcut_bar_disconnect_model (EShortcutBar *shortcut_bar)
{
	/* Remove all the current groups. */
	while (shortcut_bar->groups->len)
		e_shortcut_bar_remove_group (shortcut_bar, 0);

	if (shortcut_bar->model) {
		/* Disconnect all the signals in one go. */
		gtk_signal_disconnect_by_data (GTK_OBJECT (shortcut_bar->model), shortcut_bar);
		shortcut_bar->model = NULL;
	}
}


static void
e_shortcut_bar_on_model_destroyed	(EShortcutModel	*model,
					 EShortcutBar	*shortcut_bar)
{
	e_shortcut_bar_disconnect_model (shortcut_bar);
}


static void
e_shortcut_bar_on_group_added		(EShortcutModel *model,
					 gint		 group_num,
					 gchar		*group_name,
					 EShortcutBar	*shortcut_bar)
{
	e_shortcut_bar_add_group (shortcut_bar, group_num, group_name);
}


static void
e_shortcut_bar_on_group_removed		(EShortcutModel *model,
					 gint		 group_num,
					 EShortcutBar	*shortcut_bar)
{
	e_shortcut_bar_remove_group (shortcut_bar, group_num);
}


static void
e_shortcut_bar_on_item_added		(EShortcutModel *model,
					 gint		 group_num,
					 gint		 item_num,
					 gchar		*item_url,
					 gchar		*item_name,
					 EShortcutBar	*shortcut_bar)
{
	e_shortcut_bar_add_item (shortcut_bar, group_num, item_num,
				 item_url, item_name);
}


static void
e_shortcut_bar_on_item_removed (EShortcutModel *model,
				gint group_num,
				gint item_num,
				EShortcutBar *shortcut_bar)
{
	e_shortcut_bar_remove_item (shortcut_bar, group_num, item_num);
}



static gint
e_shortcut_bar_add_group	(EShortcutBar	*shortcut_bar,
				 gint		 position,
				 const gchar	*group_name)
{
	EShortcutBarGroup *group, tmp_group;
	gint group_num;
	GtkWidget *button, *label;

	g_return_val_if_fail (E_IS_SHORTCUT_BAR (shortcut_bar), -1);
	g_return_val_if_fail (group_name != NULL, -1);

	gtk_widget_push_colormap (gdk_rgb_get_cmap ());
	gtk_widget_push_visual (gdk_rgb_get_visual ());

	group_num = position;
	g_array_insert_val (shortcut_bar->groups, group_num, tmp_group);

	group = &g_array_index (shortcut_bar->groups,
				EShortcutBarGroup, group_num);

	group->vscrolled_bar = e_vscrolled_bar_new (NULL);
	gtk_widget_show (group->vscrolled_bar);
	gtk_signal_connect (
		GTK_OBJECT (E_VSCROLLED_BAR (group->vscrolled_bar)->up_button),
		"pressed", GTK_SIGNAL_FUNC (e_shortcut_bar_stop_editing), shortcut_bar);
	gtk_signal_connect (
		GTK_OBJECT (E_VSCROLLED_BAR (group->vscrolled_bar)->down_button),
		"pressed", GTK_SIGNAL_FUNC (e_shortcut_bar_stop_editing), shortcut_bar);

	group->icon_bar = e_icon_bar_new ();
	gtk_widget_show (group->icon_bar);
	gtk_container_add (GTK_CONTAINER (group->vscrolled_bar),
			   group->icon_bar);
	gtk_signal_connect (GTK_OBJECT (group->icon_bar), "item_selected",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_item_selected),
			    shortcut_bar);
	gtk_signal_connect (GTK_OBJECT (group->icon_bar), "item_dragged",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_item_dragged),
			    shortcut_bar);
	gtk_signal_connect (GTK_OBJECT (group->icon_bar), "drag_data_get",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_on_drag_data_get),
			    shortcut_bar);
	gtk_signal_connect (GTK_OBJECT (group->icon_bar), "drag_data_received",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_on_drag_data_received),
			    shortcut_bar);
	gtk_signal_connect (GTK_OBJECT (group->icon_bar), "drag_data_delete",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_on_drag_data_delete),
			    shortcut_bar);
	gtk_signal_connect (GTK_OBJECT (group->icon_bar), "drag_end",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_on_drag_end),
			    shortcut_bar);

	e_shortcut_bar_set_canvas_style (shortcut_bar, group->icon_bar);

	button = gtk_button_new ();
	label = e_clipped_label_new (group_name);
	gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);
	gtk_widget_show (label);
	gtk_container_add (GTK_CONTAINER (button), label);
	gtk_widget_show (button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (e_shortcut_bar_stop_editing),
			    shortcut_bar);

	gtk_drag_dest_set (GTK_WIDGET (group->icon_bar),
			   GTK_DEST_DEFAULT_ALL,
			   target_table, n_targets,
			   GDK_ACTION_COPY | GDK_ACTION_MOVE);
	gtk_drag_dest_set (GTK_WIDGET (button),
			   GTK_DEST_DEFAULT_ALL,
			   target_table, n_targets,
			   GDK_ACTION_COPY | GDK_ACTION_MOVE);

	e_group_bar_add_group (E_GROUP_BAR (shortcut_bar),
			       group->vscrolled_bar, button, -1);

	gtk_widget_pop_visual ();
	gtk_widget_pop_colormap ();

	return group_num;
}


static void
e_shortcut_bar_remove_group	(EShortcutBar	*shortcut_bar,
				 gint		 group_num)
{
	e_group_bar_remove_group (E_GROUP_BAR (shortcut_bar), group_num);
	g_array_remove_index (shortcut_bar->groups, group_num);
}


static gint
e_shortcut_bar_add_item		(EShortcutBar	*shortcut_bar,
				 gint		 group_num,
				 gint		 position,
				 const gchar	*item_url,
				 const gchar	*item_name)
{
	EShortcutBarGroup *group;
	GdkPixbuf *image;
	gint item_num;

	g_return_val_if_fail (E_IS_SHORTCUT_BAR (shortcut_bar), -1);
	g_return_val_if_fail (group_num >= 0, -1);
	g_return_val_if_fail (group_num < shortcut_bar->groups->len, -1);
	g_return_val_if_fail (item_url != NULL, -1);
	g_return_val_if_fail (item_name != NULL, -1);

	image = e_shortcut_bar_get_image_from_url (shortcut_bar, item_url);
	group = &g_array_index (shortcut_bar->groups,
				EShortcutBarGroup, group_num);

	item_num = e_icon_bar_add_item (E_ICON_BAR (group->icon_bar),
					image, item_name, position);
	gdk_pixbuf_unref (image);
	e_icon_bar_set_item_data_full (E_ICON_BAR (group->icon_bar), item_num,
				       g_strdup (item_url), g_free);

	return item_num;
}


static void
e_shortcut_bar_remove_item	(EShortcutBar	*shortcut_bar,
				 gint		 group_num,
				 gint		 item_num)
{
	EShortcutBarGroup *group;

	g_return_if_fail (E_IS_SHORTCUT_BAR (shortcut_bar));
	g_return_if_fail (group_num >= 0);
	g_return_if_fail (group_num < shortcut_bar->groups->len);

	group = &g_array_index (shortcut_bar->groups,
				EShortcutBarGroup, group_num);

	e_icon_bar_remove_item (E_ICON_BAR (group->icon_bar), item_num);
}


static void
e_shortcut_bar_set_canvas_style (EShortcutBar *shortcut_bar,
				 GtkWidget *canvas)
{
        GtkRcStyle *rc_style;

        rc_style = gtk_rc_style_new ();

        rc_style->color_flags[GTK_STATE_NORMAL] = GTK_RC_FG | GTK_RC_BG;
        rc_style->fg[GTK_STATE_NORMAL].red   = 65535;
        rc_style->fg[GTK_STATE_NORMAL].green = 65535;
        rc_style->fg[GTK_STATE_NORMAL].blue  = 65535;

        rc_style->bg[GTK_STATE_NORMAL].red   = 32512;
        rc_style->bg[GTK_STATE_NORMAL].green = 32512;
        rc_style->bg[GTK_STATE_NORMAL].blue  = 32512;

        gtk_widget_modify_style (GTK_WIDGET (canvas), rc_style);
        gtk_rc_style_unref (rc_style);
}


void
e_shortcut_bar_set_view_type (EShortcutBar *shortcut_bar,
			      gint group_num,
			      EIconBarViewType view_type)
{
	EShortcutBarGroup *group;

	g_return_if_fail (E_IS_SHORTCUT_BAR (shortcut_bar));
	g_return_if_fail (group_num >= 0);
	g_return_if_fail (group_num < shortcut_bar->groups->len);

	group = &g_array_index (shortcut_bar->groups,
				EShortcutBarGroup, group_num);

	e_icon_bar_set_view_type (E_ICON_BAR (group->icon_bar), view_type);
}


EIconBarViewType
e_shortcut_bar_get_view_type (EShortcutBar *shortcut_bar,
			      gint group_num)
{
	EShortcutBarGroup *group;

	g_return_val_if_fail (E_IS_SHORTCUT_BAR (shortcut_bar), E_ICON_BAR_SMALL_ICONS);
	g_return_val_if_fail (group_num >= 0, E_ICON_BAR_SMALL_ICONS);
	g_return_val_if_fail (group_num < shortcut_bar->groups->len, E_ICON_BAR_SMALL_ICONS);

	group = &g_array_index (shortcut_bar->groups,
				EShortcutBarGroup, group_num);

	return E_ICON_BAR (group->icon_bar)->view_type;
}


static void
e_shortcut_bar_item_selected (EIconBar *icon_bar,
			      GdkEvent *event,
			      gint item_num,
			      EShortcutBar *shortcut_bar)
{
	gint group_num;

	group_num = e_group_bar_get_group_num (E_GROUP_BAR (shortcut_bar),
					       GTK_WIDGET (icon_bar)->parent);

	gtk_signal_emit (GTK_OBJECT (shortcut_bar),
			 e_shortcut_bar_signals[ITEM_SELECTED],
			 event, group_num, item_num);
}


static void
e_shortcut_bar_item_dragged (EIconBar *icon_bar,
			     GdkEvent *event,
			     gint item_num,
			     EShortcutBar *shortcut_bar)
{
	GtkTargetList *target_list;
	gint group_num;

	group_num = e_group_bar_get_group_num (E_GROUP_BAR (shortcut_bar),
					       GTK_WIDGET (icon_bar)->parent);

	shortcut_bar->dragged_url = g_strdup (e_icon_bar_get_item_data (icon_bar, item_num));
	shortcut_bar->dragged_name = e_icon_bar_get_item_text (icon_bar, item_num);

	target_list = gtk_target_list_new (target_table, n_targets);
	gtk_drag_begin (GTK_WIDGET (icon_bar), target_list,
			GDK_ACTION_COPY | GDK_ACTION_MOVE,
			1, event);
	gtk_target_list_unref (target_list);
}


static void
e_shortcut_bar_on_drag_data_get (GtkWidget          *widget,
				 GdkDragContext     *context,
				 GtkSelectionData   *selection_data,
				 guint               info,
				 guint               time,
				 EShortcutBar	    *shortcut_bar)
{
	gchar *data;

	if (info == TARGET_SHORTCUT) {
		data = g_strdup_printf ("%s%c%s", shortcut_bar->dragged_name,
					'\0', shortcut_bar->dragged_url);
		gtk_selection_data_set (selection_data,	selection_data->target,
					8, data,
					strlen (shortcut_bar->dragged_name)
					+ strlen (shortcut_bar->dragged_url)
					+ 2);
		g_free (data);
	}
}


static void  
e_shortcut_bar_on_drag_data_received  (GtkWidget          *widget,
				       GdkDragContext     *context,
				       gint                x,
				       gint                y,
				       GtkSelectionData   *data,
				       guint               info,
				       guint               time,
				       EShortcutBar	  *shortcut_bar)
{
	gchar *item_name, *item_url;
	EIconBar *icon_bar;
	gint position, group_num;

	icon_bar = E_ICON_BAR (widget);
	position = icon_bar->dragging_before_item_num;

	if ((data->length >= 0) && (data->format == 8)
	    &&  position != -1) {
		item_name = data->data;
		item_url = item_name + strlen (item_name) + 1;

		group_num = e_group_bar_get_group_num (E_GROUP_BAR (shortcut_bar),
						       GTK_WIDGET (icon_bar)->parent);

		e_shortcut_model_add_item (shortcut_bar->model, group_num,
					   position, item_url, item_name);

		gtk_drag_finish (context, TRUE, TRUE, time);
		return;
	}
  
	gtk_drag_finish (context, FALSE, FALSE, time);
}


static void  
e_shortcut_bar_on_drag_data_delete (GtkWidget          *widget,
				    GdkDragContext     *context,
				    EShortcutBar       *shortcut_bar)
{
	EIconBar *icon_bar;
	gint group_num;

	icon_bar = E_ICON_BAR (widget);

	group_num = e_group_bar_get_group_num (E_GROUP_BAR (shortcut_bar),
					       widget->parent);

	e_shortcut_model_remove_item (shortcut_bar->model, group_num,
				      icon_bar->dragged_item_num);
}


static void
e_shortcut_bar_on_drag_end (GtkWidget      *widget,
			    GdkDragContext *context,
			    EShortcutBar   *shortcut_bar)
{
	g_free (shortcut_bar->dragged_name);
	shortcut_bar->dragged_name = NULL;

	g_free (shortcut_bar->dragged_url);
	shortcut_bar->dragged_url = NULL;
}


void
e_shortcut_bar_start_editing_item (EShortcutBar *shortcut_bar,
				   gint group_num,
				   gint item_num)
{
	EShortcutBarGroup *group;

	g_return_if_fail (E_IS_SHORTCUT_BAR (shortcut_bar));
	g_return_if_fail (group_num >= 0);
	g_return_if_fail (group_num < shortcut_bar->groups->len);

	group = &g_array_index (shortcut_bar->groups,
				EShortcutBarGroup, group_num);

	e_icon_bar_start_editing_item (E_ICON_BAR (group->icon_bar), item_num);
}


/* We stop editing any item when a scroll button is pressed. */
static void
e_shortcut_bar_stop_editing (GtkWidget *button,
			     EShortcutBar *shortcut_bar)
{
	EShortcutBarGroup *group;
	gint group_num;

	for (group_num = 0;
	     group_num < shortcut_bar->groups->len;
	     group_num++) {
		group = &g_array_index (shortcut_bar->groups,
					EShortcutBarGroup, group_num);
		e_icon_bar_stop_editing_item (E_ICON_BAR (group->icon_bar),
					      TRUE);
	}
}


/* Sets the callback which is called to return the icon to use for a particular
   URL. */
void
e_shortcut_bar_set_icon_callback (EShortcutBar *shortcut_bar,
				  EShortcutBarIconCallback cb,
				  gpointer data)
{
	shortcut_bar->icon_callback = cb;
	shortcut_bar->icon_callback_data = data;
}


static GdkPixbuf *
e_shortcut_bar_get_image_from_url (EShortcutBar *shortcut_bar,
				   const gchar *item_url)
{
	GdkPixbuf *icon = NULL;

	if (shortcut_bar->icon_callback)
		icon = (*shortcut_bar->icon_callback) (shortcut_bar,
						       item_url,
						       shortcut_bar->icon_callback_data);

	if (!icon) {
		if (!e_shortcut_bar_default_icon_loaded) {
			e_shortcut_bar_default_icon_loaded = TRUE;
			e_shortcut_bar_default_icon = e_shortcut_bar_load_image (e_shortcut_bar_default_icon_filename);
		}
		icon = e_shortcut_bar_default_icon;
		/* ref the default icon each time we return it */
		gdk_pixbuf_ref (icon);
	}

	return icon;
}


static GdkPixbuf *
e_shortcut_bar_load_image (const gchar *filename)
{
	gchar *pathname;
	GdkPixbuf *image = NULL;

	pathname = gnome_pixmap_file (filename);
	if (pathname)
		image = gdk_pixbuf_new_from_file (pathname);
	else
		g_warning ("Couldn't find pixmap: %s", filename);

	g_free (pathname);

	return image;
}
