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
#ifndef USE_GTKHTML38
#include <libgnome/gnome-i18n.h>
#include <libgnomeui/gnome-window-icon.h>
#include "editor/dialog.h"
#include "editor/properties.h"

#define GTK_HTML_EDIT_IMAGE_BWIDTH      0
#define GTK_HTML_EDIT_IMAGE_WIDTH       1
#define GTK_HTML_EDIT_IMAGE_HEIGHT      2
#define GTK_HTML_EDIT_IMAGE_HSPACE      3
#define GTK_HTML_EDIT_IMAGE_VSPACE      4
#define GTK_HTML_EDIT_IMAGE_SPINS       5

#define ALIGN_TOP    0
#define ALIGN_CENTER 1
#define ALIGN_BOTTOM 2

struct _PageData {
	GtkHTMLEditPropertyType      type;
	GtkHTMLEditPropertyCloseFunc close;
	gchar *name;
	gpointer data;
};
typedef struct _PageData PageData;

static void
prop_close (GtkHTMLEditPropertiesDialog *d)
{
	gtk_widget_grab_focus (GTK_WIDGET (d->control_data->html));
	gtk_html_edit_properties_dialog_destroy (d);
}

static void
switch_page (GtkWidget *w, GtkNotebookPage *page, gint num, GtkHTMLEditPropertiesDialog *d)
{
	if (d->dialog) {
		PageData *pd;

		pd = (PageData *) g_list_nth (d->page_data, num)->data;
		if (pd) {
			gchar *title;
			title = g_strconcat (d->title, ": ", pd->name, NULL);
			gtk_window_set_title (GTK_WINDOW (d->dialog), title);
			g_free (title);
		}
	}
}

static void
destroy_dialog (GtkWidget *w, gpointer data)
{
	((GtkHTMLEditPropertiesDialog *) data)->dialog = NULL;
}

static void
dialog_response (GtkDialog *dialog, gint response_id, GtkHTMLEditPropertiesDialog *d)
{
	switch (response_id) {
	case GTK_RESPONSE_CANCEL:
	case GTK_RESPONSE_CLOSE: /* OK */
		prop_close (d);
		break;
	}
}

GtkHTMLEditPropertiesDialog *
gtk_html_edit_properties_dialog_new (GSHTMLEditorControlData *cd, gchar *title, gchar *icon_path)
{
	GtkHTMLEditPropertiesDialog *d = g_new (GtkHTMLEditPropertiesDialog, 1);
	GtkWidget *vbox;
	GtkWindow *parent;

	d->page_data      = NULL;
	d->title          = g_strdup (title);
	d->control_data   = cd;
	parent = get_parent_window (GTK_WIDGET (cd->html));

	d->dialog = gtk_dialog_new_with_buttons (title, parent, 0,
						 GTK_STOCK_HELP, GTK_RESPONSE_HELP,
						 GTK_STOCK_CLOSE, GTK_RESPONSE_CANCEL,
						 NULL);

	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (d->dialog)->vbox), 6);
	gtk_container_set_border_width (GTK_CONTAINER (d->dialog), 6);

	d->notebook = gtk_notebook_new ();
	g_signal_connect (d->dialog, "destroy", G_CALLBACK (destroy_dialog), d);
	g_signal_connect (d->notebook, "switch_page", G_CALLBACK (switch_page), d);
	vbox = gtk_vbox_new (FALSE, 6);
	gtk_widget_show (vbox);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), d->notebook);
	gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (d->dialog)->vbox), vbox);
	gtk_widget_show (d->notebook);

	g_signal_connect (d->dialog, "response", G_CALLBACK (dialog_response), d);

	gnome_window_icon_set_from_file (GTK_WINDOW (d->dialog), icon_path);
	gtk_dialog_set_response_sensitive (GTK_DIALOG (d->dialog), 0, FALSE);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (d->dialog)->vbox), 6);

	return d;
}

static void
destroy (PageData *pd, GtkHTMLEditPropertiesDialog *d)
{
	(*pd->close) (d->control_data, pd->data);
	g_free (pd->name);
	g_free (pd);
}

void
gtk_html_edit_properties_dialog_destroy (GtkHTMLEditPropertiesDialog *d)
{
	if (d->dialog)
		gtk_widget_destroy (d->dialog);
	g_list_foreach (d->page_data, (GFunc) destroy, d);
	g_list_free    (d->page_data);
	g_list_free    (d->control_data->properties_types);
	d->control_data->properties_dialog = NULL;
	d->control_data->properties_types  = NULL;
	g_free (d->title);
	g_free (d);
}

void
gtk_html_edit_properties_dialog_add_entry (GtkHTMLEditPropertiesDialog *d,
					   GtkHTMLEditPropertyType t,
					   const gchar *name,
					   GtkHTMLEditPropertyCreateFunc create,
					   GtkHTMLEditPropertyCloseFunc close_cb)

{
	PageData *pd = g_new (PageData, 1);
	GtkWidget *page;

	page = (*create) (d->control_data, &pd->data);
	pd->close = close_cb;
	pd->type  = t;
	pd->name  = g_strdup (name);

	d->page_data = g_list_append (d->page_data, pd);
	gtk_notebook_append_page (GTK_NOTEBOOK (d->notebook), page, gtk_label_new (name));
}

void
gtk_html_edit_properties_dialog_show (GtkHTMLEditPropertiesDialog *d)
{
	if (g_list_length (d->page_data) <= 1) {
		gtk_notebook_set_show_tabs (GTK_NOTEBOOK (d->notebook), FALSE);
		gtk_notebook_set_show_border (GTK_NOTEBOOK (d->notebook), FALSE);
		if (g_list_length (d->page_data) == 1)
			gtk_container_set_border_width (GTK_CONTAINER (gtk_notebook_get_nth_page (GTK_NOTEBOOK (d->notebook), 0)), 0);
	}
	gtk_widget_show (d->dialog);
}

void
gtk_html_edit_properties_dialog_close (GtkHTMLEditPropertiesDialog *d)
{
	if (d->dialog)
		gtk_dialog_response (GTK_DIALOG (d->dialog), GTK_RESPONSE_CANCEL);
}

static gint
find_type (PageData *d, gpointer data)
{
	return (d->type == GPOINTER_TO_INT (data)) ? 0 : 1;
}

void
gtk_html_edit_properties_dialog_set_page (GtkHTMLEditPropertiesDialog *d, GtkHTMLEditPropertyType t)
{
	gint pos = g_list_position (d->page_data, g_list_find_custom (d->page_data, GINT_TO_POINTER (t),
								      (GCompareFunc) find_type));
	if (pos >= 0)
		gtk_notebook_set_current_page (GTK_NOTEBOOK (d->notebook), pos);
}
#endif
