/*
 * GnomeSword Bible Study Tool
 * cipher_key_dialog.c  - asks for key to unlock sword module
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

#include "cipher_key_dialog.h"
#include "gs_gnomesword.h"
#include "support.h"


static GtkWidget *entryKey;
static void on_btkUKok_clicked(GtkButton * button, gchar *mod_name);
static void on_btnUKcancel_clicked(GtkButton * button, gpointer user_data);

/******************************************************************************
 * Name
 *   on_btkUKok_clicked
 *
 * Synopsis
 *   #include "unlock_key_dialog.h"
 *
 *   static void on_btkUKok_clicked(GtkButton * button, gchar *mod_name)	
 *
 * Description
 *   gets key from dialog entry passed it to save_module_key and
 *   closes dialog
 *
 * Return value
 *   void
 */
static void on_btkUKok_clicked(GtkButton * button, gchar *mod_name)
{
	GtkWidget *dlg;
	gchar *buf;
	
	buf = gtk_entry_get_text(GTK_ENTRY(entryKey));
	save_module_key(mod_name, buf);
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

/******************************************************************************
 * Name
 *   on_btnUKcancel_clicked
 *
 * Synopsis
 *   #include "unlock_key_dialog.h"
 *
 *   static void on_btnUKcancel_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *   closes dialog with out saving key
 *
 * Return value
 *   void
 */
static void on_btnUKcancel_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

/******************************************************************************
 * Name
 *   gui_create_unlock_key_dialog
 *
 * Synopsis
 *   #include "unlock_key_dialog.h"
 *
 *   GtkWidget *gui_create_unlock_key_dialog(gchar *mod_name)	
 *
 * Description
 *   create dialog to get key to unlock a module
 *
 * Return value
 *   GtkWidget*
 */
GtkWidget *gui_create_unlock_key_dialog(gchar *mod_name)
{
	GtkWidget *dialog_unlock_key;
	GtkWidget *dialog_vbox;
	GtkWidget *vbox;
	GtkWidget *label;
	GtkWidget *dialog_action_area15;
	GtkWidget *btkUKok;
	GtkWidget *btnUKcancel;


	dialog_unlock_key = gnome_dialog_new(_("GnomeSword"), NULL);
	gtk_object_set_data(GTK_OBJECT(dialog_unlock_key), "dialog_unlock_key",
			    dialog_unlock_key);
	gtk_window_set_policy(GTK_WINDOW(dialog_unlock_key), FALSE, FALSE,
			      FALSE);

	dialog_vbox = GNOME_DIALOG(dialog_unlock_key)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_unlock_key), "dialog_vbox",
			    dialog_vbox);
	gtk_widget_show(dialog_vbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	gtk_object_set_data_full(GTK_OBJECT(dialog_unlock_key), "vbox",
				 vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);
	gtk_box_pack_start(GTK_BOX(dialog_vbox), vbox, TRUE, TRUE, 0);

	label = gtk_label_new(_("Enter Unlock Key"));
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(dialog_unlock_key), "label",
				 label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

	entryKey = gtk_entry_new();
	gtk_widget_ref(entryKey);
	gtk_object_set_data_full(GTK_OBJECT(dialog_unlock_key), "entryKey",
				 entryKey,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryKey);
	gtk_box_pack_start(GTK_BOX(vbox), entryKey, FALSE, FALSE, 0);
	gtk_widget_set_usize(entryKey, 247, -2);

	dialog_action_area15 = GNOME_DIALOG(dialog_unlock_key)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog_unlock_key),
			    "dialog_action_area15", dialog_action_area15);
	gtk_widget_show(dialog_action_area15);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area15),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area15),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(dialog_unlock_key),
				   GNOME_STOCK_BUTTON_OK);
	btkUKok =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dialog_unlock_key)->buttons)->
		       data);
	gtk_widget_ref(btkUKok);
	gtk_object_set_data_full(GTK_OBJECT(dialog_unlock_key), "btkUKok",
				 btkUKok,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btkUKok);
	GTK_WIDGET_SET_FLAGS(btkUKok, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dialog_unlock_key),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnUKcancel =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dialog_unlock_key)->buttons)->
		       data);
	gtk_widget_ref(btnUKcancel);
	gtk_object_set_data_full(GTK_OBJECT(dialog_unlock_key), "btnUKcancel",
				 btnUKcancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnUKcancel);
	GTK_WIDGET_SET_FLAGS(btnUKcancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(btkUKok), "clicked",
			   GTK_SIGNAL_FUNC(on_btkUKok_clicked),
			   (gchar*)mod_name);
	gtk_signal_connect(GTK_OBJECT(btnUKcancel), "clicked",
			   GTK_SIGNAL_FUNC(on_btnUKcancel_clicked), NULL);

	return dialog_unlock_key;
}
