/*
 * GnomeSword Bible Study Tool
 * mod_global_ops.c - setup for SWORD global options in the gui
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>


#include "gui/mod_global_ops.h"

GLOBAL_OPS *gui_new_globals(void)
{
	GLOBAL_OPS *ops;
	ops = g_new0(GLOBAL_OPS, 1);
	ops->words_in_red = FALSE;
	ops->strongs = FALSE;
	ops->morphs = FALSE;
	ops->footnotes = FALSE;
	ops->greekaccents = FALSE;
	ops->lemmas = FALSE;
	ops->scripturerefs = FALSE;
	ops->hebrewpoints = FALSE;
	ops->hebrewcant = FALSE;
	ops->headings = FALSE;
	ops->variants_all = FALSE;
	ops->variants_primary = FALSE;
	ops->variants_secondary = FALSE;
	return ops;	
}
