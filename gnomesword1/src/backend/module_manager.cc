/*
 * GnomeSword Bible Study Tool
 * module_manager.cc 
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#include <gnome.h>
#else
#include <glib-1.2/glib.h>
#endif
#include <swmgr.h>
#include <installmgr.h>
#include <filemgr.h>
#include <iostream>
#include <string>
#include "module_manager.hh"
#include "backend/module.hh"
#include "backend/mgr.hh"
#include "main/mod_mgr.h"
#include "main/sword.h"

using namespace sword;
using std::cout;
using std::cin;
using std::string;

SWMgr *mgr;
InstallMgr *installMgr;

void InstallMgr::preDownloadStatus(long totalBytes, long completedBytes, 
				   const char *message)
{
	update_install_status(totalBytes, completedBytes, message);
}

void InstallMgr::statusUpdate(double dltotal, double dlnow) 
{
	if (!dltotal)
		return;	
	double filefraction  = (dlnow / dltotal);
	update_install_progress(filefraction);
}

static GList *module_mgr_list_modules(SWMgr * mgr)
{
	MOD_MGR *mod_info;
	const char *buf;
	gsize bytes_read;
	gsize bytes_written;
	GError **error;
	GList *list = NULL;
	
	backend_delete_main_mgr();
	backend_init_main_mgr();
	
	SWModule *module;
	for (ModMap::iterator it = mgr->Modules.begin();
	     it != mgr->Modules.end(); it++) {
		module = it->second;
		mod_info = g_new(MOD_MGR, 1);
		mod_info->name = g_convert(module->Name(),
					   -1,
					   UTF_8,
					   OLD_CODESET,
					   &bytes_read,
					   &bytes_written, error);
		if (mod_info->name) {
			mod_info->language = (module->Lang())?
			    backend_get_language_map(module->
						     Lang()) :
			    "unknown";
			mod_info->type = g_convert(module->Type(),
						   -1,
						   UTF_8,
						   OLD_CODESET,
						   &bytes_read,
						   &bytes_written,
						   error);
			buf =
			    (module->
			     getConfigEntry("Version")) ? module->
			    getConfigEntry("Version") : " ";
			mod_info->new_version =
			    g_convert(buf, -1, UTF_8, OLD_CODESET,
				      &bytes_read, &bytes_written,
				      error);
			mod_info->old_version =
			    backend_get_module_version(mod_info->name);
			mod_info->installed =
			    backend_check_for_module(mod_info->name);
			mod_info->description = module->Description();
			list =
			    g_list_append(list, (MOD_MGR *) mod_info);
		}
	}
	return list;
}

int backend_uninstall_module(const char *modName)
{
	SWModule *module;
	ModMap::iterator it = mgr->Modules.find(modName);
	if (it == mgr->Modules.end()) {
		printf("Couldn't find module [%s] to remove\n",
			modName);
		return 0;
	}
	module = it->second;
	installMgr->removeModule(mgr, module->Name());
	return 1;
}


void backend_remote_install_module(const char *sourceName, const char *modName)
{
	InstallSourceMap::iterator source =
	    installMgr->sources.find(sourceName);
	if (source == installMgr->sources.end()) {
		fprintf(stderr, "Couldn't find remote source [%s]\n",
			sourceName);
		backend_shut_down_module_mgr();
	}
	InstallSource *is = source->second;
	SWMgr *rmgr = is->getMgr();
	SWModule *module;
	ModMap::iterator it = rmgr->Modules.find(modName);
	if (it == rmgr->Modules.end()) {
		fprintf(stderr,
			"Remote source [%s] does not make available module [%s]\n",
			sourceName, modName);
		backend_shut_down_module_mgr();
	}
	module = it->second;
	installMgr->installModule(mgr, 0, module->Name(), is);
}


void backend_local_install_module(const char *dir, const char *mod_name)
{
	SWMgr lmgr(dir);
	SWModule *module;
	ModMap::iterator it = lmgr.Modules.find(mod_name);
	if (it == lmgr.Modules.end()) {
		fprintf(stderr,
			"Module [%s] not available at path [%s]\n",
			mod_name, dir);
		backend_shut_down_module_mgr();
	}
	module = it->second;
	installMgr->installModule(mgr, dir, module->Name());
}


GList *backend_module_mgr_list_remote_sources(void)
{
	MOD_MGR_SOURCE *mms;
	GList *retval = NULL;

	for (InstallSourceMap::iterator it =
	     installMgr->sources.begin();
	     it != installMgr->sources.end(); it++) {
		mms = g_new(MOD_MGR_SOURCE, 1);
		mms->caption = it->second->caption;
		mms->type = it->second->type;
		mms->source = it->second->source;
		mms->directory = it->second->directory;
		retval = g_list_append(retval, (MOD_MGR_SOURCE *) mms);
	}
	return retval;
}

GList *backend_module_mgr_remote_list_modules(const char *sourceName)
{
	InstallSourceMap::iterator source =
	    installMgr->sources.find(sourceName);
	if (source == installMgr->sources.end()) {
		fprintf(stderr, "Couldn't find remote source [%s]\n",
			sourceName);
		backend_shut_down_module_mgr();
	}
	return module_mgr_list_modules(source->second->getMgr());
}

GList *backend_module_mgr_list_local_modules(const char *dir)
{

	SWMgr mgr(dir);
	return module_mgr_list_modules(&mgr);
}

void backend_module_mgr_refresh_remote_source(const char *sourceName)
{
	InstallSourceMap::iterator source =
	    installMgr->sources.find(sourceName);
	if (source == installMgr->sources.end()) {
		fprintf(stderr, "Couldn't find remote source [%s]\n",
			sourceName);
		backend_shut_down_module_mgr();
	}
	installMgr->refreshRemoteSource(source->second);
}

void backend_init_module_mgr_config(void)
{
	char *envhomedir = getenv("HOME");
	SWBuf confPath = (envhomedir) ? envhomedir : ".";
	confPath += "/.sword/InstallMgr/InstallMgr.conf";
	FileMgr::createParent(confPath.c_str());
	remove(confPath.c_str());

	InstallSource is("FTP");
	is.caption = "crosswire";
	is.source = "ftp.crosswire.org";
	is.directory = "/pub/sword/raw";

	SWConfig config(confPath.c_str());
	config["General"]["PassiveFTF"] = "true";
	config["Sources"]["FTPSource"] = is.getConfEnt();
	config["Sources"]["Local"] = "/mnt/cdrom";
	config.Save();
}

void backend_init_module_mgr(const char *dir)
{
	if(dir)
		mgr = new SWMgr(dir);
	else
		mgr = new SWMgr();
	char *envhomedir = getenv("HOME");
	SWBuf baseDir = (envhomedir) ? envhomedir : ".";
	baseDir += "/.sword/InstallMgr";
	installMgr = new InstallMgr(baseDir);
}


void backend_shut_down_module_mgr(void)
{
	delete installMgr;
	delete mgr;
}
