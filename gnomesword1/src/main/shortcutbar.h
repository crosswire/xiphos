/*
 * GnomeSword Bible Study Tool
 * shortcutbar.h - glue
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

#ifndef _SHORTCUTBAR_H__
#define _SHORTCUTBAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <glib-1.2/glib.h>
  
typedef struct _sword_search SEARCH_SWORD;
struct  _sword_search {
	
	gchar
	    *module_name,
	    *upper_bond, 
	    *lower_bond, 
	    *search_string;

	gint
	    search_type, 
	    search_params, 
	    found_count;

	 gboolean 
	    use_bonds, 
	    use_lastsearch_for_bonds;
};
/*** function prototypes ***/

GList *load_sb_group(gchar *filename, gchar *group_name, 
						gchar *icon_size);
void save_sb_group(gchar *file_name, gchar *group_name, gint group_num,
						     char *large_icons);
void save_sb_iconsize(gchar *file_name, char *icons);
gint get_num_shortcut_items(gint group_num); 
void get_shortcut_item_info(gint group_num,
				     gint item_num,
				     gchar ** item_url,
				     gchar ** item_name);

#ifdef __cplusplus
}
#endif

#endif

