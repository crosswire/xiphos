/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_gbs.h
     * -------------------
     * Wed Feb 20 11:26:11 2002
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

#ifndef __GS_GBS_H__
#define __GS_GBS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "gs_gnomesword.h"
#include "gs_editor.h"
	
	
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
	void addbooktoCTreeGS_GBS(SETTINGS *s, GList *books);
	GtkWidget *setupGBS(SETTINGS *s, GList *books);
	void setitemssensitivityGS_GBS(GSHTMLEditorControlData *gbsecd, 
			gint iswritable);
	GtkWidget* create_pmGBS (void);
	void setnodeinfoGS_GBS(SETTINGS *s, NODEDATA *data);
	GtkCTreeNode *addnodeGS_GBS(SETTINGS *s, NODEDATA *data);
	

#ifdef __cplusplus
}
#endif
#endif				/* __GS_GBS_H__ */

