/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_unlockkey_dlg.c
     * -------------------
     * Sat Aug 18 2001
     * copyright (C) 2001 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

#include "gs_unlockkey_dlg.h"
#include "sw_sword.h"
#include "gs_gnomesword.h"
#include "support.h"

gint modwindow;

static void on_btkUKok_clicked(GtkButton * button, gpointer user_data);
static void on_btnUKcancel_clicked(GtkButton * button, gpointer user_data);


/***  ***/
static void on_btkUKok_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg, *entry;
	gchar *buf;

	entry = GTK_WIDGET(user_data);
	buf = gtk_entry_get_text(GTK_ENTRY(entry));
	savekeySWORD(modwindow, buf);
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

/***  ***/
static void on_btnUKcancel_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

/*** create dialog to add key to unlock a module ***/
GtkWidget *create_dlgUnlockKey(gint window)
{
	GtkWidget *dlgUnlockKey;
	GtkWidget *dialog_vbox15;
	GtkWidget *vbox42;
	GtkWidget *label177;
	GtkWidget *entryKey;
	GtkWidget *dialog_action_area15;
	GtkWidget *btkUKok;
	GtkWidget *btnUKcancel;

	modwindow = window;

	dlgUnlockKey = gnome_dialog_new(_("GnomeSword"), NULL);
	gtk_object_set_data(GTK_OBJECT(dlgUnlockKey), "dlgUnlockKey",
			    dlgUnlockKey);
	gtk_window_set_policy(GTK_WINDOW(dlgUnlockKey), FALSE, FALSE,
			      FALSE);

	dialog_vbox15 = GNOME_DIALOG(dlgUnlockKey)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgUnlockKey), "dialog_vbox15",
			    dialog_vbox15);
	gtk_widget_show(dialog_vbox15);

	vbox42 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox42);
	gtk_object_set_data_full(GTK_OBJECT(dlgUnlockKey), "vbox42",
				 vbox42,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox42);
	gtk_box_pack_start(GTK_BOX(dialog_vbox15), vbox42, TRUE, TRUE, 0);

	label177 = gtk_label_new(_("Enter Unlock Key"));
	gtk_widget_ref(label177);
	gtk_object_set_data_full(GTK_OBJECT(dlgUnlockKey), "label177",
				 label177,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label177);
	gtk_box_pack_start(GTK_BOX(vbox42), label177, FALSE, FALSE, 0);

	entryKey = gtk_entry_new();
	gtk_widget_ref(entryKey);
	gtk_object_set_data_full(GTK_OBJECT(dlgUnlockKey), "entryKey",
				 entryKey,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryKey);
	gtk_box_pack_start(GTK_BOX(vbox42), entryKey, FALSE, FALSE, 0);
	gtk_widget_set_usize(entryKey, 247, -2);

	dialog_action_area15 = GNOME_DIALOG(dlgUnlockKey)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgUnlockKey),
			    "dialog_action_area15", dialog_action_area15);
	gtk_widget_show(dialog_action_area15);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area15),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area15),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgUnlockKey),
				   GNOME_STOCK_BUTTON_OK);
	btkUKok =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgUnlockKey)->buttons)->
		       data);
	gtk_widget_ref(btkUKok);
	gtk_object_set_data_full(GTK_OBJECT(dlgUnlockKey), "btkUKok",
				 btkUKok,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btkUKok);
	GTK_WIDGET_SET_FLAGS(btkUKok, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgUnlockKey),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnUKcancel =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgUnlockKey)->buttons)->
		       data);
	gtk_widget_ref(btnUKcancel);
	gtk_object_set_data_full(GTK_OBJECT(dlgUnlockKey), "btnUKcancel",
				 btnUKcancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnUKcancel);
	GTK_WIDGET_SET_FLAGS(btnUKcancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(btkUKok), "clicked",
			   GTK_SIGNAL_FUNC(on_btkUKok_clicked),
			   GTK_WIDGET(entryKey));
	gtk_signal_connect(GTK_OBJECT(btnUKcancel), "clicked",
			   GTK_SIGNAL_FUNC(on_btnUKcancel_clicked), NULL);

	return dlgUnlockKey;
}
