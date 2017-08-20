/*
 * Xiphos Bible Study Tool
 * commentary_dialog.c - dialog for a commentary module
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

#include "gui/commentary_dialog.h"
#include "gui/dialog.h"
#include "gui/xiphos.h"
#include "gui/main_window.h"
#include "gui/menu_popup.h"
#include "gui/navbar_versekey_dialog.h"
#include "gui/sidebar.h"
#include "gui/widgets.h"
#include "gui/utilities.h"

#include "main/lists.h"
#include "main/navbar.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/navbar_versekey.h"
#include "main/display.hh"

#include "gui/debug_glib_null.h"

extern gboolean dialog_freed;
extern gboolean do_display;

/****************************************************************************************
 * static - global to this file only
 */
DIALOG_DATA *cur_d;

/******************************************************************************
 * Name
 *   on_dialog_destroy
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_dialog_destroy(GObject * object,
 *						DIALOG_DATA * d)
 *
 * Description
 *   shut down the View Commentay Dialog
 *
 * Return value
 *   void
 */

static void on_dialog_destroy(GObject *object, DIALOG_DATA *d)
{
	if (!dialog_freed)
		main_free_on_destroy(d);
	dialog_freed = FALSE;
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
/******************************************************************************
 * Name
 *   gui_close_comm_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_close_comm_dialog(DIALOG_DATA * d)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_close_comm_dialog(DIALOG_DATA *d)
{
	if (d->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(d->dialog);
	}
}
#endif

/******************************************************************************
 * Name
 *   on_dialog_motion_notify_event
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   gboolean on_dialog_motion_notify_event(GtkWidget *widget,
                      GdkEventMotion  *event, DIALOG_DATA * d)
 *
 * Description
 *
 *
 * Return value
 *   gboolean
 */

static gboolean on_dialog_motion_notify_event(GtkWidget *widget,
					      GdkEventMotion *event,
					      DIALOG_DATA *d)
{
	cur_d = d;
	return FALSE;
}

#if 0
/******************************************************************************
 * Name
 *   sync_with_main
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_with_main(DIALOG_DATA * d)
 *
 * Descriptiond->navbar.lookup_entry
 *   bring the the View Commentay Dialog module into sync with main window
 *
 * Return value
 *   void
 */

static void sync_with_main(DIALOG_DATA *d)
{
	gchar *url = g_strdup_printf("sword:///%s", settings.currentverse);
	main_dialogs_url_handler(d, url, TRUE);
	cur_d = d;
	g_free(url);
}
#endif

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

static GtkWidget *create_nav_toolbar(DIALOG_DATA *d)
{
	d->navbar.type = NB_DIALOG;
	return gui_navbar_versekey_dialog_new(d);
}

static void
_popupmenu_requested_cb(XiphosHtml *html, gchar *uri, DIALOG_DATA *d)
{
	gui_menu_popup(html, cur_d->mod_name, cur_d);
	//gui_commentary_dialog_create_menu(d);
}

/******************************************************************************
 * Name
 *   gui_create_commentary_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_create_commentary_dialog(void)
 *
 * Description
 *   create a Commentary Dialog
 *
 * Return value
 *   void
 */

void gui_create_commentary_dialog(DIALOG_DATA *d, gboolean do_edit)
{
	GtkWidget *vbox30;
	GtkWidget *vbox_toolbars;
	GtkWidget *toolbar_nav;
	GtkWidget *frame19;
#ifndef USE_WEBKIT2
	GtkWidget *scrolledwindow38;
#endif

	cur_d = d;
	d->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_object_set_data(G_OBJECT(d->dialog), "d->dialog", d->dialog);
	gtk_window_set_title(GTK_WINDOW(d->dialog),
			     main_get_module_description(d->mod_name));
	//gtk_window_set_default_size(GTK_WINDOW(d->dialog), 462, 280);
	gtk_window_set_resizable(GTK_WINDOW(d->dialog), TRUE);
	if (do_edit)
		gtk_widget_set_size_request(d->dialog, 590, 380);
	else
		gtk_widget_set_size_request(d->dialog, 460, 280);

	UI_VBOX(vbox30, FALSE, 0);
	gtk_widget_show(vbox30);

	gtk_container_add(GTK_CONTAINER(d->dialog), vbox30);

	UI_VBOX(vbox_toolbars, FALSE, 0);
	gtk_widget_show(vbox_toolbars);
	gtk_box_pack_start(GTK_BOX(vbox30), vbox_toolbars, FALSE, FALSE,
			   0);

	toolbar_nav = create_nav_toolbar(d);
	gtk_widget_show(toolbar_nav);
	gtk_box_pack_start(GTK_BOX(vbox_toolbars), toolbar_nav, FALSE,
			   FALSE, 0);

	frame19 = gtk_frame_new(NULL);
	gtk_widget_show(frame19);
	gtk_box_pack_start(GTK_BOX(vbox30), frame19, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame19), GTK_SHADOW_IN);

#ifndef USE_WEBKIT2
	scrolledwindow38 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow38);
	gtk_container_add(GTK_CONTAINER(frame19), scrolledwindow38);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow38),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow38,
					    settings.shadow_type);
#endif

	d->html =
	    GTK_WIDGET(XIPHOS_HTML_NEW(((DIALOG_DATA *)d), TRUE,
				       DIALOG_COMMENTARY_TYPE));
	gtk_widget_show(d->html);
#ifdef USE_WEBKIT2
	gtk_container_add(GTK_CONTAINER(frame19), d->html);
#else
	gtk_container_add(GTK_CONTAINER(scrolledwindow38), d->html);
#endif
	g_signal_connect((gpointer)d->html,
			 "popupmenu_requested",
			 G_CALLBACK(_popupmenu_requested_cb),
			 (DIALOG_DATA *)d);

	g_signal_connect(G_OBJECT(d->dialog), "destroy",
			 G_CALLBACK(on_dialog_destroy), d);
	g_signal_connect(G_OBJECT(d->dialog),
			 "motion_notify_event",
			 G_CALLBACK(on_dialog_motion_notify_event), d);
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
void gui_commentary_dialog_sync_toggled(GtkToggleButton *button,
					DIALOG_DATA *d)
{
	if (d == NULL)
		d = cur_d;
	if (gtk_toggle_button_get_active(button)) {
		sync_with_main(d);
		d->sync = TRUE;
	} else
		d->sync = FALSE;
}
#endif

/******   end of file   ******/
