/*
 * GnomeSword Bible Study Tool
 * gbs.c - generic book support - the gui
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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


#include "gui/gtkhtml_display.h"
#include "gui/gbs.h"
#include "gui/gbs_dialog.h"
#include "gui/gbs_display.h"
#include "gui/gbs_menu.h"
#include "gui/mod_global_ops.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/find_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/bookmarks.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/lists.h"
#include "main/gbs.h"
#include "main/sword.h"
#include "main/xml.h"


/******************************************************************************
 *  externs  
 */
extern gboolean in_url;

/******************************************************************************
 *  static   
 */
static gboolean gbs_find_running;



/******************************************************************************
 * Name
 *  gui_set_gbs_frame_label
 *
 * Synopsis
 *   #include "_gbs.h"
 *
 *   void gui_set_gbs_frame_label(void)	
 *
 * Description
 *   sets gbs label to module name
 *
 * Return value
 *   void
 */

static void set_gbs_label(gchar * mod_name)
{
	/*
	 * set label to module name
	 */
	gtk_label_set_text (GTK_LABEL(widgets.label_comm),mod_name);
}

/******************************************************************************
 * Name
 *  gui_set_book_page_and_key
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_set_book_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_set_book_mod_and_key(gchar * mod_name, gchar * key)
{
	settings.comm_showing = FALSE;
	settings.whichwindow = BOOK_WINDOW;
	main_display_book();
	set_gbs_label(mod_name);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include ".h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_update_gbs_global_ops(gchar * option, gboolean choice)
{
	/*save_module_options(cur_t->mod_name, option, 
				    choice);*/
	//gbs_display(cur_g, tree_level);
}
