/*
 * Xiphos Bible Study Tool
 * bibletext.c - gui for Bible text modules
 *
 * Copyright (C) 2000-2010 Xiphos Developer Team
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

#include "xiphos_html.h"


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
GtkAdjustment* adjustment;


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
			NULL);*/
}


#ifdef USE_XIPHOS_HTML
  #ifdef USE_WEBKIT
    static gboolean
    _popupmenu_requested_cb (XiphosHtml *html,
			    gchar *uri ,
			     gpointer user_data)
  #elif USE_GTKMOZEMBED
    static void
    _popupmenu_requested_cb (XiphosHtml *html,
			     gchar *uri,
			     gpointer user_data)

  #endif
{
    gui_menu_popup (html, settings.MainWindowModule,
			NULL);  
		g_print ("in _popupmenu_requested_cb\n");
    #ifdef USE_WEBKIT
      return TRUE;
   // #elif USE_GTKMOZEMBED
      //gui_popup_pm_text();
    #endif
}
#endif

#ifndef USE_XIPHOS_HTML
/******************************************************************************
 * Name
 *  on_text_button_press_event
 *
 * Synopsis
 *   #include ".h"
 *
 *  gboolean on_text_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */
static gboolean on_text_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	switch (event->button) {
	case 1:
		break;
	case 2:
		shift_key_pressed = TRUE;
		break;
	case 3:
		gui_popup_pm_text();
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

static gboolean on_text_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer date)
{

//#ifdef GTKHTML
	gchar *key;
	const gchar *url;

	settings.whichwindow = MAIN_TEXT_WINDOW;
	/*
	 * set program title to current text module name
	 */
	gui_change_window_title(settings.MainWindowModule);
	switch (event->button) {
	case 1:
		if (!in_url) {
			key = gui_button_press_lookup(widgets.html_text);
			if (key) {
				gchar *dict = NULL;
				if (settings.useDefaultDict)
					dict =
					    g_strdup(settings.
						     DefaultDict);
				else
					dict =
					    g_strdup(settings.DictWindowModule);
				main_display_dictionary(dict, key);
				g_free(key);
				if (dict)
					g_free(dict);
			}
		}
		break;
	case 2:
		if (shift_key_pressed) {
			shift_key_pressed = FALSE;
			break;
		}
		if (!in_url)
			break;

		url = gtk_html_get_url_at (GTK_HTML(widgets.html_text),
								event->x,
								event->y);

		if (strstr(url,"sword://")) {
			gchar **work_buf = g_strsplit (url,"/",4);
			gui_open_passage_in_new_tab(work_buf[3]);
			g_strfreev(work_buf);
		}
		break;
	case 3:
		break;
	}
//#endif /* GTKHTML */
	return FALSE;
}

static gboolean
on_enter_notify_event        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
	//shift_key_pressed = FALSE;
	//gtk_widget_grab_focus (widgets.html_text);
	settings.whichwindow = MAIN_TEXT_WINDOW;
	gui_change_window_title(settings.MainWindowModule);
  	return FALSE;
}
/*
static gboolean on_key_press_event           (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
	if (event->state == GDK_CONTROL_MASK)
		g_message("GDK_CONTROL_MASK");
	if (event->state == GDK_SHIFT_MASK)
		g_message("GDK_SHIFT_MASK");
	if (event->state == GDK_MOD1_MASK)
		g_message("GDK_MOD1_MASK");
	if (event->state == GDK_MODIFIER_MASK)
		g_message("GDK_MODIFIER_MASK");
	g_message("state: %d",event->state);
#endif
	switch (event->hardware_keycode) {
		case 50:
		case 62:
			shift_key_pressed = TRUE;
  			return TRUE;
		break;
	}
	GS_message(("on_key_press_event\nkeycode: %d",event->hardware_keycode));
  	return FALSE;
}


static gboolean on_key_release_event         (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
	if (event->state == GDK_CONTROL_MASK)
		g_message("GDK_CONTROL_MASK");
	if (event->state == GDK_SHIFT_MASK)
		g_message("GDK_SHIFT_MASK");
	if (event->state == GDK_MOD1_MASK)
		g_message("GDK_MOD1_MASK");
#endif
	switch (event->hardware_keycode) {
		case 50:
		case 62:
			shift_key_pressed = FALSE;
		break;
	}
	GS_message(("on_key_release_event\nkeycode: %d",event->hardware_keycode));
  	return FALSE;
}
*/

extern NAVBAR_VERSEKEY navbar_versekey;

static
void adj_changed(GtkAdjustment * adjustment1, gpointer user_data)

{
	static int scroll = 1;
	if (!settings.chapter_scroll) return;
	if (scroll && (adjustment1->value <= adjustment1->lower)) {
		GS_message(("\ntop: %g\n",adjustment1->value));
		main_navbar_versekey_spin_chapter(navbar_versekey,0);
		scroll = 0;
	} else if (scroll && (adjustment1->value >= (adjustment1->upper - adjustment1->page_size))) {
		GS_message(("\nvalue + page_size: %g\n",adjustment1->value + adjustment1->page_size));
		main_navbar_versekey_spin_chapter(navbar_versekey,1);
		scroll = 0;
		gtk_adjustment_set_value(adjustment,2);
	} else 	scroll = 1;
}

#endif /* !USE_XIPHOS_HTML */

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
#ifdef USE_XIPHOS_HTML
	GtkWidget *eventbox1;
#else
	GtkWidget *scrolledwindow;
#endif

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);

#ifdef USE_XIPHOS_HTML

	eventbox1 = gtk_event_box_new();
	gtk_widget_show(eventbox1);
	gtk_box_pack_start(GTK_BOX(vbox),
	                   eventbox1, TRUE,
	                   TRUE, 0);
	widgets.html_text = GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, TEXT_TYPE));
	gtk_widget_show(widgets.html_text);
	gtk_container_add(GTK_CONTAINER(eventbox1),
	                  widgets.html_text);
	                  
	g_signal_connect((gpointer)widgets.html_text,
	              "popupmenu_requested",
	              G_CALLBACK (_popupmenu_requested_cb),
	              NULL);
#else

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox),
			   scrolledwindow,
	                   TRUE,
			   TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);


	adjustment = gtk_scrolled_window_get_vadjustment
					     (GTK_SCROLLED_WINDOW(scrolledwindow));
	scroll_adj_signal = g_signal_connect(GTK_OBJECT(adjustment), "value-changed",
				G_CALLBACK(adj_changed),
				NULL);
	widgets.html_text = gtk_html_new();
	gtk_widget_show(widgets.html_text);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_text);

	g_signal_connect(GTK_OBJECT(widgets.html_text), "link_clicked",
				G_CALLBACK(gui_link_clicked),
				NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_text), "on_url",
				G_CALLBACK(gui_url),
				GINT_TO_POINTER(TEXT_TYPE));
	g_signal_connect(GTK_OBJECT(widgets.html_text),"button_release_event",
				G_CALLBACK(on_text_button_release_event),
				NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_text), "button_press_event",
				G_CALLBACK(on_text_button_press_event),
				NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_text), "enter_notify_event",
				G_CALLBACK (on_enter_notify_event),
				NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_text),
			 "url_requested",
			 G_CALLBACK(url_requested), NULL);
#endif /* USE_XIPHOS_HTML */

	return 	vbox;

}
