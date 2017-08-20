/*
 * Xiphos Bible Study Tool
 * module_manager.hh
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

#ifndef __MODULE_MANAGER_HH__
#define __MODULE_MANAGER_HH__

#ifdef __cplusplus

#include <swmgr.h>
#include <swmodule.h>
#include <installmgr.h>
#include <filemgr.h>

#include <remotetrans.h>

using namespace std;
using namespace sword;

class GSStatusReporter : public StatusReporter
{
	virtual void statusUpdate(double dltotal, double dlnow);
	virtual void preStatus(long totalBytes, long completedBytes, const char *message);
};

class ModuleManager
{
	SWMgr *mgr;
	SWMgr *list_mgr;
	InstallMgr *installMgr;
	ModMap::iterator it;
	ModMap::iterator end;
	GSStatusReporter *statusReporter;
	SWBuf baseDir;

      public:
	ModuleManager();
	~ModuleManager();
	void init_ModuleManager(void);
	SWBuf ftp_password();
};

inline SWBuf ftp_password()
{
	return
#ifdef WIN32
		(SWBuf) "W"
#else
		(SWBuf) "L"
#endif
		+ (SWBuf) VERSION
		+ (SWBuf) "@xiphos.org";
}

extern "C" {

void backend_module_mgr_list_local_modules_init(bool for_install);

#endif
#include "main/mod_mgr.h"

char *backend_mod_mgr_get_config_entry(char *module_name, const char *entry);
void backend_mod_mgr_init_language_map(void);
MOD_MGR *backend_module_mgr_get_next_module(void);
GList *backend_module_mgr_list_remote_sources(void);
GList *backend_module_mgr_list_local_sources(void);
int backend_local_install_module(const char *destdir,
				 const char *srcdir,
				 const char *mod_name);
int backend_remote_install_module(const char *destdir,
				  const char *source_name,
				  const char *mod_name);
int backend_uninstall_module(const char *dir, const char *mod_name);
void backend_module_mgr_remote_list_modules_init(const char *source_name);
int backend_module_mgr_refresh_remote_source(const char *source_name);
void backend_init_module_mgr(const char *dir,
			     gboolean augment,
			     gboolean regular);
void backend_shut_down_module_mgr(void);
void backend_terminate_module_mgr(void);
void backend_init_module_mgr_config(void);
int backend_init_module_mgr_config_extras(void);
void backend_module_mgr_clear_config(void);
void backend_module_mgr_reread_config(void);
void backend_module_mgr_add_source(const char *vtype,
				   const char *type,
				   const char *caption,
				   const char *source,
				   const char *directory,
				   const char *user,
				   const char *pass,
				   const char *uid);

#ifdef __cplusplus
}
#endif

#endif
