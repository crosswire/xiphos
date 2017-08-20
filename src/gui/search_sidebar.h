/*
 * Xiphos Bible Study Tool
 * search_sidebar.h - sidebar search gui
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _SEARCH_SIDEBAR_H
#define _SEARCH_SIDEBAR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _sidebar_search SIDESEARCH;
struct _sidebar_search
{
	GtkWidget *rrbUseBounds;
	GtkWidget *entrySearch;
	GtkWidget *advanced_search;
	GtkWidget *entryLower;
	GtkWidget *entryUpper;
	GtkWidget *rbMultiword;
	GtkWidget *rbRegExp;
	GtkWidget *rbNoScope;
	GtkWidget *rbLastSearch;
	GtkWidget *rbPhraseSearch;
	GtkWidget *ckbCaseSensitive;
	GtkWidget *progressbar_search;
	GtkWidget *frame5;
	GtkWidget *frame_module;
	GtkWidget *radiobutton_search_text;
	GtkWidget *radiobutton_search_comm;
};
extern SIDESEARCH ss;

void gui_search_update_sidebar(char percent, void *userData);
void gui_create_search_sidebar(void);
void sidebar_optimized_toggled(GtkToggleButton *togglebutton, gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif
