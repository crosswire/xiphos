/*
 * GnomeSword Bible Study Tool
 * utilities.h - support functions
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

#ifdef __cplusplus
extern "C" {
#endif
	
#include <gnome.h>
#include "main/configs.h"


void gui_load_module_tree(GtkWidget * tree, gboolean is_sidebar);
MOD_FONT *get_font(gchar * mod_name);
void free_font(MOD_FONT *mf);
gchar * remove_linefeeds(gchar * buf);	
void gui_add_item2gnome_menu(GtkWidget * MainFrm, gchar * itemname,
     gchar * itemdata, gchar * submenuname, GCallback mycallback);
void gui_add_separator2menu(GtkWidget * MainFrm, gchar * subtreelabel);
void gui_add_mods_to_menus(GList * modlist, gchar * menu,
					GCallback callback);
void gui_remove_menu_items(gchar *startitem, gint numberofitems);
void gui_add_mods_2_gtk_menu(gint mod_type, GtkWidget * menu,
				GCallback callback);
GList *gui_fill_count_list(int count);
void gui_free_count_list(GList *glist);
	
#ifdef __cplusplus
}
#endif
