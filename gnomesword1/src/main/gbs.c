/*
 * GnomeSword Bible Study Tool
 * gbs.c - generic book support - the glue
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

#include "gui/_gbs.h"
#include "gui/cipher_key_dialog.h"
#include "gui/shortcutbar_main.h"

#include "main/settings.h"
#include "main/gs_bookmarks.h"
#include "main/gs_gnomesword.h"
#include "main/gbs.h"
#include "main/gs_html.h"

#include "backend/sword.h"
#include "backend/shortcutbar.h"
#include "backend/gbs_.h"


/******************************************************************************
 * Name
 *   display_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void display_gbs(int page_num, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void display_gbs(int page_num, char * key)
{
	backend_display_gbs(page_num, key);
}

/******************************************************************************
 * Name
 *   treekey_next_sibling
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   int treekey_next_sibling(char * mod_name, unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int treekey_next_sibling(char * mod_name, unsigned long offset)
{
	return backend_treekey_next_sibling(mod_name, offset);
}

/******************************************************************************
 * Name
 *   gbs_treekey_has_children
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   int gbs_treekey_has_children(char * mod_name, unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int gbs_treekey_has_children(char * mod_name, unsigned long offset)
{
	return backend_gbs_treekey_has_children(mod_name, 
					offset);
}

/******************************************************************************
 * Name
 *   gbs_get_treekey_local_name
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   char * gbs_get_treekey_local_name(char *mod_name, 
 *					unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 
 
char * gbs_get_treekey_local_name(char *mod_name, 
					unsigned long offset)
{
	return backend_gbs_get_treekey_local_name(mod_name, offset);
}

/******************************************************************************
 * Name
 *   gbs_get_treekey_offset 
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   unsigned long gbs_get_treekey_offset(char *mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   unsigned long
 */ 
 
unsigned long gbs_get_treekey_offset(char *mod_name)
{
	return backend_gbs_get_treekey_offset(mod_name);
}

/******************************************************************************
 * Name
 *   gbs_treekey_first_child
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   int gbs_treekey_first_child(char * mod_name, unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int gbs_treekey_first_child(char * mod_name, unsigned long offset)
{
	return backend_gbs_treekey_first_child(mod_name, offset);
}
 
/******************************************************************************
 * Name
 *  get_book_key
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   char *get_book_key(int book_num)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 
 
char *get_book_key(int book_num)
{
	return backend_get_book_key(book_num);
}

/******************************************************************************
 * Name
 *  display_row_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   gboolean display_row_gbs(gint book_num, gchar *offset)	
 *
 * Description
 *    
 *
 * Return value
 *   gboolean
 */ 
 
gboolean display_row_gbs(gint book_num, gchar *offset)
{
	return backend_display_row_gbs(book_num, offset);
}


/******************************************************************************
 * Name
 *  get_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   GBS_DATA *get_gbs(GList * gbs)	
 *
 * Description
 *    
 *
 * Return value
 *   GBS_DATA*
 */ 
 
GBS_DATA *get_gbs(GList * gbs)
{
	GList *tmp;
	GBS_DATA *g = NULL;

	tmp = NULL;
	tmp = gbs;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		g = (GBS_DATA *) tmp->data;
		if (!strcmp(g->bookName, settings.BookWindowModule)) {
			break;
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return g;
}


/******************************************************************************
 * Name
 *  new_gbs_display 
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void new_gbs_display(GtkWidget * html, char * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void new_gbs_display(GtkWidget * html, char * mod_name)
{
	backend_new_gbs_display(html, mod_name);
}

