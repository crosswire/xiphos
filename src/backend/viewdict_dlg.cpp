/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * sw_viewdict_dlg.cpp
    * -------------------
    * Sat Aug 11 2001
    * copyright (C) 2001 by Terry Biggs
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
#include <markupfiltmgr.h>
#include "viewdict_dlg.h"
#include "sword.h"
#include "display.h"

static SWDisplay *display;	/* to display modules in view dict dialog */
static SWMgr *mgr;	/* sword mgr for view dict dialog */
static SWModule *mod;   /* module for view dict dialog */

/***************************/
//-------------------------------------------------------------------------------------------
void
shutdownSDSWORD(void)  //-- close down show dict/lex dialog
{	
	delete mgr;	
	if(display)
		delete display;	
}

/****************************************************************************************
 *setupSDSWORD - set up the sword stuff for the view dictionary dialog
 *returns a list of dict modules
 ****************************************************************************************/
GList* setupSDSWORD(GtkWidget *text)
{
	GList *list;
	ModMap::iterator it; //-- iteratior			
	SectionMap::iterator sit; //-- iteratior
	
	extern SETTINGS *settings;
	
	mgr	= new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	mod     = NULL;
	display = new  GtkHTMLEntryDisp(text,settings);
	list = NULL;
	for(it = mgr->Modules.begin(); it != mgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
			mod = (*it).second;
			list = g_list_append(list,mod->Name());
			mod->Disp(display);
		}
	}
	return list;
}


/******************************************************************************
 * returns the description of the view commentary dialog module
 ******************************************************************************/
gchar* getSDmodDescriptionSWORD(void)
{
	return (char *) mod->Description();;
}

/******************************************************************************
 *loadSDmodSWORD - load a dictionary module into the view dictionary dialog
 *
 ******************************************************************************/
void loadSDmodSWORD(gchar *modName)
{
	ModMap::iterator it;
               
        it = mgr->Modules.find(modName);  //-- find module we want to use
	if (it != mgr->Modules.end()){		
		mod = (*it).second;  //-- set mod to new choice
		mod->SetKey("");		
		mod->Display();	 //-- display new dict		
	}
}

/******************************************************************************
 *loadSDkeysSWORD - load a dictionary's keys into clist
 *returns a list of keys
 ******************************************************************************/
void loadSDkeysSWORD(GtkWidget *clist)
{
	gchar *listitem;
	//mod->SetKey("A");
	if(!stricmp(mod->Name(),"WebstersDict"))
		mod->SetKey("A");
	else
		(*mod)=TOP; 
	mod->Display();
	mod->Error();
	for (;!mod->Error();(*mod)++){
		listitem = g_strdup((const char *)mod->KeyText()); //-- key to listitem
		gtk_clist_append(GTK_CLIST(clist) , &listitem); //-- listitem to list
		if(listitem)
			g_free(listitem);
	}
}

/******************************************************************************
 *gotokeySWORD - find new key for view dictionary dialog
 *
 ******************************************************************************/
void gotokeySWORD(gchar *newkey)
{
        mod->SetKey(newkey); //-- set key to our text
        mod->Display();
}

/******************************************************************************
 * SDdictSearchTextChangedSWORD - dict lookup text changed
 *  newkey from the dict lookup entry
 ******************************************************************************/
void SDdictSearchTextChangedSWORD(char* newkey)
{		
	if (mod){ //-- if we have a dict module	
		if(strcmp(newkey,"")){  //-- if text is not null		
			mod->SetKey(newkey); //-- set key to our text
			mod->Display();	//-- show what we found	
		}
	}
}






