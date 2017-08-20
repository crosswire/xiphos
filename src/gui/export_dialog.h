/*
 * Xiphos Bible Study Tool
 * export_dialog.h -
 *
 * Copyright (C) 2007-2017 Xiphos Developer Team
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

#ifndef _EXPORT_DIALOG_H_
#define _EXPORT_DIALOG_H_

#ifdef __cplusplus
extern "C" {
#endif

gchar *gui_get_export_filename(void);
void gui_export_dialog(void);
void _get_export_filename(void);
void on_dialog_export_passage_response(GtkDialog *dialog,
				       gint response_id,
				       gpointer user_data);
gint _check_for_distribution_license(gchar *mod_name);

#ifdef __cplusplus
}
#endif
#endif /* _EXPORT_DIALOG_H_ */
