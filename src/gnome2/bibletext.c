/*
 * Xiphos Bible Study Tool
 * bibletext.c - gui for Bible text modules
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

#include <gtk/gtk.h>

#include "xiphos_html/xiphos_html.h"

#include "gui/xiphos.h"
#include "gui/bibletext.h"
#include "gui/bibletext_dialog.h"
#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/export_dialog.h"
#include "gui/sidebar.h"
#include "gui/cipher_key_dialog.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/menu_popup.h"
#include "gui/dialog.h"
#include "gui/font_dialog.h"
#include "gui/dictlex.h"
#include "gui/tabbed_browser.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/global_ops.hh"
#include "main/display.hh"

gboolean shift_key_pressed = FALSE;
guint scroll_adj_signal;
GtkAdjustment *adjustment;

#if 0
// unneeded at this time.  disabled to silence cppcheck.
/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_popup_pm_text(void)
{
	/* gui_menu_popup (settings.MainWindowModule,
	   NULL); */
}
#endif

static gboolean
_popupmenu_requested_cb(XiphosHtml *html, gchar *uri, gpointer user_data)
{
	gui_menu_popup(html, settings.MainWindowModule, NULL);
	return TRUE;
}

/******************************************************************************
 * Name
 *   gui_create_bible_pane
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   GtkWidget *gui_create_bible_pane(void)
 *
 * Description
 *
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_bible_pane(void)
{
	GtkWidget *vbox;
#ifndef USE_WEBKIT2
	GtkWidget *scrolledwindow;
#endif

	UI_VBOX(vbox, FALSE, 0);
	gtk_widget_show(vbox);

#ifndef USE_WEBKIT2
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);
#endif

	widgets.html_text =
	    GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, TEXT_TYPE));
	gtk_widget_show(widgets.html_text);
#ifdef USE_WEBKIT2
	gtk_box_pack_start(GTK_BOX(vbox), widgets.html_text, TRUE, TRUE, 0);
#else
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_text);
#endif

	g_signal_connect((gpointer)widgets.html_text,
			 "popupmenu_requested",
			 G_CALLBACK(_popupmenu_requested_cb), NULL);

	return vbox;
}
