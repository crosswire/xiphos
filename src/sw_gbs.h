/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * sw_gbs.h
     * -------------------
     * Mon Jan 28 17:21:01 2002
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

#ifndef __SW_GBS_H_
#define __SW_GBS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gs_gnomesword.h"
	
typedef struct _GSFindDialog  GSFindDialog;
struct _GSFindDialog {
	GnomeDialog *dialog;
	GtkWidget   *htmlwidget;
	GtkWidget   *entry;
	GtkWidget   *backward;
	GtkWidget   *case_sensitive;

	gboolean     regular;
};

typedef struct _gbsdata GBS_DATA;
struct _gbsdata {
	GtkWidget *html;
	GtkWidget *ctree;
	gchar *bookName;
	gchar *bookDescription;
	gchar *searchstring;
	GSFindDialog *find_dialog;
};	


typedef struct _nodedata NODEDATA;
struct _nodedata {
	GtkCTreeNode *parent;
	GtkCTreeNode *sibling;
	gchar *buf[3];
	GdkPixmap *pixmap1;
	GdkPixmap *pixmap2;
	GdkBitmap *mask1;
	GdkBitmap *mask2;
	gboolean is_leaf;
	gboolean expanded;
};

	void setupSW_GBS(SETTINGS *s);
	void shutdownSW_GBS(void);
	void load_book_tree(SETTINGS *s,
			GtkCTreeNode *node, 
			gchar *bookName, 
			gchar *treekey,
			unsigned long offset);
	void displayinGBS(gchar *key);

#ifdef __cplusplus
}
#endif
#endif				/* __SW_GBS_H_ */
