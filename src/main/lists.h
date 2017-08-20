/*
 * Xiphos Bible Study Tool
 * lists.h glists of module names and descriptions, bible books etc.
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

#ifndef _LISTS_H
#define _LISTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

typedef struct _module_lists MOD_LISTS;
struct _module_lists
{
	GList *biblemods,
	    *commentarymods,
	    *dictionarymods,
	    *bookmods,
	    *percommods,
	    *devotionmods,
	    *prayermods,
	    *imagemods,
	    *mapmods,
	    *text_descriptions,
	    *dict_descriptions,
	    *comm_descriptions,
	    *book_descriptions, *prayer_descriptions, *options;
};

/* list types */
#define TEXT_LIST 0
#define TEXT_DESC_LIST 1
#define COMM_LIST 2
#define COMM_DESC_LIST 3
#define DICT_LIST 4
#define DICT_DESC_LIST 5
#define GBS_LIST 6
#define GBS_DESC_LIST 7
#define PRAYER_LIST 8
#define OPTIONS_LIST 9
#define PERCOMM_LIST 10
#define DEVOTION_LIST 11
#define MAP_LIST 12
#define IMAGE_LIST 13

GList *get_list(gint type);
void main_init_lists(void);
void main_shutdown_list(void);

#ifdef __cplusplus
}
#endif
#endif
