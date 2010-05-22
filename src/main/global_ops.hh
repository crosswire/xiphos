/*
 * Xiphos Bible Study Tool
 * mod_global_ops.h - setup for SWORD global options in the gui
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
	gboolean variants_primary;
	gboolean variants_secondary;
	gboolean variants_all;
	gboolean transliteration;
	gboolean commentary_by_chapter;
	gint module_type;
	gint image_content;
	gint respect_font_faces;
    
	gboolean dialog;
};

GLOBAL_OPS *main_new_globals(gchar * mod_name, int dialog);
int main_save_module_options(const char * mod_name, const char * option, int choice, int dialog);

/* main window */
void main_set_global_options(GLOBAL_OPS * ops);
/* dialog */
void main_dialog_set_global_options(gpointer backend, GLOBAL_OPS * ops);

#ifdef __cplusplus
}
#endif

#endif
