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

#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swmodule.h>
#include <versekey.h>
#include <string.h>
#include <treekeyidx.h>
#include <rawgenbook.h>

//#include "backend/sword.h"
#include "backend/gbs_.h"

using namespace sword;


/******************************************************************************
 * global to this file only
 */

static SWMgr *mgr;
static SWModule *mod;
static TreeKeyIdx *treeKey;



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
 
char *backend_get_book_key(char * book_name)
{
	mod = mgr->Modules[book_name];
	if(mod->KeyText())
		return	strdup(mod->KeyText());	
	return NULL;
}

/******************************************************************************
 * Name
 *  backend_treekey_next_sibling
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   int backend_treekey_next_sibling(unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int backend_treekey_next_sibling(unsigned long offset)
{
        if (treeKey) {
                treeKey->setOffset(offset);
                if(treeKey->nextSibling()) {
			return true;
		}	
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
 *   int backend_gbs_treekey_has_children(unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int backend_gbs_treekey_has_children(unsigned long offset)
{	
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
 *   char *backend_gbs_get_treekey_local_name(unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 
 
char *backend_gbs_get_treekey_local_name(unsigned long offset)
{	
        if (treeKey) {
                treeKey->setOffset(offset);
                //-- returned value must be freed by calling function
                return strdup((char *) treeKey->getLocalName());
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
 *   unsigned long backend_gbs_get_treekey_offset(void)	
 *
 * Description
 *    
 *
 * Return value
 *   unsigned long
 */ 
 
unsigned long backend_gbs_get_treekey_offset(void)
{
        if (treeKey) 
                return treeKey->getOffset();
        return 0;
}

/******************************************************************************
 * Name
 *  backend_gbs_treekey_first_child
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   int backend_gbs_treekey_first_child(unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int backend_gbs_treekey_first_child(unsigned long offset)
{
        if (treeKey) {
                treeKey->setOffset(offset);
		return treeKey->firstChild();
	}
        return false;
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
 
char *backend_display_gbs(char * book_name, char * key)
{
	mod = mgr->Modules[book_name];
	mod->SetKey(key);
	mod->KeyText();      //snap to entry
	return strdup(mod->RenderText());      
}

/******************************************************************************
 * Name
 *  backend_display_row_gbs
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *    char * backend_display_row_gbs(char * mod_name, gchar * offset)	
 *
 * Description
 *    
 *
 * Return value
 *   char * 
 */ 
 
char * backend_display_row_gbs(char * mod_name, char * offset)
{        
        mod = mgr->Modules[mod_name];
	if (treeKey) {
                TreeKeyIdx treenode = *treeKey;
                treenode.setOffset(strtoul(offset, NULL, 0));
                /** if not root node then display **/
                if (treenode.getOffset() > 0) {
                        mod->SetKey(treenode);
                        mod->KeyText();      //snap to entry
                }
		return strdup(mod->RenderText());
        }
        return NULL;
}

/******************************************************************************
 * Name
 *  backend_change_book
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   void backend_change_book(char * mod_name, unsigned long offset)	
 *
 * Description
 *   sets treeKey to current book 
 *
 * Return value
 *   void
 */ 
 
void backend_change_book(char * mod_name, unsigned long offset)
{
	if (treeKey)
                delete treeKey; 
	mod = mgr->Modules[mod_name];
	treeKey = (TreeKeyIdx *) mod->CreateKey();
	treeKey->setOffset(offset);
}

/******************************************************************************
 * Name
 *  backend_setup_books
 *
 * Synopsis
 *   #include "gbs_.h"
 *
 *   void backend_setup_books(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void backend_setup_books(void)
{
        mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));       //-- create sword mgrs
	treeKey = NULL;
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
	if(treeKey)
                delete treeKey; 
}

/******   end of file   ******/
