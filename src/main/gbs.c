/*
 * GnomeSword Bible Study Tool
 * gbs.c - generic book support - the glue
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
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

/* frontend */
#include "_gbs.h"
#include "cipher_key_dialog.h"

/* main */
#include "settings.h"
#include "gs_bookmarks.h"
#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gbs.h"
#include "gs_html.h"

/* backend */
#include "sword.h"
#include "shortcutbar.h"
#include "gbs_.h"

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
extern gboolean gbs_find_running;

/******************************************************************************
 * globals 
 */
GBS_DATA *cur_g;

/******************************************************************************
 * global to this file only
 */
 
/* list of gbs data structures */
static GList *gbs_list;
 
 
/******************************************************************************
 * Name
 *  get_book_key
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   char *get_book_key(int book_num)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 
 
char *get_book_key(int book_num)
{
	return backend_get_book_key(book_num);
}

/******************************************************************************
 * Name
 *  display_row_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   gboolean display_row_gbs(gint book_num, gchar *offset)	
 *
 * Description
 *    
 *
 * Return value
 *   gboolean
 */ 
 
gboolean display_row_gbs(gint book_num, gchar *offset)
{
	return backend_display_row_gbs(book_num, offset);
}

/******************************************************************************
 * Name
 *  set_book_page_and_key
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void set_book_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void set_book_page_and_key(gint page_num, gchar * key)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_gbs),
			      page_num);
	backend_display_gbs(page_num, key);
}

/******************************************************************************
 * Name
 *  add_node_children
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void add_node_children(SETTINGS *s, GtkCTreeNode *node, gchar *bookname,
						unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void add_node_children(SETTINGS *s, GtkCTreeNode *node, gchar *bookname,
						unsigned long offset)
{
	gchar buf[256], *tmpbuf;
	GtkCTreeNode *tmp_parent_node = node;
	NODEDATA nodedata, *p_nodedata;

	p_nodedata = &nodedata;
	p_nodedata->sibling = NULL;
	p_nodedata->buf[1] = bookname;

	if (backend_gbs_treekey_first_child(bookname, offset)) {
		offset = backend_gbs_get_treekey_offset(bookname);
		sprintf(buf, "%lu", offset);
		p_nodedata->parent = node;
		p_nodedata->buf[2] = buf;
		tmpbuf =
		    backend_gbs_get_treekey_local_name(bookname, offset);
		p_nodedata->buf[0] = tmpbuf;
		if (backend_gbs_treekey_has_children(bookname, offset)) {
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
		node = add_node_gbs(s, p_nodedata);
		g_free(tmpbuf);
	}

	while (backend_treekey_next_sibling(bookname, offset)) {
		offset = backend_gbs_get_treekey_offset(bookname);
		sprintf(buf, "%lu", offset);
		p_nodedata->parent = tmp_parent_node;
		p_nodedata->buf[2] = buf;
		tmpbuf =
		    backend_gbs_get_treekey_local_name(bookname, offset);
		p_nodedata->buf[0] = tmpbuf;
		if (backend_gbs_treekey_has_children(bookname, offset)) {
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
		node = add_node_gbs(s, p_nodedata);
		g_free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *  add_book_to_ctree
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void add_book_to_ctree(GtkWidget * ctree, gchar * bookName)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
static void add_book_to_ctree(GtkWidget * ctree, gchar * bookName)
{

	GtkCTreeNode *rootnode;
	gchar *buf[3];

	buf[0] = bookName;
	buf[1] = bookName;
	buf[2] = "0";
	rootnode = gtk_ctree_insert_node(GTK_CTREE(ctree),
					 NULL, NULL, buf, 3, pixmap1,
					 mask1, pixmap2, mask2, FALSE,
					 FALSE);

}

/******************************************************************************
 * Name
 *  add_node_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   GtkCTreeNode *add_node_gbs(SETTINGS * s, NODEDATA * data)	
 *
 * Description
 *    
 *
 * Return value
 *   GtkCTreeNode*
 */ 
 
GtkCTreeNode *add_node_gbs(SETTINGS * s, NODEDATA * data)
{
	GtkCTreeNode *retval;

	retval = gtk_ctree_insert_node(GTK_CTREE(s->ctree_widget_books),
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
		if (!strcmp(g->bookName, settings.BookWindowModule)) {
			break;
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return g;
}

/******************************************************************************
 * Name
 *  setup_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   GList* setup_gbs(SETTINGS * s)	
 *
 * Description
 *    
 *
 * Return value
 *   GList*
 */ 
 
void setup_gbs(SETTINGS * s, GList *mods)
{
	GtkWidget *popupmenu;
	gint count = 0;
	GList *tmp = NULL;
	gchar *bookname;
	GBS_DATA *gbs;
	
	gbs_list = NULL;
	gbs_find_running = FALSE;
	
	//mods = backend_get_list_of_mods_by_type(BOOK_MODS);
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		bookname = (gchar *) tmp->data;
		gbs = g_new(GBS_DATA, 1);
		gbs->bookName = bookname;
		gbs->searchstring = NULL;
		gbs->booknum = count;
		gbs->find_dialog = NULL;	
		gbs->has_key = backend_module_is_locked(gbs->bookName);
		gui_create_gbs_pane(bookname, s, count, gbs);
		popupmenu = gui_create_pm_gbs(gbs);
		gnome_popup_menu_attach(popupmenu, gbs->html, NULL);
		backend_new_gbs_display(gbs->html, gbs->bookName, s);
		add_book_to_ctree(gbs->ctree, gbs->bookName);
		gbs_list = g_list_append(gbs_list, (GBS_DATA *) gbs);
		sprintf(s->BookWindowModule, "%s", gbs->bookName);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(s->notebook_gbs), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook_gbs_switch_page),
			   gbs_list);

	settings.book_last_page = 0;
	g_list_free(tmp);
	return mods;
}

/******************************************************************************
 * Name
 *  shutdown_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void shutdown_gbs(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void shutdown_gbs(void)
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
