/*
 * GnomeSword Bible Study Tool
 * shortcutbar.h - glue
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

#ifndef _SHORTCUTBAR_H__
#define _SHORTCUTBAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <glib-1.2/glib.h>

/*** function prototypes ***/

GList *load_sb_group(gchar *filename, gchar *group_name, 
						gchar *icon_size);
void save_sb_group(GList * group_list, gchar *file_name, 
		gchar *group_name, gchar *large_icons);
void save_sb_iconsize(gchar *file_name, gchar *icons);
/*gint get_num_shortcut_items(gint group_num); */
/*void get_shortcut_item_info(gint group_num, gint item_num,
			gchar ** item_url, gchar ** item_name);*/

#ifdef __cplusplus
}
#endif

#endif
