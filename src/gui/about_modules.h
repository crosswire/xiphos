/*
 * Xiphos Bible Study Tool
 * about_modules.h - About dialogs for modules
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

#ifndef __ABOUT_MODULES_H_
#define __ABOUT_MODULES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

void gui_display_about_module_dialog(gchar *modname);
void gui_core_display_about_dialog(const gchar *desc,
				   const gchar *abouttext,
				   const gchar *modname);
void about_module_display(GString *str,
			  const gchar *text,
			  const gboolean tooltip);

#ifdef __cplusplus
}
#endif
#endif
