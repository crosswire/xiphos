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

#include <config.h>
#include "search.h"
#include "dialog.h"
#include "htmlengine-search.h"

struct _GtkHTMLSearchDialog {
	GnomeDialog *dialog;
	GtkHTML     *html;
	GtkWidget   *entry;
	GtkWidget   *backward;
	GtkWidget   *case_sensitive;

	gboolean     regular;
};

static void
search_cb (GtkWidget *but, GtkHTMLSearchDialog *d)
{
	html_engine_search (d->html->engine, gtk_entry_get_text (GTK_ENTRY (d->entry)),
			    GTK_TOGGLE_BUTTON (d->case_sensitive)->active,
			    GTK_TOGGLE_BUTTON (d->backward)->active == 0, d->regular);
}

static void
entry_changed (GtkWidget *entry, GtkHTMLSearchDialog *d)
{
}

static void
entry_activate (GtkWidget *entry, GtkHTMLSearchDialog *d)
{
	gnome_dialog_close (d->dialog);
	search_cb (NULL, d);
}

GtkHTMLSearchDialog *
gtk_html_search_dialog_new (GtkHTML *html, gboolean regular)
{
	GtkHTMLSearchDialog *dialog = g_new (GtkHTMLSearchDialog, 1);
	GtkWidget *hbox;

	dialog->dialog         = GNOME_DIALOG (gnome_dialog_new ((regular) ? _("Regex find") :  _("Find"), _("Find"),
								 GNOME_STOCK_BUTTON_CANCEL, NULL));
	dialog->entry          = gtk_entry_new_with_max_length (20);
	dialog->backward       = gtk_check_button_new_with_label (_("backward"));
	dialog->case_sensitive = gtk_check_button_new_with_label (_("case sensitive"));
	dialog->html           = html;
	dialog->regular        = regular;

	hbox = gtk_hbox_new (FALSE, 0);

	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->backward);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->case_sensitive);

	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), dialog->entry);
	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), hbox);
	gtk_widget_show (dialog->entry);
	gtk_widget_show_all (hbox);

	gnome_dialog_button_connect (dialog->dialog, 0, search_cb, dialog);
	gnome_dialog_close_hides (dialog->dialog, TRUE);
	gnome_dialog_set_close (dialog->dialog, TRUE);

	gnome_dialog_set_default (dialog->dialog, 0);
	gtk_widget_grab_focus (dialog->entry);

	gtk_signal_connect (GTK_OBJECT (dialog->entry), "changed",
			    entry_changed, dialog);
	gtk_signal_connect (GTK_OBJECT (dialog->entry), "activate",
			    entry_activate, dialog);

	return dialog;
}

void
gtk_html_search_dialog_destroy (GtkHTMLSearchDialog *d)
{
	g_free (d);
}

void
search (GtkHTMLControlData *cd, gboolean regular)
{
	if (cd->search_dialog)
		cd->search_dialog->regular = regular;

	RUN_DIALOG (search);

	if (cd->search_dialog)
		gtk_widget_grab_focus (cd->search_dialog->entry);
}

void
search_next (GtkHTMLControlData *cd)
{
	if (cd->html->engine->search_info) {
		html_engine_search_next (cd->html->engine);
	} else {
		search (cd, TRUE);
	}
}
