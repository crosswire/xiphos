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

void initSWORD(GtkWidget *mainform);
void shutdownSWORD(void);
void FillDictKeysSWORD(void);
void changeVerseSWORD(gchar * ref);
void searchSWORD(GtkWidget * searchFrm);
void resultsListSWORD(GtkWidget * searchFrm, 
		gint row, 
		gint column);
void strongsSWORD(gboolean choice);
void footnotesSWORD(gboolean choice);
void changecurModSWORD(gchar *modName, gboolean showchange);
void navcurcomModSWORD(gint direction);
void changecomp1ModSWORD(gint num);
void setversestyleSWORD(gboolean choice);
void chapterSWORD(void);
void verseSWORD(void);
void btnlookupSWORD(void);
void freeformlookupSWORD(GdkEventKey * event);
void changcurcomModSWORD(gchar * modName, 
		gint page_num,
		gboolean showchange);
void editnoteSWORD(gboolean editbuttonactive);
void savenoteSWORD(const gchar *data, gboolean noteModified);
void deletenoteSWORD(void);
void changcurdictModSWORD(gchar * modName, 
		gchar* keyText, 
		gint page_num);
void dictSearchTextChangedSWORD(gchar * mytext);
void dictchangekeySWORD(gint direction);
void showmoduleinfoSWORD(char *modName);
void showinfoSWORD(GtkWidget * text, GtkLabel * label);
void lookupStrongsSWORD(gint theNumber);
void setglobalopsSWORD(gchar *option, gchar *yesno);
void changepercomModSWORD(gchar * modName);
void redisplayTextSWORD(void);
gchar* getmodnameSWORD(gint num);
void navcurcommModSWORD(gint backfoward);
void setuplisteditSWORD(GtkWidget *text);
void changeLEverseSWORD(gchar *verse);
void destroyListEditorSWORD(void);
GList* setupSDSWORD(GtkWidget *text);
void loadSDmodSWORD(GtkWidget *clist,gchar *modName);
gchar* getdictmodSWORD(void);
gchar* gettextmodSWORD(void);
gchar* getcommodSWORD(void);
gchar* getcommodDescriptionSWORD(void);
void gotokeySWORD(gchar *newkey);
void shutdownSDSWORD(void);
void SDdictSearchTextChangedSWORD(char* newkey);
gchar *showfirstlineStrongsSWORD(gint theNumber);
GtkWidget *createSearchDlgSWORD(void);
void startsearchSWORD(GtkWidget *searchFrm);
GList* setupCommSWORD(GtkWidget *text);
void shutdownVCSWORD(void) ;
void loadVCmodSWORD(gchar *modName);
void gotoverseVCSWORD(gchar *newkey);
void navVCModSWORD(gint direction);
gchar* getVCmodDescriptionSWORD(void);
gchar* getSDmodDescriptionSWORD(void);
gboolean saveconfig(void);
gboolean loadconfig(void);
void shutdownVTSWORD(void);
GList* setupVTSWORD(GtkWidget *text, GtkWidget *cbBook);
void gotoverseVTSWORD(gchar *newkey);
void loadVTmodSWORD(gchar *modName);
gchar* getVTmodDescriptionSWORD(void);
GList *getBibleBooks(void);

#endif	/* __GNOMESWORD_H__ */
#ifdef __cplusplus
}
#endif

