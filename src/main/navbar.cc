/*
 * GnomeSword Bible Study Tool
 * navbar.cc - glue between all navbars and sword
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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


#include "gui/toolbar_nav.h"
#include "gui/commentary_dialog.h"
#include "gui/bibletext_dialog.h"


#include "backend/sword_main.hh"


gboolean do_display;
gboolean do_display_dict;


void main_navbar_set(NAVBAR navbar, const char * key)
{	
	char buf[5];
	char *gkey = NULL;
	int book;
	GtkTreeIter iter;
	gint i,x;	
	VerseKey vkey; 
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	
	if(!navbar.module_name)
		return;
	
	navbar.key = backend->get_valid_key(key);
	
	GtkTreeModel* chapter_store = gtk_combo_box_get_model(
			GTK_COMBO_BOX(navbar.comboboxentry_chapter));
	GtkTreeModel* verse_store = gtk_combo_box_get_model(
			GTK_COMBO_BOX(navbar.comboboxentry_verse));
	
	do_display = FALSE;
	
	vkey.AutoNormalize(1);
	gkey = g_convert(navbar.key, -1, OLD_CODESET, UTF_8, &bytes_read,
			 &bytes_written, &error);
	if(gkey == NULL) {
		g_print ("error: %s\n", error->message);
		g_error_free (error);
		return;
	}
	vkey = gkey;
	
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
	x = (vkey.books[xtestament-1][xbook-1].chapmax);
	for(i=1; i <= x; i++) {
		sprintf(buf,"%d",i);
		gtk_list_store_append (GTK_LIST_STORE(chapter_store), &iter);
		gtk_list_store_set(	GTK_LIST_STORE(chapter_store), 
					&iter, 
					0, 
					buf, 
					-1);
	}
	gtk_combo_box_set_active((GtkComboBox *)navbar.comboboxentry_chapter,
                                             xchapter-1);
	
	gtk_list_store_clear(GTK_LIST_STORE(verse_store));
	xtestament = vkey.Testament() ;
	xbook = vkey.Book();
	xchapter = vkey.Chapter();
	xverse = vkey.Verse();	
	x = (vkey.books[xtestament-1][xbook-1].versemax[xchapter-1]);
	for(i=1; i <= x; i++) {
		sprintf(buf,"%d",i);
		gtk_list_store_append (GTK_LIST_STORE(verse_store), &iter);
		gtk_list_store_set(	GTK_LIST_STORE(verse_store), 
					&iter, 
					0, 
					buf, 
					-1);
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
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	char *book = NULL;
	GtkTreeIter iter;
	int i = 0, j = 0, x = 2;
	
	if(navbar.testaments == backend->module_get_testaments(navbar.module_name))
		return;
	
	navbar.testaments = backend->module_get_testaments(navbar.module_name);
	
	do_display = FALSE;
	
	GtkTreeModel* book_model = gtk_combo_box_get_model(
			GTK_COMBO_BOX(navbar.comboboxentry_book));
	gtk_list_store_clear(GTK_LIST_STORE(book_model));
	if (backend->module_has_testament(navbar.module_name, 1)) {
		while(i < key.BMAX[0]) { 
			
			book = g_convert((const char *)key.books[0][i].name,
				     -1,
				     UTF_8,
				     OLD_CODESET,
				     &bytes_read,
				     &bytes_written,
				     &error);
			if(book == NULL) {
				g_print ("error: %s\n", error->message);
				g_error_free (error);
				continue;
			}
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
			//g_message((const char *)key.books[1][i].name);
			book = g_convert((const char *)key.books[1][i].name,
				     -1,
				     UTF_8,
				     OLD_CODESET,
				     &bytes_read,
				     &bytes_written,
				     &error);
			if(book == NULL) {
				g_print ("error: %s\n", error->message);
				g_error_free (error);
				continue;
			}
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
