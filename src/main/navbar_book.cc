/*
 * Xiphos Bible Study Tool
 * navbar_book.cc - glue between all navbar_book and sword
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
#include <versekey.h>

#include "main/navbar_book.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "gui/navbar_book.h"
#include "gui/utilities.h"

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

/******************************************************************************
 * Name
 *  check_for_parent
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *  int check_for_parent(char *book, unsigned long offset)
 *
 * Description
 *   check to see if there is a parent to  the curret item
 *
 * Return value
 *   int
 */

static int check_for_parent(char *book, unsigned long offset)
{
	unsigned long offset_save;

	offset_save = offset;
	backend->set_module(book);
	backend->set_treekey(offset);

	if (backend->treekey_parent(offset)) {
		backend->set_treekey(offset_save);
		return 1;
	}
	return 0;
}

/******************************************************************************
 * Name
 *  check_for_prev_sib
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *  int check_for_prev_sib(char * book, unsigned long offset)
 *
 * Description
 *   check to see if there is a sibling before the curret one
 *
 * Return value
 *   int
 */

static int check_for_prev_sib(char *book, unsigned long offset)
{
	unsigned long offset_save;

	offset_save = offset;
	backend->set_module(book);
	backend->set_treekey(offset);
	if (backend->treekey_prev_sibling(offset)) {
		backend->set_treekey(offset_save);
		return 1;
	}
	return 0;
}

/******************************************************************************
 * Name
 *  check_for_next_sib
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *  int check_for_next_sib(char * book, unsigned long offset)
 *
 * Description
 *   check to see if there is another sibling after the current one
 *
 * Return value
 *  int
 */

static int check_for_next_sib(char *book, unsigned long offset)
{
	unsigned long offset_save;

	offset_save = offset;
	backend->set_module(book);
	backend->set_treekey(offset);
	if (backend->treekey_next_sibling(offset)) {
		backend->set_treekey(offset_save);
		return 1;
	}
	return 0;
}

/******************************************************************************
 * Name
 *  main_navbar_book_parent
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *  void main_navbar_book_parent(void)
 *
 * Description
 *   left button clicked - display parent
 *
 * Return value
 *   void
 */

void main_navbar_book_parent(void)
{
	backend->set_module(settings.book_mod);
	backend->set_treekey(settings.book_offset);
	if (backend->treekey_parent(settings.book_offset)) {
		unsigned long offset = backend->get_treekey_offset();
		char *tmpbuf = backend->treekey_get_local_name(offset);

		gtk_entry_set_text(GTK_ENTRY(navbar_book.lookup_entry), tmpbuf);
		g_free(tmpbuf);
		tmpbuf = g_strdup_printf("%ld", offset);
		main_display_book(settings.book_mod, tmpbuf);
		g_free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *  main_navbar_book_first_child
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *  void main_navbar_book_first_child(void)
 *
 * Description
 *   right button clicked - display first child
 *
 * Return value
 *   void
 */

void main_navbar_book_first_child(void)
{
	backend->set_module(settings.book_mod);
	backend->set_treekey(settings.book_offset);
	if (backend->treekey_first_child(settings.book_offset)) {
		unsigned long offset = backend->get_treekey_offset();
		char *tmpbuf = backend->treekey_get_local_name(offset);

		gtk_entry_set_text(GTK_ENTRY(navbar_book.lookup_entry), tmpbuf);
		g_free(tmpbuf);
		tmpbuf = g_strdup_printf("%ld", offset);
		main_display_book(settings.book_mod, tmpbuf);
		g_free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *  main_navbar_book_prev
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *  void main_navbar_book_prev(void)
 *
 * Description
 *   up button clicked - display previous sibling
 *
 * Return value
 *   void
 */

void main_navbar_book_prev(void)
{
	backend->set_module(settings.book_mod);
	backend->set_treekey(settings.book_offset);
	if (backend->treekey_prev_sibling(settings.book_offset)) {
		unsigned long offset = backend->get_treekey_offset();
		char *tmpbuf = backend->treekey_get_local_name(offset);

		gtk_entry_set_text(GTK_ENTRY(navbar_book.lookup_entry), tmpbuf);
		g_free(tmpbuf);
		tmpbuf = g_strdup_printf("%ld", offset);
		main_display_book(settings.book_mod, tmpbuf);
		g_free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *  main_navbar_book_next
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *  void main_navbar_book_next(void)
 *
 * Description
 *   down button clicked - display next sibling
 *
 * Return value
 *   void
 */

void main_navbar_book_next(void)
{
	backend->set_module(settings.book_mod);
	backend->set_treekey(settings.book_offset);
	if (backend->treekey_next_sibling(settings.book_offset)) {
		unsigned long offset = backend->get_treekey_offset();
		char *tmpbuf = backend->treekey_get_local_name(offset);

		gtk_entry_set_text(GTK_ENTRY(navbar_book.lookup_entry), tmpbuf);
		g_free(tmpbuf);
		tmpbuf = g_strdup_printf("%ld", offset);
		main_display_book(settings.book_mod, tmpbuf);
		g_free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *   main_navbar_book_entry_activate
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *   void  main_navbar_book_entry_activate (const gchar* entry_text)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_navbar_book_entry_activate(const gchar *entry_text)
{
	main_display_book(settings.book_mod, entry_text);
}

/******************************************************************************
 * Name
 *  on_menu_select(
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *   void on_menu_select(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   user clicked on an item in the drop down list - send the book offset
 *   stored in user_data to main_display_book to show
 *
 * Return value
 *   void
 */

static void on_menu_select(GtkMenuItem *menuitem, gpointer user_data)
{
	char *tmpbuf = NULL;

	tmpbuf = g_strdup_printf("%d", GPOINTER_TO_INT(user_data));
	main_display_book(settings.book_mod, tmpbuf);
}

/******************************************************************************
 * Name
 *   main_book_drop_down_new
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *   GtkWidget *main_book_drop_down_new(void)
 *
 * Description
 *   create a menu of all the current items in the current section of current genbook
 *   and return it when the list button is toggled
 *
 * Return value
 *   GtkWidget * (menu)
 */

GtkWidget *main_book_drop_down_new(void)
{
	gchar *tmpbuf = NULL;
	GtkWidget *menu;
	GtkWidget *item;
	unsigned long offset;

	backend->set_module(settings.book_mod);
	backend->set_treekey(settings.book_offset);
	menu = gtk_menu_new();
	offset = settings.book_offset;
	/* take us to the first sibling */
	while (backend->treekey_prev_sibling(offset)) {
		offset = backend->get_treekey_offset();
	}
	/* add menu item for first sibling*/
	tmpbuf = backend->treekey_get_local_name(offset);
	item = gtk_menu_item_new_with_label((gchar *)tmpbuf);
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item), "activate",
			 G_CALLBACK(on_menu_select),
			 GINT_TO_POINTER(offset));
	gtk_container_add(GTK_CONTAINER(menu), item);
	g_free(tmpbuf);

	while (backend->treekey_next_sibling(offset)) {
		offset = backend->get_treekey_offset();
		/* add menu item */
		tmpbuf = backend->treekey_get_local_name(offset);
		item = gtk_menu_item_new_with_label((gchar *)tmpbuf);
		gtk_widget_show(item);
		g_signal_connect(G_OBJECT(item), "activate",
				 G_CALLBACK(on_menu_select),
				 GINT_TO_POINTER(offset));
		gtk_container_add(GTK_CONTAINER(menu), item);
		g_free(tmpbuf);
	}
	return menu;
}

/******************************************************************************
 * Name
 *   main_setup_navbar_book
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *   void main_setup_navbar_book(gchar * book_name, unsigned long offset)
 *
 *
 * Description
 *   set  sensitive state of navbar buttons and set the entry to current book key
 *
 * Return value
 *   void
 */

void main_setup_navbar_book(gchar *book_name, unsigned long offset)
{
	gchar *tmpbuf = NULL;
	XI_message(("offset: %ld", offset));
	backend->set_module(book_name);
	backend->set_treekey(offset);

	tmpbuf = backend->get_key_from_offset(offset);
	gtk_entry_set_text(GTK_ENTRY(navbar_book.lookup_entry), tmpbuf);
	gtk_widget_set_tooltip_text(navbar_book.lookup_entry, tmpbuf);

	if ((settings.book_mod == NULL) ||
	    (*settings.book_mod == '\0'))
		gui_reassign_strdup(&settings.book_mod, book_name);

	if (check_for_parent(settings.book_mod, settings.book_offset))
		gtk_widget_set_sensitive(navbar_book.button_left, TRUE);
	else
		gtk_widget_set_sensitive(navbar_book.button_left, FALSE);

	if (backend->treekey_has_children(offset)) {
		gtk_widget_set_sensitive(navbar_book.button_right, TRUE);
	} else {
		gtk_widget_set_sensitive(navbar_book.button_right, FALSE);
	}

	if (check_for_prev_sib(settings.book_mod, settings.book_offset))
		gtk_widget_set_sensitive(navbar_book.button_up, TRUE);
	else
		gtk_widget_set_sensitive(navbar_book.button_up, FALSE);

	if (check_for_next_sib(settings.book_mod, settings.book_offset))
		gtk_widget_set_sensitive(navbar_book.button_down, TRUE);
	else
		gtk_widget_set_sensitive(navbar_book.button_down, FALSE);

	free(tmpbuf);
}
