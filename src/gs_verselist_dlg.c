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
#include "gs_sword.h"
#include "gs_html.h"

GtkWidget *htmlVL;
gboolean isrunningVL=FALSE;

static void
on_linkVL_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	changeVerseSWORD((gchar*)url);
}

static void on_dglVerseList_destroy(GtkObject * object, gpointer user_data)
{
	isrunningVL = FALSE;
}

static void on_btnVLok_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

GtkWidget *create_dglVerseList(void)
{
	GtkWidget *dglVerseList;
	GtkWidget *vbox33;
	GtkWidget *scrolledwindow39;
	GtkWidget *btnVLok;

	dglVerseList = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_object_set_data(GTK_OBJECT(dglVerseList), "dglVerseList",
			    dglVerseList);
	gtk_window_set_title(GTK_WINDOW(dglVerseList), _("VerseList"));
	gtk_window_set_default_size(GTK_WINDOW(dglVerseList), 180, 165);

	vbox33 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox33);
	gtk_object_set_data_full(GTK_OBJECT(dglVerseList), "vbox33",
				 vbox33,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox33);
	gtk_container_add(GTK_CONTAINER(dglVerseList), vbox33);

	scrolledwindow39 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow39);
	gtk_object_set_data_full(GTK_OBJECT(dglVerseList),
				 "scrolledwindow39", scrolledwindow39,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow39);
	gtk_box_pack_start(GTK_BOX(vbox33), scrolledwindow39, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow39),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	htmlVL = gtk_html_new();
	gtk_widget_ref(htmlVL);
	gtk_object_set_data_full(GTK_OBJECT(dglVerseList),
				 "htmlVL", htmlVL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlVL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow39), htmlVL);
	gtk_html_load_empty(GTK_HTML(htmlVL));

	btnVLok = gnome_stock_button(GNOME_STOCK_BUTTON_OK);
	gtk_widget_ref(btnVLok);
	gtk_object_set_data_full(GTK_OBJECT(dglVerseList), "btnVLok",
				 btnVLok,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnVLok);
	gtk_box_pack_start(GTK_BOX(vbox33), btnVLok, FALSE, FALSE, 0);
	
	gtk_signal_connect(GTK_OBJECT(dglVerseList), "destroy",
			   GTK_SIGNAL_FUNC(on_dglVerseList_destroy), NULL);
	gtk_signal_connect(GTK_OBJECT(btnVLok), "clicked",
			   GTK_SIGNAL_FUNC(on_btnVLok_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(htmlVL), "link_clicked",
			   GTK_SIGNAL_FUNC(on_linkVL_clicked), NULL);
	return dglVerseList;
}
