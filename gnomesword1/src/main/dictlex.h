/*
 * GnomeSword Bible Study Tool
 * dictlex.h - dictlex glue
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

#ifndef __DICTLEX_H_
#define __DICTLEX_H_

#ifdef __cplusplus
extern "C" {
#endif

int set_dictlex_module(char * mod_name);
int set_dictlex_key(char * key);
char *get_dictlex_key(int direction);	
char *get_dictlex_text(char * mod_name, char * key);
gchar *get_first_dictlex_key(void);
gchar *get_next_dictlex_key(void);
	
#ifdef __cplusplus
}
#endif
#endif	/* __DICTLEX_H_ */
