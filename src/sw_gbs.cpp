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

#include "sw_sword.h"
#include "sw_display.h"
#include "gs_editor.h"
#include "sw_gbs.h"
#include "gs_html.h"


/***  externs  ***/
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;
extern GList 
	*bookmods,
	*sbbookmods;
extern SETTINGS *settings;

/***  globals  ***/
GtkCTreeNode *current_node;
SWModule *curbookMod;
SWMgr *swmgrBook;
SWDisplay *bookDisplay; /* to display gbs modules */	
list <SWDisplay *> displays;	// so we can delete each display we create
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
void setnodeinfoGS_GBS(SETTINGS *s, NODEDATA *data)
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
GtkCTreeNode *addnodeGS_GBS(SETTINGS *s, NODEDATA *data)
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
GtkWidget *createGBS_Pane(SWModule *mod, SETTINGS *s,gint count) {
	
	GtkWidget *hpanedGBS;
	GtkWidget *scrolledwindowCTREE_GBS;
	GtkWidget *ctreeGBS;
	GtkWidget *label;
	GtkWidget *frameGBS;
	GtkWidget *scrolledwindowHTML_GBS;	
	GtkWidget *htmlGBS;
	
	
	hpanedGBS = gtk_hpaned_new ();
	gtk_widget_ref (hpanedGBS);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "hpanedGBS", hpanedGBS,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (hpanedGBS);
	gtk_container_add (GTK_CONTAINER (s->notebookGBS), hpanedGBS);
	gtk_paned_set_position (GTK_PANED (hpanedGBS), 189);
	
	scrolledwindowCTREE_GBS = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_ref (scrolledwindowCTREE_GBS);
	gtk_object_set_data_full (GTK_OBJECT (s->app), 
			"scrolledwindowCTREE_GBS", 
			scrolledwindowCTREE_GBS,
			(GtkDestroyNotify) gtk_widget_unref);			
	gtk_widget_show (scrolledwindowCTREE_GBS);
	gtk_paned_pack1 (GTK_PANED (hpanedGBS), scrolledwindowCTREE_GBS, FALSE, TRUE);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindowCTREE_GBS), 
			GTK_POLICY_NEVER, 
			GTK_POLICY_AUTOMATIC);
	
	ctreeGBS = gtk_ctree_new (3, 0);
	gtk_widget_ref (ctreeGBS);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "ctreeGBS", ctreeGBS,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (ctreeGBS);
	gtk_container_add (GTK_CONTAINER (scrolledwindowCTREE_GBS), ctreeGBS);
	gtk_container_set_border_width (GTK_CONTAINER (ctreeGBS), 1);
	gtk_clist_set_column_width (GTK_CLIST (ctreeGBS), 0, 280);
	gtk_clist_set_column_width (GTK_CLIST (ctreeGBS), 1, 80);
	gtk_clist_set_column_width (GTK_CLIST (ctreeGBS), 2, 20);
	gtk_clist_column_titles_hide (GTK_CLIST (ctreeGBS));
	
	label = gtk_label_new (_("label"));
	gtk_widget_ref (label);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "label", label,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label);
	gtk_clist_set_column_widget (GTK_CLIST (ctreeGBS), 0, label);
	
	label = gtk_label_new (_("label200"));
	gtk_widget_ref (label);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "label", label,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label);
	gtk_clist_set_column_widget (GTK_CLIST (ctreeGBS), 1, label);
	
	label = gtk_label_new (_("label"));
	gtk_widget_ref (label);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "label", label,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label);
	gtk_clist_set_column_widget (GTK_CLIST (ctreeGBS), 2, label);
	
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
	
	htmlGBS = gs_new_html_widget(s);	
	gtk_widget_ref(htmlGBS);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "htmlGBS", htmlGBS,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlGBS);
	gtk_container_add(GTK_CONTAINER(scrolledwindowHTML_GBS),
			  htmlGBS);
			  
	label = gtk_label_new ((gchar*)mod->Name());
	gtk_widget_ref (label);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "label", label,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (s->notebookGBS), 
		gtk_notebook_get_nth_page (GTK_NOTEBOOK (s->notebookGBS), count++), label);
	
	gtk_signal_connect(GTK_OBJECT(ctreeGBS), "select_row",
			   GTK_SIGNAL_FUNC(on_ctreeGBS_select_row), GTK_CTREE(ctreeGBS));
	
	SWDisplay *disp = new EntryDisp(htmlGBS);
	mod->Disp(disp);
	displays.insert(displays.begin(), disp);
	addbooktoCTree(ctreeGBS, (gchar*)mod->Name());
	return hpanedGBS;
}

void setupSW_GBS(SETTINGS *s)
{
	ModMap::iterator 
		it;	//-- iteratior
	gint count = 0;
	
	swmgrBook = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));  //-- create sword mgrs
	displays.clear();	
	bookmods = NULL;	
	sbbookmods = NULL;
	for (it = swmgrBook->Modules.begin(); it != swmgrBook->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Generic Book")) {  
			//curbookMod = (*it).second;
			bookmods = g_list_append(bookmods, (*it).second->Name());
			sbbookmods = g_list_append(sbbookmods, (*it).second->Description());
			createGBS_Pane((*it).second, s, count) ;
		}
	}
}

void shutdownSW_GBS(void)
{
	list <SWDisplay *>::iterator it;
	
	g_list_free(bookmods);
	g_list_free(sbbookmods);
	delete swmgrBook;
	for (it = displays.begin(); it != displays.end(); it++)
		delete *it;
}

void
on_ctreeGBS_select_row(GtkCList * clist,
		    gint row,
		    gint column, 
		GdkEvent * event, 
		GtkCTree *ctree)
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
	
	static gchar 
		*lastbook = "oldbook";	
	
	ModMap::iterator it;	
	
	treeNode = gtk_ctree_node_nth(ctree, row);
	settings->ctree_widget_books = GTK_WIDGET(ctree);	
	current_node = treeNode;
	
	nodename = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[0])->text;	
	g_warning("nodename = %s",nodename);
	
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
	
			//g_warning("Lang = %s" ,(const char *)curbookMod->getConfigEntry("Lang"));
			/** if not root node then display **/
			if(treenode.getOffset() > 0) {	
				(*it).second->SetKey(treenode);
				(*it).second->KeyText(); //snap to entry
				(*it).second->Display();
			}
				
			/** fill ctree node with children **/
			if((GTK_CTREE_ROW(treeNode)->children == NULL)&&(!GTK_CTREE_ROW(treeNode)->is_leaf)){
				load_book_tree(settings, treeNode, bookname,nodename,strtoul(offset,NULL,0));	
				gtk_ctree_expand(ctree, treeNode);
			}
		}
	}
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
		node = addnodeGS_GBS(s, p_nodedata);
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
		node = addnodeGS_GBS(s, p_nodedata);
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


