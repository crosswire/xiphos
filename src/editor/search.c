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
#include <libgnome/gnome-i18n.h>
#include <gnome.h>
#include <libgnomeui/gnome-window-icon.h>
#include "search.h"
#include "dialog.h"
#include "gtkhtml/htmlengine-search.h"

struct _GtkHTMLSearchDialog {
	GtkDialog   *dialog;
	GtkHTML     *html;
	GtkWidget   *entry;
	GtkWidget   *backward;
	GtkWidget   *case_sensitive;
	GtkWidget   *regular_exp;

	GSHTMLEditorControlData *cd;
};

static void
entry_changed (GtkWidget *entry, GtkHTMLSearchDialog *d)
{
	if (d->cd->search_text)
		g_free (d->cd->search_text);
	d->cd->search_text = g_strdup (gtk_entry_get_text (GTK_ENTRY (d->entry)));

	gtk_dialog_set_response_sensitive (d->dialog, 0, TRUE);
}

static void
entry_activate (GtkWidget *entry, GtkHTMLSearchDialog *d)
{
	gtk_dialog_response (d->dialog, 0);
}

static void
backward_toggled (GtkWidget *w, GtkHTMLSearchDialog *d)
{
	gtk_dialog_set_response_sensitive (d->dialog, 0, TRUE);
}

static void
case_toggled (GtkWidget *w, GtkHTMLSearchDialog *d)
{
	/* Activate only when the case_sensitive flag is cleared */
	if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)))
		gtk_dialog_set_response_sensitive (d->dialog, 0, TRUE);
}

static void
regular_toggled (GtkWidget *w, GtkHTMLSearchDialog *d)
{
	gtk_dialog_set_response_sensitive (d->dialog, 0, TRUE);
}

static void
search_dialog_response (GtkDialog *dialog, gint response_id, GtkHTMLSearchDialog *d)
{
	g_assert (d && d->dialog );

	switch (response_id) {
	case 0: /* Search */
		if (!html_engine_search (d->html->engine, gtk_entry_get_text (GTK_ENTRY (d->entry)),
					 GTK_TOGGLE_BUTTON (d->case_sensitive)->active,
					 GTK_TOGGLE_BUTTON (d->backward)->active == 0, GTK_TOGGLE_BUTTON (d->regular_exp)->active))
			gtk_dialog_set_response_sensitive (d->dialog, 0, FALSE);
		break;

	case GTK_RESPONSE_DELETE_EVENT:
	case GTK_RESPONSE_CLOSE:
	case GTK_RESPONSE_CANCEL:
		gtk_dialog_set_response_sensitive (d->dialog, 0, TRUE);
		gtk_widget_hide (GTK_WIDGET (d->dialog));
		gtk_widget_grab_focus (GTK_WIDGET (d->cd->html));
		break;
	}
}

GtkHTMLSearchDialog *
gtk_html_search_dialog_new (GtkHTML *html, GSHTMLEditorControlData *cd)
{
	GtkHTMLSearchDialog *dialog = g_new (GtkHTMLSearchDialog, 1);
	GtkWidget *hbox, *vbox;

	/* we use CANCEL response for close, because we want Esc to close the dialog - see gtkdialog.c */
	dialog->dialog         = GTK_DIALOG (gtk_dialog_new_with_buttons (_("Find"), NULL, 0,
									  GTK_STOCK_CLOSE, GTK_RESPONSE_CANCEL,
									  GTK_STOCK_FIND, 0,
									  NULL));
	dialog->entry          = gtk_entry_new ();
	dialog->backward       = gtk_check_button_new_with_mnemonic (_("_Backward"));
	dialog->case_sensitive = gtk_check_button_new_with_mnemonic (_("Case _sensitive"));
	dialog->regular_exp    = gtk_check_button_new_with_mnemonic (_("_Regular Expression"));
	dialog->html           = html;
	dialog->cd             = cd;

	hbox = gtk_hbox_new (FALSE, 6);

	if (cd->search_text)
		gtk_entry_set_text (GTK_ENTRY (dialog->entry), cd->search_text);

	gtk_box_pack_start (GTK_BOX (hbox), dialog->backward, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), dialog->case_sensitive, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), dialog->regular_exp, FALSE, FALSE, 0);
	
	vbox = gtk_vbox_new (FALSE, 6);
	gtk_widget_show (vbox);
	gtk_box_pack_start (GTK_BOX (vbox), dialog->entry, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (dialog->dialog), 6);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
	gtk_container_set_border_width (GTK_CONTAINER (dialog->dialog->vbox), 6);
	gtk_box_set_spacing (GTK_BOX (dialog->dialog->vbox), 6);
	gtk_box_pack_start (GTK_BOX (dialog->dialog->vbox), vbox, FALSE, FALSE, 0);
	gtk_widget_show (dialog->entry);
	gtk_widget_show_all (hbox);

	gnome_window_icon_set_from_file (GTK_WINDOW (dialog->dialog), 
					PACKAGE_PIXMAPS_DIR"stock_search");

	gtk_widget_grab_focus (dialog->entry);

	g_signal_connect (dialog->dialog, "response", G_CALLBACK (search_dialog_response), dialog);
	g_signal_connect (dialog->entry, "changed", G_CALLBACK (entry_changed), dialog);
	g_signal_connect (dialog->entry, "activate", G_CALLBACK (entry_activate), dialog);
	g_signal_connect (dialog->backward, "toggled", G_CALLBACK (backward_toggled), dialog);
	g_signal_connect (dialog->case_sensitive, "toggled", G_CALLBACK (case_toggled), dialog);
	g_signal_connect (dialog->regular_exp, "toggled", G_CALLBACK (regular_toggled), dialog);

	return dialog;
}

void
gtk_html_search_dialog_destroy (GtkHTMLSearchDialog *d)
{
	g_assert (d && d->dialog);

	gtk_widget_destroy (GTK_WIDGET (d->dialog));
	g_free (d);
}

void
search (GSHTMLEditorControlData *cd)
{
	RUN_DIALOG (search, _("Find"));

	g_assert (cd->search_dialog && cd->search_dialog->dialog);

	if (!GTK_WIDGET_VISIBLE (cd->search_dialog->dialog)) {
		gtk_html_search_dialog_destroy (cd->search_dialog);
		cd->search_dialog = NULL;
	}
}

void
search_next (GSHTMLEditorControlData *cd)
{
	if (cd->html->engine->search_info) {
		html_engine_search_next (cd->html->engine);
	} else {
		search (cd);
	}
}
