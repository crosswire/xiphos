/*
 * GnomeSword Bible Study Tool
 * gs_gui_cb.c - SHORT DESCRIPTION
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
#include <config.h>
#endif

#include <gnome.h>
#include <gal/e-paned/e-hpaned.h>

#include "gs_gui_cb.h"
#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_popup_cb.h"
#include "sword.h"
#include "gs_history.h"
#include "support.h"
#include "settings.h"

extern gboolean ApplyChange;
extern gboolean 
 havebible, 
 havecomm, 
 havedict, 
 havebook, 
 havepercomm;
 
void
on_mainwindow_size_allocate(GtkWidget * widget,
			    GtkAllocation * allocation,
			    gpointer user_data)
{
	settings.gs_width = allocation->width;
	settings.gs_hight = allocation->height;
}

void on_mainwindow_destroy(GtkObject * object, SETTINGS * s)
{
	backend_shutdown(s);
	gnomesword_shutdown(s);
	gtk_exit(0);
}

void on_btn_search_clicked(GtkButton * button, gpointer user_data)
{
	SETTINGS *s;

	s = (SETTINGS *) user_data;
	showSBGroup(s, s->searchbargroup);
}

void on_cbeBook_changed(GtkEditable * editable, gpointer user_data)
{
	if (ApplyChange) {
		gchar buf[256];
		gchar *bookname =
		    gtk_entry_get_text(GTK_ENTRY
				       (settings.cbeBook));
		sprintf(buf,"%s 1:1",bookname);
		change_verse(buf);
	} 
}

gboolean
on_spbChapter_button_release_event(GtkWidget * widget,
				   GdkEventButton * event,
				   gpointer user_data)
{
	if (ApplyChange) { 
		gchar *bookname;
		gchar buf[256];
		gint chapter, verse;
	
		bookname =
		    gtk_entry_get_text(GTK_ENTRY
				       (settings.cbeBook));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (settings.spbChapter));
		verse =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (settings.spbVerse));
		sprintf(buf,"%s %d:%d",bookname,chapter,verse);
		change_verse(buf);		
		return TRUE;
	} 
	return FALSE;
}

gboolean
on_spbVerse_button_release_event(GtkWidget * widget,
				 GdkEventButton * event,
				 gpointer user_data)
{
	if (ApplyChange) { 
		gchar *bookname, buf[256];
		gint chapter, verse;
	
		bookname =
		    gtk_entry_get_text(GTK_ENTRY
				       (settings.cbeBook));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (settings.spbChapter));
		verse =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (settings.spbVerse));
		sprintf(buf,"%s %d:%d",bookname,chapter,verse);
		change_verse(buf);
		return TRUE;
	} 
	return FALSE;
}

void on_btnLookup_clicked(GtkButton * button, gpointer user_data)
{
	gchar *buf;

	buf =
	    gtk_entry_get_text(GTK_ENTRY
			       (settings.cbeFreeformLookup));
	change_verse(buf);	//-- change verse to entry text 
}

gboolean
on_cbeFreeformLookup_key_press_event(GtkWidget * widget,
				     GdkEventKey * event,
				     gpointer user_data)
{
	gchar *buf;

	buf = gtk_entry_get_text(GTK_ENTRY(widget));
	if (event->keyval == 65293 || event->keyval == 65421) {	/* enter key */
		change_verse(buf);	
		return TRUE;
	}
	return FALSE;
}

void
on_notebook3_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	static gboolean firsttime = TRUE;	//-- dont do anything if this is the first time here, but remember we were here - set firsttime to FALSE
	if (!firsttime) {
		changepagenotebook(notebook, page_num);	//-- send to changepagenotebook() function in GnomeSword.cpp
	}
	firsttime = FALSE;	//-- remember we were here
	
	if(havepercomm)
		gtk_widget_hide(settings.toolbarComments);
	gtk_widget_hide(settings.toolbarStudypad);

	if (page_num == 1 && settings.editnote) {
		gtk_widget_show(settings.toolbarComments);
	}

	else if (page_num == 2) {
		gtk_widget_show(settings.toolbarStudypad);
	}
}

void on_btnBack_clicked(GtkButton * button, gpointer user_data)
{
	historynav(settings.app, 0);
}

void on_btnFoward_clicked(GtkButton * button, gpointer user_data)
{
	historynav(settings.app, 1);
}

gboolean
on_epaned_button_release_event(GtkWidget * widget,
			       GdkEventButton * event,
			       gpointer user_data)
{
	gint panesize;
	if(!strcmp((gchar*)user_data,"epaned")) 
		panesize = 
		    e_paned_get_position(E_PANED(settings.epaned));
	else
		panesize =
		    e_paned_get_position(E_PANED(lookup_widget
				  (settings.app,
				   (gchar *) user_data)));

	if (panesize > 15) {
		if (!strcmp((gchar *) user_data, "epaned"))
			settings.shortcutbar_width = panesize;
		if (!strcmp((gchar *) user_data, "vpaned1"))
			settings.upperpane_hight = panesize;
		if (!strcmp((gchar *) user_data, "hpaned1"))
			settings.biblepane_width = panesize;
	}
	return TRUE;
}
