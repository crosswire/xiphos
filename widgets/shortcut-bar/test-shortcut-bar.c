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
 * This tests the ShortcutBar widget.
 */

#include <gnome.h>

#include "e-shortcut-bar.h"

#include <widgets/e-paned/e-hpaned.h>

#define NUM_SHORTCUT_TYPES 5
gchar *shortcut_types[NUM_SHORTCUT_TYPES] = {
	"folder:", "file:", "calendar:", "todo:", "contacts:"
};
gchar *icon_filenames[NUM_SHORTCUT_TYPES] = {
	"gnome-balsa2.png", "gnome-folder.png", "gnome-calendar.png",
	"gnome-cromagnon.png", "gnome-ccthemes.png"
};
GdkPixbuf *icon_pixbufs[NUM_SHORTCUT_TYPES];

GtkWidget *main_label;

static GdkPixbuf *icon_callback  (EShortcutBar *shortcut_bar,
				  const gchar  *url,
				  gpointer      data);

static void on_main_label_size_allocate (GtkWidget *widget,
					 GtkAllocation *allocation,
					 gpointer data);
static void quit (GtkWidget *window, GdkEvent *event, gpointer data);
static void add_test_groups (EShortcutBar *shortcut_bar);
static void add_test_group (EShortcutBar *shortcut_bar, gint i,
			    gchar *group_name);
static gint get_random_int (gint max);

static void on_shortcut_bar_item_selected (EShortcutBar *shortcut_bar,
					   GdkEvent *event,
					   gint group_num,
					   gint item_num);
static void show_standard_popup (EShortcutBar *shortcut_bar,
				 GdkEvent *event,
				 gint group_num);
static void show_context_popup (EShortcutBar *shortcut_bar,
				GdkEvent *event,
				gint group_num,
				gint item_num);

static void set_large_icons (GtkWidget *menuitem,
			     EShortcutBar *shortcut_bar);
static void set_small_icons (GtkWidget *menuitem,
			     EShortcutBar *shortcut_bar);
static void remove_group (GtkWidget *menuitem,
			  EShortcutBar *shortcut_bar);

static void rename_item (GtkWidget *menuitem,
			 EShortcutBar *shortcut_bar);
static void remove_item (GtkWidget *menuitem,
			 EShortcutBar *shortcut_bar);
static void on_set_group_button_clicked (GtkWidget *button,
					 EShortcutBar *shortcut_bar);
static void on_set_group_button_no_animation_clicked (GtkWidget *button,
						      EShortcutBar *shortcut_bar);
static void on_item_added (EShortcutModel *shortcut_model,
			   gint group_num,
			   gint item_num);
static void on_item_removed (EShortcutModel *shortcut_model,
			     gint group_num,
			     gint item_num);
static void on_group_added (EShortcutModel *shortcut_model,
			    gint group_num);
static void on_group_removed (EShortcutModel *shortcut_model,
			      gint group_num);

int
main (int argc, char *argv[])
{
	GtkWidget *window, *hpaned, *shortcut_bar, *vbox, *button;
	EShortcutModel *shortcut_model;
	gchar *pathname;
	gint i;

	gnome_init ("test-shortcut-bar", "0.1", argc, argv);

	gtk_widget_push_visual (gdk_rgb_get_visual ());
	gtk_widget_push_colormap (gdk_rgb_get_cmap ());

	window = gnome_app_new ("TestShortcutBar", "TestShortCutBar");
	gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);
	gtk_window_set_policy (GTK_WINDOW (window), FALSE, TRUE, FALSE);

	gtk_signal_connect (GTK_OBJECT (window), "delete-event",
			    GTK_SIGNAL_FUNC (quit), NULL);

	hpaned = e_hpaned_new ();
	gnome_app_set_contents (GNOME_APP (window), hpaned);
	gtk_widget_show (hpaned);

	shortcut_model = e_shortcut_model_new ();

	shortcut_bar = e_shortcut_bar_new ();
	e_shortcut_bar_set_model (E_SHORTCUT_BAR (shortcut_bar),
				  shortcut_model);
	e_paned_pack1 (E_PANED (hpaned), shortcut_bar, FALSE, TRUE);
	gtk_widget_show (shortcut_bar);
	e_shortcut_bar_set_icon_callback (E_SHORTCUT_BAR (shortcut_bar),
					  icon_callback,
					  NULL);

	gtk_signal_connect (GTK_OBJECT (shortcut_model), "item_added",
			    GTK_SIGNAL_FUNC (on_item_added), NULL);
	gtk_signal_connect (GTK_OBJECT (shortcut_model), "item_removed",
			    GTK_SIGNAL_FUNC (on_item_removed), NULL);
	gtk_signal_connect (GTK_OBJECT (shortcut_model), "group_added",
			    GTK_SIGNAL_FUNC (on_group_added), NULL);
	gtk_signal_connect (GTK_OBJECT (shortcut_model), "group_removed",
			    GTK_SIGNAL_FUNC (on_group_removed), NULL);

#if 0
	gtk_container_set_border_width (GTK_CONTAINER (shortcut_bar), 4);
#endif

	e_paned_set_position (E_PANED (hpaned), 100);
	/*e_paned_set_gutter_size (E_PANED (hpaned), 12);*/

	vbox = gtk_vbox_new (FALSE, 0);
	e_paned_pack2 (E_PANED (hpaned), vbox, TRUE, TRUE);
	gtk_widget_show (vbox);


	main_label = gtk_label_new ("Main Application Window Goes Here");
	gtk_box_pack_start (GTK_BOX (vbox), main_label, TRUE, TRUE, 0);
	gtk_widget_show (main_label);
	gtk_signal_connect (GTK_OBJECT (main_label), "size_allocate",
			    GTK_SIGNAL_FUNC (on_main_label_size_allocate),
			    NULL);

	button = gtk_button_new_with_label ("Set current group to 3rd");
	gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
	gtk_widget_show (button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_set_group_button_clicked),
			    shortcut_bar);

	button = gtk_button_new_with_label ("Set current group to 5th (no animation)");
	gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
	gtk_widget_show (button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_set_group_button_no_animation_clicked),
			    shortcut_bar);


	gtk_widget_pop_visual ();
	gtk_widget_pop_colormap ();

	/* Load our default icons. */
	for (i = 0; i < NUM_SHORTCUT_TYPES; i++) {
		pathname = gnome_pixmap_file (icon_filenames[i]);
		if (pathname)
			icon_pixbufs[i] = gdk_pixbuf_new_from_file (pathname);
		else
			icon_pixbufs[i] = NULL;
	}

	add_test_groups (E_SHORTCUT_BAR (shortcut_bar));

	gtk_signal_connect (GTK_OBJECT (shortcut_bar), "item_selected",
			    GTK_SIGNAL_FUNC (on_shortcut_bar_item_selected),
			    NULL);

	gtk_widget_show (window);
	gtk_main ();
	return 0;
}


static GdkPixbuf*
icon_callback (EShortcutBar *shortcut_bar,
	       const gchar *url,
	       gpointer data)
{
	gint i;

	for (i = 0; i < NUM_SHORTCUT_TYPES; i++) {
		if (!strncmp (url, shortcut_types[i],
			      strlen (shortcut_types[i]))) {
			gdk_pixbuf_ref (icon_pixbufs[i]);
			return icon_pixbufs[i];
		}
	}

	return NULL;
}

static void
on_main_label_size_allocate (GtkWidget *widget,
			     GtkAllocation *allocation,
			     gpointer data)
{
	g_print ("In on_main_label_size_allocate\n");
}

static void
quit (GtkWidget *window, GdkEvent *event, gpointer data)
{
	gtk_widget_destroy (window);
	gtk_exit (0);
}


static void
add_test_groups (EShortcutBar *shortcut_bar)
{
	add_test_group (shortcut_bar, 1, "Shortcuts");
	add_test_group (shortcut_bar, 2, "My Shortcuts");
	add_test_group (shortcut_bar, 3, "Longer Shortcuts");
	add_test_group (shortcut_bar, 4, "Very Long Shortcuts");
	add_test_group (shortcut_bar, 5, "Incredibly Long Shortcuts");
}


static void
add_test_group (EShortcutBar *shortcut_bar, gint i, gchar *group_name)
{
	gint group_num, item_num, num_items;
	gchar buffer[128];
	gint shortcut_type, j;

	group_num = e_shortcut_model_add_group (shortcut_bar->model, -1,
						group_name);

	if (group_num % 2)
		e_shortcut_bar_set_view_type (shortcut_bar, group_num,
					      E_ICON_BAR_SMALL_ICONS);

	num_items = get_random_int (5) + 3;
	for (j = 1; j <= num_items; j++) {
		if (j == 1)
			sprintf (buffer, "A very long shortcut with proper words so I can test the wrapping and ellipsis behaviour");
		else if (j == 2)
			sprintf (buffer, "A very long shortcut with averylongworkinthemiddlesoIcantestthewrappingandellipsisbehaviour");
		else
			sprintf (buffer, "Item %i:%i\n", i, j);

		shortcut_type = get_random_int (NUM_SHORTCUT_TYPES);
		item_num = e_shortcut_model_add_item (shortcut_bar->model,
						      group_num, -1,
						      shortcut_types[shortcut_type],
						      buffer);
	}
}


/* Returns a random integer between 0 and max - 1. */
static gint
get_random_int (gint max)
{
	gint random_num;

	random_num = (int) (max * (rand () / (RAND_MAX + 1.0)));
#if 0
	g_print ("Random num (%i): %i\n", max, random_num);
#endif
	return random_num;
}


static void
on_shortcut_bar_item_selected (EShortcutBar *shortcut_bar,
			       GdkEvent *event, gint group_num, gint item_num)
{
	gchar buffer[256];

	if (event->button.button == 1) {
		sprintf (buffer, "Item Selected - %i:%i",
			 group_num + 1, item_num + 1);
		gtk_label_set_text (GTK_LABEL (main_label), buffer);
	} else if (event->button.button == 3) {
		if (item_num == -1)
			show_standard_popup (shortcut_bar, event, group_num);
		else
			show_context_popup (shortcut_bar, event, group_num,
					    item_num);
	}
}


static void
show_standard_popup (EShortcutBar *shortcut_bar,
		     GdkEvent *event,
		     gint group_num)
{
	GtkWidget *menu, *menuitem;

	/* We don't have any commands if there aren't any groups yet. */
	if (group_num == -1)
		return;

	menu = gtk_menu_new ();

	menuitem = gtk_menu_item_new_with_label ("Large Icons");
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);
	gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			    GTK_SIGNAL_FUNC (set_large_icons), shortcut_bar);

	menuitem = gtk_menu_item_new_with_label ("Small Icons");
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);
	gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			    GTK_SIGNAL_FUNC (set_small_icons), shortcut_bar);

	menuitem = gtk_menu_item_new ();
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new_with_label ("Add New Group");
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new_with_label ("Remove Group");
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);
	gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			    GTK_SIGNAL_FUNC (remove_group), shortcut_bar);

	menuitem = gtk_menu_item_new_with_label ("Rename Group");
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new ();
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new_with_label ("Add Shortcut...");
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new ();
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new_with_label ("Hide Shortcut Bar");
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	/* Save the group num so we can get it in the callbacks. */
	gtk_object_set_data (GTK_OBJECT (menu), "group_num",
			     GINT_TO_POINTER (group_num));

	/* FIXME: Destroy menu when finished with it somehow? */
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
			event->button.button, event->button.time);
}


static void
set_large_icons (GtkWidget *menuitem,
		 EShortcutBar *shortcut_bar)
{
	GtkWidget *menu;
	gint group_num;

	menu = menuitem->parent;
	g_return_if_fail (GTK_IS_MENU (menu));

	group_num = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menu),
							  "group_num"));

	e_shortcut_bar_set_view_type (shortcut_bar, group_num,
				      E_ICON_BAR_LARGE_ICONS);
}


static void
set_small_icons (GtkWidget *menuitem,
		 EShortcutBar *shortcut_bar)
{
	GtkWidget *menu;
	gint group_num;

	menu = menuitem->parent;
	g_return_if_fail (GTK_IS_MENU (menu));

	group_num = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menu),
							  "group_num"));

	e_shortcut_bar_set_view_type (shortcut_bar, group_num,
				      E_ICON_BAR_SMALL_ICONS);
}


static void
remove_group (GtkWidget *menuitem,
	      EShortcutBar *shortcut_bar)
{
	GtkWidget *menu;
	gint group_num;

	menu = menuitem->parent;
	g_return_if_fail (GTK_IS_MENU (menu));

	group_num = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menu),
							  "group_num"));
	
	e_shortcut_model_remove_group (shortcut_bar->model, group_num);
}


static void
show_context_popup (EShortcutBar *shortcut_bar,
		    GdkEvent *event,
		    gint group_num,
		    gint item_num)
{
	GtkWidget *menu, *menuitem, *label, *pixmap;

	menu = gtk_menu_new ();

	menuitem = gtk_pixmap_menu_item_new ();
	label = gtk_label_new ("Open Folder");
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_container_add (GTK_CONTAINER (menuitem), label);
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);
	pixmap = gnome_stock_pixmap_widget (menu, GNOME_STOCK_MENU_OPEN);
	if (pixmap) {
		gtk_widget_show(pixmap);
		gtk_pixmap_menu_item_set_pixmap (GTK_PIXMAP_MENU_ITEM (menuitem), pixmap);
	}

	menuitem = gtk_menu_item_new_with_label ("Open in New Window");
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new_with_label ("Advanced Find");
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new ();
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new_with_label ("Remove from Shortcut Bar");
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);
	gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			    GTK_SIGNAL_FUNC (remove_item), shortcut_bar);

	menuitem = gtk_menu_item_new_with_label ("Rename Shortcut");
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);
	gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			    GTK_SIGNAL_FUNC (rename_item), shortcut_bar);

	menuitem = gtk_menu_item_new ();
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);

	menuitem = gtk_menu_item_new_with_label ("Properties");
	gtk_widget_set_sensitive (menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_menu_append (GTK_MENU (menu), menuitem);


	/* Save the group & item nums so we can get them in the callbacks. */
	gtk_object_set_data (GTK_OBJECT (menu), "group_num",
			     GINT_TO_POINTER (group_num));
	gtk_object_set_data (GTK_OBJECT (menu), "item_num",
			     GINT_TO_POINTER (item_num));

	/* FIXME: Destroy menu when finished with it somehow? */
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
			event->button.button, event->button.time);
}


static void
rename_item (GtkWidget *menuitem,
	     EShortcutBar *shortcut_bar)
{
	GtkWidget *menu;
	gint group_num, item_num;

	menu = menuitem->parent;
	g_return_if_fail (GTK_IS_MENU (menu));

	group_num = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menu),
							  "group_num"));
	item_num = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menu),
							 "item_num"));
	
	e_shortcut_bar_start_editing_item (shortcut_bar, group_num, item_num);
}


static void
remove_item (GtkWidget *menuitem,
	     EShortcutBar *shortcut_bar)
{
	GtkWidget *menu;
	gint group_num, item_num;

	menu = menuitem->parent;
	g_return_if_fail (GTK_IS_MENU (menu));

	group_num = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menu),
							  "group_num"));
	item_num = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (menu),
							 "item_num"));
	
	e_shortcut_model_remove_item (shortcut_bar->model, group_num,
				      item_num);
}


static void
on_set_group_button_clicked (GtkWidget *button,
			     EShortcutBar *shortcut_bar)
{
	g_print ("In on_set_group_button_clicked\n");

	e_group_bar_set_current_group_num (E_GROUP_BAR (shortcut_bar),
					   2, TRUE);
}


static void
on_set_group_button_no_animation_clicked (GtkWidget *button,
					  EShortcutBar *shortcut_bar)
{
	g_print ("In on_set_group_button_no_animation_clicked\n");

	e_group_bar_set_current_group_num (E_GROUP_BAR (shortcut_bar),
					   4, FALSE);
}


static void
on_item_added (EShortcutModel *shortcut_model,
	       gint group_num,
	       gint item_num)
{
	g_print ("In on_item_added Group:%i Item:%i\n", group_num, item_num);
}


static void
on_item_removed (EShortcutModel *shortcut_model,
		 gint group_num,
		 gint item_num)
{
	g_print ("In on_item_removed Group:%i Item:%i\n", group_num, item_num);
}


static void
on_group_added (EShortcutModel *shortcut_model,
		gint group_num)
{
	g_print ("In on_group_added Group:%i\n", group_num);
}


static void
on_group_removed (EShortcutModel *shortcut_model,
		  gint group_num)
{
	g_print ("In on_group_removed Group:%i\n", group_num);
}
