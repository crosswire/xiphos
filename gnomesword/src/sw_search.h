/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_search.h
     * -------------------
     * Wed Aug 02 2001
     * copyright (C) 2001 by Terry Biggs
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

#ifndef __SW_SEARCH_H__
#define __SW_SEARCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include "gs_gnomesword.h"
	/* search Bible text or commentaries */
	GList * searchSWORD(GtkWidget * widget, SETTINGS * s);
	void setupsearchresultsSBSW(GtkWidget * html_widget);
	void shutdownsearchresultsSBSW(void);
	void changesearchresultsSBSW(SETTINGS * s, gchar * url);

#ifdef __cplusplus
}
#endif
#endif				/* __SW_SEARCH_H__ */
