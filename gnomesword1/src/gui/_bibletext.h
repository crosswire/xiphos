/*
 * GnomeSword Bible Study Tool
 * _bibletext.h - gui for Bible text modules
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

#ifndef ___BIBLETEXT_H_
#define ___BIBLETEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gs_gnomesword.h"
/*
 * main
 */
#include "bibletext.h"
	
void gui_set_text_frame_label(void);
void on_notebook_text_switch_page(GtkNotebook * notebook,
				GtkNotebookPage * page,
				gint page_num, GList * tl); 
void gui_create_text_pane(SETTINGS * s, TEXT_DATA * t);
GtkWidget *gui_create_pm_text(TEXT_DATA * t);
	
#ifdef __cplusplus
}
#endif
#endif				/* ___BIBLETEXT_H_ */
