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

#include <glib-1.2/glib.h>

#include "main/lists.h"
#include "main/sword.h"

#include "backend/sword.h"
	
/******************************************************************************
 *  lists to keep for the life of the program
 *   
 */
typedef struct _module_lists MOD_LISTS;
struct _module_lists {
	GList	
	*biblemods,
	*commentarymods,
	*dictionarymods, 
	*bookmods, 
	*percommods,
	*devotionmods, 
	*text_descriptions, 
	*dict_descriptions,
	*comm_descriptions,
	*book_descriptions,
	*bible_books,
	*options;	
};
static MOD_LISTS *mod_lists;
static MOD_LISTS mods;


GList * get_list(gint type)
{
	switch(type) {
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
	
	mods.bible_books = backend_get_books();
	mods.options = backend_get_global_options_list();
	mods.biblemods = backend_get_list_of_mods_by_type(TEXT_MODS);
	mods.commentarymods = backend_get_list_of_mods_by_type(COMM_MODS);
	mods.dictionarymods = backend_get_list_of_mods_by_type(DICT_MODS);
	mods.bookmods = backend_get_list_of_mods_by_type(BOOK_MODS);
	mods.percommods = backend_get_list_of_percom_modules();
	mods.devotionmods = backend_get_list_of_devotion_modules();
	mods.text_descriptions = backend_get_mod_description_list_SWORD(TEXT_MODS);
	mods.comm_descriptions = backend_get_mod_description_list_SWORD(COMM_MODS);
	mods.dict_descriptions = backend_get_mod_description_list_SWORD(DICT_MODS);
	mods.book_descriptions = backend_get_mod_description_list_SWORD(BOOK_MODS);
}

void shutdown_list(void)
{	
	/* free lists */
	g_list_free(mod_lists->biblemods);
	g_list_free(mod_lists->commentarymods);
	g_list_free(mod_lists->dictionarymods);
	g_list_free(mod_lists->bookmods);
	g_list_free(mod_lists->percommods);
	g_list_free(mod_lists->text_descriptions);
	g_list_free(mod_lists->dict_descriptions);
	g_list_free(mod_lists->comm_descriptions);
	g_list_free(mod_lists->book_descriptions);
	g_list_free(mod_lists->options);
	g_list_free(mod_lists->bible_books);
}
