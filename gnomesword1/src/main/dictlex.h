/*
 * GnomeSword Bible Study Tool
 * dictlex.h - dictlex glue
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

#ifndef __DICTLEX_H_
#define __DICTLEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main/settings.h"

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
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *clist;
	GtkWidget *entry;
	GtkWidget *showtabs;
	gchar *modName;
	gchar *searchstring;
	int mod_num;
	gboolean has_key;
	DLFindDialog *find_dialog;
};
void display_dictionary_page_and_key(gint page_num, gchar * key);
GList *fill_dictlex_keys(int mod_num, int count);
void setup_dictlex(SETTINGS * s, GList *mods);
void shutdown_dictlex(void);
void set_dictionary_page_and_key(gint page_num,
				     gchar * key);
void display_dictlex(gchar * key);

gchar *get_first_key_viewdict(void);
gchar *get_next_key_viewdict(void);
void search_text_changed_viewdict(gchar * key);
void shutdown_viewdict(void);
void load_module_viewdict(gchar * module_name);
void goto_key_viewdict(gchar * key);
void setup_viewdict(GtkWidget * text, SETTINGS *s);

#ifdef __cplusplus
}
#endif
#endif	/* __DICTLEX_H_ */
