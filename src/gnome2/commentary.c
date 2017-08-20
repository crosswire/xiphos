/*
 * Xiphos Bible Study Tool
 * commentary.c - gui for commentary modules
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <gtk/gtk.h>

#include "xiphos_html/xiphos_html.h"

#include "gui/dialog.h"
#include "gui/commentary.h"
#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/xiphos.h"
#include "gui/cipher_key_dialog.h"
#include "gui/sidebar.h"
#include "gui/main_window.h"
#include "gui/menu_popup.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/tabbed_browser.h"
#include "gui/widgets.h"
#include "gui/utilities.h"

#ifdef USE_WEBKIT_EDITOR
#include "editor/webkit_editor.h"
#else
#include "editor/slib-editor.h"
#endif

#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/global_ops.hh"
#include "main/search_sidebar.h"
#include "main/display.hh"
#include "main/url.hh"

#include "gui/debug_glib_null.h"

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/commentary.h"
 *
 *   void access_to_edit_perscomm()
 *
 * Description
 *   kbd shortcut hook to pers.comm editor, from outside this source file.
 *   Note: NOT static.
 *
 * Return value
 *   void
 */

void access_to_edit_percomm()
{
	gchar *personal = "Personal";
	if (!main_is_module(personal))
		return;

	editor_create_new(personal, (gchar *)settings.currentverse,
			  NOTE_EDITOR);
}

static gboolean on_enter_notify_event(GtkWidget *widget,
				      GdkEventCrossing *event,
				      gpointer user_data)
{
	//shift_key_pressed = FALSE;
	//gtk_widget_grab_focus (widgets.html_comm);
	//settings.whichwindow = COMMENTARY_WINDOW;
	//gui_change_window_title(settings.CommWindowModule);
	return FALSE;
}

static void
_popupmenu_requested_cb(XiphosHtml *html, gchar *uri, gpointer user_data)
{
	//g_print ("in comm _popupmenu_requested_cb\n");
	gui_menu_popup(html, settings.CommWindowModule, NULL);
}

/******************************************************************************
 * Name
 *   gui_create_commentary_pane
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   GtkWidget *gui_create_commentary_pane(void)
 *
 * Description
 *
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_commentary_pane(void)
{
	GtkWidget *box_comm;
#ifndef USE_WEBKIT2
	GtkWidget *scrolledwindow;
#endif

	UI_VBOX(box_comm, FALSE, 0);
	gtk_widget_show(box_comm);

#ifndef USE_WEBKIT2
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(box_comm),
			   scrolledwindow, TRUE, TRUE, 0);
#endif

	widgets.html_comm =
	    GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, COMMENTARY_TYPE));
	gtk_widget_show(widgets.html_comm);
#ifdef USE_WEBKIT2
	gtk_box_pack_start(GTK_BOX(box_comm), widgets.html_comm, TRUE, TRUE, 0);
#else
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_comm);
#endif

	g_signal_connect((gpointer)widgets.html_comm,
			 "popupmenu_requested",
			 G_CALLBACK(_popupmenu_requested_cb), NULL);
#ifndef USE_WEBKIT2
	g_signal_connect((gpointer)scrolledwindow, "enter_notify_event",
			 G_CALLBACK(on_enter_notify_event), NULL);
#endif

	return box_comm;
}
