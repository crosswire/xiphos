/*
 * GnomeSword Bible Study Tool
 * gs_viewtext.c - text view dialog glue
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
#include <gtkhtml/gtkhtml.h>

/*
 * main
 */ 
#include "gs_gnomesword.h"
#include "settings.h"
#include "bibletext_view.h"

/*
 * backend
 */
#include "bibletext_view_.h"
#include "sword.h"

/******************************************************************************
 * externs
 */
/* is the view text dialog runing */
extern gboolean isrunningVT;
extern gboolean gsI_isrunning;

/******************************************************************************
 * Name
 *  get_book_viewtext 
 *
 * Synopsis
 *   #include "gs_viewtext.h"
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
 *   #include "gs_viewtext.h"
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
 *   #include "gs_viewtext.h"
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
 *   #include "gs_viewtext.h"
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
 *   #include "gs_viewtext.h"
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
 *   #include "gs_viewtext.h"
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
 *   #include "gs_viewtext.h"
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
 *   #include "gs_viewtext.h"
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
