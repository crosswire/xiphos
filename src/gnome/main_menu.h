/*
 * GnomeSword Bible Study Tool
 * main_menu.h - creation of and call backs for gnomesword main menu
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


#ifndef __MAIN_MENU__H_
#define __MAIN_MENU__H_

#include "gs_gnomesword.h"

void on_daily_devotion1_activate(GtkMenuItem *menuitem, 
				gpointer user_data);	
void on_help_contents_activate(GtkMenuItem * menuitem,
			        gpointer user_data);
void on_mnuHistoryitem1_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_verse_style1_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_exit1_activate(GtkMenuItem * menuitem, 
				gpointer user_data);
void on_clear1_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_about_gnomesword1_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_preferences1_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_auto_save_notes1_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void on_search_activate(GtkMenuItem * menuitem,
				gpointer user_data);
void gui_create_main_menu(GtkWidget *app);
void gui_install_menu_hints(GtkWidget *app);
#endif	/* __MAIN_MENU__H_ */
