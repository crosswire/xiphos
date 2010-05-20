/*
 * Xiphos Bible Study Tool
 * settings.h - SHORT DESCRIPTION
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _settings SETTINGS;
struct _settings {
	char
		*gs_version,	 	/* Xiphos version */
		*path_to_mods,		/* one-time retrieval */
		*MainWindowModule,	/* module to open at program startup  */
		*CommWindowModule,	/* module to open at program startup  */
		*DictWindowModule,	/* module to open at program startup  */
		**parallel_list,	/* strsplit result from xml parallels */
	        *personalcommentsmod,	/* module to open at program startup  */
		sb_search_mod[80], 	/* module for sidebar search */
	        *devotionalmod, 	/* module for devotional */
	        *DefaultDict,		/* module for default dict */
		*lex_greek,		/* module for greek lexicon - strongs or thayers */
		*lex_hebrew,		/* module for hebrew lexicon - strongs or BDB */
	        *currentverse,		/* where we are right now */
		comm_key[80],		/*  */
	        *cvparallel,		/* current verse for detached parallel */
		*percomverse,		/* current verse for personal comments */
	        *dictkey,		/* dictionary key to use at program startup */
	        *studypadfilename,	/* name of file in studypad when we closed */
		*studypaddir,		/* directory for studypad files */
		groupName[50],		/* shortcut bar group name (viewer, verse list) */
		searchText[256],	/* search string used by sidebar search */
		findText[256],		/* string used by find dialog */
		*spell_language,

	/* fonts names/colors/sizes */
	        *bible_text_color,	/* color for text */
	        *bible_bg_color,	/* color for background */
	        *currentverse_color,	/* color for current verse */
	        *link_color,		/* color for strongs, morph tags */
		*bible_verse_num_color,	/* verse number color */
		*found_color,		/* color for search word in results display */
		*font_color,		/*  */
		*highlight_fg,		/* special fg color for highlight */
		*highlight_bg,		/* special bg color for highlight */
		*verse_num_font_size_str,
		*base_font_size_str,

		*special_locale;	/* prefs dialog: pick abnormal locale */
					/* --requires restart-- */

	int
		verse_num_font_size,
		base_font_size,
      		sidebar_width,
		verselist_toppane_height, /* height of the top pane in the search results window */
		upperpane_hight,
		biblepane_width,
		biblepane_hight,
		commpane_hight,
		sidebar_notebook_hight,
		gs_width,
		gs_hight,
		searchbargroup,		  /* number of search group in shortcut bar */
		searchType,
		whichwindow,		  /* which of the main form html windows is active */
		intCurVerse,		  /* detached parallel current verse */
		parallel_page,		  /* parallel workbook page number */
		tab_page,		  /* studypad workbook page number */
		percomm_page;		  /* percomm editor workbook page number */

	int
		strongs,			/* toogle button and check menu state */
		parallel_strongs,		/* check menu state parallel mods */
		parallel_headings,		/* check menu state parallel mods */
		parallel_crossref,		/* check menu state parallel mods */
		parallel_lemmas,		/* check menu state parallel mods */
		parallel_morphs,		/* check menu state parallel mods */
		parallel_segmentation,		/* check menu state parallel mods */
		parallel_red_words,		/* check menu state parallel mods */
		parallel_transliteration,	/* check menu state parallel mods */
		parallel_variants_primary,	/* check menu state parallel mods */
		parallel_variants_secondary,	/* check menu state parallel mods */
		parallel_variants_all,		/* check menu state parallel mods */
		parallel_hebrewpoints,		/* check menu state parallel mods */
		parallel_cantillationmarks,	/* check menu state parallel mods */
	        parallel_footnotes,		/* check menu state parallel mods */
		parallel_greekaccents,		/* check menu state parallel mods */
	        versestyle,			/* verse if true, paragraph if not */
	        linkedtabs,			/* tabs linked together, move in concert */
	        showversenum,			/* display verse numbers in text (default on) */
	        readaloud,			/* pass text through (existing) "festival -server" */
	        versehighlight,			/* do special fg/bg for current verse */
	        annotate_highlight,		/* do special bg/fg for user annotations */
	        parallelpage,			/* display parallel page */
	        autosavepersonalcomments,	/* auto save personal notes */
	        formatpercom,			/* use html to format personal notes */
	        showshortcutbar,		/* display shortcut bar if true */
	        showsidebar,			/* display sidebar bar if true */
		showfavoritesgroup,		/* display shortcut bar group if true */
	        showtextgroup,			/* display shortcut bar group if true */
	        showcomgroup,			/* display shortcut bar group if true */
	        showdictgroup,			/* display shortcut bar group if true */
	        showbookgroup,			/* display shortcut bar group if true */
	        showbookmarksgroup,		/* display shortcut bar group if true */
	        showhistorygroup,		/* display shortcut bar group if true */
		showsplash,			/* show splash on program start if true */
		showdevotional,			/* show devotional on program start if true */
		text_tool,			/* show toolbar in text window if true  */
                showtexts,			/* show bible texts if true  */
                showpreview,			/* show preview pane if true  */
                showcomms,			/* show commentaries if true  */
                showdicts,			/* show dictionaries/lexicons if true */
                showparatab,			/* show parallel view in a tab if true */
                show_previewer_in_sidebar,	/* show previewer in sidebar if true */
		showbookeditor,
		xrefs_in_verse_list,		/* as opposed to old use of previewer */
		displaySearchResults,		/* are we displaying search results in chap display */
		useDefaultDict,
		browsing,			/* true for tabbed browsing */

		/** if items are docked **/
		docked,				/* true when sidebar is docked */
		dockedInt,			/* true when parallel page is docked */

		/** do we have these modules **/
		havebible,
		havecomm,
		havedict,
		havebook,
		havepercomm,
		haveprayerlist,
		/* do we need to add item to history */
		addhistoryitem;


	/* gbs */
	char *book_mod;				/* module to open at program startup  */
	char *book_key;    /*  */
	unsigned long book_offset;

	int comm_showing;			/* whether comm or book is visible */

	/* store program title */
	char program_title[256];

	/* store $home directory */
	char *homedir;

	/* store .xiphos-2.0 directory */
	char *gSwordDir;

	/* bookmarks directory */
	char *swbmDir;

	/* store dir name for shortcutbar files */
	char *shortcutbarDir;

	/* store filename for configure file - options */
	char *fnconfigure;

	/* module name for current dialog */
	char *module_dialog;

	/* apply changes to what? the text navigation bar */
	int apply_change;

	/* load xml bookmarks */
	int load_xml_bookmarks;

	/* is Bibletime on the system */
	int have_bibletime;

	/* cell height of dictionary treeview */
	int cell_height;

	/* window location at close */
	int app_x;
	int app_y;

	/* there is no settings.xml file */
	int first_run;

	/* widget shadow type */
	int shadow_type;

	/* chapter scroll */
	int chapter_scroll;

	/* auto image resize in com/book/dict */
	int imageresize;

	/* signal id dictionary combo changed */
	long signal_id;

	/* mod mgr local or remote  */
	int mod_mgr_source; /* 0 = local, 1 = remote */
	/* mod mgr local sorce  */
	int mod_mgr_local_source_index;
	/* mod mgr remote source  */
	int mod_mgr_remote_source_index;
	/* mod mgr destination  */
	int mod_mgr_destination; /* if = 0 destination is .sword
				    else sword directory  */

	/* if true use prayerlist */
	int prayerlist;

	/* if true parallel tab is showing */
	int paratab_showing;

	/* if set, this is used for an anchor jump if possible */
	char *special_anchor;
};

extern SETTINGS settings;

int settings_init(int argc, char **argv, int new_configs, int new_bookmarks);
void load_settings_structure(void);

#ifdef __cplusplus
}
#endif

#endif
