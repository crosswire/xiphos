/*
 * GnomeSword Bible Study Tool
 * commentary.cpp - support for Sword commentary modules
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
#include "commentary.h"

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

/******************************************************************************
 * Name
 *  backend_nav_commentary_COMM
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void backend_nav_commentary_COMM(gint modnum, gint direction)	
 *
 * Description
 *    navigate the current commentary module and return key
 *    return value must be freed by calling function
 *
 * Return value
 *   char *
 */
char* backend_nav_commentary_COMM(gint modnum, gint direction)
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

	co->mod->Error();	//-- clear any errors
	co->mod->Display();
	if(co->mod->KeyText())
		return g_strdup(co->mod->KeyText());
	else
		return NULL;
}

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

void backend_newDisplayCOMM(GtkWidget * html, char *modname,
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

void backend_setupCOMM(SETTINGS * s)
{
	ModMap::iterator it;	//-- iteratior
	gint count = 0;

	swmgrCOMM = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	be_comm_list = NULL;
	for (it = swmgrCOMM->Modules.begin();
	     it != swmgrCOMM->Modules.end(); it++) {
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

void backend_shutdownCOMM(void)
{
	delete swmgrCOMM;

	/***  free backend stuff  ***/
	be_comm_list = g_list_first(be_comm_list);
	while (be_comm_list != NULL) {
		BE_COMM *c = (BE_COMM *) be_comm_list->data;
		if (c->dlDisp)	//-- delete any swdisplays created
			delete c->dlDisp;
		delete(BE_COMM *) be_comm_list->data;
		be_comm_list = g_list_next(be_comm_list);
	}
	g_list_free(be_comm_list);
}

void backend_displayinCOMM(int modnum, gchar * key)
{
	BE_COMM *co;
	co = (BE_COMM *) g_list_nth_data(be_comm_list, modnum);

	co->mod->SetKey(key);
	co->mod->Display();
}

/******   end of file   ******/
