/*
 * GnomeSword Bible Study Tool
 * parallel.h - support for displaying multiple modules
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

#ifndef ___parallel_H_
#define ___parallel_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main/settings.h"

typedef enum
{
	PARALLEL1,
	PARALLEL2,
	PARALLEL3,
	PARALLEL4,
	PARALLEL5
	
}GSParallel;


void gui_change_parallel_module(GSParallel parallel, gchar * mod_name);	
void gui_check_parallel_modules(void);	
void gui_set_parallel_options_at_start(void);
void gui_update_parallel_page(void);
void gui_update_parallel_page_detached(void);
void gui_swap_parallel_with_main(char *intmod);
void gui_set_parallel_module_global_options(gchar * option, gboolean choice);
void gui_create_parallel_popup(GList *bible_description);
	
#ifdef __cplusplus
}
#endif

#endif
