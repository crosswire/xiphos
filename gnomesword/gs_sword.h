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
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNOMESWORD_H__
#define __GNOMESWORD_H__


#include <gnome.h>
//#include <string>
#include  <widgets/shortcut-bar/e-shortcut-bar.h>



void initSword(GtkWidget *mainform);

void ShutItDown(void);

void FillDictKeys(void);
void changeVerse(gchar * ref);
void searchSWORD(GtkWidget * searchFrm);
void resultsListSWORD(GtkWidget * searchFrm, 
		gint row, 
		gint column);
void setupSearchDlg(GtkWidget * searchDlg);
void strongsSWORD(gboolean choice);
void footnotesSWORD(gboolean choice);
void changecurModSWORD(gchar * modName);
void navcurcomModSWORD(gint direction);
void changecomp1ModSWORD(gchar * modName);
void changecomp2ModSWORD(gchar * modName);
void changecomp3ModSWORD(gchar * modName);
void setversestyleSWORD(gboolean choice);
void chapterSWORD(void);
void verseSWORD(void);
void btnlookupSWORD(void);
void freeformlookupSWORD(GdkEventKey * event);
void changcurcomModSWORD(gchar * modName, 
		gint page_num);
void editnoteSWORD(gboolean editbuttonactive);
void savenoteSWORD(gboolean noteModified);
void deletenoteSWORD(void);
void changcurdictModSWORD(gchar * modName, 
		gchar* keyText, 
		gint page_num);
void dictSearchTextChangedSWORD(gchar * mytext);
void dictchangekeySWORD(gint direction);
void showmoduleinfoSWORD(char *modName);
void showinfoSWORD(GtkWidget * text, 
		GtkLabel * label);
void lookupStrongsSWORD(gint theNumber);
void setglobalopsSWORD(gchar *option,
		gchar *yesno);
void changepercomModSWORD(gchar * modName);
void redisplayTextSWORD(void);
gchar* getmodnameSWORD(gint num);

#endif	/* __GNOMESWORD_H__ */
#ifdef __cplusplus
}
#endif
