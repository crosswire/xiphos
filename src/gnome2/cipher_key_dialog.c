/*
 * GnomeSword Bible Study Tool
 * cipher_key_dialog.c  - asks for key to unlock sword module
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
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

#include "gui/cipher_key_dialog.h"
#include "main/sword.h"
#include "main/bibletext.h"
#include "gui/dialog.h"



/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "unlock_key_dialog.h"
 *
 *   	
 *
 * Description
 *   create dialog to get key to unlock a module
 *
 * Return value
 *   gboolean
 */

gchar * gui_add_cipher_key(gchar *mod_name, gchar *cipher_old)
{
	gchar *retval = NULL;
	gint test;
	GS_DIALOG *info;
	
	info = gui_new_dialog();
	info->title = N_("Cipher Key?");
	info->label_top = mod_name;
	info->text1 = g_strdup(cipher_old);
	info->label1 = N_("Enter Cipher Key: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	/*** open dialog to get name for root node ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		set_module_unlocked(mod_name, info->text1);
		save_module_key(mod_name, info->text1);
		retval = g_strdup(info->text1);
	}
	g_free(info->text1);
	g_free(info);
	return retval;
}
