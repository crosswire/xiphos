/*
 * GnomeSword Bible Study Tool
 * commentary.c - glue for commentary modules and commentary dialog
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
#  include <config.h>
#endif

#include <gnome.h>

#include "gui/commentary.h"
#include "gui/commentary_dialog.h"
#include "gui/cipher_key_dialog.h"
#include "gui/shortcutbar_main.h"

#include "main/commentary.h"
#include "main/settings.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"

#include "backend/shortcutbar.h"
#include "backend/sword.h"
#include "backend/commentary_.h"
#include "backend/viewcomm.h"


/******************************************************************************
 * globals
 */ 
gboolean comm_display_change = TRUE;


/******************************************************************************
 * Name
 *   backend_display_book_heading
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void backend_display_book_heading(gint modnum)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void display_book_heading(int mod_num)
{
	backend_display_book_heading(mod_num);
}

/******************************************************************************
 * Name
 *   display_chap_heading
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   	void display_chap_heading(int mod_num)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void display_chap_heading(int mod_num)
{
	backend_display_chap_heading(mod_num);
}

/******************************************************************************
 * Name
 *   navigate_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   const char* navigate_commentary(gint modnum, gint direction)	
 *
 * Description
 *    navigate foward or backward through commentary
 *
 * Return value
 *   void
 */
 
const char* navigate_commentary(gint modnum, gint direction)
{
	return backend_nav_commentary(modnum, direction);
}

/******************************************************************************
 * Name
 *   display_comm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void display_comm(gchar * key)	
 *
 * Description
 *    display new key in current commentary
 *
 * Return value
 *   void
 */
 
void display_comm(gint page_num, gchar * key)
{        
	backend_display_commentary(page_num, key);
}

/******************************************************************************
 * Name
 *   new_display_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void new_display_commentary(GtkWidget * html, gchar * modName)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void new_display_commentary(GtkWidget * html, gchar * modName)
{
	backend_new_display_commentary(html, modName);
}

/******************************************************************************
 * Name
 *   goto_verse_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void goto_verse_viewcomm(gchar *verse)	
 *
 * Description
 *   change to new verse in viewcomm and display in commentary_dialog 
 *
 * Return value
 *   void
 */
 
void goto_verse_viewcomm(gchar *verse)
{
	backend_goto_verse_viewcomm(verse);
}

/******************************************************************************
 * Name
 *   load_module_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void load_module_viewcomm(gchar *module_name)	
 *
 * Description
 *    load module into commentary_dialog
 *
 * Return value
 *   void
 */
 
void load_module_viewcomm(gchar *module_name)
{
	backend_load_module_viewcomm(module_name);
}

/******************************************************************************
 * Name
 *   nav_module_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void nav_module_viewcomm(gint direction)	
 *
 * Description
 *    navagate commentary called by commentary_dialog
 *
 * Return value
 *   void
 */
 
void nav_module_viewcomm(gint direction)
{
	backend_nav_module_viewcomm(direction);
}

/******************************************************************************
 * Name
 *   setup_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void setup_viewcomm(GtkWidget * text)	
 *
 * Description
 *    setup viewcomm for commentary_dialog
 *
 * Return value
 *   void
 */
 
void setup_viewcomm(GtkWidget * text)
{
	backend_setup_viewcomm(text);
}

/******************************************************************************
 * Name
 *   shutdown_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void shutdown_viewcomm(void)	
 *
 * Description
 *    call backend_shutdown_viewcomm to shutdown viewcomm
 *
 * Return value
 *   void
 */
 
void shutdown_viewcomm(void)
{
	backend_shutdown_viewcomm();
}
/******  end of file  ******/
