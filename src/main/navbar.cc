/*
 * Xiphos Bible Study Tool
 * navbar.cc - glue between all navbars and sword
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
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>

#include "main/module_dialogs.h"
#include "main/navbar.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/url.hh"

//#include "gui/toolbar_nav.h"
#include "gui/commentary_dialog.h"
#include "gui/bibletext_dialog.h"

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

//#ifdef OLD_NAVBAR

gboolean do_display;
gboolean do_display_dict;

void main_navbar_set(NAVBAR navbar, const char *key)
{
	char *gkey = NULL;
	int book;
	GtkTreeIter iter;
	gint i, x;

	if (!navbar.module_name)
		return;

	SWModule *mod = backend->get_SWModule(navbar.module_name);
	if (!mod)
		return;

	VerseKey *vkey = (VerseKey *)mod->createKey();

	navbar.key = backend->get_valid_key(navbar.module_name, key);
	if (!navbar.is_dialog) {
	}
	GtkTreeModel *chapter_store = gtk_combo_box_get_model(
	    GTK_COMBO_BOX(navbar.comboboxentry_chapter));
	GtkTreeModel *verse_store = gtk_combo_box_get_model(
	    GTK_COMBO_BOX(navbar.comboboxentry_verse));

	do_display = FALSE;

	int t = backend->module_type(navbar.module_name);
	if ((t == TEXT_TYPE) || (t == COMMENTARY_TYPE))
		vkey->setAutoNormalize(1);
	vkey->setText(key);

	// we need the book index to highlight "active" in the pulldown.
	if ((backend->module_has_testament(navbar.module_name, 1)) && (vkey->getTestament() == 2))
		book = vkey->BMAX[0] + vkey->getBook();
	else
		book = vkey->getBook();

	gtk_combo_box_set_active((GtkComboBox *)navbar.comboboxentry_book,
				 book - 1);

	gtk_list_store_clear(GTK_LIST_STORE(chapter_store));

	int xchapter = vkey->getChapter();
	int xverse = vkey->getVerse();

	x = (vkey->getChapterMax());
	for (i = 1; i <= x; i++) {
		char *num = main_format_number(i);
		gtk_list_store_append(GTK_LIST_STORE(chapter_store), &iter);
		gtk_list_store_set(GTK_LIST_STORE(chapter_store),
				   &iter,
				   0,
				   num,
				   -1);
		g_free(num);
	}
	gtk_combo_box_set_active((GtkComboBox *)navbar.comboboxentry_chapter,
				 xchapter - 1);

	gtk_list_store_clear(GTK_LIST_STORE(verse_store));

	x = (vkey->getVerseMax());
	for (i = 1; i <= x; i++) {
		char *num = main_format_number(i);
		gtk_list_store_append(GTK_LIST_STORE(verse_store), &iter);
		gtk_list_store_set(GTK_LIST_STORE(verse_store),
				   &iter,
				   0,
				   num,
				   -1);
		g_free(num);
	}
	gtk_combo_box_set_active((GtkComboBox *)navbar.comboboxentry_verse,
				 xverse - 1);
	gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry),
			   navbar.key);
	do_display = TRUE;
	g_free(gkey);

	delete vkey;
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
void main_navbar_fill_book_combo(NAVBAR navbar)
{
	if (!navbar.module_name)
		return;

	SWModule *mod = backend->get_SWModule(navbar.module_name);
	if (!mod)
		return;

	VerseKey *key = (VerseKey *)mod->createKey();
	char *book = NULL;
	GtkTreeIter iter;
	int i = 0;

	if (navbar.testaments == backend->module_get_testaments(navbar.module_name))
		return;

	navbar.testaments = backend->module_get_testaments(navbar.module_name);

	do_display = FALSE;

	GtkTreeModel *book_model = gtk_combo_box_get_model(
	    GTK_COMBO_BOX(navbar.comboboxentry_book));
	gtk_list_store_clear(GTK_LIST_STORE(book_model));

	if (backend->module_has_testament(navbar.module_name, 1)) {
		while (i < key->BMAX[0]) {
			key->setTestament(1);
			key->setBook(i + 1);
			book = strdup((const char *)key->getBookName());
			XI_message(("book: %s", book));
			gtk_list_store_append(GTK_LIST_STORE(book_model), &iter);
			gtk_list_store_set(GTK_LIST_STORE(book_model),
					   &iter,
					   0,
					   book,
					   -1);

			++i;
			g_free(book);
		}
	}

	i = 0;
	if (backend->module_has_testament(navbar.module_name, 2)) {
		while (i < key->BMAX[1]) {
			key->setTestament(2);
			key->setBook(i + 1);
			book = strdup((const char *)key->getBookName());
			XI_message(("book: %s", book));
			gtk_list_store_append(GTK_LIST_STORE(book_model), &iter);
			gtk_list_store_set(GTK_LIST_STORE(book_model),
					   &iter,
					   0,
					   book,
					   -1);
			++i;
			g_free(book);
		}
	}
	main_navbar_set(navbar, navbar.key);
	do_display = TRUE;

	delete key;
}
#endif
