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

#include "gs_gnomesword.h"
#include "sword.h"
#include "display.h"
#include "gbs.h"
#include "gbs.h"
#include "settings.h"

typedef struct _backend_gbs BE_GBS;
struct _backend_gbs {
        SWModule *mod;
        TreeKeyIdx *treeKey;
        SWDisplay *bookDisplay;
        int num;
};

/***  globals  ***/
SWMgr *swmgrBook;
GList *be_gbs_list;

/***  start code  ***/
char *backend_get_book_key(int book_num)
{
	BE_GBS *g;
        g = (BE_GBS *) g_list_nth_data(be_gbs_list, book_num);
	if(g->mod->KeyText())
		return	g_strdup(g->mod->KeyText());
	else
		return NULL;
}
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

gboolean backend_treekey_next_sibling(gchar * bookname,
                                      unsigned long offset)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                treeKey->setOffset(offset);
                return treeKey->nextSibling();
        }
        return 0;
}

gboolean backend_treekeyHasChildrenGBS(gchar * bookname,
                                       unsigned long offset)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                treeKey->setOffset(offset);
                return treeKey->hasChildren();
        }
        return false;
}

char *backend_treekeyGetLocalNameGBS(gchar * bookname,
                                     unsigned long offset)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                treeKey->setOffset(offset);
                //-- returned value must be freed by calling function
                return g_strdup((char *) treeKey->getLocalName());
        }
        return NULL;
}

unsigned long backend_treekeyGetOffsetGBS(gchar * bookname)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                return treeKey->getOffset();
        }
        return 0;
}

gboolean backend_treekeyFirstChildGBS(char *bookname,
                                      unsigned long offset)
{
        TreeKeyIdx *treeKey = backend_get_tree_key(bookname);
        if (treeKey) {
                treeKey->setOffset(offset);
                return treeKey->firstChild();
        }
        return false;
}

void backend_newDisplayGBS(GtkWidget * html, char *bookname,
                           SETTINGS * s)
{
        GList *tmp = NULL;
        BE_GBS *g;

        tmp = g_list_first(be_gbs_list);
        while (tmp != NULL) {
                g = (BE_GBS *) tmp->data;
                if (!strcmp(g->mod->Name(), bookname)) {
                        g->bookDisplay = new GtkHTMLEntryDisp(html, s);
                        g->mod->Disp(g->bookDisplay);
                }
                tmp = g_list_next(tmp);
        }
        g_list_free(tmp);
}

void backend_display_book(gint booknum, gchar * key)
{
        ModMap::iterator it;
        BE_GBS *g;
        g = (BE_GBS *) g_list_nth_data(be_gbs_list, booknum);
             /** if not root node then display **/
	
		g->mod->SetKey(key);
		g->mod->KeyText();      //snap to entry
		g->mod->Display();
	      
}
gboolean backend_displayRowGBS(gint booknum, gchar * offset)
{
        ModMap::iterator it;
        BE_GBS *g;
        g = (BE_GBS *) g_list_nth_data(be_gbs_list, booknum);
        TreeKeyIdx *treeKey = backend_get_tree_key(g->mod->Name());
        if (treeKey) {
                TreeKeyIdx treenode = *treeKey;
                treenode.setOffset(strtoul(offset, NULL, 0));
                sprintf(settings.BookWindowModule, "%s",
                        (gchar *) g->mod->Name());
		sprintf(settings.book_key,"%s",treenode.getText());
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

void backend_setup_books(SETTINGS * s)
{
        ModMap::iterator it;    //-- iteratior
        gint count = 0;

        swmgrBook = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));       //-- create sword mgrs
        be_gbs_list = NULL;
        for (it = swmgrBook->Modules.begin();
             it != swmgrBook->Modules.end(); it++) {
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

void backend_shutdown_books(void)
{
        delete swmgrBook;

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

/*void displayinGBS(gchar *key) {
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
*/
/******   end of file   ******/
