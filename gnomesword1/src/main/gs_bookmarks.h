/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_bookmarks.h
    * -------------------
    * Thu July 05 2001
    * copyright (C) 2001 by tbiggs
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

#ifndef __GS_BOOKMARKS_H_
#define __GS_BOOKMARKS_H_

#ifdef __cplusplus
extern "C" {
#endif
//#include "sword.h"
#include "gs_gnomesword.h"

	typedef struct _bmtree BM_TREE;
	struct _bmtree {
		GtkWidget *ctree_widget;
		GtkCTree *ctree;
	};
	
	void add_bookmark_to_tree(GtkCTreeNode * node,
			   gchar * modName, gchar * verse);
	void loadtree(SETTINGS * s);
	GtkWidget *create_pmBookmarkTree(void);
	void
	 on_new_subgroup_activate(GtkMenuItem * menuitem,
				  gpointer user_data);
	void
	 on_add_new_group1_activate(GtkMenuItem * menuitem,
				    gpointer user_data);
	void
	 on_save_bookmarks1_activate(GtkMenuItem * menuitem,
				     gpointer user_data);
	void
	 on_edit_item_activate(GtkMenuItem * menuitem,
			       gpointer user_data);
	void
	 on_delete_item_activate(GtkMenuItem * menuitem,
				 gpointer user_data);
	void
	 on_allow_reordering_activate(GtkMenuItem * menuitem,
				      gpointer user_data);
	void
	 addverselistBM(SETTINGS * s, GList * list);
	// gint loadoldbookmarks(void);
	GtkWidget *create_dlgEditBookMark(gchar * text[3],
					  gboolean newbookmark);
	void
	 create_addBookmarkMenuBM(GtkWidget * menu,
				  GtkWidget * bookmark_menu_widget,
				  SETTINGS *);


#ifdef __cplusplus
}
#endif
#endif				/* __GS_BOOKMARKS_H_ */
