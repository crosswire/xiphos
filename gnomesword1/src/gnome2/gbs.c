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
#include <gal/e-paned/e-hpaned.h>

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
#include "gui/shortcutbar_viewer.h"
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

/******************************************************************************
 *  externs  
 */
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;
extern gboolean in_url;

/******************************************************************************
 *  static   
 */
static void add_node_children(GtkCTreeNode *node, GBS_DATA *gbs,
					unsigned long offset);
static gboolean gbs_find_running;
static GtkCTreeNode *rootnode;	
static GBS_DATA *cur_g;
static gint tree_level;	
/* list of gbs data structures */
static GList *gbs_list;	

/*
static void link_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	
}
*/

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

	retval = gtk_ctree_insert_node(GTK_CTREE(
					widgets.ctree_widget_books),
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
 
static void add_node_children(GtkCTreeNode *node, GBS_DATA * gbs,
		unsigned long offset)
{
	gchar buf[256];
	gchar *tmpbuf;
	GtkCTreeNode *tmp_parent_node = node;
	NODEDATA nodedata, *p_nodedata;
	
	
	p_nodedata = &nodedata;
	p_nodedata->sibling = NULL;
	p_nodedata->buf[1] = gbs->mod_name;

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
	
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_gbs),
			      page_num);
	
	text = display_gbs(cur_g->mod_name, key);
	
	if(text){
#ifdef USE_GTKEMBEDMOZ	
		if(!cur_g->is_rtol) 
			entry_display(cur_g->html, cur_g->mod_name,
		   		text, key, TRUE);
		else
			entry_display_mozilla(cur_g->html, cur_g->mod_name,
		   		text, key, TRUE);

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
	gchar *bookname, *nodename, *offset;
	GtkCTreeNode *treeNode;
	GtkCTreeRow *treerow;
	
	treeNode = gtk_ctree_node_nth(GTK_CTREE(gbs->ctree), row);
	widgets.ctree_widget_books = gbs->ctree;
	treerow = GTK_CTREE_ROW(treeNode); 
	
	nodename = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
					cell[0])->text;
	bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
					cell[1])->text;
	offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
					cell[2])->text;
	tree_level = treerow->level;
	
	gbs->offset = strtoul(offset, NULL, 0);
	
	change_book(bookname, gbs->offset);
	
	if (tree_level >= 1) {		
		/** fill ctree node with children **/
		if ((GTK_CTREE_ROW(treeNode)->children == NULL)
		    && (!GTK_CTREE_ROW(treeNode)->is_leaf)) {
			add_node_children(treeNode, gbs, 
				    gbs->offset);     
			gtk_ctree_expand(GTK_CTREE(gbs->ctree),
					 treeNode);
		} 
		gbs_display(gbs, offset, tree_level, treerow->is_leaf);
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
	gui_set_gbs_frame_label();
	
	change_book(g->mod_name, g->offset);
	/*
	 *  get the book key and store in settings.book_key
	 *  for adding bookmarks
	 */
	key = get_book_key(g->mod_name);
	if(key) {
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
//	strcpy(settings.sb_search_mod, g->mod_name);
	/*
	 * set search frame label to current gbs module 
	 */
//	gui_set_search_label();
	
	settings.BookWindowModule = g->mod_name;	
	xml_set_value("GnomeSword", "modules", "book",g->mod_name);
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
				GdkEventButton * event, GBS_DATA * g)
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
				gui_display_dictlex_in_viewer(
					settings.DictWindowModule,
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
	gtk_widget_show(p_gbs->frame);
	gtk_container_add(GTK_CONTAINER(p_gbs->vbox), p_gbs->frame);
	
	hpanedGBS = e_hpaned_new();
	gtk_widget_show(hpanedGBS);
	gtk_container_add(GTK_CONTAINER(p_gbs->frame), hpanedGBS);	
	e_paned_set_position(E_PANED(hpanedGBS),190);

	scrolledwindowCTREE_GBS = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindowCTREE_GBS);
	e_paned_pack1(E_PANED(hpanedGBS), scrolledwindowCTREE_GBS, TRUE,
						TRUE);
	
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowCTREE_GBS),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	p_gbs->ctree = gtk_ctree_new(3, 0);
	gtk_widget_show(p_gbs->ctree);
	gtk_container_add(GTK_CONTAINER(scrolledwindowCTREE_GBS),
			  p_gbs->ctree);
	gtk_container_set_border_width(GTK_CONTAINER(p_gbs->ctree), 1);
	gtk_clist_set_column_width(GTK_CLIST(p_gbs->ctree), 0, 680);
	gtk_clist_set_column_width(GTK_CLIST(p_gbs->ctree), 1, 1);
	gtk_clist_set_column_width(GTK_CLIST(p_gbs->ctree), 2, 1);
	gtk_clist_column_titles_hide(GTK_CLIST(p_gbs->ctree));

	label = gtk_label_new("");
	gtk_widget_show(label);
	gtk_clist_set_column_widget(GTK_CLIST(p_gbs->ctree), 0, label);

	label = gtk_label_new("");
	gtk_widget_show(label);
	gtk_clist_set_column_widget(GTK_CLIST(p_gbs->ctree), 1, label);

	label = gtk_label_new("");
	gtk_widget_show(label);
	gtk_clist_set_column_widget(GTK_CLIST(p_gbs->ctree), 2, label);

	frameGBS = gtk_frame_new(NULL);
	gtk_widget_show(frameGBS);
	e_paned_pack2(E_PANED(hpanedGBS), frameGBS, TRUE, TRUE);



#ifdef USE_GTKEMBEDMOZ
	if (!p_gbs->is_rtol) {
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
				   G_CALLBACK(gui_link_clicked), 
				   NULL);
		gtk_signal_connect(GTK_OBJECT(p_gbs->html), "on_url",
				   G_CALLBACK(gui_url), 
				   (gpointer) widgets.app);
		gtk_signal_connect(GTK_OBJECT(p_gbs->html),
				   "button_release_event",
				   G_CALLBACK(on_button_release_event),
				   p_gbs);
	}
	else {
		//gtk_moz_embed_set_comp_path("usr/lib/mozilla-1.0.1");
		p_gbs->html = gtk_moz_embed_new();
		gtk_widget_show(p_gbs->html);
		gtk_container_add(GTK_CONTAINER(frameGBS),
				  p_gbs->html);
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
			   G_CALLBACK(gui_link_clicked), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(p_gbs->html), "on_url",
			   G_CALLBACK(gui_url), 
			   (gpointer) widgets.app);
	gtk_signal_connect(GTK_OBJECT(p_gbs->html),
			   "button_release_event",
			   G_CALLBACK(on_button_release_event),
			   p_gbs);
#endif			   


	gtk_signal_connect(GTK_OBJECT(p_gbs->ctree), "select_row",
			   G_CALLBACK(on_ctreeGBS_select_row),
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
	cur_g = g;
	if(page)
		gtk_notebook_set_page(GTK_NOTEBOOK(
				  widgets.notebook_gbs), page);
	else
		on_notebook_gbs_switch_page(GTK_NOTEBOOK(
				  widgets.notebook_gbs),
				  NULL,
				  page, 
				  gbs_list);	
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
	
	create_gbs_pane(g);
	
#ifdef USE_GTKEMBEDMOZ	
	if (!g->is_rtol) {
		popupmenu = gui_create_pm_gbs(g); 		
		gnome_popup_menu_attach(popupmenu, g->html, NULL);
	}
#else
	popupmenu = gui_create_pm_gbs(g); 
	gnome_popup_menu_attach(popupmenu, g->html, NULL);
#endif
	
	add_book_to_ctree(g->ctree, g->mod_name);
	gtk_ctree_select(GTK_CTREE(g->ctree),rootnode);
	on_ctreeGBS_select_row((GtkCList *) g->ctree,0,
			0, NULL, g);
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
		gbs->key = NULL;
		gbs->mod_num = count;
		gbs->offset = 0;
		gbs->is_dialog = FALSE;	
		gbs->has_key = module_is_locked(gbs->mod_name);
		if (has_cipher_tag(gbs->mod_name)) {
			gbs->is_locked 
				= module_is_locked(gbs->mod_name);
			gbs->cipher_old 
				= get_cipher_key(gbs->mod_name);
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

	gtk_signal_connect(GTK_OBJECT(widgets.notebook_gbs), "switch_page",
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
