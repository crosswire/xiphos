/*
 * GnomeSword Bible Study Tool
 * bibletext_dialog.h - view Bible text module in a dialog
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
	GtkWidget *cbe_book;
	GtkWidget *spb_chapter;
	GtkWidget *spb_verse;
	GtkWidget *freeform_lookup;
	GtkWidget *statusbar;
	gchar key[80];
	gboolean has_key;
	TEXT_DATA *t;
};

void gui_on_lookup_bibletext_dialog_selection
	(GtkMenuItem * menuitem, gchar * dict_mod_description);
void gui_close_text_dialog(void);
void gui_open_bibletext_dialog(gchar * mod_name);
void gui_setup_bibletext_dialog(GList *mods);
void gui_shutdown_bibletext_dialog(void);

#endif

/* end of file */
