/*
 * GnomeSword Bible Study Tool
 * shortcutbar.c - glue  (: very sticky :)
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <glib-2.0/glib.h>

#include "gui/main_window.h"
#include "gui/search_dialog.h"
#include "gui/search_sidebar.h"

#include "main/search.h"
 
#include "backend/key.hh"
#include "backend/config.hh"
#include "backend/sword.h"
/******************************************************************************
 * Name
 *   set_search_global_option
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   void set_search_global_option(gchar * option, gchar * choice)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void set_search_global_option(gchar * option, gboolean choice)
{	
	char *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}
	backend_set_global_option(5, option, on_off);
}
