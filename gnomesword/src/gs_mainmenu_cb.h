/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
  /*
    * GnomeSword Bible Study Tool
    * gs_mainmenu_cb.h
    * -------------------
    * Tue Aug 21 2001
    * copyright (C) 2000 by Terry Biggs
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

#ifndef __GS_MAINMENU_CB_H__
#define __GS_MAINMENU_CB_H__

#ifdef __cplusplus
extern "C" {
#endif

	void on_help_contents_activate(GtkMenuItem * menuitem,
				       gpointer user_data);
	void on_mnuHistoryitem1_activate(GtkMenuItem * menuitem,
					gpointer user_data);
	void on_add_quickmark_activate(GtkMenuItem * menuitem,
				       	gpointer user_data);
	void on_edit_quickmarks_activate(GtkMenuItem * menuitem,
					gpointer user_data);
	void on_verse_style1_activate(GtkMenuItem * menuitem,
				      	gpointer user_data);
	void on_show_interlinear_page1_activate(GtkMenuItem * menuitem,
					gpointer user_data);
	void on_exit1_activate(GtkMenuItem * menuitem, 
					gpointer user_data);
	void on_clear1_activate(GtkMenuItem * menuitem,
					gpointer user_data);
	void on_about_gnomesword1_activate(GtkMenuItem * menuitem,
					gpointer user_data);
	void on_strongs_numbers1_activate(GtkMenuItem * menuitem,
					gpointer user_data);
	void on_footnotes1_activate(GtkMenuItem * menuitem,
				    	gpointer user_data);
	void on_preferences1_activate(GtkMenuItem * menuitem,
				      	gpointer user_data);
	void on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
					gpointer user_data);
	void on_auto_save_notes1_activate(GtkMenuItem * menuitem,
					gpointer user_data);
	void on_search1_activate(GtkMenuItem * menuitem,
				 	gpointer user_data);
	void on_morphs_activate(GtkMenuItem * menuitem,
					gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif				/* __GS_MAINMENU_CB_H__ */
