/*
 * GnomeSword Bible Study Tool
 * viewtext.cpp - sword support for veiwtext dialog
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

#include "viewtext.h"
#include "sword.h"
#include "display.h"

static SWDisplay *dispaly;	/* to display modules in view text dialog */
static SWMgr *mgr;		/* sword mgr for view text dialog */
static SWModule *mod;		/* module for view text dialog */


/******************************************************************************
 * Name
 *   backend_setup_viewtext
 *
 * Synopsis
 *   #include "viewtext.h"
 *
 *   GList *backend_setup_viewtext(GtkWidget * text)	
 *
 * Description
 *   setup the viewtext sword display
 *
 * Return value
 *   GList *
 */
GList *backend_setup_viewtext(GtkWidget * text)
{
	GList *glist;
	ModMap::iterator it;	//-- iteratior     
	SectionMap::iterator sit;	//-- iteratior
	extern SETTINGS *settings;

	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	mod = NULL;
	dispaly = new GtkHTMLChapDisp(text, settings);
	glist = NULL;

	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			mod = (*it).second;
			glist = g_list_append(glist, mod->Name());
			mod->Disp(dispaly);
		}
	}

	return glist;
}


/******************************************************************************
 * Name
 *   backend_shutdown_viewtext
 *
 * Synopsis
 *   #include "viewtext.h"
 *
 *   void backend_shutdown_viewtext(void)	
 *
 * Description
 *   shutdown down viewtext support
 *
 * Return value
 *   void
 */
void backend_shutdown_viewtext(void)
{
	delete mgr;
	if (dispaly)
		delete dispaly;
}

/******************************************************************************
 * Name
 *   backend_goto_verse_viewtext
 *
 * Synopsis
 *   #include "viewtext.h"
 *
 *   void backend_goto_verse_viewtext(gchar * newkey)	
 *
 * Description
 *   find and display new verse for view text dialog
 *
 * Return value
 *   void
 */
void backend_goto_verse_viewtext(gchar * newkey)
{
	mod->SetKey(newkey);	//-- set key to our text
	mod->Display();

}

/******************************************************************************
 * Name
 *   backend_load_module_viewtext
 *
 * Synopsis
 *   #include "viewtext.h"
 *
 *   void backend_load_module_viewtext(gchar * modName)	
 *
 * Description
 *   load a text module into the view text dialog
 *
 * Return value
 *   void
 */
void backend_load_module_viewtext(gchar * module_name)
{
	ModMap::iterator it;

	it = mgr->Modules.find(module_name);	//-- find module we want to use
	if (it != mgr->Modules.end()) {

		mod = (*it).second;	//-- set curdictMod to new choice
		mod->SetKey("");
		mod->Display();	//-- display new dict
	}
}

/******************************************************************************
 * Name
 *   backend_get_book_viewtext
 *
 * Synopsis
 *   #include "viewtext.h"
 *
 *   gchar * backend_get_book_viewtext(void)	
 *
 * Description
 *   get current book displayed in veiw text dialog
 *
 * Return value
 *   char * - must be freed by calling function
 */
char *backend_get_book_viewtext(void)
{
	char s1[255], s2[255];
	gint l;

	VerseKey key = mod->KeyText();
	sprintf(s1, "%s", (const char *) key);
	for (l = 0; l < strlen(s1); l++) {	//--  seperate book name                     
		if (isdigit(s1[l]))
			break;
	}
	strncpy(s2, s1, l);
	s2[l] = '\0';
	return g_strdup(s2);
}

/******************************************************************************
 * Name
 *  backend_get_chapter_viewtext 
 *
 * Synopsis
 *   #include "viewtext.h"
 *
 *   int backend_get_chapter_viewtext(void)	
 *
 * Description
 *   get current chapter displayed in veiw text dialog
 *
 * Return value
 *   int
 */
int backend_get_chapter_viewtext(void)
{
	VerseKey key = mod->KeyText();
	return key.Chapter();
}

/******************************************************************************
 * Name
 *   backend_get_verse_viewtext
 *
 * Synopsis
 *   #include "viewtext.h"
 *
 *   int backend_get_verse_viewtext(void)	
 *
 * Description
 *   get current verse displayed in veiw text dialog
 *
 * Return value
 *   int
 */
int backend_get_verse_viewtext(void)
{
	VerseKey key = mod->KeyText();
	return key.Verse();
}

/******************************************************************************
 * Name
 *   backend_set_global_options_viewtext
 *
 * Synopsis
 *   #include "viewtext.h"
 *
 *   void backend_set_global_options_viewtext(gchar * option, gchar * onoff)	
 *
 * Description
 *   set global options for module in view text dialog
 *   (strongs, morph tags, footnotes)
 *
 * Return value
 *   void
 */
void backend_set_global_options_viewtext(gchar * option, gchar * onoff)
{
	mgr->setGlobalOption(option, onoff);
	mod->Display();		//-- we need to show change
}
