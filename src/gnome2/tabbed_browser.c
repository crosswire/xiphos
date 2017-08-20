/*
 * Xiphos Bible Study Tool
 * tabbed_browser.c - functions to facilitate tabbed browsing of different passages at once
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

#include <unistd.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "editor/slib-editor.h"

#include "gui/xiphos.h"
#include "gui/tabbed_browser.h"
#include "gui/bibletext.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary.h"
#include "gui/sidebar.h"
#include "gui/cipher_key_dialog.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/parallel_tab.h"
#include "gui/dialog.h"
#include "gui/font_dialog.h"
#include "gui/dictlex.h"
#include "gui/widgets.h"
#include "gui/utilities.h"

#include "main/lists.h"
#include "main/navbar_versekey.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/tab_history.h"
#include "main/xml.h"

#include <glib/gstdio.h>

#include "gui/debug_glib_null.h"

static GtkWidget *tab_widget_new(PASSAGE_TAB_INFO *tbinf,
				 const gchar *label_text);
void notebook_main_add_page(PASSAGE_TAB_INFO *tbinf);
void set_current_tab(PASSAGE_TAB_INFO *pt);

gboolean stop_refresh = FALSE;
gboolean change_tabs_no_redisplay = FALSE;
gboolean closing_tab = FALSE;

GList *passage_list;

/******************************************************************************
 * externs
 */

extern gboolean sync_on;
//extern gboolean gsI_isrunning;
PASSAGE_TAB_INFO *cur_passage_tab;

/******************************************************************************
 * globals to this file only
 */
static gboolean page_change = FALSE;
static gint removed_page;
static const gchar *default_tab_filename = ".last_session_tabs";
static const gchar *no_tab_filename = ".last_session_no_tabs";

static int yes_no2true_false(const gchar *yes_no)
{
	return (yes_no && !strcmp(yes_no, "yes"));
}

static gchar *true_false2yes_no(int true_false)
{
	return (true_false ? "yes" : "no");
}

/******************************************************************************
 * Name
 *  gui_recompute_shows
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_recompute_shows(void)
 *
 * Description
 *   a new set of text/preview/comm/dict showings has been selected.
 *   re-align the displayed world with that.
 *
 * Return value
 *   void
 */
void gui_recompute_shows(gboolean flush)
{
	if (stop_refresh)
		return;
	stop_refresh = TRUE;

	if (flush)
		main_flush_widgets_content();

	if (cur_passage_tab)
		gui_reassign_strdup(&settings.currentverse,
				    cur_passage_tab->text_commentary_key);

	gui_show_hide_preview(settings.showpreview);
	gui_show_hide_texts(settings.showtexts);
	gui_show_hide_dicts(settings.showdicts);
	gui_show_hide_comms(settings.showcomms);
	gui_set_bible_comm_layout();

	sync_windows();

	stop_refresh = FALSE;
}

/******************************************************************************
 * Name
 *  gui_recompute_view_menu_choices
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_recompute_view_menu_choices(void)
 *
 * Description
 *   formerly part of gui_recompute_shows, but moved here to keep from
 *   triggering toggled signals on these items before new content is
 *   ready to be displayed
 *
 * Return value
 *   void
 */
void gui_recompute_view_menu_choices(void)
{
	change_tabs_no_redisplay = TRUE;

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewtexts_item),
				       settings.showtexts);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewcomms_item),
				       settings.showcomms);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewdicts_item),
				       settings.showdicts);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewpreview_item),
				       settings.showpreview);

	change_tabs_no_redisplay = FALSE;
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
void setup_book_editor_tab(PASSAGE_TAB_INFO *pt)
{
	/*pt->editor = editor_create_new_book_editor(pt->book_mod, PRAYERLIST_TYPE);
	   gtk_widget_show(GTK_WIDGET(pt->editor));

	   gtk_box_pack_start (GTK_BOX (widgets.vboxMain), GTK_WIDGET(pt->editor),
	   TRUE, TRUE, 0); */
}
#endif

/******************************************************************************
 * Name
 *  set_current_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void set_current_tab (PASSAGE_TAB_INFO *pt)
 *
 * Description
 *   point cur_passage_tab to the current tab and turns the close button on
 *   and off
 *
 * Return value
 *   void
 */

void set_current_tab(PASSAGE_TAB_INFO *pt)
{
	PASSAGE_TAB_INFO *ot = cur_passage_tab;

	if (stop_refresh)
		return;

	if (!closing_tab && ot != NULL && ot->button_close != NULL) {
		gtk_widget_set_sensitive(ot->button_close, FALSE);
	}
	cur_passage_tab = pt;
	if (pt != NULL && pt->button_close != NULL) {
		//main_update_tab_history_menu((PASSAGE_TAB_INFO*)pt);
		gtk_widget_set_sensitive(pt->button_close, TRUE);

		/* adopt panel shows from passage tab memory. */
		settings.showtexts = pt->showtexts;
		settings.showpreview = pt->showpreview;
		settings.showcomms = pt->showcomms;
		settings.showdicts = pt->showdicts;
		settings.comm_showing = pt->comm_showing;
		gui_recompute_shows(TRUE);
	}
}

/******************************************************************************
 * Name
 *  pick_tab_label
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void pick_tab_label(PASSAGE_TAB_INFO *pt)
 *
 * Description
 *   selects a tab label based on panel shows.
 *
 * Return value
 *   GString *
 *   ** caller must free it **
 */

GString *pick_tab_label(PASSAGE_TAB_INFO *pt)
{
	GString *str = g_string_new(NULL);

	const char *abbrev_text = main_get_abbreviation(pt->text_mod);
	const char *abbrev_comm =
	    main_get_abbreviation(pt->commentary_mod);
	const char *abbrev_dict = main_get_abbreviation(pt->dictlex_mod);
	const char *abbrev_book = main_get_abbreviation(pt->book_mod);

	if (pt->showparallel) {
		g_string_printf(str, "%s", _("Parallel View"));
		return str;
	}

	if (pt->showtexts || (pt->showcomms && pt->comm_showing)) {
		g_string_printf(str, "%s: %s",
				(pt->showtexts
				     ? (abbrev_text ? abbrev_text : pt->text_mod)
				     : (pt->commentary_mod
					    ? (abbrev_comm ? abbrev_comm : pt->commentary_mod)
					    : "[no commentary]")),
				pt->text_commentary_key);
	} else {
		g_string_printf(str, "%s",
				(pt->showcomms
				     ? (pt->book_mod
					    ? (abbrev_book ? abbrev_book : pt->book_mod)
					    : "[no book]")
				     : (pt->dictlex_mod
					    ? (abbrev_dict ? abbrev_dict : pt->dictlex_mod)
					    : "[no dict]")));
	}
	return str;
}

/******************************************************************************
 * Name
 *  notebook_main_add_page
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void notebook_main_add_page(PASSAGE_TAB_INFO *tbinf)
 *
 * Description
 *   adds a new page and label to the main notebook for a new scripture passage
 *
 * Return value
 *   void
 */
void notebook_main_add_page(PASSAGE_TAB_INFO *tbinf)
{
	GtkWidget *tab_widget;
	GtkWidget *menu_label;
	GString *str;

	str = pick_tab_label(tbinf);
	UI_VBOX(tbinf->page_widget, FALSE, 0);
	if (tbinf->showparallel)
		widgets.parallel_tab = tbinf->page_widget;
	gtk_widget_show(tbinf->page_widget);

	tab_widget = tab_widget_new(tbinf, str->str);
	/*gtk_notebook_insert_page(GTK_NOTEBOOK(widgets.notebook_main),
	   tbinf->page_widget,
	   tab_widget,
	   tbinf->showparallel ? 1 : -1); */
	gtk_notebook_append_page(GTK_NOTEBOOK(widgets.notebook_main),
				 tbinf->page_widget, tab_widget);

	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.notebook_main),
					 tbinf->page_widget, str->str);

	menu_label = gtk_label_new(str->str);
	gtk_notebook_set_menu_label(GTK_NOTEBOOK(widgets.notebook_main),
				    tbinf->page_widget, menu_label);
	g_string_free(str, TRUE);
}

/******************************************************************************
 * Name
 *   gui_save_tabs
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 * Description
 *   Saves the information for the current set of tabs.
 *
 * Return value
 *
 */

void gui_save_tabs(const gchar *filename)
{
	xmlDocPtr xml_doc;
	xmlNodePtr root_node;
	xmlNodePtr cur_node;
	xmlNodePtr section_node;
	//      xmlAttrPtr xml_attr;
	//const xmlChar *xml_filename;
	gchar *file;
	GList *tmp = NULL;

	if (NULL == filename)
		filename = default_tab_filename;

	if (g_access(filename, F_OK) == 0) {
		/* we're done, just copy for local use that can be free'd */
		file = g_strdup(filename);
	} else {
		gchar *tabs_dir = g_strdup_printf("%s/tabs/", settings.gSwordDir);
		if (g_access(tabs_dir, F_OK) == -1) {
			if ((g_mkdir(tabs_dir, S_IRWXU)) == -1) {
				gui_generic_warning_modal(_("Can't create tabs dir."));
				return;
			}
		}
		file = g_strdup_printf("%s%s", tabs_dir, filename);
		g_free(tabs_dir);
	}
	//xml_filename = (const xmlChar *) file;

	xml_doc = xmlNewDoc((const xmlChar *)"1.0");

	if (xml_doc == NULL) {
		gui_generic_warning_modal("Tabs document not created successfully.");
		return;
	}

	root_node = xmlNewNode(NULL, (const xmlChar *)"Xiphos_Tabs");
	//xml_attr =
	xmlNewProp(root_node, (const xmlChar *)"Version",
		   (const xmlChar *)VERSION);
	xmlDocSetRootElement(xml_doc, root_node);

	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *)"tabs", NULL);

	for (tmp = g_list_first(passage_list); tmp != NULL;
	     tmp = g_list_next(tmp)) {

		PASSAGE_TAB_INFO *pt = (PASSAGE_TAB_INFO *)tmp->data;

		cur_node = xmlNewChild(section_node,
				       NULL, (const xmlChar *)"tab",
				       NULL);
		xmlNewProp(cur_node, (const xmlChar *)"text_mod",
			   (const xmlChar *)pt->text_mod);
		xmlNewProp(cur_node, (const xmlChar *)"commentary_mod",
			   (const xmlChar *)pt->commentary_mod);
		xmlNewProp(cur_node, (const xmlChar *)"dictlex_mod",
			   (const xmlChar *)pt->dictlex_mod);
		xmlNewProp(cur_node, (const xmlChar *)"book_mod",
			   (const xmlChar *)pt->book_mod);
		xmlNewProp(cur_node,
			   (const xmlChar *)"text_commentary_key",
			   (const xmlChar *)pt->text_commentary_key);
		xmlNewProp(cur_node, (const xmlChar *)"dictlex_key",
			   (const xmlChar *)pt->dictlex_key);
		xmlNewProp(cur_node, (const xmlChar *)"book_offset",
			   (const xmlChar *)pt->book_offset);
		xmlNewProp(cur_node, (const xmlChar *)"comm_showing",
			   (const xmlChar *)
			   true_false2yes_no(pt->comm_showing));
		xmlNewProp(cur_node, (const xmlChar *)"showtexts",
			   (const xmlChar *)
			   true_false2yes_no(pt->showtexts));
		xmlNewProp(cur_node, (const xmlChar *)"showpreview",
			   (const xmlChar *)
			   true_false2yes_no(pt->showpreview));
		xmlNewProp(cur_node, (const xmlChar *)"showcomms",
			   (const xmlChar *)
			   true_false2yes_no(pt->showcomms));
		xmlNewProp(cur_node, (const xmlChar *)"showdicts",
			   (const xmlChar *)
			   true_false2yes_no(pt->showdicts));
		xmlNewProp(cur_node, (const xmlChar *)"showparallel",
			   (const xmlChar *)
			   true_false2yes_no(pt->showparallel));
	}
	xmlSaveFormatFile(file, xml_doc, 1);
	g_free(file);
	xmlFreeDoc(xml_doc);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void _save_off_tabs(const gchar *filename)
 *
 * Description
 *   Saves the information for the current set of tabs.
 *
 * Return value
 *
 */

static void _save_off_tab(const gchar *filename)
{
	xmlDocPtr xml_doc;
	xmlNodePtr root_node;
	xmlNodePtr cur_node;
	xmlNodePtr section_node;
	//xmlAttrPtr xml_attr;
	gchar *tabs_dir;
	gchar *file;

	if (NULL == filename)
		filename = no_tab_filename;

	tabs_dir = g_strdup_printf("%s/tabs/", settings.gSwordDir);

	if (g_access(tabs_dir, F_OK) == -1) {
		if ((g_mkdir(tabs_dir, S_IRWXU)) == -1) {
			gui_generic_warning_modal("Can't create tabs dir.");
			return;
		}
	}
	file = g_strdup_printf("%s%s", tabs_dir, filename);
	g_free(tabs_dir);

	xml_doc = xmlNewDoc((const xmlChar *)"1.0");

	if (xml_doc == NULL) {
		gui_generic_warning_modal("Tabs document not created successfully.");
		return;
	}

	root_node = xmlNewNode(NULL, (const xmlChar *)"Xiphos_Tabs");
	//xml_attr =
	xmlNewProp(root_node, (const xmlChar *)"Version",
		   (const xmlChar *)VERSION);
	xmlDocSetRootElement(xml_doc, root_node);

	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *)"tabs", NULL);

	cur_node = xmlNewChild(section_node,
			       NULL, (const xmlChar *)"tab", NULL);
	xmlNewProp(cur_node, (const xmlChar *)"text_mod",
		   (const xmlChar *)settings.MainWindowModule);
	xmlNewProp(cur_node, (const xmlChar *)"commentary_mod",
		   (const xmlChar *)settings.CommWindowModule);
	xmlNewProp(cur_node, (const xmlChar *)"dictlex_mod",
		   (const xmlChar *)settings.DictWindowModule);
	xmlNewProp(cur_node, (const xmlChar *)"book_mod",
		   (const xmlChar *)settings.book_mod);
	xmlNewProp(cur_node, (const xmlChar *)"text_commentary_key",
		   (const xmlChar *)settings.currentverse);
	xmlNewProp(cur_node, (const xmlChar *)"dictlex_key",
		   (const xmlChar *)settings.dictkey);
	xmlNewProp(cur_node, (const xmlChar *)"book_offset",
		   (const xmlChar *)settings.book_key);
	xmlNewProp(cur_node, (const xmlChar *)"comm_showing",
		   (const xmlChar *)
		   true_false2yes_no(settings.comm_showing));
	xmlNewProp(cur_node, (const xmlChar *)"showtexts",
		   (const xmlChar *)
		   true_false2yes_no(settings.showtexts));
	xmlNewProp(cur_node, (const xmlChar *)"showpreview",
		   (const xmlChar *)
		   true_false2yes_no(settings.showpreview));
	xmlNewProp(cur_node, (const xmlChar *)"showcomms",
		   (const xmlChar *)
		   true_false2yes_no(settings.showcomms));
	xmlNewProp(cur_node, (const xmlChar *)"showdicts",
		   (const xmlChar *)
		   true_false2yes_no(settings.showdicts));
	xmlNewProp(cur_node, (const xmlChar *)"showparallel",
		   (const xmlChar *)"no");

	xmlSaveFormatFile(file, xml_doc, 1);
	g_free(file);
	xmlFreeDoc(xml_doc);
}

/******************************************************************************
 * Name
 *   gui_load_tabs
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 * Description
 *   Load the tabs from the given file.
 *
 * Return value
 *
 */

void gui_load_tabs(const gchar *filename)
{
	xmlDocPtr xml_doc;
	xmlNodePtr tmp_node, childnode;
	//const xmlChar *xml_filename;
	gboolean error = FALSE;
	gboolean back_compat_need_save = FALSE;
	settings.showparatab = FALSE;
	PASSAGE_TAB_INFO *pt = NULL, *pt_first = NULL;

	stop_refresh = TRUE;

	if (filename == NULL) {
		error = TRUE;
	} else {
		gchar *file;

		if (g_access(filename, F_OK) == 0) {
			/* we're done, just copy for local use that can be free'd */
			file = g_strdup(filename);
		} else {
			gchar *tabs_dir =
			    g_strdup_printf("%s/tabs/",
					    settings.gSwordDir);
			if (g_access(tabs_dir, F_OK) == -1) {
				XI_message(("Creating new tabs directory\n"));
				gui_save_tabs(filename);
			}
			file = g_strdup_printf("%s%s", tabs_dir, filename);
			g_free(tabs_dir);
		}

		/* we need this for first time non tabbed browsing */
		if (!settings.browsing && g_access(file, F_OK) == -1) {
			_save_off_tab(filename);
		}
		//xml_filename = (const xmlChar *) file;
		xml_doc = xmlParseFile(file);
		g_free(file);
		if (xml_doc == NULL) {
			gui_generic_warning_modal("Tabs document not parsed successfully.");
			error = TRUE;
		} else {
			tmp_node = xmlDocGetRootElement(xml_doc);
			if (tmp_node == NULL) {
				gui_generic_warning_modal("Tabs document is empty.");
				xmlFreeDoc(xml_doc);
				error = TRUE;
			} else if (xmlStrcmp(tmp_node->name,
					     (const xmlChar *)"Xiphos_Tabs")) {
				gui_generic_warning_modal("Tabs document has wrong type, root node != Xiphos_Tabs");
				xmlFreeDoc(xml_doc);
				error = TRUE;
			}
		}

		if (error == FALSE) {
			for (childnode = tmp_node->children;
			     childnode != NULL;
			     childnode = childnode->next) {
				if (!xmlStrcmp(childnode->name,
					       (const xmlChar *)"tabs")) {
					tmp_node = childnode;
					for (tmp_node = tmp_node->children;
					     tmp_node != NULL;
					     tmp_node = tmp_node->next) {
						if (!xmlStrcmp(tmp_node->name,
							       (const xmlChar *)"tab")) {
							gchar *val;

							pt = g_new0(PASSAGE_TAB_INFO,
								    1);
							if (pt_first ==
							    NULL)
								pt_first =
								    pt;

							/* load per-tab module information. */
							val = (gchar *)
							    xmlGetProp(tmp_node,
								       (const xmlChar *)"text_mod");
							pt->text_mod =
							    g_strdup(val);
							xmlFree(val);
							val = (gchar *)
							    xmlGetProp(tmp_node,
								       (const xmlChar *)"commentary_mod");
							pt->commentary_mod =
							    g_strdup(val);
							xmlFree(val);
							val = (gchar *)
							    xmlGetProp(tmp_node,
								       (const xmlChar *)"dictlex_mod");
							pt->dictlex_mod =
							    g_strdup(val);
							xmlFree(val);
							val = (gchar *)
							    xmlGetProp(tmp_node,
								       (const xmlChar *)"book_mod");
							pt->book_mod =
							    g_strdup(val);
							xmlFree(val);
							val = (gchar *)
							    xmlGetProp(tmp_node,
								       (const xmlChar *)"text_commentary_key");
							pt->text_commentary_key = g_strdup(val);
							xmlFree(val);
							val = (gchar *)
							    xmlGetProp(tmp_node,
								       (const xmlChar *)"dictlex_key");
							pt->dictlex_key =
							    g_strdup(val);
							xmlFree(val);
							val = (gchar *)
							    xmlGetProp(tmp_node,
								       (const xmlChar *)"book_offset");
							pt->book_offset =
							    g_strdup(val);
							xmlFree(val);
							val = (gchar *)
							    xmlGetProp(tmp_node,
								       (const xmlChar *)"comm_showing");
							pt->comm_showing =
							    yes_no2true_false(val);
							xmlFree(val);
							val = (gchar *)
							    xmlGetProp(tmp_node,
								       (const xmlChar *)"showparallel");
							pt->showparallel =
							    yes_no2true_false(val);
							xmlFree(val);
							if (pt->showparallel) {
								settings.showparatab = TRUE;
								pt->paratab =
								    gui_create_parallel_tab();
								gtk_box_pack_start(GTK_BOX(widgets.page),
										   pt->paratab,
										   TRUE,
										   TRUE,
										   0);
								gtk_widget_hide(pt->paratab);
								gui_parallel_tab_sync((gchar *)
										      settings.currentverse);
								settings.showparatab = TRUE;
								sync_on =
								    TRUE;
							} else
								pt->paratab = NULL;

							/*
							 * load per-tab "show" state.
							 * includes backward compatibility:
							 * if there is no per-tab state,
							 * take tab state from global state.
							 */
							if ((val =
								 (gchar *)
							     xmlGetProp(tmp_node,
									(const xmlChar *)"showtexts"))) {
								pt->showtexts = yes_no2true_false(val);
								xmlFree(val);
								val =
								    (gchar *)
								    xmlGetProp(tmp_node,
									       (const xmlChar *)"showpreview");
								pt->showpreview = yes_no2true_false(val);
								xmlFree(val);
								val =
								    (gchar *)
								    xmlGetProp(tmp_node,
									       (const xmlChar *)"showcomms");
								pt->showcomms = yes_no2true_false(val);
								xmlFree(val);
								val =
								    (gchar *)
								    xmlGetProp(tmp_node,
									       (const xmlChar *)"showdicts");
								pt->showdicts = yes_no2true_false(val);
								xmlFree(val);

							} else {
								pt->showtexts = settings.showtexts;
								pt->showpreview = settings.showpreview;
								pt->showcomms = settings.showcomms;
								pt->showdicts = settings.showdicts;
								back_compat_need_save = TRUE;
							}

							pt->history_items =
							    0;
							pt->current_history_item = 0;
							pt->first_back_click = TRUE;
							main_add_tab_history_item((PASSAGE_TAB_INFO *)pt);

							gui_reassign_strdup(&settings.currentverse,
									    pt->text_commentary_key);

							passage_list =
							    g_list_append(passage_list,
									  (PASSAGE_TAB_INFO *)pt);
							notebook_main_add_page(pt);
						}
					}
				}
			}
		}
		xmlFreeDoc(xml_doc);

		/* backward compatibility completion. */
		if (back_compat_need_save)
			gui_save_tabs(filename);

		if (error == TRUE || pt == NULL) {
			pt = g_new0(PASSAGE_TAB_INFO, 1);
			pt->text_mod = g_strdup(settings.MainWindowModule);
			pt->commentary_mod =
			    g_strdup(settings.CommWindowModule);
			pt->dictlex_mod =
			    g_strdup(settings.DictWindowModule);
			pt->book_mod = g_strdup(settings.book_mod); //NULL;
			pt->text_commentary_key =
			    g_strdup(settings.currentverse);
			pt->dictlex_key = g_strdup(settings.dictkey);
			pt->book_offset = NULL; //settings.book_offset = atol(xml_get_value( "keys", "offset"));

			pt->paratab = NULL;

			pt->showtexts = settings.showtexts;
			pt->showpreview = settings.showpreview;
			pt->showcomms = settings.showcomms;
			pt->showdicts = settings.showdicts;

			pt->history_items = 0;
			pt->current_history_item = 0;
			pt->first_back_click = TRUE;
			//                      main_add_tab_history_item((PASSAGE_TAB_INFO*)pt);
			passage_list =
			    g_list_append(passage_list,
					  (PASSAGE_TAB_INFO *)pt);
			notebook_main_add_page(pt);
		} else {
			// first passage is current/displayed.
			pt = pt_first;

			pt->paratab = NULL;
			// This is a hack to keep gs from loading the settings
			// from the last session into the last tab loaded here.
			gui_reassign_strdup(&settings.MainWindowModule,
					    pt->text_mod);
			gui_reassign_strdup(&settings.CommWindowModule,
					    pt->commentary_mod);
			gui_reassign_strdup(&settings.DictWindowModule,
					    pt->dictlex_mod);
			gui_reassign_strdup(&settings.book_mod,
					    pt->book_mod);
			gui_reassign_strdup(&settings.currentverse,
					    pt->text_commentary_key);
			gui_reassign_strdup(&settings.dictkey,
					    pt->dictlex_key);
			settings.book_offset = atol(pt->book_offset);
		}
	}

	stop_refresh = FALSE;
	set_current_tab(pt);
	gui_recompute_view_menu_choices();
}

/******************************************************************************
 * Name
 *  on_notebook_main_close_page
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void on_notebook_main_close_page(GtkButton * button, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_notebook_main_close_page(GtkButton *button,
					gpointer user_data)
{
	PASSAGE_TAB_INFO *pt = (PASSAGE_TAB_INFO *)user_data;
	closing_tab = TRUE;
	gui_close_passage_tab(gtk_notebook_page_num(GTK_NOTEBOOK(widgets.notebook_main),
						    pt->page_widget));
	closing_tab = FALSE;
}

/******************************************************************************
 * Name
 *  tab_widget_new
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   GtkWidget* tab_widget_new(PASSAGE_TAB_INFO *tbinf,const gchar *label_text)
 *
 * Description
 *   creates a tab widget that contains a label and a close button
 *
 * Return value
 *   GtkWidget*
 */

static GtkWidget *tab_widget_new(PASSAGE_TAB_INFO *tbinf,
				 const gchar *label_text)
{
	GtkWidget *box;
#ifdef USE_GTK_3
//	GdkRGBA color;
#else
	GdkColor color;
#endif

	g_return_val_if_fail(label_text != NULL, NULL);
#if GTK_CHECK_VERSION(3, 10, 0)
	tbinf->button_close = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);
#if GTK_CHECK_VERSION(3, 20, 0)
	gtk_button_set_relief(GTK_BUTTON(tbinf->button_close), GTK_RELIEF_NONE);
#else
#endif
#else
	GtkWidget *tmp_toolbar_icon = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	tbinf->button_close = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(tbinf->button_close), tmp_toolbar_icon);
	gtk_button_set_relief(GTK_BUTTON(tbinf->button_close), GTK_RELIEF_NONE);
#endif

#ifndef USE_GTK_3
	gtk_rc_parse_string("style \"tab-button-style\"\n"
			    "{\n"
			    "    GtkWidget::focus-padding = 0\n"
			    "    GtkWidget::focus-line-width = 0\n"
			    "    xthickness = 0\n"
			    "    ythickness = 0\n"
			    "    GtkButton::internal-border = {0, 0, 0, 0}\n"
			    "    GtkButton::default-border = {0, 0, 0, 0}\n"
			    "    GtkButton::default-outside-border = {0, 0, 0, 0}\n"
			    "}\n"
			    "widget \"*.button-close\" style \"tab-button-style\"");
	gtk_widget_set_name(GTK_WIDGET(tbinf->button_close),
			    "button-close");
#else
	gtk_widget_set_size_request(tbinf->button_close, 18, 16);
#endif

#ifndef USE_GTK_3
	GtkRequisition r;
	gtk_widget_size_request(tbinf->button_close, &r);
#endif

	gtk_widget_set_sensitive(tbinf->button_close, FALSE);
	gtk_widget_show(tbinf->button_close);
	tbinf->tab_label = GTK_LABEL(gtk_label_new(label_text));
	gtk_widget_show(GTK_WIDGET(tbinf->tab_label));

#ifdef USE_GTK_3
#else
	color.red = 0;
	color.green = 0;
	color.blue = 0;

	gtk_widget_modify_fg(tbinf->button_close, GTK_STATE_NORMAL,
			     &color);
	gtk_widget_modify_fg(tbinf->button_close, GTK_STATE_INSENSITIVE,
			     &color);
	gtk_widget_modify_fg(tbinf->button_close, GTK_STATE_ACTIVE,
			     &color);
	gtk_widget_modify_fg(tbinf->button_close, GTK_STATE_PRELIGHT,
			     &color);
	gtk_widget_modify_fg(tbinf->button_close, GTK_STATE_SELECTED,
			     &color);
#endif

	UI_HBOX(box, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(tbinf->tab_label),
			   TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), tbinf->button_close, FALSE, FALSE,
			   0);

	gtk_widget_show(box);

	g_signal_connect(G_OBJECT(tbinf->button_close), "clicked",
			 G_CALLBACK(on_notebook_main_close_page), tbinf);

	return box;
}

/******************************************************************************
 * Name
 *  gui_notebook_main_switch_page
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_notebook_main_switch_page(GtkNotebook * notebook,
 *				  GtkNotebookPage * page,
 *				  gint page_num, GList * tl)
 *
 * Description
 *   sets gui to new passage
 *
 * Return value
 *   void
 */
#ifdef USE_GTK_3
void gui_notebook_main_switch_page(GtkNotebook *notebook,
				   gpointer arg,
				   gint page_num, GList **tl)
#else
void gui_notebook_main_switch_page(GtkNotebook *notebook,
				   GtkNotebookPage *page,
				   gint page_num, GList **tl)
#endif
{
	gboolean comm_showing;
	gint number_of_pages = gtk_notebook_get_n_pages(notebook);
	PASSAGE_TAB_INFO *pt;

	if (stop_refresh)
		return;

	XI_message(("on_notebook_main_switch_page"));
	page_change = TRUE;
	/* get data structure for new passage */
	/*
	 * this is needed to stop seg fault if the left tab is closed when
	 * there are only two tabs - because number_of_pages equals 2 even
	 * thought there is only 1.
	 */
	if (number_of_pages == 2 && removed_page == 0)
		pt = (PASSAGE_TAB_INFO *)g_list_nth_data(*tl, 0);
	else
		pt = (PASSAGE_TAB_INFO *)g_list_nth_data(*tl, page_num);
	removed_page = 1;
//cur_passage_tab = pt;

#ifdef USE_TREEVIEW_PATH
	if (cur_passage_tab && cur_passage_tab->book_mod)
		gui_collapse_treeview_to_book(GTK_TREE_VIEW(sidebar.module_list),
					      cur_passage_tab->book_mod);
#endif /* USE_TREEVIEW_PATH */

	if (!pt->showparallel) {
		if (cur_passage_tab && cur_passage_tab->paratab)
			gtk_widget_hide(cur_passage_tab->paratab);
		gtk_widget_show(widgets.hpaned);
	}

	set_current_tab(pt);

	companion_activity = TRUE;
	if (pt->showparallel) {
		gtk_widget_hide(widgets.hpaned);
		if (pt->paratab)
			gtk_widget_show(pt->paratab);
		companion_activity = FALSE;
		page_change = FALSE;
		settings.paratab_showing = TRUE;
		return;
	} else
		settings.paratab_showing = FALSE;

	//sets the book mod and key
	main_display_book(pt->book_mod, pt->book_offset);

#ifdef USE_TREEVIEW_PATH
	if (pt->showcomms && pt->book_mod)
		gui_expand_treeview_to_path(GTK_TREE_VIEW(sidebar.module_list),
					    pt->book_mod);
#endif /* USE_TREEVIEW_PATH */

	comm_showing = settings.comm_showing;
	settings.comm_showing = 1;
	//sets the commentary mod and key
	main_display_commentary(pt->commentary_mod,
				pt->text_commentary_key);
	settings.comm_showing = comm_showing;
	//sets the text mod and key
	main_display_bible(pt->text_mod, pt->text_commentary_key);

	navbar_versekey.module_name =
	    g_string_assign(navbar_versekey.module_name, pt->text_mod);
	navbar_versekey.key =
	    g_string_assign(navbar_versekey.key, pt->text_commentary_key);
	main_update_nav_controls(navbar_versekey.module_name->str,
				 pt->text_commentary_key);

	//sets the dictionary mod and key
	main_display_dictionary(pt->dictlex_mod, pt->dictlex_key);

	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book),
				      (pt->comm_showing ? 0 : 1));

	gui_recompute_view_menu_choices();

	companion_activity = FALSE;

	page_change = FALSE;
}

/******************************************************************************
 * Name
 *  gui_select_nth_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_select_nth_tab(gint page_num)
 *
 * Description
 *   from ctrl-DIGIT in main_window.c, select a tab.
 *   the tab list is 0-based: page_num must already be 0-normalized.
 *
 * Return value
 *   void
 */

void gui_select_nth_tab(gint page_num)
{
	gint number_of_pages =
	    gtk_notebook_get_n_pages(GTK_NOTEBOOK(widgets.notebook_main));

	/* within bounds of available tabs? */
	if (page_num >= number_of_pages)
		return;

	settings.tab_page = page_num;
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_main),
				      page_num);
	gui_notebook_main_switch_page(GTK_NOTEBOOK(widgets.notebook_main),
				      NULL, page_num, &passage_list);
}

/******************************************************************************
 * Name
 *  gui_set_tab_label
 *
 * Synopsis
 *   #include "tabbed_browser.h"
if *
 *   void gui_set_tab_label(const char *key, gboolean one_tab)
 *
 * Description
 *   sets tab label(s) to current verse.
 *   dependent on linkedtabs setting, either just cur or all.
 *   one_tab means don't loop the set, just set current.
 *
 * Return value
 *   void
 */

void gui_set_tab_label(const gchar *key, gboolean one_tab)
{
	if (stop_refresh)
		return;

	if ((settings.linkedtabs && !one_tab) || (cur_passage_tab == NULL)) {
		GList *tmp = NULL;
		for (tmp = g_list_first(passage_list); tmp != NULL;
		     tmp = g_list_next(tmp))
			gui_set_named_tab_label(key, (PASSAGE_TAB_INFO *)
						     tmp->data,
						((PASSAGE_TAB_INFO *)
						 tmp->data ==
						 cur_passage_tab));
	} else {
		gui_set_named_tab_label(key, cur_passage_tab, TRUE);
	}
}

/******************************************************************************
 * Name
 *  gui_set_named_tab_label
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_set_named_tab_label(const char *key, PASSAGE_TAB_INFO *pt)
 *
 * Description
 *   sets specified tab label to current verse
 *
 * Return value
 *   void
 */

void gui_set_named_tab_label(const gchar *key, PASSAGE_TAB_INFO *pt,
			     gboolean update)
{
	GString *str;

	if (stop_refresh)
		return;

	gui_reassign_strdup(&pt->text_commentary_key, (char *)key);
	str = pick_tab_label(pt);

	gtk_label_set_text(pt->tab_label, str->str);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.notebook_main),
					 pt->page_widget, str->str);
	if (update)
		main_add_tab_history_item((PASSAGE_TAB_INFO *)pt);
	g_string_free(str, TRUE);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_update_tab_struct(const gchar *text_mod,
			   const gchar *commentary_mod,
			   const gchar *dictlex_mod,
			   const gchar *book_mod,
			   const gchar *dictlex_key,
			   const gchar *book_offset,
			   gboolean comm_showing,
			   gboolean showtexts,
			   gboolean showpreview,
			   gboolean showcomms, gboolean showdicts)
{
	if (stop_refresh)
		return;

	/*	if (!settings.browsing)
	        return;
*/

	if (page_change)
		return;
	if (!cur_passage_tab)
		return;
	cur_passage_tab->showtexts = showtexts;
	cur_passage_tab->showpreview = showpreview;
	cur_passage_tab->showcomms = showcomms;
	cur_passage_tab->showdicts = showdicts;

	if (text_mod) {
		gui_reassign_strdup(&cur_passage_tab->text_mod,
				    (char *)text_mod);
	}
	if (commentary_mod) {
		cur_passage_tab->comm_showing = comm_showing;
		gui_reassign_strdup(&cur_passage_tab->commentary_mod,
				    (char *)commentary_mod);
	}
	if (dictlex_mod) {
		gui_reassign_strdup(&cur_passage_tab->dictlex_mod,
				    (char *)dictlex_mod);
	}
	if (book_mod) {
		cur_passage_tab->comm_showing = comm_showing;
		gui_reassign_strdup(&cur_passage_tab->book_mod,
				    (char *)book_mod);
	}
	if (book_offset) {
		gui_reassign_strdup(&cur_passage_tab->book_offset,
				    (char *)book_offset);
	}
	if (dictlex_key) {
		gui_reassign_strdup(&cur_passage_tab->dictlex_key,
				    (char *)dictlex_key);
	}
}

/******************************************************************************
 * Name
 *  gui_open_passage_in_new_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_open_passage_in_new_tab(gchar *verse_key)
 *
 * Description
 *   opens the given verse in a new passage tab
 *
 * Return value
 *   void
 */
void gui_open_passage_in_new_tab(gchar *verse_key)
{
	PASSAGE_TAB_INFO *pt;

	if (stop_refresh)
		return;

	if (!settings.browsing)
		return;

	if ((pt = g_new0(PASSAGE_TAB_INFO, 1)) == NULL) {
		gui_generic_warning("Could not allocate a new tab");
		return;
	}
	pt->text_mod = g_strdup(settings.MainWindowModule);
	pt->commentary_mod = g_strdup(settings.CommWindowModule);
	pt->dictlex_mod = g_strdup(settings.DictWindowModule);
	pt->book_mod = g_strdup(settings.book_mod);
	pt->paratab = NULL;

	pt->text_commentary_key = g_strdup(verse_key);
	pt->dictlex_key = g_strdup(settings.dictkey);
	pt->book_offset = g_strdup_printf("%ld", settings.book_offset);
	pt->comm_showing = settings.comm_showing;

	if (cur_passage_tab && cur_passage_tab->showparallel) {
		pt->showtexts = 1;
		pt->showcomms = 1;
		pt->showdicts = 1;
	} else {
		pt->showtexts = settings.showtexts;
		pt->showcomms = settings.showcomms;
		pt->showdicts = settings.showdicts;
	}

	pt->showpreview = settings.showpreview;
	pt->showparallel = FALSE;

	pt->history_items = 0;
	pt->current_history_item = 0;
	pt->first_back_click = TRUE;
	main_add_tab_history_item((PASSAGE_TAB_INFO *)pt);

	passage_list =
	    g_list_append(passage_list, (PASSAGE_TAB_INFO *)pt);
	//set_current_tab(pt);
	notebook_main_add_page(pt);

	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_main),
				      gtk_notebook_page_num(GTK_NOTEBOOK(widgets.notebook_main),
							    pt->page_widget));
}

/******************************************************************************
 * Name
 *  gui_open_parallel_view_in_new_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_open_parallel_view_in_new_tab(void)
 *
 * Description
 *   opens the parallel view in a new tab
 *
 * Return value
 *   void
 */

void gui_open_parallel_view_in_new_tab(void)
{
	PASSAGE_TAB_INFO *pt;

	if (stop_refresh)
		return;

	if (!settings.browsing)
		return;

	if ((pt = g_new0(PASSAGE_TAB_INFO, 1)) == NULL) {
		gui_generic_warning("Could not allocate a new tab");
		return;
	}

	pt->showtexts = FALSE;
	pt->showpreview = TRUE;
	pt->showcomms = FALSE;
	pt->showdicts = FALSE;
	pt->showparallel = TRUE;
	pt->text_mod = g_strdup(settings.MainWindowModule);
	pt->commentary_mod = g_strdup(settings.CommWindowModule);
	pt->dictlex_mod = g_strdup(settings.DictWindowModule);
	pt->book_mod = g_strdup(settings.book_mod);
	pt->showbookeditor = FALSE;
	pt->comm_showing = FALSE;

	pt->text_commentary_key = g_strdup(settings.currentverse);
	pt->dictlex_key = g_strdup(settings.dictkey);
	pt->book_offset = g_strdup_printf("%ld", settings.book_offset);

	passage_list =
	    g_list_append(passage_list, (PASSAGE_TAB_INFO *)pt);
	set_current_tab(pt);
	gui_recompute_view_menu_choices();
	notebook_main_add_page(pt);
	pt->paratab = gui_create_parallel_tab();
	gui_parallel_tab_sync((gchar *)settings.currentverse);
	/*gtk_box_pack_start(GTK_BOX(widgets.page), pt->paratab, TRUE, TRUE,
	   0); */
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_main),
				      gtk_notebook_page_num(GTK_NOTEBOOK(widgets.notebook_main),
							    pt->page_widget));
}

/******************************************************************************
 * Name
 *  gui_open_module_in_new_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_open_module_in_new_tab(gchar *verse_key)
 *
 * Description
 *   opens the given module in a new passage tab
 *
 * Return value
 *   void
 */

void gui_open_module_in_new_tab(gchar *module)
{
	PASSAGE_TAB_INFO *pt;
	gint module_type;

	if (stop_refresh)
		return;

	if (!settings.browsing)
		return;

	if ((pt = g_new0(PASSAGE_TAB_INFO, 1)) == NULL) {
		gui_generic_warning("Could not allocate a new tab");
		return;
	}
	module_type = main_get_mod_type(module);

	pt->showtexts = FALSE;
	pt->showpreview = settings.showpreview;
	pt->showcomms = FALSE;
	pt->showdicts = FALSE;
	pt->showparallel = FALSE;
	pt->paratab = NULL;

	switch (module_type) {
	case -1:
		return;
		break;
	case TEXT_TYPE:
		pt->text_mod = g_strdup(module);
		pt->commentary_mod = g_strdup(settings.CommWindowModule);
		pt->dictlex_mod = g_strdup(settings.DictWindowModule);
		pt->book_mod = g_strdup(settings.book_mod);
		pt->showtexts = TRUE;
		break;
	case COMMENTARY_TYPE:
		pt->text_mod = g_strdup(settings.MainWindowModule);
		pt->commentary_mod = g_strdup(module);
		pt->dictlex_mod = g_strdup(settings.DictWindowModule);
		pt->book_mod = g_strdup(settings.book_mod);
		pt->showcomms = TRUE;
		pt->comm_showing = 1;
		break;
	case DICTIONARY_TYPE:
		pt->text_mod = g_strdup(settings.MainWindowModule);
		pt->commentary_mod = g_strdup(settings.CommWindowModule);
		pt->dictlex_mod = g_strdup(module);
		pt->book_mod = g_strdup(settings.book_mod);
		pt->showdicts = TRUE;
		break;
	case BOOK_TYPE:
		pt->text_mod = g_strdup(settings.MainWindowModule);
		pt->commentary_mod = g_strdup(settings.CommWindowModule);
		pt->dictlex_mod = g_strdup(settings.DictWindowModule);
		pt->book_mod = g_strdup(module);
		pt->showcomms = TRUE;
		pt->comm_showing = 0;
		break;
	case PRAYERLIST_TYPE:
		pt->text_mod = g_strdup(settings.MainWindowModule);
		pt->commentary_mod = g_strdup(settings.CommWindowModule);
		pt->dictlex_mod = g_strdup(settings.DictWindowModule);
		pt->book_mod = g_strdup(module);
		pt->showcomms = TRUE;
		pt->comm_showing = 0;
		break;
	}

	pt->text_commentary_key = g_strdup(settings.currentverse);
	pt->dictlex_key = g_strdup(settings.dictkey);
	pt->book_offset = g_strdup_printf("%ld", settings.book_offset);

	passage_list =
	    g_list_append(passage_list, (PASSAGE_TAB_INFO *)pt);
	//set_current_tab(pt);
	notebook_main_add_page(pt);

	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_main),
				      gtk_notebook_page_num(GTK_NOTEBOOK(widgets.notebook_main),
							    pt->page_widget));
}

/******************************************************************************
 * Name
 *  gui_close_all_tabs
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_close_all_tabs(void)
 *
 * Description
 *   called from preferences dialog when disable browsing is chosen
 *
 * Return value
 *   void
 */

void gui_close_all_tabs(void)
{
	gint i;
	gint number_of_pages =
	    gtk_notebook_get_n_pages(GTK_NOTEBOOK(widgets.notebook_main));

	if (stop_refresh)
		return;

	for (i = number_of_pages - 1; i > -1; i--) {
		PASSAGE_TAB_INFO *pt =
		    (PASSAGE_TAB_INFO *)g_list_nth_data(passage_list,
							(guint)i);
		passage_list = g_list_remove(passage_list, pt);
		g_free(pt->text_mod);
		g_free(pt->commentary_mod);
		g_free(pt->dictlex_mod);
		g_free(pt->book_mod);
		g_free(pt->text_commentary_key);
		g_free(pt->dictlex_key);
		g_free(pt->book_offset);
		g_free(pt);
		gtk_notebook_remove_page(GTK_NOTEBOOK(widgets.notebook_main), i);
	}

	g_list_free(passage_list);
	passage_list = NULL;
	cur_passage_tab = NULL;
	//      gui_set_text_frame_label(cur_t);
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
/******************************************************************************
 * Name
 *  gui_open_tabs
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_open_tabs(void)
 *
 * Description
 *   called from preferences dialog when enable browsing is chosen
 *
 * Return value
 *   void
 */
void gui_open_tabs(void)
{
	if (stop_refresh)
		return;

	removed_page = 1;
	cur_passage_tab = NULL;
	passage_list = NULL;

	gui_load_tabs(default_tab_filename);

	gtk_widget_show(widgets.button_new_tab);
}
#endif

/******************************************************************************
 * Name
 *  gui_close_passage_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_close_passage_tab(gint pagenum)
 *
 * Description
 *   closes the given passage tab or the current one if pagenum = -1
 *
 * Return value
 *   void
 */
void gui_close_passage_tab(gint pagenum)
{
	if (stop_refresh)
		return;

	if (-1 == pagenum)
		pagenum =
		    gtk_notebook_get_current_page(GTK_NOTEBOOK(widgets.notebook_main));
	if (1 ==
	    gtk_notebook_get_n_pages(GTK_NOTEBOOK(widgets.notebook_main)))
		return;
	PASSAGE_TAB_INFO *pt =
	    (PASSAGE_TAB_INFO *)g_list_nth_data(passage_list,
						(guint)pagenum);
	if (pt->showparallel || (2 <=
				 gtk_notebook_get_n_pages(GTK_NOTEBOOK(widgets.notebook_main)))) {
		// (surely this is always true!?)

		if (pagenum > 0) {
			gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_main), 0);
			cur_passage_tab = (PASSAGE_TAB_INFO *)
			    g_list_nth_data(passage_list, (guint)0);
		}
	}

	stop_refresh = TRUE;
	sync_windows();
	stop_refresh = FALSE;

	passage_list = g_list_remove(passage_list, pt);
	if (pt->text_mod)
		g_free(pt->text_mod);
	if (pt->commentary_mod)
		g_free(pt->commentary_mod);
	if (pt->dictlex_mod)
		g_free(pt->dictlex_mod);
	if (pt->book_mod)
		g_free(pt->book_mod);
	if (pt->text_commentary_key)
		g_free(pt->text_commentary_key);
	if (pt->dictlex_key)
		g_free(pt->dictlex_key);
	if (pt->book_offset)
		g_free(pt->book_offset);
	if (pt->showparallel) {
		gtk_widget_hide(pt->paratab);
		gui_destroy_parallel_tab();
		settings.showparatab = FALSE;
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.parallel_tab_item),
					       settings.showparatab);
	}
	g_free(pt);
	//cur_passage_tab = NULL;
	removed_page = pagenum;
	gtk_notebook_remove_page(GTK_NOTEBOOK(widgets.notebook_main),
				 pagenum);
}

void on_notebook_main_new_tab_clicked(GtkButton *button,
				      gpointer user_data)
{
	gui_open_passage_in_new_tab(settings.currentverse);
}

/******************************************************************************
 * Name
 *  gui_notebook_main_setup
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_notebook_main_setup(int tabs)
 *
 * Description
 *   set up notebook for browsing multiple passages
 *
 * Return value
 *   void
 */
void gui_notebook_main_setup(int tabs, const char *tabsfile)
{
	if (stop_refresh)
		return;

	removed_page = 1;
	cur_passage_tab = NULL;
	passage_list = NULL;

	gui_load_tabs(tabsfile ? tabsfile
			       : (tabs ? default_tab_filename : no_tab_filename));

	g_signal_connect(G_OBJECT(widgets.notebook_main),
			 "switch-page",
			 G_CALLBACK(gui_notebook_main_switch_page),
			 &passage_list);

	g_signal_connect(G_OBJECT(widgets.button_new_tab), "clicked",
			 G_CALLBACK(on_notebook_main_new_tab_clicked),
			 NULL);

	//show the new tab button here instead of in main_window.c so it
	//doesn't get shown if !settings.browsing
	gtk_widget_show(widgets.button_new_tab);
	gui_notebook_main_switch_page(GTK_NOTEBOOK(widgets.notebook_main),
				      NULL,
				      settings.tab_page, &passage_list);
}

/******************************************************************************
 * Name
 *  gui_notebook_main_shutdown
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *  void gui_notebook_main_shutdown(int tabs)
 *
 * Description
 *   shut down main notebook and clean mem
 *
 * Return value
 *   void
 */
void gui_notebook_main_shutdown(int tabs)
{
	if (stop_refresh)
		return;

	gui_save_tabs(tabs ? default_tab_filename : no_tab_filename);
	passage_list = g_list_first(passage_list);
	while (passage_list != NULL) {
		PASSAGE_TAB_INFO *pt =
		    (PASSAGE_TAB_INFO *)passage_list->data;
		g_free(pt->text_mod);
		g_free(pt->commentary_mod);
		g_free(pt->dictlex_mod);
		g_free(pt->book_mod);
		g_free(pt->text_commentary_key);
		g_free(pt->dictlex_key);
		g_free(pt->book_offset);
		g_free((PASSAGE_TAB_INFO *)passage_list->data);
		passage_list = g_list_next(passage_list);
	}
	g_list_free(passage_list);
	cur_passage_tab = NULL;
}

void gui_tab_set_showtexts(int show)
{
	if (cur_passage_tab)
		cur_passage_tab->showtexts = show;
}

void gui_tab_set_showpreview(int show)
{
	if (cur_passage_tab)
		cur_passage_tab->showpreview = show;
}

void gui_tab_set_showcomms(int show)
{
	if (cur_passage_tab)
		cur_passage_tab->showcomms = show;
}

void gui_tab_set_showdicts(int show)
{
	if (cur_passage_tab)
		cur_passage_tab->showdicts = show;
}

static int _is_paratab_showing(void)
{
	if (settings.paratab_showing)
		return 1;
	return 0;
}

static void _tabs_on(void)
{
	xml_set_value("Xiphos", "tabs", "browsing", "1");
	settings.browsing = TRUE;
	gui_close_all_tabs();
	gui_load_tabs(default_tab_filename);
	gtk_widget_show(widgets.hboxtb);
	gtk_widget_show(widgets.hpaned);
}

static void _tabs_off(void)
{
	int page = _is_paratab_showing();
	xml_set_value("Xiphos", "tabs", "browsing", "0");
	gui_save_tabs(default_tab_filename);
	if (settings.showparatab) {
		gui_close_passage_tab(gtk_notebook_page_num(GTK_NOTEBOOK(widgets.notebook_main),
							    widgets.parallel_tab));
		settings.showparatab = FALSE;
	}
	gui_close_all_tabs();
	settings.browsing = FALSE;
	gui_load_tabs(no_tab_filename);
	gtk_widget_hide(widgets.hboxtb);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
				      page);
}

void gui_tabs_on_off(int on)
{
	if (on)
		_tabs_on();
	else
		_tabs_off();
}
