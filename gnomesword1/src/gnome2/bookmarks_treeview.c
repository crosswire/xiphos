/*
 * GnomeSword Bible Study Tool
 * bookmarks_treeview.c - gui for bookmarks using treeview
 *
 * Copyright (C) 2003 GnomeSword Developer Team
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
#include <libxml/parser.h>
//#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "gui/gnomesword.h"
#include "gui/bookmarks_treeview.h"
#include "gui/bookmarks_menu.h"
#include "gui/utilities.h"
#include "gui/main_window.h"
#include "gui/dialog.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/sidebar.h"
#include "gui/toolbar_nav.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/module_dialogs.h"
#include "main/url.hh"

//TreePixbufs *pixbufs;
GtkTreeStore *model;
gboolean button_one;
gboolean button_two;
gboolean use_dialog;
GtkTreeSelection *current_selection;
GtkTreeView *bookmark_tree;
static gboolean save_reorderable;
static void add_item_to_tree(GtkTreeIter * iter, GtkTreeIter * parent,
			     BOOKMARK_DATA * data);

BookMarksPixbufs *bm_pixbufs;
/******************************************************************************
 * Name
 *   gui_verselist_to_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void gui_verselist_to_bookmarks(GList * list)
 *
 * Description
 *   add results of search to tree as a root node with children
 *
 * Return value
 *   void
 */

void gui_verselist_to_bookmarks(GList * verses)
{
	gint test;
	const gchar *key_buf = NULL;
	gchar *tmpbuf;
	gchar *module_name;
	GString *str;
	GtkTreeIter parent;
	GtkTreeIter iter;
	GS_DIALOG *info;
	RESULTS *list_item;

	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &iter);
	info = gui_new_dialog();
	info->stock_icon = GTK_STOCK_OPEN;
	info->title = N_("Bookmark");
	info->label_top = N_("Enter Folder Name");
	info->text1 = g_strdup(settings.searchText);
	info->label1 = N_("Folder: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	/*** open dialog to get name for root node ***/

	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		gtk_tree_store_append(GTK_TREE_STORE(model), &parent,
				      &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &parent,
				   COL_OPEN_PIXBUF,
				   bm_pixbufs->pixbuf_opened,
				   COL_CLOSED_PIXBUF,
				   bm_pixbufs->pixbuf_closed,
				   COL_CAPTION, info->text1, COL_KEY,
				   NULL, COL_MODULE, NULL, -1);
//              set_results_position((char) 1); // TOP 
		str = g_string_new(" ");
		while (verses) {
			list_item = (RESULTS *) verses->data;
			module_name = list_item->module;
			tmpbuf = list_item->key;
			g_string_sprintf(str, "%s, %s", tmpbuf,
					 module_name);
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &iter, &parent);
			gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
					   COL_OPEN_PIXBUF,
					   bm_pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION, str->str,
					   COL_KEY, tmpbuf, COL_MODULE,
					   module_name, -1);
			verses = g_list_next(verses);
		}
		g_string_free(str, TRUE);
		bookmarks_changed = TRUE;
		gui_save_bookmarks_treeview();
	}
	g_free(info->text1);
	g_free(info);
	gtk_widget_set_sensitive(sidebar.menu_item_save_search, FALSE);
}

/******************************************************************************
 * Name
 *   gui_save_old_bookmarks_to_new
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void gui_save_old_bookmarks_to_new(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   save bookmark tree 
 *
 * Return value
 *   void
 */

void gui_save_old_bookmarks_to_new(GNode * gnode)
{
	gchar buf[256];

	sprintf(buf, "%s/bookmarks/%s", settings.gSwordDir,
		"bookmarks.xml");
	xml_save_gnode_to_bookmarks(gnode, g_strdup(buf));
}


/******************************************************************************
 * Name
 *   get_xml_folder_data
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void get_xml_folder_data(xmlNodePtr cur, BOOKMARK_DATA * data)
 *
 * Description
 *    get date from xml bookmark folder and put into 
 *    BOOKMARK_DATA structure
 *
 * Return value
 *   void
 */

static void get_xml_folder_data(xmlNodePtr cur, BOOKMARK_DATA * data)
{
	xmlChar *folder;

	folder = xmlGetProp(cur, (const xmlChar *) "caption");
	data->caption = g_strdup((char *)folder);
	data->key = NULL;	//g_strdup("GROUP");
	data->module = NULL;	//g_strdup("GROUP");
	data->module_desc = NULL;	//g_strdup("GROUP");
	data->description = NULL;	//g_strdup("GROUP");
	data->is_leaf = FALSE;
	data->opened = bm_pixbufs->pixbuf_opened;
	data->closed = bm_pixbufs->pixbuf_closed;
}


/******************************************************************************
 * Name
 *    get_xml_bookmark_data
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void get_xml_bookmark_data(xmlNodePtr cur, BOOKMARK_DATA * data)	
 *
 * Description
 *    get date from xml bookmark and put into BOOKMARK_DATA structure
 *
 * Return value
 *   void
 */

static void get_xml_bookmark_data(xmlNodePtr cur, BOOKMARK_DATA * data)
{
	xmlChar *mod1;
	xmlChar *key;
	xmlChar *caption;
	xmlChar *mod_desc;
	xmlChar *description;
	gchar buf[500];
	gchar *url = NULL;

	data->opened = bm_pixbufs->pixbuf_helpdoc;
	data->closed = NULL;
	mod1 = xmlGetProp(cur, (const xmlChar *)"modulename");
	key = xmlGetProp(cur, (const xmlChar *)"key");
	//caption = xmlGetProp(cur, "caption"); 
	mod_desc = xmlGetProp(cur, (const xmlChar *)"moduledescription");
	description = xmlGetProp(cur, (const xmlChar *)"description");
/*	if (caption) {
		if (strlen(caption) > 0) {
			data->caption = g_strdup(caption);
		} else {
			sprintf(buf, "%s, %s", key, mod1);
			data->caption = g_strdup(buf);
		}
	} else {*/
	caption = xmlGetProp(cur, (const xmlChar *)"description");
	if ((char *)caption) {
		if (strlen((char *)caption) > 0) {
			data->caption = g_strdup((char *)caption);
		} else {
			sprintf(buf, "%s, %s", key, mod1);
			data->caption = g_strdup((char *)buf);
		}
	} else
		data->caption = g_strdup((char *)key);
//      }
/*	if(strstr(key,"sword://"))
		url = g_strdup(key);
	else 
		url = g_strdup_printf("sword://%s/%s",mod1,key);
*/
	data->key = g_strdup((char *)key);
	data->module = g_strdup((char *)mod1);
	data->description = g_strdup((char *)description);
	data->module_desc = g_strdup((char *)mod_desc);
	data->is_leaf = TRUE;
//      if(url) g_free(url);
}


/******************************************************************************
 * Name
 *  free_bookmark_data
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void free_bookmark_data(BOOKMARK_DATA *data)
 *
 * Description
 *    frees the BOOKMARK_DATA structure
 *
 * Return value
 *   void
 */

static void free_bookmark_data(BOOKMARK_DATA * data)
{
	if (data->caption)
		g_free(data->caption);
	if (data->key)
		g_free(data->key);
	if (data->module)
		g_free(data->module);
	if (data->module_desc)
		g_free(data->module_desc);
	if (data->description)
		g_free(data->description);
}

/******************************************************************************
 * Name
 *  add_node_to_ctree
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   GtkCTreeNode *add_node_to_ctree(GtkCTree * ctree, 
 *			GtkCTreeNode *node, BOOKMARK_DATA * data)	
 *
 * Description
 *    actually add the GtkCTreeNode to the bookmark ctree
 *
 * Return value
 *   GtkCTreeNode
 */

static void add_item_to_tree(GtkTreeIter * iter, GtkTreeIter * parent,
			     BOOKMARK_DATA * data)
{
	gtk_tree_store_append(GTK_TREE_STORE(model), iter, parent);

	gtk_tree_store_set(GTK_TREE_STORE(model), iter,
			   COL_OPEN_PIXBUF, data->opened,
			   COL_CLOSED_PIXBUF, data->closed,
			   COL_CAPTION, data->caption,
			   COL_KEY, data->key,
			   COL_MODULE, data->module,
			   COL_MODULE_DESC, data->module_desc,
			   COL_DESCRIPTION, data->description, -1);
}

/******************************************************************************
 * Name
 *  add_node
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void add_node(xmlDocPtr doc, xmlNodePtr cur, GtkCTree * ctree,
						GtkCTreeNode *node)	
 *
 * Description
 *    parse the xml bookmarks and add to bookmark ctree
 *
 * Return value
 *   void
 */

static void add_node(xmlNodePtr cur, GtkTreeIter * parent)
{
	GtkTreeIter iter;
	BOOKMARK_DATA data, *p = NULL;
	xmlNodePtr work = NULL;

	p = &data;
	if (cur == NULL)
		return;

	for (work = cur->xmlChildrenNode; work; work = work->next) {
		if (!xmlStrcmp
		    (work->name, (const xmlChar *) "Bookmark")) {
			get_xml_bookmark_data(work, p);
			add_item_to_tree(&iter, parent, p);
			free_bookmark_data(p);
		} else if (!xmlStrcmp
			   (work->name, (const xmlChar *) "Folder")) {
			get_xml_folder_data(work, p);
			add_item_to_tree(&iter, parent, p);
			free_bookmark_data(p);
			add_node(work, &iter);
		}
	}
}


/******************************************************************************
 * Name
 *  gui_parse_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void gui_parse_bookmarks(GtkCTree * ctree)	
 *
 * Description
 *    load a xml bookmark file
 *
 * Return value
 *   void
 */

void parse_bookmarks(GtkTreeView * tree, const xmlChar * file,
		     GtkTreeIter * parent)
{
	xmlNodePtr cur = NULL;
	BOOKMARK_DATA data, *p = NULL;
	GtkTreeIter iter;
	GtkTreePath *path;

	p = &data;
	cur = xml_load_bookmark_file(file);
	//cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *) "Bookmark")) {
			get_xml_bookmark_data(cur, p);
			add_item_to_tree(&iter, parent, p);
			free_bookmark_data(p);
		} else {
			get_xml_folder_data(cur, p);
			if (p->caption) {
				add_item_to_tree(&iter, parent, p);
			}
			free_bookmark_data(p);
			add_node(cur, &iter);
		}

		if (cur->next)
			cur = cur->next;
		else
			break;
	}
	xml_free_bookmark_doc();
	path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), parent);
	gtk_tree_view_expand_to_path(tree, path);
	gtk_tree_path_free(path);
}

/******************************************************************************
 * Name
 *   gui_load_removed
 *
 * Synopsis
 *   #include "gui/bookmarks_treview.h"
 *
 *   void gui_load_removed()
 *
 * Description
 *    restore bookmarks that have been removed and saved
 *
 * Return value
 *   void
 */

void gui_load_removed(const xmlChar * file)
{
	xmlNodePtr cur = NULL;
	BOOKMARK_DATA data, *p = NULL;
	GtkTreeIter iter;
	GtkTreeIter parent;
	GtkTreePath *path;
	gchar *caption = NULL;
	GtkTreeSelection *selection;
	GtkTreeIter selected;

	selection = gtk_tree_view_get_selection(bookmark_tree);
	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;
	p = &data;

	cur = xml_load_bookmark_file(file);
	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *) "Bookmark")) {
			get_xml_bookmark_data(cur, p);
			add_item_to_tree(&iter, &parent, p);
			free_bookmark_data(p);
		} else {
			get_xml_folder_data(cur, p);
			if (p->caption) {
				add_item_to_tree(&parent, &selected, p);
			}
			free_bookmark_data(p);
			add_node(cur, &parent);
		}

		if (cur->next)
			cur = cur->next;
		else
			break;
	}
	xml_free_bookmark_doc();
	path =
	    gtk_tree_model_get_path(GTK_TREE_MODEL(model), &selected);
	gtk_tree_view_expand_to_path(bookmark_tree, path);
	gtk_tree_path_free(path);
}


/******************************************************************************
 * Name
 *  load_xml_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void load_xml_bookmarks(GtkCTree * ctree)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void load_xml_bookmarks(GtkTreeView * tree, GtkTreeIter * iter)
{
	GString *str;
	const xmlChar *file;

	str = g_string_new(settings.swbmDir);
	g_string_sprintf(str, "%s/bookmarks.xml", settings.swbmDir);
	file = (const xmlChar *) str->str;
	parse_bookmarks(tree, file, iter);
	g_string_free(str, TRUE);
}


/******************************************************************************
 * Name
 *   row_changed
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void row_changed(GtkTreeModel *treemodel, GtkTreePath *arg1,
 *                             GtkTreeIter *arg2, gpointer user_data)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void row_changed(GtkTreeModel * treemodel, GtkTreePath * arg1,
			GtkTreeIter * arg2, gpointer user_data)
{
	bookmarks_changed = TRUE;
}


/******************************************************************************
 * Name
 *   row_changed
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void row_deleted(GtkTreeModel * treemodel, GtkTreePath * arg1, 
 *							gpointer user_data)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void row_deleted(GtkTreeModel * treemodel, GtkTreePath * arg1,
			gpointer user_data)
{
	bookmarks_changed = TRUE;
	gui_save_bookmarks_treeview();
}


/******************************************************************************
 * Name
 *   create_pixbufs
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void create_pixbufs(void)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void create_pixbufs(void)
{
	//GtkWidget *image;

	bm_pixbufs = g_new0(BookMarksPixbufs, 1);
	bm_pixbufs->pixbuf_closed =
	    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
				     "/epiphany-bookmarks.png", NULL);
	if (!bm_pixbufs->pixbuf_closed)
		bm_pixbufs->pixbuf_closed
		    = gtk_widget_render_icon(widgets.app,
					     GNOME_STOCK_BOOK_BLUE,
					     GTK_ICON_SIZE_MENU, NULL);

	bm_pixbufs->pixbuf_opened =
	    gtk_widget_render_icon(widgets.app,
				   GNOME_STOCK_BOOK_OPEN,
				   GTK_ICON_SIZE_MENU, NULL);
	bm_pixbufs->pixbuf_helpdoc =
	    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
				     "/epiphany-bookmark-page.png",
				     NULL);
	if (!bm_pixbufs->pixbuf_helpdoc)
		bm_pixbufs->pixbuf_helpdoc
		    = gtk_widget_render_icon(widgets.app,
					     GTK_STOCK_DND,
					     GTK_ICON_SIZE_MENU, NULL);
}

/******************************************************************************
 * Name
 *   add_columns
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void add_columns(GtkTreeView * tree)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void add_columns(GtkTreeView * tree)
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

	column = gtk_tree_view_column_new();

	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes
	    (column, renderer,
	     "pixbuf", COL_OPEN_PIXBUF,
	     "pixbuf-expander-open", COL_OPEN_PIXBUF,
	     "pixbuf-expander-closed", COL_CLOSED_PIXBUF, NULL);



	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_CAPTION, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_sort_column_id(column, COL_CAPTION);


	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_KEY, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_MODULE, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_MODULE_DESC,
					    NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_DESCRIPTION,
					    NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);
}


/******************************************************************************
 * Name
 *   create_model
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   GtkTreeModel *create_model(void)
 *
 * 
 * Description
 *    
 *
 * Return value
 *   GtkTreeModel *
 */

static GtkTreeModel *create_model(void)
{
	/* create tree store */
	model = gtk_tree_store_new(N_COLUMNS,
				   GDK_TYPE_PIXBUF,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_STRING);
	return GTK_TREE_MODEL(model);
}


/******************************************************************************
 * Name
 *   button_release_event
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   gboolean button_release_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
 *
 * Description
 *   catch button 3 and select the row the pointer is over
 *   but does not display the bookmark 
 *
 * Return value
 *   void
 */

static gboolean button_release_event(GtkWidget * widget,
				     GdkEventButton * event,
				     gpointer data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeIter selected;
	gboolean is_selected = FALSE;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;
	gchar *url = NULL;
	gint which_tree = GPOINTER_TO_INT(data);
	button_one = FALSE;
	button_two = FALSE;


	//g_warning(main_url_encode("test this"));

	selection = gtk_tree_view_get_selection(bookmark_tree);
	current_selection = selection;
	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption,
				   3, &key,
				   4, &module,
				   5, &mod_desc, 6, &description, -1);
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected) && key != NULL) {
			gtk_widget_set_sensitive(menu.in_tab, TRUE);
			gtk_widget_set_sensitive(menu.in_dialog, TRUE);
			gtk_widget_set_sensitive(menu.new, FALSE);
			gtk_widget_set_sensitive(menu.insert, FALSE);
			gtk_widget_set_sensitive(menu.rr_submenu,
						 FALSE);
		} else {
			gtk_widget_set_sensitive(menu.in_tab, FALSE);
			gtk_widget_set_sensitive(menu.in_dialog, FALSE);
			gtk_widget_set_sensitive(menu.new, TRUE);
			gtk_widget_set_sensitive(menu.insert, TRUE);
			gtk_widget_set_sensitive(menu.rr_submenu, TRUE);
		}

		gtk_widget_set_sensitive(menu.bibletime,
					 settings.have_bibletime);
		gtk_widget_set_sensitive(menu.edit, TRUE);
		gtk_widget_set_sensitive(menu.delete, TRUE);
		is_selected = TRUE;
	}
	switch (event->button) {
	case 1:
		button_one = TRUE;
		break;
	case 2:
		button_two = TRUE;
		break;
	case 3:
		g_free(caption);
		g_free(key);
		g_free(module);
		gtk_menu_popup(GTK_MENU(menu.menu),
			       NULL, NULL, NULL, NULL,
			       event->button, event->time);
		if (GTK_CHECK_MENU_ITEM(menu.reorder)->active) {
			gtk_widget_set_sensitive(menu.in_tab, FALSE);
			gtk_widget_set_sensitive(menu.in_dialog, FALSE);
			gtk_widget_set_sensitive(menu.new, FALSE);
			gtk_widget_set_sensitive(menu.insert, FALSE);
			gtk_widget_set_sensitive(menu.edit, FALSE);
			gtk_widget_set_sensitive(menu.delete, FALSE);
			gtk_widget_set_sensitive(menu.bibletime, FALSE);
			gtk_widget_set_sensitive(menu.rr_submenu,
						 FALSE);
			gtk_widget_set_sensitive(menu.remove, FALSE);
			gtk_widget_set_sensitive(menu.restore, FALSE);
			return TRUE;
		} else {
			gtk_widget_set_sensitive(menu.bibletime,
						 settings.
						 have_bibletime);
			return FALSE;
		}
		break;

	}
	if (is_selected) {
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected) && key != NULL) {			
			if(!strcmp(module,"studypad"))
				url =
				    g_strdup_printf
				    ("gnomesword.url?action=showStudypad&"
				     "type=9&value=%s&module=%s",
				     main_url_encode(key),
				     main_url_encode(module));
				
			else
				url =
				    g_strdup_printf
				    ("gnomesword.url?action=showBookmark&"
				     "type=%s&value=%s&module=%s",
				     (button_one) ? "currentTab" : "newTab",
				     main_url_encode(key),
				     main_url_encode(module));
			main_url_handler(url, TRUE);
			g_free(url);

		}
		g_free(caption);
		g_free(key);
		g_free(module);
	}
	return FALSE;
}

/*
gboolean
on_treeview_modules_drag_drop          (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gint             x,
                                        gint             y,
                                        guint            time,
                                        gpointer         user_data)
{
g_message("on_treeview_modules_drag_drop");
  return FALSE;
}
*/


/******************************************************************************
 * Name
 *   gui_create_bookmark_tree
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   GtkWidget *gui_create_bookmark_tree(void)
 *
 * Description
 *    
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_bookmark_tree(void)
{
	GtkWidget *tree;
	GtkTreeIter iter;
	GtkTreeModel *tree_model;

	gui_create_bookmark_menu();
	create_pixbufs();
	tree_model = create_model();
	tree = gtk_tree_view_new_with_model(tree_model);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

	add_columns(GTK_TREE_VIEW(tree));
	gtk_tree_store_append(GTK_TREE_STORE(tree_model), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(tree_model), &iter,
			   COL_OPEN_PIXBUF, bm_pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, bm_pixbufs->pixbuf_closed,
			   COL_CAPTION, "Bookmarks",
			   COL_KEY, NULL, COL_MODULE, NULL, -1);

	load_xml_bookmarks(GTK_TREE_VIEW(tree), &iter);
	/*g_signal_connect(G_OBJECT(tree),
	   "row-collapsed", 
	   G_CALLBACK(expand_release_cursor_row),
	   NULL); */
	/*
	   g_signal_connect (G_OBJECT(tree) , "drag_drop",
	   G_CALLBACK (on_treeview_modules_drag_drop),
	   NULL);
	 */
	g_signal_connect_after(G_OBJECT(tree),
			       "button_release_event",
			       G_CALLBACK(button_release_event),
			       GINT_TO_POINTER(0));
	use_dialog = FALSE;
	bookmark_tree = GTK_TREE_VIEW(tree);

	g_signal_connect(model, "row-changed",
			 G_CALLBACK(row_changed), NULL);
	g_signal_connect(model, "row-deleted",
			 G_CALLBACK(row_deleted), NULL);

	return tree;
}



/******************************************************************************
 * Name
 *   button_release_dialog
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   gboolean button_release_dialog(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static GtkWidget *treeview8;
static GtkWidget *entry5;
static GtkWidget *entry6;
static GtkWidget *entry7;
static GtkWidget *okbutton1;
static GtkWidget *button22;
static gboolean button_release_dialog(GtkWidget * widget,
				      GdkEventButton * event,
				      gpointer data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeIter selected;
	gchar *key = NULL;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW
						(treeview8));

	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   3, &key, -1);
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected) && key != NULL) {
			gtk_widget_set_sensitive(button22, FALSE);
			gtk_widget_set_sensitive(okbutton1, FALSE);
		} else {
			gtk_widget_set_sensitive(button22, TRUE);
			gtk_widget_set_sensitive(okbutton1, TRUE);
		}
		if (key)
			g_free(key);
	}
	return FALSE;
}

static 
void add_bookmark_button(void)
{
	GtkTreeIter selected;
	GtkTreeIter iter;
	BOOKMARK_DATA *data;
	GtkTreeSelection *selection;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview8));
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	data = g_new(BOOKMARK_DATA, 1);
	data->caption = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY
						    (entry5)));
	data->key = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY
						(entry6)));
	data->module = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY
						   (entry7)));
	
	if(!strcmp(data->module,"studypad"))
		data->module_desc = "studypad";
	else
		data->module_desc =
		    	main_get_module_description(data->module);
	
	data->description = g_strdup((gchar *)gtk_entry_get_text
				     (GTK_ENTRY(entry5)));

	data->is_leaf = TRUE;
	data->opened = bm_pixbufs->pixbuf_helpdoc;
	data->closed = NULL;
	add_item_to_tree(&iter, &selected, data);
	bookmarks_changed = TRUE;
	gui_save_bookmarks(NULL, NULL);

	g_free(data->caption);
	g_free(data->key);
	g_free(data->module);
	g_free(data->description);
	g_free(data);
}

static void add_folder_button(void)
{
	GtkTreeIter selected;
	GtkTreeIter iter;
	BOOKMARK_DATA *data;
	GtkTreeSelection *selection;
	char *t, *buf;
	gint test;
	GS_DIALOG *info;
	GString *str;
	GtkTreePath *path;
	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview8));
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	t = "|";
	str = g_string_new("");
	info = gui_new_dialog();
	//info->stock_icon = GTK_STOCK_OPEN;
	info->title = N_("Bookmark");
	g_string_printf(str, "<span weight=\"bold\">%s</span>",
			_("Enter Folder Name"));
	info->label_top = str->str;
	info->text1 = g_strdup(_("Folder Name"));
	info->label1 = N_("Folder: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	data = g_new(BOOKMARK_DATA, 1);
	/*** open dialog to get name for new folder ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		buf = g_strdelimit(info->text1, t, ' ');
		data->caption = g_strdup(buf);
		data->key = NULL;
		data->module = NULL;
		data->module_desc = NULL;
		data->description = NULL;
		data->is_leaf = FALSE;
		data->opened = bm_pixbufs->pixbuf_opened;
		data->closed = bm_pixbufs->pixbuf_closed;
		add_item_to_tree(&iter, &selected, data);
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
		g_free(data->caption);
		path =
		    gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter);
		gtk_tree_view_expand_to_path(GTK_TREE_VIEW (treeview8), path);
		gtk_tree_selection_select_path(selection, path);
		gtk_tree_path_free(path);
	}
	g_free(data);
	g_free(info->text1);
	g_free(info);
	g_string_free(str, TRUE);
}

static void on_dialog_add_bookmark_response(GtkDialog * dialog,
					    gint response_id,
					    gpointer user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_CANCEL: /*  cancel button presed  */
	case GTK_RESPONSE_NONE:   /*  dialog destroyed  */
		gtk_widget_destroy(GTK_WIDGET(dialog));
		break;
	case GTK_RESPONSE_OK: /*  add button presed  */
		add_bookmark_button();
		gtk_widget_destroy(GTK_WIDGET(dialog));
		break;
	case GTK_RESPONSE_ACCEPT: /*  add folder presed  */
		add_folder_button();
		break;
	}
}


GtkWidget *gui_create_dialog_add_bookmark(gchar * label,
					  gchar * module_name,
					  gchar * key)
{
	GtkWidget *dialog_add_bookmark;
	GtkWidget *dialog_vbox1;
	GtkWidget *hbox10;
	GtkWidget *vbox11;
	GtkWidget *label24;
	GtkWidget *label25;
	GtkWidget *label26;
	GtkWidget *vbox12;
	GtkWidget *scrolledwindow8;
	GtkWidget *dialog_action_area1;
	GtkWidget *cancelbutton1;
	GtkWidget *alignment3;
	GtkWidget *hbox7;
	GtkWidget *image9;
	GtkWidget *label21;
	GtkTooltips *tooltips;
	GtkTreePath *path;
	GtkTreeIter iter;
	GtkTreeSelection *selection = NULL;

	tooltips = gtk_tooltips_new();

	dialog_add_bookmark = gtk_dialog_new();
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_add_bookmark), 6);
	gtk_window_set_title(GTK_WINDOW(dialog_add_bookmark),
			     _("Add Bookmark"));
	gtk_window_set_position(GTK_WINDOW(dialog_add_bookmark),
				GTK_WIN_POS_MOUSE);
	gtk_window_set_default_size(GTK_WINDOW(dialog_add_bookmark), -1,
				    346);
	gtk_window_set_type_hint(GTK_WINDOW(dialog_add_bookmark),
				 GDK_WINDOW_TYPE_HINT_DIALOG);

	dialog_vbox1 = GTK_DIALOG(dialog_add_bookmark)->vbox;
	gtk_widget_show(dialog_vbox1);

	hbox10 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox10);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox10, FALSE, TRUE,
			   0);

	vbox11 = gtk_vbox_new(TRUE, 3);
	gtk_widget_show(vbox11);
	gtk_box_pack_start(GTK_BOX(hbox10), vbox11, TRUE, TRUE, 0);

	label24 = gtk_label_new(_("Label:"));
	gtk_widget_show(label24);
	gtk_box_pack_start(GTK_BOX(vbox11), label24, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(label24), 0, 0.5);

	label25 = gtk_label_new(_("Verse:"));
	gtk_widget_show(label25);
	gtk_box_pack_start(GTK_BOX(vbox11), label25, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(label25), 0, 0.5);

	label26 = gtk_label_new(_("Module:"));
	gtk_widget_show(label26);
	gtk_box_pack_start(GTK_BOX(vbox11), label26, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(label26), 0, 0.5);

	vbox12 = gtk_vbox_new(FALSE, 3);
	gtk_widget_show(vbox12);
	gtk_box_pack_start(GTK_BOX(hbox10), vbox12, TRUE, TRUE, 0);

	entry5 = gtk_entry_new();
	gtk_widget_show(entry5);
	gtk_box_pack_start(GTK_BOX(vbox12), entry5, FALSE, FALSE, 0);

	gtk_entry_set_text((GtkEntry *) entry5, label);

	entry6 = gtk_entry_new();
	gtk_widget_show(entry6);
	gtk_box_pack_start(GTK_BOX(vbox12), entry6, FALSE, FALSE, 0);

	gtk_entry_set_text((GtkEntry *) entry6, key);

	entry7 = gtk_entry_new();
	gtk_widget_show(entry7);
	gtk_box_pack_start(GTK_BOX(vbox12), entry7, FALSE, FALSE, 0);
	gtk_entry_set_text((GtkEntry *) entry7, module_name);

	scrolledwindow8 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow8);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), scrolledwindow8, TRUE,
			   TRUE, 0);

	treeview8 = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	gtk_widget_show(treeview8);
	gtk_container_add(GTK_CONTAINER(scrolledwindow8), treeview8);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview8),
					  FALSE);
	add_columns(GTK_TREE_VIEW(treeview8));
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &iter);
	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview8));
	path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter);
	gtk_tree_view_expand_to_path(GTK_TREE_VIEW(treeview8), path);
	gtk_tree_selection_select_path(selection, path);
	gtk_tree_path_free(path);



	dialog_action_area1 =
	    GTK_DIALOG(dialog_add_bookmark)->action_area;
	gtk_widget_show(dialog_action_area1);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1),
				  GTK_BUTTONBOX_END);

/* new folder */
	button22 = gtk_button_new();
	gtk_widget_show(button22);
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog_add_bookmark),
				     button22, GTK_RESPONSE_ACCEPT);
	GTK_WIDGET_SET_FLAGS(button22, GTK_CAN_DEFAULT);
	gtk_tooltips_set_tip(tooltips, button22,
			     _("Create new folder"), NULL);
	//gtk_widget_set_sensitive(button22, FALSE);

	alignment3 = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_widget_show(alignment3);
	gtk_container_add(GTK_CONTAINER(button22), alignment3);

	hbox7 = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbox7);
	gtk_container_add(GTK_CONTAINER(alignment3), hbox7);

	image9 =
	    gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image9);
	gtk_box_pack_start(GTK_BOX(hbox7), image9, FALSE, FALSE, 0);

	label21 = gtk_label_new_with_mnemonic(_("New ..."));
	gtk_widget_show(label21);
	gtk_box_pack_start(GTK_BOX(hbox7), label21, FALSE, FALSE, 0);

/* cancel */
	cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
	gtk_widget_show(cancelbutton1);
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog_add_bookmark),
				     cancelbutton1,
				     GTK_RESPONSE_CANCEL);
	GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

/* ok - add */
	okbutton1 = gtk_button_new_from_stock("gtk-add");
	gtk_widget_show(okbutton1);
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog_add_bookmark),
				     okbutton1, GTK_RESPONSE_OK);
	GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);


	g_signal_connect((gpointer) dialog_add_bookmark, "response",
			 G_CALLBACK(on_dialog_add_bookmark_response),
			 NULL);
	g_signal_connect_after(G_OBJECT(treeview8), "button_release_event",
			 G_CALLBACK(button_release_dialog), 
			 NULL);

	return dialog_add_bookmark;
}
