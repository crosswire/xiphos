/*
 * Xiphos Bible Study Tool
 * navbar.cc - glue between all navbars and sword
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
#  include <config.h>
#endif
#include <gnome.h>
#include <swmgr.h>
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

//#ifdef OLD_NAVBAR

gboolean do_display;
gboolean do_display_dict;


void main_navbar_set(NAVBAR navbar, const char * key)
{	
	char *gkey = NULL;
	int book;
	GtkTreeIter iter;
	gint i,x;	
	VerseKey vkey; 
	
	if(!navbar.module_name)
		return;
	
	navbar.key = backend->get_valid_key(key);
	if(!navbar.is_dialog) {
		
		
	}
	GtkTreeModel* chapter_store = gtk_combo_box_get_model(
			GTK_COMBO_BOX(navbar.comboboxentry_chapter));
	GtkTreeModel* verse_store = gtk_combo_box_get_model(
			GTK_COMBO_BOX(navbar.comboboxentry_verse));
	
	do_display = FALSE;
	
	vkey.AutoNormalize(1);
	vkey = key;
	
	if((backend->module_has_testament(navbar.module_name, 1))
				&& (vkey.Testament() == 2))
		book = 39 + vkey.Book();
	else 
		book = vkey.Book();
	
	gtk_combo_box_set_active((GtkComboBox *)navbar.comboboxentry_book,
                                             book-1);
	
	gtk_list_store_clear(GTK_LIST_STORE(chapter_store));
	char xtestament = vkey.Testament() ;
	char xbook = vkey.Book();
	int xchapter = vkey.Chapter();
	int xverse = vkey.Verse();
#ifdef SWORD_MULTIVERSE
	x = (vkey.getChapterMax());
#else
	x = (vkey.books[xtestament-1][xbook-1].chapmax);
#endif
	for(i=1; i <= x; i++) {
		char *num = main_format_number(i);
		gtk_list_store_append (GTK_LIST_STORE(chapter_store), &iter);
		gtk_list_store_set(GTK_LIST_STORE(chapter_store), 
				   &iter, 
				   0, 
				   num, 
				   -1);
		g_free(num);
	}
	gtk_combo_box_set_active((GtkComboBox *)navbar.comboboxentry_chapter,
                                             xchapter-1);
	
	gtk_list_store_clear(GTK_LIST_STORE(verse_store));
	xtestament = vkey.Testament() ;
	xbook = vkey.Book();
	xchapter = vkey.Chapter();
	xverse = vkey.Verse();	
#ifdef SWORD_MULTIVERSE
	x = (vkey.getVerseMax());
#else
	x = (vkey.books[xtestament-1][xbook-1].versemax[xchapter-1]);
#endif
	for(i=1; i <= x; i++) {
		char *num = main_format_number(i);
		gtk_list_store_append (GTK_LIST_STORE(verse_store), &iter);
		gtk_list_store_set(GTK_LIST_STORE(verse_store), 
				   &iter, 
				   0, 
				   num, 
				   -1);
		g_free(num);
	}
	gtk_combo_box_set_active((GtkComboBox *)navbar.comboboxentry_verse,
                                             xverse-1);
	gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry),
				navbar.key);
	do_display = TRUE;
	g_free(gkey);	
}


void main_navbar_fill_book_combo(NAVBAR navbar)
{
	VerseKey key; 
	VerseKey key_abrev; 
	char *book = NULL;
	GtkTreeIter iter;
	int i = 0;
	
	if(navbar.testaments == backend->module_get_testaments(navbar.module_name))
		return;
	
	navbar.testaments = backend->module_get_testaments(navbar.module_name);
	
	do_display = FALSE;
	
	GtkTreeModel* book_model = gtk_combo_box_get_model(
			GTK_COMBO_BOX(navbar.comboboxentry_book));
	gtk_list_store_clear(GTK_LIST_STORE(book_model));
	if (backend->module_has_testament(navbar.module_name, 1)) {
		while(i < key.BMAX[0]) { 		
#ifdef SWORD_MULTIVERSE
			key.Testament(1);
			key.Book(i+1);
			book = strdup((const char *) key.getBookName());
#else
			book = strdup((const char *) key.books[0][i].name);
#endif
			char *mykey = g_strdup_printf("%s 1:1",book);
			if(!main_get_raw_text(navbar.module_name, mykey)){
				GS_message(("book-out: %s",book));
				g_free(book);
				g_free(mykey);
				continue;
			}
			GS_message(("book: %s",book));
			gtk_list_store_append (GTK_LIST_STORE(book_model), &iter);
			gtk_list_store_set(	GTK_LIST_STORE(book_model), 
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
		while(i < key.BMAX[1]) {			
#ifdef SWORD_MULTIVERSE
			key.Testament(2);
			key.Book(i+1);
			book = strdup((const char *) key.getBookName());
#else
			book = strdup((const char *) key.books[1][i].name);
#endif
			char *mykey = g_strdup_printf("%s 1:1",book);
			if(!main_get_raw_text(navbar.module_name, mykey)){
				GS_message(("book-in: %s",book));
				g_free(book);
				g_free(mykey);
				continue;
			}
			GS_message(("book: %s",book));
			gtk_list_store_append (GTK_LIST_STORE(book_model), &iter);
			gtk_list_store_set(	GTK_LIST_STORE(book_model), 
						&iter, 
						0, 
						book, 
						-1);
			++i;
			g_free(book);
		}
	}	
	main_navbar_set(navbar,navbar.key);
	do_display = TRUE;
}
