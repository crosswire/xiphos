
/*
    GnomeSword Bible Study Tool
    *  gbs.c - sword lib support for general book modules
    *
    *  Copyright (C) 2000,2001,2002 GnomeSword Developer Team
    *
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

	gboolean backend_treekey_next_sibling(gchar *bookname, 
			unsigned long offset);
	gboolean backend_treekeyHasChildrenGBS(gchar *bookname, 
			unsigned long offset);
	char *backend_treekeyGetLocalNameGBS(gchar *bookname, 
			unsigned long offset);
	unsigned long backend_treekeyGetOffsetGBS(gchar *bookname);
	void backend_newDisplayGBS(GtkWidget *html, 
			char *bookName, 
			SETTINGS *s);
	gboolean backend_treekeyFirstChildGBS(char *bookname,  
			unsigned long offset);
	gboolean backend_displayRowGBS(gint booknum,
			gchar *offset);
	void backend_setupGBS(SETTINGS *s);
	void backend_shutdownGBS(void);
	//void displayinGBS(gchar *key);

#ifdef __cplusplus
}
#endif
#endif				/* __SW_GBS_H_ */
