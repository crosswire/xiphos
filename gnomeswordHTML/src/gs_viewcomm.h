/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_viewcomm.h
                             -------------------
    begin                : Sat Mar 24 2001
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
	void
	 on_btnVCClose_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnSync_clicked(GtkButton * button, gpointer user_data);

	void
	 on_cbEntry_changed(GtkEditable * editable, gpointer user_data);

	void
	 on_btnGoto_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnPrev_clicked(GtkButton * button, gpointer user_data);

	void
	 on_btnNext_clicked(GtkButton * button, gpointer user_data);

	GtkWidget *create_dlgViewComm(GtkWidget *app);

#ifdef __cplusplus
}
#endif
