/*
 * Xiphos Bible Study Tool
 * main_menu.h - creation of and call backs for xiphos main menu
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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

#ifndef __MAIN_MENU__H_
#define __MAIN_MENU__H_

#ifdef __cplusplus
extern "C" {
#endif

void gui_about_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_daily_devotion1_activate(GtkMenuItem *menuitem, 
				 gpointer user_data);	
void on_help_contents_activate(GtkMenuItem * menuitem,
			       gpointer user_data);
void on_menuHistoryitem1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);
/*void on_verse_style1_activate(GtkCheckMenuItem * menuitem,
				gpointer user_data);*/
void on_exit1_activate(GtkMenuItem * menuitem, 
		       gpointer user_data);
void on_clear1_activate(GtkMenuItem * menuitem,
			gpointer user_data);
void on_about_xiphos1_activate(GtkMenuItem * menuitem,
				   gpointer user_data);
void on_preferences1_activate(GtkMenuItem * menuitem,
			      gpointer user_data);
void on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
					  gpointer user_data);
void on_about_translation_activate(GtkMenuItem * menuitem,
				   gpointer user_data);
void on_auto_save_notes1_activate(GtkMenuItem * menuitem,
				  gpointer user_data);
void on_search_activate(GtkMenuItem * menuitem,
			gpointer user_data);
void on_save_tabs(GtkMenuItem * menuitem,
		  gpointer user_data);
void on_load_tabs(GtkMenuItem * menuitem,
		  gpointer user_data);
void gui_create_main_menu(GtkWidget *app);
void gui_install_menu_hints(GtkWidget *app);
void gui_parallel_tab_activate(GtkCheckMenuItem * menuitem, gpointer user_data);
	
#ifdef __cplusplus
}
#endif
#endif
