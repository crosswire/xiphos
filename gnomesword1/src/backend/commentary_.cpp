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
#include <config.h>
#endif

#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>

#include "backend/commentary_.h"

using namespace sword;

/******************************************************************************
 * static - global to this file only 
 */
static SWMgr *mgr;
static VerseKey versekey;


/******************************************************************************
 * Name
 *   backend_set_commentary_key
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   void backend_set_commentary_key(char * mod_name, char * key)	
 *
 * Description
 *   set commentary key
 *
 * Return value
 *   void
 */

void backend_set_commentary_key(char * mod_name, char * key)
{
	SWModule *mod = mgr->Modules[mod_name];
	if (mod){
		versekey = key;
		mod->SetKey(versekey);
	}
}

/******************************************************************************
 * Name
 *   backend_get_commentary_key
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   char *backend_get_commentary_key(char *mod_name)	
 *
 * Description
 *   return commentary key
 *
 * Return value
 *   char *
 */

char *backend_get_commentary_key(char * mod_name)
{
	SWModule *mod = mgr->Modules[mod_name];
	if (mod){
		char *key = (char *)mod->KeyText();
		if(key)
			return strdup(key);		
	}
	return NULL;
}

/******************************************************************************
 * Name
 *   backend_get_commentary_text
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   char *backend_get_commentary_text(char *mod_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */

char *backend_get_commentary_text(char * mod_name, char * key)
{
	SWModule *mod = mgr->Modules[mod_name];
	if (mod){
		versekey.Persist(1);
		versekey = key;
		mod->SetKey(versekey);
		return strdup((char *) mod->RenderText());		
	}
	return NULL;
}

/******************************************************************************
 * Name
 *  backend_get_book_heading
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *    char *backend_get_book_heading(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *backend_get_book_heading(char * mod_name, char * key)
{
	char newkey[256];
	char *buf;	
	
	SWModule *mod = mgr->Modules[mod_name];
	if (mod) {
		versekey = key;
		mod->SetKey(versekey);
		
		VerseKey vkey;
		vkey = key; 
		const char *book = 
			vkey.books[vkey.Testament() - 1][vkey.Book() - 1].name;
		
		sprintf(newkey,"%s 0:0",book);
		
		versekey.AutoNormalize(0);
		versekey = newkey;
		mod->SetKey(versekey);
		mod->Error();	
		buf = (char *) mod->RenderText();
		versekey.AutoNormalize(1);
		return strdup(buf);
	}
	return NULL;
}

/******************************************************************************
 * Name
 *   backend_get_chap_heading
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   char *backend_get_chap_heading(char * mod_name, char * key)
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *backend_get_chap_heading(char * mod_name, char * key)
{
	char newkey[256];
	char *buf;	
	
	SWModule *mod = mgr->Modules[mod_name];
	if (mod) {
		versekey = key;
		mod->SetKey(versekey);
		
		VerseKey vkey;
		vkey = key; 
		const char *book = 
			vkey.books[vkey.Testament() - 1][vkey.Book() - 1].name;
		int chapter = vkey.Chapter();
		sprintf(newkey,"%s %d:0",book,chapter);
		
		versekey.AutoNormalize(0);
		versekey = newkey;
		mod->SetKey(versekey);
		mod->Error();	
		buf = (char *) mod->RenderText();
		versekey.AutoNormalize(1);
		return strdup(buf);
	}
	return NULL;
}

/******************************************************************************
 * Name
 *  backend_nav_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *    char* backend_nav_commentary(char * mod_name, gint direction)	
 *
 * Description
 *    navigate the current commentary module and return key
 *    return value must be freed by calling function
 *
 * Return value
 *   char *
 */
 
char* backend_nav_commentary(char * mod_name, int direction)
{
	SWModule *mod = mgr->Modules[mod_name];
	if (mod){
		switch (direction) {
		case 0:
			(*mod)--;
			break;
		case 1:
			(*mod)++;
			break;
		}	
		mod->Error();
		return strdup((char *) mod->KeyText());
	}
	return NULL;
}

/******************************************************************************
 * Name
 *    backend_setup_commentary
 *
 * Synopsis
 *   #include "commentary_.h"
 *
 *   void backend_setup_commentary(void)	
 *
 * Description
 *   set up sword commentary support
 *    
 *
 * Return value
 *   void
 */

void backend_setup_commentary(void)
{	
	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
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
}

/******   end of file   ******/
