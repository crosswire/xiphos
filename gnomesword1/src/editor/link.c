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
#include <libgnome/gnome-i18n.h>
#include <glade/glade.h>

#include "gtkhtml/htmlcolor.h"
#include "gtkhtml/htmlcolorset.h"
#include "gtkhtml/htmlengine-edit.h"
#include "gtkhtml/htmlengine-edit-text.h"
#include "gtkhtml/htmlengine-edit-fontstyle.h"
#include "gtkhtml/htmlengine-edit-cut-and-paste.h"
#include "gtkhtml/htmlimage.h"
#include "gtkhtml/htmlselection.h"
#include "gtkhtml/htmlsettings.h"
#include "gtkhtml/htmltext.h"

#include "editor/properties.h"
#include "editor/link.h"
#include "editor/utils.h"

struct _GtkHTMLEditLinkProperties {
	GSHTMLEditorControlData *cd;
	GtkWidget *entry_description;
	GtkWidget *label_description;
	GtkWidget *entry_url;

	HTMLObject *object;
	gint offset;
	gboolean selection;
	gboolean description_empty;
	gboolean insert;
	int insert_start_offset;
	int insert_end_offset;
	HTMLObject *insert_object;

	gboolean disable_change;
};
typedef struct _GtkHTMLEditLinkProperties GtkHTMLEditLinkProperties;

static void
url_changed (GtkWidget *w, GtkHTMLEditLinkProperties *d)
{
	const char *url, *desc;

	if (d->disable_change)
		return;

	url = gtk_entry_get_text (GTK_ENTRY (d->entry_url));
	desc = gtk_entry_get_text (GTK_ENTRY (d->entry_description));
	if (d->selection)
		html_engine_set_link (d->cd->html->engine, url);
	else {
		if (!desc || !*desc || d->description_empty) {
			gtk_entry_set_text (GTK_ENTRY (d->entry_description), url);
			d->description_empty = TRUE;
		}
	}
}

static void
description_changed (GtkWidget *w, GtkHTMLEditLinkProperties *d)
{
	HTMLEngine *e = d->cd->html->engine;
	const char *text;
	int len;

	d->description_empty = FALSE;

	if (d->disable_change || !editor_has_html_object (d->cd, d->insert_object))
		return;

	html_cursor_jump_to (e->cursor, e, d->insert_object, d->insert_start_offset);
	html_engine_set_mark (e);
	html_cursor_jump_to (e->cursor, e, d->insert_object, d->insert_end_offset);
	html_engine_delete (e);
	text = gtk_entry_get_text (GTK_ENTRY (w));
	if (text && *text) {
		len = g_utf8_strlen (text, -1);
		html_engine_paste_link (e, text, len, gtk_entry_get_text (GTK_ENTRY (d->entry_url)));
		d->insert_object = e->cursor->object;
	} else
		len = 0;
	d->insert_end_offset = d->insert_start_offset + len;
}

static void
link_set_ui (GtkHTMLEditLinkProperties *d)
{
	HTMLEngine *e = d->cd->html->engine;

	d->disable_change = TRUE;

	if (html_engine_is_selection_active (e)) {
		d->selection = TRUE;

		gtk_widget_hide (d->label_description);
		gtk_widget_hide (d->entry_description);
	} else {
		char *url = NULL;

		if (HTML_IS_TEXT (e->cursor->object))
			url = html_object_get_complete_url (e->cursor->object, e->cursor->offset);

		d->selection = FALSE;
		d->insert = TRUE;
		d->insert_object = e->cursor->object;

		if (url) {
			gtk_entry_set_text (GTK_ENTRY (d->entry_url), url);
			gtk_widget_hide (d->label_description);
			gtk_widget_hide (d->entry_description);
		
			if (HTML_IS_IMAGE (d->insert_object)) {
				d->insert_start_offset = 0;
				d->insert_end_offset = 1;
			} else {
				Link *link;
				link = html_text_get_link_at_offset (HTML_TEXT (d->insert_object), e->cursor->offset);

				if (link) {
					d->insert_start_offset = link->start_offset;
					d->insert_end_offset = link->end_offset;
				}
			}
		} else {
			gtk_entry_set_text (GTK_ENTRY (d->entry_url), "http://");

			d->insert_start_offset = d->insert_end_offset = e->cursor->offset;
		}
	}

	d->disable_change = FALSE;

	/* link_text = html_text_get_link_text (data->text, data->offset);
	gtk_entry_set_text (GTK_ENTRY (data->entry_text), link_text);
	g_free (link_text);

	url = html_object_get_complete_url (HTML_OBJECT (data->text), data->offset);
	gtk_entry_set_text (GTK_ENTRY (data->entry_url), url ? url : "");
	g_free (url); */
}

static void
test_url_clicked (GtkWidget *w, GtkHTMLEditLinkProperties *d)
{
	const char *url = gtk_entry_get_text (GTK_ENTRY (d->entry_url));

	if (url)
		gnome_url_show (url, NULL);
}

static GtkWidget *
link_widget (GtkHTMLEditLinkProperties *d, gboolean insert)
{
	GtkWidget *link_page, *button;
	GladeXML *xml;

	xml = glade_xml_new ("gtkhtml-editor-properties.glade", "link_page", NULL);
	if (!xml)
		g_error (_("Could not load glade file."));

	link_page = glade_xml_get_widget (xml, "link_page");

	editor_check_stock ();
	button = gtk_button_new_from_stock (GTKHTML_STOCK_TEST_URL);
	g_signal_connect (button, "clicked", G_CALLBACK (test_url_clicked), d);
	gtk_widget_show (button);
	gtk_table_attach (GTK_TABLE (glade_xml_get_widget (xml, "table_link")), button, 2, 3, 0, 1, 0, 0, 0, 0);

	d->entry_url = glade_xml_get_widget (xml, "entry_url");
	g_signal_connect (d->entry_url, "changed", G_CALLBACK (url_changed), d);

	d->entry_description = glade_xml_get_widget (xml, "entry_description");
	g_signal_connect (d->entry_description, "changed", G_CALLBACK (description_changed), d);

	d->label_description = glade_xml_get_widget (xml, "label_description");

	/*GtkWidget *vbox, *hbox, *button, *frame, *f1;

	vbox = gtk_vbox_new (FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 12);

	data->entry_text = gtk_entry_new ();
	data->entry_url  = gtk_entry_new ();

	gtk_box_pack_start (GTK_BOX (vbox), editor_hig_vbox (_("Link Text"), data->entry_text), FALSE, FALSE, 0);

	if (html_engine_is_selection_active (cd->html->engine)) {
		gchar *str;

		str = html_engine_get_selection_string (cd->html->engine);
		gtk_entry_set_text (GTK_ENTRY (data->entry_text), str);
		g_free (str);
	}

	hbox = gtk_hbox_new (FALSE, 5);
	editor_check_stock ();
	button = gtk_button_new_from_stock (GTKHTML_STOCK_TEST_URL);
	gtk_box_pack_start (GTK_BOX (hbox), data->entry_url, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), editor_hig_vbox (_("Click Will Follow This URL"), hbox), FALSE, FALSE, 0);

	if (!insert) {
		gtk_widget_set_sensitive (data->entry_text, FALSE);
		set_ui (data);
	}

	g_signal_connect (data->entry_text, "changed", G_CALLBACK (changed), data);
	g_signal_connect (data->entry_url, "changed", G_CALLBACK (changed), data);
	g_signal_connect (button, "clicked", G_CALLBACK (test_clicked), data);*/

	gtk_widget_show_all (link_page);
	link_set_ui (d);

	return link_page;
}

GtkWidget *
link_insert (GSHTMLEditorControlData *cd, gpointer *set_data)
{
	GtkHTMLEditLinkProperties *data = g_new0 (GtkHTMLEditLinkProperties, 1);

	*set_data = data;
	data->cd = cd;

	return link_widget (data, TRUE);
}

GtkWidget *
link_properties (GSHTMLEditorControlData *cd, gpointer *set_data)
{
	GtkHTMLEditLinkProperties *data = g_new0 (GtkHTMLEditLinkProperties, 1);

	*set_data = data;
	data->cd = cd;

	return link_widget (data, FALSE);
}

void
link_close_cb (GSHTMLEditorControlData *cd, gpointer get_data)
{
	g_free (get_data);
}
