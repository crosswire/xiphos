/*
 * Xiphos Bible Study Tool
 * commentary.c - gui for commentary modules
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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
#  include <config.h>
#endif

#include <errno.h>
#include <gnome.h>


#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#include "editor/html-editor.h"
#endif

#include "gui/dialog.h"
#include "gui/commentary.h"
#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/xiphos.h"
#include "gui/cipher_key_dialog.h"
#include "gui/sidebar.h"
#include "gui/main_window.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"


#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/global_ops.hh"
#include "main/search_sidebar.h"
#include "main/display.hh"


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
	
#ifdef USE_GTKHTML38
	editor_create_new (personal,(gchar *) settings.currentverse,TRUE);
#else
	main_dialogs_open (personal, (gchar *) settings.currentverse);
#endif
}



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
static gboolean on_comm_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:		
    		gui_menu_popup (settings.CommWindowModule, NULL);
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "commentary.h"
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

static gboolean on_comm_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	extern gboolean in_url;
	gchar *key;
	const gchar *url;
	gchar *buf = NULL;
	
	settings.whichwindow = COMMENTARY_WINDOW;

#ifdef GTKHTML	
	switch (event->button) {
	case 1:
		if (in_url) 
			break;
		key = gui_button_press_lookup(widgets.html_comm);
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
				dict =
				    g_strdup(settings.
					     DictWindowModule);
			main_display_dictionary(dict, key);
			g_free(key);
			if (dict)
				g_free(dict);
		}
		break;
	case 2:
		if (!in_url)
			break;
#ifdef USE_GTKHTML38
		url = gtk_html_get_url_at (GTK_HTML(widgets.html_comm),		
								event->x,
								event->y);
#else
		url = html_engine_get_link_at (GTK_HTML(widgets.html_comm)->engine,
					 event->x,
					 event->y);
#endif
		if (url) {
			if (strstr(url,"sword://")) {
				gchar **work_buf = g_strsplit (url,"/",4);
				gui_open_passage_in_new_tab(work_buf[3]);
				g_strfreev(work_buf);
			}
		} else {
			gui_generic_warning("Middle-click invalid.");
		}
		break;
	case 3:
		break;
	}
#endif /* GTKHTML */
	return FALSE;
}


static gboolean on_enter_notify_event(GtkWidget * widget,
				      GdkEventCrossing * event,
				      gpointer user_data)
{
	//shift_key_presed = FALSE;
	//gtk_widget_grab_focus (widgets.html_comm);
	//settings.whichwindow = COMMENTARY_WINDOW;
	//gui_change_window_title(settings.CommWindowModule);
  	return FALSE;
}


#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb (GeckoHtml *html,
			     gchar *uri,
			     gpointer user_data)
{	
    	gui_menu_popup (settings.CommWindowModule, NULL);
}
#endif

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
	GtkWidget *scrolledwindow;
	GtkWidget *eventbox1;
	box_comm = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(box_comm);
    
#ifdef USE_GTKMOZEMBED	
	eventbox1 = gtk_event_box_new ();
	gtk_widget_show (eventbox1);
	gtk_box_pack_start(GTK_BOX(box_comm),
			   eventbox1, TRUE,
			   TRUE, 0);
	widgets.html_comm = GTK_WIDGET(gecko_html_new(NULL, FALSE, COMMENTARY_TYPE));
	gtk_widget_show(widgets.html_comm);
	gtk_container_add(GTK_CONTAINER(eventbox1),
			 widgets.html_comm);
	
	g_signal_connect((gpointer)widgets.html_comm,
		      "popupmenu_requested",
		      G_CALLBACK (_popupmenu_requested_cb),
		      NULL);
	g_signal_connect ((gpointer) eventbox1, "enter_notify_event",
		    G_CALLBACK (on_enter_notify_event),
		    NULL);

#else
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(box_comm),
			   scrolledwindow, TRUE,
			   TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	/*gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow,
                                             settings.shadow_type);*/

	widgets.html_comm = gtk_html_new();
	gtk_widget_show(widgets.html_comm);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_comm);
	g_signal_connect(GTK_OBJECT(widgets.html_comm), "link_clicked",
				   G_CALLBACK(gui_link_clicked),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_comm), "on_url",
				   G_CALLBACK(gui_url),
				   GINT_TO_POINTER(COMMENTARY_TYPE));
	g_signal_connect(GTK_OBJECT(widgets.html_comm),
				   "button_press_event",
				   G_CALLBACK
				   (on_comm_button_press_event),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_comm),
				   "button_release_event",
				   G_CALLBACK
				   (on_comm_button_release_event),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_comm),
			   "url_requested",
			   G_CALLBACK(url_requested), NULL);
#endif
	return box_comm;
}

