/*
 * GnomeSword Bible Study Tool
 * dictlex_.h - SHORT DESCRIPTION
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

#ifndef __DICTLEX__H_
#define __DICTLEX__H_

#ifdef __cplusplus
extern "C" {
#endif
	
char *backend_get_first_dictlex_key(void);
char *backend_get_next_dictlex_key(void);	
int backend_set_dictlex_module(char * mod_name);
int backend_set_dictlex_key(char * key);
char *backend_get_dictlex_key(int direction);
char *backend_get_dictlex_text(char * mod_name, char * key);
	
void backend_new_dictlex_display(GtkWidget *html, int mod_num);
void backend_setup_dictlex(void);
void backend_shutdown_dictlex(void);
//void backend_display_dictlex(int mod_num, char *key);
void backend_dictentryTextChangedDL(char *modname, char *mytext);	
	
#ifdef __cplusplus
}
#endif

#endif

