/***************************************************************************
                          GnomeSword.h  -  description
                             -------------------
    begin                : Mon May 8 2000
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
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */
#ifndef __GNOMESWORD_H__
#define __GNOMESWORD_H__


#include <gnome.h>
//#include <string>
#include  <widgets/shortcut-bar/e-shortcut-bar.h>

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
	gint    currentverse_red,	//-- current verse colors
	        currentverse_green, currentverse_blue;
	gint    notebook3page,	//-- notebook 3 page number
	        notebook1page,		//-- commentaries notebook
	        notebook2page;		//-- dict - lex notebook
	gint	shortcutbarsize;
	gboolean strongs,	//-- toogle button and check menu state
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
	        showhistorygroup;
};

typedef struct _listitem LISTITEM;
struct _listitem {
	gint type;
	gint level;
	gchar item[80];
	gchar preitem[80];
	gchar subitem[80];
	gchar menu[80];
};


void FillDictKeys(char *ModName);

void changeVerse(gchar * ref);

void ShutItDown(void);

void UpdateChecks(GtkWidget * mainform);

void initSword(GtkWidget *mainform);

void searchSWORD(GtkWidget * searchFrm);

void resultsListSWORD(GtkWidget * searchFrm, gint row, gint column);

void setupSearchDlg(GtkWidget * searchDlg);

void strongsSWORD(gboolean choice);

void addBookmark(void);

void editbookmarksLoad(GtkWidget * editdlg);

void footnotesSWORD(gboolean choice);

void changecurModSWORD(gchar * modName);

void navcurcomModSWORD(gint direction);

void changecomp1ModSWORD(gchar * modName);

void changecomp2ModSWORD(gchar * modName);

void changecomp3ModSWORD(gchar * modName);

void setversestyleSWORD(gboolean choice);

void showIntPage(gboolean choice);

void chapterSWORD(void);

void verseSWORD(void);

void btnlookupSWORD(void);

void freeformlookupSWORD(GdkEventKey * event);

void changcurcomModSWORD(gchar * modName, gint page_num);

void editnoteSWORD(gboolean editbuttonactive);

void savenoteSWORD(gboolean noteModified);

void deletenoteSWORD(void);

void changcurdictModSWORD(gchar * modName, gchar* keyText, gint page_num);
void dictSearchTextChangedSWORD(gchar * mytext);

void dictchangekeySWORD(gint direction);

void addHistoryItem(gchar *ref);

void changepercomModSWORD(gchar * modName);

void setcurrentversecolor(gint arg1, gint arg2, gint arg3);

void setautosave(gboolean choice);

void clearhistory(void);

void printfile(void);

void openpropertiesbox(void);

void changepagenotebook(GtkNotebook * notebook, gint page_num);

void showmoduleinfoSWORD(char *modName);

void showinfoSWORD(GtkWidget * text, GtkLabel * label);

void newSP(GtkWidget * text);

void setformatoption(GtkWidget * button);

gint getversenumber(GtkWidget * text);

gint getdictnumber(GtkWidget * text);

void sbchangeModSword(gint group_num, gint item_num);

void
applyoptions(gboolean showshortcut,
	     gboolean showcomtabs,
	     gboolean showdicttabs,
	     gboolean showtextgroup, 
	     gboolean showcomgroup, 
	     gboolean showdictgroup);

void add_sb_group(EShortcutBar * shortcut_bar, gchar * group_name);

void lookupStrongsSWORD(gint theNumber);



#endif	/* __GNOMESWORD_H__ */
