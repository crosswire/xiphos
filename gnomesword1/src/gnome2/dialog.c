/*
 * GnomeSword Bible Study Tool
 * dialog.c - 
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
#include <config.h>
#endif



#include <gnome.h>
#include "gui/dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"

static GtkWidget *gs_dialog;
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
 *   on_dialog1_response
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   void on_dialog1_response(GtkDialog * dialog, gint response_id,
 *							GS_DIALOG * info)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_dialog1_response(GtkDialog * dialog, gint response_id,
				GS_DIALOG * info)
{
	g_warning("%d", response_id);
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


GtkWidget *create_dialog_alert(GS_DIALOG * info)
{
	GtkWidget *dialog_alert;
	GtkWidget *dialog_vbox2;
	GtkWidget *hbox3;
	GtkWidget *image5;
	GtkWidget *vbox2;
	GtkWidget *label7;
	GtkWidget *dialog_action_area2;
	GtkWidget *cancelbutton1;
	GtkWidget *okbutton2;

	dialog_alert = gtk_dialog_new();
	gtk_container_set_border_width(GTK_CONTAINER(dialog_alert), 5);
	gtk_window_set_title(GTK_WINDOW(dialog_alert), _(" "));
	gtk_window_set_resizable(GTK_WINDOW(dialog_alert), FALSE);
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

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox2);
	gtk_box_pack_start(GTK_BOX(hbox3), vbox2, TRUE, TRUE, 0);

	label7 = gtk_label_new(info->label_top);
	gtk_widget_show(label7);
	gtk_box_pack_start(GTK_BOX(vbox2), label7, FALSE, FALSE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label7), TRUE);
	gtk_label_set_justify(GTK_LABEL(label7), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label7), TRUE);
	gtk_misc_set_alignment(GTK_MISC(label7), 0.5, 0);

	dialog_action_area2 = GTK_DIALOG(dialog_alert)->action_area;
	gtk_widget_show(dialog_action_area2);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area2),
				  GTK_BUTTONBOX_END);

	if (info->no)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_NO, GTK_RESPONSE_NO);
	if (info->yes)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_YES, GTK_RESPONSE_YES);

	if (info->cancel)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_CANCEL,
				      GTK_RESPONSE_CANCEL);
	if (info->ok)
		gtk_dialog_add_button(GTK_DIALOG(dialog_alert),
				      GTK_STOCK_OK, GTK_RESPONSE_OK);

	g_signal_connect((gpointer) dialog_alert, "response",
			 G_CALLBACK(on_dialog1_response), NULL);

	return dialog_alert;
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
	GtkWidget *dialog_action_area21;
	GtkWidget *hbuttonbox1;
	GtkWidget *button_ok;
	GtkWidget *button_cancel;
	GtkWidget *button_yes;
	GtkWidget *button_no;
	GString *str;


	gs_dialog = gtk_dialog_new();
	info->dialog = gs_dialog;
	gtk_object_set_data(GTK_OBJECT(gs_dialog), "gs_dialog",
			    gs_dialog);
	gtk_window_set_title(GTK_WINDOW(gs_dialog),
			     info->title ? info->title : " ");
	gtk_window_set_policy(GTK_WINDOW(gs_dialog), TRUE, TRUE, FALSE);
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
		/*image = gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
		   "/gs2-48x48.png"); */
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
		//str = g_string_new("");
		//g_string_printf(str,"<span weight=\"bold\">%s</span>",info->label_top);
		label_top = gtk_label_new(NULL);
		gtk_label_set_markup((GtkLabel *) label_top,
				     info->label_top);
		gtk_widget_show(label_top);
		gtk_box_pack_start(GTK_BOX(vbox84), label_top, TRUE,
				   TRUE, 0);
		//g_string_free(str,TRUE);
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
	/*
	   label5 = gtk_label_new(_("label5"));
	   gtk_widget_show(label5);
	   gtk_table_attach(GTK_TABLE(table14), label5, 0, 1, 4, 5,
	   (GtkAttachOptions) (GTK_FILL),
	   (GtkAttachOptions) (0), 0, 0);
	   gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

	   entry5 = gtk_entry_new();
	   gtk_widget_show(entry5);
	   gtk_table_attach(GTK_TABLE(table14), entry5, 1, 2, 4, 5,
	   (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
	   (GtkAttachOptions) (0), 0, 0);

	   label6 = gtk_label_new(_("label6"));
	   gtk_widget_show(label6);
	   gtk_table_attach(GTK_TABLE(table14), label6, 0, 1, 5, 6,
	   (GtkAttachOptions) (GTK_FILL),
	   (GtkAttachOptions) (0), 0, 0);
	   gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

	   entry6 = gtk_entry_new();
	   gtk_widget_show(entry6);
	   gtk_table_attach(GTK_TABLE(table14), entry6, 1, 2, 5, 6,
	   (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
	   (GtkAttachOptions) (0), 0, 0);

	 */
	g_signal_connect((gpointer) gs_dialog, "response",
			 G_CALLBACK(on_dialog1_response), info);

	gtk_widget_show_all(gs_dialog);
	return gs_dialog;
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
 *   void
 */

gint gui_gs_dialog(GS_DIALOG * info)
{
	GtkWidget *dialog;
	static gboolean is_running = FALSE;

	if (!is_running) {
		dialog = gs_dialog_build(info);
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
 *   void
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
