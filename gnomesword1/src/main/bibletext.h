/*
 * GnomeSword Bible Study Tool
 * bibletext.h - support for commentary modules
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

#ifndef __BIBLETEXT_H_
#define __BIBLETEXT_H_

#ifdef __cplusplus
extern "C" {
#endif
	
void set_module_unlocked(char * mod_name, char * key);
char *get_bibletext_text(char *mod_name, char *key);
char *get_bibletext_striptext(char *mod_name, char *key);
void set_text_global_option(char * option, char * choice);
void set_text_module_global_option(char * option,
				   int choice);
const char* text_get_description(char * mod_name);
int check_for_global_option(char * mod_name, char * option);
int save_module_options(char * mod_name, char * option, 
				    int choice);
int load_module_options(char * modName, char * option);
	
char *get_interlinear_module_text(char * mod_name, char * tmpkey);
void set_interlinear_global_option(char * option, char * yesno);
	
#ifdef __cplusplus
}
#endif
#endif				/* __BIBLETEXT_H_ */
