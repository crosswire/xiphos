/*
 * Xiphos Bible Study Tool
 * module_manager.cc
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

/*
 * Sword must be built with curl support for xiphos's module manager to
 * work as expected
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
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
#include "main/settings.h"
#include "main/sword.h"

#include "gui/debug_glib_null.h"

using namespace sword;
using std::cout;
using std::cin;
using std::string;

static SWMgr *mgr = NULL;
static SWMgr *list_mgr = NULL;
static InstallMgr *installMgr = NULL;
static ModMap::iterator it;
static ModMap::iterator end;

// separate iterator/end pair for listing sources' modules.
// avoids being overused by other areas of mod.mgr use.
static ModMap::iterator list_it;
static ModMap::iterator list_end;

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
	double filefraction = (dlnow / dltotal);
	update_install_progress(filefraction);
}

ModuleManager::ModuleManager()
	: mgr(NULL),
	  list_mgr(NULL),
	  installMgr(NULL)
{
	const gchar *envhomedir = g_getenv(HOMEVAR);
	baseDir = (envhomedir) ? envhomedir : ".";
	baseDir += "/" DOTSWORD "/InstallMgr";
	statusReporter = new GSStatusReporter();
}

ModuleManager::~ModuleManager()
{
}

char *backend_mod_mgr_get_config_entry(char *module_name,
				       const char *entry)
{
	ModMap::iterator it; //-- iteratior

	it = mgr->Modules.find(module_name);
	if (it != mgr->Modules.end()) {
		SWModule *mod = (*it).second;
		return g_strdup((char *)mod->getConfigEntry(entry));
	} else
		return NULL;
}

int backend_mod_mgr_is_module(const char *mod_name)
{
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

	if (list_it != list_end) {
		SWModule *module = list_it->second;
		mod_info = g_new(MOD_MGR, 1);
		gchar *name = g_strdup(module->getName());

		if (name) {
			mod_info->name = g_strdup(name);

			mod_info->abbreviation = (char *)module->getConfigEntry("Abbreviation");
			if (mod_info->abbreviation) {
				mod_info->abbreviation = g_strdup(mod_info->abbreviation);
				main_add_abbreviation(mod_info->name, mod_info->abbreviation);
			}

			mod_info->language =
			    main_get_language_map(module->getLanguage());
			mod_info->type = g_strdup(module->getType());
			mod_info->about = g_strdup((char *)module->getConfigEntry("About"));

			char *vers = (char *)module->getConfigEntry("Version");
			mod_info->new_version = strdup(vers ? vers : " ");

			vers = (char *)module->getConfigEntry("MinimumVersion");
			mod_info->min_version = strdup(vers ? vers : " ");

			char *installsize = (char *)module->getConfigEntry("InstallSize");
			if (installsize && (isdigit(*installsize))) {
				int isize = atoi(installsize);
				char *num;
				// round to nearest M or k.
				if (isize > (3 * 1024 * 1024)) {
					num = main_format_number((isize + (512 * 1024)) / (1024 * 1024));
					mod_info->installsize =
					    g_strdup_printf("%sM", num);
				} else {
					num = main_format_number((isize + 512) / 1024);
					mod_info->installsize =
					    g_strdup_printf("%sk", num);
				}
				g_free(num);
			} else
				mod_info->installsize = g_strdup("-?-");

			char *feature = (char *)module->getConfigEntry("Feature");
			mod_info->is_devotional = (feature && !strcmp(feature, "DailyDevotion"));

			char *category = (char *)module->getConfigEntry("Category");
			mod_info->is_maps = (category && !strcmp(category, "Maps"));
			mod_info->is_images = (category && !strcmp(category, "Images"));
			mod_info->is_glossary = (category && !strcmp(category, "Glossaries"));
			mod_info->is_cult =
			    (category &&
			     !strcmp(category, "Cults / Unorthodox / Questionable Material"));

			mod_info->old_version =
			    backend_mod_mgr_get_config_entry(name, "Version");
			mod_info->installed =
			    backend_mod_mgr_is_module(name);
			mod_info->description = g_strdup(module->getDescription());
			mod_info->locked =
			    ((module->getConfigEntry("CipherKey")) ? 1 : 0);
			++list_it;
			g_free(name);
			return (MOD_MGR *)mod_info;
		}
	}
	g_free(mod_info);
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
	list_it = source->second->getMgr()->Modules.begin();
	list_end = source->second->getMgr()->Modules.end();
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

void backend_module_mgr_list_local_modules_init(bool for_install)
{
	if (for_install) {
		list_it = list_mgr->Modules.begin();
		list_end = list_mgr->Modules.end();
	} else {
		list_it = mgr->Modules.begin();
		list_end = mgr->Modules.end();
	}
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
	SWMgr *tmp_mgr;
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
	module = it->second;
	retval = installMgr->removeModule(tmp_mgr, module->getName());
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

int backend_remote_install_module(const char *destdir,
				  const char *sourceName,
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
	ModMap::iterator it = rmgr->Modules.find(modName);
	if (it == rmgr->Modules.end()) {
		printf("Remote source [%s] does not make available module [%s]\n",
		       sourceName, modName);
		return -1;
	}

	SWMgr dmgr((destdir ? destdir : settings.path_to_mods),
		   true, 0, false, false);
	return installMgr->installModule(&dmgr, 0, modName, is);
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
	SWMgr lmgr((destdir ? destdir : settings.path_to_mods),
		   true, 0, false, false);
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
	GList *retval = NULL;
	const gchar *envhomedir = g_getenv(HOMEVAR);
	SWBuf baseDir = (envhomedir) ? envhomedir : ".";
	baseDir += "/" DOTSWORD "/InstallMgr";

	InstallMgr *inst_mgr = new InstallMgr(baseDir,
					      (StatusReporter *)0,
					      (SWBuf) "ftp",
					      (SWBuf) ftp_password());
	inst_mgr->setUserDisclaimerConfirmed(true);

	for (InstallSourceMap::iterator it =
		 inst_mgr->sources.begin();
	     it != inst_mgr->sources.end(); ++it) {
		MOD_MGR_SOURCE *mms = g_new(MOD_MGR_SOURCE, 1);
		mms->caption = g_strdup(it->second->caption);
		mms->type = g_strdup(it->second->type);
		mms->source = g_strdup(it->second->source);
		mms->directory = g_strdup(it->second->directory);
		mms->user = g_strdup(it->second->u);
		mms->pass = g_strdup(it->second->p);
		mms->uid = g_strdup(it->second->uid);
		retval = g_list_append(retval, (MOD_MGR_SOURCE *)mms);
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
	GList *retval = NULL;
	const gchar *envhomedir = g_getenv(HOMEVAR);
	SWBuf confPath = (envhomedir) ? envhomedir : ".";
	confPath += "/" DOTSWORD "/InstallMgr/InstallMgr.conf";
	SWConfig *installConf = new SWConfig(confPath.c_str());

	SectionMap::iterator sourcesSection;
	ConfigEntMap::iterator sourceBegin;
	ConfigEntMap::iterator sourceEnd;

	sourcesSection = installConf->getSections().find("Sources");
	if (sourcesSection != installConf->getSections().end()) {
		sourceBegin = sourcesSection->second.lower_bound("DIRSource");
		sourceEnd = sourcesSection->second.upper_bound("DIRSource");

		while (sourceBegin != sourceEnd) {
			MOD_MGR_SOURCE *mms = g_new(MOD_MGR_SOURCE, 1);
			InstallSource *is = new InstallSource("DIR",
							      sourceBegin->second.c_str());
			mms->caption = is->caption;
			mms->type = is->type;
			mms->source = is->source;
			mms->directory = is->directory;
			mms->user = is->u;
			mms->pass = is->p;
			mms->uid = is->uid;
			retval = g_list_append(retval, (MOD_MGR_SOURCE *)mms);
			++sourceBegin;
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
	InstallSourceMap::iterator source = installMgr->sources.find(sourceName);

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

	const gchar *envhomedir = g_getenv(HOMEVAR);
	SWBuf confPath = (envhomedir) ? envhomedir : ".";
	confPath += "/" DOTSWORD "/InstallMgr/InstallMgr.conf";
	FileMgr::createParent(confPath.c_str());
	remove(confPath.c_str());

	SWConfig config(confPath.c_str());

	InstallSource is("FTP");
	is.caption = "CrossWire";
	is.source = "ftp.crosswire.org";
	is.directory = "/pub/sword/raw";

	config["General"]["PassiveFTP"] = "true";
	config["Sources"]["FTPSource"] = is.getConfEnt();
	config.save();

	InstallSource is_local("DIR");
	is_local.caption = "cdrom";
	is_local.source = "[local]";
	is_local.directory = "/mnt/cdrom";
	config["Sources"]["DIRSource"] = is_local.getConfEnt();
	config.save();
}

int
backend_init_module_mgr_config_extras()
{
	bool needNewInstallMgr = (installMgr == NULL);
	int retval;

	if (needNewInstallMgr)
		backend_init_module_mgr(NULL, false, false);

	retval = installMgr->refreshRemoteSourceConfiguration();

	if (needNewInstallMgr) {
		backend_terminate_module_mgr();
		backend_shut_down_module_mgr();
	}
	return retval;
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
	const gchar *envhomedir = g_getenv(HOMEVAR);
	SWBuf confPath = (envhomedir) ? envhomedir : ".";
	confPath += "/" DOTSWORD "/InstallMgr/InstallMgr.conf";
	FileMgr::createParent(confPath.c_str());
	remove(confPath.c_str());

	SWConfig config(confPath.c_str());

	config["General"]["PassiveFTP"] = "true";
	config.save();
	backend_module_mgr_reread_config();
}

/******************************************************************************
 * Name
 *   backend_module_mgr_reread_config
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void backend_module_mgr_reread_config(void)
 *
 * Description
 *   re-init's installMgr's sense of sources.
 *
 * Return value
 *   void
 */

void backend_module_mgr_reread_config(void)
{
	installMgr->readInstallConf();
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
 *				   const char * directory,
 *				   const char * user,
 *				   const char * pass,
 *				   const char * uid)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void backend_module_mgr_add_source(const char *vtype,
				   const char *type,
				   const char *caption,
				   const char *source,
				   const char *directory,
				   const char *user,
				   const char *pass,
				   const char *uid)
{
	const gchar *envhomedir = g_getenv(HOMEVAR);
	SWBuf confPath = (envhomedir) ? envhomedir : ".";
	confPath += "/" DOTSWORD "/InstallMgr/InstallMgr.conf";
	FileMgr::createParent(confPath.c_str());

	SWConfig config(confPath.c_str());

	InstallSource is(type);
	is.caption = caption;
	is.source = source;
	is.directory = directory;
	is.u = user;
	is.p = pass;
	is.uid = uid;
	config.getSections()["Sources"].insert(ConfigEntMap::value_type(vtype,
								   is.getConfEnt().c_str()));
	config.save();
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
			     gboolean augment,
			     gboolean regular)
{
// complicated overloading just to init a swmgr.
// beyond the directory, we send in 3 defaults plus "something."
// autoload    -> true
// filtermgr   -> null (0)
// multimod    -> false
// AUGMENTHOME -> are we setting the regular mgr?
//                then "as sent" ([a] total list or [b] per-area list)
//		  else "false for local install" (always per-area)

#ifdef CHATTY
	GTimer *t;
	double d;
	t = g_timer_new();
#endif

	if (regular) { // use main (regular) swmgr.
		if (mgr)
			delete mgr;
		mgr = new SWMgr(dir, true, 0, false, augment);
	} else {
		if (list_mgr)
			delete list_mgr;
		list_mgr = new SWMgr(dir, true, 0, false, false);
	}

#ifdef CHATTY
	g_timer_stop(t);
	d = g_timer_elapsed(t, NULL);
	XI_message(("create SWMgr time is %f", d));
#endif

	const gchar *envhomedir = g_getenv(HOMEVAR);
	SWBuf baseDir = (envhomedir) ? envhomedir : ".";
	baseDir += "/" DOTSWORD "/InstallMgr";

	GSStatusReporter *statusReporter = new GSStatusReporter();

	if (installMgr)
		delete installMgr;
	installMgr = new InstallMgr(baseDir,
				    statusReporter,
				    (SWBuf) "ftp",
				    (SWBuf) ftp_password());
	installMgr->setUserDisclaimerConfirmed(true);
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
	installMgr = NULL;
	delete mgr;
	mgr = NULL;
	delete list_mgr;
	list_mgr = NULL;
}
