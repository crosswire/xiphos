/*
 * Xiphos Bible Study Tool
 * bibletext_dialog.c - view Bible text module in a dialog
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

#include "gui/bibletext_dialog.h"
#include "gui/display_info.h"
#include "gui/font_dialog.h"
#include "gui/sidebar.h"
#include "gui/main_window.h"
#include "gui/menu_popup.h"
#include "gui/navbar_versekey_dialog.h"
#include "gui/xiphos.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/navbar.h"
#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"
#include "main/display.hh"

#include "gui/debug_glib_null.h"

/******************************************************************************
 * static - global to this file only
 */
static DIALOG_DATA *cur_vt;

/******************************************************************************
 * externs
 */
extern gboolean bible_freed;

/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void dialog_destroy(GtkObject *object, DL_DIALOG * dlg)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void dialog_destroy(GObject *object, DIALOG_DATA *vt)
{
	if (!bible_freed)
		main_free_on_destroy(vt);
	bible_freed = FALSE;
}

static gboolean on_dialog_motion_notify_event(GtkWidget *widget,
					      GdkEventMotion *event,
					      DIALOG_DATA *vt)
{
	cur_vt = vt;
	return FALSE;
}

/******************************************************************************
 * Name
 *   sync_with_main
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_with_main(DIALOG_DATA * c)
 *
 * Descriptionc->navbar.lookup_entry
 *   bring the the View Commentay Dialog module into sync with main window
 *
 * Return value
 *   void
 */

static void sync_with_main(DIALOG_DATA *c)
{
	gchar *url = g_strdup_printf("sword:///%s", settings.currentverse);
	main_dialogs_url_handler(c, url, TRUE);
	g_free(url);
}

/******************************************************************************
 * Name
 *   sync_toggled
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_toggled(GtkToggleButton * button, DIALOG_DATA * c)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_bible_dialog_sync_toggled(GtkToggleButton *button,
				   DIALOG_DATA *c)
{
	if (c == NULL)
		c = cur_vt;
	if (gtk_toggle_button_get_active(button)) {
		sync_with_main(c);
		c->sync = TRUE;
	} else
		c->sync = FALSE;
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

static GtkWidget *create_nav_toolbar(DIALOG_DATA *c)
{
	c->navbar.type = NB_DIALOG;
	return gui_navbar_versekey_dialog_new(c);
}

static void
_popupmenu_requested_cb(XiphosHtml *html, gchar *uri, gpointer user_data)
{
	DIALOG_DATA *d = (DIALOG_DATA *)cur_vt;
	gui_menu_popup(html, d->mod_name, d); //gui_text_dialog_create_menu(d);
}

/******************************************************************************
 * Name
 *   create_bibletext_dialog
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *   void create_bibletext_dialog(DIALOG_DATA * vt)
 *
 * Description
 *   create a text dialog
 *
 * Return value
 *   void
 */

void gui_create_bibletext_dialog(DIALOG_DATA *vt)
{
	GtkWidget *vbox33;
	GtkWidget *paned;
	//GtkWidget *frame;
	GtkWidget *swVText;

	vt->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_object_set_data(G_OBJECT(vt->dialog), "dlg->dialog", vt->dialog);
	gtk_window_set_title(GTK_WINDOW(vt->dialog),
			     main_get_module_description(vt->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(vt->dialog), 350, 450);
	gtk_window_set_resizable(GTK_WINDOW(vt->dialog), TRUE);

	UI_VBOX(vbox33, FALSE, 0);
	gtk_widget_show(vbox33);
	gtk_container_add(GTK_CONTAINER(vt->dialog), vbox33);

	vt->toolbar_nav = create_nav_toolbar(vt);
	gtk_widget_show(vt->toolbar_nav);
	gtk_box_pack_start(GTK_BOX(vbox33), vt->toolbar_nav, FALSE, FALSE,
			   0);

	paned = UI_VPANE();
	gtk_box_pack_start(GTK_BOX(vbox33), paned, TRUE, TRUE, 0);
	gtk_widget_show(paned);

	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)swVText,
					    settings.shadow_type);
	gtk_paned_add1((GtkPaned *)paned, swVText);

	vt->html = GTK_WIDGET(XIPHOS_HTML_NEW(vt, TRUE, DIALOG_TEXT_TYPE));
	gtk_widget_show(vt->html);
	gtk_container_add(GTK_CONTAINER(swVText), vt->html);
	g_signal_connect((gpointer)vt->html,
			 "popupmenu_requested",
			 G_CALLBACK(_popupmenu_requested_cb), vt);

	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(swVText);
	//gtk_container_add(GTK_CONTAINER(frame), swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)swVText,
					    settings.shadow_type);

	gtk_paned_add2((GtkPaned *)paned, swVText);

	vt->previewer = GTK_WIDGET(XIPHOS_HTML_NEW(vt, TRUE, VIEWER_TYPE));
	gtk_widget_show(vt->previewer);
	gtk_container_add(GTK_CONTAINER(swVText), vt->previewer);

	gtk_paned_set_position(GTK_PANED(paned), 250);
	vt->statusbar = gtk_statusbar_new();
	gtk_widget_show(vt->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox33), vt->statusbar, FALSE, FALSE,
			   0);

	g_signal_connect(G_OBJECT(vt->dialog),
			 "destroy", G_CALLBACK(dialog_destroy), vt);

	g_signal_connect(G_OBJECT(vt->dialog),
			 "motion_notify_event",
			 G_CALLBACK(on_dialog_motion_notify_event), vt);
}

/******   end of file   ******/
