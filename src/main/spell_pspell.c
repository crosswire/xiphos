/*
 * GnomeSword Bible Study Tool
 * spell_pspell.c - spell checking using pspell
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
#  include "config.h"
#endif

#ifdef USE_PSPELL

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif
#include <pspell/pspell.h>
#include <string.h>

#include "main/spell.h"

static PspellManager * spell_checker;
 
/******************************************************************************
 * Name
 *   check_for_error
 *
 * Synopsis
 *   #include "main/spell_pspell.h"
 *
 *   void check_for_error(void)
 *
 * Description
 *   check for pspell error and print it if found
 *
 * Return value
 *   void
 */

void check_for_error(void)
{
	if (pspell_manager_error_number(spell_checker) != 0) {
		g_warning("Pspell Error: %s\n", 
			pspell_manager_error_message(spell_checker));
	}
}


/******************************************************************************
 * Name
 *   store_replacement
 *
 * Synopsis
 *   #include "main/spell_pspell.h"
 *
 *   void store_replacement(const gchar * missed_word, const gchar * new_word)
 *
 * Description
 *   helps aspell learn
 *
 * Return value
 *   void
 */

void store_replacement(const gchar * missed_word, const gchar * new_word)
{
	pspell_manager_store_replacement(spell_checker,  
                                missed_word, -1, 
                                new_word, -1); 
}


/******************************************************************************
 * Name
 *   add_to_session
 *
 * Synopsis
 *   #include "main/spell_pspell.h"
 *
 *   int add_to_session(const gchar * word)
 *
 * Description
 *   add word to current session so it will not be flagged again
 *
 * Return value
 *   int
 */

int add_to_session(const gchar * word)
{

	return pspell_manager_add_to_session(spell_checker, 
						word, -1);
}


/******************************************************************************
 * Name
 *   add_to_personal
 *
 * Synopsis
 *   #include "main/spell_pspell.h"
 *
 *   int add_to_personal(const gchar * word)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int add_to_personal(const gchar * word)
{
	return pspell_manager_add_to_personal(spell_checker,
						word, -1);	
}


/******************************************************************************
 * Name
 *   get_suggest_list
 *
 * Synopsis
 *   #include "main/spell_pspell.h"
 *
 *   GList * get_suggest_list(const gchar * word)
 *
 * Description
 *   returns a list of suggested words to replace word
 *
 * Return value
 *   GList * 
 */

GList * get_suggest_list(const gchar * word)
{
	GList * retval = NULL;
	const char * new_word; 
	PspellWordList * suggestions;
	PspellStringEmulation * elements;
		
	suggestions = (PspellWordList *)pspell_manager_suggest(
						spell_checker, 
						word,
						strlen(word));
        elements = pspell_word_list_elements(suggestions); 
        	
        while ( (new_word = pspell_string_emulation_next(elements)) 
							!= NULL ) {
		retval = g_list_append(retval, g_strdup(new_word));
        } 
        delete_pspell_string_emulation(elements); 
	return retval;
}


/******************************************************************************
 * Name
 *   check_word_spell
 *
 * Synopsis
 *   #include "main/spell_pspell.h"
 *
 *   int check_word_spell(const char * word)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int check_word_spell(const char * word)
{
	return pspell_manager_check(spell_checker,  word, strlen(word));
}


/******************************************************************************
 * Name
 *   init_spell
 *
 * Synopsis
 *   #include "main/spell_pspell.h"
 *
 *   int init_spell(void)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int init_spell(void)
{
	PspellCanHaveError * ret;
	PspellConfig * config;
	
	
	config = new_pspell_config();
	pspell_config_replace(config, "language-tag", "en_US");  
	pspell_config_replace(config, "spelling", "american");
	pspell_config_replace(config, "encoding","utf-8");

	ret = new_pspell_manager(config);
	delete_pspell_config(config);
	
	if (pspell_error_number(ret) != 0) {
		g_warning("pspell error %s",pspell_error_message(ret));
		return -1;
	}
	
	spell_checker = to_pspell_manager(ret);
	config = pspell_manager_config(spell_checker);
	
	g_print("Using: \n");
	g_print("language tag \"%s\"\n",pspell_config_retrieve(config, "language-tag"));
	g_print("encoding \"%s\"\n",pspell_config_retrieve(config, "encoding"));
	g_print("spelling \"%s\"\n",pspell_config_retrieve(config, "spelling"));
	g_print("module \"%s\"\n",pspell_config_retrieve(config, "module"));
	g_print("path \"%s\"\n",pspell_config_retrieve(config, "master"));
	delete_pspell_config(config);
	
	return 1;
}


/******************************************************************************
 * Name
 *   kill_spell
 *
 * Synopsis
 *   #include "main/spell_pspell.h"
 *
 *   int kill_spell(void)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int kill_spell(void)
{	
	pspell_manager_save_all_word_lists(spell_checker); 
	pspell_manager_clear_session(spell_checker);
	//delete_pspell_manager(spell_checker);
	return 1;
}

#endif /* end USE_PSPELL */
