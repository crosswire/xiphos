/*
 * GnomeSword Bible Study Tool
 * mgr.hh - SHORT DESCRIPTION
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

#ifndef _MGR_HH_
#define _MGR_HH_

#ifdef __cplusplus
extern "C" {
#endif

void backend_init_main_mgr(void);
void backend_delete_main_mgr(void);	
void backend_new_module_mgr(void);
void backend_delete_module_mgr(void);
 
void backend_set_global_option_iterator(void);
char *backend_get_next_global_option(void);	

	
#ifdef __cplusplus
}
#endif

#endif

