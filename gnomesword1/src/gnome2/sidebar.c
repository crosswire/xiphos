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
#include <gtkhtml/htmlengine.h>
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
#include "gui/html.h"

#include "main/sword.h"
#include "main/gbs.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/dictlex.h"
#include "main/module.h"
#include "main/xml.h"

SIDEBAR sidebar;
static GtkWidget *vl_html;
static GtkWidget *menu1;
static gchar *s_module_name;
static gint button_mod_list;
static gint button_vl_html;
static gchar *buf_module;
GList *list_of_verses;



/******************************************************************************
 * Name
 *   get_module_from_url
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gchar *get_module_from_url(const gchar * url)
 *
 * Description
 *   returns a module name when passed a url like:
 *   version="KJV" passage="John 3:16"
 *
 * Return value
 *   gchar*
 */ 

static gchar *get_module_from_url(const gchar * url)
{
	gchar *mybuf = NULL;
	gchar *retval = NULL;
	gchar newmod[80];
	gint i = 0;
	
	mybuf = strstr(url, "version=");
	if(mybuf != NULL) {
		i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			while (i < strlen(mybuf)) {
				newmod[i] = mybuf[i];
				newmod[i + 1] = '\0';
				++i;
			}
		}
		if (check_for_module(newmod))
			retval = g_strdup(newmod);
		else 
			retval = g_strdup(xml_get_value("modules", "bible"));
	} 
	return retval;	
}


/******************************************************************************
 * Name
 *   get_verse_from_url
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gchar *get_verse_from_url(const gchar * url)
 *
 * Description
 *   returns a verse reference when passed a url like:
 *   version="KJV" passage="John 3:16"
 *
 * Return value
 *   gchar*
 */ 

static gchar *get_verse_from_url(const gchar * url)
{
	gchar *mybuf = NULL;
	gchar *retval = NULL;
	gchar newref[80];
	gint i = 0;
	
	mybuf = strstr(url, "passage=");
	if(mybuf != NULL) {
		i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			while (i < strlen(mybuf)) {
				newref[i] = mybuf[i];
				newref[i + 1] = '\0';
				++i;
			}
		}
		retval = g_strdup(newref);
	} 
	return retval;	
}


/******************************************************************************
 * Name
 *   on_close_button_clicked
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void on_close_button_clicked(GtkButton * button,  gpointer user_data)
 *
 * Description
 *    hide the sidebar
 *
 * Return value
 *   void
 */
 
static void on_close_button_clicked(GtkButton * button,  gpointer user_data)
{
	gui_sidebar_showhide();
}


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

static void on_notebook_switch_page(GtkNotebook *notebook,
				    GtkNotebookPage *page,
                                    guint page_num,
                                    gpointer user_data)
{
 	GtkWidget *label;
	
	label = GTK_WIDGET(user_data);
	switch(page_num) {
		case 0:
			gtk_label_set_text (GTK_LABEL(label), _("Modules"));
		break;
		case 1:
			gtk_label_set_text (GTK_LABEL(label), _("Bookmarks"));
		break;
		case 2:
			gtk_label_set_text (GTK_LABEL(label), _("Search"));
		break;
		case 3:
			gtk_label_set_text (GTK_LABEL(label), _("Verse List"));
		break;
		case 4:
			gtk_label_set_text (GTK_LABEL(label), _("Viewer"));
		break;
	}
	
}


/******************************************************************************
 * Name
 *   gui_link_clicked
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void link_clicked(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

static void link_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar *buf = NULL;
	gchar *mod_name = NULL;
	gchar *key = NULL;
	
	buf = g_strdup(url);
	key = get_verse_from_url(buf);
	if(buf) g_free(buf);
	buf = g_strdup(url);
	mod_name = get_module_from_url(buf);	
	gui_change_module_and_key(mod_name, key);
	g_free(buf);
	g_free(mod_name);
	g_free(key);
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
	/*gtk_option_menu_set_history(GTK_OPTION_MENU
				    (sidebar.optionmenu1), 4);*/
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
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
		/*gtk_option_menu_set_history(GTK_OPTION_MENU
					    (sidebar.optionmenu1), 4);*/
		gtk_notebook_set_current_page(GTK_NOTEBOOK
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
	RESULTS *list_item;
	
	
	list_of_verses = g_list_first(list_of_verses);
	if(list_of_verses) {
		while(list_of_verses) {
			g_free(list_of_verses->data);
			list_of_verses = g_list_next(list_of_verses);
		}
		g_list_free(list_of_verses);
	}
	list_of_verses = NULL;
	
	strcpy(settings.sb_search_mod,module_name);
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.results_list));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);
	
	strcpy(sidebar.mod_name, module_name);
	
	if (verse_list[0] == 'S' && verse_list[1] == 'e'
	    && verse_list[2] == 'e') {
		verse_list += 4;
	}
	i = 0;
	
	count = start_parse_verse_list(verse_list,key);
	while (count--) {
		next_verse = get_next_verse_list_element(i++);
		if (!next_verse)
			break;
		list_item = g_new(RESULTS,1);
		list_item->module = module_name;
		list_item->key = (gchar *) next_verse;
		list_of_verses = g_list_append(list_of_verses, 
						(RESULTS *) list_item);
	}
	tmp = list_of_verses;
	while (tmp != NULL) {
		list_item = (RESULTS *) tmp->data;
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, 0,
					   (const char *) list_item->key, -1);
		
		if (i == 0)
			first_key = g_strdup((const char *) list_item->key);
		++i;
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
	/*gtk_option_menu_set_history(GTK_OPTION_MENU
				    (sidebar.optionmenu1), 3);*/
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      3);
	//g_string_free(str, TRUE);
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
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
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
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
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
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      2);
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
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      3);

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
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      4);
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
	g_signal_connect(GTK_OBJECT(sidebar.btn_save), "clicked",
			   G_CALLBACK(on_btnSBSaveVL_clicked), NULL);

	g_signal_connect(GTK_OBJECT(sidebar.tbtn_view_main), "toggled",
			   G_CALLBACK
			   (on_tbtnSBViewMain_toggled), NULL);

	g_signal_connect(GTK_OBJECT(sidebar.html_widget), "on_url",
			   G_CALLBACK(gui_url), widgets.app);
	*/
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

static gboolean on_modules_list_button_release(GtkWidget * widget,
						   GdkEventButton *
						   event,
						   gpointer user_data)
{
	GtkTreeSelection * selection;
	gint sbtype;
	GtkTreeIter selected;
	gchar *mod = NULL;
	gint mod_type;
	GtkTreeModel *model;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(sidebar.module_list));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.module_list));
	
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return FALSE;
	if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected))
		return FALSE;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0, &mod, -1);

	if (!mod)
		return FALSE;
	
	button_mod_list = 1;

	switch (event->button) {
	case 1:
		button_mod_list = 1;
		break;
	case 2:
		button_mod_list = 2;
		break;
	case 3:
		if (!g_utf8_collate(mod, _("Parallel View"))) {
			g_free(mod);
			return FALSE;
		}
		buf_module = g_strdup(mod);
		gtk_menu_popup(GTK_MENU
			       (sidebar.menu_modules),
			       NULL, NULL, NULL, NULL,
			       0, gtk_get_current_event_time());
		return FALSE;
			
		break;
	}
	
	
	if (!g_utf8_collate(mod, _("Parallel View"))) 
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (widgets. button_parallel_view),
					     TRUE);
	
	
	sbtype = get_mod_type(mod);
	switch (sbtype) {
	case 0:
		if(button_mod_list == 2) {
			gui_open_bibletext_dialog(mod);
			break;
		}
		if (GTK_TOGGLE_BUTTON(widgets.button_parallel_view)->
		    active) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (widgets.
						      button_parallel_view),
						     FALSE);
		}
		gui_change_module_and_key(mod, settings.currentverse);
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
		gtk_notebook_set_current_page
		    (GTK_NOTEBOOK(widgets.workbook_lower), 0);
		gui_change_module_and_key(mod, settings.dictkey);
		break;
	case 3:
		if(button_mod_list == 2) {
			gui_open_gbs_dialog(mod);
			break;
		}
		gtk_notebook_set_current_page
		    	(GTK_NOTEBOOK(widgets.workbook_lower), 1);
		gui_change_module_and_key(mod, NULL);
		break;
	}
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
			//gui_change_module_and_key(settings.sb_search_mod, key);
			break;
		case 2:
			gui_open_verse_in_new_tab(key);
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
 *   vpaned_srch_rslt_button_release_event
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gboolean vpaned_srch_rslt_button_release_event(GtkWidget *widget,
 *                           GdkEventButton  *event, gpointer user_data)
 *
 * Description
 *   This function is called when the pane in the search result window is
 *   resized.  It saves the position in the settings.
 *
 * Return value
 *   gboolean
 */

gboolean vpaned_srch_rslt_button_release_event(GtkWidget * widget,
					       GdkEventButton * event,
					       gpointer user_data)
{
	gint panesize;
	gchar layout[80];

	panesize = gtk_paned_get_position(GTK_PANED(widget));

	settings.verselist_toppane_height = panesize;
	sprintf(layout, "%d", settings.verselist_toppane_height);
	xml_set_value("GnomeSword", "layout", "vltoppaneheight", layout);
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
	gui_verselist_to_bookmarks(list_of_verses);
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
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_TEXT_NUMBERED_LIST,
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
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_ABOUT,
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
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  gboolean on_button_release_event(GtkWidget * widget,
			    GdkEventButton * event, TEXT_DATA * t)	
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

static gboolean on_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{	
	gchar *key;
	const gchar *url;
	gchar *buf = NULL;
	
	switch (event->button) {
	case 1:		
		break;
	case 2:		
		url = html_engine_get_link_at (GTK_HTML(data)->engine,
					 event->x,
					 event->y);		
		buf = get_verse_from_url(url);
		if(buf) {
			gui_open_verse_in_new_tab(buf);
			g_free(buf);	
		}			
		break;
	case 3:
		break;
	}
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
	if (settings.verselist_toppane_height == 0)
		settings.verselist_toppane_height = 100;
	gtk_paned_set_position(GTK_PANED(vpaned_srch_rslt),
				settings.verselist_toppane_height);

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
//	gtk_widget_set_size_request (frame4, 100, -1);

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

	g_signal_connect(GTK_OBJECT(sidebar.html_widget), 
			 "link_clicked",
			 G_CALLBACK(link_clicked),
			 NULL);
	g_signal_connect((gpointer) sidebar.html_widget,
			 "button_release_event",
			 G_CALLBACK(on_button_release_event),
			 sidebar.html_widget);

	g_signal_connect((gpointer) sidebar.results_list,
			 "button_release_event",
			 G_CALLBACK(gui_verselist_button_release_event),
			 NULL);
	g_signal_connect((gpointer) sidebar.results_list,
			 "button_press_event",
			 G_CALLBACK(on_treeview_button_press_event),
			 NULL);
	g_signal_connect(GTK_OBJECT(vpaned_srch_rslt),
			"button_release_event",
			G_CALLBACK(vpaned_srch_rslt_button_release_event),
			NULL);
}


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
 *   
 */

static void menu_position_under (GtkMenu *menu, 
		     int *x, 
		     int *y,
		     gboolean *push_in,
		     gpointer user_data)
{
	GtkWidget *widget;
	
	g_return_if_fail (GTK_IS_BUTTON (user_data));
	g_return_if_fail (GTK_WIDGET_NO_WINDOW (user_data));

	widget = GTK_WIDGET (user_data);
	
	gdk_window_get_origin (widget->window, x, y);
	
	*x += widget->allocation.x;
	*y += widget->allocation.y + widget->allocation.height;

	*push_in = FALSE;
}


/******************************************************************************
 * Name
 *   select_button_press_callback
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   gboolean select_button_press_callback (GtkWidget *widget,
 *			      GdkEventButton *event,
 *			      gpointer user_data)
 *
 * Description
 *    make the tooglebutton act like a gtk optionmenu by dropping a popup
 *    under the button
 *
 * Return value
 *   gboolean
 */

static gboolean select_button_press_callback (GtkWidget *widget,
			      GdkEventButton *event,
			      gpointer user_data)
{
	extern GtkWidget *menu;
	
	if ((event->type == GDK_BUTTON_PRESS) && event->button == 1) {
		gtk_widget_grab_focus (widget);
		
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
		gtk_menu_popup (GTK_MENU (sidebar.optionmenu1),
				NULL, NULL, menu_position_under, widget, 
				event->button, event->time);

		return TRUE;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   menu_deactivate_callback
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   void menu_deactivate_callback (GtkWidget *widget, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void menu_deactivate_callback (GtkWidget *widget, gpointer user_data)
{
	GtkWidget *menu_button;
	
	menu_button = GTK_WIDGET (user_data);
		
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (menu_button), FALSE);
}


static GnomeUIInfo menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, 
	 N_("_Modules"),
	 NULL,
	 (gpointer) on_modules_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK,
	 GNOME_STOCK_BOOK_RED, 0,
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
	 GTK_STOCK_FIND,
	 0,
	 (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM,
	 N_("Verse _List"),
	 NULL,
	 (gpointer) on_search_results_activate, NULL,
	 NULL,
	 GNOME_APP_PIXMAP_STOCK,
	 GNOME_STOCK_TEXT_BULLETED_LIST,
	 0,
	 (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM,
	 N_("_Viewer"),
	 NULL,
	 (gpointer) on_viewer_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK,
	 GTK_STOCK_JUSTIFY_LEFT,
	 0,
	 (GdkModifierType)
	 0,
	 NULL},
	GNOMEUIINFO_END
};


/******************************************************************************
 * Name
 *   create_menu
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *
 *   GtkWidget* create_menu(void)
 *
 * Description
 *
 *
 * Return value
 *   GtkWidget*
 */

static GtkWidget* create_menu(void)
{
  GtkWidget *menu;

  menu = gtk_menu_new ();
  gnome_app_fill_menu (GTK_MENU_SHELL (menu), menu_uiinfo,
                       NULL, FALSE, 0);
  return menu;
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
	GtkWidget *frame;
	GtkWidget *hbox;
	GtkWidget *select_button;
	GtkWidget *alignment1;
	GtkWidget *select_hbox;
	GtkWidget *title_label;
	GtkWidget *arrow;
	GtkWidget *close_button;
	GtkWidget *image;
	GtkWidget *shortcut_box;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GObject *mod_selection;

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);
	gtk_paned_pack1(GTK_PANED(paned), vbox1, FALSE, TRUE);
	widgets.shortcutbar = vbox1;

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox1), frame, FALSE, FALSE, 0);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_container_add (GTK_CONTAINER (frame), hbox);
	
	select_button = gtk_toggle_button_new ();
	gtk_button_set_relief (GTK_BUTTON (select_button), GTK_RELIEF_NONE);
	gtk_widget_show (select_button);
	
	g_signal_connect (select_button, 
			  "button_press_event",
			  G_CALLBACK (select_button_press_callback),
			  NULL);
			  
	alignment1 = gtk_alignment_new (0.03, 0.5, 0, 0);
	gtk_widget_show (alignment1);
	gtk_container_add (GTK_CONTAINER (select_button), alignment1);
	
	select_hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (select_hbox);
	
	title_label = gtk_label_new (_("Modules"));
	
	gtk_widget_show (title_label);
	gtk_box_pack_start (GTK_BOX (select_hbox), 
				title_label,
				FALSE, FALSE, 0);
	
	arrow = gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_NONE);
	gtk_widget_show (arrow);
	gtk_box_pack_start (GTK_BOX (select_hbox), arrow, FALSE, FALSE, 0);
	
	gtk_container_add (GTK_CONTAINER (alignment1), select_hbox); 
	gtk_box_pack_start (GTK_BOX (hbox), select_button, FALSE, TRUE, 0);
	
	close_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (close_button), GTK_RELIEF_NONE);
	
			  
	gtk_widget_show (close_button);
	
	image = gtk_image_new_from_stock (GTK_STOCK_CLOSE, 
					  GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (image);
	
	gtk_container_add (GTK_CONTAINER (close_button), image);
	
	g_signal_connect ((gpointer) close_button, "clicked",
                    G_CALLBACK (on_close_button_clicked),
                    title_label);
		    
	gtk_box_pack_end (GTK_BOX (hbox), close_button, FALSE, FALSE, 0);
	
	
	shortcut_box = gtk_hbox_new (TRUE, 0);
	gtk_widget_show (shortcut_box);
	gtk_box_pack_end (GTK_BOX (hbox),
			  shortcut_box,
			  TRUE, TRUE, 0);

/***/
	widgets.notebook_sidebar = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_sidebar);
	gtk_box_pack_start(GTK_BOX(vbox1), widgets.notebook_sidebar,
			   TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(widgets.notebook_sidebar, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK
				   (widgets.notebook_sidebar), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK
				     (widgets.notebook_sidebar), FALSE);

	g_signal_connect ((gpointer) widgets.notebook_sidebar, "switch-page",
                    G_CALLBACK (on_notebook_switch_page),
                    title_label);
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

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Found",
							     renderer,
							     "text", 0,
							     NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gtk_tree_view_append_column(GTK_TREE_VIEW(sidebar.module_list),
					    column);

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
	
	gui_load_module_tree(sidebar.module_list, TRUE);
		     
	g_signal_connect((gpointer) sidebar.module_list,
			 "button_release_event",
			 G_CALLBACK(on_modules_list_button_release),
			 NULL);

	sidebar.optionmenu1 = create_menu();
	g_signal_connect (sidebar.optionmenu1,
			  "deactivate",
			  G_CALLBACK (menu_deactivate_callback),
			  select_button);
	sidebar.menu_modules = create_menu_modules();

	return vbox1;
}
