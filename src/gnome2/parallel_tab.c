/*
 * Xiphos Bible Study Tool
 * parallel_dialog.c - dialog for parallel in a tab
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
#ifndef USE_GTKBUILDER
#include <glade/glade-xml.h>
#endif
#include "xiphos_html/xiphos_html.h"

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

static void sync_with_main(const gchar *key);

static void
_popupmenu_requested_cb(XiphosHtml *html, gchar *uri, gpointer user_data)
{
	gui_popup_menu_parallel();
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
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

void gui_parallel_close_tab(GtkButton *button, gpointer user_data)
{
	//gtk_widget_destroy(parallel_UnDock_Dialog);
}
#endif

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

static void on_parallel_tab_destroy(GObject *object, gpointer user_data)
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

static void sync_with_main(const gchar *key)
{
	gchar *buf = NULL;

	if (key)
		buf = (gchar *)main_url_encode(key);
	else
		buf = (gchar *)
		    main_url_encode(xml_get_value("keys", "verse"));
	XI_message(("%s", buf));
	if (buf && (strlen(buf) > 3)) {
		gchar *url =
		    g_strdup_printf("passagestudy.jsp?action=showParallel&"
				    "type=verse&value=%s", buf);

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
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navbar_parallel.button_sync)))
		sync_with_main(NULL);
}

void gui_parallel_tab_sync(const gchar *key)
{
	sync_with_main(key);
}

void gui_force_parallel_tab_sync(void)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(navbar_parallel.button_sync),
				     settings.linkedtabs);
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
#ifndef USE_WEBKIT2
	GtkWidget *scrolled_window;
#endif

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
				   FALSE);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
				      0);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
				     TRUE);

	UI_VBOX(parallel_vbox, FALSE, 0);
	g_signal_connect(G_OBJECT(parallel_vbox), "destroy",
			 G_CALLBACK(on_parallel_tab_destroy), NULL);
	gtk_widget_show(parallel_vbox);
	gtk_box_pack_start(GTK_BOX(widgets.page), parallel_vbox, TRUE,
			   TRUE, 0);
	toolbar29 = create_nav_toolbar();
	gtk_widget_show(toolbar29);
	gtk_box_pack_start(GTK_BOX(parallel_vbox), toolbar29, FALSE, FALSE,
			   0);

	UI_HBOX(box_parallel_labels, TRUE, 2);
	gtk_widget_show(box_parallel_labels);
	gtk_box_pack_start(GTK_BOX(parallel_vbox), box_parallel_labels,
			   FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(box_parallel_labels),
				       2);

#if 0
	if (settings.parallel_list) {
		GtkWidget *plabel;
		gchar *label;
		gint modidx;

		for (modidx = 0; settings.parallel_list[modidx]; ++modidx) {
			plabel = gtk_label_new(NULL);
			gtk_widget_show(plabel);
			gtk_box_pack_start(GTK_BOX(box_parallel_labels),
					   plabel, FALSE, FALSE, 0);
			gtk_label_set_use_markup(GTK_LABEL(plabel), TRUE);

			label =
			    g_strdup_printf
			    ("<span color='%s' weight='bold'>%s</span>",
			     settings.bible_verse_num_color,
			     settings.parallel_list[modidx]);
			gtk_label_set_markup(GTK_LABEL(plabel), label);
			g_free(label);
		}
	}
#endif /* 0 */

#ifndef USE_WEBKIT2
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolled_window);
	gtk_box_pack_start(GTK_BOX(parallel_vbox), scrolled_window, TRUE,
			   TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolled_window,
					    settings.shadow_type);
#endif

	widgets.html_parallel_dialog =
	    GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, PARALLEL_TYPE));
	gtk_widget_show(widgets.html_parallel_dialog);
#ifdef USE_WEBKIT2
	gtk_box_pack_start(GTK_BOX(parallel_vbox), widgets.html_parallel_dialog, TRUE, TRUE, 0);
#else
	gtk_container_add(GTK_CONTAINER(scrolled_window),
			  widgets.html_parallel_dialog);
#endif

	g_signal_connect((gpointer)widgets.html_parallel_dialog,
			 "popupmenu_requested",
			 G_CALLBACK(_popupmenu_requested_cb), NULL);
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
