/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_gnomesword.h
                             -------------------
    begin                : Tue Dec 05 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/

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
#ifdef __cplusplus
extern "C" {
#endif


//#include <gtkhtml/gtkhtml.h>
#include "gs_sword.h"
	
#if USE_SHORTCUTBAR	
//#include  <gal/shortcut-bar/e-shortcut-bar.h>
#endif /* USE_SHORTCUTBAR */
	
typedef struct _mods GS_MODS;
struct _mods {
	gchar	*name,
			*type,
			*key,
			*description;
	gint		modnum;
};
//extern MODULES *textmodule;
//extern MODULES *commmodule;
//extern MODULES *dictmodule;
typedef struct _gsfonts GS_FONTS;
struct _gsfonts {
	gchar	bible_font_size[4], /*** html font sizes ***/
			commentary_font_size[4],
			dictionary_font_size[4],
			interlinear_font_size[4];
};


typedef struct _gs_layout GS_LAYOUT;
struct _gs_layout {
	gint	shortcutbar_width,
		upperpane_hight,
		biblepane_width,
		gs_width,
		gs_hight;	
};

typedef struct _gs_lexicon GS_LEXICON;
struct _gs_lexicon {
	gchar 	greek[80],
			hebrew[80];
};

typedef struct _gsnbpages GS_NB_PAGES;
struct _gsnbpages {
	gint	nbTextModspage, /* Bible window */
		notebook1page, /* commentary window  */
		notebook2page, /* dict/lex window  */
		notebook3page; /* main notebook */	
};

typedef struct _gstabs GS_TABS;
struct _gstabs {
	gboolean 	textwindow, /* show module tabs in text window if true  */
				commwindow, /* show module tabs in commentary window if true  */
				dictwindow; /* show module tabs in text dict/lex if true  */
};
 
typedef struct _settings SETTINGS;
struct _settings {
	gchar	   MainWindowModule[80],	/* modules to open at program startup  */
	           Interlinear1Module[80],
	           Interlinear2Module[80],
	           Interlinear3Module[80], 
	           Interlinear4Module[80],
	           Interlinear5Module[80], 
	           personalcommentsmod[80], 
	           currentverse[80],	/* verse to use at program startup */
	           dictkey[80],		/* dictionary key to use at program startup - the one we shut down with */
	           studypadfilename[255],   /* name of file in studypad when we closed */
	           currentverse_color[15];  /* color for current verse */
	gint       currentverse_red,	/* current verse colors */
	           currentverse_green, 
	           currentverse_blue;
	gint       notebook3page,	//-- notebook 3 page number
	           notebook1page,	//-- commentaries notebook
	           notebook2page;	//-- dict - lex notebook
	gint	   shortcutbarsize;
	gboolean   strongs,	//-- toogle button and check menu states
	           footnotes,
	           versestyle,
	           interlinearpage,
	           autosavepersonalcomments,
	           formatpercom,
	           showcomtabs,
	           showdicttabs,
	           showshortcutbar,
	           showtextgroup,
	           showcomgroup,
	           showdictgroup,
	           showbookmarksgroup,
	           showhistorygroup;
};

typedef struct _gs_app GS_APP;
struct _gs_app {
	GtkWidget  *app,
			*Bible,
			*Commentary,
			*Dicttionary,
			*Interlinear,
			*Percom,
			*ctree_widget;		
	
};
typedef struct _interlinear INTERLINEAR;
struct _interlinear {
	gchar 	ver1[80],
		ver2[80],
		ver3[80],
		ver4[80],
		ver5[80];
};



void initGnomeSword(GtkWidget *app, 
		SETTINGS *settings, 
		GList *biblemods, 
		GList *commentarymods , 
		GList *dictionarymods, 
		GList *percommods);
void addnotebookpages(GtkWidget *notebook, 
		GList *list); 
void UpdateChecks(GtkWidget * mainform);
void applyoptions(GtkWidget *app,
		gboolean showshortcut,
		gboolean showtexttabs,
	     	gboolean showcomtabs,
	     	gboolean showdicttabs,
	     	gboolean showtextgroup, 
	     	gboolean showcomgroup, 
	     	gboolean showdictgroup, 
	     	gboolean showhistorygroup);

gint getversenumber(GtkWidget * text);
gint getdictnumber(GtkWidget * text);
void sbchangeModSword(GtkWidget *app, 
		GtkWidget *shortcut_bar, 
		gint group_num, 
		gint item_num);		
void setformatoption(GtkWidget * button);
void changepagenotebook(GtkNotebook * notebook, 
		gint page_num);
void openpropertiesbox(void);
void editbookmarksLoad(GtkWidget * editdlg);
void addBookmark(GtkWidget *app);
void showIntPage(GtkWidget *app, 
		gboolean choice);
void setcurrentversecolor(gint arg1, 
		gint arg2, 
		gint arg3);
void setautosave(gboolean choice);
void fillSBtoolbars(GtkWidget *app,
		GList *biblelist,
		GList *commentarylist,
		GList *dictionarylist);
void setupSidebar(GtkWidget *app);
gchar *gdouble_arr_to_hex(gdouble *color,
		gint websafe);


#ifdef __cplusplus
}
#endif

