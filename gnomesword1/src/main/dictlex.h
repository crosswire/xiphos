/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_dictlex.h
     * -------------------
     * Wed Apr  3 16:18:53 2002
     * copyright (C) 2002 by Terry Biggs
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

#ifndef __DICTLEX_H_
#define __DICTLEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "settings.h"

typedef struct _DLFindDialog DLFindDialog;
struct _DLFindDialog {
	GnomeDialog *dialog;
	GtkWidget *htmlwidget;
	GtkWidget *entry;
	GtkWidget *backward;
	GtkWidget *case_sensitive;
	gboolean regular;
};

typedef struct _dldata DL_DATA;
struct _dldata {
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *clist;
	GtkWidget *entry;
	GtkWidget *showtabs;
	gchar *modName;
	gchar *searchstring;
	int mod_num;
	gboolean has_key;
	DLFindDialog *find_dialog;
};
void display_dictionary_page_and_key(gint page_num, gchar * key);
GList *fill_dictlex_keys(int mod_num, int count);
void setup_dictlex(SETTINGS * s, GList *mods);
void shutdown_dictlex(void);
void set_dictionary_page_and_key(gint page_num,
				     gchar * key);
void display_dictlex(gchar * key);

#ifdef __cplusplus
}
#endif
#endif	/* __DICTLEX_H_ */
