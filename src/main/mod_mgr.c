/*
 * GnomeSword Bible Study Tool
 * mod_mgr.c 
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#include <gnome.h>
#else
#include <glib-1.2/glib.h>
#endif


#include "main/mod_mgr.h"

#include "backend/module_manager.hh"



/******************************************************************************
 * Name
 *   update_install_status
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   void update_install_status(long total, long done, const char *message)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void update_install_status(long total, long done, const char *message)
{
	gui_update_install_status(total, done, message);
}


/******************************************************************************
 * Name
 *   update_install_progress
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   void update_install_progress(double fraction)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void update_install_progress(double fraction)
{
	gui_update_install_progressbar(fraction);
}


/******************************************************************************
 * Name
 *   mod_mgr_list_remote_sources
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   GList *mod_mgr_list_remote_sources(void)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

GList *mod_mgr_list_remote_sources(void)
{
	return backend_module_mgr_list_remote_sources();
}


/******************************************************************************
 * Name
 *   mod_mgr_list_local_sources
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   GList *mod_mgr_list_local_sources(void)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

GList *mod_mgr_list_local_sources(void)
{
	return backend_module_mgr_list_local_sources();
}


/******************************************************************************
 * Name
 *   mod_mgr_uninstall
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   int mod_mgr_uninstall(const char *mod_name)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int mod_mgr_uninstall(const char *dir, const char *mod_name)
{
	return backend_uninstall_module(dir, mod_name);
}


/******************************************************************************
 * Name
 *  mod_mgr_remote_install 
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   int mod_mgr_remote_install(const char *source_name, const char *mod_name)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int mod_mgr_remote_install(const char *source_name, const char *mod_name)
{
	return backend_remote_install_module(source_name, mod_name);
}


/******************************************************************************
 * Name
 *   mod_mgr_local_install_module
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   int mod_mgr_local_install_module(const char *dir, const char *mod_name)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int mod_mgr_local_install_module(const char *dir, const char *mod_name)
{
	return backend_local_install_module(dir, mod_name);
}


/******************************************************************************
 * Name
 *   mod_mgr_list_local_modules
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   GList *mod_mgr_list_local_modules(const char *dir)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

GList *mod_mgr_list_local_modules(const char *dir)
{
	GList *list = NULL;	
	MOD_MGR *mod_info;
	
	backend_delete_main_mgr();
	backend_init_main_mgr();
	backend_init_module_mgr(dir);
	
	backend_module_mgr_list_local_modules_init();
	while((mod_info = backend_module_mgr_get_next_module()) != NULL) {
		list = g_list_append(list, (MOD_MGR*) mod_info);
	}
	return list;
}


/******************************************************************************
 * Name
 *   mod_mgr_remote_list_modules
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   GList *mod_mgr_remote_list_modules(const char *source_name)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

GList *mod_mgr_remote_list_modules(const char *source_name)
{
	GList *list = NULL;	
	MOD_MGR *mod_info;
	
	backend_delete_main_mgr();
	backend_init_main_mgr();
	
	backend_module_mgr_remote_list_modules_init(source_name);
	while((mod_info = backend_module_mgr_get_next_module()) != NULL) {
		list = g_list_append(list, (MOD_MGR*) mod_info);
	}
	return list;
}


/******************************************************************************
 * Name
 *   mod_mgr_refresh_remote_source
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   int mod_mgr_refresh_remote_source(const char *source_name)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int mod_mgr_refresh_remote_source(const char *source_name)
{
	return backend_module_mgr_refresh_remote_source(source_name);
}


/******************************************************************************
 * Name
 *   mod_mgr_init_config
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   void mod_mgr_init_config()
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void mod_mgr_init_config()
{
	backend_init_module_mgr_config();
}


/******************************************************************************
 * Name
 *   mod_mgr_init
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   void mod_mgr_init(const char *dir)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void mod_mgr_init(const char *dir)
{
	backend_init_module_mgr(dir);
}


/******************************************************************************
 * Name
 *   mod_mgr_shut_down
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   void mod_mgr_shut_down(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void mod_mgr_shut_down(void)
{
	backend_shut_down_module_mgr();
}
