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

typedef struct _DLFindDialog DLFindDialog;
struct _DLFindDialog {
	GnomeDialog *dialog;
	GtkWidget *htmlwidget;
	GtkWidget *entry;
	GtkWidget *backward;
	GtkWidget *case_sensitive;
	gboolean regular;
};

typedef struct _dldata DL_DATA;
struct _dldata {
	GtkWidget *vbox;
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *clist;
	GtkWidget *entry;
	GtkWidget *showtabs;
	gchar *mod_name;
	gchar *search_string;
	int mod_num;
	gboolean has_key;
	DLFindDialog *find_dialog;
};

void gui_display_dictlex(gchar * key);
void gui_set_dictionary_page_and_key(gint page_num, gchar * key);
void gui_add_new_dict_pane(DL_DATA *dl);
void gui_setup_dictlex(GList *mods);
void gui_shutdown_dictlex(void);
void on_entryDictLookup_changed(GtkEditable * editable,
						       DL_DATA * d);
void on_btnSyncDL_clicked(GtkButton * button, DL_DATA * d);
void on_clistDictLex_select_row(GtkCList * clist, gint row,
			   gint column, GdkEvent * event, DL_DATA * d);

#endif

