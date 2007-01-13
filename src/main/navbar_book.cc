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

#include "main/navbar_book.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "gui/navbar_book.h"


#include "backend/sword_main.hh"


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *  
 *
 * Description
 *   
 *
 * Return value
 *   
 */

void main_navbar_book_parent(void)
{
	char *tmpbuf = NULL;
	unsigned long offset;
	
	backend->set_module(settings.book_mod);
	backend->set_treekey(settings.book_offset);
	//offset = backend->get_treekey_offset();		
	if (backend->treekey_parent(settings.book_offset)) {
		offset = backend->get_treekey_offset();		
		tmpbuf = backend->treekey_get_local_name(offset);
		gtk_entry_set_text(GTK_ENTRY(navbar_book.lookup_entry),  tmpbuf);
		g_free(tmpbuf);
		tmpbuf = g_strdup_printf("%ld",offset);
		//main_setup_navbar_book(settings.book_mod, offset);
		main_display_book(settings.book_mod,tmpbuf);
		g_free(tmpbuf);
	}
	

}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main/navbar_book.h"
 *
 *  
 *
 * Description
 *   
 *
 * Return value
 *   
 */

void main_navbar_book_first_child(void)
{
	char *tmpbuf = NULL;
	unsigned long offset;
	
	backend->set_module(settings.book_mod);
	backend->set_treekey(settings.book_offset);
	//offset = backend->get_treekey_offset();		
	if (backend->treekey_first_child(settings.book_offset)) {
		offset = backend->get_treekey_offset();		
		tmpbuf = backend->treekey_get_local_name(offset);
		gtk_entry_set_text(GTK_ENTRY(navbar_book.lookup_entry),  tmpbuf);
		g_free(tmpbuf);
		tmpbuf = g_strdup_printf("%ld",offset);
		//main_setup_navbar_book(settings.book_mod, offset);
		main_display_book(settings.book_mod,tmpbuf);
		g_free(tmpbuf);
	}
	

}


static
void on_menu_select(GtkMenuItem * menuitem, gpointer user_data)
{
	char *tmpbuf = NULL;
	
	//main_setup_navbar_book(settings.book_mod, GPOINTER_TO_INT(user_data));
	tmpbuf = g_strdup_printf("%ld",GPOINTER_TO_INT(user_data));
	main_display_book(settings.book_mod,tmpbuf);

}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   
 *
 * Description
 *   text in the dictionary entry has changed and the entry activated
 *
 * Return value
 *   void
 */

GtkWidget *main_book_drop_down_new(void)
{	
	gchar *tmpbuf = NULL;
	GtkWidget *menu;
	GtkWidget * item;
	unsigned long offset;
	
	backend->set_module(settings.book_mod);
	backend->set_treekey(settings.book_offset);
	menu = gtk_menu_new();
	offset = settings.book_offset;
	
	while(backend->treekey_prev_sibling(offset)) {
		offset = backend->get_treekey_offset();
	}
	
		tmpbuf = backend->treekey_get_local_name(offset);
		
		/* add menu item */
		item =
		    gtk_menu_item_new_with_label((gchar *) tmpbuf);
		gtk_widget_show(item);
		g_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK(on_menu_select),
				   GINT_TO_POINTER(offset));	
		gtk_container_add(GTK_CONTAINER(menu), item); 
		g_free(tmpbuf);
	while (backend->treekey_next_sibling(offset)) {
		offset = backend->get_treekey_offset();
		tmpbuf = backend->treekey_get_local_name(offset);
		
		/* add menu item */
		item =
		    gtk_menu_item_new_with_label((gchar *) tmpbuf);
		gtk_widget_show(item);
		g_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK(on_menu_select),
				   GINT_TO_POINTER(offset));	
		gtk_container_add(GTK_CONTAINER(menu), item); 
		g_free(tmpbuf);
	}
	return menu;
} 


void main_setup_navbar_book(gchar * book_name, unsigned long offset)
{
	gchar buf[256];
	gchar *tmpbuf = NULL;
	
	backend->set_module(book_name);
	backend->set_treekey(offset);
	
//	if (backend->treekey_first_child(offset)) {
//		offset = backend->get_treekey_offset();
//		sprintf(buf, "%lu", offset);
		tmpbuf = backend->treekey_get_local_name(offset);
		gtk_entry_set_text(GTK_ENTRY(navbar_book.lookup_entry),  tmpbuf);
		
		if(offset > 0)
			gtk_widget_set_sensitive(navbar_book.button_up,TRUE);
		else
			gtk_widget_set_sensitive(navbar_book.button_up,FALSE);		
			
		if (backend->treekey_has_children(offset)) {
			gtk_widget_set_sensitive(navbar_book.button_down,TRUE);
			/*gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CAPTION,
					   (gchar *) tmpbuf, COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) buf,
					   -1);*/
		} else {
			gtk_widget_set_sensitive(navbar_book.button_down,FALSE);
			/*gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION,
					   (gchar *) tmpbuf, COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) buf,
					   -1);*/
		}
		free(tmpbuf);
//	}

/*	while (backend->treekey_next_sibling(offset)) {
		offset = backend->get_treekey_offset();
		sprintf(buf, "%lu", offset);
		tmpbuf = backend->treekey_get_local_name(offset);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		if (backend->treekey_has_children(offset)) {
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CAPTION,
					   (gchar *) tmpbuf, COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) buf,
					   -1);
		} else {
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION,
					   (gchar *) tmpbuf, COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) buf,
					   -1);
		}
		free(tmpbuf);
	}
*/
}
