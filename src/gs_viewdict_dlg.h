/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_viewdict.h
                             -------------------
    begin                : Fri Jan 05 2001
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

GtkWidget *create_dlgViewDict(GtkWidget *app);
void initSD(gchar *modName);

/*
 * callbacks
 */
void
on_btnClose_clicked                    (GtkButton       *button,
                                        gpointer         user_data);
void
on_btnVDSync_clicked                     (GtkButton       *button,
                                        gpointer         user_data);
void
on_dlgShowDict_destroy                 (GtkObject       *object,
                                        gpointer         user_data);

void
on_ceSDMods_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_entrySDLookup_changed               (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_entrySDLookup_key_release_event     (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

void
on_btnSDClose_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_clKeys_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);


#ifdef __cplusplus
}
#endif

