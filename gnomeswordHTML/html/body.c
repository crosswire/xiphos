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
#include <gal/widgets/widget-color-combo.h>
#include "htmlengine-edit-clueflowstyle.h"
#include "htmlimage.h"
#include "htmlcolor.h"
#include "htmlcolorset.h"
#include "htmlsettings.h"
#include "body.h"
#include "properties.h"
#include "utils.h"

struct _GtkHTMLEditBodyProperties {
	GtkHTMLControlData *cd;

	GtkWidget *pixmap_entry;
	GtkWidget *use_bg_image;

	GdkColor   color [HTMLColors];
	gboolean   color_changed [HTMLColors];

	GtkHTML   *sample;
};
typedef struct _GtkHTMLEditBodyProperties GtkHTMLEditBodyProperties;

static void
fill_sample (GtkHTMLEditBodyProperties *d)
{
	gchar *body, *body_tag, *bg_image, *fname;
	fname = gtk_entry_get_text (GTK_ENTRY (gnome_pixmap_entry_gtk_entry
					       (GNOME_PIXMAP_ENTRY (d->pixmap_entry))));
	bg_image = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (d->use_bg_image)) && fname
		? g_strdup_printf (" background=\"%s\"", fname) : g_strdup ("");
	body_tag = g_strdup_printf ("<body bgcolor=#%02x%02x%02x link=#%02x%02x%02x text=#%02x%02x%02x%s>",
				    d->color [HTMLBgColor].red >> 8,
				    d->color [HTMLBgColor].green >> 8,
				    d->color [HTMLBgColor].blue >> 8,
				    d->color [HTMLLinkColor].red >> 8,
				    d->color [HTMLLinkColor].green >> 8,
				    d->color [HTMLLinkColor].blue >> 8,
				    d->color [HTMLTextColor].red >> 8,
				    d->color [HTMLTextColor].green >> 8,
				    d->color [HTMLTextColor].blue >> 8,
				    bg_image);

	body  = g_strconcat (body_tag,
			     "The quick brown <a href=\"mailto:fox\">fox</a> jumps over the lazy <a href=\"mailto:dog\">dog</a>.",
			     NULL);

	gtk_html_load_from_string (d->sample, body, -1);
	g_free (bg_image);
	g_free (body_tag);
	g_free (body);
}

static void
color_changed (GtkWidget *w, GdkColor *color, gboolean by_user, GtkHTMLEditBodyProperties *data)
{
	gint idx;

	/* If the color was changed programatically there's not need to set things */
	if (!by_user)
		return;
		
	idx = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (w), "type"));
	data->color [idx] = color
		? *color
		: html_colorset_get_color (data->cd->html->engine->defaultSettings->color_set, idx)->color;
	data->color_changed [idx] = TRUE;
	gtk_html_edit_properties_dialog_change (data->cd->properties_dialog);
	fill_sample (data);
}

static void
bg_check_cb (GtkWidget *w, GtkHTMLEditBodyProperties *data)
{
	gtk_html_edit_properties_dialog_change (data->cd->properties_dialog);
	fill_sample (data);
}

/* static gboolean
hide_preview (GtkHTMLEditBodyProperties *d)
{
	gnome_pixmap_entry_set_preview (GNOME_PIXMAP_ENTRY (d->pixmap_entry), FALSE);

	return FALSE;
} */

static void
entry_changed (GtkWidget *w, GtkHTMLEditBodyProperties *data)
{
	gchar *text = gtk_entry_get_text (GTK_ENTRY (w));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (data->use_bg_image), (*text) ? TRUE : FALSE);
	gtk_html_edit_properties_dialog_change (data->cd->properties_dialog);	
	fill_sample (data);
}

GtkWidget *
body_properties (GtkHTMLControlData *cd, gpointer *set_data)
{
	GtkHTMLEditBodyProperties *data = g_new0 (GtkHTMLEditBodyProperties, 1);
	GtkWidget *hbox, *vbox, *frame, *combo, *table;
	GSList *group;
	HTMLColor *color;

	*set_data = data;
	data->cd = cd;

	table = gtk_table_new (2, 2, FALSE);
	gtk_container_border_width (GTK_CONTAINER (table), 3);
	gtk_table_set_col_spacings (GTK_TABLE (table), 3);
	gtk_table_set_row_spacings (GTK_TABLE (table), 2);
	frame = gtk_frame_new (_("Background Image"));
	vbox = gtk_vbox_new (FALSE, 2);
	gtk_container_border_width (GTK_CONTAINER (vbox), 3);
	data->use_bg_image = gtk_check_button_new_with_label (_("Enable"));
	data->pixmap_entry = gnome_pixmap_entry_new ("background_image", _("Background Image"), TRUE);
	if (cd->html->engine->bgPixmapPtr) {
		HTMLImagePointer *ip = (HTMLImagePointer *) cd->html->engine->bgPixmapPtr;
		guint off = 0;
		 if (!strncmp (ip->url, "file:", 5))
			 off = 5;

		 gtk_entry_set_text (GTK_ENTRY (gnome_pixmap_entry_gtk_entry (GNOME_PIXMAP_ENTRY (data->pixmap_entry))),
				     ip->url + off);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (data->use_bg_image), TRUE);
	} else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (data->use_bg_image), FALSE);
	gtk_signal_connect (GTK_OBJECT (data->use_bg_image), "toggled", bg_check_cb, data);
	gtk_signal_connect (GTK_OBJECT (gnome_pixmap_entry_gtk_entry (GNOME_PIXMAP_ENTRY (data->pixmap_entry))),
			    "changed", GTK_SIGNAL_FUNC (entry_changed), data);

	gtk_box_pack_start (GTK_BOX (vbox), data->use_bg_image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), data->pixmap_entry, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), vbox);
	gtk_table_attach_defaults (GTK_TABLE (table), frame, 0, 1, 0, 1);
	gtk_table_attach_defaults (GTK_TABLE (table), sample_frame (&data->sample), 0, 2, 1, 2);

	frame = gtk_frame_new (_("Colors"));
	vbox = gtk_vbox_new (FALSE, 2);
	gtk_container_border_width (GTK_CONTAINER (vbox), 3);

	group = NULL;
#define ADD_COLOR(x, ct, g) \
        data->color [ct] = html_colorset_get_color_allocated (cd->html->engine->painter, ct)->color; \
        color = html_colorset_get_color (cd->html->engine->defaultSettings->color_set, ct); \
        html_color_alloc (color, cd->html->engine->painter); \
	combo = color_combo_new (NULL, _("Automatic"), \
				 &color->color, \
				 color_group_fetch ("body_" g, cd)); \
        color_combo_set_color (COLOR_COMBO (combo), &data->color [ct]); \
        gtk_object_set_data (GTK_OBJECT (combo), "type", GINT_TO_POINTER (ct)); \
        gtk_signal_connect (GTK_OBJECT (combo), "changed", GTK_SIGNAL_FUNC (color_changed), data); \
	hbox = gtk_hbox_new (FALSE, 3); \
	gtk_box_pack_start (GTK_BOX (hbox), combo, FALSE, FALSE, 0); \
	gtk_box_pack_start (GTK_BOX (hbox), gtk_label_new (x), FALSE, FALSE, 0); \
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

	ADD_COLOR (_("Text"), HTMLTextColor, "text");
	ADD_COLOR (_("Link"), HTMLLinkColor, "link");
	ADD_COLOR (_("Background"), HTMLBgColor, "bg");

	gtk_container_add (GTK_CONTAINER (frame), vbox);
	gtk_table_attach_defaults (GTK_TABLE (table), frame, 1, 2, 0, 1);
	fill_sample (data);
	/* gtk_idle_add (hide_preview, data); */

	gtk_widget_show_all (table);

	return table;
}

void
body_apply_cb (GtkHTMLControlData *cd, gpointer get_data)
{
	GtkHTMLEditBodyProperties *data = (GtkHTMLEditBodyProperties *) get_data;
	gboolean redraw = FALSE;

#define APPLY_COLOR(c) \
	if (data->color_changed [c]) { \
		html_colorset_set_color (cd->html->engine->settings->color_set, &data->color [c], c); \
                redraw = TRUE; \
        }

	APPLY_COLOR (HTMLTextColor);
	APPLY_COLOR (HTMLLinkColor);
	APPLY_COLOR (HTMLBgColor);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (data->use_bg_image))) {
		HTMLEngine *e = data->cd->html->engine;
		gchar *file = g_strconcat ("file:", gtk_entry_get_text (GTK_ENTRY
									(gnome_pixmap_entry_gtk_entry
									 (GNOME_PIXMAP_ENTRY (data->pixmap_entry)))), NULL);

		if (e->bgPixmapPtr != NULL)
			html_image_factory_unregister(e->image_factory, e->bgPixmapPtr, NULL);
		e->bgPixmapPtr = html_image_factory_register(e->image_factory, NULL, file);
		g_free (file);
		redraw = TRUE;
	}

	if (redraw)
		gtk_widget_queue_draw (GTK_WIDGET (cd->html));
}

void
body_close_cb (GtkHTMLControlData *cd, gpointer get_data)
{
	g_free (get_data);
}
