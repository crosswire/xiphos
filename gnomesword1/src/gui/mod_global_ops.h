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

typedef struct _global_ops GLOBAL_OPS;
struct  _global_ops {
	gboolean
	    words_in_red,
	    strongs,
	    morphs,
	    footnotes,
	    greekaccents,
	    lemmas,
	    scripturerefs,
	    hebrewpoints, 
	    hebrewcant, 
	    headings, 
	    variants_all, 
	    variants_primary, 
	    variants_secondary;
	gint module_type;
};

GLOBAL_OPS *gui_new_globals(void);
void gui_add_global_option_items(gchar * mod_name, 
				GtkWidget * module_options_menu, 
				gint type_module,
				GLOBAL_OPS * ops);

#endif
