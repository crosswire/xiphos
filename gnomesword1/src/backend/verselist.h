
/*
 * GnomeSword Bible Study Tool
 * verselist.h - parse verse list retrun list
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
 
#ifndef __VERSELIST_H__
#define __VERSELIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include "gs_gnomesword.h"

	GList *backend_get_verse_list(gchar * modName,
			      gchar * vlist, SETTINGS * s);
	void backend_setup_verselist(GtkWidget * html_widget,
				     SETTINGS * s);
	void backend_shutdown_verselist(void);
	void backend_verselist_change_verse(SETTINGS * s, gchar * url);

#ifdef __cplusplus
}
#endif
#endif				/* __VERSELIST_H__ */
