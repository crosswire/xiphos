/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_Interlinear_dlg.c
     * -------------------
     * Thu Feb  7 21:55:14 2002 
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
#include <gtkhtml/gtkhtml.h>
#include "gs_detach_int.h"
#include "gs_gnomesword.h"
#include "sw_sword.h"

GtkWidget *Interlinear_UnDock_Dialog;
GtkWidget *vboxInt;

void
undock_interlinear_page(SETTINGS *s)
{	
	Interlinear_UnDock_Dialog = create_dlgInterlinear(s); 	
	gtk_widget_reparent(s->frameInt , vboxInt);
	gtk_notebook_remove_page(GTK_NOTEBOOK(s->workbook_lower),
                                             2);
	updateIntDlg(s);
	gtk_widget_show(Interlinear_UnDock_Dialog);
}

void
on_btnDockInt_clicked(GtkButton *button, SETTINGS *s)
{
	gtk_widget_destroy(Interlinear_UnDock_Dialog);
}


static void
on_dlgInterlinear_destroy(GtkObject *object, SETTINGS *s)
{	
	GtkWidget 
		*tab_label,
		*menu_label,
		*vbox;
	tab_label = gtk_label_new(_("Interlinear"));
	gtk_widget_show(tab_label);
	menu_label = gtk_label_new(_("Interlinear Page"));
	gtk_widget_show(menu_label);
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox",
				 vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);	
	
	gtk_notebook_insert_page_menu(GTK_NOTEBOOK(s->workbook_lower),
                                             vbox,
                                             tab_label,
                                             menu_label,
                                             2);
	gtk_widget_reparent(s->frameInt , vbox);
	s->dockedInt = TRUE;
	updateinterlinearpage();	
}
/*
static void
on_optionmenu1_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


static void
on_optionmenu2_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


static void
on_optionmenu3_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


static void
on_optionmenu4_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


static void
on_optionmenu5_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


static void
on_optionmenuGlobal_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}

*/
static void
on_buttonIntSync_clicked               (GtkButton       *button,
                                        SETTINGS *s)
{
	changeVerseSWORD(s->currentverse);
}

GtkWidget*
create_dlgInterlinear (SETTINGS *s)
{
  GtkWidget *dlgInterlinear;
  GtkWidget *dialog_vbox19;
//  GtkWidget *vseparator20;
//  GtkWidget *vseparator21;
  GtkWidget *toolbar29;
/*  GtkWidget *optionmenu1;
  GtkWidget *optionmenu1_menu;
  GtkWidget *optionmenu2;
  GtkWidget *optionmenu2_menu;
  GtkWidget *optionmenu3;
  GtkWidget *optionmenu3_menu;
  GtkWidget *optionmenu4;
  GtkWidget *optionmenu4_menu;
  GtkWidget *optionmenu5;
  GtkWidget *optionmenu5_menu;
  GtkWidget *optionmenuGlobal;
  GtkWidget *optionmenuGlobal_menu;
  GtkWidget *glade_menuitem;*/
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *buttonIntSync;
  GtkWidget *dialog_action_area19;
  GtkWidget *btnDockInt;

  dlgInterlinear = gnome_dialog_new (_("GnomeSWORD - Interlinear Page"), NULL);
  gtk_object_set_data (GTK_OBJECT (dlgInterlinear), "dlgInterlinear", dlgInterlinear);
  gtk_window_set_default_size (GTK_WINDOW (dlgInterlinear), 627, 354);
  gtk_window_set_policy (GTK_WINDOW (dlgInterlinear), TRUE, TRUE, FALSE);

  dialog_vbox19 = GNOME_DIALOG (dlgInterlinear)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlgInterlinear), "dialog_vbox19", dialog_vbox19);
  gtk_widget_show (dialog_vbox19);

  vboxInt = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vboxInt);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "vboxInt", vboxInt,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vboxInt);
  gtk_box_pack_start (GTK_BOX (dialog_vbox19), vboxInt, TRUE, TRUE, 0);

  toolbar29 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar29);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "toolbar29", toolbar29,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar29);
  gtk_box_pack_start (GTK_BOX (vboxInt), toolbar29, FALSE, FALSE, 0);
/*
  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_ref (optionmenu1);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "optionmenu1", optionmenu1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (optionmenu1);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), optionmenu1, NULL, NULL);
  optionmenu1_menu = gtk_menu_new ();
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), optionmenu1_menu);

  optionmenu2 = gtk_option_menu_new ();
  gtk_widget_ref (optionmenu2);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "optionmenu2", optionmenu2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (optionmenu2);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), optionmenu2, NULL, NULL);
  optionmenu2_menu = gtk_menu_new ();
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu2), optionmenu2_menu);

  optionmenu3 = gtk_option_menu_new ();
  gtk_widget_ref (optionmenu3);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "optionmenu3", optionmenu3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (optionmenu3);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), optionmenu3, NULL, NULL);
  optionmenu3_menu = gtk_menu_new ();
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu3), optionmenu3_menu);

  optionmenu4 = gtk_option_menu_new ();
  gtk_widget_ref (optionmenu4);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "optionmenu4", optionmenu4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (optionmenu4);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), optionmenu4, NULL, NULL);
  optionmenu4_menu = gtk_menu_new ();
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu4), optionmenu4_menu);

  optionmenu5 = gtk_option_menu_new ();
  gtk_widget_ref (optionmenu5);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "optionmenu5", optionmenu5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (optionmenu5);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), optionmenu5, NULL, NULL);
  optionmenu5_menu = gtk_menu_new ();
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu5), optionmenu5_menu);

  vseparator20 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator20);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "vseparator20", vseparator20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator20);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), vseparator20, NULL, NULL);
  gtk_widget_set_usize (vseparator20, 5, 7);

  optionmenuGlobal = gtk_option_menu_new ();
  gtk_widget_ref (optionmenuGlobal);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "optionmenuGlobal", optionmenuGlobal,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (optionmenuGlobal);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), optionmenuGlobal, NULL, NULL);
  optionmenuGlobal_menu = gtk_menu_new ();
  glade_menuitem = gtk_menu_item_new_with_label (_("Stongs"));
  gtk_widget_show (glade_menuitem);
  gtk_menu_append (GTK_MENU (optionmenuGlobal_menu), glade_menuitem);
  glade_menuitem = gtk_menu_item_new_with_label (_("Morph Tags"));
  gtk_widget_show (glade_menuitem);
  gtk_menu_append (GTK_MENU (optionmenuGlobal_menu), glade_menuitem);
  glade_menuitem = gtk_menu_item_new_with_label (_("Footnotes"));
  gtk_widget_show (glade_menuitem);
  gtk_menu_append (GTK_MENU (optionmenuGlobal_menu), glade_menuitem);
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuGlobal), optionmenuGlobal_menu);

  vseparator21 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator21);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "vseparator21", vseparator21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator21);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), vseparator21, NULL, NULL);
  gtk_widget_set_usize (vseparator21, 5, 7);
*/
  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgInterlinear, GNOME_STOCK_PIXMAP_REFRESH);
  buttonIntSync = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button6"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (buttonIntSync);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "buttonIntSync", buttonIntSync,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (buttonIntSync);

  dialog_action_area19 = GNOME_DIALOG (dlgInterlinear)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlgInterlinear), "dialog_action_area19", dialog_action_area19);
  gtk_widget_show (dialog_action_area19);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area19), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area19), 8);

  gnome_dialog_append_button_with_pixmap (GNOME_DIALOG (dlgInterlinear),
                                          _("Attach"), GNOME_STOCK_PIXMAP_HOME);
  btnDockInt = GTK_WIDGET (g_list_last (GNOME_DIALOG (dlgInterlinear)->buttons)->data);
  gtk_widget_ref (btnDockInt);
  gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "btnDockInt", btnDockInt,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnDockInt);
  GTK_WIDGET_SET_FLAGS (btnDockInt, GTK_CAN_DEFAULT);
  
  gtk_signal_connect (GTK_OBJECT (dlgInterlinear), "destroy",
                      GTK_SIGNAL_FUNC (on_dlgInterlinear_destroy),
                      s);
  gtk_signal_connect (GTK_OBJECT (btnDockInt), "clicked",
                      GTK_SIGNAL_FUNC (on_btnDockInt_clicked),
                      s);
/*
  gtk_signal_connect (GTK_OBJECT (optionmenu1), "clicked",
                      GTK_SIGNAL_FUNC (on_optionmenu1_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (optionmenu2), "clicked",
                      GTK_SIGNAL_FUNC (on_optionmenu2_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (optionmenu3), "clicked",
                      GTK_SIGNAL_FUNC (on_optionmenu3_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (optionmenu4), "clicked",
                      GTK_SIGNAL_FUNC (on_optionmenu4_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (optionmenu5), "clicked",
                      GTK_SIGNAL_FUNC (on_optionmenu5_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (optionmenuGlobal), "clicked",
                      GTK_SIGNAL_FUNC (on_optionmenuGlobal_clicked),
                      NULL);
*/
  gtk_signal_connect (GTK_OBJECT (buttonIntSync), "clicked",
                      GTK_SIGNAL_FUNC (on_buttonIntSync_clicked),
                      s);
  return dlgInterlinear;
}

