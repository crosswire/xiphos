/*
 * Xiphos Bible Study Tool
 * parallel_dialog.c - dialog for detached parallel
 *
 * Copyright (C) 2000-2011 Xiphos Developer Team
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

#include <gtk/gtk.h>
#ifdef GTKHTML
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif
#ifndef USE_GTKBUILDER
  #include <glade/glade-xml.h>
#endif
#include "../xiphos_html/xiphos_html.h"

#include "gui/parallel_tab.h"
#include "gui/parallel_view.h"
#include "gui/xiphos.h"
#include "gui/navbar_versekey_parallel.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/lists.h"
#include "main/navbar.h"
#include "main/navbar_versekey.h"
#include "main/parallel_view.h"
#include "main/settings.h"
#include "main/url.hh"
#include "main/xml.h"

#include "gui/debug_glib_null.h"

extern gboolean shift_key_pressed;

GtkWidget *entrycbIntBook;
GtkWidget *sbIntChapter;
GtkWidget *sbIntVerse;
GtkWidget *entryIntLookup;

extern gboolean sync_on;
/******************************************************************************
 * static
 */
static GtkWidget *parallel_vbox;
NAVBAR_VERSEKEY navbar_parallel;

static void sync_with_main(const gchar * key);


#ifdef USE_XIPHOS_HTML
static void
_popupmenu_requested_cb(XiphosHtml *html,
			gchar *uri,
			gpointer user_data)
{
	gui_popup_menu_parallel();
}
#else
static gboolean
_popupmenu_requested_cb(GtkHTML *html,
			GdkEventButton * event,
			gpointer date)
{
	if (event->button == 3) {
		gui_popup_menu_parallel();
		return 1;
	}
	return 0;
}
#endif


/******************************************************************************
 * Name
 *   on_btnDockInt_clicked
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *
 *   void on_btnDockInt_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *
 *
 *
 *
 *
 * Return value
 *   void
 */

void gui_parallel_close_tab(GtkButton * button, gpointer user_data)
{
	//gtk_widget_destroy(parallel_UnDock_Dialog);
}


/******************************************************************************
 * Name
 *   on_dlgparallel_destroy
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *
 *   void on_dlgparallel_destroy(GtkObject *object, gpointer user_data)
 *
 * Description
 *
 *
 *
 *
 *
 * Return value
 *   void
 */

static void on_parallel_tab_destroy(GObject * object,
				      gpointer user_data)
{

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                             TRUE);

	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
					1);
	settings.dockedInt = TRUE;
	main_update_parallel_page();
}


/******************************************************************************
 * Name
 *   sync_with_main
 *
 * Synopsis
 *   #include "gui/parallel_dialog.h"
 *
 *   void sync_with_main(void)
 *
 * Description navbar.lookup_entry
 *   bring the the Parallel Dialog into sync with main window
 *
 * Return value
 *   void
 */

static void sync_with_main(const gchar * key)
{
	gchar *buf = NULL;
	gchar *url = NULL;

	if (key)
		buf = (gchar*)main_url_encode(key);
	else
		buf = (gchar*)main_url_encode(xml_get_value("keys", "verse"));
	GS_message(("%s",buf));
	if (buf && (strlen(buf) > 3)) {
		url =
		    g_strdup_printf("passagestudy.jsp?action=showParallel&"
					"type=verse&value=%s",
					buf);

		main_url_handler(url, TRUE);
		g_free(url);
	}
}


/******************************************************************************
 * Name
 *   gui_keep_parallel_dialog_in_sync
 *
 * Synopsis
 *   #include "gui/parallel_dialog.h"
 *
 *   void gui_keep_parallel_dialog_in_sync(const char * key)
 *
 *
 *
 * Return value
 *   void
 */

void gui_keep_parallel_tab_in_sync(void)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(navbar_parallel.button_sync)))
		sync_with_main(NULL);
}

void gui_parallel_tab_sync(const gchar * key)
{
	sync_with_main(key);
}

void gui_force_parallel_tab_sync(void)
{
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(navbar_parallel.button_sync), settings.linkedtabs);
	sync_on = settings.linkedtabs;
	/* tell somebody that this button has been hacked */
}


/******************************************************************************
 * Name
 *   create_nav_toolbar
 *
 * Synopsis
 *   #include ".h"
 *
 *   GtkWidget *create_nav_toolbar(void)
 *
 * Description
 *    create navigation toolbar and
 *
 * Return value
 *   void
 */

static GtkWidget *create_nav_toolbar(void)
{
	return gui_navbar_versekey_parallel_new();
}

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
		//gui_popup_pm_text();
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_text_button_release_event
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  gboolean on_text_button_release_event(GtkWidget * widget,
 *					  GdkEventButton * event,
 *					  DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

static gboolean on_text_button_release_event(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer date)
{
	switch (event->button) {
	case 1:

		break;
	case 2:
		if (shift_key_pressed) {
			shift_key_pressed = FALSE;
			break;
		}
		break;
	case 3:
		break;
	}
	return FALSE;
}
#endif /* !USE_XIPHOS_HTML */

/******************************************************************************
 * Name
 *   create_parallel_tab
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *
 *   GtkWidget *create_parallel_tab(void)
 *
 * Description
 *
 *
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *_create_parallel_tab(void)
{
	GtkWidget *toolbar29;
  	GtkWidget *box_parallel_labels;
#ifdef  USE_GTKMOZEMBED
	GtkWidget *frame;
#else
	GtkWidget *scrolled_window;
#endif
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                             FALSE);
	gtk_notebook_set_current_page (GTK_NOTEBOOK(widgets.notebook_bible_parallel),
					0);
	gtk_notebook_set_show_border (GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                        TRUE);

	parallel_vbox = gtk_vbox_new(FALSE, 0);
	g_signal_connect(G_OBJECT(parallel_vbox), "destroy",
			   G_CALLBACK(on_parallel_tab_destroy),
			   NULL);
	gtk_widget_show(parallel_vbox);
	gtk_box_pack_start(GTK_BOX(widgets.page), parallel_vbox, TRUE, TRUE,
			   0);
	toolbar29 = create_nav_toolbar();
	gtk_widget_show(toolbar29);
	gtk_box_pack_start(GTK_BOX(parallel_vbox), toolbar29, FALSE, FALSE, 0);

	box_parallel_labels = gtk_hbox_new (TRUE, 2);
	gtk_widget_show (box_parallel_labels);
	gtk_box_pack_start (GTK_BOX (parallel_vbox), box_parallel_labels, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box_parallel_labels), 2);

#if 0
	if (settings.parallel_list) {
		GtkWidget *plabel;
		gchar *label;
		gint modidx;

		for (modidx = 0; settings.parallel_list[modidx]; ++modidx) {
			plabel = gtk_label_new(NULL);
			gtk_widget_show(plabel);
			gtk_box_pack_start(GTK_BOX (box_parallel_labels), plabel, FALSE, FALSE, 0);
			gtk_label_set_use_markup(GTK_LABEL(plabel), TRUE);

			label = g_strdup_printf("<span color='%s' weight='bold'>%s</span>",
						settings.bible_verse_num_color,
						settings.parallel_list[modidx]);
			gtk_label_set_markup(GTK_LABEL(plabel), label);
			g_free(label);
		}
	}
#endif /* 0 */
#ifndef  USE_GTKMOZEMBED
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolled_window);
	gtk_box_pack_start(GTK_BOX(parallel_vbox), scrolled_window, TRUE, TRUE,0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolled_window,
                                             settings.shadow_type);
#endif
#ifdef USE_XIPHOS_HTML
    
	widgets.html_parallel_dialog = GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, PARALLEL_TYPE));
	gtk_widget_show(widgets.html_parallel_dialog);
    
 #ifdef USE_WEBKIT 
	gtk_container_add(GTK_CONTAINER(scrolled_window),
			  widgets.html_parallel_dialog);

 #else
	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(parallel_vbox), frame, TRUE, TRUE, 0);
	
	gtk_container_add(GTK_CONTAINER(frame),
			  widgets.html_parallel_dialog);
    
 #endif /* USE_WEBKIT */ 
	
/*
	eventbox = gtk_event_box_new ();
	gtk_widget_show (eventbox);
	gtk_container_add(GTK_CONTAINER(frame), eventbox);
*/
	g_signal_connect((gpointer)widgets.html_parallel_dialog,
			 "popupmenu_requested",
			 G_CALLBACK (_popupmenu_requested_cb),
			 NULL);
#else

	widgets.html_parallel_dialog = gtk_html_new();
	gtk_widget_show(widgets.html_parallel_dialog);
	gtk_html_load_empty(GTK_HTML(widgets.html_parallel_dialog));
	gtk_container_add(GTK_CONTAINER(scrolled_window),
			  widgets.html_parallel_dialog);

	g_signal_connect(G_OBJECT(widgets.html_parallel_dialog), "link_clicked",
				G_CALLBACK(gui_link_clicked),
				NULL);
	g_signal_connect(G_OBJECT(widgets.html_parallel_dialog), "on_url",
				G_CALLBACK(gui_url),
				GINT_TO_POINTER(TEXT_TYPE));
	g_signal_connect(G_OBJECT(widgets.html_parallel_dialog),"button_release_event",
				G_CALLBACK(on_text_button_release_event),
				NULL);
	g_signal_connect(G_OBJECT(widgets.html_parallel_dialog), "button_press_event",
				G_CALLBACK(on_text_button_press_event),
				NULL);

	g_signal_connect(G_OBJECT(widgets.html_parallel_dialog),
			 "button_release_event",
			 G_CALLBACK (_popupmenu_requested_cb),
			 NULL);
	g_signal_connect(G_OBJECT(widgets.html_parallel_dialog),
			 "url_requested",
			 G_CALLBACK(url_requested), NULL);
#endif
	gtk_widget_hide(widgets.hpaned);
	return parallel_vbox;

}


GtkWidget *gui_create_parallel_tab(void)
{
	settings.dockedInt = FALSE;
	return _create_parallel_tab();
}


void gui_destroy_parallel_tab(void)
{
	settings.dockedInt = TRUE;
	sync_on = FALSE;
	gtk_widget_destroy(parallel_vbox);
	gtk_widget_show(widgets.hpaned);
}

/******   end of file   ******/
