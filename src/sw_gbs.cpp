/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * sw_gbs.cpp (book support)
    * -------------------
    * Mon Jan 28 17:17:46 2002
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
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <dirent.h> 
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <treekeyidx.h>
#include <rawgenbook.h>

#include "gs_gnomesword.h"
#include "sw_sword.h"
#include "sw_display.h"
#include "sw_shortcutbar.h"
#include "sw_gbs.h"
#include "gs_html.h"
#include "gs_find_dlg.h"


/***  externs  ***/
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;
extern GList 
	*bookmods,
	*sbbookmods,
	*sbdictmods;
extern SETTINGS *settings;
extern gboolean in_url;

/***  globals  ***/
GtkCTreeNode *current_node;
SWMgr *swmgrBook;
SWDisplay *bookDisplay; /* to display gbs modules */	
list <SWDisplay *> displays;	// so we can delete each display we create
GList *gbs_data;

/***   ***/

static TreeKeyIdx* getTreeKey(SWModule *mod)
{
	TreeKeyIdx *treeKey =  (TreeKeyIdx*)mod->CreateKey();
	if(treeKey)
		return treeKey;
	else 
		return 0;
}

static
void addbooktoCTree(GtkWidget *ctree, gchar *bookName) {
	
	GtkCTreeNode * rootnode;
	gchar * buf[3];	
	
	buf[0] = bookName;
	buf[1] = bookName;
	buf[2] = "0";
	rootnode = gtk_ctree_insert_node(GTK_CTREE(ctree),
			NULL, NULL, buf, 3, pixmap1,
			mask1, pixmap2, mask2, FALSE,
			FALSE);
	
}
static
void setnodeinfoSW_GBS(SETTINGS *s, NODEDATA *data)
{
	gtk_ctree_set_node_info(GTK_CTREE(s->ctree_widget_books),
		data->parent,
		*data->buf,
		3,
		data->pixmap1,
		data->mask1,
		data->pixmap2, 
		data->mask2, 
		data->is_leaf,
		data->expanded);	
}
static
GtkCTreeNode *addnodeSW_GBS(SETTINGS *s, NODEDATA *data)
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
			data->is_leaf, 
			data->expanded);
	return retval;
}

static
void on_ctreeGBS_select_row(GtkCList * clist,
		    gint row,
		    gint column, 
		GdkEvent * event, 
		GBS_DATA *gbs)
{	
	gchar 
		*bookname, 
		*nodename, 
		*offset,
		*text[3];
	
	GtkCTreeNode 
		*treeNode;
	
	gboolean 
		is_leaf = false;
		
	ModMap::iterator it;
	
	treeNode = gtk_ctree_node_nth(GTK_CTREE(gbs->ctree), row);
	settings->ctree_widget_books = gbs->ctree;	
	settings->htmlBook = gbs->html;
	current_node = treeNode;
	
	nodename = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[0])->text;	
	bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[1])->text;	
	offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[2])->text;
				     
	if (swmgrBook) {
		it = swmgrBook->Modules.find(bookname);
		if (it != swmgrBook->Modules.end()) {
			TreeKeyIdx *treeKey =  getTreeKey((*it).second);
			TreeKeyIdx treenode = *treeKey;
			treenode.setOffset(strtoul(offset,NULL,0));
			sprintf(settings->BookWindowModule,"%s",(gchar*)(*it).second->Name());
			
			/** if not root node then display **/
			if(treenode.getOffset() > 0) {	
				(*it).second->SetKey(treenode);
				(*it).second->KeyText(); //snap to entry
				(*it).second->Display();
			}
				
			/** fill ctree node with children **/
			if((GTK_CTREE_ROW(treeNode)->children == NULL)&&(!GTK_CTREE_ROW(treeNode)->is_leaf)){
				load_book_tree(settings, treeNode, bookname,nodename,strtoul(offset,NULL,0));	
				gtk_ctree_expand(GTK_CTREE(gbs->ctree), treeNode);
			}
		}
	}
}

static
GBS_DATA *getgbs(GList *gbs)
{
	guint bookindex = 0;	
	ModMap::iterator it;
	
	for (it = swmgrBook->Modules.begin(); it != swmgrBook->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Generic Books")) {
			if (!strcmp((*it).second->Name(), settings->BookWindowModule)) {
				break;
			}
			++bookindex;
		}
	}
	return  (GBS_DATA*)g_list_nth_data(gbs, bookindex);
}

static
void on_notebookGBS_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, 
			GList *data_gbs)
{	
	GBS_DATA *g, *g_old;
	g_old = (GBS_DATA*)g_list_nth_data(data_gbs, settings->gbsLastPage);
	g = (GBS_DATA*)g_list_nth_data(data_gbs, page_num);
	//-- change tab label to current book name
	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(settings->workbook_lower),
		gtk_notebook_get_nth_page(GTK_NOTEBOOK(settings->workbook_lower),1), 
		g->bookName); 
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings->workbook_lower),
                gtk_notebook_get_nth_page(GTK_NOTEBOOK(settings->workbook_lower),1), 
		g->bookName);
	
	sprintf(settings->BookWindowModule,"%s",g->bookName);
	
	if(settings->finddialog) {
		gnome_dialog_close(g_old->find_dialog->dialog);
		searchGS_FIND_DLG(g, FALSE, settings->findText);
	}
	
	settings->gbsLastPage = page_num;
}

/****  popup menu call backs  ****/

static
void on_copy_activate(GtkMenuItem *menuitem,
			       GBS_DATA *gbs)
{
	copyGS_HTML(gbs->html);
}

static
void on_find_activate(GtkMenuItem *menuitem,
			       GBS_DATA *gbs)
{
	searchGS_FIND_DLG(gbs, FALSE, NULL);
}

static
void on_lookup_word_activate(GtkMenuItem * menuitem,
				gchar *modDescription)
{	
	GBS_DATA *g;
	gchar modName[16];
	gchar *key;
	
	g = getgbs(gbs_data);
	memset(modName, 0, 16);
	modNameFromDesc(modName, modDescription);
	g_warning("modName = %s",modName);
	key = lookupGS_HTML(g->html, true);
	if(key) {
		displaydictlexSBSW(modName, key, settings);
		g_free(key); 
	}
}

static
void on_lookup_selection_activate(GtkMenuItem * menuitem,
				gchar *modDescription)
{
	GBS_DATA *g;
	gchar modName[16];	
	gchar *key;
	
	g = getgbs(gbs_data);
	memset(modName, 0, 16);
	modNameFromDesc(modName, modDescription);
	key = lookupGS_HTML(g->html, false);
	if(key) {
		displaydictlexSBSW(modName, key, settings);
		g_free(key);
	}
}
static
void on_same_lookup_word_activate(GtkMenuItem * menuitem,
				GBS_DATA *g)
{	
	gchar *key = lookupGS_HTML(g->html, true);
	if(key) {
		displaydictlexSBSW(settings->DictWindowModule, key, settings);
		g_free(key);
	}
}

static
void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
				GBS_DATA *g)
{
	gchar *key = lookupGS_HTML(g->html, false);
	if(key) {
		displaydictlexSBSW(settings->DictWindowModule, key, settings);
		g_free(key);
	}	
}

static void
on_view_book_activate(GtkMenuItem * menuitem, 
				gpointer user_data)
{	
	gint page;
	
	page = GPOINTER_TO_INT(user_data);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebookGBS), page);
}

static gboolean
on_button_release_event (GtkWidget *widget,
				GdkEventButton  *event,
				GBS_DATA *g)
{
	gchar *key;
	switch (event->button) {
	  case 1:if(!in_url) {
			key = buttonpresslookupGS_HTML(g->html);
			if(key) {
				displaydictlexSBSW(settings->DictWindowModule, key, settings);
				g_free(key);
			}
			return true;
		}
		break;
	  case 2: 
		break;
	  case 3:
		break;
	}
	return false;
}

static
GtkWidget* create_pmGBS(GBS_DATA *gbs)
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
	GtkWidget *item6;
	GtkWidget *find;
	GList *tmp;
	gint i;
	
	tmp = NULL;
	
	pmGBS = gtk_menu_new ();
	gtk_object_set_data (GTK_OBJECT (pmGBS), "pmGBS", pmGBS);
	pmGBS_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (pmGBS));
	
	copy = gtk_menu_item_new_with_label (_("Copy"));
	gtk_widget_ref (copy);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "copy", copy,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (copy);
	gtk_container_add (GTK_CONTAINER (pmGBS), copy);

	find = gtk_menu_item_new_with_label (_("Find"));
	gtk_widget_ref (find);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "find", find,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (find);
	gtk_container_add (GTK_CONTAINER (pmGBS), find);
	
	separator = gtk_menu_item_new ();
	gtk_widget_ref (separator);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "separator", separator,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (separator);
	gtk_container_add (GTK_CONTAINER (pmGBS), separator);
	gtk_widget_set_sensitive (separator, FALSE);
	
	lookup_word = gtk_menu_item_new_with_label (_("Lookup Word"));
	gtk_widget_ref (lookup_word);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "lookup_word", lookup_word,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (lookup_word);
	gtk_container_add (GTK_CONTAINER (pmGBS), lookup_word);
	
	lookup_word_menu = gtk_menu_new ();
	gtk_widget_ref (lookup_word_menu);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "lookup_word_menu", lookup_word_menu,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lookup_word), lookup_word_menu);
	lookup_word_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (lookup_word_menu));
	
	usecurrent1 = gtk_menu_item_new_with_label ("Use Current Dictionary");
	gtk_widget_ref (usecurrent1);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "usecurrent1", usecurrent1,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (usecurrent1);
	gtk_container_add (GTK_CONTAINER (lookup_word_menu), usecurrent1);	
	
	
	lookup_selection = gtk_menu_item_new_with_label (_("Lookup Selection"));
	gtk_widget_ref (lookup_selection);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "lookup_selection", lookup_selection,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (lookup_selection);
	gtk_container_add (GTK_CONTAINER (pmGBS), lookup_selection);
	
	lookup_selection_menu = gtk_menu_new ();
	gtk_widget_ref (lookup_selection_menu);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "lookup_selection_menu", lookup_selection_menu,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lookup_selection), lookup_selection_menu);
	lookup_selection_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (lookup_selection_menu));
	
	usecurrent2 = gtk_menu_item_new_with_label ("Use Current Dictionary");
	gtk_widget_ref (usecurrent2);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "usecurrent2", usecurrent2,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (usecurrent2);
	gtk_container_add (GTK_CONTAINER (lookup_selection_menu), usecurrent2);
	
	
	separator = gtk_menu_item_new ();
	gtk_widget_ref (separator);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "separator", separator,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (separator);
	gtk_container_add (GTK_CONTAINER (pmGBS), separator);
	gtk_widget_set_sensitive (separator, FALSE);
	
	view_book = gtk_menu_item_new_with_label (_("View Book"));
	gtk_widget_ref (view_book);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "view_book", view_book,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (view_book);
	gtk_container_add (GTK_CONTAINER (pmGBS), view_book);
	
	view_book_menu = gtk_menu_new ();
	gtk_widget_ref (view_book_menu);
	gtk_object_set_data_full (GTK_OBJECT (pmGBS), "view_book_menu", view_book_menu,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (view_book), view_book_menu);
	view_book_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (view_book_menu));
	
	tmp = sbdictmods;
	while (tmp != NULL) {
		item3 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		item4 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_widget_ref(item4);
		gtk_object_set_data_full(GTK_OBJECT(pmGBS), "item3", item3,
					 (GtkDestroyNotify)gtk_widget_unref);
		gtk_object_set_data_full(GTK_OBJECT(pmGBS), "item4", item4,
					 (GtkDestroyNotify)gtk_widget_unref);
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
					 item3,
					 (GtkDestroyNotify)
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
			GTK_SIGNAL_FUNC(on_same_lookup_word_activate),
			gbs);
	gtk_signal_connect(GTK_OBJECT(usecurrent2), "activate",
			GTK_SIGNAL_FUNC(on_same_lookup_selection_activate),
			gbs);
	
	gtk_signal_connect (GTK_OBJECT (copy), "activate",
                      GTK_SIGNAL_FUNC (on_copy_activate),
                      gbs);
	gtk_signal_connect (GTK_OBJECT (find), "activate",
                      GTK_SIGNAL_FUNC (on_find_activate),
                      gbs);
  return pmGBS;
}

static
GtkWidget *createGBS_Pane(SWModule *mod, SETTINGS *s,gint count, GBS_DATA *p_gbs) {
	
	GtkWidget *hpanedGBS;
	GtkWidget *scrolledwindowCTREE_GBS;
	GtkWidget *label;
	GtkWidget *frameGBS;
	GtkWidget *scrolledwindowHTML_GBS;
	GtkWidget *popupmenu;
	
	hpanedGBS = gtk_hpaned_new ();
	gtk_widget_ref (hpanedGBS);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "hpanedGBS", hpanedGBS,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (hpanedGBS);
	gtk_container_add (GTK_CONTAINER (s->notebookGBS), hpanedGBS);
	gtk_paned_set_position (GTK_PANED (hpanedGBS), 239);
	
	scrolledwindowCTREE_GBS = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_ref (scrolledwindowCTREE_GBS);
	gtk_object_set_data_full (GTK_OBJECT (s->app), 
			"scrolledwindowCTREE_GBS", 
			scrolledwindowCTREE_GBS,
			(GtkDestroyNotify) gtk_widget_unref);			
	gtk_widget_show (scrolledwindowCTREE_GBS);
	gtk_paned_pack1 (GTK_PANED (hpanedGBS), scrolledwindowCTREE_GBS, FALSE, TRUE);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindowCTREE_GBS), 
			GTK_POLICY_AUTOMATIC, 
			GTK_POLICY_AUTOMATIC);
	
	p_gbs->ctree = gtk_ctree_new (3, 0);
	gtk_widget_ref (p_gbs->ctree);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "p_gbs->ctree", p_gbs->ctree,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (p_gbs->ctree);
	gtk_container_add (GTK_CONTAINER (scrolledwindowCTREE_GBS), p_gbs->ctree);
	gtk_container_set_border_width (GTK_CONTAINER (p_gbs->ctree), 1);
	gtk_clist_set_column_width (GTK_CLIST (p_gbs->ctree), 0, 680);
	gtk_clist_set_column_width (GTK_CLIST (p_gbs->ctree), 1, 1);
	gtk_clist_set_column_width (GTK_CLIST (p_gbs->ctree), 2, 1);
	gtk_clist_column_titles_hide (GTK_CLIST (p_gbs->ctree));
	
	label = gtk_label_new (_("label"));
	gtk_widget_ref (label);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "label", label,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label);
	gtk_clist_set_column_widget (GTK_CLIST (p_gbs->ctree), 0, label);
	
	label = gtk_label_new (_("label200"));
	gtk_widget_ref (label);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "label", label,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label);
	gtk_clist_set_column_widget (GTK_CLIST (p_gbs->ctree), 1, label);
	
	label = gtk_label_new (_("label"));
	gtk_widget_ref (label);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "label", label,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label);
	gtk_clist_set_column_widget (GTK_CLIST (p_gbs->ctree), 2, label);
	
	frameGBS = gtk_frame_new (NULL);
	gtk_widget_ref (frameGBS);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "frameGBS", frameGBS,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (frameGBS);
	gtk_paned_pack2 (GTK_PANED (hpanedGBS), frameGBS, TRUE, TRUE);
	
	scrolledwindowHTML_GBS = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_ref (scrolledwindowHTML_GBS);
	gtk_object_set_data_full (GTK_OBJECT (s->app), 
			"scrolledwindowHTML_GBS", 
			scrolledwindowHTML_GBS,
			(GtkDestroyNotify) gtk_widget_unref);			
	gtk_widget_show (scrolledwindowHTML_GBS);
	gtk_container_add (GTK_CONTAINER (frameGBS), scrolledwindowHTML_GBS);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindowHTML_GBS), 
			GTK_POLICY_AUTOMATIC, 
			GTK_POLICY_AUTOMATIC);
	
	p_gbs->html = gs_new_html_widget(s);	
	gtk_widget_ref(p_gbs->html);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "p_gbs->html", p_gbs->html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p_gbs->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowHTML_GBS),
			  p_gbs->html);
			  
	label = gtk_label_new ((gchar*)mod->Name());
	gtk_widget_ref (label);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "label", label,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (s->notebookGBS), 
		gtk_notebook_get_nth_page (GTK_NOTEBOOK (s->notebookGBS), count++), label);
	
	gtk_signal_connect(GTK_OBJECT(p_gbs->ctree), "select_row",
			   GTK_SIGNAL_FUNC(on_ctreeGBS_select_row), p_gbs);
	
	gtk_signal_connect (GTK_OBJECT (p_gbs->html), "button_release_event",
                      GTK_SIGNAL_FUNC (on_button_release_event), p_gbs);
		      
	SWDisplay *disp = new EntryDisp(p_gbs->html,s);
	mod->Disp(disp);
	displays.insert(displays.begin(), disp);
	addbooktoCTree(p_gbs->ctree, (gchar*)mod->Name());
	popupmenu = create_pmGBS(p_gbs);
	gnome_popup_menu_attach(popupmenu,p_gbs->html,NULL);
	p_gbs->find_dialog = NULL;
	return hpanedGBS;
}

void setupSW_GBS(SETTINGS *s)
{
	ModMap::iterator 
		it;	//-- iteratior
	gint count = 0;
	
	swmgrBook = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));  //-- create sword mgrs
	displays.clear();
	gbs_data = NULL;
	for (it = swmgrBook->Modules.begin(); it != swmgrBook->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Generic Books")) { 
			GBS_DATA *gbs = new GBS_DATA;
			gbs->bookName = (*it).second->Name();			
			gbs->bookDescription = (*it).second->Description();		
			gbs->searchstring = NULL;
			createGBS_Pane((*it).second, s, count, gbs) ;
			gbs_data = g_list_append(gbs_data, (GBS_DATA *) gbs);
			sprintf(s->BookWindowModule,"%s",gbs->bookName);
		}
	}
	gtk_signal_connect(GTK_OBJECT(s->notebookGBS), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebookGBS_switch_page),
			   gbs_data);
	
	settings->gbsLastPage = 0;
}

void shutdownSW_GBS(void)
{
	list <SWDisplay *>::iterator it;
	
	delete swmgrBook;
	for (it = displays.begin(); it != displays.end(); it++)
		delete *it;
	
	gbs_data = g_list_first(gbs_data);
	while(gbs_data != NULL) {
		GBS_DATA *g = (GBS_DATA *)gbs_data->data;
		if(g->find_dialog) g_free(g->find_dialog); //-- free any search dialogs created
		delete (GBS_DATA *) gbs_data->data;
		gbs_data = g_list_next(gbs_data);
	}
	g_list_free(gbs_data);
}

static void addNodeChildren(SETTINGS *s, 
			GtkCTreeNode *node, 
			gchar *bookname,
			TreeKeyIdx treeKey) 
{
	gchar 
		buf[256];
	GtkCTreeNode 
		*tmp_parent_node = node;
	NODEDATA 
		nodedata,
		*p_nodedata;
	
	p_nodedata = &nodedata;		
	p_nodedata->sibling = NULL;
	p_nodedata->buf[1] = bookname;
	
	if (treeKey.firstChild()) {
		sprintf(buf,"%lu",treeKey.getOffset());
		p_nodedata->parent = node;
		p_nodedata->buf[2] = buf;			
		p_nodedata->buf[0] = (gchar*)treeKey.getLocalName();
		if(treeKey.hasChildren()){
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
		node = addnodeSW_GBS(s, p_nodedata);
	}
	
	while(treeKey.nextSibling()){
		sprintf(buf,"%lu",treeKey.getOffset());
		p_nodedata->parent = tmp_parent_node;
		p_nodedata->buf[2] = buf;			
		p_nodedata->buf[0] = (gchar*)treeKey.getLocalName();
		if(treeKey.hasChildren()){
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
		node = addnodeSW_GBS(s, p_nodedata);
	}	
}

void load_book_tree(SETTINGS *s, 
			GtkCTreeNode *node, 
			gchar *bookName, 
			gchar *gtreekey,
			unsigned long offset)
{
	ModMap::iterator it;	//-- module iterator
		 
	it = swmgrBook->Modules.find(bookName);	//-- find module (modName)			
	TreeKeyIdx *treeKey =  getTreeKey((*it).second);	
	
	if (treeKey) {
		TreeKeyIdx root = *treeKey;	
		if(offset == 0)
			root.root();
		else 
			root.setOffset(offset);		
		addNodeChildren(s,node,bookName, root);	
	}
	else 
		cout << "oops\n";	
}

void displayinGBS(gchar *key)
{
	GBS_DATA *g;
	ModMap::iterator it;	
	
	g = getgbs(gbs_data);
	it = swmgrBook->Modules.find(g->bookName);
	if (it != swmgrBook->Modules.end()) {
		(*it).second->SetKey(key);
		(*it).second->KeyText(); //snap to entry
		(*it).second->Display();
	}
}


/******   end of file   ******/
