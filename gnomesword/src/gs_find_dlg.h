/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_find_dlg.h
     * -------------------
     * Sun Mar 10 20:17:08 2002
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
#ifndef _GS_FIND_DLG_H_
#define _GS_FIND_DLG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>


typedef struct _GSFindDialog  GSFindDialog;
struct _GSFindDialog {
	GnomeDialog *dialog;
	GtkHTML     *html;
	GtkWidget   *entry;
	GtkWidget   *backward;
	GtkWidget   *case_sensitive;

	gboolean     regular;
};

GSFindDialog * gs_find_dialog_new(GtkWidget *htmlwidget);
void gs_find_dialog_destroy(GSFindDialog *d);
void searchGS_FIND_DLG(GtkWidget *html_widget, gboolean regular, gchar *text);
void search_nextGS_FIND_DLG(GtkWidget *html_widget);

#ifdef __cplusplus
}
#endif
#endif /* _GS_FIND_DLG_H_ */
