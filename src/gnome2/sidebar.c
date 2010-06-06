/*
 * Xiphos Bible Study Tool
 * sidebar.c - create and maintain the new sidebar bar
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
#include <config.h>
#endif

#include <gtkhtml/gtkhtml.h>
#include <glade/glade-xml.h>

#include "editor/slib-editor.h"


#include "gui/sidebar.h"
#include "gui/bookmarks_treeview.h"
#include "gui/export_bookmarks.h"
#include "gui/utilities.h"
#include "gui/about_modules.h"
#include "gui/main_window.h"
#include "gui/html.h"
#include "gui/xiphos.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/widgets.h"
#include "gui/search_sidebar.h"
#include "gui/tabbed_browser.h"
#include "gui/search_dialog.h"

#ifdef USE_GTKMOZEMBED
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "webkit/wk-html.h"
#endif
#endif

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/prayerlists.h"
#include "main/previewer.h"
#include "main/sidebar.h"
#include "main/url.hh"
#include "main/xml.h"
#include "main/module_dialogs.h"

#include "gui/debug_glib_null.h"


SIDEBAR sidebar;
static GtkWidget *button_bookmarks;
static GtkWidget *button_search;
static GtkWidget *button_v_lists;
static GtkWidget *button_modules;
static gchar *buf_module;
GList *list_of_verses;
GtkListStore *model_verselist;
gboolean is_search_result;

extern gboolean shift_key_pressed;


static GtkWidget* create_menu_modules(void);
void on_export_verselist_activate (GtkMenuItem * menuitem, gpointer user_data);


/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void  gui_sync_module_treeview(gint direction)
{

}


#ifdef USE_TREEVIEW_PATH
/******************************************************************************
 * Name
 *   gui_save_treeview_path_string
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void gui_save_treeview_path_string (const gchar * path_str, const gchar * book_name)
 *
 * Description
 *   saves a books's treeview path in .xiphos/book_path.conf
 *
 * Return value
 *   void
 */
void gui_save_treeview_path_string (const gchar * path_str, const gchar * book_name)
{
	gchar *file = g_strdup_printf("%s/book_path.conf", settings.gSwordDir);
	save_conf_file_item(file, book_name, "PATH", path_str);
	GS_message (("book %s, path %s, file %s\n", book_name, path_str, file));
	g_free(file);
}


/******************************************************************************
 * Name
 *   gui_collapse_treeview_to_book
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void gui_collapse_treeview_to_book (GtkTreeView * tree, const gchar * book_name)
 *
 * Description
 *   collapses a book treeview to it's name - it's called by tabbed browsing
 *   before the next book is expanded
 *
 * Return value
 *   void
 */


void gui_collapse_treeview_to_book (GtkTreeView * tree, const gchar * book_name)
{
	gchar file[250];
	gchar *path_string = NULL;
	gchar *tmp_path_string = NULL;
	gchar **work_buf = NULL;
	GtkTreePath *path;

	sprintf(file, "%s/book_path.conf", settings.gSwordDir);
	path_string = get_conf_file_item (file, (gchar*)book_name, "PATH");

	if (!path_string) return;

	work_buf = g_strsplit ( path_string, ":", 4);

	tmp_path_string = g_strdup_printf ("%s:%s:%s",
					   work_buf[0],
					   work_buf[1],
					   work_buf[2]);

	path = gtk_tree_path_new_from_string ((gchar *)tmp_path_string);
	gtk_tree_view_collapse_row (tree, path);

	gtk_tree_path_free(path);
	g_free (path_string);
	g_free (tmp_path_string);
	g_strfreev(work_buf);
}

/******************************************************************************
 * Name
 *   gui_expand_treeview_to_path
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gboolean gui_expand_treeview_to_path (GtkTreeView * tree, const gchar * book_name)
 *
 * Description
 *   expands a books treeview to it's last path called by tabbed browsing when
 *   a tab is shown that has a book  (and soon by the book editor - I hope)
 *
 * Return value
 *   gboolean
 */

gboolean gui_expand_treeview_to_path (GtkTreeView * tree, const gchar * book_name)
{
	gchar file[250];
	gchar *path_string = NULL;
	gchar *tmp_path_string = NULL;
	gchar *mod = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeSelection *selection;
	gchar **work_buf = NULL;
	gint i = 3;

	sprintf(file, "%s/book_path.conf", settings.gSwordDir);
	path_string = get_conf_file_item (file, (gchar*)book_name, "PATH");
	if (!path_string) return 0;

	work_buf = g_strsplit ( path_string, ":", -1);
	GS_message (("\n\nbuf[0]: %s\nbuf[1]: %s\nbuf[2]: %s\n\n",
		     work_buf[0],
		     work_buf[1],
		     work_buf[2]));
	model = gtk_tree_view_get_model (tree);

	tmp_path_string = g_strdup_printf ("%s:%s:%s",
					   work_buf[0],
					   work_buf[1],
					   work_buf[2]);
	if (gtk_tree_model_get_iter_from_string (model, &iter,
						 (gchar *)tmp_path_string)) {
		gtk_tree_model_get(model, &iter,  3, &mod, -1);
		if (!g_utf8_collate(mod, book_name)) {
			selection = gtk_tree_view_get_selection (tree);
			path = gtk_tree_path_new_from_string ((gchar *)tmp_path_string);
			gtk_tree_view_expand_to_path (tree, path);
			gtk_tree_selection_select_path (selection, path);

			main_expand_treeview_to_path(model,iter);
			gtk_tree_path_free(path);
			while (work_buf[i]) {
				GS_message (("\n\nwork_buf[%d]: %s\n\n",
					     i,
					     work_buf[i]));

				tmp_path_string =
					g_strdup_printf ("%s:%s",
							 tmp_path_string,
							 work_buf[i]);
				gtk_tree_model_get_iter_from_string (model,
								     &iter,
								     (gchar *)tmp_path_string);

				GS_message (("\n\nmod: %s\npath: %s\n\n",
					     mod,
					     tmp_path_string));
				path = gtk_tree_path_new_from_string (
						   (gchar *)tmp_path_string);
				gtk_tree_view_expand_to_path (tree, path);
				gtk_tree_selection_select_path (selection, path);

				main_expand_treeview_to_path(model,iter);
				gtk_tree_path_free(path);
				++i;
			}
		}
		g_free (mod);
	}
	path = gtk_tree_path_new_from_string ((gchar *)tmp_path_string);
	gtk_tree_view_scroll_to_cell (tree,
                                       path,
                                       NULL,
                                       FALSE,
                                       0.0,
                                       0.0);
	gtk_tree_path_free(path);

	g_free (tmp_path_string);

	g_strfreev(work_buf);

	return 1;
}
#endif /* USE_TREEVIEW_PATH */

/******************************************************************************
 * Name
 *   on_notebook_switch_page
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_notebook_switch_page(GtkNotebook *notebook,
 *				    GtkNotebookPage *page,
 *                                  guint page_num,
 *                                  gpointer user_data)
 *
 * Description
 *   sets the sidebar menu button label to the current page
 *
 * Return value
 *   void
 */

static void on_notebook_switch_page(GtkNotebook * notebook,
				    GtkNotebookPage * page,
				    guint page_num, gpointer user_data)
{
	switch (page_num) {
	case 0:
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_v_lists), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_bookmarks), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_search), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_modules), TRUE);
		break;

	case 1:
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_v_lists), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_bookmarks), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_search), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_modules), FALSE);
		break;

	case 2:
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_v_lists), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_bookmarks), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_search), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_modules), FALSE);
		break;

	case 3:
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_v_lists), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_bookmarks), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_search), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_modules), FALSE);
		break;
	}
}


/******************************************************************************
 * Name
 *   add_columns
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void add_columns(GtkTreeView * treeview)
 *
 * Description
 *   add columns to listview
 *
 * Return value
 *   void
 */

static void add_columns(GtkTreeView * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes("Results",
							  renderer,
							  "text", 0,
							  NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gtk_tree_view_append_column(treeview, column);
}


/******************************************************************************
 * Name
 *   gui_set_sidebar_program_start
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void gui_set_sidebar_program_start(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_set_sidebar_program_start(void)
{
	/*
	 *  show hide shortcut bar - set to options setting
	 */
	if (settings.showshortcutbar) {
		gtk_widget_show(widgets.shortcutbar);
		gtk_paned_set_position(GTK_PANED(widgets.epaned),
				       settings.sidebar_width);
	}
/*
	else if (!settings.showshortcutbar && settings.showdevotional) {
		gtk_widget_show(widgets.shortcutbar);
		gui_sidebar_showhide();
	}
*/
	else {
		gtk_widget_hide(widgets.shortcutbar);
		gtk_paned_set_position(GTK_PANED(widgets.epaned), 1);
	}

	/* set hight of bible and commentary pane */
	//gtk_paned_set_position(GTK_PANED(widgets.vpaned),settings.upperpane_hight);

	/* set width of bible pane */
	//gtk_paned_set_position(GTK_PANED(widgets.hpaned), settings.biblepane_width);

	if (!settings.docked) {
		settings.docked = TRUE;
		//gui_attach_detach_shortcutbar();
	}
}


/******************************************************************************
 * Name
 *  gui_sidebar_showhide
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void gui_sidebar_showhide(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_sidebar_showhide(void)
{
	if (!settings.docked) {
		gdk_window_raise(GTK_WIDGET(widgets.dock_sb)->window);
		return;
	}

	if (settings.showshortcutbar) {
		xml_set_value("Xiphos", "misc", "show_sidebar",	"0");
		settings.showshortcutbar = FALSE;
		gtk_widget_hide(widgets.shortcutbar);
		sync_windows();
		settings.biblepane_width = GTK_WIDGET(widgets.vpaned)->allocation.width;

	} else {
		xml_set_value("Xiphos", "misc", "show_sidebar",	"1");
		settings.showshortcutbar = TRUE;
		gtk_paned_set_position(GTK_PANED(widgets.epaned),
				       settings.sidebar_width);
		gtk_widget_show(widgets.shortcutbar);
		sync_windows();
		settings.biblepane_width = GTK_WIDGET(widgets.vpaned)->allocation.width;

	}
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

static void on_modules_activate(GtkToggleButton * button,
				gpointer user_data)
{
	if (button->active) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.
							   notebook_sidebar), 0);
	}
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

static void on_bookmarks_activate(GtkToggleButton * button,
				  gpointer user_data)
{
	if (button->active) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.
							   notebook_sidebar), 1);
	}
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

static void on_search_activate(GtkToggleButton * button,
			       gpointer user_data)
{
	if (button->active) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.
							   notebook_sidebar), 2);
	}
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
static void on_search_results_activate(GtkToggleButton * button,
				       gpointer user_data)
{
	if (button->active) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.
							   notebook_sidebar), 3);
	}
}


/******************************************************************************
 * Name
 *   on_modules_list_button_release
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gboolean on_modules_list_button_release(GtkWidget *widget,
 *                           GdkEventButton  *event, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   gboolean
 */

static gboolean on_modules_list_button_release(GtkWidget *widget,
					       GdkEventButton *event,
					       gpointer user_data)
{
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GtkTreeModel *model;
	gchar *mod = NULL;
	gchar *caption = NULL;

	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(sidebar.module_list));

	if (!gtk_tree_selection_get_selected(selection, &model, &selected))
		return FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 2, &caption, 3, &mod, -1);

	//uncomment the following two lines if you want to see how it looks without
	//triangls or plus symbols
	//gtk_tree_view_set_show_expanders(sidebar.module_list, FALSE);
	//gtk_tree_view_set_level_indentation(sidebar.module_list, 12);
	GtkTreePath *path = gtk_tree_model_get_path(model, &selected);
	if (gtk_tree_view_row_expanded (GTK_TREE_VIEW(sidebar.module_list), path))
	       gtk_tree_view_collapse_row ( GTK_TREE_VIEW(sidebar.module_list), path );
        else
	       gtk_tree_view_expand_row ( GTK_TREE_VIEW(sidebar.module_list), path, FALSE );
	gtk_tree_path_free ( path );

	switch (event->button) {
	case 1:
		main_mod_treeview_button_one(model, selected);
		break;

	case 2:
		if (mod && (g_utf8_collate(mod, _("Parallel View")))
		    	&& (g_utf8_collate(mod, _("Standard View"))))
			gui_open_module_in_new_tab(mod);
		break;

	case 3:
		if (mod && (g_utf8_collate(mod, _("Parallel View")))
		    	&& (g_utf8_collate(mod, _("Standard View")))//) {
		    	&& (main_get_mod_type(mod) != PRAYERLIST_TYPE)) {
			buf_module = mod;
			create_menu_modules ();
			/*gtk_menu_popup(GTK_MENU(sidebar.menu_modules),
				       NULL, NULL, NULL, NULL,
				       0, gtk_get_current_event_time());*/
			g_free(caption);
			return FALSE;
		}
	    	if (caption && (!g_utf8_collate(caption, _("Prayer List/Journal")))) {
			gui_menu_prayerlist_popup(NULL, NULL);
			g_free(mod);
			return FALSE;
		}
	    	if (mod && (main_get_mod_type(mod) == PRAYERLIST_TYPE)) {
			buf_module = mod;
			gtk_menu_popup(GTK_MENU(sidebar.menu_prayerlist_mod),
				       NULL, NULL, NULL, NULL,
				       0, gtk_get_current_event_time());
			g_free(caption);
			return FALSE;
		}

		break;
	}
	g_free(caption);
	g_free(mod);
	return FALSE;
}


/******************************************************************************
 * Name
 *   gui_verselist_button_release_event
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gboolean gui_verselist_button_release_event(GtkWidget *widget,
 *                          GdkEventButton  *event, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   gboolean
 */

gboolean gui_verselist_button_release_event(GtkWidget * widget,
					    GdkEventButton * event,
					    gpointer user_data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter selected;
	gchar *key = NULL;
	gchar *text = NULL;

	selection =
	    gtk_tree_view_get_selection((GtkTreeView *) sidebar.results_list);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.results_list));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0, &key, -1);
	if (!key)
		return FALSE;

	if (event) {
		switch (event->button) {
		case 2:
			gui_open_passage_in_new_tab(key);
			/* fall through... */

		case 3:				/* button 3 is no-op */
			return FALSE;

		default:			/* button 1 handled below */
			break;
		}
	}
	// UTF-8 workaround (domcox)
	// main_get_search_results_text (renderText) doesn't render
	// non-ascii chars saved in numeric character reference format
	// in personal notes.
	// Notes are now saved in utf8 encoding (Xiphos > 2.2.2.1)
	if (main_get_mod_type(settings.sb_search_mod) == PERCOM_TYPE)
		text = main_get_raw_text(settings.sb_search_mod, key);
	else
		text = main_get_search_results_text(settings.sb_search_mod, key);

	if (text) {
		settings.displaySearchResults = TRUE;
		main_entry_display(settings.show_previewer_in_sidebar
				     ? sidebar.html_viewer_widget
				     : widgets.html_previewer_text, //sidebar.html_widget,
			      settings.sb_search_mod, text, key, TRUE);
		settings.displaySearchResults = FALSE;
		g_free(text);
		gtk_widget_grab_focus(sidebar.results_list);
	}

	g_free(key);
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_treeview_button_press_event
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gboolean on_treeview_button_press_event(GtkWidget *widget,
 *                           GdkEventButton  *event, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   gboolean
 */

static gboolean on_treeview_button_press_event(GtkWidget * widget,
					       GdkEventButton * event,
					       gpointer user_data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter selected;
	gchar *key = NULL;

	selection =
	    gtk_tree_view_get_selection((GtkTreeView *) sidebar.results_list);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.results_list));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0, &key, -1);
	if (!key)
		return FALSE;

	if (event->type == GDK_2BUTTON_PRESS)
	{
		if (verse_selected)
			g_free(verse_selected);
		verse_selected = g_strdup_printf("sword:///%s", key);
		main_url_handler(verse_selected, TRUE);
	}
	switch (event->button) {
	case 3:
		gtk_menu_popup ((GtkMenu*)sidebar.menu_item_save_search, NULL, NULL, NULL, NULL, 2,
		     			gtk_get_current_event_time());
		return TRUE;

	default:
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_save_list_as_bookmarks_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_save_list_as_bookmarks_activate (GtkMenuItem *menuitem,
 *                                       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */
/*
G_MODULE_EXPORT static void on_save_list_as_bookmarks_activate(GtkMenuItem * menuitem,
					       gpointer user_data)
{
	gui_verselist_to_bookmarks(list_of_verses);
}
*/

/******************************************************************************
 * Name
 *   on_open_in_dialog_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_open_in_dialog_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void
on_open_in_dialog_activate(GtkMenuItem * menuitem,
				       gpointer user_data)
{
	if (main_get_mod_type(buf_module) == PERCOM_TYPE)
		editor_create_new((gchar *)buf_module,
				  (gchar *)settings.currentverse,
				  NOTE_EDITOR);
	else if (main_get_mod_type(buf_module) == PRAYERLIST_TYPE)
		editor_create_new((gchar *)buf_module,
				  "0",
				  BOOK_EDITOR);
	else
		main_dialogs_open(buf_module, NULL);

	g_free(buf_module);
	buf_module = NULL;
}


/******************************************************************************
 * Name
 *   on_open_in_tab_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_open_in_tab_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void
on_open_in_tab_activate2(GtkMenuItem * menuitem,
				    gpointer user_data)
{
	gui_open_module_in_new_tab(buf_module);
	g_free(buf_module);
	buf_module = NULL;
}


/******************************************************************************
 * Name
 *   on_about2_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_about2_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void
on_about2_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gui_display_about_module_dialog(buf_module);
	g_free(buf_module);
	buf_module = NULL;
}


G_MODULE_EXPORT void
on_save_list_as_a_single_bookmark_activate (GtkMenuItem * menuitem,
                                        gpointer user_data)
{
	gui_verselist_to_bookmarks(list_of_verses, TRUE);
}


G_MODULE_EXPORT void
on_save_list_as_a_series_of_bookmarks_activate (GtkMenuItem * menuitem,
                                        gpointer user_data)
{
	gui_verselist_to_bookmarks(list_of_verses, FALSE);
}


G_MODULE_EXPORT void
on_export_verselist_activate (GtkMenuItem * menuitem,
                                        gpointer user_data)
{
	gui_export_bookmarks_dialog((is_search_result?SEARCH_RESULTS_EXPORT:VERSE_LIST_EXPORT), list_of_verses);
}

/******************************************************************************
 * Name
 *   create_results_menu
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   GtkWidget* create_results_menu (void)
 *
 * Description
 *
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *create_results_menu(void)
{
	GtkWidget *menu;
	gchar *glade_file;
	GladeXML *gxml;

	glade_file = gui_general_user_file ("xi-menus.glade", FALSE);
	g_return_val_if_fail ((glade_file != NULL), NULL);

	gxml = glade_xml_new (glade_file, "menu_verselist", NULL);

	g_free (glade_file);
	g_return_val_if_fail ((gxml != NULL), NULL);

	menu = glade_xml_get_widget (gxml, "menu_verselist");
    	/* connect signals and data */
	glade_xml_signal_autoconnect_full
		(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func, NULL);

	return menu;
}

/******************************************************************************
 * Name
 *   create_menu_modules
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *    void create_menu_modules (void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */
static
GtkWidget* create_menu_modules(void)
{
	gchar *glade_file;
	GladeXML *gxml;

	glade_file = gui_general_user_file ("xi-menus.glade", FALSE);
	g_return_val_if_fail ((glade_file != NULL), NULL);

	gxml = glade_xml_new (glade_file, "menu_modules", NULL);

	g_free (glade_file);
	g_return_val_if_fail ((gxml != NULL), NULL);

	GtkWidget *menu 	= glade_xml_get_widget (gxml, "menu_modules");
	glade_xml_signal_autoconnect_full
		(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func, NULL);

	gtk_menu_popup ((GtkMenu*)menu, NULL, NULL, NULL, NULL, 2,
			gtk_get_current_event_time());

	return menu;
}


G_MODULE_EXPORT void
on_simple_activate(GtkMenuItem *menuitem,
		   gpointer    user_data)
{
	main_prayerlist_basic_create();
}

G_MODULE_EXPORT void
on_subject_activate(GtkMenuItem *menuitem,
		    gpointer    user_data)
{
	main_prayerlist_subject_create();
}

G_MODULE_EXPORT void
on_monthly_activate(GtkMenuItem *menuitem,
		    gpointer    user_data)
{
	main_prayerlist_monthly_create();
}

G_MODULE_EXPORT void
on_journal_activate(GtkMenuItem *menuitem,
		    gpointer    user_data)
{
	main_prayerlist_journal_create();
}

G_MODULE_EXPORT void
on_outlined_topic_activate(GtkMenuItem *menuitem,
			   gpointer    user_data)
{
	main_prayerlist_outlined_topic_create();
}

GtkWidget *create_menu_prayerlist(void)
{
	GtkWidget *menu;
	gchar *glade_file;
	GladeXML *gxml;

	glade_file = gui_general_user_file ("xi-menus.glade", FALSE);
	g_return_val_if_fail ((glade_file != NULL), NULL);

	gxml = glade_xml_new (glade_file, "menu_prayerlist", NULL);

	g_free (glade_file);
	g_return_val_if_fail ((gxml != NULL), NULL);

	menu = glade_xml_get_widget (gxml, "menu_prayerlist");
	glade_xml_signal_autoconnect_full
		(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func, NULL);
	return menu;

}

void
on_edit_activate(GtkMenuItem *menuitem,
		 gpointer    user_data)
{
	editor_create_new(buf_module, NULL, BOOK_EDITOR);
}

GtkWidget *create_menu_prayerlist_mod(void)
{
	GtkWidget *menu;
	gchar *glade_file;
	GladeXML *gxml;

	glade_file = gui_general_user_file ("xi-menus.glade", FALSE);
	g_return_val_if_fail ((glade_file != NULL), NULL);

	gxml = glade_xml_new (glade_file, "menu_prayerlist_mod", NULL);

	g_free (glade_file);
	g_return_val_if_fail ((gxml != NULL), NULL);

	menu = glade_xml_get_widget (gxml, "menu_prayerlist_mod");
	glade_xml_signal_autoconnect_full
		(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func, NULL);
	return menu;
}

G_MODULE_EXPORT void gui_menu_prayerlist_popup(GtkMenuItem *menuitem, gpointer user_data)
{
	gtk_menu_popup(GTK_MENU(sidebar.menu_prayerlist),
		       NULL, NULL, NULL, NULL,
		       0, gtk_get_current_event_time());
}

static void tree_selection_changed_cb(GtkTreeSelection * selection,
				      gpointer data)
{
	gui_verselist_button_release_event(NULL, NULL, NULL);
}


static gboolean tree_key_press_cb(GtkWidget * widget,
				  GdkEventKey * event,
				  gpointer user_data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter selected;
	gchar *key = NULL;

	selection = gtk_tree_view_get_selection((GtkTreeView *) sidebar.results_list);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW (sidebar.results_list));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0, &key, -1);
	if (!key)
		return FALSE;

	GS_warning(("%d", event->keyval));
	if (event) {

		switch (event->keyval) {
		case 0xff0d:	/* "65293" */
		case 0xff8d:	/* "65421" */
			{
				gchar *url = g_strdup_printf("sword://%s/%s",
							     settings.sb_search_mod,
							     key);
				main_url_handler(url, TRUE);
				g_free(url);
			}
			break;

		case 0x20:	/* "32" */
			gui_open_passage_in_new_tab(key);
			sync_windows();
			break;

		case 0xffe1:	/* shift keys */
		case 0xffe2:
			GS_warning(("shift key pressed"));
			shift_key_pressed =  TRUE;
			sync_windows();
			break;

		default:
			break;
		}
	}
	g_free(key);

	sync_windows();

	gtk_widget_grab_focus(sidebar.results_list);
	return FALSE;
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
	GtkWidget *scrolledwindow3;
	GtkTreeSelection *selection;
	sidebar.menu_item_save_search = create_results_menu();

	scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow3);
	gtk_container_add(GTK_CONTAINER(notebook), scrolledwindow3);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow3),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow3,
					    settings.shadow_type);

	/* create list model */
	model_verselist = gtk_list_store_new(1, G_TYPE_STRING);

	sidebar.results_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model_verselist));
	gtk_widget_show(sidebar.results_list);
	gtk_container_add(GTK_CONTAINER(scrolledwindow3), sidebar.results_list);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(sidebar.results_list), TRUE);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(sidebar.results_list), FALSE);
	add_columns(GTK_TREE_VIEW(sidebar.results_list));

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(sidebar.results_list));


	g_signal_connect((gpointer) sidebar.results_list,
			 "key_press_event",
			 G_CALLBACK(tree_key_press_cb), NULL);
	g_signal_connect((gpointer) selection,
			 "changed",
			 G_CALLBACK(tree_selection_changed_cb), NULL);
	g_signal_connect((gpointer) sidebar.results_list,
			 "button_release_event",
			 G_CALLBACK(gui_verselist_button_release_event),
			 NULL);
	g_signal_connect((gpointer) sidebar.results_list,
			 "button_press_event",
			 G_CALLBACK(on_treeview_button_press_event),
			 NULL);
}


/******************************************************************************
 * Name
 *   paned_button_release_event
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   gboolean paned_button_release_event(GtkWidget * widget,
 *			GdkEventButton * event, gpointer user_data)
 *
 * Description
 *    get and store pane sizes
 *
 * Return value
 *   void
 */

static gboolean paned_button_release_event(GtkWidget * widget,
					   GdkEventButton * event,
					   gpointer user_data)
{
	gint panesize;
	gchar layout[80];

	panesize = gtk_paned_get_position(GTK_PANED(widget));

	if (panesize > 15) {
		settings.sidebar_notebook_hight = panesize;
		sprintf(layout, "%d", settings.sidebar_notebook_hight);
		xml_set_value("Xiphos", "layout",
			      "sidebar_notebook_hight", layout);
	}
	return FALSE;
}

void gui_show_previewer_in_sidebar(gint choice)
{
	if (choice) {
		gtk_widget_show(widgets.box_side_preview);
#ifdef USE_GTKMOZEMBED
		gtk_widget_show (widgets.box_side_preview);
#else
		gtk_widget_show_all (widgets.box_side_preview);
#endif
		gtk_widget_hide(widgets.vbox_previewer);
		gtk_paned_set_position(GTK_PANED(widgets.paned_sidebar),
				       settings.sidebar_notebook_hight);
	} else {
#ifdef USE_GTKMOZEMBED
		gtk_widget_show (widgets.vbox_previewer);
#else
		gtk_widget_show_all (widgets.vbox_previewer);
#endif
		gtk_widget_hide(widgets.box_side_preview);
		gtk_paned_set_position(GTK_PANED(widgets.vpaned),
				       settings.biblepane_hight);
	}
	main_set_previewer_widget(choice);
	main_init_previewer();
}

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
	GtkWidget *scrolledwindow4;
	GtkWidget *scrolledwindow_bm;
	GtkWidget *title_label = NULL;
#ifdef USE_GTKMOZEMBED
	GtkWidget *frame;
	GtkWidget *eventbox;
#else
	GtkWidget *scrolledwindow;
#endif

	GtkWidget *table2;

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);

	widgets.paned_sidebar = gtk_vpaned_new();
	gtk_paned_pack1 (GTK_PANED (paned), widgets.paned_sidebar, FALSE, TRUE);
	gtk_widget_show (widgets.paned_sidebar);
	gtk_paned_pack1 (GTK_PANED (widgets.paned_sidebar), vbox1, FALSE, TRUE);
	widgets.box_side_preview = gtk_vbox_new(FALSE, 0);
	gtk_paned_pack2 (GTK_PANED (widgets.paned_sidebar),
			widgets.box_side_preview, FALSE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(widgets.box_side_preview), 2);
	g_signal_connect (GTK_OBJECT (widgets.paned_sidebar),
			"button_release_event",
			G_CALLBACK (paned_button_release_event),
			(gchar *) "paned_sidebar");
	widgets.shortcutbar = widgets.paned_sidebar;

#ifdef USE_GTKMOZEMBED
	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(widgets.box_side_preview), frame,
				TRUE, TRUE,
			   	0);


	eventbox = gtk_event_box_new();
	gtk_widget_show(eventbox);
	gtk_container_add(GTK_CONTAINER(frame), eventbox);
	
	sidebar.html_viewer_widget = GTK_WIDGET(wk_html_new());//embed_new(VIEWER_TYPE);
	gtk_container_add(GTK_CONTAINER(eventbox), sidebar.html_viewer_widget);
#else
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(widgets.box_side_preview), scrolledwindow, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow,
					    settings.shadow_type);
	sidebar.html_viewer_widget = gtk_html_new();
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  sidebar.html_viewer_widget);
	g_signal_connect(GTK_OBJECT(sidebar.html_viewer_widget),
			 "link_clicked", G_CALLBACK(gui_link_clicked),
			 NULL);
#endif /* USE_GTKMOZEMBED*/
	gtk_widget_show(sidebar.html_viewer_widget);

	/* ---------------------------------------------------------------- */
	/* 2x2 button box set: modules/bookmarks/search/vlist */
	/* ---------------------------------------------------------------- */
	table2 = gtk_table_new(2, 2, TRUE);
	gtk_widget_show(table2);
	gtk_box_pack_start(GTK_BOX(vbox1), table2, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(table2), 2);
	gtk_table_set_row_spacings(GTK_TABLE(table2), 6);
	gtk_table_set_col_spacings(GTK_TABLE(table2), 6);

	button_bookmarks = gtk_toggle_button_new_with_mnemonic(_("Bookmarks"));
	gtk_widget_show(button_bookmarks);
	gtk_table_attach(GTK_TABLE(table2), button_bookmarks, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_button_set_relief(GTK_BUTTON(button_bookmarks), GTK_RELIEF_HALF);
	gtk_button_set_focus_on_click(GTK_BUTTON(button_bookmarks), FALSE);

	button_search = gtk_toggle_button_new_with_mnemonic(_("Search"));
	gtk_widget_show(button_search);
	gtk_table_attach(GTK_TABLE(table2), button_search, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_button_set_relief(GTK_BUTTON(button_search), GTK_RELIEF_HALF);
	gtk_button_set_focus_on_click(GTK_BUTTON(button_search), FALSE);

	button_v_lists = gtk_toggle_button_new_with_mnemonic(_("Verse List"));
	gtk_widget_show(button_v_lists);
	gtk_table_attach(GTK_TABLE(table2), button_v_lists, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_button_set_relief(GTK_BUTTON(button_v_lists), GTK_RELIEF_HALF);
	gtk_button_set_focus_on_click(GTK_BUTTON(button_v_lists), FALSE);

	button_modules = gtk_toggle_button_new_with_mnemonic(_("Modules"));
	gtk_widget_show(button_modules);
	gtk_table_attach(GTK_TABLE(table2), button_modules, 0, 1, 0, 1,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_button_set_relief(GTK_BUTTON(button_modules), GTK_RELIEF_HALF);
	gtk_button_set_focus_on_click(GTK_BUTTON(button_modules), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_modules), TRUE);
	/* ---------------------------------------------------------------- */

	widgets.notebook_sidebar = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_sidebar);

	gtk_box_pack_start(GTK_BOX(vbox1), widgets.notebook_sidebar, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(widgets.notebook_sidebar, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_sidebar), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.notebook_sidebar), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(widgets.notebook_sidebar), 2);

	scrolledwindow4 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow4);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  scrolledwindow4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow4),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow4,
					    settings.shadow_type);

	sidebar.module_list = gtk_tree_view_new();
	gtk_widget_show(sidebar.module_list);
	gtk_container_add(GTK_CONTAINER(scrolledwindow4),
			  sidebar.module_list);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(sidebar.module_list), FALSE);
	main_create_pixbufs();
	main_add_mod_tree_columns(GTK_TREE_VIEW(sidebar.module_list));

	scrolledwindow_bm = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_bm);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  scrolledwindow_bm);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_bm),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow_bm,
					    settings.shadow_type);

	widgets.bookmark_tree = gui_create_bookmark_tree();
	gtk_widget_show(widgets.bookmark_tree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_bm),
			  widgets.bookmark_tree);

	gui_create_search_sidebar();

	create_search_results_page(widgets.notebook_sidebar);

	main_load_module_tree(sidebar.module_list);

	g_signal_connect_after((gpointer) sidebar.module_list,
			       "button_release_event",
			       G_CALLBACK
			       (on_modules_list_button_release), NULL);

	// sidebar.menu_modules = create_menu_modules();
	sidebar.menu_prayerlist = create_menu_prayerlist();
	sidebar.menu_prayerlist_mod = create_menu_prayerlist_mod();

	g_signal_connect((gpointer) button_bookmarks, "toggled",
			 G_CALLBACK(on_bookmarks_activate), NULL);
	g_signal_connect((gpointer) button_search, "toggled",
			 G_CALLBACK(on_search_activate), NULL);
	g_signal_connect((gpointer) button_v_lists, "toggled",
			 G_CALLBACK(on_search_results_activate), NULL);
	g_signal_connect((gpointer) button_modules, "toggled",
			 G_CALLBACK(on_modules_activate), NULL);

	g_signal_connect((gpointer) widgets.notebook_sidebar,
			 "switch-page",
			 G_CALLBACK(on_notebook_switch_page),
			 title_label);

	return vbox1;
}
