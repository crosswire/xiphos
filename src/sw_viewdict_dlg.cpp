/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                               sw_viewdict_dlg.cpp
                             -------------------
 				Sat Aug 11 2001
    		      copyright (C) 2001 by Terry Biggs
  			      tbiggs@users.sf.net
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
#include "sw_viewdict_dlg.h"
#include "sw_gnomesword.h"
#include "gs_display.h"
#include "sw_utility.h"

SWDisplay *SDDisplay;	/* to display modules in view dict dialog */
SWMgr *SDMgr;	/* sword mgr for view dict dialog */
SWModule *SDMod;   /* module for view dict dialog */

/***************************/
//-------------------------------------------------------------------------------------------
void
shutdownSDSWORD(void)  //-- close down show dict/lex dialog
{	
	delete SDMgr;	
	if(SDDisplay)
		delete SDDisplay;	
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
	
	SDMgr	= new SWMgr();
	SDMod     = NULL;
	SDDisplay = new  GtkHTMLEntryDisp(text);
	//SDDisplay = new  GtkHTMLEntryDisp(text);
	list = NULL;
	for(it = SDMgr->Modules.begin(); it != SDMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
			SDMod = (*it).second;
			list = g_list_append(list,SDMod->Name());
			sit = SDMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters			
			ConfigEntMap &section = (*sit).second;
			addrenderfiltersSWORD(SDMod, section);		
			SDMod->Disp(SDDisplay);
		}
	}
	return list;
}


/******************************************************************************
 * returns the description of the view commentary dialog module
 ******************************************************************************/
gchar* getSDmodDescriptionSWORD(void)
{
	return (char *) SDMod->Description();;
}

/******************************************************************************
 *loadSDmodSWORD - load a dictionary module into the view dictionary dialog
 *
 ******************************************************************************/
void loadSDmodSWORD(GtkWidget *clist, gchar *modName)
{
	ModMap::iterator it;
        
        
        gtk_clist_clear( GTK_CLIST(clist)); //-- start with empty list	
        it = SDMgr->Modules.find(modName);  //-- find module we want to use
	if (it != SDMgr->Modules.end()){		
		SDMod = (*it).second;  //-- set curdictMod to new choice
		SDMod->SetKey("");		
		SDMod->Display();	 //-- display new dict		
	}
}

/******************************************************************************
 *loadSDkeysSWORD - load a dictionary keys into clist
 *returns a list of keys
 ******************************************************************************/
void loadSDkeysSWORD(GtkWidget *clist)
{
	gchar *listitem;
	//SDMod->SetKey("A");
	if(!stricmp(SDMod->Name(),"WebstersDict"))
		SDMod->SetKey("A");
	else
		(*SDMod)=TOP; 
	SDMod->Display();
	SDMod->Error();
	for (;!SDMod->Error();(*SDMod)++){
		listitem = g_strdup((const char *)SDMod->KeyText()); //-- key to listitem
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
        SDMod->SetKey(newkey); //-- set key to our text
        SDMod->Display();
}

/******************************************************************************
 * SDdictSearchTextChangedSWORD - dict lookup text changed
 *  newkey from the dict lookup entry
 ******************************************************************************/
void SDdictSearchTextChangedSWORD(char* newkey)
{		
	if (SDMod){ //-- if we have a dict module	
		if(strcmp(newkey,"")){  //-- if text is not null		
			SDMod->SetKey(newkey); //-- set key to our text
			SDMod->Display();	//-- show what we found	
		}
	}
}






