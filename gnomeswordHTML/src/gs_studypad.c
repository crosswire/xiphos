/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          gs_studypad.c  -  description
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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>

#include "callback.h"
#include "support.h"
#include "gs_gnomesword.h"
#include "interface.h"

#if USE_GNOMEPRINT
#include "printstuff.h"
#endif /* USE_GNOMEPRINT */

#include "gs_file.h"
#include "gs_studypad.h"


/******************************************************************************
 * externals
******************************************************************************/
extern gchar *current_filename;	/* file in studypad */
extern char *homedir;
extern gboolean file_changed;	/* ??? */
extern GString *gs_clipboard; /* declared in gs_gnomesword.c, freed in gs_sword.cpp */
extern gint answer; /* do we want to save studybad file on shutdown */
extern GtkWidget *text3;
extern GtkWidget *statusbar2;

/******************************************************************************
 * StudyPad callbacks
******************************************************************************/

/******************************************************************************
 *
******************************************************************************/
void on_btnSPnew_clicked(GtkButton * button, gpointer user_data)
{
	/* send text widget to gs_gnomesword.c to start new file */
	newSP(text3);
}

/******************************************************************************
 *
******************************************************************************/
void on_btnPrint_clicked(GtkButton * button, gpointer user_data)
{
#if USE_GNOMEPRINT
	/* pass studypad text widget to print */
	file_print(GTK_WIDGET(button), current_filename, 1);
#endif /* USE_GNOMEPRINT */
}

/******************************************************************************
 *
******************************************************************************/
void on_btnOpenFile_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *openFile;
	gchar *msg, buf[255];
	GtkWidget *msgbox;
	
	/* if study pad file has changed let's ask about saving it */
	if(file_changed){ 	
		msg = g_strdup_printf(_("``%s'' has been modified.  Do you wish to save it?"), current_filename);
		msgbox = create_InfoBox();
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
		g_free (msg);
		switch (answer){		
			case 0: saveFile(current_filename);
				break;
			default:
				break;
		}
	}   	
	sprintf(buf,"%s/*.pad",homedir);
	openFile = create_fileselection1();
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(openFile),
					buf);
	gtk_widget_show(openFile);
}

/******************************************************************************
 *
******************************************************************************/
void on_btnSaveFile_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *savemyFile, *ok_button2;
	gchar buf[255];

	sprintf(buf,"%s/.pad",homedir);	
	if (current_filename) {
		saveFile(current_filename);
		return;
	} else {
		savemyFile = create_fileselectionSave();
		gtk_file_selection_set_filename(GTK_FILE_SELECTION
						(savemyFile),
						buf);
		ok_button2 = lookup_widget(savemyFile, "ok_button2");
		gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
				   GTK_SIGNAL_FUNC(on_ok_button2_clicked),
				   NULL);
		gtk_widget_show(savemyFile);
	}
}

/******************************************************************************
 *
******************************************************************************/
void on_btnSaveFileAs_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *savemyFile, *ok_button2;
        gchar buf[255];

	sprintf(buf,"%s/.pad",homedir);
	savemyFile = create_fileselectionSave();
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(savemyFile),
					buf);
	ok_button2 = lookup_widget(savemyFile, "ok_button2");
	gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button2_clicked), NULL);
	gtk_widget_show(savemyFile);
}

/******************************************************************************
 *
******************************************************************************/
void on_btnCut_clicked(GtkButton * button, gpointer user_data)
{
	gtk_editable_cut_clipboard(GTK_EDITABLE(GTK_TEXT(text3)));
}

/******************************************************************************
 *
******************************************************************************/
void on_btnCopy_clicked(GtkButton * button, gpointer user_data)
{
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text3)));
}

/******************************************************************************
 *
******************************************************************************/
void on_btnPaste_clicked(GtkButton * button, gpointer user_data)
{
	gtk_text_insert(GTK_TEXT(text3), NULL, //&gtkText->style->black
			NULL, NULL,
			gs_clipboard->str, -1);
}

/******************************************************************************
 *
******************************************************************************/
void on_text3_changed(GtkEditable * editable, gpointer user_data)
{
	//GtkWidget *statusbar;
	gchar charbuf[255];

	
	if (current_filename) {
		sprintf(charbuf, "%s - modified.", current_filename);
		gtk_statusbar_push(GTK_STATUSBAR(statusbar2), 1, charbuf);
	} else {
		gtk_statusbar_push(GTK_STATUSBAR(statusbar2), 1,
				   "modified");
	}
	file_changed = TRUE;
}

/******************************************************************************
 * StudyPad functions
******************************************************************************/

/*****************************************************************************
 *newSP - start new file in studypad
 *text - studypad text widget
*****************************************************************************/
void newSP(GtkWidget *text) 
{
	GtkWidget *msgbox;
	gchar *msg;
	
	/* if study pad file has changed let's ask about saving it */
	if(file_changed){ 	
		msg = g_strdup_printf(_("``%s'' has been modified.  Do you wish to save it?"), current_filename);
		msgbox = create_InfoBox();
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
		g_free (msg);
		switch (answer){		
			case 0: saveFile(current_filename);
				break;
			default:
				break;
		}
	}   	
        current_filename = NULL;
        gtk_text_set_point(GTK_TEXT(text), 0);
	gtk_text_forward_delete (GTK_TEXT (text), gtk_text_get_length((GTK_TEXT(text))));
	gtk_statusbar_push (GTK_STATUSBAR (statusbar2), 1, "-untitled-");
	file_changed = FALSE;	
}

