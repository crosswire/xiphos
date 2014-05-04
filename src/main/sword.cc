/*
 * Xiphos Bible Study Tool
 * sword.cc - glue
 *
 * Copyright (C) 2000-2014 Xiphos Developer Team
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


#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>

#include <swmgr.h>
#include <swmodule.h>
#include <stringmgr.h>
#include <localemgr.h>

extern "C" {
#include "gui/bibletext.h"
#include "main/gtk_compat.h"
}

#include <ctype.h>
#include <time.h>

#include "gui/main_window.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"
#include "gui/commentary.h"
#include "gui/dialog.h"
#include "gui/parallel_dialog.h"
#include "gui/parallel_tab.h"
#include "gui/parallel_view.h"
#include "gui/tabbed_browser.h"
#include "gui/xiphos.h"
#include "gui/sidebar.h"
#include "gui/utilities.h"
#include "gui/cipher_key_dialog.h"
#include "gui/main_menu.h"

#include "main/display.hh"
#include "main/lists.h"
#include "main/navbar.h"
#include "main/navbar_book.h"
#include "main/search_sidebar.h"
#include "main/previewer.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"
#include "main/parallel_view.h"
#include "main/modulecache.hh"

#include "backend/sword_main.hh"
#include "backend/gs_stringmgr.h"
#include "backend/biblesync.hh"

#include "gui/debug_glib_null.h"

#ifdef HAVE_DBUS
#include "gui/ipc.h"
#endif

BibleSync *biblesync;

using namespace sword;

extern GtkWidget *cbe_book;
extern GtkWidget *spb_chapter;
extern GtkWidget *spb_verse;
extern GtkWidget *cbe_freeform_lookup;
extern gboolean shift_key_pressed;

char *sword_locale = NULL;
gboolean companion_activity = FALSE;

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
	SWMgr *mgr = backend->get_mgr();

	backend->display_mod = mgr->Modules[mod_name];
	vkey = (VerseKey*)(SWKey*)(*backend->display_mod);
	vkey->setIntros(1);
	vkey->setAutoNormalize(0);
	vkey->setChapter(0);
	vkey->setVerse(0);
	backend->display_mod->display();
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
	SWMgr *mgr = backend->get_mgr();

	backend->display_mod = mgr->Modules[mod_name];
	backend->display_mod->setKey(settings.currentverse);
	vkey = (VerseKey*)(SWKey*)(*backend->display_mod);
	vkey->setIntros(1);
	vkey->setAutoNormalize(0);
	vkey->setVerse(0);
	backend->display_mod->display();
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

void main_save_note(const gchar * module_name,
		    const gchar * key_str,
		    const gchar * note_str)
{
	// Massage encoded spaces ("%20") back to real spaces.
	// This is a sick. unreliable hack that should be removed
	// after the underlying problem is fixed in Sword.
	gchar *rework;
	for (rework = (char*)strstr(note_str, "%20");
	     rework;
	     rework = strstr(rework+1, "%20")) {
		*rework = ' ';
		(void) strcpy(rework+1, rework+3);
	}

	GS_message(("note module %s\nnote key %s\nnote text%s",
		    module_name,
		    key_str,
		    note_str));
	backend->save_note_entry(module_name, key_str, note_str);

	main_display_commentary(module_name, settings.currentverse);
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

void main_delete_note(const gchar * module_name,
		      const gchar * key_str)
{
	backend->set_module_key(module_name, key_str);
	GS_message(("note module %s\nnote key %s\n",
		    module_name,
		    key_str));
	backend->delete_entry();

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

void main_set_module_unlocked(const char * mod_name, char * key)
{
	SWMgr *mgr = backend->get_mgr();
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

void main_save_module_key(const char * mod_name, char * key)
{
	backend->save_module_key((char *) mod_name, key);
}


/******************************************************************************
 * Name
 *  main_getText
 *
 * Synopsis
 *   #include "main/sword.h"
 *   void main_getText(gchar * key)
 *
 * Description
 *   get unabbreviated key
 *
 * Return value
 *   char *
 */

char *main_getText(char *key)
{
	VerseKey vkey(key);
	return strdup((char *)vkey.getText());
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
	xml_set_value("Xiphos", "keys", "verse", val_key);
	settings.currentverse = xml_get_value("keys", "verse");

	settings.apply_change = FALSE;

	navbar_versekey.module_name = g_string_assign(navbar_versekey.module_name,settings.MainWindowModule);
	navbar_versekey.key = g_string_assign(navbar_versekey.key,val_key);
	main_navbar_versekey_set(navbar_versekey, val_key);

	settings.apply_change = TRUE;

#ifdef HAVE_DBUS
	IpcObject* ipc = ipc_get_main_ipc();
	if (ipc)
		ipc_object_navigation_signal (ipc, (const gchar*)val_key, NULL);
#endif
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
	return backend->get_render_text((char *)mod_name,(char *)key);
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
	SWMgr *mgr = backend->get_mgr();
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
	gchar *language_file;
	FILE *language;
	gchar *s, *end, *abbrev, *name, *newline;
	gchar *mapspace;
	size_t length;

	if ((language_file = gui_general_user_file("languages", FALSE)) == NULL) {
		gui_generic_warning
		    (_("Xiphos's file for language\nabbreviations is missing."));
		return;
	}
	GS_message(("%s",language_file));

	if ((language = fopen(language_file, "r")) == NULL) {
		gui_generic_warning
		    (_("Xiphos's language abbreviation\nfile cannot be opened."));
		g_free(language_file);
		return;
	}
	g_free(language_file);
	(void) fseek(language, 0L, SEEK_END);
	length = ftell(language);
	rewind(language);

	if ((length == 0) ||
	    (mapspace = (gchar*)g_malloc(length+2)) == NULL) {
		fclose(language);
		gui_generic_warning
		    (_("Xiphos cannot allocate space\nfor language abbreviations."));
		return;
	}
	if (fread(mapspace, 1, length, language) != length) {
		fclose(language);
		g_free(mapspace);
		gui_generic_warning
		    (_("Xiphos cannot read the\nlanguage abbreviation file."));
		return;
	}
	fclose(language);		// safe to do even while mmap is active.
	*(end = length + mapspace) = '\0';

	for (s = mapspace; s < end; ++s) {
		if ((newline = strchr(s, '\n')) == NULL) {
			GS_message(("incomplete last line in languages"));
			break;
		}
		*newline = '\0';

		if ((*s == '#') || (s == newline)) {
			s = newline;	// comment or empty line.
			continue;
		}

		abbrev = s;
		if ((name = strchr(s, '\t')) == NULL) {
			GS_message(("tab-less line in languages"));
			break;
		}
		*(name++) = '\0';	// NUL-terminate abbrev, mark name.
		languageMap[SWBuf(abbrev)] = SWBuf(name);
		s = newline;
	}

	g_free(mapspace);
}

const char *main_get_language_map(const char *language) {
	if (language == NULL)
		return "Unknown";
	return languageMap[language].c_str();
}

char **main_get_module_language_list(void)
{
	return backend->get_module_language_list();
}


/******************************************************************************
 * Name
 *   set_sword_locale
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   char *set_sword_locale(const char *sys_locale)
 *
 * Description
 *   set sword's idea of the locale in which the user operates
 *
 * Return value
 *   char *
 */
char *set_sword_locale(const char *sys_locale)
{
	int ncmp[3] = { 100, 5, 2 };	// fixed data

	if (sys_locale) {
		SWBuf locale;
		StringList localelist = LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
		StringList::iterator it;

		// length-limited match
		for (int i = 0; i < 3; ++i) {
			for (it = localelist.begin(); it != localelist.end(); ++it) {
				locale = *it;
				if (!strncmp(sys_locale, locale.c_str(), ncmp[i])) {
					LocaleMgr::getSystemLocaleMgr()->
					    setDefaultLocaleName(locale.c_str());
					return g_strdup(locale.c_str());
				}
			}
		}

	}

	// either we were given a null sys_locale, or it didn't match anything.
	char *err = g_strdup_printf(_("No matching locale found for `%s'.\n%s"),
				    sys_locale,
				    _("Book names and menus may not be translated."));
	gui_generic_warning(err);
	g_free(err);
	return NULL;
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
	StringMgr::setSystemStringMgr( new GS_StringMgr() );

	const char *lang = getenv("LANG");
	if (!lang) lang = "C";
	sword_locale = set_sword_locale(lang);
	lang = LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName();

	backend = new BackEnd();

	backend->init_SWORD(0);
	settings.path_to_mods = main_get_path_to_mods();
//#ifndef DEBUG
	g_chdir(settings.path_to_mods);
//#else
//	GS_warning(("no chdir(SWORD_PATH) => modmgr 'archive' may not work"));
//#endif
	GS_print(("%s sword-%s\n", "Starting", backend->get_sword_version()));
	GS_print(("%s\n", "Initiating SWORD"));
	GS_print(("%s: %s\n","path to sword", settings.path_to_mods));
	GS_print(("%s %s\n", "SWORD locale is", lang));
	GS_print(("%s\n", "Checking for SWORD Modules"));
	settings.spell_language = strdup(lang);
	main_init_lists();

	//
	// BibleSync backend startup.  identify the user by name.
	//
	string user = (string)g_get_real_name()
	    + " (" + g_get_user_name() + ")";
	biblesync = new BibleSync("Xiphos", PACKAGE_VERSION, user);
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
	if (sword_locale)
		free((char*)sword_locale);
	sword_locale = NULL;
	if (backend)
	        delete backend;
	backend = NULL;

	GS_print(("%s\n", "SWORD is shutdown"));
}

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

	if (!mod_name)
		return;
	if (strcmp(settings.DictWindowModule, mod_name)) {
		xml_set_value("Xiphos", "modules", "dict", mod_name);
		settings.DictWindowModule = xml_get_value("modules", "dict");
	}

	key = g_strdup((gchar*)gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict)));

	backend->set_module_key(mod_name, key);
	g_free(key);
	key = backend->get_module_key();

	xml_set_value("Xiphos", "keys", "dictionary", key);
	settings.dictkey = xml_get_value("keys", "dictionary");

	main_check_unlock(mod_name, TRUE);

	backend->set_module_key(mod_name, key);
	backend->display_mod->display();

	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);
	g_free(key);
}



static void dict_key_list_select(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), (gchar*) user_data);
	gtk_widget_activate(widgets.entry_dict);
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
	gchar *new_key;
	gchar *key = NULL;
	GtkWidget *menu;
	GtkWidget * item;

	menu = gtk_menu_new();

	if (!settings.havedict || !mod_name)
		return NULL;
	if (strcmp(settings.DictWindowModule,mod_name)) {
		xml_set_value("Xiphos", "modules", "dict",
					mod_name);
		settings.DictWindowModule = xml_get_value(
					"modules", "dict");
	}
	key = g_strdup((gchar*)gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict)));

	GS_message(("\nold_key: %s\nkey: %s",old_key,key));
	backend->set_module_key(mod_name, key);
	g_free(key);
	key = backend->get_module_key();

	xml_set_value("Xiphos", "keys", "dictionary", key);
	settings.dictkey = xml_get_value("keys", "dictionary");

	main_check_unlock(mod_name, TRUE);

	backend->set_module_key(mod_name, key);
	backend->display_mod->display();

	new_key = g_strdup((char*)backend->display_mod->getKeyText());

	for (i = 0; i < (count / 2)+1; i++) {
		(*backend->display_mod)--;
	}

	for (i = 0; i < count; i++) {
		free(new_key);
		(*backend->display_mod)++;
		new_key = g_strdup((char*)backend->display_mod->getKeyText());
		/* add menu item */
		item =
		    gtk_menu_item_new_with_label((gchar *) new_key);
		gtk_widget_show(item);
		g_signal_connect(G_OBJECT(item), "activate",
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
	key = g_strdup((char*)backend->display_mod->getKeyText());
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);
	gtk_widget_activate(widgets.entry_dict);
	g_free(key);
}

void main_display_book(const char * mod_name,
		       const char * key)
{
	if (!settings.havebook || !mod_name)
		return;

	if (key == NULL)
		key = "0";

	GS_message(("main_display_book\nmod_name: %s\nkey: %s", mod_name, key));

	if (!backend->is_module(mod_name))
		return;
	if (!settings.book_mod)
		settings.book_mod = g_strdup((char*)mod_name);

	if (strcmp(settings.book_mod, mod_name)) {
		xml_set_value("Xiphos", "modules", "book", mod_name);
		gui_reassign_strdup(&settings.book_mod, (gchar *)mod_name);
	}

	if (!isdigit(key[0])) {
		xml_set_value("Xiphos", "keys", "book", key);
		settings.book_key = xml_get_value("keys", "book");

		backend->set_module(mod_name);
		backend->set_treekey(0);
		settings.book_offset = backend->treekey_set_key((char*)key);
	} else {
		settings.book_offset = atol(key);
		if (settings.book_offset < 4)
			settings.book_offset = 4;
		xml_set_value("Xiphos", "keys", "book", key);
		settings.book_key = xml_get_value("keys", "book");
		xml_set_value("Xiphos", "keys", "offset", key);

		backend->set_module(mod_name);
		backend->set_treekey(settings.book_offset);
	}

	main_check_unlock(mod_name, TRUE);

	backend->display_mod->display();
	main_setup_navbar_book(settings.book_mod, settings.book_offset);
	//if (settings.browsing)
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

void main_display_commentary(const char * mod_name,
			     const char * key)
{
	if (!settings.havecomm || !settings.comm_showing)
		return;

	if (!mod_name)
		mod_name = ((settings.browsing && (cur_passage_tab != NULL))
			    ? g_strdup(cur_passage_tab->commentary_mod)
			    : xml_get_value("modules", "comm"));

	if (!mod_name || !backend->is_module(mod_name))
		return;
	if (!settings.CommWindowModule)
		settings.CommWindowModule = g_strdup((gchar*)mod_name);

	settings.comm_showing = TRUE;
	settings.whichwindow = COMMENTARY_WINDOW;

	if (strcmp(settings.CommWindowModule, mod_name)) {
		xml_set_value("Xiphos", "modules", "comm", mod_name);
		gui_reassign_strdup(&settings.CommWindowModule, (gchar *)mod_name);

		// handle a conf directive "Companion=This,That,TheOther"
		char *companion = main_get_mod_config_entry(mod_name, "Companion");
		gchar **name_set = (companion ? g_strsplit(companion, ",", -1) : NULL);

		if (companion &&
		    (!companion_activity) &&
		    name_set[0] &&
		    *name_set[0] &&
		    backend->is_module(name_set[0]) &&
		    ((settings.MainWindowModule == NULL) ||
		     strcmp(name_set[0], settings.MainWindowModule))) {
			companion_activity = TRUE;

			gint name_length = g_strv_length(name_set);
			char *companion_question =
			    g_strdup_printf(_("Module %s has companion modules:\n%s.\n"
					      "Would you like to open these as well?%s"),
					    mod_name, companion,
					    ((name_length > 1)
					     ? _("\n\nThe first will open in the main window\n"
						 "and others in separate windows.")
					     : ""));

			if (gui_yes_no_dialog(companion_question, NULL)) {
				main_display_bible(name_set[0], key);
				for (int i = 1; i < name_length; i++) {
					main_dialogs_open(name_set[i], key);
				}
			}
			g_free(companion_question);
			companion_activity = FALSE;
		}
		if (name_set)  g_strfreev(name_set);
		if (companion) g_free(companion);
	}

	main_check_unlock(mod_name, TRUE);

	backend->set_module_key(mod_name, key);
	backend->display_mod->display();

	//if (settings.browsing)
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

void main_display_dictionary(const char * mod_name,
			     const char * key)
{
	const gchar *old_key;

	// for devotional use.
	gchar buf[10];

	if (!settings.havedict || !mod_name)
		return;

	GS_message(("main_display_dictionary\nmod_name: %s\nkey: %s", mod_name, key));

	if (!backend->is_module(mod_name))
		return;
	if (!settings.DictWindowModule)
		settings.DictWindowModule = g_strdup((gchar*)mod_name);

	if (key == NULL)
		key = (char*)"Grace";

	if (strcmp(settings.DictWindowModule, mod_name)) {
		// new dict -- is it actually a devotional?
		time_t curtime;
		struct tm *loctime;
		char *feature;
		if ((feature = (char *)backend->get_mgr()->
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
		xml_set_value("Xiphos", "modules", "dict", mod_name);
		gui_reassign_strdup(&settings.DictWindowModule, (gchar *)mod_name);
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

	//if (settings.browsing)
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


void main_display_bible(const char * mod_name,
			const char * key)
{
	gchar *file = NULL;
	gchar *style = NULL;
	gchar *val_key = NULL;

	/* keeps us out of a crash causing loop */
	extern guint scroll_adj_signal;
	extern GtkAdjustment* adjustment;
	if (adjustment)
		g_signal_handler_block(adjustment, scroll_adj_signal);

	if (!gtk_widget_get_realized (GTK_WIDGET(widgets.html_text))) return;
	if (!mod_name)
		mod_name = ((settings.browsing && (cur_passage_tab != NULL))
			    ? g_strdup(cur_passage_tab->text_mod)
			    : xml_get_value("modules", "bible"));


	if (!settings.havebible || !mod_name)
		return;
	if (!backend->is_module(mod_name))
		return;

	if (!settings.MainWindowModule)
		settings.MainWindowModule = g_strdup((gchar*)mod_name);

	if (strcmp(settings.currentverse, key)) {
		xml_set_value("Xiphos", "keys", "verse",
					key);
		settings.currentverse = xml_get_value(
					"keys", "verse");
	}

	if (strcmp(settings.MainWindowModule, mod_name)) {
		xml_set_value("Xiphos", "modules", "bible", mod_name);
		gui_reassign_strdup(&settings.MainWindowModule, (gchar *)mod_name);

		// handle a conf directive "Companion=This,That,TheOther"
		char *companion = main_get_mod_config_entry(mod_name, "Companion");
		gchar **name_set = (companion ? g_strsplit(companion, ",", -1) : NULL);

		if (companion &&
		    (!companion_activity) &&
		    name_set[0] &&
		    *name_set[0] &&
		    backend->is_module(name_set[0]) &&
		    ((settings.CommWindowModule == NULL) ||
		     strcmp(name_set[0], settings.CommWindowModule))) {
			companion_activity = TRUE;

			gint name_length = g_strv_length(name_set);
			char *companion_question =
			    g_strdup_printf(_("Module %s has companion modules:\n%s.\n"
					      "Would you like to open these as well?%s"),
					    mod_name, companion,
					    ((name_length > 1)
					     ? _("\n\nThe first will open in the main window\n"
						 "and others in separate windows.")
					     : ""));

			if (gui_yes_no_dialog(companion_question, NULL)) {
				main_display_commentary(name_set[0], key);
				for (int i = 1; i < name_length; i++) {
					main_dialogs_open(name_set[i], key);
				}
			}
			g_free(companion_question);
			companion_activity = FALSE;
		}
		if (name_set)  g_strfreev(name_set);
		if (companion) g_free(companion);

		navbar_versekey.module_name = g_string_assign(navbar_versekey.module_name,
							      settings.MainWindowModule);

		navbar_versekey.key = g_string_assign(navbar_versekey.key,
						      settings.currentverse);

		main_search_sidebar_fill_bounds_combos();
	}

	settings.whichwindow = MAIN_TEXT_WINDOW;

	file = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	style = get_conf_file_item(file, mod_name, "style");
	if ((style) && !strcmp(style,"verse"))
		settings.versestyle = TRUE;
	else
		settings.versestyle = FALSE;
	g_free(style);
	g_free(file);

	main_check_unlock(mod_name, TRUE);

	if (backend->module_has_testament(mod_name,
				backend->get_key_testament(key))) {
			backend->set_module_key(mod_name, key);
			backend->display_mod->display();
	} else {
		if (backend->get_key_testament(key) == 1)
			val_key = main_update_nav_controls("Matthew 1:1");
		else
			val_key = main_update_nav_controls("Genesis 1:1");

		backend->set_module_key(mod_name, val_key);
		backend->display_mod->display();
		g_free(val_key);
	}

	GS_message(("mod_name = %s",mod_name));
	//if (settings.browsing) {
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
	//}

	gui_change_window_title(settings.MainWindowModule);
	// (called _after_ tab data updated so not overwritten with old tab)

	/*
	 * change parallel verses
	 */
	if (settings.dockedInt)
		main_update_parallel_page();
	else {
		if (settings.showparatab)
			gui_keep_parallel_tab_in_sync();
		else
			gui_keep_parallel_dialog_in_sync();
	}

	//
	// BibleSync.
	//
	if (!settings.bs_receiving &&		// no re-xmit of recv'd nav.
	    ((biblesync->getMode() == BSP_MODE_PERSONAL) ||
	     (biblesync->getMode() == BSP_MODE_SPEAKER)))
	    // audience does not xmit nav.
	{
	    sync_windows();		// update display for current page

	    string group = "1";		// default if determination fails.
	    char pchar[2];
	    gint pagenum = (cur_passage_tab
			    ? gtk_notebook_page_num(
				    GTK_NOTEBOOK(widgets.notebook_main),
				    cur_passage_tab->page_widget)
			    : 1);

	    if ((pagenum != -1) && (pagenum < 9))
	    {
		pchar[0] = pagenum + '1';		// 0-based list.
		pchar[1] = '\0';
		group = (string)pchar;
	    }

	    // ** this key is not to be freed. **
	    char *osis_key = (char *)main_get_osisref_from_key(mod_name, key);
	    biblesync->Transmit(BSP_SYNC,
				(string)mod_name, (string)osis_key,
				(string)"", group);
	}
	// -- BibleSync --

	if (adjustment)
		g_signal_handler_unblock(adjustment, scroll_adj_signal);
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
	gchar *prettybuf;
	time_t curtime;
	struct tm *loctime;
	gchar *text;

	/*
	 * This makes sense only if you've installed & defined one.
	 */
	if (settings.devotionalmod == NULL) {
		GList *glist = get_list(DEVOTION_LIST);

		if (g_list_length(glist) != 0) {
			xml_set_value("Xiphos", "modules", "devotional",
				      (char*)glist->data);
			gui_reassign_strdup(&settings.devotionalmod, (gchar *)glist->data);
		} else {
			gui_generic_warning(_("Daily devotional was requested, but there are none installed."));
		}
	}

	/*
	 * Get the current time, converted to local time.
	 */
	curtime = time(NULL);
	loctime = localtime(&curtime);
	strftime(buf, 10, "%m.%d", loctime);
	prettybuf = g_strdup_printf("<b>%s %d</b>",
				    month_names[loctime->tm_mon],
				    loctime->tm_mday);

	text = backend->get_render_text(settings.devotionalmod, buf);
	if (text) {
		main_entry_display(settings.show_previewer_in_sidebar
				     ? sidebar.html_viewer_widget
				     : widgets.html_previewer_text,
			      settings.devotionalmod, text, prettybuf, TRUE);
		g_free(text);
	}
	g_free(prettybuf);
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
	backend->textDisplay = new GTKChapDisp(widgets.html_text,backend);
	backend->commDisplay = new GTKEntryDisp(widgets.html_comm,backend);
	backend->bookDisplay = new GTKEntryDisp(widgets.html_book,backend);
	backend->dictDisplay = new GTKEntryDisp(widgets.html_dict,backend);
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
	SWMgr *mgr = backend->get_mgr();
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
	SWMgr *mgr = backend->get_mgr();
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
	GDir *dir;
	const gchar *ent;
	SWBuf name;

	name =  moddir;
	name += "/mods.d";
	if ((dir = g_dir_open(name, 0, NULL))) {
	        g_dir_rewind(dir);
		while ((ent = g_dir_read_name(dir))) {
		        name =  moddir;
			name += "/mods.d/";
			name += ent;
			SWConfig *config = new SWConfig(name.c_str());
			if (config->Sections.find(module_name) !=
			        config->Sections.end()) {
			        gchar *ret_name = g_strdup(ent);
				g_dir_close(dir);
				delete config;
				return ret_name;
			}
			else
			        delete config;
		}
		g_dir_close(dir);
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
	gchar *cipherkey = backend->get_config_entry(mod_name, (char *)"CipherKey");
	int retval = (cipherkey != NULL);
	g_free(cipherkey);
	return retval;
}


#define	CIPHER_INTRO	\
_("<b>Locked Module.</b>\n\n<u>You are opening a module which requires a <i>key</i>.</u>\n\nThe module is locked, meaning that the content is encrypted by its publisher, and you must enter its key in order for the content to become useful.  This key should have been received by you on the web page which confirmed your purchase, or perhaps sent via email after purchase.\n\nPlease enter the key in the dialog.")

/******************************************************************************
 * Name
 *  main_check_unlock
 *
 * Synopsis
 *   #include "main/.h"
 *
 *   int main_check_unlock(const char *mod_name)
 *
 * Description
 *
 *
 * Return value
 *   int
 */

void main_check_unlock(const char *mod_name, gboolean conditional)
{
	gchar *cipher_old, *cipher_key;

	cipher_old = main_get_mod_config_entry(mod_name, "CipherKey");

	/* if forced by the unlock menu item, or it's present but empty... */
	if (!conditional ||					
	    ((cipher_old != NULL) && (*cipher_old == '\0'))) {

		if (conditional) {
			GtkWidget *dialog;
			dialog = gtk_message_dialog_new_with_markup
			    (NULL,	/* no need for a parent window */
			     GTK_DIALOG_DESTROY_WITH_PARENT,
			     GTK_MESSAGE_INFO,
			     GTK_BUTTONS_OK,
			     CIPHER_INTRO);
			g_signal_connect_swapped(dialog, "response",
						 G_CALLBACK(gtk_widget_destroy),
						 dialog);
			gtk_widget_show(dialog);
		}

		cipher_key = gui_add_cipher_key(mod_name, cipher_old);
		if (cipher_key) {
			ModuleCacheErase(mod_name);
			redisplay_to_realign();
			g_free(cipher_key);
		}
	}
	g_free(cipher_old);
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

const char *main_get_module_description(const char * module_name)
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

/******************************************************************************
 * Name
 *  main_flush_widgets_content
 *
 * Synopsis
 *   #include "main/sword.h"
 *   void main_flush_widgets_content()
 *
 * Description
 *   cleans content from all subwindow widgets.
 *
 * Return value
 *   int
 */
void main_flush_widgets_content(void)
{
	GString *blank_html_content = g_string_new(NULL);
	g_string_printf(blank_html_content,
			"<html><head></head><body bgcolor=\"%s\" text=\"%s\"> </body></html>",
			settings.bible_bg_color, settings.bible_text_color);

	if (gtk_widget_get_realized(GTK_WIDGET(widgets.html_text)))
		HtmlOutput(blank_html_content->str, widgets.html_text, NULL, NULL);
	if (gtk_widget_get_realized(GTK_WIDGET(widgets.html_comm)))
		HtmlOutput(blank_html_content->str, widgets.html_comm, NULL, NULL);
	if (gtk_widget_get_realized(GTK_WIDGET(widgets.html_dict)))
		HtmlOutput(blank_html_content->str, widgets.html_dict, NULL, NULL);
	if (gtk_widget_get_realized(GTK_WIDGET(widgets.html_book)))
		HtmlOutput(blank_html_content->str, widgets.html_book, NULL, NULL);
	g_string_free(blank_html_content, TRUE);
}

/******************************************************************************
 * Name
 *  main_is_Bible_key
 *
 * Synopsis
 *   #include "main/sword.h"
 *   void main_is_Bible_key()
 *
 * Description
 *   returns boolean status of whether input is a legit Bible key.
 *
 * Return value
 *   gboolean
 */
gboolean main_is_Bible_key(gchar *key)
{
	return (gboolean)(backend->is_Bible_key(key, settings.currentverse) != 0);
}

/******************************************************************************
 * Name
 *  main_get_osisref_from_key
 *
 * Synopsis
 *   #include "main/sword.h"
 *   void main_get_osisref_from_key()
 *
 * Description
 *   returns OSISRef-formatted key value.
 *
 * Return value
 *   const char *
 */
const char *
main_get_osisref_from_key(const char *module, const char *key)
{
	return backend->get_osisref_from_key(module, key);
}

/******************************************************************************
 * Name
 *  main_biblesync_navigate
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void main_biblesync_navigate(cmd, bible, ref, alt, info, dump)
 *
 * Description
 *   navigates per incoming BibleSync request, or handles
 *   announcement or error instead.
 *   used as a callback out of class BibleSync.
 *
 * Return value
 *   void
 */
void
main_biblesync_navigate(char cmd,
			string bible, string ref, string alt,
			string group, string domain,
			string info,  string dump)
{
    string message;

    // lockout: prevent re-xmit of what we're processing.
    settings.bs_receiving = TRUE;

    switch (cmd)
    {

    // error
    case 'E':
	if (settings.bs_debug)
	{
	    // recv error occurred: explanation in info, pkt in dump.
	    message = info + "\n\n" + dump;
	    gui_generic_warning((char *)message.c_str());
	}
	break;

    // mismatch
    case 'M':
	if (settings.bs_mismatch)
	{
	    message = "BibleSync: "
		+ (string)_("Mismatched packet\n\n")
		+ info
		+ ((info.substr(0,8) == "announce")
		   ? ("\nMessage: " + alt)
		   : ("\nBible: " + bible + "; Ref: " + ref + "; Alt: " + alt
		      + "\nGroup: " + group + "; Domain: " + domain))
		+ (settings.bs_debug
		   ? ("\n\n" + (string)dump)
		   : "");
	    gui_generic_warning((char *)message.c_str());
	}
	break;

    // announce
    case 'A':
	if (settings.bs_presence)
	{
	    if (settings.bs_debug)
	    {
		message = _("Presence announcement:\n\n") + dump;
		gui_generic_warning((char *)message.c_str());
	    }
	    gui_generic_warning((char *)alt.c_str());
	}
	break;

    // navigation
    case 'N':
	if (settings.bs_debug)
	{
	    message = _("Navigation packet:\n\n") + dump;
	    gui_generic_warning((char *)message.c_str());
	}

	if (backend->is_module(bible.c_str()))
	{
	    // Xiphos does nothing with "alt", the alternate reference.

	    // direct navigation, or via verse list?
	    if (settings.bs_navdirect &&
		(strpbrk(ref.c_str(), "-;,") == NULL))	// not a multi-ref
	    {
		// Xiphos interprets param "group" as a tab#.
		char tab = group.c_str()[0];
		if ((group.length() == 1) &&		// 1-character string
		    (tab >= '1') && (tab <= '9'))
		{
		    tab -= '1';				// 0-based list
		    
		    // select tab if not already current, avoid screen flash.
		    if (cur_passage_tab &&
			(tab != gtk_notebook_page_num(
				 GTK_NOTEBOOK(widgets.notebook_main),
				 cur_passage_tab->page_widget)))
		    {
			gui_select_nth_tab(tab);
		    }
		    settings.showtexts = 1;		// make panes visible
		    settings.showcomms = 1;
		    settings.comm_showing = 1;
		    gui_recompute_shows(FALSE);
		}
		//main_display_bible(bible.c_str(), ref.c_str());
		main_url_handler(((string)"sword://"
				  + bible
				  + "/"
				  + ref).c_str(), 1);
	    }
	    else
	    {
		// verse list as general preference, or due to multi-ref.
		main_display_verse_list_in_sidebar
		    (settings.currentverse,
		     (gchar*)bible.c_str(), (gchar*)ref.c_str());
	    }
	}
	else
	{
	    message = "BibleSync: " + (string)_("Unknown module ") + bible;
	    gui_generic_warning((char *)message.c_str());
	}
	break;

    default:
	message = _("ERROR: unknown BibleSync indicator: ")
	    + cmd
	    + (string)".\n"
	    + (string)_("Other indications are:")
	    + "\nbible: " + bible
	    + "\nref: " + ref
	    + "\nalt: " + alt
	    + "\ngroup: " + group
	    + "\ndomain: " + domain
	    + "\ninfo: " + info
	    + "\ndump:\n" + dump;
	gui_generic_warning((char *)message.c_str());
	break;
    }

    // unlock.
    settings.bs_receiving = FALSE;
}


/******************************************************************************
 * Name
 *  main_biblesync_mode_select
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void main_biblesync_mode_select(int m)
 *
 * Description
 *   checks incoming argument and assigns mode appropriately.
 *   starts receiver if we didn't disable.
 *
 * Return value
 *   void
 */
void
main_biblesync_mode_select(int m, char *p)
{
    BibleSync_mode mode;

    switch (m)
    {
    case 0:
	mode = BSP_MODE_DISABLE;    break;
    case 1:
	mode = BSP_MODE_PERSONAL;   break;
    case 2:
	mode = BSP_MODE_SPEAKER;    break;
    case 3:
	mode = BSP_MODE_AUDIENCE;   break;
    default:
	mode = N_BSP_MODE;          break;	// error
    }
    if (mode != N_BSP_MODE)
    {
	if (biblesync->setMode(mode, p, main_biblesync_navigate)
	    != BSP_MODE_DISABLE)
	{
	    // every time we set an active mode, we must start the receiver.
	    // glib will stop it on its own any time we go back to disable.
	    g_timeout_add(1000, (GSourceFunc)BibleSync::Receive, biblesync);
	}
    }
}

/******************************************************************************
 * Name
 *  main_biblesync_personal
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void main_biblesync_personal()
 *
 * Description
 *   determines whether BibleSync is set in Personal mode.
 *
 * Return value
 *   int
 */
int main_biblesync_personal()
{
    return biblesync->getMode() == BSP_MODE_PERSONAL;
}

/******************************************************************************
 * Name
 *  main_biblesync_active
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void main_biblesync_active()
 *
 * Description
 *   determines whether BibleSync is active or disabled.
 *
 * Return value
 *   int
 */
int main_biblesync_active()
{
    return biblesync->getMode() != BSP_MODE_DISABLE;
}

/******************************************************************************
 * Name
 *  main_biblesync_active_xmit_allowed
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void main_biblesync_active_xmit_allowed()
 *
 * Description
 *   determines whether BibleSync is alive and this user may send.
 *
 * Return value
 *   int
 */
int main_biblesync_active_xmit_allowed()
{
    return biblesync->getMode() == BSP_MODE_PERSONAL ||
	   biblesync->getMode() == BSP_MODE_SPEAKER;
}

/******************************************************************************
 * Name
 *  main_biblesync_get_passphrase
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   const char *main_biblesync_get_passphrase()
 *
 * Description
 *   retrieves the currently-set session passphrase.
 *
 * Return value
 *   void
 */
const char *main_biblesync_get_passphrase()
{
    return biblesync->getPassphrase().c_str();
}

/******************************************************************************
 * Name
 *  main_biblesync_transmit_verse_list
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   const char *main_biblesync_transmit_verse_list()
 *
 * Description
 *   ships out a multi-ref from verse list handler.
 *
 * Return value
 *   void
 */
void main_biblesync_transmit_verse_list(char *modname, char *vlist)
{
    biblesync->Transmit(BSP_SYNC, (string)modname, (string)vlist);
    // remaining args irrelevant => defaulted.
}

/******************************************************************************
 * Name
 *  main_biblesync_privacy
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   const char *main_biblesync_privacy(gboolean)
 *
 * Description
 *   en/disables privacy in personal mode (TTL 0).
 *
 * Return value
 *   void
 */
void main_biblesync_privacy(gboolean privacy)
{
    biblesync->setPrivate(privacy);
}
