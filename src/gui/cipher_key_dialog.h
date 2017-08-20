/*
 * Xiphos Bible Study Tool
 * cipher_key_dialog.h  - asks for key to unlock sword module
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

#ifndef __CIPHER_KEY_DIALOG_H__
#define __CIPHER_KEY_DIALOG_H__

#ifdef __cplusplus
extern "C" {
#endif

gchar *gui_add_cipher_key(const char *mod_name,
			  gchar *cipher_old);

#ifdef __cplusplus
}
#endif
#endif
