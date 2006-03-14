/*
 * GnomeSword Bible Study Tool
 * _dictlex.h - gui for dictionary/lexicon modules
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

#ifndef ___DICTLEX_H_
#define ___DICTLEX_H_

void gui_create_pm_dictionary(void);
void gui_lookup_dictlex_selection(GtkMenuItem * menuitem,
				gchar * dict_mod_description);
void gui_set_dictlex_mod_and_key(gchar *mod, gchar *key);
void gui_display_dictlex(gchar * key);
GtkWidget *gui_create_dictionary_pane(void);
#endif
