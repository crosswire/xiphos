/***************************************************************************
                                    sw_bookmarks.cpp
                             -------------------
    begin                : Thu July 05 2001
    copyright            : (C) 2001 by tbiggs
    email                : tbiggs@users.sf.net
 ***************************************************************************/
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

#include "gs_gnomesword.h"
#include "sw_bookmarks.h"
#include "gs_bookmarks.h"

GtkStyle *style;
extern gchar *swbmDir;
extern SETTINGS *settings;
extern BM_TREE *p_bmtree;
extern BM_TREE bmtree;
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;

void AddSection(GtkCTree *ctree, SWConfig *config, const gchar *section, GtkCTreeNode *parent)
{
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	gboolean is_leaf;
	int i=0; 
	GtkCTreeNode *node;
	
	node = NULL;
	if((sit = config->Sections.find(section)) != config->Sections.end()){
		for(eit = (*sit).second.begin(); eit != (*sit).second.end(); eit++){
			char *token, *text[2];
			token=strtok((char *)(*eit).second.c_str(),",");
			text[0] = token;
			while(token != NULL){
				text[1] = token;
				token=strtok(NULL,",");
			}
			if(!strcmp(text[1],"GROUP")){
				is_leaf = false;
				node = gtk_ctree_insert_node(ctree,parent,node,text, 3, pixmap1,mask1,pixmap2, mask2, is_leaf, FALSE);
				gtk_ctree_node_set_row_data_full(ctree,node, style,(GtkDestroyNotify) gtk_style_unref);
			}else{
				is_leaf = true;
				node = gtk_ctree_insert_node(ctree,parent,node,text, 3, pixmap3,mask3,NULL, NULL, is_leaf, FALSE);
				gtk_ctree_node_set_row_data_full(ctree,node, style,(GtkDestroyNotify) gtk_style_unref);
			}
			//node = gtk_ctree_insert_node(ctree,parent,node,text, 3, pixmap1,mask1,pixmap2, mask2, is_leaf, FALSE);			
			AddSection(ctree,config,(*eit).first.c_str(),node);
		}
	}
}

/******************************************************************************
 * load bookmarks - using sword SWConfig
  most of this code is form sword-1.5.2 bibleCS bookmarkfrm.cpp
 ******************************************************************************/
void loadbookmarks(GtkWidget *ctree_widget)
{	
	SWConfig *bookmarkInfo; 
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	GtkCTreeNode *node;
	char conffile[256];
	DIR *dir;
	struct dirent *ent;
	GtkCTree *ctree;
	
	ctree = GTK_CTREE(ctree_widget);
	gtk_clist_freeze(GTK_CLIST(ctree));
	gtk_clist_clear(GTK_CLIST(ctree));
	
	sprintf(conffile,"%spersonal.conf", swbmDir);	
	bookmarkInfo = new SWConfig(conffile);	
	 
	if((sit = bookmarkInfo->Sections.find("ROOT")) != bookmarkInfo->Sections.end()){
		if((eit = (*sit).second.begin()) != (*sit).second.end()){
			char *token, *text[2];
			token=strtok((char *)(*eit).second.c_str(),",");
			text[0] = token;
			while(token != NULL){
				text[1] = token;
				token=strtok(NULL,",");
			}
			node = gtk_ctree_insert_node(ctree,NULL,NULL,text, 3, pixmap1,mask1,pixmap2, mask2, FALSE, FALSE);
			style = gtk_style_new();
			style->base[GTK_STATE_NORMAL].red = 0;
						style->base[GTK_STATE_NORMAL].green = 45000;
						style->base[GTK_STATE_NORMAL].blue = 55000;
						gtk_ctree_node_set_row_data_full(ctree,node, style,(GtkDestroyNotify) gtk_style_unref);
			AddSection(ctree, bookmarkInfo,(*eit).first.c_str(),node);
		}
	}  
	delete bookmarkInfo;
	
	if(dir = opendir(swbmDir)){
		rewinddir(dir);
		while ((ent = readdir(dir))) {
			if ((strcmp(ent->d_name, "personal.conf")) && (strcmp(ent->d_name, "."))&& (strcmp(ent->d_name, ".."))) {
				sprintf(conffile,"%s%s",swbmDir,ent->d_name);
				bookmarkInfo = new SWConfig(conffile);
				if ((sit = bookmarkInfo->Sections.find("ROOT")) != bookmarkInfo->Sections.end()) {
					if ((eit = (*sit).second.begin()) != (*sit).second.end()) {	// Currently supports only ONE topsection per file because on save, each topsection designates which file to rewrite
						char *token, *text[2];
						token=strtok((char *)(*eit).second.c_str(),",");
						text[0] = token;
						while(token != NULL){
							text[1] = token;
							token=strtok(NULL,",");
						}
						node = gtk_ctree_insert_node(ctree,NULL,NULL,text, 3, pixmap1,mask1,pixmap2, mask2, FALSE, FALSE);
						gtk_ctree_node_set_row_data_full(ctree,node, style,(GtkDestroyNotify) gtk_style_unref);
						
						AddSection(ctree, bookmarkInfo, (*eit).first.c_str(), node);
					}
				}
				delete bookmarkInfo;
			}
		}
		closedir(dir);
	}
	gtk_clist_thaw(GTK_CLIST(ctree));
	delete style;
}

/******************************************************************************
 * save gnomesword configuration - using sword SWConfig
 ******************************************************************************/
void savebookmarks(GtkWidget *ctree_widget)
{
	gchar *text, *tmpbuf;
	GtkCTreeNode *node;
	guint16     level;
	gboolean    *is_leaf;
	gboolean     *expanded;	
	GtkCTree *ctree;
	
	text= " ";
	tmpbuf=" ";	
	ctree = GTK_CTREE(ctree_widget);
	gtk_ctree_expand_recursive (ctree, NULL);
	
	for(int i=0; i< GTK_CLIST(ctree)->rows; i++){
		node = gtk_ctree_node_nth(ctree, i);		
		/*gtk_ctree_get_node_info(ctree,
						  node,
						  &text,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  is_leaf,
						  expanded);*/
		level = GTK_CTREE_ROW (node)->level;
		g_warning("text = %s level = %d", text ,level);
		//gtk_clist_get_text(GTK_CLIST(ctree), i, 1, &tmpbuf);
	}
}





