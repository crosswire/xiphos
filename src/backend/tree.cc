/*
 * GnomeSword Bible Study Tool
 * tree.cc - sword lib support for treekey
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

#include "backend/tree.hh"
#include "backend/sword.h"
#include "backend/sword_defs.h"

using namespace sword;



 
/******************************************************************************
 * Name
 *  backend_treekey_next_sibling
 *
 * Synopsis
 *   #include "backend/tree.hh"
 *
 *   int backend_treekey_next_sibling(unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int backend_get_parent(unsigned long offset)
{
        if (sw.treeKey) {
                sw.treeKey->setOffset(offset);
                if(sw.treeKey->parent()) {
			return true;
		}	
        }
        return 0;
}

 
/******************************************************************************
 * Name
 *  backend_treekey_next_sibling
 *
 * Synopsis
 *   #include "backend/tree.hh"
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
        if (sw.treeKey) {
                sw.treeKey->setOffset(offset);
                if(sw.treeKey->nextSibling()) {
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
 *   #include "backend/tree.hh"
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
        if (sw.treeKey) {
                sw.treeKey->setOffset(offset);
		return sw.treeKey->hasChildren();
        }
        return false;
}

/******************************************************************************
 * Name
 *  backend_gbs_get_treekey_local_name
 *
 * Synopsis
 *   #include "backend/tree.hh"
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
        if (sw.treeKey) {
                sw.treeKey->setOffset(offset);
                //-- returned value must be freed by calling function
                return strdup((char *) sw.treeKey->getLocalName());
        }
        return NULL;
}


/******************************************************************************
 * Name
 *  backend_gbs_get_treekey_offset
 *
 * Synopsis
 *   #include "backend/tree.hh"
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
        if (sw.treeKey) 
                return sw.treeKey->getOffset();
        return 0;
}


/******************************************************************************
 * Name
 *  backend_gbs_treekey_first_child
 *
 * Synopsis
 *   #include "backend/tree.hh"
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
        if (sw.treeKey) {
                sw.treeKey->setOffset(offset);
		return sw.treeKey->firstChild();
	}
        return false;
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "backend/tree.hh"
 *
 *    	
 *
 * Description
 *    
 *
 * Return value
 *   char * 
 */ 
 
char * backend_get_text_from_offset(char * module_name, unsigned long offset)
{        
	//SWModule *mod = sw.gbs_mgr->Modules[module_name];
	if (sw.treeKey) {
                TreeKeyIdx treenode = *sw.treeKey;
                treenode.setOffset(offset);
                /** if not root node then display **/
                if (treenode.getOffset() > 0) {
                        sw.gbs_mod->SetKey(treenode);
                        sw.gbs_mod->KeyText();      //snap to entry
                }
		return strdup(sw.gbs_mod->RenderText());
        }
        return NULL;
}

char * backend_get_key_from_offset(unsigned long offset)
{        
	//SWModule *mod = sw.gbs_mgr->Modules[module_name];
	if (sw.treeKey) {
                TreeKeyIdx treenode = *sw.treeKey;
                treenode.setOffset(offset);
                /** if not root node then display **/
                if (treenode.getOffset() > 0) {
                        sw.gbs_mod->SetKey(treenode);
                        sw.gbs_mod->KeyText();      //snap to entry
                }
		return strdup(sw.gbs_mod->KeyText());
        }
        return NULL;
}

/******************************************************************************
 * Name
 *  backend_set_treekey
 *
 * Synopsis
 *   #include "backend/tree.hh"
 *
 *   void backend_set_treekey(char * module_name, unsigned long offset)	
 *
 * Description
 *   sets treeKey to current module 
 *
 * Return value
 *   void
 */ 
 
void backend_set_treekey(char * module_name, unsigned long offset)
{
	//sw.gbs_mod = sw.display_mgr->Modules[module_name];
	if (sw.treeKey)
                delete sw.treeKey; 
	
	sw.treeKey = (TreeKeyIdx *) sw.gbs_mod->CreateKey();
       // TreeKeyIdx treenode = *sw.treeKey;
	//treenode.setOffset(offset); 
	sw.treeKey->setOffset(offset);
	
	sw.gbs_mod->SetKey(backend_get_key_from_offset(offset));
        //g_warning("backend_set_treekey = %s",sw.gbs_mod->KeyText());  
	//(const char *) *sw.gbs_mod;
}


/******************************************************************************
 * Name
 *  backend_setup_treekey
 *
 * Synopsis
 *   #include "backend/tree.hh"
 *
 *   void backend_setup_treekey(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void backend_setup_treekey(void)
{
      	sw.treeKey = NULL;
}

/******************************************************************************
 * Name
 *  
 *backend_shutdown_treekey
 * Synopsis
 *   #include "backend/tree.hh"
 *
 *   	void backend_shutdown_treekey(void)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void backend_shutdown_treekey(void)
{
     	if(sw.treeKey)
                delete sw.treeKey; 
}
