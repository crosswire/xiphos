
/*
 * GnomeSword Bible Study Tool
 * properties.cpp - save gnomesword properties and settings
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

#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "settings.h"

	gboolean backend_load_properties(void);
	gboolean backend_save_properties(gboolean shutdown);
	gboolean backend_create_properties_from_setup(void);
	gboolean backend_create_properties(void);

#ifdef __cplusplus
}
#endif
#endif				/* __PROPERTIES_H__ */
