/*
 * GnomeSword Bible Study Tool
 * mod_global_ops.h - setup for SWORD global options in the gui
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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
 
 
#ifndef ___MOD_GLOBAL_OPS_H_
#define ___MOD_GLOBAL_OPS_H_

#ifdef __cplusplus
extern "C" {
#endif
	
 
typedef struct _global_ops GLOBAL_OPS;
#include "main/module_dialogs.h"
struct  _global_ops {
	gboolean words_in_red;
	gboolean strongs;
	gboolean morphs;
	gboolean footnotes;
	gboolean greekaccents;
	gboolean lemmas;
	gboolean scripturerefs;
	gboolean hebrewpoints; 
	gboolean hebrewcant; 
	gboolean headings; 
	gboolean variants_all; 
	gboolean variants_primary; 
	gboolean variants_secondary;
	gint module_type;
};

int main_save_module_options(char * mod_name, char * option, int choice);
void main_set_global_options(GLOBAL_OPS * ops);
void main_set_strongs_morphs_off(GLOBAL_OPS * ops);
GLOBAL_OPS *main_new_globals(gchar * mod_name);
void main_dialog_set_global_options(DIALOG_DATA * t);

#ifdef __cplusplus
}
#endif

#endif
