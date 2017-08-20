/*
 * Xiphos Bible Study Tool
 * search_sidebar.h - glue
 *
 * Copyright (C) 2004-2017 Xiphos Developer Team
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

#ifndef __SEARCH_SIDEBAR_H__
#define __SEARCH_SIDEBAR_H__

#ifdef __cplusplus
extern "C" {
#endif

void main_sidebar_search_percent_update(char percent,
					void *userData);
void main_do_sidebar_search(gpointer user_data);
void main_sidebar_perscomm_dump(void);
void main_init_sidebar_search_backend(void);
void main_delete_sidebar_search_backend(void);
void main_search_sidebar_fill_bounds_combos(void);

#ifdef __cplusplus
}
#endif
#endif
