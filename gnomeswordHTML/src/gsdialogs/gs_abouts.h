/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_abouts.h
                             -------------------
    begin                : Tue Mar 13 2001
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
#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

void on_btnAboutModuleOK_clicked(GtkButton * button,
				     gpointer user_data);
void on_btnAboutSwordOK_clicked(GtkButton * button,
				    gpointer user_data);
 
GtkWidget *create_about2(void);
GtkWidget *create_AboutSword(void);
GtkWidget *create_aboutmodules(gchar *buf);
 
 
#ifdef __cplusplus
}
#endif

