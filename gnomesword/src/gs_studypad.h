/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
  /*
     * GnomeSword Bible Study Tool
     * gs_studypad.h
     * -------------------
     * Tue Dec 12 2000
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

#ifndef __GS_STUDYPAD_H_
#define __GS_STUDYPAD_H_

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
 * StudyPad callbacks
******************************************************************************/
	void on_btnSPnew_clicked(GtkButton * button, 
					gpointer user_data);
	void on_btnPrint_clicked(GtkButton * button, 
					gpointer user_data);
	void on_btnOpenFile_clicked(GtkButton * button,
				    	gpointer user_data);
	void on_btnSaveFile_clicked(GtkButton * button,
				    	gpointer user_data);
	void on_btnSaveFileAs_clicked(GtkButton * button,
				      	gpointer user_data);
	void on_btnCut_clicked(GtkButton * button, 
					gpointer user_data);
	void on_btnCopy_clicked(GtkButton * button, 
					gpointer user_data);
	void on_btnPaste_clicked(GtkButton * button, 
					gpointer user_data);
	void on_text3_changed(GtkEditable * editable, 
					gpointer user_data);

/******************************************************************************
 * StudyPad functions
******************************************************************************/
	void newSP(GtkWidget * text);

#ifdef __cplusplus
}
#endif
#endif				/* __GS_STUDYPAD_H_ */
