/*
 * GnomeSword Bible Study Tool
 * viewcomm.cpp - sword support for veiwcomm dialog
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

#ifndef __VIEWCOMM_H__
#define __VIEWCOMM_H__

#ifdef __cplusplus
extern "C" {
#endif

void backend_setup_viewcomm(GtkWidget * text);
void backend_shutdown_viewcomm(void);
void backend_load_module_viewcomm(gchar * modName);
void backend_goto_verse_viewcomm(gchar * newkey);
void backend_nav_module_viewcomm(gint direction);
char *backend_get_first_module_viewcomm(void);
char *backend_get_next_module_viewcomm(void);

#ifdef __cplusplus
}
#endif

#endif

