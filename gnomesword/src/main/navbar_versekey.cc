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
#include <versekey.h>

#include "main/module_dialogs.h"
#include "main/navbar_versekey.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "gui/navbar_versekey.h"
#include "gui/tabbed_browser.h"

#include "backend/sword_main.hh"


extern gboolean do_display;
extern gboolean do_display_dict;


static DIALOG_DATA * c_dialog;

/******************************************************************************
 * Name
 *   main_get_valid_key
 *
 * Synopsis
 *   #include "main/navbar_versekey.h"
 *   
 *   const char *main_get_valid_key(char * key)
 *
 * Description
 *   get a valid versekey from the backend
 *
 * Return value
 *   void
 */

const char *main_get_valid_key(char * key)
{
	return backend->get_valid_key(key);
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
	VerseKey vkey; 
	char *gkey = NULL;
	char *tmpkey = NULL;
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	int book;
	
	if(!navbar.module_name->len)
		return;
	
	tmpkey = backend->get_valid_key(navbar.key->str);
	
	vkey.AutoNormalize(1);
	gkey = g_convert(tmpkey, -1, OLD_CODESET, UTF_8, &bytes_read,
			 &bytes_written, &error);
	if(gkey == NULL) {
		g_print ("error: %s\n", error->message);
		g_error_free (error);
		return;
	}
	vkey = gkey;
	
	book = (direction)?(vkey.Book()+1):(vkey.Book()-1);
	vkey.Book(book);
	tmpkey = g_strdup_printf("%s 1:1",vkey.getBookName());
	gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry), tmpkey);
	gtk_widget_activate(navbar.lookup_entry);
	g_free(tmpkey);
	g_free(gkey);
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
	
	VerseKey vkey; 
	char *gkey = NULL;
	char *tmpkey = NULL;
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	int chapter;
	
	if(!navbar.module_name->len)
		return;
	
	tmpkey = backend->get_valid_key(navbar.key->str);
	
	vkey.AutoNormalize(1);
	gkey = g_convert(tmpkey, -1, OLD_CODESET, UTF_8, &bytes_read,
			 &bytes_written, &error);
	if(gkey == NULL) {
		g_print ("error: %s\n", error->message);
		g_error_free (error);
		return;
	}
	vkey = gkey;
	chapter = (direction)?(vkey.Chapter()+1):(vkey.Chapter()-1);
	vkey.Chapter(chapter);
	tmpkey = g_strdup_printf("%s %d:1",vkey.getBookName(),vkey.Chapter());
	gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry), tmpkey);
	gtk_widget_activate(navbar.lookup_entry);
	g_free(tmpkey);
	g_free(gkey);
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
	
	VerseKey vkey; 
	char *gkey = NULL;
	char *tmpkey = NULL;
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	int verse;
	
	if(!navbar.module_name->len)
		return;
	
	tmpkey = backend->get_valid_key(navbar.key->str);
	
	vkey.AutoNormalize(1);
	gkey = g_convert(tmpkey, -1, OLD_CODESET, UTF_8, &bytes_read,
			 &bytes_written, &error);
	if(gkey == NULL) {
		g_print ("error: %s\n", error->message);
		g_error_free (error);
		return;
	}
	vkey = gkey;
	verse = (direction)?(vkey.Verse()+1):(vkey.Verse()-1);
	vkey.Verse(verse);
	tmpkey = g_strdup_printf("%s %d:%d",vkey.getBookName(),
					    vkey.Chapter(),
					    vkey.Verse());
	gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry), tmpkey);
	gtk_widget_activate(navbar.lookup_entry);
	g_free(tmpkey);
	g_free(gkey);
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

static
void on_nt_book_menu_select(GtkMenuItem * menuitem, gpointer user_data)
{
	VerseKey vkey;
	char book = GPOINTER_TO_INT(user_data);
	
	vkey.AutoNormalize(1);
	vkey = navbar_versekey.key->str;
	vkey.Testament(2);
	vkey.Book(book+1);
	gtk_entry_set_text(GTK_ENTRY((c_dialog)? c_dialog->navbar.lookup_entry:navbar_versekey.lookup_entry),vkey.getText());
	gtk_widget_activate((c_dialog)? c_dialog->navbar.lookup_entry:navbar_versekey.lookup_entry);
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

static
void on_ot_book_menu_select(GtkMenuItem * menuitem, gpointer user_data)
{
	VerseKey vkey;
	char book = GPOINTER_TO_INT(user_data);
	
	vkey.AutoNormalize(1);
	vkey = (c_dialog)? c_dialog->navbar.key->str:navbar_versekey.key->str;
	vkey.Testament(0);
	vkey.Book(book+1);
	gtk_entry_set_text(GTK_ENTRY((c_dialog)? c_dialog->navbar.lookup_entry:navbar_versekey.lookup_entry),vkey.getText());
	gtk_widget_activate((c_dialog)? c_dialog->navbar.lookup_entry:navbar_versekey.lookup_entry);
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

static
void on_chapter_menu_select(GtkMenuItem * menuitem, gpointer user_data)
{
	VerseKey vkey;
	char chapter = GPOINTER_TO_INT(user_data);
		
	vkey.AutoNormalize(1);
	vkey = (c_dialog)? c_dialog->navbar.key->str:navbar_versekey.key->str;
	vkey.Chapter(chapter);
	gtk_entry_set_text(GTK_ENTRY((c_dialog)? c_dialog->navbar.lookup_entry:navbar_versekey.lookup_entry),vkey.getText());
	gtk_widget_activate((c_dialog)? c_dialog->navbar.lookup_entry:navbar_versekey.lookup_entry);
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

static
void on_verse_menu_select(GtkMenuItem * menuitem, gpointer user_data)
{	
	VerseKey vkey;
	char verse = GPOINTER_TO_INT(user_data);
	
	vkey.AutoNormalize(1);
	vkey = (c_dialog)? c_dialog->navbar.key->str:navbar_versekey.key->str;
	vkey.Verse(verse);
	gtk_entry_set_text(GTK_ENTRY((c_dialog)? c_dialog->navbar.lookup_entry:navbar_versekey.lookup_entry),vkey.getText());
	gtk_widget_activate((c_dialog)? c_dialog->navbar.lookup_entry:navbar_versekey.lookup_entry);
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
 *   seperate key info John 3:16 to book=john chapter=3 verse=16 and write
 *   to navbar labels - also set lookup entry text
 *
 * Return value
 *   void
 */

void main_navbar_versekey_set(NAVBAR_VERSEKEY navbar, const char * key)
{	
	char buf[5];
	char *gkey = NULL;
	char *tmpbuf = NULL;
	int book;
	gint i,x;	
	VerseKey vkey; 
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	
	if(!navbar.module_name->len)
		return;
	
	vkey.AutoNormalize(1);
	gkey = g_convert(key, -1, OLD_CODESET, UTF_8, &bytes_read,
			 &bytes_written, &error);
	if(gkey == NULL) {
		g_print ("error: %s\n", error->message);
		g_error_free (error);
		return;
	}
	vkey = gkey;
	
	if((backend->module_has_testament(navbar.module_name->str, 1))
				&& (vkey.Testament() == 2))
		book = 39 + vkey.Book();
	else 
		book = vkey.Book();
	
	
	tmpbuf = g_strdup_printf("<b>%s</b>",vkey.getBookName());
	gtk_label_set_label(GTK_LABEL(navbar.label_book_menu),tmpbuf);
	
	tmpbuf = g_strdup_printf("<b>%d</b>",vkey.Chapter());
	gtk_label_set_label(GTK_LABEL(navbar.label_chapter_menu),tmpbuf);
	
	tmpbuf = g_strdup_printf("<b>%d</b>",vkey.Verse());
	gtk_label_set_label(GTK_LABEL(navbar.label_verse_menu),tmpbuf);
	
	navbar.key = g_string_assign(navbar.key,(char*)vkey.getText());
	gtk_entry_set_text(GTK_ENTRY(navbar.lookup_entry),
				navbar.key->str);
	if(tmpbuf)
		g_free(tmpbuf);
	if(gkey)
		g_free(gkey);	
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

GtkWidget *main_versekey_drop_down_verse_menu(NAVBAR_VERSEKEY navbar, gpointer data)
{
	VerseKey vkey; 
	char *gkey = NULL;
	char buf[5];
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	gint i,x;
	GtkWidget *menu;
	GtkMenuShell *menu_shell;
	GtkWidget *item;
	GtkWidget *select_item;
	c_dialog = NULL;
	c_dialog = (DIALOG_DATA *) data;
	
	menu = gtk_menu_new();
	menu_shell = GTK_MENU_SHELL(menu);
	
	vkey.AutoNormalize(1);
	gkey = g_convert(navbar.key->str, -1, OLD_CODESET, UTF_8, &bytes_read,
			 &bytes_written, &error);
	if(gkey == NULL) {
		g_print ("error: %s\n", error->message);
		g_error_free (error);
		return NULL;
	}
	vkey = gkey;
	char xtestament = vkey.Testament() ;
	char xbook = vkey.Book();
	int xchapter = vkey.Chapter();
	int xverse = vkey.Verse();
		
	x = (vkey.books[xtestament-1][xbook-1].versemax[xchapter-1]);
	for(i=1; i <= x; i++) {
		sprintf(buf,"%d",i);
		if (i == xverse) {
			select_item = gtk_menu_item_new_with_label(buf);
			gtk_widget_show(select_item);	
			gtk_menu_shell_append(menu_shell, select_item);
			g_signal_connect(GTK_OBJECT(select_item), "activate",
				   G_CALLBACK
				   (on_verse_menu_select),
				   GINT_TO_POINTER(i));
		} else {
			item = gtk_menu_item_new_with_label(buf);
			gtk_widget_show(item);	
			gtk_menu_shell_append(menu_shell, item);
			g_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK
				   (on_verse_menu_select),
				   GINT_TO_POINTER(i));
		}
	}
	if(select_item)
		gtk_menu_shell_select_item(menu_shell,select_item);
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

GtkWidget *main_versekey_drop_down_chapter_menu(NAVBAR_VERSEKEY navbar, gpointer data)
{
	VerseKey vkey; 
	char *gkey = NULL;
	char buf[5];
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	vkey.AutoNormalize(1);
	gint i,x;
	GtkWidget *menu;
	GtkMenuShell *menu_shell;
	GtkWidget *item;
	GtkWidget *select_item;
	c_dialog = NULL;
	c_dialog = (DIALOG_DATA *) data;
	
	menu = gtk_menu_new();
	menu_shell = GTK_MENU_SHELL(menu);
	gkey = g_convert(navbar.key->str, -1, OLD_CODESET, UTF_8, &bytes_read,
			 &bytes_written, &error);
	if(gkey == NULL) {
		g_print ("error: %s\n", error->message);
		g_error_free (error);
		return NULL;
	}
	vkey = gkey;
	char xtestament = vkey.Testament() ;
	char xbook = vkey.Book();
	int xchapter = vkey.Chapter();
	x = (vkey.books[xtestament-1][xbook-1].chapmax);
	for(i=1; i <= x; i++) {
		sprintf(buf,"%d",i);
		if (i == xchapter) {
			select_item = gtk_menu_item_new_with_label(buf);
			gtk_widget_show(select_item);
			gtk_menu_shell_append(menu_shell, select_item);
			g_signal_connect(GTK_OBJECT(select_item), "activate",
				   G_CALLBACK
				   (on_chapter_menu_select),
				   GINT_TO_POINTER(i));
		} else {
			item = gtk_menu_item_new_with_label(buf);
			gtk_widget_show(item);	
			gtk_menu_shell_append(menu_shell, item);
			g_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK
				   (on_chapter_menu_select),
				   GINT_TO_POINTER(i));
		}
	}
	if(select_item)
		gtk_menu_shell_select_item(menu_shell,select_item);	
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

GtkWidget *main_versekey_drop_down_book_menu(NAVBAR_VERSEKEY navbar, gpointer data)
{
	VerseKey key; 
	VerseKey key_current; 
	GtkWidget *menu;
	GtkWidget *item;
	GtkWidget *select_item;
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	char *book = NULL;
	char *current_book = NULL;
	int i = 0, j = 0, x = 2;
	GtkMenuShell *menu_shell;
	c_dialog = NULL;
	c_dialog = (DIALOG_DATA *) data;
/*	if(c_dialog)
		g_message("c_dialog");
	else
		g_message("!c_dialog");*/
	if(navbar.testaments == backend->module_get_testaments(navbar.module_name->str))
		return NULL;
	
	navbar.testaments = backend->module_get_testaments(navbar.module_name->str);
	key_current = navbar.key->str;
	current_book = g_convert((const char *) key_current.getBookName(),
				     -1,
				     UTF_8,
				     OLD_CODESET,
				     &bytes_read,
				     &bytes_written,
				     &error);
	menu = gtk_menu_new();
	menu_shell = GTK_MENU_SHELL(menu);
	
	if (backend->module_has_testament(navbar.module_name->str, 1)) {
		while(i < key.BMAX[0]) { 
			book = g_convert((const char *) key.books[0][i].name,
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
			if (!strcmp(book, current_book)) {
				select_item = gtk_menu_item_new_with_label(book);
				gtk_widget_show(select_item);
				gtk_menu_shell_append(menu_shell, select_item);
				g_signal_connect(GTK_OBJECT(select_item), "activate",
					   G_CALLBACK
					   (on_ot_book_menu_select),
					   GINT_TO_POINTER(i));
			} else {
				item = gtk_menu_item_new_with_label(book);
				gtk_widget_show(item);
				gtk_menu_shell_append(menu_shell, item);
				g_signal_connect(GTK_OBJECT(item), "activate",
					   G_CALLBACK
					   (on_ot_book_menu_select),
					   GINT_TO_POINTER(i));
			}
			++i;
			g_free(book);
		}
	}
	i = 0;
	if (backend->module_has_testament(navbar.module_name->str, 2)) {
		while(i < key.BMAX[1]) {
			book = g_convert((const char *) key.books[1][i].name,
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
			if (!strcmp(book, current_book)) {
				select_item = gtk_menu_item_new_with_label(book);
				gtk_widget_show(select_item);
				gtk_menu_shell_append(menu_shell, select_item);
				g_signal_connect(GTK_OBJECT(select_item), "activate",
					   G_CALLBACK
					   (on_nt_book_menu_select),
					   GINT_TO_POINTER(i));
			} else {
				item = gtk_menu_item_new_with_label(book);
				gtk_widget_show(item);
				gtk_menu_shell_append(menu_shell, item);
				g_signal_connect(GTK_OBJECT(item), "activate",
					   G_CALLBACK
					   (on_nt_book_menu_select),
					   GINT_TO_POINTER(i));
			}
			++i;
			g_free(book);
		}
	}
	if(select_item)
		gtk_menu_shell_select_item(menu_shell,select_item);
	if(current_book)
		g_free(current_book);	
	return menu;
}
