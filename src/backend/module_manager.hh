/*
 * GnomeSword Bible Study Tool
 * module_manager.hh
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

#ifndef __MODULE_MANAGER_HH__
#define __MODULE_MANAGER_HH__

#ifdef __cplusplus
extern "C" {
#endif


GList *backend_module_mgr_list_remote_sources(void);
GList *backend_module_mgr_list_local_modules(const char *dir);
int backend_local_install_module(const char *dir, const char *mod_name);	
int backend_remote_install_module(const char *source_name, const char *mod_name);
int backend_uninstall_module(const char *mod_name);
//void backend_module_mgr_list_remote_sources(void);
GList *backend_module_mgr_remote_list_modules(const char *source_name);
int backend_module_mgr_refresh_remote_source(const char *source_name);
void backend_init_module_mgr_config(void);
void backend_init_module_mgr(const char *dir);
void backend_shut_down_module_mgr(void);


#ifdef __cplusplus
}
#endif

#endif
