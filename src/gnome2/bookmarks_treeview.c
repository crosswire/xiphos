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
#include <gal/shortcut-bar/e-shortcut-bar.h>
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
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/key.h"
#include "main/xml.h"

TreePixbufs *pixbufs;
GtkTreeStore *model;
gboolean button_one;
gboolean button_two;
gboolean use_dialog;
GtkTreeSelection *current_selection;
GtkTreeView *bookmark_tree;
static gboolean save_reorderable;
static void add_item_to_tree(GtkTreeIter *iter,GtkTreeIter *parent, BOOKMARK_DATA * data);


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

void gui_verselist_to_bookmarks(gchar * module_name)
{
	gint test;
	const gchar *key_buf = NULL;
	gchar *tmpbuf;
	GString *str;
	GtkTreeIter parent;
	GtkTreeIter iter;
	GS_DIALOG *info;
	
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
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, info->text1, 
			   COL_KEY, NULL,
			   COL_MODULE, NULL,
			   -1);
		set_results_position((char) 1);	/* TOP */
		str = g_string_new(" ");
		while ((key_buf = get_next_result_key()) != NULL) {
			tmpbuf = (gchar *) key_buf;
			g_string_sprintf(str, "%s, %s", tmpbuf,
					 module_name);			
			gtk_tree_store_append(GTK_TREE_STORE(model), &iter,
				      &parent);		
			gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
				   COL_CLOSED_PIXBUF, NULL,
				   COL_CAPTION, str->str, 
				   COL_KEY, tmpbuf,
				   COL_MODULE, module_name,
				   -1);
		}
		g_string_free(str, TRUE);
		bookmarks_changed = TRUE;
		gui_save_bookmarks_treeview();
	}
	g_free(info->text1);
	g_free(info);
	gtk_widget_set_sensitive(sidebar.menu_item_save_search,FALSE);
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
 *   goto_bookmark
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void goto_bookmark(gchar * mod_name, gchar * key)
 *
 * Description
 *   test for module type and call the appropriate function to
 *   display bookmark
 *
 * Return value
 *   void
 */

static void goto_bookmark(gchar * mod_name, gchar * key)
{
	gint module_type;
	
	if(!check_for_module(mod_name)) 
		mod_name = settings.MainWindowModule;
	
	if (use_dialog) {
		module_type = get_mod_type(mod_name);
		switch (module_type) {
			case -1:
				break;
			case TEXT_TYPE:
				gui_bibletext_dialog_goto_bookmark(mod_name,
								   key);
				break;
			case COMMENTARY_TYPE:
				gui_commentary_dialog_goto_bookmark(mod_name,
								    key);
				break;
			case DICTIONARY_TYPE:
				gui_dictionary_dialog_goto_bookmark(mod_name,
								    key);
				break;
			case BOOK_TYPE:
				gui_gbs_dialog_goto_bookmark(mod_name, key);
				break;
		}
	} else
		gui_change_module_and_key(mod_name, key);
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

	folder = xmlGetProp(cur, "caption");
	data->caption = g_strdup(folder);
	data->key = NULL; //g_strdup("GROUP");
	data->module = NULL; //g_strdup("GROUP");
	data->is_leaf = FALSE;
	data->opened = pixbufs->pixbuf_opened;
	data->closed = pixbufs->pixbuf_closed;
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
	gchar buf[500];

	data->opened = pixbufs->pixbuf_helpdoc;
	data->closed = NULL;
	mod1 = xmlGetProp(cur, "modulename");
	key = xmlGetProp(cur, "key");
	caption = xmlGetProp(cur, "caption");	// needed to load early devel release 
	if (caption) {
		if (strlen(caption) > 0) {
			data->caption = g_strdup(caption);
		} else {
			sprintf(buf, "%s, %s", key, mod1);
			data->caption = g_strdup(buf);
		}
	} else {
		caption = xmlGetProp(cur, "description");
		if (caption) {
			if (strlen(caption) > 0) {
				data->caption = g_strdup(caption);
			} else {
				sprintf(buf, "%s, %s", key, mod1);
				data->caption = g_strdup(buf);
			}
		} else
			data->caption = g_strdup(key);
	}
	data->key = g_strdup(key);
	data->module = g_strdup(mod1);
	data->is_leaf = TRUE;
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
	if(data->caption) 
		g_free(data->caption);
	if(data->key) 
		g_free(data->key);
	if(data->module) 
		g_free(data->module);
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

static void add_item_to_tree(GtkTreeIter *iter,GtkTreeIter *parent, BOOKMARK_DATA * data)
{
	gtk_tree_store_append(GTK_TREE_STORE(model), iter,
			      parent);
	
		gtk_tree_store_set(GTK_TREE_STORE(model), iter, 
			   COL_OPEN_PIXBUF, data->opened,
			   COL_CLOSED_PIXBUF, data->closed,
			   COL_CAPTION, data->caption, 
			   COL_KEY, data->key,
			   COL_MODULE, data->module,
			   -1);
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

static void add_node(xmlNodePtr cur, GtkTreeIter *parent)
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

void parse_bookmarks(GtkTreeView *tree, const xmlChar * file,
			    GtkTreeIter *parent)
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
			if(p->caption) {
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
 *   
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
	GtkTreeSelection* selection;
	GtkTreeIter selected;

	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
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
			if(p->caption) {
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
	path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), &selected);
	gtk_tree_view_expand_to_path(bookmark_tree, path);
	gtk_tree_path_free(path);
}

/******************************************************************************
 * Name
 *   tree_selection_changed
 *
 * Synopsis
 *   #include "gui/.h"
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

static void tree_selection_changed(GtkTreeSelection * selection,
				   gpointer data)
{
	GtkTreeIter selected;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;

	current_selection = selection;
	
	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption, 3, &key, 4, &module, -1);
		
		if(!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model),
				     &selected) && key != NULL) {
			if(button_one)
				goto_bookmark(module, key);
			else if(button_two) {
				use_dialog = TRUE; 
				goto_bookmark(module, key);
				use_dialog = FALSE; 
			}
			gtk_widget_set_sensitive(menu.in_dialog, TRUE);
			gtk_widget_set_sensitive(menu.new, FALSE);
			gtk_widget_set_sensitive(menu.insert, FALSE);
			gtk_widget_set_sensitive(menu.rr_submenu, FALSE);
		}
		else {
			gtk_widget_set_sensitive(menu.in_dialog, FALSE);
			gtk_widget_set_sensitive(menu.new, TRUE);
			gtk_widget_set_sensitive(menu.insert, TRUE);
			gtk_widget_set_sensitive(menu.rr_submenu, TRUE);
		}
			
		g_free(caption);
		g_free(key);
		g_free(module);
	}
	gtk_widget_set_sensitive(menu.edit, TRUE);
	gtk_widget_set_sensitive(menu.delete, TRUE);
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

static void load_xml_bookmarks(GtkTreeView *tree, GtkTreeIter *iter)
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
	pixbufs = g_new0(TreePixbufs, 1);
	pixbufs->pixbuf_closed =
	    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
				     "/book_closed.png", NULL);
	pixbufs->pixbuf_opened =
	    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
				     "/book_open.png", NULL);
	pixbufs->pixbuf_helpdoc =
	    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR 
				     "/helpdoc.png", NULL);
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
	gtk_tree_view_column_set_sort_column_id(column, COL_CAPTION );


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
				   G_TYPE_STRING);
	return GTK_TREE_MODEL(model);	
}

/******************************************************************************
 * Name
 *   button_press_event
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   gboolean button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
 *
 * Description
 *   catch button 3 and select the row the pointer is over
 *   but does not display the bookmark 
 *
 * Return value
 *   void
 */

static gboolean button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer data)
{
	button_one = FALSE;
	button_two = FALSE;
	switch(event->button) {
		case 1:
			button_one = TRUE;
			break;
		case 2:
			button_two = TRUE;
			break;
		case 3:
			gtk_menu_popup(GTK_MENU(menu.menu),
			       NULL, NULL, NULL, NULL,
			       event->button, event->time);
			if(GTK_CHECK_MENU_ITEM(menu.reorder)->active) {			
				gtk_widget_set_sensitive(menu.in_dialog, FALSE);
				gtk_widget_set_sensitive(menu.new, FALSE);
				gtk_widget_set_sensitive(menu.insert, FALSE);
				gtk_widget_set_sensitive(menu.add, FALSE);
				gtk_widget_set_sensitive(menu.edit, FALSE);
				gtk_widget_set_sensitive(menu.point, FALSE);
				gtk_widget_set_sensitive(menu.delete, FALSE);
				gtk_widget_set_sensitive(menu.bibletime, FALSE);			
				gtk_widget_set_sensitive(menu.rr_submenu,FALSE);
				gtk_widget_set_sensitive(menu.remove, FALSE);
				gtk_widget_set_sensitive(menu.restore, FALSE);
				return TRUE;
			}
			else {
				gtk_widget_set_sensitive(menu.bibletime,
						       settings.have_bibletime);	
				
			}
			break;
		
	}
	return FALSE;
}

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
	GtkTreeModel *model;
	GtkTreeIter iter;
	GObject *selection;
	
	gui_create_bookmark_menu();
	create_pixbufs();	
	model = create_model();
	tree = gtk_tree_view_new_with_model(model);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree),
					  FALSE);
	selection =
	    G_OBJECT(gtk_tree_view_get_selection
		     (GTK_TREE_VIEW(tree)));
	
	add_columns(GTK_TREE_VIEW(tree));
	gtk_tree_store_append(GTK_TREE_STORE(model), &iter,
			      NULL);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION,"Bookmarks", 
			   COL_KEY, NULL,
			   COL_MODULE,NULL,
			   -1);

	load_xml_bookmarks(GTK_TREE_VIEW(tree),&iter);
	g_signal_connect(G_OBJECT(tree),
				 "button_press_event",
				G_CALLBACK(button_press_event),
				 NULL);
	g_signal_connect(selection, "changed",
			 G_CALLBACK(tree_selection_changed), NULL);
	use_dialog = FALSE;
	bookmark_tree = GTK_TREE_VIEW(tree); 
	
	g_signal_connect(model, "row-changed",
			 G_CALLBACK(row_changed), NULL);
	g_signal_connect(model, "row-deleted",
			 G_CALLBACK(row_deleted), NULL);
	
	return tree;
}
