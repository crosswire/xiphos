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

#include <string.h>
#include "gtkhtml.h"
#include "htmlcursor.h"
#include "htmlengine-edit-fontstyle.h"
#include "htmlengine-edit-cut-and-paste.h"
#include "htmlengine-edit-rule.h"
#include "htmlengine-save.h"
#include "htmlrule.h"

#include "config.h"
#include "properties.h"
#include "dialog.h"
#include "rule.h"
#include "utils.h"

#define GTK_HTML_EDIT_RULE_WIDTH       0
#define GTK_HTML_EDIT_RULE_SIZE        1
#define GTK_HTML_EDIT_RULE_SPINS       2

struct _GtkHTMLEditRuleProperties {
	GtkHTMLControlData *cd;
	HTMLRule *rule;

	gint width;
	gint size;

	GtkWidget   *check [GTK_HTML_EDIT_RULE_SPINS];
	GtkWidget   *spin  [GTK_HTML_EDIT_RULE_SPINS];
	GtkObject   *adj   [GTK_HTML_EDIT_RULE_SPINS];
	gboolean     set   [GTK_HTML_EDIT_RULE_SPINS];

	gboolean percent;
	GtkWidget *width_measure;

	gboolean shaded;
	GtkWidget *shaded_check;

	HTMLHAlignType align;
	GtkWidget *align_option;

	GtkHTML *sample;

	gboolean disable_change;
};
typedef struct _GtkHTMLEditRuleProperties GtkHTMLEditRuleProperties;

#define CHANGE if (!d->disable_change) gtk_html_edit_properties_dialog_change (d->cd->properties_dialog)
#define FILL 	if (!d->disable_change) fill_sample (d)
#define VAL(x) (gint)GTK_ADJUSTMENT (d->adj [GTK_HTML_EDIT_RULE_ ## x])->value

static void
fill_sample (GtkHTMLEditRuleProperties *d)
{
	gchar *body, *width, *size, *align, *noshade, *bg;

	bg      = html_engine_save_get_sample_body (d->cd->html->engine, NULL);
	width   = d->set [GTK_HTML_EDIT_RULE_WIDTH]
		? g_strdup_printf (" width=%d%s", VAL (WIDTH),
				   d->percent ? "%" : "") : g_strdup ("");
	size    = d->set [GTK_HTML_EDIT_RULE_SIZE]
		? g_strdup_printf (" size=%d", VAL (SIZE))
		: g_strdup ("");
	noshade = g_strdup (d->shaded ? "" : " noshade");
	align   = d->align != HTML_HALIGN_CENTER ? g_strdup_printf (" align=%s",
								    d->align == HTML_HALIGN_LEFT ? "left" : "right")
		: g_strdup ("");
	body    = g_strconcat (bg, "<br><hr", width, size, align, noshade, ">", NULL);

	printf ("body: %s\n", body);

	gtk_html_load_from_string (d->sample, body, -1);

	g_free (bg);
	g_free (width);
	g_free (size);
	g_free (noshade);
	g_free (align);
	g_free (body);
}

static void
check_toggled (GtkWidget *check, GtkHTMLEditRuleProperties *d)
{
	guint idx;

	for (idx = 0; idx < GTK_HTML_EDIT_RULE_SPINS; idx++)
		if (check == d->check [idx])
			break;
	if (check != d->check [idx])
		g_assert_not_reached ();

	d->set [idx] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check));
	gtk_widget_set_sensitive (d->spin [idx], d->set [idx]);	
	FILL;
	CHANGE;
}

static void
adj_changed (GtkAdjustment *adj, GtkHTMLEditRuleProperties *d)
{
	FILL;
}

static void
checked_val (GtkHTMLEditRuleProperties *d, gint idx, const gchar *name)
{
	d->check [idx] = gtk_check_button_new_with_label (name);
	d->adj   [idx] = gtk_adjustment_new (0, 0, 32767, 1, 1, 1);
	d->spin  [idx] = gtk_spin_button_new (GTK_ADJUSTMENT (d->adj [idx]), 1, 0);

	gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (d->check [idx]), d->set [idx]);
	gtk_widget_set_sensitive (d->spin [idx], d->set [idx]);

	gtk_signal_connect (GTK_OBJECT (d->check [idx]), "toggled", GTK_SIGNAL_FUNC (check_toggled), d);
	gtk_signal_connect (GTK_OBJECT (d->adj [idx]), "value_changed", adj_changed, d);
}

static void
align_menu_activate (GtkWidget *mi, GtkHTMLEditRuleProperties *d)
{
	d->align = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (mi), "idx"));
	FILL;
	CHANGE;
}

static void
percent_menu_activate (GtkWidget *mi, GtkHTMLEditRuleProperties *d)
{
	d->percent = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (mi), "idx"));
	FILL;
	CHANGE;
}

static void
width_toggled (GtkWidget *check, GtkHTMLEditRuleProperties *d)
{
	gtk_widget_set_sensitive (d->width_measure,
				  gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check)));
	FILL;
	CHANGE;
}

static void
shade_toggled (GtkWidget *check, GtkHTMLEditRuleProperties *d)
{
	d->shaded = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check));
	FILL;
	CHANGE;
}

#define HBOX \
	hbox  = gtk_hbox_new (FALSE, 3); \
	gtk_container_border_width (GTK_CONTAINER (hbox), 3)

#undef ADD_VAL
#define ADD_VAL(x,y) \
	checked_val (data, x, (y)); \
	gtk_box_pack_start (GTK_BOX (hbox), data->check [x], FALSE, FALSE, 0); \
	gtk_box_pack_start (GTK_BOX (hbox), data->spin  [x], FALSE, FALSE, 0);

#undef ADD_ITEM
#define ADD_ITEM(n,f) \
	menuitem = gtk_menu_item_new_with_label (n); \
        gtk_menu_append (GTK_MENU (menu), menuitem); \
        gtk_widget_show (menuitem); \
        gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (f), data); \
        gtk_object_set_data (GTK_OBJECT (menuitem), "idx", GINT_TO_POINTER (mcounter)); \
        mcounter++;

static GtkHTMLEditRuleProperties *
data_new (GtkHTMLControlData *cd)
{
	GtkHTMLEditRuleProperties *data = g_new0 (GtkHTMLEditRuleProperties, 1);

	/* fill data */
	data->cd             = cd;
	data->disable_change = TRUE;
	data->rule           = NULL;

	/* default values */
	data->width          = 100;
	data->size           = 2;
	data->percent        = TRUE;
	data->shaded         = TRUE;
	data->align          = HTML_HALIGN_CENTER;

	return data;
}

static GtkWidget *
rule_widget (GtkHTMLEditRuleProperties *data)
{
	GtkWidget *vbox, *mhb, *hbox, *frame, *menu, *menuitem, *vb1;
	gint mcounter;

	/* prepare content */
	vbox = gtk_vbox_new (FALSE, 2);
	gtk_container_border_width (GTK_CONTAINER (vbox), 3);

	mhb  = gtk_hbox_new (FALSE, 3);

	/* size */
	frame = gtk_frame_new (_("Size"));
	vb1   = gtk_vbox_new (FALSE, 0);
	menu  = gtk_menu_new ();
	mcounter = 0;
	HBOX;
	ADD_ITEM (_("Pixels"), percent_menu_activate);
	ADD_ITEM (_("Percent %"), percent_menu_activate);
	data->width_measure = gtk_option_menu_new ();
	gtk_option_menu_set_menu (GTK_OPTION_MENU (data->width_measure), menu);
	gtk_option_menu_set_history (GTK_OPTION_MENU (data->width_measure), data->percent);
	gtk_box_pack_start (GTK_BOX (hbox), data->width_measure, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vb1), hbox, FALSE, FALSE, 0);

	HBOX;
	ADD_VAL (GTK_HTML_EDIT_RULE_WIDTH, _("length"));
	gtk_adjustment_set_value (GTK_ADJUSTMENT (data->adj [GTK_HTML_EDIT_RULE_WIDTH]), data->width);
	gtk_widget_set_sensitive (data->width_measure, data->set [GTK_HTML_EDIT_RULE_WIDTH]);
	gtk_signal_connect (GTK_OBJECT (data->check [GTK_HTML_EDIT_RULE_WIDTH]), "toggled",
			    GTK_SIGNAL_FUNC (width_toggled), data);
	gtk_box_pack_start (GTK_BOX (vb1), hbox, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), vb1);
	gtk_box_pack_start (GTK_BOX (mhb), frame, FALSE, FALSE, 0);

	/* style */
	frame = gtk_frame_new (_("Style"));
	vb1   = gtk_vbox_new (FALSE, 0);

	/* align */
	HBOX;
	menu  = gtk_menu_new ();
	mcounter = HTML_HALIGN_LEFT;
	ADD_ITEM (_("Left"), align_menu_activate);
	ADD_ITEM (_("Center"), align_menu_activate);
	ADD_ITEM (_("Right"), align_menu_activate);
	data->align_option = gtk_option_menu_new ();
	gtk_option_menu_set_menu (GTK_OPTION_MENU (data->align_option), menu);
	gtk_option_menu_set_history (GTK_OPTION_MENU (data->align_option), data->align);
	gtk_box_pack_start (GTK_BOX (hbox), data->align_option, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vb1), hbox, FALSE, FALSE, 0);

	/* weight */
	HBOX;
	ADD_VAL (GTK_HTML_EDIT_RULE_SIZE, _("width"));
	gtk_adjustment_set_value (GTK_ADJUSTMENT (data->adj [GTK_HTML_EDIT_RULE_SIZE]), data->size);
	gtk_box_pack_start_defaults (GTK_BOX (vb1), hbox);

	/* shaded */
	HBOX;
	data->shaded_check = gtk_check_button_new_with_label (_("shaded"));
	gtk_signal_connect (GTK_OBJECT (data->shaded_check), "toggled", shade_toggled, data);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (data->shaded_check), data->shaded);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), data->shaded_check);
	gtk_box_pack_start_defaults (GTK_BOX (vb1), hbox);

	gtk_container_add (GTK_CONTAINER (frame), vb1);
	gtk_box_pack_start_defaults (GTK_BOX (mhb), frame);
	gtk_box_pack_start (GTK_BOX (vbox), mhb, FALSE, FALSE, 0);

	/* sample */
	gtk_box_pack_start (GTK_BOX (vbox), sample_frame (&data->sample), FALSE, FALSE, 0);
	fill_sample (data);

	data->disable_change = FALSE;

	return vbox;
}

GtkWidget *
rule_properties (GtkHTMLControlData *cd, gpointer *set_data)
{
	GtkHTMLEditRuleProperties *data = data_new (cd);
	GtkWidget *rv;

	g_assert (HTML_OBJECT_TYPE (cd->html->engine->cursor->object) == HTML_TYPE_RULE);

	*set_data     = data;
	data->rule    = HTML_RULE (cd->html->engine->cursor->object);
	data->shaded  = data->rule->shade;
	data->percent = HTML_OBJECT (data->rule)->percent > 0 ? TRUE : FALSE;
	data->width   = HTML_OBJECT (data->rule)->percent > 0 ? HTML_OBJECT (data->rule)->percent : data->rule->length;
	data->size    = data->rule->size;
	data->align   = data->rule->halign;

	rv = rule_widget (data);
	data->disable_change = TRUE;
	if (data->width != 100 || !data->percent)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (data->check [GTK_HTML_EDIT_RULE_WIDTH]), TRUE);
	if (data->size != 2)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (data->check [GTK_HTML_EDIT_RULE_SIZE]), TRUE);
	data->disable_change = FALSE;

	return rv;
}

GtkWidget *
rule_insert (GtkHTMLControlData *cd, gpointer *set_data)
{
	GtkHTMLEditRuleProperties *data = data_new (cd);
	GtkWidget *rv;

	*set_data = data;
	rv = rule_widget (data);
	gtk_html_edit_properties_dialog_change (data->cd->properties_dialog);

	return rv;
}

void
rule_insert_cb (GtkHTMLControlData *cd, gpointer get_data)
{
	GtkHTMLEditRuleProperties *d = (GtkHTMLEditRuleProperties *) get_data;

	html_engine_insert_rule (cd->html->engine,
				 VAL (WIDTH), d->percent ? VAL (WIDTH) : 0, VAL (SIZE),
				 d->shaded, d->align);
}

void
rule_apply_cb (GtkHTMLControlData *cd, gpointer get_data)
{
	GtkHTMLEditRuleProperties *d = (GtkHTMLEditRuleProperties *) get_data;

	html_rule_set (d->rule, cd->html->engine, VAL (WIDTH), d->percent ? VAL (WIDTH) : 0, VAL (SIZE), d->shaded, d->align);
}

void
rule_close_cb (GtkHTMLControlData *cd, gpointer get_data)
{
	g_free (get_data);
}
