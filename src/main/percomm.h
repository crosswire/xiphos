/*
 * GnomeSword Bible Study Tool
 * percomm.h - support for personal commentary modules
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

#ifndef __PERCOMM_H_
#define __PERCOMM_H_

#ifdef __cplusplus
extern "C" {
#endif
int save_percomm_options(char * mod_name, char * option, 
				    int choice);
int load_percomm_options(char * modName, char * option);
char *get_percomm_key(void);
char *get_percomm_text(char * key);	
void set_percomm_key(char * key);
void change_percomm_module(char * mod_name);
void save_percomm_note(char *note);
void delete_percomm_note(void);
int is_personal_comment(char * mod_name);
#ifdef __cplusplus
}
#endif

#endif

