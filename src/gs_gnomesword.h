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
 
#ifndef __GS_GNOMESWORD_H__
#define __GS_GNOMESWORD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gs_sword.h"
	

typedef struct _mods GS_MODS;
struct _mods {
	gchar	*name,
			*type,
			*key,
			*description;
	gint		modnum;
};
 
typedef struct _settings SETTINGS;
struct _settings {
	GtkWidget
		*app,
		*ctree_widget,
		*versestyle_item;	/* widget to access toggle menu - for versestyle */
	gchar	
		MainWindowModule[80],	/* modules to open at program startup  */
		CommWindowModule[80],
		DictWindowModule[80],
	        Interlinear1Module[80],
	        Interlinear2Module[80],
	        Interlinear3Module[80], 
	        Interlinear4Module[80],
	        Interlinear5Module[80], 
	        personalcommentsmod[80], 
		lex_greek[80],
		lex_hebrew[80],
	        currentverse[80],	/* verse to use at program startup */
	        dictkey[80],		/* dictionary key to use at program startup - the one we shut down with */
	        studypadfilename[255],   /* name of file in studypad when we closed */
		studypaddir[255],	    /* directory for studypad files */
	        bible_text_color[15], /* color for text */
	        bible_bg_color[15],  /* color for background */
	        currentverse_color[15],  /* color for current verse */	
	        currentverse_bg_color[15],  /* color for current verse background */
	        link_color[15],  /* color for strongs, morph tags */	
		bible_verse_num_color[15],
		bible_font_size[4], /*** html font sizes ***/
		commentary_font_size[4],
		dictionary_font_size[4],
		interlinear_font_size[4],
		verselist_font_size[4],
		verse_num_font_size[4];			
	gint 
      		currentverse_red,	/* current verse colors */
	        currentverse_green, 
	        currentverse_blue,
		shortcutbar_width,
		upperpane_hight,
		biblepane_width,
		gs_width,
		gs_hight,	
		notebook3page;	//-- notebook 3 page number
	gboolean   
		usedefault,
		strongs,	//-- toogle button and check menu states
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
	        showhistorygroup,
		showsplash,
		text_tabs, /* show module tabs in text window if true  */
		comm_tabs, /* show module tabs in commentary window if true  */
		dict_tabs; /* show module tabs in text dict/lex if true  */; 
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
gint getversenumber(GtkWidget * text);
gint getdictnumber(GtkWidget * text);
void setformatoption(GtkWidget * button);
void changepagenotebook(GtkNotebook * notebook, 
		gint page_num);
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
gint string_is_color(gchar *color);
gchar *gdouble_arr_to_hex(gdouble *color,
		gint websafe);
gdouble *hex_to_gdouble_arr(gchar *color);
#ifdef __cplusplus
}
#endif
#endif /* __GS_GNOMESWORD_H__ */

