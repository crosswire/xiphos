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

#include <gnome.h>

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
changcurcomModSWORD	 	(gchar *modName);

void
editnoteSWORD					(bool editbuttonactive);

void
savenoteSWORD					(bool noteModified);

void
deletenoteSWORD				(void);

void
changcurdictModSWORD	(gchar *modName, 
										string keyText);
void
dictSearchTextChangedSWORD(gchar *mytext);

void
addHistoryItem					(void);

void
changepercomModSWORD		(gchar* modName);

void
setcurrentversecolor		(gint arg1, gint arg2, gint arg3);

void
setautosave							(gboolean choice);