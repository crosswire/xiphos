/*
 * GnomeSword Bible Study Tool
 * gbs_dialog.h - dialog for displaying a gbs module
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

#ifndef __GBS_DIALOG_H_
#define __GBS_DIALOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gui/gbs.h"

#include "main/global_ops.hh"
	
typedef struct _gbsdata GBS_DATA;
struct _gbsdata {
	GtkWidget *dialog;
	GtkWidget *statusbar;	
	GtkWidget *vbox;
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *tree; 
	GtkWidget *showtabs;
	GtkWidget *module_options_menu;
	gchar *mod_name;
	gchar *search_string;
	gchar *key;
	gchar *cipher_key;
	gchar *cipher_old;
	gint mod_num;
	gint display_level;
	unsigned long offset;
	
	gboolean has_key;
	gboolean is_locked;
	gboolean is_leaf;
	gboolean is_dialog;
	gboolean is_rtol;
	
	GLOBAL_OPS *ops;
};
	
void gui_gbs_dialog_goto_bookmark(gchar * mod_name, gchar * key);
void gui_open_gbs_dialog(gchar * mod_name);
void gui_setup_gbs_dialog(GList *mods);
void gui_shutdown_gbs_dialog(void) ;
void gui_close_gbs_dialog(GBS_DATA * dlg);
void gui_update_gbs_dialog_display(void);
	
#ifdef __cplusplus
}
#endif

#endif
