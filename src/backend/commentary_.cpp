/*
 * GnomeSword Bible Study Tool
 * commentary_.cpp - support for Sword commentary modules
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
#include <markupfiltmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "gs_gnomesword.h"
#include "sword.h"
#include "display.h"
#include "commentary_.h"

typedef struct _backend_comm BE_COMM;
struct _backend_comm {
	SWModule *mod;
	SWDisplay *dlDisp;
	int num;
};

/******************************************************************************
 * static - global to this file only 
 */
static SWMgr *mgr;
static GList *be_comm_list;

/******************************************************************************
 * Name
 *  backend_display_book_heading
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   void backend_display_book_heading(gint modnum)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
 
void backend_display_book_heading(gint modnum)
{
	char key[256];
	BE_COMM *c = (BE_COMM *) g_list_nth_data(be_comm_list, modnum);

	VerseKey vkey,nkey;
	vkey = settings.currentverse; //c->mod->KeyText();
	const char *book = vkey.books[vkey.Testament() - 1][vkey.Book() -
							 1].name;
		
	sprintf(key,"%s 0:0",book);
	nkey.Persist(1);
	nkey.AutoNormalize(1);
	nkey = settings.currentverse;
	c->mod->SetKey(nkey);
	
	nkey.AutoNormalize(0);
	nkey = key;
	c->mod->SetKey(nkey);
	c->mod->Error();	
	c->mod->Display();
	nkey.Persist(0);
}


/******************************************************************************
 * Name
 *  backend_display_chap_heading
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void backend_display_chap_heading(gint modnum)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
 
void backend_display_chap_heading(gint modnum)
{
	char key[256];
	BE_COMM *c = (BE_COMM *) g_list_nth_data(be_comm_list, modnum);

	VerseKey vkey,nkey;
	vkey = settings.currentverse; //c->mod->KeyText();
	const char *book = vkey.books[vkey.Testament() - 1][vkey.Book() -
							 1].name;
	int chap = vkey.Chapter();
	
	sprintf(key,"%s %d:0",book,chap);
	nkey.Persist(1);
	nkey.AutoNormalize(1);
	nkey = settings.currentverse;
	c->mod->SetKey(nkey);
	
	nkey.AutoNormalize(0);
	nkey = key;
	c->mod->SetKey(nkey);
	c->mod->Error();	
	c->mod->Display();
	nkey.Persist(0);
}


/******************************************************************************
 * Name
 *  backend_nav_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void backend_nav_commentary(gint modnum, gint direction)	
 *
 * Description
 *    navigate the current commentary module and return key
 *    return value must be freed by calling function
 *
 * Return value
 *   char *
 */
 
const char* backend_nav_commentary(gint modnum, gint direction)
{
	BE_COMM *co;

	co = (BE_COMM *) g_list_nth_data(be_comm_list, modnum);

	switch (direction) {
	case 0:
		(*co->mod)--;
		break;
	case 1:
		(*co->mod)++;
		break;
	}

	co->mod->Error();	
	co->mod->Display();
	if(co->mod->KeyText())
		return co->mod->KeyText();
	else
		return NULL;
}

/******************************************************************************
 * Name
 *  backend_get_module
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   SWModule *backend_get_module(char *modname)	
 *
 * Description
 *    
 *    
 *
 * Return value
 *   SWModule *
 */

static SWModule *backend_get_module(char *modname)
{
	GList *tmp = NULL;
	BE_COMM *c;
	SWModule *mod = NULL;

	tmp = be_comm_list;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		c = (BE_COMM *) tmp->data;
		if (!strcmp(c->mod->Name(), modname)) {
			mod = c->mod;
			break;
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return mod;
}

/******************************************************************************
 * Name
 *   backend_new_display_commentary
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   void backend_new_display_commentary(GtkWidget * html, char *modname,
 *							SETTINGS * s)	
 *
 * Description
 *    create a sword display for a commentary module
 *    
 *
 * Return value
 *   void
 */
 
void backend_new_display_commentary(GtkWidget * html, char *modname,
							SETTINGS * s)
{
	GList *tmp = NULL;
	BE_COMM *c;

	tmp = g_list_first(be_comm_list);
	while (tmp != NULL) {
		c = (BE_COMM *) tmp->data;
		if (!strcmp(c->mod->Name(), modname)) {
			c->dlDisp = new GtkHTMLEntryDisp(html, s);
			c->mod->Disp(c->dlDisp);
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *    backend_setup_commentary
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   void backend_setup_commentary(SETTINGS * s)	
 *
 * Description
 *   set up sword commentary support
 *    
 *
 * Return value
 *   void
 */

void backend_setup_commentary(SETTINGS * s)
{
	ModMap::iterator it;
	gint count = 0;

	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	be_comm_list = NULL;
	for (it = mgr->Modules.begin();
	     it != mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), COMM_MODS)) {
			BE_COMM *c = new BE_COMM;
			c->mod = (*it).second;
			c->num = count;
			be_comm_list =
			    g_list_append(be_comm_list, (BE_COMM *) c);
			++count;
		}
	}
}

/******************************************************************************
 * Name
 *    backend_shutdown_commentary
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   void backend_shutdown_commentary(void)	
 *
 * Description
 *    shut down sword commentary support
 *    and cleanup
 *
 * Return value
 *   void
 */

void backend_shutdown_commentary(void)
{
	delete mgr;
	/*
	 * free backend stuff  
	 */
	be_comm_list = g_list_first(be_comm_list);
	while (be_comm_list != NULL) {
		BE_COMM *c = (BE_COMM *) be_comm_list->data;
		/*
		 * delete any swdisplays created
		 */
		if (c->dlDisp)
			delete c->dlDisp;
		
		delete(BE_COMM *) be_comm_list->data;
		be_comm_list = g_list_next(be_comm_list);
	}
	g_list_free(be_comm_list);
}

/******************************************************************************
 * Name
 *  backend_display_commentary
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   void backend_display_commentary(int modnum, gchar * key)	
 *
 * Description
 *    display commentary
 *    
 *
 * Return value
 *   void
 */

void backend_display_commentary(int modnum, gchar * key)
{
	BE_COMM *co;
	co = (BE_COMM *) g_list_nth_data(be_comm_list, modnum);

	co->mod->SetKey(key);
	co->mod->Display();
}

/******   end of file   ******/
