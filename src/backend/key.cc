/*
 * GnomeSword Bible Study Tool
 * key.cc - SHORT DESCRIPTION
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif

#include <swmodule.h>

#include <versekey.h>
#include <regex.h>
#include <pthread.h>
#include <string.h>

#include "backend/key.hh"
//#include "backend/module.hh"
#include "backend/sword.h"
#include "backend/sword_defs.h"

#include "main/settings.h"
#include "main/search.h"

using std::string;
using std::map;
using std::list;
using namespace sword;


ListKey results;
ListKey search_range;
ListKey	search_scope_list; //--search list for searching verses found on last search
ListKey verses;
VerseKey search_scope_bounds; //----- sets lower and upper search bounds
SWKey *current_scope;	


/******************************************************************************
 * Name
 *    backend_clear_scope
 *
 * Synopsis
 *   #include "backend/search.h"
 *
 *   int backend_clear_scope(void)
 *
 * Description
 *   clear the current scope
 *
 * Return value
 *   int
 */

int backend_clear_scope(void)
{
	current_scope = 0;	//------------ clear scope
	return 1;
}

/******************************************************************************
 * Name
 *    backend_clear_results
 *
 * Synopsis
 *   #include "backend/key.hh"
 *
 *   int backend_clear_results(void)
 *
 * Description
 *   clears the search results list
 *
 * Return value
 *   int
 */

int backend_clear_results(void)
{
	results.ClearList();
	return results.Count();	
}


/******************************************************************************
 * Name
 *    backend_clear_search_list
 *
 * Synopsis
 *   #include "backend/search.h"
 *
 *   int backend_clear_search_list(void)
 *
 * Description
 *   clear the search scope list - filled during last search
 *
 * Return value
 *   int
 */

int backend_clear_search_list(void)
{
	search_scope_list.ClearList();	//------------ clear scope search list
	return search_scope_list.Count ();
}


/******************************************************************************
 * Name
 *    backend_set_scope2last_search
 *
 * Synopsis
 *   #include "backend/search.h"
 *
 *   int backend_set_scope2last_search(void)
 *
 * Description
 *   set search scope to last search results
 *
 * Return value
 *   int (error)
 */

int backend_set_scope2last_search(void)
{
	current_scope = &search_scope_list;//-- move searchlist into current_scope
	return 1;
}


/******************************************************************************
 * Name
 *    backend_set_scope2range
 *
 * Synopsis
 *   #include "backend/search.h"
 *
 *   void backend_set_scope2range(void)
 *
 * Description
 *   set search scope to search range
 *
 * Return value
 *   void
 */

void backend_set_scope2range(void)
{
	current_scope = &search_range;
}


void backend_set_results_position(char pos)
{
	results.setPosition((char)pos);
}


/******************************************************************************
 * Name
 *    backend_get_next_result_key
 *
 * Synopsis
 *   #include "backend/key.hh"
 *
 *   const char *backend_get_next_result_key(void)
 *
 * Description
 *   returns the next key from the last search's 
 *   results list (ListKey results)
 *
 * Return value
 *   const char *
 */

const char *backend_get_next_result_key(void)
{
	const char *retval = NULL;
	
	while(!results.Error()) {
		retval = results.getText();
		results++;
		return retval;
	}
	return NULL;	
}


/******************************************************************************
 * Name
 *    backend_set_range
 *
 * Synopsis
 *   #include "backend/key.hh"
 *
 *   int backend_set_range(char * list)
 *
 * Description
 *   set search range by passing a list 
 *   ie "Mat-John;James" to ParseVerseList
 *   return the list count (number of elements in list)
 *
 * Return value
 *   int
 */

int backend_set_range(char * list)
{
	search_range = VerseKey().ParseVerseList(list, "", true);
	return search_range.Count ();
}

/******************************************************************************
 * Name
 *    backend_start_parse_verse_list
 *
 * Synopsis
 *   #include "backend/key.hh"
 *
 *   int backend_start_parse_verse_list(char * list)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int backend_start_parse_verse_list(char * list)
{
	VerseKey key;
	
	verses = key.ParseVerseList(list, key, true);
	return verses.Count();
}


/******************************************************************************
 * Name
 *    backend_get_next_verse_list_element
 *
 * Synopsis
 *   #include "backend/key.hh"
 *
 *   char *backend_get_next_verse_list_element(int count)
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *backend_get_next_verse_list_element(int count)
{
	char *retval = NULL;
	
	while(!verses.Error()) {	
		VerseKey *element = SWDYNAMIC_CAST(VerseKey, 
					verses.GetElement(count));
		if (element) {
			retval = strdup((char *)
				(string(element->LowerBound()) + " - " + 
				string(element->UpperBound())).c_str());
		}
		else	
			retval = strdup((const char *)
				*verses.GetElement(count));
		verses++;
		return retval;	
	}
	return NULL;
}


/******************************************************************************
 * Name
 *   backend_get_valid_key
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *backend_get_valid_key(char *key)	
 *
 * Description
 *    returns a valid Bible reference - must be freed by calling function
 *
 * Return value
 *   char *
 */

char *backend_get_valid_key(char *key)
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return strdup((char *) vkey.getText());
}

const char *backend_get_book_from_key(char *key)
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return vkey.books[vkey.Testament() - 1][vkey.Book() -
							 1].name;
}

int backend_get_chapter_from_key(char *key)
{
	unsigned char chapters;
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return vkey.Chapter();
}

/** Returns the number of chapters for the given book. */
const unsigned int backend_chapter_count(char *key) 
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	
	char testament = vkey.Testament() ;
	char book = vkey.Book();
	return (vkey.books[testament-1][book-1].chapmax);
}

/** Returns the number of verses  for the given chapter. */
const unsigned int backend_verse_count(char *key) 
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	
	char testament = vkey.Testament() ;
	char book = vkey.Book();
	int chapter = vkey.Chapter();
	
	return (vkey.books[testament-1][book-1].versemax[chapter-1]);
	/*}
	else if (book>=1 && (book - staticKey.BMAX[0]) <= (unsigned int)staticKey.BMAX[1] && hasTestament(NewTestament)) {	//is the book in the new testament?
		if (chapter <= chapterCount(book) )	//does the chapter exist?
			result = staticKey.books[1][book-1-staticKey.BMAX[0]].versemax[chapter-1];
	}*/
}

int backend_get_verse_from_key(char *key)
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return vkey.Verse();
}


/**********************************************************************
 * Name
 *   backend_get_next_book_of_bible
 *
 * Synopsis
 *   #include "backend/sword.h"
 *   
 *   char *backend_get_next_book_of_bible(void)
 *
 * Description
 *   Returns the next book of the Bible.
 *
 * Return value
 *   char*
 */

char *backend_get_next_book_of_bible(void)
{
	static VerseKey key;
	static int i = 0, j = 0;
	char *retval = NULL;
	
	while(i < 2) {
		while(j < key.BMAX[i]) {
			retval = (char *) key.books[i][j].name;
			j++;
			return retval;
		}
		j = 0;
		i++;
	}
	return NULL;
	
}

