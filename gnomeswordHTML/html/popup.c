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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtkhtml.h>
#include <htmlcursor.h>
#include <htmlengine.h>
#include <htmllinktext.h>
#include <htmlengine-edit-cut-and-paste.h>
#include <htmlimage.h>
#include <htmlselection.h>

#include "popup.h"
//#include "spell.h"
#include "properties.h"
#include "paragraph.h"
#include "image.h"
#include "text.h"
#include "link.h"
#include "body.h"

#define DEBUG
#ifdef DEBUG
#include "gtkhtmldebug.h"
#endif

static void
copy (GtkWidget *mi, GtkHTMLControlData *cd)
{
	html_engine_copy (cd->html->engine);
}

static void
cut (GtkWidget *mi, GtkHTMLControlData *cd)
{
	html_engine_cut (cd->html->engine);
}

static void
paste (GtkWidget *mi, GtkHTMLControlData *cd)
{
	html_engine_paste (cd->html->engine);
}

static void
remove_link (GtkWidget *mi, GtkHTMLControlData *cd)
{
	if (!html_engine_is_selection_active (cd->html->engine))
		html_engine_select_word (cd->html->engine);
	html_engine_insert_link (cd->html->engine, NULL, NULL);
}

static void
show_prop_dialog (GtkHTMLControlData *cd, GtkHTMLEditPropertyType start)
{
	GtkHTMLEditPropertyType t;
	GList *cur;

	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, FALSE, _("Properties"));

	cur = cd->properties_types;
	while (cur) {
		t = GPOINTER_TO_INT (cur->data);
		switch (t) {
		case GTK_HTML_EDIT_PROPERTY_TEXT:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Text"),
								   text_properties,
								   text_apply_cb,
								   text_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_IMAGE:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Image"),
								   image_properties,
								   image_apply_cb,
								   image_close_cb);
								   break;
		case GTK_HTML_EDIT_PROPERTY_PARAGRAPH:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Paragraph"),
								   paragraph_properties,
								   paragraph_apply_cb,
								   paragraph_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_LINK:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Link"),
								   link_properties,
								   link_apply_cb,
								   link_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_BODY:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Page"),
								   body_properties,
								   body_apply_cb,
								   body_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_RULE:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Rule"),
								   rule_properties,
								   rule_apply_cb,
								   rule_close_cb);
			break;
		}
		cur = cur->next;
	}

	gtk_html_edit_properties_dialog_show (cd->properties_dialog);
	if (start >= 0)
		gtk_html_edit_properties_dialog_set_page (cd->properties_dialog, start);
}

static void
prop_dialog (GtkWidget *mi, GtkHTMLControlData *cd)
{
	show_prop_dialog (cd, GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (mi), "type")));
}

static void
spell_suggest (GtkWidget *mi, GtkHTMLControlData *cd)
{
	HTMLEngine *e = cd->html->engine;
 
	/* gtk_signal_emit_by_name (GTK_OBJECT (cd->html), "spell_suggestion_request",
	   e->spell_checker, html_engine_get_word (e)); */
	/* spell_suggestion_request (cd->html, html_engine_get_word (e), cd);  */
}

static void
dump_tree_simple (GtkWidget *mi, GtkHTMLControlData *cd)
{
	gtk_html_debug_dump_tree_simple (cd->html->engine->clue, 0);
}

static void
dump_tree (GtkWidget *mi, GtkHTMLControlData *cd)
{
	gtk_html_debug_dump_tree (cd->html->engine->clue, 0);
}

#define ADD_ITEM(l,f,t) \
		menuitem = gtk_menu_item_new_with_label (_(l)); \
                gtk_object_set_data (GTK_OBJECT (menuitem), "type", GINT_TO_POINTER (t)); \
		gtk_menu_append (GTK_MENU (menu), menuitem); \
		gtk_widget_show (menuitem); \
		gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (f), cd); \
                (*items)++; items_sep++

#define ADD_SEP \
        if (items_sep) { \
                menuitem = gtk_menu_item_new (); \
                gtk_menu_append (GTK_MENU (menu), menuitem); \
                gtk_widget_show (menuitem); \
		items_sep = 0; \
        }

#define ADD_PROP(x) \
        cd->properties_types = g_list_append (cd->properties_types, GINT_TO_POINTER (GTK_HTML_EDIT_PROPERTY_ ## x))

static GtkWidget *
prepare_properties_and_menu (GtkHTMLControlData *cd, guint *items)
{
	HTMLEngine *e = cd->html->engine;
	HTMLObject *obj;
	GtkWidget *menu;
	GtkWidget *menuitem;
	guint items_sep = 0;

	obj  = cd->html->engine->cursor->object;
	menu = gtk_menu_new ();

	if (cd->properties_types) {
		g_list_free (cd->properties_types);
		cd->properties_types = NULL;
	}

#ifdef DEBUG
	ADD_ITEM ("Dump tree (simple)", dump_tree_simple, -1);
	ADD_ITEM ("Dump tree", dump_tree, -1);
	ADD_SEP;
#endif

	if (!html_engine_is_selection_active (e) && obj && html_object_is_text (obj) && !html_engine_word_is_valid (e)) {
		ADD_SEP;
		ADD_ITEM ("Suggest word", spell_suggest, -1);
	}

	if (html_engine_is_selection_active (e)
	    || (obj
		&& (HTML_OBJECT_TYPE (obj) == HTML_TYPE_LINKTEXT
		    || (HTML_OBJECT_TYPE (obj) == HTML_TYPE_IMAGE
			&& (HTML_IMAGE (obj)->url
			    || HTML_IMAGE (obj)->target))))) {
		    ADD_ITEM (_("Remove link"), remove_link, -1);
	}

	if (html_engine_is_selection_active (e)) {
		ADD_SEP;
		ADD_ITEM ("Copy", copy, -1);
		ADD_ITEM ("Cut",  cut, -1);
	}
	if (e->clipboard) {
		if (!html_engine_is_selection_active (e)) {
			ADD_SEP;
		}
		ADD_ITEM ("Paste",  paste, -1);
	}
	if (html_engine_is_selection_active (e)) {
		ADD_SEP;
		ADD_ITEM ("Text...", prop_dialog, GTK_HTML_EDIT_PROPERTY_TEXT);
		ADD_PROP (TEXT);
		ADD_ITEM ("Paragraph...", prop_dialog, GTK_HTML_EDIT_PROPERTY_PARAGRAPH);
		ADD_PROP (PARAGRAPH);
		ADD_ITEM ("Link...", prop_dialog, GTK_HTML_EDIT_PROPERTY_LINK);
		ADD_PROP (LINK);
	} else if (obj) {
		switch (HTML_OBJECT_TYPE (obj)) {
		case HTML_TYPE_RULE:
			ADD_SEP;
			ADD_ITEM ("Rule...", prop_dialog, GTK_HTML_EDIT_PROPERTY_RULE);
			ADD_PROP (RULE);
			break;
		case HTML_TYPE_IMAGE:
			ADD_SEP;
			ADD_ITEM ("Image...", prop_dialog, GTK_HTML_EDIT_PROPERTY_IMAGE);
			ADD_PROP (IMAGE);
			ADD_ITEM ("Paragraph...", prop_dialog, GTK_HTML_EDIT_PROPERTY_PARAGRAPH);
			ADD_PROP (PARAGRAPH);
			ADD_ITEM ("Link...", prop_dialog, GTK_HTML_EDIT_PROPERTY_LINK);
			ADD_PROP (LINK);
			break;
		case HTML_TYPE_LINKTEXT:
		case HTML_TYPE_TEXT:
			ADD_SEP;
			ADD_PROP (TEXT);
			ADD_ITEM ("Text...", prop_dialog, GTK_HTML_EDIT_PROPERTY_TEXT);
			ADD_PROP (PARAGRAPH);
			ADD_ITEM ("Paragraph...", prop_dialog, GTK_HTML_EDIT_PROPERTY_PARAGRAPH);
			ADD_PROP (LINK);
			ADD_ITEM ("Link...", prop_dialog, GTK_HTML_EDIT_PROPERTY_LINK);
			break;

		default:
		}
	}
	ADD_SEP;
	ADD_PROP (BODY);
	ADD_ITEM ("Page...", prop_dialog, GTK_HTML_EDIT_PROPERTY_BODY);

	gtk_widget_show (menu);

	return menu;
}

gint
popup_show (GtkHTMLControlData *cd, GdkEventButton *event)
{
	GtkWidget *menu;
	guint items;

	menu = prepare_properties_and_menu (cd, &items);
	if (items)
		gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 
				event ? event->button : 0, event ? event->time : 0);
	gtk_widget_unref (menu);

	return (items > 0);
}

void
property_dialog_show (GtkHTMLControlData *cd)
{
	guint items;

	gtk_widget_unref (prepare_properties_and_menu (cd, &items));
	if (items)
		show_prop_dialog (cd, -1);
}
