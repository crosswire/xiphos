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
#include "htmlcolor.h"
#include "htmlcolorset.h"
#include "htmlengine-edit.h"
#include "htmlengine-edit-fontstyle.h"
#include "htmlengine-save.h"
#include "htmlsettings.h"

#include "text.h"
#include "properties.h"
#include "utils.h"

struct _GtkHTMLEditTextProperties {

	GtkHTMLControlData *cd;

	GtkWidget *color_combo;
	GtkWidget *style_option;
	GtkWidget *sel_size;
	GtkWidget *check [4];

	gboolean color_changed;
	gboolean style_changed;

	GtkHTMLFontStyle style_and;
	GtkHTMLFontStyle style_or;
	HTMLColor *color;

	GtkHTML *sample;
};
typedef struct _GtkHTMLEditTextProperties GtkHTMLEditTextProperties;

#define STYLES 4
static GtkHTMLFontStyle styles [STYLES] = {
	GTK_HTML_FONT_STYLE_BOLD,
	GTK_HTML_FONT_STYLE_ITALIC,
	GTK_HTML_FONT_STYLE_UNDERLINE,
	GTK_HTML_FONT_STYLE_STRIKEOUT,
};

#define CVAL(i) (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (d->check [i])))

static gint get_size (GtkHTMLFontStyle s);

static void
fill_sample (GtkHTMLEditTextProperties *d)
{
	gchar *body, *size, *color, *bg;

	bg    = html_engine_save_get_sample_body (d->cd->html->engine, NULL);
	size  = g_strdup_printf ("<font size=%d>", get_size (d->style_or) + 1);
	color = g_strdup_printf ("<font color=#%02x%02x%02x>",
				 d->color->color.red   >> 8,
				 d->color->color.green >> 8,
				 d->color->color.blue  >> 8);
	body  = g_strconcat (bg,
			     CVAL (0) ? "<b>" : "",
			     CVAL (1) ? "<i>" : "",
			     CVAL (2) ? "<u>" : "",
			     CVAL (3) ? "<s>" : "",
			     size,
			     color,
			     "The quick brown fox jumps over the lazy dog.", NULL);

	gtk_html_load_from_string (d->sample, body, -1);
	g_free (color);
	g_free (size);
	g_free (bg);
	g_free (body);
}

static void
color_changed (GtkWidget *w, GdkColor *color, gboolean by_user, GtkHTMLEditTextProperties *data)
{
	html_color_unref (data->color);
	data->color = color
		&& color != &html_colorset_get_color (data->cd->html->engine->settings->color_set, HTMLTextColor)->color
		? html_color_new_from_gdk_color (color)
		: html_colorset_get_color (data->cd->html->engine->settings->color_set, HTMLTextColor);
	html_color_ref (data->color);
	data->color_changed = TRUE;
	gtk_html_edit_properties_dialog_change (data->cd->properties_dialog);
	fill_sample (data);
}

static void
set_size (GtkWidget *w, GtkHTMLEditTextProperties *data)
{
	gint size = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (w), "size"));

	data->style_and &= ~GTK_HTML_FONT_STYLE_SIZE_MASK;
	data->style_or  &= ~GTK_HTML_FONT_STYLE_SIZE_MASK;
	data->style_or  |= size;
	data->style_changed = TRUE;
	gtk_html_edit_properties_dialog_change (data->cd->properties_dialog);
	fill_sample (data);
}

static void
set_style (GtkWidget *w, GtkHTMLEditTextProperties *d)
{
	GtkHTMLFontStyle style = GPOINTER_TO_UINT (gtk_object_get_data (GTK_OBJECT (w), "style"));

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w))) {
		d->style_or  |= style;
		d->style_and |= style;
	} else
		d->style_and &= ~style;

	d->style_changed = TRUE;
	gtk_html_edit_properties_dialog_change (d->cd->properties_dialog);
	fill_sample (d);
}

static gint
get_size (GtkHTMLFontStyle s)
{
	return (s & GTK_HTML_FONT_STYLE_SIZE_MASK)
		? (s & GTK_HTML_FONT_STYLE_SIZE_MASK) - GTK_HTML_FONT_STYLE_SIZE_1
		: 2;
}

GtkWidget *
text_properties (GtkHTMLControlData *cd, gpointer *set_data)
{
	GtkHTMLEditTextProperties *data = g_new (GtkHTMLEditTextProperties, 1);
	GtkWidget *vbox, *frame, *table, *menu, *menuitem, *hbox;
	gint i;

	*set_data = data;

	data->cd = cd;
	data->color_changed   = FALSE;
	data->style_changed   = FALSE;
	data->style_and       = GTK_HTML_FONT_STYLE_MAX;
	data->style_or        = html_engine_get_font_style (cd->html->engine);
	data->color           = html_engine_get_color (cd->html->engine);
	html_color_ref (data->color);

	table = gtk_table_new (3, 2, FALSE);
	gtk_container_border_width (GTK_CONTAINER (table), 3);
	gtk_table_set_col_spacings (GTK_TABLE (table), 3);
	gtk_table_set_row_spacings (GTK_TABLE (table), 2);

	frame = gtk_frame_new (_("Style"));
	vbox = gtk_vbox_new (FALSE, 2);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 3);

#define ADD_CHECK(x) \
	data->check [i] = gtk_check_button_new_with_label (x); \
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (data->check [i]), data->style_or & styles [i]); \
        gtk_object_set_data (GTK_OBJECT (data->check [i]), "style", GUINT_TO_POINTER (styles [i])); \
        gtk_signal_connect (GTK_OBJECT (data->check [i]), "toggled", set_style, data); \
	gtk_box_pack_start (GTK_BOX (vbox), data->check [i], FALSE, FALSE, 0); i++

	i=0;
	ADD_CHECK (_("Bold"));
	ADD_CHECK (_("Italic"));
	ADD_CHECK (_("Underline"));
	ADD_CHECK (_("Strikeout"));

	gtk_container_add (GTK_CONTAINER (frame), vbox);
	gtk_table_attach (GTK_TABLE (table), frame, 0, 1, 0, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);

	frame = gtk_frame_new (_("Size"));
	menu = gtk_menu_new ();

#undef ADD_ITEM
#define ADD_ITEM(n) \
	menuitem = gtk_menu_item_new_with_label (_(n)); \
        gtk_menu_append (GTK_MENU (menu), menuitem); \
        gtk_widget_show (menuitem); \
        gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (set_size), data); \
        gtk_object_set_data (GTK_OBJECT (menuitem), "size", GINT_TO_POINTER (i)); i++;

	i=GTK_HTML_FONT_STYLE_SIZE_1;
	ADD_ITEM("-2");
	ADD_ITEM("-1");
	ADD_ITEM("+0");
	ADD_ITEM("+1");
	ADD_ITEM("+2");
	ADD_ITEM("+3");
	ADD_ITEM("+4");

	data->sel_size = gtk_option_menu_new ();
	gtk_option_menu_set_menu (GTK_OPTION_MENU (data->sel_size), menu);
	gtk_option_menu_set_history (GTK_OPTION_MENU (data->sel_size), get_size (data->style_or));
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 3);
	gtk_box_pack_start (GTK_BOX (vbox), data->sel_size, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), vbox);
	gtk_table_attach (GTK_TABLE (table), frame, 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

	/* color selection */
	frame = gtk_frame_new (_("Color"));
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 3);
	data->color_combo = color_combo_new (NULL, _("Automatic"),
					     &html_colorset_get_color (data->cd->html->engine->settings->color_set,
								       HTMLTextColor)->color,
					     color_group_fetch ("text", data->cd));
        gtk_signal_connect (GTK_OBJECT (data->color_combo), "changed", GTK_SIGNAL_FUNC (color_changed), data);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), data->color_combo, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (frame), hbox);
	gtk_table_attach (GTK_TABLE (table), frame, 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

	/* sample */
	gtk_table_attach (GTK_TABLE (table), sample_frame (&data->sample), 0, 2, 2, 3, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
	fill_sample (data);

	gtk_widget_show_all (table);

	return table;
}

void
text_apply_cb (GtkHTMLControlData *cd, gpointer get_data)
{
	GtkHTMLEditTextProperties *data = (GtkHTMLEditTextProperties *) get_data;

	if (data->style_changed)
		gtk_html_set_font_style (cd->html, data->style_and, data->style_or);

	if (data->color_changed)
		gtk_html_set_color (cd->html, data->color);

	data->color_changed = FALSE;
	data->style_changed = FALSE;
}

void
text_close_cb (GtkHTMLControlData *cd, gpointer get_data)
{
	GtkHTMLEditTextProperties *data = (GtkHTMLEditTextProperties *) get_data;

	html_color_unref (data->color);

	g_free (get_data);
}
