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
#include <string.h>
#include "htmlengine-edit-fontstyle.h"
#include "htmlengine-edit-cut-and-paste.h"
#include "htmlselection.h"

#include "properties.h"
#include "dialog.h"
#include "link.h"

struct _GtkHTMLEditLinkProperties {
	GtkHTMLControlData *cd;
	GtkWidget *entry;
	gboolean url_changed;
};
typedef struct _GtkHTMLEditLinkProperties GtkHTMLEditLinkProperties;

static void
set_link (GtkWidget *w, GtkHTMLEditLinkProperties *data)
{
	data->url_changed = TRUE;
	gtk_html_edit_properties_dialog_change (data->cd->properties_dialog);
}

static void
clear (GtkWidget *w, GtkHTMLEditLinkProperties *data)
{
	gtk_entry_set_text (GTK_ENTRY (data->entry), "");
}

GtkWidget *
link_properties (GtkHTMLControlData *cd, gpointer *set_data)
{
	GtkWidget *vbox, *hbox, *button;
	GtkHTMLEditLinkProperties *data = g_new (GtkHTMLEditLinkProperties, 1);
	const gchar *url;

	*set_data = data;
	data->cd = cd;
	data->url_changed = FALSE;

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (vbox), 3);
	hbox = gtk_hbox_new (FALSE, 3);

	data->entry = gtk_entry_new ();
	url = (html_engine_is_selection_active (cd->html->engine))
		? html_engine_get_document_url (cd->html->engine)
		: html_engine_get_url (cd->html->engine);
	if (url)
		gtk_entry_set_text (GTK_ENTRY (data->entry), url);
	button = gtk_button_new_with_label (_("Clear"));
	gtk_signal_connect (GTK_OBJECT (button), "clicked", clear, data);
	gtk_signal_connect (GTK_OBJECT (data->entry), "changed", set_link, data);
	gtk_box_pack_start (GTK_BOX (hbox), gtk_label_new (_("URL")), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), data->entry, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

	gtk_widget_show_all (vbox);

	return vbox;
}

void
link_apply_cb (GtkHTMLControlData *cd, gpointer get_data)
{
	GtkHTMLEditLinkProperties *data = (GtkHTMLEditLinkProperties *) get_data;
	HTMLEngine *e = cd->html->engine;
	gchar *url;
	gchar *target = "";

	if (!data->url_changed)
		return;

	url = gtk_entry_get_text (GTK_ENTRY (data->entry));
	if (*url)
		html_engine_insert_link (e, url, target);
	else
		html_engine_insert_link (e, NULL, NULL);
}

void
link_close_cb (GtkHTMLControlData *cd, gpointer get_data)
{
	g_free (get_data);
}
