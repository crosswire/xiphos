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

#ifndef __GS_DICTLEX_H_
#define __GS_DICTLEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gs_gnomesword.h"

typedef struct _DLFindDialog  DLFindDialog;
struct _DLFindDialog {
	GnomeDialog *dialog;
	GtkWidget   *htmlwidget;
	GtkWidget   *entry;
	GtkWidget   *backward;
	GtkWidget   *case_sensitive;
	gboolean     regular;
};

typedef struct _dldata DL_DATA;
struct _dldata {
	GtkWidget *html;
	GtkWidget *clist;
	GtkWidget *entry;
	GtkWidget *showtabs;
	gchar *modName;
	gchar *modDescription;
	gchar *searchstring;
	DLFindDialog *find_dialog;
};	


void gui_setupDL(SETTINGS *s);
void gui_shutdownDL(void);
void gui_set_dictionary_page_and_key(gint page_num, gchar *key);
#ifdef __cplusplus
}
#endif
#endif	/* __GS_DICTLEX_H_ */
