/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*  This file is part of the GtkHTML library.

    Copyright (C) 2000 Helix Code, Inc.
    Authors:           Radek Doulik (rodo@helixcode.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
/* 
 * added to gnomesword Fri Mar  1 12:45:48 2002
*/
#include <config.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
#include <gal/widgets/e-unicode.h>
#include "gs_editor.h"
#include "gs_editor_search.h"

struct _GSHTMLSearchDialog {
	GnomeDialog *dialog;
	GtkHTML     *html;
	GtkWidget   *entry;
	GtkWidget   *backward;
	GtkWidget   *case_sensitive;

	gboolean     regular;
};


static void
next_button_cb(GtkWidget *but, GtkHTML *html)
{
	if (html->engine->search_info)
		html_engine_search_next (html->engine);	
}

static void
search_cb(GtkWidget *but, GSHTMLSearchDialog *d)
{
	char *text;

	text = e_utf8_gtk_entry_get_text(GTK_ENTRY(d->entry));
	html_engine_search(d->html->engine, text,
			GTK_TOGGLE_BUTTON(d->case_sensitive)->active,
			GTK_TOGGLE_BUTTON(d->backward)->active == 0, d->regular);
	g_free (text);
}

static void
close_dialog(GtkWidget *but, GSHTMLSearchDialog *d)
{
	gnome_dialog_close (d->dialog);	
}

static void
entry_changed (GtkWidget *entry, GSHTMLSearchDialog *d)
{
}

static void
entry_activate (GtkWidget *entry, GSHTMLSearchDialog *d)
{
	//gnome_dialog_close (d->dialog);
	search_cb (NULL, d);
}

GSHTMLSearchDialog *
gs_editor_search_dialog_new (GtkHTML *html)
{
	GSHTMLSearchDialog *dialog = g_new (GSHTMLSearchDialog, 1);
	GtkWidget *hbox;

	dialog->dialog         = GNOME_DIALOG (gnome_dialog_new (NULL, _("Find"), _("Find Next"), GNOME_STOCK_BUTTON_CANCEL, NULL));
	dialog->entry          = gtk_entry_new_with_max_length (20);
	dialog->backward       = gtk_check_button_new_with_label (_("backward"));
	dialog->case_sensitive = gtk_check_button_new_with_label (_("case sensitive"));
	dialog->html           = html;
	dialog->regular        = FALSE;

	hbox = gtk_hbox_new (FALSE, 0);

	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->backward);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->case_sensitive);

	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), dialog->entry);
	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), hbox);
	gtk_widget_show (dialog->entry);
	gtk_widget_show_all (hbox);

	gnome_dialog_button_connect (dialog->dialog, 0, search_cb, dialog);
	gnome_dialog_button_connect (dialog->dialog, 1, next_button_cb, (GtkHTML*)html);
	gnome_dialog_button_connect (dialog->dialog, 2, close_dialog, dialog);
	gnome_dialog_close_hides (dialog->dialog, TRUE);
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
gs_editor_search_dialog_destroy (GSHTMLSearchDialog *d)
{
	g_free (d);
}

void
search (GSHTMLEditorControlData *ecd, gboolean regular, gchar *text)
{
	RUN_DIALOG (search, regular ? _("Find Regular Expression") :  _("Find"));
	
	if (ecd->search_dialog)
		ecd->search_dialog->regular = regular;

	if (ecd->search_dialog) {
		if(text)
			gtk_entry_set_text(GTK_ENTRY(ecd->search_dialog->entry),text);
		/*
		else if(html_engine_is_selection_active(ecd->html->engine)){
			gtk_html_copy(ecd->html); 
			gtk_editable_paste_clipboard(GTK_EDITABLE(GTK_ENTRY(ecd->search_dialog->entry)));
		}		
		else 
			gtk_entry_set_text(GTK_ENTRY(ecd->search_dialog->entry),"");
		*/
		gtk_widget_grab_focus (ecd->search_dialog->entry);		
	}
}

void
search_next (GSHTMLEditorControlData *ecd)
{
	if (ecd->html->engine->search_info) {
		html_engine_search_next (ecd->html->engine);
	} else {
		search (ecd, FALSE, NULL);
	}
}


