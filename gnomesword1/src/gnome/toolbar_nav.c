/*
 * GnomeSword Bible Study Tool
 * toolbar_nav.c - Bible text navigation toolbar
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
#include <gal/e-paned/e-hpaned.h>

/* frontend */
#include "toolbar_nav.h"

/* main */ 
#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_popup_cb.h"
#include "gs_history.h"
#include "settings.h"
#include "lists.h"

extern gboolean ApplyChange;


static void on_cbeBook_changed(GtkEditable * editable, gpointer user_data)
{
	if (ApplyChange) {
		gchar buf[256];
		gchar *bookname =
		    gtk_entry_get_text(GTK_ENTRY(settings.cbeBook));
		sprintf(buf, "%s 1:1", bookname);
		change_verse(buf);
	}
}

static gboolean
on_spbChapter_button_release_event(GtkWidget * widget,
				   GdkEventButton * event,
				   gpointer user_data)
{
	if (ApplyChange) {
		gchar *bookname;
		gchar buf[256];
		gint chapter, verse;

		bookname =
		    gtk_entry_get_text(GTK_ENTRY(settings.cbeBook));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (settings.
						      spbChapter));
		verse =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (settings.
						      spbVerse));
		sprintf(buf, "%s %d:%d", bookname, chapter, verse);
		change_verse(buf);
		return TRUE;
	}
	return FALSE;
}

static gboolean
on_spbVerse_button_release_event(GtkWidget * widget,
				 GdkEventButton * event,
				 gpointer user_data)
{
	if (ApplyChange) {
		gchar *bookname, buf[256];
		gint chapter, verse;

		bookname =
		    gtk_entry_get_text(GTK_ENTRY(settings.cbeBook));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (settings.
						      spbChapter));
		verse =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (settings.
						      spbVerse));
		sprintf(buf, "%s %d:%d", bookname, chapter, verse);
		change_verse(buf);
		return TRUE;
	}
	return FALSE;
}

static void on_btnLookup_clicked(GtkButton * button, gpointer user_data)
{
	gchar *buf;

	buf = gtk_entry_get_text(GTK_ENTRY(settings.cbeFreeformLookup));
	change_verse(buf);	//-- change verse to entry text 
}

static gboolean
on_cbeFreeformLookup_key_press_event(GtkWidget * widget,
				     GdkEventKey * event,
				     gpointer user_data)
{
	gchar *buf;

	buf = gtk_entry_get_text(GTK_ENTRY(widget));
	/* if <enter> key */
	if (event->keyval == 65293 || event->keyval == 65421) {	
		change_verse(buf);
		return TRUE;
	}
	return FALSE;
}

static void on_btnBack_clicked(GtkButton * button, gpointer user_data)
{
	historynav(settings.app, 0);
}

static void on_btnFoward_clicked(GtkButton * button, gpointer user_data)
{
	historynav(settings.app, 1);
}


GtkWidget *gui_create_nav_toolbar(void)
{

	GtkWidget *handleboxNavBar;
	GtkWidget *toolbarNav;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *cbBook;
	GtkObject *spbChapter_adj;
	GtkObject *spbVerse_adj;
	GtkWidget *btnLookup;
	GtkWidget *btnBack;
	GtkWidget *btnFoward;

	handleboxNavBar = gtk_handle_box_new();
	gtk_widget_ref(handleboxNavBar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "handleboxNavBar", handleboxNavBar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handleboxNavBar);

	toolbarNav =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarNav);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "toolbarNav",
				 toolbarNav,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarNav);
	gtk_container_add(GTK_CONTAINER(handleboxNavBar), toolbarNav);
	gtk_widget_set_usize(toolbarNav, -2, 34);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarNav),
				      GTK_RELIEF_NONE);

	cbBook = gtk_combo_new();
	gtk_widget_ref(cbBook);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "cbBook",
				 cbBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbBook);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), cbBook, NULL,
				  NULL);

	/*
	 * get and load books of the Bible 
	 */
	gtk_combo_set_popdown_strings(GTK_COMBO(cbBook),
				      get_list(BOOKS_LIST));

	settings.cbeBook = GTK_COMBO(cbBook)->entry;
	gtk_widget_ref(settings.cbeBook);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "settings.cbeBook", settings.cbeBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.cbeBook);
	gtk_entry_set_text(GTK_ENTRY(settings.cbeBook), _("Romans"));

	spbChapter_adj = gtk_adjustment_new(8, -1, 151, 1, 10, 10);
	settings.spbChapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbChapter_adj), 1, 0);
	gtk_widget_ref(settings.spbChapter);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "settings.spbChapter",
				 settings.spbChapter,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.spbChapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav),
				  settings.spbChapter, NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON
				    (settings.spbChapter), TRUE);

	spbVerse_adj = gtk_adjustment_new(28, -1, 180, 1, 10, 10);
	settings.spbVerse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVerse_adj), 1, 0);
	gtk_widget_ref(settings.spbVerse);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "settings.spbVerse", settings.spbVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.spbVerse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav),
				  settings.spbVerse, NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(settings.spbVerse),
				    TRUE);

	settings.cbeFreeformLookup = gtk_entry_new();
	gtk_widget_ref(settings.cbeFreeformLookup);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "settings.cbeFreeformLookup",
				 settings.cbeFreeformLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.cbeFreeformLookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav),
				  settings.cbeFreeformLookup, NULL,
				  NULL);
	gtk_entry_set_text(GTK_ENTRY(settings.cbeFreeformLookup),
			   _("Romans 8:28"));

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	btnLookup =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
		GTK_TOOLBAR_CHILD_BUTTON, NULL,
		_("Goto verse"),
		_("Go to verse in free form lookup and add verse to history"),
		NULL, tmp_toolbar_icon, NULL,
		NULL);
	gtk_widget_ref(btnLookup);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnLookup",
				 btnLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLookup);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_BACK);
	btnBack =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Back"),
				       _
				       ("Go backward through history list"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnBack);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnBack",
				 btnBack,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnBack);
	gtk_widget_set_sensitive(btnBack, FALSE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_FORWARD);
	btnFoward =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Foward"),
				       _
				       ("Go foward through history list"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnFoward);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnFoward",
				 btnFoward,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnFoward);
	gtk_widget_set_sensitive(btnFoward, FALSE);

	gtk_signal_connect(GTK_OBJECT(settings.cbeBook), "changed",
			   GTK_SIGNAL_FUNC(on_cbeBook_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(settings.spbChapter),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_spbChapter_button_release_event), NULL);
	gtk_signal_connect(GTK_OBJECT(settings.spbVerse),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_spbVerse_button_release_event), NULL);
	gtk_signal_connect(GTK_OBJECT(settings.cbeFreeformLookup),
			   "key_press_event",
			   GTK_SIGNAL_FUNC
			   (on_cbeFreeformLookup_key_press_event),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnLookup), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLookup_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnBack), "clicked",
			   GTK_SIGNAL_FUNC(on_btnBack_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnFoward), "clicked",
			   GTK_SIGNAL_FUNC(on_btnFoward_clicked), NULL);

	return handleboxNavBar;
}
