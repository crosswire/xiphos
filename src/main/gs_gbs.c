/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_gbs.c (generic book support)
     * -------------------
     * Tue Apr  2 09:35:20 2002
     * copyright (C) 2002 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

#include "gs_gnomesword.h"
#include "gs_gbs.h"
#include "sword.h"
#include "shortcutbar.h"
#include "gbs.h"
#include "gs_html.h"
#include "gs_find_dlg.h"


/***  externs  ***/
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;
extern GList * sbbookmods, *sbdictmods;
extern SETTINGS *settings;
extern gboolean in_url;

/***  globals  ***/
GList *gbs_list;
gboolean show_tabs_gbs;

/***  start code  ***/
static
void gui_add_node_children(SETTINGS * s,
			    GtkCTreeNode * node,
			    gchar * bookname, unsigned long offset)
{
    gchar buf[256], *tmpbuf;
    GtkCTreeNode * tmp_parent_node = node;
    NODEDATA nodedata, *p_nodedata;

    p_nodedata = &nodedata;
    p_nodedata->sibling = NULL;
    p_nodedata->buf[1] = bookname;

    if (backend_treekeyFirstChildGBS(bookname, offset)) {
	offset = backend_treekeyGetOffsetGBS(bookname);
	sprintf(buf, "%lu", offset);
	p_nodedata->parent = node;
	p_nodedata->buf[2] = buf;
	tmpbuf = backend_treekeyGetLocalNameGBS(bookname, offset);
	p_nodedata->buf[0] = tmpbuf;
	if (backend_treekeyHasChildrenGBS(bookname, offset)) {
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
	node = gui_addnodeGBS(s, p_nodedata);
	g_free(tmpbuf);
    }

    while (backend_treekey_next_sibling(bookname, offset)) {
	offset = backend_treekeyGetOffsetGBS(bookname);
	sprintf(buf, "%lu", offset);
	p_nodedata->parent = tmp_parent_node;
	p_nodedata->buf[2] = buf;
	tmpbuf = backend_treekeyGetLocalNameGBS(bookname, offset);
	p_nodedata->buf[0] = tmpbuf;
	if (backend_treekeyHasChildrenGBS(bookname, offset)) {
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
	node = gui_addnodeGBS(s, p_nodedata);
	g_free(tmpbuf);
    }
}


static
void gui_add_book_to_ctree(GtkWidget * ctree, gchar * bookName)
{

    GtkCTreeNode *rootnode;
    gchar *buf[3];

    buf[0] = bookName;
    buf[1] = bookName;
    buf[2] = "0";
    rootnode = gtk_ctree_insert_node(GTK_CTREE(ctree),
				     NULL, NULL, buf, 3, pixmap1,
				     mask1, pixmap2, mask2, FALSE, FALSE);

}

GtkCTreeNode *gui_addnodeGBS(SETTINGS * s, NODEDATA * data)
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


/*    */
static
void on_ctreeGBS_select_row(GtkCList * clist,
			    gint row,
			    gint column, GdkEvent * event, GBS_DATA * gbs)
{
    gchar * bookname, *nodename, *offset;

    GtkCTreeNode * treeNode;



    treeNode = gtk_ctree_node_nth(GTK_CTREE(gbs->ctree), row);
    settings->ctree_widget_books = gbs->ctree;
    settings->htmlBook = gbs->html;

    nodename = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				cell[0])->text;
    bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				cell[1])->text;
    offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.cell[2])->text;

    if (backend_displayRowGBS(gbs->booknum, offset)) {
	/** fill ctree node with children **/
	if ((GTK_CTREE_ROW(treeNode)->children == NULL)
	    && (!GTK_CTREE_ROW(treeNode)->is_leaf)) {
	    gui_add_node_children(settings, treeNode, bookname, strtoul(offset, NULL, 0));	//load_book_tree(settings, treeNode, bookname,nodename,strtoul(offset,NULL,0));      
	    gtk_ctree_expand(GTK_CTREE(gbs->ctree), treeNode);
	}
    }
}

static
GBS_DATA *getgbs(GList * gbs)
{
    GList *tmp;
    GBS_DATA *g = NULL;

    tmp = NULL;
    tmp = gbs;
    tmp = g_list_first(tmp);
    while (tmp != NULL) {
	g = (GBS_DATA *) tmp->data;
	if (!strcmp(g->bookName, settings->BookWindowModule)) {
	    break;
	}
	tmp = g_list_next(tmp);
    }
    g_list_free(tmp);
    return g;
}

static
void on_notebookGBS_switch_page(GtkNotebook * notebook,
				GtkNotebookPage * page,
				gint page_num, GList * data_gbs)
{
    GBS_DATA *g, *g_old;
    g_old = (GBS_DATA *) g_list_nth_data(data_gbs, settings->gbsLastPage);
    g = (GBS_DATA *) g_list_nth_data(data_gbs, page_num);
    //-- change tab label to current book name
    gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(settings->workbook_lower),
				    gtk_notebook_get_nth_page(GTK_NOTEBOOK
							      (settings->
							       workbook_lower),
							      1),
				    g->bookName);
    gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
				     (settings->workbook_lower),
				     gtk_notebook_get_nth_page(GTK_NOTEBOOK
							       (settings->
								workbook_lower),
							       1),
				     g->bookName);

    sprintf(settings->BookWindowModule, "%s", g->bookName);

    if (settings->finddialog) {
	gnome_dialog_close(g_old->find_dialog->dialog);
	searchGS_FIND_DLG(g, FALSE, settings->findText);
    }
    GTK_CHECK_MENU_ITEM(g->showtabs)->active = show_tabs_gbs;
    settings->gbsLastPage = page_num;
}

/****  popup menu call backs  ****/
static
void on_copy_activate(GtkMenuItem * menuitem, GBS_DATA * gbs)
{
    copyGS_HTML(gbs->html);
}

static
void on_find_activate(GtkMenuItem * menuitem, GBS_DATA * gbs)
{
    searchGS_FIND_DLG(gbs, FALSE, NULL);
}

static
void on_lookup_word_activate(GtkMenuItem * menuitem,
			     gchar * modDescription)
{
    GBS_DATA *g;
    gchar modName[16];
    gchar *key;

    g = getgbs(gbs_list);
    memset(modName, 0, 16);
    modNameFromDesc(modName, modDescription);
    key = lookupGS_HTML(g->html, TRUE);
    if (key) {
	displaydictlexSBSW(modName, key, settings);
	g_free(key);
    }
}

static
void on_lookup_selection_activate(GtkMenuItem * menuitem,
				  gchar * modDescription)
{
    GBS_DATA *g;
    gchar modName[16];
    gchar *key;

    g = getgbs(gbs_list);
    memset(modName, 0, 16);
    modNameFromDesc(modName, modDescription);
    key = lookupGS_HTML(g->html, FALSE);
    if (key) {
	displaydictlexSBSW(modName, key, settings);
	g_free(key);
    }
}
static
void on_same_lookup_word_activate(GtkMenuItem * menuitem, GBS_DATA * g)
{
    gchar *key = lookupGS_HTML(g->html, TRUE);
    if (key) {
	displaydictlexSBSW(settings->DictWindowModule, key, settings);
	g_free(key);
    }
}

static
void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
				       GBS_DATA * g)
{
    gchar *key = lookupGS_HTML(g->html, FALSE);
    if (key) {
	displaydictlexSBSW(settings->DictWindowModule, key, settings);
	g_free(key);
    }
}

static void
on_view_book_activate(GtkMenuItem * menuitem, gpointer user_data)
{
    gint page;

    page = GPOINTER_TO_INT(user_data);
    gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebookGBS), page);
}

static gboolean
on_button_release_event(GtkWidget * widget,
			GdkEventButton * event, GBS_DATA * g)
{
    gchar *key;
    switch (event->button) {
    case 1:
	if (!in_url) {
	    key = buttonpresslookupGS_HTML(g->html);
	    if (key) {
		displaydictlexSBSW(settings->DictWindowModule, key,
				   settings);
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


void on_showtabs_activate(GtkMenuItem * menuitem, SETTINGS * s)
{
    show_tabs_gbs = GTK_CHECK_MENU_ITEM(menuitem)->active;
    if (show_tabs_gbs)
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookGBS), TRUE);
    else
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookGBS), FALSE);
}

static
GtkWidget *create_pmGBS(GBS_DATA * gbs)
{
    GtkWidget *pmGBS;
    GtkAccelGroup *pmGBS_accels;
    GtkWidget *copy;
    GtkWidget *separator;
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
    //GtkWidget *showtabs;
    GList *tmp;
    gint i;

    tmp = NULL;

    pmGBS = gtk_menu_new();
    gtk_object_set_data(GTK_OBJECT(pmGBS), "pmGBS", pmGBS);
    pmGBS_accels = gtk_menu_ensure_uline_accel_group(GTK_MENU(pmGBS));

    copy = gtk_menu_item_new_with_label(_("Copy"));
    gtk_widget_ref(copy);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "copy", copy,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(copy);
    gtk_container_add(GTK_CONTAINER(pmGBS), copy);

    find = gtk_menu_item_new_with_label(_("Find"));
    gtk_widget_ref(find);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "find", find,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(find);
    gtk_container_add(GTK_CONTAINER(pmGBS), find);

    gbs->showtabs = gtk_check_menu_item_new_with_label(_("Show Tabs"));
    gtk_widget_ref(gbs->showtabs);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "gbs->showtabs",
			     gbs->showtabs,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(gbs->showtabs);
    gtk_container_add(GTK_CONTAINER(pmGBS), gbs->showtabs);

    separator = gtk_menu_item_new();
    gtk_widget_ref(separator);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "separator", separator,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(separator);
    gtk_container_add(GTK_CONTAINER(pmGBS), separator);
    gtk_widget_set_sensitive(separator, FALSE);

    lookup_word = gtk_menu_item_new_with_label(_("Lookup Word"));
    gtk_widget_ref(lookup_word);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "lookup_word", lookup_word,
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
	gtk_menu_ensure_uline_accel_group(GTK_MENU(lookup_word_menu));

    usecurrent1 = gtk_menu_item_new_with_label("Use Current Dictionary");
    gtk_widget_ref(usecurrent1);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "usecurrent1", usecurrent1,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(usecurrent1);
    gtk_container_add(GTK_CONTAINER(lookup_word_menu), usecurrent1);


    lookup_selection = gtk_menu_item_new_with_label(_("Lookup Selection"));
    gtk_widget_ref(lookup_selection);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "lookup_selection",
			     lookup_selection,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(lookup_selection);
    gtk_container_add(GTK_CONTAINER(pmGBS), lookup_selection);

    lookup_selection_menu = gtk_menu_new();
    gtk_widget_ref(lookup_selection_menu);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "lookup_selection_menu",
			     lookup_selection_menu,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(lookup_selection),
			      lookup_selection_menu);
    lookup_selection_menu_accels =
	gtk_menu_ensure_uline_accel_group(GTK_MENU(lookup_selection_menu));

    usecurrent2 = gtk_menu_item_new_with_label("Use Current Dictionary");
    gtk_widget_ref(usecurrent2);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "usecurrent2", usecurrent2,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(usecurrent2);
    gtk_container_add(GTK_CONTAINER(lookup_selection_menu), usecurrent2);


    separator = gtk_menu_item_new();
    gtk_widget_ref(separator);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "separator", separator,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(separator);
    gtk_container_add(GTK_CONTAINER(pmGBS), separator);
    gtk_widget_set_sensitive(separator, FALSE);

    view_book = gtk_menu_item_new_with_label(_("View Book"));
    gtk_widget_ref(view_book);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "view_book", view_book,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(view_book);
    gtk_container_add(GTK_CONTAINER(pmGBS), view_book);

    view_book_menu = gtk_menu_new();
    gtk_widget_ref(view_book_menu);
    gtk_object_set_data_full(GTK_OBJECT(pmGBS), "view_book_menu",
			     view_book_menu,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_book), view_book_menu);
    view_book_menu_accels =
	gtk_menu_ensure_uline_accel_group(GTK_MENU(view_book_menu));

    tmp = sbdictmods;
    while (tmp != NULL) {
	item3 = gtk_menu_item_new_with_label((gchar *) tmp->data);
	item4 = gtk_menu_item_new_with_label((gchar *) tmp->data);
	gtk_widget_ref(item3);
	gtk_widget_ref(item4);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "item3", item3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "item4", item4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(item3);
	gtk_widget_show(item4);
	gtk_signal_connect(GTK_OBJECT(item3), "activate",
			   GTK_SIGNAL_FUNC(on_lookup_word_activate),
			   (gchar *) tmp->data);
	gtk_signal_connect(GTK_OBJECT(item4), "activate",
			   GTK_SIGNAL_FUNC(on_lookup_selection_activate),
			   (gchar *) tmp->data);
	gtk_container_add(GTK_CONTAINER(lookup_word_menu), item3);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu), item4);
	tmp = g_list_next(tmp);
    }
    g_list_free(tmp);

    i = 0;
    tmp = sbbookmods;
    while (tmp != NULL) {
	item3 = gtk_menu_item_new_with_label((gchar *) tmp->data);
	gtk_widget_ref(item3);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "item3",
				 item3, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(item3);
	gtk_signal_connect(GTK_OBJECT(item3), "activate",
			   GTK_SIGNAL_FUNC
			   (on_view_book_activate), GINT_TO_POINTER(i));
	gtk_container_add(GTK_CONTAINER(view_book_menu), item3);
	++i;
	tmp = g_list_next(tmp);
    }
    g_list_free(tmp);
	/*** these two are for using the current dictionary for lookup ***/
    gtk_signal_connect(GTK_OBJECT(usecurrent1), "activate",
		       GTK_SIGNAL_FUNC(on_same_lookup_word_activate), gbs);
    gtk_signal_connect(GTK_OBJECT(usecurrent2), "activate",
		       GTK_SIGNAL_FUNC(on_same_lookup_selection_activate),
		       gbs);

    gtk_signal_connect(GTK_OBJECT(copy), "activate",
		       GTK_SIGNAL_FUNC(on_copy_activate), gbs);
    gtk_signal_connect(GTK_OBJECT(find), "activate",
		       GTK_SIGNAL_FUNC(on_find_activate), gbs);
    gtk_signal_connect(GTK_OBJECT(gbs->showtabs), "activate",
		       GTK_SIGNAL_FUNC(on_showtabs_activate), settings);
    return pmGBS;
}

GtkWidget *createGBS_Pane(gchar * modName, SETTINGS * s, gint count,
			  GBS_DATA * p_gbs)
{

    GtkWidget *hpanedGBS;
    GtkWidget *scrolledwindowCTREE_GBS;
    GtkWidget *label;
    GtkWidget *frameGBS;
    GtkWidget *scrolledwindowHTML_GBS;
    GtkWidget *popupmenu;

    hpanedGBS = gtk_hpaned_new();
    gtk_widget_ref(hpanedGBS);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "hpanedGBS", hpanedGBS,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(hpanedGBS);
    gtk_container_add(GTK_CONTAINER(s->notebookGBS), hpanedGBS);
    gtk_paned_set_position(GTK_PANED(hpanedGBS), 239);

    scrolledwindowCTREE_GBS = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_ref(scrolledwindowCTREE_GBS);
    gtk_object_set_data_full(GTK_OBJECT(s->app),
			     "scrolledwindowCTREE_GBS",
			     scrolledwindowCTREE_GBS,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(scrolledwindowCTREE_GBS);
    gtk_paned_pack1(GTK_PANED(hpanedGBS), scrolledwindowCTREE_GBS, FALSE,
		    TRUE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				   (scrolledwindowCTREE_GBS),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);

    p_gbs->ctree = gtk_ctree_new(3, 0);
    gtk_widget_ref(p_gbs->ctree);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "p_gbs->ctree",
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
    gtk_object_set_data_full(GTK_OBJECT(s->app), "label", label,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(label);
    gtk_clist_set_column_widget(GTK_CLIST(p_gbs->ctree), 0, label);

    label = gtk_label_new(_("label200"));
    gtk_widget_ref(label);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "label", label,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(label);
    gtk_clist_set_column_widget(GTK_CLIST(p_gbs->ctree), 1, label);

    label = gtk_label_new(_("label"));
    gtk_widget_ref(label);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "label", label,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(label);
    gtk_clist_set_column_widget(GTK_CLIST(p_gbs->ctree), 2, label);

    frameGBS = gtk_frame_new(NULL);
    gtk_widget_ref(frameGBS);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "frameGBS", frameGBS,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(frameGBS);
    gtk_paned_pack2(GTK_PANED(hpanedGBS), frameGBS, TRUE, TRUE);

    scrolledwindowHTML_GBS = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_ref(scrolledwindowHTML_GBS);
    gtk_object_set_data_full(GTK_OBJECT(s->app),
			     "scrolledwindowHTML_GBS",
			     scrolledwindowHTML_GBS,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(scrolledwindowHTML_GBS);
    gtk_container_add(GTK_CONTAINER(frameGBS), scrolledwindowHTML_GBS);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				   (scrolledwindowHTML_GBS),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);

    p_gbs->html = gs_new_html_widget(s);
    gtk_widget_ref(p_gbs->html);
    gtk_object_set_data_full(GTK_OBJECT(s->app),
			     "p_gbs->html", p_gbs->html,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(p_gbs->html);
    gtk_container_add(GTK_CONTAINER(scrolledwindowHTML_GBS), p_gbs->html);

    label = gtk_label_new(modName);
    gtk_widget_ref(label);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "label", label,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(label);
    gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->notebookGBS),
			       gtk_notebook_get_nth_page(GTK_NOTEBOOK
							 (s->notebookGBS),
							 count), label);
    gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->notebookGBS),
				     gtk_notebook_get_nth_page(GTK_NOTEBOOK
							       (s->
								notebookGBS),
							       count),
				     (gchar *) modName);

    gtk_signal_connect(GTK_OBJECT(p_gbs->ctree), "select_row",
		       GTK_SIGNAL_FUNC(on_ctreeGBS_select_row), p_gbs);

    gtk_signal_connect(GTK_OBJECT(p_gbs->html), "button_release_event",
		       GTK_SIGNAL_FUNC(on_button_release_event), p_gbs);

    gui_add_book_to_ctree(p_gbs->ctree, modName);
    popupmenu = create_pmGBS(p_gbs);
    gnome_popup_menu_attach(popupmenu, p_gbs->html, NULL);
    p_gbs->find_dialog = NULL;
    return hpanedGBS;
}

void gui_setupGBS(SETTINGS * s)
{
    gint count = 0;
    GList *tmp = NULL;
    GList *mods = NULL;
    gchar *bookname;
    GBS_DATA *gbs;
    show_tabs_gbs = FALSE;
    gbs_list = NULL;

    mods = backend_getModListOfTypeSWORD(BOOK_MODS);
    tmp = mods;
    tmp = g_list_first(tmp);
    while (tmp != NULL) {
	bookname = (gchar *) tmp->data;
	gbs = g_new(GBS_DATA, 1);
	gbs->bookName = bookname;
	gbs->bookDescription = backend_get_module_description(bookname);
	gbs->searchstring = NULL;
	gbs->booknum = count;
	createGBS_Pane(bookname, s, count, gbs);
	backend_newDisplayGBS(gbs->html, gbs->bookName, s);
	gbs_list = g_list_append(gbs_list, (GBS_DATA *) gbs);
	sprintf(s->BookWindowModule, "%s", gbs->bookName);
	++count;
	tmp = g_list_next(tmp);
    }

    gtk_signal_connect(GTK_OBJECT(s->notebookGBS), "switch_page",
		       GTK_SIGNAL_FUNC(on_notebookGBS_switch_page),
		       gbs_list);

    settings->gbsLastPage = 0;
    g_list_free(tmp);
    g_list_free(mods);
}

void gui_shutdownGBS(void)
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


/******   end of file   ******/
