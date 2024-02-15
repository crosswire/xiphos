/*
 * Xiphos Bible Study Tool
 * gbs.c - generic book support - the gui
 *
 * Copyright (C) 2000-2024 Xiphos Developer Team
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

#include <gtk/gtk.h>

#include <gtk/gtk.h>

#include "xiphos_html/xiphos_html.h"

#include "editor/slib-editor.h"

#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/gbs.h"
#include "gui/gbs_dialog.h"
#include "gui/xiphos.h"
#include "gui/cipher_key_dialog.h"
#include "gui/find_dialog.h"
#include "gui/sidebar.h"
#include "gui/bookmarks.h"
#include "gui/main_window.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"
#include "gui/navbar_book.h"
#include "gui/tabbed_browser.h"
#include "gui/menu_popup.h"
#include "gui/utilities.h"

#include "main/settings.h"
#include "main/lists.h"
#include "main/global_ops.hh"
#include "main/sword.h"
#include "main/xml.h"
#include "main/display.hh"
#include "main/url.hh"

#include "gui/debug_glib_null.h"

static void
_popupmenu_requested_cb(XiphosHtml *html, gchar *uri, gpointer user_data)
{
	gui_menu_popup(html, settings.book_mod, NULL);
}

/******************************************************************************
 * Name
 *   gui_create_book_pane
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   GtkWidget *gui_create_book_pane(void)
 *
 * Description
 *
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_book_pane(void)
{
	GtkWidget *box;
#ifndef USE_WEBKIT2
	GtkWidget *scrolledwindow;
#endif
	GtkWidget *navbar;

	UI_VBOX(box, FALSE, 0);
	gtk_widget_show(box);

	navbar = gui_navbar_book_new();
	gtk_box_pack_start(GTK_BOX(box), navbar, FALSE, FALSE, 0);
#ifndef USE_WEBKIT2
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(box), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
#endif

	widgets.html_book =
	    GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, BOOK_TYPE));
	gtk_widget_show(widgets.html_book);
#ifdef USE_WEBKIT2
	gtk_box_pack_start(GTK_BOX(box), widgets.html_book, TRUE, TRUE, 0);
#else
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_book);
#endif

	g_signal_connect((gpointer)widgets.html_book,
			 "popupmenu_requested",
			 G_CALLBACK(_popupmenu_requested_cb), NULL);
	return box;
}

//******  end of file  ******/
