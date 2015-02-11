/*
 * Xiphos Bible Study Tool
 * cipher_key_dialog.c  - asks for key to unlock sword module
 *
 * Copyright (C) 2000-2015 Xiphos Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include "gui/cipher_key_dialog.h"
#include "gui/dialog.h"

#include "main/mod_mgr.h"
#include "main/settings.h"
#include "main/sword.h"

/******************************************************************************
 * Name
 *   gui_add_cipher_key
 *
 * Synopsis
 *   #include "unlock_key_dialog.h"
 *
 *   gchar *gui_add_cipher_key(gchar *mod_name, gchar *cipher_old)
 *
 * Description
 *   create dialog to get key to unlock a module
 *
 * Return value
 *   gboolean
 */

gchar *gui_add_cipher_key(const char *mod_name, gchar *cipher_old)
{
	gchar *retval = NULL;
	GS_DIALOG *info;

	info = gui_new_dialog();
#ifdef HAVE_GTK_310
	info->stock_icon = "dialog-warning";
#else
	info->stock_icon = GTK_STOCK_DIALOG_WARNING;
#endif
	info->label_top =
	    g_strdup_printf(_("Cipher key for module %s"), mod_name);
	info->label_middle = _("for:");
	info->label_bottom = (char *)mod_name;
	info->text1 = g_strdup(cipher_old);
	info->label1 = _("Enter Key: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	/*** open dialog ***/
	if (gui_gs_dialog(info) == GS_OK) {
		main_set_module_unlocked(mod_name, info->text1);
		main_save_module_key(mod_name, info->text1);
		main_update_module_lists();
		retval = g_strdup(info->text1);
	}
	g_free(info->label_top);
	g_free(info->text1);
	g_free(info);
	return retval;
}
