/*
 * GnomeSword Bible Study Tool
 * gnomesword.h - 
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

#ifndef __GNOMESWORD_H__
#define __GNOMESWORD_H__

#ifdef __cplusplus
extern "C" {
#endif

	
#define MAIN_TEXT_WINDOW 0 
#define INTERLINEAR_WINDOW 1
#define COMMENTARY_WINDOW 2
#define DICTIONARY_WINDOW 3
#define BOOK_WINDOW 4
#define PERCOMM_WINDOW 5
#define STUDYPAD_WINDOW 6
    
 
typedef struct _app_widgets WIDGETS;
struct  _app_widgets {
	GtkWidget
		*app,  /* main form */
		*appbar, /* gnome status/progress bar */
		*epaned, /* shortcut bar and main panle */
		*shortcutbar,  /* the shortcut bar widget */
		*ctree_widget, /* bookmarks tree */
		*ctree_widget_books, /* books tree */
		*workbook_upper, /* notebook - commentaries and editors */	
		*workbook_lower, /* notebook - dict/lex, gbs, and interlinear page */
		*notebook_gbs, /* notebook - gbs */
		*notebook_dict,  /* notebook - dict/lex */
		*notebook_comm,  /* notebook - commentaries */
		*notebook_text,  /* notebook - texts */
		*notebook_percomm,  /* notebook - personal comments */
	    
	/* dock/undock stuff */
		*dock_sb, /* dock for shortcut bar */
		*frame_interlinear, /* frame for interlinear html  */ 
	
	/* toolbars */
		*hbox_toolbar,
		*toolbar_comments,
		*toolbar_studypad,
	
	/* html widgets */	
		*html_search_report,  /* html widget for serch report shortcut bar */
		*html_interlinear, /* interlinear html widget */
		*html_text, /* texts html widget */
		*html_comm, /* html commentary widget */
		*html_percomm, /* html personal commentary widget */
		*html_dict, /* html dictionary widget */
		*html_book, /* gbs html widget */
		*html_studypad, /* studypad html widget */
		
	/* popup menu item widgets */
		*versestyle_item;	/* widget to access toggle menu - for versestyle */
};
extern WIDGETS widgets;		
/*** function prototypes ***/

void init_gnomesword(void);
void shutdown_gnomesword(void);

#ifdef __cplusplus
}
#endif

#endif

