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

#include "sw_gnomesword.h"
#include "sw_display.h"
#include "gs_editor.h"
#include "sw_gbs.h"
#include "gs_gbs.h"


extern GSHTMLEditorControlData *gbsecd;
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;
extern GList 
	*bookmods,
	*sbbookmods;
extern GtkCTreeNode *current_node;	

SWModule *curbookMod;
SWMgr *swmgrBook;
SWDisplay *bookDisplay; /* to display gbs modules */	

static bool changeBookMod(GSHTMLEditorControlData *ecd, gchar *newbook)
{
	ModMap::iterator it;	//-- module iterator		     
	it = swmgrBook->Modules.find(newbook);	//-- find module (modName)
	
	if (it != swmgrBook->Modules.end ()) {
		curbookMod = (*it).second;		
		gint context_id2 = gtk_statusbar_get_context_id(GTK_STATUSBAR(ecd->statusbar),
					 "GnomeSWORD");
		gtk_statusbar_pop(GTK_STATUSBAR(ecd->statusbar), context_id2);
		sprintf(ecd->filename,"%s",(gchar*)curbookMod->Name());
		gtk_statusbar_push(GTK_STATUSBAR(ecd->statusbar), context_id2,
			   ecd->filename);
		return true;
	}
	return false;
}

static TreeKeyIdx* getTreeKey(SWModule *mod)
{
	TreeKeyIdx *treeKey =  (TreeKeyIdx*)mod->CreateKey();
	if(treeKey)
		return treeKey;
	else 
		return 0;
}

static void appendChild(TreeKeyIdx *treeKey, gchar *text) 
{
	treeKey->appendChild();
	treeKey->setLocalName(text);
	treeKey->save();
}

static void appendSibling(TreeKeyIdx *treeKey, gchar *text) {
	if (treeKey->getOffset()) {
		treeKey->append();
		treeKey->setLocalName(text);
		treeKey->save();
	}
	else	cout << "Can't add sibling to root node\n";
}

void addchildSW_GBS(SETTINGS *s, gchar *name)
{
	gchar *bookname, *nodename, *offset;
	static gchar *lastbook = "oldbook";
	gchar tmpbuf[256];	
	NODEDATA nodedata,
		*p_nodedata;
	
	name = g_strchomp(name); //-- remove trailing spaces
	p_nodedata = &nodedata;	
	
	nodename = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(current_node)->row.
				     cell[0])->text;	
	bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(current_node)->row.
				     cell[1])->text;	
	offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(current_node)->row.
				     cell[2])->text;
	
	if(strcmp(lastbook,bookname)){	
		if(changeBookMod(gbsecd, bookname))
			lastbook = bookname;
		else return;
	}	
	
	TreeKeyIdx *treeKey =  getTreeKey(curbookMod);
	treeKey->setOffset(strtoul(offset,NULL,0));
	
	p_nodedata->parent = current_node;
	p_nodedata->sibling = NULL;	
	p_nodedata->buf[0] = nodename;
	p_nodedata->buf[1] = bookname;
	p_nodedata->buf[2] = offset;
	p_nodedata->pixmap1 = pixmap1;
	p_nodedata->mask1 = mask1;
	p_nodedata->pixmap2 = pixmap2;
	p_nodedata->mask2 = mask2;
	p_nodedata->is_leaf = FALSE;
	p_nodedata->expanded = TRUE;
	
	setnodeinfoGS_GBS(s, p_nodedata);
	appendChild(treeKey, name);
	
	treeKey->firstChild(); 
	sprintf(tmpbuf,"%lu",treeKey->getOffset());
	
	p_nodedata->buf[0] = name;	
	p_nodedata->buf[2] = tmpbuf;
	p_nodedata->pixmap1 = pixmap3;
	p_nodedata->mask1 = mask3;
	p_nodedata->pixmap2 = NULL;
	p_nodedata->mask2 = NULL;
	p_nodedata->is_leaf = TRUE;
	p_nodedata->expanded = FALSE;
	
	current_node = addnodeGS_GBS(s, p_nodedata);
}

void addSiblingSW_GBS(SETTINGS *s, gchar *name)
{
	gchar *bookname, *offset;
	static gchar *lastbook = "oldbook";
	gchar tmpbuf[256];
	NODEDATA nodedata,
		*p_nodedata;
	
	name = g_strchomp(name); //-- remove trailing spaces
	p_nodedata = &nodedata;		
	
	p_nodedata->parent = GTK_CTREE_ROW(current_node)->parent;
	bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(current_node)->row.
				     cell[1])->text;	
	offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(current_node)->row.
				     cell[2])->text;
				     
	if(strcmp(lastbook,bookname)){
		if(changeBookMod(gbsecd, bookname))
			lastbook = bookname;
		else return;
	}	
	
	TreeKeyIdx *treeKey =  getTreeKey(curbookMod);
	treeKey->setOffset(strtoul(offset,NULL,0));
	appendSibling(treeKey, name);
	treeKey->nextSibling();
	sprintf(tmpbuf,"%lu",treeKey->getOffset());
	
	p_nodedata->sibling = NULL; /** must be null to keep correct tree order **/	
	p_nodedata->buf[0] = name;
	p_nodedata->buf[1] = bookname;	
	p_nodedata->buf[2] = tmpbuf;
	p_nodedata->pixmap1 = pixmap3;
	p_nodedata->mask1 = mask3;
	p_nodedata->pixmap2 = NULL;
	p_nodedata->mask2 = NULL;
	p_nodedata->is_leaf = TRUE;
	p_nodedata->expanded = FALSE;
	
	current_node = addnodeGS_GBS(s, p_nodedata);	
}

void setupSW_GBS(SETTINGS *s)
{
	swmgrBook = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));  //-- create sword mgrs
	curbookMod = NULL;
	bookDisplay = 0;
	bookDisplay = new EntryDisp(s->htmlBook);
	loadBookListSW_GBS(s);
}

void shutdownSW_GBS(void)
{
	g_list_free(bookmods);
	g_list_free(sbbookmods);
	delete swmgrBook;
	if (bookDisplay)
		delete bookDisplay;
}

void loadBookListSW_GBS(SETTINGS *s)
{	
	ModMap::iterator 
		it;	//-- iteratior
	
	g_list_free(bookmods);
	g_list_free(sbbookmods);
	bookmods = NULL;	
	sbbookmods = NULL;
	
	for (it = swmgrBook->Modules.begin(); it != swmgrBook->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Generic Book")) {  
			curbookMod = (*it).second;
			bookmods = g_list_append(bookmods, curbookMod->Name());
			sbbookmods = g_list_append(sbbookmods, curbookMod->Description());
			curbookMod->Disp(bookDisplay);
		}
	}	
}

//-------------------------------------------------------------------------------------------
void savebookSW_GBS(gchar *buf)	//-- save personal comments
{
	if(buf)
		*curbookMod << (const char *) buf;	//-- save note!
}


static void createModuleConf(gchar *bookName, gchar *fileName)
{
	extern gchar *homedir;
	gchar buf[255], pathbuf[255];

	sprintf(buf, "%s/.sword/mods.d/%s.conf", homedir, fileName);
	SWConfig module_conf(buf);
	
	sprintf(pathbuf,"./modules/genbook/rawbook/%s/%s",fileName,fileName);
	module_conf[bookName]["DataPath"] = pathbuf;
	module_conf[bookName]["ModDrv"] = "RawGenBook";	
	module_conf[bookName]["BlockType"] = "BOOK";	
	module_conf[bookName]["Version"] = "1.0";	
	module_conf[bookName]["SourceType"] = "HTML";	
	module_conf[bookName]["Lang"] = "en";	
	module_conf[bookName]["Description"] = "A GBS book";	
	module_conf[bookName]["About"] = bookName;		
	
	module_conf.Save();	
}

static bool createBookDir(gchar *dir)
{
	if ((mkdir(dir, S_IRWXU)) == 0) {
		return true;
	}
 	else{
		cout << "could not create Book Dir\n";
		return false;
	}
}

static void setEntryText(RawGenBook *book, gchar *text) 
{
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);
	if (treeKey->getOffset()) {
		(*book) << text;
	}
	else	cout << "Can't add entry text to root node\n";
}

void addnewbookSW_GBS(SETTINGS *s, gchar *bookName, gchar *fileName)
{
	extern gchar 
		*homedir;
	gchar 
		*buf[3],
		tmpbuf[256];
	string 
		bookpath;
	TreeKeyIdx 
		*treeKey;
	
	fileName = g_strchomp(fileName); //-- remove trailing spaces
	fileName = g_strdelimit(fileName," /*?|",'_'); //-- remove chars from filename
	bookName = g_strchomp(bookName); //-- remove trailing spaces

	buf[0] = bookName;
	buf[1] = fileName;
	
	bookpath = "/.sword/modules/genbook/rawbook/";
	bookpath = homedir + bookpath;
	bookpath = bookpath + fileName;
	
	if(createBookDir((gchar*)bookpath.c_str())){
		bookpath = bookpath + "/" + fileName;
		RawGenBook::createModule((gchar*)bookpath.c_str());
		createModuleConf(bookName,fileName);
		cout << bookpath << "\n";
	}
	
	RawGenBook *book = new RawGenBook((gchar*)bookpath.c_str());
	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	treeKey = (TreeKeyIdx *)(SWKey *)(*book);
	appendChild(treeKey, bookName);
	treeKey->firstChild();
	setEntryText(book, bookName);
	sprintf(tmpbuf,"%lu",treeKey->getOffset());
	buf[2] = tmpbuf;
	current_node = gtk_ctree_insert_node(GTK_CTREE(s->ctree_widget_books),
				current_node, NULL, buf, 3,
				pixmap1, mask1, pixmap2,
				mask2, FALSE, FALSE);
	delete treeKey;
	delete book;
}

/***  does nothing yet ****fixme****  ***/
gint deleteNodeSW_GBS(SETTINGS *s)
{
	gchar *bookname, *offset;
	static gchar *lastbook = "oldbook";
	
	bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(current_node)->row.
				     cell[1])->text;	
	offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(current_node)->row.
				     cell[2])->text;
				     
	if(strcmp(lastbook,bookname)){
		if(changeBookMod(gbsecd, bookname))
			lastbook = bookname;
		else return 0;
	}	
	
	TreeKeyIdx *treeKey =  getTreeKey(curbookMod);
	treeKey->setOffset(strtoul(offset,NULL,0));
	return 0;
}

void
on_ctreeBooks_select_row(GtkCList * clist,
		    	gint row,
		    	gint column, 
			GdkEvent * event, 
			SETTINGS *s)
{	
	gchar 
		*bookname, 
		*nodename, 
		*offset;
	GtkCTreeNode 
		*treeNode;
	gboolean 
		is_leaf = false;
	gint 
		iswritable = 0;
	static gchar 
		*lastbook = "oldbook";	
	
	treeNode = gtk_ctree_node_nth(GTK_CTREE(s->ctree_widget_books), row);
	current_node = treeNode;
	
	nodename = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[0])->text;	
	bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[1])->text;	
	offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(treeNode)->row.
				     cell[2])->text;
				     
	/** if we have changed to a different book **/			     
	if(strcmp(lastbook,bookname)){
		if(changeBookMod(gbsecd, bookname))
			lastbook = bookname;
		else 
			return;	
	}
	
	TreeKeyIdx *treeKey =  getTreeKey(curbookMod);
	TreeKeyIdx treenode = *treeKey;
	treenode.setOffset(strtoul(offset,NULL,0));
	
	if(!curbookMod->isWritable()) 
		iswritable = 0;
	else
		iswritable = 1;
		
	/** if not root node then display **/
	if(treenode.getOffset() > 0) {	
		curbookMod->SetKey(treenode);
		curbookMod->KeyText(); //snap to entry
		curbookMod->Display();
	}
	
	else {
		// let's display the module about information for the root node
		showmoduleinfoSWORD(curbookMod->Name(), TRUE);
		if(iswritable)
			iswritable = 2;
	}
	
	setitemssensitivityGS_GBS(gbsecd, iswritable);
	/** fill ctree node with children **/
	if((GTK_CTREE_ROW(treeNode)->children == NULL)&&(!GTK_CTREE_ROW(treeNode)->is_leaf)){
		load_book_tree(s, treeNode, bookname,nodename,strtoul(offset,NULL,0));	
		gtk_ctree_expand(GTK_CTREE(s->ctree_widget_books), treeNode);
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
	curbookMod = (*it).second;
			
	TreeKeyIdx *treeKey =  getTreeKey(curbookMod);	
	
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
