/*
 * GnomeSword Bible Study Tool
 * url.h - support functions
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

#ifndef __URL_H_
#define __URL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

enum {
	TYPE_URI,
	BLANK,
	MODULE,
	KEY,
	TYPE_NOTE,
	NOTE_NUM
};

gint main_url_handler(const gchar * url, gboolean clicked);
gint main_main_get_mod_type_from_url(const gchar * url);
	

#ifdef __cplusplus
}
#endif

#endif
