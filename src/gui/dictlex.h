/*
 * GnomeSword Bible Study Tool
 * _dictlex.h - gui for dictionary/lexicon modules
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

#ifndef ___DICTLEX_H_
#define ___DICTLEX_H_


typedef struct _dldata DL_DATA;
struct _dldata {
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *clist;
	GtkWidget *listview;
	GtkWidget *entry;
	GtkWidget *showtabs;
	GtkWidget *module_options_menu;

#ifdef USE_GNOME2	
	GObject *mod_selection;
#endif	
	gchar *mod_name;
	gchar *search_string;
	gchar *key;
	gchar *cipher_key;
	gchar *cipher_old;
	
	int mod_num;
	gulong sig_id;
	
	gboolean has_key;
	gboolean is_locked;
	gboolean is_dialog;
	gboolean is_rtol;
};
extern DL_DATA *cur_d;

void gui_lookup_dictlex_selection(GtkMenuItem * menuitem,
					 gchar * dict_mod_description);
void gui_set_dict_frame_label(void);
void gui_set_dictlex_mod_and_key(gchar *mod, gchar *key);
void gui_display_dictlex(gchar * key);
void gui_set_dictionary_page_and_key(gint page_num, gchar * key);
void gui_add_new_dict_pane(DL_DATA *dl);
gint gui_setup_dictlex(GList *mods);
void gui_shutdown_dictlex(void);
void on_entryDictLookup_changed(GtkEditable * editable,
						       DL_DATA * d);
void on_btnSyncDL_clicked(GtkButton * button, DL_DATA * d);
void on_clistDictLex_select_row(GtkWidget * clist, gint row,
			   gint column, GdkEvent * event, DL_DATA * d);

#endif
