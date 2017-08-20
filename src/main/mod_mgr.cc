/*
 * Xiphos Bible Study Tool
 * mod_mgr.cc
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <swmgr.h>
#include <swmodule.h>
#include <localemgr.h>
#include <swlocale.h>
#include <swversion.h>
#include "main/mod_mgr.h"
#include "main/sword.h"
#include "main/parallel_view.h"

#include "gui/mod_mgr.h"

#include "backend/module_manager.hh"
#include "backend/sword_main.hh"

int main_module_mgr_index_mod(char *module_name)
{
	return backend->do_module_index(module_name);
}

int main_module_mgr_delete_index_mod(char *module_name)
{
	return backend->do_module_delete_index(module_name);
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

	// parallel backend needs to know, too.
	main_delete_parallel_view();
	main_init_parallel_view();
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

void update_install_status(long total,
			   long done,
			   const char *message)
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

void main_index_percent_update(char percent,
			       void *userData)
{
	char maxHashes = *((char *)userData);
	char buf[80];
	static char printed = 0;

	/* update search dialog progress */
	while ((((float)percent) / 100) * maxHashes > printed) {
		sprintf(buf, "%f", (((float)percent) / 100));
		float num = (float)percent / 100;
		gui_update_install_progressbar((gdouble)num);
		printed++;
	}
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

int mod_mgr_remote_install(const char *dir,
			   const char *source_name,
			   const char *mod_name)
{
	return backend_remote_install_module(dir, source_name, mod_name);
}

/******************************************************************************
 * Name
 *   mod_mgr_local_install_module
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   int mod_mgr_local_install_module(const char *destdir, const char *srcdir, const char *mod_name)
 *
 * Description
 *
 *
 * Return value
 *   int
 */

int mod_mgr_local_install_module(const char *destdir,
				 const char *srcdir,
				 const char *mod_name)
{
	return backend_local_install_module(destdir, srcdir, mod_name);
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

GList *mod_mgr_list_local_modules(const char *dir,
				  gboolean augment)
{
	GList *list = NULL;
	MOD_MGR *mod_info;
	SWVersion version = SWVersion::currentVersion;

	backend_init_module_mgr(dir, augment, augment);

	backend_module_mgr_list_local_modules_init(!augment);
	while ((mod_info = backend_module_mgr_get_next_module()) != NULL) {
		if (version.compare(mod_info->min_version) < 0) {
			/* Sword is too old for this new module. */
			g_free(mod_info->name);
			g_free(mod_info->about);
			g_free(mod_info->abbreviation);
			g_free(mod_info->type);
			g_free(mod_info->new_version);
			g_free(mod_info->old_version);
			g_free(mod_info->min_version);
			g_free(mod_info->installsize);
			g_free(mod_info);
		} else {
			list = g_list_append(list, (MOD_MGR *)mod_info);
		}
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
	SWVersion version = SWVersion::currentVersion;

	backend_module_mgr_remote_list_modules_init(source_name);
	while ((mod_info = backend_module_mgr_get_next_module()) != NULL) {
		if (version.compare(mod_info->min_version) < 0) {
			/* Sword is too old for this new module. */
			g_free(mod_info->name);
			g_free(mod_info->about);
			g_free(mod_info->abbreviation);
			g_free(mod_info->type);
			g_free(mod_info->new_version);
			g_free(mod_info->old_version);
			g_free(mod_info->min_version);
			g_free(mod_info->installsize);
			g_free(mod_info);
		} else {
			list = g_list_append(list, (MOD_MGR *)mod_info);
		}
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

int mod_mgr_init_config_extras()
{
	return backend_init_module_mgr_config_extras();
}

void mod_mgr_clear_config(void)
{
	backend_module_mgr_clear_config();
}

void mod_mgr_reread_config(void)
{
	backend_module_mgr_reread_config();
}

void mod_mgr_add_source(const char *vtype,
			const char *type,
			const char *caption,
			const char *source,
			const char *directory,
			const char *user,
			const char *pass,
			const char *uid)
{
	backend_module_mgr_add_source(vtype,
				      type,
				      caption,
				      source,
				      directory,
				      user,
				      pass,
				      uid);
}

/******************************************************************************
 * Name
 *   mod_mgr_init
 *
 * Synopsis
 *   #include "main/mod_mgr.hh"
 *
 *   void mod_mgr_init(const char *dir, gboolean augment)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void mod_mgr_init(const char *dir,
		  gboolean augment,
		  gboolean regular)
{
	backend_init_module_mgr(dir, augment, regular);
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
