/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_info_box.c
    * -------------------
    * Sat June 30 2001
    * copyright (C) 2001 by tbiggs
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
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "gs_info_box.h"
#include "support.h"

GtkWidget *create_InfoBox(void)
{
	GtkWidget *InfoBox;
	GtkWidget *dialog_vbox6;
	GtkWidget *hbox20;
	GtkWidget *pixmap2;
	GtkWidget *vbox24;
	GtkWidget *lbInfoBox1;
	GtkWidget *lbInfoBox2;
	GtkWidget *lbInfoBox3;
	GtkWidget *dialog_action_area6;
	GtkWidget *btnInfoBoxYes;
	GtkWidget *btnInfoBoxNo;

	InfoBox = gnome_dialog_new(_("GnomeSword - InfoBox"), NULL);
	gtk_object_set_data(GTK_OBJECT(InfoBox), "InfoBox", InfoBox);
	gtk_widget_set_usize(InfoBox, 216, -2);
	gtk_container_set_border_width(GTK_CONTAINER(InfoBox), 4);
	gtk_window_set_modal(GTK_WINDOW(InfoBox), TRUE);

	dialog_vbox6 = GNOME_DIALOG(InfoBox)->vbox;
	gtk_object_set_data(GTK_OBJECT(InfoBox), "dialog_vbox6",
			    dialog_vbox6);
	gtk_widget_show(dialog_vbox6);

	hbox20 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox20);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "hbox20", hbox20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox20);
	gtk_box_pack_start(GTK_BOX(dialog_vbox6), hbox20, TRUE, TRUE, 0);

	pixmap2 =
    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR "/GnomeSword.xpm");
	gtk_widget_ref(pixmap2);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "pixmap2", pixmap2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap2);
	gtk_box_pack_start(GTK_BOX(hbox20), pixmap2, TRUE, TRUE, 0);

	vbox24 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox24);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "vbox24", vbox24,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox24);
	gtk_box_pack_start(GTK_BOX(hbox20), vbox24, TRUE, TRUE, 0);

	lbInfoBox1 = gtk_label_new("File");
	gtk_widget_ref(lbInfoBox1);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "lbInfoBox1",
				 lbInfoBox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbInfoBox1);
	gtk_box_pack_start(GTK_BOX(vbox24), lbInfoBox1, FALSE, FALSE, 0);

	lbInfoBox2 = gtk_label_new(_("in StudyPad is not saved!"));
	gtk_widget_ref(lbInfoBox2);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "lbInfoBox2",
				 lbInfoBox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbInfoBox2);
	gtk_box_pack_start(GTK_BOX(vbox24), lbInfoBox2, FALSE, FALSE, 0);

	lbInfoBox3 = gtk_label_new(_("Shall I save it?"));
	gtk_widget_ref(lbInfoBox3);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "lbInfoBox3",
				 lbInfoBox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbInfoBox3);
	gtk_box_pack_start(GTK_BOX(vbox24), lbInfoBox3, FALSE, FALSE, 0);

	dialog_action_area6 = GNOME_DIALOG(InfoBox)->action_area;
	gtk_object_set_data(GTK_OBJECT(InfoBox), "dialog_action_area6",
			    dialog_action_area6);
	gtk_widget_show(dialog_action_area6);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area6),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area6), 8);

	gnome_dialog_append_button(GNOME_DIALOG(InfoBox),
				   GNOME_STOCK_BUTTON_YES);
	btnInfoBoxYes = g_list_last(GNOME_DIALOG(InfoBox)->buttons)->data;
	gtk_widget_ref(btnInfoBoxYes);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "btnInfoBoxYes",
				 btnInfoBoxYes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnInfoBoxYes);
	GTK_WIDGET_SET_FLAGS(btnInfoBoxYes, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(InfoBox),
				   GNOME_STOCK_BUTTON_NO);
	btnInfoBoxNo = g_list_last(GNOME_DIALOG(InfoBox)->buttons)->data;
	gtk_widget_ref(btnInfoBoxNo);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "btnInfoBoxNo",
				 btnInfoBoxNo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnInfoBoxNo);
	GTK_WIDGET_SET_FLAGS(btnInfoBoxNo, GTK_CAN_DEFAULT);
/*
	gtk_signal_connect(GTK_OBJECT(btnInfoBoxYes), "clicked",
			   GTK_SIGNAL_FUNC(on_btnInfoBoxYes_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnInfoBoxNo), "clicked",
			   GTK_SIGNAL_FUNC(on_btnInfoBoxNo_clicked), NULL);
*/
	return InfoBox;
}
