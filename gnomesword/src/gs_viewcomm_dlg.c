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

#include "gs_viewcomm_dlg.h"
#include "gs_sword.h"
#include "gs_html.h"

/****************************************************************************************
 *globals
 ****************************************************************************************/
gboolean isrunningVC = FALSE;	/* is the view commentary dialog runing */
GtkWidget *text8;
GtkWidget *cbChangeMod;
GtkWidget *cbEntry;
GList *commList;
GtkWidget *dlgViewComm;

/****************************************************************************************
 * externs
 ****************************************************************************************/
extern gchar current_verse[];

/****************************************************************************************
 *callbacks
 ****************************************************************************************/
 
/*
 *
 */
void on_btnVCClose_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

/*
 * shut down the View Commentay Dialog
 */
static
void on_dlgViewComm_destroy(GtkObject * object, gpointer user_data)
{
	isrunningVC = FALSE;
	shutdownVCSWORD();
}

/*
 * bring the the View Commentay Dialog module into sync with main window
 */
void on_btnSync_clicked(GtkButton * button, gpointer user_data)
{
	gotoverseVCSWORD(current_verse);
}

/*
 * load new commentary module
 */
void on_cbEntry_changed(GtkEditable * editable, gpointer user_data)
{
	gchar *buf, title[256];
	static gboolean firsttime = TRUE;

	if (!firsttime) {
		buf = gtk_entry_get_text(GTK_ENTRY(editable));
		loadVCmodSWORD(buf);
		buf = gtk_entry_get_text(GTK_ENTRY(GTK_WIDGET(user_data)));
		gotoverseVCSWORD(buf);
	}
	sprintf(title, "GnomeSword - %s", getVCmodDescriptionSWORD());
	gtk_window_set_title(GTK_WINDOW(dlgViewComm), title);
	firsttime = FALSE;
}

/*
 * goto new versekey
 */
void on_btnGoto_clicked(GtkButton * button, gpointer user_data)
{
	gchar *buf;

	buf = gtk_entry_get_text(GTK_ENTRY(GTK_WIDGET(user_data)));
	gotoverseVCSWORD(buf);
	//g_warning(buf);
}

/*
 * goto previous module key
 */
void on_btnPrev_clicked(GtkButton * button, gpointer user_data)
{
	navVCModSWORD(0);
}

/*
 * goto next module key
 */
void on_btnNext_clicked(GtkButton * button, gpointer user_data)
{
	navVCModSWORD(1);
}

/*
 * create the View Commentary Dialog
 */
GtkWidget *create_dlgViewComm(GtkWidget *app)
{
	GtkWidget *dialog_vbox11;
	GtkWidget *vbox30;
	GtkWidget *toolbar28;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSync;
	GtkWidget *entry1;
	GtkWidget *btnGoto;
	GtkWidget *btnPrev;
	GtkWidget *btnNext;
	GtkWidget *frame19;
	GtkWidget *scrolledwindow38;
	GtkWidget *dialog_action_area11;
	GtkWidget *btnClose;

	dlgViewComm =
	    gnome_dialog_new(_("GnomeSword - View Commentary"), NULL);
	gtk_object_set_data(GTK_OBJECT(dlgViewComm), "dlgViewComm",
			    dlgViewComm);
	GTK_WINDOW(dlgViewComm)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_default_size(GTK_WINDOW(dlgViewComm), 462, 280);
	gtk_window_set_policy(GTK_WINDOW(dlgViewComm), TRUE, TRUE, FALSE);

	dialog_vbox11 = GNOME_DIALOG(dlgViewComm)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgViewComm), "dialog_vbox11",
			    dialog_vbox11);
	gtk_widget_show(dialog_vbox11);

	vbox30 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox30);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "vbox30", vbox30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox30);
	gtk_box_pack_start(GTK_BOX(dialog_vbox11), vbox30, TRUE, TRUE, 0);

	toolbar28 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar28);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "toolbar28",
				 toolbar28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar28);
	gtk_box_pack_start(GTK_BOX(vbox30), toolbar28, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar28),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(dlgViewComm,
				      GNOME_STOCK_PIXMAP_REFRESH);
	btnSync =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar28),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button7"),
				       _("Sync with main window"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSync);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "btnSync",
				 btnSync,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSync);

	cbChangeMod = gtk_combo_new();
	gtk_widget_ref(cbChangeMod);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "cbChangeMod",
				 cbChangeMod,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbChangeMod);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar28), cbChangeMod,
				  NULL, NULL);

	cbEntry = GTK_COMBO(cbChangeMod)->entry;
	gtk_widget_ref(cbEntry);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "cbEntry",
				 cbEntry,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbEntry);

	entry1 = gtk_entry_new();
	gtk_widget_ref(entry1);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "entry1", entry1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry1);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar28), entry1,
				  _("Enter a verse"), NULL);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(dlgViewComm,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	btnGoto =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar28),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button10"), _("Goto Verse"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnGoto);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "btnGoto",
				 btnGoto,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnGoto);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(dlgViewComm,
				      GNOME_STOCK_PIXMAP_DOWN);
	btnPrev =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar28),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button9"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnPrev);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "btnPrev",
				 btnPrev,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPrev);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(dlgViewComm, GNOME_STOCK_PIXMAP_UP);
	btnNext =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar28),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button8"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnNext);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "btnNext",
				 btnNext,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnNext);

	frame19 = gtk_frame_new(NULL);
	gtk_widget_ref(frame19);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "frame19",
				 frame19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame19);
	gtk_box_pack_start(GTK_BOX(vbox30), frame19, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame19), GTK_SHADOW_IN);

	scrolledwindow38 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow38);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm),
				 "scrolledwindow38", scrolledwindow38,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow38);
	gtk_container_add(GTK_CONTAINER(frame19), scrolledwindow38);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow38),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	text8 = gtk_html_new();
	gtk_widget_ref(text8);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "text8", text8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(text8);
	gtk_container_add(GTK_CONTAINER(scrolledwindow38), text8);

	dialog_action_area11 = GNOME_DIALOG(dlgViewComm)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgViewComm),
			    "dialog_action_area11", dialog_action_area11);
	gtk_widget_show(dialog_action_area11);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area11),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area11),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgViewComm),
				   GNOME_STOCK_BUTTON_CLOSE);
	btnClose = g_list_last(GNOME_DIALOG(dlgViewComm)->buttons)->data;
	gtk_widget_ref(btnClose);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewComm), "btnClose",
				 btnClose,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnClose);
	GTK_WIDGET_SET_FLAGS(btnClose, GTK_CAN_DEFAULT);
	
	gtk_signal_connect(GTK_OBJECT(dlgViewComm), "destroy",
			   GTK_SIGNAL_FUNC(on_dlgViewComm_destroy), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSync), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSync_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(cbEntry), "changed",
			   GTK_SIGNAL_FUNC(on_cbEntry_changed), (GtkWidget *)entry1);
	gtk_signal_connect(GTK_OBJECT(btnGoto), "clicked",
			   GTK_SIGNAL_FUNC(on_btnGoto_clicked), (GtkWidget *)entry1);
	gtk_signal_connect(GTK_OBJECT(btnPrev), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPrev_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnNext), "clicked",
			   GTK_SIGNAL_FUNC(on_btnNext_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnClose), "clicked",
			   GTK_SIGNAL_FUNC(on_btnVCClose_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(text8), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);			   
	gtk_signal_connect (GTK_OBJECT (text8), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);

	commList = NULL;
	commList = setupCommSWORD(text8);
	gtk_combo_set_popdown_strings(GTK_COMBO(cbChangeMod), commList);
	gtk_entry_set_text(GTK_ENTRY(cbEntry), getcommodSWORD());
	gtk_entry_set_text(GTK_ENTRY(entry1), current_verse);
	gotoverseVCSWORD(current_verse);
	g_list_free(commList);
	isrunningVC = TRUE;
	return dlgViewComm;
}
