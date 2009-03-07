/*
 * Xiphos Bible Study Tool
 * main_window.h - main window gui 
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
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
/*
typedef struct _main_window MAIN_WINDOW;
struct  _main_window {
	GtkWidget *main_window;
	GtkWidget *status_bar;
	
	GtkWidget *nb_sidebar;
	
	TAB_PAGE current_page;
};
*/
#include "main/settings.h"

void gui_show_hide_texts(int choice);
void gui_show_hide_preview(int choice);
void gui_show_hide_comms(int choice);
void gui_show_hide_dicts(int choice);
void gui_set_bible_comm_layout(void);	
void gui_change_window_title(char * module_name);
void create_mainwindow(void);
void gui_show_main_window(void);

#ifdef __cplusplus
}
#endif

#ifndef WIN32

#define	GS_KEY_F1	67
#define	GS_KEY_F2	68
#define	GS_KEY_F3	69
#define	GS_KEY_F4	70

#define	GS_KEY_B	56
#define	GS_KEY_C	54
#define	GS_KEY_D	40
#define	GS_KEY_F	41
#define	GS_KEY_G	42
#define	GS_KEY_L	46
#define	GS_KEY_N	57
#define	GS_KEY_P	33
#define	GS_KEY_Q	24
#define	GS_KEY_Z	52

#define	GS_KEY_PLUS	21
#define	GS_KEY_MINUS	20

#else

#define	GS_KEY_F1	112
#define	GS_KEY_F2	113
#define	GS_KEY_F3	114
#define	GS_KEY_F4	115

#define	GS_KEY_B	66
#define	GS_KEY_C	67
#define	GS_KEY_D	68
#define	GS_KEY_F	70
#define	GS_KEY_G	71
#define	GS_KEY_L	76
#define	GS_KEY_N	78
#define	GS_KEY_P	80
#define	GS_KEY_Q	81
#define	GS_KEY_Z	90

#define	GS_KEY_PLUS	187
#define	GS_KEY_MINUS	189

#endif /* !WIN32 */

#endif
