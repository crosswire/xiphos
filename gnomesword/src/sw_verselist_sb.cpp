/***************************************************************************
                                sw_verselist_sb.cpp
                             -------------------
    begin                : Thur Aug 02 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>

#include "gs_gnomesword.h"
#include "gs_html.h"
#include "gs_display.h"
#include "gs_shortcutbar.h"

#include "sw_utility.h"
#include "sw_verselist_sb.h"

static SWDisplay 
	*verselistsbDisplay,	/* to display modules in verselist dialog */
	*verselisttextsbDisplay;
static SWMgr 
	*verselistsbMgr; 
static SWModule 
	*verselistsbMod;   /* module for verselist dialog */


/*
 * parse vlist for verses and
 * display list in Verse List Shortcut bar
 */
gboolean getVerseListSBSWORD(gchar *modName, gchar *vlist, SETTINGS *s)
{
	gboolean retval=FALSE;
	gchar buf[256], firstkey[256];
	ListKey tmpVerseList;
	VerseKey DefaultVSKey;
	gint count=0;
	
	
	for(int i=0; i<strlen(vlist); i++){
		if(vlist[i] == '+') 
			vlist[i] = ' ';
		if(vlist[i] == ',') 
			vlist[i] = ';';
	}
	tmpVerseList = DefaultVSKey.ParseVerseList((char *)vlist, DefaultVSKey);
	beginHTML(s->vlsbhtml, FALSE);
	while (!tmpVerseList.Error()) {
		sprintf(buf,"<font size=\"%s\"><a href=\"%s\">%s</a></font><br>",
					s->verselist_font_size,
					(const char *)tmpVerseList,
					(const char *)tmpVerseList);
		displayHTML(s->vlsbhtml, buf, strlen(buf));
		if(!count) sprintf(firstkey,"%s",(const char *)tmpVerseList);
		tmpVerseList++;
		++count;
	}
	endHTML(s->vlsbhtml);
	if(count>0){ 
		showSBVerseList(s);
		retval = TRUE;
		ModMap::iterator it; 	
		it = verselistsbMgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
		if (it != verselistsbMgr->Modules.end()){ //-- if we find the module	
			verselistsbMod = (*it).second;  //-- change module to new module
			verselistsbMod->SetKey(firstkey); //-- set key to the first one in the list
			verselistsbMod->Display(); 
		}	
	}	
	return retval;
}


/****************************************************************************************
 *setupVLSWORD - set up the sword stuff for the verselist dialog
 ****************************************************************************************/
void setupVerseListSBSWORD(GtkWidget *html_widget)
{	
	ModMap::iterator it; //-- iteratior	
	SectionMap::iterator sit; //-- iteratior
	
	verselistsbMgr	= new SWMgr();
	verselistsbMod     = NULL;
	verselistsbDisplay = new  GtkHTMLEntryDisp(html_widget);
	verselisttextsbDisplay = new  GTKutf8ChapDisp(html_widget);
	
	for(it = verselistsbMgr->Modules.begin(); it != verselistsbMgr->Modules.end(); it++){
		verselistsbMod = (*it).second;
		sit = verselistsbMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters			
		ConfigEntMap &section = (*sit).second;
		addrenderfiltersSWORD(verselistsbMod, section);
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			verselistsbMod->Disp(verselisttextsbDisplay);			
		}else{
			verselistsbMod->Disp(verselistsbDisplay);
		}
	}
}

/*** close down verselist dialog ***/
void shutdownverselistSBSWORD(void) 
{	
	delete verselistsbMgr;	
	if(verselistsbDisplay)
		delete verselistsbDisplay;	
	if(verselisttextsbDisplay)
		delete verselisttextsbDisplay ;
}

void
changeVerseListSBSWORD(gchar *url)
{
	verselistsbMod->SetKey(url);
	verselistsbMod->Display();	
}



