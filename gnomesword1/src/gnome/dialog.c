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

static GtkWidget *gs_dialog;
static GtkWidget *entry1 = NULL;
static GtkWidget *entry2 = NULL;
static GtkWidget *entry3 = NULL;
static GtkWidget *entry4 = NULL;
static GtkWidget *entry5 = NULL;
static GtkWidget *entry6 = NULL;
static gint retval = 3;

/*
gnome_dialog_new();

gnome_dialog_run_and_close 
*/

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
	if(entry1){
		if(info->text1) g_free(info->text1);
		info->text1 = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
	}
	if(entry2){
		if(info->text2) g_free(info->text2);
		info->text2 = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
	}
	if(entry3){
		if(info->text3) g_free(info->text3);
		info->text3 = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry3)));
	}
	if(entry4){
		if(info->text4) g_free(info->text4);
		info->text4 = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry4)));
	}
	if(entry5){
		if(info->text5) g_free(info->text5);
		info->text5 = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry5)));
	}
	if(entry6){
		if(info->text6) g_free(info->text6);
		info->text6 = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry6)));
	}
	
}

/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   void dialog_destroy(GtkObject * object, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_destroy(GtkObject * object, gpointer user_data)
{
	gtk_main_quit();
}


/******************************************************************************
 * Name
 *   on_button_ok_clicked
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   void on_button_ok_clicked(GtkButton * button, GS_DIALOG * info)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_button_ok_clicked(GtkButton * button, GS_DIALOG * info)
{
	retval = GS_OK;
	get_entry_text(info);
	gtk_widget_destroy(gs_dialog);
}


/******************************************************************************
 * Name
 *   on_button_cancel_clicked
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   void on_button_cancel_clicked(GtkButton * button,
				     GS_DIALOG * info)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_button_cancel_clicked(GtkButton * button,
				     GS_DIALOG * info)
{
	retval = GS_CANCEL;
	gtk_widget_destroy(gs_dialog);
}


/******************************************************************************
 * Name
 *   on_button_yes_clicked
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   void on_button_yes_clicked(GtkButton * button,
 *				  GS_DIALOG * info)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_button_yes_clicked(GtkButton * button,
				  GS_DIALOG * info)
{
	retval = GS_YES;
	get_entry_text(info);
	gtk_widget_destroy(gs_dialog);
}


/******************************************************************************
 * Name
 *   on_button_no_clicked
 *
 * Synopsis
 *   #include "gui/dialog.h"
 *
 *   void on_button_no_clicked(GtkButton * button, GS_DIALOG * info)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_button_no_clicked(GtkButton * button, GS_DIALOG * info)
{
	retval = GS_NO;
	gtk_widget_destroy(gs_dialog);
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

	GtkWidget *dialog_vbox21;
	GtkWidget *vbox83;
	GtkWidget *hbox83;
	GtkWidget *pixmap;
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

	gs_dialog = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(gs_dialog), "gs_dialog",
			    gs_dialog);
	gtk_window_set_title(GTK_WINDOW(gs_dialog), _("GnomeSWORD"));
	gtk_window_set_policy(GTK_WINDOW(gs_dialog), TRUE, TRUE, FALSE);

	info->dialog = gs_dialog;
	
	dialog_vbox21 = GTK_DIALOG(gs_dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(gs_dialog), "dialog_vbox21",
			    dialog_vbox21);
	gtk_widget_show(dialog_vbox21);

	vbox83 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox83);
	gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "vbox83",
				 vbox83,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox83);
	gtk_box_pack_start(GTK_BOX(dialog_vbox21), vbox83, TRUE, TRUE,
			   0);


	hbox83 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox83);
	gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "hbox83",
				 hbox83,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox83);
	gtk_box_pack_start(GTK_BOX(vbox83), hbox83, TRUE, TRUE, 0);

	pixmap = gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					    "/GnomeSword.xpm");
	gtk_widget_ref(pixmap);
	gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "pixmap",
				 pixmap,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap);
	gtk_box_pack_start(GTK_BOX(hbox83), pixmap, TRUE, TRUE, 0);

	vbox84 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox84);
	gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "vbox84",
				 vbox84,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox84);
	gtk_box_pack_start(GTK_BOX(hbox83), vbox84, TRUE, TRUE, 0);

	if (info->label_top) {
		label_top = gtk_label_new(info->label_top);
		gtk_widget_ref(label_top);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "label_top", label_top,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(label_top);
		gtk_box_pack_start(GTK_BOX(vbox84), label_top, TRUE,
				   TRUE, 0);
	}

	if (info->label_middle) {
		label_middle = gtk_label_new(info->label_middle);
		gtk_widget_ref(label_middle);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "label_middle", label_middle,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(label_middle);
		gtk_box_pack_start(GTK_BOX(vbox84), label_middle, TRUE,
				   TRUE, 0);
	}

	table14 = gtk_table_new(6, 2, FALSE);
	gtk_widget_ref(table14);
	gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "table14",
				 table14,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table14);
	gtk_box_pack_start(GTK_BOX(vbox84), table14, TRUE, TRUE, 0);

	if (info->label1) {
		label1 = gtk_label_new(info->label1);
		gtk_widget_ref(label1);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "label1", label1,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(label1);
		gtk_table_attach(GTK_TABLE(table14), label1, 0, 1, 0, 1,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

		entry1 = gtk_entry_new();
		gtk_widget_ref(entry1);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "entry1", entry1,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(entry1);
		gtk_table_attach(GTK_TABLE(table14), entry1, 1, 2, 0, 1,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if(info->text1)
			gtk_entry_set_text(GTK_ENTRY(entry1),info->text1);
	}

	if (info->label2) {
		label2 = gtk_label_new(info->label2);
		gtk_widget_ref(label2);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "label2", label2,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(label2);
		gtk_table_attach(GTK_TABLE(table14), label2, 0, 1, 1, 2,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

		entry2 = gtk_entry_new();
		gtk_widget_ref(entry2);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "entry2", entry2,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(entry2);
		gtk_table_attach(GTK_TABLE(table14), entry2, 1, 2, 1, 2,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if(info->text2)
			gtk_entry_set_text(GTK_ENTRY(entry2),info->text2);
	}

	if (info->label3) {
		label3 = gtk_label_new(info->label3);
		gtk_widget_ref(label3);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "label3", label3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(label3);
		gtk_table_attach(GTK_TABLE(table14), label3, 0, 1, 2, 3,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);


		entry3 = gtk_entry_new();
		gtk_widget_ref(entry3);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "entry3", entry3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(entry3);
		gtk_table_attach(GTK_TABLE(table14), entry3, 1, 2, 2, 3,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if(info->text3)
			gtk_entry_set_text(GTK_ENTRY(entry3),info->text3);
	}

	if (info->label4) {
		label4 = gtk_label_new(info->label4);
		gtk_widget_ref(label4);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "label4", label4,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(label4);
		gtk_table_attach(GTK_TABLE(table14), label4, 0, 1, 3, 4,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

		entry4 = gtk_entry_new();
		gtk_widget_ref(entry4);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "entry4", entry4,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(entry4);
		gtk_table_attach(GTK_TABLE(table14), entry4, 1, 2, 3, 4,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		if(info->text4)
			gtk_entry_set_text(GTK_ENTRY(entry4),info->text4);
	}
	/*
	   label5 = gtk_label_new(_("label5"));
	   gtk_widget_ref(label5);
	   gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "label5",
	   label5,
	   (GtkDestroyNotify) gtk_widget_unref);
	   gtk_widget_show(label5);
	   gtk_table_attach(GTK_TABLE(table14), label5, 0, 1, 4, 5,
	   (GtkAttachOptions) (GTK_FILL),
	   (GtkAttachOptions) (0), 0, 0);
	   gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);
	
	   entry5 = gtk_entry_new();
	   gtk_widget_ref(entry5);
	   gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "entry5",
	   entry5,
	   (GtkDestroyNotify) gtk_widget_unref);
	   gtk_widget_show(entry5);
	   gtk_table_attach(GTK_TABLE(table14), entry5, 1, 2, 4, 5,
	   (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
	   (GtkAttachOptions) (0), 0, 0);

	   label6 = gtk_label_new(_("label6"));
	   gtk_widget_ref(label6);
	   gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "label6",
	   label6,
	   (GtkDestroyNotify) gtk_widget_unref);
	   gtk_widget_show(label6);
	   gtk_table_attach(GTK_TABLE(table14), label6, 0, 1, 5, 6,
	   (GtkAttachOptions) (GTK_FILL),
	   (GtkAttachOptions) (0), 0, 0);
	   gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

	   entry6 = gtk_entry_new();
	   gtk_widget_ref(entry6);
	   gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "entry6",
	   entry6,
	   (GtkDestroyNotify) gtk_widget_unref);
	   gtk_widget_show(entry6);
	   gtk_table_attach(GTK_TABLE(table14), entry6, 1, 2, 5, 6,
	   (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
	   (GtkAttachOptions) (0), 0, 0);

	 */
	if (info->label_bottom) {
		label_bottom = gtk_label_new(info->label_bottom);
		gtk_widget_ref(label_bottom);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "label_bottom", label_bottom,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(label_bottom);
		gtk_box_pack_start(GTK_BOX(vbox84), label_bottom, TRUE,
				   TRUE, 0);
	}



	dialog_action_area21 = GTK_DIALOG(gs_dialog)->action_area;
	gtk_object_set_data(GTK_OBJECT(gs_dialog),
			    "dialog_action_area21",
			    dialog_action_area21);
	gtk_widget_show(dialog_action_area21);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area21), 10);

	hbuttonbox1 = gtk_hbutton_box_new();
	gtk_widget_ref(hbuttonbox1);
	gtk_object_set_data_full(GTK_OBJECT(gs_dialog), "hbuttonbox1",
				 hbuttonbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbuttonbox1);
	gtk_box_pack_start(GTK_BOX(dialog_action_area21), hbuttonbox1,
			   TRUE, TRUE, 0);

	if (info->ok) {
		button_ok = gnome_stock_button(GNOME_STOCK_BUTTON_OK);
		gtk_widget_ref(button_ok);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "button_ok", button_ok,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(button_ok);
		gtk_container_add(GTK_CONTAINER(hbuttonbox1),
				  button_ok);
		GTK_WIDGET_SET_FLAGS(button_ok, GTK_CAN_DEFAULT);

		gtk_signal_connect(GTK_OBJECT(button_ok), "clicked",
				   GTK_SIGNAL_FUNC
				   (on_button_ok_clicked),
				   info);
	}

	if (info->cancel) {
		button_cancel =
		    gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
		gtk_widget_ref(button_cancel);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "button_cancel", button_cancel,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(button_cancel);
		gtk_container_add(GTK_CONTAINER(hbuttonbox1),
				  button_cancel);
		GTK_WIDGET_SET_FLAGS(button_cancel, GTK_CAN_DEFAULT);

		gtk_signal_connect(GTK_OBJECT(button_cancel), "clicked",
				   GTK_SIGNAL_FUNC
				   (on_button_cancel_clicked), 
				info);

	}

	if (info->yes) {
		button_yes = gnome_stock_button(GNOME_STOCK_BUTTON_YES);
		gtk_widget_ref(button_yes);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "button_yes", button_yes,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(button_yes);
		gtk_container_add(GTK_CONTAINER(hbuttonbox1),
				  button_yes);
		GTK_WIDGET_SET_FLAGS(button_yes, GTK_CAN_DEFAULT);

		gtk_signal_connect(GTK_OBJECT(button_yes), "clicked",
				   GTK_SIGNAL_FUNC
				   (on_button_yes_clicked), 
				info);

	}

	if (info->no) {
		button_no = gnome_stock_button(GNOME_STOCK_BUTTON_NO);
		gtk_widget_ref(button_no);
		gtk_object_set_data_full(GTK_OBJECT(gs_dialog),
					 "button_no", button_no,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(button_no);
		gtk_container_add(GTK_CONTAINER(hbuttonbox1),
				  button_no);
		GTK_WIDGET_SET_FLAGS(button_no, GTK_CAN_DEFAULT);

		gtk_signal_connect(GTK_OBJECT(button_no), "clicked",
				   GTK_SIGNAL_FUNC
				   (on_button_no_clicked), 
				info);

	}

        gtk_signal_connect (GTK_OBJECT (gs_dialog), "delete_event",
			GTK_SIGNAL_FUNC (gtk_main_quit),
			NULL);
	gtk_signal_connect(GTK_OBJECT(gs_dialog), "destroy",
			   GTK_SIGNAL_FUNC(dialog_destroy), NULL);

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
	GS_DIALOG * info;
	
	info = g_new(GS_DIALOG, 1);
	
	info->dialog = NULL;
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
	GtkWidget *window;
	static gboolean is_running = FALSE;
	
	if(!is_running) {
		window = gs_dialog_build(info);
		retval = 4;
		is_running = TRUE;
		gtk_main ();
//		g_warning("dialog closed");
		is_running = FALSE;
		return retval;
	}
	return 4;
   // gtk_window_remove_accel_group (GTK_WINDOW (window), accel_table);
}
