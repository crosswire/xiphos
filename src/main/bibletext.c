/*
 * GnomeSword Bible Study Tool
 * bibletext.c - support for Bible text modules
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

#include <gnome.h>

#include "gui/_bibletext.h"
#include "gui/shortcutbar_main.h"
#include "gui/cipher_key_dialog.h"
#include "backend/module_options.h"

#include "main/settings.h"
#include "main/bibletext.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"

#include "backend/bibletext_.h"
#include "backend/bibletext_view_.h"
#include "backend/sword.h"
#include "backend/shortcutbar.h"


/******************************************************************************
 * Name
 *  text_get_description
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   const char* text_get_description(int mod_num)	
 *
 * Description
 *    
 *
 * Return value
 *   const char*
 */
 
const char *text_get_description(int mod_num)
{
	return backend_get_text_module_description(mod_num);
}

/******************************************************************************
 * Name
 *  check_for_global_option
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  	gint check_for_global_option(gint mod_num, gchar * option)
 *
 * Description
 *    get global options for a module
 *
 * Return value
 *   gint
 */

gint check_for_global_option(gint mod_num, gchar * option)
{
	return backend_check_for_global_option(mod_num, option);
}

/******************************************************************************
 * Name
 *  set_text_module_global_options
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void set_text_module_global_options(gchar * option, gboolean choice)	
 *
 * Description
 *   set sword global option 
 *
 * Return value
 *   void
 */

void set_text_module_global_option(gchar * option, gboolean choice)
{
	gchar *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}
	
	backend_save_module_options(settings.MainWindowModule,
				    option, on_off);
	backend_set_text_global_option(option, on_off);
	display_text(settings.currentverse);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void set_text_global_option(gchar * option, gchar * choice)
{
	backend_set_text_global_option(option, choice);
}

/******************************************************************************
 * Name
 *  display_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   void display_text(gchar * key)
 *  	
 *
 * Description
 *   call backend_display_text and pass module page num and key
 *
 * Return value
 *   void
 */

void display_text(gchar * key) 
{
	backend_display_text(settings.text_last_page, key);
}

/******************************************************************************
 * Name
 *  new_text_display
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void new_text_display(GtkWidget * html, gchar * mod_name)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void new_text_display(GtkWidget * html, gchar * mod_name)
{
	backend_new_text_display(html, mod_name);
}

/******************************************************************************
 * Name
 *  get_book_viewtext 
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *    const char *get_book_viewtext(void)	
 *
 * Description
 *   get current book displayed in veiw text dialog
 *
 * Return value
 *   const char *
 */

const char *get_book_viewtext(void)
{
	return backend_get_book_viewtext();
}

/******************************************************************************
 * Name
 *  get_chapter_viewtext 
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   int get_chapter_viewtext(void)	
 *
 * Description
 *   get current chapter displayed in veiw text dialog
 *
 * Return value
 *   int
 */

int get_chapter_viewtext(void)
{
	return backend_get_chapter_viewtext();
}

/******************************************************************************
 * Name
 *   get_verse_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   int get_verse_viewtext(void)	
 *
 * Description
 *   get current verse displayed in veiw text dialog
 *
 * Return value
 *   int
 */

int get_verse_viewtext(void)
{	
	return backend_get_verse_viewtext();
}

/******************************************************************************
 * Name
 *   goto_verse_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void goto_verse_viewtext(gchar * verse)	
 *
 * Description
 *   find and display new verse for view text dialog
 *
 * Return value
 *   void
 */

void goto_verse_viewtext(char * verse)
{
	backend_goto_verse_viewtext(verse);
}

/******************************************************************************
 * Name
 *   load_module_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void load_module_viewtext(gchar * modName)	
 *
 * Description
 *   load a text module into the view text dialog
 *
 * Return value
 *   void
 */

void load_module_viewtext(char * module_name)
{
	backend_load_module_viewtext(module_name);
}

/******************************************************************************
 * Name
 *   shutdown_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void shutdown_viewtext(void)	
 *
 * Description
 *   shutdown down sword viewtext support
 *
 * Return value
 *   void
 */

void shutdown_viewtext(void)
{
	backend_shutdown_viewtext();
}

/******************************************************************************
 * Name
 *   set_global_options_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void set_global_options_viewtext(gchar *option, gboolean choice)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void set_global_options_viewtext(gchar *option, gboolean choice)
{
	if (choice) {	
		backend_set_global_options_viewtext(option,
						    "On");
	} else {		
		backend_set_global_options_viewtext(option,
						    "Off");
	}
}

/******************************************************************************
 * Name
 *   setup_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void setup_viewtext(GtkWidget * text)	
 *
 * Description
 *   setup the viewtext sword display
 *
 * Return value
 *   void
 */

void setup_viewtext(GtkWidget * text)
{
	backend_setup_viewtext(text);
}
