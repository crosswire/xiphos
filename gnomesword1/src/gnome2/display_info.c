/*
 * GnomeSword Bible Study Tool
 * _display_info.c - display information (ie strongs) in a dialog
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

//#include "gui/gtkhtml_display.h"
#include "gui/display_info.h"

#include "main/sword.h"
#include "main/settings.h"

gboolean gsI_isrunning = FALSE;
GtkWidget *dialog_display_info;


static GtkWidget *html_widget;


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "display_info.h"
 *
 *   	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_display_mod_and_key(gchar * mod_name, gchar * key)
{
	gchar *text = NULL;

	text = main_get_rendered_text(mod_name, key);
	if (text) {
		main_entry_display(html_widget, mod_name, text, key, TRUE);
		free(text);
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "display_info.h"
 *
 *   	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_display_text_information(gchar * information)
{
	gboolean was_editable = gtk_html_get_editable(GTK_HTML(html_widget));
	if (was_editable)
		gtk_html_set_editable(GTK_HTML(html_widget), FALSE);
	
	gtk_html_load_from_string(GTK_HTML(html_widget),information,strlen(information));
	gtk_html_set_editable(GTK_HTML(html_widget), was_editable);
}

/******************************************************************************
 * Name
 *   on_dlgInformation_destroy
 *
 * Synopsis
 *   #include "_display_info.h"
 *
 *   void on_dlgInformation_destroy(GtkObject * object, 
 *						gpointer user_data)	
 *
 * Description
 *    shut it down
 *
 * Return value
 *   void
 */

static void on_dlgInformation_destroy(GtkObject * object,
				      gpointer user_data)
{
	gsI_isrunning = FALSE;
}

/******************************************************************************
 * Name
 *   on_btnInfoOK_clicked
 *
 * Synopsis
 *   #include "_display_info.h"
 *
 *   void on_btnInfoOK_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void button_close_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

/******************************************************************************
 * Name
 *   gui_create_display_informtion_dialog
 *
 * Synopsis
 *   #include "_display_info.h"
 *
 *   GtkWidget *gui_create_display_informtion_dialog(void)	
 *
 * Description
 *    create information dialog 
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_display_informtion_dialog(void)
{
	
	GtkWidget *dialog_vbox23;
	GtkWidget *hbox;
	GtkWidget *frame71;
	GtkWidget *scrolledwindow70;
	GtkWidget *dialog_action_area23;
	GtkWidget *hbuttonbox2;
	GtkWidget *button_close;
	GtkWidget *image;

	dialog_display_info = gtk_dialog_new();
	gtk_container_set_border_width(GTK_CONTAINER(dialog_display_info),
				       6);
	gtk_object_set_data(GTK_OBJECT(dialog_display_info),
			    "dialog_display_info",
			    dialog_display_info);
	gtk_window_set_title(GTK_WINDOW(dialog_display_info),
			     " ");
	GTK_WINDOW(dialog_display_info)->type =
	    GTK_WINDOW_TOPLEVEL;
	gtk_window_set_default_size(GTK_WINDOW
				    (dialog_display_info), 350,
				    200);
	gtk_window_set_policy(GTK_WINDOW(dialog_display_info),
			      TRUE, TRUE, FALSE);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog_display_info), FALSE);

	dialog_vbox23 = GTK_DIALOG(dialog_display_info)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_display_info),
			    "dialog_vbox23", dialog_vbox23);
	gtk_widget_show(dialog_vbox23);


	hbox = gtk_hbox_new(FALSE, 12);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(dialog_vbox23), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 6);
	
	image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO,
					     GTK_ICON_SIZE_DIALOG);
	gtk_widget_show(image);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, TRUE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(image), 0.5, 0);
	
	scrolledwindow70 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow70);
	gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow70, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow70),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	html_widget = gtk_html_new();
	gtk_widget_show(html_widget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow70), html_widget);


	dialog_action_area23 =
	    GTK_DIALOG(dialog_display_info)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog_display_info),
			    "dialog_action_area23",
			    dialog_action_area23);
	gtk_widget_show(dialog_action_area23);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area23), 10);

	hbuttonbox2 = gtk_hbutton_box_new();
	gtk_widget_show(hbuttonbox2);
	gtk_box_pack_start(GTK_BOX(dialog_action_area23), hbuttonbox2,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox2),
				  GTK_BUTTONBOX_END);

	button_close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	
	gtk_widget_show(button_close);
	gtk_container_add(GTK_CONTAINER(hbuttonbox2), button_close);
	GTK_WIDGET_SET_FLAGS(button_close, GTK_CAN_DEFAULT);

	g_signal_connect(GTK_OBJECT(button_close), "clicked",
			   G_CALLBACK(button_close_clicked), NULL);

	g_signal_connect(GTK_OBJECT(dialog_display_info),
			   "destroy",
			   G_CALLBACK(on_dlgInformation_destroy),
			   NULL);


	gtk_widget_show(dialog_display_info);
	gsI_isrunning = TRUE;
	return dialog_display_info;
}
