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

#include <glib-2.0/glib.h>

/* managers */
#define TEXT_MGR 0 
#define COMM_MGR 1
#define DICT_MGR 2
#define GBS_MGR 3
#define MAIN_MGR 4
#define SEARCH_MGR 5
#define MODULE_MGR 6
#define PERCOM_MGR 7
#define INTER_MGR 8 
#define DISPLAY_MGR 9 	


	
void backend_init_managers(void);
void backend_delete_managers(void);	
void backend_init_main_mgr(void);
void backend_delete_main_mgr(void);	
void backend_new_module_mgr(void);
void backend_delete_module_mgr(void);
 
void backend_set_global_option_iterator(void);
char *backend_get_path_to_mods(void);	

void backend_set_global_option(int manager, char *option, char *yesno);

void backend_set_module_unlocked(char *mod_name, char *key);	
//void backend_get_global_options_list(GList * list);
void backend_setup_display_mgr(void);

#ifdef __cplusplus
}
#endif

#endif
