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

#include "gs_sword.h"


typedef struct _settings SETTINGS;
struct _settings {
	gchar   MainWindowModule[80],	//-- modules to open at program startup
	           Interlinear1Module[80],
	           Interlinear2Module[80],
	           Interlinear3Module[80], 
	           personalcommentsmod[80], 
	           currentverse[80],	//-- verse to use at program startup 
	           dictkey[80],		//-- dictionary key to use at program startup - the one we shut down with
	           studypadfilename[255];	//-- name of file in open in study when we closed or last file in studypad
	           //------- 8
	gint      currentverse_red,	//-- current verse colors
	           currentverse_green, 
	           currentverse_blue;
	gint      notebook3page,	//-- notebook 3 page number
	           notebook1page,		//-- commentaries notebook
	           notebook2page;		//-- dict - lex notebook
	gint	  shortcutbarsize;
			//------- 7
	gboolean   strongs,	//-- toogle button and check menu state
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
	        	//------- 13
	        	//------- 28
};

void initGnomeSword(GtkWidget *app, 
		SETTINGS *settings, 
		GList *biblemods, 
		GList *commentarymods , 
		GList *dictionarymods, 
		GList *percommods);
void addnotebookpages(GtkWidget *notebook, GList *list);
void UpdateChecks(GtkWidget * mainform);
void applyoptions(GtkWidget *app,
		gboolean showshortcut,
	     	gboolean showcomtabs,
	     	gboolean showdicttabs,
	     	gboolean showtextgroup, 
	     	gboolean showcomgroup, 
	     	gboolean showdictgroup, 
	     	gboolean showhistorygroup);
gint add_sb_group(EShortcutBar * shortcut_bar, 
		gchar * group_name);
		
#ifdef __cplusplus
}
#endif
