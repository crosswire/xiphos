
/*
 * GnomeSword Bible Study Tool
 * create_shortcutbar_viewer.h - create viewer group gui
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
		*html_widget,
		*html_viewer_widget;
};


GtkWidget * gui_create_shortcutbar_viewer(GtkWidget * vp, SETTINGS * s);

#ifdef __cplusplus
}
#endif
#endif				/* _CREATE_SHORTCUTBAR_VIEWER_H */

