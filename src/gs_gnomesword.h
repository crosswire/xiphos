/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_gnomesword.h
    * -------------------
    * Tue Dec 05 2000
    * copyright (C) 2001 by tbiggs
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
 
#ifndef __GS_GNOMESWORD_H__
#define __GS_GNOMESWORD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sw_gnomesword.h"

#define MAIN_TEXT_WINDOW 0 
#define INTERLINEAR_WINDOW 1
#define COMMENTARY_WINDOW 2
#define DICTIONARY_WINDOW 3
	
typedef struct _settings SETTINGS;
struct _settings {
	GtkWidget
		*app,  /* main form */
		*appbar, /* gnome status/progress bar */
		*shortcut_bar,  /* the shortcut bar widget */
		*ctree_widget, /* bookmarks tree */
		*ctree_widget_books, /* books tree */
		*hboxToolbar, /* for editor toolbars ??? */
	
	/* html widgets */
		*vlsbhtml,  /* html widget for verselist list in shortcut bar */	
		*srhtml,  /* html widget for serch results list in shortcut bar */	
		*htmlRP,  /* html widget for serch report shortcut bar */
		*htmlcomm, /* html commentary widget */
		*htmlSP, /* studypad html widger */
	
	/* popup menu item widgets */
		*versestyle_item,	/* widget to access toggle menu - for versestyle */
		*add_bookmark_menu, /* add bookmark menu item */
		*unlocktextmod_item, /* unlock module menu item */
		*unlockcommmod_item, /* unlock module menu item */
		*unlockdictmod_item, /* unlock module menu item */
	
	/* global option menu check item widgets */
		*strongsnum,
		*footnotes,
		*hebrewpoints,
		*cantillationmarks,
		*greekaccents,
		*morphs,
		*crossrefs,
		*lemmas,
		*headings;
		
	gchar	
		gs_version[16], /* GnomeSWORD version */
		MainWindowModule[80],	/* module to open at program startup  */
		CommWindowModule[80],	/* module to open at program startup  */
		DictWindowModule[80],	/* module to open at program startup  */
	        Interlinear1Module[80],	/* module to open at program startup  */
	        Interlinear2Module[80],	/* module to open at program startup  */
	        Interlinear3Module[80], 	/* module to open at program startup  */
	        Interlinear4Module[80],	/* module to open at program startup  */
	        Interlinear5Module[80], 	/* module to open at program startup  */
	        personalcommentsmod[80], 	/* module to open at program startup  */
	        devotionalmod[80], /* module to use for devotional */
		lex_greek[80], /* module to use for greek lexicon - strongs or thayers*/
		lex_hebrew[80], /* module to use for hebrew lexicon - strongs or BDB*/
	        currentverse[80],	/* verse to use at program startup */
		*percomverse, /* current verse for personal comments */
	        dictkey[80],		/* dictionary key to use at program startup - the one we shut down with */
	        studypadfilename[255],   /* name of file in studypad when we closed */
		studypaddir[255],	    /* directory for studypad files */
		groupName[50], /* ??? */
		searchText[256]; /* ??? */	
		
	gchar /* fonts, font colors and font sizes */
	        bible_text_color[15], /* color for text */
	        bible_bg_color[15],  /* color for background */
	        currentverse_color[15],  /* color for current verse */	
	        currentverse_bg_color[15],  /* color for current verse background */
	        link_color[15],  /* color for strongs, morph tags */	
		bible_verse_num_color[15], /* verse number color */
		found_color[15], /* color to use for search word in results display */
		bible_font_size[4], /*** html font sizes ***/
		commentary_font_size[4],
		dictionary_font_size[4],
		interlinear_font_size[4],
		verselist_font_size[4],
		verse_num_font_size[4],
		default_font[126], /* use if no other font is requested */
		greek_font[126],
		hebrew_font[126],
		unicode_font[126];		
		
	gint 
      		shortcutbar_width,
		upperpane_hight,
		biblepane_width,
		gs_width,
		tmp_width,
		gs_hight,	
		notebook3page,	//-- notebook 3 page number
		searchbargroup,      //-- number of search group in shortcut bar
		searchType,
		whichwindow;      	/* which of the main form html windows is active */
						/* 0=text, 1= comm, 2=dict */
						
	gboolean   
		usedefault, /* use default settings in gnomesword or those used for last session */
		strongs,	//-- toogle button and check menu state
		strongsint, //-- check menu state interlinear mods
		headingsint, //-- check menu state interlinear mods	
		crossrefint, //-- check menu state interlinear mods	
		lemmasint, //-- check menu state interlinear mods		
		morphsint, //-- check menu state interlinear mods
		hebrewpointsint, //-- check menu state interlinear mods
		cantillationmarksint, //-- check menu state interlinear mods
	        footnotesint, //-- check menu state interlinear mods
		greekaccentsint, //-- check menu state interlinear mods
	        versestyle, /* use verse style if true of paragraph style if not */
	        interlinearpage, /* display interlinear page */
	        autosavepersonalcomments, /* auto save personal notes */
	        formatpercom, /* use html to format personal notes */
	        showshortcutbar, /* display shortcut bar if true */
		showfavoritesgroup, /* display shortcut bar group if true */
	        showtextgroup, /* display shortcut bar group if true */
	        showcomgroup, /* display shortcut bar group if true */
	        showdictgroup, /* display shortcut bar group if true */
	        showbookmarksgroup, /* display shortcut bar group if true */
	        showhistorygroup, /* display shortcut bar group if true */
		showsplash, /* show splash on program start if true */
		showdevotional, /* show devotional on program start if true */
		text_tabs, /* show module tabs in text window if true  */
		comm_tabs, /* show module tabs in commentary window if true  */
		dict_tabs, /* show module tabs in text dict/lex if true  */
		displaySearchResults, /* are we displaying search results in chap display */
		showinmain, /* when verse list item clicked show in main form if true */
		editnote, /* true when percom html widget is in edit mode */
		notefollow, /* notes follow Bible text when true */
		modifiedSP; /* studypad file have been modified */
		
	GList 
		*settingslist; /* glist for saveing verse lists - freed in shutdownSWORD() gs_sword.cpp*/
};


/*** function prototypes ***/

void initGnomeSword(GtkWidget *app, 
		SETTINGS *settings, 
		GList *biblemods, 
		GList *commentarymods , 
		GList *dictionarymods, 
		GList *percommods,
		GtkWidget *splash);
gint addnotebookpages(GtkWidget *notebook, 
		GList *list,
		gchar *modName); 
void UpdateChecks(GtkWidget * mainform);
void setformatoption(GtkWidget * button);
void changepagenotebook(GtkNotebook * notebook, 
		gint page_num);
void addQuickmark(GtkWidget *app);
void showIntPage(GtkWidget *app, 
		gboolean choice);
void setautosave(gboolean choice);
void fillSBtoolbars(GtkWidget *app,
		GList *biblelist,
		GList *commentarylist,
		GList *dictionarylist);
void setupSidebar(GtkWidget *app);
gint string_is_color(gchar *color);
gchar *gdouble_arr_to_hex(gdouble *color,
		gint websafe);
gdouble *hex_to_gdouble_arr(gchar *color);


#ifdef __cplusplus
}
#endif

#endif /* __GS_GNOMESWORD_H__ */

