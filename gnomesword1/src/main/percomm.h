/*
 * GnomeSword Bible Study Tool
 * percomm.h - support for personal commentary modules
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

#ifndef __PERCOMM_H_
#define __PERCOMM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "settings.h"
#include "_editor.h"

typedef struct _percomm_data PC_DATA;
struct _percomm_data {
	GtkWidget *html;
	//GtkWidget *frame;
	//GtkWidget *showtabs;
	gchar *mod_name;
	gchar *mod_description;
	gchar *search_string;
	gchar *key;
	gint mod_num;
	gboolean is_locked;
	GSHTMLEditorControlData *ec;
};

void editor_save_note(GtkWidget * html_widget);
void save_percomm_note(gchar *note);
void delete_percomm_note(void);
void percomm_page_changed(gint page_num, PC_DATA * p);
void tabs(gboolean choice);
void set_percomm_page_and_key(gint page_num, gchar * key);
void display_percomm(gchar * key);
void setup_percomm(SETTINGS * s, GList *mods);
void shutdown_percomm(void);

#ifdef __cplusplus
}
#endif
#endif	/* __PERCOMM_H_ */
