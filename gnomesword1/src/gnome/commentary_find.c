/*
 * GnomeSword Bible Study Tool
 * commentary_find.c 
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
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
#include <gal/widgets/e-unicode.h>

#include "gui/commentary_find.h"
#include "main/settings.h"

typedef GnomeDialog ** (*DialogCtor)(GtkWidget *htmlwidget);
#define FIND_DIALOG(name,title) comm_find_dialog ((GnomeDialog ***)&c-> name ## _dialog, c->html, (DialogCtor) comm_ ## name ## _dialog_new, title)

gboolean comm_find_running;

static void comm_find_dialog(GnomeDialog ***dialog, GtkWidget *html, 
				DialogCtor ctor, const gchar *title);


/******************************************************************************
 * Name
 *  next_button_cb
 *
 * Synopsis
 *   #include "commentary_find_dialog.h"
 *
 *   void next_button_cb(GtkWidget *but,  GtkWidget *htmlwidget)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void next_button_cb(GtkWidget *but,  GtkWidget *htmlwidget)
{
	GtkHTML *html;
	
	html = GTK_HTML(htmlwidget);
	if (html->engine->search_info)
		html_engine_search_next (html->engine);	
}

/******************************************************************************
 * Name
 *   search_cb
 *
 * Synopsis
 *   #include "commentary_find_dialog.h"
 *
 *   void search_cb(GtkWidget *but, COMMFindDialog *d)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void search_cb(GtkWidget *but, COMMFindDialog *d)
{
	char *text;
	GtkHTML *html;
	
	html = GTK_HTML(d->htmlwidget);
	text = e_utf8_gtk_entry_get_text(GTK_ENTRY(d->entry));
	html_engine_search(html->engine, text,
			GTK_TOGGLE_BUTTON(d->case_sensitive)->active,
			GTK_TOGGLE_BUTTON(d->backward)->active == 0, d->regular);
	sprintf(settings.findText,"%s",text);
	g_free (text);
}

/******************************************************************************
 * Name
 *   comm_find_close_dialog
 *
 * Synopsis
 *   #include "commentary_find_dialog.h"
 *
 *   void comm_find_close_dialog(GtkWidget *but, COMMFindDialog *d)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void comm_find_close_dialog(GtkWidget *but, COMMFindDialog *d)
{
	comm_find_running = FALSE;
	gnome_dialog_close(d->dialog);
}

/******************************************************************************
 * Name
 *   entry_activate
 *
 * Synopsis
 *   #include "commentary_find_dialog.h"
 *
 *   void entry_activate (GtkWidget *entry, COMMFindDialog *d)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void entry_activate (GtkWidget *entry, COMMFindDialog *d)
{
	search_cb (NULL, d);
}

/******************************************************************************
 * Name
 *   comm_find_dialog_new
 *
 * Synopsis
 *   #include "commentary_find_dialog.h"
 *
 *   COMMFindDialog *comm_find_dialog_new(GtkWidget *htmlwidget)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static COMMFindDialog *comm_find_dialog_new(GtkWidget *htmlwidget)
{
	COMMFindDialog *dialog = g_new (COMMFindDialog, 1); /* must be freed by calling module */
	GtkWidget *hbox;
	
	dialog->dialog         = GNOME_DIALOG (gnome_dialog_new (NULL, _("Find"), _("Find Next"), GNOME_STOCK_BUTTON_CANCEL, NULL));
	dialog->entry          = gtk_entry_new_with_max_length (20);
	dialog->backward       = gtk_check_button_new_with_label (_("backward"));
	dialog->case_sensitive = gtk_check_button_new_with_label (_("case sensitive"));
	dialog->htmlwidget     = htmlwidget;
	dialog->regular        = FALSE;

	hbox = gtk_hbox_new (FALSE, 0);

	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->backward);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->case_sensitive);

	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), dialog->entry);
	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), hbox);
	gtk_widget_show (dialog->entry);
	gtk_widget_show_all (hbox);

	gnome_dialog_button_connect (dialog->dialog, 0, search_cb, dialog);
	gnome_dialog_button_connect (dialog->dialog, 1, next_button_cb, dialog->htmlwidget);
	gnome_dialog_button_connect (dialog->dialog, 2, comm_find_close_dialog, dialog);
	gnome_dialog_close_hides (dialog->dialog, TRUE);	
	gnome_dialog_set_close (dialog->dialog, FALSE);

	gnome_dialog_set_default (dialog->dialog, 0);
	gtk_widget_grab_focus (dialog->entry);
	
	gtk_signal_connect (GTK_OBJECT (dialog->entry), "activate",
			    entry_activate, dialog);

	return dialog;
}

/******************************************************************************
 * Name
 *   search_comm_find_dlg
 *
 * Synopsis
 *   #include "commentary_find_dialog.h"
 *
 *   void search_comm_find_dlg(COMM_DATA *c, gboolean regular, gchar *text)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void search_comm_find_dlg(COMM_DATA *c, gboolean regular, gchar *text)
{
	gchar buf[256];
	
	sprintf(buf,"%s in %s", _("Find"), c->mod_name);
	
	FIND_DIALOG(find, buf);
	
	if (c->find_dialog)
		c->find_dialog->regular = regular;

	if (c->find_dialog) {
		if(text)
			gtk_entry_set_text(GTK_ENTRY(c->find_dialog->entry),text);
		
		gtk_widget_grab_focus(c->find_dialog->entry);
		comm_find_running = TRUE;
		//gdk_window_raise(GTK_WIDGET(g->find_dialog->dialog)->window);		
	}
}

/******************************************************************************
 * Name
 *  comm_find_dialog
 *
 * Synopsis
 *   #include "commentary_find_dialog.h"
 *
 *   void comm_find_dialog(GnomeDialog ***dialog, GtkWidget *html, 
 *				DialogCtor ctor, const gchar *title)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void comm_find_dialog(GnomeDialog ***dialog, GtkWidget *html, 
				DialogCtor ctor, const gchar *title)
{
	if (*dialog) {
		gtk_window_set_title (GTK_WINDOW (**dialog), title);
		gtk_widget_show (GTK_WIDGET (**dialog));
		gdk_window_raise (GTK_WIDGET (**dialog)->window);
	} else {
		*dialog = ctor (html);
		gtk_window_set_title (GTK_WINDOW (**dialog), title);
		gtk_widget_show (GTK_WIDGET (**dialog));
	}
}

/*** end of file ***/

