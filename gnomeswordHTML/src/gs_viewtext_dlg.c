/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_viewcomm.c
                             -------------------
    begin                : Sat Mar 24 2001
    copyright            : (C) 2001 by Terry Biggs
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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

#include "gs_viewtext_dlg.h"
#include "gs_sword.h"
#include "gs_html.h"


/****************************************************************************************
 *globals
 ****************************************************************************************/
gboolean isrunningVT = FALSE;	/* is the view commentary dialog runing */
GtkWidget *text;
GList *textList;
GtkWidget *dlgViewText;

/****************************************************************************************
 * externs
 ****************************************************************************************/
extern gchar current_verse[];

/******************************************************************************
 * viewtext callbacks
 ******************************************************************************/

static void
on_cbeModule_changed                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
	gchar *buf, title[256];
	static gboolean firsttime = TRUE;

	//if (!firsttime) {
		buf = gtk_entry_get_text(GTK_ENTRY(editable));
		loadVTmodSWORD(buf);
		//buf = gtk_entry_get_text(GTK_ENTRY(GTK_WIDGET(user_data)));
		gotoverseVTSWORD(current_verse);
	//} 
	sprintf(title, "GnomeSword - %s", getVTmodDescriptionSWORD());
	gtk_window_set_title(GTK_WINDOW(dlgViewText), title);
	firsttime = FALSE;
}


static void
on_btnSync_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

}

/*
 * shut down the View Text Dialog
 */
static void
on_dlgViewText_destroy (GtkObject * object, gpointer user_data)
{
	isrunningVT = FALSE;
	shutdownVTSWORD ();
}


static void
on_btnVTAdd_clicked (GtkButton * button, gpointer user_data)
{

}


static void
on_btnVTClose_clicked (GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


/******************************************************************************
 * viewtext ui
 ******************************************************************************/

GtkWidget *
create_dlgViewText (void)
{
	
	GtkWidget *dialog_vbox14;
	GtkWidget *vbox33;
	GtkWidget *toolbar29;
	GtkWidget *combo10;
	GtkWidget *cbeModule;
	GtkWidget *btnVTAdd;
	GtkWidget *btnSync;	
	GtkWidget *toolbar30;
	GtkWidget *combo11;
	GtkWidget *combo_entry11;
	GtkWidget *combo12;
	GtkWidget *combo_entry12;
	GtkWidget *combo13;
	GtkWidget *entry2;
	GtkWidget *frame21;
	GtkWidget *swVText;
	GtkWidget *dialog_action_area14;
	GtkWidget *btnVTClose;

	dlgViewText = gnome_dialog_new (_("GnomeSword"), NULL);
	gtk_object_set_data (GTK_OBJECT (dlgViewText), "dlgViewText",
			     dlgViewText);
	gtk_window_set_default_size (GTK_WINDOW (dlgViewText), 300, 412);

	dialog_vbox14 = GNOME_DIALOG (dlgViewText)->vbox;
	gtk_object_set_data (GTK_OBJECT (dlgViewText), "dialog_vbox14",
			     dialog_vbox14);
	gtk_widget_show (dialog_vbox14);

	vbox33 = gtk_vbox_new (FALSE, 0);
	gtk_widget_ref (vbox33);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "vbox33", vbox33,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (vbox33);
	gtk_box_pack_start (GTK_BOX (dialog_vbox14), vbox33, TRUE, TRUE, 0);

	toolbar29 =
		gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL,
				 GTK_TOOLBAR_TEXT);
	gtk_widget_ref (toolbar29);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "toolbar29",
				  toolbar29,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (toolbar29);
	gtk_box_pack_start (GTK_BOX (vbox33), toolbar29, FALSE, FALSE, 0);

	combo10 = gtk_combo_new ();
	gtk_widget_ref (combo10);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo10",
				  combo10,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (combo10);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), combo10, NULL,
				   NULL);

	cbeModule = GTK_COMBO (combo10)->entry;
	gtk_widget_ref (cbeModule);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "cbeModule",
				  cbeModule,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (cbeModule);

	btnVTAdd = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL,
					       _("Add"),
					       NULL, NULL, NULL, NULL, NULL);
	gtk_widget_ref (btnVTAdd);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "btnVTAdd",
				  btnVTAdd,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (btnVTAdd);

	btnSync = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Sync"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (btnSync);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "btnSync", btnSync,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnSync);



	toolbar30 =
		gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL,
				 GTK_TOOLBAR_BOTH);
	gtk_widget_ref (toolbar30);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "toolbar30",
				  toolbar30,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (toolbar30);
	gtk_box_pack_start (GTK_BOX (vbox33), toolbar30, FALSE, FALSE, 0);

	combo11 = gtk_combo_new ();
	gtk_widget_ref (combo11);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo11",
				  combo11,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (combo11);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar30), combo11, NULL,
				   NULL);

	combo_entry11 = GTK_COMBO (combo11)->entry;
	gtk_widget_ref (combo_entry11);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo_entry11",
				  combo_entry11,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (combo_entry11);

	combo12 = gtk_combo_new ();
	gtk_widget_ref (combo12);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo12",
				  combo12,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (combo12);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar30), combo12, NULL,
				   NULL);
	gtk_widget_set_usize (combo12, 59, -2);

	combo_entry12 = GTK_COMBO (combo12)->entry;
	gtk_widget_ref (combo_entry12);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo_entry12",
				  combo_entry12,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (combo_entry12);

	combo13 = gtk_combo_new ();
	gtk_widget_ref (combo13);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo13",
				  combo13,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (combo13);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar30), combo13, NULL,
				   NULL);
	gtk_widget_set_usize (combo13, 59, -2);

	entry2 = GTK_COMBO (combo13)->entry;
	gtk_widget_ref (entry2);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "entry2", entry2,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (entry2);

	frame21 = gtk_frame_new (NULL);
	gtk_widget_ref (frame21);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "frame21",
				  frame21,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (frame21);
	gtk_box_pack_start (GTK_BOX (vbox33), frame21, TRUE, TRUE, 0);
	gtk_widget_set_usize (frame21, -2, 400);

	swVText = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_ref (swVText);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "swVText",
				  swVText,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (swVText);
	gtk_container_add (GTK_CONTAINER (frame21), swVText);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swVText),
					GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	text = gtk_html_new ();
	gtk_widget_ref (text);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "text", text,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (text);
	gtk_container_add (GTK_CONTAINER (swVText), text);

	dialog_action_area14 = GNOME_DIALOG (dlgViewText)->action_area;
	gtk_object_set_data (GTK_OBJECT (dlgViewText), "dialog_action_area14",
			     dialog_action_area14);
	gtk_widget_show (dialog_action_area14);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area14),
				   GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area14), 8);

	gnome_dialog_append_button (GNOME_DIALOG (dlgViewText),
				    GNOME_STOCK_BUTTON_CLOSE);
	btnVTClose =
		GTK_WIDGET (g_list_last
			    (GNOME_DIALOG (dlgViewText)->buttons)->data);
	gtk_widget_ref (btnVTClose);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "btnVTClose",
				  btnVTClose,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (btnVTClose);
	GTK_WIDGET_SET_FLAGS (btnVTClose, GTK_CAN_DEFAULT);

	gtk_signal_connect (GTK_OBJECT (dlgViewText), "destroy",
			    GTK_SIGNAL_FUNC (on_dlgViewText_destroy), NULL);
	gtk_signal_connect (GTK_OBJECT (btnVTAdd), "clicked",
			    GTK_SIGNAL_FUNC (on_btnVTAdd_clicked), NULL);
	gtk_signal_connect (GTK_OBJECT (btnVTClose), "clicked",
			    GTK_SIGNAL_FUNC (on_btnVTClose_clicked), NULL);
	
  gtk_signal_connect (GTK_OBJECT (cbeModule), "changed",
                      GTK_SIGNAL_FUNC (on_cbeModule_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnSync), "clicked",
                      GTK_SIGNAL_FUNC (on_btnSync_clicked),
                      NULL);
		      
		      
		      
	textList = NULL;
	textList = setupVTSWORD(text);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo10), textList);
	gtk_entry_set_text(GTK_ENTRY(cbeModule), gettextmodSWORD());
	//loadVTmodSWORD(gettextmodSWORD());
	//gtk_entry_set_text(GTK_ENTRY(entry1), current_verse);
	gotoverseVTSWORD(current_verse);
	g_list_free(textList);
	isrunningVT = TRUE;

	return dlgViewText;
}
