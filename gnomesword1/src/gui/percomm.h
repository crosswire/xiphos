/*
 * GnomeSword Bible Study Tool
 * _percom.c - personal comments gui
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

#ifndef ___PERCOMM_H_
#define ___PERCOMM_H_

#include <gnome.h>
#include "main/percomm.h"
#include "gui/editor.h"

typedef struct _percomm_data PC_DATA;
struct _percomm_data {
	GtkWidget *vbox;
	GtkWidget *html;
	gchar *mod_name;
	gchar *search_string;
	gint mod_num;
	gboolean is_locked;
	GSHTMLEditorControlData *ec;
};

void gui_save_note(GSHTMLEditorControlData * e);
void gui_percomm_tabs(gboolean choice);
void gui_display_percomm(gchar * key);
void gui_set_percomm_page_and_key(gint page_num, gchar * key);
void gui_add_new_percomm_pane(PC_DATA *p);
void gui_setup_percomm(GList *mods);
void gui_shutdown_percomm(void);
#endif

