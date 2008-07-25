/*
 * GnomeSword Bible Study Tool
 * sword.cc - glue 
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


#include <gnome.h>
#include <swmgr.h>
#include <swmodule.h>
#include <stringmgr.h>
#include <localemgr.h>

#ifdef USE_GTKMOZEMBED
#include <gtkmozembed.h>
#include <gtkmozembed_internal.h>
#include <nsIDOMMouseEvent.h>
#ifndef HAVE_GECKO_1_9
#include <dom/nsIDOMKeyEvent.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>
#include "gui/bibletext.h"
#ifdef __cplusplus
}
#endif

#include <ctype.h>
#include <time.h>
#include <dirent.h>

#include "gui/main_window.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"
#include "gui/commentary.h"
#include "gui/dialog.h"
#include "gui/parallel_dialog.h"
#include "gui/parallel_view.h"
#include "gui/tabbed_browser.h"
#include "gui/gnomesword.h"
#include "gui/sidebar.h"
//#include "gui/search_sidebar.h"
#include "gui/utilities.h"
//#include "gui/html.h"

#include "main/display.hh"
#include "main/lists.h"
#include "main/navbar.h"
#include "main/navbar_book.h"
#include "main/search_sidebar.h"
#include "main/previewer.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"
 
#include "main/parallel_view.h"
#include "backend/sword_main.hh"
#include "backend/gs_stringmgr.h"

char *OLD_CODESET;
using namespace sword; 
	
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"

extern GtkWidget *cbe_book;
extern GtkWidget *spb_chapter;
extern GtkWidget *spb_verse;
extern GtkWidget *cbe_freeform_lookup;
extern gboolean shift_key_presed;

gboolean style_display = TRUE;
char *sword_locale = NULL;

/******************************************************************************
 * Name
 *   main_book_heading
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   void main_book_heading(char * mod_name)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_book_heading(char * mod_name)
{	
	VerseKey *vkey;	
	SWMgr *mgr = backend->get_display_mgr();
	
	backend->display_mod = mgr->Modules[mod_name];
	vkey = (VerseKey*)(SWKey*)(*backend->display_mod);	
	vkey->Headings(1);
	vkey->AutoNormalize(0);
	vkey->Verse(0);
	vkey->Chapter(0);
	backend->display_mod->Display();
}


/******************************************************************************
 * Name
 *   main_chapter_heading
 *
 * Synopsis
 *   #include "main/module_dialogs.h"
 *
 *   void main_chapter_heading(char * mod_name)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_chapter_heading(char * mod_name)
{	
	VerseKey *vkey;	
	SWMgr *mgr = backend->get_display_mgr();
	
	backend->display_mod = mgr->Modules[mod_name];
	backend->display_mod->setKey(settings.currentverse);
	vkey = (VerseKey*)(SWKey*)(*backend->display_mod);	
	vkey->Headings(1);
	vkey->AutoNormalize(0);
	vkey->Verse(0);
	backend->display_mod->Display();
}


/******************************************************************************
 * Name
 *   main_save_note
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   void main_save_note(const gchar * module_name, 
 *				          const gchar * key_str , 
 *				          const gchar * note_str )	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_save_note(	const gchar * module_name, 
			const gchar * key_str, 
			const gchar * note_str )
{
	// Massage encoded spaces ("%20") back to real spaces.
	// This is a sick. unreliable hack that should be removed
	// after the underlying problem is fixed in Sword.
	gchar *rework;
	for (rework = strstr(note_str, "%20");
	     rework;
	     rework = strstr(rework+1, "%20")) {
		*rework = ' ';
		(void) strcpy(rework+1, rework+3);
	}

#ifdef USE_GTKHTML38
	backend->set_module_key(module_name, key_str);
	GS_message(("note module %s\nnote key %s\nnote text%s",
		    module_name,
		    key_str,
		    note_str));
	backend->save_entry(note_str);
#endif
	if ((!strcmp(settings.CommWindowModule, module_name)) &&  
	    (!strcmp(settings.currentverse, key_str)))
		main_display_commentary(module_name, key_str);
}

/******************************************************************************
 * Name
 *   main_delete_note
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   void main_delete_note(DIALOG_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_delete_note(	const gchar * module_name, const gchar * key_str)
{
#ifdef USE_GTKHTML38
	backend->set_module_key(module_name, key_str);
	GS_message(("note module %s\nnote key %s\n",
		    module_name,
		    key_str));
	backend->delete_entry();
#endif	
	if ((!strcmp(settings.CommWindowModule,module_name)) &&  
	    (!strcmp(settings.currentverse, key_str)))
		main_display_commentary(module_name, key_str);
}

	
/******************************************************************************
 * Name
 *  set_module_unlocked
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void set_module_unlocked(char *mod_name, char *key)	
 *
 * Description
 *   unlocks locked module - 
 *
 * Return value
 *   void
 */

void main_set_module_unlocked(char * mod_name, char * key)
{
	SWMgr *mgr = backend->get_display_mgr();
	mgr->setCipherKey(mod_name, key);	
}


/******************************************************************************
 * Name
 *  main_save_module_key
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   void main_save_module_key(gchar * mod_name, gchar * key)	
 *
 * Description
 *    to unlock locked modules
 *
 * Return value
 *   void
 */ 

void main_save_module_key(char * mod_name, char * key)
{
	backend->save_module_key(mod_name, key);
}


/******************************************************************************
 * Name
 *   main_update_nav_controls
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   gchar *main_update_nav_controls(const gchar * key)	
 *
 * Description
 *   updates the nav toolbar controls 
 *
 * Return value
 *   gchar *
 */

gchar *main_update_nav_controls(const gchar * key)
{
	char *val_key = backend->get_valid_key(key);
	
	/* 
	 *  remember verse 
	 */
	xml_set_value("GnomeSword", "keys", "verse", val_key);
	settings.currentverse = xml_get_value("keys", "verse");
	
	settings.apply_change = FALSE;
#ifdef OLD_NAVBAR
	main_navbar_set(navbar_main, val_key);
#else
	navbar_versekey.module_name = g_string_assign(navbar_versekey.module_name,settings.MainWindowModule);
	
	navbar_versekey.key = g_string_assign(navbar_versekey.key,val_key);
	main_navbar_versekey_set(navbar_versekey, val_key);
#endif
	settings.apply_change = TRUE;
	return val_key;
}


/******************************************************************************
 * Name
 *  get_module_key
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   char *get_module_key(void)	
 *
 * Description
 *    returns module key
 *
 * Return value
 *   char *
 */

char *main_get_active_pane_key(void)
{
	if (settings.havebible) {
		switch (settings.whichwindow) {
		case MAIN_TEXT_WINDOW:
		case COMMENTARY_WINDOW:
			return (char *) settings.currentverse;
			break;
		case DICTIONARY_WINDOW:
			return (char *) settings.dictkey;
			break;
		case parallel_WINDOW:
			return (char *) settings.cvparallel;
			break;
		case BOOK_WINDOW:
			return (char *) settings.book_key;
			break;
		}
	}
	return NULL;
}

/******************************************************************************
 * Name
 *  get_module_name
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   char *get_module_name(void)	
 *
 * Description
 *    returns module name
 *
 * Return value
 *   char *
 */

char *main_get_active_pane_module(void)
{
	if (settings.havebible) {
		switch (settings.whichwindow) {
		case MAIN_TEXT_WINDOW:
			return (char *) xml_get_value("modules",
						       "bible");
			break;
		case COMMENTARY_WINDOW:
			return (char *) xml_get_value("modules",
						       "comm");
			break;
		case DICTIONARY_WINDOW:
			return (char *) settings.DictWindowModule;
			break;
		case BOOK_WINDOW:
			return (char *) settings.book_mod;
			break;
		}
	}
	return NULL;
}

/******************************************************************************
 * Name
 *  module_name_from_description
 *
 * Synopsis
 *   #include ".h"
 *
 *   void module_name_from_description(gchar *mod_name, gchar *description)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

char *main_module_name_from_description(char *description)
{
	return backend->module_name_from_description(description);
}

/******************************************************************************
 * Name
 *  main_get_sword_version
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   const char *main_get_sword_version(void)	
 *
 * Description
 *    
 *
 * Return value
 *   const char *
 */ 

const char *main_get_sword_version(void)
{
	return backend->get_sword_version();
}



/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *main_get_treekey_local_name(unsigned long offset)
{
	return backend->treekey_get_local_name(offset);
}


/******************************************************************************
 * Name
 *   get_search_results_text
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *get_search_results_text(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *main_get_search_results_text(char * mod_name, char * key)	
{
	return g_strdup(backend->get_render_text((char *)mod_name,(char *)key));
}



/******************************************************************************
 * Name
 *  main_get_path_to_mods
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   	gchar *main_get_path_to_mods(void)
 *
 * Description
 *    returns the path to the sword modules
 *
 * Return value
 *   gchar *
 */ 

char *main_get_path_to_mods(void)
{
	SWMgr *mgr = backend->get_main_mgr();
	char *path = mgr->prefixPath;
	return (path ? g_strdup(path) : NULL);
}

/******************************************************************************
 * Name
 *  main_init_language_map
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void main_init_language_map(void)
 *
 * Description
 *   initializes the hard-coded abbrev->name mapping.
 *
 * Return value
 *   void
 */ 

typedef std::map < SWBuf, SWBuf > ModLanguageMap;
ModLanguageMap languageMap;

void main_init_language_map() {
	/* --list from Bibletime-1.3-- */

        // language names already in their native character sets
        // are no longer locale-sensitive within _() macros.

	languageMap[SWBuf("aa")]  = SWBuf("Qafár af");		// "Afar"
	languageMap[SWBuf("ab")]  = SWBuf("Аҧсуа");		// "Abkhazian"
	languageMap[SWBuf("ae")]  = SWBuf(_("Avestan"));
	languageMap[SWBuf("af")]  = SWBuf(_("Afrikaans"));
	languageMap[SWBuf("am")]  = SWBuf("አማርኛ");		// "Amharic"
	languageMap[SWBuf("ang")] = SWBuf("English, Old (ca.450-1100)");
	languageMap[SWBuf("ar")]  = SWBuf("العربية");		// "Arabic"
	languageMap[SWBuf("as")]  = SWBuf("অসমীয়া");		// "Assamese"
	languageMap[SWBuf("ay")]  = SWBuf("Aymar aru");		// "Aymara"
	languageMap[SWBuf("az")]  = SWBuf("Azərbaycan");		// "Azerbaijani"
	languageMap[SWBuf("ba")]  = SWBuf("Башҡорт");		// "Bashkir"
	languageMap[SWBuf("be")]  = SWBuf("Беларуская");		// "Belarusian"
	languageMap[SWBuf("bg")]  = SWBuf("Български");		// "Bulgarian"
	languageMap[SWBuf("bh")]  = SWBuf("अङ्गिका");		// "Bihari"
	languageMap[SWBuf("bi")]  = SWBuf(_("Bislama"));
	languageMap[SWBuf("bn")]  = SWBuf("বাংলা");		// "Bengali"
	languageMap[SWBuf("bo")]  = SWBuf("བོད་ཡིག");		// "Tibetan"
	languageMap[SWBuf("br")]  = SWBuf("Brezhoneg");		// "Breton"
	languageMap[SWBuf("bs")]  = SWBuf("Bosanski");		// "Bosnian"
	languageMap[SWBuf("ca")]  = SWBuf("Català");		// "Catalan"
	languageMap[SWBuf("ce")]  = SWBuf("Нохчийн мотт");	// "Chechen"
	languageMap[SWBuf("ceb")] = SWBuf("Sinugboanon");	// "Cebuano" (cf. wikipedia)
	languageMap[SWBuf("ch")]  = SWBuf("Chamoru");		// "Chamorro"
	languageMap[SWBuf("co")]  = SWBuf("Lingua corsa");	// "Corsican"
	languageMap[SWBuf("cop")] = SWBuf(_("Coptic"));		// "ⲙⲛⲧⲣⲙⲛⲕⲏⲙⲉ"
								// (missing font: untranslated)
	languageMap[SWBuf("cs")]  = SWBuf("Česky");		// "Czech"
	languageMap[SWBuf("cu")]  = SWBuf("Ѩзыкъ словѣньскъ");	// "Church Slavic"
	languageMap[SWBuf("cv")]  = SWBuf("Чăвашла");		// "Chuvash"
	languageMap[SWBuf("cy")]  = SWBuf("Cymraeg");		// "Welsh"
	languageMap[SWBuf("da")]  = SWBuf("Dansk");		// "Danish"
	languageMap[SWBuf("de")]  = SWBuf("Deutsch");		// "German"
	languageMap[SWBuf("dz")]  = SWBuf("རྫོང་ཁ");		// "Dzongkha"
	languageMap[SWBuf("el")]  = SWBuf("Ελληνικά ·(1453-)");	// "Greek, Modern (1453-)"
	languageMap[SWBuf("en")]  = SWBuf("English");
	languageMap[SWBuf("en_US")] = SWBuf("American English");
/*  */	languageMap[SWBuf("enm")] = SWBuf("English, Middle (1100-1500)");
	languageMap[SWBuf("eo")]  = SWBuf("Esperanto");
	languageMap[SWBuf("es")]  = SWBuf("Español");		// "Spanish"
	languageMap[SWBuf("et")]  = SWBuf("Eesti");		// "Estonian"
	languageMap[SWBuf("eu")]  = SWBuf("Euskara");		// "Basque"
	languageMap[SWBuf("fa")]  = SWBuf("فارسی");		// "Farsi (Persian)"
	languageMap[SWBuf("fi")]  = SWBuf("Suomi");		// "Finnish"
	languageMap[SWBuf("fj")]  = SWBuf("Na vosa vaka-Viti");	// "Fijian"
	languageMap[SWBuf("fo")]  = SWBuf("Føroyskt");		// "Faroese"
	languageMap[SWBuf("fr")]  = SWBuf("Français");		// "French"
	languageMap[SWBuf("fy")]  = SWBuf("Frysk");		// "Frisian"
	languageMap[SWBuf("ga")]  = SWBuf("Gaeilge");		// "Irish"
	languageMap[SWBuf("gd")]  = SWBuf("Gàidhlig");		// "Gaelic (Scots)"
	languageMap[SWBuf("gl")]  = SWBuf("galego");		// "Gallegan"
	languageMap[SWBuf("gn")]  = SWBuf("avañe'ẽ");		// "Guarani"
	languageMap[SWBuf("gn")]  = SWBuf("ગુજરાતી");		// "Gujarati"
/*  */	languageMap[SWBuf("got")] = SWBuf(_("Gothic"));		// "𐌲𐌿𐍄𐌹𐍃𐌺𐌰 𐍂𐌰𐌶𐌳𐌰"
								// (missing font: untranslated)
	languageMap[SWBuf("gv")]  = SWBuf("Gaelg");		// "Manx"
	languageMap[SWBuf("grc")] = SWBuf("Κοινὴ Ἑλληνική");	// "Greek, Ancient (to 1453)"
	languageMap[SWBuf("he")]  = SWBuf("עברית");		// "Hebrew"
	languageMap[SWBuf("haw")] = SWBuf("Hawai`i");		// "Hawaiian"
	languageMap[SWBuf("hi")]  = SWBuf("हिन्दी");		// "Hindi"
	languageMap[SWBuf("ho")]  = SWBuf("Hiri Motu");		// "Hiri Motu"
	languageMap[SWBuf("hr")]  = SWBuf("Hrvatski");		// "Croatian"
	languageMap[SWBuf("ht")]  = SWBuf("Krèyol ayisyen");	// "French, Haitian Creole"
	languageMap[SWBuf("hu")]  = SWBuf("Magyar");		// "Hungarian"
	languageMap[SWBuf("hy")]  = SWBuf("Հայերեն");		// "Armenian"
	languageMap[SWBuf("hz")]  = SWBuf("Otjiherero");	// "Herero"
	languageMap[SWBuf("ia")]  = SWBuf(_("Interlingua"));
	languageMap[SWBuf("ie")]  = SWBuf(_("Interlingue"));
	languageMap[SWBuf("id")]  = SWBuf("Bahasa Indonesia");	// "Indonesian"
/*  */	languageMap[SWBuf("ik")]  = SWBuf("Iñupiak");		// "Inupiaq"
	languageMap[SWBuf("is")]  = SWBuf("Íslenska");		// "Icelandic"
	languageMap[SWBuf("it")]  = SWBuf("Italiano");		// "Italian"
	languageMap[SWBuf("iu")]  = SWBuf("ᐃᓄᒃᑎᑐᑦ");		// "Inuktitut"
	languageMap[SWBuf("ja")]  = SWBuf("日本語");		// "Japanese"
	languageMap[SWBuf("ka")]  = SWBuf("ქართული");		// "Georgian"
	languageMap[SWBuf("kek")] = SWBuf("Q'eqchi'");		// "Kekchi"
	languageMap[SWBuf("ki")]  = SWBuf("Gĩkũyũ");	// "Kikuyu"
	languageMap[SWBuf("kj")]  = SWBuf("Oshikwanyama");	// "Kuanyama"
	languageMap[SWBuf("kk")]  = SWBuf("Қазақша");		// "Kazakh"
	languageMap[SWBuf("kl")]  = SWBuf(_("Kalaallisut"));
	languageMap[SWBuf("km")]  = SWBuf("ភាសាខ្មែរ");		// "Khmer"
	languageMap[SWBuf("kn")]  = SWBuf("ಕನ್ನಡ");		// "Kannada"
	languageMap[SWBuf("ko")]  = SWBuf("한국어");	// "Korean"
	languageMap[SWBuf("ks")]  = SWBuf("कश्मीरी - (كشميري)");	// "Kashmiri"
	languageMap[SWBuf("ku")]  = SWBuf("Kurdî / كوردي");	// "Kurdish"
	languageMap[SWBuf("kv")]  = SWBuf("Коми");		// "Komi"
	languageMap[SWBuf("kw")]  = SWBuf("Kernewek");		// "Cornish"
	languageMap[SWBuf("ky")]  = SWBuf("Հայերեն");		// "Kirghiz"
	languageMap[SWBuf("la")]  = SWBuf("Latina");		// "Latin"
	languageMap[SWBuf("lb")]  = SWBuf("Lëtzebuergesch");	// "Letzeburgesch"
	languageMap[SWBuf("ln")]  = SWBuf("Lingála");		// "Lingala"
	languageMap[SWBuf("lo")]  = SWBuf("ລາວ");		// "Lao"
	languageMap[SWBuf("lt")]  = SWBuf("Lietuvių");	// "Lithuanian"
	languageMap[SWBuf("lv")]  = SWBuf("Latviešu");		// "Latvian"
	languageMap[SWBuf("mg")]  = SWBuf("Malagasy");		// "Malagasy"
	languageMap[SWBuf("mh")]  = SWBuf("Kajin M̧ajeļ");	// "Marshall"
	languageMap[SWBuf("mi")]  = SWBuf("Māori");		// "Maori"
	languageMap[SWBuf("mk")]  = SWBuf("Македонски");		// "Macedonian"
	languageMap[SWBuf("ml")]  = SWBuf("മലയാളം");		// "Malayalam"
	languageMap[SWBuf("mn")]  = SWBuf("Монгол");		// "Mongolian"
	languageMap[SWBuf("mo")]  = SWBuf("лимба молдовеняскэ");	// "Moldavian"
	languageMap[SWBuf("mr")]  = SWBuf("मराठी");		// "Marathi"
	languageMap[SWBuf("ms")]  = SWBuf("Bahasa Melayu");	// "Malay"
	languageMap[SWBuf("mt")]  = SWBuf("Malti");		// "Maltese"
	languageMap[SWBuf("my")]  = SWBuf(_("Burmese"));	// "မြန်မာစာ"
								// (missing font: untranslated)
	languageMap[SWBuf("na")]  = SWBuf("Ekakairũ Naoero");	// "Nauru"
#if 0
	// correct.
	languageMap[SWBuf("nb")]  = SWBuf("Bokmål");		// "Norwegian Bokmål"
#else
	// a hack while g_utf8_validate() dislikes 'å'.
	languageMap[SWBuf("nb")]  = SWBuf("Bokmaal");		// "Norwegian Bokmaal"
#endif
/*  */	languageMap[SWBuf("nd")]  = SWBuf("Sindebele");		// "Ndebele, North"
	languageMap[SWBuf("nds")] = SWBuf("Plattdüütsch");	// "Low German; Low Saxon"
	languageMap[SWBuf("ne")]  = SWBuf("नेपाली");		// "Nepali"
	languageMap[SWBuf("ng")]  = SWBuf("Oshiwambo");		// "Ndonga"
	languageMap[SWBuf("nl")]  = SWBuf("Nederlands");	// "Dutch"
	languageMap[SWBuf("nn")]  = SWBuf("Nynorsk");		// "Norwegian Nynorsk"
#if 0
	// correct.
	languageMap[SWBuf("no")]  = SWBuf("Norsk (bokmål)");	// "Norwegian"
#else
	// a hack while g_utf8_validate() dislikes 'å'.
	languageMap[SWBuf("no")]  = SWBuf("Norsk (bokmaal)");	// "Norwegian"
#endif
	languageMap[SWBuf("nr")]  = SWBuf("Nrebele");		// "Ndebele, South"
	languageMap[SWBuf("nv")]  = SWBuf("Diné bizaad");	// "Navajo"
	languageMap[SWBuf("ny")]  = SWBuf("Chicheŵa");		// "Chichewa; Nyanja"
	languageMap[SWBuf("oc")]  = SWBuf("Occitan");		// "Occitan (post 1500); Proven"
	languageMap[SWBuf("om")]  = SWBuf("Oromoo");		// "Oromo"
	languageMap[SWBuf("or")]  = SWBuf("ଓଡ଼ିଆ");		// "Oriya"
	languageMap[SWBuf("os")]  = SWBuf("Иронау");		// "Ossetian; Ossetic"
	languageMap[SWBuf("pa")]  = SWBuf("ਪੰਜਾਬੀ پنجابی");	// "Panjabi"
	languageMap[SWBuf("pap")] = SWBuf("Papiamentu");	// "Papiamento"
	languageMap[SWBuf("pi")]  = SWBuf("Pāḷi");		// "Pali"
	languageMap[SWBuf("pl")]  = SWBuf("Polski");		// "Polish"
	languageMap[SWBuf("pot")] = SWBuf("Neshnabémwen");	// "Potawatomi" (cf. wikipedia)
	languageMap[SWBuf("ppk")] = SWBuf(_("Uma"));
	languageMap[SWBuf("ps")]  = SWBuf("پښتو");		// "Pushto"
	languageMap[SWBuf("pt")]  = SWBuf("Português");		// "Portuguese"
	languageMap[SWBuf("qu")]  = SWBuf("Quechua");		// "Quechua"
	languageMap[SWBuf("rm")]  = SWBuf("rumantsch");		// "Raeto-Romance"
	languageMap[SWBuf("rn")]  = SWBuf("Kirundi");		// "Rundi"
	languageMap[SWBuf("ro")]  = SWBuf("Română");		// "Romanian"
	languageMap[SWBuf("ru")]  = SWBuf("Русский");		// "Russian"
	languageMap[SWBuf("rw")]  = SWBuf(_("Kinyarwanda"));
	languageMap[SWBuf("sa")]  = SWBuf("संस्कृत");		// "Sanskrit"
	languageMap[SWBuf("sc")]  = SWBuf("Sardu");		// "Sardinian"
	languageMap[SWBuf("sco")] = SWBuf("Scots");
	languageMap[SWBuf("sd")]  = SWBuf("سنڌي");		// "Sindhi"
	languageMap[SWBuf("se")]  = SWBuf("Sámegiella");	// "Northern Sami"
	languageMap[SWBuf("sg")]  = SWBuf("Sängö");		// "Sango"
	languageMap[SWBuf("si")]  = SWBuf("සිංහල");		// "Sinhalese"
	languageMap[SWBuf("sk")]  = SWBuf("Slovenčina");	// "Slovak"
	languageMap[SWBuf("sl")]  = SWBuf("Slovenščina");	// "Slovenian"
	languageMap[SWBuf("sm")]  = SWBuf("Gagana Samoa");	// "Samoan"
/*  */	languageMap[SWBuf("sn")]  = SWBuf("chiShona");		// "Shona"
	languageMap[SWBuf("so")]  = SWBuf("Soomaaliga");	// "Somali"
	languageMap[SWBuf("sq")]  = SWBuf("Shqip");		// "Albanian"
	languageMap[SWBuf("sr")]  = SWBuf("srpski jezik");	// "Serbian"
	languageMap[SWBuf("ss")]  = SWBuf("siSwati");		// "Swati"
	languageMap[SWBuf("st")]  = SWBuf("Sesotho");		// "Sotho, Southern"
	languageMap[SWBuf("su")]  = SWBuf("Basa Sunda");	// "Sundanese"
	languageMap[SWBuf("sv")]  = SWBuf("Svenska");		// "Swedish"
	languageMap[SWBuf("sw")]  = SWBuf("Kiswahili");		// "Swahili"
	languageMap[SWBuf("syr")] = SWBuf("ܣܘܪܝܝܐ");		// "Syriac"
	languageMap[SWBuf("ta")]  = SWBuf("தமிழ்");		// "Tamil"
	languageMap[SWBuf("te")]  = SWBuf("తెలుగు");		// "Telugu"
	languageMap[SWBuf("tg")]  = SWBuf("تاجیکی");		// "Tajik"
	languageMap[SWBuf("th")]  = SWBuf("ไทย");		// "Thai"
	languageMap[SWBuf("tk")]  = SWBuf("Türkmen");		// "Turkmen"
	languageMap[SWBuf("tl")]  = SWBuf("Tagalog");
	languageMap[SWBuf("tlh")] = SWBuf("tlhIngan Hol");	// "Klingon"
	languageMap[SWBuf("tn")]  = SWBuf("Setswana");		// "Tswana"
	languageMap[SWBuf("tr")]  = SWBuf("Türkçe");		// "Turkish"
	languageMap[SWBuf("ts")]  = SWBuf("Xitsonga");		// "Tsonga"
	languageMap[SWBuf("tt")]  = SWBuf("Татарча");		// "Tatar"
	languageMap[SWBuf("tw")]  = SWBuf(_("Twi"));
	languageMap[SWBuf("ty")]  = SWBuf("Reo Mā`ohi");	// "Tahitian"
	languageMap[SWBuf("ug")]  = SWBuf("ئۇيغۇرچە");		// "Uighur"
	languageMap[SWBuf("uk")]  = SWBuf("Українська");		// "Ukrainian"
	languageMap[SWBuf("ur")]  = SWBuf("اُردو");		// "Urdu"
	languageMap[SWBuf("uz")]  = SWBuf("O'zbek");		// "Uzbek"
	languageMap[SWBuf("vi")]  = SWBuf("Tiếng Việt");		// "Vietnamese"
	languageMap[SWBuf("vo")]  = SWBuf("Volapük");		// "Volapk"
/*  */	languageMap[SWBuf("wo")]  = SWBuf("Wolof");		// "Wolof"
	languageMap[SWBuf("xh")]  = SWBuf("isiXhosa");		// "Xhosa" -- causes gettext problems
	languageMap[SWBuf("x-E-BAR")] = SWBuf("Boarisch");	// "Bavarian"
	languageMap[SWBuf("x-E-GSW")] = SWBuf(_("Alemannisch"));
	languageMap[SWBuf("x-E-ITZ")] = SWBuf("Itza'");		// "Itz"
	languageMap[SWBuf("x-E-JIV")] = SWBuf(_("Shuar"));
	languageMap[SWBuf("x-E-LMO")] = SWBuf("Lumbaart");	// "Lombard"
	languageMap[SWBuf("x-E-MKJ")] = SWBuf("Македонски");	// "Macedonian"
	languageMap[SWBuf("x-E-PDG")] = SWBuf("Tok Pisin");	// "Tok Pisin"
	languageMap[SWBuf("x-E-RMY")] = SWBuf("Romani,·Vlax");	// "Romani, Vlax"
	languageMap[SWBuf("x-E-SAJ")] = SWBuf("Sängö");		// "Sango"
	languageMap[SWBuf("x-E-SRN")] = SWBuf("nengre");	// "Sranan"
	languageMap[SWBuf("yi")]  = SWBuf("ייִדיש");		// "Yiddish"
	languageMap[SWBuf("za")]  = SWBuf("Saw cuengh");	// "Zhuang"
	languageMap[SWBuf("zh")]  = SWBuf("中文");		// "Chinese"
	languageMap[SWBuf("zu")]  = SWBuf("isiZulu");		// "Zulu"
}

const char *main_get_language_map(const char *language) {
	if (language == NULL)
		return "Unknown";
	return languageMap[language].c_str();
}


/******************************************************************************
 * Name
 *   backend_init
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   void main_init_backend(void)	
 *
 * Description
 *   start sword 
 *
 * Return value
 *   void
 */

void main_init_backend(void)
{
	char *sys_locale = NULL;
	const char *lang = getenv("LANG");
	SWMgr mgr;
	
	if (!lang) lang="C";

	StringMgr::setSystemStringMgr( new GS_StringMgr() );

	backend = new BackEnd();
	backend->init_SWORD(0);
	sword_locale = backend->set_sword_locale(lang);
	GS_print(("%s sword-%s\n", _("Starting"), backend->get_sword_version()));
	GS_print(("%s\n", _("Initiating SWORD")));
	GS_print(("%s: %s\n",_("path to sword"),mgr.prefixPath));
	GS_print(("%s %s\n", _("System locale is"),lang));
	GS_print(("%s %s\n", _("SWORD locale is"), sword_locale));
	GS_print(("OLD_CODESET = %s\n\n", OLD_CODESET));
	GS_print(("%s\n", _("Checking for SWORD Modules")));
	sys_locale = strdup(lang);
	settings.spell_language = strdup(sys_locale);
	main_init_lists();
	free(sys_locale);
}


/******************************************************************************
 * Name
 *   shutdown_sword
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void shutdown_sword(void)	
 *
 * Description
 *   close down sword by deleting backend;
 *
 * Return value
 *   void
 */

void main_shutdown_backend(void)
{
	if(sword_locale)
		free((char*)sword_locale);
	sword_locale = NULL;
	if (backend) 
	        delete backend; 
	backend = NULL;
		
	GS_print(("%s\n", _("SWORD is shutdown")));
}

/******************************************************************************
 * Name
 *   main_locked_module_display
 *
 * Synopsis
 *   #include ".h"
 *
 *   main_locked_module_display(GtkWidget * html_widget,
				  gchar * mod_name, gchar * cipher_key)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
/*
void main_locked_module_display(gpointer data,
				  char * mod_name, char * cipher_key)
{
	GtkWidget *html_widget = (GtkWidget *) data;
	GtkHTML *html = GTK_HTML(html_widget);
	GtkHTMLStreamStatus status1;
	GtkHTMLStream *htmlstream;
	gchar buf[500];
	GString *str;

	str = g_string_new("");
	htmlstream =
	    gtk_html_begin(html);

	g_string_printf(str, "%s", HTML_START "<body><br>");
	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
	}

	if (!cipher_key) {
		g_string_printf(str, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name, "</b>", _("module is locked."));
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	} else {
		g_string_printf(str, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name,
			"</b>", _("module has been unlocked."));
		
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	}

	if (!cipher_key) {
		g_string_printf(str, "%s %s %s%s%s %s %s %s",
			"<br><br>",
			_("If you have the cipher key you can"),
			"<a href=\"U",
			mod_name,
			"\">",
			_("click here"),
			" </a>", _("to unlock the module"));
		
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}

		g_string_printf(str, "%s%s",
			"<br><br>",
			_
			("You will need to restart GnomeSword after you unlock it."));

		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	} else {
		g_string_printf(str, "%s%s",
			"<br><br>",
			_("You need to restart GnomeSword to view it"));
		
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	}

	g_string_printf(str, "%s", "</body></html>");
	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	g_string_free(str,TRUE);

}
*/
/******************************************************************************
 * Name
 *   main_dictionary_entry_changed
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   void main_dictionary_entry_changed(char * mod_name)
 *
 * Description
 *   text in the dictionary entry has changed and the entry activated
 *
 * Return value
 *   void
 */

void main_dictionary_entry_changed(char * mod_name)
{	
	gchar *key = NULL;
/*	gchar *key2 = NULL;
	gsize bytes_read;	
	gsize bytes_written;	
	GError **error = NULL;    */

	if (!mod_name) 
		return;
	if (strcmp(settings.DictWindowModule, mod_name)) {
		xml_set_value("GnomeSword", "modules", "dict", mod_name);
		settings.DictWindowModule = xml_get_value("modules", "dict");
	}	

	key = g_strdup((gchar*)gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict)));
	
//	key2 = g_utf8_strup(key, strlen(key));
/*	key2 = g_convert(  key,	
			     -1,	
			     UTF_8,	
			     OLD_CODESET,	
			     &bytes_read,	
			     &bytes_written,
			     error);	   */	

//	backend->set_module_key(mod_name, key2);
	backend->set_module_key(mod_name, key);
//	g_free(key2);
	g_free(key);
	key = backend->get_module_key();

/*	key2 = g_convert(  key,	
			     -1,	
			     OLD_CODESET,	
			     UTF_8,	
			     &bytes_read,	
			     &bytes_written,
			     error);	   */
		
	xml_set_value("GnomeSword", "keys", "dictionary", key);
	settings.dictkey = xml_get_value("keys", "dictionary");
	
	backend->set_module_key(mod_name, key);
	backend->display_mod->Display();
	
//	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key2);
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);
	g_free(key);
//	g_free(key2);
} 



static void dict_key_list_select(GtkMenuItem * menuitem, gpointer user_data)
{
/*	gchar *buf;                                       
	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;	*/

/*	buf = g_convert((char*)(gchar*) user_data,
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);
	GS_message(("\nuser_data: %s\nbuf: %s",(gchar*) user_data,buf));  */
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), (gchar*) user_data);
	gtk_widget_activate(widgets.entry_dict);
//	g_free(buf);
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   
 *
 * Description
 *   text in the dictionary entry has changed and the entry activated
 *
 * Return value
 *   void
 */

GtkWidget *main_dictionary_drop_down_new(char * mod_name, char * old_key)
{	
	gint count = 9, i;
	gchar *new_key, *text = NULL;
	gchar *key = NULL;
//	gchar *key2 = NULL;
	gint height;
	GtkWidget *menu;
	GtkWidget * item;
/*	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;    */
	
	menu = gtk_menu_new();
	
	if (!settings.havedict || !mod_name)
		return NULL;
	if (strcmp(settings.DictWindowModule,mod_name)) {
		xml_set_value("GnomeSword", "modules", "dict",
					mod_name);
		settings.DictWindowModule = xml_get_value(
					"modules", "dict");
	}	
	key = g_strdup((gchar*)gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict)));
	
	GS_message(("\nold_key: %s\nkey: %s",old_key,key));
//	key2 = g_utf8_strup(key,strlen(key));
/*	key2 = g_convert(  key,	
			     -1,	
			     UTF_8,	
			     OLD_CODESET,	
			     &bytes_read,	
			     &bytes_written,
			     error);    */

//	backend->set_module_key(mod_name, key2);
	backend->set_module_key(mod_name, key);
//	g_free(key2);
	g_free(key);
	key = backend->get_module_key();
	
	xml_set_value("GnomeSword", "keys", "dictionary", key);
	settings.dictkey = xml_get_value("keys", "dictionary");
	
	backend->set_module_key(mod_name, key);
	backend->display_mod->Display();
	/*new_key = g_locale_to_utf8((char*)backend->display_mod->KeyText(),
                                             -1,
                                             NULL,
                                             &bytes_written,
                                             NULL);*/
	
	new_key = g_strdup((char*)backend->display_mod->KeyText());
	
	for (i = 0; i < (count / 2)+1; i++) {
		//free(new_key);
		(*backend->display_mod)--;
		/*new_key = g_locale_to_utf8((char*)backend->display_mod->KeyText(),
                                             -1,
                                             NULL,
                                             &bytes_written,
                                             NULL);*/
		//new_key = g_strdup((char*)backend->display_mod->KeyText());
	}

	for (i = 0; i < count; i++) {
		free(new_key);			
		(*backend->display_mod)++;
		/*new_key = g_locale_to_utf8((char*)backend->display_mod->KeyText(),
                                             -1,
                                             NULL,
                                             &bytes_written,
                                             NULL);*/
		new_key = g_strdup((char*)backend->display_mod->KeyText());
		/* add menu item */
		item =
		    gtk_menu_item_new_with_label((gchar *) new_key);
		gtk_widget_show(item);
		g_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK(dict_key_list_select),
				   g_strdup(new_key));		
		gtk_container_add(GTK_CONTAINER(menu), item); 
	}
	
	free(new_key);
	g_free(key);
	return menu;
} 

/******************************************************************************
 * Name
 *   main_dictionary_button_clicked
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   void main_dictionary_button_clicked(gint direction)
 *
 * Description
 *   The back or foward dictinary key button was clicked.
 *   the module key is set to the current dictkey.
 *   then the module is incremented or decremented.
 *   the new key is returned from the module and the dictionary entry is set
 *   to the new key. The entry is then activated.
 *
 * Return value
 *   void
 */


void main_dictionary_button_clicked(gint direction)
{	
	gchar *key = NULL;
	
	if (!settings.havedict || !settings.DictWindowModule)
		return;
		
	backend->set_module_key(settings.DictWindowModule, 
				settings.dictkey);
	if (direction == 0)
		(*backend->display_mod)--;
	else
		(*backend->display_mod)++;
	key = g_strdup((char*)backend->display_mod->KeyText());	
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);
	gtk_widget_activate(widgets.entry_dict);
	g_free(key);
}

void main_display_book(const char * mod_name, const char * key)     //, unsigned long offset)
{
	GS_message(("main_display_book\nmod_name: %s\nkey: %s", mod_name, key));

	if (!settings.havebook || !mod_name)
		return;
	if (!backend->is_module(mod_name))
		return;
	if (!settings.book_mod)
		settings.book_mod = (char*)mod_name;

	if (strcmp(settings.book_mod, mod_name)) {
		xml_set_value("GnomeSword", "modules", "book", mod_name);
		settings.book_mod = xml_get_value("modules", "book");
	}
	
	if (key == NULL)  // && offset == -1)
		key = "0";

	if (!isdigit(key[0])) {
		xml_set_value("GnomeSword", "keys", "book", key);
		settings.book_key = xml_get_value("keys", "book");

		backend->set_module(mod_name);
		backend->set_treekey(0);
		settings.book_offset = backend->treekey_set_key((char*)key);
	} else {
		settings.book_offset = atol(key);
		if(settings.book_offset < 4)
			settings.book_offset = 4;
		xml_set_value("GnomeSword", "keys", "book", key);
		settings.book_key = xml_get_value("keys", "book");
		xml_set_value("GnomeSword", "keys", "offset", key);

		backend->set_module(mod_name);	
		backend->set_treekey(settings.book_offset);
	}
	
	backend->display_mod->Display();
	main_setup_navbar_book(settings.book_mod, settings.book_offset);
	if (settings.browsing)
		gui_update_tab_struct(NULL,
				      NULL,
				      NULL,
				      mod_name,
				      NULL,
				      key,
				      FALSE,
				      settings.showtexts,
				      settings.showpreview,
				      settings.showcomms,
				      settings.showdicts);
}

void main_display_commentary(const char * mod_name, const char * key)
{	
	if (!settings.havecomm || !settings.comm_showing)
		return;
	
	if (!mod_name)
		mod_name = xml_get_value("modules", "comm");
	
	if (!mod_name || !backend->is_module(mod_name))
		return;
	if (!settings.CommWindowModule)
		settings.CommWindowModule = (char*)mod_name;
	
	settings.comm_showing = TRUE;
	settings.whichwindow = COMMENTARY_WINDOW;
						
	if (strcmp(settings.CommWindowModule,mod_name)) {
		xml_set_value("GnomeSword", "modules", "comm", mod_name);
		settings.CommWindowModule = xml_get_value( "modules", "comm");
	}
	
	backend->set_module_key(mod_name, key);
	backend->display_mod->Display();
	
	if (settings.browsing)
		gui_update_tab_struct(NULL,
				      mod_name,
				      NULL,
				      NULL,
				      NULL,
				      NULL,
				      TRUE,
				      settings.showtexts,
				      settings.showpreview,
				      settings.showcomms,
				      settings.showdicts);
}

void main_display_dictionary(const char * mod_name, const char * key)
{
	const gchar *old_key;

	// for devotional use.
	gchar buf[10];

	GS_message(("main_display_dictionary\nmod_name: %s\nkey: %s", mod_name, key));

	if (!settings.havedict || !mod_name)
		return;
	if (!backend->is_module(mod_name))
		return;
	if (!settings.DictWindowModule)
		settings.DictWindowModule = (char*)mod_name;

	if (key == NULL)
		key = (char*)"Grace";

	if (strcmp(settings.DictWindowModule, mod_name)) {
		// new dict -- is it actually a devotional?
		time_t curtime;
		struct tm *loctime;
		char *feature;
		if ((feature = (char *)backend->get_main_mgr()->
					getModule(mod_name)->
					getConfigEntry("Feature")) &&
		    !strcmp(feature, "DailyDevotion")) {
			if ((strlen(key) != 5) ||		// blunt tests.
			    (key[0] < '0') || (key[0] > '9') ||
			    (key[1] < '0') || (key[1] > '9') ||
			    (key[2] != '.')                  ||
			    (key[3] < '0') || (key[3] > '9') ||
			    (key[4] < '0') || (key[4] > '9')) {	// not MM.DD
				curtime = time(NULL);
				loctime = localtime(&curtime);
				strftime(buf, 10, "%m.%d", loctime);
				key = buf;
			}
		}
		xml_set_value("GnomeSword", "modules", "dict", mod_name);
		settings.DictWindowModule = xml_get_value("modules", "dict");
	}
	
	 // old_key is uppercase
 	key = g_utf8_strup(key, -1);
	old_key = gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict));
	if (!strcmp(old_key, key))
		main_dictionary_entry_changed(settings.DictWindowModule);
	else {
		gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);
		gtk_widget_activate(widgets.entry_dict);
	}
	
	if (settings.browsing)
		gui_update_tab_struct(NULL,
				      NULL,
				      mod_name,
				      NULL,
				      key,
				      NULL,
				      settings.comm_showing,
				      settings.showtexts,
				      settings.showpreview,
				      settings.showcomms,
				      settings.showdicts);
}


void main_display_prev_verse(const char * mod_name, char * current_verse)
{
	char *url = NULL;
	char *key = NULL;
	
	backend->set_module_key(mod_name, current_verse);
	(*backend->display_mod)--;
	//backend->display_mod
	key = (char*)backend->display_mod->getKeyText();
	url = g_strdup_printf("sword://%s/%s", mod_name,key);
	main_url_handler(url, TRUE);
	g_free(url);
	gtk_widget_grab_focus(widgets.html_text);
/*	url = g_strdup_printf("gnomesword.url?action=showBookmark&"
					"type=%s&value=%s&module=%s",
					"newTab",
					main_url_encode(key), 
					main_url_encode(mod_name));*/
}


void main_display_next_verse(const char * mod_name, char * current_verse)
{
	char *url = NULL;
	char *key = NULL;
	
	backend->set_module_key(mod_name, current_verse);
	(*backend->display_mod)++;
	url = g_strdup_printf("sword://%s/%s", mod_name,key);
	main_url_handler(url, TRUE);
	g_free(url);
	gtk_widget_grab_focus(widgets.html_text);
}


void main_display_bible(const char * mod_name, const char * key)
{
	gchar *file = NULL;
	gchar *style = NULL;
	gchar *val_key = NULL;
	
#ifndef USE_GTKMOZEMBED
	extern guint scroll_adj_signal;
	extern GtkAdjustment* adjustment;
	
	/* keeps us out of a crash causing loop */	
	g_signal_handler_block(adjustment, scroll_adj_signal);
#endif	
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(widgets.html_text))) return;
	if (!mod_name)
		mod_name = xml_get_value("modules", "bible");	
	
	
	if (!settings.havebible || !mod_name)
		return;
	if (!backend->is_module(mod_name))
		return;
	
	if (!settings.MainWindowModule)
		settings.MainWindowModule = (char*)mod_name;
	
	if (strcmp(settings.currentverse, key)) {
		xml_set_value("GnomeSword", "keys", "verse",
					key);
		settings.currentverse = xml_get_value(
					"keys", "verse");
	}
	
	if (strcmp(settings.MainWindowModule, mod_name)) {
		xml_set_value("GnomeSword", "modules", "bible",
					mod_name);
		settings.MainWindowModule = xml_get_value(
					"modules", "bible");
#ifdef OLD_NAVBAR
		gui_reassign_strdup(&navbar_main.module_name, settings.MainWindowModule);
		gui_reassign_strdup(&navbar_main.key, settings.currentverse);
		main_navbar_fill_book_combo(navbar_main);
#else
		//if (navbar_versekey.module_name) 
			//g_free(navbar_versekey.module_name);
		navbar_versekey.module_name = g_string_assign(navbar_versekey.module_name,
							      settings.MainWindowModule);
		//if (navbar_versekey.key)
			//g_free(navbar_versekey.key);
		navbar_versekey.key = g_string_assign(navbar_versekey.key,
						      settings.currentverse);
#endif
		main_search_sidebar_fill_bounds_combos();
	}
	
	settings.whichwindow = MAIN_TEXT_WINDOW;
	gui_change_window_title(settings.MainWindowModule);
	
	file = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	style = get_conf_file_item(file, mod_name, "style");
	if ((style) && strcmp(style,"verse"))
		settings.versestyle = FALSE;
	else	
		settings.versestyle = TRUE;
	g_free(style);
	g_free(file);
	
	style_display = FALSE;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.versestyle_item),
				       settings.versestyle);
	style_display = TRUE;
	
	if (backend->module_has_testament(mod_name,
				backend->get_key_testament(key))) {
			backend->set_module_key(mod_name, key);
			backend->display_mod->Display();
	} else {
		if (backend->get_key_testament(key) == 1)
			val_key = main_update_nav_controls("Matthew 1:1");
		else
			val_key = main_update_nav_controls("Genesis 1:1");
		
		backend->set_module_key(mod_name, val_key);
		backend->display_mod->Display();
		g_free(val_key);			
	}
	
	GS_print(("mod_name = %s\n",mod_name));	
	if (settings.browsing) {
		gui_update_tab_struct(mod_name,
				      NULL,
				      NULL,
				      NULL,
				      NULL,
				      NULL,
				      settings.comm_showing,
				      settings.showtexts,
				      settings.showpreview,
				      settings.showcomms,
				      settings.showdicts);
		gui_set_tab_label(settings.currentverse, FALSE);
	}

	/*
	 * change parallel verses
	 */
	if (settings.dockedInt) 
		main_update_parallel_page();
	else
		gui_keep_parallel_dialog_in_sync();
#ifndef USE_GTKMOZEMBED
	g_signal_handler_unblock(adjustment, scroll_adj_signal);
#endif
}




/******************************************************************************
 * Name
 *   main_display_devotional
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   void main_display_devotional(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_display_devotional(void)
{
	gchar buf[10];
	time_t curtime;
	struct tm *loctime;
	gchar *text;

	/*
	 * This makes sense only if you've installed & defined one.
	 */
	if (settings.devotionalmod == NULL) {
		GList *glist = get_list(DEVOTION_LIST);

		if (g_list_length(glist) != 0) {
			xml_set_value("GnomeSword", "modules", "devotional",
				      (char*)glist->data );
			settings.devotionalmod = xml_get_value("modules",
							       "devotional");
		} else {
			gui_generic_warning(_("Daily devotional was requested, but there are none installed."));
		}
	}

	/*
	 * Get the current time.
	 */
	curtime = time(NULL);

	/*
	 * Convert it to local time representation.
	 */
	loctime = localtime(&curtime);

	/*
	 * Print it out in a nice format.
	 */
	strftime(buf, 10, "%m.%d", loctime);
	
	
	text = backend->get_render_text(settings.devotionalmod, buf);
//	g_message("modname: %s\nloctime: %s\ntext: %s",settings.devotionalmod, buf, text);
	if (text) {
		main_entry_display(sidebar.html_viewer_widget,
			      settings.devotionalmod, text, buf, TRUE);
		free(text);
	}
}

void main_refresh_all(void)
{ 
	main_display_bible(settings.MainWindowModule, settings.currentverse);
	main_display_commentary(settings.CommWindowModule, settings.currentverse);
	main_display_book(settings.book_mod, settings.book_key);	
	main_display_dictionary(settings.DictWindowModule, settings.dictkey);
}
    
void main_setup_displays(void)
{ 
/*
#ifdef USE_GTKMOZEMBED
	backend->textDisplay = new GtkMozChapDisp(widgets.html_text,backend);
//	backend->RTOLDisplay = new GtkMozChapDisp(widgets.html_text,backend);
	backend->commDisplay = new GTKMozEntryDisp(widgets.html_comm,backend);
	backend->bookDisplay = new GTKMozEntryDisp(widgets.html_book,backend);
	backend->dictDisplay = new GTKMozEntryDisp(widgets.html_dict,backend);

#else*/
#ifndef USE_GTKMOZEMBED
	backend->RTOLDisplay = new GTKTextviewChapDisp(widgets.textview,backend);
#endif
	backend->textDisplay = new GTKChapDisp(widgets.html_text,backend);
	backend->commDisplay = new GTKEntryDisp(widgets.html_comm,backend);
	backend->bookDisplay = new GTKEntryDisp(widgets.html_book,backend);
	backend->dictDisplay = new GTKEntryDisp(widgets.html_dict,backend);
//#endif
}

const char *main_get_module_language(const char *module_name)
{
	return backend->module_get_language(module_name);
}



/******************************************************************************
 * Name
 *  main_check_for_global_option
 *
 * Synopsis
 *   #include ".h"
 *
 *  	gint main_check_for_global_option(gchar * mod_name, gchar * option)
 *
 * Description
 *    get global options for a module
 *
 * Return value
 *   gint
 */

gint main_check_for_global_option(gchar * mod_name, gchar * option)
{
	return backend->has_global_option(mod_name, option);
}

/******************************************************************************
 * Name
 *   main_is_module
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   int main_is_module(char * mod_name)
 *
 * Description
 *    check for presents of a module by name
 *
 * Return value
 *   int
 */
 
int main_is_module(char * mod_name)
{
	return backend->is_module(mod_name);
}

/******************************************************************************
 * Name
 *   main_has_search_framework
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   int main_has_search_framework(char * mod_name)
 *
 * Description
 *    tells us whether CLucene is available
 *
 * Return value
 *   int (boolean)
 */

int main_has_search_framework(char *mod_name)
{
	SWMgr *mgr = backend->get_main_mgr();
	SWModule *mod = mgr->getModule(mod_name);
	return (mod && mod->hasSearchFramework());
}

/******************************************************************************
 * Name
 *   main_optimal_search
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   int main_optimal_search(char * mod_name)
 *
 * Description
 *    tells us whether a CLucene index exists
 *
 * Return value
 *   int (boolean)
 */

int main_optimal_search(char *mod_name)
{
	SWMgr *mgr = backend->get_main_mgr();
	SWModule *mod = mgr->Modules.find(mod_name)->second;
	return mod->isSearchOptimallySupported("God", -4, 0, 0);
}

/******************************************************************************
 * Name
 *  get_mod_about_info
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   gchar *get_mod_about_info(char * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   gchar *
 */ 

char *main_get_mod_about_info(char * mod_name)
{
	return backend->get_config_entry(mod_name, (char*)"About");
}

char *main_get_mod_config_entry(const char * module_name,
				      const char * entry)
{
	return backend->get_config_entry((char*)module_name, (char*)entry);
}

char *main_get_mod_config_file(const char * module_name,
				     const char * moddir)
{
#ifdef  SWORD_SHOULD_HAVE_A_WAY_TO_GET_A_CONF_FILENAME_FROM_A_MODNAME
	return backend->get_config_file((char*)module_name, (char*)moddir);
#else
	DIR *dir;
	struct dirent *ent;
	SWBuf name;

	name =  moddir;
	name += "/mods.d";
	if (dir = opendir(name)) {
		rewinddir(dir);
		while (ent = readdir(dir)) {
			if ((strcmp(ent->d_name, ".")) &&
			    (strcmp(ent->d_name, ".."))) {
				name =  moddir;
				name += "/mods.d/";
				name += ent->d_name;
				SWConfig *config = new SWConfig(name.c_str());
				if (config->Sections.find(module_name) !=
				    config->Sections.end()) {
					char *ret_name = g_strdup(ent->d_name);
					closedir(dir);
					delete config;
					return ret_name;
				}
				else
					delete config;
			}
		}
		closedir(dir);
	}
	return NULL;
#endif
}

int main_is_mod_rtol(const char * module_name)
{
	char *direction = backend->get_config_entry((char*)module_name, (char*)"Direction");
	return (direction && !strcmp(direction, "RtoL"));
}

/******************************************************************************
 * Name
 *  main_has_cipher_tag
 *
 * Synopsis
 *   #include "main/.h"
 *
 *   int main_has_cipher_tag(char *mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int main_has_cipher_tag(char *mod_name)
{
	return (backend->get_config_entry(mod_name, (char *)"CipherKey") != NULL);
}


/******************************************************************************
 * Name
 *   main_get_striptext
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   char *main_get_striptext(char *module_name, char *key)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *main_get_striptext(char *module_name, char *key)
{
	return backend->get_strip_text(module_name, key);
}


/******************************************************************************
 * Name
 *   main_get_striptext
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   char *main_get_striptext(char *module_name, char *key)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *main_get_striptext_from_string(char *module_name, char *string)
{
	return backend->get_strip_text_from_string(module_name, string);
}

/******************************************************************************
 * Name
 *   main_get_rendered_text
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   char *main_get_rendered_text(char *module_name, char *key)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *main_get_rendered_text(const char *module_name, const char *key)
{
	return backend->get_render_text(module_name, key);
}


/******************************************************************************
 * Name
 *   main_get_raw_text
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   char *main_get_raw_text(char *module_name, char *key)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *main_get_raw_text(char *module_name, char *key)
{
	return backend->get_raw_text(module_name, key);
}

/******************************************************************************
 * Name
 *   main_get_raw_text
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   char *main_get_raw_text(char *module_name, char *key)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *main_get_book_key_from_offset(unsigned long offset)
{
	return backend->get_key_from_offset(offset);
}

/******************************************************************************
 * Name
 *  main_get_mod_type
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   int main_get_mod_type(char * mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int main_get_mod_type(char * mod_name)
{
	
	return backend->module_type(mod_name);
}


/******************************************************************************
 * Name
 *  main_get_module_description
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   gchar *main_get_module_description(gchar * module_name)	
 *
 * Description
 *    
 *
 * Return value
 *   gchar *
 */ 

char *main_get_module_description(char * module_name)
{
	return backend->module_description(module_name);
}

/******************************************************************************
 * Name
 *  main_format_number
 *
 * Synopsis
 *   #include "main/sword.h"
 *   char *main_format_number(int x)	
 *
 * Description
 *   returns a digit string in either "latinate arabic" (normal) or
 *   farsi characters.
 *   re_encode_digits is chosen at startup in settings.c.
 *   caller must free allocated string space when finished with it.
 *
 * Return value
 *   char *
 */ 

int re_encode_digits = FALSE;

char *
main_format_number(int x)
{
	char *digits = g_strdup_printf("%d", x);

	if (re_encode_digits) {
		//
		// "\333\260" is farsi "zero".
		//
		char *d, *f, *farsi = g_new(char, 2*(strlen(digits)+1));
			// 2 "chars" per farsi-displayed digit + slop.

		for (d = digits, f = farsi; *d; ++d) {
			*(f++) = '\333';
			*(f++) = '\260' + ((*d) - '0');
		}
		*f = '\0';
		g_free(digits);
		return farsi;
	}
	return digits;
}

/******************************************************************************
 * Name
 *  main_deformat_number
 *
 * Synopsis
 *   #include "main/sword.h"
 *   int main_deformat_number(char *digitstring)
 *
 * Description
 *   returns the numeric value of the digitstring with
 *   respect to whether it's ordinary digits or farsi.
 *
 * Return value
 *   int
 */ 

int
main_deformat_number(char *digitstring)
{
	if (re_encode_digits) {
		// we can afford to make big assumptions because
		// we produced this string, so we can play fast-n-loose.
		// - digits have a particular content.
		// - never negative.
		// - one digit == 2 characters & only the 2nd is important.
		char *d;
		int result = 0;
		d = digitstring;
		while (*d) {
		    result = 10*result + (*(++d) - '\260');
		    ++d;
		}
		return result;
	}
	return atoi(digitstring);
}
