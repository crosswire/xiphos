/*
 * Xiphos Bible Study Tool
 * sidebar_dialog.c - dialog for detached sidebar
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
#include <glib/gi18n.h>

#include "gui/sidebar.h"
#include "gui/sidebar_dialog.h"
#include "gui/utilities.h"
#include "gui/xiphos.h"
#include "gui/widgets.h"

#include "main/previewer.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "gui/debug_glib_null.h"

#include "xiphos_html/xiphos_html.h"

static GtkWidget *create_sidebar_dialog(void);
static GtkWidget *vbox_dock;

/******************************************************************************
 * Name
 *   gui_attach_detach_sidebar
 *
 * Synopsis
 *   #include "sidebar_dialog.h"
 *
 *   void gui_attach_detach_sidebar(void)
 *
 * Description
 *   dock/undock shortcut bar
 *
 *
 * Return value
 *
 */

void gui_attach_detach_sidebar(void)
{
	gint biblepanesize;

	if (settings.docked) {
		settings.docked = FALSE;
		biblepanesize = settings.gs_width / 2;
		widgets.dock_sb = create_sidebar_dialog();
#if GTK_CHECK_VERSION(3, 10, 0)
		gtk_container_add(GTK_CONTAINER(vbox_dock),
				  widgets.shortcutbar);
#else
		gtk_widget_reparent(widgets.shortcutbar, vbox_dock);
#endif
		settings.showshortcutbar = TRUE;
		gtk_paned_set_position(GTK_PANED(widgets.epaned), 0);
		/*gtk_paned_set_position(GTK_PANED(widgets.hpaned),
		   biblepanesize); */

		/* ugly fix until someone can make mozembed work with 'gtk_widget_reparent()' */
		gtk_widget_destroy(sidebar.html_viewer_widget);
		sidebar.html_viewer_widget =
		    GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, SB_VIEWER_TYPE));
		gtk_container_add(GTK_CONTAINER(sidebar.html_viewer_eventbox),
				  sidebar.html_viewer_widget);

		gtk_widget_show_all(widgets.dock_sb);
		gtk_widget_show(sidebar.html_viewer_widget);

		xml_set_value("Xiphos", "misc", "sidebar_docked", "0");
	} else {
		settings.docked = TRUE;
		biblepanesize =
		    (settings.gs_width - settings.sidebar_width) / 2;
		gtk_paned_set_position(GTK_PANED(widgets.epaned),
				       settings.sidebar_width);
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       biblepanesize);
#if GTK_CHECK_VERSION(3, 10, 0)
		gtk_container_add(GTK_CONTAINER(widgets.epaned),
				  widgets.shortcutbar);
#else
		gtk_widget_reparent(widgets.shortcutbar, widgets.epaned);
#endif

		/* ugly fix until someone can make mozembed work with 'gtk_widget_reparent()' */
		gtk_widget_destroy(sidebar.html_viewer_widget);
		sidebar.html_viewer_widget =
		    GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, SB_VIEWER_TYPE));
		gtk_container_add(GTK_CONTAINER(sidebar.html_viewer_eventbox),
				  sidebar.html_viewer_widget);

		gtk_widget_show(sidebar.html_viewer_widget);
		gtk_widget_destroy(widgets.dock_sb);

		xml_set_value("Xiphos", "misc", "sidebar_docked", "1");
	}
	main_set_previewer_widget(TRUE);
	main_init_previewer();
}

/******************************************************************************
 * Name
 *   on_dialog_destroy
 *
 * Synopsis
 *   #include "sidebar_dialog.h"
 *
 *   void on_dialog_destroy(GtkObject *object, gpointer user_data)
 *
 * Description
 *   send the sidebar back to the main window before we
 *   destroy the dialog
 *
 *
 * Return value
 *   void
 */

static void on_dialog_destroy(GObject *object, gpointer user_data)
{
	/* we need the if to prevent a loop */
	if (!settings.docked)
		gui_attach_detach_sidebar();
}

/******************************************************************************
 * Name
 *   create_sidebar_dialog
 *
 * Synopsis
 *   #include "sidebar_dialog.h"
 *
 *   GtkWidget*  create_sidebar_dialog(void)
 *
 * Description
 *   create dialog to hold the sidebar when detached
 *
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *create_sidebar_dialog(void)
{
	GtkWidget *dlgDock;
	gchar *title;

	dlgDock = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_object_set_data(G_OBJECT(dlgDock), "dlgDock", dlgDock);
	gtk_window_set_title(GTK_WINDOW(dlgDock), _("Sidebar"));
	gtk_window_set_resizable(GTK_WINDOW(dlgDock), TRUE);
	gtk_widget_set_size_request(dlgDock, settings.sidebar_width,
				    settings.gs_height);

	UI_VBOX(vbox_dock, FALSE, 0);
	gtk_widget_show(vbox_dock);
	gtk_container_add(GTK_CONTAINER(dlgDock), vbox_dock);

	title =
	    g_strdup_printf("%s - %s", _("Sidebar"),
			    settings.program_title);
	gtk_window_set_title(GTK_WINDOW(dlgDock), title);
	g_free(title);

	g_signal_connect(G_OBJECT(dlgDock), "destroy",
			 G_CALLBACK(on_dialog_destroy), NULL);

	return dlgDock;
}
