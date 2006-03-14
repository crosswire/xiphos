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

#ifndef USE_GTKHTML38
#include <libgnome/gnome-i18n.h>
#include <string.h>
#include "gtkhtml/gtkhtml.h"
#include "gtkhtml/gtkhtml-private.h"

#include "gtkhtml/htmlclueflow.h"
#include "gtkhtml/htmlcursor.h"
#include "gtkhtml/htmlengine.h"
#include "gtkhtml/htmltext.h"
#include "gtkhtml/htmlengine-edit.h"
#include "gtkhtml/htmlengine-edit-cut-and-paste.h"
#include "gtkhtml/htmlengine-edit-table.h"
#include "gtkhtml/htmlengine-edit-tablecell.h"
#include "gtkhtml/htmlimage.h"
#include "gtkhtml/htmlselection.h"
#include "gtkhtml/htmltable.h"
#include "gtkhtml/htmltablecell.h"

#include "body.h"
#include "cell.h"
#include "image.h"
//#include "menubar.h"
#include "rule.h"
#include "popup.h"
#include "properties.h"
#include "link.h"
#include "paragraph.h"
#include "spell.h"
#include "table.h"
#include "text.h"

#include "main/settings.h"

//#define ICONDIR "./art"

/* #define DEBUG */
#ifdef DEBUG
#include "gtkhtml/gtkhtmldebug.h"
#endif

#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
static void
undo (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_undo (cd->html);
}

static void
redo (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_redo (cd->html);
}

static void
copy (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_copy (cd->html);
}

static void
cut (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_cut (cd->html);
}

static void
paste (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_paste (cd->html, FALSE);
}

static void
paste_cite (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_paste (cd->html, TRUE);
}

static void
insert_link (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	link_insert (cd, NULL);
	
	
	
/*	if (cd->properties_dialog)
		gtk_html_edit_properties_dialog_close (cd->properties_dialog);

	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, _("Insert"), PACKAGE_PIXMAPS_DIR "/insert-link-24.png");

	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_LINK, _("Link"),
						   link_insert,
						   link_close_cb);

	gtk_html_edit_properties_dialog_show (cd->properties_dialog);
	gtk_html_edit_properties_dialog_set_page (cd->properties_dialog, GTK_HTML_EDIT_PROPERTY_LINK);
*/	
	
}

static void
remove_link (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	html_engine_selection_push (cd->html->engine);
	if (!html_engine_is_selection_active (cd->html->engine))
		html_engine_select_word_editable (cd->html->engine);
	html_engine_set_link (cd->html->engine, NULL);
	html_engine_selection_pop (cd->html->engine);
}

static void
insert_table_cb (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
//	insert_table (cd);
}

static void
insert_row_above (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	html_engine_insert_table_row (cd->html->engine, FALSE);
}

static void
insert_row_below (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	html_engine_insert_table_row (cd->html->engine, TRUE);
}

static void
insert_column_before (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	html_engine_insert_table_column (cd->html->engine, FALSE);
}

static void
insert_column_after (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	html_engine_insert_table_column (cd->html->engine, TRUE);
}

static void
delete_table (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_command (cd->html, "delete-table");
}

static void
delete_row (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_command (cd->html, "delete-table-row");
}

static void
delete_column (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_command (cd->html, "delete-table-column");
}

static void
delete_cell_contents (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_command (cd->html, "delete-cell-contents");
}

static void
show_prop_dialog (GSHTMLEditorControlData *cd, GtkHTMLEditPropertyType start)
{
	GtkHTMLEditPropertyType t;
	GList *cur;

	if (cd->properties_dialog)
		gtk_html_edit_properties_dialog_close (cd->properties_dialog);
	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, _("Properties"),PACKAGE_PIXMAPS_DIR"/gs2-48x48.png");

	cur = cd->properties_types;
	while (cur) {
		t = GPOINTER_TO_INT (cur->data);
		switch (t) {
		case GTK_HTML_EDIT_PROPERTY_TEXT:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Text"),
								   text_properties,
								   text_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_LINK:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Link"),
								   link_properties,
								   link_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_IMAGE:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Image"),
								   image_properties,
								   image_close_cb);
								   break;
		case GTK_HTML_EDIT_PROPERTY_PARAGRAPH:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Paragraph"),
								   paragraph_properties,
								   paragraph_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_BODY:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Page"),
								   body_properties,
								   body_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_RULE:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Rule"),
								   rule_properties,
								   rule_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_TABLE:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Table"),
								   table_properties,
								   table_close_cb);
			break;
		case GTK_HTML_EDIT_PROPERTY_CELL:
			gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
								   t, _("Cell"),
								   cell_properties,
								   cell_close_cb);
			break;
		default:
			;
		}
		cur = cur->next;
	}

	gtk_html_edit_properties_dialog_show (cd->properties_dialog);
	if (start > GTK_HTML_EDIT_PROPERTY_NONE)
		gtk_html_edit_properties_dialog_set_page (cd->properties_dialog, start);
}

static void
prop_dialog (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	show_prop_dialog (cd, GPOINTER_TO_INT (g_object_get_data (G_OBJECT (mi), "type")));
}
// gnome_icon_theme_lookup_icon ()
static void
link_prop_dialog (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	if (cd->properties_dialog)
		gtk_html_edit_properties_dialog_close (cd->properties_dialog);

	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, _("Properties"), GTKHTML_DATA_DIR"/icons/insert-link-24.png");

	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_LINK, _("Link"),
						   link_properties,
						   link_close_cb);

	gtk_html_edit_properties_dialog_show (cd->properties_dialog);
	gtk_html_edit_properties_dialog_set_page (cd->properties_dialog, GTK_HTML_EDIT_PROPERTY_LINK);
}

static void
spell_check_cb (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	spell_check_dialog (cd, FALSE);
}

static void
spell_add (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	HTMLEngine *e = cd->html->engine;
	gchar *word;

	word = html_engine_get_spell_word (e);
	if (word) {
		spell_add_to_personal (cd->html, word, settings.spell_language, cd);//g_object_get_data (G_OBJECT (mi), "abbrev")
		g_free (word);
	}
	html_engine_spell_check (e);
}

static void
spell_ignore (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	HTMLEngine *e = cd->html->engine;
	gchar *word;

	word = html_engine_get_spell_word (e);
	if (word) {
		spell_add_to_session (cd->html, word, cd);
		g_free (word);
	}
	html_engine_spell_check (e);
}
/*
#ifdef DEBUG
static void
dump_tree_simple (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_debug_dump_tree_simple (cd->html->engine->clue, 0);
}

static void
dump_tree (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_debug_dump_tree (cd->html->engine->clue, 0);
}

static void
insert_html (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gtk_html_insert_html (cd->html, "<BR>Hello dude!<BR><PRE>--\nrodo\n</PRE>");
}
#endif
*/
static void
bookmark_item (GtkWidget *mi, GSHTMLEditorControlData *cd)
{
	gchar *label = NULL;
	
	if(cd->studypad) {
		label = g_strdup_printf("%s, %s",cd->filename, "studypad");
		gui_bookmark_dialog(label, "studypad", cd->filename);
		g_free(label);
	} else if(cd->personal_comments) {
		label = g_strdup_printf("%s, %s",cd->key, cd->filename);
		gui_bookmark_dialog(label, cd->filename, cd->key);
		g_free(label);	
	}
}

#define ADD_ITEM_BASE(f,t) \
                g_object_set_data (G_OBJECT (menuitem), "type",  \
		GINT_TO_POINTER (GTK_HTML_EDIT_PROPERTY_ ## t)); \
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem); \
		gtk_widget_show (menuitem); \
		g_signal_connect (menuitem, "activate", G_CALLBACK (f), cd); \
                (*items)++; items_sep++

#define ADD_ITEM(l,f,t) \
		menuitem = gtk_menu_item_new_with_label (l); \
                ADD_ITEM_BASE (f,t)

#define ADD_STOCK(i,f) \
		menuitem = gtk_image_menu_item_new_from_stock (i, NULL); \
                ADD_ITEM_BASE (f,NONE);

#define ADD_STOCK_SENSITIVE(i,f,s) \
		menuitem = gtk_image_menu_item_new_from_stock (i, NULL); \
                ADD_ITEM_BASE (f,NONE); \
                gtk_widget_set_sensitive (menuitem, s);

#define ADD_SEP \
        if (items_sep) { \
                menuitem = gtk_menu_item_new (); \
                gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem); \
                gtk_widget_show (menuitem); \
		items_sep = 0; \
        }

#define ADD_PROP(x) \
        cd->properties_types = g_list_append (cd->properties_types, GINT_TO_POINTER (GTK_HTML_EDIT_PROPERTY_ ## x)); \
        (*props) ++;

#define SUBMENU(l) \
		        menuitem = gtk_menu_item_new_with_label (l); \
			gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem); \
			gtk_widget_show (menuitem); \
			(*items)++; items_sep++; \
			submenu = gtk_menu_new (); \
			gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu); \
			menuparent = menu; \
			menu = submenu

#define END_SUBMENU \
			gtk_widget_show (menu); \
			menu = menuparent

static gint
get_n_languages (GSHTMLEditorControlData *cd)
{
	gint i, n = 0;

	if (cd->languages)
		for (i = 0; i < cd->languages->_length; i ++)
			if (strstr (html_engine_get_language (cd->html->engine), cd->languages->_buffer [i].abbreviation))
				n ++;

	return n;
}

static const gchar *
get_language (GSHTMLEditorControlData *cd)
{
	gint i;
	const gchar *abbrev = NULL;

	if (cd->languages)
		for (i = 0; i < cd->languages->_length; i ++)
			if (strstr (html_engine_get_language (cd->html->engine), cd->languages->_buffer [i].abbreviation)) {
				abbrev = cd->languages->_buffer [i].abbreviation;
			}

	return abbrev;
}

static GtkWidget *
prepare_properties_and_menu (GSHTMLEditorControlData *cd, guint *items, guint *props)
{
	HTMLEngine *e = cd->html->engine;
	HTMLObject *obj;
	GtkWidget *menu;
	GtkWidget *submenu, *menuparent;
	GtkWidget *menuitem;
	guint items_sep = 0;
	gboolean active = FALSE;

	obj  = cd->html->engine->cursor->object;
	menu = gtk_menu_new ();
	*items = 0;
	*props = 0;

	if (cd->properties_types) {
		g_list_free (cd->properties_types);
		cd->properties_types = NULL;
	}
/*
#ifdef DEBUG
	ADD_ITEM ("Dump tree (simple)", dump_tree_simple, NONE);
	ADD_ITEM ("Dump tree", dump_tree, NONE);
	ADD_ITEM ("Insert HTML", insert_html, NONE);
	ADD_SEP;
#endif
*/
	active = html_engine_is_selection_active (e);
	ADD_STOCK (GTK_STOCK_UNDO, undo); 
	ADD_STOCK (GTK_STOCK_REDO, redo); 
	ADD_SEP;
	ADD_ITEM (_("Bookmark"), bookmark_item, NONE);

	ADD_SEP;
	ADD_STOCK_SENSITIVE (GTK_STOCK_CUT,  cut, active);
	ADD_STOCK_SENSITIVE (GTK_STOCK_COPY, copy, active);
	ADD_STOCK (GTK_STOCK_PASTE, paste);
	ADD_ITEM (_("Paste Quotation"),  paste_cite, NONE);

	if (cd->format_html) {
		ADD_SEP;
		ADD_ITEM (_("Insert Link"), insert_link, NONE);
		if (((active && html_engine_selection_contains_link (e))
		     || (obj
			 && (HTML_OBJECT_TYPE (obj) == HTML_TYPE_LINKTEXT
			     || (HTML_OBJECT_TYPE (obj) == HTML_TYPE_IMAGE
				 && (HTML_IMAGE (obj)->url
				     || HTML_IMAGE (obj)->target)))))) {
			ADD_ITEM (_("Remove Link"), remove_link, NONE);
		}
	}
	
	if (obj) {
		if (cd->format_html) {
			ADD_SEP;
			SUBMENU (_("Style"));
			switch (HTML_OBJECT_TYPE (obj)) {
			case HTML_TYPE_TEXT:
				ADD_ITEM (_("Text Style..."), prop_dialog, TEXT);
				ADD_PROP (TEXT);
				ADD_ITEM (_("Paragraph Style..."), prop_dialog, PARAGRAPH);
				ADD_PROP (PARAGRAPH);
				break;
			case HTML_TYPE_LINKTEXT:
				ADD_ITEM (_("Link Style..."), link_prop_dialog, LINK);
				ADD_PROP (LINK);
				ADD_ITEM (_("Paragraph Style..."), prop_dialog, PARAGRAPH);
				ADD_PROP (PARAGRAPH);
				break;
			case HTML_TYPE_RULE:
				ADD_ITEM (_("Rule Style..."), prop_dialog, RULE);
				ADD_PROP (RULE);
				break;
			case HTML_TYPE_IMAGE:
				ADD_ITEM (_("Image Style..."), prop_dialog, IMAGE);
				ADD_PROP (IMAGE);
				ADD_ITEM (_("Paragraph Style..."), prop_dialog, PARAGRAPH);
				ADD_PROP (PARAGRAPH);
				break;
			default:
				;
			}
			if (obj->parent && obj->parent->parent && HTML_IS_TABLE_CELL (obj->parent->parent)) {
				if (cd->format_html) {
					ADD_PROP (CELL);
					ADD_ITEM (_("Cell Style..."), prop_dialog, CELL);
					if (obj->parent->parent->parent && HTML_IS_TABLE (obj->parent->parent->parent)) {
						ADD_PROP (TABLE);
						ADD_ITEM (_("Table Style..."), prop_dialog, TABLE);
					}
				}
			}
			if (cd->format_html) {
				ADD_PROP (BODY);
				ADD_ITEM (_("Page Style..."), prop_dialog, BODY);
			}
			END_SUBMENU;
			if (obj->parent && obj->parent->parent && HTML_IS_TABLE_CELL (obj->parent->parent)) {
				ADD_SEP;
				SUBMENU (_("Table insert"));
				ADD_ITEM (_("Table"), insert_table_cb, NONE);
				ADD_SEP;
				ADD_ITEM (_("Row above"), insert_row_above, NONE);
				ADD_ITEM (_("Row below"), insert_row_below, NONE);
				ADD_SEP;
				ADD_ITEM (_("Column before"), insert_column_before, NONE);
				ADD_ITEM (_("Column after"), insert_column_after, NONE);
				END_SUBMENU;
			}
		}
		if (obj->parent && obj->parent->parent && HTML_IS_TABLE_CELL (obj->parent->parent)) {
			SUBMENU (_("Table delete"));
			ADD_ITEM (_("Table"), delete_table, NONE);
			ADD_ITEM (_("Row"), delete_row, NONE);
			ADD_ITEM (_("Column"), delete_column, NONE);
			ADD_ITEM (_("Cell contents"), delete_cell_contents, NONE);
			END_SUBMENU;
		}
	}

	if (!active && obj && html_object_is_text (obj)
	    && !html_engine_spell_word_is_valid (e)) {
		ADD_SEP;
		ADD_ITEM (_("Check Word Spelling..."), spell_check_cb, NONE);
		if (get_n_languages (cd) > 1) {
			gchar *lang;
			gint i;

			SUBMENU (_("Add Word to"));

			for (i = 0; i < cd->languages->_length; i ++) {
				if (strstr (html_engine_get_language (cd->html->engine), cd->languages->_buffer [i].abbreviation)) {
					lang = g_strdup_printf (_("%s Dictionary"), cd->languages->_buffer [i].name);
					ADD_ITEM (lang, spell_add, NONE);
					g_object_set_data (G_OBJECT (menuitem), "abbrev", cd->languages->_buffer [i].abbreviation);
					g_free (lang);
				}
			}
			
			END_SUBMENU;
		} else {
			ADD_ITEM (_("Add Word to Dictionary"), spell_add, NONE);
			g_object_set_data (G_OBJECT (menuitem), "abbrev", (gpointer) get_language (cd));
		}
		ADD_ITEM (_("Ignore Misspelled Word"), spell_ignore, NONE);
	}

	ADD_SEP;
	SUBMENU (_("Input Methods"));
	gtk_im_multicontext_append_menuitems (GTK_IM_MULTICONTEXT (cd->html->priv->im_context), 
					      GTK_MENU_SHELL (menu));
	END_SUBMENU;

	if (*items == 0) {
		gtk_object_sink (GTK_OBJECT (menu));
		menu = NULL;
	} else
		gtk_widget_show (menu);

	return menu;
}

gint
popup_show (GSHTMLEditorControlData *cd, GdkEventButton *event)
{
	GtkWidget *menu;
	guint items, props;

	menu = prepare_properties_and_menu (cd, &items, &props);

	if (items)
		gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
				event ? event->button : 0, event ? event->time : 0);

	return (items > 0);
}

static void
set_position (GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer data)
{
	GSHTMLEditorControlData *cd = (GSHTMLEditorControlData *) data;
	HTMLEngine *e = cd->html->engine;
	gint xw, yw;

	gdk_window_get_origin (GTK_WIDGET (cd->html)->window, &xw, &yw);
	html_object_get_cursor_base (e->cursor->object, e->painter, e->cursor->offset, x, y);
	*x += xw + e->leftBorder;
	*y += yw + e->topBorder;
}

gint
popup_show_at_cursor (GSHTMLEditorControlData *cd)
{
	GtkWidget *menu;
	guint items, props;

	menu = prepare_properties_and_menu (cd, &items, &props);
	gtk_widget_show (menu);
	if (items)
		gtk_menu_popup (GTK_MENU (menu), NULL, NULL, set_position, cd, 0, 0);

	return (items > 0);
}

void
property_dialog_show (GSHTMLEditorControlData *cd)
{
	GtkWidget *menu;
	guint items, props;

	menu = prepare_properties_and_menu (cd, &items, &props);
	gtk_object_sink (GTK_OBJECT (menu));
	if (props)
		show_prop_dialog (cd, GTK_HTML_EDIT_PROPERTY_NONE);
}
#endif
