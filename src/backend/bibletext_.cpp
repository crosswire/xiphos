/*
 * GnomeSword Bible Study Tool
 * bibletext_.cpp - support for Sword Bible text modules
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

/*
 * backend
 */
#include "bibletext_.h"

#include "sword.h"
#include "display.h"

typedef struct _backend_text BE_TEXT;
struct _backend_text {
	SWModule *mod;
	SWDisplay *disp;
	int num;
};

/******************************************************************************
 * globals to this file only 
 */

static SWMgr *mgr;
static GList *be_text_list;

/******************************************************************************
 * Name
 *  backend_nav_text_module
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void backend_nav_text_module(gint modnum, gint direction)	
 *
 * Description
 *    navigate the current text module
 *
 * Return value
 *   void
 */
 
void backend_nav_text_module(int modnum, int direction)
{
	BE_TEXT *t;

	t = (BE_TEXT *) g_list_nth_data(be_text_list, modnum);

	switch (direction) {
	case 0:
		(*t->mod)--;
		break;
	case 1:
		(*t->mod)++;
		break;
	}

	t->mod->Error(); /* clear any errors */
	t->mod->Display();
}

/******************************************************************************
 * Name
 *  backend_new_text_display
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void backend_new_text_display(GtkWidget * html,char *modname,SETTINGS * s)	
 *
 * Description
 *   create new sword dispaly 
 *
 * Return value
 *   void
 */

void backend_new_text_display(GtkWidget * html, char *modname,
			      SETTINGS * s)
{
	GList *tmp = NULL;
	BE_TEXT *t;

	tmp = g_list_first(be_text_list);
	while (tmp != NULL) {
		t = (BE_TEXT *) tmp->data;
		if (!strcmp(t->mod->Name(), modname)) {
			t->disp = new GtkHTMLChapDisp(html, s);
			t->mod->Disp(t->disp);
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *  backend_setup_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void backend_setup_text(SETTINGS * s)	
 *
 * Description
 *   setup sword text module support
 *
 * Return value
 *   void
 */

void backend_setup_text(SETTINGS * s)
{
	ModMap::iterator it;
	gint count = 0;

	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	be_text_list = NULL;
	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), TEXT_MODS)) {
			BE_TEXT *t = new BE_TEXT;
			t->mod = (*it).second;
			t->num = count;
			be_text_list =
			    g_list_append(be_text_list, (BE_TEXT *) t);
			++count;
		}
	}
}

/******************************************************************************
 * Name
 *  backend_shutdown_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void backend_shutdown_text(void)	
 *
 * Description
 *   shut down sword suppoet
 *
 * Return value
 *   void
 */

void backend_shutdown_text(void)
{
	delete mgr;
	/*
	 * free the backend stuff
	 */
	be_text_list = g_list_first(be_text_list);
	while (be_text_list != NULL) {
		BE_TEXT *t = (BE_TEXT *) be_text_list->data;
		if (t->disp)	//-- delete any swdisplays created
			delete t->disp;
		delete(BE_TEXT *) be_text_list->data;
		be_text_list = g_list_next(be_text_list);
	}
	g_list_free(be_text_list);
}

/******************************************************************************
 * Name
 *  backend_displayin_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void backend_displayin_text(int modnum, gchar * key)	
 *
 * Description
 *   display new key or module
 *
 * Return value
 *   void
 */

void backend_display_text(int modnum, char *key)
{
	BE_TEXT *t;
	t = (BE_TEXT *) g_list_nth_data(be_text_list, modnum);

	t->mod->SetKey(key);
	t->mod->Display();
}

/******************************************************************************
 * Name
 *  backend_set_text_global_option
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void backend_set_text_global_option(char * option, char * yesno)	
 *
 * Description
 *   sets sword module global options ie strongs, morph tags or footnotes
 *
 * Return value
 *   void
 */

void backend_set_text_global_option(char * option, char * yesno)
{
	/* 
	 * turn option on or off 
	 */
	mgr->setGlobalOption(option, yesno);
}

/******************************************************************************
 * Name
 *  backend_set_module_unlocked
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void backend_set_module_unlocked(char *mod_name, char *key)	
 *
 * Description
 *   unlocks locked module - FIXME: does not work :(
 *
 * Return value
 *   void
 */

void backend_set_module_unlocked(char *mod_name, char *key)
{	/* this does not work */
	g_warning("module is %s\nkey = %s",mod_name,key);
	mgr->setCipherKey(mod_name, key);	
}

/******************************************************************************
 * Name
 *  backend_check_for_global_option
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   gboolean backend_check_for_global_option(int mod_num, char *option)	
 *
 * Description
 *   returns true is module has option
 *
 * Return value
 *   gboolean
 */

gboolean backend_check_for_global_option(int mod_num, char *option)
{		
	BE_TEXT *t = (BE_TEXT *) g_list_nth_data(be_text_list, mod_num);
	return t->mod->getConfig().has("GlobalOptionFilter",option);
}

/******   end of file   ******/
