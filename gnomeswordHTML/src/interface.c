/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            interface.c
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/

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

#include <gnome.h>
#include <gtk/gtk.h>

#include "callback.h"
#include "gs_studypad.h"
//#include "gs_editor.h"
#include "gs_ui.h"
#include "support.h"
#include "gs_popup_cb.h"

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

	InfoBox = gnome_dialog_new("GnomeSword - InfoBox", NULL);
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
	    create_pixmap(InfoBox, "gnomesword/GnomeSword.xpm", FALSE);
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

	lbInfoBox2 = gtk_label_new("in StudyPad is not saved!");
	gtk_widget_ref(lbInfoBox2);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "lbInfoBox2",
				 lbInfoBox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbInfoBox2);
	gtk_box_pack_start(GTK_BOX(vbox24), lbInfoBox2, FALSE, FALSE, 0);

	lbInfoBox3 = gtk_label_new("Shall I save it?");
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

	gtk_signal_connect(GTK_OBJECT(btnInfoBoxYes), "clicked",
			   GTK_SIGNAL_FUNC(on_btnInfoBoxYes_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnInfoBoxNo), "clicked",
			   GTK_SIGNAL_FUNC(on_btnInfoBoxNo_clicked), NULL);

	return InfoBox;
}

GtkWidget *create_dlgSettings(void)
{
	GtkWidget *dlgSettings;
	GtkWidget *dialog_vbox9;
	GtkWidget *hbox22;
	GtkWidget *pixmap5;
	GtkWidget *notebook7;
	GtkWidget *hbox23;
	GtkWidget *label103;
	GtkWidget *cpfgCurrentverse;
	GtkWidget *label98;
	GtkWidget *vbox28;
	GtkWidget *cbtnShowSCB;
	GtkWidget *cbtnShowCOMtabs;
	GtkWidget *cbtnShowDLtabs;
	GtkWidget *label123;
	GtkWidget *vbox29;
	GtkWidget *cbtnShowTextgroup;
	GtkWidget *cbtnShowComGroup;
	GtkWidget *cbtnShowDictGroup;
	GtkWidget *cbtnShowHistoryGroup;
	GtkWidget *label124;
	GtkWidget *hbox24;
	GtkWidget *cbtnPNformat;
	GtkWidget *label122;
	GtkWidget *dialog_action_area9;
	GtkWidget *btnPropertyboxOK;
	GtkWidget *btnPropertyboxApply;
	GtkWidget *btnPropertyboxCancel;

	dlgSettings = gnome_dialog_new("GnomeSword - Settings", NULL);
	gtk_object_set_data(GTK_OBJECT(dlgSettings), "dlgSettings",
			    dlgSettings);
	gtk_window_set_policy(GTK_WINDOW(dlgSettings), FALSE, FALSE,
			      FALSE);

	dialog_vbox9 = GNOME_DIALOG(dlgSettings)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgSettings), "dialog_vbox9",
			    dialog_vbox9);
	gtk_widget_show(dialog_vbox9);

	hbox22 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox22);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox22", hbox22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox22);
	gtk_box_pack_start(GTK_BOX(dialog_vbox9), hbox22, TRUE, TRUE, 0);

	pixmap5 =
	    create_pixmap(dlgSettings, "gnomesword/GnomeSword.xpm", FALSE);
	gtk_widget_ref(pixmap5);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "pixmap5",
				 pixmap5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap5);
	gtk_box_pack_start(GTK_BOX(hbox22), pixmap5, TRUE, TRUE, 0);

	notebook7 = gtk_notebook_new();
	gtk_widget_ref(notebook7);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "notebook7",
				 notebook7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook7);
	gtk_box_pack_start(GTK_BOX(hbox22), notebook7, TRUE, TRUE, 0);

	hbox23 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox23);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox23", hbox23,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox23);
	gtk_container_add(GTK_CONTAINER(notebook7), hbox23);

	label103 = gtk_label_new("Current Verse Color");
	gtk_widget_ref(label103);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label103",
				 label103,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label103);
	gtk_box_pack_start(GTK_BOX(hbox23), label103, FALSE, FALSE, 0);
	gtk_widget_set_usize(label103, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label103), GTK_JUSTIFY_LEFT);

	cpfgCurrentverse = gnome_color_picker_new();
	gtk_widget_ref(cpfgCurrentverse);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cpfgCurrentverse", cpfgCurrentverse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cpfgCurrentverse);
	gtk_box_pack_start(GTK_BOX(hbox23), cpfgCurrentverse, TRUE, FALSE,
			   0);

	label98 = gtk_label_new("Main window");
	gtk_widget_ref(label98);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label98",
				 label98,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label98);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     0), label98);

	vbox28 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox28);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox28", vbox28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox28);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox28);

	cbtnShowSCB = gtk_check_button_new_with_label("Show Shortcut bar");
	gtk_widget_ref(cbtnShowSCB);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "cbtnShowSCB",
				 cbtnShowSCB,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowSCB);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowSCB, FALSE, FALSE, 0);

	cbtnShowCOMtabs =
	    gtk_check_button_new_with_label
	    ("Show Commentary notebook tabs");
	gtk_widget_ref(cbtnShowCOMtabs);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowCOMtabs", cbtnShowCOMtabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowCOMtabs);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowCOMtabs, FALSE, FALSE,
			   0);

	cbtnShowDLtabs =
	    gtk_check_button_new_with_label("Show Dict/Lex notebook tabs");
	gtk_widget_ref(cbtnShowDLtabs);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "cbtnShowDLtabs",
				 cbtnShowDLtabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowDLtabs);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowDLtabs, FALSE, FALSE,
			   0);

	label123 = gtk_label_new("Interface");
	gtk_widget_ref(label123);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label123",
				 label123,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label123);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     1), label123);

	vbox29 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox29);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox29", vbox29,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox29);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox29);

	cbtnShowTextgroup =
	    gtk_check_button_new_with_label
	    ("Show Bible Text Group in Shortcut Bar");
	gtk_widget_ref(cbtnShowTextgroup);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowTextgroup", cbtnShowTextgroup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowTextgroup);
	gtk_box_pack_start(GTK_BOX(vbox29), cbtnShowTextgroup, FALSE,
			   FALSE, 0);

	cbtnShowComGroup =
	    gtk_check_button_new_with_label
	    ("Show Commentary Group in Shortcut Bar");
	gtk_widget_ref(cbtnShowComGroup);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowComGroup", cbtnShowComGroup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowComGroup);
	gtk_box_pack_start(GTK_BOX(vbox29), cbtnShowComGroup, FALSE, FALSE,
			   0);

	cbtnShowDictGroup =
	    gtk_check_button_new_with_label
	    ("Show Dict/Lex Group in Shortcut bar");
	gtk_widget_ref(cbtnShowDictGroup);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowDictGroup", cbtnShowDictGroup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowDictGroup);
	gtk_box_pack_start(GTK_BOX(vbox29), cbtnShowDictGroup, FALSE,
			   FALSE, 0);

	cbtnShowHistoryGroup =
	    gtk_check_button_new_with_label
	    ("Show History Group in Shortcut Bar");
	gtk_widget_ref(cbtnShowHistoryGroup);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowHistoryGroup",
				 cbtnShowHistoryGroup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowHistoryGroup);
	gtk_box_pack_start(GTK_BOX(vbox29), cbtnShowHistoryGroup, FALSE,
			   FALSE, 0);

	label124 = gtk_label_new("Shortcut Bar");
	gtk_widget_ref(label124);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label124",
				 label124,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label124);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     2), label124);

	hbox24 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox24);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox24", hbox24,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox24);
	gtk_container_add(GTK_CONTAINER(notebook7), hbox24);

	cbtnPNformat = gtk_check_button_new_with_label("Use Formatting");
	gtk_widget_ref(cbtnPNformat);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "cbtnPNformat",
				 cbtnPNformat,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnPNformat);
	gtk_box_pack_start(GTK_BOX(hbox24), cbtnPNformat, FALSE, FALSE, 0);

	label122 = gtk_label_new("Personal Notes");
	gtk_widget_ref(label122);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label122",
				 label122,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label122);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     3), label122);

	dialog_action_area9 = GNOME_DIALOG(dlgSettings)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgSettings), "dialog_action_area9",
			    dialog_action_area9);
	gtk_widget_show(dialog_action_area9);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area9),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area9), 8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSettings),
				   GNOME_STOCK_BUTTON_OK);
	btnPropertyboxOK =
	    g_list_last(GNOME_DIALOG(dlgSettings)->buttons)->data;
	gtk_widget_ref(btnPropertyboxOK);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnPropertyboxOK", btnPropertyboxOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPropertyboxOK);
	gtk_widget_set_sensitive(btnPropertyboxOK, FALSE);
	GTK_WIDGET_SET_FLAGS(btnPropertyboxOK, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSettings),
				   GNOME_STOCK_BUTTON_APPLY);
	btnPropertyboxApply =
	    g_list_last(GNOME_DIALOG(dlgSettings)->buttons)->data;
	gtk_widget_ref(btnPropertyboxApply);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnPropertyboxApply",
				 btnPropertyboxApply,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPropertyboxApply);
	gtk_widget_set_sensitive(btnPropertyboxApply, FALSE);
	GTK_WIDGET_SET_FLAGS(btnPropertyboxApply, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSettings),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnPropertyboxCancel =
	    g_list_last(GNOME_DIALOG(dlgSettings)->buttons)->data;
	gtk_widget_ref(btnPropertyboxCancel);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnPropertyboxCancel",
				 btnPropertyboxCancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPropertyboxCancel);
	GTK_WIDGET_SET_FLAGS(btnPropertyboxCancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(cpfgCurrentverse), "color_set",
			   GTK_SIGNAL_FUNC(on_cpfgCurrentverse_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowSCB), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowSCB_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowCOMtabs), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowCOMtabs_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowDLtabs), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowDLtabs_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowTextgroup), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowTextgroup_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowComGroup), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowComGroup_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowDictGroup), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowDictGroup_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowHistoryGroup), "toggled",
			   GTK_SIGNAL_FUNC
			   (on_cbtnShowHistoryGroup_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnPNformat), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnPNformat_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPropertyboxOK_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxApply), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPropertyboxApply_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxCancel), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btnPropertyboxCancel_clicked), NULL);

	return dlgSettings;
}



