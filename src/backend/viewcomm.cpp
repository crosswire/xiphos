/*
 * GnomeSword Bible Study Tool
 * viewcomm.cpp - sword support for veiwcomm dialog
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
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <markupfiltmgr.h>

#include "backend/viewcomm.h"
#include "backend/sword.h"
#include "backend/display.h"
#include "main/settings.h"

static SWDisplay *display;	/* to display modules in view comm dialog */
static SWMgr *mgr;		/* sword mgr for view comm dialog */
static SWModule *mod;		/* module for view comm dialog */
static ModMap::iterator mdoule_iterator;
static VerseKey verse_key;

/******************************************************************************
 * Name
 *   backend_setup_viewcomm
 *
 * Synopsis
 *   #include "viewcom.h"
 *   
 *   void backend_setup_viewcomm(GtkWidget *text)	
 *
 * Description
 *    set up the sword stuff for the view commentary dialog
 * 
 * Return value
 *   void
 */
void backend_setup_viewcomm(GtkWidget * text)
{
	ModMap::iterator it;
	SectionMap::iterator sit;

	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	mod = NULL;
	display = new GtkHTMLEntryDisp(text);

	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Commentaries")) {
			mod = (*it).second;
			mod->Disp(display);
		}
	}
}

/******************************************************************************
 * Name
 *   backend_shutdown_viewcomm
 *
 * Synopsis
 *   #include "viewcom.h"
 *   void backend_shutdown_viewcomm(void) 
 *   	
 *
 * Description
 *    close down view comm dialog
 * 
 * Return value
 *   void
 */
void backend_shutdown_viewcomm(void)
{
	delete mgr;
	if (display)
		delete display;
}

/******************************************************************************
 * Name
 *   backend_load_module_viewcomm
 *
 * Synopsis
 *   #include "viewcom.h"
 *
 *   void backend_load_module_viewcomm(char *modName) 	
 *
 * Description
 *    load a commentary module into the view commentary dialog
 * 
 * Return value
 *   void
 */
void backend_load_module_viewcomm(char *modName)
{
	ModMap::iterator it;

	it = mgr->Modules.find(modName);	//-- find module we want to use
	if (it != mgr->Modules.end()) {
		mod = (*it).second;	//-- set mod to new choice
		mod->SetKey("");
		mod->Display();	//-- display new dict
	}
}

/******************************************************************************
 * Name
 *   backend_goto_verse_viewcomm
 *
 * Synopsis
 *   #include "viewcom.h"
 *
 *    void backend_goto_verse_viewcomm(char *newkey)	
 *
 * Description
 *    find new verse for view commentary dialog
 * 
 * Return value
 *   void
 */
void backend_goto_verse_viewcomm(char *newkey)
{
	verse_key.AutoNormalize(0);
	verse_key.Persist(1);	// when set to a module, make the module hold on to this actual key and not a copy
	verse_key = newkey;	// set to our new verse key
	mod->SetKey(verse_key);	//-- set key to our text
	mod->Display();
	verse_key.AutoNormalize(1);
}

/******************************************************************************
 * Name
 *   backend_nav_module_viewcomm
 *
 * Synopsis
 *   #include "viewcom.h"
 *
 *   void backend_nav_module_viewcomm(int direction) 	
 *
 * Description
 *    navigate the current commentary module
 * 
 * Return value
 *   void
 */
void backend_nav_module_viewcomm(int direction)
{
	switch (direction) {
	case 0:
		(*mod)--;
		break;
	case 1:
		(*mod)++;
		break;
	}
	mod->Error();		//-- clear any errors
	mod->Display();
}

/******************************************************************************
 * Name
 *   backend_get_first_module_viewcomm
 *
 * Synopsis
 *   #include "viewcom.h"
 *
 *   char * backend_get_first_module_viewcomm(void)	
 *
 * Description
 *    returns the first module in the modmap
 * 
 * Return value
 *   char *
 */
char *backend_get_first_module_viewcomm(void)
{
	char *retval = NULL;

	for (mdoule_iterator = mgr->Modules.begin();
	     mdoule_iterator != mgr->Modules.end(); mdoule_iterator++) {
		if (!strcmp
		    ((*mdoule_iterator).second->Type(),
		     "Commentaries")) {
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
 *   backend_get_next_module_viewcomm
 *
 * Synopsis
 *   #include "viewcomm.h"
 *
 *   char * backend_get_next_module_viewcomm(void)	
 *
 * Description
 *    returns the first module in the modmap
 * 
 * Return value
 *   char *
 */
char *backend_get_next_module_viewcomm(void)
{
	char *retval = NULL;

	++mdoule_iterator;

	for (; mdoule_iterator != mgr->Modules.end(); mdoule_iterator++) {
		if (!strcmp
		    ((*mdoule_iterator).second->Type(),
		     "Commentaries")) {
			//- retval must be freed by calling function
			retval =
			    g_strdup((const char *) (*mdoule_iterator).
				     second->Name());
			return retval;
		}
	}
	return retval;
}
