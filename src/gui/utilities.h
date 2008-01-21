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
#include "main/module_dialogs.h"

gint gui_of2tf(const gchar * on_off);
gchar *gui_tf2of(gint true_false);
void gui_reassign_strdup(gchar **where, gchar *what);
void gui_set_progressbar_text(GtkWidget * pb, gchar * text);
void gui_set_progressbar_fraction(GtkWidget * pb, gdouble fraction);
void gui_set_combo_index(GtkWidget * combo, gint index);	
void gui_clear_combo(GtkWidget * combo);
void gui_add_item_to_combo(GtkWidget * combo, gchar * item);
void gui_glade_signal_connect_func (const gchar *cb_name, GObject *obj, 
			   const gchar *signal_name, const gchar *signal_data,
			   GObject *conn_obj, gboolean conn_after,
			   gpointer user_data);
gchar *gui_general_user_file (const char *fname, gboolean critical);
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
gchar *ncr_to_utf8(gchar * text);
void reading_selector(char *modname,
		      char *key,
		      DIALOG_DATA *dialog,
		      GtkMenuItem *menuitem,
		      gpointer user_data);
	
#ifdef __cplusplus
}
#endif
