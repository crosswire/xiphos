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


/*
 *  Sword must be built with curl support for gnomesword's module manager to
 *  work as expected
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef SUSE_10_2
#include <glib.h>
#else
#include <glib-2.0/glib.h>
#endif
#include <swmgr.h>
#include <localemgr.h>
#include <swlocale.h>
#include <installmgr.h>
#include <filemgr.h>
#include <iostream>
#include <string>

#include "module_manager.hh"
#include "backend/sword_main.hh"
#include "main/mod_mgr.h"
#include "main/sword.h"

#ifdef DEBUG 
static const gchar *f_message = "backend/module_manager.cc line #%d \"%s\" = %s\n";
#endif
using namespace sword;
using std::cout;
using std::cin;
using std::string;

static SWMgr *mgr;
static SWMgr *list_mgr;
static InstallMgr *installMgr;
static ModMap::iterator it;
static ModMap::iterator end;

/******************************************************************************
 * Name
 *   preStatus
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void StatusReporter::preStatus(long totalBytes, long completedBytes, 
 *				   const char *message)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void GSStatusReporter::preStatus(long totalBytes,
				 long completedBytes, 
				 const char *message)
{
	GS_print((f_message,83,"message",message));
	update_install_status(totalBytes, completedBytes, message);
}

/******************************************************************************
 * Name
 *   statusUpdate
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void StatusReporter::statusUpdate(double dltotal, double dlnow)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void GSStatusReporter::statusUpdate(double dltotal,
				    double dlnow) 
{
	if (!dltotal)
		return;	
	double filefraction  = (dlnow / dltotal);
	update_install_progress(filefraction);
}


ModuleManager::ModuleManager() {	
	char *envhomedir = getenv("HOME");
	baseDir = (envhomedir) ? envhomedir : ".";
	baseDir += "/.sword/InstallMgr";
	statusReporter = new GSStatusReporter();
}
 
ModuleManager::~ModuleManager() {
	
}
/*
void ModuleManager::initMgr(const char *dir)
{
	if (dir)
		mgr = new SWMgr(dir);
	else
		mgr = new SWMgr();
}

void ModuleManager::deleteMgr(void)
{
	delete mgr;
}

void ModuleManager::initModuleMgr(void)
{	
	installMgr = new InstallMgr(baseDir, statusReporter);
}

void ModuleManager::deleteModuleMgr(void)
{	
	delete installMgr;
}


MOD_MGR *ModuleManager::getNextModule(void)
{
	MOD_MGR *mod_info = NULL;
	const char *buf;
	gsize bytes_read;
	gsize bytes_written;
	GError **error;
	SWModule *module;
	
	if (it != end) {
		module = it->second;
		mod_info = g_new(MOD_MGR, 1);
//		mod_info->name = g_convert(module->Name(),
					   -1,
					   UTF_8,
					   OLD_CODESET,
					   &bytes_read,
					   &bytes_written, error);
		if (mod_info->name) {
			mod_info->language = (module->Lang())?
				backend->get_language_map(module->Lang()): 
				"unknown";
//			mod_info->type = g_convert(module->Type(),
						   -1,
						   UTF_8,
						   OLD_CODESET,
						   &bytes_read,
						   &bytes_written,
						   error);
			buf =
			    (module->getConfigEntry("Version")) ? module->
			    getConfigEntry("Version") : " ";
//			mod_info->new_version = g_convert(buf, 
						-1, UTF_8, OLD_CODESET,
				      		&bytes_read, 
						&bytes_written,
				      		error);
			mod_info->old_version =
			    backend->get_config_entry(mod_info->name, "Version"); //backend_get_module_version(mod_info->name);
			mod_info->installed =
			    backend->is_module(mod_info->name);
			mod_info->description = module->Description();
			mod_info->locked = 
				(module->getConfigEntry("CipherKey")) ? 1 : 0;
			it++;	
			return (MOD_MGR *) mod_info;
		}
	} 
	return NULL;
}

*/

/******************************************************************************
 * Name
 *   backend_module_mgr_get_path_to_mods
 *
 * Synopsisg_message("module_manager.cc:515 Description=%s",mod_info->description);
 *   #include "backend/module_manager.hh"
 *
 *   char *backend_module_mgr_get_path_to_mods(void)
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *backend_module_mgr_get_path_to_mods(void)
{
	char *path = mgr->prefixPath;
	return g_strdup(path);
}

char *backend_mod_mgr_get_config_entry(char *module_name,
				       const char *entry) {
	SWModule *mod;
	ModMap::iterator it;	//-- iteratior
	//-- iterate through the modules until we find modName  
	it = mgr->Modules.find(module_name);
	//-- if we find the module
	if (it != mgr->Modules.end()) {
		mod = (*it).second;
		return g_strdup((char *) mod->getConfigEntry(entry));
	} else
		return NULL;
}

int backend_mod_mgr_is_module(const char *mod_name) {
	ModMap::iterator it = mgr->Modules.find(mod_name);
	if (it != mgr->Modules.end()) {
		return 1;
	}
	return 0;
}

/******************************************************************************
 * Name
 *   backend_module_mgr_get_next_module
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   GList *backend_module_mgr_get_next_module()
 *
 * Description
 *   
 *
 * Return value
 *   MOD_MGR *
 */

MOD_MGR *backend_module_mgr_get_next_module(void)
{
	MOD_MGR *mod_info = NULL;
	SWModule *module;
	
	if (it != end) {
		module = it->second;
		mod_info = g_new(MOD_MGR, 1);
		gchar *name = module->Name();

		if (name) {
			mod_info->name = g_strdup(name);
			mod_info->language =
			    main_get_language_map(module->Lang());
			mod_info->type = strdup(module->Type());

			char *vers = (char *)module->getConfigEntry("Version");
			mod_info->new_version = strdup(vers ? vers : " ");

			char *installsize = (char *)module->getConfigEntry("InstallSize");
			if (installsize && (isdigit(*installsize))) {
				int isize = atoi(installsize);
				char *num;
				// round to nearest M or k.
				if (isize > (3*1024*1024)) {
					num = main_format_number((isize+(512*1024))/(1024*1024));
					mod_info->installsize =
					    g_strdup_printf("%sM", num);
				} else {
					num = main_format_number((isize+512)/1024);
					mod_info->installsize =
					    g_strdup_printf("%sk", num);
				}
				g_free(num);
			} else
				mod_info->installsize = g_strdup("-?-");

			char *feature = (char *)module->getConfigEntry("Feature");
			mod_info->is_devotional = (feature && !strcmp(feature, "DailyDevotion"));

			char *category = (char *)module->getConfigEntry("Category");
			mod_info->is_maps   = (category && !strcmp(category, "Maps"));
			mod_info->is_images = (category && !strcmp(category, "Images"));

			mod_info->old_version =
			    backend_mod_mgr_get_config_entry(mod_info->name, "Version");
			mod_info->installed =
			    backend_mod_mgr_is_module(mod_info->name);
			mod_info->description = module->Description();
			mod_info->locked = 
			    ((module->getConfigEntry("CipherKey")) ? 1 : 0);
			it++;	
			return (MOD_MGR *) mod_info;
		}
	} 
	return NULL;
}


/******************************************************************************
 * Name
 *   backend_module_mgr_remote_list_modules
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   GList *backend_module_mgr_remote_list_modules(const char *sourceName)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

void backend_module_mgr_remote_list_modules_init(const char *sourceName)
{
	InstallSourceMap::iterator source =
	    installMgr->sources.find(sourceName);
	if (source == installMgr->sources.end()) {
		fprintf(stderr, "Couldn't find remote source [%s]\n",
			sourceName);
		backend_shut_down_module_mgr();
	}
	it = source->second->getMgr()->Modules.begin();
	end = source->second->getMgr()->Modules.end();
}

/******************************************************************************
 * Name
 *   backend_module_mgr_list_local_modules
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   GList *backend_module_mgr_list_local_modules(const char *dir)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

void backend_module_mgr_list_local_modules_init()
{
	it = mgr->Modules.begin();
	end = mgr->Modules.end();
}

/******************************************************************************
 * Name
 *   backend_uninstall_module
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   int backend_uninstall_module(const char *modName)

 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int backend_uninstall_module(const char *dir,
			     const char *modName)
{
	SWModule *module;
	SWMgr  *tmp_mgr;
	int retval = -1;
	if (dir)
		tmp_mgr = new SWMgr(dir);
	else
		tmp_mgr = new SWMgr();
	
	ModMap::iterator it = tmp_mgr->Modules.find(modName);
	if (it == tmp_mgr->Modules.end()) {
		printf("Couldn't find module [%s] to remove\n",
			modName);
		return -1;
	}
	//printf("\nprefixPath = %s\n",tmp_mgr->prefixPath);
	module = it->second;
	retval = installMgr->removeModule(tmp_mgr, module->Name());
	delete tmp_mgr;
	return retval;
}


/******************************************************************************
 * Name
 *   backend_remote_install_module
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   int backend_remote_install_module(const char *sourceName, const char *modName)

 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int backend_remote_install_module(const char *sourceName,
				  const char *modName)
{
	InstallSourceMap::iterator source =
	    installMgr->sources.find(sourceName);
	if (source == installMgr->sources.end()) {
		printf("Couldn't find remote source [%s]\n",
			sourceName);
		return -1;
	}
	InstallSource *is = source->second;
	SWMgr *rmgr = is->getMgr();
	SWModule *module;
	ModMap::iterator it = rmgr->Modules.find(modName);
	if (it == rmgr->Modules.end()) {
		printf("Remote source [%s] does not make available module [%s]\n",
			sourceName, modName);
		return -1;
	}
	module = it->second;
	return installMgr->installModule(mgr, 0, module->Name(), is);
}


/******************************************************************************
 * Name
 *   backend_local_install_module
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   int backend_local_install_module(const char *destdir, const char *srcdir, const char *mod_name)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int backend_local_install_module(const char *destdir,
				 const char *srcdir,
				 const char *mod_name)
{
	SWMgr lmgr(destdir);
	return installMgr->installModule(&lmgr, srcdir, mod_name);
}


/******************************************************************************
 * Name
 *   backend_module_mgr_list_remote_sources
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   GList *backend_module_mgr_list_remote_sources(void)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

GList *backend_module_mgr_list_remote_sources(void)
{
	MOD_MGR_SOURCE *mms;
	GList *retval = NULL;
	char *envhomedir = getenv("HOME");
	SWBuf baseDir = (envhomedir) ? envhomedir : ".";
	baseDir += "/.sword/InstallMgr";
	InstallMgr *inst_mgr = new InstallMgr(baseDir);
	
	for (InstallSourceMap::iterator it =
	     inst_mgr->sources.begin();
	     it != inst_mgr->sources.end(); it++) {
		mms = g_new(MOD_MGR_SOURCE, 1);
		mms->caption = g_strdup(it->second->caption);
		mms->type = g_strdup(it->second->type);
		mms->source = g_strdup(it->second->source);
		mms->directory = g_strdup(it->second->directory);
		retval = g_list_append(retval, (MOD_MGR_SOURCE *) mms);
	}
	delete inst_mgr;
	return retval;
}


/******************************************************************************
 * Name
 *   backend_module_mgr_list_local_sources
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   GList *backend_module_mgr_list_local_sources(void)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

GList *backend_module_mgr_list_local_sources(void)
{
	MOD_MGR_SOURCE *mms;
	GList *retval = NULL;	
	char *envhomedir = getenv("HOME");
	SWBuf confPath = (envhomedir) ? envhomedir : ".";
	confPath += "/.sword/InstallMgr/InstallMgr.conf";
	SWConfig *installConf= new SWConfig(confPath.c_str());
	
	SectionMap::iterator sourcesSection;
	ConfigEntMap::iterator sourceBegin;
	ConfigEntMap::iterator sourceEnd;
	
	sourcesSection = installConf->Sections.find("Sources");
	if (sourcesSection != installConf->Sections.end()) {
		sourceBegin = sourcesSection->second.lower_bound("DIRSource");
		sourceEnd = sourcesSection->second.upper_bound("DIRSource");

		while (sourceBegin != sourceEnd) {
			mms = g_new(MOD_MGR_SOURCE, 1);
			InstallSource *is = new InstallSource("DIR", 
					sourceBegin->second.c_str());
			mms->caption = is->caption;
			mms->type = is->type;
			mms->source = is->source;
			mms->directory = is->directory;
			retval = g_list_append(retval,(MOD_MGR_SOURCE*) mms);
			sourceBegin++;
		}
	}
	delete installConf;
	return retval;
}


/******************************************************************************
 * Name
 *   backend_module_mgr_refresh_remote_source
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   int backend_module_mgr_refresh_remote_source(const char *sourceName)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int backend_module_mgr_refresh_remote_source(const char *sourceName)
{
	InstallSourceMap::iterator source = installMgr->sources.begin();
	
	source = installMgr->sources.find(sourceName);
	if (source == installMgr->sources.end()) {
		printf("Couldn't find remote source [%s]\n",
			sourceName);
		return 1;
	}
	installMgr->refreshRemoteSource(source->second);
	return 0;
}

/******************************************************************************
 * Name
 *   backend_init_module_mgr_config
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void backend_init_module_mgr_config(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void backend_init_module_mgr_config(void)
{
	
	char *envhomedir = getenv("HOME");
	SWBuf confPath = (envhomedir) ? envhomedir : ".";
	confPath += "/.sword/InstallMgr/InstallMgr.conf";
	FileMgr::createParent(confPath.c_str());
	remove(confPath.c_str());
	
	SWConfig config(confPath.c_str());

	InstallSource is("FTP");
	is.caption = "Crosswire";
	is.source = "ftp.crosswire.org";
	is.directory = "/pub/sword/raw";
	
	config["General"]["PassiveFTP"] = "true";
	config["Sources"]["FTPSource"] = is.getConfEnt();
	config.Save();
	
	InstallSource is_local("DIR");
	is_local.caption = "cdrom";
	is_local.source = "[local]";
	is_local.directory = "/mnt/cdrom";
	config["Sources"]["DIRSource"] = is_local.getConfEnt();	
	config.Save();
}


/******************************************************************************
 * Name
 *   backend_module_mgr_clear_config
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void backend_module_mgr_clear_config(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void backend_module_mgr_clear_config(void)
{
	char *envhomedir = getenv("HOME");
	SWBuf confPath = (envhomedir) ? envhomedir : ".";
	confPath += "/.sword/InstallMgr/InstallMgr.conf";
	FileMgr::createParent(confPath.c_str());
	remove(confPath.c_str());
	
	SWConfig config(confPath.c_str());
	
	config["General"]["PassiveFTP"] = "true";
	config.Save();	
}


/******************************************************************************
 * Name
 *   backend_module_mgr_add_source
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   backend_module_mgr_add_source(const char * vtype,
 *				   const char * type,
 *				   const char * caption,
 *				   const char * source,
 *				   const char * directory)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void backend_module_mgr_add_source(const char * vtype,
				   const char * type,
				   const char * caption,
				   const char * source,
				   const char * directory)
{
	char *envhomedir = getenv("HOME");
	SWBuf confPath = (envhomedir) ? envhomedir : ".";
	confPath += "/.sword/InstallMgr/InstallMgr.conf";
	FileMgr::createParent(confPath.c_str());
	
	SWConfig config(confPath.c_str());

	InstallSource is(type);
	is.caption = caption;
	is.source = source;
	is.directory = directory;
	config.Sections["Sources"].insert(ConfigEntMap::value_type(vtype, 
		is.getConfEnt().c_str()));	
	config.Save();
}

char *set_mod_mgr_locale(const char *sys_locale) {
	const char *mylocale;
	char *retval = NULL;
	char buf[32];
	int i = 0;
	SWLocale *sw_locale;
	
	if (sys_locale) {
		if (!strncmp(sys_locale,"ru_RU",5)) {
			sys_locale = "ru_RU-koi8-r";		
		} else if (!strncmp(sys_locale,"ru_RU-koi8-r",10)){
			if (strlen(sys_locale) >  12) {
				for(i = 0; i < 12; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
			
		} else if (!strncmp(sys_locale,"uk_UA-cp1251",10)){
			if (strlen(sys_locale) > 12 ) {
				for(i = 0; i < 12; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
			
		} else if (!strncmp(sys_locale,"uk_UA-koi8-u",10)){
			if (strlen(sys_locale) > 12 ) {
				for(i = 0; i < 12; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
			
		} else if (!strncmp(sys_locale,"pt_BR",5)){
			if (strlen(sys_locale) > 5 ) {
				for(i = 0; i < 5; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
			
		} else if (!strncmp(sys_locale,"en_GB",5)){
			if (strlen(sys_locale) > 5 ) {
				for(i = 0; i < 5; i++) {
					buf[i] = sys_locale[i];
					buf[i+1] = '\0';
				}
				sys_locale = buf;
			}
		} else {
			if (strlen(sys_locale) > 2 ) {
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
	if (sw_locale) {
		OLD_CODESET = (char*)sw_locale->getEncoding();
	} else {
		OLD_CODESET = (char*)"iso8859-1";
	}
	return retval;
}

/******************************************************************************
 * Name
 *   backend_init_module_mgr
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void backend_init_module_mgr(const char *dir, gboolean augment)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void backend_init_module_mgr(const char *dir,
			     gboolean augment)
{
	if (dir) {
		if (augment) {
			// normal case: this will include ~/.sword content.
			mgr = new SWMgr(dir);
		} else {
			// local directory install special case.
			mgr = new SWMgr(dir, true, 0, false, false);
			// see template.  re-assert 3 of 4 defaults:
			// autoload    -> true
			// filtermgr   -> null
			// multimod    -> false
			// NON-DEFAULT:
			// AUGMENTHOME -> FALSE
			// because we do not want to include everything already
			// installed as we prepare to do more installations.
		}
	} else {
		mgr = new SWMgr();
	}

	const char *lang = getenv("LANG");	
	if (!lang)
		lang="C";
	set_mod_mgr_locale(lang);

	char *envhomedir = getenv("HOME");
	SWBuf baseDir = (envhomedir) ? envhomedir : ".";
	baseDir += "/.sword/InstallMgr";

	GSStatusReporter *statusReporter = new GSStatusReporter();

	installMgr = new InstallMgr(baseDir, statusReporter);
}

/******************************************************************************
 * Name
 *   backend_terminate_module_mgr
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void backend_terminate_module_mgr(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void backend_terminate_module_mgr(void)
{
	installMgr->terminate();
}

/******************************************************************************
 * Name
 *   backend_shut_down_module_mgr
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void backend_shut_down_module_mgr(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void backend_shut_down_module_mgr(void)
{
	delete installMgr;
	delete mgr;
}
