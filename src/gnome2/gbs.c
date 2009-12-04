/*
 * Xiphos Bible Study Tool
 * gbs.c - generic book support - the gui
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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

#ifdef USE_GTKMOZEMBED
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "gecko/gecko-html.h"
#endif
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

#ifdef USE_GTKHTML3_14_23
#include "editor/slib-editor.h"
#else
#include "editor/bonobo-editor.h"
#endif

#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/gbs.h"
#include "gui/gbs_dialog.h"
#include "gui/xiphos.h"
#include "gui/cipher_key_dialog.h"
#include "gui/find_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/bookmarks.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"
#include "gui/navbar_book.h"
#include "gui/tabbed_browser.h"
#include "gui/menu_popup.h"

#include "main/settings.h"
#include "main/lists.h"
#include "main/global_ops.hh"
#include "main/sword.h"
#include "main/xml.h"
#include "main/display.hh"
#include "main/url.hh"

#include "gui/debug_glib_null.h"

//static void create_menu(GdkEventButton * event);


/******************************************************************************
 * Name
 *  on_comm_button_press_event
 *
 * Synopsis
 *   #include ".h"
 *
 *  gboolean on_comm_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */
#ifndef USE_GTKMOZEMBED	
static gboolean on_book_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	if (!settings.havebook)
		return FALSE;	
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		gui_menu_popup (settings.book_mod, NULL);
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  gboolean on_button_release_event(GtkWidget * widget,
			    GdkEventButton * event, DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

extern gboolean in_url;

static gboolean on_book_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
#ifdef GTKHTML
	gchar *key;
	const gchar *url;
#endif /*GTKHTML */
	
	if (!settings.havebook)
		return FALSE;
	settings.whichwindow = BOOK_WINDOW;
	
#ifdef GTKHTML
	switch (event->button) {
	case 1:
		if (in_url) 
			break;
		key = gui_button_press_lookup(widgets.html_book);
		if (key) {
			if (g_strstr_len(key,strlen(key),"*")) {
				key = g_strdelimit(key, "*", ' ');
				key = g_strstrip(key);
				url = g_strdup_printf(
					"xiphos.url?action=showModInfo&value=1&module=%s",
					key);
				main_url_handler(url,TRUE);
				g_free((gchar*)url);
				g_free(key);
				break;
			}
			gchar *dict = NULL;
			if (settings.useDefaultDict)
				dict =
				    g_strdup(settings.
					     DefaultDict);
			else
				dict = g_strdup(settings.DictWindowModule);
			
			main_display_dictionary(dict, key);
			g_free(key);
			if (dict)
				g_free(dict);
		}
		break;
	case 2:
		if (!in_url)
			break;
		url = gtk_html_get_url_at (GTK_HTML(widgets.html_text),		
								event->x,
								event->y);
		if (url && (strstr(url,"sword://"))) {
			gchar **work_buf = g_strsplit (url,"/",4);
			gui_open_passage_in_new_tab(work_buf[3]);
			g_strfreev(work_buf);
		}
		break;
	case 3:
		break;
	}
#endif
	return FALSE;
}
#endif /* !USE_GTKMOZEMBED */

/******************************************************************************
 * Name
 *  gui_set_gbs_frame_label
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_set_gbs_frame_label(void)	
 *
 * Description
 *   sets gbs label to module name
 *
 * Return value
 *   void
 */
#if 0
static void set_gbs_label(gchar * mod_name)
{
	/*
	 * set label to module name
	 */
	gtk_label_set_text (GTK_LABEL(widgets.label_comm),mod_name);
}
#endif /* 0 */

/******************************************************************************
 * Name
 *  gui_set_book_page_and_key
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_set_book_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_set_book_mod_and_key(gchar * mod_name, gchar * key)
{
	main_display_book(mod_name, key);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include ".h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_update_gbs_global_ops(gchar * option, gboolean choice)
{
	/*save_module_options(cur_t->mod_name, option, 
				    choice);*/
	//gbs_display(cur_g, tree_level);
}


#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb (GeckoHtml *html,
			     gchar *uri,
			     gpointer user_data)
{	
	gui_menu_popup (settings.book_mod, NULL);
}
#endif

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
#ifdef USE_GTKMOZEMBED	
	GtkWidget *eventbox;
#else
	GtkWidget *scrolledwindow;
#endif /* USE_GTKMOZEMBED */	
	GtkWidget *navbar;
	
	box = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(box);

	navbar = gui_navbar_book_new();
	gtk_box_pack_start(GTK_BOX(box), navbar, FALSE, FALSE, 0);

#ifdef USE_GTKMOZEMBED	
	eventbox = gtk_event_box_new ();
	gtk_widget_show (eventbox);
	gtk_box_pack_start(GTK_BOX(box), eventbox, TRUE, TRUE, 0);
	widgets.html_book = GTK_WIDGET(gecko_html_new(NULL, FALSE, BOOK_TYPE)); //embed_new(BOOK_TYPE);
	gtk_widget_show(widgets.html_book);
	gtk_container_add(GTK_CONTAINER(eventbox),
			 widgets.html_book);
	
	g_signal_connect((gpointer)widgets.html_book,
		      "popupmenu_requested",
		      G_CALLBACK (_popupmenu_requested_cb),
		      NULL);
#else
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(box), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
/*	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow,
                                             settings.shadow_type);*/

	widgets.html_book = gtk_html_new();
	gtk_widget_show(widgets.html_book);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_book);

	g_signal_connect(GTK_OBJECT(widgets.html_book), "link_clicked",
				   G_CALLBACK(gui_link_clicked),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_book), "on_url",
				   G_CALLBACK(gui_url),
				   GINT_TO_POINTER(BOOK_TYPE));
	g_signal_connect(GTK_OBJECT(widgets.html_book),
				   "button_press_event",
				   G_CALLBACK
				   (on_book_button_press_event),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_book),
				   "button_release_event",
				   G_CALLBACK
				   (on_book_button_release_event),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_book),
			   "url_requested",
			   G_CALLBACK(url_requested), NULL);
#endif
	return box;
}



//******  end of file  ******/
