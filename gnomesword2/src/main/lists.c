/*
 * GnomeSword Bible Study Tool
 * lists.c - glists of module names and descriptions, bible books etc.
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

#include <glib-2.0/glib.h>
#include <string.h>

#include "main/lists.h"
#include "main/sword.h"
#include "main/settings.h"

#include "backend/sword.h"
#include "backend/mgr.hh"
#include "backend/module.hh"
#include "backend/key.hh"

/******************************************************************************
 *  lists to keep for the life of the program
 *   
 */
typedef struct _module_lists MOD_LISTS;
struct _module_lists {
	GList
	    * biblemods,
	    *commentarymods,
	    *dictionarymods,
	    *bookmods,
	    *percommods,
	    *devotionmods,
	    *text_descriptions,
	    *dict_descriptions,
	    *comm_descriptions,
	    *book_descriptions, *bible_books, *options;
};
static MOD_LISTS *mod_lists;
static MOD_LISTS mods;

GList *get_list(gint type)
{
	switch (type) {
	case TEXT_LIST:
		return mod_lists->biblemods;
		break;
	case TEXT_DESC_LIST:
		return mod_lists->text_descriptions;
		break;
	case COMM_LIST:
		return mod_lists->commentarymods;
		break;
	case COMM_DESC_LIST:
		return mod_lists->comm_descriptions;
		break;
	case DICT_LIST:
		return mod_lists->dictionarymods;
		break;
	case DICT_DESC_LIST:
		return mod_lists->dict_descriptions;
		break;
	case GBS_LIST:
		return mod_lists->bookmods;
		break;
	case GBS_DESC_LIST:
		return mod_lists->book_descriptions;
		break;
	case PERCOMM_LIST:
		return mod_lists->percommods;
		break;
	case BOOKS_LIST:
		return mod_lists->bible_books;
		break;
	case OPTIONS_LIST:
		return mod_lists->options;
		break;
	case DEVOTION_LIST:
		return mod_lists->devotionmods;
		break;
	}
	return NULL;
}



void init_lists(void)
{
	guint number_mods = 0;
	gint type = -1;
	char *buf = NULL;
	char *name = NULL;
	NAME_TYPE *nt;

	mod_lists = &mods;
	/* set glist to null */
	mods.biblemods = NULL;
	mods.commentarymods = NULL;
	mods.dictionarymods = NULL;
	mods.percommods = NULL;
	mods.bookmods = NULL;
	mods.devotionmods = NULL;
	mods.options = NULL;
	mods.text_descriptions = NULL;
	mods.comm_descriptions = NULL;
	mods.dict_descriptions = NULL;
	mods.book_descriptions = NULL;
	mods.bible_books = NULL;

	settings.havebible = FALSE;
	settings.havecomm = FALSE;
	settings.havedict = FALSE;
	settings.havebook = FALSE;
	settings.havepercomm = FALSE;

	backend_new_module_mgr();

	while ((buf = backend_get_next_book_of_bible()) != NULL) {
		mods.bible_books = g_list_append(mods.bible_books,
						 (char *) buf);
	}

	backend_set_global_option_iterator();
	while ((buf = backend_get_next_global_option()) != NULL) {
		mods.options =
		    g_list_append(mods.options, (char *) buf);
	}

	backend_set_module_iterators();
	while ((nt = backend_get_next_module_name()) != NULL) {
		name = nt->name;
		type = nt->type;
		//g_warning("list item = %s type = %d\n",name, type);
		
		switch (type) {
		case 0:
			mods.biblemods =
			    g_list_append(mods.biblemods,
					  (char *) name);
//			printf("Bible item = %s\n",name);
			break;
		case COMMENTARY_TYPE:
			mods.commentarymods =
			    g_list_append(mods.commentarymods,
					  (char *) name);
//			printf("Comm item = %s\n",name);
			break;
		case DICTIONARY_TYPE:
			mods.dictionarymods =
			    g_list_append(mods.dictionarymods,
					  (char *) name);
			break;
		case BOOK_TYPE:
			mods.bookmods =
			    g_list_append(mods.bookmods,
					  (char *) name);
			break;
		}
	}

	backend_set_module_iterators();
	while ((nt = backend_get_next_module_description()) != NULL) {
		name = nt->name;
		type = nt->type;
		//g_warning("list item = %s type = %d\n",name, type);
		
		switch (type) {
		case TEXT_TYPE:
			mods.text_descriptions =
			    g_list_append(mods.text_descriptions,
					  (char *) name);
			break;
		case COMMENTARY_TYPE:
			mods.comm_descriptions =
			    g_list_append(mods.comm_descriptions,
					  (char *) name);
			break;
		case DICTIONARY_TYPE:
			mods.dict_descriptions =
			    g_list_append(mods.dict_descriptions,
					  (char *) name);
			break;
		case BOOK_TYPE:
			mods.book_descriptions =
			    g_list_append(mods.book_descriptions,
					  (char *) name);
			break;
		}
	}

	backend_set_module_iterators();
	while ((buf = backend_get_next_percom_name()) != NULL) {
		if (!strcmp(buf, "+"))
			continue;
		mods.percommods = g_list_append(mods.percommods,
						(char *) buf);
	}

	backend_set_module_iterators();
	while ((buf = backend_get_next_devotion_name()) != NULL) {
		if (!strcmp(buf, "+"))
			continue;
		mods.devotionmods = g_list_append(mods.devotionmods,
						  (char *) buf);
	}

	backend_delete_module_mgr();

	if (mods.biblemods != NULL)
		settings.havebible = TRUE;
	number_mods = g_list_length(mods.biblemods);
	g_print("\nNumber of Text modules = %d\n", number_mods);

	if (mods.commentarymods != NULL)
		settings.havecomm = TRUE;
	number_mods = g_list_length(mods.commentarymods);
	g_print("Number of Commentary modules = %d\n", number_mods);

	if (mods.dictionarymods != NULL)
		settings.havedict = TRUE;
	number_mods = g_list_length(mods.dictionarymods);
	g_print("Number of Dict/lex modules = %d\n", number_mods);

	if (mods.bookmods != NULL)
		settings.havebook = TRUE;
	number_mods = g_list_length(mods.bookmods);
	g_print("Number of Book modules = %d\n", number_mods);

	if (mods.percommods != NULL)
		settings.havepercomm = TRUE;
	number_mods = g_list_length(mods.percommods);
	g_print("Number of Percomm modules = %d\n", number_mods);

	number_mods = g_list_length(mods.devotionmods);
	g_print("Number of Devotion modules = %d\n\n", number_mods);
}

void shutdown_list(void)
{
	/* free lists */
	while (mod_lists->options != NULL) {
		g_free((char *) mod_lists->options->data);
		mod_lists->options = g_list_next(mod_lists->options);
	}
	g_list_free(mod_lists->options);

	while (mod_lists->biblemods != NULL) {
		g_free((char *) mod_lists->biblemods->data);
		mod_lists->biblemods =
		    g_list_next(mod_lists->biblemods);
	}
	g_list_free(mod_lists->biblemods);

	while (mod_lists->commentarymods != NULL) {
		g_free((char *) mod_lists->commentarymods->data);
		mod_lists->commentarymods =
		    g_list_next(mod_lists->commentarymods);
	}
	g_list_free(mod_lists->commentarymods);

	while (mod_lists->dictionarymods != NULL) {
		g_free((char *) mod_lists->dictionarymods->data);
		mod_lists->dictionarymods =
		    g_list_next(mod_lists->dictionarymods);
	}
	g_list_free(mod_lists->dictionarymods);

	while (mod_lists->bookmods != NULL) {
		g_free((char *) mod_lists->bookmods->data);
		mod_lists->bookmods = g_list_next(mod_lists->bookmods);
	}
	g_list_free(mod_lists->bookmods);

	while (mod_lists->percommods != NULL) {
		g_free((char *) mod_lists->percommods->data);
		mod_lists->percommods =
		    g_list_next(mod_lists->percommods);
	}
	g_list_free(mod_lists->percommods);

	while (mod_lists->text_descriptions != NULL) {
		g_free((char *) mod_lists->text_descriptions->data);
		mod_lists->text_descriptions =
		    g_list_next(mod_lists->text_descriptions);
	}
	g_list_free(mod_lists->text_descriptions);

	while (mod_lists->dict_descriptions != NULL) {
		g_free((char *) mod_lists->dict_descriptions->data);
		mod_lists->dict_descriptions =
		    g_list_next(mod_lists->dict_descriptions);
	}
	g_list_free(mod_lists->dict_descriptions);

	while (mod_lists->comm_descriptions != NULL) {
		g_free((char *) mod_lists->comm_descriptions->data);
		mod_lists->comm_descriptions =
		    g_list_next(mod_lists->comm_descriptions);
	}
	g_list_free(mod_lists->comm_descriptions);

	while (mod_lists->book_descriptions != NULL) {
		g_free((char *) mod_lists->book_descriptions->data);
		mod_lists->book_descriptions =
		    g_list_next(mod_lists->book_descriptions);
	}
	g_list_free(mod_lists->book_descriptions);

	while (mod_lists->devotionmods != NULL) {
		g_free((char *) mod_lists->devotionmods->data);
		mod_lists->devotionmods =
		    g_list_next(mod_lists->devotionmods);
	}
	g_list_free(mod_lists->devotionmods);

	g_list_free(mod_lists->bible_books);
}
