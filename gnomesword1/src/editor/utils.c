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
#include <string.h>
#include "gtkhtml/gtkhtml-compat.h"
#include "gtkhtml/htmlcursor.h"
#include "gtkhtml/htmlengine.h"
#include "gtkhtml/htmlobject.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gnome.h>
#include "editor/utils.h"
#include "editor/properties.h"

//#define ICONDIR "./art"

GtkWidget *
color_table_new (GtkSignalFunc f, gpointer data)
{
	GtkWidget *table, *button;
	GtkStyle *style;
	guint val, i, j, base_val;

	table = gtk_table_new (8, 8, TRUE);
	for (val=0, i=0; i<8; i++)
		for (j=0; j<8; j++, val++) {

			button = gtk_button_new ();
			gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_HALF);
			gtk_widget_set_size_request (button, 16, 16);
			style = gtk_style_copy (button->style);

#define VAL (base_val << 14) | 0x1555*base_val
			base_val = (val & 12) >> 2;
			style->bg [GTK_STATE_NORMAL].red   = VAL;
			base_val = (((val & 16) >> 2) | (val & 2)) >> 1;
			style->bg [GTK_STATE_NORMAL].green = VAL;
			base_val = ((val & 32) >> 4) | (val & 1);
			style->bg [GTK_STATE_NORMAL].blue  = VAL;
			style->bg [GTK_STATE_ACTIVE] = style->bg [GTK_STATE_NORMAL];
			style->bg [GTK_STATE_PRELIGHT] = style->bg [GTK_STATE_NORMAL];
			style->bg [GTK_STATE_SELECTED] = style->bg [GTK_STATE_NORMAL];
			style->bg [GTK_STATE_INSENSITIVE] = style->bg [GTK_STATE_NORMAL];

			/* printf ("rgb: %4x %4x %4x\n",
				style->bg [GTK_STATE_NORMAL].red,
				style->bg [GTK_STATE_NORMAL].green,
				style->bg [GTK_STATE_NORMAL].blue); */

			g_signal_connect (button, "clicked", G_CALLBACK (f), data);
			gtk_widget_set_style (button, style);
			gtk_table_attach_defaults (GTK_TABLE (table), button, i, i+1, j, j+1);
		}

	return table;
}

void
url_requested2 (GtkHTML *html, const gchar *url, GtkHTMLStream *handle)
{
	GtkHTMLStreamStatus status;
	gint fd;

	if (!strncmp (url, "file:", 5))
		url += 5;

	fd = open (url, O_RDONLY);
	status = GTK_HTML_STREAM_OK;
	if (fd != -1) {
		ssize_t size;
		void *buf = alloca (1 << 7);
		while ((size = read (fd, buf, 1 << 7))) {
			if (size == -1) {
				status = GTK_HTML_STREAM_ERROR;
				break;
			} else
				gtk_html_write (html, handle, (const gchar *) buf, size);
		}
	} else
		status = GTK_HTML_STREAM_ERROR;
	gtk_html_end (html, handle, status);
}

GtkWidget *
sample_frame (GtkHTML **html)
{
	GtkWidget *scroll_frame, *vbox;

	*html = GTK_HTML (gtk_html_new ());
	scroll_frame = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll_frame), GTK_SHADOW_IN);
	gtk_container_set_border_width (GTK_CONTAINER (scroll_frame), 6);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll_frame), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scroll_frame), GTK_WIDGET (*html));

	gtk_widget_show (GTK_WIDGET (*html));
	gtk_widget_show (scroll_frame);
	vbox = editor_hig_vbox_full (_("Sample"), scroll_frame, TRUE);

	g_signal_connect (*html, "url_requested", G_CALLBACK (url_requested2), NULL);

	return vbox;
}

GtkWidget *
editor_hig_vbox_full (gchar *text, GtkWidget *control, gboolean vexpand)
{
        GtkWidget *vbox, *hbox, *label;
        gchar *markup;

        markup = g_strconcat ("<span weight=\"bold\">", text, "</span>", NULL);
	label = gtk_label_new (markup);
        g_free (markup);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (label), .0, .5);

	vbox = gtk_vbox_new (FALSE, 12);
	hbox = gtk_hbox_new (FALSE, 0);

	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0); \
	gtk_box_pack_start (GTK_BOX (hbox), gtk_label_new ("    "), FALSE, FALSE, 0); \
	gtk_box_pack_start (GTK_BOX (hbox), control, TRUE, TRUE, 0); \
	gtk_box_pack_start (GTK_BOX (vbox), hbox, vexpand, vexpand, 0); \

	gtk_widget_show (label);
	gtk_widget_show (hbox);
	gtk_widget_show (vbox);

	return vbox;
}

GtkWidget *
editor_hig_vbox (gchar *text, GtkWidget *control)
{
	return editor_hig_vbox_full (text, control, FALSE);
}

GtkWidget *
editor_hig_inner_hbox (gchar *text, GtkWidget *control)
{
	GtkWidget *hbox, *label;

	hbox = gtk_hbox_new (FALSE, 6);
	label = gtk_label_new_with_mnemonic (text);
	gtk_misc_set_alignment (GTK_MISC (label), .0, .5);

	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), control, FALSE, FALSE, 0);

	gtk_widget_show (label);
	gtk_widget_show (hbox);

	return hbox;
}

void
editor_hig_attach_row (GtkWidget *table, gchar *text, GtkWidget *control, int row)
{
	GtkWidget *label;

	label = gtk_label_new_with_mnemonic (text);
	gtk_misc_set_alignment (GTK_MISC (label), .0, .5);

	gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (table), control, 1, 2, row, row + 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
}

static gboolean stock_test_url_added = FALSE;
static GtkStockItem test_url_items [] =
{
	{ GTKHTML_STOCK_TEST_URL, N_("_Visit..."), 0, 0, GETTEXT_PACKAGE }
};

void
editor_check_stock ()
{
	if (!stock_test_url_added) {
		GdkPixbuf *pixbuf;
		GError *error = NULL;

		pixbuf = gdk_pixbuf_new_from_file (GTKHTML_DATA_DIR"/icons/insert-link-16.png", &error);
		if (!pixbuf) {
			g_error_free (error);
		} else {
			GtkIconSet *test_url_iconset = gtk_icon_set_new_from_pixbuf (pixbuf);

			if (test_url_iconset) {
				GtkIconFactory *factory = gtk_icon_factory_new ();

				gtk_icon_factory_add (factory, GTKHTML_STOCK_TEST_URL, test_url_iconset);
				gtk_icon_factory_add_default (factory);
			}
			gtk_stock_add_static (test_url_items, G_N_ELEMENTS (test_url_items));
		}
		stock_test_url_added = TRUE;
	}
}

gboolean
editor_has_html_object (GSHTMLEditorControlData *cd, HTMLObject *o)
{
	HTMLEngine *e = cd->html->engine;
	guint position = e->cursor->position;

	if (e->cursor->object != o)
		if (!o->parent || !html_cursor_jump_to (e->cursor, e, o, 0)) {
			GtkWidget *dialog;
			printf ("d: %p\n", cd->properties_dialog);
			dialog = gtk_message_dialog_new (GTK_WINDOW (cd->properties_dialog->dialog),
							 GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
							 _("The editted object was removed from the document.\nCannot apply your changes."));
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			html_cursor_jump_to_position (e->cursor, e, position);
			return FALSE;
		}

	html_cursor_jump_to_position (e->cursor, e, position);
	return TRUE;
}
#endif
