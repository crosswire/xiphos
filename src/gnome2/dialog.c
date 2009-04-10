/*
 * Xiphos Bible Study Tool
 * dialog.c - 
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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

#include "main/settings.h"
#include "main/sword.h"

//static GtkWidget *gs_dialog;
static GtkWidget *entry1 = NULL;
static GtkWidget *entry2 = NULL;
static GtkWidget *entry3 = NULL;
static GtkWidget *entry4 = NULL;
static GtkWidget *entry5 = NULL;
static GtkWidget *entry6 = NULL;
static gint retval = 3;



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

static void get_entry_text(GS_DIALOG * info)
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
 *   void on_dialog_response(GtkDialog * dialog, gint response_id,
 *							GS_DIALOG * info)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_dialog_response(GtkDialog * dialog, gint response_id,
			       GS_DIALOG * info)
{
	GS_warning(("%d", response_id));
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

static GtkWidget *create_dialog_alert(GS_DIALOG * info)
{
	GtkWidget *dialog_alert;
	GtkWidget *dialog_vbox2;
	GtkWidget *hbox3;
	GtkWidget *image5;
	GtkWidget *vbox2;
	GtkWidget *label7;
	GtkWidget *label10;
	GtkWidget *dialog_action_area2;
//	GtkWidget *cancelbutton1;
//	GtkWidget *okbutton2;
	GtkWidget *scrolledwindow;
	GtkWidget *viewport;

	dialog_alert = gtk_dialog_new();
	gtk_container_set_border_width(GTK_CONTAINER(dialog_alert), 5);
	gtk_window_set_title(GTK_WINDOW(dialog_alert), " ");
	//gtk_window_set_resizable(GTK_WINDOW(dialog_alert), FALSE);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog_alert), FALSE);

	dialog_vbox2 = GTK_DIALOG(dialog_alert)->vbox;
	gtk_widget_show(dialog_vbox2);

	hbox3 = gtk_hbox_new(FALSE, 12);
	gtk_widget_show(hbox3);
	gtk_box_pack_start(GTK_BOX(dialog_vbox2), hbox3, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox3), 6);

	if (info->stock_icon) {
		image5 =
		    gtk_image_new_from_stock(info->stock_icon,
					     GTK_ICON_SIZE_DIALOG);
		gtk_widget_show(image5);
		gtk_box_pack_start(GTK_BOX(hbox3), image5, FALSE, TRUE,
				   0);
		gtk_misc_set_alignment(GTK_MISC(image5), 0.5, 0);
		gtk_misc_set_padding(GTK_MISC(image5), 12, 0);
	}
	
	
	vbox2 = gtk_vbox_new(FALSE, 6);
	gtk_widget_show(vbox2);
	gtk_box_pack_start(GTK_BOX(hbox3), vbox2, TRUE, TRUE, 0);

	label7 = gtk_label_new(info->label_top);
	gtk_widget_show(label7);	
	gtk_box_pack_start(GTK_BOX(vbox2), label7, FALSE, FALSE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label7), TRUE);
	gtk_label_set_justify(GTK_LABEL(label7), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label7), TRUE);
	gtk_misc_set_alignment(GTK_MISC(label7), 0, 0.5);

	if (info->label2) {	
		gtk_window_set_default_size (GTK_WINDOW (dialog_alert), 380, 200);
		scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
		gtk_widget_show (scrolledwindow);
		gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow, TRUE, TRUE, 0);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		
		viewport = gtk_viewport_new (NULL, NULL);
		gtk_widget_show (viewport);
		gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
		
		label10 = gtk_label_new(info->label2);
		gtk_widget_show(label10);
		gtk_container_add (GTK_CONTAINER (viewport), label10);
		gtk_label_set_justify(GTK_LABEL(label10), GTK_JUSTIFY_LEFT);
		gtk_misc_set_alignment(GTK_MISC(label10), 0, 0.5);
	}
	
	dialog_action_area2 = GTK_DIALOG(dialog_alert)->action_area;
	gtk_widget_show(dialog_action_area2);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area2),
				  GTK_BUTTONBOX_END);

	if (info->ok)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_OK, GTK_RESPONSE_OK);
	if (info->yes)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_YES, GTK_RESPONSE_YES);
	if (info->save)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
                                             GTK_STOCK_SAVE,
                                             GTK_RESPONSE_YES);
	if (info->no)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_NO, GTK_RESPONSE_NO);
	if (info->cancel)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_CANCEL,
				      GTK_RESPONSE_CANCEL);
	if (info->no_save)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
                                             _("Close without Saving"),
                                             GTK_RESPONSE_NO);

	g_signal_connect((gpointer) dialog_alert, "response",
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

static GtkWidget *create_dialog_request(GS_DIALOG * info)
{
	GtkWidget *dialog_request;
	GtkWidget *dialog_vbox3;
	GtkWidget *hbox4;
	GtkWidget *image6;
	GtkWidget *vbox3;
	GtkWidget *label8;
	GtkWidget *table2;
	GtkWidget *label9;
	GtkWidget *label10;
	GtkWidget *label11;
	GtkWidget *label12;
	GtkWidget *dialog_action_area3;
//	GtkWidget *cancelbutton2;
//	GtkWidget *okbutton3;

	dialog_request = gtk_dialog_new();
	info->dialog = dialog_request;
	gtk_container_set_border_width(GTK_CONTAINER(dialog_request),
				       6);
	gtk_window_set_title(GTK_WINDOW(dialog_request), 
			     info->title ? info->title : " ");
	gtk_window_set_modal(GTK_WINDOW(dialog_request), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(dialog_request), FALSE);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog_request), FALSE);

	dialog_vbox3 = GTK_DIALOG(dialog_request)->vbox;
	gtk_widget_show(dialog_vbox3);

	hbox4 = gtk_hbox_new(FALSE, 12);
	gtk_widget_show(hbox4);
	gtk_box_pack_start(GTK_BOX(dialog_vbox3), hbox4, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox4), 6);

	if (info->stock_icon) {
		image6 =
		    gtk_image_new_from_stock(info->stock_icon,
					     GTK_ICON_SIZE_DIALOG);
		gtk_widget_show(image6);
		gtk_box_pack_start(GTK_BOX(hbox4), image6, FALSE, TRUE,
				   0);
		gtk_misc_set_alignment(GTK_MISC(image6), 0.5, 0);
		gtk_misc_set_padding(GTK_MISC(image6), 12, 0);
	}
	vbox3 = gtk_vbox_new(FALSE, 4);
	gtk_widget_show(vbox3);
	gtk_box_pack_start(GTK_BOX(hbox4), vbox3, TRUE, TRUE, 0);

	label8 = gtk_label_new(info->label_top);
	gtk_widget_show(label8);
	gtk_box_pack_start(GTK_BOX(vbox3), label8, FALSE, FALSE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label8), TRUE);
	gtk_label_set_justify(GTK_LABEL(label8), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label8), TRUE);
	gtk_misc_set_alignment(GTK_MISC(label8), 0.5, 0);

	table2 = gtk_table_new(3, 2, FALSE);
	gtk_widget_show(table2);
	gtk_box_pack_start(GTK_BOX(vbox3), table2, TRUE, TRUE, 0);
	gtk_table_set_row_spacings(GTK_TABLE(table2), 3);
	gtk_table_set_col_spacings(GTK_TABLE(table2), 3);

	if (info->label1) {
		label9 = gtk_label_new(info->label1);
		gtk_widget_show(label9);
		gtk_table_attach(GTK_TABLE(table2), label9, 0, 1, 0, 1,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_label_set_justify(GTK_LABEL(label9),
				      GTK_JUSTIFY_LEFT);
		gtk_misc_set_alignment(GTK_MISC(label9), 0, 0.5);

		entry1 = gtk_entry_new();
		gtk_widget_show(entry1);
		gtk_table_attach(GTK_TABLE(table2), entry1, 1, 2, 0, 1,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if (info->text1)
			gtk_entry_set_text(GTK_ENTRY(entry1),
					   info->text1);
	}

	if (info->label2) {
		label10 = gtk_label_new(info->label2);
		gtk_widget_show(label10);
		gtk_table_attach(GTK_TABLE(table2), label10, 0, 1, 1, 2,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_label_set_justify(GTK_LABEL(label10),
				      GTK_JUSTIFY_LEFT);
		gtk_misc_set_alignment(GTK_MISC(label10), 0, 0.5);

		entry2 = gtk_entry_new();
		gtk_widget_show(entry2);
		gtk_table_attach(GTK_TABLE(table2), entry2, 1, 2, 1, 2,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if (info->text2)
			gtk_entry_set_text(GTK_ENTRY(entry2),
					   info->text2);

	}

	if (info->label3) {
		label11 = gtk_label_new(info->label3);
		gtk_widget_show(label11);
		gtk_table_attach(GTK_TABLE(table2), label11, 0, 1, 2, 3,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_label_set_justify(GTK_LABEL(label11),
				      GTK_JUSTIFY_LEFT);
		gtk_misc_set_alignment(GTK_MISC(label11), 0, 0.5);

		entry3 = gtk_entry_new();
		gtk_widget_show(entry3);
		gtk_table_attach(GTK_TABLE(table2), entry3, 1, 2, 2, 3,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if (info->text3)
			gtk_entry_set_text(GTK_ENTRY(entry3),
					   info->text3);

	}

	if (info->label4) {
		label12 = gtk_label_new(info->label4);
		gtk_widget_show(label12);
		gtk_table_attach(GTK_TABLE(table2), label12, 0, 1, 3, 4,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_label_set_justify(GTK_LABEL(label12),
				      GTK_JUSTIFY_LEFT);
		gtk_misc_set_alignment(GTK_MISC(label12), 0, 0.5);

		entry4 = gtk_entry_new();
		gtk_widget_show(entry4);
		gtk_table_attach(GTK_TABLE(table2), entry4, 1, 2, 3, 4,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if (info->text4)
			gtk_entry_set_text(GTK_ENTRY(entry4),
					   info->text4);

	}

	dialog_action_area3 = GTK_DIALOG(dialog_request)->action_area;
	gtk_widget_show(dialog_action_area3);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area3),
				  GTK_BUTTONBOX_END);

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

	g_signal_connect((gpointer) dialog_request, "response",
			 G_CALLBACK(on_dialog_response), info);

	return dialog_request;
}



/******************************************************************************
 * Name
 *   gs_dialog_build
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   GtkWidget *gs_dialog_build(GS_DIALOG * info)
 *
 * Description
 *   creates the dialog
 *
 * Return value
 *   GtkWidget *
 */
/*
static GtkWidget *gs_dialog_build(GS_DIALOG * info)
{

	GtkWidget *dialog_vbox;
	GtkWidget *hbox83;
	GtkWidget *image;
	GtkWidget *vbox84;
	GtkWidget *label_top;
	GtkWidget *label_middle;
	GtkWidget *table14;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *label3;
	GtkWidget *label4;
	//GtkWidget *label5;
	//GtkWidget *label6;
	GtkWidget *label_bottom;
//	GtkWidget *dialog_action_area21;
//	GtkWidget *hbuttonbox1;
//	GtkWidget *button_ok;
//	GtkWidget *button_cancel;
//	GtkWidget *button_yes;
//	GtkWidget *button_no;
//	GString *str;


	gs_dialog = gtk_dialog_new();
	info->dialog = gs_dialog;
	gtk_object_set_data(GTK_OBJECT(gs_dialog), "gs_dialog",
			    gs_dialog);
	gtk_window_set_title(GTK_WINDOW(gs_dialog),
			     info->title ? info->title : " ");
	gtk_window_set_resizable(GTK_WINDOW(gs_dialog), TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(gs_dialog), 12);

	if (info->no)
		gtk_dialog_add_button(GTK_DIALOG(gs_dialog),
				      GTK_STOCK_NO, GTK_RESPONSE_NO);
	if (info->yes)
		gtk_dialog_add_button(GTK_DIALOG(gs_dialog),
				      GTK_STOCK_YES, GTK_RESPONSE_YES);

	if (info->cancel)
		gtk_dialog_add_button(GTK_DIALOG(gs_dialog),
				      GTK_STOCK_CANCEL,
				      GTK_RESPONSE_CANCEL);
	if (info->ok)
		gtk_dialog_add_button(GTK_DIALOG(gs_dialog),
				      GTK_STOCK_OK, GTK_RESPONSE_OK);

	dialog_vbox = GTK_DIALOG(gs_dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(gs_dialog), "dialog_vbox",
			    dialog_vbox);
	gtk_widget_show(dialog_vbox);

	hbox83 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox83);
	gtk_box_pack_start(GTK_BOX(dialog_vbox), hbox83, TRUE, TRUE, 0);

	if (info->stock_icon) {
		image = gtk_image_new_from_stock(info->stock_icon,
						 GTK_ICON_SIZE_DND);
		gtk_widget_show(image);
		gtk_box_pack_start(GTK_BOX(hbox83), image, FALSE, TRUE,
				   0);
		gtk_misc_set_alignment(GTK_MISC(image), 0.5, 0);
		gtk_misc_set_padding(GTK_MISC(image), 12, 0);
	}

	vbox84 = gtk_vbox_new(FALSE, 8);
	gtk_widget_show(vbox84);
	gtk_box_pack_start(GTK_BOX(hbox83), vbox84, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox84), 6);

	if (info->label_top) {
		label_top = gtk_label_new(NULL);
		gtk_label_set_markup((GtkLabel *) label_top,
				     info->label_top);
		gtk_widget_show(label_top);
		gtk_box_pack_start(GTK_BOX(vbox84), label_top, TRUE,
				   TRUE, 0);
	}

	if (info->label_middle) {
		label_middle = gtk_label_new(info->label_middle);
		gtk_widget_show(label_middle);
		gtk_box_pack_start(GTK_BOX(vbox84), label_middle, TRUE,
				   TRUE, 0);
	}

	if (info->label_bottom) {
		label_bottom = gtk_label_new(info->label_bottom);
		gtk_widget_show(label_bottom);
		gtk_box_pack_start(GTK_BOX(vbox84), label_bottom, TRUE,
				   TRUE, 0);
	}

	table14 = gtk_table_new(6, 2, FALSE);;
	gtk_widget_show(table14);
	gtk_box_pack_start(GTK_BOX(vbox84), table14, TRUE, TRUE, 0);
	gtk_table_set_row_spacings(GTK_TABLE(table14), 4);
	gtk_table_set_col_spacings(GTK_TABLE(table14), 7);

	if (info->label1) {
		label1 = gtk_label_new(info->label1);
		gtk_widget_show(label1);
		gtk_table_attach(GTK_TABLE(table14), label1, 0, 1, 0, 1,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

		entry1 = gtk_entry_new();
		gtk_widget_show(entry1);
		gtk_table_attach(GTK_TABLE(table14), entry1, 1, 2, 0, 1,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_widget_set_size_request(entry1, 250, -1);
		if (info->text1)
			gtk_entry_set_text(GTK_ENTRY(entry1),
					   info->text1);
	}

	if (info->label2) {
		label2 = gtk_label_new(info->label2);
		gtk_widget_show(label2);
		gtk_table_attach(GTK_TABLE(table14), label2, 0, 1, 1, 2,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

		entry2 = gtk_entry_new();
		gtk_widget_show(entry2);
		gtk_table_attach(GTK_TABLE(table14), entry2, 1, 2, 1, 2,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if (info->text2)
			gtk_entry_set_text(GTK_ENTRY(entry2),
					   info->text2);
	}

	if (info->label3) {
		label3 = gtk_label_new(info->label3);
		gtk_widget_show(label3);
		gtk_table_attach(GTK_TABLE(table14), label3, 0, 1, 2, 3,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);


		entry3 = gtk_entry_new();
		gtk_widget_show(entry3);
		gtk_table_attach(GTK_TABLE(table14), entry3, 1, 2, 2, 3,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if (info->text3)
			gtk_entry_set_text(GTK_ENTRY(entry3),
					   info->text3);
	}

	if (info->label4) {
		label4 = gtk_label_new(info->label4);
		gtk_widget_show(label4);
		gtk_table_attach(GTK_TABLE(table14), label4, 0, 1, 3, 4,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

		entry4 = gtk_entry_new();
		gtk_widget_show(entry4);
		gtk_table_attach(GTK_TABLE(table14), entry4, 1, 2, 3, 4,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if (info->text4)
			gtk_entry_set_text(GTK_ENTRY(entry4),
					   info->text4);
	}
	g_signal_connect((gpointer) gs_dialog, "response",
			 G_CALLBACK(on_dialog_response), info);

	gtk_widget_show_all(gs_dialog);
	return gs_dialog;
}
*/

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
	GS_DIALOG *info;

	info = g_new0(GS_DIALOG, 1);

	info->stock_icon = NULL;
	info->dialog = NULL;
	info->title = NULL;
	info->text1 = NULL;
	info->text2 = NULL;
	info->text3 = NULL;
	info->text4 = NULL;
	info->text5 = NULL;
	info->text6 = NULL;
	info->label_top = NULL;
	info->label_middle = NULL;
	info->label1 = NULL;
	info->label2 = NULL;
	info->label3 = NULL;
	info->label4 = NULL;
	info->label5 = NULL;
	info->label6 = NULL;
	info->label_bottom = NULL;
	info->ok = FALSE;
	info->cancel = FALSE;
	info->yes = FALSE;
	info->no = FALSE;
	/* set entrys to null */
	entry1 = NULL;
	entry2 = NULL;
	entry3 = NULL;
	entry4 = NULL;
	entry5 = NULL;
	entry6 = NULL;

	return info;
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
 *
 * Return value
 *   void
 */

void gui_generic_warning(char *message)
{
	GS_DIALOG *dialog;
	gchar *dialog_text;

	dialog = gui_new_dialog();
	dialog->stock_icon = GTK_STOCK_DIALOG_INFO;
	
	dialog_text = g_strdup_printf("<span weight=\"bold\">%s</span>",
				      _("Xiphos:"));
	dialog->label_top = dialog_text;
	dialog->label2 = message;
	dialog->ok = TRUE;

	gui_alert_dialog(dialog);
	g_free(dialog);
	g_free(dialog_text);
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

gint gui_gs_dialog(GS_DIALOG * info)
{
	GtkWidget *dialog;
	static gboolean is_running = FALSE;

	if (!is_running) {
		dialog = create_dialog_request(info);
		retval = 4;
		is_running = TRUE;
		gtk_dialog_run((GtkDialog *) dialog);
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

gint gui_alert_dialog(GS_DIALOG * info)
{
	GtkWidget *dialog;
	static gboolean is_running = FALSE;

	if (!is_running) {
		dialog = create_dialog_alert(info);	//gs_dialog_build(info);
		retval = 4;
		is_running = TRUE;
		gtk_dialog_run((GtkDialog *) dialog);
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

gint gui_close_confirmation_dialog(GS_DIALOG * info)
{
	GtkWidget *dialog;
	static gboolean is_running = FALSE;

	if (!is_running) {
		dialog = create_dialog_alert(info);	//gs_dialog_build(info);
		gtk_dialog_add_button (GTK_DIALOG (dialog),
				       _("Close _without Saving"),
				       GTK_RESPONSE_NO);
	
		gtk_dialog_add_buttons (GTK_DIALOG (dialog),
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE, GTK_RESPONSE_YES, NULL);
		retval = 4;
		is_running = TRUE;
		gtk_dialog_run((GtkDialog *) dialog);
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
	yes_no->stock_icon = (icon ? icon : GTK_STOCK_DIALOG_QUESTION);
	yes_no->label_top = question;
	yes_no->yes = TRUE;
	yes_no->no = TRUE;

	result = gui_alert_dialog(yes_no);
	g_free(yes_no);
	return result == GS_YES;
}
