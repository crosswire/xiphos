/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*  This file is part of the GtkHTML library.

    Copyright (C) 2001 Ximian, Inc.
    Authors:           Radek Doulik (rodo@ximian.com)

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
#include <string.h>
#include <glade/glade.h>
#include "gi-color-combo.h"

#include "gtkhtml/gtkhtml.h"

#include "gtkhtml/htmlclue.h"
#include "gtkhtml/htmlcolor.h"
#include "gtkhtml/htmlcolorset.h"
#include "gtkhtml/htmlcursor.h"
#include "gtkhtml/htmlengine.h"
#include "gtkhtml/htmlengine-edit-table.h"
#include "gtkhtml/htmlengine-save.h"
#include "gtkhtml/htmlimage.h"
#include "gtkhtml/htmltable.h"
#include "gtkhtml/htmlsettings.h"

#include "properties.h"
#include "table.h"
#include "utils.h"

#define ICONDIR "./art"

typedef struct
{	
	GSHTMLEditorControlData *cd;

	HTMLTable *table;

	GtkWidget *combo_bg_color;
	GtkWidget *entry_bg_pixmap;
	GtkWidget *spin_spacing;
	GtkWidget *spin_padding;
	GtkWidget *spin_border;
	GtkWidget *option_align;
	GtkWidget *spin_width;
	GtkWidget *check_width;
	GtkWidget *option_width;
	GtkWidget *spin_cols;
	GtkWidget *spin_rows;

	gboolean   disable_change;
} GtkHTMLEditTableProperties;

static GtkHTMLEditTableProperties *
data_new (GSHTMLEditorControlData *cd, HTMLTable *table)
{
	GtkHTMLEditTableProperties *data = g_new0 (GtkHTMLEditTableProperties, 1);

	/* fill data */
	data->cd = cd;
	data->table = table;

	return data;
}

static void
changed_bg_color (GtkWidget *w, GdkColor *color, gboolean custom, gboolean by_user, gboolean is_default, GtkHTMLEditTableProperties *d)
{
	/* If the color was changed programatically there's not need to set things */
	if (!by_user)
		return;

	html_engine_table_set_bg_color (d->cd->html->engine, d->table, color);		
}

static void
changed_bg_pixmap (GtkWidget *w, GtkHTMLEditTableProperties *d)
{
	char *url;
	const char *file;

	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	html_cursor_forward (d->cd->html->engine->cursor, d->cd->html->engine);
	file = gtk_entry_get_text (GTK_ENTRY (w));
	if (file && *file)
		url = g_strconcat ("file://", file, NULL);
	else
		url = NULL;
	html_engine_table_set_bg_pixmap (d->cd->html->engine, d->table, url);
	g_free (url);
}

static void
changed_spacing (GtkWidget *w, GtkHTMLEditTableProperties *d)
{
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	html_cursor_forward (d->cd->html->engine->cursor, d->cd->html->engine);
	html_engine_table_set_spacing (d->cd->html->engine, d->table, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_spacing)), FALSE);
}

static void
changed_padding (GtkWidget *w, GtkHTMLEditTableProperties *d)
{
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	html_cursor_forward (d->cd->html->engine->cursor, d->cd->html->engine);
	html_engine_table_set_padding (d->cd->html->engine, d->table, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_padding)), FALSE);
}

static void
changed_border (GtkWidget *w, GtkHTMLEditTableProperties *d)
{
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	html_cursor_forward (d->cd->html->engine->cursor, d->cd->html->engine);
	html_engine_table_set_border_width (d->cd->html->engine, d->table, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_border)), FALSE);
}

static void
changed_align (GtkWidget *w, GtkHTMLEditTableProperties *d)
{
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	html_cursor_forward (d->cd->html->engine->cursor, d->cd->html->engine);
	html_engine_table_set_align (d->cd->html->engine, d->table,
					     g_list_index (GTK_MENU_SHELL (w)->children, gtk_menu_get_active (GTK_MENU (w))) + HTML_HALIGN_LEFT);
}

static void
set_width (GtkHTMLEditTableProperties *d)
{
	GtkWidget *menu;

	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	html_cursor_forward (d->cd->html->engine->cursor, d->cd->html->engine);
	menu = gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_width));
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (d->check_width))) {
		if (g_list_index (GTK_MENU_SHELL (menu)->children, gtk_menu_get_active (GTK_MENU (menu))))
			html_engine_table_set_width (d->cd->html->engine, d->table, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_width)), TRUE);
		else
			html_engine_table_set_width (d->cd->html->engine, d->table, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_width)), FALSE);
	} else
			html_engine_table_set_width (d->cd->html->engine, d->table, 0, FALSE);
}

static void
changed_width (GtkWidget *w, GtkHTMLEditTableProperties *d)
{
	set_width (d);
}

static void
set_has_width (GtkWidget *check, GtkHTMLEditTableProperties *d)
{
	set_width (d);
}

static void
changed_width_percent (GtkWidget *w, GtkHTMLEditTableProperties *d)
{
	set_width (d);
}

static void
changed_cols (GtkWidget *w, GtkHTMLEditTableProperties *d)
{
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	html_cursor_forward (d->cd->html->engine->cursor, d->cd->html->engine);
	html_engine_table_set_cols (d->cd->html->engine, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_cols)));
}

static void
changed_rows (GtkWidget *w, GtkHTMLEditTableProperties *d)
{
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	html_cursor_forward (d->cd->html->engine->cursor, d->cd->html->engine);
	html_engine_table_set_rows (d->cd->html->engine, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_rows)));
}

/*
 * FIX: set spin adjustment upper to 100000
 *      as glade cannot set it now
 */
#define UPPER_FIX(x) gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (d->spin_ ## x))->upper = 100000.0

static GtkWidget *
table_widget (GtkHTMLEditTableProperties *d)
{
	GtkWidget *table_page;
	GladeXML *xml;

	xml = glade_xml_new ("gtkhtml-editor-properties.glade", "table_page", GETTEXT_PACKAGE);
	if (!xml)
		g_error (_("Could not load glade file."));

	table_page = glade_xml_get_widget (xml, "table_page");

	gtk_box_pack_start (GTK_BOX (glade_xml_get_widget (xml, "table_rows_hbox")),
			    gtk_image_new_from_file (ICONDIR "/table-row-16.png"),
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (glade_xml_get_widget (xml, "table_cols_hbox")),
			    gtk_image_new_from_file (ICONDIR "/table-column-16.png"),
			    FALSE, FALSE, 0);

	d->combo_bg_color = color_combo_new (NULL, _("Transparent"), NULL,
					     color_group_fetch ("table_bg_color", d->cd));
        color_combo_box_set_preview_relief (COLOR_COMBO (d->combo_bg_color), GTK_RELIEF_NORMAL); \
        g_signal_connect (d->combo_bg_color, "color_changed", G_CALLBACK (changed_bg_color), d);
	gtk_box_pack_start (GTK_BOX (glade_xml_get_widget (xml, "bg_color_hbox")), d->combo_bg_color, FALSE, FALSE, 0);

	d->entry_bg_pixmap = glade_xml_get_widget (xml, "entry_table_bg_pixmap");
	g_signal_connect (gnome_file_entry_gtk_entry (GNOME_FILE_ENTRY (d->entry_bg_pixmap)),
			  "changed", G_CALLBACK (changed_bg_pixmap), d);

	d->spin_spacing = glade_xml_get_widget (xml, "spin_spacing");
	g_signal_connect (d->spin_spacing, "value_changed", G_CALLBACK (changed_spacing), d);
	d->spin_padding = glade_xml_get_widget (xml, "spin_padding");
	g_signal_connect (d->spin_padding, "value_changed", G_CALLBACK (changed_padding), d);
	d->spin_border  = glade_xml_get_widget (xml, "spin_border");
	g_signal_connect (d->spin_border, "value_changed", G_CALLBACK (changed_border), d);
	UPPER_FIX (padding);
	UPPER_FIX (spacing);
	UPPER_FIX (border);

	d->option_align = glade_xml_get_widget (xml, "option_table_align");
	g_signal_connect (gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_align)), "selection-done",
			  G_CALLBACK (changed_align), d);

	d->spin_width   = glade_xml_get_widget (xml, "spin_table_width");
	g_signal_connect (d->spin_width, "value_changed", G_CALLBACK (changed_width), d);
	UPPER_FIX (width);
	d->check_width  = glade_xml_get_widget (xml, "check_table_width");
	g_signal_connect (d->check_width, "toggled", G_CALLBACK (set_has_width), d);
	d->option_width = glade_xml_get_widget (xml, "option_table_width");
	g_signal_connect (gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_width)), "selection-done",
			  G_CALLBACK (changed_width_percent), d);

	d->spin_cols = glade_xml_get_widget (xml, "spin_table_columns");
	g_signal_connect (d->spin_cols, "value_changed", G_CALLBACK (changed_cols), d);
	d->spin_rows = glade_xml_get_widget (xml, "spin_table_rows");
	g_signal_connect (d->spin_rows, "value_changed", G_CALLBACK (changed_rows), d);
	UPPER_FIX (cols);
	UPPER_FIX (rows);

	gtk_widget_show_all (table_page);
	gnome_pixmap_entry_set_preview (GNOME_PIXMAP_ENTRY (d->entry_bg_pixmap), FALSE);

	return table_page;
}

static void
set_ui (GtkHTMLEditTableProperties *d)
{
	if (editor_has_html_object (d->cd, HTML_OBJECT (d->table))) {
		HTMLHAlignType halign;
		int width = 0;
		gboolean percent = FALSE, has_width = FALSE;

		d->disable_change = TRUE;

		html_cursor_forward (d->cd->html->engine->cursor, d->cd->html->engine);
		color_combo_set_color (COLOR_COMBO (d->combo_bg_color), d->table->bgColor);

		if (d->table->bgPixmap) {
			int off = 0;

			if (!strncasecmp ("file://", d->table->bgPixmap->url, 7))
				off = 7;
			else if (!strncasecmp ("file:", d->table->bgPixmap->url, 5))
				off = 5;

			gtk_entry_set_text (GTK_ENTRY (gnome_file_entry_gtk_entry (GNOME_FILE_ENTRY (d->entry_bg_pixmap))),
					    d->table->bgPixmap->url + off);
		}

		gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_spacing), d->table->spacing);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_padding), d->table->padding);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_border),  d->table->border);

		g_return_if_fail (HTML_OBJECT (d->table)->parent);
		halign = HTML_CLUE (HTML_OBJECT (d->table)->parent)->halign;
		if (halign == HTML_HALIGN_NONE)
			halign = HTML_HALIGN_LEFT;
		gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_align), halign - HTML_HALIGN_LEFT);

		if (HTML_OBJECT (d->table)->percent) {
			width = HTML_OBJECT (d->table)->percent;
			percent = TRUE;
			has_width = TRUE;
		} else if (d->table->specified_width) {
			width = d->table->specified_width;
			percent = FALSE;
			has_width = TRUE;
		} else
			has_width = FALSE;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_width), has_width);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_width),  width);
		if (percent)
			gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_width), 1);
		else
			gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_width), 0);

		gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_cols),  d->table->totalCols);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_rows),  d->table->totalRows);

		d->disable_change = FALSE;
	}
}

GtkWidget *
table_properties (GSHTMLEditorControlData *cd, gpointer *set_data)
{
	GtkHTMLEditTableProperties *data = data_new (cd, html_engine_get_table (cd->html->engine));
	GtkWidget *rv;

	*set_data = data;
	rv = table_widget (data);
	set_ui (data);

	return rv;
}

void
table_close_cb (GSHTMLEditorControlData *cd, gpointer get_data)
{
	g_free (get_data);
}
