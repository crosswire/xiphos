/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * sw_sword.h
     * -------------------
     * Mon May 8 2000
     * copyright (C) 2000 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

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

#ifndef __SW_GNOMESWORD_H__
#define __SW_GNOMESWORD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include "gs_gnomesword.h"
	gchar *backend_getmodDescriptionSWORD(gchar *modName); 
	void initSWORD(SETTINGS *s);
	void modNameFromDesc(gchar * modName, gchar * modDesc);
	void updateIntDlg(SETTINGS *s);
	void updateinterlinearpage(void);
	void shutdownSWORD(void);
	void FillDictKeysSWORD(void);
	void changeVerseSWORD(gchar * ref);
	void ChangeVerseSWORD(void);
	void changeVerseComSWORD(void);
	void resultsListSWORD(GtkWidget * searchFrm, gint row, gint column);
	void globaloptionsSWORD(gchar *option, gint window, gboolean choice, gboolean showchange);
	void changecurModSWORD(gchar * modName, gboolean showchange);
	void navcurcomModSWORD(gint direction);
	void changecomp1ModSWORD(gchar * modName);
	void changecomp2ModSWORD(gchar * modName);
	void changecomp3ModSWORD(gchar * modName);
	void setversestyleSWORD(gboolean choice);
	/*** change Bible book ***/
	void bookSWORD(void);
	gchar *intchangeverseSWORD(GtkWidget *book, 
			GtkWidget *chapter, 
			GtkWidget *verse, 
			GtkWidget *entry);
	gchar *intsyncSWORD(GtkWidget *book, 
			GtkWidget *chapter, 
			GtkWidget *verse, 
			GtkWidget *entry,
			gchar *key);	
	void verseSWORD(void);
	void btnlookupSWORD(void);
	void freeformlookupSWORD(GdkEventKey * event);
	void changcurcomModSWORD(gchar * modName, gboolean showchange);
	void savenoteSWORD(gchar *buf);
	void deletenoteSWORD(void);
	void changcurdictModSWORD(gchar * modName, gchar * keyText);
	void dictSearchTextChangedSWORD(gchar * mytext);
	void dictchangekeySWORD(gint direction);
	void showmoduleinfoSWORD(char *modName, gboolean isGBS);
	void setglobalopsSWORD(gint window, gchar * option, gchar * yesno);
	void changepercomModSWORD(gchar * modName);
	void redisplayTextSWORD(void);
	gchar *getmodnameSWORD(gint num);
	gchar *getdictmodSWORD(void);
	gchar *gettextmodSWORD(void);
	gchar *getcommodSWORD(void);
	gchar *getcommodDescriptionSWORD(void);
	GList *backend_getBibleBooksSWORD(void);
	void gotoBookmarkSWORD(gchar * modName, gchar * key);
	gchar *getmodkeySWORD(gint num);
	const char *getSwordVerionSWORD(void);
	void swapmodsSWORD(gchar * intmod);
	void loadpreferencemodsSWORD(void);
	void gs_firstrunSWORD(void);
	void applyfontcolorandsizeSWORD(void);	
	/*** write individual module key to <module>.conf ***/
	void savekeySWORD(gint modwindow, gchar * key);	
	/*** write individual module font information to <module>.conf ***/
	gboolean savefontinfoSWORD(gchar *modName, gchar *modtag, gchar * fontinfo);
	/*** display daily devotional ***/
	void displayDevotional(void);
	/******************************************************************************
	 * get module type - Bible text, Commentary or Dict/Lex
	 ******************************************************************************/
	gint get_mod_typeSWORD(gchar *modName);

#ifdef __cplusplus
}
#endif
#endif				/* __SW_GNOMESWORD_H__ */
