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

#ifndef __SWORD_H__
#define __SWORD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include "gs_gnomesword.h"

/* these strings are not seen by users */
#define TEXT_MODS "Biblical Texts"
#define COMM_MODS "Commentaries"
#define DICT_MODS "Lexicons / Dictionaries"
#define BOOK_MODS "Generic Books"



	void backend_firstInitSWORD(void);
	void initSWORD(SETTINGS * s);
	void modNameFromDesc(gchar * modName, gchar * modDesc);
	void updateIntDlg(SETTINGS * s);
	void updateinterlinearpage(void);
	void backend_shutdown(SETTINGS * s);
	void FillDictKeysSWORD(void);
	void changeVerseSWORD(gchar * ref);
	void ChangeVerseSWORD(void);
	void changeVerseComSWORD(void);
	void resultsListSWORD(GtkWidget * searchFrm,
			      gint row, gint column);
	void globaloptionsSWORD(gchar * option, gint window,
				gboolean choice, gboolean showchange);
	void changecurModSWORD(gchar * modName, gboolean showchange);
	void navcurcomModSWORD(gint direction);
	void changecomp1ModSWORD(gchar * modName);
	void changecomp2ModSWORD(gchar * modName);
	void changecomp3ModSWORD(gchar * modName);
	void setversestyleSWORD(gboolean choice);
	void backend_set_locale(char *locale);
	/*** change Bible book ***/
	void bookSWORD(void);
	gchar *intchangeverseSWORD(GtkWidget * book,
				   GtkWidget * chapter,
				   GtkWidget * verse,
				   GtkWidget * entry);
	gchar *intsyncSWORD(GtkWidget * book,
			    GtkWidget * chapter,
			    GtkWidget * verse,
			    GtkWidget * entry, gchar * key);
	void verseSWORD(void);
	void btnlookupSWORD(void);
	void freeformlookupSWORD(GdkEventKey * event);
	void changcurcomModSWORD(gchar * modName, gboolean showchange);
	void savenoteSWORD(gchar * buf);
	void deletenoteSWORD(void);
	void changcurdictModSWORD(gchar * modName, gchar * keyText);
	void dictSearchTextChangedSWORD(gchar * mytext);
	void dictchangekeySWORD(gint direction);
	void setglobalopsSWORD(gint window, gchar * option,
			       gchar * yesno);
	void changepercomModSWORD(gchar * modName);
	void redisplayTextSWORD(void);
	gchar *getmodnameSWORD(gint num);
	gchar *getdictmodSWORD(void);
	gchar *gettextmodSWORD(void);
	gchar *getcommodSWORD(void);
	gchar *getcommodDescriptionSWORD(void);
	GList *backend_get_books(void);
	void gotoBookmarkSWORD(gchar * modName, gchar * key);
	gchar *getmodkeySWORD(gint num);
	const char *getSwordVerionSWORD(void);
	void swapmodsSWORD(gchar * intmod);
	void loadpreferencemodsSWORD(void);
	void backend_display_new_font_color_and_size(SETTINGS *s);
	void backend_save_module_key(gint modwindow, gchar * key, SETTINGS *s);
	void backend_display_devotional(SETTINGS * s);
	int backend_get_mod_type(gchar * modName);
	GList *backend_get_list_of_mods_by_type(char *mod_type);
	GList *backend_get_mod_description_list_SWORD(char *mod_type);
	gchar *backend_get_module_description(gchar * modName);
	gchar *backend_get_path_to_mods(void);
	gchar *backend_get_mod_aboutSWORD(gchar * modname);

#ifdef __cplusplus
}
#endif
#endif				/* __SWORD_H__ */
/*****  end of file    ******/
