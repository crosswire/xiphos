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

#ifndef _KEY_H_
#define _KEY_H_

#ifdef __cplusplus
extern "C" {
#endif



const char *get_next_result_key(void);
int set_range(char * list);
int start_parse_verse_list(char * list);
void set_results_position(char pos);
char *get_next_verse_list_element(int count);
int clear_scope(void);	
int clear_search_list(void);
int set_scope2last_search(void);
void set_scope2range(void);
char *get_valid_key(const char *key);
const unsigned int chapter_count(char *key);
const unsigned int verse_count(char *key);
const char *get_book_from_key(char *key);
int get_chapter_from_key(char *key);
int get_verse_from_key(char *key);	
	

#ifdef __cplusplus
}
#endif
#endif
