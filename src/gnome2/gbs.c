/*
 * GnomeSword Bible Study Tool
 * gbs.c - generic book support - the gui
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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

#ifdef USE_GTKEMBEDMOZ
#include <gtkmozembed.h>
#endif


#include "gui/gtkhtml_display.h"
#include "gui/gbs.h"
#include "gui/gbs_dialog.h"
#include "gui/gbs_display.h"
#include "gui/gbs_menu.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/find_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/bookmarks.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/lists.h"
#include "main/gbs.h"
#include "main/sword.h"
#include "main/xml.h"

typedef struct _treeitem TreeItem;
struct _treeitem {
	GtkWidget *tree;
	gchar *module_name;
	gchar *item_name;
	gchar *offset;
	gboolean is_leaf;
	gboolean expanded;
	GdkPixbuf *pixbuf_opened;
	GdkPixbuf *pixbuf_closed;
};

typedef struct {
	GdkPixbuf *pixbuf_opened;
	GdkPixbuf *pixbuf_closed;
	GdkPixbuf *pixbuf_helpdoc;
} TreePixbufs;

enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_TITLE,
	COL_BOOK,
	COL_OFFSET,
	N_COLUMNS
};
/******************************************************************************
 *  externs  
 */
extern gboolean in_url;

/******************************************************************************
 *  static   
 */
static void add_node_children(GtkCTreeNode * node, GBS_DATA * gbs,
			      unsigned long offset);
static gboolean gbs_find_running;
static GtkCTreeNode *rootnode;
static GBS_DATA *cur_g;
static gint tree_level;
/* list of gbs data structures */
static GList *gbs_list;
static GtkTreeModel *model;
static TreePixbufs *pixbufs;
/*
static void link_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	
}
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
 *  get_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   GBS_DATA *get_gbs(GList * gbs)	
 *
 * Description
 *    
 *
 * Return value
 *   GBS_DATA*
 */

GBS_DATA *get_gbs(GList * gbs)
{
	GList *tmp;
	GBS_DATA *g = NULL;

	tmp = NULL;
	tmp = gbs;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		g = (GBS_DATA *) tmp->data;
		if (!strcmp(g->mod_name, settings.BookWindowModule)) {
			break;
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return g;
}


/******************************************************************************
 * Name
 *  add_tree_item
 *
 * Synopsis
 *   #include "gui/gbs.h"
 *
 *   void add_tree_item()	
 *
 * Description
 *    
 *
 * Return value
 *   GtkCTreeNode*
 */

static void add_tree_item(TreeItem * item, GtkTreeIter parent)
{

	GtkTreeIter iter;
	
	gtk_tree_store_append(GTK_TREE_STORE(model), &iter, &parent);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, item->pixbuf_opened,
			   COL_CLOSED_PIXBUF, item->pixbuf_closed,
			   COL_TITLE, item->item_name,
			   COL_BOOK, item->module_name,
			   COL_OFFSET, item->offset, -1);
}


/******************************************************************************
 * Name
 *  add_children_to_root
 *
 * Synopsis
 *   #include "gui/gbs.h"
 *
 *   void add_children_to_root(gchar *bookname,
 *   				unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void add_children_to_tree(GBS_DATA * gbs, GtkTreeIter iter,
				 unsigned long offset)
{
	gchar buf[256];
	gchar *tmpbuf;
	TreeItem treeitem, *p_treeitem;
	GdkPixbuf *open;
	GdkPixbuf *closed;

	p_treeitem = &treeitem;
	p_treeitem->module_name = gbs->mod_name;
	p_treeitem->tree = gbs->tree;

	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   -1);

  
	if (gbs_treekey_first_child(offset)) {
		offset = gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_treeitem->offset = buf;
		tmpbuf = gbs_get_treekey_local_name(offset);
		p_treeitem->item_name = (gchar *) tmpbuf;
		if (gbs_treekey_has_children(offset)) {
			p_treeitem->pixbuf_opened =
				pixbufs->pixbuf_closed;
			p_treeitem->pixbuf_closed =
			    pixbufs->pixbuf_opened;
			p_treeitem->is_leaf = FALSE;
			p_treeitem->expanded = FALSE;
		} else {
			p_treeitem->pixbuf_opened =
			    pixbufs->pixbuf_helpdoc;
			p_treeitem->pixbuf_closed = NULL;
			p_treeitem->is_leaf = TRUE;
			p_treeitem->expanded = FALSE;
		}
		add_tree_item(p_treeitem, iter);
		free(tmpbuf);
	}

	while (treekey_next_sibling(offset)) {
		offset = gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_treeitem->offset = buf;
		tmpbuf = gbs_get_treekey_local_name(offset);
		p_treeitem->item_name = (gchar *) tmpbuf;
		if (gbs_treekey_has_children(offset)) {
			p_treeitem->pixbuf_opened =
			    pixbufs->pixbuf_closed;
			p_treeitem->pixbuf_closed =
			    pixbufs->pixbuf_opened;
			p_treeitem->is_leaf = FALSE;
			p_treeitem->expanded = FALSE;
		} else {
			p_treeitem->pixbuf_opened =
			    pixbufs->pixbuf_helpdoc;
			p_treeitem->pixbuf_closed = NULL;
			p_treeitem->is_leaf = TRUE;
			p_treeitem->expanded = FALSE;
		}
		add_tree_item(p_treeitem, iter);
		free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *  gui_set_book_page_and_key
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_set_book_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_set_book_page_and_key(gint page_num, gchar * key)
{
	gchar *text;

	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_gbs),
			      page_num);

	text = display_gbs(cur_g->mod_name, key);

	if (text) {
#ifdef USE_GTKEMBEDMOZ
		if (!cur_g->is_rtol)
			entry_display(cur_g->html, cur_g->mod_name,
				      text, key, TRUE);
		else
			entry_display_mozilla(cur_g->html,
					      cur_g->mod_name, text,
					      key, TRUE);

#else
		entry_display(cur_g->html, cur_g->mod_name,
			      text, key, TRUE);
#endif
		free(text);
	}
}

/******************************************************************************
 * Name
 *  gui_set_gbs_frame_label
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void gui_set_gbs_frame_label(void)	
 *
 * Description
 *   sets gbs frame label to module name or null
 *
 * Return value
 *   void
 */

void gui_set_gbs_frame_label(void)
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module name
	 */
	if (settings.book_tabs)
		gtk_frame_set_label(GTK_FRAME(cur_g->frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(cur_g->frame),
				    cur_g->mod_name);

}


/******************************************************************************
 * Name
 *  on_notebook_gbs_switch_page
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_notebook_gbs_switch_page(GtkNotebook * notebook,	
 *		GtkNotebookPage * page,	gint page_num, GList * data_gbs)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void on_notebook_gbs_switch_page(GtkNotebook * notebook,
				 GtkNotebookPage * page, gint page_num,
				 GList * data_gbs)
{
	GBS_DATA *g, *g_old;
	gchar *key;

	g_old =
	    (GBS_DATA *) g_list_nth_data(data_gbs,
					 settings.book_last_page);
	g = (GBS_DATA *) g_list_nth_data(data_gbs, page_num);
	cur_g = g;

	if (!g->frame)
		gui_add_new_gbs_pane(g);
	gui_set_gbs_frame_label();

	change_book(g->mod_name, g->offset);
	/*
	 *  get the book key and store in settings.book_key
	 *  for adding bookmarks
	 */
	key = get_book_key(g->mod_name);
	if (key) {
		settings.book_key = key;
		xml_set_value("GnomeSword", "key", "book", key);
		free(key);
	} else {
		settings.book_key = NULL;
		xml_set_value("GnomeSword", "key", "book", NULL);
	}
	gui_change_window_title(g->mod_name);
	/*
	 * set search module to current gbs module 
	 */
//      strcpy(settings.sb_search_mod, g->mod_name);
	/*
	 * set search frame label to current gbs module 
	 */
//      gui_set_search_label();

	settings.BookWindowModule = g->mod_name;
	xml_set_value("GnomeSword", "modules", "book", g->mod_name);
	GTK_CHECK_MENU_ITEM(g->showtabs)->active = settings.book_tabs;
	settings.book_last_page = page_num;
	widgets.html_book = g->html;
}

/******************************************************************************
 * Name
 *  on_bookmark_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_bookmark_activate(GtkMenuItem * menuitem, GBS_DATA * gbs)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
/*
static void on_bookmark_activate(GtkMenuItem * menuitem, GBS_DATA * gbs)
{
	gchar *key = get_book_key(gbs->mod_name);
	if(key){
		gui_add_bookmark_to_tree(NULL, gbs->mod_name, key);
		free(key);
	}
}
*/

/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   gboolean on_button_release_event(GtkWidget * widget,
 *				GdkEventButton * event, GBS_DATA * g)	
 *
 * Description
 *    
 *
 * Return value
 *   gboolean
 */

static gboolean on_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					GBS_DATA * g)
{
	gchar *key;
	cur_g = g;
	settings.whichwindow = BOOK_WINDOW;
	gui_change_window_title(g->mod_name);

	switch (event->button) {
	case 1:
		if (!in_url) {
			key = gui_button_press_lookup(g->html);
			if (key) {
				gui_display_dictlex_in_sidebar(settings.
							       DictWindowModule,
							       key);
				g_free(key);
			}
		}
		break;
	case 2:
		break;
	case 3:
		break;
	}
	return FALSE;
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

static void add_columns(GtkTreeView * tree)
{
	GtkTreeViewColumn *column;
	GtkTreeViewColumn *column2;
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

static void tree_selection_changed(GtkTreeSelection * selection,
				   GBS_DATA * g)
{
	GtkTreeIter selected;
	GtkTreePath *path;
	gchar *name = NULL;
	gchar *book = NULL;
	gchar *offset = NULL;


	model = gtk_tree_view_get_model(GTK_TREE_VIEW(g->tree));


	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		path = gtk_tree_model_get_path(model, &selected);
		tree_level = gtk_tree_path_get_depth(path);
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &name, 3, &book, 4, &offset, -1);

		if (offset) {
			//g_warning("%s in %s at %s",name,book,offset);
			g->offset = strtoul(offset, NULL, 0);
			change_book(book, g->offset);
			settings.book_key =
			    gbs_get_treekey_local_name(g->offset);
			if (!gtk_tree_model_iter_has_child
			    (model, &selected)
			    && gbs_treekey_has_children(g->offset)) {
				add_children_to_tree(g, selected,
						     g->offset);
				gtk_tree_view_expand_to_path
				    (GTK_TREE_VIEW(g->tree), path);
			}
			gbs_display(g, offset, tree_level,
				    gtk_tree_model_iter_has_child(model,
								  &selected));
			g_free(name);
			g_free(book);
			g_free(offset);
		}
		gtk_tree_path_free(path);
	}
}

/******************************************************************************
 * Name
 *  gui_create_gbs_pane
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_create_gbs_pane(GBS_DATA *p_gbs)	
 *
 * Description
 *    
 *
 * Return value
 *  void 
 */

static void create_gbs_pane(GBS_DATA * p_gbs)
{

	GtkWidget *hpanedGBS;
	GtkWidget *frame;
	GtkWidget *scrolledwindowCTREE_GBS;
	GtkWidget *label;
	GtkWidget *frameGBS;
	GtkWidget *scrolledwindowHTML_GBS;
	GObject *selection;
	
	p_gbs->frame = gtk_frame_new(NULL);
	gtk_widget_show(p_gbs->frame);
	gtk_container_add(GTK_CONTAINER(p_gbs->vbox), p_gbs->frame);
	gtk_frame_set_shadow_type(GTK_FRAME(p_gbs->frame),
				  GTK_SHADOW_NONE);

	hpanedGBS = gtk_hpaned_new();
	gtk_widget_show(hpanedGBS);
	gtk_container_add(GTK_CONTAINER(p_gbs->frame), hpanedGBS);
	gtk_paned_set_position(GTK_PANED(hpanedGBS), 195);

	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_paned_pack1(GTK_PANED(hpanedGBS), frame, TRUE, TRUE);

	scrolledwindowCTREE_GBS = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindowCTREE_GBS);
	gtk_container_add(GTK_CONTAINER(frame),
			  scrolledwindowCTREE_GBS);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowCTREE_GBS),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	model = create_model();
	p_gbs->tree = gtk_tree_view_new_with_model(model);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(p_gbs->tree),
					  FALSE);
	gtk_widget_show(p_gbs->tree);
	gtk_container_add(GTK_CONTAINER(scrolledwindowCTREE_GBS),
			  p_gbs->tree);
	add_columns(GTK_TREE_VIEW(p_gbs->tree));

	selection =
	    G_OBJECT(gtk_tree_view_get_selection
		     (GTK_TREE_VIEW(p_gbs->tree)));


	frameGBS = gtk_frame_new(NULL);
	gtk_widget_show(frameGBS);
	gtk_paned_pack2(GTK_PANED(hpanedGBS), frameGBS, TRUE, TRUE);



#ifdef USE_GTKEMBEDMOZ
	if (!p_gbs->is_rtol) {
		scrolledwindowHTML_GBS =
		    gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_show(scrolledwindowHTML_GBS);
		gtk_container_add(GTK_CONTAINER(frameGBS),
				  scrolledwindowHTML_GBS);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
					       (scrolledwindowHTML_GBS),
					       GTK_POLICY_AUTOMATIC,
					       GTK_POLICY_AUTOMATIC);

		p_gbs->html = gtk_html_new();
		gtk_widget_show(p_gbs->html);
		gtk_container_add(GTK_CONTAINER(scrolledwindowHTML_GBS),
				  p_gbs->html);
		gtk_html_load_empty(GTK_HTML(p_gbs->html));
		gtk_signal_connect(GTK_OBJECT(p_gbs->html),
				   "link_clicked",
				   G_CALLBACK(gui_link_clicked), NULL);
		gtk_signal_connect(GTK_OBJECT(p_gbs->html), "on_url",
				   G_CALLBACK(gui_url),
				   (gpointer) widgets.app);
		gtk_signal_connect(GTK_OBJECT(p_gbs->html),
				   "button_release_event",
				   G_CALLBACK(on_button_release_event),
				   p_gbs);
	} else {
		//gtk_moz_embed_set_comp_path("usr/lib/mozilla-1.0.1");
		p_gbs->html = gtk_moz_embed_new();
		gtk_widget_show(p_gbs->html);
		gtk_container_add(GTK_CONTAINER(frameGBS), p_gbs->html);
		gtk_widget_realize(p_gbs->html);
/*
		gtk_signal_connect(GTK_OBJECT(p_gbs->html),
				   "dom_mouse_click",
				   G_CALLBACK(mozilla_mouse_click),
				   NULL);
*/
	}


#else
	scrolledwindowHTML_GBS = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindowHTML_GBS);
	gtk_container_add(GTK_CONTAINER(frameGBS),
			  scrolledwindowHTML_GBS);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowHTML_GBS),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	p_gbs->html = gtk_html_new();
	gtk_widget_show(p_gbs->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowHTML_GBS),
			  p_gbs->html);
	gtk_html_load_empty(GTK_HTML(p_gbs->html));
	gtk_signal_connect(GTK_OBJECT(p_gbs->html), "link_clicked",
			   G_CALLBACK(gui_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(p_gbs->html), "on_url",
			   G_CALLBACK(gui_url), (gpointer) widgets.app);
	gtk_signal_connect(GTK_OBJECT(p_gbs->html),
			   "button_release_event",
			   G_CALLBACK(on_button_release_event), p_gbs);
#endif

	g_signal_connect(selection, "changed",
			 G_CALLBACK(tree_selection_changed), p_gbs);
}


/******************************************************************************
 * Name
 *  add_book_to_tree
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void add_book_to_tree(GtkWidget * tree, gchar * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void add_book_to_tree(GBS_DATA * g)
{
	GtkTreeIter iter;
	GtkTreePath *path;
	gtk_tree_store_append(GTK_TREE_STORE(model), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_TITLE, g->mod_name,
			   COL_BOOK, g->mod_name, COL_OFFSET, NULL, -1);
	change_book(g->mod_name, 0);
	add_children_to_tree(g, iter, gbs_get_treekey_offset());
	path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_view_expand_to_path(GTK_TREE_VIEW(g->tree), path);
	gtk_tree_path_free(path);

}


/******************************************************************************
 * Name
 *  set_gbs_page
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void set_gbs_page(gchar * modname, GList * comm_list)	
 *
 * Description
 *    change gbs page without changing key
 *
 * Return value
 *   void
 */

static void set_gbs_page(gchar * book_name, GList * gbs_list)
{
	gint page = 0;
	GBS_DATA *g = NULL;

	gbs_list = g_list_first(gbs_list);
	if(book_name) {
		while (gbs_list != NULL) {
			g = (GBS_DATA *) gbs_list->data;
			if (!strcmp(g->mod_name, book_name))
				break;
			++page;
			gbs_list = g_list_next(gbs_list);
		}
	}	
	else {
		g = (GBS_DATA *) gbs_list->data;
		page = 0;
	}	
	cur_g = g;
	
	if (page)
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_gbs), page);
	else
		on_notebook_gbs_switch_page(GTK_NOTEBOOK
					    (widgets.notebook_gbs),
					    NULL, page, gbs_list);
	gui_set_gbs_frame_label();

	settings.book_last_page = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_gbs),
				   settings.book_tabs);
}

/******************************************************************************
 * Name
 *  gui_add_new_gbs_pane
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_add_new_gbs_pane(GBS_DATA * g)
 *
 * Description
 *   creates a gbs pane when user selects a gbs text module
 *
 * Return value
 *   void
 */

void gui_add_new_gbs_pane(GBS_DATA * g)
{
	GtkWidget *popupmenu;

	create_pixbufs();
	create_gbs_pane(g);

	popupmenu = gui_create_pm_gbs(g);
	gnome_popup_menu_attach(popupmenu, g->html, NULL);
	add_book_to_tree(g);
}

/******************************************************************************
 * Name
 *  add_vbox_to_notebook
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void add_vbox_to_notebook(GBS_DATA * g)
 *
 * Description
 *   adds a vbox and label to the gbs notebook for each gbs module
 *
 * Return value
 *   void
 */

static void add_vbox_to_notebook(GBS_DATA * g)
{
	GtkWidget *label;

	g->vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(g->vbox);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_gbs), g->vbox);


	label = gtk_label_new(g->mod_name);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_gbs),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(widgets.notebook_gbs),
				    g->mod_num), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.notebook_gbs),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.notebook_gbs),
					  g->mod_num),
					 (gchar *) g->mod_name);
}


/******************************************************************************
 * Name
 *  gui_setup_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_setup_gbs(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_setup_gbs(GList * mods, gint starting_page)
{
	gint count = 0;
	GList *tmp = NULL;
	gchar *bookname;
	GBS_DATA *gbs;

	gbs_list = NULL;
	gbs_find_running = FALSE;

	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		bookname = (gchar *) tmp->data;
		gbs = g_new0(GBS_DATA, 1);
		gbs->frame = NULL;
		gbs->mod_name = bookname;
		gbs->search_string = NULL;
		gbs->key = NULL;
		gbs->mod_num = count;
		gbs->offset = 0;
		gbs->is_dialog = FALSE;
		gbs->has_key = module_is_locked(gbs->mod_name);
		if (has_cipher_tag(gbs->mod_name)) {
			gbs->is_locked
			    = module_is_locked(gbs->mod_name);
			gbs->cipher_old = get_cipher_key(gbs->mod_name);
		}

		else {

			gbs->is_locked = 0;
			gbs->cipher_old = NULL;
		}
		gbs->is_rtol = is_module_rtl(gbs->mod_name);
		gbs->display_level = get_display_level(gbs->mod_name);
		//g_warning("DisplayLevel %s = %d",gbs->mod_name, gbs->display_level);
		add_vbox_to_notebook(gbs);
		gbs_list = g_list_append(gbs_list, (GBS_DATA *) gbs);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(widgets.notebook_gbs),
			   "switch_page",
			   G_CALLBACK(on_notebook_gbs_switch_page),
			   gbs_list);


	set_gbs_page(settings.BookWindowModule, gbs_list);
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *  gui_shutdown_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_shutdown_gbs(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_shutdown_gbs(void)
{
	gbs_list = g_list_first(gbs_list);
	while (gbs_list != NULL) {
		g_free((GBS_DATA *) gbs_list->data);
		gbs_list = g_list_next(gbs_list);
	}
	g_list_free(gbs_list);
}
