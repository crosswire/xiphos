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

#include "gs_gnomesword.h"
#include "settings.h"

const char *backend_get_percomm_key(int mod_num);
void backend_save_personal_comment(int mod_num, char * buf);
void backend_delete_personal_comment(int mod_num);
void backend_nav_percomm_module(int mod_num, int direction);
void backend_new_percomm_display(GtkWidget * html, char *mod_name,
					      SETTINGS * s);
void backend_setup_percomm(SETTINGS * s);
void backend_shutdown_percomm(void);
void backend_display_percomm(int mod_num, char *key);
	
	
#ifdef __cplusplus
}
#endif
#endif	/* __PERCOMM__H_ */
