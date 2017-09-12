/*
 * Xiphos Bible Study Tool
 * settings.h - structure definitions
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

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _settings SETTINGS;
struct _settings
{
	char *gs_version,	 /* Xiphos version */
	    *path_to_mods,	/* one-time retrieval */
	    *MainWindowModule,    /* module to open at program startup  */
	    *CommWindowModule,    /* module to open at program startup  */
	    *DictWindowModule,    /* module to open at program startup  */
	    **parallel_list,      /* strsplit result from xml parallels */
	    *personalcommentsmod, /* module to open at program startup  */
	    sb_search_mod[80],    /* module for sidebar search */
	    *devotionalmod,       /* module for devotional */
	    *DefaultDict,	 /* module for default dict */
	    *lex_greek,		  /* module for greek lexicon - strongs or thayers */
	    *lex_hebrew,	  /* module for hebrew lexicon - strongs or BDB */
	    *currentverse,	/* where we are right now */
	    *cvparallel,	  /* current verse for detached parallel */
	    *dictkey,		  /* dictionary key to use at program startup */
	    *studypadfilename,    /* name of file in studypad when we closed */
	    *studypaddir,	 /* directory for studypad files */
	    searchText[256],      /* search string used by sidebar search */
	    findText[256],	/* string used by find dialog */
	    *spell_language,
	    /* fonts names/colors/sizes */
	    *bible_text_color,						    /* color for text */
	    *bible_bg_color,						    /* color for background */
	    *currentverse_color,					    /* color for current verse */
	    *link_color,						    /* color for strongs, morph tags */
	    *bible_verse_num_color,					    /* verse number color */
	    *found_color,						    /* color for search word in results display */
	    *font_color,						    /*  */
	    *highlight_fg,						    /* special fg color for highlight */
	    *highlight_bg,						    /* special bg color for highlight */
	    *verse_num_font_size_str, *base_font_size_str, *special_locale; /* prefs dialog: pick abnormal locale */
	/* --requires restart-- */

	int
	    verse_num_font_size,
	    verse_num_bold,																													   /* verse number customizations */
	    verse_num_bracket, verse_num_superscript, base_font_size, sidebar_width, verselist_toppane_height,																			   /* height of the top pane in the search results window */
	    biblepane_width, biblepane_height, commpane_height, sidebar_notebook_height, gs_width, gs_height, parallel_height, parallel_width, modmgr_height, modmgr_width, advsearch_height, advsearch_width, prefs_height, prefs_width, searchType, whichwindow, /* which of the main form html windows is active */
	    intCurVerse,																													   /* detached parallel current verse */
	    parallel_page,																													   /* parallel workbook page number */
	    tab_page,																														   /* studypad workbook page number */
	    percomm_page;																													   /* percomm editor workbook page number */

	int
	    strongs,			 /* toogle button and check menu state */
	    parallel_strongs,		 /* check menu state parallel mods */
	    parallel_headings,		 /* check menu state parallel mods */
	    parallel_crossref,		 /* check menu state parallel mods */
	    parallel_lemmas,		 /* check menu state parallel mods */
	    parallel_morphs,		 /* check menu state parallel mods */
	    parallel_segmentation,       /* check menu state parallel mods */
	    parallel_red_words,		 /* check menu state parallel mods */
	    parallel_transliteration,    /* check menu state parallel mods */
	    parallel_variants_primary,   /* check menu state parallel mods */
	    parallel_variants_secondary, /* check menu state parallel mods */
	    parallel_variants_all,       /* check menu state parallel mods */
	    parallel_hebrewpoints,       /* check menu state parallel mods */
	    parallel_cantillationmarks,  /* check menu state parallel mods */
	    parallel_footnotes,		 /* check menu state parallel mods */
	    parallel_greekaccents,       /* check menu state parallel mods */
	    parallel_xlit,		 /* check menu state parallel mods */
	    parallel_enumerated,	 /* check menu state parallel mods */
	    parallel_glosses,		 /* check menu state parallel mods */
	    parallel_morphseg,		 /* check menu state parallel mods */
	    versestyle,			 /* verse if true, paragraph if not */
	    linkedtabs,			 /* tabs linked together, move in concert */
	    showversenum,		 /* display verse numbers in text (default on) */
	    readaloud,			 /* pass text through "festival -server" */
	    versehighlight,		 /* do special fg/bg for current verse */
	    annotate_highlight,		 /* do special bg/fg for user annotations */
					 /* saved startup displays */
	    display_parallel,		 /* detached parallel */
	    display_modmgr,		 /* mod.mgr */
	    display_advsearch,		 /* advanced search */
	    display_prefs,		 /* preferences */
	    formatpercom,		 /* use html to format personal notes */
	    showshortcutbar,		 /* display shortcut bar if true */
	    showsplash,			 /* show splash on program start if true */
	    showdevotional,		 /* show devotional on program start if true */
	    showtexts,			 /* show bible texts if true  */
	    showpreview,		 /* show preview pane if true  */
	    showcomms,			 /* show commentaries if true  */
	    showdicts,			 /* show dictionaries/lexicons if true */
	    showparatab,		 /* show parallel view in a tab if true */
	    show_previewer_in_sidebar,   /* show previewer in sidebar if true */
	    xrefs_in_verse_list,	 /* as opposed to old use of previewer */
	    displaySearchResults,	/* are we displaying search results in chap display */
	    useDefaultDict,
	    browsing,  /* true for tabbed browsing */
		       /** if items are docked **/
	    docked,    /* true when sidebar is docked */
	    dockedInt, /* true when parallel page is docked */
		       /** do we have these modules **/
	    havebible,
	    havecomm,
	    havedict, havebook, havepercomm, haveprayerlist,
	    /* do we need to add item to history */
	    addhistoryitem;

	int
	    bs_mode,	 // [0..3] for radio button select
			     // 0=disable; 1=personal; 2=speaker; 3=audience.
	    bs_privacy,      // in personal mode, whether private (ttl 0)
	    bs_navdirect,    // or via verse list (false).
	    bs_debug,	// show pkt traces.
	    bs_presence,     // show presence announcements.
	    bs_mismatch,     // show mismatch packets.
	    bs_receiving,    // lock: don't re-xmit what we recv.
	    bs_listen_set,   // 0 = some, 1 = all, 2 = none.
	    bs_group_tab;    // use protocol's group# as tab#.
	char *bs_passphrase; // session exclusion/isolation.

	/* gbs */
	char *book_mod; /* module to open at program startup  */
	char *book_key; /*  */
	unsigned long book_offset;

	int comm_showing; /* whether comm or book is visible */

	/* store program title */
	char program_title[256];

	/* store $home directory */
	char *homedir;

	/* store .xiphos directory */
	char *gSwordDir;

	/* bookmarks directory */
	char *swbmDir;

	/* store filename for configure file - options */
	char *fnconfigure;

	/* apply changes to what? the text navigation bar */
	int apply_change;

	/* load xml bookmarks */
	int load_xml_bookmarks;

	/* cell height of dictionary treeview */
	int cell_height;

	/* window location at close */
	int app_x, app_y;
	int maximized;	/* gboolean */
	int parallel_x, parallel_y;
	int modmgr_x, modmgr_y;
	int advsearch_x, advsearch_y;
	int prefs_x, prefs_y;

	/* there is no settings.xml file */
	int first_run;

	/* widget shadow type */
	int shadow_type;

	/* chapter scroll */
	int chapter_scroll;

	/* auto image resize in com/book/dict */
	int imageresize;

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

	/* statusbar setting */
	int statusbar;

	/* parallel color alternation */
	int alternation;

	/* if true parallel tab is showing */
	int paratab_showing;

	/* if set, this is used for an anchor jump if possible */
	char *special_anchor;
};

extern SETTINGS settings;

int settings_init(int argc, char **argv, int new_configs,
		  int new_bookmarks);
void load_settings_structure(void);

#ifdef __cplusplus
}
#endif
#endif
