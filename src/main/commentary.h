/*
 * GnomeSword Bible Study Tool
 * commentary.h - glue for commentary modules and commentary dialog
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

#ifndef __GS_COMMENTARY_H_
#define __GS_COMMENTARY_H_

#ifdef __cplusplus
extern "C" {
#endif
	
int main_save_module_options_comm(char * mod_name, char * option, int choice);	
//void main_set_commentary_key(char * mod_name, char * key);
//char *get_commentary_key(char *mod_name);	
//char *get_commentary_text(char * mod_name, char * key);
//char *get_book_heading(char * mod_name, char * key);
//char *get_chapter_heading(char * mod_name, char * key);
//char *navigate_commentary(char * mod_name, int direction);
	
#ifdef __cplusplus
}
#endif

#endif
