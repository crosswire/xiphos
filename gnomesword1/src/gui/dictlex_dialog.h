/*
 * GnomeSword Bible Study Tool
 * dictlex_dialog.h - dialog for displaying a dictlex module
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

#ifndef __DICTLEX_DIALOG_H_
#define __DICTLEX_DIALOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gui/dictlex.h"
	
typedef struct _dl_dialog DL_DIALOG;
struct  _dl_dialog{
	GtkWidget *dialog;	
	GtkWidget *btn_close;
	DL_DATA *d;
};

void gui_open_dictlex_dialog(int mod_num);

void initSD(gchar * modName);
void gui_setup_dictlex_dialog(GList *mods);
void gui_shutdown_dictlex_dialog(void) ;

#ifdef __cplusplus
}
#endif

#endif

