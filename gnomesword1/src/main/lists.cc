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

#include <gnome.h>
#include <glib-2.0/glib.h>
#include <gal/widgets/e-unicode.h>

#include <string.h>

#include "main/lists.h"
#include "main/sword.h"
#include "main/settings.h"

#include "backend/sword.h"
#include "backend/sword_main.hh"


/******************************************************************************
 *  lists to keep for the life of the program
 */
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



void main_init_lists(void)
{	
	gboolean start_backend = FALSE;
	
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
	
	if(!backend) {
		start_backend = TRUE;
		backend = new BackEnd();
	}
	mods.bible_books = backend->fill_Bible_books(2);
	mods.options = backend->get_module_options();
	backend->init_lists(mod_lists);
	if(start_backend)
		delete backend;

	settings.havebible = g_list_length(mods.biblemods);
	g_print("\n%s = %d\n", _("Number of Text modules"), settings.havebible);
	
	settings.havecomm = g_list_length(mods.commentarymods);
	g_print("%s = %d\n", _("Number of Commentary modules"), settings.havecomm);

	settings.havedict = g_list_length(mods.dictionarymods);
	g_print("%s = %d\n", _("Number of Dict/lex modules"), settings.havedict);

	settings.havebook = g_list_length(mods.bookmods);
	g_print("%s = %d\n", _("Number of Book modules"), settings.havebook);

	settings.havepercomm = g_list_length(mods.percommods);
	g_print("%s = %d\n", _("Number of Percomm modules"), settings.havepercomm);

	g_print("%s = %d\n\n", _("Number of Devotion modules"), 
					g_list_length(mods.devotionmods));
}

void main_shutdown_list(void)
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

	while (mod_lists->bible_books != NULL) {
		g_free((char *) mod_lists->bible_books->data);
		mod_lists->bible_books =
		    g_list_next(mod_lists->bible_books);
	}
	g_list_free(mod_lists->bible_books);
}
