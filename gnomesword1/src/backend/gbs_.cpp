/*
 * GnomeSword Bible Study Tool
 * gbs.c - sword lib support for general book modules
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

#include "main/settings.h"
#include "backend/sword.h"
#include "backend/display.h"
#include "backend/gbs_.h"

typedef struct _backend_gbs BE_GBS;
struct _backend_gbs {
        SWModule *mod;
        TreeKeyIdx *treeKey;
        SWDisplay *bookDisplay;
        int num;
};


/******************************************************************************
 * global to this file only
 */

static SWMgr *mgr;
static GList *be_gbs_list;


/******************************************************************************
 * Name
 *  backend_get_book_key
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   char *backend_get_book_key(int book_num)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 
 
char *backend_get_book_key(int book_num)
{
	BE_GBS *g;
        g = (BE_GBS *) g_list_nth_data(be_gbs_list, book_num);
	if(g->mod->KeyText())
		return	strdup(g->mod->KeyText());	
}

/******************************************************************************
 * Name
 *  backend_get_tree_key
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   TreeKeyIdx *backend_get_tree_key(char *bookname)	
 *
 * Description
 *    
 *
 * Return value
 *   TreeKeyIdx *
 */ 
 
static TreeKeyIdx *backend_get_tree_key(char *bookname)
{
        GList *tmp = NULL;
        BE_GBS *g;

        tmp = be_gbs_list;
        tmp = g_list_first(tmp);
        while (tmp != NULL) {
                g = (BE_GBS *) tmp->data;
                if (!strcmp(g->mod->Name(), bookname)) {
                        return g->treeKey;
                }
                tmp = g_list_next(tmp);
        }
        g_list_free(tmp);
        return 0;
}

/******************************************************************************
 * Name
 *  
 *backend_getModule
 * Synopsis
 *   #include "gbs_.h"
 *
 *   SWModule *backend_getModule(char *bookname)	
 *
 * Description
 *    
 *
 * Return value
 *   SWModule *
 */ 
 
static SWModule *backend_getModule(char *bookname)
{
        GList *tmp = NULL;
        BE_GBS *g;

        tmp = be_gbs_list;
        tmp = g_list_first(tmp);
        while (tmp != NULL) {
                g = (BE_GBS *) tmp->data;
                if (!strcmp(g->mod->Name(), bookname)) {
                        return g->mod;
                }
                tmp = g_list_next(tmp);
        }
        g_list_free(tmp);
        return 0;
}

/******************************************************************************
 * Name
 *  backend_treekey_next_sibling
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   int backend_treekey_next_sibling(gchar * bookname,
 *                                      unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int backend_treekey_next_sibling(gchar * bookname,
                                      unsigned long offset)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                treeKey->setOffset(offset);
                return treeKey->nextSibling();
        }
        return 0;
}

/******************************************************************************
 * Name
 *  backend_gbs_treekey_has_children
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   int backend_gbs_treekey_has_children(gchar * bookname,
  *                                      unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int backend_gbs_treekey_has_children(gchar * bookname,
                                       unsigned long offset)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                treeKey->setOffset(offset);
                return treeKey->hasChildren();
        }
        return false;
}

/******************************************************************************
 * Name
 *  backend_gbs_get_treekey_local_name
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   char *backend_gbs_get_treekey_local_name(gchar * bookname,
 *                                    unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 
 
char *backend_gbs_get_treekey_local_name(gchar * bookname,
                                     unsigned long offset)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                treeKey->setOffset(offset);
                //-- returned value must be freed by calling function
                return strdup((char *) treeKey->getLocalName());
                //return (const char *)treeKey->getLocalName();
        }
        return NULL;
}

/******************************************************************************
 * Name
 *  backend_gbs_get_treekey_offset
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   unsigned long backend_gbs_get_treekey_offset(gchar * bookname)	
 *
 * Description
 *    
 *
 * Return value
 *   unsigned long
 */ 
 
unsigned long backend_gbs_get_treekey_offset(gchar * bookname)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                return treeKey->getOffset();
        }
        return 0;
}

/******************************************************************************
 * Name
 *  backend_gbs_treekey_first_child
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   int backend_gbs_treekey_first_child(char *bookname,
  *                                     unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int backend_gbs_treekey_first_child(char *bookname,
                                      unsigned long offset)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                treeKey->setOffset(offset);
                return treeKey->firstChild();
        }
        return false;
}

/******************************************************************************
 * Name
 *  backend_new_gbs_display
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   void backend_new_gbs_display(GtkWidget * html, char *bookname,
 *						SETTINGS * s)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void backend_new_gbs_display(GtkWidget * html, char *bookname)
{
        GList *tmp = NULL;
        BE_GBS *g;

        tmp = g_list_first(be_gbs_list);
        while (tmp != NULL) {
                g = (BE_GBS *) tmp->data;
                if (!strcmp(g->mod->Name(), bookname)) {
                        g->bookDisplay = new GtkHTMLEntryDisp(html);
                        g->mod->Disp(g->bookDisplay);
                }
                tmp = g_list_next(tmp);
        }
        g_list_free(tmp);
}

/******************************************************************************
 * Name
 *  backend_display_gbs
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   void backend_display_gbs(gint booknum, gchar * key)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void backend_display_gbs(gint booknum, gchar * key)
{
        BE_GBS *g;
        g = (BE_GBS *) g_list_nth_data(be_gbs_list, booknum);
	g->mod->SetKey(key);
	g->mod->KeyText();      //snap to entry
	g->mod->Display();
	      
}

/******************************************************************************
 * Name
 *  backend_display_row_gbs
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   int backend_display_row_gbs(gint booknum, gchar * offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int backend_display_row_gbs(gint booknum, gchar * offset)
{
        BE_GBS *g = (BE_GBS *) g_list_nth_data(be_gbs_list, booknum);
        TreeKeyIdx *treeKey = backend_get_tree_key(g->mod->Name());
        if (treeKey) {
                TreeKeyIdx treenode = *treeKey;
                treenode.setOffset(strtoul(offset, NULL, 0));
                strcpy(settings.BookWindowModule,
				(gchar*)g->mod->Name());
		strcpy(settings.book_key,treenode.getText());
                /** if not root node then display **/
                if (treenode.getOffset() > 0) {
                        g->mod->SetKey(treenode);
                        g->mod->KeyText();      //snap to entry
                        g->mod->Display();
                }
                return true;
        }
        return false;
}

/******************************************************************************
 * Name
 *  backend_setup_books
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   void backend_setup_books(SETTINGS * s)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void backend_setup_books(SETTINGS * s)
{
        ModMap::iterator it;    //-- iteratior
        gint count = 0;

        mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));       //-- create sword mgrs
        be_gbs_list = NULL;
        for (it = mgr->Modules.begin();
             it != mgr->Modules.end(); it++) {
                if (!strcmp((*it).second->Type(), "Generic Books")) {
                        BE_GBS *be_gbs = new BE_GBS;
                        be_gbs->mod = (*it).second;
                        be_gbs->treeKey =
                            (TreeKeyIdx *) (*it).second->CreateKey();
                        be_gbs->num = count;
                        be_gbs_list =
                            g_list_append(be_gbs_list,
                                          (BE_GBS *) be_gbs);
                        ++count;
                }
        }
}

/******************************************************************************
 * Name
 *  backend_shutdown_books
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   void backend_shutdown_books(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void backend_shutdown_books(void)
{
        delete mgr;

        /***  free backend stuff  ***/
        be_gbs_list = g_list_first(be_gbs_list);
        while (be_gbs_list != NULL) {
                BE_GBS *g = (BE_GBS *) be_gbs_list->data;
                if (g->bookDisplay)
                        delete g->bookDisplay;  //-- delete any swdisplays created
                if (g->treeKey)
                        delete g->treeKey;      //-- delete any treekeys created     
                delete(BE_GBS *) be_gbs_list->data;
                be_gbs_list = g_list_next(be_gbs_list);
        }
        g_list_free(be_gbs_list);
}

/******   end of file   ******/
