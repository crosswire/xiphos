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
#include <config.h>
#endif

#include <gnome.h>

#include <swmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <markupfiltmgr.h>

#include "backend/bibletext_view_.h"
#include "backend/sword.h"
#include "backend/display.h"

#include "main/settings.h"

static SWMgr *mgr;		/* sword mgr for view text dialog */


/******************************************************************************
 * Name
 *   backend_setup_viewtext
 *
 * Synopsis
 *   #include "viewtext.h"
 *
 *   void backend_setup_viewtext(GtkWidget * text)	
 *
 * Description
 *   setup the viewtext sword display
 *
 * Return value
 *   void
 */

void backend_setup_viewtext(void)
{
	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
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
void backend_set_global_options_viewtext(char * option, char * onoff)
{
	mgr->setGlobalOption(option, onoff);
}

/******************************************************************************
 * Name
 *   backend_get_viewtext_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   char *backend_get_viewtext_text(char *mod_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */

char *backend_get_viewtext_text(char *mod_name, char *key)
{
	SWModule *mod = mgr->Modules[mod_name];
	if (mod)
		mod->SetKey(key);
	else
		return NULL;
	return strdup((char *) mod->RenderText());
}

