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
#include "gui/editor.h"
#include "gui/bookmarks_treeview.h"
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
#include "main/dictlex.h"
#include "main/module.h"

SIDEBAR sidebar;
static GtkWidget *vl_html;
static GtkWidget *menu1;
static gchar *s_module_name;
static gint button_mod_list;
static gint button_vl_html;
static gchar *buf_module;

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
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);

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
 *   gui_display_devotional_in_sidebar
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void gui_display_devotional_in_sidebar(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_display_devotional_in_sidebar(void)
{
	gchar buf[80];
	time_t curtime;
	struct tm *loctime;

	/*
	 * Get the current time.
	 */
	curtime = time(NULL);

	/*
	 * Convert it to local time representation.
	 */
	loctime = localtime(&curtime);

	/*
	 * Print it out in a nice format.
	 */
	strftime(buf, 80, "%m.%d", loctime);
	gtk_option_menu_set_history(GTK_OPTION_MENU
				    (sidebar.optionmenu1), 4);
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      4);
	gui_display_dictlex_in_sidebar(settings.devotionalmod, buf);
}


/******************************************************************************
 * Name
 *   gui_set_sidebar_porgram_start
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void gui_set_sidebar_porgram_start(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_set_sidebar_porgram_start(void)
{
	/*
	 *  show hide shortcut bar - set to options setting
	 */
	if (settings.showshortcutbar) {
		gtk_widget_show(widgets.shortcutbar);
		gtk_paned_set_position(GTK_PANED(widgets.epaned),
				       settings.shortcutbar_width);
	}

	else if (!settings.showshortcutbar && settings.showdevotional) {
		gtk_widget_show(widgets.shortcutbar);
		gui_sidebar_showhide();
	}

	else {
		gtk_widget_hide(widgets.shortcutbar);
		gtk_paned_set_position(GTK_PANED(widgets.epaned), 1);
	}

	/* set hight of bible and commentary pane */
	gtk_paned_set_position(GTK_PANED(widgets.vpaned),
			       settings.upperpane_hight);

	/* set width of bible pane */
	gtk_paned_set_position(GTK_PANED(widgets.hpaned),
			       settings.biblepane_width);

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
		settings.showshortcutbar = FALSE;
		settings.biblepane_width = settings.gs_width / 2;
		gtk_widget_hide(widgets.shortcutbar);
		gtk_paned_set_position(GTK_PANED(widgets.epaned), 0);
		gtk_paned_set_position(GTK_PANED
				       (widgets.hpaned),
				       settings.biblepane_width);
	} else {
		settings.showshortcutbar = TRUE;
		settings.biblepane_width =
		    (settings.gs_width -
		     settings.shortcutbar_width) / 2;
		gtk_paned_set_position(GTK_PANED(widgets.epaned),
				       settings.shortcutbar_width);
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       settings.biblepane_width);
		gtk_widget_show(widgets.shortcutbar);
	}
}

/******************************************************************************
 * Name
 *   gui_display_dictlex_in_sidebar
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gboolean gui_display_dictlex_in_sidebar(char *mod_name, char *key)
 *
 * Description
 *
 *
 * Return value
 *   gboolean
 */

gboolean gui_display_dictlex_in_sidebar(char *mod_name, char *key)
{
	if (settings.showshortcutbar) {
		gchar *text;
		gtk_option_menu_set_history(GTK_OPTION_MENU
					    (sidebar.optionmenu1), 4);
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_sidebar), 4);
		text = get_dictlex_text(mod_name, key);
		if (text) {
			entry_display(sidebar.html_viewer_widget,
				      mod_name, text, key, TRUE);
			free(text);
			return TRUE;
		}
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void verse_list_link_clicked(GtkHTML * html, const gchar * url,
 *				    SB_VIEWER * sv)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void verse_list_link_clicked(GtkHTML * html, const gchar * url,
				    gpointer data)
{
	int type;
	gchar *text = NULL;

	type = get_mod_type(s_module_name);
	switch (type) {
	case TEXT_TYPE:
		chapter_display(sidebar.htmlshow, sidebar.mod_name,
				NULL, (gchar *) url, FALSE);
		break;
	case COMMENTARY_TYPE:
	case DICTIONARY_TYPE:
		text = get_module_text(4, s_module_name, (gchar *) url);
		if (text) {
			entry_display(sidebar.htmlshow, s_module_name,
				      text, (gchar *) url, TRUE);
			free(text);
		}

		break;
	default:
		break;
	}

	if (settings.showinmain)
		gui_change_verse((gchar *) url);
}


/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void gui_display_verse_list_in_sidebar(gchar * key, gchar * module_name,
 *				       gchar * verse_list)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_display_verse_list_in_sidebar(gchar * key, gchar * module_name,
				       gchar * verse_list)
{
	GList *tmp = NULL;
	gboolean oddkey = TRUE;
	gchar buf[256], *utf8str, *colorkey;
	gchar *first_key = NULL;
	gchar *next_verse = NULL;
	gint i = 0;
	gint count = 0;
	GString *str;
	GtkTreeModel *model;
	GtkListStore *list_store;	
	GtkTreeSelection *selection;
	GtkTreePath *path;
	GtkTreeIter iter;

	strcpy(settings.sb_search_mod,module_name);
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.results_list));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);
	
	str = g_string_new("");
	s_module_name = g_strdup(module_name);
	strcpy(sidebar.mod_name, module_name);
	if (verse_list[0] == 'S' && verse_list[1] == 'e'
	    && verse_list[2] == 'e') {
		verse_list += 4;
	}
	if (verse_list[0] == 'c' && verse_list[1] == 'h'
	    && verse_list[2] == '.') {
		verse_list += 4;
		g_string_sprintf(str, "%s %s", get_book_from_key(key),
				 verse_list);
		verse_list = str->str;

	}
	for (i = 0; i < strlen(verse_list); i++) {
		if (verse_list[i] == '+')
			verse_list[i] = ' ';
		if (verse_list[i] == ',')
			verse_list[i] = ';';
	}
	i = 0;
	
	count = start_parse_verse_list(verse_list);
	while (count--) {
		next_verse = get_next_verse_list_element(i++);
		if (!next_verse)
			break;
		tmp = g_list_append(tmp, (gchar *) next_verse);
		//g_free(next_verse);
	}
	while (tmp != NULL) {
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, 0,
					   (const char *) tmp->data, -1);
		
		if (i == 0)
			first_key = g_strdup((const char *) tmp->data);
		++i;

		gui_display_html(vl_html, str->str, str->len);
		tmp = g_list_next(tmp);
	}
	
	selection = gtk_tree_view_get_selection
                                          (GTK_TREE_VIEW(sidebar.results_list));
	if(!gtk_tree_model_get_iter_first(model,&iter))
		return;
	path = gtk_tree_model_get_path(model,&iter);				
	gtk_tree_selection_select_path(selection,
                                             path);

	gtk_tree_path_free(path);
	
	gui_verselist_button_release_event(NULL,NULL,NULL);
	gtk_option_menu_set_history(GTK_OPTION_MENU
				    (sidebar.optionmenu1), 3);
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      3);
	g_string_free(str, TRUE);
}

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
	GtkTreeModel *model;



	model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_widget));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected))
		return;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0, &mod, -1);

	if (!mod)
		return;

	if (button_mod_list == 3) {
		if (!g_utf8_collate(mod, _("Parallel View"))) {
			g_free(mod);
			return;
		}
		buf_module = g_strdup(mod);
		gtk_menu_popup(GTK_MENU
			       (sidebar.menu_modules),
			       NULL, NULL, NULL, NULL,
			       0, gtk_get_current_event_time());
		return;
	}
	
	if (!g_utf8_collate(mod, _("Parallel View"))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (widgets.
					      button_parallel_view),
					     TRUE);
	}
	
	sbtype = get_mod_type(mod);
	switch (sbtype) {
	case 0:
		if(button_mod_list == 2) {
			gui_open_bibletext_dialog(mod);
			break;
		}
		if (GTK_TOGGLE_BUTTON(widgets.button_parallel_view)->
		    active) {
			//settings.MainWindowModule = mod;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (widgets.
						      button_parallel_view),
						     FALSE);
			    
		}
		gui_change_module_and_key(mod, settings. currentverse);
		break;
	case 1:
		if(button_mod_list == 2) {
			gui_open_commentary_dialog(mod);
			break;
		}
		gui_change_module_and_key(mod, settings.currentverse);
		break;
	case 2:
		if(button_mod_list == 2) {
			gui_open_dictlex_dialog(mod);
			break;
		}
		gtk_notebook_set_page
		    (GTK_NOTEBOOK(widgets.workbook_lower), 0);
		gui_change_module_and_key(mod, settings.dictkey);
		break;
	case 3:
		if(button_mod_list == 2) {
			gui_open_gbs_dialog(mod);
			break;
		}
		gtk_notebook_set_page
		    	(GTK_NOTEBOOK(widgets.workbook_lower), 1);
		gui_change_module_and_key(mod, NULL);
		break;
	}
	g_free(mod);
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
static void on_search_results_activate(GtkMenuItem * menuitem,
				       gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      3);

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

	sidebar.html_viewer_widget = gtk_html_new();
	gtk_widget_show(sidebar.html_viewer_widget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  sidebar.html_viewer_widget);
	gtk_html_load_empty(GTK_HTML(sidebar.html_viewer_widget));
/*
	gtk_signal_connect(GTK_OBJECT(sidebar.btn_save), "clicked",
			   G_CALLBACK(on_btnSBSaveVL_clicked), NULL);

	gtk_signal_connect(GTK_OBJECT(sidebar.tbtn_view_main), "toggled",
			   G_CALLBACK
			   (on_tbtnSBViewMain_toggled), NULL);

	gtk_signal_connect(GTK_OBJECT(sidebar.html_widget), "on_url",
			   G_CALLBACK(gui_url), widgets.app);
	*/
}


/******************************************************************************
 * Name
 *   on_modules_list_button_release_event
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gboolean on_modules_list_button_release_event(GtkWidget *widget,
 *                          GdkEventButton  *event, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   gboolean
 */

static gboolean on_modules_list_button_release_event(GtkWidget * widget,
						     GdkEventButton *
						     event,
						     gpointer user_data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter selected;
	gchar *key = NULL;
	gchar *text = NULL;

/*
	selection =
	    gtk_tree_view_get_selection((GtkTreeView *) sidebar.results_list);
	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.results_list));
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0, &key, -1);
	if (!key)
		return FALSE;


	switch (event->button) {
	case 1:
		break;
	case 2:
		gui_change_module_and_key(settings.sb_search_mod, key);
		break;
	case 3:

		break;
	default:
		break;
	}

	text = get_search_results_text(settings.sb_search_mod, key);
	if (text) {
		settings.displaySearchResults = TRUE;
		entry_display(sidebar.html_widget,
			      settings.sb_search_mod, text, key, TRUE);
		settings.displaySearchResults = FALSE;
		free(text);
	}
	free(key);*/
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_modules_list_button_press_event
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gboolean on_modules_list_button_press_event(GtkWidget *widget,
 *                           GdkEventButton  *event, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   gboolean
 */

static gboolean on_modules_list_button_press_event(GtkWidget * widget,
						   GdkEventButton *
						   event,
						   gpointer user_data)
{
	button_mod_list = 1;

	switch (event->button) {
	case 1:
		button_mod_list = 1;
		break;
	case 2:
		button_mod_list = 2;
		break;
	case 3:
		button_mod_list = 3;	
		break;
	default:
		break;
	}
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
	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.results_list));
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0, &key,
			   -1);
	if (!key)
		return FALSE;

	if(event) {
		switch (event->button) {
		case 1:
			break;
		case 2:
			gui_change_module_and_key(settings.sb_search_mod, key);
			break;
		case 3:
	
			break;
		default:
			break;
		}
	}
	text = get_search_results_text(settings.sb_search_mod, key);
	if (text) {
		settings.displaySearchResults = TRUE;
		entry_display(sidebar.html_widget,
			      settings.sb_search_mod, text, key, TRUE);
		settings.displaySearchResults = FALSE;
		free(text);
	}
	free(key);
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


	switch (event->button) {
	case 1:
		break;
	case 2:

		break;
	case 3:
		return TRUE;
		break;
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

void on_save_list_as_bookmarks_activate(GtkMenuItem * menuitem,
					gpointer user_data)
{
	gui_verselist_to_bookmarks(settings.sb_search_mod);
}


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

void on_open_in_dialog_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gint module_type;

	module_type = get_mod_type(buf_module);
	switch (module_type) {
	case -1:
		break;
	case TEXT_TYPE:
		gui_open_bibletext_dialog(buf_module);
		break;
	case COMMENTARY_TYPE:
		gui_open_commentary_dialog(buf_module);
		break;
	case DICTIONARY_TYPE:
		gui_open_dictlex_dialog(buf_module);
		break;
	case BOOK_TYPE:
		gui_open_gbs_dialog(buf_module);
		break;
	}
	g_free(buf_module);
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

void on_about2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_display_about_module_dialog(buf_module, FALSE);
	g_free(buf_module);
}


static GnomeUIInfo results_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Save List"),
	 N_("Save the search results as bookmarks"),
	 (gpointer) on_save_list_as_bookmarks_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-text-numbered-list",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};


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
	GtkWidget *menu1;

	menu1 = gtk_menu_new();
	gnome_app_fill_menu(GTK_MENU_SHELL(menu1), results_menu_uiinfo,
			    NULL, FALSE, 0);
	sidebar.menu_item_save_search = results_menu_uiinfo[0].widget;
	gtk_widget_set_sensitive(sidebar.menu_item_save_search, FALSE);
	return menu1;
}

static GnomeUIInfo menu_modules_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Open in dialog"),
	 N_("Open selected module in a dialog"),
	 (gpointer) on_open_in_dialog_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_FILENAME, PACKAGE_PIXMAPS_DIR "/dlg-un.png",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("About"),
	 N_("View information about the selected dialog"),
	 (gpointer) on_about2_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-about",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

/******************************************************************************
 * Name
 *   create_menu_modules
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *    GtkWidget* create_menu_modules (void)
 *
 * Description
 *
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *create_menu_modules(void)
{
	GtkWidget *menu_modules;

	menu_modules = gtk_menu_new();
	gnome_app_fill_menu(GTK_MENU_SHELL(menu_modules),
			    menu_modules_uiinfo, NULL, FALSE, 0);
	return menu_modules;
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
	GtkListStore *model;
	GObject *selection;
	GtkWidget *menu = create_results_menu();

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(notebook), vbox);
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

	/* create list model */
	model = gtk_list_store_new(1, G_TYPE_STRING);

	sidebar.results_list =
	    gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	gtk_widget_show(sidebar.results_list);
	gtk_container_add(GTK_CONTAINER(scrolledwindow3),
			  sidebar.results_list);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW
				     (sidebar.results_list), TRUE);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW
					  (sidebar.results_list),
					  FALSE);
	add_columns(GTK_TREE_VIEW(sidebar.results_list));

	selection = G_OBJECT(gtk_tree_view_get_selection
			     (GTK_TREE_VIEW(sidebar.results_list)));

	frame4 = gtk_frame_new(NULL);
	gtk_widget_show(frame4);
	gtk_paned_pack2(GTK_PANED(vpaned_srch_rslt), frame4, TRUE,
			TRUE);

	gnome_popup_menu_attach(menu, sidebar.results_list, NULL);
	gnome_app_install_menu_hints(GNOME_APP(widgets.app),
				     results_menu_uiinfo);
	scrolledwindow4 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow4);
	gtk_container_add(GTK_CONTAINER(frame4), scrolledwindow4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow4),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);


	sidebar.html_widget = gtk_html_new();
	gtk_widget_show(sidebar.html_widget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow4),
			  sidebar.html_widget);
	gtk_html_load_empty(GTK_HTML(sidebar.html_widget));


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

/*static void create_verse_list_page(GtkWidget * notebook)
{
	GtkWidget *vbox;
	GtkWidget *frame1;
	GtkWidget *scrolledwindow1;
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

	sidebar.htmlshow = gtk_html_new();
	gtk_widget_show(sidebar.htmlshow);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2),
			  sidebar.htmlshow);

	g_signal_connect(G_OBJECT(vl_html), "link_clicked",
			   G_CALLBACK(verse_list_link_clicked), NULL);

}
*/
static GnomeUIInfo menu1_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("_Modules"),
	 NULL,
	 (gpointer) on_modules_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK,
	 "gnome-stock-book-red", 0,
	 (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM,
	 N_("_Bookmarks"), NULL,
	 (gpointer) on_bookmarks_activate,
	 NULL,
	 NULL, GNOME_APP_PIXMAP_STOCK,
	 "epiphany-bookmarks", 0,
	 (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM,
	 N_("_Search"),
	 NULL,
	 (gpointer) on_search_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK,
	 "gtk-find",
	 0,
	 (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM,
	 N_("Verse _List"),
	 NULL,
	 (gpointer) on_search_results_activate, NULL,
	 NULL,
	 GNOME_APP_PIXMAP_STOCK,
	 "gnome-stock-text-bulleted-list",
	 0,
	 (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM,
	 N_("_Viewer"),
	 NULL,
	 (gpointer) on_viewer_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK,
	 "gtk-justify-left",
	 0,
	 (GdkModifierType)
	 0,
	 NULL},
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
	GtkWidget *scrolledwindow4;
	GtkWidget *label2;
	GtkWidget *scrolledwindow_bm;
	GtkWidget *scrolledwindow5;
	GtkWidget *treeview2;
	GtkWidget *label3;
	GtkWidget *empty_notebook_page;
	GtkWidget *label4;
	GtkWidget *vbox2;
	GtkWidget *vbox_search_results;
	GtkWidget *vbox_verse_list;
	GtkWidget *vbox_viewer;
	GObject *mod_selection;

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);
	gtk_paned_pack1(GTK_PANED(paned), vbox1, FALSE, TRUE);
	widgets.shortcutbar = vbox1;

	sidebar.optionmenu1 = gtk_option_menu_new();
	gtk_widget_show(sidebar.optionmenu1);
	gtk_box_pack_start(GTK_BOX(vbox1), sidebar.optionmenu1, FALSE,
			   TRUE, 0);

	menu1 = gtk_menu_new();
	gnome_app_fill_menu(GTK_MENU_SHELL(menu1), menu1_uiinfo,
			    NULL, FALSE, 0);

	gtk_option_menu_set_menu(GTK_OPTION_MENU(sidebar.optionmenu1),
				 menu1);

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
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	sidebar.module_list = gtk_tree_view_new();
	gtk_widget_show(sidebar.module_list);
	gtk_container_add(GTK_CONTAINER(scrolledwindow4),
			  sidebar.module_list);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW
					  (sidebar.module_list), FALSE);


	scrolledwindow_bm = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_bm);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  scrolledwindow_bm);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_bm),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	widgets.bookmark_tree = gui_create_bookmark_tree();
	gtk_widget_show(widgets.bookmark_tree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_bm),
			  widgets.bookmark_tree);


	gui_create_search_sidebar();

	create_search_results_page(widgets.notebook_sidebar);
	create_viewer_page(widgets.notebook_sidebar);

	vbox_viewer = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_viewer);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  vbox_viewer);

	gui_load_module_tree(sidebar.module_list, TRUE);
	mod_selection =
	    G_OBJECT(gtk_tree_view_get_selection
		     (GTK_TREE_VIEW(sidebar.module_list)));
		     
	g_signal_connect((gpointer) sidebar.module_list,
			 "button_press_event",
			 G_CALLBACK(on_modules_list_button_press_event),
			 NULL);

	g_signal_connect(mod_selection, "changed",
			 G_CALLBACK(mod_selection_changed),
			 sidebar.module_list);

	sidebar.menu_modules = create_menu_modules();

	return vbox1;

}
