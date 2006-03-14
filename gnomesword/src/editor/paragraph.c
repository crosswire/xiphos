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
#include "gtkhtml/htmlengine-edit-clueflowstyle.h"
#include "gtkhtml/htmlengine-save.h"
#include "gtkhtml/htmlselection.h"

#include "paragraph.h"
#include "properties.h"
#include "utils.h"

struct _GtkHTMLEditParagraphProperties {
	GSHTMLEditorControlData *cd;
	GtkWidget *style_option;
};
typedef struct _GtkHTMLEditParagraphProperties GtkHTMLEditParagraphProperties;

static void
set_style (GtkWidget *w, GtkHTMLEditParagraphProperties *data)
{
	GtkHTMLParagraphStyle style = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (w), "style"));

	if (gtk_html_get_paragraph_style (data->cd->html) != style)
		gtk_html_set_paragraph_style (data->cd->html, style);
}

static void
set_align (GtkWidget *w, GtkHTMLEditParagraphProperties *data)
{
	GtkHTMLParagraphAlignment align = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (w), "align"));

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)) && gtk_html_get_paragraph_alignment (data->cd->html) != align)
		gtk_html_set_paragraph_alignment (data->cd->html, align);
}

GtkWidget *
paragraph_properties (GSHTMLEditorControlData *cd, gpointer *set_data)
{
	GtkHTMLEditParagraphProperties *data = g_new0 (GtkHTMLEditParagraphProperties, 1);
	GtkWidget *hbox, *menu, *menuitem, *vbox, *radio, *table, *icon;
	GSList *group;
	gint h=0, i=0;

	*set_data = data;
	data->cd = cd;

	table = gtk_table_new (2, 1, FALSE);
	gtk_table_set_col_spacings (GTK_TABLE (table), 18);
	gtk_table_set_row_spacings (GTK_TABLE (table), 18);

	menu = gtk_menu_new ();

#define ADD_SEP menuitem = gtk_separator_menu_item_new (); gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem); gtk_widget_show (menuitem)
#undef ADD_ITEM
#define ADD_ITEM(n,s) \
	menuitem = gtk_menu_item_new_with_label (n); \
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem); \
        gtk_widget_show (menuitem); \
        if (gtk_html_get_paragraph_style (data->cd->html) == s) h=i; i++; \
        g_signal_connect (menuitem, "activate", G_CALLBACK (set_style), data); \
        g_object_set_data (G_OBJECT (menuitem), "style", GINT_TO_POINTER (s));

	ADD_ITEM (_("Normal"),       GTK_HTML_PARAGRAPH_STYLE_NORMAL);
	ADD_SEP;
	if (cd->format_html) {
		ADD_ITEM (_("Header 1"),     GTK_HTML_PARAGRAPH_STYLE_H1);
		ADD_ITEM (_("Header 2"),     GTK_HTML_PARAGRAPH_STYLE_H2);
		ADD_ITEM (_("Header 3"),     GTK_HTML_PARAGRAPH_STYLE_H3);
		ADD_ITEM (_("Header 4"),     GTK_HTML_PARAGRAPH_STYLE_H4);
		ADD_ITEM (_("Header 5"),     GTK_HTML_PARAGRAPH_STYLE_H5);
		ADD_ITEM (_("Header 6"),     GTK_HTML_PARAGRAPH_STYLE_H6);
		ADD_SEP;
	}
	ADD_ITEM (_("Dot item"),     GTK_HTML_PARAGRAPH_STYLE_ITEMDOTTED);
	ADD_ITEM (_("Number item"),   GTK_HTML_PARAGRAPH_STYLE_ITEMDIGIT);
	ADD_ITEM (_("Roman item"),   GTK_HTML_PARAGRAPH_STYLE_ITEMROMAN);
	ADD_ITEM (_("Alphabeta item"),   GTK_HTML_PARAGRAPH_STYLE_ITEMALPHA);
	ADD_SEP;
	if (cd->format_html) {
		ADD_ITEM (_("Address"),      GTK_HTML_PARAGRAPH_STYLE_ADDRESS);
	}
	ADD_ITEM (_("Preformatted"),          GTK_HTML_PARAGRAPH_STYLE_PRE);

	data->style_option = gtk_option_menu_new ();
	gtk_option_menu_set_menu (GTK_OPTION_MENU (data->style_option), menu);
	gtk_option_menu_set_history (GTK_OPTION_MENU (data->style_option), h);

	hbox = gtk_hbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (hbox), gtk_label_new_with_mnemonic (_("_Style:")), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), data->style_option, FALSE, FALSE, 0);

	gtk_table_attach (GTK_TABLE (table), editor_hig_vbox (_("General"), hbox), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

	hbox = gtk_hbox_new (FALSE, 12);

#define ADD_RADIO(x,a,icon_name) \
	radio = gtk_radio_button_new_with_label (group, x); \
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio)); \
        icon = gtk_image_new_from_file (PACKAGE_PIXMAPS_DIR"/gs2-48x48.png"); \
	gtk_box_pack_start (GTK_BOX (hbox), icon, FALSE, FALSE, 0); \
	gtk_box_pack_start (GTK_BOX (hbox), radio, FALSE, FALSE, 0); \
        if (a == gtk_html_get_paragraph_alignment (data->cd->html)) gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio), TRUE); \
        g_signal_connect (radio, "toggled", G_CALLBACK (set_align), data); \
        g_object_set_data (G_OBJECT (radio), "align", GINT_TO_POINTER (a));

	group = NULL;
	ADD_RADIO (_("Left"), GTK_HTML_PARAGRAPH_ALIGNMENT_LEFT, "left");
	ADD_RADIO (_("Center"), GTK_HTML_PARAGRAPH_ALIGNMENT_CENTER, "center");
	ADD_RADIO (_("Right"), GTK_HTML_PARAGRAPH_ALIGNMENT_RIGHT, "right");

	gtk_table_attach (GTK_TABLE (table), editor_hig_vbox (_("Alignment"), hbox), 0, 1, 1, 2, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 12);
	gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0);
	gtk_widget_show_all (vbox);

	return vbox;
}

void
paragraph_close_cb (GSHTMLEditorControlData *cd, gpointer get_data)
{
	g_free (get_data);
}
#endif
