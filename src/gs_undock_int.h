/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_interlinear_dlg.h
     * -------------------
     * Thu Feb  7 21:57:05 2002
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

#ifndef __GS_INTERLINEAR_DLG_H__
#define __GS_INTERLINEAR_DLG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "gs_gnomesword.h"

void undock_interlinear_page(SETTINGS *s);
void on_btnDockInt_clicked(GtkButton *button, SETTINGS *s);
GtkWidget *create_dlgInterlinear (SETTINGS *s);

#ifdef __cplusplus
}
#endif
#endif				/* __GS_INTERLINEAR_DLG_H__ */
