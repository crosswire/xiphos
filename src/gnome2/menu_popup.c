/*
 * Xiphos Bible Study Tool
 * menu_popup.c - main window panes and dialogs popup menus
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#ifndef USE_GTKBUILDER
#include <glade/glade-xml.h>
#endif

#include <glib.h>
#include <glib/gstdio.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "xiphos_html/xiphos_html.h"

#include "gui/menu_popup.h"
#include "gui/cipher_key_dialog.h"
#include "gui/dialog.h"
#include "gui/utilities.h"
#include "gui/widgets.h"
#include "gui/bookmark_dialog.h"
#include "gui/export_dialog.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/about_modules.h"

#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/display.hh"
#include "main/search_sidebar.h"
#include "main/mod_mgr.h"
#include "main/url.hh"

#ifdef USE_WEBKIT_EDITOR
#include "editor/webkit_editor.h"
#else
#include "editor/slib-editor.h"
#endif

#include "gui/debug_glib_null.h"

static gchar *menu_mod_name = NULL;
static DIALOG_DATA *dialog = NULL;
static int is_dialog = 0;

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

gint _get_type_mod_list(void)
{
	switch (main_get_mod_type(menu_mod_name)) {
	case TEXT_TYPE:
		return TEXT_DESC_LIST;
		break;
	case COMMENTARY_TYPE:
	case PERCOM_TYPE:
		return COMM_DESC_LIST;
		break;
	case DICTIONARY_TYPE:
		return DICT_DESC_LIST;
		break;
	case BOOK_TYPE:
		return GBS_DESC_LIST;
		break;
	default:
		return -1;
		break;
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

gchar *_get_key(gchar *mod_name)
{
	gchar *key = NULL;

	if (is_dialog)
		return g_strdup(dialog->key);

	switch (main_get_mod_type(mod_name)) {
	case TEXT_TYPE:
	case COMMENTARY_TYPE:
	case PERCOM_TYPE:
		key = g_strdup(settings.currentverse);
		break;
	case DICTIONARY_TYPE:
		key = g_strdup(settings.dictkey);
		break;
	case BOOK_TYPE:
	case PRAYERLIST_TYPE:
		key = g_strdup(settings.book_key);
		break;
	}
	return key;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

GtkWidget *_get_html(void)
{
	if (is_dialog)
		return dialog->html;
	else
		switch (main_get_mod_type(menu_mod_name)) {
		case TEXT_TYPE:
			return widgets.html_text;
			break;
		case COMMENTARY_TYPE:
		case PERCOM_TYPE:
			return widgets.html_comm;
			break;
		case DICTIONARY_TYPE:
			return widgets.html_dict;
			break;
		case BOOK_TYPE:
		case PRAYERLIST_TYPE:
			return widgets.html_book;
			break;
		}
	return NULL;
}

/******************************************************************************
 * Name
 *   _global_option_main_pane
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   void _global_option_main_pane(GtkMenuItem * menuitem,
			   GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void _global_option_main_pane(GtkMenuItem *menuitem, const gchar *option)
{
	gchar *key = _get_key(menu_mod_name);
	gchar *mod =
	    (gchar *)(is_dialog ? dialog->mod_name : menu_mod_name);
	XI_message(("module option = %s", option));
	if (key) {
		gchar *url = g_strdup_printf("sword://%s/%s",
					     mod,
					     key);
		main_save_module_options(mod, (gchar *)option,
					 gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem)));
		if (is_dialog) {
			/* show the change */
			main_dialogs_url_handler(dialog, url, TRUE);
		} else {
			/* show the change */
			main_url_handler(url, TRUE);
		}

		g_free(url);
		g_free(key);
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

static void on_edit_percomm_activate(GtkMenuItem *menuitem,
				     gpointer user_data)
{
	gchar *key;

	if (is_dialog)
		key = g_strdup(dialog->key);
	else
		key = _get_key(menu_mod_name);
	if (key) {
		XI_message(("\n\npercomm key: %s\n\n", key));
		editor_create_new((gchar *)user_data, (gchar *)key,
				  NOTE_EDITOR);
		g_free(key);
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

static void on_edit_prayerlist_activate(GtkMenuItem *menuitem,
					gpointer user_data)
{
	XI_message(("settings.book_key: %s", (char *)(is_dialog ? dialog->key : settings.book_key)));
	editor_create_new((gchar *)user_data,
			  (gchar *)(is_dialog ? dialog->key : settings.book_key), BOOK_EDITOR);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_popup_about_activate(GtkMenuItem *menuitem,
					     gpointer user_data)
{
	gui_display_about_module_dialog((is_dialog ? dialog->mod_name : menu_mod_name));
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_popup_bookmark_activate(GtkMenuItem *menuitem,
						gpointer user_data)
{
	gchar *key;

	if (is_dialog)
		return;

	if ((key = _get_key(menu_mod_name))) {
		gchar *label = g_strdup_printf("%s, %s",
					       key,
					       menu_mod_name);
		gui_bookmark_dialog(label, menu_mod_name, key);
		g_free(label);
		g_free(key);
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_popup_export_passage_activate(GtkMenuItem *
							  menuitem,
						      gpointer user_data)
{
	if (is_dialog)
		return;

	gui_export_dialog();
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_popup_print_activate(GtkMenuItem *menuitem,
					     gpointer user_data)
{
	/* there is some weirdness here, from having eliminated gtkhtml3.
	 * we must understand why this is an interesting conditional.  */
	if (is_dialog) {
		XIPHOS_HTML_PRINT_DOCUMENT((XiphosHtml *)user_data);
	} else {
		XIPHOS_HTML_PRINT_DOCUMENT((XiphosHtml *)user_data);
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_close_activate(GtkMenuItem *menuitem,
				       gpointer user_data)
{
	/* FIXME */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_popup_copy_activate(GtkMenuItem *menuitem,
					    gpointer user_data)
{
	/* ditto above comment re: printing */
	if (is_dialog) {
		XIPHOS_HTML_COPY_SELECTION(dialog->html);
	} else {
		XIPHOS_HTML_COPY_SELECTION(_get_html());
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_popup_find_activate(GtkMenuItem *menuitem,
					    gpointer user_data)
{
	if (is_dialog)
		gui_find_dlg(dialog->html, dialog->mod_name, FALSE, NULL);
	else
		gui_find_dlg(_get_html(), menu_mod_name, FALSE, NULL);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_popup_font_activate(GtkMenuItem *menuitem,
					    gpointer user_data)
{
	if (is_dialog) {
		gchar *url = g_strdup_printf("sword://%s/%s",
					     dialog->mod_name,
					     dialog->key);
		gui_set_module_font(dialog->mod_name);
		/* show the change */
		main_dialogs_url_handler(dialog, url, TRUE);
		g_free(url);
	} else {
		gchar *key = _get_key(menu_mod_name);
		if (key) {
			gchar *url = g_strdup_printf("sword://%s/%s",
						     menu_mod_name,
						     key);
			gui_set_module_font(menu_mod_name);
			/* show the change */
			main_url_handler(url, TRUE);
			g_free(url);
			g_free(key);
		}
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_verse_per_line_activate(GtkCheckMenuItem *
						    menuitem,
						gpointer user_data)
{
	gchar *file = g_strdup_printf("%s/modops.conf",
				      settings.gSwordDir);

	gchar *url = g_strdup_printf("sword://%s/%s",
				     (is_dialog ? dialog->mod_name : settings.MainWindowModule),
				     (is_dialog ? dialog->key : settings.currentverse));

	save_conf_file_item(file, (is_dialog
				       ? dialog->mod_name
				       : settings.MainWindowModule),
			    "style",
			    (gtk_check_menu_item_get_active(menuitem)
				 ? "verse"
				 : "paragraph"));
	if (settings.havebible) {
		if (is_dialog) {
			/* show the change */
			main_dialogs_url_handler(dialog, url, TRUE);
		} else {
			settings.versestyle =
			    gtk_check_menu_item_get_active(menuitem);
			main_url_handler(url, TRUE);
		}
	}
	g_free(url);
	g_free(file);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_words_of_christ_in_red_activate(GtkCheckMenuItem *
							    menuitem,
							gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Words of Christ in Red"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_strong_s_numbers_activate(GtkCheckMenuItem *
						      menuitem,
						  gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Strong's Numbers"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_morphological_tags_activate(GtkCheckMenuItem *
							menuitem,
						    gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Morphological Tags"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_footnotes_activate(GtkCheckMenuItem *menuitem,
					   gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Footnotes"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_greek_accents_activate(GtkCheckMenuItem *menuitem,
					       gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Greek Accents"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_lemmas_activate(GtkCheckMenuItem *menuitem,
					gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Lemmas"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void
on_scripture_cross_references_activate(GtkCheckMenuItem *menuitem,
				       gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Cross-references"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_hebrew_vowel_points_activate(GtkCheckMenuItem *
							 menuitem,
						     gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Hebrew Vowel Points"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_hebrew_cantillation_activate(GtkCheckMenuItem *
							 menuitem,
						     gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Hebrew Cantillation"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_headings_activate(GtkCheckMenuItem *menuitem,
					  gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Headings"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_transliteration_activate(GtkCheckMenuItem *
						     menuitem,
						 gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Transliteration"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_commentary_by_chapter_activate(GtkCheckMenuItem *
							   menuitem,
						       gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Commentary by Chapter"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_doublespace_activate(GtkCheckMenuItem *menuitem,
					     gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Doublespace"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_xrefnotenumbers_activate(GtkCheckMenuItem *
						     menuitem,
						 gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "XrefNoteNumbers"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_xlit_activate(GtkCheckMenuItem *menuitem,
				      gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Transliterated Forms"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_enumerated_activate(GtkCheckMenuItem *menuitem,
					    gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Enumerations"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_glosses_activate(GtkCheckMenuItem *menuitem,
					 gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Glosses"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_morphseg_activate(GtkCheckMenuItem *menuitem,
					  gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Morpheme Segmentation"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_primary_reading_activate(GtkCheckMenuItem *
						     menuitem,
						 gpointer user_data)
{
	gchar *key = NULL;

	if (is_dialog) {
		reading_selector(dialog->mod_name,
				 dialog->key,
				 dialog,
				 (GtkMenuItem *)menuitem,
				 GINT_TO_POINTER(0));
	} else {
		if ((key = _get_key(menu_mod_name))) {
			reading_selector(menu_mod_name,
					 key,
					 NULL,
					 (GtkMenuItem *)menuitem,
					 GINT_TO_POINTER(0));
			g_free(key);
		}
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_secondary_reading_activate(GtkCheckMenuItem *
						       menuitem,
						   gpointer user_data)
{
	gchar *key = NULL;

	if (is_dialog) {
		reading_selector(dialog->mod_name,
				 dialog->key,
				 dialog,
				 (GtkMenuItem *)menuitem,
				 GINT_TO_POINTER(1));
	} else {
		if ((key = _get_key(menu_mod_name))) {
			reading_selector(menu_mod_name,
					 key,
					 NULL,
					 (GtkMenuItem *)menuitem,
					 GINT_TO_POINTER(1));
			g_free(key);
		}
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_all_readings_activate(GtkCheckMenuItem *menuitem,
					      gpointer user_data)
{
	gchar *key = NULL;

	if (is_dialog) {
		reading_selector(dialog->mod_name,
				 dialog->key,
				 dialog,
				 (GtkMenuItem *)menuitem,
				 GINT_TO_POINTER(2));
	} else {
		if ((key = _get_key(menu_mod_name))) {
			reading_selector(menu_mod_name,
					 key,
					 NULL,
					 (GtkMenuItem *)menuitem,
					 GINT_TO_POINTER(2));
			g_free(key);
		}
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_image_content_activate(GtkCheckMenuItem *menuitem,
					       gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Image Content"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_respect_font_faces_activate(GtkCheckMenuItem *
							menuitem,
						    gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem *)menuitem, "Respect Font Faces"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_unlock_this_module_activate(GtkMenuItem *menuitem,
						    gpointer user_data)
{
	if (is_dialog)
		return;
	main_check_unlock(menu_mod_name, FALSE);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_display_book_heading_activate(GtkMenuItem *
							  menuitem,
						      gpointer user_data)
{
	if (is_dialog)
		main_dialogs_book_heading(dialog);
	else
		main_book_heading(menu_mod_name);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_display_chapter_heading_activate(GtkMenuItem *
							     menuitem,
							 gpointer
							     user_data)
{
	if (is_dialog)
		main_dialogs_chapter_heading(dialog);
	else
		main_chapter_heading(menu_mod_name);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_use_current_dictionary_activate(GtkMenuItem *
							    menuitem,
							gpointer user_data)
{
	XIPHOS_HTML_COPY_SELECTION(_get_html());
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict, 0,
				   -1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);
}

/******************************************************************************
 * Name
 *   on_lookup_google_activate
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 * Description
 *   offer mouse-swept selection as a google maps browser reference.
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_lookup_google_activate(GtkMenuItem *menuitem,
					       gpointer user_data)
{
	gchar *dict_key;

	XIPHOS_HTML_COPY_SELECTION(_get_html());
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict, 0,
				   -1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	dict_key =
	    g_strdup(gtk_editable_get_chars((GtkEditable *)widgets.entry_dict, 0, -1));

	if ((dict_key == NULL) || (*dict_key == '\0')) {
		gui_generic_warning("No selection made");
	} else {
		gchar *showstr =
		    g_strconcat("http://www.biblemap.org/#", dict_key, NULL);
		xiphos_open_default(showstr);
		g_free(showstr);
	}
	g_free(dict_key);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_rename_perscomm_activate(GtkMenuItem *menuitem,
						 gpointer user_data)
{
	if (is_dialog)
		return;

#if defined(WIN32)
	gui_generic_warning(_("Renaming is not available in Windows.\n\n"
			      "Xiphos is limited by Windows' filesystem,\n"
			      "because it disallows the renaming of filename\n"
			      "components of currently-open files,\n"
			      "such as the contents of this commentary.\n"
			      "Therefore, personal commentary renaming is\n"
			      "not available in the Windows environment."));
#else
	GS_DIALOG *info;
	GString *workstr;
	char *s;
	char *datapath_old, *datapath_new;
	const char *conf_old;
	char *conf_new;
	char *sworddir, *modsdir;
	FILE *result;

	// get a new name for the module.
	info = gui_new_dialog();
	info->title = _("Rename Commentary");
	workstr = g_string_new("");
	g_string_printf(workstr, "<span weight=\"bold\">%s</span>",
			_("Choose Commentary Name"));
	info->label_top = workstr->str;
	info->text1 = g_strdup(_("New Name"));
	info->label1 = _("Name: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	if (gui_gs_dialog(info) != GS_OK)
		goto out1;

	for (s = info->text1; *s; ++s) {
		if (!isalnum(*s) && (*s != '_')) {
			gui_generic_warning_modal(_("Module names must contain [A-Za-z0-9_] only."));
			goto out1;
		}
	}

	if (main_is_module(info->text1)) {
		gui_generic_warning_modal(_("Xiphos already knows a module by that name."));
		goto out1;
	}

	sworddir = g_strdup_printf("%s/" DOTSWORD, settings.homedir);
	modsdir = g_strdup_printf("%s/mods.d", sworddir);

	conf_old =
	    main_get_mod_config_file(settings.CommWindowModule, sworddir);

	conf_new = g_strdup(info->text1); // dirname is lowercase.
	for (s = conf_new; *s; ++s)
		if (isupper(*s))
			*s = tolower(*s);

	datapath_old =
	    main_get_mod_config_entry(settings.CommWindowModule,
				      "DataPath");
	datapath_new = g_strdup(datapath_old);
	if ((s = strstr(datapath_new, "rawfiles/")) == NULL) {
		gui_generic_warning_modal("Malformed datapath in old configuration!");
		goto out2;
	}

	*(s + 9) = '\0'; // skip past "rawfiles/".
	s = g_strdup_printf("%s%s", datapath_new, conf_new);
	g_free(datapath_new); // out with the old...
	datapath_new = s;     // ..and in with the new.

	// move old data directory to new.
	if ((g_chdir(sworddir) != 0) ||
	    (rename(datapath_old, datapath_new) != 0)) {
		gui_generic_warning_modal("Failed to rename directory.");
		goto out2;
	}
	// manufacture new .conf from old.
	g_string_printf(workstr,
			"( cd \"%s\" && sed -e '/^\\[/s|^.*$|[%s]|' -e '/^DataPath=/s|rawfiles/.*$|rawfiles/%s/|' < \"%s\" > \"%s.conf\" ) 2>&1",
			modsdir, info->text1, conf_new, conf_old,
			conf_new);
	if ((result = popen(workstr->str, "r")) == NULL) {
		g_string_printf(workstr,
				_("Failed to create new configuration:\n%s"),
				strerror(errno));
		gui_generic_warning_modal(workstr->str);
		goto out2;
	} else {
		gchar output[258];
		if (fgets(output, 256, result) != NULL) {
			g_string_truncate(workstr, 0);
			g_string_append(workstr,
					_("Configuration build error:\n\n"));
			g_string_append(workstr, output);
			gui_generic_warning_modal(workstr->str);
			goto out2; // necessary?  advisable?
		}
		pclose(result);
	}

	// unlink old conf.
	g_string_printf(workstr, "%s/%s", modsdir, conf_old);
	if (unlink(workstr->str) != 0) {
		g_string_printf(workstr,
				"Unlink of old configuration failed:\n%s",
				strerror(errno));
		gui_generic_warning_modal(workstr->str);
		goto out2;
	}
	main_update_module_lists();
	settings.CommWindowModule = g_strdup(info->text1);
	main_display_commentary(info->text1, settings.currentverse);

out2:
	g_free(conf_new);
	g_free((char *)conf_old);
	g_free(datapath_old);
	g_free(datapath_new);
	g_free(modsdir);
	g_free(sworddir);
out1:
	g_free(info->text1);
	g_free(info);
	g_string_free(workstr, TRUE);
#endif /* !WIN32 */
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

G_MODULE_EXPORT void on_dump_perscomm_activate(GtkMenuItem *menuitem,
					       gpointer user_data)
{
	main_sidebar_perscomm_dump();
}

/******************************************************************************
 * Name
 *   on_read_selection_aloud_activate
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 * Description
 *   takes mouse-swept text and funnels it through TTS.
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_read_selection_aloud_activate(GtkMenuItem *
							  menuitem,
						      gpointer user_data)
{
	gchar *dict_key;
	int len;
	GtkWidget *html_widget = _get_html();

	XIPHOS_HTML_COPY_SELECTION(html_widget);
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict, 0,
				   -1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	dict_key =
	    g_strdup(gtk_editable_get_chars((GtkEditable *)widgets.entry_dict, 0, -1));
	len = (dict_key ? strlen(dict_key) : 0);

	if (dict_key && len && *dict_key) {
		ReadAloud(0, dict_key);
		g_free(dict_key);
	} else
		gui_generic_warning("No selection made");
}

/******************************************************************************
 * Name
 *   on_mark_verse_activate
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 * Description
 *   mark/unmark a verse for highlighting and possible user comment.
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_mark_verse_activate(GtkMenuItem *menuitem,
					    gpointer user_data)
{
	gchar *key;

	if (is_dialog)
		return;

	if ((key = _get_key(menu_mod_name))) {
		gui_mark_verse_dialog(menu_mod_name, key);
		g_free(key);
	}
}

/******************************************************************************
 * Name
 *   _add_global_opts
 *
 * Synopsis
 *   gui/menu_popup.h
 *
 *   GtkWidget * _add_global_opts(const gcahr * module_name)
 *
 * Description
 *
 *
 * Return value
 *   GtkWidget*
 */

#ifdef USE_GTKBUILDER
G_MODULE_EXPORT void _add_and_check_global_opts(GtkBuilder *gxml,
#else
G_MODULE_EXPORT void _add_and_check_global_opts(GladeXML *gxml,
#endif
						const gchar *mod_name,
						GtkWidget *submenu,
						DIALOG_DATA *d)
{
	GtkWidget *item;
	GLOBAL_OPS *ops = NULL;
	gint modtype =
	    main_get_mod_type((gchar *)(is_dialog ? d->mod_name : mod_name));

	ops = main_new_globals((gchar *)mod_name);

	item = UI_GET_ITEM(gxml, "verse_per_line");

	if (mod_name && (modtype == TEXT_TYPE)) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->verse_per_line);
	}

	item = UI_GET_ITEM(gxml, "words_of_christ_in_red");

	if ((main_check_for_global_option((gchar *)mod_name, "GBFRedLetterWords")) ||
	    (main_check_for_global_option((gchar *)mod_name, "OSISRedLetterWords"))) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->words_in_red);
	}

	item = UI_GET_ITEM(gxml, "strongs_numbers");

	if ((main_check_for_global_option((gchar *)mod_name, "GBFStrongs")) ||
	    (main_check_for_global_option((gchar *)mod_name, "ThMLStrongs")) ||
	    (main_check_for_global_option((gchar *)mod_name, "OSISStrongs"))) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->strongs);
	}

	item = UI_GET_ITEM(gxml, "morphological_tags");

	if (main_check_for_global_option((gchar *)mod_name, "GBFMorph") ||
	    main_check_for_global_option((gchar *)mod_name, "ThMLMorph") || main_check_for_global_option((gchar *)mod_name,
													 "OSISMorph")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->morphs);
	}

	item = UI_GET_ITEM(gxml, "footnotes");

	if (main_check_for_global_option((gchar *)mod_name, "GBFFootnotes") || main_check_for_global_option((gchar *)mod_name,
													    "ThMLFootnotes") ||
	    main_check_for_global_option((gchar *)mod_name,
					 "OSISFootnotes")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->footnotes);
	}

	item = UI_GET_ITEM(gxml, "greek_accents");

	if (main_check_for_global_option((gchar *)mod_name, "UTF8GreekAccents")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->greekaccents);
	}

	item = UI_GET_ITEM(gxml, "lemmas");

	if (main_check_for_global_option((gchar *)mod_name, "ThMLLemma") || main_check_for_global_option((gchar *)mod_name,
													 "OSISLemma")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->lemmas);
	}

	item = UI_GET_ITEM(gxml, "cross-references");

	if (main_check_for_global_option((gchar *)mod_name, "ThMLScripref") || main_check_for_global_option((gchar *)mod_name,
													    "OSISScripref")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->scripturerefs);
	}

	item = UI_GET_ITEM(gxml, "hebrew_vowel_points");

	if (main_check_for_global_option((gchar *)mod_name, "UTF8HebrewPoints")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->hebrewpoints);
	}

	item = UI_GET_ITEM(gxml, "hebrew_cantillation");

	if (main_check_for_global_option((gchar *)mod_name, "UTF8Cantillation")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->hebrewcant);
	}

	item = UI_GET_ITEM(gxml, "headings");

	if (main_check_for_global_option((gchar *)mod_name, "ThMLHeadings") || main_check_for_global_option((gchar *)mod_name,
													    "OSISHeadings")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->headings);
	}

	item = UI_GET_ITEM(gxml, "transliteration");
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
				       ops->transliteration);

	item = UI_GET_ITEM(gxml, "variants");

	if (main_check_for_global_option((gchar *)mod_name, "ThMLVariants") || main_check_for_global_option((gchar *)mod_name,
													    "OSISVariants")) {
		gtk_widget_show(item);

		item = UI_GET_ITEM(gxml, "primary_reading");
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->variants_primary);

		item = UI_GET_ITEM(gxml, "secondary_reading");
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->variants_secondary);

		item = UI_GET_ITEM(gxml, "all_readings");
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->variants_all);
	}

	item = UI_GET_ITEM(gxml, "xlit");

	if (main_check_for_global_option((gchar *)mod_name, "OSISXlit")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->xlit);
	}

	item = UI_GET_ITEM(gxml, "enumerated");

	if (main_check_for_global_option((gchar *)mod_name, "OSISEnum")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->enumerated);
	}

	item = UI_GET_ITEM(gxml, "glosses");

	if (main_check_for_global_option((gchar *)mod_name, "OSISGlosses") || main_check_for_global_option((gchar *)mod_name,
													   "OSISRuby")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->glosses);
	}

	item = UI_GET_ITEM(gxml, "morphseg");

	if (main_check_for_global_option((gchar *)mod_name, "OSISMorphSegmentation")) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->morphseg);
	}

	item = UI_GET_ITEM(gxml, "image_content");

	if (ops->image_content != -1) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->image_content);
	}

	item = UI_GET_ITEM(gxml, "respect_font_faces");

	if (ops->respect_font_faces != -1) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->respect_font_faces);
	}

	item = UI_GET_ITEM(gxml, "commentary_by_chapter");

	if ((modtype == COMMENTARY_TYPE) || (modtype == PERCOM_TYPE)) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->commentary_by_chapter);
	}

	item = UI_GET_ITEM(gxml, "doublespace");
#ifdef USE_GTK_3
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
				       ops->doublespace);
#else
	GTK_CHECK_MENU_ITEM(item)->active = ops->doublespace;
#endif

	item = UI_GET_ITEM(gxml, "xrefnotenumbers");

	if ((ops->scripturerefs &&
	     (main_check_for_global_option((gchar *)mod_name, "ThMLScripref") || main_check_for_global_option((gchar *)mod_name,
													      "OSISScripref"))) ||
	    (ops->footnotes &&
	     (main_check_for_global_option((gchar *)mod_name, "ThMLFootnotes") || main_check_for_global_option((gchar *)mod_name,
													       "OSISFootnotes") ||
	      main_check_for_global_option((gchar *)mod_name,
					   "GBFFootnotes")))) {
		gtk_widget_show(item);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       ops->xrefnotenumbers);
	} else {
		gtk_widget_hide(item);
	}

	g_free(ops);
}

/******************************************************************************
 * Name
 *  on_view_mod_activate
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   void on_view_mod_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   show a different module
 *
 * Return value
 *   void
 */

static void on_view_mod_activate(GtkMenuItem *menuitem,
				 gpointer user_data)
{
	if (is_dialog)
		return;

	gchar *module_name = main_module_name_from_description((gchar *)user_data);
	gchar *key;

	if (module_name && (key = _get_key(menu_mod_name))) {
		gchar *url = g_strdup_printf("sword://%s/%s", module_name, key);
		main_url_handler(url, TRUE);
		g_free(url);
	}
	g_free(module_name);
	g_free(key);
}

/******************************************************************************
 * Name
 *  gui_lookup_bibletext_selection
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 * void gui_lookup_bibletext_selection(GtkMenuItem * menuitem,
					 gchar * dict_mod_description)
 *
 * Description
 *   lookup selection in a dict/lex module
 *
 * Return value
 *   void
 */

static void _lookup_selection(GtkMenuItem *menuitem,
			      gchar *dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;
	GtkWidget *html = _get_html();

	if (!html)
		return;
	mod_name = main_module_name_from_description(dict_mod_description);
	XIPHOS_HTML_COPY_SELECTION(html);
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict, 0,
				   -1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);
	dict_key =
	    g_strdup(gtk_editable_get_chars((GtkEditable *)widgets.entry_dict, 0, -1));

	if (dict_key && mod_name) {
		main_display_dictionary(mod_name, dict_key);
	}
	if (dict_key)
		g_free(dict_key);
	if (mod_name)
		g_free(mod_name);
}

/******************************************************************************
 * Name
 *   _create_popup_menu
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   GtkWidget * _create_popup_menu ( const gchar * mod_name, DIALOG_DATA * d)
 *
 * Description
 *   creates the needed popup and returns it to gui_menu_popup ()
 *
 * Return value
 *   GtkWidget*
 */

static GtkWidget *_create_popup_menu(XiphosHtml *html, const gchar *mod_name,
				     DIALOG_DATA *d)
{
	gchar *glade_file;
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml;
#else
	GladeXML *gxml;
#endif
	const gchar *mname = (is_dialog ? d->mod_name : mod_name);
	XI_message(("_create_popup_menu mod_name:%s", mod_name));
	if (!mname || !*mname)
		return NULL;

#ifdef USE_GTKBUILDER
#if GTK_CHECK_VERSION(3, 14, 0)
	glade_file =
	    gui_general_user_file("xi-menus-popup.gtkbuilder", FALSE);
#else
	glade_file =
	    gui_general_user_file("xi-menus-popup_old.gtkbuilder", FALSE);
#endif
#else
	glade_file = gui_general_user_file("xi-menus.glade", FALSE);
#endif
	g_return_val_if_fail((glade_file != NULL), NULL);

#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	gxml = glade_xml_new(glade_file, "menu_popup", NULL);
#endif
	g_free(glade_file);
	g_return_val_if_fail((gxml != NULL), NULL);

	GtkWidget *menu = UI_GET_ITEM(gxml, "menu_popup");
	GtkWidget *bookmark = UI_GET_ITEM(gxml, "bookmark");
	GtkWidget *open = UI_GET_ITEM(gxml, "open_module2");
	GtkWidget *export_ = UI_GET_ITEM(gxml, "export_passage");
	GtkWidget *close = UI_GET_ITEM(gxml, "close");
	GtkWidget *note = UI_GET_ITEM(gxml, "note");
	GtkWidget *mark_verse = UI_GET_ITEM(gxml, "mark_verse");
	GtkWidget *open_edit = UI_GET_ITEM(gxml, "open_in_editor");
	GtkWidget *mod_opt_sub = UI_GET_ITEM(gxml, "module_options1_menu");
	GtkWidget *lookup = UI_GET_ITEM(gxml, "lookup_selection1");
	GtkWidget *lookup_sub =
	    UI_GET_ITEM(gxml, "lookup_selection1_menu");
	GtkWidget *unlock = UI_GET_ITEM(gxml, "unlock_this_module");
	GtkWidget *book_heading =
	    UI_GET_ITEM(gxml, "display_book_heading");
	GtkWidget *chapter_heading =
	    UI_GET_ITEM(gxml, "display_chapter_heading");
	GtkWidget *rename_percomm = UI_GET_ITEM(gxml, "rename_perscomm");
	GtkWidget *dump_percomm = UI_GET_ITEM(gxml, "dump_perscomm");

	GtkWidget *open_sub = gtk_menu_new();
	GtkWidget *note_sub = gtk_menu_new();

	gtk_widget_hide(unlock);
	gtk_widget_hide(book_heading);
	gtk_widget_hide(export_);
	gtk_widget_hide(chapter_heading);
	gtk_widget_hide(rename_percomm);
	gtk_widget_hide(dump_percomm);
	gtk_widget_hide(note);
	gtk_widget_hide(open_edit);
	gtk_widget_hide(mark_verse);
	gtk_widget_hide(close); /* FIXME: hide until connected to dialog close */

	if (is_dialog) {
		gtk_widget_hide(open);
		gtk_widget_hide(bookmark);
		gtk_widget_hide(export_);
	} else {
		gtk_widget_hide(close);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(open), open_sub);
		gui_add_mods_2_gtk_menu(_get_type_mod_list(), open_sub,
					(GCallback)on_view_mod_activate);

		/* unlock from main window only */
		if (main_has_cipher_tag((gchar *)mod_name))
			gtk_widget_show(unlock);
	}

	switch (main_get_mod_type((gchar *)mname)) {
	case TEXT_TYPE:
		gtk_widget_show(export_);
		if (is_dialog)
			break;
		gtk_widget_show(note);
		gtk_widget_show(mark_verse);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(note), note_sub);
		gui_add_mods_2_gtk_menu(PERCOMM_LIST, note_sub, (GCallback)
					on_edit_percomm_activate);
		break;

	case COMMENTARY_TYPE:
		gtk_widget_show(export_);
		gtk_widget_show(book_heading);
		gtk_widget_show(chapter_heading);
		break;

	case PERCOM_TYPE:
		gtk_widget_show(export_);
		gtk_widget_show(open_edit);
		g_signal_connect(G_OBJECT(open_edit),
				 "activate",
				 G_CALLBACK(on_edit_percomm_activate),
				 (gchar *)(is_dialog ? d->mod_name : mod_name));

		gtk_widget_show(rename_percomm);
		gtk_widget_show(dump_percomm);
		break;

	case DICTIONARY_TYPE:
		break;

	case BOOK_TYPE:
		break;

	case PRAYERLIST_TYPE:
		gtk_widget_show(open_edit);
		g_signal_connect(G_OBJECT(open_edit),
				 "activate",
				 G_CALLBACK(on_edit_prayerlist_activate),
				 (gchar *)(is_dialog ? d->mod_name : mod_name));
		break;
	}

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lookup), lookup_sub);

	gui_add_mods_2_gtk_menu(DICT_DESC_LIST, lookup_sub,
				(GCallback)_lookup_selection);

	/* = glade_xml_get_widget (gxml, ""); */
	_add_and_check_global_opts(gxml,
				   (char *)(is_dialog ? d->mod_name : mod_name), mod_opt_sub, d);
/* connect signals and data */
#ifdef USE_GTKBUILDER
	gtk_builder_connect_signals(gxml, html);
/*gtk_builder_connect_signals_full
	   (gxml, (GtkBuilderConnectFunc)gui_glade_signal_connect_func, html); */
#else
	glade_xml_signal_autoconnect_full(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func,
					  html);
#endif

	return menu;
}

/******************************************************************************
 * Name
 *   gui_menu_popup
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   void gui_menu_popup (const gchar * mod_name, DIALOG_DATA * d)
 *
 * Description
 *   determens if a main window or dialog popup is needed
 *   and calls _create_popup_menu() to create the menu and
 *   then pops the menu up
 *
 * Return value
 *   void
 */

void gui_menu_popup(XiphosHtml *html, const gchar *mod_name,
		    DIALOG_DATA *d)
{
	GtkWidget *menu;

	if (d) {
		dialog = d;
		menu_mod_name = NULL; //(gchar*) mod_name;
		is_dialog = TRUE;
	} else if (mod_name) {
		menu_mod_name = (gchar *)mod_name;
		dialog = NULL;
		is_dialog = FALSE;
	} else
		return;

	menu = _create_popup_menu(html, mod_name, d);
	if (menu)
#if GTK_CHECK_VERSION(3, 22, 0)
		gtk_menu_popup_at_pointer((GtkMenu *)menu, NULL);
#else
		gtk_menu_popup((GtkMenu *)menu, NULL, NULL, NULL, NULL, 2,
			       gtk_get_current_event_time());
#endif
	else
		gui_generic_warning(_("No module in this pane."));
}
