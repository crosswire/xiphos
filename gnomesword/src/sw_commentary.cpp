/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * sw_dictlex.cpp (book support)
     * -------------------
     * Wed Apr  3 21:23:15 2002
     * copyright (C) 2002 by Terry Biggs
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
#include <markupfiltmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "gs_gnomesword.h"
#include "sw_sword.h"
#include "sw_display.h"
#include "sw_commentary.h"
//#include "gs_gbs.h"

typedef struct _backend_comm BE_COMM;
struct _backend_comm {
    SWModule *mod;
    SWDisplay *dlDisp;
    int num;
};

/*
 * externs  
 */
extern SETTINGS *settings;

/*
 * globals  
 */
SWMgr *swmgrCOMM;
GList *be_comm_list;

/***  start code  ***/

static SWModule *backend_getModule(char *modname)
{
    GList *tmp = NULL;
    BE_COMM * c;
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

void backend_newDisplayCOMM(GtkWidget * html, char *modname, SETTINGS * s)
{
    GList *tmp = NULL;
    BE_COMM * c;

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

void backend_setupCOMM(SETTINGS * s)
{
    ModMap::iterator it;	//-- iteratior
    gint count = 0;

    swmgrCOMM = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgrs
    be_comm_list = NULL;
    for (it = swmgrCOMM->Modules.begin(); it != swmgrCOMM->Modules.end(); it++) {
	if (!strcmp((*it).second->Type(), COMM_MODS)) {
	    BE_COMM *c = new BE_COMM;
	    c->mod = (*it).second;
	    c->num = count;
	    be_comm_list = g_list_append(be_comm_list, (BE_COMM *) c);
	    ++count;
	}
    }
}

void backend_shutdownCOMM(void)
{
    delete swmgrCOMM;

	/***  free backend stuff  ***/
    be_comm_list = g_list_first(be_comm_list);
    while (be_comm_list != NULL) {
	BE_COMM *c = (BE_COMM *) be_comm_list->data;
	if (c->dlDisp)
	    delete c->dlDisp;	//-- delete any swdisplays created
	delete(BE_COMM *) be_comm_list->data;
	be_comm_list = g_list_next(be_comm_list);
    }
    g_list_free(be_comm_list);
}

void backend_displayinCOMM(int modnum, gchar * key)
{ 
    BE_COMM * co;
    co = (BE_COMM *)g_list_nth_data( be_comm_list, modnum);
    
    co->mod->SetKey(key);
    co->mod->Display();
}


void backend_dictentryTextChangedCOMM(gchar * modname, char *mytext)
{
    SWModule *mod = backend_getModule(modname);
    if (mod) {			//-- if we have a dict module  
	if (strcmp(mytext, "")) {	//-- if text is not null                
	    mod->SetKey(mytext);	//-- set key to our text
	    mod->Display();	//-- show what we found       
	}
    }
}

/******   end of file   ******/
