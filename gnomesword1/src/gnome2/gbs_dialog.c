/*
 * GnomeSword Bible Study Tool
 * gbs_dialog.c - dialog for displaying a gbs module
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

#ifdef USE_GTKEMBEDMOZ
#include <gtkmozembed.h>
#endif

#include "gui/gtkhtml_display.h"
#include "gui/gbs_dialog.h"
#include "gui/gbs_display.h"
#include "gui/gbs_menu.h"
#include "gui/html.h"
#include "gui/widgets.h"

#include "main/gbs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"

typedef struct _treeitem TreeItem;
struct _treeitem {
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
 * static - global to this file only
 */
static GList *dialog_list;
static GBS_DATA *cur_dlg;
static gboolean dialog_freed;
//static GtkCTreeNode *rootnode;
static GtkTreeModel *model;
static TreePixbufs *pixbufs;
static gint tree_level;	

static void create_pixbufs (void)
{
	pixbufs = g_new0(TreePixbufs, 1);
	pixbufs->pixbuf_closed = gtk_widget_render_icon(widgets.app,
                                             GNOME_STOCK_BOOK_BLUE, 
                                             GTK_ICON_SIZE_MENU,
                                             NULL);
	pixbufs->pixbuf_opened =
		gtk_widget_render_icon(widgets.app,
                                             GNOME_STOCK_BOOK_OPEN, 
                                             GTK_ICON_SIZE_MENU,
                                             NULL);
	pixbufs->pixbuf_helpdoc =
		gtk_widget_render_icon(widgets.app,
                                             GTK_STOCK_DND, 
                                             GTK_ICON_SIZE_MENU,
                                             NULL);
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
 
static void add_tree_item(TreeItem *item, GtkTreeIter parent)
{
	
	GtkTreeIter iter;
	
	gtk_tree_store_append(GTK_TREE_STORE(model), &iter, &parent);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 
				COL_OPEN_PIXBUF,item->pixbuf_opened,
				COL_CLOSED_PIXBUF,item->pixbuf_closed,
				COL_TITLE, item->item_name, 
				COL_BOOK, item->module_name, 
				COL_OFFSET, item->offset, 
				-1);
	
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

	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   -1);
	
	if (gbs_treekey_first_child(offset)) {
		offset = gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_treeitem->offset = buf;
		tmpbuf =
		    gbs_get_treekey_local_name(offset);
		p_treeitem->item_name = (gchar*)tmpbuf;
		if (gbs_treekey_has_children(offset)) {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_closed;
			p_treeitem->pixbuf_closed = pixbufs->pixbuf_opened;
			p_treeitem->is_leaf = FALSE;
			p_treeitem->expanded = FALSE;
		} else {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_helpdoc;
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
		tmpbuf =
		    gbs_get_treekey_local_name(offset);
		p_treeitem->item_name = (gchar*)tmpbuf;
		if (gbs_treekey_has_children(offset)) {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_closed;
			p_treeitem->pixbuf_closed = pixbufs->pixbuf_opened;
			p_treeitem->is_leaf = FALSE;
			p_treeitem->expanded = FALSE;
		} else {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_helpdoc;
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
 *   free_on_destroy
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void free_on_destroy(DL_DIALOG * dlg)
 *
 * Description
 *   removes dialog from dialog_list when dialog is destroyed other than
 *   program shut down
 *
 * Return value
 *   void
 */

static void free_on_destroy(GBS_DATA * dlg)
{
	dialog_list = g_list_remove(dialog_list, (GBS_DATA *) dlg);
//      g_warning("shuting down %s dialog", dlg->mod_name);
	g_free(dlg);
}


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

static void dialog_destroy(GtkObject * object, GBS_DATA * dlg)
{
	if (!dialog_freed)
		free_on_destroy(dlg);
	dialog_freed = FALSE;
}


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

void gui_close_gbs_dialog(GBS_DATA * dlg)
{
	if (dlg->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(dlg->dialog);
	}
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

static void dialog_url(GtkHTML * html, const gchar * url,
		       GBS_DATA * dlg)
{
	gchar buf[255];
	gint context_id2;

	cur_dlg = dlg;

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(dlg->statusbar),
					 "GnomeSword");
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


/******************************************************************************
 * Name
 *   link_clicked
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void link_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void link_clicked(GtkButton * button, gpointer user_data)
{

}


/******************************************************************************
 * Name
 *   button_press
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   gboolean button_press(GtkWidget * widget,
				GdkEventButton * event,GBS_DATA  * g)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static gboolean button_press(GtkWidget * widget,
			     GdkEventButton * event, GBS_DATA * g)
{
	cur_dlg = g;
	return FALSE;
}

static GtkTreeModel *create_model (void)
{
  GtkTreeStore *model;

  /* create tree store */
  model = gtk_tree_store_new (N_COLUMNS, 
			  GDK_TYPE_PIXBUF,
			  GDK_TYPE_PIXBUF,
			  G_TYPE_STRING,
			  G_TYPE_STRING,
			  G_TYPE_STRING);

 

  return GTK_TREE_MODEL(model);
}

static void add_columns(GtkTreeView *tree)
{
	GtkTreeViewColumn *column;
	GtkTreeViewColumn *column2;
	GtkCellRenderer *renderer;
  
	column = gtk_tree_view_column_new ();
	
	renderer = GTK_CELL_RENDERER (gtk_cell_renderer_pixbuf_new ());
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes
		(column, renderer,
		 "pixbuf", COL_OPEN_PIXBUF,
		 "pixbuf-expander-open", COL_OPEN_PIXBUF,
		 "pixbuf-expander-closed", COL_CLOSED_PIXBUF,
		 NULL);
	
	
	
	renderer = GTK_CELL_RENDERER (gtk_cell_renderer_text_new ());
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "text", COL_TITLE,
					     NULL);
	gtk_tree_view_append_column (tree, column);
	
	
	column = gtk_tree_view_column_new ();
	renderer = GTK_CELL_RENDERER (gtk_cell_renderer_text_new ());
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "text", COL_BOOK,
					     NULL);
	gtk_tree_view_append_column (tree, column);
	gtk_tree_view_column_set_visible(column,FALSE);
	
	column = gtk_tree_view_column_new ();
	renderer = GTK_CELL_RENDERER (gtk_cell_renderer_text_new ());
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "text", COL_OFFSET,
					     NULL);
	gtk_tree_view_append_column (tree, column);
	gtk_tree_view_column_set_visible(column,FALSE);
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
	
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(g->tree));
		

	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		path = gtk_tree_model_get_path(model, &selected);
		tree_level = gtk_tree_path_get_depth(path);
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 
					2,&name,
					3,&book, 
					4,&offset, 
					-1);
		gtk_tree_path_free(path);
		
		if (offset) {
			//g_warning("%s in %s at %s",name,book,offset);
			g->offset = strtoul(offset, NULL, 0);
			change_book(book, g->offset);
			settings.book_key = gbs_get_treekey_local_name(g->offset);
			if( !gtk_tree_model_iter_has_child(model, &selected) &&
				gbs_treekey_has_children(g->offset)) {
				add_children_to_tree(g, selected, g->offset);
			}
			gbs_display(g, offset, tree_level,  
					gtk_tree_model_iter_has_child(
						model,
                                                &selected));
			g_free(name);
			g_free(book);
			g_free(offset);
		}
	}
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
	
	gtk_tree_store_append(GTK_TREE_STORE(model), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 
				    COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
				    COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				    COL_TITLE,g->mod_name,
				    COL_BOOK,g->mod_name,
				    COL_OFFSET,NULL,
				    -1);
	change_book(g->mod_name, 0);
	add_children_to_tree(g, iter, gbs_get_treekey_offset());
	
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

static void create_gbs_dialog(GBS_DATA * dlg)
{
	GtkWidget *vbox_dialog;
	GtkWidget *frame_gbs;
	GtkWidget *hpaned;
	GtkWidget *scrolledwindow_ctree;
	GtkWidget *label241;
	GtkWidget *label242;
	GtkWidget *label243;
	GtkWidget *scrolledwindow_html;
	GObject *selection;


	dlg->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(dlg->dialog), "dlg->dialog",
			    dlg->dialog);
	gtk_window_set_title(GTK_WINDOW(dlg->dialog),
			     get_module_description(dlg->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(dlg->dialog), 525, 306);
	gtk_window_set_policy(GTK_WINDOW(dlg->dialog), TRUE, TRUE,
			      FALSE);

	vbox_dialog = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_dialog);
	gtk_container_add(GTK_CONTAINER(dlg->dialog), vbox_dialog);
/*
	frame_gbs = gtk_frame_new(NULL);
	gtk_widget_show(frame_gbs);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), frame_gbs, TRUE, TRUE,
			   0);
*/
	hpaned = gtk_hpaned_new();
	gtk_widget_show(hpaned);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), hpaned, TRUE, TRUE,
			   0);
	//gtk_container_add(GTK_CONTAINER(frame_gbs), hpaned);
	gtk_paned_set_position(GTK_PANED(hpaned), 190);

	scrolledwindow_ctree = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_ctree);
	gtk_paned_pack1(GTK_PANED(hpaned), scrolledwindow_ctree, FALSE,
		      TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_ctree),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);	
				    
	model = create_model();			       
	dlg->tree = gtk_tree_view_new_with_model(model);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(dlg->tree),
					  FALSE);
	gtk_widget_show(dlg->tree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_ctree),
			  dlg->tree);
	add_columns(GTK_TREE_VIEW(dlg->tree));
	
	selection =
	    G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(dlg->tree)));

#ifdef USE_GTKEMBEDMOZ
	if (!dlg->is_rtol) {

		scrolledwindow_html = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_show(scrolledwindow_html);
		gtk_paned_pack2(GTK_PANED(hpaned), scrolledwindow_html, TRUE, TRUE);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
					       (scrolledwindow_html),
					       GTK_POLICY_NEVER,
					       GTK_POLICY_AUTOMATIC);
		dlg->html = gtk_html_new();
		gtk_widget_show(dlg->html);
		gtk_container_add(GTK_CONTAINER(scrolledwindow_html),
				  dlg->html);
		gtk_html_load_empty(GTK_HTML(dlg->html));
		g_signal_connect(GTK_OBJECT(dlg->html), "on_url",
				   G_CALLBACK(dialog_url),
				   (GBS_DATA *) dlg);
		g_signal_connect(GTK_OBJECT(dlg->html), "link_clicked",
				   G_CALLBACK(link_clicked),
				   (GBS_DATA *) dlg);
		g_signal_connect(GTK_OBJECT(dlg->html),
				   "button_press_event",
				   G_CALLBACK(button_press),
				   (GBS_DATA *) dlg);
	}
	else {
		dlg->html = gtk_moz_embed_new();
		gtk_widget_show(dlg->html);
		gtk_paned_pack2(GTK_PANED(hpaned), dlg->html, TRUE, TRUE);
		gtk_widget_realize(dlg->html);	
	}
#else	   
	scrolledwindow_html = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_html);
	gtk_paned_pack2(GTK_PANED(hpaned), scrolledwindow_html, FALSE, TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_html),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	dlg->html = gtk_html_new();
	gtk_widget_show(dlg->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_html),
			  dlg->html);
	gtk_html_load_empty(GTK_HTML(dlg->html));
	g_signal_connect(GTK_OBJECT(dlg->html), "on_url",
			   G_CALLBACK(dialog_url),
			   (GBS_DATA *) dlg);
	g_signal_connect(GTK_OBJECT(dlg->html), "link_clicked",
			   G_CALLBACK(link_clicked),
			   (GBS_DATA *) dlg);
	g_signal_connect(GTK_OBJECT(dlg->html),
			   "button_press_event",
			   G_CALLBACK(button_press),
			   (GBS_DATA *) dlg);
			   
#endif	

	g_signal_connect(selection, "changed",
			 G_CALLBACK(tree_selection_changed), dlg);
	dlg->statusbar = gtk_statusbar_new();
	gtk_widget_show(dlg->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), dlg->statusbar, FALSE,
			   FALSE, 0);
			   
	g_signal_connect(GTK_OBJECT(dlg->dialog), "destroy",
			   G_CALLBACK(dialog_destroy),
			   (GBS_DATA *) dlg);
}



/******************************************************************************
 * Name
 *   gui_gbs_dialog_goto_bookmark
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void gui_gbs_dialog_goto_bookmark(gchar * mod_name, gchar * key)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_gbs_dialog_goto_bookmark(gchar * mod_name, gchar * key)
{
	gchar *text;
	GList *tmp = NULL;
	tmp = g_list_first(dialog_list);
	while (tmp != NULL) {
		GBS_DATA *dlg = (GBS_DATA *) tmp->data;
		if(!strcmp(dlg->mod_name, mod_name)) {
			dlg->key = key;
			text = display_gbs(dlg->mod_name, key);	
			if(text){
				entry_display(dlg->html, dlg->mod_name,
				   text, key, TRUE);
				free(text);
			}
			gdk_window_raise(dlg->dialog->window);			
			return;
		}		
		tmp = g_list_next(tmp);
	}
	gui_open_gbs_dialog(mod_name);
	cur_dlg->key = key;
	text = display_gbs(cur_dlg->mod_name, key);	
	if(text){
		entry_display(cur_dlg->html, cur_dlg->mod_name,
		   text, key, TRUE);
		free(text);
	}
}

/******************************************************************************
 * Name
 *   gui_open_gbs_dialog
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void gui_open_gbs_dialog(gint mod_num)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_open_gbs_dialog(gchar * mod_name)
{
	GBS_DATA *dlg;
	GtkWidget *popupmenu;

	create_pixbufs();
	dlg = g_new0(GBS_DATA, 1);
	dlg->search_string = NULL;
	dlg->dialog = NULL;
	dlg->is_dialog = TRUE;
	dlg->mod_name = g_strdup(mod_name);
	dlg->is_rtol = is_module_rtl(dlg->mod_name);
	create_gbs_dialog(dlg);
	if (has_cipher_tag(dlg->mod_name)) {
		dlg->is_locked = module_is_locked(dlg->mod_name);
		dlg->cipher_old = get_cipher_key(dlg->mod_name);
	} else {
		dlg->is_locked = 0;
		dlg->cipher_old = NULL;
	}
	dlg->display_level = get_display_level(dlg->mod_name);
	if(!dlg->is_rtol) {
		popupmenu = gui_create_pm_gbs(dlg);
		gnome_popup_menu_attach(popupmenu, dlg->html, NULL);
	}
	gtk_widget_show(dlg->dialog);
	cur_dlg = dlg;
	dialog_list = g_list_append(dialog_list, (GBS_DATA *) dlg);
	
	add_book_to_tree(dlg);
}


/******************************************************************************
 * Name
 *   gui_setup_gbs_dialog
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void gui_setup_gbs_dialog(GList * mods)
 *
 * Description
 *   called at program start to init vars
 *
 * Return value
 *   void
 */

void gui_setup_gbs_dialog(GList * mods)
{
	dialog_list = NULL;
	dialog_freed = FALSE;
}

/******************************************************************************
 * Name
 *   gui_shutdown_gbs_dialog
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *  	void gui_shutdown_gbs_dialog(void)
 *
 * Description
 *   called at program shut down to free any remaining dialogs
 *
 * Return value
 *   void
 */

void gui_shutdown_gbs_dialog(void)
{
	dialog_list = g_list_first(dialog_list);
	while (dialog_list != NULL) {
		GBS_DATA *dlg = (GBS_DATA *) dialog_list->data;
		dialog_freed = TRUE;
		/* 
		 *  destroy any dialogs created 
		 */
		if (dlg->dialog)
			gtk_widget_destroy(dlg->dialog);
		/* 
		 * free each TEXT_DATA item created 
		 */
		if (dlg->mod_name)
			g_free(dlg->mod_name);
		g_free((GBS_DATA *) dialog_list->data);
		dialog_list = g_list_next(dialog_list);
	}
	g_list_free(dialog_list);
}

//******  end of file  ******/
