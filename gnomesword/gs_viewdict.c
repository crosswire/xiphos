/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_viewdict.c
                             -------------------
    begin                : Tue Dec 05 2000
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
 

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>

#include "gs_viewdict.h"
#include "gs_sword.h"

/****************************************************************************************
 *globals
 ****************************************************************************************/
GtkWidget *clKeys;  
GtkWidget *textSDmodule;
GtkWidget *cbSDMods;
GtkWidget *ceSDMods;
GtkWidget *frameShowDict;
GList *dictList;

gboolean isrunningSD = FALSE;    /* is the view dictionary dialog runing */

/****************************************************************************************
 *externs
 ****************************************************************************************/
extern GtkWidget *MainFrm;

/****************************************************************************************
 *callbacks
 ****************************************************************************************/
/*
 *
 */
void
on_dlgShowDict_destroy                 (GtkObject       *object,
                                        gpointer         user_data)
{
     isrunningSD = FALSE;
     shutdownSDSWORD();
}

/*
 *
 */
void
on_ceSDMods_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
        gchar *buf;
        static gboolean firsttime = TRUE;         	
	
	if(!firsttime)
	{
                buf = gtk_entry_get_text(GTK_ENTRY(editable));
                loadSDmodSWORD(clKeys,buf);
                gtk_frame_set_label(frameShowDict,buf);  /* set frame label to current Module name  */
        }
        firsttime = FALSE;
}


void
on_entrySDLookup_changed               (GtkEditable     *editable,
                                        gpointer         user_data)
{       gchar *buf;

        buf = gtk_entry_get_text(GTK_ENTRY(editable));
        SDdictSearchTextChangedSWORD(buf);
}


gboolean
on_entrySDLookup_key_release_event     (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_btnSDClose_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
     gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


void
on_clKeys_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
        gchar *buf;

        gtk_clist_get_text(clist, row, 0, &buf);
        gotokeySWORD(buf);
}


/****************************************************************************************
 *initSD
 *modName - name of module to show in dialog
 *
 ****************************************************************************************/
void initSD(gchar *modName)
{
	
	loadSDmodSWORD( clKeys, modName);
	
	
}

/****************************************************************************************
 *create_dlgShowDict - create the interface
 *
 ****************************************************************************************/
GtkWidget*
create_dlgShowDict (void)
{
  GtkWidget *dlgShowDict;
  GtkWidget *vbox31;
  GtkWidget *toolbar27;
  GtkWidget *label126;
  GtkWidget *entrySDLookup;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *btnSDClose;
  GtkWidget *hbox26;
  GtkWidget *scrolledwindow37;
  GtkWidget *hpaned2;
  GtkWidget *label125;
  GtkWidget *scrolledwindow36;
  GtkWidget *textSDmodule;

  dlgShowDict = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (dlgShowDict), "dlgShowDict", dlgShowDict);
  gtk_container_set_border_width (GTK_CONTAINER (dlgShowDict), 4);
  gtk_window_set_title (GTK_WINDOW (dlgShowDict), "GnomeSword - Show Dictionary");
  gtk_window_set_default_size (GTK_WINDOW (dlgShowDict), 631, 200);
  gtk_window_set_policy (GTK_WINDOW (dlgShowDict), TRUE, TRUE, FALSE);

  vbox31 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox31);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "vbox31", vbox31,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox31);
  gtk_container_add (GTK_CONTAINER (dlgShowDict), vbox31);

  toolbar27 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar27);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "toolbar27", toolbar27,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar27);
  gtk_box_pack_start (GTK_BOX (vbox31), toolbar27, FALSE, FALSE, 0);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar27), GTK_RELIEF_NONE);

  cbSDMods = gtk_combo_new ();
  gtk_widget_ref (cbSDMods);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "cbSDMods", cbSDMods,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbSDMods);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar27), cbSDMods, NULL, NULL);

  ceSDMods = GTK_COMBO (cbSDMods)->entry;
  gtk_widget_ref (ceSDMods);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "ceSDMods", ceSDMods,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ceSDMods);
  gtk_widget_set_usize (ceSDMods, 158, -2);

  label126 = gtk_label_new ("Lookup: ");
  gtk_widget_ref (label126);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "label126", label126,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label126);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar27), label126, NULL, NULL);
  gtk_widget_set_usize (label126, 53, -2);

  entrySDLookup = gtk_entry_new ();
  gtk_widget_ref (entrySDLookup);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "entrySDLookup", entrySDLookup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entrySDLookup);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar27), entrySDLookup, NULL, NULL);
  gtk_widget_set_usize (entrySDLookup, 125, -2);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgShowDict, GNOME_STOCK_PIXMAP_CLOSE);
  btnSDClose = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar27),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Close",
                                "Close Dictionary Dialog", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnSDClose);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "btnSDClose", btnSDClose,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnSDClose);

  frameShowDict = gtk_frame_new ("Dictionary");
  gtk_widget_ref (frameShowDict);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "frameShowDict", frameShowDict,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frameShowDict);
  gtk_box_pack_start (GTK_BOX (vbox31), frameShowDict, TRUE, TRUE, 0);

  hbox26 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox26);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "hbox26", hbox26,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox26);
  gtk_container_add (GTK_CONTAINER (frameShowDict), hbox26);

  hpaned2 = gtk_hpaned_new ();
  gtk_widget_ref (hpaned2);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "hpaned2", hpaned2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hpaned2);
  gtk_box_pack_start (GTK_BOX (hbox26), hpaned2, TRUE, TRUE, 0);
  gtk_paned_set_handle_size (GTK_PANED (hpaned2), 6);
  gtk_paned_set_position (GTK_PANED (hpaned2), 151);

  scrolledwindow37 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow37);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "scrolledwindow37", scrolledwindow37,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow37);
  gtk_paned_pack1 (GTK_PANED (hpaned2), scrolledwindow37, FALSE, TRUE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow37), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  clKeys = gtk_clist_new (1);
  gtk_widget_ref (clKeys);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "clKeys", clKeys,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (clKeys);
  gtk_container_add (GTK_CONTAINER (scrolledwindow37), clKeys);
  gtk_clist_set_column_width (GTK_CLIST (clKeys), 0, 80);
  gtk_clist_column_titles_hide (GTK_CLIST (clKeys));

  label125 = gtk_label_new ("label125");
  gtk_widget_ref (label125);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "label125", label125,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label125);
  gtk_clist_set_column_widget (GTK_CLIST (clKeys), 0, label125);

  scrolledwindow36 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow36);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "scrolledwindow36", scrolledwindow36,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow36);
  gtk_paned_pack2 (GTK_PANED (hpaned2), scrolledwindow36, TRUE, TRUE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow36), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  textSDmodule = gtk_text_new (NULL, NULL);
  gtk_widget_ref (textSDmodule);
  gtk_object_set_data_full (GTK_OBJECT (dlgShowDict), "textSDmodule", textSDmodule,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (textSDmodule);
  gtk_container_add (GTK_CONTAINER (scrolledwindow36), textSDmodule);

  gtk_signal_connect (GTK_OBJECT (dlgShowDict), "destroy",
                      GTK_SIGNAL_FUNC (on_dlgShowDict_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (ceSDMods), "changed",
                      GTK_SIGNAL_FUNC (on_ceSDMods_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entrySDLookup), "changed",
                      GTK_SIGNAL_FUNC (on_entrySDLookup_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entrySDLookup), "key_release_event",
                      GTK_SIGNAL_FUNC (on_entrySDLookup_key_release_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnSDClose), "clicked",
                      GTK_SIGNAL_FUNC (on_btnSDClose_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (clKeys), "select_row",
                      GTK_SIGNAL_FUNC (on_clKeys_select_row),
                      NULL);

  dictList = NULL;
  dictList = setupSDSWORD(textSDmodule);  
  gtk_combo_set_popdown_strings(GTK_COMBO(cbSDMods), dictList);
  g_list_free(dictList);
  isrunningSD = TRUE;
  return dlgShowDict;
}