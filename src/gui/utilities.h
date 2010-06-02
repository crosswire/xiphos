/*
 * Xiphos Bible Study Tool
 * utilities.h - support functions
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef GS_UTILITIES_H
#define GS_UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libxml/parser.h>
#include "main/configs.h"
#include "main/module_dialogs.h"
#include <gsf/gsf-outfile.h>

extern gint stop_window_sync;
void sync_windows(void);

gint gui_of2tf(const gchar * on_off);
gchar *gui_tf2of(gint true_false);
void gui_reassign_strdup(gchar **where, gchar *what);
void gui_set_statusbar (const gchar * message);
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
void gui_load_module_tree(GtkWidget * tree, gboolean limited);
MOD_FONT *get_font(gchar * mod_name);
void free_font(MOD_FONT *mf);
gchar * remove_linefeeds(gchar * buf);
void gui_add_mods_to_menus(GList * modlist, gchar * menu,
					GCallback callback);
void gui_add_mods_2_gtk_menu(gint mod_type, GtkWidget * menu,
				GCallback callback);
gchar *ncr_to_utf8(gchar * text);
void reading_selector(char *modname,
		      char *key,
		      DIALOG_DATA *dialog,
		      GtkMenuItem *menuitem,
		      gpointer user_data);

void language_init();
void language_make_list(GList *modlist,
			GtkTreeStore *store,
			GtkTreeIter text,
			GtkTreeIter commentary,
			GtkTreeIter map,
			GtkTreeIter image,
			GtkTreeIter devotional,
			GtkTreeIter dictionary,
			GtkTreeIter glossary,
			GtkTreeIter book,
			GtkTreeIter cult,
			GtkTreeIter *update,
			GtkTreeIter *uninstalled,
			void (*add)(GtkTreeModel *, GtkTreeIter, gchar **),
			gboolean limited);

GList *get_current_list(GtkTreeView *treeview);
gchar *get_modlist_string(GList * mods);

char *image_locator(const char *image);
GtkWidget *pixmap_finder(char *image);
GdkPixbuf *pixbuf_finder(const char *image, int size, GError **error);

void HtmlOutput(char *text, GtkWidget *gtkText, MOD_FONT *mf, char *anchor);
void set_window_icon(GtkWindow *window);
gboolean xiphos_open_default(const gchar *file);

void archive_addfile(GsfOutfile *output, const gchar *file, const gchar *name);
void archive_adddir(GsfOutfile *output, gchar *path, const gchar *name);
void xiphos_create_archive(gchar* conf_file, gchar* datapath, gchar *zip,
			   const gchar *destination);


#ifdef WIN32
gchar* xiphos_win32_get_subdir(const gchar *subdir);
#endif
void utilities_parse_treeview(xmlNodePtr parent,
                              GtkTreeIter * tree_parent,
                              GtkTreeModel *model);

enum {
	LANGSET_BIBLE,
	LANGSET_COMMENTARY,
	LANGSET_DICTIONARY,
	LANGSET_GENBOOK,
	LANGSET_MAP,
	LANGSET_IMAGE,
	LANGSET_CULT,
	LANGSET_DEVOTIONAL,
	LANGSET_GLOSSARY,
	LANGSET_UPDATE,
	LANGSET_UNINSTALLED,
	N_LANGSET_MODTYPES
};

#define	LANGSET_COUNT	200

void language_init(void);

#ifdef __cplusplus
}
#endif

#endif /* GS_UTILITIES_H */
