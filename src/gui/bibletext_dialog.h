/*
 * GnomeSword Bible Study Tool
 * bibletext_dialog.h - view Bible text module in a dialog
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
 
#ifndef __BIBLETEXT_DIALOG_H__
#define __BIBLETEXT_DIALOG_H__


#ifdef __cplusplus
extern "C" {
#endif
//#include "gui/bibletext.h"

#include "main/global_ops.hh"
#include "main/module_dialogs.h"


void gui_create_bibletext_dialog(TEXT_DATA * vt);
void gui_on_lookup_bibletext_dialog_selection
	(GtkMenuItem * menuitem, gchar * dict_mod_description);

#ifdef __cplusplus
}
#endif

#endif

/* end of file */
