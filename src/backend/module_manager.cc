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
static gchar *f_message = "backend/module_manager.cc line #%d \"%s\" = %s\n";
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


typedef map < SWBuf, SWBuf > MM_ModLanguageMap;
MM_ModLanguageMap mm_languageMap;

const char *backend_mod_mgr_get_language_map(const char *language) {
	return mm_languageMap[language].c_str();
}


void backend_mod_mgr_init_language_map(void) {
	/* --list form Bibletime-1.3-- */
	//mm_languageMap[SWBuf("aa")] = SWBuf("Afar");
	//mm_languageMap[SWBuf("ab")] = SWBuf("Abkhazian");
	//mm_languageMap[SWBuf("ae")] = SWBuf("Avestan");
	mm_languageMap[SWBuf("af")] = SWBuf(_("Afrikaans")); /* done */
	//mm_languageMap[SWBuf("am")] = SWBuf("Amharic");
	mm_languageMap[SWBuf("ang")] = SWBuf(_("English, Old (ca.450-1100)")); /* done */
	mm_languageMap[SWBuf("ar")] = SWBuf(_("Arabic")); /* done */
	//mm_languageMap[SWBuf("as")] = SWBuf("Assamese");
	//mm_languageMap[SWBuf("ay")] = SWBuf("Aymara");
	mm_languageMap[SWBuf("az")] = SWBuf(_("Azerbaijani")); /* done */
	//mm_languageMap[SWBuf("ba")] = SWBuf("Bashkir");
	mm_languageMap[SWBuf("be")] = SWBuf(_("Belarusian")); /* done */
	mm_languageMap[SWBuf("bg")] = SWBuf(_("Bulgarian")); /* done */
	//mm_languageMap[SWBuf("bh")] = SWBuf("Bihari");
	//mm_languageMap[SWBuf("bi")] = SWBuf("Bislama");
	//mm_languageMap[SWBuf("bn")] = SWBuf("Bengali");
	//mm_languageMap[SWBuf("bo")] = SWBuf("Tibetan");
	mm_languageMap[SWBuf("br")] = SWBuf(_("Breton")); /* done */
	mm_languageMap[SWBuf("bs")] = SWBuf(_("Bosnian")); /* done */
	mm_languageMap[SWBuf("ca")] = SWBuf(_("Catalan")); /* done */
	//mm_languageMap[SWBuf("ce")] = SWBuf("Chechen");
	mm_languageMap[SWBuf("ceb")] = SWBuf(_("Cebuano")); /* done */
	mm_languageMap[SWBuf("ch")] = SWBuf("Chamorro");
	//mm_languageMap[SWBuf("co")] = SWBuf("Corsican");
	mm_languageMap[SWBuf("cop")] = SWBuf(_("Coptic")); /* done */
	mm_languageMap[SWBuf("cs")] = SWBuf(_("Czech")); /* done */
	mm_languageMap[SWBuf("cu")] = SWBuf(_("Church Slavic")); /* done */
	//mm_languageMap[SWBuf("cv")] = SWBuf("Chuvash");
	mm_languageMap[SWBuf("cy")] = SWBuf(_("Welsh")); /* done */
	mm_languageMap[SWBuf("da")] = SWBuf(_("Danish")); /* done */
	mm_languageMap[SWBuf("de")] = SWBuf(_("German")); /* done */
	//mm_languageMap[SWBuf("dz")] = SWBuf("Dzongkha");
	mm_languageMap[SWBuf("el")] = SWBuf(_("Greek, Modern (1453-)")); /* done */
	mm_languageMap[SWBuf("en")] = SWBuf(_("English"));
	mm_languageMap[SWBuf("en_US")] = SWBuf(_("American English")); /* done */
/*  */	mm_languageMap[SWBuf("enm")] =
	    SWBuf(_("English, Middle (1100-1500)")); /* done */
	mm_languageMap[SWBuf("eo")] = SWBuf(_("Esperanto")); /* done */
	mm_languageMap[SWBuf("es")] = SWBuf(_("Spanish")); /* done */
	mm_languageMap[SWBuf("et")] = SWBuf(_("Estonian")); /* done */
	mm_languageMap[SWBuf("eu")] = SWBuf(_("Basque")); /* done */
	mm_languageMap[SWBuf("fa")] = SWBuf(_("Farsi (Persian)"));
	mm_languageMap[SWBuf("fi")] = SWBuf(_("Finnish")); /* done */
	//mm_languageMap[SWBuf("fj")] = SWBuf("Fijian");
	//mm_languageMap[SWBuf("fo")] = SWBuf("Faroese");
	mm_languageMap[SWBuf("fr")] = SWBuf(_("French")); /* done */
	mm_languageMap[SWBuf("fy")] = SWBuf(_("Frisian")); /* done */
	mm_languageMap[SWBuf("ga")] = SWBuf(_("Irish")); /* done */
	mm_languageMap[SWBuf("gd")] = SWBuf(_("Gaelic (Scots)")); /* done */
	//mm_languageMap[SWBuf("gl")] = SWBuf("Gallegan");
	//mm_languageMap[SWBuf("gn")] = SWBuf("Guarani");
	//mm_languageMap[SWBuf("gn")] = SWBuf("Gujarati");
/*  */	mm_languageMap[SWBuf("got")] = SWBuf(_("Gothic")); /* done */
	mm_languageMap[SWBuf("gv")] = SWBuf(_("Manx")); /* done */
	mm_languageMap[SWBuf("grc")] = SWBuf(_("Greek, Ancient (to 1453)")); /* done */
	mm_languageMap[SWBuf("he")] = SWBuf(_("Hebrew")); /* done */
	mm_languageMap[SWBuf("haw")] = SWBuf(_("Hawaiian")); /* done */
	//mm_languageMap[SWBuf("hi")] = SWBuf("Hindi");
	//mm_languageMap[SWBuf("ho")] = SWBuf("Hiri Motu");
	mm_languageMap[SWBuf("hr")] = SWBuf(_("Croatian")); /* done */
	mm_languageMap[SWBuf("ht")] = SWBuf(_("French, Haitian Creole")); /* done */
	mm_languageMap[SWBuf("hu")] = SWBuf(_("Hungarian")); /* done */
	mm_languageMap[SWBuf("hy")] = SWBuf(_("Armenian")); /* done */
	//mm_languageMap[SWBuf("hz")] = SWBuf("Herero");
	//mm_languageMap[SWBuf("ia")] = SWBuf("Interlingua");
	//mm_languageMap[SWBuf("ie")] = SWBuf("Interlingue");
	mm_languageMap[SWBuf("id")] = SWBuf(_("Indonesian")); /* done */
/*  */	//mm_languageMap[SWBuf("ik")] = SWBuf("Inupiaq");
	mm_languageMap[SWBuf("is")] = SWBuf(_("Icelandic")); /* done */
	mm_languageMap[SWBuf("it")] = SWBuf(_("Italian")); /* done */
	//mm_languageMap[SWBuf("iu")] = SWBuf("Inuktitut"); 
	mm_languageMap[SWBuf("ja")] = SWBuf(_("Japanese")); /* done */
	mm_languageMap[SWBuf("ka")] = SWBuf(_("Georgian")); /* done */
	mm_languageMap[SWBuf("kek")] = SWBuf(_("Kekchi")); /* done */
	//mm_languageMap[SWBuf("ki")] = SWBuf("Kikuyu");
	//mm_languageMap[SWBuf("kj")] = SWBuf("Kuanyama");
	//mm_languageMap[SWBuf("kk")] = SWBuf("Kazakh");
	//mm_languageMap[SWBuf("kl")] = SWBuf("Kalaallisut");
	//mm_languageMap[SWBuf("km")] = SWBuf("Khmer");
	//mm_languageMap[SWBuf("kn")] = SWBuf("Kannada");
	mm_languageMap[SWBuf("ko")] = SWBuf(_("Korean")); /* done */
	//mm_languageMap[SWBuf("ks")] = SWBuf("Kashmiri");
	mm_languageMap[SWBuf("ku")] = SWBuf(_("Kurdish")); /* done */
	//mm_languageMap[SWBuf("kv")] = SWBuf("Komi");
	//mm_languageMap[SWBuf("kw")] = SWBuf("Cornish");
	mm_languageMap[SWBuf("ky")] = SWBuf(_("Kirghiz")); /* done */
	mm_languageMap[SWBuf("la")] = SWBuf(_("Latin")); /* done */
	//mm_languageMap[SWBuf("lb")] = SWBuf("Letzeburgesch");
	//mm_languageMap[SWBuf("ln")] = SWBuf("Lingala");
	//mm_languageMap[SWBuf("lo")] = SWBuf("Lao");
	mm_languageMap[SWBuf("lt")] = SWBuf(_("Lithuanian")); /* done */
	mm_languageMap[SWBuf("lv")] = SWBuf(_("Latvian")); /* done */
	//mm_languageMap[SWBuf("mg")] = SWBuf("Malagasy");
	//mm_languageMap[SWBuf("mh")] = SWBuf("Marshall");
	mm_languageMap[SWBuf("mi")] = SWBuf(_("Maori")); /* done */
	mm_languageMap[SWBuf("mk")] = SWBuf(_("Macedonian")); /* done */
	//mm_languageMap[SWBuf("ml")] = SWBuf("Malayalam");
	//mm_languageMap[SWBuf("mn")] = SWBuf("Mongolian");
	//mm_languageMap[SWBuf("mo")] = SWBuf("Moldavian");
	//mm_languageMap[SWBuf("mr")] = SWBuf("Marathi");
	mm_languageMap[SWBuf("ms")] = SWBuf(_("Malay")); /* done */
	mm_languageMap[SWBuf("mt")] = SWBuf(_("Maltese")); /* done */
	mm_languageMap[SWBuf("my")] = SWBuf("Burmese");
	//mm_languageMap[SWBuf("na")] = SWBuf("Nauru");
	//mm_languageMap[SWBuf("nb")] = SWBuf("Norwegian Bokm");
/*  */	//mm_languageMap[SWBuf("nd")] = SWBuf("Ndebele, North");
	mm_languageMap[SWBuf("nds")] = SWBuf(_("Low German; Low Saxon")); /* done */
	//mm_languageMap[SWBuf("ne")] = SWBuf("Nepali");
	//mm_languageMap[SWBuf("ng")] = SWBuf("Ndonga");
	mm_languageMap[SWBuf("nl")] = SWBuf(_("Dutch")); /* done */
	//mm_languageMap[SWBuf("nn")] = SWBuf("Norwegian Nynorsk");
	mm_languageMap[SWBuf("no")] = SWBuf(_("Norwegian")); /* done */
	//mm_languageMap[SWBuf("nr")] = SWBuf("Ndebele, South");
	//mm_languageMap[SWBuf("nv")] = SWBuf("Navajo");
	//mm_languageMap[SWBuf("ny")] = SWBuf("Chichewa; Nyanja");
	//mm_languageMap[SWBuf("oc")] = SWBuf("Occitan (post 1500); Proven");
	//mm_languageMap[SWBuf("om")] = SWBuf("Oromo");
	//mm_languageMap[SWBuf("or")] = SWBuf("Oriya");
	//mm_languageMap[SWBuf("os")] = SWBuf("Ossetian; Ossetic");
	//mm_languageMap[SWBuf("pa")] = SWBuf("Panjabi");
	mm_languageMap[SWBuf("pap")] = SWBuf(_("Papiamento")); /* done */
	//mm_languageMap[SWBuf("pi")] = SWBuf("Pali");
	mm_languageMap[SWBuf("pl")] = SWBuf(_("Polish")); /* done */
	mm_languageMap[SWBuf("pot")] = SWBuf(_("Potawatomi")); /* done */
	mm_languageMap[SWBuf("ppk")] = SWBuf(_("Uma")); /* done */
	//mm_languageMap[SWBuf("ps")] = SWBuf("Pushto");
	mm_languageMap[SWBuf("pt")] = SWBuf(_("Portuguese")); /* done */
	//mm_languageMap[SWBuf("qu")] = SWBuf("Quechua");
	//mm_languageMap[SWBuf("rm")] = SWBuf("Raeto-Romance");
	//mm_languageMap[SWBuf("rn")] = SWBuf("Rundi");
	mm_languageMap[SWBuf("ro")] = SWBuf(_("Romanian")); /* done */
	mm_languageMap[SWBuf("ru")] = SWBuf(_("Russian")); /* done */
	//mm_languageMap[SWBuf("rw")] = SWBuf("Kinyarwanda");
	//mm_languageMap[SWBuf("sa")] = SWBuf("Sanskrit");
	//mm_languageMap[SWBuf("sc")] = SWBuf("Sardinian");
	mm_languageMap[SWBuf("sco")] = SWBuf(_("Scots")); /* done */
	//mm_languageMap[SWBuf("sd")] = SWBuf("Sindhi");
	//mm_languageMap[SWBuf("se")] = SWBuf("Northern Sami");
	//mm_languageMap[SWBuf("sg")] = SWBuf("Sango");
	//mm_languageMap[SWBuf("si")] = SWBuf("Sinhalese");
	mm_languageMap[SWBuf("sk")] = SWBuf(_("Slovak")); /* done */
	mm_languageMap[SWBuf("sl")] = SWBuf(_("Slovenian")); /* done */
	//mm_languageMap[SWBuf("sm")] = SWBuf("Samoan");
/*  */	//mm_languageMap[SWBuf("sn")] = SWBuf("Shona");
	mm_languageMap[SWBuf("so")] = SWBuf(_("Somali")); /* done */
	mm_languageMap[SWBuf("sq")] = SWBuf(_("Albanian")); /* done */
	//mm_languageMap[SWBuf("sr")] = SWBuf("Serbian");
	//mm_languageMap[SWBuf("ss")] = SWBuf("Swati");
	//mm_languageMap[SWBuf("st")] = SWBuf("Sotho, Southern");
	//mm_languageMap[SWBuf("su")] = SWBuf("Sundanese");
	mm_languageMap[SWBuf("sv")] = SWBuf(_("Swedish")); /* done */
	mm_languageMap[SWBuf("sw")] = SWBuf(_("Swahili")); /* done */
	mm_languageMap[SWBuf("syr")] = SWBuf(_("Syriac")); /* done */
	mm_languageMap[SWBuf("ta")] = SWBuf(_("Tamil")); /* done */
	//mm_languageMap[SWBuf("te")] = SWBuf("Telugu");
	//mm_languageMap[SWBuf("tg")] = SWBuf("Tajik");
	mm_languageMap[SWBuf("th")] = SWBuf(_("Thai")); /* done */
	//mm_languageMap[SWBuf("tk")] = SWBuf("Turkmen");
	mm_languageMap[SWBuf("tl")] = SWBuf(_("Tagalog")); /* done */
	mm_languageMap[SWBuf("tlh")] = SWBuf(_("Klingon")); /* done */
	mm_languageMap[SWBuf("tn")] = SWBuf(_("Tswana")); /* done */
	mm_languageMap[SWBuf("tr")] = SWBuf(_("Turkish")); /* done */
	//mm_languageMap[SWBuf("ts")] = SWBuf("Tsonga");
	//mm_languageMap[SWBuf("tt")] = SWBuf("Tatar");
	//mm_languageMap[SWBuf("tw")] = SWBuf("Twi");
	mm_languageMap[SWBuf("ty")] = SWBuf(_("Tahitian")); /* done */
	//mm_languageMap[SWBuf("ug")] = SWBuf("Uighur");
	mm_languageMap[SWBuf("uk")] = SWBuf(_("Ukrainian")); /* done */
	//mm_languageMap[SWBuf("ur")] = SWBuf("Urdu");
	//mm_languageMap[SWBuf("uz")] = SWBuf("Uzbek");
	mm_languageMap[SWBuf("vi")] = SWBuf(_("Vietnamese")); /* done */
	mm_languageMap[SWBuf("vo")] = SWBuf("Volapük");/* if commented,  accentued u causes gettext problems */
/*  */	//mm_languageMap[SWBuf("wo")] = SWBuf("Wolof");
	mm_languageMap[SWBuf("xh")] = SWBuf(_("Xhosa")); 
	mm_languageMap[SWBuf("x-E-BAR")] = SWBuf(_("Bavarian")); /* done */
	mm_languageMap[SWBuf("x-E-GSW")] = SWBuf(_("Alemannisch")); /* done */
	mm_languageMap[SWBuf("x-E-ITZ")] = SWBuf(_("Itz")); /* done */
	mm_languageMap[SWBuf("x-E-JIV")] = SWBuf(_("Shuar")); /* done */
	mm_languageMap[SWBuf("x-E-LMO")] = SWBuf(_("Lombard")); /* done */
	mm_languageMap[SWBuf("x-E-MKJ")] = SWBuf(_("Macedonian")); /* done */
	mm_languageMap[SWBuf("x-E-PDG")] = SWBuf(_("Tok Pisin")); /* done */
	mm_languageMap[SWBuf("x-E-RMY")] = SWBuf(_("Romani, Vlax")); /* done */
	mm_languageMap[SWBuf("x-E-SAJ")] = SWBuf(_("Sango")); /* done */
	mm_languageMap[SWBuf("x-E-SRN")] = SWBuf(_("Sranan")); /* done */
	//mm_languageMap[SWBuf("yi")] = SWBuf("Yiddish");
	//mm_languageMap[SWBuf("za")] = SWBuf("Zhuang");
	mm_languageMap[SWBuf("zh")] = SWBuf(_("Chinese")); /* done */
	mm_languageMap[SWBuf("zu")] = SWBuf(_("Zulu")); /* done */
}

/******************************************************************************
 * Name
 *   preDownloadStatus
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

void GSStatusReporter::preStatus(long totalBytes, long completedBytes, 
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

void GSStatusReporter::statusUpdate(double dltotal, double dlnow) 
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
	if(dir)
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
	
	if(it != end) {
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

char *backend_mod_mgr_get_config_entry(char * module_name, char * entry) {
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
 *   module_mgr_list_modules
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   GList *module_mgr_list_modules(SWMgr * mgr)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
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
			    (module->Lang()
			     ? backend_mod_mgr_get_language_map(module->Lang())
			     : "unknown");
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

int backend_uninstall_module(const char *dir, const char *modName)
{
	SWModule *module;
	SWMgr  *tmp_mgr;
	int retval = -1;
	if(dir)
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

int backend_remote_install_module(const char *sourceName, const char *modName)
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
 *   int backend_local_install_module(const char *dir, const char *mod_name)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int backend_local_install_module(const char *dir, const char *mod_name)
{
	SWMgr lmgr(dir);
	SWModule *module;
	ModMap::iterator it = lmgr.Modules.find(mod_name);
	if (it == lmgr.Modules.end()) {
		printf("Module [%s] not available at path [%s]\n",
			mod_name, dir);
		return -1;
	}
	module = it->second;
	return installMgr->installModule(mgr, dir, module->Name());
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
	is.caption = "crosswire";
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

/******************************************************************************
 * Name
 *   backend_init_module_mgr
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void backend_init_module_mgr(const char *dir)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void backend_init_module_mgr(const char *dir, gboolean augment)
{
	if (dir) {
		if (augment) {
			// normal case: this will include ~/.sword content.
			mgr = new SWMgr(dir);
		} else {
			// local directory install special case.
			mgr = new SWMgr(dir, true, 0, false, false);
			// re-assert defaults:
			// autoload    -> true
			// filtermgr   -> null
			// multimod    -> false
			// NON-DEFAULT:
			// AUGMENTHOME -> FALSE
			// because we do not want to include everything already
			// installed as prepare to do more installations.
		}
	} else {
		mgr = new SWMgr();
	}

	const char *lang = getenv("LANG");	
	if (!lang) lang="C";
	//printf("dir = %s\n",dir);
	set_mod_mgr_locale(lang);
	char *envhomedir = getenv("HOME");
	SWBuf baseDir = (envhomedir) ? envhomedir : ".";
	baseDir += "/.sword/InstallMgr";
	GSStatusReporter *statusReporter = new GSStatusReporter();
	installMgr = new InstallMgr(baseDir, statusReporter);
	//installMgr = new InstallMgr(baseDir);
	backend_mod_mgr_init_language_map();
	//backend_module_mgr_list_local_sources();
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/module_manager.hh"
 *
 *   void (void)
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
