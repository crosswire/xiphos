/*
 * GnomeSword Bible Study Tool
 * navbar_book_dialog.c - navigation bar for genbook dialog modules
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>

#include "gui/navbar_book_dialog.h"


#include "main/module_dialogs.h"
#include "main/navbar_book.h"
#include "main/navbar_book_dialog.h"
#include "main/settings.h"



/******************************************************************************
 * Name
 *   menu_deactivate_callback
 *
 * Synopsis
 *   #include "gui/navbar_book_dialog.h"
 *
 *   void menu_deactivate_callback (GtkWidget *widget, gpointer user_data)
 *
 * Description
 *   return toogle button to normal
 *
 * Return value
 *   void
 */

static void menu_deactivate_callback(GtkWidget * widget, gpointer user_data)
{
	GtkWidget *menu_button;

	menu_button = GTK_WIDGET(user_data);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(menu_button), FALSE);
}


/******************************************************************************
 * Name
 *   menu_position_under
 *
 * Synopsis
 *   #include "gui/navbar_book_dialog.h"
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

static 
void menu_position_under(GtkMenu * menu, int * x, int * y,
				gboolean * push_in, gpointer user_data)
{
	GtkWidget *widget;

	g_return_if_fail(GTK_IS_BUTTON(user_data));
	g_return_if_fail(GTK_WIDGET_NO_WINDOW(user_data));

	widget = GTK_WIDGET(user_data);

	gdk_window_get_origin(widget->window, x, y);

	*x += widget->allocation.x;
	*y += widget->allocation.y + widget->allocation.height;

	*push_in = FALSE;
}

/******************************************************************************
 * Name
 *   select_button_press_callback
 *
 * Synopsis
 *   #include "gui/navbar_book_dialog.h"
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

static gboolean select_button_press_callback(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer user_data)
{
	GtkWidget *menu;

	if(!settings.havebook)
		return FALSE;	
	menu = main_navbar_book_dialog_drop_down_new(user_data);	
	g_signal_connect(menu, "deactivate",
			 G_CALLBACK(menu_deactivate_callback), widget);
	if ((event->type == GDK_BUTTON_PRESS) && event->button == 1) {
		gtk_widget_grab_focus(widget);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
			       menu_position_under, widget, event->button,
			       event->time);
		return TRUE;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_button_parent_clicked
 *
 * Synopsis
 *   #include "gui/navbar_book_dialog.h"
 *
 *  void on_button_parent_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   call main_navbar_book_dialog_parent() in main/navbar_book.cc
 *
 * Return value
 *   void
 */

static
void on_button_parent_clicked(GtkButton * button, gpointer user_data)
{
	if(!settings.havebook)
		return;	
	main_navbar_book_dialog_parent(user_data);
}


/******************************************************************************
 * Name
 *  on_button_child_clicked
 *
 * Synopsis
 *   #include "gui/navbar_book_dialog.h"
 *
 *  void on_button_child_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   call main_navbar_book_dialog_first_child() in main/navbar_book.cc
 *
 * Return value
 *   void
 */

static
void on_button_child_clicked(GtkButton * button, gpointer user_data)
{
	if(!settings.havebook)
		return;	
	main_navbar_book_dialog_first_child(user_data);
}


/******************************************************************************
 * Name
 *  on_button_prev_clicked
 *
 * Synopsis
 *   #include "gui/navbar_book_dialog.h"
 *
 *  void on_button_prev_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   call main_navbar_book_dialog_prev() in main/navbar_book.cc
 *
 * Return value
 *   void
 */

static
void on_button_prev_clicked(GtkButton * button, gpointer user_data)
{
	if(!settings.havebook)
		return;	
	main_navbar_book_dialog_prev(user_data);
}


/******************************************************************************
 * Name
 *  on_button_next_clicked
 *
 * Synopsis
 *   #include "gui/navbar_book_dialog.h"
 *
 *  void on_button_next_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   call main_navbar_book_dialog_next(); in main/navbar_book.cc
 *
 * Return value
 *   void
 */

static
void on_button_next_clicked(GtkButton * button, gpointer user_data)
{
	if(!settings.havebook)
		return;	
	main_navbar_book_dialog_next(user_data);
}


/******************************************************************************
 * Name
 *  gui_navbar_book_dialog_new
 *
 * Synopsis
 *   #include "gui/navbar_book_dialog.h"
 *
 *  GtkWidget *gui_navbar_book_dialog_new(void)
 *
 * Description
 *   create a new gen book navigation toolbar and return it
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_navbar_book_dialog_new(DIALOG_DATA * d)
{
	GtkWidget *vbox1;
	GtkWidget *hbox1;
	GtkWidget *image1;
	GtkWidget *arrow1;
	GtkWidget *image2;

	d->navbar_book.tooltips = gtk_tooltips_new();
	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox1);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 0);

	d->navbar_book.lookup_entry = gtk_entry_new();
	gtk_widget_show(d->navbar_book.lookup_entry);
	gtk_box_pack_start(GTK_BOX(hbox1), d->navbar_book.lookup_entry, TRUE,
			   TRUE, 0);
	gtk_editable_set_editable(GTK_EDITABLE(d->navbar_book.lookup_entry),
				  FALSE);
	gtk_entry_set_invisible_char(GTK_ENTRY(d->navbar_book.lookup_entry),
				     9679);

	d->navbar_book.button_list = gtk_toggle_button_new();
	gtk_widget_show(d->navbar_book.button_list);
	gtk_box_pack_start(GTK_BOX(hbox1), d->navbar_book.button_list, FALSE,
			   FALSE, 0);
	//gtk_button_set_relief (GTK_BUTTON (d->navbar_book.button_list), GTK_RELIEF_NONE);

	arrow1 = gtk_arrow_new(GTK_ARROW_DOWN, GTK_SHADOW_OUT);
	gtk_widget_show(arrow1);
	gtk_container_add(GTK_CONTAINER(d->navbar_book.button_list), arrow1);

	d->navbar_book.button_left = gtk_button_new();
	gtk_widget_show(d->navbar_book.button_left);
	gtk_box_pack_start(GTK_BOX(hbox1), d->navbar_book.button_left, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(d->navbar_book.tooltips, d->navbar_book.button_left,
			     _("Go to parent"), NULL);

	gtk_button_set_relief(GTK_BUTTON(d->navbar_book.button_left),
			      GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(GTK_BUTTON(d->navbar_book.button_left),
				      FALSE);

	image1 = gtk_image_new_from_icon_name("stock_left",
					      GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image1);
	gtk_container_add(GTK_CONTAINER(d->navbar_book.button_left), image1);

	d->navbar_book.button_up = gtk_button_new();
	gtk_widget_show(d->navbar_book.button_up);
	gtk_box_pack_start(GTK_BOX(hbox1), d->navbar_book.button_up, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(d->navbar_book.tooltips, d->navbar_book.button_up,
			     _("Go to previous item"), NULL);
	gtk_button_set_relief(GTK_BUTTON(d->navbar_book.button_up),
			      GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(GTK_BUTTON(d->navbar_book.button_up),
				      FALSE);

	image1 = gtk_image_new_from_icon_name("stock_up",
					      GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image1);
	gtk_container_add(GTK_CONTAINER(d->navbar_book.button_up), image1);

	d->navbar_book.button_down = gtk_button_new();
	gtk_widget_show(d->navbar_book.button_down);
	gtk_box_pack_start(GTK_BOX(hbox1), d->navbar_book.button_down, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(d->navbar_book.tooltips, d->navbar_book.button_down,
			     _("Go to next item"), NULL);
	gtk_button_set_relief(GTK_BUTTON(d->navbar_book.button_down),
			      GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(GTK_BUTTON(d->navbar_book.button_down),
				      FALSE);

	image2 = gtk_image_new_from_icon_name("stock_down",
					      GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image2);
	gtk_container_add(GTK_CONTAINER(d->navbar_book.button_down), image2);

	d->navbar_book.button_right = gtk_button_new();
	gtk_widget_show(d->navbar_book.button_right);
	gtk_box_pack_start(GTK_BOX(hbox1), d->navbar_book.button_right, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(d->navbar_book.tooltips,
			     d->navbar_book.button_right,
			     _("Go to first child"), NULL);

	gtk_button_set_relief(GTK_BUTTON(d->navbar_book.button_right),
			      GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(GTK_BUTTON(d->navbar_book.button_right),
				      FALSE);

	image1 = gtk_image_new_from_icon_name("stock_right",
					      GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image1);
	gtk_container_add(GTK_CONTAINER(d->navbar_book.button_right), image1);

	g_signal_connect((gpointer) d->navbar_book.button_up, "clicked",
			 G_CALLBACK(on_button_prev_clicked), d);
	g_signal_connect((gpointer) d->navbar_book.button_down, "clicked",
			 G_CALLBACK(on_button_next_clicked), d);
	g_signal_connect((gpointer) d->navbar_book.button_left, "clicked",
			 G_CALLBACK(on_button_parent_clicked), d);
	g_signal_connect((gpointer) d->navbar_book.button_right, "clicked",
			 G_CALLBACK(on_button_child_clicked), d);
	g_signal_connect((gpointer) d->navbar_book.button_list,
			 "button_press_event",
			 G_CALLBACK(select_button_press_callback), d);
	return vbox1;
}
