/*
 * GnomeSword Bible Study Tool
 * display_info.c - 
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
#  include <config.h>
#endif

#include <gnome.h>

#include "display_info.h"

#include "_display_info.h"

#include "display_info_.h"

//gboolean gsI_isrunning = FALSE;

void loadmodandkey(gchar * modName, gchar * newkey)
{
        /* backend/display_info.cpp */
	backend_dispaly_info_load_modudle(modName, newkey);
}

void display_info_setup(GtkWidget * text)
{
	backend_display_info_setup(text);
}

void dispaly_info_shutdown(void)
{
	backend_dispaly_info_shutdown(); /* backend/display_info.cpp */
}
