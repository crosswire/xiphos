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

#include <swmgr.h>
#include <swmodule.h>
#include <localemgr.h>
#include <swlocale.h>
#include "main/mod_mgr.h"
#include "main/sword.h"

#include "gui/mod_mgr.h"

#include "backend/module_manager.hh"
#include "backend/sword_main.hh"


char *main_module_mgr_set_sword_locale(const char *sys_locale) {
	const char *mylocale;
	char *retval = NULL;
	char buf[32];
	int i = 0;
	SWLocale *sw_locale;
	
	if(sys_locale) {
		if(!strncmp(sys_locale,"ru_RU",5)) {
			sys_locale = "ru_RU-koi8-r";		
		} else if(!strncmp(sys_locale,"ru_RU-koi8-r",10)){
			if(strlen(sys_locale) >  12) {
				for(i = 0; i < 12; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
			
		} else if(!strncmp(sys_locale,"uk_UA-cp1251",10)){
			if(strlen(sys_locale) > 12 ) {
				for(i = 0; i < 12; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
			
		} else if(!strncmp(sys_locale,"uk_UA-koi8-u",10)){
			if(strlen(sys_locale) > 12 ) {
				for(i = 0; i < 12; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
			
		} else if(!strncmp(sys_locale,"pt_BR",5)){
			if(strlen(sys_locale) > 5 ) {
				for(i = 0; i < 5; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
			
		} else if(!strncmp(sys_locale,"en_GB",5)){
			if(strlen(sys_locale) > 5 ) {
				for(i = 0; i < 5; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
		} else {
			if(strlen(sys_locale) > 2 ) {
				buf[0] = sys_locale[0];
				buf[1] = sys_locale[1];
				buf[2] = '\0';
				sys_locale = buf;
			}
		}
		retval = strdup(sys_locale);
		LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(sys_locale);
		sw_locale = LocaleMgr::getSystemLocaleMgr()->getLocale(sys_locale);
	}
	if(sw_locale) {
		OLD_CODESET = (char*)sw_locale->getEncoding();
	} else {
		OLD_CODESET = "iso8859-1";
	}
	return retval;
}

char *main_module_mgr_get_path_to_mods(void)
{
	return backend_module_mgr_get_path_to_mods();
}


int main_module_mgr_index_mod(char * module_name)
{
	return backend->do_module_index(module_name, 0);
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
 
void main_update_module_lists(void)
{
	main_shutdown_list();
	delete backend;		
	backend = new BackEnd();
	backend->init_SWORD(0);
	main_init_lists();
}


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


void main_index_percent_update(char percent, void *userData)
{
	char maxHashes = *((char *) userData);
	float num;
	char buf[80];
	static char printed = 0;
	
	/* update search dialog progress */
	while ((((float) percent) / 100) * maxHashes > printed) {
		sprintf(buf, "%f", (((float) percent) / 100));
		num = (float) percent / 100;
		gui_update_install_progressbar((gdouble)num);
		printed++;
	}
	while (gtk_events_pending())
		gtk_main_iteration();
	printed = 0;
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
	
//	backend_delete_main_mgr();
//	backend_init_main_mgr();
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

void mod_mgr_clear_config(void)
{
	backend_module_mgr_clear_config();
}

void mod_mgr_add_source(const char * vtype, 
			const char * type, 
			const char * caption, 
			const char * source, 
			const char * directory)
{
	backend_module_mgr_add_source(vtype, 
				   type, 
				   caption, 
				   source, 
				   directory);
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
 *   mod_mgr_terminate
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   void mod_mgr_terminate(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void mod_mgr_terminate(void)
{
	backend_terminate_module_mgr();
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
