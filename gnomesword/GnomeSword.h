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
#include <string>

typedef struct _settings SETTINGS;
struct _settings
{
	gchar   	MainWindowModule[80],  //-- modules to open at program startup
						Interlinear1Module[80],
						Interlinear2Module[80],
						Interlinear3Module[80],
						personalcommentsmod[80];
	gchar			currentverse[80],      //-- verse to use at program startup	
						dictkey[80];        //-- dictionary key to use at program startup - the one we shut down with
	gint			currentverse_red,   //-- current verse colors
						currentverse_green,
						currentverse_blue;
	gint			notebook3page,  //-- notebook 3 page number
						notebook1page,  //-- commentaries notebook
						notebook2page;  //-- dict - lex notebook
	gboolean	strongs,           //-- toogle button and check menu state
						footnotes,
						versestyle,
						interlinearpage,
						autosavepersonalcomments;
};

void 
FillDictKeys(char *ModName);

void 
changeVerse(gchar *ref);

void 
ShutItDown(void);

void 
UpdateChecks(GtkWidget *mainform);


void
initSword					(GtkWidget *mainform, 	
								GtkWidget *menu1, 
								GtkWidget *menu2, 
								GtkWidget *menu3, 
								GtkWidget *menu4,
								GtkWidget *menu5);

void
searchSWORD			 (GtkWidget *searchFrm);

void
resultsListSWORD	  (GtkWidget *searchFrm, gint row, gint column);

void
setupSearchDlg		   (GtkWidget *searchDlg);

void
strongsSWORD		  (bool choice);

void
addBookmark				(void);

void
editbookmarksLoad			(GtkWidget *editdlg);

void
loadbookmarks			(GtkWidget *MainFrm);

void
footnotesSWORD		  (bool choice);

void
changecurModSWORD	  (gchar *modName);

void
changecomp1ModSWORD(gchar *modName);

void
changecomp2ModSWORD(gchar *modName);

void
changecomp3ModSWORD(gchar *modName);

void
setversestyleSWORD			(bool choice);

void
showIntPage						 (bool choice);

void
chapterSWORD				  (void);

void
verseSWORD					  (void);

void
btnlookupSWORD				(void);

void
freeformlookupSWORD		  (GdkEventKey  *event);

void
changcurcomModSWORD	 	(gchar *modName, gint page_num);

void
editnoteSWORD					(bool editbuttonactive);

void
savenoteSWORD					(bool noteModified);

void
deletenoteSWORD				(void);

void
changcurdictModSWORD	(gchar *modName, 
										string keyText,
										gint page_num);
void
dictSearchTextChangedSWORD(gchar *mytext);

void
dictchangekeySWORD		(gint direction);

void
addHistoryItem					(void);

void
changepercomModSWORD		(gchar* modName);

void
setcurrentversecolor		(gint arg1, gint arg2, gint arg3);

void
setautosave							(gboolean choice);

void
clearhistory						(void);

void
printfile								(void);

void
openpropertiesbox				(void);

void
changepagenotebook			(GtkNotebook *notebook,
												gint page_num);

#endif /* __GNOMESWORD_H__ */