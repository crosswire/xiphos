/*
 * GnomeSword Bible Study Tool
 * studypad_dialog.c - dialog for detached editors
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
//#include <gtkhtml/gtkhtml.h>

#include "gui/studypad_dialog.h"
#include "gui/studypad.h"
#include "gui/utilities.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/settings.h"

/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include "studypad_dialog.h"
 *
 *   void dialog_destroy(GtkObject *object, DL_DIALOG * dlg)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void dialog_destroy (GtkObject *object, gpointer data)
{	
	
	//gui_studypad_can_close();
	widgets.studypad_dialog = NULL;
	settings.studypad_dialog_exist = FALSE;	
}

/******************************************************************************
 * Name
 *   gui_attach_detach_studypad
 *
 * Synopsis
 *   #include "studypad_dialog.h"
 *   
 *   void gui_attach_detach_studypad(void)
 *
 * Description
 *   dock/undock studypad
 *   
 *
 * Return value
 *   
 */

void gui_attach_detach_studypad(void)
{
	
}



static GtkWidget *create_dialog(void)
{
	GtkWidget *dialog;
	dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(dialog),"dialog",dialog);
	/*gtk_widget_set_usize(dialog, 535, 280); */
	gtk_widget_set_size_request(dialog, 590, 300);
	gtk_window_set_policy(GTK_WINDOW
			      (dialog),
			      TRUE, TRUE, FALSE);
	gtk_window_set_title((GtkWindow *)dialog,
                                (const gchar*)N_("StudyPad"));
	gtk_widget_show(dialog);
	g_signal_connect(G_OBJECT(dialog), "destroy",
                      G_CALLBACK(dialog_destroy),
			   NULL);	 
	
	return dialog;	
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "studypad_dialog.h"
 *
 *   	
 *
 * Description
 *    
 *
 * Return value
 *   
 */

gint gui_open_studypad_dialog(gchar * file_name)
{
	widgets.studypad_dialog = create_dialog();

	widgets.html_studypad =
	  	gui_create_studypad_control(widgets.studypad_dialog,
							  file_name);
	/*g_signal_connect_object(G_OBJECT(widgets.studypad_dialog),
                                             "destroy",
                                             G_CALLBACK(dialog_destroy),
                                             NULL,
                                             0);*/
	return TRUE;
}
