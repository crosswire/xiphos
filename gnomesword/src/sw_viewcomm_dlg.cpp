/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * sw_viewcomm_dlg.cpp
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

#include "sw_viewcomm_dlg.h"
#include "sw_gnomesword.h"
#include "sw_display.h"
#include "sw_utility.h"

SWDisplay *VCDisplay;	/* to display modules in view comm dialog */
SWMgr *VCMgr;	/* sword mgr for view comm dialog */
SWModule *VCMod;   /* module for view comm dialog */
VerseKey DefaultVCKey;
/******************************************************************************
 * returns the description of the view commentary dialog module
 ******************************************************************************/
gchar* getVCmodDescriptionSWORD(void)
{
	return (char *) VCMod->Description();;
}

/****************************************************************************************
 *setupCommSWORD - set up the sword stuff for the view commentary dialog
 *returns a list of commentary modules
 ****************************************************************************************/
GList* setupCommSWORD(GtkWidget *text)
{
	GList *list;
	ModMap::iterator it; //-- iteratior	
	SectionMap::iterator sit; //-- iteratior
	
	VCMgr	= new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	VCMod     = NULL;
	VCDisplay = new  GtkHTMLEntryDisp(text);
	//VCDisplay = new  GtkHTMLEntryDisp(text);
	list = NULL;
	for(it = VCMgr->Modules.begin(); it != VCMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Commentaries")){
			VCMod = (*it).second;
			list = g_list_append(list,VCMod->Name());
			VCMod->Disp(VCDisplay);
		}
	}
	return list;
}

//-------------------------------------------------------------------------------------------
void shutdownVCSWORD(void)  //-- close down view comm dialog program
{	
	delete VCMgr;	
	if(VCDisplay)
		delete VCDisplay;	
}


/******************************************************************************
 *loadVCmodSWORD - load a commentary module into the view commentary dialog
 *
 ******************************************************************************/
void loadVCmodSWORD(gchar *modName)
{
        ModMap::iterator it;
        
        it = VCMgr->Modules.find(modName);  //-- find module we want to use
	if (it != VCMgr->Modules.end()){		
		VCMod = (*it).second;  //-- set curdictMod to new choice
		VCMod->SetKey("");		
		VCMod->Display();	 //-- display new dict
	}
}

/******************************************************************************
 *gotoverseVCSWORD - find new verse for view commentary dialog
 *
 ******************************************************************************/
void gotoverseVCSWORD(gchar *newkey)
{       
	DefaultVCKey.AutoNormalize(0);
	DefaultVCKey.Persist(1);	// when set to a module, make the module hold on to this actual key and not a copy
	DefaultVCKey = newkey; // set to our new verse key
	VCMod->SetKey(DefaultVCKey); //-- set key to our text
        VCMod->Display();
	DefaultVCKey.AutoNormalize(1);
}

void navVCModSWORD(gint direction)  //-- navigate the current commentary module
{
        switch(direction){   
		case 0: (*VCMod)--;
			break;
		case 1: (*VCMod)++;
			break;
        }
        VCMod->Error(); //-- clear any errors
        VCMod->Display();
}


