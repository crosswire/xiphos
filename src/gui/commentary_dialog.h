/*
 * GnomeSword Bible Study Tool
 * commentary_dialog.h - dialog for a commentary module
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

#ifndef __COMMENTARY_DIALOG_H_
#define __COMMENTARY_DIALOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gui/editor.h"	
#include "gui/commentary.h"
	
typedef struct _viewcomm VIEW_COMM;
struct _viewcomm {
	GtkWidget *dialog;
	GtkWidget *btn_sync;
	GtkWidget *btn_goto;
	GtkWidget *btn_prev;
	GtkWidget *btn_next;
	GtkWidget *btn_close;
	GtkWidget *entry_key;
	GtkWidget *btn_print;
	
	GtkWidget *cbe_book;
	GtkWidget *spb_chapter;
	GtkWidget *spb_verse;
	GtkWidget *freeform_lookup;
	GtkWidget *statusbar;
	gboolean has_key;
	GSHTMLEditorControlData *ec;
	COMM_DATA *c;
};

void gui_on_lookup_commentary_dialog_selection
	(GtkMenuItem * menuitem, gchar * dict_mod_description);
void gui_close_comm_dialog(void);
void gui_open_commentary_dialog(gchar * mod_name);
void gui_open_commentary_editor(gchar * mod_name);
void gui_setup_commentary_dialog(GList *mods);
void gui_shutdown_commentary_dialog(void);
void gui_display_commentary_in_dialog(gchar * key);

#ifdef __cplusplus
}
#endif

#endif

