/*
 * GnomeSword Bible Study Tool
 * dictlex_.cpp - support for Sword commentary modules
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
#include <markupfiltmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "main/gs_gnomesword.h"

#include "backend/sword.h"
#include "backend/display.h"
#include "backend/dictlex_.h"

typedef struct _backend_dl BE_DL;
struct _backend_dl {
	SWModule *mod;
	SWDisplay *dlDisp;
	int num;
};

/******************************************************************************
 * global to this file only 
 */

static SWMgr *swmgrDL;
static GList *be_dl_list;


/******************************************************************************
 * Name
 *  backend_new_dictlex_display
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   void backend_new_dictlex_display(GtkWidget * html, int mod_num,
 *							SETTINGS * s)	
 *
 * Description
 *   create new sword display for module 
 *
 * Return value
 *   void
 */
 
void backend_new_dictlex_display(GtkWidget * html, int mod_num,
							SETTINGS * s)
{
	BE_DL *d;

	d = (BE_DL *) g_list_nth_data(be_dl_list, mod_num);
	d->dlDisp = new GtkHTMLEntryDisp(html, s);
	d->mod->Disp(d->dlDisp);
		
}

/******************************************************************************
 * Name
 *  backend_setup_dictlex
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   void backend_setup_dictlex(SETTINGS * s)
 *
 * Description
 *   setup sword support for dict/lex modules 
 *
 * Return value
 *   void
 */
 
void backend_setup_dictlex(SETTINGS * s)
{
	ModMap::iterator it;	//-- iteratior
	gint count = 0;

	swmgrDL = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgrs
	be_dl_list = NULL;
	for (it = swmgrDL->Modules.begin();
	     it != swmgrDL->Modules.end(); it++) {
		if (!strcmp
		    ((*it).second->Type(), "Lexicons / Dictionaries")) {	    
			if(!strcmp((*it).second->Name(),"Thayer"))
				s->havethayer = true;
			if(!strcmp((*it).second->Name(),"BDB"))
				s->havebdb = true;
			BE_DL *be_dl = new BE_DL;
			be_dl->mod = (*it).second;
			be_dl->num = count;
			be_dl_list =
			    g_list_append(be_dl_list, (BE_DL *) be_dl);
			++count;
		}
	}
}

/******************************************************************************
 * Name
 *  backend_shutdown_dictlex
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   void backend_shutdown_dictlex(void)	
 *
 * Description
 *    shutdown and cleanup
 *
 * Return value
 *   void
 */
 
void backend_shutdown_dictlex(void)
{
	delete swmgrDL;

	/***  free backend stuff  ***/
	be_dl_list = g_list_first(be_dl_list);
	while (be_dl_list != NULL) {
		BE_DL *d = (BE_DL *) be_dl_list->data;
		if (d->dlDisp)
			delete d->dlDisp;	//-- delete any swdisplays created
		delete(BE_DL *) be_dl_list->data;
		be_dl_list = g_list_next(be_dl_list);
	}
	g_list_free(be_dl_list);
}

/******************************************************************************
 * Name
 *  backend_display_dictlex
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   void backend_display_dictlex(int mod_num, char * key)	
 *
 * Description
 *    display dictionary at key
 *
 * Return value
 *   void
 */
 
void backend_display_dictlex(int mod_num, char * key)
{
	BE_DL *d;

	d = (BE_DL *) g_list_nth_data(be_dl_list, mod_num);	
	
	d->mod->SetKey(key);
	d->mod->Display();
}

/******************************************************************************
 * Name
 *  backend_fill_dictlex_keys
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   GList *backend_fill_dictlex_keys(int mod_num, int count)	
 *
 * Description
 *    returns a list of dictionary keys to fill the clist
 *    count is the number of keys it will take to fill the clist
 * Return value
 *   GList* 
 */
 
GList *backend_fill_dictlex_keys(int mod_num, int count)
{	
	SWKey saveKey;		//-- for starting point
	char *listitem;	//-- list widget to display items found
	GList *retList = NULL;
	BE_DL *d;

	d = (BE_DL *) g_list_nth_data(be_dl_list, mod_num);

	d->mod->KeyText(); //-- snap to entry
	saveKey = d->mod->KeyText();	//-- save our place

	if (d->mod) {	//-- make sure we have module to work with             

		for (int i = 0; i < (count / 2); i++)
			(*d->mod)++;/* get equal number of keys before and 
				       after our starting key(saveKey) */

		for (int i = 0; i < count - 1; i++)
			(*d->mod)--;

		d->mod->Error(); //-- clear errors

		for (; !d->mod->Error() && count;
		     count--, (*d->mod)++) {
			/* must be freed by calling function */
			listitem = g_strdup((const char *) d->mod->KeyText());
			retList = g_list_append(retList, (char *) listitem);
		}
	}
	d->mod->SetKey(saveKey);
	
	return retList;
}
