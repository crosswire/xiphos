/*
 * Xiphos Bible Study Tool
 * dialog.c -
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
#include "gui/dialog.h"
#include "gui/widgets.h"
#include "gui/utilities.h"

#include "main/settings.h"
#include "main/sword.h"

#include "gui/debug_glib_null.h"

static GtkWidget *entry1 = NULL;
static GtkWidget *entry2 = NULL;
static GtkWidget *entry3 = NULL;
static GtkWidget *entry4 = NULL;
static GtkWidget *entry5 = NULL;
static GtkWidget *entry6 = NULL;
static gint retval = 3;

GS_DIALOG *standard_info;
GtkWidget *dialog_request;

/******************************************************************************
 * Name
 *   get_entry_text
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   void get_entry_text(GS_DIALOG * info)
 *
 * Description
 *   get dialog entry widget's text
 *
 * Return value
 *   void
 */

static void get_entry_text(GS_DIALOG *info)
{
	if (entry1) {
		if (info->text1)
			g_free(info->text1);
		info->text1 =
		    g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
	}
	if (entry2) {
		if (info->text2)
			g_free(info->text2);
		info->text2 =
		    g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
	}
	if (entry3) {
		if (info->text3)
			g_free(info->text3);
		info->text3 =
		    g_strdup(gtk_entry_get_text(GTK_ENTRY(entry3)));
	}
	if (entry4) {
		if (info->text4)
			g_free(info->text4);
		info->text4 =
		    g_strdup(gtk_entry_get_text(GTK_ENTRY(entry4)));
	}
	if (entry5) {
		if (info->text5)
			g_free(info->text5);
		info->text5 =
		    g_strdup(gtk_entry_get_text(GTK_ENTRY(entry5)));
	}
	if (entry6) {
		if (info->text6)
			g_free(info->text6);
		info->text6 =
		    g_strdup(gtk_entry_get_text(GTK_ENTRY(entry6)));
	}
}

/******************************************************************************
 * Name
 *   on_dialog_response
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   void on_dialog_response(GtkDialog * dialog, gint response_id, GS_DIALOG * info)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_dialog_response(GtkDialog *dialog, gint response_id,
			       GS_DIALOG *info)
{
	switch (response_id) {
	case GTK_RESPONSE_OK:
		retval = GS_OK;
		get_entry_text(info);
		break;
	case GTK_RESPONSE_YES:
		retval = GS_YES;
		get_entry_text(info);
		break;
	case GTK_RESPONSE_NO:
		retval = GS_NO;
		break;
	case GTK_RESPONSE_CANCEL:
		retval = GS_CANCEL;
		break;
	}
}

/******************************************************************************
 * Name
 *   on_dialog_enter
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   void on_dialog_enter(void)
 *
 * Description
 *   canned Enter key route to on_dialog_response
 *
 * Return value
 *   void
 */

static void on_dialog_enter(void)
{
	on_dialog_response(GTK_DIALOG(dialog_request), GTK_RESPONSE_OK,
			   standard_info);
	gtk_widget_destroy(GTK_WIDGET(dialog_request));
}

static GtkWidget *create_dialog_alert(GS_DIALOG *info)
{
	GtkWidget *dialog_alert;
	GtkWidget *dialog_vbox2;
	GtkWidget *hbox3;
	GtkWidget *vbox2;
	GtkWidget *label7;
	GtkWidget *dialog_action_area2;

	dialog_alert = gtk_dialog_new();
	gtk_container_set_border_width(GTK_CONTAINER(dialog_alert), 5);
	gtk_window_set_title(GTK_WINDOW(dialog_alert), " ");

#ifndef USE_GTK_3
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog_alert), FALSE);
#endif

	dialog_vbox2 =
	    gtk_dialog_get_content_area(GTK_DIALOG(dialog_alert));
	gtk_widget_show(dialog_vbox2);

	UI_HBOX(hbox3, FALSE, 12);
	gtk_widget_show(hbox3);
	gtk_box_pack_start(GTK_BOX(dialog_vbox2), hbox3, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox3), 6);

	if (info->stock_icon) {
		GtkWidget *image5 =
#if GTK_CHECK_VERSION(3, 10, 0)
		    gtk_image_new_from_icon_name
#else
		    gtk_image_new_from_stock
#endif
		    (info->stock_icon, GTK_ICON_SIZE_DIALOG);
		gtk_widget_show(image5);
		gtk_box_pack_start(GTK_BOX(hbox3), image5, FALSE, TRUE, 0);
#if !GTK_CHECK_VERSION(3, 10, 0)
		gtk_misc_set_alignment(GTK_MISC(image5), 0.5, 0);
		gtk_misc_set_padding(GTK_MISC(image5), 12, 0);
#endif
	}

	UI_VBOX(vbox2, FALSE, 6);
	gtk_widget_show(vbox2);
	gtk_box_pack_start(GTK_BOX(hbox3), vbox2, TRUE, TRUE, 0);

	label7 = gtk_label_new(info->label_top);
	gtk_widget_show(label7);
	gtk_box_pack_start(GTK_BOX(vbox2), label7, FALSE, FALSE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label7), TRUE);
	gtk_label_set_justify(GTK_LABEL(label7), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label7), TRUE);
#if !GTK_CHECK_VERSION(3, 10, 0)
	gtk_misc_set_alignment(GTK_MISC(label7), 0, 0.5);
#endif

	if (info->label2) {
		gtk_window_set_default_size(GTK_WINDOW(dialog_alert), 380,
					    200);
		GtkWidget *scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_show(scrolledwindow);
		gtk_box_pack_start(GTK_BOX(vbox2), scrolledwindow, TRUE,
				   TRUE, 0);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
					       GTK_POLICY_AUTOMATIC,
					       GTK_POLICY_AUTOMATIC);

		GtkWidget *viewport = gtk_viewport_new(NULL, NULL);
		gtk_widget_show(viewport);
		gtk_container_add(GTK_CONTAINER(scrolledwindow), viewport);

		GtkWidget *label10 = gtk_label_new(info->label2);
		gtk_widget_show(label10);
		gtk_container_add(GTK_CONTAINER(viewport), label10);
		gtk_label_set_justify(GTK_LABEL(label10),
				      GTK_JUSTIFY_LEFT);
#if !GTK_CHECK_VERSION(3, 10, 0)
		gtk_misc_set_alignment(GTK_MISC(label10), 0, 0.5);
#endif
	}

	dialog_action_area2 =
#if GTK_CHECK_VERSION(3, 12, 0)
	    gtk_dialog_get_content_area(GTK_DIALOG(dialog_alert));
#else
	    gtk_dialog_get_action_area(GTK_DIALOG(dialog_alert));
#endif
	gtk_widget_show(dialog_action_area2);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area2),
				  GTK_BUTTONBOX_END);
#if GTK_CHECK_VERSION(3, 10, 0)
	if (info->ok)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      _("_Ok"), GTK_RESPONSE_OK);
	if (info->yes)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      _("_Yes"), GTK_RESPONSE_YES);
	if (info->save)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      _("_Save"), GTK_RESPONSE_YES);
	if (info->no)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      _("_No"), GTK_RESPONSE_NO);
	if (info->cancel)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      _("_Cancel"), GTK_RESPONSE_CANCEL);
#else
	if (info->ok)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_OK, GTK_RESPONSE_OK);
	if (info->yes)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_YES, GTK_RESPONSE_YES);
	if (info->save)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_SAVE, GTK_RESPONSE_YES);
	if (info->no)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_NO, GTK_RESPONSE_NO);
	if (info->cancel)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_CANCEL,
				      GTK_RESPONSE_CANCEL);
#endif
	if (info->no_save)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      _("Close without Saving"),
				      GTK_RESPONSE_NO);

	g_signal_connect((gpointer)dialog_alert, "response",
			 G_CALLBACK(on_dialog_response), info);

	return dialog_alert;
}

/******************************************************************************
 * Name
 *   create_dialog_request
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   GtkWidget *create_dialog_request(GS_DIALOG * info)
 *
 * Description
 *   creates the dialog
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_dialog_request(GS_DIALOG *info)
{
	GtkWidget *dialog_vbox3 = NULL;
	GtkWidget *hbox4 = NULL;
	GtkWidget *vbox3 = NULL;
	GtkWidget *label8 = NULL;
	GtkWidget *table2 = NULL;
	GtkWidget *label9 = NULL;
	GtkWidget *label10 = NULL;
	GtkWidget *label11 = NULL;
	GtkWidget *label12 = NULL;
	GtkWidget *label13 = NULL;
	GtkWidget *label14 = NULL;
	GtkWidget *dialog_action_area3 = NULL;
#if GTK_CHECK_VERSION(3, 4, 0)
	gint nextrow = 0;
#endif

	dialog_request = gtk_dialog_new();
	info->dialog = dialog_request;
	gtk_container_set_border_width(GTK_CONTAINER(dialog_request), 6);
	gtk_window_set_title(GTK_WINDOW(dialog_request),
			     (info->title ? info->title : " "));
	gtk_window_set_modal(GTK_WINDOW(dialog_request), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(dialog_request), FALSE);
#ifndef USE_GTK_3
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog_request), FALSE);
#endif
	dialog_vbox3 =
	    gtk_dialog_get_content_area(GTK_DIALOG(dialog_request));
	gtk_widget_show(dialog_vbox3);

	UI_HBOX(hbox4, FALSE, 12);
	gtk_widget_show(hbox4);
	gtk_box_pack_start(GTK_BOX(dialog_vbox3), hbox4, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox4), 6);

	if (info->stock_icon) {
#if GTK_CHECK_VERSION(3, 10, 0)
		GtkWidget *image6 =
		    gtk_image_new_from_icon_name(info->stock_icon,
						 GTK_ICON_SIZE_DIALOG);

#else
		GtkWidget *image6 =
		    gtk_image_new_from_stock(info->stock_icon,
					     GTK_ICON_SIZE_DIALOG);
#endif
		gtk_widget_show(image6);
		gtk_box_pack_start(GTK_BOX(hbox4), image6, FALSE, TRUE, 0);
#if !GTK_CHECK_VERSION(3, 10, 0)
		gtk_misc_set_alignment(GTK_MISC(image6), 0.5, 0);
		gtk_misc_set_padding(GTK_MISC(image6), 12, 0);
#endif
	}
	UI_VBOX(vbox3, FALSE, 4);
	gtk_widget_show(vbox3);
	gtk_box_pack_start(GTK_BOX(hbox4), vbox3, TRUE, TRUE, 0);

	label8 = gtk_label_new(info->label_top);
	gtk_widget_show(label8);
	gtk_box_pack_start(GTK_BOX(vbox3), label8, FALSE, FALSE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label8), TRUE);
	gtk_label_set_justify(GTK_LABEL(label8), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label8), TRUE);
#if !GTK_CHECK_VERSION(3, 10, 0)
	gtk_misc_set_alignment(GTK_MISC(label8), 0.5, 0);
#endif

	if (info->label1) {
		label9 = gtk_label_new(info->label1);
		gtk_widget_show(label9);
		gtk_label_set_justify(GTK_LABEL(label9), GTK_JUSTIFY_LEFT);
#if !GTK_CHECK_VERSION(3, 10, 0)
		gtk_misc_set_alignment(GTK_MISC(label9), 0, 0.5);
#endif

		entry1 = gtk_entry_new();
		gtk_widget_show(entry1);
		if (info->text1)
			gtk_entry_set_text(GTK_ENTRY(entry1), info->text1);
		g_signal_connect((gpointer)entry1, "activate",
				 G_CALLBACK(on_dialog_enter), info);
	}

	if (info->label2) {
		label10 = gtk_label_new(info->label2);
		gtk_widget_show(label10);
		gtk_label_set_justify(GTK_LABEL(label10),
				      GTK_JUSTIFY_LEFT);
#if !GTK_CHECK_VERSION(3, 10, 0)
		gtk_misc_set_alignment(GTK_MISC(label10), 0, 0.5);
#endif

		entry2 = gtk_entry_new();
		gtk_widget_show(entry2);
		if (info->text2)
			gtk_entry_set_text(GTK_ENTRY(entry2), info->text2);
		g_signal_connect((gpointer)entry2, "activate",
				 G_CALLBACK(on_dialog_enter), info);
	}

	if (info->label3) {
		label11 = gtk_label_new(info->label3);
		gtk_widget_show(label11);
		gtk_label_set_justify(GTK_LABEL(label11),
				      GTK_JUSTIFY_LEFT);
#if !GTK_CHECK_VERSION(3, 10, 0)
		gtk_misc_set_alignment(GTK_MISC(label11), 0, 0.5);
#endif

		entry3 = gtk_entry_new();
		gtk_widget_show(entry3);
		if (info->text3)
			gtk_entry_set_text(GTK_ENTRY(entry3), info->text3);
		g_signal_connect((gpointer)entry3, "activate",
				 G_CALLBACK(on_dialog_enter), info);
	}

	if (info->label4) {
		label12 = gtk_label_new(info->label4);
		gtk_widget_show(label12);
		gtk_label_set_justify(GTK_LABEL(label12),
				      GTK_JUSTIFY_LEFT);
#if !GTK_CHECK_VERSION(3, 10, 0)
		gtk_misc_set_alignment(GTK_MISC(label12), 0, 0.5);
#endif

		entry4 = gtk_entry_new();
		gtk_widget_show(entry4);

		if (info->text4)
			gtk_entry_set_text(GTK_ENTRY(entry4), info->text4);
		g_signal_connect((gpointer)entry4, "activate",
				 G_CALLBACK(on_dialog_enter), info);
	}

	if (info->label5) {
		label13 = gtk_label_new(info->label5);
		gtk_widget_show(label13);
		gtk_label_set_justify(GTK_LABEL(label13),
				      GTK_JUSTIFY_LEFT);
#if !GTK_CHECK_VERSION(3, 10, 0)
		gtk_misc_set_alignment(GTK_MISC(label13), 0, 0.5);
#endif

		entry5 = gtk_entry_new();
		gtk_widget_show(entry5);

		if (info->text5)
			gtk_entry_set_text(GTK_ENTRY(entry5), info->text5);
		g_signal_connect((gpointer)entry5, "activate",
				 G_CALLBACK(on_dialog_enter), info);
	}

	if (info->label6) {
		label14 = gtk_label_new(info->label6);
		gtk_widget_show(label14);
		gtk_label_set_justify(GTK_LABEL(label14),
				      GTK_JUSTIFY_LEFT);
#if !GTK_CHECK_VERSION(3, 10, 0)
		gtk_misc_set_alignment(GTK_MISC(label14), 0, 0.5);
#endif

		entry6 = gtk_entry_new();
		gtk_widget_show(entry6);

		if (info->text6)
			gtk_entry_set_text(GTK_ENTRY(entry6), info->text6);
		g_signal_connect((gpointer)entry6, "activate",
				 G_CALLBACK(on_dialog_enter), info);
	}
#if GTK_CHECK_VERSION(3, 4, 0)
	table2 = gtk_grid_new();
	gtk_widget_show(table2);
	gtk_box_pack_start(GTK_BOX(vbox3), table2, TRUE, TRUE, 0);
	gtk_grid_set_row_spacing(GTK_GRID(table2), 3);
	gtk_grid_set_column_spacing(GTK_GRID(table2), 3);

	if (info->label1) {
		gtk_grid_attach(GTK_GRID(table2), label9, 0, 0, 1, 1);

		gtk_grid_attach(GTK_GRID(table2), entry1, 1, 0, 1, 1);
		++nextrow;
	}

	if (info->label2) {
		gtk_grid_insert_row(GTK_GRID(table2), nextrow);

		gtk_grid_attach(GTK_GRID(table2), label10, 0, 1, 1, 1);

		gtk_grid_attach(GTK_GRID(table2), entry2, 1, 1, 1, 1);
		++nextrow;
	}

	if (info->label3) {
		gtk_grid_insert_row(GTK_GRID(table2), nextrow);

		gtk_grid_attach(GTK_GRID(table2), label11, 0, 2, 1, 1);

		gtk_grid_attach(GTK_GRID(table2), entry3, 1, 2, 1, 1);
		++nextrow;
	}

	if (info->label4) {
		gtk_grid_insert_row(GTK_GRID(table2), nextrow);

		gtk_grid_attach(GTK_GRID(table2), label12, 0, 3, 1, 1);

		gtk_grid_attach(GTK_GRID(table2), entry4, 1, 3, 1, 1);
		++nextrow;
	}

	if (info->label5) {
		gtk_grid_insert_row(GTK_GRID(table2), nextrow);

		gtk_grid_attach(GTK_GRID(table2), label13, 0, 4, 1, 1);

		gtk_grid_attach(GTK_GRID(table2), entry5, 1, 4, 1, 1);
		++nextrow;
	}

	if (info->label6) {
		gtk_grid_insert_row(GTK_GRID(table2), nextrow);

		gtk_grid_attach(GTK_GRID(table2), label14, 0, 5, 1, 1);

		gtk_grid_attach(GTK_GRID(table2), entry6, 1, 5, 1, 1);
	}
#else
	table2 = gtk_table_new(3, 2, FALSE);
	gtk_widget_show(table2);
	gtk_box_pack_start(GTK_BOX(vbox3), table2, TRUE, TRUE, 0);
	gtk_table_set_row_spacings(GTK_TABLE(table2), 3);
	gtk_table_set_col_spacings(GTK_TABLE(table2), 3);

	if (info->label1) {
		gtk_table_attach(GTK_TABLE(table2), label9, 0, 1, 0, 1,
				 (GtkAttachOptions)(GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);

		gtk_table_attach(GTK_TABLE(table2), entry1, 1, 2, 0, 1,
				 (GtkAttachOptions)(GTK_EXPAND |
						    GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);
	}

	if (info->label2) {
		gtk_table_attach(GTK_TABLE(table2), label10, 0, 1, 1, 2,
				 (GtkAttachOptions)(GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);

		gtk_table_attach(GTK_TABLE(table2), entry2, 1, 2, 1, 2,
				 (GtkAttachOptions)(GTK_EXPAND |
						    GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);
	}

	if (info->label3) {
		gtk_table_attach(GTK_TABLE(table2), label11, 0, 1, 2, 3,
				 (GtkAttachOptions)(GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);

		gtk_table_attach(GTK_TABLE(table2), entry3, 1, 2, 2, 3,
				 (GtkAttachOptions)(GTK_EXPAND |
						    GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);
	}

	if (info->label4) {
		gtk_table_attach(GTK_TABLE(table2), label12, 0, 1, 3, 4,
				 (GtkAttachOptions)(GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);

		gtk_table_attach(GTK_TABLE(table2), entry4, 1, 2, 3, 4,
				 (GtkAttachOptions)(GTK_EXPAND |
						    GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);
	}

	if (info->label5) {

		gtk_table_attach(GTK_TABLE(table2), label13, 0, 1, 4, 5,
				 (GtkAttachOptions)(GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);

		gtk_table_attach(GTK_TABLE(table2), entry5, 1, 2, 4, 5,
				 (GtkAttachOptions)(GTK_EXPAND |
						    GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);
	}

	if (info->label6) {

		gtk_table_attach(GTK_TABLE(table2), label14, 0, 1, 5, 6,
				 (GtkAttachOptions)(GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);

		gtk_table_attach(GTK_TABLE(table2), entry6, 1, 2, 5, 6,
				 (GtkAttachOptions)(GTK_EXPAND |
						    GTK_FILL),
				 (GtkAttachOptions)(0), 0, 0);
	}
#endif
	dialog_action_area3 =
#if GTK_CHECK_VERSION(3, 12, 0)
	    gtk_dialog_get_content_area(GTK_DIALOG(dialog_request));
#else
	    gtk_dialog_get_action_area(GTK_DIALOG(dialog_request));
#endif
	gtk_widget_show(dialog_action_area3);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area3),
				  GTK_BUTTONBOX_END);
#if GTK_CHECK_VERSION(3, 10, 0)
	if (info->no)
		gtk_dialog_add_button(GTK_DIALOG(dialog_request),
				      _("_No"), GTK_RESPONSE_NO);
	if (info->yes)
		gtk_dialog_add_button(GTK_DIALOG(dialog_request),
				      _("_Yes"), GTK_RESPONSE_YES);
	if (info->cancel)
		gtk_dialog_add_button(GTK_DIALOG(dialog_request),
				      _("_Cancel"), GTK_RESPONSE_CANCEL);
	if (info->ok)
		gtk_dialog_add_button(GTK_DIALOG(dialog_request),
				      _("_OK"), GTK_RESPONSE_OK);
#else
	if (info->no)
		gtk_dialog_add_button(GTK_DIALOG(dialog_request),
				      GTK_STOCK_NO, GTK_RESPONSE_NO);
	if (info->yes)
		gtk_dialog_add_button(GTK_DIALOG(dialog_request),
				      GTK_STOCK_YES, GTK_RESPONSE_YES);
	if (info->cancel)
		gtk_dialog_add_button(GTK_DIALOG(dialog_request),
				      GTK_STOCK_CANCEL,
				      GTK_RESPONSE_CANCEL);
	if (info->ok)
		gtk_dialog_add_button(GTK_DIALOG(dialog_request),
				      GTK_STOCK_OK, GTK_RESPONSE_OK);
#endif
	g_signal_connect((gpointer)dialog_request, "response",
			 G_CALLBACK(on_dialog_response), info);

	return dialog_request;
}

/******************************************************************************
 * Name
 *   gui_new_dialog
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   GS_DIALOG *gui_new_dialog(void)
 *
 * Description
 *
 *
 * Return value
 *   GS_DIALOG *
 */

GS_DIALOG *gui_new_dialog(void)
{
	standard_info = g_new0(GS_DIALOG, 1);

	/* set entrys to null */
	entry1 = NULL;
	entry2 = NULL;
	entry3 = NULL;
	entry4 = NULL;
	entry5 = NULL;
	entry6 = NULL;

	return standard_info;
}

/******************************************************************************
 * Name
 *   gui_generic_warning_modal
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   GS_DIALOG *gui_generic_warning_modal(char *message)
 *
 * Description
 *   Issues a generic warning dialog box, to keep user informed.
 *   This version is "old style," waits for user to hit OK.
 *   For tragic conditions, e.g. startup problems before exit.
 *
 * Return value
 *   void
 */

void gui_generic_warning_modal(const char *message)
{
	GS_DIALOG *dialog;
	gchar *dialog_text;

	dialog = gui_new_dialog();
	dialog->stock_icon =
#if GTK_CHECK_VERSION(3, 10, 0)
	    "dialog-information";
#else
	    GTK_STOCK_DIALOG_INFO;
#endif

	dialog_text = g_strdup_printf("<span weight=\"bold\">%s</span>",
				      _("Xiphos:"));
	dialog->label_top = dialog_text;
	dialog->label2 = (char *)message;
	dialog->ok = TRUE;

	gui_alert_dialog(dialog);
	g_free(dialog);
	g_free(dialog_text);
}

/******************************************************************************
 * Name
 *   gui_generic_warning
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   GS_DIALOG *gui_generic_warning(char *message)
 *
 * Description
 *   Issues a generic warning dialog box, to keep user informed.
 *   Non-modal, for transient things not needing user interaction.
 *
 * Return value
 *   void
 */

void gui_generic_warning(const char *message)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new_with_markup(NULL, /* no need for a parent window */
						    GTK_DIALOG_DESTROY_WITH_PARENT,
						    GTK_MESSAGE_INFO,
						    GTK_BUTTONS_OK,
						    "<b><big>Xiphos</big></b>\n\n%s",
						    message);

	g_signal_connect_swapped(dialog, "response",
				 G_CALLBACK(gtk_widget_destroy), dialog);
	gtk_widget_show(dialog);
}

/******************************************************************************
 * Name
 *   gui_gs_dialog
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   gint gui_gs_dialog(GS_DIALOG * info)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

gint gui_gs_dialog(GS_DIALOG *info)
{
	static gboolean is_running = FALSE;

	if (!is_running) {
		GtkWidget *dialog = create_dialog_request(info);
		retval = 4;
		is_running = TRUE;
		gtk_dialog_run((GtkDialog *)dialog);
		is_running = FALSE;
		gtk_widget_destroy(dialog);
		return retval;
	}
	return 4;
}

/******************************************************************************
 * Name
 *   gui_alert_dialog
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   gint gui_alert_dialog(GS_DIALOG * info)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

gint gui_alert_dialog(GS_DIALOG *info)
{
	static gboolean is_running = FALSE;

	if (!is_running) {
		GtkWidget *dialog = create_dialog_alert(info); //gs_dialog_build(info);
		retval = 4;
		is_running = TRUE;
		gtk_dialog_run((GtkDialog *)dialog);
		is_running = FALSE;
		gtk_widget_destroy(dialog);
		return retval;
	}
	return 4;
}

/******************************************************************************
 * Name
 *   gui_close_confirmation_dialog
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   gint gui_close_confirmation_dialog(GS_DIALOG * info)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

gint gui_close_confirmation_dialog(GS_DIALOG *info)
{
	static gboolean is_running = FALSE;

	if (!is_running) {
		GtkWidget *dialog = create_dialog_alert(info);
		gtk_dialog_add_button(GTK_DIALOG(dialog),
				      _("Close _without Saving"),
				      GTK_RESPONSE_NO);
#if GTK_CHECK_VERSION(3, 10, 0)
		gtk_dialog_add_buttons(GTK_DIALOG(dialog),
				       "_Cancel", GTK_RESPONSE_CANCEL,
				       "document-save", GTK_RESPONSE_YES,
				       NULL);
#else
		gtk_dialog_add_buttons(GTK_DIALOG(dialog),
				       GTK_STOCK_CANCEL,
				       GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE,
				       GTK_RESPONSE_YES, NULL);
#endif
		retval = 4;
		is_running = TRUE;
		gtk_dialog_run((GtkDialog *)dialog);
		is_running = FALSE;
		gtk_widget_destroy(dialog);
		return retval;
	}
	return 4;
}

/******************************************************************************
 * Name
 *   gui_yes_no_dialog
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   gint gui_yes_no_dialog(char *question)
 *
 * Description
 *   ask a simple synchronous yes/no question.
 *
 * Return value
 *   gint
 */

gint gui_yes_no_dialog(char *question, char *icon)
{
	GS_DIALOG *yes_no;
	gint result;

	yes_no = gui_new_dialog();
#if GTK_CHECK_VERSION(3, 10, 0)
	yes_no->stock_icon = (icon ? icon : "dialog-question");
#else
	yes_no->stock_icon = (icon ? icon : GTK_STOCK_DIALOG_QUESTION);
#endif
	yes_no->label_top = question;
	yes_no->yes = TRUE;
	yes_no->no = TRUE;

	result = gui_alert_dialog(yes_no);
	g_free(yes_no);
	return result == GS_YES;
}
