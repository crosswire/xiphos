/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * gs_studypad.c
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>

#include "gs_studypad.h"
#include "gs_gui_cb.h"
#include "gs_gnomesword.h"
#include "gs_info_box.h"
#include "gs_file.h"
#include "printstuff.h"
#include "support.h"


/******************************************************************************
 * externals
******************************************************************************/
extern gchar *current_filename;	/* file in studypad */
extern char *homedir;
extern gboolean file_changed;	/* ??? */
extern GString *gs_clipboard; /* declared in gs_gnomesword.c, freed in gs_sword.cpp */
extern gint answer; /* do we want to save studybad file on shutdown */

/******************************************************************************
 * StudyPad callbacks
******************************************************************************/

/******************************************************************************
 *
******************************************************************************/
void on_btnSPnew_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *text;

	text = lookup_widget(GTK_WIDGET(button), "text3");
	/* send text widget to gs_gnomesword.c to start new file */
	newSP(text);
}

/******************************************************************************
 *
******************************************************************************/
void on_btnPrint_clicked(GtkButton * button, gpointer user_data)
{
	/* pass studypad text widget to print */
	file_print(GTK_WIDGET(button), current_filename, 1);
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
	GtkWidget *text;
	text = lookup_widget(GTK_WIDGET(button), "text3");
	gtk_editable_cut_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

/******************************************************************************
 *
******************************************************************************/
void on_btnCopy_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(GTK_WIDGET(button), "text3");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

/******************************************************************************
 *
******************************************************************************/
void on_btnPaste_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(GTK_WIDGET(button), "text3");
	gtk_editable_paste_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

/******************************************************************************
 *
******************************************************************************/
void on_text3_changed(GtkEditable * editable, gpointer user_data)
{
	GtkWidget *statusbar;
	gchar charbuf[255];

	statusbar = lookup_widget(GTK_WIDGET(editable), "statusbar2");
	if (current_filename) {
		sprintf(charbuf, _("%s - modified."), current_filename);
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1, charbuf);
	} else {
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1,
				   _("modified"));
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
        GtkWidget *statusbar;
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
	statusbar = lookup_widget((text),"statusbar2");
	gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, _("-untitled-"));
	file_changed = FALSE;	
}

