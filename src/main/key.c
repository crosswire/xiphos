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
#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif



#include "backend/key.hh"

#include "main/key.h"
#include "main/sword.h"

/******************************************************************************
 * Name
 *   clear_scope
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   int clear_scope(void)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int clear_scope(void)
{
	return backend_clear_scope();
}


/******************************************************************************
 * Name
 *   clear_search_list
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   int clear_search_list(void)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int clear_search_list(void)
{
	return backend_clear_search_list();
}


/******************************************************************************
 * Name
 *   set_scope2last_search
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   int set_scope2last_search(void)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int set_scope2last_search(void)
{
	return backend_set_scope2last_search();
}


/******************************************************************************
 * Name
 *   set_scope2range
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   void set_scope2range(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void set_scope2range(void)
{
	backend_set_scope2range();
}


/******************************************************************************
 * Name
 *    clear_results
 *
 * Synopsis
 *   #include "main/key.h"
 *
 *   int clear_results(void)
 *
 * Description
 *   clears the search results list
 *
 * Return value
 *   int
 */

int clear_results(void)
{
	return backend_clear_results();	
}



void set_results_position(char pos)
{
	backend_set_results_position(pos);
}

/******************************************************************************
 * Name
 *    get_next_result_key
 *
 * Synopsis
 *   #include "main/key.h"
 *
 *   const char *get_next_result_key(void)
 *
 * Description
 *   returns the next key from the last search's 
 *   results list (ListKey results)
 *
 * Return value
 *   const char *
 */

const char *get_next_result_key(void)
{
	
	return backend_get_next_result_key();	
}


/******************************************************************************
 * Name
 *    set_range
 *
 * Synopsis
 *   #include "main/key.h"
 *
 *   int set_range(char * list)
 *
 * Description
 *   set search range by passing a list 
 *   ie "Mat-John;James" to ParseVerseList
 *   return the list count (number of elements in list)
 *
 * Return value
 *   int
 */

int set_range(char * list)
{
	
	return backend_set_range(list);
}


/******************************************************************************
 * Name
 *    start_parse_verse_list
 *
 * Synopsis
 *   #include "main/key.h"
 *
 *   int start_parse_verse_list(char * list)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int start_parse_verse_list(const char * list, char * current_key)
{
	
	return backend_start_parse_verse_list(list, current_key);
}

/******************************************************************************
 * Name
 *    start_parse_range_list
 *
 * Synopsis
 *   #include "main/key.h"
 *
 *   int start_parse_verse_list(char * list)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int start_parse_range_list(const char * list)
{
	
	return backend_start_parse_range_list(list);
}


/******************************************************************************
 * Name
 *   get_valid_key
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *get_valid_key(char *key)	
 *
 * Description
 *    returns a valid Bible reference - must be freed by calling function
 *
 * Return value
 *   char *
 */

char *get_valid_key(const char *key)
{
	gchar *buf;
	gsize bytes_read;
	gsize bytes_written;
	GError **error;	
	char *mykey = g_convert(key,
			     -1,
			     OLD_CODESET,
			     UTF_8,
			     &bytes_read,
			     &bytes_written,
			     error);
	buf = g_convert((char *) backend_get_valid_key(mykey),
			     -1,
			     UTF_8,
			     OLD_CODESET,
			     &bytes_read,
			     &bytes_written,
			     error);
	g_free(mykey);	
	return  buf;
}


/******************************************************************************
 * Name
 *  get_book_from_key
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   const char *get_book_from_key(char *key)	
 *
 * Description
 *    
 *
 * Return value
 *   const char *
 */ 

char *get_book_from_key(const char *key)
{
	gchar *buf;
	gsize bytes_read;
	gsize bytes_written;
	GError **error;	
	char *mykey = g_convert(key,
			     -1,
			     OLD_CODESET,
			     UTF_8,
			     &bytes_read,
			     &bytes_written,
			     error);
	buf = 
		g_convert((char *) backend_get_book_from_key(mykey),
			     -1,
			     UTF_8,
			     OLD_CODESET,
			     &bytes_read,
			     &bytes_written,
			     error);
	g_free(mykey);
	
	return buf;
}


/******************************************************************************
 * Name
 *  get_chapter_from_key
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   int get_chapter_from_key(char *key)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int get_chapter_from_key(const char *key)
{
	gint retval = 0;
	gsize bytes_read;
	gsize bytes_written;
	GError **error;	
	char *mykey = g_convert(key,
			     -1,
			     "iso8859-1",
			     "UTF-8",
			     &bytes_read,
			     &bytes_written,
			     error);
	retval = backend_get_chapter_from_key(mykey);
	g_free(mykey);
	return retval;
}


/******************************************************************************
 * Name
 *  get_verse_from_key
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   int get_verse_from_key(char *key)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int get_verse_from_key(const char *key)
{
	gint retval = 0;
	gsize bytes_read;
	gsize bytes_written;
	GError **error;	
	char *mykey = g_convert(key,
			     -1,
			     "iso8859-1",
			     "UTF-8",
			     &bytes_read,
			     &bytes_written,
			     error);
	retval = backend_get_verse_from_key(mykey);
	g_free(mykey);
	return retval;
}


/******************************************************************************
 * Name
 *    get_next_verse_list_element
 *
 * Synopsis
 *   #include "main/key.h"
 *
 *   char *get_next_verse_list_element(int count)
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *get_next_verse_list_element(int count)
{
	
	return backend_get_next_verse_list_element(count);
}


/** Returns the number of chapters for the given book. */
const unsigned int chapter_count(const char *key) 
{
	return backend_chapter_count(key);
}

/** Returns the number of verses  for the given chapter. */
const unsigned int verse_count(const char *key) 
{
	return backend_verse_count(key);
}
