/***************************************************************************
                          gs_popup_cb.h  -  description
                             -------------------
    begin                : Thu Feb 8 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
	
void on_boldNE_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_italicNE_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_referenceNE_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_underlineNE_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_greekNE_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_goto_reference_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_goto_reference2_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_lookup_selection2_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_lookup_selection_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_lookup_word1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_about_this_module5_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_john_3_1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_mainText_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_1st_interlinear_window1_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_2nd_interlinear_window1_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_3rd_interlinear_window1_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_copy4_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_kjv1_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_about_this_module_activate(GtkMenuItem * menuitem,
					gpointer user_data);
void on_auto_scroll1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);					
void on_lookup_selection4_activate(GtkMenuItem * menuitem,
				       gpointer user_data);
void on_goto_reference3_activate(GtkMenuItem * menuitem,
				     gpointer user_data);
void on_show_tabs_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_view_in_new_window_activate(GtkMenuItem * menuitem,
					gpointer user_data);  
void on_view_in_new_window2_activate(GtkMenuItem * menuitem,
		gpointer user_data);  
void on_change_module_activate(GtkMenuItem * menuitem, 
		gpointer user_data);
void on_changeint1mod_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_changeint2mod_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_changeint3mod_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_changeint4mod_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_changeint5mod_activate(GtkMenuItem * menuitem,
		gpointer user_data);
void on_viewtext_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_show_morphs_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_show_strongs_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_show_footnotes_activate(GtkMenuItem * menuitem, gpointer user_data);


#ifdef __cplusplus
}
#endif

