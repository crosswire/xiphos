/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_popup_cb.h
    * -------------------
    * Thu Feb 8 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
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
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifndef __GS_POPUP_CB_H_
#define __GS_POPUP_CB_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include "gs_gnomesword.h"

	void on_undockInt_activate(GtkMenuItem * menuitem,  SETTINGS *s);
	/*** toogle global options in interlinear window ***/
	void on_edit_book_activate(GtkMenuItem * menuitem,  gchar *user_data);
	void on_save_book_activate(GtkMenuItem * menuitem, gchar *user_data);
	void on_print_item_activate(GtkMenuItem * menuitem, gchar * html);
	void on_int_global_options_activate(GtkMenuItem * menuitem, 
				gpointer user_data);
	void on_about_this_module5_activate(GtkMenuItem * menuitem,
					    gpointer user_data);
	void on_john_3_1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);
	void on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data);
	void on_copy4_activate(GtkMenuItem * menuitem, gpointer user_data);
	void on_paste1_activate(GtkMenuItem * menuitem,
				gpointer user_data);
	void on_kjv1_activate(GtkMenuItem * menuitem, gpointer user_data);
	void on_about_this_module_activate(GtkMenuItem * menuitem,
					   gpointer user_data);
	void on_auto_scroll1_activate(GtkMenuItem * menuitem,
				      gpointer user_data);
	void on_lookup_selection4_activate(GtkMenuItem * menuitem,
					   gpointer user_data);
	void on_show_tabs_activate(GtkMenuItem * menuitem,
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
	void on_viewtext_activate(GtkMenuItem * menuitem,
				  gpointer user_data);
	void on_show_morphs_activate(GtkMenuItem * menuitem,
				     gpointer user_data);
	void on_show_strongs_activate(GtkMenuItem * menuitem,
				      gpointer user_data);
	void on_show_footnotes_activate(GtkMenuItem * menuitem,
					gpointer user_data);
	void on_dict_select_activate(GtkMenuItem * menuitem,
				     gpointer user_data);
	void on_com_select_activate(GtkMenuItem * menuitem,
				    gpointer user_data);

#ifdef __cplusplus
}
#endif

#endif /* __GS_POPUP_CB_H_ */
