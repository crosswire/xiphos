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
#include <string.h>
#include "gi-color-combo.h"
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-edit-clueflowstyle.h>
#include <gtkhtml/htmlimage.h>
#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlsettings.h>
#include "body.h"
#include "properties.h"
#include "utils.h"
//#define ICONDIR "./art"
struct _GtkHTMLEditBodyProperties {
	GSHTMLEditorControlData *cd;

	GtkWidget *pixmap_entry;
	GtkWidget *option_template;
	GtkWidget *combo [3];
	GtkWidget *entry_title;
};
typedef struct _GtkHTMLEditBodyProperties GtkHTMLEditBodyProperties;

#define TEMPLATES 9
typedef struct {
	gchar *name;
	gchar *bg_pixmap;
	GdkColor bg_color;
	GdkColor text_color;
	GdkColor link_color;
	gint left_margin;
} BodyTemplate;

static BodyTemplate body_templates [TEMPLATES] = {
	{
		N_("None"),
		NULL,
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		10,
	},
	{
		N_("Perforated Paper"),
		GTKHTML_DATA_DIR"/icons/paper.png",
		{0, 0xffff, 0xffff, 0xffff},
		{0, 0, 0, 0},
		{0, 0, 0x3380, 0x6680},
		30,
	},
	{
		N_("Blue Ink"),
		GTKHTML_DATA_DIR"/icons/texture.png",
		{0, 0xffff, 0xffff, 0xffff},
		{0, 0x1fff, 0x1fff, 0x8fff},
		{0, 0, 0, 0xffff},
		10,
	},
	{
		N_("Paper"),
		GTKHTML_DATA_DIR"/icons/rect.png",
		{0, 0xffff, 0xffff, 0xffff},
		{0, 0, 0, 0},
		{0, 0, 0, 0xffff},
		10,
  	},
	{
		N_("Ribbon"),
		GTKHTML_DATA_DIR"/icons/ribbon.jpg",
		{0, 0xffff, 0xffff, 0xffff},
		{0, 0, 0, 0},
		{0, 0x9900, 0x3300, 0x6600},
		70,
  	},
	{
		N_("Midnight"),
		GTKHTML_DATA_DIR"/icons/midnight-stars.jpg",
		{0, 0, 0, 0},
		{0, 0xffff, 0xffff, 0xffff},
		{0, 0xffff, 0x9900, 0},
		10,
  	},
	{
		N_("Confidential"),
		GTKHTML_DATA_DIR"/icons/confidential-stamp.jpg",
		{0, 0xffff, 0xffff, 0xffff},
		{0, 0, 0, 0},
		{0, 0, 0, 0xffff},
		10,
  	},
	{
		N_("Draft"),
		GTKHTML_DATA_DIR"/icons/draft-stamp.jpg",
		{0, 0xffff, 0xffff, 0xffff},
		{0, 0, 0, 0},
		{0, 0, 0, 0xffff},
		10,
  	},
	{
		N_("Graph paper"),
		GTKHTML_DATA_DIR"/icons/draft-paper.png",
		{0, 0xffff, 0xffff, 0xffff},
		{0, 0, 0, 0x8000},
		{0, 0xe300, 0x2100, 0x2300},
		10,
  	},
};

static void
color_changed (GtkWidget *w, GdkColor *color, gboolean custom, gboolean by_user, gboolean is_default,
	       GtkHTMLEditBodyProperties *data)
{
	gint idx;
		
	idx = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (w), "type"));
	if (color)
		html_colorset_set_color (data->cd->html->engine->settings->color_set, color, idx);
	else
		html_colorset_set_color (data->cd->html->engine->settings->color_set,
					 &html_colorset_get_color (data->cd->html->engine->defaultSettings->color_set, idx)->color,
					 idx);
	html_object_change_set_down (data->cd->html->engine->clue, HTML_CHANGE_RECALC_PI);
	gtk_widget_queue_draw (GTK_WIDGET (data->cd->html));
}

static void
entry_changed (GtkWidget *w, GtkHTMLEditBodyProperties *data)
{
	HTMLEngine *e = data->cd->html->engine;
	const char *fname;

	/* FIXME: extend the API with bg image query/setting */
	if (e->bgPixmapPtr != NULL) {
		html_image_factory_unregister (e->image_factory, e->bgPixmapPtr, NULL);
		e->bgPixmapPtr = NULL;
	}

	fname = gtk_entry_get_text (GTK_ENTRY (w));
	if (fname && *fname) {
		gchar *file = g_strconcat ("file:", fname, NULL);

		e->bgPixmapPtr = html_image_factory_register (e->image_factory, NULL, file, TRUE);
		g_free (file);
	}
	gtk_widget_queue_draw (GTK_WIDGET (data->cd->html));
}

static void
changed_template (GtkWidget *w, GtkHTMLEditBodyProperties *d)
{
	gint template, left_margin = 10;

	template = g_list_index (GTK_MENU_SHELL (w)->children, gtk_menu_get_active (GTK_MENU (w)));

	gtk_entry_set_text (GTK_ENTRY (gnome_file_entry_gtk_entry (GNOME_FILE_ENTRY (d->pixmap_entry))),
			    body_templates [template].bg_pixmap ? body_templates [template].bg_pixmap : "");

	if (template) {
		color_combo_set_color (COLOR_COMBO (d->combo [2]), &body_templates [template].bg_color);
		color_combo_set_color (COLOR_COMBO (d->combo [0]), &body_templates [template].text_color);
		color_combo_set_color (COLOR_COMBO (d->combo [1]), &body_templates [template].link_color);
		left_margin = body_templates [template].left_margin;
	} else {
		color_combo_set_color (COLOR_COMBO (d->combo [2]),
				       &html_colorset_get_color_allocated (d->cd->html->engine->settings->color_set,
									   d->cd->html->engine->painter,
									   HTMLBgColor)->color);
		color_combo_set_color (COLOR_COMBO (d->combo [0]),
				       &html_colorset_get_color_allocated (d->cd->html->engine->settings->color_set,
									   d->cd->html->engine->painter,
									   HTMLTextColor)->color);
		color_combo_set_color (COLOR_COMBO (d->combo [1]),
				       &html_colorset_get_color_allocated (d->cd->html->engine->settings->color_set,
									   d->cd->html->engine->painter,
									   HTMLLinkColor)->color);
	}

	/* FIXME: add API for margins query/setting to libgtkhtml */
	d->cd->html->engine->leftBorder = left_margin;
}

static void
fill_templates (GtkHTMLEditBodyProperties *d)
{
	GtkWidget *menu;
	gint i;

	menu = gtk_menu_new ();

	for (i = 0; i < TEMPLATES; i ++) {
		GtkWidget *item;

		item = gtk_menu_item_new_with_label (_(body_templates [i].name));
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
		gtk_widget_show (item);
	}
	gtk_option_menu_set_menu (GTK_OPTION_MENU (d->option_template), menu);
}

GtkWidget *
body_properties (GSHTMLEditorControlData *cd, gpointer *set_data)
{
	GtkHTMLEditBodyProperties *data = g_new0 (GtkHTMLEditBodyProperties, 1);
	GtkWidget *main_vbox, *hbox, *combo, *label, *t1;
	GSList *group;
	HTMLColor *color;
	gint i;

	*set_data = data;
	data->cd = cd;

	main_vbox = gtk_vbox_new (FALSE, 12);
	gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 12);

	t1 = gtk_table_new (2, 2, FALSE);
	gtk_table_set_col_spacings (GTK_TABLE (t1), 6);
	gtk_table_set_row_spacings (GTK_TABLE (t1), 6);

	data->option_template = gtk_option_menu_new ();
	fill_templates (data);
	hbox = gtk_hbox_new (FALSE, 1);
	gtk_box_pack_start (GTK_BOX (hbox), data->option_template, FALSE, FALSE, 0);
	editor_hig_attach_row (t1, _("T_emplate:"), hbox, 0);
	
	/* data->entry_title = gtk_entry_new ();
	if (gtk_html_get_title (data->cd->html)) {
		gtk_entry_set_text (GTK_ENTRY (data->entry_title), 
				    gtk_html_get_title (data->cd->html));
	}
	g_signal_connect (data->entry_title, "changed", G_CALLBACK (entry_changed), data);
	editor_hig_attach_row (t1, _("T_itle:"), data->entry_title, 1);	*/

	gtk_box_pack_start (GTK_BOX (main_vbox), editor_hig_vbox (_("General"), t1), FALSE, FALSE, 0);

	t1 = gtk_table_new (3, 2, FALSE);
	gtk_table_set_col_spacings (GTK_TABLE (t1), 6);
	gtk_table_set_row_spacings (GTK_TABLE (t1), 6);

	group = NULL;
	i = 0;
#define ADD_COLOR(x, ct, g) \
        color = html_colorset_get_color (cd->html->engine->settings->color_set, ct); \
        html_color_alloc (color, cd->html->engine->painter); \
	data->combo [i] = combo = color_combo_new (NULL, _("Automatic"), \
				 &color->color, \
				 color_group_fetch ("body_" g, cd)); \
        color_combo_box_set_preview_relief (COLOR_COMBO (data->combo [i]), GTK_RELIEF_NORMAL); \
        g_object_set_data (G_OBJECT (combo), "type", GINT_TO_POINTER (ct)); \
	hbox = gtk_hbox_new (FALSE, 3); \
        label = gtk_label_new_with_mnemonic (x); \
        gtk_misc_set_alignment (GTK_MISC (label), .0, .5); \
        gtk_table_attach (GTK_TABLE (t1), label, 0, 1, i, i + 1, GTK_FILL, GTK_FILL, 0, 0); \
        gtk_table_attach (GTK_TABLE (t1), combo, 1, 2, i, i + 1, GTK_FILL, GTK_FILL, 0, 0); \
        i ++

	ADD_COLOR (_("_Text:"), HTMLTextColor, "text");
	ADD_COLOR (_("_Link:"), HTMLLinkColor, "link");
	ADD_COLOR (_("_Background:"), HTMLBgColor, "bg");

	gtk_box_pack_start (GTK_BOX (main_vbox), editor_hig_vbox (_("Colors"), t1), FALSE, FALSE, 0);

	data->pixmap_entry = gnome_pixmap_entry_new ("background_image", _("Background Image"), FALSE);
	if (cd->html->engine->bgPixmapPtr) {
		HTMLImagePointer *ip = (HTMLImagePointer *) cd->html->engine->bgPixmapPtr;
		guint off = 0;
		 if (!strncmp (ip->url, "file:", 5))
			 off = 5;

		 gtk_entry_set_text (GTK_ENTRY (gnome_file_entry_gtk_entry (GNOME_FILE_ENTRY (data->pixmap_entry))),
				     ip->url + off);
	}


	hbox = gtk_hbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (hbox), gtk_label_new_with_mnemonic (_("_Source:")), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), data->pixmap_entry, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (main_vbox), editor_hig_vbox (_("Background Image"), hbox), FALSE, FALSE, 0);

	/* gtk_idle_add (hide_preview, data); */

	/* set ui */
#define SET_COLOR(ct) \
        color_combo_set_color (COLOR_COMBO (combo), &html_colorset_get_color_allocated (cd->html->engine->settings->color_set, cd->html->engine->painter, ct)->color);

	SET_COLOR (HTMLTextColor);
	SET_COLOR (HTMLLinkColor);
	SET_COLOR (HTMLBgColor);

	/* connect signal handlers */
	gtk_widget_show_all (main_vbox);

	g_signal_connect (gtk_option_menu_get_menu (GTK_OPTION_MENU (data->option_template)),
			  "selection-done", G_CALLBACK (changed_template), data);
        g_signal_connect (data->combo [0], "color_changed", G_CALLBACK (color_changed), data);
        g_signal_connect (data->combo [1], "color_changed", G_CALLBACK (color_changed), data);
        g_signal_connect (data->combo [2], "color_changed", G_CALLBACK (color_changed), data);
	g_signal_connect (gnome_file_entry_gtk_entry (GNOME_FILE_ENTRY (data->pixmap_entry)),
			  "changed", G_CALLBACK (entry_changed), data);

	return main_vbox;
}

void
body_close_cb (GSHTMLEditorControlData *cd, gpointer get_data)
{
	g_free (get_data);
}
