/*
 * GnomeSword Bible Study Tool
 * commentary.h - gui for commentary modules
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

#ifndef ___COMMENTARY_H_
#define ___COMMENTARY_H_

typedef struct _commdata COMM_DATA;

#include "gui/commentary_find.h"

struct _commdata {
	GtkWidget *vbox;
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *frame_toolbar;
	GtkWidget *btnCOMMSync;
	GtkWidget *btnCOMMBack;
	GtkWidget *btnCOMMForward;
	GtkWidget *btnCOMMPrint;
	GtkWidget *btn_book_heading;
	GtkWidget *btn_chap_heading;
	GtkWidget *showtabs;
	GtkWidget *showtoolbar;
	gchar *mod_name;
	gchar *search_string;
	gchar key[80];
	gint modnum;
	gboolean has_key;
	gboolean book_heading;
	gboolean chapter_heading;
	COMMFindDialog *find_dialog;
};

void gui_set_commentary_page_and_key(gint page_num,
				     gchar * key);
void gui_display_commentary(gchar * key);
void gui_add_new_comm_pane(COMM_DATA *c);
void gui_setup_commentary(GList *mods);
void gui_shutdown_commentary(void);
#endif

