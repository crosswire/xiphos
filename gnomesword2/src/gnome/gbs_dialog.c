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
#include <gal/e-paned/e-hpaned.h>

#ifdef USE_GTKEMBEDMOZ
#include <gtkmozembed.h>
#endif

#include "gui/gtkhtml_display.h"
#include "gui/gbs_dialog.h"
#include "gui/gbs_display.h"
#include "gui/gbs_menu.h"
#include "gui/html.h"

#include "main/gbs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"


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
 * static - global to this file only
 */
static GList *dialog_list;
static GBS_DATA *cur_dlg;
static gboolean dialog_freed;
static GtkCTreeNode *rootnode;


/******************************************************************************
 * Name
 *  add_book_to_ctree
 *
 * Synopsis
 *   #include "gbs_dialog.h"
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
 *  add_node_gbs
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   GtkCTreeNode *add_node_gbs(GBS_DATA * dlg, NODEDATA *data)	
 *
 * Description
 *    
 *
 * Return value
 *   GtkCTreeNode*
 */

static GtkCTreeNode *add_node_gbs(GBS_DATA * dlg, NODEDATA * data)
{
	GtkCTreeNode *retval;

	retval = gtk_ctree_insert_node(GTK_CTREE(dlg->ctree),
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
 *   #include "gbs_dialog.h"
 *
 *   void add_node_children(GBS_DATA * dlg, GtkCTreeNode *node, 
 *			gchar *bookname, unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void add_node_children(GBS_DATA * dlg, GtkCTreeNode * node,
			      gchar * bookname, unsigned long offset)
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
		tmpbuf = gbs_get_treekey_local_name(offset);
		p_nodedata->buf[0] = (gchar *) tmpbuf;
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
		node = add_node_gbs(dlg, p_nodedata);
		free(tmpbuf);
	}

	while (treekey_next_sibling(offset)) {
		offset = gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_nodedata->parent = tmp_parent_node;
		p_nodedata->buf[2] = buf;
		tmpbuf = gbs_get_treekey_local_name(offset);
		p_nodedata->buf[0] = (gchar *) tmpbuf;
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
		node = add_node_gbs(dlg, p_nodedata);
		free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *  on_ctreeGBS_select_row
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void on_ctreeGBS_select_row(GtkCTree *ctree, GList *node,
 *					gint column, GBS_DATA * dlg)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void ctree_select_row(GtkCTree * ctree, GList * node,
			     gint column, GBS_DATA * dlg)
{
	gchar *bookname, *nodename, *offset;
	GtkCTreeNode *treeNode;
	GtkCTreeRow *treerow;

	treeNode = GTK_CTREE_NODE(node);
	treerow = GTK_CTREE_ROW(treeNode); 

	nodename = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.
				    cell[0])->text;
	bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.
				    cell[1])->text;
	offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.
				  cell[2])->text;
	dlg->offset = strtoul(offset, NULL, 0);

	change_book(bookname, dlg->offset);
	
	if (treerow->level >= 1) {		
		/* fill ctree node with children */
		if ((GTK_CTREE_ROW(node)->children == NULL)
		    && (!GTK_CTREE_ROW(node)->is_leaf)) {
			add_node_children(dlg, treeNode, bookname,
					  strtoul(offset, NULL, 0));
			gtk_ctree_expand(GTK_CTREE(dlg->ctree),
					 treeNode);
		}
		gbs_display(dlg,offset,treerow->level,treerow->is_leaf);
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


	dlg->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(dlg->dialog), "dlg->dialog",
			    dlg->dialog);
	gtk_window_set_title(GTK_WINDOW(dlg->dialog),
			     get_module_description(dlg->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(dlg->dialog), 525, 306);

	vbox_dialog = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_dialog);
	gtk_container_add(GTK_CONTAINER(dlg->dialog), vbox_dialog);

	frame_gbs = gtk_frame_new(NULL);
	gtk_widget_show(frame_gbs);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), frame_gbs, TRUE, TRUE,
			   0);

	hpaned = e_hpaned_new();
	gtk_widget_show(hpaned);
	gtk_container_add(GTK_CONTAINER(frame_gbs), hpaned);
	e_paned_set_position(E_PANED(hpaned), 190);

	scrolledwindow_ctree = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_ctree);
	e_paned_pack1(E_PANED(hpaned), scrolledwindow_ctree, FALSE,
		      TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_ctree),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	dlg->ctree = gtk_ctree_new(3, 0);
	gtk_widget_show(dlg->ctree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_ctree),
			  dlg->ctree);
	gtk_clist_set_column_width(GTK_CLIST(dlg->ctree), 0, 600);
	gtk_clist_set_column_width(GTK_CLIST(dlg->ctree), 1, 40);
	gtk_clist_set_column_width(GTK_CLIST(dlg->ctree), 2, 40);
	gtk_clist_column_titles_hide(GTK_CLIST(dlg->ctree));

	label241 = gtk_label_new("");
	gtk_widget_show(label241);
	gtk_clist_set_column_widget(GTK_CLIST(dlg->ctree), 0, label241);

	label242 = gtk_label_new("");
	gtk_widget_show(label242);
	gtk_clist_set_column_widget(GTK_CLIST(dlg->ctree), 1, label242);

	label243 = gtk_label_new("");
	gtk_widget_show(label243);
	gtk_clist_set_column_widget(GTK_CLIST(dlg->ctree), 2, label243);


#ifdef USE_GTKEMBEDMOZ
	if (!dlg->is_rtol) {

		scrolledwindow_html = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_show(scrolledwindow_html);
		e_paned_pack2(E_PANED(hpaned), scrolledwindow_html, TRUE, TRUE);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
					       (scrolledwindow_html),
					       GTK_POLICY_NEVER,
					       GTK_POLICY_AUTOMATIC);
		dlg->html = gtk_html_new();
		gtk_widget_show(dlg->html);
		gtk_container_add(GTK_CONTAINER(scrolledwindow_html),
				  dlg->html);
		gtk_html_load_empty(GTK_HTML(dlg->html));
		gtk_signal_connect(GTK_OBJECT(dlg->html), "on_url",
				   G_CALLBACK(dialog_url),
				   (GBS_DATA *) dlg);
		gtk_signal_connect(GTK_OBJECT(dlg->html), "link_clicked",
				   G_CALLBACK(link_clicked),
				   (GBS_DATA *) dlg);
		gtk_signal_connect(GTK_OBJECT(dlg->html),
				   "button_press_event",
				   G_CALLBACK(button_press),
				   (GBS_DATA *) dlg);
	}
	else {
		dlg->html = gtk_moz_embed_new();
		gtk_widget_show(dlg->html);
		e_paned_pack2(E_PANED(hpaned), dlg->html, TRUE, TRUE);
		gtk_widget_realize(dlg->html);	
	}
#else	   
	scrolledwindow_html = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_html);
	e_paned_pack2(E_PANED(hpaned), scrolledwindow_html, TRUE, TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_html),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	dlg->html = gtk_html_new();
	gtk_widget_show(dlg->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_html),
			  dlg->html);
	gtk_html_load_empty(GTK_HTML(dlg->html));
	gtk_signal_connect(GTK_OBJECT(dlg->html), "on_url",
			   G_CALLBACK(dialog_url),
			   (GBS_DATA *) dlg);
	gtk_signal_connect(GTK_OBJECT(dlg->html), "link_clicked",
			   G_CALLBACK(link_clicked),
			   (GBS_DATA *) dlg);
	gtk_signal_connect(GTK_OBJECT(dlg->html),
			   "button_press_event",
			   G_CALLBACK(button_press),
			   (GBS_DATA *) dlg);
			   
#endif	

	dlg->statusbar = gtk_statusbar_new();
	gtk_widget_show(dlg->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), dlg->statusbar, FALSE,
			   FALSE, 0);
			   
	gtk_signal_connect(GTK_OBJECT(dlg->dialog), "destroy",
			   G_CALLBACK(dialog_destroy),
			   (GBS_DATA *) dlg);
	gtk_signal_connect(GTK_OBJECT(dlg->ctree), "tree_select_row",
			   G_CALLBACK(ctree_select_row),
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

	dlg = g_new(GBS_DATA, 1);
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
	add_book_to_ctree(dlg->ctree, dlg->mod_name);
	gtk_ctree_select(GTK_CTREE(dlg->ctree), rootnode);
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
		g_warning("oops!!!!!");
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
