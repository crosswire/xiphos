/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_preferences_dlg.h
    * -------------------
    * Fri July 27 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
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
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */
  
#ifndef __GS_PREFERENCES_DLG_H_
#define __GS_PREFERENCES_DLG_H_ 

#ifdef __cplusplus
extern "C" {
#endif
#include "gs_gnomesword.h"

GtkWidget *create_dlgSettings(SETTINGS *s,
				GList *biblelist,
				GList *commlist,
				GList *dictlist,
				GList *percomlist,
				GList *devotionlist);

#ifdef __cplusplus
}
#endif

#endif /* __GS_PREFERENCES_DLG_H_ */

