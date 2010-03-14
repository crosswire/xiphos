/*
 * Xiphos Bible Study Tool
 * shortcutbar_viewer.h - create viewer group gui
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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

#ifndef _CREATE_SHORTCUTBAR_VIEWER_H
#define _CREATE_SHORTCUTBAR_VIEWER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _shortcutbar_viewer SB_VIEWER;
struct _shortcutbar_viewer {
	GtkWidget
		*clist,
		*notebook,
		*tbtn_view_main,
		*btn_save,
		*htmlshow,
		*html_widget,
		*html_viewer_widget;
	gchar   mod_name[80];
};
extern SB_VIEWER *sv;

void gui_display_devotional(void);
void gui_show_sb_verseList(void);
gboolean gui_display_dictlex_in_viewer(char *modName, char *key);
void gui_display_verse_list(gchar * key, gchar *module_name,
						gchar *verse_list);
GtkWidget * gui_create_shortcutbar_viewer(GtkWidget *vp);

#ifdef __cplusplus
}
#endif

#endif
