/*
 * GnomeSword Bible Study Tool
 * viewdict.cpp - sword support for veiwtext dialog
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

#ifndef __VIEWDICT_H__
#define __VIEWDICT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main/gs_gnomesword.h"
#include "main/settings.h"

void backend_setup_viewdict(GtkWidget * text);
void backend_shutdown_viewdict(void);
void backend_load_module_viewdict(char *module_name);
void backend_goto_key_viewdict(char *new_key);
void backend_search_text_changed_viewdict(char *new_key);
char *backend_get_first_key_viewdict(void);
char *backend_get_next_key_viewdict(void);
char *backend_get_first_module_viewdict(void);
char *backend_get_next_module_viewdict(void);

#ifdef __cplusplus
}
#endif

#endif

