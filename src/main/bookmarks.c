/*
 * GnomeSword Bible Study Tool
 * bookmarks.cpp- functions to load and save bookmarks
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

#include "main/bookmarks.h"
#include "main/settings.h"
#include "backend/bookmarks.h"

/******************************************************************************
 * Name
 *   save_bookmarks
 *
 * Synopsis
 *   #include "main/bookmarks.h"
 *
 *   void save_bookmarks(GNode * bookmark_tree)
 *
 * Description
 *   save gnomesword bookmarks - using sword SWConfig
 *
 * Return value
 *   void
 */

void save_bookmarks(GNode * bookmark_tree)
{
       backend_save_bookmarks(bookmark_tree, settings.swbmDir); 
}


/******************************************************************************
 * Name
 *   create_bookmarks
 *
 * Synopsis
 *   #include "main/bookmarks.h"
 *
 *   void create_bookmarks(char *dir)
 *
 * Description
 *   no bookmark dir was found on startup - so create one
 *
 * Return value
 *   void
 */

void create_bookmarks(char *dir)
{
        backend_create_bookmarks(dir);
}



/******************************************************************************
 * Name
 *   load_bookmarks
 *
 * Synopsis
 *   #include "main/bookmarks.h"
 *
 *   GNode * load_bookmarks(void)
 *
 * Description
 *   load bookmarks - using sword SWConfig
 *
 * Return value
 *   GNode *
 */

GNode * load_bookmarks(void)
{
	return backend_load_bookmarks(settings.swbmDir);
}

