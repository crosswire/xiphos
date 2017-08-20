/*
 * Xiphos Bible Study Tool
 * display_info.c - display information (ie strongs) in a dialog
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

#include "xiphos_html/xiphos_html.h"

#include "gui/display_info.h"
#include "gui/utilities.h"

#include "main/previewer.h"
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

void gui_display_mod_and_key(const gchar *mod_name, const gchar *key)
{
	gchar *text = NULL;
	XI_message(("mod_name: %s  key: %s", mod_name, key));
	text = main_get_rendered_text(mod_name, key);
	XI_message(("%s", text));
	if (text) {
		main_entry_display(html_widget,
				   (gchar *)mod_name,
				   text, (gchar *)key, TRUE);
		g_free(text);
	}
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
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

void gui_display_text_information(gchar *information)
{
	XIPHOS_HTML_OPEN_STREAM(html_widget, "text/html");
	XIPHOS_HTML_WRITE(html_widget, information, strlen(information));
	XIPHOS_HTML_CLOSE(html_widget);
}
#endif

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

static void on_dlgInformation_destroy(GObject *object, gpointer user_data)
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

static void button_close_clicked(GtkButton *button, gpointer user_data)
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
	GtkWidget *dialog_action_area23;
	GtkWidget *hbuttonbox2;
	GtkWidget *button_close;
	GtkWidget *image;

	dialog_display_info = gtk_dialog_new();
	gtk_container_set_border_width(GTK_CONTAINER(dialog_display_info),
				       6);
	g_object_set_data(G_OBJECT(dialog_display_info),
			  "dialog_display_info", dialog_display_info);
	gtk_window_set_title(GTK_WINDOW(dialog_display_info), " ");
	//GTK_WINDOW(dialog_display_info)->type = GTK_WINDOW_TOPLEVEL;
	gtk_window_set_default_size(GTK_WINDOW(dialog_display_info), 350, 200);
	gtk_window_set_resizable(GTK_WINDOW(dialog_display_info), TRUE);

#ifndef USE_GTK_3
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog_display_info),
				     FALSE);
#endif

	dialog_vbox23 = gtk_dialog_get_content_area(GTK_DIALOG(dialog_display_info)); //GTK_DIALOG(dialog_display_info)->vbox;
	g_object_set_data(G_OBJECT(dialog_display_info),
			  "dialog_vbox23", dialog_vbox23);
	gtk_widget_show(dialog_vbox23);

	UI_HBOX(hbox, FALSE, 12);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(dialog_vbox23), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 6);

	image =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_image_new_from_icon_name("dialog-information",
					 GTK_ICON_SIZE_DIALOG);
#else
	    gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO,
				     GTK_ICON_SIZE_DIALOG);
#endif
	gtk_widget_show(image);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, TRUE, 0);
#if !GTK_CHECK_VERSION(3, 10, 0)
	gtk_misc_set_alignment(GTK_MISC(image), 0.5, 0);
#endif
	html_widget = GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, 30)); //gtk_html_new();
	gtk_widget_show(html_widget);
	gtk_box_pack_start(GTK_BOX(hbox), html_widget, TRUE, TRUE, 0);
	//gtk_container_add(GTK_CONTAINER(scrolledwindow70), html_widget);

	dialog_action_area23 =
#if GTK_CHECK_VERSION(3, 12, 0)
	    gtk_dialog_get_content_area(GTK_DIALOG(dialog_display_info));
#else
	    gtk_dialog_get_action_area(GTK_DIALOG(dialog_display_info));
#endif
	g_object_set_data(G_OBJECT(dialog_display_info),
			  "dialog_action_area23", dialog_action_area23);
	gtk_widget_show(dialog_action_area23);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area23), 10);

#ifdef USE_GTK_3
	hbuttonbox2 = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
#else
	hbuttonbox2 = gtk_hbutton_box_new();
#endif
	gtk_widget_show(hbuttonbox2);
	gtk_box_pack_start(GTK_BOX(dialog_action_area23), hbuttonbox2,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox2),
				  GTK_BUTTONBOX_END);

	button_close =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_button_new_from_icon_name("window-close",
					  GTK_ICON_SIZE_BUTTON);
#else
	    gtk_button_new_from_stock(GTK_STOCK_CLOSE);
#endif

	gtk_widget_show(button_close);
	gtk_container_add(GTK_CONTAINER(hbuttonbox2), button_close);
	gtk_widget_set_can_default(button_close, 1);

	g_signal_connect(G_OBJECT(button_close), "clicked",
			 G_CALLBACK(button_close_clicked), NULL);

	g_signal_connect(G_OBJECT(dialog_display_info),
			 "destroy",
			 G_CALLBACK(on_dlgInformation_destroy), NULL);

	gtk_widget_show(dialog_display_info);
	gsI_isrunning = TRUE;
	return dialog_display_info;
}
