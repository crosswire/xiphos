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
//	void appendSibblingSW_GBS(TreeKeyIdx *treeKey, gchar *text);
//	void appendChildSW_GBS(TreeKeyIdx *treeKey, gchar *text);
	void addchildSW_GBS(SETTINGS *s, gchar *name);
	void addSiblingSW_GBS(SETTINGS *s, gchar *name);
	void setupSW_GBS(SETTINGS *s);
	void shutdownSW_GBS(void);
	void loadBookListSW_GBS(SETTINGS *s);
	void savebookSW_GBS(gchar *buf);
	gint deleteNodeSW_GBS(SETTINGS *s);
	void addnewbookSW_GBS(SETTINGS *s, gchar *bookName, gchar *filename);
	void on_ctreeBooks_select_row(GtkCList * clist,
		    	gint row,
		    	gint column, 
			GdkEvent * event, 
			SETTINGS *s);
	void load_book_tree(SETTINGS *s,
			GtkCTreeNode *node, 
			gchar *bookName, 
			gchar *treekey,
			unsigned long offset);


#ifdef __cplusplus
}
#endif
#endif				/* __SW_GBS_H_ */
