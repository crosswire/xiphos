/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                                 sw_verselist_sb.h
                             -------------------
    begin                : Thur Aug 02 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/

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
#ifndef __SW_VERSELIST_SB_H__
#define __SW_VERSELIST_SB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>	
#include "gs_gnomesword.h"

gboolean getVerseListSBSWORD(gchar *modName, 
					gchar *vlist, 
					SETTINGS *s);
void setupVerseListSBSWORD(GtkWidget *html_widget);
void shutdownverselistSBSWORD(void);
void changeVerseListSBSWORD(gchar *url);
	

#ifdef __cplusplus
}
#endif
#endif /* __SW_VERSELIST_SB_H__ */

