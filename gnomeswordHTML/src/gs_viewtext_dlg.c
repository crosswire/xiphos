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
#include "support.h"

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
on_tbtVTStrongs_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


static void
on_tbtnVTFootnotes_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


static void
on_cbeChapter_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


static void
on_cbeVerse_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


static void
on_cbeBook_changed                     (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


static void
on_btnGotoVerse_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

}



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
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *btnVTAdd;
  GtkWidget *btnSync;
  GtkWidget *vseparator16;
  GtkWidget *tbtVTStrongs;
  GtkWidget *tbtnVTFootnotes;
  GtkWidget *toolbar30;
  GtkWidget *combo11;
  GtkWidget *cbeBook;
  GtkWidget *combo12;
  GtkWidget *cbeChapter;
  GtkWidget *combo13;
  GtkWidget *cbeVerse;
  GtkWidget *btnGotoVerse;
  GtkWidget *frame21;
  GtkWidget *swVText;
  GtkWidget *dialog_action_area14;
  GtkWidget *btnVTClose;

  dlgViewText = gnome_dialog_new (_("GnomeSword"), NULL);
  gtk_object_set_data (GTK_OBJECT (dlgViewText), "dlgViewText", dlgViewText);
  gtk_window_set_default_size (GTK_WINDOW (dlgViewText), 300, 412);

  dialog_vbox14 = GNOME_DIALOG (dlgViewText)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlgViewText), "dialog_vbox14", dialog_vbox14);
  gtk_widget_show (dialog_vbox14);

  vbox33 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox33);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "vbox33", vbox33,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox33);
  gtk_box_pack_start (GTK_BOX (dialog_vbox14), vbox33, TRUE, TRUE, 0);

  toolbar29 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar29);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "toolbar29", toolbar29,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar29);
  gtk_box_pack_start (GTK_BOX (vbox33), toolbar29, FALSE, FALSE, 0);
  gtk_toolbar_set_space_size (GTK_TOOLBAR (toolbar29), 3);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar29), GTK_RELIEF_NONE);

  combo10 = gtk_combo_new ();
  gtk_widget_ref (combo10);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo10", combo10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo10);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), combo10, NULL, NULL);

  cbeModule = GTK_COMBO (combo10)->entry;
  gtk_widget_ref (cbeModule);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "cbeModule", cbeModule,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbeModule);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgViewText, GNOME_STOCK_PIXMAP_ADD);
  btnVTAdd = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Add"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVTAdd);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "btnVTAdd", btnVTAdd,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVTAdd);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgViewText, GNOME_STOCK_PIXMAP_REFRESH);
  btnSync = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Sync"),
                                _("Sync with main window"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnSync);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "btnSync", btnSync,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnSync);

  vseparator16 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator16);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "vseparator16", vseparator16,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator16);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), vseparator16, NULL, NULL);
  gtk_widget_set_usize (vseparator16, 7, 13);

  tmp_toolbar_icon = create_pixmap (dlgViewText, "gnomesword/strongs2.xpm", TRUE);
  tbtVTStrongs = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("Strongs"),
                                _("Toggle Strongs Numbers"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (tbtVTStrongs);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "tbtVTStrongs", tbtVTStrongs,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (tbtVTStrongs);

  tmp_toolbar_icon = create_pixmap (dlgViewText, "gnomesword/footnote3.xpm", TRUE);
  tbtnVTFootnotes = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("Footnotes"),
                                _("Toggle Footnotes"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (tbtnVTFootnotes);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "tbtnVTFootnotes", tbtnVTFootnotes,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (tbtnVTFootnotes);
  gtk_widget_set_usize (tbtnVTFootnotes, 30, 30);

  toolbar30 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar30);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "toolbar30", toolbar30,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar30);
  gtk_box_pack_start (GTK_BOX (vbox33), toolbar30, FALSE, FALSE, 0);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar30), GTK_RELIEF_NONE);

  combo11 = gtk_combo_new ();
  gtk_widget_ref (combo11);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo11", combo11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo11);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar30), combo11, NULL, NULL);

  cbeBook = GTK_COMBO (combo11)->entry;
  gtk_widget_ref (cbeBook);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "cbeBook", cbeBook,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbeBook);

  combo12 = gtk_combo_new ();
  gtk_widget_ref (combo12);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo12", combo12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo12);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar30), combo12, NULL, NULL);
  gtk_widget_set_usize (combo12, 59, -2);

  cbeChapter = GTK_COMBO (combo12)->entry;
  gtk_widget_ref (cbeChapter);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "cbeChapter", cbeChapter,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbeChapter);

  combo13 = gtk_combo_new ();
  gtk_widget_ref (combo13);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "combo13", combo13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo13);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar30), combo13, NULL, NULL);
  gtk_widget_set_usize (combo13, 59, -2);

  cbeVerse = GTK_COMBO (combo13)->entry;
  gtk_widget_ref (cbeVerse);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "cbeVerse", cbeVerse,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbeVerse);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgViewText, GNOME_STOCK_PIXMAP_JUMP_TO);
  btnGotoVerse = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar30),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button7"),
                                _("Go to verse"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnGotoVerse);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "btnGotoVerse", btnGotoVerse,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnGotoVerse);

  frame21 = gtk_frame_new (NULL);
  gtk_widget_ref (frame21);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "frame21", frame21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame21);
  gtk_box_pack_start (GTK_BOX (vbox33), frame21, TRUE, TRUE, 0);
  gtk_widget_set_usize (frame21, -2, 400);

  swVText = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (swVText);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "swVText", swVText,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (swVText);
  gtk_container_add (GTK_CONTAINER (frame21), swVText);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swVText), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	text = gtk_html_new ();
	gtk_widget_ref (text);
	gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "text", text,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (text);
	gtk_container_add (GTK_CONTAINER (swVText), text);

  dialog_action_area14 = GNOME_DIALOG (dlgViewText)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlgViewText), "dialog_action_area14", dialog_action_area14);
  gtk_widget_show (dialog_action_area14);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area14), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area14), 8);

  gnome_dialog_append_button (GNOME_DIALOG (dlgViewText), GNOME_STOCK_BUTTON_CLOSE);
  btnVTClose = GTK_WIDGET (g_list_last (GNOME_DIALOG (dlgViewText)->buttons)->data);
  gtk_widget_ref (btnVTClose);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "btnVTClose", btnVTClose,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVTClose);
  GTK_WIDGET_SET_FLAGS (btnVTClose, GTK_CAN_DEFAULT);
  
  gtk_signal_connect(GTK_OBJECT(dlgViewText), "destroy",
						   GTK_SIGNAL_FUNC
						   (on_dlgViewText_destroy),
						   NULL);

  gtk_signal_connect (GTK_OBJECT (cbeModule), "changed",
                      GTK_SIGNAL_FUNC (on_cbeModule_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnVTAdd), "clicked",
                      GTK_SIGNAL_FUNC (on_btnVTAdd_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnSync), "clicked",
                      GTK_SIGNAL_FUNC (on_btnSync_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (tbtVTStrongs), "toggled",
                      GTK_SIGNAL_FUNC (on_tbtVTStrongs_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (tbtnVTFootnotes), "toggled",
                      GTK_SIGNAL_FUNC (on_tbtnVTFootnotes_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cbeBook), "changed",
                      GTK_SIGNAL_FUNC (on_cbeBook_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cbeChapter), "changed",
                      GTK_SIGNAL_FUNC (on_cbeChapter_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cbeVerse), "changed",
                      GTK_SIGNAL_FUNC (on_cbeVerse_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnGotoVerse), "clicked",
                      GTK_SIGNAL_FUNC (on_btnGotoVerse_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnVTClose), "clicked",
                      GTK_SIGNAL_FUNC (on_btnVTClose_clicked),
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

