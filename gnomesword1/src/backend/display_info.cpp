/*
 * GnomeSword Bible Study Tool
 * display_info.cpp - About dialogs for GnomeSword
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

#include <swmodule.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <versekey.h>

#include "gs_gnomesword.h"
#include "display_info.h"
#include "display.h"
#include "settings.h"

/***********************************************************************
 * for dispaly dict/lex information when a link is clicked in a view
 * Text or Commentary dialog
 */

static SWDisplay *display;	/* to display modules in view dialog */
static SWMgr *mgr;		/* sword mgr for view comm dialog */
static SWModule *mod;		/* module for view text dialog */

/******************************************************************************
 * backend_dispaly_info_load_modudle - load a module into the 
 * information dialog
 ******************************************************************************/
void backend_dispaly_info_load_modudle(gchar * modName, gchar * newkey)
{
	ModMap::iterator it;

	it = mgr->Modules.find(modName);	//-- find module we want to use
	if (it != mgr->Modules.end()) {

		mod = (*it).second;	//-- set mod to new choice
		mod->SetKey(newkey);
		mod->Display();	//-- display new dict
	}
}

/**************************************************************************
 * backend_display_info_setup - set up the sword stuff for 
 * the information dialog
 **************************************************************************/
void backend_display_info_setup(GtkWidget * text)
{
	ModMap::iterator it;	//-- iteratior     
	SectionMap::iterator sit;	//-- iteratior        

	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	mod = NULL;
	display = new GtkHTMLEntryDisp(text, &settings);
	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
		if (!strcmp
		    ((*it).second->Type(), "Lexicons / Dictionaries")) {
			mod = (*it).second;
			mod->Disp(display);
		}
	}
}

/******************************************************************************
 *backend_dispaly_info_shutdown - called when info dialog is destroyed
 ******************************************************************************/
void backend_dispaly_info_shutdown(void)
{
	delete mgr;
	if (display)
		delete display;
}

/******************************************************************************
 *backend_dispaly_info_change_key - find new key
 ******************************************************************************/
void backend_dispaly_info_change_key(gchar * newkey)
{
	mod->SetKey(newkey);
	mod->Display();
}

/******  end of file  ******/
