/*
 * Xiphos Bible Study Tool
 * lists.c - glists of module names and descriptions, bible books etc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>

#include "main/lists.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/xml.h"
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
	case OPTIONS_LIST:
		return mod_lists->options;
		break;
	case DEVOTION_LIST:
		return mod_lists->devotionmods;
		break;
	case MAP_LIST:
		return mod_lists->mapmods;
		break;
	case IMAGE_LIST:
		return mod_lists->imagemods;
		break;
	case PRAYER_LIST:
		return mod_lists->prayermods;
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
	mods.prayermods = NULL;
	mods.mapmods = NULL;
	mods.imagemods = NULL;
	mods.options = NULL;
	mods.text_descriptions = NULL;
	mods.comm_descriptions = NULL;
	mods.dict_descriptions = NULL;
	mods.book_descriptions = NULL;
	//mods.prayer_descriptions = NULL;

	settings.havebible = FALSE;
	settings.havecomm = FALSE;
	settings.havedict = FALSE;
	settings.havebook = FALSE;
	settings.havepercomm = FALSE;
	settings.haveprayerlist = FALSE;

	if (backend) {
		mods.options = backend->get_module_options();
	} else {
		start_backend = TRUE;
		backend = new BackEnd();
	}
	backend->init_lists(mod_lists);
	if (start_backend) {
		delete backend;
		backend = NULL;
	}

	settings.havebible = g_list_length(mods.biblemods);

	settings.havecomm = g_list_length(mods.commentarymods);

	settings.havedict = g_list_length(mods.dictionarymods);

	settings.havebook = g_list_length(mods.bookmods);

	settings.havepercomm = g_list_length(mods.percommods);

	settings.haveprayerlist = g_list_length(mods.prayermods);

	if (g_list_length(mods.devotionmods) == 1) {
		xml_set_value("Xiphos", "modules", "devotional", (char *)mods.devotionmods->data);
		settings.devotionalmod = xml_get_value("modules", "devotional");
	}

	XI_print(("%s = %d\n", "Number of Text modules", settings.havebible));
	XI_print(("%s = %d\n", "Number of Commentary modules", settings.havecomm));
	XI_print(("%s = %d\n", "Number of Dict/lex modules", settings.havedict));
	XI_print(("%s = %d\n", "Number of Book modules", settings.havebook));
	XI_print(("%s = %d\n", "Number of Percomm modules", settings.havepercomm));
	XI_print(("%s = %d\n", "Number of Devotion modules",
		  g_list_length(mods.devotionmods)));
	XI_print(("%s = %d\n\n", "Number of Prayer modules",
		  settings.haveprayerlist));
}

void main_shutdown_list(void)
{
	/* free lists */
	if (!mod_lists)
		return;

	while (mod_lists->options != NULL) {
		g_free((char *)mod_lists->options->data);
		mod_lists->options = g_list_next(mod_lists->options);
	}
	g_list_free(mod_lists->options);

	while (mod_lists->biblemods != NULL) {
		g_free((char *)mod_lists->biblemods->data);
		mod_lists->biblemods =
		    g_list_next(mod_lists->biblemods);
	}
	g_list_free(mod_lists->biblemods);

	while (mod_lists->commentarymods != NULL) {
		g_free((char *)mod_lists->commentarymods->data);
		mod_lists->commentarymods =
		    g_list_next(mod_lists->commentarymods);
	}
	g_list_free(mod_lists->commentarymods);

	while (mod_lists->dictionarymods != NULL) {
		g_free((char *)mod_lists->dictionarymods->data);
		mod_lists->dictionarymods =
		    g_list_next(mod_lists->dictionarymods);
	}
	g_list_free(mod_lists->dictionarymods);

	while (mod_lists->bookmods != NULL) {
		g_free((char *)mod_lists->bookmods->data);
		mod_lists->bookmods = g_list_next(mod_lists->bookmods);
	}
	g_list_free(mod_lists->bookmods);

	while (mod_lists->percommods != NULL) {
		g_free((char *)mod_lists->percommods->data);
		mod_lists->percommods =
		    g_list_next(mod_lists->percommods);
	}
	g_list_free(mod_lists->percommods);
	while (mod_lists->prayermods != NULL) {
		g_free((char *)mod_lists->prayermods->data);
		mod_lists->prayermods =
		    g_list_next(mod_lists->prayermods);
	}
	g_list_free(mod_lists->prayermods);
	while (mod_lists->text_descriptions != NULL) {
		g_free((char *)mod_lists->text_descriptions->data);
		mod_lists->text_descriptions =
		    g_list_next(mod_lists->text_descriptions);
	}
	g_list_free(mod_lists->text_descriptions);

	while (mod_lists->dict_descriptions != NULL) {
		g_free((char *)mod_lists->dict_descriptions->data);
		mod_lists->dict_descriptions =
		    g_list_next(mod_lists->dict_descriptions);
	}
	g_list_free(mod_lists->dict_descriptions);

	while (mod_lists->comm_descriptions != NULL) {
		g_free((char *)mod_lists->comm_descriptions->data);
		mod_lists->comm_descriptions =
		    g_list_next(mod_lists->comm_descriptions);
	}
	g_list_free(mod_lists->comm_descriptions);

	while (mod_lists->book_descriptions != NULL) {
		g_free((char *)mod_lists->book_descriptions->data);
		mod_lists->book_descriptions =
		    g_list_next(mod_lists->book_descriptions);
	}
	g_list_free(mod_lists->book_descriptions);

	while (mod_lists->devotionmods != NULL) {
		g_free((char *)mod_lists->devotionmods->data);
		mod_lists->devotionmods =
		    g_list_next(mod_lists->devotionmods);
	}
	g_list_free(mod_lists->devotionmods);
	mod_lists = NULL;
}
