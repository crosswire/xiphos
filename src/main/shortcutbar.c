/*
 * GnomeSword Bible Study Tool
 * shortcutbar.c - glue  (: very sticky :)
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
#include <config.h>
#endif

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif
#include <stdio.h>
#include <string.h>

#include "gui/shortcutbar_main.h"

#include "main/shortcutbar.h"
#include "main/settings.h"

#include "backend/sword.h"

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

GList *load_sb_group(gchar * file_name, gchar * group_name,
		     gchar * icon_size)
{
	GList *glist = NULL;
	gchar conf_file[256];
	const gchar *buf;
	
	strcpy(conf_file, file_name);
	//g_warning(conf_file);
	backend_open_config_file(conf_file);
	sprintf(group_name, "%s",
		backend_get_config_value("Shortcut Info","Group Name"));
	sprintf(icon_size, "%s",
		backend_get_config_value("Shortcut Info","Large Icon"));
	
	backend_set_config_to_get_labels("ROOT");
	while ((buf = backend_get_next_config_value()) != NULL) {
		glist = g_list_append(glist, (gchar*)buf);
	}
	backend_close_config_file();
	return glist;
	//return backend_load_sb_group(filename, group_name, icon_size);
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

void save_sb_group(GList * group_list, gchar * file_name, 
		gchar * group_name, char *large_icons)
{
	GList *tmp = NULL;
	char conf_file[256];
	char buf[80];
	int j = 0;
	//const gchar *buf;
	
	sprintf(conf_file, "%s/%s", settings.shortcutbarDir, file_name);
	
	backend_open_config_file(conf_file);	
	backend_save_value_to_config_file("Shortcut Info", 
					"Group Name", group_name);
	backend_save_value_to_config_file("Shortcut Info", 
					"Large Icon", large_icons);
	backend_erase_config_section("ROOT");
	tmp = group_list;
	while(tmp != NULL) {
		sprintf(buf, "branch%d", j++);
		backend_save_value_to_config_file("ROOT", 
					buf, (char *) tmp->data);
		g_print("saving list item: %s\n", (char *) tmp->data);
		g_free((char *) tmp->data);
		tmp = g_list_next(tmp);		
	}
	g_list_free(tmp);
	backend_close_config_file();
	/*
	backend_save_sb_group(group_list, file_name, group_name,
			      large_icons);
	*/
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
	
	char conf_file[256];
	sprintf(conf_file, "%s/%s", settings.shortcutbarDir, file_name);
	//g_warning(conf_file);
	backend_open_config_file(conf_file);
	backend_save_value_to_config_file("Shortcut Info", 
					"Large Icon", icons);	
	backend_close_config_file();
	
	/*backend_save_sb_iconsize(file_name, icons);*/
	
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

/*void get_shortcut_item_info(gint group_num, gint item_num,
			    gchar ** item_url, gchar ** item_name)
{
	gui_get_shortcut_item_info(group_num, item_num,
				   item_url, item_name);

}*/


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

/*gint get_num_shortcut_items(gint group_num)
{
	return gui_get_num_shortcut_items(group_num);

}*/
