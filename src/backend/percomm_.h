/*
 * GnomeSword Bible Study Tool
 * percomm_.h - support for Sword personal commentary modules
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

#ifndef __PERCOMM__H_
#define __PERCOMM__H_

#ifdef __cplusplus
extern "C" {
#endif
	
char *backend_get_percomm_text(char * key);	
void backend_set_percomm_key(char * key);	
void backend_change_percomm_module(char * mod_name);
//char *backend_get_percomm_key(void);
void backend_save_personal_comment(char * note);
void backend_delete_personal_comment(void);
void backend_setup_percomm(void);
void backend_shutdown_percomm(void);

#ifdef __cplusplus
}
#endif

#endif

