/*
 * Xiphos Bible Study Tool
 * main_menu.h - creation of and call backs for xiphos main menu
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __MAIN_MENU__H_
#define __MAIN_MENU__H_

#ifdef __cplusplus
extern "C" {
#endif

GtkWidget *gui_create_main_menu(void);
void gui_parallel_tab_activate(GtkCheckMenuItem *menuitem,
			       gpointer user_data);
void on_module_manager_activate(GtkMenuItem *menuitem,
				gpointer user_data);
void on_preferences_activate(GtkMenuItem *menuitem,
			     gpointer user_data);
void on_help_contents_activate(GtkMenuItem *menuitem,
			       gpointer user_data);
void on_mailing_list_activate(GtkMenuItem *menuitem,
			      gpointer user_data);
void on_view_releases_activate(GtkMenuItem *menuitem,
			       gpointer user_data);
void on_live_chat_activate(GtkMenuItem *menuitem,
			   gpointer user_data);
void on_report_bug_activate(GtkMenuItem *menuitem,
			    gpointer user_data);
void on_about_the_sword_project_activate(GtkMenuItem *menuitem,
					 gpointer user_data);
void on_about_translation_activate(GtkMenuItem *menuitem,
				   gpointer user_data);
void on_daily_devotion_activate(GtkMenuItem *menuitem,
				gpointer user_data);
void on_search_activate(GtkMenuItem *menuitem,
			gpointer user_data);
void on_linked_tabs_activate(GtkCheckMenuItem *menuitem,
			     gpointer user_data);
void on_read_aloud_activate(GtkCheckMenuItem *menuitem,
			    gpointer user_data);
void on_show_verse_numbers_activate(GtkCheckMenuItem *menuitem,
				    gpointer user_data);
void on_versehighlight_activate(GtkCheckMenuItem *menuitem,
				gpointer user_data);
void on_side_preview_activate(GtkCheckMenuItem *menuitem,
			      gpointer user_data);
void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_about_xiphos_activate(GtkMenuItem *menuitem,
			      gpointer user_data);
void on_save_session_activate(GtkMenuItem *menuitem,
			      gpointer user_data);
void redisplay_to_realign(void);
void on_open_session_activate(GtkMenuItem *menuitem,
			      gpointer user_data);
void on_show_bible_text_activate(GtkCheckMenuItem *menuitem,
				 gpointer user_data);
void on_preview_activate(GtkCheckMenuItem *menuitem,
			 gpointer user_data);
void on_show_commentary_activate(GtkCheckMenuItem *menuitem,
				 gpointer user_data);
void on_show_dictionary_lexicon_activate(GtkCheckMenuItem *
					     menuitem,
					 gpointer user_data);
void on_open_studypad_activate(GtkMenuItem *menuitem,
			       gpointer user_data);
void on_advanced_search_activate(GtkMenuItem *menuitem,
				 gpointer user_data);
void on_attach_detach_sidebar_activate(GtkMenuItem *menuitem,
				       gpointer user_data);
void on_sidebar_showhide_activate(GtkMenuItem *menuitem,
				  gpointer user_data);
void link_uri_hook(GtkLinkButton *button, const gchar *link,
		   gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif
