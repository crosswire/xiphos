/*
 * GnomeSword Bible Study Tool
 * spell.h - spell checking using pspell
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
 
#ifndef _SPELL_H_
#define _SPELL_H_
 
extern char *spell_language;

void check_for_error(void);
void store_replacement(const gchar * word, const gchar * new_word, 
						char * language);
int add_to_session(const gchar * word);
int add_to_personal(const gchar * word, char * language);
GList * get_suggest_list(const gchar * word);
int check_word_spell(const char * word);
int init_spell(void);
int kill_spell(void);
GList *get_dictionary_languages(gpointer data);
void set_dictionary_language(char * language);

#endif
