/*
 * GnomeSword Bible Study Tool
 * module_options.h - load and save individual module options
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

#ifndef __MODULE_OPTIONS_H__
#define __MODULE_OPTIONS_H__

#ifdef __cplusplus
extern "C" {
#endif

int backend_load_module_options(char * modName, char * option, 
					char * dir, char * conf);
int backend_save_module_options(char * modName, char * option, 
			char * value, char * dir, char * conf);

#ifdef __cplusplus
}
#endif

#endif

