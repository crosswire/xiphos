
/*
 * GnomeSword Bible Study Tool
 * verselist_sb.cpp - parse verse list retrun list
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
#include <swmodule.h>
#include <versekey.h>
#include <gal/widgets/e-unicode.h>

#include "gs_gnomesword.h"
#include "sword.h"
#include "gs_html.h"
#include "display.h"
#include "gs_shortcutbar.h"
#include "verselist.h"
#include "support.h"


/***********************************************************************************************
 externals
***********************************************************************************************/

/***********************************************************************************************
 static
***********************************************************************************************/
static SWDisplay 
	* entry_display,	/* to display modules in verselist dialog */
	* chapter_display;
static SWMgr * mgr;
static SWModule * mod;		/* module for verselist dialog */

/******************************************************************************
 * Name
 *   backend_get_verse_list
 *
 * Synopsis
 *   #include "verselist.h"
 *
 *   GList * backend_get_verse_list(gchar *module_name, gchar *vlist, SETTINGS *s)	
 *
 * Description
 *   get a list of verses from a string
 *
 * Return value
 *   GList *
 */
GList *backend_get_verse_list(gchar * module_name, gchar * vlist,
			      SETTINGS * s)
{
	GList *retval = NULL;
	gchar firstkey[256];
	ListKey tmp_verse_list;
	VerseKey default_verse_key;
	gint count = 0;

	for (int i = 0; i < strlen(vlist); i++) {
		if (vlist[i] == '+')
			vlist[i] = ' ';
		if (vlist[i] == ',')
			vlist[i] = ';';
	}
	tmp_verse_list =
	    default_verse_key.ParseVerseList((char *) vlist,
					     default_verse_key);

	while (!tmp_verse_list.Error()) {
		retval =
		    g_list_append(retval,
				  g_strdup((const char *)
					   tmp_verse_list));
		if (!count)
			sprintf(firstkey, "%s",
				(const char *) tmp_verse_list);
		tmp_verse_list++;
		++count;
	}
	if (count > 0) {
		ModMap::iterator it;
		it = mgr->Modules.find(module_name);	//-- iterate through the modules until we find modName - modName was passed by the callback
		if (it != mgr->Modules.end()) {	//-- if we find the module       
			mod = (*it).second;	//-- change module to new module
			mod->SetKey(firstkey);	//-- set key to the first one in the list
			mod->Display();
		}
	}
	return retval;
}

/******************************************************************************
 * Name
 *   backend_setup_verselist
 *
 * Synopsis
 *   #include "verselist.h"
 *
 *   void backend_setup_verselist(GtkWidget *html_widget, SETTINGS *s)	
 *
 * Description
 *   setup verse list display
 *
 * Return value
 *   void
 */
void backend_setup_verselist(GtkWidget * html_widget, SETTINGS * s)
{
	ModMap::iterator it;	//-- iteratior     
	SectionMap::iterator sit;	//-- iteratior

	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgrs
	mod = NULL;
	entry_display = new GtkHTMLEntryDisp(html_widget, s);
	chapter_display = new GtkHTMLChapDisp(html_widget, s);

	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
		mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			mod->Disp(chapter_display);
		} else {
			mod->Disp(entry_display);
		}
	}
}

/******************************************************************************
 * Name
 *   backend_shutdown_verselist
 *
 * Synopsis
 *   #include "verselist.h"
 *
 *   void backend_shutdown_verselist(void)	
 *
 * Description
 *   shutdown verse list display
 *
 * Return value
 *   void
 */
void backend_shutdown_verselist(void)
{
	delete mgr;
	if (entry_display)
		delete entry_display;
	if (chapter_display)
		delete chapter_display;
}

/******************************************************************************
 * Name
 *   backend_verselist_change_verse
 *
 * Synopsis
 *   #include "verselist.h"
 *
 *   void backend_verselist_change_verse(SETTINGS *s, gchar *url)	
 *
 * Description
 *   display new verse
 *
 * Return value
 *   void
 */
void backend_verselist_change_verse(SETTINGS * s, gchar * url)
{
	mod->SetKey(url);
	mod->Display();
}
