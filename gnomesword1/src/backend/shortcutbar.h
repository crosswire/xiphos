/*
 * GnomeSword Bible Study Tool
 * shortcutbar.h - functions for saving and loading shortcutbar groups
 *                 and getting verse list
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

#ifndef __SHORTCUTBAR__H__
#define __SHORTCUTBAR__H__

#ifdef __cplusplus
extern "C" {
#endif
	
void backend_save_sb_iconsize(char *filename, char *large_icons);
GList *backend_load_sb_group(char *filename, 
			char *group_name, char *use_largeicons);
void backend_save_sb_group(GList * items, char *filename, 
			char *group_name, char *large_icons);
GList *backend_get_verse_list(char *modName, char *vlist);
	
#ifdef __cplusplus
}
#endif

#endif
