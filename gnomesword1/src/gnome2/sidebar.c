/*
 * GnomeSword Bible Study Tool
 * sidebar.c - create and maintain the new sidebar bar
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/shortcut-bar/e-vscrolled-bar.h>
#include <gal/widgets/e-unicode.h>

#include "gui/sidebar.h"
#include "gui/shortcutbar_dialog.h"
#include "gui/shortcutbar_search.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/editor.h"
#include "gui/bookmarks.h"
#include "gui/utilities.h"
#include "gui/about_modules.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/widgets.h"

#include "main/shortcutbar.h"
#include "main/sword.h"
#include "main/gbs.h"
#include "main/settings.h"
#include "main/lists.h"


static SB_VIEWER sb_v;


/******************************************************************************
 * Name
 *   mod_selection_changed
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void mod_selection_changed(GtkTreeSelection * selection,
 *		      GtkWidget * tree_widget)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void mod_selection_changed(GtkTreeSelection * selection,
				  GtkWidget * tree_widget)
{
	gint sbtype;
	GtkTreeIter selected;
	gchar *mod = NULL;
	gint mod_type;
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(tree_widget));

	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0,
				   &mod, -1);
		if (mod) {
			sbtype = get_mod_type(mod);
			if (sbtype == 0 || sbtype == 1)
				gui_change_module_and_key(mod,
							  settings.
							  currentverse);
			else if (sbtype == 3) {
				gtk_notebook_set_page
				    (GTK_NOTEBOOK
				     (widgets.workbook_lower), 1);
				gui_change_module_and_key(mod, NULL);
			}

			else
				gui_change_module_and_key(mod,
							  settings.
							  dictkey);
			//g_warning("mod = %s\nkey = %s",mod,settings.dictkey);
			g_free(mod);
		}
	}
}


/******************************************************************************
 * Name
 *   load_module_tree
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void load_module_tree(GtkWidget * tree)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void load_module_tree(GtkWidget * tree)
{
	gint i;
	static gboolean need_column = TRUE;
	GtkTreeStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeIter child_iter;
	GList *tmp = NULL;
	GObject *mod_selection;


	need_column = TRUE;
	store = gtk_tree_store_new(1, G_TYPE_STRING);
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, "Biblical Texts", -1);

	//gtk_list_store_clear(store);
	tmp = get_list(TEXT_LIST);
	while (tmp != NULL) {
		gtk_tree_store_append(store, &child_iter, &iter);
		gtk_tree_store_set(store, &child_iter,
				   0, (gchar *) tmp->data, -1);
		tmp = g_list_next(tmp);

	}
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, "Commentaries", -1);

	tmp = get_list(COMM_LIST);
	while (tmp != NULL) {
		gtk_tree_store_append(store, &child_iter, &iter);
		gtk_tree_store_set(store, &child_iter,
				   0, (gchar *) tmp->data, -1);
		tmp = g_list_next(tmp);
	}
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, "Dict/Lex", -1);

	tmp = get_list(DICT_LIST);
	while (tmp != NULL) {
		gtk_tree_store_append(store, &child_iter, &iter);
		gtk_tree_store_set(store, &child_iter,
				   0, (gchar *) tmp->data, -1);
		tmp = g_list_next(tmp);

	}
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, "General Books", -1);

	tmp = get_list(GBS_LIST);
	while (tmp != NULL) {
		gtk_tree_store_append(store, &child_iter, &iter);
		gtk_tree_store_set(store, &child_iter,
				   0, (gchar *) tmp->data, -1);
		tmp = g_list_next(tmp);

	}


	gtk_tree_view_set_model(GTK_TREE_VIEW(tree),
				GTK_TREE_MODEL(store));
	if (need_column) {
		renderer = gtk_cell_renderer_text_new();
		column =
		    gtk_tree_view_column_new_with_attributes("Found",
							     renderer,
							     "text", 0,
							     NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tree),
					    column);
		need_column = FALSE;
	}

	mod_selection =
	    G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)));

	g_signal_connect(mod_selection, "changed",
			 G_CALLBACK(mod_selection_changed), tree);


}

/******************************************************************************
 * Name
 *   on_modules_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_modules_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_modules_activate(GtkMenuItem * menuitem,
				gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      0);
}

/******************************************************************************
 * Name
 *   on_bookmarks_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_bookmarks_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_bookmarks_activate(GtkMenuItem * menuitem,
				  gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      1);
}


/******************************************************************************
 * Name
 *   on_search_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_search_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_search_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      2);
}


/******************************************************************************
 * Name
 *   on_viewer_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_viewer_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_viewer_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      4);
}


/******************************************************************************
 * Name
 *   on_search_results_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_search_results_activate (GtkMenuItem *menuitem, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
static
void on_search_results_activate(GtkMenuItem * menuitem,
				gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      3);

}


/******************************************************************************
 * Name
 *   on_verse_list_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_verse_list_activate(GtkMenuItem *menuitem, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_verse_list_activate(GtkMenuItem * menuitem,
				   gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      5);

}

/******************************************************************************
 * Name
 *   create_viewer_page
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void create_viewer_page(GtkWidget * notebook)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void create_viewer_page(GtkWidget * notebook)
{
	GtkWidget *frame;
	GtkWidget *scrolledwindow;

	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_container_add(GTK_CONTAINER(notebook), frame);

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_container_add(GTK_CONTAINER(frame), scrolledwindow);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	sv->html_viewer_widget = gtk_html_new();
	gtk_widget_show(sv->html_viewer_widget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  sv->html_viewer_widget);
	gtk_html_load_empty(GTK_HTML(sv->html_viewer_widget));
/*			   
	gtk_signal_connect(GTK_OBJECT(sv->btn_save), "clicked",
			   G_CALLBACK(on_btnSBSaveVL_clicked), NULL);
			   
	gtk_signal_connect(GTK_OBJECT(sv->tbtn_view_main), "toggled",
			   G_CALLBACK
			   (on_tbtnSBViewMain_toggled), NULL);
			   
	gtk_signal_connect(GTK_OBJECT(sv->html_widget), "on_url",
			   G_CALLBACK(gui_url), widgets.app);
	*/
}

/******************************************************************************
 * Name
 *   create_search_results_page
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void create_search_results_page(GtkWidget * notebook)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void create_search_results_page(GtkWidget * notebook)
{
	GtkWidget *vbox;
	GtkWidget *frame;
	GtkWidget *scrolledwindow;
	GtkWidget *vpaned_srch_rslt;
	GtkWidget *frame3;
	GtkWidget *scrolledwindow3;
	GtkWidget *frame4;
	GtkWidget *scrolledwindow4;

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(notebook), vbox);

	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 0);

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_container_add(GTK_CONTAINER(frame), scrolledwindow);
	gtk_widget_set_usize(scrolledwindow, -2, 65);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_NEVER);

	widgets.html_search_report = gtk_html_new();
	gtk_widget_show(widgets.html_search_report);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_search_report);
	gtk_html_load_empty(GTK_HTML(widgets.html_search_report));

	vpaned_srch_rslt = gtk_vpaned_new();
	gtk_widget_show(vpaned_srch_rslt);
	gtk_box_pack_start(GTK_BOX(vbox), vpaned_srch_rslt, TRUE,
			   TRUE, 0);
	gtk_paned_set_position(GTK_PANED(vpaned_srch_rslt), 100);

	frame3 = gtk_frame_new(NULL);
	gtk_widget_show(frame3);
	gtk_paned_pack1(GTK_PANED(vpaned_srch_rslt), frame3, TRUE,
			TRUE);

	scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow3);
	gtk_container_add(GTK_CONTAINER(frame3), scrolledwindow3);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow3),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	sv->clist = gtk_clist_new(1);
	gtk_widget_show(sv->clist);
	gtk_container_add(GTK_CONTAINER(scrolledwindow3), sv->clist);
	gtk_clist_set_column_width(GTK_CLIST(sv->clist), 0, 100);
	gtk_clist_column_titles_hide(GTK_CLIST(sv->clist));

	frame4 = gtk_frame_new(NULL);
	gtk_widget_show(frame4);
	gtk_paned_pack2(GTK_PANED(vpaned_srch_rslt), frame4, TRUE,
			TRUE);

	scrolledwindow4 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow4);
	gtk_container_add(GTK_CONTAINER(frame4), scrolledwindow4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow4),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);


	sv->html_widget = gtk_html_new();
	gtk_widget_show(sv->html_widget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow4),
			  sv->html_widget);
	gtk_html_load_empty(GTK_HTML(sv->html_widget));
	/*
	   gtk_signal_connect(GTK_OBJECT(sv->clist),
	   "select_row",
	   G_CALLBACK
	   (on_clistSearchResults_select_row), NULL); */
}



/******************************************************************************
 * Name
 *   create_verse_list_page
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void create_verse_list_page(GtkWidget * notebook)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void create_verse_list_page(GtkWidget * notebook)
{
	GtkWidget *vbox;
	GtkWidget *frame1;
	GtkWidget *scrolledwindow1;
	GtkWidget *vl_html;
	GtkWidget *frame2;
	GtkWidget *scrolledwindow2;


	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(notebook), vbox);

	frame1 = gtk_frame_new(NULL);
	gtk_widget_show(frame1);
	gtk_box_pack_start(GTK_BOX(vbox), frame1, FALSE, TRUE, 0);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow1);
	gtk_container_add(GTK_CONTAINER(frame1), scrolledwindow1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_widget_set_usize(scrolledwindow1, -2, 75);

	vl_html = gtk_html_new();
	gtk_widget_show(vl_html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), vl_html);

	frame2 = gtk_frame_new(NULL);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox), frame2, TRUE, TRUE, 0);

	scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow2);
	gtk_container_add(GTK_CONTAINER(frame2), scrolledwindow2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow2),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	sv->htmlshow = gtk_html_new();
	gtk_widget_show(sv->htmlshow);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), sv->htmlshow);
	/*
	   gtk_signal_connect(GTK_OBJECT(vl_html), "link_clicked",
	   G_CALLBACK(verse_list_link_clicked), sv);
	 */
}

static GnomeUIInfo menu1_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("_Modules"),
	 NULL,
	 (gpointer) on_modules_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-book-red",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Bookmarks"),
	 NULL,
	 (gpointer) on_bookmarks_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-book-open",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Search"),
	 NULL,
	 (gpointer) on_search_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-find",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Search _Results"),
	 NULL,
	 (gpointer) on_search_results_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-find",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Viewer"),
	 NULL,
	 (gpointer) on_viewer_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-justify-left",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Verse _List"),
	 NULL,
	 (gpointer) on_verse_list_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-text-bulleted-list",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

/******************************************************************************
 * Name
 *   gui_create_sidebar
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   GtkWidget *gui_create_sidebar(GtkWidget * paned)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_sidebar(GtkWidget * paned)
{
	GtkWidget *vbox1;
	GtkWidget *toolbar2;
	GtkWidget *optionmenu1;
	GtkWidget *menu1;
	GtkWidget *scrolledwindow4;
	GtkWidget *treeview1;
	GtkWidget *label2;
	GtkWidget *scrolledwindow_bm;
	GtkWidget *scrolledwindow5;
	GtkWidget *treeview2;
	GtkWidget *label3;
	GtkWidget *empty_notebook_page;
	GtkWidget *label4;
	GtkWidget *scrolledwindow_search;
	GtkWidget *viewport_search;
	GtkWidget *vbox2;
	GtkWidget *vbox_search_results;
	GtkWidget *vbox_verse_list;
	GtkWidget *vbox_viewer;

	sv = &sb_v;
	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);
	gtk_paned_pack1(GTK_PANED(paned), vbox1, FALSE, TRUE);
	widgets.shortcutbar = vbox1;

	toolbar2 = gtk_toolbar_new();
	gtk_widget_show(toolbar2);
	gtk_box_pack_start(GTK_BOX(vbox1), toolbar2, FALSE, FALSE, 0);
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar2), GTK_TOOLBAR_BOTH);

	optionmenu1 = gtk_option_menu_new();
	gtk_widget_show(optionmenu1);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar2), optionmenu1,
				  NULL, NULL);

	menu1 = gtk_menu_new();
	gnome_app_fill_menu(GTK_MENU_SHELL(menu1), menu1_uiinfo,
			    NULL, FALSE, 0);

	gtk_option_menu_set_menu(GTK_OPTION_MENU(optionmenu1), menu1);

	widgets.notebook_sidebar = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_sidebar);
	gtk_box_pack_start(GTK_BOX(vbox1), widgets.notebook_sidebar,
			   TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(widgets.notebook_sidebar, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK
				   (widgets.notebook_sidebar), FALSE);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK
				    (widgets.notebook_sidebar), TRUE);

	scrolledwindow4 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow4);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  scrolledwindow4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow4),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	treeview1 = gtk_tree_view_new();
	gtk_widget_show(treeview1);
	gtk_container_add(GTK_CONTAINER(scrolledwindow4), treeview1);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview1),
					  FALSE);


	scrolledwindow_bm = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_bm);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  scrolledwindow_bm);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_bm),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	widgets.bookmark_tree = gtk_ctree_new(3, 0);
	gtk_widget_show(widgets.bookmark_tree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_bm),
			  widgets.bookmark_tree);
	gtk_clist_set_column_width(GTK_CLIST(widgets.bookmark_tree), 0,
				   280);
	gtk_clist_set_column_width(GTK_CLIST(widgets.bookmark_tree), 1,
				   80);
	gtk_clist_set_column_width(GTK_CLIST(widgets.bookmark_tree), 2,
				   80);


	scrolledwindow_search = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_search);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  scrolledwindow_search);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_search),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	viewport_search = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport_search);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_search),
			  viewport_search);


	create_search_results_page(widgets.notebook_sidebar);
	create_viewer_page(widgets.notebook_sidebar);
	create_verse_list_page(widgets.notebook_sidebar);

	vbox_viewer = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_viewer);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  vbox_viewer);

	load_module_tree(treeview1);
	gui_load_bookmark_tree();
	gui_create_shortcutbar_search(viewport_search);


	return vbox1;

}
