/*
 * GnomeSword Bible Study Tool
 * percomm_.cpp - support for Sword personal commentary modules
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

#include "backend/percomm_.h"
#include "backend/sword.h"
#include "backend/display.h"

typedef struct _backend_percomm BE_PC;
struct _backend_percomm {
	SWModule *mod;
	SWDisplay *disp;
	int num;
};

/******************************************************************************
 * globals to this file only 
 */

static SWMgr *mgr;
static GList *be_percomm_list;

/******************************************************************************
 * Name
 *  backend_get_percomm_key
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   char *backend_get_percomm_key(int mod_num)	
 *
 * Description
 *    returns the key for the current personal commentary
 *
 * Return value
 *   const char *
 */
 
const char *backend_get_percomm_key(int mod_num)
{
	BE_PC *p;
	p = (BE_PC *) g_list_nth_data(be_percomm_list, mod_num);
	return p->mod->KeyText();
}

/******************************************************************************
 * Name
 *  backend_save_personal_comment
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   void backend_save_personal_comment(int mod_num, char * buf)
 *
 * Description
 *   save buf to the current personal commentary at current key 
 *
 * Return value
 *   void
 */
 
void backend_save_personal_comment(int mod_num, char * buf)
{	
	BE_PC *p;
	p = (BE_PC *) g_list_nth_data(be_percomm_list, mod_num);
	if (buf)
		*p->mod << (const char *) buf;
}

/******************************************************************************
 * Name
 *  backend_delete_personal_comment
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   void backend_delete_personal_comment(int mod_num)
 *
 * Description
 *    delete the data at the currnet key in current personal commentary
 *
 * Return value
 *   void
 */
 
void backend_delete_personal_comment(int mod_num)
{
	BE_PC *p;
	p = (BE_PC *) g_list_nth_data(be_percomm_list, mod_num);
	g_warning("key %s of module %s would have be deleted",p->mod->KeyText(),p->mod->Name());
	/* not finished - does nothing */ 
}

/******************************************************************************
 * Name
 *  backend_nav_percomm_module
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   void backend_nav_percomm_module(int mod_num, int direction)	
 *
 * Description
 *    navigate the current percomm module
 *
 * Return value
 *   void
 */
 
void backend_nav_percomm_module(int mod_num, int direction)
{
	BE_PC *p;

	p = (BE_PC *) g_list_nth_data(be_percomm_list, mod_num);

	switch (direction) {
	case 0:
		(*p->mod)--;
		break;
	case 1:
		(*p->mod)++;
		break;
	}

	p->mod->Error(); /*clear any errors */
	p->mod->Display();
}

/******************************************************************************
 * Name
 *  backend_new_percomm_display
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   void backend_new_percomm_display(GtkWidget *html,char *mod_name,SETTINGS *s)	
 *
 * Description
 *   create new sword dispaly 
 *
 * Return value
 *   void
 */

void backend_new_percomm_display(GtkWidget * html, char *mod_name,
			      SETTINGS * s)
{
	GList *tmp = NULL;
	BE_PC *p;

	tmp = g_list_first(be_percomm_list);
	while (tmp != NULL) {
		p = (BE_PC *) tmp->data;
		if (!strcmp(p->mod->Name(), mod_name)) {
			p->disp = new GtkHTMLEntryDisp(html, s);
			p->mod->Disp(p->disp);
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *  backend_setup_percomm
 *
 * Synopsis
 *   #include "percomm_.h"
 *   
 *   void backend_setup_percomm(SETTINGS * s)	
 *
 * Description
 *   setup sword text module support
 *
 * Return value
 *   void
 */

void backend_setup_percomm(SETTINGS * s)
{
	ModMap::iterator it;
	gint count = 0;

	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	be_percomm_list = NULL;
	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), COMM_MODS)) {
			if ((*mgr->config->Sections[(*it).second->Name()].
			     find("ModDrv")).second == "RawFiles") {
				BE_PC *p = new BE_PC;
				p->mod = (*it).second;
				p->num = count;
				be_percomm_list =
				    g_list_append(be_percomm_list, (BE_PC *) p);
				++count;
			}
		}
	}
}

/******************************************************************************
 * Name
 *  backend_shutdown_percomm
 *
 * Synopsis
 *   #include "percomm_.h"
 *   
 *   void backend_shutdown_percomm(void)	
 *
 * Description
 *   shut down sword suppoet
 *
 * Return value
 *   void
 */

void backend_shutdown_percomm(void)
{
	delete mgr;
	/*
	 * free the backend stuff
	 */
	be_percomm_list = g_list_first(be_percomm_list);
	while (be_percomm_list != NULL) {
		BE_PC *p = (BE_PC *) be_percomm_list->data;
		/* 
		 * delete any swdisplays created 
		 */
		if (p->disp)
			delete p->disp;
		delete(BE_PC *) be_percomm_list->data;
		be_percomm_list = g_list_next(be_percomm_list);
	}
	g_list_free(be_percomm_list);
}

/******************************************************************************
 * Name
 *  backend_display_percomm
 *
 * Synopsis
 *   #include "percomm_.h"
 *   
 *   void backend_display_percomm(int mod_num, char *key)	
 *
 * Description
 *   display new key or module
 *
 * Return value
 *   void
 */

void backend_display_percomm(int mod_num, char *key)
{
	BE_PC *p;
	p = (BE_PC *) g_list_nth_data(be_percomm_list, mod_num);

	p->mod->SetKey(key);
	p->mod->Display();
}
	
/******   end of file   ******/
