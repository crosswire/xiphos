/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_viewtext_sw.h
                             -------------------
    begin                : Tue June 12 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sf.net
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
 
 #ifndef __gs_viewtext_sw_H__
#define __gs_viewtext_sw_H__

#ifdef __cplusplus
extern "C" {
#endif



#include <gnome.h>
	
	
GList* VTsetupSWORD(GtkWidget *text, GtkWidget *cbBook);
void VTshutdownSWORD(void);
void VTgotoverseSWORD(gchar *newkey);
void VTloadmodSWORD(gchar *modName);
gchar* VTgetmodDescriptionSWORD(void);
void VTchangeverseSWORD(gchar * verse);
gchar *VTgetbookSWORD(void);
gint VTgetchapterSWORD(void);
gint VTgetverseSWORD(void);
void VTsetGlobalOptionsSWORD(gchar *option, gchar *onoff);
	
	
#ifdef __cplusplus
}
#endif
	
#endif /* __gs_viewtext_sw_H__ */	

