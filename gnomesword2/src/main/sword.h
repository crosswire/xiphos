/*
 * GnomeSword Bible Study Tool
 * sword.h - glue
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

#ifndef _SWORD_H__
#define _SWORD_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include <glib-1.2/glib.h>

#include "main/module.h"
#include "main/key.h"	
#include "main/configs.h"	
/*** these defs need to be seen by gui and backend ***/
	
/* module types */
#define TEXT_TYPE 0 
#define COMMENTARY_TYPE 1 
#define DICTIONARY_TYPE 2
#define BOOK_TYPE 3
       
/* these strings are not seen by users */
#define TEXT_MODS "Biblical Texts"
#define COMM_MODS "Commentaries"
#define DICT_MODS "Lexicons / Dictionaries"
#define BOOK_MODS "Generic Books"

/*** end defs ***/


/*** function prototypes ***/

int gbs_treekey_get_parent(unsigned long offset);
char *module_name_from_description(char *description);
const char *get_sword_version(void);
char *get_search_results_text(char * mod_name, char * key);
char *get_path_to_mods(void);
void init_sword(void);
void shutdown_backend(void);
char *get_text_from_offset(char * module_name, unsigned long offset);
int set_module(int manager, char * module_name);


#ifdef __cplusplus
}
#endif

#endif
