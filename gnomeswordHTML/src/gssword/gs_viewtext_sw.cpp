/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_viewtext_sw.cpp
                             -------------------
    begin                : Tue June 12 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sf.net
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
#include "gs_viewtext_sw.h"
#include "gs_sword.h"
#include "gs_display.h"
#include "sw_utility.h"

SWDisplay *VTDisplay;	/* to display modules in view comm dialog */
SWMgr *VTMgr;	/* sword mgr for view comm dialog */
SWModule *VTMod;   /* module for view text dialog */



/****************************************************************************************
 *setupVTSWORD - set up the sword stuff for the viewtext dialog
 *returns a list of text modules
 ****************************************************************************************/
GList* VTsetupSWORD(GtkWidget *text, GtkWidget *cbBook)
{
	GList *list;
	ModMap::iterator it; //-- iteratior	
	SectionMap::iterator sit; //-- iteratior
	GList *cbBook_items;

	/* fill book combo box */
	cbBook_items = getBibleBooks();
	gtk_combo_set_popdown_strings(GTK_COMBO(cbBook), cbBook_items);
	VTMgr	= new SWMgr();
	VTMod     = NULL;
	VTDisplay = new  GTKhtmlChapDisp(text);
	list = NULL;
	for(it = VTMgr->Modules.begin(); it != VTMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			VTMod = (*it).second;
			sit = VTMgr->config->Sections.find((*it).second->Name()); //-- add render filters			
			ConfigEntMap &section = (*sit).second;
			addrenderfiltersSWORD(VTMod, section);
			//havebible = TRUE;		
			list = g_list_append(list,VTMod->Name());
			VTMod->Disp(VTDisplay);
		}
	}
	g_list_free(cbBook_items);
	return list;
}


//-------------------------------------------------------------------------------------------
void VTshutdownSWORD(void)  //-- close down viewtext dialog program
{	
	delete VTMgr;	
	if(VTDisplay)
		delete VTDisplay;	
}


/******************************************************************************
 *gotoverseVTSWORD - find new verse for view text dialog
 *
 ******************************************************************************/
void VTgotoverseSWORD(gchar *newkey)
{
        VTMod->SetKey(newkey); //-- set key to our text
        VTMod->Display();
	
}

/******************************************************************************
 *loadVTmodSWORD - load a text module into the view commentary dialog
 *
 ******************************************************************************/
void VTloadmodSWORD(gchar *modName)
{
        ModMap::iterator it;
        
        it = VTMgr->Modules.find(modName);  //-- find module we want to use
	if (it != VTMgr->Modules.end()){
		
		VTMod = (*it).second;  //-- set curdictMod to new choice
		VTMod->SetKey("");		
		VTMod->Display();	 //-- display new dict
	}
}


/******************************************************************************
 * returns the description of the view text dialog module
 ******************************************************************************/
gchar* VTgetmodDescriptionSWORD(void)
{
	return (char *) VTMod->Description();;
}

/******************************************************************************
 * change verse in VT dialog
 ******************************************************************************/
void VTchangeverseSWORD(gchar * verse)
{
	if (VTMod) {
		VTMod->SetKey(verse);
		VTMod->Display();
	}
}

gchar *VTgetbookSWORD(void)
{
	char s1[255],s2[255];
	gint l;
	
	VerseKey VTKey = VTMod->KeyText();
	sprintf(s1,"%s",(const char *)VTKey);
	for(l=0;l<strlen(s1);l++){ //--  seperate book name			
		if(isdigit(s1[l])) break;
	}
	strncpy(s2,s1,l);
	s2[l] = '\0';
	return s2;
}

gint VTgetchapterSWORD(void)
{
	VerseKey VTKey = VTMod->KeyText();			
	return VTKey.Chapter();	
}

gint VTgetverseSWORD(void)
{
	VerseKey VTKey = VTMod->KeyText();
	return VTKey.Verse();
}

/******************************************************************************
 * set global options   (strongs, morph tags, footnotes)
 ******************************************************************************/
void VTsetGlobalOptionsSWORD(gchar *option, gchar *onoff)
{
	VTMgr->setGlobalOption(option, onoff);
	VTMod->Display(); //-- we need to show change
}



