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
#include <glade/glade.h>

#include "gtkhtml/gtkhtml.h"
#include "gtkhtml/htmlcursor.h"
#include "gtkhtml/htmlengine-edit-fontstyle.h"
#include "gtkhtml/htmlengine-edit-cut-and-paste.h"
#include "gtkhtml/htmlengine-edit-rule.h"
#include "gtkhtml/htmlengine-save.h"
#include "gtkhtml/htmlrule.h"

#include "properties.h"
#include "rule.h"
#include "utils.h"

struct _GtkHTMLEditRuleProperties
{
	GSHTMLEditorControlData *cd;

	HTMLRule *rule;

	GtkWidget *spin_length;
	GtkWidget *option_length_percent;
	GtkWidget *spin_width;
	GtkWidget *option_align;
	GtkWidget *check_shaded;
	GtkWidget *option_template;

	gboolean   disable_change;
};
typedef struct _GtkHTMLEditRuleProperties GtkHTMLEditRuleProperties;

static void
set_length (GtkHTMLEditRuleProperties *d)
{
	GtkWidget *menu;

	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->rule)))
		return;

	menu = gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_length_percent));

	if (g_list_index (GTK_MENU_SHELL (menu)->children, gtk_menu_get_active (GTK_MENU (menu))))
		html_rule_set_length (d->rule, d->cd->html->engine, 0, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_length)));
	else
		html_rule_set_length (d->rule, d->cd->html->engine, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_length)), 0);
}

static void
changed_length (GtkWidget *check, GtkHTMLEditRuleProperties *d)
{
	set_length (d);
}

static void
changed_width (GtkWidget *check, GtkHTMLEditRuleProperties *d)
{
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->rule)))
		return;

	html_rule_set_size (d->rule, d->cd->html->engine, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (d->spin_width)));
}

static void
changed_length_percent (GtkWidget *w, GtkHTMLEditRuleProperties *d)
{
	set_length (d);
}

static void
changed_align (GtkWidget *w, GtkHTMLEditRuleProperties *d)
{
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->rule)))
		return;

	switch (g_list_index (GTK_MENU_SHELL (w)->children, gtk_menu_get_active (GTK_MENU (w)))) {
	case 0:
		html_rule_set_align (d->rule, d->cd->html->engine, HTML_HALIGN_LEFT);
		break;
	case 1:
		html_rule_set_align (d->rule, d->cd->html->engine, HTML_HALIGN_CENTER);
		break;
	case 2:
		html_rule_set_align (d->rule, d->cd->html->engine, HTML_HALIGN_RIGHT);
		break;
	}
}

static void
shaded_toggled (GtkWidget *check, GtkHTMLEditRuleProperties *d)
{
	if (d->disable_change || !editor_has_html_object (d->cd, HTML_OBJECT (d->rule)))
		return;

	html_rule_set_shade (d->rule, d->cd->html->engine, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check)));
}

static GtkHTMLEditRuleProperties *
data_new (GSHTMLEditorControlData *cd)
{
	GtkHTMLEditRuleProperties *data = g_new0 (GtkHTMLEditRuleProperties, 1);

	/* fill data */
	data->cd             = cd;
	data->disable_change = FALSE;
	data->rule           = NULL;

	return data;
}

#define UPPER_FIX(x) gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (d->spin_ ## x))->upper = 100000.0

static void
set_ui (GtkHTMLEditRuleProperties *d)
{
	d->disable_change = TRUE;

	if (d->rule) {
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_width),  d->rule->size);
		if (HTML_OBJECT (d->rule)->percent > 0) {
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_length), HTML_OBJECT (d->rule)->percent);
			gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_length_percent), 1);
		} else {
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (d->spin_length), d->rule->length);
			gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_length_percent), 0);
		}

		if (d->rule->halign == HTML_HALIGN_RIGHT)
			gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_align), 2);
		else if (d->rule->halign == HTML_HALIGN_LEFT)
			gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_align), 0);
		else
			/* center is default rule's halign */
			gtk_option_menu_set_history (GTK_OPTION_MENU (d->option_align), 1);

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d->check_shaded), d->rule->shade);
	}

	d->disable_change = FALSE;
}

static GtkWidget *
rule_widget (GtkHTMLEditRuleProperties *d, gboolean insert)
{
	GtkWidget *rule_page;
	GladeXML *xml;

	xml = glade_xml_new (GTKHTML_DATA_DIR"/gtkhtml-editor-properties.glade", "rule_page", NULL);
	if (!xml)
		g_error (_("Could not load glade file."));

	rule_page = glade_xml_get_widget (xml, "rule_page");

	d->spin_length   = glade_xml_get_widget (xml, "spin_rule_length");
	g_signal_connect (d->spin_length, "value_changed", G_CALLBACK (changed_length), d);
	UPPER_FIX (length);
	d->spin_width   = glade_xml_get_widget (xml, "spin_rule_width");
	g_signal_connect (d->spin_width, "value_changed", G_CALLBACK (changed_width), d);
	UPPER_FIX (width);
	d->option_length_percent = glade_xml_get_widget (xml, "option_rule_percent");
	g_signal_connect (gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_length_percent)),
			  "selection-done", G_CALLBACK (changed_length_percent), d);

	d->option_align = glade_xml_get_widget (xml, "option_rule_align");
	g_signal_connect (gtk_option_menu_get_menu (GTK_OPTION_MENU (d->option_align)),
			  "selection-done", G_CALLBACK (changed_align), d);

	d->check_shaded = glade_xml_get_widget (xml, "check_rule_shaded");
	g_signal_connect (d->check_shaded, "toggled", G_CALLBACK (shaded_toggled), d);

	d->disable_change = FALSE;

	return rule_page;
}

GtkWidget *
rule_properties (GSHTMLEditorControlData *cd, gpointer *set_data)
{
	GtkHTMLEditRuleProperties *d = data_new (cd);
	GtkWidget *rv;

	g_assert (HTML_OBJECT_TYPE (cd->html->engine->cursor->object) == HTML_TYPE_RULE);

	*set_data         = d;
	d->rule           = HTML_RULE (cd->html->engine->cursor->object);

	rv = rule_widget (d, FALSE);
	set_ui (d);

	return rv;
}

void
rule_close_cb (GSHTMLEditorControlData *cd, gpointer get_data)
{
	g_free (get_data);
}
