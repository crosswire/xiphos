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

#include <gtkhtml/htmlclue.h>
#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlcursor.h>
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-edit-table.h>
#include <gtkhtml/htmlengine-edit-tablecell.h>
#include <gtkhtml/htmlengine-save.h>
#include "gtkhtml/htmlimage.h"
#include "gtkhtml/htmltable.h"
#include "gtkhtml/htmltablecell.h"
#include "gtkhtml/htmlsettings.h"

#include "properties.h"
#include "cell.h"
#include "utils.h"

#define ICONDIR "./art"

typedef enum
{
	CELL_SCOPE_CELL,
	CELL_SCOPE_ROW,
	CELL_SCOPE_COLUMN,
	CELL_SCOPE_TABLE
} CellScope;

typedef struct
{	
	GSHTMLEditorControlData *cd;
	HTMLTableCell *cell;
	HTMLTable *table;
	CellScope  scope;

	GtkWidget *combo_bg_color;
	GtkWidget *entry_bg_pixmap;
	GtkWidget *option_halign;
	GtkWidget *option_valign;
	GtkWidget *spin_width;
	GtkWidget *check_width;
	GtkWidget *option_width;
	GtkWidget *spin_cspan;
	GtkWidget *spin_rspan;
	GtkWidget *check_wrap;
	GtkWidget *check_header;

	gboolean   disable_change;

} GtkHTMLEditCellProperties;

static GtkHTMLEditCellProperties *
data_new (GSHTMLEditorControlData *cd, HTMLTableCell *cell)
{
	GtkHTMLEditCellProperties *data = g_new0 (GtkHTMLEditCellProperties, 1);

	/* fill data */
	data->cd = cd;
	data->scope = CELL_SCOPE_CELL;
	data->cell = cell;
	g_return_val_if_fail (data->cell, NULL);
	data->table = HTML_TABLE (HTML_OBJECT (data->cell)->parent);
	g_return_val_if_fail (data->table && HTML_IS_TABLE (data->table), NULL);

	return data;
}

static void
cell_set_prop (GtkHTMLEditCellProperties *d, void (*set_fn)(HTMLTableCell *, GtkHTMLEditCellProperties *))
{
	HTMLEngine *e = d->cd->html->engine;
	guint position;
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	position = d->cd->html->engine->cursor->position;
	switch (d->scope) {
	case CELL_SCOPE_CELL:
		(*set_fn) (d->cell, d);
		break;
	case CELL_SCOPE_ROW:
		if (html_engine_table_goto_row (e, d->table, d->cell->row)) {
			HTMLTableCell *cell = html_engine_get_table_cell (e);

			while (cell && cell->row == d->cell->row) {
				if (HTML_OBJECT (cell)->parent == HTML_OBJECT (d->table))
					(*set_fn) (cell, d);
				html_engine_next_cell (e, FALSE);
				cell = html_engine_get_table_cell (e);
			}
		}
		break;
	case CELL_SCOPE_COLUMN:
		if (html_engine_table_goto_col (e, d->table, d->cell->col)) {
			HTMLTableCell *cell = html_engine_get_table_cell (e);

			while (cell) {
				if (cell->col == d->cell->col && HTML_OBJECT (cell)->parent == HTML_OBJECT (d->table))
					(*set_fn) (cell, d);
				html_engine_next_cell (e, FALSE);
				cell = html_engine_get_table_cell (e);
			}
		}
		break;
	case CELL_SCOPE_TABLE:
		if (html_engine_goto_table_0 (e, d->table)) {
			HTMLTableCell *cell;

			html_cursor_forward (e->cursor, e);
			cell = html_engine_get_table_cell (e);
			while (cell) {
				if (HTML_OBJECT (cell)->parent == HTML_OBJECT (d->table))
					(*set_fn) (cell, d);
				html_engine_next_cell (e, FALSE);
				cell = html_engine_get_table_cell (e);
			}
		}
		break;
	}

	html_cursor_jump_to_position (e->cursor, e, position);
}

static void
set_bg_color (HTMLTableCell *cell, GtkHTMLEditCellProperties *d)
{
	html_engine_table_cell_set_bg_color (d->cd->html->engine, cell, color_combo_get_color (COLOR_COMBO (d->combo_bg_color), NULL));
}

static void
changed_bg_color (GtkWidget *w, GdkColor *color, gboolean custom, gboolean by_user, gboolean is_default, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_bg_color);
}

static void
set_bg_pixmap (HTMLTableCell *cell, GtkHTMLEditCellProperties *d)
{
	const char *file;
	char *url = NULL;

	file = gtk_entry_get_text (GTK_ENTRY (gnome_file_entry_gtk_entry (GNOME_FILE_ENTRY (d->entry_bg_pixmap))));
	if (file && *file)
		url = g_strconcat ("file://", file, NULL);

	html_engine_table_cell_set_bg_pixmap (d->cd->html->engine, cell, url);
	g_free (url);
}

static void
changed_bg_pixmap (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_bg_pixmap);
}

static void
set_halign (HTMLTableCell *cell, GtkHTMLEditCellProperties *d)
{
	GtkWidget *menu = gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_halign));
	html_engine_table_cell_set_halign (d->cd->html->engine, cell,
					   g_list_index (GTK_MENU_SHELL (menu)->children, gtk_menu_get_active (GTK_MENU (menu))) + HTML_HALIGN_LEFT);
}

static void
changed_halign (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_halign);
}

static void
set_valign (HTMLTableCell *cell, GtkHTMLEditCellProperties *d)
{
	GtkWidget *menu = gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_valign));
	html_engine_table_cell_set_valign (d->cd->html->engine, cell,
					   g_list_index (GTK_MENU_SHELL (menu)->children, gtk_menu_get_active (GTK_MENU (menu))) + HTML_VALIGN_TOP);
}

static void
changed_valign (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_valign);
}

static void
set_cspan (HTMLTableCell *cell, GtkHTMLEditCellProperties *d)
{
	html_engine_set_cspan (d->cd->html->engine, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_cspan)));
}

static void
changed_cspan (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_cspan);
}

static void
set_rspan (HTMLTableCell *cell, GtkHTMLEditCellProperties *d)
{
	html_engine_set_rspan (d->cd->html->engine, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_rspan)));
}

static void
changed_rspan (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_rspan);
}

static void
set_width (HTMLTableCell *cell, GtkHTMLEditCellProperties *d)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (d->check_width))) {
		GtkWidget *menu = gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_width));
		if (g_list_index (GTK_MENU_SHELL (menu)->children, gtk_menu_get_active (GTK_MENU (menu))))
			html_engine_table_cell_set_width (d->cd->html->engine, cell,
							  gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_width)), TRUE);
		else
			html_engine_table_cell_set_width (d->cd->html->engine, cell,
							  gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_width)), FALSE);
	} else
		html_engine_table_cell_set_width (d->cd->html->engine, cell, 0, FALSE);
}

static void
changed_width (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_width);
}

static void
set_has_width (GtkWidget *check, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_width);
}

static void
changed_width_percent (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_width);
}

static void
set_wrap (HTMLTableCell *cell, GtkHTMLEditCellProperties *d)
{
	html_engine_table_cell_set_no_wrap (d->cd->html->engine, cell, !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (d->check_wrap)));
}

static void
changed_wrap (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_wrap);
}

static void
set_header (HTMLTableCell *cell, GtkHTMLEditCellProperties *d)
{
	html_engine_table_cell_set_heading (d->cd->html->engine, cell, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (d->check_header)));
}

static void
changed_heading (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	cell_set_prop (d, set_header);
}

/*
 * FIX: set spin adjustment upper to 100000
 *      as glade cannot set it now
 */
#define UPPER_FIX(x) gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (d->spin_ ## x))->upper = 100000.0

static void
cell_scope_cell (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)))
		d->scope = CELL_SCOPE_CELL;
}

static void
cell_scope_table (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)))
		d->scope = CELL_SCOPE_TABLE;
}

static void
cell_scope_row (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)))
		d->scope = CELL_SCOPE_ROW;
}

static void
cell_scope_column (GtkWidget *w, GtkHTMLEditCellProperties *d)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)))
		d->scope = CELL_SCOPE_COLUMN;
}

static GtkWidget *
cell_widget (GtkHTMLEditCellProperties *d)
{
	GtkWidget *cell_page;
	GladeXML *xml;

	xml = glade_xml_new ("gtkhtml-editor-properties.glade", "cell_page", GETTEXT_PACKAGE);
	if (!xml)
		g_error (_("Could not load glade file."));

	cell_page          = glade_xml_get_widget (xml, "cell_page");

	gtk_table_attach (GTK_TABLE (glade_xml_get_widget (xml, "cell_scope_table1")),
			  gtk_image_new_from_file (gnome_icon_theme_lookup_icon (d->cd->icon_theme, "stock_select-cell", 16, NULL, NULL)),
			  0, 1, 0, 1, 0, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (glade_xml_get_widget (xml, "cell_scope_table1")),
			  gtk_image_new_from_file (ICONDIR "/table-table-16.png"),
			  0, 1, 1, 2, 0, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (glade_xml_get_widget (xml, "cell_scope_table2")),
			  gtk_image_new_from_file (ICONDIR "/table-row-16.png"),
			  0, 1, 0, 1, 0, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (glade_xml_get_widget (xml, "cell_scope_table2")),
			  gtk_image_new_from_file (ICONDIR "/table-column-16.png"),
			  0, 1, 1, 2, 0, 0, 0, 0);

	d->combo_bg_color = color_combo_new (NULL, _("Transparent"), NULL,
					     color_group_fetch ("cell_bg_color", d->cd));
        color_combo_box_set_preview_relief (COLOR_COMBO (d->combo_bg_color), GTK_RELIEF_NORMAL); \
        g_signal_connect (d->combo_bg_color, "color_changed", G_CALLBACK (changed_bg_color), d);
	gtk_box_pack_start (GTK_BOX (glade_xml_get_widget (xml, "bg_color_hbox")), d->combo_bg_color, FALSE, FALSE, 0);

	d->entry_bg_pixmap = glade_xml_get_widget (xml, "entry_cell_bg_pixmap");
	g_signal_connect (gnome_file_entry_gtk_entry (GNOME_FILE_ENTRY (d->entry_bg_pixmap)),
			    "changed", G_CALLBACK (changed_bg_pixmap), d);

	d->option_halign = glade_xml_get_widget (xml, "option_cell_halign");
	g_signal_connect (gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_halign)), "selection-done",
			  G_CALLBACK (changed_halign), d);
	d->option_valign = glade_xml_get_widget (xml, "option_cell_valign");
	g_signal_connect (gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_valign)), "selection-done",
			  G_CALLBACK (changed_valign), d);

	d->spin_width   = glade_xml_get_widget (xml, "spin_cell_width");
	UPPER_FIX (width);
	g_signal_connect (d->spin_width, "value_changed", G_CALLBACK (changed_width), d);
	d->check_width  = glade_xml_get_widget (xml, "check_cell_width");
	g_signal_connect (d->check_width, "toggled", G_CALLBACK (set_has_width), d);
	d->option_width = glade_xml_get_widget (xml, "option_cell_width");
	g_signal_connect (gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_width)), "selection-done",
			  G_CALLBACK (changed_width_percent), d);

	d->check_wrap = glade_xml_get_widget (xml, "check_cell_wrap");
	d->check_header = glade_xml_get_widget (xml, "check_cell_header");
	g_signal_connect (d->check_wrap, "toggled", G_CALLBACK (changed_wrap), d);
	g_signal_connect (d->check_header, "toggled", G_CALLBACK (changed_heading), d);

        g_signal_connect (glade_xml_get_widget (xml, "cell_radio"), "toggled", G_CALLBACK (cell_scope_cell), d);
        g_signal_connect (glade_xml_get_widget (xml, "table_radio"), "toggled", G_CALLBACK (cell_scope_table), d);
        g_signal_connect (glade_xml_get_widget (xml, "row_radio"), "toggled", G_CALLBACK (cell_scope_row), d);
        g_signal_connect (glade_xml_get_widget (xml, "col_radio"), "toggled", G_CALLBACK (cell_scope_column), d);

	d->spin_cspan   = glade_xml_get_widget (xml, "spin_cell_cspan");
	d->spin_rspan   = glade_xml_get_widget (xml, "spin_cell_rspan");
	g_signal_connect (d->spin_cspan, "value_changed", G_CALLBACK (changed_cspan), d);
	g_signal_connect (d->spin_rspan, "value_changed", G_CALLBACK (changed_rspan), d);

	gtk_widget_show_all (cell_page);
	gnome_pixmap_entry_set_preview (GNOME_PIXMAP_ENTRY (d->entry_bg_pixmap), FALSE);

	return cell_page;
}

static void
set_ui (GtkHTMLEditCellProperties *d)
{
	if (!editor_has_html_object (d->cd, HTML_OBJECT (d->table)))
		return;

	d->disable_change = TRUE;

	if (d->cell->have_bg)
		color_combo_set_color (COLOR_COMBO (d->combo_bg_color), &d->cell->bg);

	if (d->cell->have_bgPixmap) {
		int off = 0;

		if (!strncasecmp ("file://", d->cell->bgPixmap->url, 7))
			off = 7;
		else if (!strncasecmp ("file:", d->cell->bgPixmap->url, 5))
			off = 5;
		gtk_entry_set_text (GTK_ENTRY (gnome_file_entry_gtk_entry (GNOME_FILE_ENTRY (d->entry_bg_pixmap))),
				    d->cell->bgPixmap->url + off);
	}

	if (HTML_CLUE (d->cell)->halign == HTML_HALIGN_NONE)
		gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_halign), HTML_HALIGN_LEFT);
	else
		gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_halign), HTML_CLUE (d->cell)->halign - HTML_HALIGN_LEFT);
	gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_valign), HTML_CLUE (d->cell)->valign - HTML_VALIGN_TOP);

	if (d->cell->percent_width) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_width), TRUE);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_width), d->cell->fixed_width);
		gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_width), 1);
	} else if (d->cell->fixed_width) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_width), TRUE);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_width), d->cell->fixed_width);
		gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_width), 0);
	} else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_width), FALSE);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_wrap), !d->cell->no_wrap);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_header), d->cell->heading);

	gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_cspan),  d->cell->cspan);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_rspan),  d->cell->rspan);

	d->disable_change = FALSE;
}

GtkWidget *
cell_properties (GSHTMLEditorControlData *cd, gpointer *set_data)
{
	GtkHTMLEditCellProperties *data = data_new (cd, html_engine_get_table_cell (cd->html->engine));
	GtkWidget *rv;

	*set_data = data;
	rv        = cell_widget (data);
	set_ui (data);

	return rv;
}

void
cell_close_cb (GSHTMLEditorControlData *cd, gpointer get_data)
{
	g_free (get_data);
}
