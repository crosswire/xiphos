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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>

#include "gui/gtkhtml_display.h"
#include "gui/gbs.h"
#include "gui/cipher_key_dialog.h"
#include "gui/gbs_find.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"

#include "main/settings.h"
#include "main/lists.h"
#include "main/gbs.h"
#include "main/gs_bookmarks.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"

/******************************************************************************
 *  externs  
 */
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;

/******************************************************************************
 *  static   
 */
static void add_node_children(GtkCTreeNode *node, gchar *bookname,
					unsigned long offset);
/* list of gbs data structures */
static GList *gbs_list;		
static GBS_DATA *cur_g;
static gboolean in_url;
static gboolean gbs_find_running;


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
 *  add_node_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   GtkCTreeNode *add_node_gbs(NODEDATA *data)	
 *
 * Description
 *    
 *
 * Return value
 *   GtkCTreeNode*
 */ 
 
static GtkCTreeNode *add_node_gbs(NODEDATA * data)
{
	GtkCTreeNode *retval;

	retval = gtk_ctree_insert_node(GTK_CTREE(settings.ctree_widget_books),
				       data->parent,
				       data->sibling,
				       data->buf,
				       3,
				       data->pixmap1,
				       data->mask1,
				       data->pixmap2,
				       data->mask2,
				       data->is_leaf, data->expanded);
	return retval;
}
		
/******************************************************************************
 * Name
 *  add_node_children
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void add_node_children(GtkCTreeNode *node, gchar *bookname,
 *   				unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
static void add_node_children(GtkCTreeNode *node, gchar *bookname,
		unsigned long offset)
{
	gchar buf[256];
	gchar *tmpbuf;
	GtkCTreeNode *tmp_parent_node = node;
	NODEDATA nodedata, *p_nodedata;

	p_nodedata = &nodedata;
	p_nodedata->sibling = NULL;
	p_nodedata->buf[1] = bookname;

	if (gbs_treekey_first_child(offset)) {
		offset = gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_nodedata->parent = node;
		p_nodedata->buf[2] = buf;
		tmpbuf =
		    gbs_get_treekey_local_name(offset);
		p_nodedata->buf[0] = (gchar*)tmpbuf;
		if (gbs_treekey_has_children(offset)) {
			p_nodedata->pixmap1 = pixmap1;
			p_nodedata->mask1 = mask1;
			p_nodedata->pixmap2 = pixmap2;
			p_nodedata->mask2 = mask2;
			p_nodedata->is_leaf = FALSE;
			p_nodedata->expanded = FALSE;
		} else {
			p_nodedata->pixmap1 = pixmap3;
			p_nodedata->mask1 = mask3;
			p_nodedata->pixmap2 = NULL;
			p_nodedata->mask2 = NULL;
			p_nodedata->is_leaf = TRUE;
			p_nodedata->expanded = FALSE;
		}
		node = add_node_gbs(p_nodedata);
		free(tmpbuf);
	}

	while (treekey_next_sibling(offset)) {
		offset = gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_nodedata->parent = tmp_parent_node;
		p_nodedata->buf[2] = buf;
		tmpbuf =
		    gbs_get_treekey_local_name(offset);
		p_nodedata->buf[0] = (gchar*)tmpbuf;
		if (gbs_treekey_has_children(offset)) {
			p_nodedata->pixmap1 = pixmap1;
			p_nodedata->mask1 = mask1;
			p_nodedata->pixmap2 = pixmap2;
			p_nodedata->mask2 = mask2;
			p_nodedata->is_leaf = FALSE;
			p_nodedata->expanded = FALSE;
		} else {
			p_nodedata->pixmap1 = pixmap3;
			p_nodedata->mask1 = mask3;
			p_nodedata->pixmap2 = NULL;
			p_nodedata->mask2 = NULL;
			p_nodedata->is_leaf = TRUE;
			p_nodedata->expanded = FALSE;
		}
		node = add_node_gbs(p_nodedata);
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
	
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_gbs),
			      page_num);
	
	text = display_gbs(cur_g->mod_name, key);
	
	if(text){
		entry_display(cur_g->html, cur_g->mod_name,
		   text, key, TRUE);
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

void gui_set_gbs_frame_label(GBS_DATA *g)
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module name
	 */	
	if (settings.book_tabs)
		gtk_frame_set_label(GTK_FRAME(g->frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(g->frame), g->mod_name);
	
}

/******************************************************************************
 * Name
 *  on_ctreeGBS_select_row
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_ctreeGBS_select_row(GtkCList * clist, gint row,
 *			gint column, GdkEvent * event, GBS_DATA * gbs)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_ctreeGBS_select_row(GtkCList * clist, gint row,
			gint column, GdkEvent * event, GBS_DATA * gbs)
{
	gchar *bookname, *nodename, *offset, *key, *text = NULL;
	GtkCTreeNode *treeNode;
	
	treeNode = gtk_ctree_node_nth(GTK_CTREE(gbs->ctree), row);
	settings.ctree_widget_books = gbs->ctree;

	nodename = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
					cell[0])->text;
	bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
					cell[1])->text;
	offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
					cell[2])->text;
	gbs->offset = strtoul(offset, NULL, 0);
	
	text = display_row_gbs(gbs->mod_name, offset);
	if (text) {
	/** fill ctree node with children **/
		if ((GTK_CTREE_ROW(treeNode)->children == NULL)
		    && (!GTK_CTREE_ROW(treeNode)->is_leaf)) {
			add_node_children(treeNode, bookname, 
				    strtoul(offset, NULL, 0));     
			gtk_ctree_expand(GTK_CTREE(gbs->ctree),
					 treeNode);
		}
		key = get_book_key(gbs->mod_name);
		if(key) {
			strcpy(settings.book_key, key);
		} else {
			settings.book_key[0] = '\0';
		}	
		
		entry_display(gbs->html, gbs->mod_name,
		   text, key, TRUE);
		if(key) 
			free(key);
		free(text);
	} else {
		settings.book_key[0] = '\0';
	}
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
		GtkNotebookPage * page,	gint page_num, GList * data_gbs)
{
	GBS_DATA *g, *g_old;
	gchar *key;
	
	g_old =
	    (GBS_DATA *) g_list_nth_data(data_gbs,
					 settings.book_last_page);
	g = (GBS_DATA *) g_list_nth_data(data_gbs, page_num);
	cur_g = g;
	
	if(!g->frame)
		gui_add_new_gbs_pane(g);
	gui_set_gbs_frame_label(g);
	
	change_book(g->mod_name, g->offset);
	/*
	 *  get the book key and store in settings.book_key
	 *  for adding bookmarks
	 */
	key = get_book_key(g->mod_name);
	if(key) {
		strcpy(settings.book_key, key);
		free(key);
	} else {
		settings.book_key[0] = '\0';
	}
	
	
	strcpy(settings.BookWindowModule, g->mod_name);

	if (gbs_find_running) {
		gbs_find_close_dialog(NULL, g_old->find_dialog);
		search_gbs_find_dlg(g, FALSE, settings.findText);
	}
	GTK_CHECK_MENU_ITEM(g->showtabs)->active = settings.book_tabs;
	settings.book_last_page = page_num;	
	settings.html_book = g->html;
}

/******************************************************************************
 * Name
 *  on_copy_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   	void on_copy_activate(GtkMenuItem * menuitem, GBS_DATA * gbs)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_copy_activate(GtkMenuItem * menuitem, GBS_DATA * gbs)
{
	gtk_html_copy(GTK_HTML(gbs->html));
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

static void on_bookmark_activate(GtkMenuItem * menuitem, GBS_DATA * gbs)
{
	gchar *key = get_book_key(gbs->mod_name);
	if(key){
		add_bookmark_to_tree(NULL, gbs->mod_name, key);
		free(key);
	}
}

/******************************************************************************
 * Name
 *  on_find_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_find_activate(GtkMenuItem * menuitem, GBS_DATA * gbs)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_find_activate(GtkMenuItem * menuitem, GBS_DATA * gbs)
{
	if(!gbs_find_running) search_gbs_find_dlg(gbs, FALSE, NULL);
}

/******************************************************************************
 * Name
 *  on_lookup_word_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_lookup_word_activate(GtkMenuItem * menuitem,
					gchar * modDescription)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_lookup_word_activate(GtkMenuItem * menuitem,
					gchar * modDescription)
{
	gchar mod_name[16];
	gchar *key;
	
	memset(mod_name, 0, 16);
	module_name_from_description(mod_name, modDescription);
	key = get_word_or_selection(cur_g->html, TRUE);
	if (key && mod_name) {
		gui_display_dictlex_in_viewer(mod_name, key);
		g_free(key);
		g_free(mod_name);
	}
}

/******************************************************************************
 * Name
 *  on_lookup_selection_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   	void on_lookup_selection_activate(GtkMenuItem * menuitem,
 *					gchar * modDescription)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_lookup_selection_activate(GtkMenuItem * menuitem,
					gchar * modDescription)
{
	gchar mod_name[16];
	gchar *key;
	
	memset(mod_name, 0, 16);
	module_name_from_description(mod_name, modDescription);	
	key = get_word_or_selection(cur_g->html, FALSE);
	if (key && mod_name) {
		gui_display_dictlex_in_viewer(mod_name, key);
		g_free(key);
		g_free(mod_name);
	}
}

/******************************************************************************
 * Name
 *  on_same_lookup_word_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_same_lookup_word_activate(GtkMenuItem * menuitem, 
 *							GBS_DATA * g)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_same_lookup_word_activate(GtkMenuItem * menuitem, 
							GBS_DATA * g)
{
	gchar *key = get_word_or_selection(g->html, TRUE);
	if (key) {
		gui_display_dictlex_in_viewer(settings.DictWindowModule,
					  key);
		g_free(key);
	}
}

/******************************************************************************
 * Name
 *  on_same_lookup_selection_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
 *							GBS_DATA * g)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
							GBS_DATA * g)
{
	gchar *key = get_word_or_selection(g->html, FALSE);
	if (key) {
		gui_display_dictlex_in_viewer(settings.DictWindowModule,
					  key);
		g_free(key);
	}
}

/******************************************************************************
 * Name
 *  on_view_book_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_view_book_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_view_book_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{
	gint page;

	page = GPOINTER_TO_INT(user_data);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_gbs),
			      page);
}

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
				GdkEventButton * event, GBS_DATA * g)
{
	gchar *key;
	
	settings.whichwindow = BOOK_WINDOW;
	
	switch (event->button) {
	case 1:
		if (!in_url) {
			key = buttonpresslookupGS_HTML(g->html);
			if (key) {
				gui_display_dictlex_in_viewer(settings.DictWindowModule,
							  key
							  );
				g_free(key);
			}
			return TRUE;
		}
		break;
	case 2:
		break;
	case 3:
		break;
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *  on_showtabs_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_showtabs_activate(GtkMenuItem *menuitem, GBS_DATA *g)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_showtabs_activate(GtkMenuItem *menuitem, GBS_DATA *g)
{
	settings.book_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebook_gbs),
				   settings.book_tabs);
	gui_set_gbs_frame_label(g);
}

/******************************************************************************
 * Name
 *  on_unlock_key_activate
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void on_unlock_key_activate(GtkMenuItem * menuitem, GBS_DATA * g)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void on_unlock_key_activate(GtkMenuItem * menuitem, GBS_DATA * g)
{
	GtkWidget *dlg;
	
	dlg = gui_create_cipher_key_dialog(g->mod_name);
	gtk_widget_show(dlg);
}

/******************************************************************************
 * Name
 *  gui_create_pm_gbs
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   GtkWidget *gui_create_pm_gbs(GBS_DATA * gbs)	
 *
 * Description
 *    
 *
 * Return value
 *   GtkWidget *
 */ 

static GtkWidget *create_pm_gbs(GBS_DATA * gbs)
{
	GtkWidget *pmGBS;
	GtkAccelGroup *pmGBS_accels;
	GtkWidget *copy;
	GtkWidget *separator;
	GtkWidget *bookmark;
	GtkWidget *lookup_word;
	GtkWidget *lookup_word_menu;
	GtkAccelGroup *lookup_word_menu_accels;
	GtkWidget *usecurrent1;
	GtkWidget *lookup_selection;
	GtkWidget *lookup_selection_menu;
	GtkAccelGroup *lookup_selection_menu_accels;
	GtkWidget *usecurrent2;
	GtkWidget *item3;
	GtkWidget *item4;
	GtkWidget *view_book;
	GtkWidget *view_book_menu;
	GtkAccelGroup *view_book_menu_accels;
	GtkWidget *find;
	GList *tmp;
	gint i;

	tmp = NULL;

	pmGBS = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmGBS), "pmGBS", pmGBS);
	pmGBS_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(pmGBS));

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_ref(copy);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "copy", copy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(pmGBS), copy);
	
	bookmark = gtk_menu_item_new_with_label(_("Bookmark"));
	gtk_widget_ref(bookmark);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "bookmark", bookmark,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(bookmark);
	gtk_container_add(GTK_CONTAINER(pmGBS), bookmark);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_ref(find);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "find", find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(pmGBS), find);

	gbs->showtabs =
	    gtk_check_menu_item_new_with_label(_("Show Tabs"));
	gtk_widget_ref(gbs->showtabs);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "gbs->showtabs",
				 gbs->showtabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(gbs->showtabs);
	gtk_container_add(GTK_CONTAINER(pmGBS), gbs->showtabs);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "separator",
				 separator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pmGBS), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	lookup_word = gtk_menu_item_new_with_label(_("Lookup Word"));
	gtk_widget_ref(lookup_word);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "lookup_word",
				 lookup_word,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lookup_word);
	gtk_container_add(GTK_CONTAINER(pmGBS), lookup_word);

	lookup_word_menu = gtk_menu_new();
	gtk_widget_ref(lookup_word_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "lookup_word_menu",
				 lookup_word_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lookup_word),
				  lookup_word_menu);
	lookup_word_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (lookup_word_menu));

	usecurrent1 =
	    gtk_menu_item_new_with_label("Use Current Dictionary");
	gtk_widget_ref(usecurrent1);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "usecurrent1",
				 usecurrent1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(usecurrent1);
	gtk_container_add(GTK_CONTAINER(lookup_word_menu), usecurrent1);


	lookup_selection =
	    gtk_menu_item_new_with_label(_("Lookup Selection"));
	gtk_widget_ref(lookup_selection);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "lookup_selection",
				 lookup_selection,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lookup_selection);
	gtk_container_add(GTK_CONTAINER(pmGBS), lookup_selection);

	lookup_selection_menu = gtk_menu_new();
	gtk_widget_ref(lookup_selection_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS),
				 "lookup_selection_menu",
				 lookup_selection_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lookup_selection),
				  lookup_selection_menu);
	lookup_selection_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (lookup_selection_menu));

	usecurrent2 =
	    gtk_menu_item_new_with_label("Use Current Dictionary");
	gtk_widget_ref(usecurrent2);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "usecurrent2",
				 usecurrent2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(usecurrent2);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent2);


	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "separator",
				 separator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pmGBS), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	view_book = gtk_menu_item_new_with_label(_("View Book"));
	gtk_widget_ref(view_book);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "view_book",
				 view_book,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(view_book);
	gtk_container_add(GTK_CONTAINER(pmGBS), view_book);

	view_book_menu = gtk_menu_new();
	gtk_widget_ref(view_book_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "view_book_menu",
				 view_book_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_book),
				  view_book_menu);
	view_book_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(view_book_menu));
	
	/*
	   if module has cipher key include this item
	 */
	if(gbs->has_key) {
		GtkWidget *add_module_key;
		separator = gtk_menu_item_new();
		gtk_widget_ref(separator);
		gtk_object_set_data_full(GTK_OBJECT(pmGBS), "separator",
					 separator, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(separator);	
		gtk_container_add(GTK_CONTAINER(pmGBS), separator);
		gtk_widget_set_sensitive(separator, FALSE);
			
		add_module_key = gtk_menu_item_new_with_label (_("Unlock This Module"));
		gtk_widget_ref (add_module_key);
		gtk_object_set_data_full (GTK_OBJECT (pmGBS), "add_module_key",add_module_key ,
				    (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show (add_module_key);
		gtk_container_add (GTK_CONTAINER (pmGBS), add_module_key);
			
		gtk_signal_connect (GTK_OBJECT (add_module_key), "activate",
                      	GTK_SIGNAL_FUNC (on_unlock_key_activate),
                      	gbs);
	}
	
	tmp = get_list(DICT_DESC_LIST);//mod_lists->dict_descriptions; //backend_get_mod_description_list_SWORD(DICT_MODS);
	while (tmp != NULL) {
		item3 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		item4 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_widget_ref(item4);
		gtk_object_set_data_full(GTK_OBJECT(pmGBS), "item3",
					 item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_object_set_data_full(GTK_OBJECT(pmGBS), "item4",
					 item4,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);
		gtk_widget_show(item4);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_lookup_word_activate),
				   (gchar *) tmp->data);
		gtk_signal_connect(GTK_OBJECT(item4), "activate",
				   GTK_SIGNAL_FUNC
				   (on_lookup_selection_activate),
				   (gchar *) tmp->data);
		gtk_container_add(GTK_CONTAINER(lookup_word_menu),
				  item3);
		gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
				  item4);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	i = 0;
	tmp = get_list(GBS_DESC_LIST);
	while (tmp != NULL) {
		item3 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_object_set_data_full(GTK_OBJECT(pmGBS), "item3",
					 item3, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_view_book_activate),
				   GINT_TO_POINTER(i));
		gtk_container_add(GTK_CONTAINER(view_book_menu), item3);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	/*** these two are for using the current dictionary for lookup ***/
	gtk_signal_connect(GTK_OBJECT(usecurrent1), "activate",
			   GTK_SIGNAL_FUNC
			   (on_same_lookup_word_activate), gbs);
	gtk_signal_connect(GTK_OBJECT(usecurrent2), "activate",
			   GTK_SIGNAL_FUNC
			   (on_same_lookup_selection_activate), gbs);

	gtk_signal_connect(GTK_OBJECT(copy), "activate",
			   GTK_SIGNAL_FUNC(on_copy_activate), gbs);
	gtk_signal_connect(GTK_OBJECT(bookmark), "activate",
			   GTK_SIGNAL_FUNC(on_bookmark_activate), gbs);
	gtk_signal_connect(GTK_OBJECT(find), "activate",
			   GTK_SIGNAL_FUNC(on_find_activate), gbs);
	gtk_signal_connect(GTK_OBJECT(gbs->showtabs), "activate",
			   GTK_SIGNAL_FUNC(on_showtabs_activate),
			   gbs);
	return pmGBS;
}

/******************************************************************************
 * Name
 *  gui_create_gbs_pane
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void gui_create_gbs_pane(GBS_DATA *p_gbs)	
 *
 * Description
 *    
 *
 * Return value
 *  void 
 */ 

static void create_gbs_pane(GBS_DATA *p_gbs)
{

	GtkWidget *hpanedGBS;
	GtkWidget *scrolledwindowCTREE_GBS;
	GtkWidget *label;
	GtkWidget *frameGBS;
	GtkWidget *scrolledwindowHTML_GBS;

	p_gbs->frame = gtk_frame_new(NULL);
	gtk_widget_ref(p_gbs->frame);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p_gbs->frame",
				 p_gbs->frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p_gbs->frame);
	gtk_container_add(GTK_CONTAINER(p_gbs->vbox), p_gbs->frame);
	
	hpanedGBS = gtk_hpaned_new();
	gtk_widget_ref(hpanedGBS);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "hpanedGBS",
				 hpanedGBS,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpanedGBS);
	gtk_container_add(GTK_CONTAINER(p_gbs->frame), hpanedGBS);
	gtk_paned_set_position(GTK_PANED(hpanedGBS), 239);

	scrolledwindowCTREE_GBS = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowCTREE_GBS);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindowCTREE_GBS",
				 scrolledwindowCTREE_GBS,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindowCTREE_GBS);
	gtk_paned_pack1(GTK_PANED(hpanedGBS), scrolledwindowCTREE_GBS,
			FALSE, TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowCTREE_GBS),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	p_gbs->ctree = gtk_ctree_new(3, 0);
	gtk_widget_ref(p_gbs->ctree);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p_gbs->ctree",
				 p_gbs->ctree,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p_gbs->ctree);
	gtk_container_add(GTK_CONTAINER(scrolledwindowCTREE_GBS),
			  p_gbs->ctree);
	gtk_container_set_border_width(GTK_CONTAINER(p_gbs->ctree), 1);
	gtk_clist_set_column_width(GTK_CLIST(p_gbs->ctree), 0, 680);
	gtk_clist_set_column_width(GTK_CLIST(p_gbs->ctree), 1, 1);
	gtk_clist_set_column_width(GTK_CLIST(p_gbs->ctree), 2, 1);
	gtk_clist_column_titles_hide(GTK_CLIST(p_gbs->ctree));

	label = gtk_label_new(_("label"));
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_clist_set_column_widget(GTK_CLIST(p_gbs->ctree), 0, label);

	label = gtk_label_new(_("label200"));
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_clist_set_column_widget(GTK_CLIST(p_gbs->ctree), 1, label);

	label = gtk_label_new(_("label"));
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_clist_set_column_widget(GTK_CLIST(p_gbs->ctree), 2, label);

	frameGBS = gtk_frame_new(NULL);
	gtk_widget_ref(frameGBS);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frameGBS",
				 frameGBS,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameGBS);
	gtk_paned_pack2(GTK_PANED(hpanedGBS), frameGBS, TRUE, TRUE);

	scrolledwindowHTML_GBS = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowHTML_GBS);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindowHTML_GBS",
				 scrolledwindowHTML_GBS,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindowHTML_GBS);
	gtk_container_add(GTK_CONTAINER(frameGBS),
			  scrolledwindowHTML_GBS);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowHTML_GBS),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	p_gbs->html = gtk_html_new();
	
	gtk_widget_ref(p_gbs->html);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "p_gbs->html", p_gbs->html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p_gbs->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowHTML_GBS),
			  p_gbs->html);
	gtk_html_load_empty(GTK_HTML(p_gbs->html));


	gtk_signal_connect(GTK_OBJECT(p_gbs->ctree), "select_row",
			   GTK_SIGNAL_FUNC(on_ctreeGBS_select_row),
			   p_gbs);
	gtk_signal_connect(GTK_OBJECT(p_gbs->html), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(p_gbs->html), "on_url",
			   GTK_SIGNAL_FUNC(on_url), 
			   (gpointer) settings.app);
	gtk_signal_connect(GTK_OBJECT(p_gbs->html),
			   "button_release_event",
			   GTK_SIGNAL_FUNC(on_button_release_event),
			   p_gbs);
}

/******************************************************************************
 * Name
 *  add_book_to_ctree
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void add_book_to_ctree(GtkWidget * ctree, gchar * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
static void add_book_to_ctree(GtkWidget * ctree, gchar * mod_name)
{

	GtkCTreeNode *rootnode;
	gchar *buf[3];

	buf[0] = mod_name;
	buf[1] = mod_name;
	buf[2] = "0";
	rootnode = gtk_ctree_insert_node(GTK_CTREE(ctree),
					 NULL, NULL, buf, 3, pixmap1,
					 mask1, pixmap2, mask2, FALSE,
					 FALSE);

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
	while (gbs_list != NULL) {
		g = (GBS_DATA *) gbs_list->data;
		if (!strcmp(g->mod_name, book_name))
			break;
		++page;
		gbs_list = g_list_next(gbs_list);
	}
		
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_gbs), page);
	gui_set_gbs_frame_label(g);
		
	settings.book_last_page = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebook_gbs),
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
	
	create_gbs_pane(g);
	popupmenu = create_pm_gbs(g);
	gnome_popup_menu_attach(popupmenu, g->html, NULL);
	add_book_to_ctree(g->ctree, g->mod_name);
	
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
	gtk_widget_ref(g->vbox);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), 
			"g->vbox", g->vbox,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(g->vbox);
	gtk_container_add(GTK_CONTAINER(settings.notebook_gbs), g->vbox);
	
	
	label = gtk_label_new(g->mod_name);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label",
				 label, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.notebook_gbs),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(settings.notebook_gbs),
				    g->mod_num), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (settings.notebook_gbs),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (settings.notebook_gbs),
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
 
void gui_setup_gbs(GList *mods)
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
		bookname = (gchar *)tmp->data;
		gbs = g_new(GBS_DATA, 1);
		gbs->frame = NULL;
		gbs->mod_name = bookname;
		gbs->search_string = NULL;
		gbs->mod_num = count;
		gbs->offset = 0;
		gbs->find_dialog = NULL;	
		gbs->has_key = module_is_locked(gbs->mod_name);
		add_vbox_to_notebook(gbs);
		gbs_list = g_list_append(gbs_list, (GBS_DATA *) gbs);		
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(settings.notebook_gbs), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook_gbs_switch_page),
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
		GBS_DATA *g = (GBS_DATA *) gbs_list->data;
		if (g->find_dialog)
			g_free(g->find_dialog);	//-- free any search dialogs created
		g_free((GBS_DATA *) gbs_list->data);
		gbs_list = g_list_next(gbs_list);
	}
	g_list_free(gbs_list);
}
