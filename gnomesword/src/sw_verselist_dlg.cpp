/***************************************************************************
                                sw_verselist_dlg.cpp
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
#include "gs_verselist_dlg.h"
#include "gs_html.h"
#include "gs_display.h"

#include "sw_utility.h"
#include "sw_verselist_dlg.h"

static SWDisplay 
	*verselistDisplay,	/* to display modules in verselist dialog */
	*verselisttextDisplay;
static SWMgr 
	*verselistMgr; 
static SWModule 
	*verselistMod;   /* module for verselist dialog */

extern GtkWidget 
	*htmlVL;
extern gboolean 
	isrunningVL;

/*
 * parse vlist for verses
 */
gboolean getVerseListSWORD(gchar *modName, gchar *vlist, SETTINGS *s)
{
	gboolean retval=FALSE;
	gchar buf[256], firstkey[256];
	//const char *firstkey;
	ListKey tmpVerseList;
	VerseKey DefaultVSKey;
	gint count=0;
	static GtkWidget *dlg;
	
	if(!isrunningVL){
		dlg = create_dglVerseList();
		isrunningVL = TRUE;
	}		
	for(int i=0; i<strlen(vlist); i++){
		if(vlist[i] == '+') 
			vlist[i] = ' ';
		if(vlist[i] == ',') 
			vlist[i] = ';';
	}
	tmpVerseList = DefaultVSKey.ParseVerseList((char *)vlist, DefaultVSKey);
	beginHTML(htmlVL, FALSE);
	while (!tmpVerseList.Error()) {
		sprintf(buf,"<font size=\"%s\"><a href=\"%s\">%s</a></font><br>",
					s->verselist_font_size,
					(const char *)tmpVerseList,
					(const char *)tmpVerseList);
		displayHTML(htmlVL, buf, strlen(buf));
		if(!count) sprintf(firstkey,"%s",(const char *)tmpVerseList);
		tmpVerseList++;
		++count;
	}
	endHTML(htmlVL);
	if(count>0){ 
		gtk_widget_show(dlg);
		retval = TRUE;
		ModMap::iterator it; 	
		it = verselistMgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
		if (it != verselistMgr->Modules.end()){ //-- if we find the module	
			verselistMod = (*it).second;  //-- change module to new module
			verselistMod->SetKey(firstkey); //-- set key to the first one in the list
			verselistMod->Display(); 
		}	
	}	
	return retval;
}


/****************************************************************************************
 *setupVLSWORD - set up the sword stuff for the verselist dialog
 ****************************************************************************************/
void setupVerseListSWORD(GtkWidget *html_widget)
{	
	ModMap::iterator it; //-- iteratior	
	SectionMap::iterator sit; //-- iteratior
	
	verselistMgr	= new SWMgr();
	verselistMod     = NULL;
	verselistDisplay = new  GtkHTMLEntryDisp(html_widget);
	verselisttextDisplay = new  GTKutf8ChapDisp(html_widget);
	
	for(it = verselistMgr->Modules.begin(); it != verselistMgr->Modules.end(); it++){
		verselistMod = (*it).second;
		sit = verselistMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters			
		ConfigEntMap &section = (*sit).second;
		addrenderfiltersSWORD(verselistMod, section);
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			verselistMod->Disp(verselisttextDisplay);			
		}else{
			verselistMod->Disp(verselistDisplay);
		}
	}
}

/*** close down verselist dialog ***/
void shutdownverselistSWORD(void) 
{	
	delete verselistMgr;	
	if(verselistDisplay)
		delete verselistDisplay;	
	if(verselisttextDisplay)
		delete verselisttextDisplay ;
}

void
changeVerseListSWORD(gchar *url)
{
	verselistMod->SetKey(url);
	verselistMod->Display();	
}



