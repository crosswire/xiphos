/***************************************************************************
                          gs_studypad.h  -  description
                             -------------------
    begin                : Tue Dec 12 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
 * StudyPad callbacks
******************************************************************************/
void on_btnSPnew_clicked(GtkButton * button, gpointer user_data);
void on_btnPrint_clicked(GtkButton * button, gpointer user_data);
void on_btnOpenFile_clicked(GtkButton * button, gpointer user_data);
void on_btnSaveFile_clicked(GtkButton * button, gpointer user_data);
void on_btnSaveFileAs_clicked(GtkButton * button, gpointer user_data);
void on_btnCut_clicked(GtkButton * button, gpointer user_data);
void on_btnCopy_clicked(GtkButton * button, gpointer user_data);
void on_btnPaste_clicked(GtkButton * button, gpointer user_data);
void on_text3_changed(GtkEditable * editable, gpointer user_data);


/******************************************************************************
 * StudyPad functions
******************************************************************************/		
void newSP(GtkWidget * text);

#ifdef __cplusplus
}
#endif
