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

#include "gui/_display_info.h"
#include "main/display_info.h"
#include "backend/display_info_.h"

/******************************************************************************
 * Name
 *   loadmodandkey
 *
 * Synopsis
 *   #include "display_info.h"
 *
 *   void loadmodandkey(gchar * modName, gchar * newkey)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void loadmodandkey(gchar * modName, gchar * newkey)
{
	backend_dispaly_info_load_modudle(modName, newkey);
}

/******************************************************************************
 * Name
 *   display_info_setup
 *
 * Synopsis
 *   #include "display_info.h"
 *
 *   void display_info_setup(GtkWidget * text)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void display_info_setup(GtkWidget * text)
{
	backend_display_info_setup(text);
}

/******************************************************************************
 * Name
 *   dispaly_info_shutdown
 *
 * Synopsis
 *   #include "display_info.h"
 *
 *   void dispaly_info_shutdown(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void dispaly_info_shutdown(void)
{
	backend_dispaly_info_shutdown(); /* backend/display_info.cpp */
}
