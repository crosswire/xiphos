/*
 * GnomeSword Bible Study Tool
 * shortcutbar.h - create and maintain the main shortcut bar
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

#ifndef __GS_SHORTCUTBAR_H_
#define __GS_SHORTCUTBAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "settings.h"

typedef struct _search_opt SEARCH_OPT;
struct _search_opt {
	GtkWidget
	    * ckbCommentary,
	    *ckbPerCom, 
	    *ckbGBS, 
	    *rbPhraseSearch;

	gchar
	    * module_name,
	    *upper_bond, 
	    *lower_bond, 
	    *search_string;

	gint
	    search_type, 
	    search_params, 
	    found_count;

	 gboolean 
	    use_bonds, 
	    use_lastsearch_for_bonds;
};

void showSBGroup(SETTINGS * s, gint groupnum);
void changegroupnameSB(SETTINGS * s,
		       gchar * groupName, gint groupNum);
void gui_setup_shortcut_bar(SETTINGS * s);
void gui_update_shortcut_bar(SETTINGS * s);
void gui_create_mod_list_menu(gint group_num, GtkWidget * menu,
			   GtkWidget * shortcut_menu_widget,
			   gint mod_type);
#ifdef __cplusplus
}
#endif
#endif				/* __GS_SHORTCUTBAR_H_ */
