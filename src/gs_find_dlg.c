/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_find_dlg.c
    * -------------------
    * Sun Mar 10 20:21:17 2002
    * copyright (C) 2001 by tbiggs
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
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include "gs_find_dlg.h"


static void
next_button_cb(GtkWidget *but, GtkHTML *html)
{
	if (html->engine->search_info)
		html_engine_search_next (html->engine);	
}

static void
search_cb(GtkWidget *but, GSFindDialog *d)
{
	char *text;

	text = e_utf8_gtk_entry_get_text(GTK_ENTRY(d->entry));
	html_engine_search(d->html->engine, text,
			GTK_TOGGLE_BUTTON(d->case_sensitive)->active,
			GTK_TOGGLE_BUTTON(d->backward)->active == 0, d->regular);
	g_free (text);
}

static void
close_dialog(GtkWidget *but, GSFindDialog *d)
{
	gnome_dialog_close(d->dialog);	
	gs_find_dialog_destroy(d);
}

static void
entry_changed (GtkWidget *entry, GSFindDialog *d)
{
}

static void
entry_activate (GtkWidget *entry, GSFindDialog *d)
{
	search_cb (NULL, d);
}

GSFindDialog *
gs_find_dialog_new (GtkWidget *htmlwidget)
{
	GSFindDialog *dialog = g_new (GSFindDialog, 1);
	GtkWidget *hbox;
	
	dialog->dialog         = GNOME_DIALOG (gnome_dialog_new (NULL, _("Find"), _("Find Next"), GNOME_STOCK_BUTTON_CANCEL, NULL));
	dialog->entry          = gtk_entry_new_with_max_length (20);
	dialog->backward       = gtk_check_button_new_with_label (_("backward"));
	dialog->case_sensitive = gtk_check_button_new_with_label (_("case sensitive"));
	dialog->html           = GTK_HTML(htmlwidget);
	dialog->regular        = FALSE;

	hbox = gtk_hbox_new (FALSE, 0);

	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->backward);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->case_sensitive);

	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), dialog->entry);
	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), hbox);
	gtk_widget_show (dialog->entry);
	gtk_widget_show_all (hbox);

	gnome_dialog_button_connect (dialog->dialog, 0, search_cb, dialog);
	gnome_dialog_button_connect (dialog->dialog, 1, next_button_cb, (GtkHTML*)dialog->html);
	gnome_dialog_button_connect (dialog->dialog, 2, close_dialog, dialog);
	gnome_dialog_set_close (dialog->dialog, FALSE);

	gnome_dialog_set_default (dialog->dialog, 0);
	gtk_widget_grab_focus (dialog->entry);

	gtk_signal_connect (GTK_OBJECT (dialog->entry), "changed",
			    entry_changed, dialog);
	gtk_signal_connect (GTK_OBJECT (dialog->entry), "activate",
			    entry_activate, dialog);

	return dialog;
}

void
gs_find_dialog_destroy(GSFindDialog *d)
{
	g_free (d);
}

void
searchGS_FIND_DLG(GtkWidget *html_widget, gboolean regular, gchar *text)
{
	GSFindDialog *dialog;
	
	dialog = gs_find_dialog_new (html_widget);
	
	if (dialog)
		dialog->regular = regular;

	if (dialog) {
		if(text)
			gtk_entry_set_text(GTK_ENTRY(dialog->entry),text);
		
		gtk_widget_grab_focus(dialog->entry);
		gtk_widget_show(GTK_WIDGET(dialog->dialog));
		gdk_window_raise(GTK_WIDGET(dialog->dialog)->window);		
	}
}

void
search_nextGS_FIND_DLG(GtkWidget *html_widget)
{
	GtkHTML *html;
	html = GTK_HTML(html_widget);
	if (html->engine->search_info) {
		html_engine_search_next (html->engine);
	} else {
		searchGS_FIND_DLG(html_widget, FALSE, NULL);
	}
}


/*** end ***/

