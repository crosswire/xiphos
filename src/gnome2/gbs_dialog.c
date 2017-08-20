/*
 * Xiphos Bible Study Tool
 * gbs_dialog.c - dialog for displaying a gbs module
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

#include "gui/gbs_dialog.h"
#include "gui/navbar_book_dialog.h"
#include "gui/widgets.h"
#include "gui/menu_popup.h"
#include "gui/utilities.h"

#include "main/module_dialogs.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"

extern gboolean dialog_freed;

enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_TITLE,
	COL_BOOK,
	COL_OFFSET,
	N_COLUMNS
};

/******************************************************************************
 * static - global to this file only
 */
//static GList *dialog_list;
static DIALOG_DATA *cur_dlg;
//static GtkCTreeNode *rootnode;
static GtkTreeModel *model;
//static gint tree_level;

/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void dialog_destroy(GtkObject *object, DL_DIALOG * dlg)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void dialog_destroy(GObject *object, DIALOG_DATA *dlg)
{
	if (!dialog_freed)
		main_free_on_destroy(dlg);
	dialog_freed = FALSE;
}

/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, DL_DATA * d)
 *
 * Description
 *
 *
 * Return value
 *   void
 */
/*
static void dialog_url(GtkHTML * html, const gchar * url,
		       DIALOG_DATA * dlg)
{
	gchar buf[255];
	gint context_id2;

	cur_dlg = dlg;

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(dlg->statusbar),
					 "Xiphos");
	gtk_statusbar_pop(GTK_STATUSBAR(dlg->statusbar), context_id2);


	if (url == NULL) {
		gtk_statusbar_push(GTK_STATUSBAR(dlg->statusbar),
				   context_id2, "");
	}

	else {
		if (*url == '@') {
			++url;
			sprintf(buf, _("Show %s in main window"), url);
		}

		else if (*url == '[') {
			++url;
			while (*url != ']') {
				++url;
			}
			++url;
			sprintf(buf, "%s", url);
		}

		else if (*url == '*') {
			++url;
			sprintf(buf, "%s", url);
		}

		else
			sprintf(buf, _("Go to %s"), url);

		gtk_statusbar_push(GTK_STATUSBAR(dlg->statusbar),
				   context_id2, buf);
	}

}
*/

/******************************************************************************
 * Name
 *   tree_selection_changed
 *
 * Synopsis
 *   #include "gui/gbs.h"
 *
 *   void tree_selection_changed(GtkTreeSelection * selection,
 *		      GtkWidget * tree_widget)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void tree_selection_changed(GtkTreeSelection *selection,
				   DIALOG_DATA *g)
{
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(g->tree));

	main_dialogs_tree_selection_changed(model, selection, TRUE, g);
}

static GtkTreeModel *create_model(void)
{
	GtkTreeStore *model;

	/* create tree store */
	model = gtk_tree_store_new(N_COLUMNS,
				   GDK_TYPE_PIXBUF,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_STRING);
	return GTK_TREE_MODEL(model);
}

static void add_columns(GtkTreeView *tree)
{
	GtkTreeViewColumn *column;
	//      GtkTreeViewColumn *column2;
	GtkCellRenderer *renderer;

	column = gtk_tree_view_column_new();

	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COL_OPEN_PIXBUF,
					    "pixbuf-expander-open", COL_OPEN_PIXBUF,
					    "pixbuf-expander-closed", COL_CLOSED_PIXBUF, NULL);

	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_TITLE, NULL);
	gtk_tree_view_append_column(tree, column);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_BOOK, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_OFFSET, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);
}

static void
_popupmenu_requested_cb(XiphosHtml *html, gchar *uri, DIALOG_DATA *d)
{
	gui_menu_popup(html, cur_dlg->mod_name, cur_dlg);
}

/******************************************************************************
 * Name
 *   create_gbs_dialog
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void create_gbs_dialog(GBS_DATA * dlg)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_create_gbs_dialog(DIALOG_DATA *dlg)
{
	GtkWidget *vbox_dialog;
	GtkWidget *navbar;
	GtkWidget *hpaned;
	GtkWidget *scrolledwindow_ctree;
#ifndef USE_WEBKIT2
	GtkWidget *scrolledwindow_html;
#endif
	GObject *selection;

	dlg->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_object_set_data(G_OBJECT(dlg->dialog), "dlg->dialog",
			  dlg->dialog);
	gtk_window_set_title(GTK_WINDOW(dlg->dialog),
			     main_get_module_description(dlg->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(dlg->dialog), 525, 306);
	gtk_window_set_resizable(GTK_WINDOW(dlg->dialog), TRUE);

	UI_VBOX(vbox_dialog, FALSE, 0);
	gtk_widget_show(vbox_dialog);
	gtk_container_add(GTK_CONTAINER(dlg->dialog), vbox_dialog);

	navbar = gui_navbar_book_dialog_new(dlg);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), navbar, FALSE, FALSE, 0);

	hpaned = UI_HPANE();
	gtk_widget_show(hpaned);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), hpaned, TRUE, TRUE, 0);
	//gtk_container_add(GTK_CONTAINER(frame_gbs), hpaned);
	//gtk_paned_set_position(GTK_PANED(hpaned), 190);

	scrolledwindow_ctree = gtk_scrolled_window_new(NULL, NULL);
	//gtk_widget_show(scrolledwindow_ctree);
	gtk_paned_pack1(GTK_PANED(hpaned), scrolledwindow_ctree, FALSE,
			TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_ctree),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow_ctree,
					    settings.shadow_type);

	model = create_model();
	dlg->tree = gtk_tree_view_new_with_model(model);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(dlg->tree), FALSE);
	gtk_widget_show(dlg->tree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_ctree), dlg->tree);
	add_columns(GTK_TREE_VIEW(dlg->tree));

	selection =
	    G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(dlg->tree)));

#ifndef USE_WEBKIT2
	scrolledwindow_html = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_html);
	gtk_paned_pack2(GTK_PANED(hpaned), scrolledwindow_html, FALSE,
			TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_html),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow_html,
					    settings.shadow_type);
#endif

	dlg->html =
	    GTK_WIDGET(XIPHOS_HTML_NEW(((DIALOG_DATA *)dlg), TRUE, DIALOG_BOOK_TYPE));
	gtk_widget_show(dlg->html);
#ifdef USE_WEBKIT2
	gtk_paned_pack2(GTK_PANED(hpaned), dlg->html, FALSE, TRUE);
#else
	gtk_container_add(GTK_CONTAINER(scrolledwindow_html), dlg->html);
#endif
	g_signal_connect((gpointer)dlg->html,
			 "popupmenu_requested",
			 G_CALLBACK(_popupmenu_requested_cb),
			 (DIALOG_DATA *)dlg);

	g_signal_connect(selection, "changed",
			 G_CALLBACK(tree_selection_changed),
			 (DIALOG_DATA *)dlg);
	dlg->statusbar = gtk_statusbar_new();
	gtk_widget_show(dlg->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), dlg->statusbar, FALSE,
			   FALSE, 0);

	g_signal_connect(G_OBJECT(dlg->dialog), "destroy",
			 G_CALLBACK(dialog_destroy), (DIALOG_DATA *)dlg);
	cur_dlg = dlg;
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
/******************************************************************************
 * Name
 *   gui_close_gbs_dialog
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void gui_close_gbs_dialog(DL_DIALOG *dlg)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_close_gbs_dialog(DIALOG_DATA *dlg)
{
	if (dlg->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(dlg->dialog);
	}
}
#endif

//******  end of file  ******/
