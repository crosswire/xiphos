/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_shortcutbar.h
     * -------------------
     * Thu Jun 14 2001
     * copyright (C) 2001 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifndef __GS_SHORTCUTBAR_H_
#define __GS_SHORTCUTBAR_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "gs_gnomesword.h"

        typedef struct _search_opt SEARCH_OPT;
        struct _search_opt {
                GtkWidget
                        *ckbCommentary,
                        *ckbPerCom, 
                        *ckbGBS, 
                        *rbPhraseSearch;
		gchar 
			*module_name,
			*upper_bond,
			*lower_bond,
			*search_string;
		
		int 
			search_type,
			search_params,
			found_count;
		
		gboolean 
			use_bonds,
			use_lastsearch_for_bonds;
        };
	void fill_search_results_clist(GList *glist, SEARCH_OPT *so, SETTINGS *s);
        void setupforDailyDevotion ( SETTINGS * s );
        void showSBVerseList ( SETTINGS * s );
        void showSBGroup ( SETTINGS * s, gint groupnum );
        void changegroupnameSB ( SETTINGS * s,
                                 gchar * groupName, gint groupNum );
        void on_btnSBDock_clicked ( GtkButton * button, SETTINGS * s );
        void on_btnSB_clicked ( GtkButton * button, SETTINGS * s );
        void setupSB ( SETTINGS * s );
        void update_shortcut_bar ( SETTINGS * s );
        void create_modlistmenu_sb ( gint group_num, GtkWidget * menu,
                                     GtkWidget * shortcut_menu_widget,
                                     gchar * modtype );
        void gs_shortcut_model_get_item_info ( GtkWidget *
                                               shortcutbar_widget,
                                               gint group_num,
                                               gint item_num,
                                               gchar ** item_url,
                                               gchar ** item_name );
        gint gs_shortcut_model_get_num_items ( GtkWidget *
                                               shortcutbar_widget,
                                               gint group_num );
#ifdef __cplusplus
}
#endif
#endif                          /* __GS_SHORTCUTBAR_H_ */
