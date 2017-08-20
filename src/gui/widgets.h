/*
 * Xiphos Bible Study Tool
 * widgets.h -
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

#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _app_widgets WIDGETS;
struct _app_widgets
{
	GtkWidget *app,				 /* main form */
	    *appbar,				 /* gtk status/progress bar */
	    *vboxMain,				 /*  */
	    *page,				 /*  */
	    *epaned,				 /* sidebar bar and main panel */
	    *paned_sidebar,			 /*pane to show previewer in sidebar bar */
	    *box_side_preview,			 /* box for packing previewer in side bar */
	    *vpaned,				 /* bible/previewer paned */
	    *vpaned2,				 /* commentary/dictionary paned */
	    *hpaned,				 /* bible/commentary paned */
	    *bible_info_paned,			 /* shortcut bar and main panle */
	    *shortcutbar,			 /* the shortcut bar widget */
	    *bookmark_tree,			 /* bookmarks tree */
	    *ctree_widget_books,		 /* books tree */
	    *workbook_lower,			 /* notebook - dict/lex, gbs, and interlinear */
	    *notebook_gbs,			 /* notebook - gbs */
	    *notebook_dict,			 /* notebook - dict/lex */
	    *box_comm,				 /* notebook - commentaries */
	    *notebook_bible_parallel,		 /* notebook -  */
	    *notebook_text,			 /* notebook - bible texts */
	    *notebook_percomm,			 /* notebook - personal comments */
	    *notebook_sidebar,			 /* notebook - sidebar */
	    *notebook_comm_book, *notebook_main, /* main notebook to have multiple passages open at once */
	    *vbox_text,				 /* vbox for bible + simple parallel */
	    *box_dict, *vbox_previewer, *parallel_tab,
	    /* dock/undock stuff */
	    *dock_sb,	/* dock for shortcut bar */
	    *frame_parallel, /* frame for interlinear html  */
			     /* editor_dialog */
	    *studypad_dialog,
	    /* entry widgets */
	    *entry_dict,
	    /* toolbars */
	    *toolbar_comments, *toolbar_studypad,
	    /* html widgets */
	    *eventbox1, *html_search_report, /* html widget for serch report shortcut bar */
	    *html_parallel,		     /* parallel html widget */
	    *html_parallel_dialog,	   /* undocked parallel html widget */
	    *html_text,			     /* texts html widget */
	    *html_comm,			     /* html commentary widget */
	    *html_percomm,		     /* html personal commentary widget */
	    *html_dict,			     /* html dictionary widget */
	    *html_book,			     /* gbs html widget */
	    *html_studypad,		     /* studypad html widget */
	    *html_previewer,		     /* to display information (ie strongs) at
					   bottom of text pane */
	    *html_previewer_text,
	    /* book of the Bible combo */
	    *cb_books,
	    /* listview widget */
	    *listview_dict, *store,
	    /* label widgets */
	    *label_comm, *label_dict,
	    /* comboboxentry widgets */
	    *comboboxentry_dict,
	    /* popup menu submenu widgets */
	    *add_bookmark_submenu,
	    /* popup menu item widgets */
	    *viewtexts_item, *viewpreview_item, *parallel_tab_item, *side_preview_item, *viewcomms_item, *viewdicts_item, *linkedtabs_item, *showversenum_item, *readaloud_item, *versehighlight_item, *annotate_highlight_item, *xrefs_in_verse_list_item, *new_journal_item, *button_dict_book, *button_new_tab, /* creates a new passage tab */
	    *hboxtb;																																				   /* container for browsing notebook and button */
};
extern WIDGETS widgets;

#ifdef __cplusplus
}
#endif
#endif
