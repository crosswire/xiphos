/*
 * GnomeSword Bible Study Tool
 * key.hh - SHORT DESCRIPTION
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

#ifndef _KEY_HH_
#define _KEY_HH_

#ifdef __cplusplus
extern "C" {
#endif
#include <gnome.h>

int backend_clear_scope(void);	
int backend_clear_search_list(void);	
int backend_clear_results(void);
int backend_set_scope2last_search(void);
void backend_set_scope2range(void);
void backend_set_results_position(char pos);	
const char *backend_get_next_result_key(void);
int backend_set_range(char * list);
int backend_start_parse_verse_list(const char * list, char * current_key);
int backend_start_parse_range_list(const char * list);
char *backend_get_next_verse_list_element(int count);

char *backend_get_valid_key(const char *key);
char *backend_get_book_from_key(const char *key);
int backend_get_chapter_from_key(const char *key);
int backend_get_verse_from_key(const char *key);
const unsigned int backend_chapter_count(const char *key);
const unsigned int backend_verse_count(const char *key);
GList *backend_get_books_of_bible(int testament);

#ifdef __cplusplus
}
#endif
#endif
