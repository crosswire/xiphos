/*
 * GnomeSword Bible Study Tool
 * url.c - support functions
 *
 * Copyright (C) 2004 GnomeSword Developer Team
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>

#include "gui/url.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/lists.h"
#include "main/settings.h"
#include "main/configs.h"
#include "main/module.h"
#include "main/sword.h"

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   
 *
 * Description
 *  
 *
 * Return value
 *   
 */



/******************************************************************************
 * Name
 *   sword_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint sword_uri(const gchar * url)
 *
 * Description
 *   handle a sword uri in the form 'sword://KJV/1John5:8'
 *                                   'sword://MHC/Genesis1:1'
 *                                   'sword:///Romans8:28'
 *   and display in the appropriate pane
 *
 * Return value
 *   gint
 */

static gint sword_uri(const gchar * url)
{
	gchar *buf = NULL;
	gchar *module = NULL;
	const gchar *key = NULL;
	static gchar **work_buf = NULL;
	gint mod_type;
	gint verse_count;
	gboolean change_verse = FALSE;
		
	work_buf = g_strsplit (url,"/",4);
	verse_count = start_parse_verse_list(work_buf[3], settings.currentverse);
	if(check_for_module(work_buf[2])) {
		mod_type = get_mod_type(work_buf[2]);
		switch(mod_type) {
			case TEXT_TYPE:			
				key = get_valid_key(work_buf[3]);
				xml_set_value("GnomeSword", "modules", "bible",
				      work_buf[2]);
				settings.MainWindowModule = work_buf[2];
				if(strcmp(key,settings.currentverse))
					change_verse = TRUE;
				gui_change_module_and_key(
					settings.MainWindowModule, 
					key);
				if(change_verse) {
					settings.currentverse = (gchar*)key;
					gui_change_verse(settings.currentverse);
				}
				if(key) g_free((gchar*)key);
				return 1;	
			break;
			case COMMENTARY_TYPE:				
				key = get_valid_key(work_buf[3]);
				xml_set_value("GnomeSword", "modules", "comm",
				      work_buf[2]);
				settings.CommWindowModule = work_buf[2];
				if(strcmp(key,settings.currentverse))
					change_verse = TRUE;
				gui_change_module_and_key(
					settings.CommWindowModule,
					key);
				if(change_verse) {
					settings.currentverse = (gchar*)key;
					gui_change_verse(settings.currentverse);
				}
				if(key) g_free((gchar*)key);
				return 1;				
			break;
			case DICTIONARY_TYPE:
				settings.dictkey = work_buf[3];
				xml_set_value("GnomeSword", "modules", "dict",
				      work_buf[2]);
				settings.DictWindowModule = work_buf[2];
				gui_change_module_and_key(
					settings.DictWindowModule,
					settings.dictkey);
				gtk_notebook_set_current_page(
					GTK_NOTEBOOK(widgets.workbook_lower),0);
				return 1;	
			break;
			case BOOK_TYPE:
				settings.book_key = work_buf[3];
				xml_set_value("GnomeSword", "modules", "book",
				      work_buf[2]);
				settings.BookWindowModule = work_buf[2];
				gui_change_module_and_key(
					settings.BookWindowModule,
					settings.book_key);
				gtk_notebook_set_current_page(
					GTK_NOTEBOOK(widgets.workbook_lower),1);
				return 1;	
			break;
		}
	} else { /* module name not found or not given */
		if(verse_count) { 
			key = get_valid_key(work_buf[3]); 
			settings.currentverse = (gchar*)key;
			/* display in current Bible and Commentary */
			gui_change_verse(settings.currentverse);
			if(key) g_free((gchar*)key);
		}
	}
	return 0;
	
}

/******************************************************************************
 * Name
 *   gui_url_handler
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint gui_url_handler(const gchar * url)
 *
 * Description
 *   
 *
 * Return value
 *   gint
 */

gint gui_url_handler(const gchar * url)
{	
	if(strstr(url,"sword://"))
		return sword_uri(url);

}

/******   end of file   ******/
