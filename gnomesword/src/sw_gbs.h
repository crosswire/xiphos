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

	void on_ctreeBooks_select_row(GtkCList * clist,
		    	gint row,
		    	gint column, 
			GdkEvent * event, 
			gpointer user_data);
	void load_book_tree(GtkWidget *ctree, 
			GtkCTreeNode *node, 
			gchar *bookName, 
			gchar *treekey,
			unsigned long offset);


#ifdef __cplusplus
}
#endif
#endif				/* __SW_GBS_H_ */
