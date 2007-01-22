/*
 * GnomeSword Bible Study Tool
 * widgets.h - 
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _app_widgets WIDGETS;
struct  _app_widgets {
	GtkWidget
		*app,  /* main form */
		*appbar, /* gnome status/progress bar */
		*epaned, /* sidebar bar and main panle */
		*vpaned, /* bible/previewer paned */
		*vpaned2, /* commentary/dictionary paned */
		*hpaned, /* bible/commentary paned */
		*bible_info_paned, /* shortcut bar and main panle */
		*shortcutbar,  /* the shortcut bar widget */
		*bookmark_tree, /* bookmarks tree */
		*ctree_widget_books, /* books tree */
		*workbook_lower, /* notebook - dict/lex, gbs, and interlinear */
		*notebook_gbs, /* notebook - gbs */
		*notebook_dict,  /* notebook - dict/lex */
		*box_comm,  /* notebook - commentaries */
		*notebook_bible_parallel,  /* notebook -  */
		*notebook_text,  /* notebook - bible texts */
		*notebook_percomm,  /* notebook - personal comments */
		*notebook_sidebar,  /* notebook - sidebar */
		*notebook_comm_book,
		*notebook_main,	/* main notebook to have multiple passages open at once */
		*vbox_percomm,  /* vbox for personal comments notebook */
		*vbox_text,  /* vbox for  */
		*box_dict,
		*vbox_previewer,
	    
	/* dock/undock stuff */
		*dock_sb, /* dock for shortcut bar */
		*frame_parallel, /* frame for interlinear html  */ 
	
	/* editor_dialog */
		*studypad_dialog,
		
	/* entry widgets */
		*entry_dict,
	
	/* toolbars */
		*toolbar_comments,
		*toolbar_studypad,
	
	/* html widgets */
		*eventbox1,	
		*html_search_report,  /* html widget for serch report shortcut bar */
		*html_parallel, /* parallel html widget */
		*html_parallel_dialog, /* undocked parallel html widget */
		*html_text, /* texts html widget */
		*html_comm, /* html commentary widget */
		*html_percomm, /* html personal commentary widget */
		*html_dict, /* html dictionary widget */
		*html_book, /* gbs html widget */
		*html_studypad, /* studypad html widget */
		*bible_info_win, /* to display information (ie strongs) at  
                                     bottom of text pane */
				     
	/* book of the Bible combo */
		*cb_books,
		
	/* textview widget */
		*textview,
	/* listview widget */
		*listview_dict,
		*store,
		
	/* label widgets */
		*label_comm,
		*label_dict,
		
	/* comboboxentry widgets */
		*comboboxentry_dict,
		
	/* popup menu submenu widgets */	
		*add_bookmark_submenu,
		
	/* popup menu item widgets */
                *viewtexts_item, /* widget to access toggle menu - for showtexts */
		*viewpreview_item, 
                *viewcomms_item, /* widget to access toggle menu - for showcommss */
                *viewdicts_item, /* widget to access toggle menu - for showdicts */
		*versestyle_item,	/* widget to access toggle menu - for versestyle */
		*pinnedtabs_item,	/* widget to access toggle menu - for pinnedtabs */
		*showversenum_item,	/* widget to access toggle menu - for showversenum */
		*readaloud_item,	/* widget to access toggle menu - for reading out loud */
		*versehighlight_item,	/* widget to access toggle menu - for verse highlight */
		*doublespace_item,	/* widget to access toggle menu - for doublespace */
		*button_dict_book,      /* main toolbar togglebutton */
//		*button_parallel_view,  /* main toolbar togglebutton */
		*button_new_tab,	/* creates a new passage tab */
		*hboxtb;		/* container for browsing notebook and button */
};
extern WIDGETS widgets;	

#ifdef __cplusplus
}
#endif

#endif
