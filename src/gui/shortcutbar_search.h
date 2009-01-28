/*
 * Xiphos Bible Study Tool
 * shortcutbar_search.h - create search group gui
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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

#ifndef _SHORTCUTBAR_SEARCH_H
#define _SHORTCUTBAR_SEARCH_H

#ifdef __cplusplus
extern "C" {
#endif	
	
void gui_search_update(char percent, void *userData);	
void gui_set_search_label(void);	
void gui_create_shortcutbar_search(GtkWidget *vp);

#ifdef __cplusplus
}
#endif

#endif
