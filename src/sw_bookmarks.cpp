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
list <string> bmfiles;
GtkCTreeNode *personal_node;


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
			is_leaf = false;
			node = gtk_ctree_insert_node(ctree,parent,node,text, 3, pixmap1,mask1,pixmap2, mask2, is_leaf, FALSE);
			gtk_ctree_node_set_row_data_full(ctree,node, style,(GtkDestroyNotify) gtk_style_unref);
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
	char conffile[500];
	DIR *dir;
	struct dirent *ent;
	GtkCTree *ctree;
	
	ctree = GTK_CTREE(ctree_widget);
	gtk_clist_freeze(GTK_CLIST(ctree));
	gtk_clist_clear(GTK_CLIST(ctree));
	
	sprintf(conffile,"%sPersonal.conf", swbmDir);	
	g_warning(conffile);
	
	if (access(conffile, F_OK) == -1) return;
		
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
			*bmfiles.insert(bmfiles.begin(),conffile);
			personal_node = gtk_ctree_insert_node(ctree,NULL,NULL,text, 3, pixmap1,mask1,pixmap2, mask2, FALSE, FALSE);
			style = gtk_style_new();
			style->base[GTK_STATE_NORMAL].red = 0;
			style->base[GTK_STATE_NORMAL].green = 45000;
			style->base[GTK_STATE_NORMAL].blue = 55000;
			gtk_ctree_node_set_row_data_full(ctree,personal_node, style,(GtkDestroyNotify) gtk_style_unref);
			AddSection(ctree, bookmarkInfo,(*eit).first.c_str(),personal_node);
		}
	}  
	delete bookmarkInfo;
	
	if(dir = opendir(swbmDir)){
		rewinddir(dir);
		while ((ent = readdir(dir))) {
			if ((strcmp(ent->d_name, "Personal.conf")) && (strcmp(ent->d_name, "."))&& (strcmp(ent->d_name, ".."))) {
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
						*bmfiles.insert(bmfiles.begin(),conffile);
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
 * 
 ******************************************************************************/
void AddSectionToConf(SWConfig *config, string section, GtkCTreeNode *node, int index)
{
	GtkCTreeNode *work;
	ConfigEntMap sit;
	char buf[15], tmpbuf[500];
	static int i=0;
	
	if (!GTK_CTREE_ROW (node)->is_leaf){
		sit = config->Sections[section.c_str()];
		for (work = GTK_CTREE_ROW (node)->children; work;
	   				work = GTK_CTREE_ROW (work)->sibling){
			++i;			
			sprintf(buf, "branch%d", i);
			sprintf(tmpbuf,"%s,%s", GTK_CELL_PIXTEXT (GTK_CTREE_ROW (work)->row.cell[0])->text,
					GTK_CELL_PIXTEXT (GTK_CTREE_ROW (work)->row.cell[1])->text);	
			sit.erase(buf); sit.insert(ConfigEntMap::value_type (buf,tmpbuf));
			//g_warning("%s=%s",buf,tmpbuf); 
			AddSectionToConf(config, buf, work, i);
		}		
		config->Sections[section.c_str()] = sit;
	}
}


/******************************************************************************
 * save gnomesword bookmarks - using sword SWConfig
 ******************************************************************************/
void savebookmarks(GtkWidget *ctree_widget)
{
	GtkCTreeNode *node;
	GtkCTree *ctree;
	gchar persfile[256], 
			buf[80],
			tmpbuf[500],
			conffile[256];
	SWConfig *bmconf;
	ConfigEntMap emap;
	SectionMap::iterator sit;
	list <string>::iterator it;
	
	sprintf(persfile,"%spersonal.conf",swbmDir);
	
	/*** delete all bookmark files before saving in case a top level was deleter ***/	
	for( it = bmfiles.begin(); it != bmfiles.end(); it++){
		//g_warning((*it).c_str());
		if((!strcmp((*it).c_str(),persfile)) || (strcmp((*it).c_str(), persfile))) /* we used this code so we can add autosave options */
			unlink((*it).c_str());
	}
	ctree = GTK_CTREE(ctree_widget);
	/* collapse tree so we only iterate through the roots */
	gtk_ctree_collapse_recursive (ctree, NULL); 	
	/*******************************************************************/
	int j=0;
	for(int i=0; i< GTK_CLIST(ctree)->rows; i++){
		node = gtk_ctree_node_nth(ctree, i);
  		if (!node)
    			return;
		//g_warning("ROOT");	
		sprintf(conffile,"%s%s", swbmDir, GTK_CELL_PIXTEXT (GTK_CTREE_ROW (node)->row.cell[1])->text);
		 bmconf = new SWConfig(conffile);
		emap = bmconf->Sections["ROOT"];
		sprintf(buf, "branch%d", j++);
		sprintf(tmpbuf,"%s,%s", GTK_CELL_PIXTEXT (GTK_CTREE_ROW (node)->row.cell[0])->text,
					GTK_CELL_PIXTEXT (GTK_CTREE_ROW (node)->row.cell[1])->text);
		emap.erase(buf); emap.insert(ConfigEntMap::value_type(buf, tmpbuf));	
		AddSectionToConf(bmconf, buf, node, j);
		bmconf->Sections["ROOT"] = emap;
		bmconf->Save();
		delete bmconf;  		
	}
    /***************************************************************************/	
}

/******************************************************************************
 * no bookmark dir found on startup
 ******************************************************************************/
void 
createbookmarksBM(gchar *dir)
{
	gchar buf[255];

	sprintf(buf, "%s/Personal.conf", dir);
	SWConfig bmInfo(buf);
	
	bmInfo["ROOT"]["branch0"] = "Personal,Personal.conf";
	bmInfo["branch0"]["branch1"] = "[What must I do to be saved?],GROUP";
	bmInfo["branch0"]["branch5"]="[What is the Gospel?],GROUP";
	bmInfo["branch0"]["branch7"]="Romans 8:28,KJV";
	bmInfo["branch0"]["branch8"]="Rev 1:5,KJV";
	
	bmInfo["branch1"]["branch2"]="Romans 1:16,KJV";
	bmInfo["branch1"]["branch3"]="Eph 2:8,KJV";
	bmInfo["branch1"]["branch4"]="Acts  16:31,KJV";
	
	bmInfo["branch5"]["branch6"]="1 Cor 15:1,KJV";
	
	bmInfo.Save(); 	
}




