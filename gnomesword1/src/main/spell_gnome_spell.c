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

#ifdef USE_GNOME_SPELL


#include <gnome.h>
#include <string.h>
#include <bonobo.h>

#include "Spell.h"
#include "main/spell.h"


CORBA_Environment   ev;
GNOME_Spell_Dictionary en;
 


/******************************************************************************
 * Name
 *   store_replacement
 *
 * Synopsis
 *   #include "main/spell.h"
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
	
	GNOME_Spell_Dictionary_setCorrection(en,
					     missed_word,
					     new_word,
					     "en",
					     &ev);
}


/******************************************************************************
 * Name
 *   add_to_session
 *
 * Synopsis
 *   #include "main/spell.h"
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

	GNOME_Spell_Dictionary_addWordToSession (en, word, &ev);
	
}


/******************************************************************************
 * Name
 *   add_to_personal
 *
 * Synopsis
 *   #include "main/spell.h"
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
	GNOME_Spell_Dictionary_addWordToPersonal(en, word, "en", &ev);
}


/******************************************************************************
 * Name
 *   get_suggest_list
 *
 * Synopsis
 *   #include "main/spell.h"
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
	GNOME_Spell_StringSeq *seq;
	gint i = 0;	
	
	seq = GNOME_Spell_Dictionary_getSuggestions (en, word, &ev);
	for (i=0; i<seq->_length; i++)
		retval = g_list_append(retval, g_strdup(seq->_buffer [i++]));
	CORBA_free (seq);
	return retval;
}


/******************************************************************************
 * Name
 *   check_word_spell
 *
 * Synopsis
 *   #include "main/spell.h"
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
	return GNOME_Spell_Dictionary_checkWord (en, word, &ev);
	
}


/******************************************************************************
 * Name
 *   init_spell
 *
 * Synopsis
 *   #include "main/spell.h"
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
	CORBA_sequence_GNOME_Spell_Language *language_seq;
	gint i;
	GNOME_Spell_StringSeq *seq;

	if (!bonobo_ui_init ("test-spell", VERSION, NULL, NULL))
		g_error (_("I could not initialize Bonobo"));
	bonobo_activate ();

	en = bonobo_get_object ("OAFIID:GNOME_Spell_Dictionary:0.3" , "GNOME/Spell/Dictionary", 0);

	if (en == CORBA_OBJECT_NIL) {
		g_error ("Could not create an instance of the spell component");
		return 1;
	}

	CORBA_exception_init (&ev);

//	GNOME_Spell_Dictionary_getLanguages (en, &ev);
	GNOME_Spell_Dictionary_setLanguage (en, "en", &ev);
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
	CORBA_exception_free (&ev);
	bonobo_object_release_unref (en, NULL);
	return 1;
}

#endif /* end USE_GNOME_SPELL */
