/*
 * GnomeSword Bible Study Tool
 * main_window.h - main window gui 
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

#ifndef __MAIN_WINDOW_H_
#define __MAIN_WINDOW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include "gui/toolbar_nav.h"
#include "main/navbar_versekey.h"
	
typedef struct _tab_page TAB_PAGE;
struct  _tab_page {
	GtkWidget *vbox;
	GtkWidget *paned_text_preview;
	GtkWidget *paned_text;
	GtkWidget *paned_comm;
	GtkWidget *paned_book;
	
	// ***** html widgets *****
	GtkWidget *html_text;
	GtkWidget *html_parallel;
	GtkWidget *html_comm;
	GtkWidget *html_dict;
	GtkWidget *html_book;
	GtkWidget *html_preview;
	
	// ***** backend  *****
	gpointer backend;
	// **** modules *****
	gchar *mod_text;
	gchar *mod_comm;
	gchar *mod_dict;
	gchar *mod_book;
	// **** parallel modules *****
	gchar *mod_par1;
	gchar *mod_par2;
	gchar *mod_par3;
	gchar *mod_par4;
	gchar *mod_par5;
	// ***** keys *****
	gchar *text_comm_key;
	gchar *dict_key;
	gchar *book_key;
#ifdef OLD_NAVBAR	
	NAV_BAR nav_bar;
#else	
	NAVBAR_VERSEKEY nav_bar;
#endif
};

typedef struct _main_window MAIN_WINDOW;
struct  _main_window {
	GtkWidget *main_window;
	GtkWidget *status_bar;
	
	GtkWidget *nb_sidebar;
	
	TAB_PAGE current_page;
};

#include "main/settings.h"

void gui_show_hide_texts(int choice);
void gui_show_hide_preview(int choice);
void gui_show_hide_comms(int choice);
void gui_show_hide_dicts(int choice);
void gui_set_bible_comm_layout(void);	
void gui_change_window_title(char * module_name);
void gui_search_appbar_update(char percent, void *userData);
void create_mainwindow(void);
void gui_show_main_window(void);

#ifdef __cplusplus
}
#endif

#endif
