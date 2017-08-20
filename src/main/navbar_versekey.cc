/*
 * Xiphos Bible Study Tool
 * navbar_versekey.cc - glue between all navbar_versekey and sword
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
#include <swmodule.h>
#include <versekey.h>

#include "main/module_dialogs.h"
#include "main/navbar_versekey.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "gui/navbar_versekey.h"
#include "gui/tabbed_browser.h"

#ifdef USE_WEBKIT_EDITOR
#include "editor/webkit_editor.h"
#else
#include "editor/slib-editor.h"
#endif

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

extern gboolean do_display;
extern gboolean do_display_dict;

static DIALOG_DATA *c_dialog;
static EDITOR *c_editor;
static gint c_type;

/******************************************************************************
 * Name
 *   main_get_valid_key
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   const char *main_get_valid_key(const char *module, const char * key)
 *
 * Description
 *   get a valid versekey from the backend
 *
 * Return value
 *   void
 */

const char *main_get_valid_key(const char *module_name, const char *key)
{
	return backend->get_valid_key(module_name, key);
}

/******************************************************************************
 * Name
 *  main_navbar_versekey_spin_book
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   void main_navbar_versekey_spin_book(NAVBAR_VERSEKEY navbar, int direction)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_navbar_versekey_spin_book(NAVBAR_VERSEKEY navbar, int direction)
{
	char *tmpkey = NULL;
	int book;

	if (!navbar.module_name->len)
		return;

	SWModule *mod = backend->get_SWModule(navbar.module_name->str);
	if (!mod)
		return;

	VerseKey *vkey = (VerseKey *)mod->createKey();
	tmpkey = backend->get_valid_key(navbar.module_name->str, navbar.key->str);

	vkey->setAutoNormalize(1);
	vkey->setText(tmpkey);

	book = vkey->getBook() + (direction ? 1 : -1);
	vkey->setBook(book);

	tmpkey = g_strdup_printf("%s 1:1", vkey->getBookName());
	gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry), tmpkey);
	gtk_widget_activate(navbar.lookup_entry);
	g_free(tmpkey);
	delete vkey;
}

/******************************************************************************
 * Name
 *  main_navbar_versekey_spin_chapter
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   void main_navbar_versekey_spin_chapter(NAVBAR_VERSEKEY navbar, int direction)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_navbar_versekey_spin_chapter(NAVBAR_VERSEKEY navbar, int direction)
{
	char *tmpkey = NULL;
	int chapter;

	if (!navbar.module_name->len)
		return;

	SWModule *mod = backend->get_SWModule(navbar.module_name->str);
	if (!mod)
		return;

	VerseKey *vkey = (VerseKey *)mod->createKey();
	tmpkey = backend->get_valid_key(navbar.module_name->str, navbar.key->str);

	vkey->setAutoNormalize(1);
	vkey->setText(tmpkey);

	chapter = vkey->getChapter() + (direction ? 1 : -1);
	vkey->setChapter(chapter);

	tmpkey = g_strdup_printf("%s %d:1", vkey->getBookName(), vkey->getChapter());
	gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry), tmpkey);
	gtk_widget_activate(navbar.lookup_entry);
	g_free(tmpkey);
	delete vkey;
}

/******************************************************************************
 * Name
 *  main_navbar_versekey_spin_verse
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   char * main_navbar_versekey_spin_verse(NAVBAR_VERSEKEY navbar, int direction)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_navbar_versekey_spin_verse(NAVBAR_VERSEKEY navbar, int direction)
{

	char *tmpkey = NULL;
	int verse;

	if (!navbar.module_name->len)
		return;

	SWModule *mod = backend->get_SWModule(navbar.module_name->str);
	if (!mod)
		return;

	VerseKey *vkey = (VerseKey *)mod->createKey();
	tmpkey = backend->get_valid_key(navbar.module_name->str, navbar.key->str);

	vkey->setAutoNormalize(1);
	vkey->setText(tmpkey);

	verse = vkey->getVerse() + (direction ? 1 : -1);
	vkey->setVerse(verse);

	tmpkey = g_strdup_printf("%s %d:%d", vkey->getBookName(),
				 vkey->getChapter(), vkey->getVerse());
	gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry), tmpkey);
	gtk_widget_activate(navbar.lookup_entry);
	g_free(tmpkey);
	delete vkey;
}

/******************************************************************************
 * Name
 *  on_nt_book_menu_select
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   void on_nt_book_menu_select(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   user selected new new testament book from dropdown menu - change verse key
 *   to new book set lookup_entry text to new verse key and activate entry
 *
 * Return value
 *   void
 */

static void on_nt_book_menu_select(GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *entry = NULL;
	int book = GPOINTER_TO_INT(user_data);
	gchar *name, *key;

	switch (c_type) {
	case NB_MAIN:
		name = navbar_versekey.module_name->str;
		key = navbar_versekey.key->str;
		entry = navbar_versekey.lookup_entry;
		break;

	case NB_PARALLEL:
		name = navbar_parallel.module_name->str;
		key = navbar_parallel.key->str;
		entry = navbar_parallel.lookup_entry;
		break;

	case NB_DIALOG:
		if (!c_dialog)
			return;
		name = c_dialog->navbar.module_name->str;
		key = c_dialog->navbar.key->str;
		entry = c_dialog->navbar.lookup_entry;
		break;

	case NB_EDITOR:
		if (!c_editor)
			return;
		name = c_editor->navbar.module_name->str;
		key = c_editor->navbar.key->str;
		entry = c_editor->navbar.lookup_entry;
		break;
	}

	if (entry) {
		SWModule *mod = backend->get_SWModule(name);
		if (mod) {
			VerseKey *vkey = (VerseKey *)mod->createKey();
			vkey->setAutoNormalize(1);
			vkey->setText(key);
			vkey->setTestament(2);
			vkey->setBook(book + 1);

			gtk_entry_set_text(GTK_ENTRY(entry), vkey->getText());
			gtk_widget_activate(entry);

			delete vkey;
		}
	}
}

/******************************************************************************
 * Name
 *  on_ot_book_menu_select
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   void on_ot_book_menu_select(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   user selected new old testament book from dropdown menu - change verse key
 *   to new book set lookup_entry text to new verse key and activate entry
 *
 * Return value
 *   void
 */

static void on_ot_book_menu_select(GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *entry = NULL;
	int book = GPOINTER_TO_INT(user_data);
	gchar *name, *key;

	switch (c_type) {
	case NB_MAIN:
		name = navbar_versekey.module_name->str;
		key = navbar_versekey.key->str;
		entry = navbar_versekey.lookup_entry;
		break;

	case NB_PARALLEL:
		name = navbar_parallel.module_name->str;
		key = navbar_parallel.key->str;
		entry = navbar_parallel.lookup_entry;
		break;

	case NB_DIALOG:
		if (!c_dialog)
			return;
		name = c_dialog->navbar.module_name->str;
		key = c_dialog->navbar.key->str;
		entry = c_dialog->navbar.lookup_entry;
		break;

	case NB_EDITOR:
		if (!c_editor)
			return;
		name = c_editor->navbar.module_name->str;
		key = c_editor->navbar.key->str;
		entry = c_editor->navbar.lookup_entry;
		break;
	}

	if (entry) {
		SWModule *mod = backend->get_SWModule(name);
		if (mod) {
			VerseKey *vkey = (VerseKey *)mod->createKey();
			vkey->setAutoNormalize(1);
			vkey->setText(key);
			vkey->setTestament(1);
			vkey->setBook(book + 1);

			gtk_entry_set_text(GTK_ENTRY(entry), vkey->getText());
			gtk_widget_activate(entry);

			delete vkey;
		}
	}
}

/******************************************************************************
 * Name
 *  on_chapter_menu_select
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   void on_chapter_menu_select(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   user selected new chapter from dropdown menu - change verse key to new chapter
 *   set lookup_entry text to new verse key and activate entry
 *
 * Return value
 *   void
 */

static void on_chapter_menu_select(GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *entry = NULL;
	int chapter = GPOINTER_TO_INT(user_data);
	gchar *name, *key;

	switch (c_type) {
	case NB_MAIN:
		name = navbar_versekey.module_name->str;
		key = navbar_versekey.key->str;
		entry = navbar_versekey.lookup_entry;
		break;

	case NB_PARALLEL:
		name = navbar_parallel.module_name->str;
		key = navbar_parallel.key->str;
		entry = navbar_parallel.lookup_entry;
		break;

	case NB_DIALOG:
		if (!c_dialog)
			return;
		name = c_dialog->navbar.module_name->str;
		key = c_dialog->navbar.key->str;
		entry = c_dialog->navbar.lookup_entry;
		break;

	case NB_EDITOR:
		if (!c_editor)
			return;
		name = c_editor->navbar.module_name->str;
		key = c_editor->navbar.key->str;
		entry = c_editor->navbar.lookup_entry;
		break;
	}
	if (entry) {
		SWModule *mod = backend->get_SWModule(name);
		if (mod) {
			VerseKey *vkey = (VerseKey *)mod->createKey();
			vkey->setAutoNormalize(1);
			vkey->setText(key);
			vkey->setChapter(chapter);

			gtk_entry_set_text(GTK_ENTRY(entry), vkey->getText());
			gtk_widget_activate(entry);

			delete vkey;
		}
	}
}

/******************************************************************************
 * Name
 *  on_verse_menu_select
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   void on_verse_menu_select(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   user selected new verse from dropdown menu - change verse key to new verse
 *   set lookup_entry text to new verse key and activate entry
 *
 * Return value
 *   void
 */

static void on_verse_menu_select(GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *entry = NULL;
	int verse = GPOINTER_TO_INT(user_data);
	gchar *name, *key;

	switch (c_type) {
	case NB_MAIN:
		name = navbar_versekey.module_name->str;
		key = navbar_versekey.key->str;
		entry = navbar_versekey.lookup_entry;
		break;

	case NB_PARALLEL:
		name = navbar_parallel.module_name->str;
		key = navbar_parallel.key->str;
		entry = navbar_parallel.lookup_entry;
		break;

	case NB_DIALOG:
		if (!c_dialog)
			return;
		name = c_dialog->navbar.module_name->str;
		key = c_dialog->navbar.key->str;
		entry = c_dialog->navbar.lookup_entry;
		break;

	case NB_EDITOR:
		if (!c_editor)
			return;
		name = c_editor->navbar.module_name->str;
		key = c_editor->navbar.key->str;
		entry = c_editor->navbar.lookup_entry;
		break;
	}
	if (entry) {
		SWModule *mod = backend->get_SWModule(name);
		if (mod) {
			VerseKey *vkey = (VerseKey *)mod->createKey();
			vkey->setAutoNormalize(1);
			vkey->setText(key);
			vkey->setVerse(verse);

			gtk_entry_set_text(GTK_ENTRY(entry), vkey->getText());
			gtk_widget_activate(entry);

			delete vkey;
		}
	}
}

/******************************************************************************
 * Name
 *  main_navbar_versekey_set
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   void main_navbar_versekey_set(NAVBAR_VERSEKEY navbar, const char * key)
 *
 * Description
 *   separate key info John 3:16 to book=john chapter=3 verse=16 and write
 *   to navbar labels - also set lookup entry text
 *
 * Return value
 *   void
 */

void main_navbar_versekey_set(NAVBAR_VERSEKEY navbar, const char *key)
{
	char *tmpbuf = NULL;

	if (!navbar.module_name->len)
		return;

	SWModule *mod = backend->get_SWModule(navbar.module_name->str);
	if (!mod)
		return;

	// previously, we set and normalized the key, but we also
	// kept a record of whether that key made sense.
	if (navbar.valid_key) {
		VerseKey *vkey = (VerseKey *)mod->createKey();
		vkey->setAutoNormalize(1);
		vkey->setText(key);

		tmpbuf = g_strdup_printf("<b>%s</b>", vkey->getBookName());
		gtk_label_set_label(GTK_LABEL(navbar.label_book_menu), tmpbuf);
		g_free(tmpbuf);

		gchar *num = main_format_number(vkey->getChapter());
		tmpbuf = g_strdup_printf("<b>%s</b>", num);
		g_free(num);
		gtk_label_set_label(GTK_LABEL(navbar.label_chapter_menu), tmpbuf);
		g_free(tmpbuf);

		num = main_format_number(vkey->getVerse());
		tmpbuf = g_strdup_printf("<b>%s</b>", num);
		g_free(num);
		gtk_label_set_label(GTK_LABEL(navbar.label_verse_menu), tmpbuf);
		g_free(tmpbuf);

		navbar.key = g_string_assign(navbar.key, (char *)vkey->getText());
		gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry), navbar.key->str);

		delete vkey;
	} else {
		tmpbuf = g_strdup(" ");
		gtk_label_set_label(GTK_LABEL(navbar.label_book_menu), tmpbuf);
		gtk_label_set_label(GTK_LABEL(navbar.label_chapter_menu), tmpbuf);
		gtk_label_set_label(GTK_LABEL(navbar.label_verse_menu), tmpbuf);
		gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry), tmpbuf);
		g_free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *  main_versekey_drop_down_verse_menu
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   GtkWidget *main_versekey_drop_down_verse_menu(NAVBAR_VERSEKEY navbar)
 *
 * Description
 *   create and return a dropdown menu of verses for the current chapter
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *main_versekey_drop_down_verse_menu(NAVBAR_VERSEKEY navbar,
					      gint nb_type,
					      gpointer dialog,
					      gpointer editor)
{
	gint i, x;
	GtkWidget *menu;
	GtkMenuShell *menu_shell;
	GtkWidget *item;
	GtkWidget *select_item = NULL;
	c_dialog = (DIALOG_DATA *)dialog;
	c_editor = (EDITOR *)editor;
	c_type = nb_type;

	menu = gtk_menu_new();
	menu_shell = GTK_MENU_SHELL(menu);

	SWModule *mod = backend->get_SWModule(navbar.module_name->str);
	if (!mod)
		return menu;

	VerseKey *vkey = (VerseKey *)mod->createKey();
	vkey->setAutoNormalize(1);
	vkey->setText(navbar.key->str);
	int xverse = vkey->getVerse();

	x = vkey->getVerseMax();
	delete vkey;

	for (i = 1; i <= x; i++) {
		gchar *num = main_format_number(i);
		item = gtk_menu_item_new_with_label(num);
		gtk_widget_show(item);
		gtk_menu_shell_append(menu_shell, item);
		g_signal_connect(G_OBJECT(item), "activate",
				 G_CALLBACK(on_verse_menu_select),
				 GINT_TO_POINTER(i));

		if (i == xverse)
			select_item = item;
		g_free(num);
	}

	if (select_item)
		gtk_menu_shell_select_item(menu_shell, select_item);

	return menu;
}

/******************************************************************************
 * Name
 *  main_versekey_drop_down_chapter_menu
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   GtkWidget *main_versekey_drop_down_chapter_menu(NAVBAR_VERSEKEY navbar)
 *
 * Description
 *   create and return a dropdown menu for chapters for the current book
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *main_versekey_drop_down_chapter_menu(NAVBAR_VERSEKEY navbar,
						gint nb_type,
						gpointer dialog,
						gpointer editor)
{
	gint i, x;
	GtkWidget *menu;
	GtkMenuShell *menu_shell;
	GtkWidget *item;
	GtkWidget *select_item = NULL;
	c_dialog = (DIALOG_DATA *)dialog;
	c_editor = (EDITOR *)editor;
	c_type = nb_type;

	menu = gtk_menu_new();
	menu_shell = GTK_MENU_SHELL(menu);

	SWModule *mod = backend->get_SWModule(navbar.module_name->str);
	if (!mod)
		return menu;

	VerseKey *vkey = (VerseKey *)mod->createKey();
	vkey->setAutoNormalize(1);
	vkey->setText(navbar.key->str);
	int xchapter = vkey->getChapter();

	x = vkey->getChapterMax();
	delete vkey;

	for (i = 1; i <= x; i++) {
		gchar *num = main_format_number(i);
		item = gtk_menu_item_new_with_label(num);
		gtk_widget_show(item);
		gtk_menu_shell_append(menu_shell, item);
		g_signal_connect(G_OBJECT(item), "activate",
				 G_CALLBACK(on_chapter_menu_select),
				 GINT_TO_POINTER(i));

		if (i == xchapter)
			select_item = item;
		g_free(num);
	}

	if (select_item)
		gtk_menu_shell_select_item(menu_shell, select_item);

	return menu;
}

/******************************************************************************
 * Name
 *  main_versekey_drop_down_book_menu
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *
 *   GtkWidget *main_versekey_drop_down_book_menu(NAVBAR_VERSEKEY navbar, gpointer data)
 *
 * Description
 *   create and return a dropdown menu of books of the current Bible module
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *main_versekey_drop_down_book_menu(NAVBAR_VERSEKEY navbar,
					     gint nb_type,
					     gpointer dialog,
					     gpointer editor)
{
	GtkWidget *menu;
	GtkMenuShell *menu_shell;
	GtkWidget *item;
	GtkWidget *select_item = NULL;
	char *book = NULL;
	char *current_book = NULL;
	int i = 0;

	c_dialog = (DIALOG_DATA *)dialog;
	c_editor = (EDITOR *)editor;
	c_type = nb_type;

	menu = gtk_menu_new();
	menu_shell = GTK_MENU_SHELL(menu);

	SWModule *mod = backend->get_SWModule(navbar.module_name->str);
	if (!mod)
		return menu;

	VerseKey *key = (VerseKey *)mod->createKey();
	VerseKey *key_current = (VerseKey *)mod->createKey();
	key->setAutoNormalize(1);
	key_current->setAutoNormalize(1);
	key_current->setText(navbar.key->str);
	current_book = strdup((const char *)key_current->getBookName());

	if (backend->module_has_testament(navbar.module_name->str, 1)) {
		while (i < key->BMAX[0]) {
			key->setTestament(1);
			key->setBook(i + 1);
			book = strdup((const char *)key->getBookName());
			item = gtk_menu_item_new_with_label(book);
			gtk_widget_show(item);
			gtk_menu_shell_append(menu_shell, item);
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK(on_ot_book_menu_select),
					 GINT_TO_POINTER(i));

			if (!strcmp(book, current_book))
				select_item = item;
			++i;
			g_free(book);
		}
	}

	i = 0;
	if (backend->module_has_testament(navbar.module_name->str, 2)) {
		while (i < key->BMAX[1]) {
			key->setTestament(2);
			key->setBook(i + 1);
			book = strdup((const char *)key->getBookName());
			item = gtk_menu_item_new_with_label(book);
			gtk_widget_show(item);
			gtk_menu_shell_append(menu_shell, item);
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK(on_nt_book_menu_select),
					 GINT_TO_POINTER(i));

			if (!strcmp(book, current_book))
				select_item = item;
			++i;
			g_free(book);
		}
	}

	delete key;
	delete key_current;

	if (select_item)
		gtk_menu_shell_select_item(menu_shell, select_item);
	if (current_book)
		g_free(current_book);
	return menu;
}
