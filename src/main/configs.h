/*
 * Xiphos Bible Study Tool
 * configs.h -
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __CONFIGS_H__
#define __CONFIGS_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mod_font MOD_FONT;
struct _mod_font
{
	const char *mod_name;
	char *old_font;
	const char *new_font;
	char *old_font_size;
	int old_font_size_value;
	const char *new_font_size;
	int no_font;
};

char *get_conf_file_item(const char *file, const char *mod_name,
			 const char *item);
void save_conf_file_item(const char *file, const char *mod_name,
			 const char *item, const char *value);
#ifdef __cplusplus
}
#endif
#endif
