/*
 * GnomeSword Bible Study Tool
 * bibletext_dialog.h - view Bible text module in a dialog
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
 
#ifndef __BIBLETEXT_DIALOG_H__
#define __BIBLETEXT_DIALOG_H__

#include "gui/bibletext.h"

typedef struct _viewtext VIEW_TEXT;
struct _viewtext {
	GtkWidget *dialog;
	GtkWidget *btn_sync;
	GtkWidget *btn_close;
	GtkWidget *cbe_book;
	GtkWidget *spb_chapter;
	GtkWidget *spb_verse;
	GtkWidget *html;
	GtkWidget *btn_print;
	gchar *mod_name;
	gchar key[80];
	gint dialog_number;
	gboolean has_key;
	TEXT_DATA *t;
};	

void gui_open_bibletext_dialog(gint mod_num);
void gui_setup_bibletext_dialog(GList *mods);
void gui_shutdown_bibletext_dialog(void);

#endif

