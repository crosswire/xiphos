/*
 * GnomeSword Bible Study Tool
 * toolbar_nav.h - Bible text navigation toolbar
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

#ifndef __TOOLBAR_NAV_H_
#define __TOOLBAR_NAV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

typedef struct _toolbar_nav NAV_BAR;
struct  _toolbar_nav {
	GtkWidget *lookup_entry;
	GtkWidget *button_back;
	GtkWidget *button_forward;
};
extern NAV_BAR nav_bar;


GtkWidget *gui_create_nav_toolbar(GtkWidget * app);
	
#ifdef __cplusplus
}
#endif

#endif
