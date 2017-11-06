/*
 * Xiphos Bible Study Tool
 * navbar_book.c - navigation bar for genbook modules
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include "gui/navbar_book.h"
#include "gui/utilities.h"

#include "main/navbar_book.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/url.hh"

NAVBAR_BOOK navbar_book;

/******************************************************************************
 * Name
 *   menu_deactivate_callback
 *
 * Synopsis
 *   #include "gui/navbar_book.h"
 *
 *   void menu_deactivate_callback (GtkWidget *widget, gpointer user_data)
 *
 * Description
 *   return toogle button to normal
 *
 * Return value
 *   void
 */

static void menu_deactivate_callback(GtkWidget *widget,
				     gpointer user_data)
{
	GtkWidget *menu_button;

	menu_button = GTK_WIDGET(user_data);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(menu_button),
				     FALSE);
}

/******************************************************************************
 * Name
 *   menu_position_under
 *
 * Synopsis
 *   #include "gui/navbar_book.h"
 *
 *   void menu_position_under(GtkMenu * menu, int * x, int * y,
 *				gboolean * push_in, gpointer user_data)
 *
 * Description
 *   position drop down menu under toogle button
 *
 *
 * Return value
 *   void
 */

static void menu_position_under(GtkMenu *menu, int *x, int *y,
				gboolean *push_in, gpointer user_data)
{
	GtkWidget *widget;
	GtkAllocation allocation;
	g_return_if_fail(GTK_IS_BUTTON(user_data));
#if GTK_CHECK_VERSION(2, 20, 0)
	g_return_if_fail(gtk_widget_get_window(user_data));
#else
	g_return_if_fail(GTK_WIDGET_NO_WINDOW(user_data));
#endif

	widget = GTK_WIDGET(user_data);

	gdk_window_get_origin(gtk_widget_get_window(widget), x, y);
	gtk_widget_get_allocation(widget, &allocation);
	*x += allocation.x;
	*y += allocation.y + allocation.height;

	*push_in = FALSE;
}

static gboolean lookup_entry_press_callback(GtkWidget *widget,
					    GdkEventKey *event,
					    gpointer user_data)
{
	if (!settings.havebook)
		return FALSE;
	/* this does not work on my system - hope I'm not messing things up
	   for others - 104 is the enter key on the number pad
	   switch (event->hardware_keycode) {
	   case 98:
	   gtk_button_clicked(GTK_BUTTON(navbar_book.button_up));
	   return TRUE;
	   break;
	   case 104:
	   gtk_button_clicked(GTK_BUTTON(navbar_book.button_down));
	   return TRUE;
	   break;
	   case 100:
	   gtk_button_clicked(GTK_BUTTON(navbar_book.button_left));
	   return TRUE;
	   break;
	   case 102:
	   gtk_button_clicked(GTK_BUTTON(navbar_book.button_right));
	   return TRUE;
	   break;
	   }
	 */
	XI_message(("lookup_entry_press_callback\nkeycode: %d",
		    event->hardware_keycode));
	return FALSE;
}

/******************************************************************************
 * Name
 *   select_button_press_callback
 *
 * Synopsis
 *   #include "gui/navbar_book.h"
 *
 *   gboolean select_button_press_callback (GtkWidget *widget,
 *			      GdkEventButton *event,
 *			      gpointer user_data)
 *
 * Description
 *    make the tooglebutton act like a gtk optionmenu by dropping a popup
 *    under the button
 *
 * Return value
 *   gboolean
 */

static gboolean select_button_press_callback(GtkWidget *widget,
					     GdkEventButton *event,
					     gpointer user_data)
{
	GtkWidget *menu;

	if (!settings.havebook)
		return FALSE;
	menu = main_book_drop_down_new();
	g_signal_connect(menu, "deactivate",
			 G_CALLBACK(menu_deactivate_callback), widget);
	if ((event->type == GDK_BUTTON_PRESS) && event->button == 1) {
		gtk_widget_grab_focus(widget);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),
					     TRUE);
#if GTK_CHECK_VERSION(3, 22, 0)
		gtk_menu_popup_at_widget(GTK_MENU(menu), widget, 0, 0, NULL);
#else
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
			       menu_position_under, widget, event->button,
			       event->time);
#endif
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *  on_button_parent_clicked
 *
 * Synopsis
 *   #include "gui/navbar_book.h"
 *
 *  void on_button_parent_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   call main_navbar_book_parent() in main/navbar_book.cc
 *
 * Return value
 *   void
 */

static void on_button_parent_clicked(GtkButton *button, gpointer user_data)
{
	if (!settings.havebook)
		return;
	main_navbar_book_parent();
}

/******************************************************************************
 * Name
 *  on_button_child_clicked
 *
 * Synopsis
 *   #include "gui/navbar_book.h"
 *
 *  void on_button_child_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   call main_navbar_book_first_child() in main/navbar_book.cc
 *
 * Return value
 *   void
 */

static void on_button_child_clicked(GtkButton *button, gpointer user_data)
{
	if (!settings.havebook)
		return;
	main_navbar_book_first_child();
}

/******************************************************************************
 * Name
 *  on_button_prev_clicked
 *
 * Synopsis
 *   #include "gui/navbar_book.h"
 *
 *  void on_button_prev_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   call main_navbar_book_prev() in main/navbar_book.cc
 *
 * Return value
 *   void
 */

static void on_button_prev_clicked(GtkButton *button, gpointer user_data)
{
	if (!settings.havebook)
		return;
	main_navbar_book_prev();
}

/******************************************************************************
 * Name
 *  on_button_next_clicked
 *
 * Synopsis
 *   #include "gui/navbar_book.h"
 *
 *  void on_button_next_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   call main_navbar_book_next(); in main/navbar_book.cc
 *
 * Return value
 *   void
 */

static void on_button_next_clicked(GtkButton *button, gpointer user_data)
{
	if (!settings.havebook)
		return;
	main_navbar_book_next();
}

/******************************************************************************
 * Name
 *    on_entry_activate
 *
 * Synopsis
 *   #include "gui/navbar_book.h"
 *
 *  void on_entry_activate (GtkEntry *entry, gpointer  user_data)
 *
 * Description
 *    get entry text and call main_navbar_book_entry_activate();
 *    in main/navbar_book.cc
 *
 * Return value
 *   void
 */

void on_entry_activate(GtkEntry *entry, gpointer user_data)
{
	const gchar *entry_buf = NULL;

	entry_buf = gtk_entry_get_text(entry);
	main_navbar_book_entry_activate(entry_buf);
}

/******************************************************************************
 * Name
 *  gui_navbar_book_new
 *
 * Synopsis
 *   #include "gui/navbar_book.h"
 *
 *  GtkWidget *gui_navbar_book_new(void)
 *
 * Description
 *   create a new gen book navigation toolbar and return it
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_navbar_book_new(void)
{
	GtkWidget *vbox1;
	GtkWidget *hbox1;
	GtkWidget *image1;
	GtkWidget *arrow1;
	GtkWidget *image2;

	UI_VBOX(vbox1, FALSE, 0);
	gtk_widget_show(vbox1);

	UI_HBOX(hbox1, FALSE, 0);
	gtk_widget_show(hbox1);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 0);

	navbar_book.lookup_entry = gtk_entry_new();
	gtk_widget_show(navbar_book.lookup_entry);
	gtk_box_pack_start(GTK_BOX(hbox1), navbar_book.lookup_entry, TRUE,
			   TRUE, 0);
	gtk_editable_set_editable(GTK_EDITABLE(navbar_book.lookup_entry),
				  TRUE);
	gtk_entry_set_invisible_char(GTK_ENTRY(navbar_book.lookup_entry),
				     9679);

	navbar_book.button_list = gtk_toggle_button_new();
	gtk_widget_show(navbar_book.button_list);
	gtk_box_pack_start(GTK_BOX(hbox1), navbar_book.button_list, FALSE,
			   FALSE, 0);

#if GTK_CHECK_VERSION(3, 14, 0)
	arrow1 =
	    gtk_image_new_from_icon_name("open-menu-symbolic",
					 GTK_ICON_SIZE_BUTTON);
#else
	arrow1 = gtk_arrow_new(GTK_ARROW_DOWN, GTK_SHADOW_OUT);
#endif
	gtk_widget_show(arrow1);
	gtk_container_add(GTK_CONTAINER(navbar_book.button_list), arrow1);

	navbar_book.button_left = gtk_button_new();
	gtk_widget_show(navbar_book.button_left);
	gtk_box_pack_start(GTK_BOX(hbox1), navbar_book.button_left, FALSE,
			   FALSE, 0);
	gtk_widget_set_tooltip_text(navbar_book.button_left,
				    _("Go outward, to the section containing this one"));

	gtk_button_set_relief(GTK_BUTTON(navbar_book.button_left),
			      GTK_RELIEF_NONE);
#if GTK_CHECK_VERSION(3, 20, 0)
	gtk_widget_set_focus_on_click(GTK_WIDGET(navbar_book.button_left), FALSE);
#else
	gtk_button_set_focus_on_click(GTK_BUTTON(navbar_book.button_left), FALSE);
#endif

	image1 =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_image_new_from_icon_name("go-previous-symbolic", GTK_ICON_SIZE_BUTTON);
#else
	    gtk_image_new_from_stock(GTK_STOCK_GO_BACK, GTK_ICON_SIZE_BUTTON);
#endif
	gtk_widget_show(image1);
	gtk_container_add(GTK_CONTAINER(navbar_book.button_left), image1);

	navbar_book.button_up = gtk_button_new();
	gtk_widget_show(navbar_book.button_up);
	gtk_box_pack_start(GTK_BOX(hbox1), navbar_book.button_up, FALSE,
			   FALSE, 0);
	gtk_widget_set_tooltip_text(navbar_book.button_up,
				    _("Go to previous item"));
	gtk_button_set_relief(GTK_BUTTON(navbar_book.button_up),
			      GTK_RELIEF_NONE);
#if GTK_CHECK_VERSION(3, 20, 0)
	gtk_widget_set_focus_on_click(GTK_WIDGET(navbar_book.button_up), FALSE);
#else
	gtk_button_set_focus_on_click(GTK_BUTTON(navbar_book.button_up), FALSE);
#endif

	image1 =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_image_new_from_icon_name("go-up-symbolic", GTK_ICON_SIZE_BUTTON);
#else
	    gtk_image_new_from_stock(GTK_STOCK_GO_UP, GTK_ICON_SIZE_BUTTON);
#endif
	gtk_widget_show(image1);
	gtk_container_add(GTK_CONTAINER(navbar_book.button_up), image1);

	navbar_book.button_down = gtk_button_new();
	gtk_widget_show(navbar_book.button_down);
	gtk_box_pack_start(GTK_BOX(hbox1), navbar_book.button_down, FALSE,
			   FALSE, 0);
	gtk_widget_set_tooltip_text(navbar_book.button_down,
				    _("Go to next item"));
	gtk_button_set_relief(GTK_BUTTON(navbar_book.button_down),
			      GTK_RELIEF_NONE);
#if GTK_CHECK_VERSION(3, 20, 0)
	gtk_widget_set_focus_on_click(GTK_WIDGET(navbar_book.button_down), FALSE);
#else
	gtk_button_set_focus_on_click(GTK_BUTTON(navbar_book.button_down), FALSE);
#endif

	image2 =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_image_new_from_icon_name("go-down-symbolic", GTK_ICON_SIZE_BUTTON);
#else
	    gtk_image_new_from_stock(GTK_STOCK_GO_DOWN, GTK_ICON_SIZE_BUTTON);
#endif

	gtk_widget_show(image2);
	gtk_container_add(GTK_CONTAINER(navbar_book.button_down), image2);

	navbar_book.button_right = gtk_button_new();
	gtk_widget_show(navbar_book.button_right);
	gtk_box_pack_start(GTK_BOX(hbox1), navbar_book.button_right, FALSE,
			   FALSE, 0);
	gtk_widget_set_tooltip_text(navbar_book.button_right,
				    _("Go inward, to the first subsection"));

	gtk_button_set_relief(GTK_BUTTON(navbar_book.button_right),
			      GTK_RELIEF_NONE);
#if GTK_CHECK_VERSION(3, 20, 0)
	gtk_widget_set_focus_on_click(GTK_WIDGET(navbar_book.button_right), FALSE);
#else
	gtk_button_set_focus_on_click(GTK_BUTTON(navbar_book.button_right), FALSE);
#endif

	image1 =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_image_new_from_icon_name("go-next-symbolic", GTK_ICON_SIZE_BUTTON);
#else
	    gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_BUTTON);
#endif
	gtk_widget_show(image1);
	gtk_container_add(GTK_CONTAINER(navbar_book.button_right), image1);

	g_signal_connect((gpointer)navbar_book.lookup_entry,
			 "activate", G_CALLBACK(on_entry_activate), NULL);
	g_signal_connect((gpointer)navbar_book.button_up, "clicked",
			 G_CALLBACK(on_button_prev_clicked), NULL);
	g_signal_connect((gpointer)navbar_book.button_down, "clicked",
			 G_CALLBACK(on_button_next_clicked), NULL);
	g_signal_connect((gpointer)navbar_book.button_left, "clicked",
			 G_CALLBACK(on_button_parent_clicked), NULL);
	g_signal_connect((gpointer)navbar_book.button_right, "clicked",
			 G_CALLBACK(on_button_child_clicked), NULL);
	g_signal_connect((gpointer)navbar_book.button_list,
			 "button_press_event",
			 G_CALLBACK(select_button_press_callback), NULL);
	g_signal_connect((gpointer)navbar_book.lookup_entry,
			 "key_press_event",
			 G_CALLBACK(lookup_entry_press_callback), NULL);
	return vbox1;
}
