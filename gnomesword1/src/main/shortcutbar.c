/*
 * GnomeSword Bible Study Tool
 * shortcutbar.c - glue  (: very sticky :)
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

#include <glib-1.2/glib.h>

#include "gui/shortcutbar_main.h"

#include "main/shortcutbar.h"

#include "backend/sword.h"
#include "backend/shortcutbar.h"

/******************************************************************************
 * Name
 *  load_sb_group
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   GList *load_sb_group(gchar *filename, gchar *group_name, 
 *						gchar *icon_size)	
 *
 * Description
 *    
 *
 * Return value
 *  GList * 
 */

GList *load_sb_group(gchar * filename, gchar * group_name,
		     gchar * icon_size)
{
	return backend_load_sb_group(filename, group_name, icon_size);
}


/******************************************************************************
 * Name
 *  save_sb_group
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void save_sb_group(gchar *file_name, gchar *group_name, gint group_num,
						     char *large_icons)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void save_sb_group(gchar * file_name, gchar * group_name,
		   gint group_num, char *large_icons)
{
	gchar *item_url, *item_name;
	gint j, number_of_items = 0;
	GList *group_list = NULL;

	number_of_items = get_num_shortcut_items(group_num);

	for (j = 0; j < number_of_items; j++) {
		get_shortcut_item_info(group_num, j, &item_url,
				       &item_name);
		
		group_list = g_list_append(group_list, (char *) g_strdup(item_name));
		g_warning("%s", item_name);
		g_free(item_url);
		g_free(item_name);
	}
	backend_save_sb_group(group_list, file_name, group_name, group_num,
			      large_icons);
	g_list_free(group_list);
}


/******************************************************************************
 * Name
 *  save_sb_iconsize
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void save_sb_iconsize(gchar *file_name, char *icons)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void save_sb_iconsize(gchar * file_name, char *icons)
{
	backend_save_sb_iconsize(file_name, icons);
}


/******************************************************************************
 * Name
 *    get_shortcut_item_info
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void get_shortcut_item_info(gint group_num, gint item_num, 
 *				gchar **item_url, gchar **item_name)	
 *
 * Description
 *   get shortcut item information
 *
 * Return value
 *   void
 */

void get_shortcut_item_info(gint group_num, gint item_num,
			    gchar ** item_url, gchar ** item_name)
{
	gui_get_shortcut_item_info(group_num, item_num,
				   item_url, item_name);

}


/******************************************************************************
 * Name
 *    get_num_shortcut_items
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   gint get_num_shortcut_items(gint group_num)	
 *
 * Description
 *   returns the number of shortcut items in the current group
 *
 * Return value
 *   gint
 */

gint get_num_shortcut_items(gint group_num)
{
	return gui_get_num_shortcut_items(group_num);

}
