/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_getinfo_dlg.c
     * -------------------
     * Wed Feb 20 15:17:34 2002
     * copyright (C) 2002 by Terry Biggs
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


#include "gs_getinfo_dlg.h"
#include "support.h"



static void on_btnGTOK_clicked(GtkButton * button, GETINFNO *data)
{
	GtkWidget 
		*dlg, 
		*entry0, 
		*entry1, 
		*entry2;
	
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	
	entry0 = lookup_widget(dlg, "entryGT1");
	
	/*** freed by calling function ***/
	data->text[0] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry0)));
	
	if(data->howmany == 2 || data->howmany == 3){
		entry1 = lookup_widget(dlg, "entryGT2");
		data->text[1] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));		
	}
	else
		data->text[1] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry0)));
	if(data->howmany == 3){
		entry2 = lookup_widget(dlg, "entryGT3");
		data->text[2] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
	}
	else
		data->text[2] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry0)));
	
	gtk_widget_destroy(dlg);

}


static void on_btnGTApply_clicked(GtkButton * button, GETINFNO *data)
{
	GtkWidget 
		*dlg, 
		*entry0, 
		*entry1, 
		*entry2;
	
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	
	entry0 = lookup_widget(dlg, "entryGT1");
	entry1 = lookup_widget(dlg, "entryGT2");
	entry2 = lookup_widget(dlg, "entryGT3");
	
	/*** freed by calling function ***/
	data->text[0] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry0)));
	
	if(data->howmany == 2 || data->howmany == 3)
		data->text[1] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
	else
		data->text[1] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry0)));
	if(data->howmany == 3)
		data->text[2] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
	else
		data->text[2] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry0)));

}


static void on_btnGTCancel_clicked(GtkButton * button, GETINFNO *data)
{

}



GtkWidget *create_dlgGetTextInfo(GETINFNO *info)
{
	
	GtkWidget *dlgGetTextInfo;
	GtkWidget *dialog_vbox20;
	GtkWidget *vbox50;
	GtkWidget *lbGT0;
	GtkWidget *tableGT1;
	GtkWidget *lbGT1;
	GtkWidget *entryGT1;
	GtkWidget *dialog_action_area20;
	GtkWidget *btnGTOK;
	GtkWidget *btnGTApply;
	GtkWidget *btnGTCancel;
	
	if(!info->howmany)
		return NULL;
	
	
	
	dlgGetTextInfo = gnome_dialog_new(_("GnomeSWORD"), NULL);
	gtk_object_set_data(GTK_OBJECT(dlgGetTextInfo), "dlgGetTextInfo",
			    dlgGetTextInfo);
	gtk_window_set_policy(GTK_WINDOW(dlgGetTextInfo), FALSE, FALSE,
			      FALSE);
	gtk_window_set_position (GTK_WINDOW (dlgGetTextInfo), GTK_WIN_POS_CENTER);
	
	dialog_vbox20 = GNOME_DIALOG(dlgGetTextInfo)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgGetTextInfo), "dialog_vbox20",
			    dialog_vbox20);
	gtk_widget_show(dialog_vbox20);

	vbox50 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox50);
	gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "vbox50",
				 vbox50,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox50);
	gtk_box_pack_start(GTK_BOX(dialog_vbox20), vbox50, TRUE, TRUE, 0);

	lbGT0 = gtk_label_new(info->labelText[0]);
	gtk_widget_ref(lbGT0);
	gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "lbGT0",
				 lbGT0,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbGT0);
	gtk_box_pack_start(GTK_BOX(vbox50), lbGT0, FALSE, FALSE, 0);

	tableGT1 = gtk_table_new(1, 2, FALSE);
	gtk_widget_ref(tableGT1);
	gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "tableGT1",
				 tableGT1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tableGT1);
	gtk_box_pack_start(GTK_BOX(vbox50), tableGT1, TRUE, TRUE, 0);

	lbGT1 = gtk_label_new(info->labelText[1]);
	gtk_widget_ref(lbGT1);
	gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "lbGT1",
				 lbGT1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbGT1);
	gtk_table_attach(GTK_TABLE(tableGT1), lbGT1, 0, 1, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(lbGT1, 90, -2);
	gtk_label_set_justify(GTK_LABEL(lbGT1), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(lbGT1), 0, 0.5);

	entryGT1 = gtk_entry_new();
	gtk_widget_ref(entryGT1);
	gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "entryGT1",
				 entryGT1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryGT1);
	gtk_entry_set_text(GTK_ENTRY(entryGT1),info->text[0]);
	gtk_table_attach(GTK_TABLE(tableGT1), entryGT1, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
			 
	if (info->howmany == 2 || info->howmany == 3) {
		GtkWidget *tableGT2;
		GtkWidget *lbGT2;
		GtkWidget *entryGT2;
		
		tableGT2 = gtk_table_new(1, 2, FALSE);
		gtk_widget_ref(tableGT2);
		gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "tableGT2",
					 tableGT2,
					 (GtkDestroyNotify) gtk_widget_unref);		
		gtk_widget_show(tableGT2);
		gtk_box_pack_start(GTK_BOX(vbox50), tableGT2, TRUE, TRUE, 0);
	
		lbGT2 = gtk_label_new(info->labelText[2]);
		gtk_widget_ref(lbGT2);
		gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "lbGT2",
					 lbGT2,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(lbGT2);
		gtk_table_attach(GTK_TABLE(tableGT2), lbGT2, 0, 1, 0, 1,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_widget_set_usize(lbGT2, 90, -2);
		gtk_label_set_justify(GTK_LABEL(lbGT2), GTK_JUSTIFY_LEFT);
		gtk_misc_set_alignment(GTK_MISC(lbGT2), 0, 0.5);
	
		entryGT2 = gtk_entry_new();
		gtk_widget_ref(entryGT2);
		gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "entryGT2",
					 entryGT2,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(entryGT2);
		gtk_entry_set_text(GTK_ENTRY(entryGT2),info->text[1]);
		gtk_table_attach(GTK_TABLE(tableGT2), entryGT2, 1, 2, 0, 1,
				 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
	}	
	
	if (info->howmany == 3) {		
		GtkWidget *tableGT3;
		GtkWidget *lbGT3;
		GtkWidget *entryGT3;
		tableGT3 = gtk_table_new(1, 2, FALSE);
		gtk_widget_ref(tableGT3);
		gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo),
					 "tableGT3", tableGT3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tableGT3);
		gtk_box_pack_start(GTK_BOX(vbox50), tableGT3, TRUE, TRUE,
				   0);

		lbGT3 = gtk_label_new(info->labelText[3]);
		gtk_widget_ref(lbGT3);
		gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo),
					 "lbGT3", lbGT3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(lbGT3);
		gtk_table_attach(GTK_TABLE(tableGT3), lbGT3, 0, 1, 0, 1,
				 (GtkAttachOptions) (GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
		gtk_widget_set_usize(lbGT3, 90, -2);
		gtk_label_set_justify(GTK_LABEL(lbGT3), GTK_JUSTIFY_LEFT);
		gtk_misc_set_alignment(GTK_MISC(lbGT3), 0, 0.5);

		entryGT3 = gtk_entry_new();
		gtk_widget_ref(entryGT3);
		gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo),
					 "entryGT3", entryGT3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(entryGT3);
		gtk_entry_set_text(GTK_ENTRY(entryGT3),info->text[2]);
		gtk_table_attach(GTK_TABLE(tableGT3), entryGT3, 1, 2, 0, 1,
				 (GtkAttachOptions) (GTK_EXPAND |
						     GTK_FILL),
				 (GtkAttachOptions) (0), 0, 0);
	}
	
	dialog_action_area20 = GNOME_DIALOG(dlgGetTextInfo)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgGetTextInfo),
			    "dialog_action_area20", dialog_action_area20);
	gtk_widget_show(dialog_action_area20);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area20),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgGetTextInfo),
				   GNOME_STOCK_BUTTON_OK);
	btnGTOK =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgGetTextInfo)->buttons)->
		       data);
	gtk_widget_ref(btnGTOK);
	gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "btnGTOK",
				 btnGTOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnGTOK);
	GTK_WIDGET_SET_FLAGS(btnGTOK, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgGetTextInfo),
				   GNOME_STOCK_BUTTON_APPLY);
	btnGTApply =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgGetTextInfo)->buttons)->
		       data);
	gtk_widget_ref(btnGTApply);
	gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "btnGTApply",
				 btnGTApply,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnGTApply);
	GTK_WIDGET_SET_FLAGS(btnGTApply, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgGetTextInfo),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnGTCancel =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgGetTextInfo)->buttons)->
		       data);
	gtk_widget_ref(btnGTCancel);
	gtk_object_set_data_full(GTK_OBJECT(dlgGetTextInfo), "btnGTCancel",
				 btnGTCancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnGTCancel);
	GTK_WIDGET_SET_FLAGS(btnGTCancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(btnGTOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnGTOK_clicked), 
			   info);
	gtk_signal_connect(GTK_OBJECT(btnGTApply), "clicked",
			   GTK_SIGNAL_FUNC(on_btnGTApply_clicked),
			   info);
	gtk_signal_connect(GTK_OBJECT(btnGTCancel), "clicked",
			   GTK_SIGNAL_FUNC(on_btnGTCancel_clicked),
			   info);
	gtk_widget_show(dlgGetTextInfo);
	return dlgGetTextInfo;
}
