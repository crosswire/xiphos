/*
 * GnomeSword Bible Study Tool
 * commentary_.h - support for Sword commentary modules
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

#ifndef __COMMENTARY__H_
#define __COMMENTARY__H_

#ifdef __cplusplus
extern "C" {
#endif
	
void backend_set_commentary_key(char * mod_name, char * key);	
char *backend_get_commentary_key(char *mod_name);	
char *backend_get_commentary_text(char *mod_name, char *key);	
char *backend_get_book_heading(char * mod_name, char * key);
char *backend_get_chap_heading(char * mod_name, char * key);
char *backend_nav_commentary(char * mod_name, int direction);
void backend_setup_commentary(void);
void backend_shutdown_commentary(void);

#ifdef __cplusplus
}
#endif

#endif

