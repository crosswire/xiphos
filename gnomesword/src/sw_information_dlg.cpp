/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * gs_information_sw.cpp
    * -------------------
    * Thu Jun 14 2001
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
#include <config.h>
#endif

#include <gnome.h>

#include <swmodule.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <versekey.h>

#include "gs_gnomesword.h"
#include "sw_information_dlg.h"
#include "sw_display.h"

SWDisplay *VIDDisplay;  /* to display modules in view comm dialog */
SWMgr *VIDMgr;  /* sword mgr for view comm dialog */
SWModule *VIDMod;   /* module for view text dialog */

extern SETTINGS *settings;
/******************************************************************************
 *loadVIDmodSWORD - load a module into the information dialog
 ******************************************************************************/
void VIDloadmodSWORD(gchar *modName, gchar *newkey)
{
        ModMap::iterator it;

        it = VIDMgr->Modules.find(modName);  //-- find module we want to use
	if (it != VIDMgr->Modules.end()){
		
		VIDMod = (*it).second;  //-- set curdictMod to new choice
		VIDMod->SetKey(newkey);		
		VIDMod->Display();	 //-- display new dict
	}
}

/****************************************************************************************
 *setupVIDSWORD - set up the sword stuff for the information dialog
 ****************************************************************************************/
void VIDsetupSWORD(GtkWidget *text)
{
	ModMap::iterator it; //-- iteratior	
	SectionMap::iterator sit; //-- iteratior    	
	
	VIDMgr	= new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgr
	VIDMod     = NULL;
	VIDDisplay = new  GtkHTMLEntryDisp(text,settings);
	for(it = VIDMgr->Modules.begin(); it != VIDMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
			VIDMod = (*it).second;
			VIDMod->Disp(VIDDisplay);
		}
	}
}


//-------------------------------------------------------------------------------------------
void VIDshutdownSWORD(void)  //-- close down viewtext dialog program
{	
	delete VIDMgr;	
	if(VIDDisplay)
		delete VIDDisplay;	
}


/******************************************************************************
 *gotokeyVIDSWORD - find new key
 *
 ******************************************************************************/
void VIDgotokeySWORD(gchar *newkey)
{
        VIDMod->SetKey(newkey);
        VIDMod->Display();
}









