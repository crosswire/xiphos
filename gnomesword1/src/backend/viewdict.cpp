
/*
 * GnomeSword Bible Study Tool
 * viewdict.cpp - sword support for veiwtext dialog
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
#  include <config.h>
#endif

#include <gnome.h>

#include <swmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <markupfiltmgr.h>
#include "viewdict.h"
#include "sword.h"
#include "display.h"

static SWDisplay *display;	/* to display modules in view dict dialog */
static SWMgr *mgr;		/* sword mgr for view dict dialog */
static SWModule *mod;		/* module for view dict dialog */
static ModMap::iterator mdoule_iterator;


/******************************************************************************
 * Name
 *   backend_setup_viewdict
 *
 * Synopsis
 *   #include "viewdict.h"
 *
 *   void backend_setup_viewdict(GtkWidget * text)	
 *
 * Description
 *   setup the viewdict sword display
 *
 * Return value
 *   void
 */
void backend_setup_viewdict(GtkWidget * text, SETTINGS * s)
{
	ModMap::iterator it;	//-- iteratior                     
	SectionMap::iterator sit;	//-- iteratior

	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	mod = NULL;
	display = new GtkHTMLEntryDisp(text, s);
	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
		if (!strcmp
		    ((*it).second->Type(), "Lexicons / Dictionaries")) {
			mod = (*it).second;
			mod->Disp(display);
		}
	}
}

/******************************************************************************
 * Name
 *   backend_shutdown_viewdict
 *
 * Synopsis
 *   #include "viewdict.h"
 *
 *   GList *backend_shutdown_viewdict(GtkWidget * text)	
 *
 * Description
 *   close down view dict/lex 
 * 
 * Return value
 *   GList *
 */
void backend_shutdown_viewdict(void)	
{
	delete mgr;
	if (display)
		delete display;
}

/******************************************************************************
 * Name
 *   backend_load_module_viewdict
 *
 * Synopsis
 *   #include "viewdict.h"
 *
 *   	void backend_load_module_viewdict(char *module_name)
 *
 * Description
 *   
 * load a dictionary module into the view dictionary
 * Return value
 *   void
 */
void backend_load_module_viewdict(char *module_name)
{
	ModMap::iterator it;

	it = mgr->Modules.find(module_name);	//-- find module we want to use
	if (it != mgr->Modules.end()) {
		mod = (*it).second;	//-- set mod to new choice
		mod->SetKey("");
		mod->Display();	//-- display new dict          
	}
}

/******************************************************************************
 * Name
 *   backend_goto_key_viewdict
 *
 * Synopsis
 *   #include "viewdict.h"
 *
 *   void backend_goto_key_viewdict(gchar *newkey)	
 *
 * Description
 *    find new key for view dictionary
 * 
 * Return value
 *   void
 */
void backend_goto_key_viewdict(char * new_key)
{
	mod->SetKey(new_key);	//-- set key to our text
	mod->Display();
}

/******************************************************************************
 * Name
 *   backend_search_text_changed_viewdict
 *
 * Synopsis
 *   #include "viewdict.h"
 *
 *   void backend_search_text_changed_viewdict(gchar* new_key)	
 *
 * Description
 *    find new key from the dict lookup entry
 * 
 * Return value
 *   void
 */
void backend_search_text_changed_viewdict(char * new_key)
{
	if (mod) {		//-- if we have a dict module 
		if (strcmp(new_key, "")) {	//-- if text is not null               
			mod->SetKey(new_key);	//-- set key to our text
			mod->Display();	//-- show what we found 
		}
	}
}

/******************************************************************************
 * Name
 *   backend_get_first_key_viewdict
 *
 * Synopsis
 *   #include "viewdict.h"
 *
 *   char * backend_get_first_key_viewdict(void)	
 *
 * Description
 *    returns the first key in the module
 * 
 * Return value
 *   char *
 */
char *backend_get_first_key_viewdict(void)
{
	char *retval = NULL;

	if (!stricmp(mod->Name(), "WebstersDict"))
		mod->SetKey("A");
	else
		(*mod) = TOP;

	if (!mod->Error()) {
		//- retval must be freed by calling function
		retval = g_strdup((const char *) mod->KeyText());
	}
	return retval;
}

/******************************************************************************
 * Name
 *   backend_get_next_key_viewdict
 *
 * Synopsis
 *   #include "viewdict.h"
 *
 *   char * backend_get_next_key_viewdict(void)	
 *
 * Description
 *    returns the next key in the module
 * 
 * Return value
 *   char *
 */
char *backend_get_next_key_viewdict(void)
{
	char *retval = NULL;

	(*mod)++;
	if (!mod->Error()) {
		//- retval must be freed by calling function
		retval = g_strdup((const char *) mod->KeyText());
	}
	return retval;
}

/******************************************************************************
 * Name
 *   backend_get_first_module_viewdict
 *
 * Synopsis
 *   #include "viewdict.h"
 *
 *   char * backend_get_first_module_viewdict(void)	
 *
 * Description
 *    returns the first module in the modmap
 * 
 * Return value
 *   char *
 */
char *backend_get_first_module_viewdict(void)
{
	char *retval = NULL;

	for (mdoule_iterator = mgr->Modules.begin();
	     mdoule_iterator != mgr->Modules.end(); mdoule_iterator++) {
		if (!strcmp
		    ((*mdoule_iterator).second->Type(),
		     "Lexicons / Dictionaries")) {
			//- retval must be freed by calling function
			retval =
			    g_strdup((const char *) (*mdoule_iterator).
				     second->Name());
			return retval;
		}
	}
	return retval;
}

/******************************************************************************
 * Name
 *   backend_get_next_module_viewdict
 *
 * Synopsis
 *   #include "viewdict.h"
 *
 *   char * backend_get_next_module_viewdict(void)	
 *
 * Description
 *    returns the first module in the modmap
 * 
 * Return value
 *   char *
 */
char *backend_get_next_module_viewdict(void)
{
	char *retval = NULL;

	++mdoule_iterator;

	for (; mdoule_iterator != mgr->Modules.end(); mdoule_iterator++) {
		if (!strcmp
		    ((*mdoule_iterator).second->Type(),
		     "Lexicons / Dictionaries")) {
			//- retval must be freed by calling function
			retval =
			    g_strdup((const char *) (*mdoule_iterator).
				     second->Name());
			return retval;
		}
	}
	return retval;
}
