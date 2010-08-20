/*
 * Xiphos Bible Study Tool
 * parallel_dialog.c - dialog for detached parallel
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
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <glade/glade-xml.h>
#ifdef USE_GTKMOZEMBED
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "webkit/wk-html.h"
#endif
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif  /* USE_GTKMOZEMBED */

#include "gui/parallel_dialog.h"
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

extern gboolean do_display;

GtkWidget *entrycbIntBook;
GtkWidget *sbIntChapter;
GtkWidget *sbIntVerse;
GtkWidget *entryIntLookup;

/******************************************************************************
 * static
 */
static GtkWidget *dialog_parallel;

static GtkWidget *parallel_UnDock_Dialog;
static GtkWidget *vboxInt;
static gboolean ApplyChangeBook;
static NAVBAR navbar;
NAVBAR_VERSEKEY navbar_parallel;

static GtkWidget *create_parallel_dialog(void);
static void sync_with_main(void);


#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb(WkHtml *html,
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
 *   undock_parallel_page
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *
 *   void undock_parallel_page(void)
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

void gui_undock_parallel_page(void)
{
	ApplyChangeBook = FALSE;
	parallel_UnDock_Dialog = create_parallel_dialog();

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                             FALSE);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
					0);


	gtk_widget_show(parallel_UnDock_Dialog);
	//main_update_parallel_page_detached();
	//g_free(settings.cvparallel);
	settings.dockedInt = FALSE;
	ApplyChangeBook = TRUE;
	sync_with_main();
}

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

void gui_btnDockInt_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(parallel_UnDock_Dialog);
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

static void on_dlgparallel_destroy(GtkObject * object,
				      gpointer user_data)
{

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                             TRUE);

	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
					1);
	settings.dockedInt = TRUE;
	main_update_parallel_page();

	settings.parallelpage = 0;
	xml_set_value("Xiphos", "layout", "parallelopen", "0");
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

static void sync_with_main(void)
{
	gchar *buf = NULL;

	buf = (gchar*)main_url_encode (settings.currentverse);

	if (buf && (strlen(buf) > 3)) {
		gchar *url =
		    g_strdup_printf ("passagestudy.jsp?action=showParallel&"
					"type=verse&value=%s",
					buf);
		main_url_handler (url, TRUE);
		g_free (url);
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

void gui_keep_parallel_dialog_in_sync(void)
{
	if (GTK_TOGGLE_BUTTON(navbar_parallel.button_sync)->active)
		sync_with_main();
}


/******************************************************************************
 * Name
 *   gui_set_parallel_navbar
 *
 * Synopsis
 *   #include "gui/parallel_dialog.h"
 *
 *   void gui_set_parallel_navbar(const char key)
 *
 *
 *
 * Return value
 *   void
 */

void gui_set_parallel_navbar(const char * key)
{
	main_navbar_set(navbar, key);
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


/******************************************************************************
 * Name
 *   on_parallel_configure_event
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   gboolean on_parallel_configure_event(GtkWidget * widget,
 *				   GdkEventConfigure * event,
 *				   gpointer user_data)
 *
 * Description
 *   remember placement+size of parallel window.
 *   cloned from on_configure_event
 *
 * Return value
 *   gboolean
 */

static gboolean on_parallel_configure_event(GtkWidget * widget,
					    GdkEventConfigure * event,
					    gpointer user_data)
{
	gchar layout[80];
	gint x;
	gint y;

 	gdk_window_get_root_origin(
	    GDK_WINDOW(dialog_parallel->window), &x, &y);

	settings.parallel_width  = event->width;
	settings.parallel_height = event->height;
	settings.parallel_x = x;
	settings.parallel_y = y;

	sprintf(layout, "%d", settings.parallel_width);
	xml_set_value("Xiphos", "layout", "parallel_width", layout);

	sprintf(layout, "%d", settings.parallel_height);
	xml_set_value("Xiphos", "layout", "parallel_height", layout);

	sprintf(layout, "%d", settings.parallel_x);
	xml_set_value("Xiphos", "layout", "parallel_x", layout);

	sprintf(layout, "%d", settings.parallel_y);
	xml_set_value("Xiphos", "layout", "parallel_y", layout);
	xml_save_settings_doc(settings.fnconfigure);

	return FALSE;
}

/******************************************************************************
 * Name
 *   create_parallel_dialog
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *
 *   GtkWidget *create_parallel_dialog(void)
 *
 * Description
 *
 *
 *
 * Return value
 *   GtkWidget *
 */
static
GtkWidget *create_parallel_dialog(void)
{
  	GtkWidget *box_parallel_labels;
	GtkWidget *dialog_vbox25;
	GtkWidget *toolbar29;
	GtkWidget *dialog_action_area25;
	GtkWidget *hbuttonbox4;
	GtkWidget *btnDockInt;
#ifdef USE_GTKMOZEMBED
	GtkWidget *eventbox;
	GtkWidget *frame;
#else
	GtkWidget *scrolled_window;
#endif
	gchar title[256];

	sprintf(title,"%s - %s", settings.program_title, _("Parallel"));

	dialog_parallel = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog_parallel), title);

	g_object_set_data(G_OBJECT(dialog_parallel),
			  "dialog_parallel", dialog_parallel);
	gtk_window_set_default_size(GTK_WINDOW(dialog_parallel),
				    settings.parallel_width,
				    settings.parallel_height);
	gtk_window_set_resizable(GTK_WINDOW(dialog_parallel), TRUE);

	dialog_vbox25 = GTK_DIALOG(dialog_parallel)->vbox;
	g_object_set_data(G_OBJECT(dialog_parallel),
			  "dialog_vbox25", dialog_vbox25);
	gtk_widget_show(dialog_vbox25);



	vboxInt = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vboxInt);
	gtk_box_pack_start(GTK_BOX(dialog_vbox25), vboxInt, TRUE, TRUE,
			   0);
	toolbar29 = create_nav_toolbar();
	gtk_widget_show(toolbar29);
	gtk_box_pack_start(GTK_BOX(vboxInt), toolbar29, FALSE, FALSE,
			   0);

	box_parallel_labels = gtk_hbox_new (TRUE, 2);
	gtk_widget_show (box_parallel_labels);
	gtk_box_pack_start (GTK_BOX (vboxInt), box_parallel_labels, FALSE, TRUE, 0);
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

#ifdef USE_GTKMOZEMBED
	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(vboxInt), frame, TRUE, TRUE,0);

	eventbox = gtk_event_box_new ();
	gtk_widget_show (eventbox);
	gtk_container_add(GTK_CONTAINER(frame), eventbox);

	widgets.html_parallel_dialog = GTK_WIDGET(wk_html_new()); //embed_new(PARALLEL_TYPE); //gtk_moz_embed_new();
	gtk_widget_show(widgets.html_parallel_dialog);
	gtk_container_add(GTK_CONTAINER(eventbox),
			  widgets.html_parallel_dialog);
	g_signal_connect((gpointer)widgets.html_parallel_dialog,
			 "popupmenu_requested",
			 G_CALLBACK (_popupmenu_requested_cb),
			 NULL);
#else
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolled_window);
	gtk_box_pack_start(GTK_BOX(vboxInt), scrolled_window, TRUE, TRUE,0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolled_window,
                                             settings.shadow_type);

	widgets.html_parallel_dialog = gtk_html_new();
	gtk_widget_show(widgets.html_parallel_dialog);
	gtk_html_load_empty(GTK_HTML(widgets.html_parallel_dialog));
	gtk_container_add(GTK_CONTAINER(scrolled_window),
			  widgets.html_parallel_dialog);
	g_signal_connect(GTK_OBJECT(widgets.html_parallel_dialog),
			 "button_release_event",
			 G_CALLBACK (_popupmenu_requested_cb),
			 NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_parallel_dialog),
			 "url_requested",
			 G_CALLBACK(url_requested), NULL);
#endif
	dialog_action_area25 =
	    GTK_DIALOG(dialog_parallel)->action_area;
	g_object_set_data(G_OBJECT(dialog_parallel),
			  "dialog_action_area25",
			  dialog_action_area25);
	gtk_widget_show(dialog_action_area25);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area25), 10);

	hbuttonbox4 = gtk_hbutton_box_new();
	gtk_widget_show(hbuttonbox4);
	gtk_box_pack_start(GTK_BOX(dialog_action_area25), hbuttonbox4,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox4),
				  GTK_BUTTONBOX_END);

	btnDockInt = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	gtk_widget_show(btnDockInt);
	gtk_container_add(GTK_CONTAINER(hbuttonbox4), btnDockInt);
	GTK_WIDGET_SET_FLAGS(btnDockInt, GTK_CAN_DEFAULT);


	g_signal_connect(GTK_OBJECT(dialog_parallel), "destroy",
			   G_CALLBACK(on_dlgparallel_destroy),
			   NULL);
	g_signal_connect(GTK_OBJECT(btnDockInt), "clicked",
			   G_CALLBACK(gui_btnDockInt_clicked),
			   NULL);

	g_signal_connect((gpointer) dialog_parallel, 
			 "configure_event",
			 G_CALLBACK(on_parallel_configure_event), NULL);

	settings.parallelpage = 1;
	xml_set_value("Xiphos", "layout", "parallelopen", "1");

	set_window_icon(GTK_WINDOW(dialog_parallel));

	/*
	 * (from xiphos.c)
	 * a little paranoia:
	 * clamp geometry values to a reasonable bound.
	 * sometimes xiphos gets insane reconfig events as it dies,
	 * especially if it's due to just shutting linux down.
	 */
	if ((settings.parallel_x < 0) || (settings.parallel_x > 2000))
		settings.parallel_x = 0;
	if ((settings.parallel_y < 0) || (settings.parallel_y > 2000))
		settings.parallel_y = 0;

 	gtk_window_move(GTK_WINDOW(dialog_parallel),settings.parallel_x,settings.parallel_y);

	return dialog_parallel;
}

/******   end of file   ******/
