/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
 /*
    * GnomeSword Bible Study Tool
    * gs_abouts.h
    * -------------------
    * Tue Mar 13 2001
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GS_ABOUTS_H_
#define __GS_ABOUTS_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

void on_btnAboutModuleOK_clicked(GtkButton * button,
				     gpointer user_data);
void on_btnAboutSwordOK_clicked(GtkButton * button,
				    gpointer user_data); 
GtkWidget *gui_create_about_modules(void);
GtkWidget *gui_create_about_sword(void);
GtkWidget *gui_create_about_gnomesword(void);
 
#ifdef __cplusplus
}
#endif

#endif /* __GS_ABOUTS_H_ */

