/***************************************************************************
                                  gs_verselist_dlg.c
                             -------------------
    begin                : Mon July 16 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

#include "gs_verselist_dlg.h"
#include "sw_verselist_dlg.h"
#include "gs_html.h"

GtkWidget *htmlVL;
static GtkWidget *htmlVLshow;
gboolean isrunningVL=FALSE;

static void
on_linkVL_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	changeVerseListSWORD((gchar*)url);
}

static void on_dglVerseList_destroy(GtkObject * object, gpointer user_data)
{
	isrunningVL = FALSE;
	shutdownverselistSWORD(); 
}

static void on_btnVLok_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

GtkWidget *create_dglVerseList(void)
{
  GtkWidget *dglVerseList;
  GtkWidget *vbox33;
  GtkWidget *frame28;
  GtkWidget *scrolledwindow41;
  GtkWidget *frame29;
  GtkWidget *scrolledwindow40;
  GtkWidget *btnVLok;

  dglVerseList = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (dglVerseList), "dglVerseList", dglVerseList);
  gtk_window_set_title (GTK_WINDOW (dglVerseList), _("VerseList"));
  gtk_window_set_default_size (GTK_WINDOW (dglVerseList), 225, 299);
  gtk_window_set_policy (GTK_WINDOW (dglVerseList), TRUE, TRUE, FALSE);

  vbox33 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox33);
  gtk_object_set_data_full (GTK_OBJECT (dglVerseList), "vbox33", vbox33,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox33);
  gtk_container_add (GTK_CONTAINER (dglVerseList), vbox33);

  frame28 = gtk_frame_new (NULL);
  gtk_widget_ref (frame28);
  gtk_object_set_data_full (GTK_OBJECT (dglVerseList), "frame28", frame28,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame28);
  gtk_box_pack_start (GTK_BOX (vbox33), frame28, TRUE, TRUE, 0);

  scrolledwindow41 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow41);
  gtk_object_set_data_full (GTK_OBJECT (dglVerseList), "scrolledwindow41", scrolledwindow41,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow41);
  gtk_container_add (GTK_CONTAINER (frame28), scrolledwindow41);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow41), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);


	htmlVL = gtk_html_new();
	gtk_widget_ref(htmlVL);
	gtk_object_set_data_full(GTK_OBJECT(dglVerseList),
				 "htmlVL", htmlVL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlVL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow41), htmlVL);
	gtk_html_load_empty(GTK_HTML(htmlVL));

  frame29 = gtk_frame_new (NULL);
  gtk_widget_ref (frame29);
  gtk_object_set_data_full (GTK_OBJECT (dglVerseList), "frame29", frame29,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame29);
  gtk_box_pack_start (GTK_BOX (vbox33), frame29, TRUE, TRUE, 0);
  gtk_widget_set_usize (frame29, -2, 188);

  scrolledwindow40 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow40);
  gtk_object_set_data_full (GTK_OBJECT (dglVerseList), "scrolledwindow40", scrolledwindow40,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow40);
  gtk_container_add (GTK_CONTAINER (frame29), scrolledwindow40);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow40), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	htmlVLshow = gtk_html_new();
	gtk_widget_ref(htmlVLshow);
	gtk_object_set_data_full(GTK_OBJECT(dglVerseList),
				 "htmlVLshow", htmlVLshow,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlVLshow);
	gtk_container_add(GTK_CONTAINER(scrolledwindow40), htmlVLshow);
	gtk_html_load_empty(GTK_HTML(htmlVLshow));	
	
  btnVLok = gnome_stock_button (GNOME_STOCK_BUTTON_OK);
  gtk_widget_ref (btnVLok);
  gtk_object_set_data_full (GTK_OBJECT (dglVerseList), "btnVLok", btnVLok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVLok);
  gtk_box_pack_start (GTK_BOX (vbox33), btnVLok, FALSE, FALSE, 0);
  
  gtk_signal_connect(GTK_OBJECT(dglVerseList), "destroy",
			GTK_SIGNAL_FUNC(on_dglVerseList_destroy), 
			NULL);
  gtk_signal_connect (GTK_OBJECT (btnVLok), "clicked",
                      	GTK_SIGNAL_FUNC (on_btnVLok_clicked),
                      	NULL);
  gtk_signal_connect(GTK_OBJECT(htmlVL), "link_clicked",
			GTK_SIGNAL_FUNC(on_linkVL_clicked), 
			NULL);	
	
	setupVerseListSWORD(htmlVLshow);	
	isrunningVL = TRUE;
  return dglVerseList;
}
