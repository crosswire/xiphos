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
#include "gs_html_editor.h"
#include "sw_gbs.h"

//extern SETTINGS *settings;
extern GSHTMLEditorControlData *gbsecd;
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;

extern SWMgr *mainMgr;
extern SWModule *curbookMod;



static TreeKeyIdx* getTreeKey(SWModule *mod)
{
	TreeKeyIdx *treeKey =  (TreeKeyIdx*)mod->CreateKey();
	if(treeKey)
		return treeKey;
	else 
		return 0;
}

void
on_ctreeBooks_select_row(GtkCList * clist,
		    	gint row,
		    	gint column, 
			GdkEvent * event, 
			gpointer user_data)
{
	SETTINGS *s;
	gchar *bookname, *nodename, *offset;
	gboolean is_leaf = false;
	GtkCTreeNode *treeNode;
	static gchar *lastbook = "oldbook";
	
	s = (SETTINGS*)user_data;
	
	treeNode = gtk_ctree_node_nth(GTK_CTREE(s->ctree_widget_books), row);

	nodename =
		GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[0])->text;
	
	bookname =
		GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[1])->text;
	offset =
		GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[2])->text;
				     
	/** if we have change to a different book **/			     
	if(strcmp(lastbook,bookname)){	
		ModMap::iterator it;	//-- module iterator		     
		it = mainMgr->Modules.find(bookname);	//-- find module (modName)
		curbookMod = (*it).second;
		lastbook = bookname;
		gint context_id2 = gtk_statusbar_get_context_id(GTK_STATUSBAR(gbsecd->statusbar),
					 "GnomeSWORD");
		gtk_statusbar_pop(GTK_STATUSBAR(gbsecd->statusbar), context_id2);
		sprintf(gbsecd->filename,"%s",(gchar*)curbookMod->Name());
		gtk_statusbar_push(GTK_STATUSBAR(gbsecd->statusbar), context_id2,
			   gbsecd->filename);
	}
	
	TreeKeyIdx *treeKey =  getTreeKey(curbookMod);
	TreeKeyIdx treenode = *treeKey;
	treenode.setOffset(strtoul(offset,NULL,0));
	
	/** if not root node display **/
	if(treenode.getOffset() > 0) {	
		curbookMod->SetKey(treenode);
		curbookMod->KeyText(); //snap to entry
		curbookMod->Display();
	}
	
	/** fill ctree node with children **/
	if((GTK_CTREE_ROW(treeNode)->children == NULL)&&(!GTK_CTREE_ROW(treeNode)->is_leaf)){
		load_book_tree(s->ctree_widget_books, treeNode, bookname,nodename,strtoul(offset,NULL,0));	
		gtk_ctree_expand(GTK_CTREE(s->ctree_widget_books), treeNode);
	}	
}


static void addNodeChildren(GtkWidget *ctree, 
					GtkCTreeNode *node, 
					gchar *bookname,
					TreeKeyIdx treeKey) 
{
	gchar *text[3], buf[256];
	GtkCTreeNode *parent_node;
	GtkCTreeNode *tmp_parent_node = node;
	
	text[1] = bookname;
	text[2] = "0";
	if (treeKey.firstChild()) {
		sprintf(buf,"%lu",treeKey.getOffset());
		text[2] = buf;
		text[0] = (gchar*)treeKey.getLocalName();
		if(treeKey.hasChildren()){
			node = gtk_ctree_insert_node(GTK_CTREE(ctree),
								node, 	//-- the parent node to be.
								NULL,	//-- the sibling node to be.
								text, 
								3, 
								pixmap1,
								mask1,
								pixmap2, 
								mask2, 
								FALSE, 
								FALSE);			
		} else {
			node = gtk_ctree_insert_node(GTK_CTREE(ctree),
								node, 	//-- the parent node to be.
								NULL,	//-- the sibling node to be.
								text, 
								3, 
								pixmap3,
								mask3,
								NULL, 
								NULL, 
								TRUE, 
								FALSE);
		}
	}
	
	while(treeKey.nextSibling()){
		text[0] = (gchar*)treeKey.getLocalName();
		sprintf(buf,"%lu",treeKey.getOffset());
		text[2] = buf;
		if(treeKey.hasChildren())
			node = gtk_ctree_insert_node(GTK_CTREE(ctree),
								tmp_parent_node,
								NULL, /** must be null to keep correct tree order **/
								text, 
								3, 
								pixmap1,
								mask1,
								pixmap2, 
								mask2, 
								FALSE, 
								FALSE);
		else
			node = gtk_ctree_insert_node(GTK_CTREE(ctree),
								tmp_parent_node,
								NULL, /** must be null to keep correct tree order **/
								text, 
								3, 
								pixmap3,
								mask3,
								NULL, 
								NULL, 
								true, 
								FALSE);
	}

}

void load_book_tree(GtkWidget *ctree, 
				GtkCTreeNode *node, 
				gchar *bookName, 
				gchar *gtreekey,
				unsigned long offset)
{
	ModMap::iterator it;	//-- module iterator
		 
	it = mainMgr->Modules.find(bookName);	//-- find module (modName)
	curbookMod = (*it).second;
			
	TreeKeyIdx *treeKey =  getTreeKey(curbookMod);	
	
	if (treeKey) {
		TreeKeyIdx root = *treeKey;	
		//if(!strcmp(gtreekey, "root")) 
		if(offset == 0)
			root.root();
		else 
			root.setOffset(offset);		
		addNodeChildren(ctree,node,bookName, root);	
	}
	else 
		cout << "oops\n";
	
	
}
