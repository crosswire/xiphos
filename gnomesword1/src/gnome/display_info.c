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

#include "gui/gtkhtml_display.h"
#include "gui/display_info.h"

#include "main/sword.h"
#include "main/gs_gnomesword.h"

gboolean gsI_isrunning = FALSE;

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
	
	text = get_module_text(mod_name, key);
	if(text) {
		entry_display(html_widget, mod_name,
		   text, key, TRUE);
		free(text);
	}
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

static void on_btnInfoOK_clicked(GtkButton * button, gpointer user_data)
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
	GtkWidget *dlgInformation;
	GtkWidget *dialog_vbox15;
	GtkWidget *scwnInfo;
	GtkWidget *dialog_action_area15;
	GtkWidget *btnInfoOK;

	dlgInformation = gnome_dialog_new(NULL, NULL);
	gtk_object_set_data(GTK_OBJECT(dlgInformation), "dlgInformation",
			    dlgInformation);
	GTK_WINDOW(dlgInformation)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_default_size(GTK_WINDOW(dlgInformation), 308, 267);
	gtk_window_set_policy(GTK_WINDOW(dlgInformation), FALSE, TRUE,
			      TRUE);

	dialog_vbox15 = GNOME_DIALOG(dlgInformation)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgInformation), "dialog_vbox15",
			    dialog_vbox15);
	gtk_widget_show(dialog_vbox15);

	scwnInfo = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scwnInfo);
	gtk_object_set_data_full(GTK_OBJECT(dlgInformation), "scwnInfo",
				 scwnInfo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scwnInfo);
	gtk_box_pack_start(GTK_BOX(dialog_vbox15), scwnInfo, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwnInfo),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	html_widget = gtk_html_new();
	gtk_widget_ref(html_widget);
	gtk_object_set_data_full(GTK_OBJECT(dlgInformation), "html_widget", html_widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(html_widget);
	gtk_container_add(GTK_CONTAINER(scwnInfo), html_widget);

	dialog_action_area15 = GNOME_DIALOG(dlgInformation)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgInformation),
			    "dialog_action_area15", dialog_action_area15);
	gtk_widget_show(dialog_action_area15);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area15),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area15),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgInformation),
				   GNOME_STOCK_BUTTON_OK);
	btnInfoOK =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgInformation)->buttons)->
		       data);
	gtk_widget_ref(btnInfoOK);
	gtk_object_set_data_full(GTK_OBJECT(dlgInformation), "btnInfoOK",
				 btnInfoOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnInfoOK);
	GTK_WIDGET_SET_FLAGS(btnInfoOK, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(dlgInformation), "destroy",
			   GTK_SIGNAL_FUNC(on_dlgInformation_destroy), NULL);
	gtk_signal_connect(GTK_OBJECT(btnInfoOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnInfoOK_clicked), NULL);
	gsI_isrunning = TRUE;
        
        
	return dlgInformation;
}
