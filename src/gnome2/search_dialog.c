/*
 * GnomeSword Bible Study Tool
 * search_dialog.c - gui for searching Sword modules
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/widgets/e-unicode.h>
#include <regex.h>

#include "gui/search_dialog.h"
#include "gui/html.h"
#include "gui/gtkhtml_display.h"
#include "gui/utilities.h"
#include "gui/main_window.h"
#include "gui/dialog.h"

#include "main/search.h"
#include "main/configs.h"
#include "main/lists.h"
#include "main/key.h"
#include "main/module.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/commentary.h"
#include "main/bibletext.h"
#include "main/xml.h"

#define SEARCHING N_("Searching the ")
#define SMODULE N_(" Module")
#define FINDS N_("finds in ")

typedef struct _search_dialog SEARCH_DIALOG;
struct _search_dialog {
	GtkWidget *dialog;
	GtkWidget *label_search_module;
	GtkWidget *notebook;
	GtkWidget *togglebutton_show_main;
	GtkWidget *combo_list;
	GtkWidget *combo_entry_list;
	GtkWidget *module_lists;
	GtkWidget *clist_modules;
	GtkWidget *entry_list_name;
	GtkWidget *rb_current_module;
	GtkWidget *label_mod_select;
	GtkWidget *label_scope;
	GtkWidget *rb_custom_range;
	GtkWidget *rb_mod_list;
	GtkWidget *rb_custom_list;
	GtkWidget *rb_last;
	GtkWidget *rb_no_scope;
	GtkToggleButton *which_scope;
	GtkWidget *combo_range;
	GtkWidget *combo_entry_range;
	GtkWidget *clist_range;
	GtkWidget *clist_ranges;
	GtkWidget *entry_range_name;
	GtkWidget *entry_range_text;
	GtkWidget *results_html;
	GtkWidget *report_html;
	GtkWidget *preview_html;
	GtkWidget *search_entry;
	GtkWidget *rb_words;
	GtkWidget *rb_regexp;
	GtkWidget *rb_exact_phrase;
	GtkWidget *cb_case_sensitive;
	GtkWidget *ctree;
	GtkWidget *progressbar;
	GtkWidget *cb_include_strongs;
	GtkWidget *cb_include_morphs;
	GtkWidget *cb_include_footnotes;
	GSList *_201_group;

	EShortcutModel *shortcut_model;
	GtkWidget *shortcut_bar;

	gint text_group;
	gint comm_group;
	gint dict_group;
	gint book_group;
	gint module_group;
	gint range_group;
	gint results_group;
	gint which_bounds;
	gint module_count;
	gint modules_row;
	gint custom_range_row;
	gint custom_list_row;
	gint range_rows;
	gint list_rows;
	
	gboolean show_in_main;

	gchar *search_mod;
};




static gchar *get_modlist_string(GList * mods);

static SEARCH_DIALOG search;
static gboolean is_running = FALSE;



/******************************************************************************
 * Name
 *   gui_add_to_found_list
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void gui_add_to_found_list(gchar * result_text, gchar * module)
 *
 * Description
 *   loads result_text (key) and modlue (name) to the results html widget
 *   we got here from  
 *   on_button_begin_search_clicked -> 
 *   do_dialog_search -> 
 *   backend_do_dialog_search ->
 *   add_to_found_list ->
 *
 * Return value
 *   void
 */

static void add_to_found_list(gchar * result_text, gchar * module)
{
	gchar buf[256];
	GString *str;
	
	str = g_string_new("");

	g_string_printf(str,
		"&nbsp; <A HREF=\"version=%s passage=%s\"NAME=\"%s\" >%s, %s</A><br>",
		module, result_text, module, module, result_text);
	if (str->len) {
		gui_display_html(search.results_html, str->str, str->len);
	}
	g_string_free(str,TRUE);
}

/******************************************************************************
 * Name
 *   get_current_list
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   GList *get_current_list(void)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

static GList *get_current_list(void)
{
	GList *items = NULL;
	gchar *text;
	gint i = 0;

	while (gtk_clist_get_text((GtkCList *) search.clist_modules,
				  i++, 1, &text)) {
		items = g_list_append(items, (gchar *) g_strdup(text));
	}
	return items;
}

/******************************************************************************
 * Name
 *   add_modlist_to_label
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void add_modlist_to_label(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_modlist_to_label(void)
{
	GList *mods = NULL;
	gchar *mod_list;

	mods = get_current_list();
	mod_list = get_modlist_string(mods);
	gtk_label_set_text(GTK_LABEL(search.label_search_module),
			   mod_list);
	g_free(mod_list);
}


/******************************************************************************
 * Name
 *   gui_search_dialog_appbar_update
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void gui_search_dialog_appbar_update(char percent, void *userData)
 *
 * Description
 *   update the progress bar during search
 *
 * Return value
 *   void
 */

void gui_search_dialog_appbar_update(char percent, void *userData)
{
	char maxHashes = *((char *) userData);
	float num;
	char buf[80];
	static char printed = 0;

	while ((((float) percent) / 100) * maxHashes > printed) {
		sprintf(buf, "%f", (((float) percent) / 100));
		num = (float) percent / 100;
		/*gnome_appbar_set_progress((GnomeAppBar *) search.
					  progressbar, num);*/
		printed++;
	}
	while (gtk_events_pending())
		gtk_main_iteration();
	printed = 0;
}


/******************************************************************************
 * Name
 *   button_clean
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void button_clean(GtkButton * button, 
 *						gpointer user_data)
 *
 * Description
 *   FIXME: this is really ugly :(
 *
 * Return value
 *   void
 */

static void button_clean(GtkButton * button, gpointer user_data)
{
	gboolean editable;

	editable = gtk_html_get_editable(GTK_HTML(search.report_html));
	if (!editable)
		gtk_html_set_editable(GTK_HTML(search.report_html),
				      TRUE);
	gtk_html_select_all(GTK_HTML(search.report_html));
	gtk_html_cut(GTK_HTML(search.report_html));
	gtk_html_set_editable(GTK_HTML(search.report_html), editable);


	editable = gtk_html_get_editable(GTK_HTML(search.results_html));
	if (!editable)
		gtk_html_set_editable(GTK_HTML(search.results_html),
				      TRUE);
	gtk_html_select_all(GTK_HTML(search.results_html));
	gtk_html_cut(GTK_HTML(search.results_html));
	gtk_html_set_editable(GTK_HTML(search.results_html), editable);

	editable = gtk_html_get_editable(GTK_HTML(search.preview_html));
	if (!editable)
		gtk_html_set_editable(GTK_HTML(search.preview_html),
				      TRUE);
	gtk_html_select_all(GTK_HTML(search.preview_html));
	gtk_html_cut(GTK_HTML(search.preview_html));
	gtk_html_set_editable(GTK_HTML(search.preview_html), editable);


}


/******************************************************************************
 * Name
 *   get_custom_list_from_name
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   gchar *get_custom_list_from_name(gchar * name)
 *
 * Description
 *   used by setup_search() - returns the search range (scope)
 *
 * Return value
 *   gchar *
 */

static GList *get_custom_list_from_name(const gchar * label)
{
	GList *items = NULL;
	gchar *mod_list = NULL;
	gchar *t;
	gchar *token;

	search.module_count = 0;
	t = ",";

	mod_list =
	    (gchar *) xml_get_list_from_label("modlists", "modlist",
					      label);
	token = strtok(mod_list, t);
	++search.module_count;
	while (token != NULL) {
		++search.module_count;
		items = g_list_append(items, (gchar *)
				      g_strdup(token));
		token = strtok(NULL, t);
	}
	return items;
}


/******************************************************************************
 * Name
 *   get_current_search_mod
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   GList *get_current_search_mod(void)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

static GList *get_current_search_mod(void)
{
	GList *items = NULL;

	items =
	    g_list_append(items, (gchar *) g_strdup(search.search_mod));
	return items;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

gchar *get_modlist_string(GList * mods)
{
	gchar *rv;
	GString *str;

	str = g_string_new("");

	while (mods != NULL) {
		str = g_string_append(str, (gchar *) mods->data);
		mods = g_list_next(mods);
		if (mods != NULL)
			str = g_string_append_c(str, ',');
	}
	g_list_free(mods);
	rv = g_strdup(str->str);
	g_string_free(str, TRUE);
	return rv;
}


/******************************************************************************
 * Name
 *   add_ranges
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void add_ranges(void)
 *
 * Description
 *   add ranges to clist_range
 *
 * Return value
 *   void
 */

static void add_ranges(void)
{
	GList *items = NULL;
	gchar *buf[2];

	gtk_clist_clear(GTK_CLIST(search.clist_range));

	if (xml_set_section_ptr("ranges")) {
		if (xml_get_label()) {
			buf[0] = xml_get_label();
			buf[1] = xml_get_list();
			search.range_rows =
			    gtk_clist_append(GTK_CLIST
					     (search.clist_range), buf);
			items = g_list_append(items, g_strdup(buf[0]));
			g_free(buf[0]);
			g_free(buf[1]);
		}

		while (xml_next_item()) {
			if (xml_get_label()) {
				buf[0] = xml_get_label();
				buf[1] = xml_get_list();
				search.range_rows =
				    gtk_clist_append(GTK_CLIST
						     (search.
						      clist_range),
						     buf);
				items =
				    g_list_append(items,
						  g_strdup(buf[0]));
				g_free(buf[0]);
				g_free(buf[1]);
			}
		}
	}

	if (items != NULL)
		gtk_combo_set_popdown_strings(GTK_COMBO
					      (search.combo_range),
					      items);
	while (items != NULL) {
		g_free((gchar *) items->data);
		items = g_list_next(items);
	}
	g_list_free(items);
}

/******************************************************************************
 * Name
 *   add_modlist
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void add_modlist(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_modlist(void)
{
	GList *items = NULL;
	gchar *buf[2];

	gtk_clist_clear(GTK_CLIST(search.module_lists));

	if (xml_set_section_ptr("modlists")) {
		if (xml_get_label()) {
			buf[0] = xml_get_label();
			buf[1] = xml_get_list();
			search.list_rows =
			    gtk_clist_append(GTK_CLIST
					     (search.module_lists),
					     buf);
			items = g_list_append(items, g_strdup(buf[0]));
			g_free(buf[0]);
			g_free(buf[1]);
		}

		while (xml_next_item()) {
			if (xml_get_label()) {
				buf[0] = xml_get_label();
				buf[1] = xml_get_list();
				search.list_rows =
				    gtk_clist_append(GTK_CLIST
						     (search.
						      module_lists),
						     buf);
				items =
				    g_list_append(items,
						  g_strdup(buf[0]));
				g_free(buf[0]);
				g_free(buf[1]);
			}
		}
	}

	if (items != NULL)
		gtk_combo_set_popdown_strings(GTK_COMBO
					      (search.combo_list),
					      items);

	while (items != NULL) {
		g_free((gchar *) items->data);
		items = g_list_next(items);
	}
	g_list_free(items);
}

/******************************************************************************
 * Name
 *   change_mods_select_label
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void change_mods_select_label(char *mod_name)
 *
 * Description
 *   change module name to current search module name
 *   or list name if searching a list of modules
 *
 * Return value
 *   void
 */

static void change_mods_select_label(char *mod_name)
{
	if (GTK_TOGGLE_BUTTON(search.rb_current_module)->active) {
		gtk_label_set_text(GTK_LABEL(search.label_mod_select),
				   mod_name);
	} else
		add_modlist_to_label();
}


/******************************************************************************
 * Name
 *   fill_group
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void fill_group(gint group_num, GList * mod_lists)
 *
 * Description
 *   fill shortcut bar group with module names
 *
 * Return value
 *   void
 */

static void fill_group(gchar * folder, GList * mod_lists)
{
	GList *tmp = NULL;
	gchar *text[1];
	GdkPixmap *pixmap_closed;
	GdkBitmap *mask_closed;
	GdkPixmap *pixmap_opened;
	GdkBitmap *mask_opened;
	GtkCTreeNode *node;
	extern GdkPixmap *pixmap1;
	extern GdkPixmap *pixmap2;
	extern GdkPixmap *pixmap3;
	extern GdkBitmap *mask1;
	extern GdkBitmap *mask2;
	extern GdkBitmap *mask3;

	text[0] = folder;

	pixmap_opened = pixmap1;
	mask_opened = mask1;
	pixmap_closed = pixmap2;
	mask_closed = mask2;

	node = gtk_ctree_insert_node(GTK_CTREE(search.ctree),
				     NULL,
				     NULL,
				     text,
				     3,
				     pixmap_opened,
				     mask_opened,
				     pixmap_closed,
				     mask_closed, FALSE, FALSE);
	pixmap_closed = NULL;
	mask_closed = NULL;
	pixmap_opened = pixmap3;
	mask_opened = mask3;
	tmp = mod_lists;
	while (tmp != NULL) {
		text[0] = (gchar *) tmp->data;

		gtk_ctree_insert_node(GTK_CTREE(search.ctree),
				      node,
				      NULL,
				      text,
				      3,
				      pixmap_opened,
				      mask_opened,
				      pixmap_closed,
				      mask_closed, TRUE, FALSE);

		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}


/******************************************************************************
 * Name
 *   set_up_search
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void set_up_search(void)
 *
 * Description
 *   checks for and setups some of the search options
 *
 * Return value
 *   void
 */

static void set_up_search(void)
{
	const gchar *label;
	gchar *range = NULL;

	gui_begin_html(search.results_html, TRUE);
	clear_scope();
	if (GTK_TOGGLE_BUTTON(search.rb_custom_range)->active) {
		clear_search_list();
		label =
		    gtk_entry_get_text(GTK_ENTRY
				       (search.combo_entry_range));
		range =
		    (gchar *) xml_get_list_from_label("ranges", "range",
						      label);
		if (range) {
			set_range(range);
			set_scope2range();
		}
	}

	else if (GTK_TOGGLE_BUTTON(search.rb_last)->active) {
		set_scope2last_search();
	}

	else {
		clear_search_list();
	}
}


/******************************************************************************
 * Name
 *   on_button_close
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_button_close(GtkButton * button,
 *				    gpointer user_data)
 *
 * Description
 *   shutdown and destroy the search dialog
 *
 * Return value
 *   void
 */

static void on_button_close(GtkButton * button, gpointer user_data)
{
	is_running = FALSE;
	gtk_widget_destroy(search.dialog);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void check_search_global_options(void)
{
	set_search_global_option("Strong's Numbers", 
		GTK_TOGGLE_BUTTON(search.cb_include_strongs)->active);
	
	set_search_global_option("Morphological Tags", 
		GTK_TOGGLE_BUTTON(search.cb_include_morphs)->active);
	
	set_search_global_option("Footnotes", 
		GTK_TOGGLE_BUTTON(search.cb_include_footnotes)->active);
}



/******************************************************************************
 * Name
 *   on_button_begin_search
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_button_begin_search(GtkButton * button,
 *					   gpointer user_data)
 *
 * Description
 *   starts the search 
 *
 * Return value
 *   void
 */

static void on_button_begin_search(GtkButton * button,
				   gpointer user_data)
{
	gint search_type, search_params, finds;
	const gchar *search_string;
	gchar *module;
	gchar buf[256], *utf8str;
	gint utf8len;
	GList *search_mods = NULL;
	const gchar *key_buf;
	GString *str;

	GtkHTMLStreamStatus status2 = 0;
	GtkHTML *html;
	GtkHTMLStream *htmlstream2;

	str = g_string_new("");
	
	html = GTK_HTML(search.report_html);
	htmlstream2 =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");
	g_string_printf(str,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);
	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream2, str->str,
			       str->len);
	}

	set_up_search();

	gtk_notebook_set_page(GTK_NOTEBOOK(search.notebook), 1);

	search_type = GTK_TOGGLE_BUTTON
	    (search.rb_regexp)->active ? 0 :
	    GTK_TOGGLE_BUTTON(search.rb_exact_phrase)->active ? -1 : -2;

	settings.searchType = search_type;

	search_params = GTK_TOGGLE_BUTTON
	    (search.cb_case_sensitive)->active ? 0 : REG_ICASE;

	search_string =
	    gtk_entry_get_text(GTK_ENTRY(search.search_entry));

	if (GTK_TOGGLE_BUTTON(search.rb_custom_list)->active) {
		const gchar *name;
		name =
		    gtk_entry_get_text(GTK_ENTRY
				       (search.combo_entry_list));
		search_mods = get_custom_list_from_name(name);
	} else if (GTK_TOGGLE_BUTTON(search.rb_mod_list)->active) {
		search_mods = get_current_list();
	} else
		search_mods = get_current_search_mod();
	
	check_search_global_options();
	
	search_mods = g_list_first(search_mods);
	while (search_mods != NULL) {
		module = (gchar *) search_mods->data;

		sprintf(buf, "%s %s %s", SEARCHING, module, SMODULE);
		gnome_appbar_set_status(GNOME_APPBAR
					(search.progressbar), buf);

		finds = do_module_search(module, search_string,
					 search_type, search_params,
					 TRUE);

		while ((key_buf = get_next_result_key()) != NULL) {
			add_to_found_list((gchar *) key_buf,
					  (gchar *) module);
		}

		g_string_printf(str, "%d %s <A HREF=\"%s\">%s</A><br>",
			finds, FINDS, module, module);
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream2,
				       str->str, str->len);
		}

		g_free(module);
		search_mods = g_list_next(search_mods);
	}
	g_list_free(search_mods);

	gtk_html_end(GTK_HTML(html), htmlstream2, status2);
	gui_end_html(search.results_html);
/*	gnome_appbar_set_progress((GnomeAppBar *) search.progressbar,
				  (gfloat) 0);*/
	gnome_appbar_set_status(GNOME_APPBAR(search.progressbar),
				_("Search Finished"));
	g_string_free(str,TRUE);
}


/******************************************************************************
 * Name
 *   on_togglebutton_show_main
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_togglebutton_show_main(GtkToggleButton * togglebutton,
 *						    gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */


static void on_togglebutton_show_main(GtkToggleButton * togglebutton,
				      gpointer user_data)
{
	search.show_in_main = togglebutton->active;
}


/******************************************************************************
 * Name
 *   gui_url
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void gui_url(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Description
 *   called when mouse moves over an url (link)
 *
 * Return value
 *   void
 */

static void url(GtkHTML * html, const gchar * url, gpointer data)
{
	/***  moved out of url - clear appbar  ***/
	if (url == NULL) {


	}
	/***  we are in an url  ***/
	else {
		gchar *buf = NULL, *modbuf = NULL;
		gchar newmod[80], newref[80];
		gint i = 0, havemod = 0;
		gchar *text = NULL;

		/*** thml verse reference ***/
		if (!strncmp(url, "version=", 7)
		    || !strncmp(url, "passage=", 7)) {
			gchar *mybuf = NULL;
			mybuf = strstr(url, "version=");
			if (mybuf) {
				mybuf = strchr(mybuf, '=');
				++mybuf;
				i = 0;
				while (mybuf[i] != ' ') {
					newmod[i] = mybuf[i];
					newmod[i + 1] = '\0';
					++i;
					++havemod;
				}
			}
			mybuf = NULL;
			mybuf = strstr(url, "passage=");
			i = 0;
			if (mybuf) {
				mybuf = strchr(mybuf, '=');
				++mybuf;
				while (i < strlen(mybuf)) {
					newref[i] = mybuf[i];
					newref[i + 1] = '\0';
					++i;
				}
			}
			if (havemod > 2) {
				modbuf = newmod;
			} else {
				modbuf = settings.MainWindowModule;
			}
			buf = g_strdup(newref);

			if (buf) {
				gchar *str;
				text = get_striptext(4, modbuf, buf);
				str = remove_linefeeds(text);
				gnome_appbar_set_status(GNOME_APPBAR
							(search.
							 progressbar),
							str);
				g_free(str);
				g_free(buf);
			}
			if (text)
				free(text);

		}

	}
}


/******************************************************************************
 * Name
 *   link_clicked
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void link_clicked(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

static void link_clicked(GtkHTML * html, const gchar * url,
			 gpointer data)
{
	gchar *buf = NULL, *modbuf = NULL;
	gchar newmod[80], newref[80];
	gint i = 0, havemod = 0;
	gchar *text = NULL;

	/*** thml verse reference ***/
	if (!strncmp(url, "version=", 7)
	    || !strncmp(url, "passage=", 7)) {
		gchar *mybuf = NULL;
		mybuf = strstr(url, "version=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			i = 0;
			while (mybuf[i] != ' ') {
				newmod[i] = mybuf[i];
				newmod[i + 1] = '\0';
				++i;
				++havemod;
			}
		}
		mybuf = NULL;
		mybuf = strstr(url, "passage=");
		i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			while (i < strlen(mybuf)) {
				newref[i] = mybuf[i];
				newref[i + 1] = '\0';
				++i;
			}
		}
		if (havemod > 2) {
			modbuf = newmod;
		} else {
			modbuf = settings.MainWindowModule;
		}
		buf = g_strdup(newref);
		settings.displaySearchResults = TRUE;
		if (GTK_TOGGLE_BUTTON(search.togglebutton_show_main)->
		    active) {
			gui_change_module_and_key(modbuf, buf);

		}
		
		text = get_module_text(5, modbuf, buf);
		entry_display(search.preview_html, modbuf,
			      text, buf, TRUE);
		
		if(search.show_in_main)
			gui_change_module_and_key(modbuf, buf);
		
		settings.displaySearchResults = FALSE;
		
		if (text)
			free(text);
		g_free(buf);

	}
}


/******************************************************************************
 * Name
 *   report_link_clicked
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void report_link_clicked(GtkHTML * html, const gchar * url,
 *				gpointer data)
 *
 * Description
 *   jumps to the start of the module list in result_html
 *
 * Return value
 *   void
 */

static void report_link_clicked(GtkHTML * html, const gchar * url,
				gpointer data)
{
	gtk_html_jump_to_anchor(GTK_HTML(search.results_html), url);
}


/******************************************************************************
 * Name
 *   custom_range_select_row
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void custom_range_select_row(GtkCList * clist, gint row,
 *				    gint column, GdkEvent * event,
 *				    gpointer user_data)
 *
 * Description
 *   row selection in the clist_range has changed
 *   range name entry and range entry are updated
 *
 * Return value
 *   void
 */

static void custom_range_select_row(GtkCList * clist, gint row,
				    gint column, GdkEvent * event,
				    gpointer user_data)
{
	gchar *text, *range;

	search.custom_range_row = row;
	gtk_clist_get_text(clist, row, 0, &text);
	gtk_clist_get_text(clist, row, 1, &range);
	gtk_entry_set_text(GTK_ENTRY(search.entry_range_name), text);
	gtk_entry_set_text(GTK_ENTRY(search.entry_range_text), range);
}


/******************************************************************************
 * Name
 *   custom_list_select_row
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void custom_list_select_row(GtkCList * clist, gint row,
 *				    gint column, GdkEvent * event,
 *				    gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void custom_list_select_row(GtkCList * clist, gint row,
				   gint column, GdkEvent * event,
				   gpointer user_data)
{
	gchar *name, *modules, *module[2];
	GList *tmp = NULL;

	search.custom_list_row = row;
	gtk_clist_get_text(clist, row, 0, &name);
	gtk_clist_get_text(clist, row, 1, &modules);
	gtk_entry_set_text(GTK_ENTRY(search.entry_list_name), name);

	gtk_clist_clear(GTK_CLIST(search.clist_modules));
	tmp =
	    get_custom_list_from_name(gtk_entry_get_text
				      (GTK_ENTRY
				       (search.entry_list_name)));

	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		module[1] = (gchar *) tmp->data;
		module[0] = get_module_description((gchar *) tmp->data);
		gtk_clist_append(GTK_CLIST(search.clist_modules),
				 module);
		g_free((gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	if (!GTK_TOGGLE_BUTTON(search.rb_current_module)->active)
		add_modlist_to_label();
}


/******************************************************************************
 * Name
 *   modules_select_row
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void modules_select_row(GtkCList * clist, gint row,
 *				    gint column, GdkEvent * event,
 *				    gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void modules_select_row(GtkCList * clist, gint row,
			       gint column, GdkEvent * event,
			       gpointer user_data)
{
	search.modules_row = row;
}


/******************************************************************************
 * Name
 *   list_name_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void list_name_changed(GtkEditable * editable,
 *			       gpointer user_data)
 *
 * Description
 *   text in the range name entry has changed
 *   name text in the clist_range is updated to match
 *
 * Return value
 *   void
 */

static void list_name_changed(GtkEditable * editable,
			      gpointer user_data)
{
	const gchar *text;

	text = gtk_entry_get_text(GTK_ENTRY(editable));
	gtk_clist_set_text((GtkCList *) search.module_lists,
			   search.custom_list_row, 0, text);
}


/******************************************************************************
 * Name
 *   range_name_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void range_name_changed(GtkEditable * editable,
 *			       gpointer user_data)
 *
 * Description
 *   text in the range name entry has changed
 *   name text in the clist_range is updated to match
 *
 * Return value
 *   void
 */

static void range_name_changed(GtkEditable * editable,
			       gpointer user_data)
{
	const gchar *text;

	text = gtk_entry_get_text(GTK_ENTRY(editable));
	gtk_clist_set_text((GtkCList *) search.clist_range,
			   search.custom_range_row, 0, text);
}


/******************************************************************************
 * Name
 *   range_text_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void range_text_changed(GtkEditable * editable,
 *			       gpointer user_data)
 *
 * Description
 *   text in the range text entry has changed
 *   text is sent to the backend to get the search range list
 *   then clist_ranges is updated to the range list
 *   and the range text in the clist_range is updated
 *
 * Return value
 *   void
 */

static void range_text_changed(GtkEditable * editable,
			       gpointer user_data)
{
	const gchar *entry;
	gchar *buf = NULL;
	gint count;
	gint i = 0;

	gtk_clist_clear((GtkCList *) search.clist_ranges);
	entry = gtk_entry_get_text(GTK_ENTRY(editable));
	count = start_parse_verse_list(entry);

	while (count--) {
		buf = get_next_verse_list_element(i++);
		if (!buf)
			break;
		gtk_clist_append((GtkCList *) search.clist_ranges,
				 &buf);
		g_free(buf);
	}
	gtk_clist_set_text((GtkCList *) search.clist_range,
			   search.custom_range_row, 1, entry);
}


/******************************************************************************
 * Name
 *   new_modlist
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void new_modlist(GtkButton * button, gpointer user_data)
 *
 * Description
 *   adds a new custom module list to the 
 *
 * Return value
 *   void
 */

static void new_modlist(GtkButton * button, gpointer user_data)
{
	gchar *text[2], buf[80];

	sprintf(buf, "New List%d", search.list_rows);
	text[0] = buf;
	text[1] = "";

	search.module_count = 0;
	gtk_clist_clear((GtkCList *) search.clist_modules);
	search.custom_list_row = gtk_clist_append((GtkCList *) search.
						  module_lists, text);
	search.list_rows = search.custom_list_row;
	gtk_entry_set_text(GTK_ENTRY(search.entry_list_name), text[0]);
	gtk_clist_select_row((GtkCList *) search.module_lists,
			     search.custom_list_row, 0);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void clear_modules(GtkButton * button, gpointer user_data)
{
	gint test;
	GS_DIALOG *info;

	info = gui_new_dialog();
	info->label_top =
	    N_("Do you really want to CLEAR the module list?");
	info->yes = TRUE;
	info->no = TRUE;

	test = gui_gs_dialog(info);
	if (test != GS_YES)
		return;

	gtk_clist_clear((GtkCList *) search.clist_modules);
	gtk_clist_set_text((GtkCList *)
				   search.
				   module_lists,
				   search.custom_list_row, 1, "");
	g_free(info);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void delete_module(GtkButton * button, gpointer user_data)
{
	gint test;
	GS_DIALOG *info;
	GList *mods = NULL;
	gchar *mod_list;

	info = gui_new_dialog();
	info->label_top =
	    N_("Do you really want to REMOVE\n the selected module?");
	info->yes = TRUE;
	info->no = TRUE;

	test = gui_gs_dialog(info);
	if (test != GS_YES)
		return;
	gtk_clist_remove((GtkCList *) search.clist_modules,
			 search.modules_row);

	mods = get_current_list();
	mod_list = get_modlist_string(mods);
	if (mod_list) {
		gtk_clist_set_text((GtkCList *)
				   search.
				   module_lists,
				   search.custom_list_row, 1, mod_list);
		g_free(mod_list);
	}
	g_free(info);
}


/******************************************************************************
 * Name
 *   save_modlist
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void save_modlist(GtkButton * button, gpointer user_data)
 *
 * Description
 *   saves the custom module list
 *
 * Return value
 *   void
 */

static void save_modlist(GtkButton * button, gpointer user_data)
{
	gint i = 0;
	gchar *text1;
	gchar *text2;
	
	for (i = 0; i < (search.list_rows + 1); i++) {

		gtk_clist_get_text((GtkCList *) search.module_lists,
				   i, 0, &text1);
		gtk_clist_get_text((GtkCList *) search.module_lists,
				   i, 1, &text2);
		xml_set_list_item("modlists", "modlist", text1, text2);
	}
	add_modlist();
}


/******************************************************************************
 * Name
 *   new_range
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void new_range(GtkButton * button, gpointer user_data)
 *
 * Description
 *   adds a new custom range to the clist_range
 *
 * Return value
 *   void
 */

static void new_range(GtkButton * button, gpointer user_data)
{
	gchar *text[2];

	text[0] = "[New Range]";
	text[1] = "";

	search.custom_range_row = gtk_clist_append((GtkCList *) search.
						   clist_range, text);
	search.range_rows = search.custom_range_row;
	gtk_entry_set_text(GTK_ENTRY(search.entry_range_name), text[0]);
	gtk_entry_set_text(GTK_ENTRY(search.entry_range_text), "");
	gtk_clist_select_row((GtkCList *) search.clist_range,
			     search.custom_range_row, 0);
}


/******************************************************************************
 * Name
 *   save_range
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void save_range(GtkButton * button, gpointer user_data)
 *
 * Description
 *   saves the custom range list
 *
 * Return value
 *   void
 */

static void save_range(GtkButton * button, gpointer user_data)
{
	gint i = 0;
	gchar *text1;
	gchar *text2;

	for (i = 0; i < (search.range_rows + 1); i++) {

		gtk_clist_get_text((GtkCList *) search.clist_range,
				   i, 0, &text1);
		gtk_clist_get_text((GtkCList *) search.clist_range,
				   i, 1, &text2);
		xml_set_list_item("ranges", "range", text1, text2);
	}
	add_ranges();

}


/******************************************************************************
 * Name
 *   delete_range
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void delete_range(GtkButton * button, gpointer user_data)
 *
 * Description
 *   delete the selected custom range
 *
 * Return value
 *   void
 */

static void delete_range(GtkButton * button, gpointer user_data)
{
	gchar *name_string = NULL;
	gint test;
	GS_DIALOG *info;


	gtk_clist_get_text(GTK_CLIST(search.clist_range),
			   search.custom_range_row, 0, &name_string);

	info = gui_new_dialog();
	info->label_top =
	    N_("Are you sure you want ot delete this range?");
	info->label_bottom = name_string;
	info->yes = TRUE;
	info->no = TRUE;

	test = gui_gs_dialog(info);
	if (test != GS_YES)
		return;

	gtk_clist_remove((GtkCList *) search.clist_range,
			 search.custom_range_row);
	--search.range_rows;
	save_range(NULL, NULL);
	g_free(info);
}


/******************************************************************************
 * Name
 *   delete_list
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void delete_list(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void delete_list(GtkButton * button, gpointer user_data)
{
	gchar *name_string = NULL;
	gint test;
	GS_DIALOG *info;


	gtk_clist_get_text(GTK_CLIST(search.module_lists),
			   search.custom_list_row, 0, &name_string);

	info = gui_new_dialog();
	info->label_top =
	    N_("Are you sure you want ot delete this list?");
	info->label_bottom = name_string;
	info->yes = TRUE;
	info->no = TRUE;

	test = gui_gs_dialog(info);
	if (test != GS_YES)
		return;

	gtk_clist_remove((GtkCList *) search.module_lists,
			 search.custom_list_row);
	--search.list_rows;
	save_modlist(NULL, NULL);

	g_free(info);
}


/******************************************************************************
 * Name
 *   scope_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void scope_toggled(GtkToggleButton *togglebutton,
						gpointer user_data)
 *
 * Description
 *   remember which scope button was pressed last
 *   does not remember rb_last
 *
 * Return value
 *   void
 */

static void scope_toggled(GtkToggleButton * togglebutton,
			  gpointer user_data)
{
	search.which_scope = togglebutton;
}


/******************************************************************************
 * Name
 *   mod_list_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void mod_list_toggled(GtkToggleButton *togglebutton,
						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void mod_list_toggled(GtkToggleButton * togglebutton,
			     gpointer user_data)
{
	if (GTK_TOGGLE_BUTTON(search.rb_current_module)->active)
		gtk_label_set_text(GTK_LABEL(search.label_mod_select),
				   search.search_mod);
	else
		add_modlist_to_label();
}


/******************************************************************************
 * Name
 *   current_module_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void current_module_toggled(GtkToggleButton *togglebutton,
						gpointer user_data)
 *
 * Description
 *   sets rb_last to insensitive if either use module list
 *   or use custom list is clicked
 *   also set scope button to last one used before rb_last
 *
 * Return value
 *   void
 */

static void current_module_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data)
{
	if (togglebutton->active)
		gtk_widget_set_sensitive(search.rb_last, TRUE);
	else {
		gtk_widget_set_sensitive(search.rb_last, FALSE);
		gtk_toggle_button_set_active(search.which_scope, TRUE);
	}
}


/******************************************************************************
 * Name
 *   on_ctree_select_row
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_ctree_select_row(GtkCTree * ctree,
		    GList * node, gint column, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_ctree_select_row(GtkCTree * ctree,
				GList * node, gint column,
				gpointer user_data)
{
	gchar *mod_desc = NULL;
	gchar *mod_list = NULL;
	gchar *mod_name = NULL;
	gchar *text[2];
	GtkCTreeNode *selected_node;
	GList *mods = NULL;

	selected_node = (GtkCTreeNode *) node;
	/* if node is leaf we need to add mod to list */
	if (GTK_CTREE_ROW(selected_node)->is_leaf) {
		mod_desc =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[0])->text;
	} else {
		gtk_ctree_toggle_expansion(ctree, selected_node);
		return;
	}

	if (mod_desc) {
		
		mod_name = module_name_from_description(mod_desc);
	}
	if (mod_name) {
		text[0] = mod_desc;
		text[1] = mod_name;
	} else
		return;

	if (GTK_TOGGLE_BUTTON(search.rb_current_module)->active) {
		search.search_mod = g_strdup(mod_name);
		search.module_count = 1;
	} else {
		gtk_clist_append(GTK_CLIST(search.clist_modules), text);

		mods = get_current_list();
		mod_list = get_modlist_string(mods);
		
		if (mod_list) {
			gtk_clist_set_text((GtkCList *)
					   search.
					   module_lists,
					   search.
					   custom_list_row,
					   1, mod_list);
			g_free(mod_list);
		}
		++search.module_count;
	}
	change_mods_select_label(mod_name);
	g_free(mod_name);
}



/******************************************************************************
 * Name
 *   create_search_dialog
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   GtkWidget *create_search_dialog(void)
 *
 * Description
 *   creates and returns the search dialog
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_search_dialog(void)
{
	GtkWidget *search_dialog;
	GtkWidget *hbox_search;
	GtkWidget *hpaned8;
	GtkWidget *vbox57;
	GtkWidget *frame_search;
	GtkWidget *vbox82;
	GtkWidget *label_search_module;
	GtkWidget *hbox71;
	GtkWidget *toolbar37;
	GtkWidget *label230;
	GtkWidget *vbox_entry;
	GtkWidget *entry17;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *button_begin_search;
	GtkWidget *vseparator25;
	GtkWidget *button_close;
	GtkWidget *frame66;
	GtkWidget *notebook8;
	GtkWidget *frame47;
	GtkWidget *vbox58;
	GtkWidget *frame_scope;
	GtkWidget *vbox77;
	GtkWidget *hbox77;
	GSList *hbox77_group = NULL;
	GtkWidget *rb_no_scope;
	GtkWidget *rb_last;
	GtkWidget *rb_custom_range;
	GtkWidget *hbox78;
	GtkWidget *combo_custom_range;
	GList *combo_custom_range_items = NULL;
	GtkWidget *combo_entry_custom_range;
	GtkWidget *frame_modlists;
	GtkWidget *vbox79;
	GtkWidget *hbox79;
	GSList *hbox79_group = NULL;
	GtkWidget *rb_one_mod;
	GtkWidget *rb_mod_list;
	GtkWidget *rb_custom_list;
	GtkWidget *hbox80;
	GtkWidget *combo31;
	GtkWidget *combo_entry20;
	GtkWidget *hbox74;
	GtkWidget *vbox72;
	GtkWidget *frame49;
	GtkWidget *vbox63;
	GSList *_301_group = NULL;
	GtkWidget *rb_words;
	GtkWidget *tb_regexp;
	GtkWidget *rb_exact_phrase;
	GtkWidget *frame57;
	GtkWidget *vbox64;
	GtkWidget *cb_case_sensitive;
	GtkWidget *label204;
	GtkWidget *frame67a;
	GtkWidget *vbox73;
	GtkWidget *toolbar36;
	GtkWidget *button_modlist_new;
	GtkWidget *button_clear_list;
	GtkWidget *button_save_mods;
	GtkWidget *vseparator27;
	GtkWidget *button_delete_list;
	GtkWidget *hbox72;
	GtkWidget *hbox75;
	GtkWidget *hbox82;
	GtkWidget *scrolledwindow68;
	GtkWidget *label236;
	GtkWidget *label237;
	GtkWidget *frame67;
	GtkWidget *vbox80;
	GtkWidget *frame69;
	GtkWidget *entry_modlist_name;
	GtkWidget *frame68;
	GtkWidget *vbox81;
	GtkWidget *toolbar39;
	GtkWidget *button28;
	GtkWidget *vseparator33;
	GtkWidget *button30;
	GtkWidget *scrolledwindow66;
	GtkWidget *scrolledwindow_ctree;
	GtkWidget *clist_modules;
	GtkWidget *label232;
	GtkWidget *label233;
	GtkWidget *label205;
	GtkWidget *frame_custom_ranges;
	GtkWidget *vbox78;
	GtkWidget *toolbar38;
	GtkWidget *button_new_range;
	GtkWidget *button_save_range;
	GtkWidget *vseparator26;
	GtkWidget *button_delete_range;
	GtkWidget *hbox76;
	GtkWidget *scrolledwindow67;
	GtkWidget *clist_custom_range;
	GtkWidget *label234;
	GtkWidget *label235;
	GtkWidget *frame66a;
	GtkWidget *vbox76;
	GtkWidget *label227;
	GtkWidget *entry_range_name;
	GtkWidget *label228;
	GtkWidget *entry_range_text;
	GtkWidget *label229;
	GtkWidget *scrolledwindow65;
	GtkWidget *clist_ranges;
	GtkWidget *label226;
	GtkWidget *label206;
	GtkWidget *vbox70;
	GtkWidget *toolbar35;
	GtkWidget *togglebutton_show_main;
	GtkWidget *vseparator22;
	GtkWidget *button_save_results;
	GtkWidget *vseparator23;
	GtkWidget *button_clear_results;
	GtkWidget *hbox68;
	GtkWidget *hpaned7;
	GtkWidget *vpaned3;
	GtkWidget *vbox65;
	GtkWidget *frame_report;
	GtkWidget *scrolledwindow_report;
	GtkWidget *frame_preview;
	GtkWidget *scrolledwindow_preview;
	GtkWidget *frame_results;
	GtkWidget *scrolledwindow_results;
	GtkWidget *appbar_search;
	GtkWidget *label207;
	GtkTooltips *tooltips;
	gchar title[256];

	sprintf(title, "%s - %s", settings.program_title, _("Search"));

	tooltips = gtk_tooltips_new();

	search_dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(search_dialog), "search_dialog",
			    search_dialog);
	gtk_window_set_title(GTK_WINDOW(search_dialog), title);
	gtk_window_set_default_size(GTK_WINDOW(search_dialog), 598,
				    337);
	gtk_window_set_policy(GTK_WINDOW(search_dialog), TRUE, TRUE,
			      FALSE);

	hbox_search = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox_search);
	gtk_container_add(GTK_CONTAINER(search_dialog), hbox_search);

	hpaned8 = gtk_hpaned_new();
	gtk_widget_show(hpaned8);
	gtk_box_pack_start(GTK_BOX(hbox_search), hpaned8, TRUE, TRUE,
			   0);
	gtk_paned_set_position(GTK_PANED(hpaned8), 150);

	vbox57 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox57);
	gtk_paned_pack2(GTK_PANED(hpaned8), vbox57, TRUE, TRUE);

	frame_search = gtk_frame_new(_("Search"));
	gtk_widget_show(frame_search);
	gtk_box_pack_start(GTK_BOX(vbox57), frame_search, FALSE, TRUE,
			   0);

	vbox82 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox82);
	gtk_container_add(GTK_CONTAINER(frame_search), vbox82);

	label_search_module = gtk_label_new(_("<Module>"));
	gtk_widget_show(label_search_module);
	gtk_box_pack_start(GTK_BOX(vbox82), label_search_module, TRUE,
			   TRUE, 0);
	gtk_widget_set_usize(label_search_module, 80, -2);
	gtk_label_set_justify(GTK_LABEL(label_search_module),
			      GTK_JUSTIFY_LEFT);

	hbox71 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox71);
	gtk_box_pack_start(GTK_BOX(vbox82), hbox71, TRUE, TRUE, 0);

	toolbar37 =
	    gtk_toolbar_new();
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar37), GTK_TOOLBAR_ICONS);
	gtk_widget_show(toolbar37);
	gtk_box_pack_start(GTK_BOX(hbox71), toolbar37, TRUE, TRUE, 0);
/*	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar37),
				      GTK_RELIEF_NONE);*/

	label230 = gtk_label_new(_(" for "));
	gtk_widget_show(label230);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar37), label230,
				  NULL, NULL);

	vbox_entry = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_entry);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar37), vbox_entry,
				  NULL, NULL);

	entry17 = gtk_entry_new();
	gtk_widget_show(entry17);
	gtk_box_pack_start(GTK_BOX(vbox_entry), entry17, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(entry17, 350, -2);
	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-search", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar37)));
	button_begin_search =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar37),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Begin"), _("Begin search"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(button_begin_search);

	vseparator25 = gtk_vseparator_new();
	gtk_widget_show(vseparator25);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar37), vseparator25,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator25, 6, 7);
	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-exit", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar37)));
	button_close =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar37),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Close"),
				       _("Close search dialog"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(button_close);

	frame66 = gtk_frame_new(NULL);
	gtk_widget_show(frame66);
	gtk_box_pack_start(GTK_BOX(vbox57), frame66, TRUE, TRUE, 0);

	notebook8 = gtk_notebook_new();
	gtk_widget_show(notebook8);
	gtk_container_add(GTK_CONTAINER(frame66), notebook8);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook8), TRUE);
	gtk_notebook_set_tab_hborder(GTK_NOTEBOOK(notebook8), 8);
	gtk_notebook_set_tab_vborder(GTK_NOTEBOOK(notebook8), 1);

	frame47 = gtk_frame_new(NULL);
	gtk_widget_show(frame47);
	gtk_container_add(GTK_CONTAINER(notebook8), frame47);

	vbox58 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox58);
	gtk_container_add(GTK_CONTAINER(frame47), vbox58);

	frame_scope = gtk_frame_new(_("Scope"));
	gtk_widget_show(frame_scope);
	gtk_box_pack_start(GTK_BOX(vbox58), frame_scope, FALSE, TRUE,
			   0);

	vbox77 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox77);
	gtk_container_add(GTK_CONTAINER(frame_scope), vbox77);

	hbox77 = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox77);
	gtk_box_pack_start(GTK_BOX(vbox77), hbox77, TRUE, TRUE, 0);

	rb_no_scope =
	    gtk_radio_button_new_with_label(hbox77_group,
					    _("Entire Module   "));
	hbox77_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_no_scope));
	gtk_widget_show(rb_no_scope);
	gtk_box_pack_start(GTK_BOX(hbox77), rb_no_scope, TRUE, TRUE, 0);
	gtk_tooltips_set_tip(tooltips, rb_no_scope,
			     _("Search entire module(s)"), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_no_scope),
				     TRUE);

	rb_last =
	    gtk_radio_button_new_with_label(hbox77_group,
					    _("Current Results   "));
	hbox77_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_last));
	gtk_widget_show(rb_last);
	gtk_box_pack_start(GTK_BOX(hbox77), rb_last, TRUE, TRUE, 0);
	gtk_tooltips_set_tip(tooltips, rb_last,
			     _
			     ("Use the results of the last search for the search scope"),
			     NULL);

	rb_custom_range =
	    gtk_radio_button_new_with_label(hbox77_group,
					    _("Custom Range   "));
	hbox77_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_custom_range));
	gtk_widget_show(rb_custom_range);
	gtk_box_pack_start(GTK_BOX(hbox77), rb_custom_range, TRUE, TRUE,
			   0);
	gtk_tooltips_set_tip(tooltips, rb_custom_range,
			     _("Use a custom search range"), NULL);

	hbox78 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox78);
	gtk_box_pack_start(GTK_BOX(vbox77), hbox78, TRUE, TRUE, 0);

	combo_custom_range = gtk_combo_new();
	gtk_widget_show(combo_custom_range);
	gtk_box_pack_start(GTK_BOX(hbox78), combo_custom_range, TRUE,
			   TRUE, 0);
	combo_custom_range_items =
	    g_list_append(combo_custom_range_items, (gpointer) "");
	gtk_combo_set_popdown_strings(GTK_COMBO(combo_custom_range),
				      combo_custom_range_items);
	g_list_free(combo_custom_range_items);

	combo_entry_custom_range = GTK_COMBO(combo_custom_range)->entry;
	gtk_widget_show(combo_entry_custom_range);

	frame_modlists = gtk_frame_new(_("Modules"));
	gtk_widget_show(frame_modlists);
	gtk_box_pack_start(GTK_BOX(vbox58), frame_modlists, FALSE, TRUE,
			   0);

	vbox79 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox79);
	gtk_container_add(GTK_CONTAINER(frame_modlists), vbox79);

	hbox79 = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox79);
	gtk_box_pack_start(GTK_BOX(vbox79), hbox79, TRUE, TRUE, 0);

	rb_one_mod =
	    gtk_radio_button_new_with_label(hbox79_group,
					    _("Single Module"));
	hbox79_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_one_mod));
	gtk_widget_show(rb_one_mod);
	gtk_box_pack_start(GTK_BOX(hbox79), rb_one_mod, FALSE, TRUE, 0);
	gtk_tooltips_set_tip(tooltips, rb_one_mod,
			     _("Use a single module for the search"),
			     NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_one_mod),
				     TRUE);

	rb_mod_list =
	    gtk_radio_button_new_with_label(hbox79_group,
					    _("Module List"));
	hbox79_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_mod_list));
	gtk_widget_show(rb_mod_list);
	gtk_box_pack_start(GTK_BOX(hbox79), rb_mod_list, FALSE, TRUE,
			   0);
	gtk_tooltips_set_tip(tooltips, rb_mod_list,
			     _("Use current module list for search"),
			     NULL);

	rb_custom_list =
	    gtk_radio_button_new_with_label(hbox79_group,
					    _("Custom List"));
	hbox79_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_custom_list));
	gtk_widget_show(rb_custom_list);
	gtk_box_pack_start(GTK_BOX(hbox79), rb_custom_list, FALSE, TRUE,
			   0);
	gtk_tooltips_set_tip(tooltips, rb_custom_list,
			     _
			     ("Use a custom module list for the search"),
			     NULL);

	hbox80 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox80);
	gtk_box_pack_start(GTK_BOX(vbox79), hbox80, TRUE, TRUE, 0);

	combo31 = gtk_combo_new();
	gtk_widget_show(combo31);
	gtk_box_pack_start(GTK_BOX(hbox80), combo31, TRUE, TRUE, 0);

	combo_entry20 = GTK_COMBO(combo31)->entry;
	gtk_widget_show(combo_entry20);

	hbox74 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox74);
	gtk_box_pack_start(GTK_BOX(vbox58), hbox74, TRUE, TRUE, 0);

	vbox72 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox72);
	gtk_box_pack_start(GTK_BOX(hbox74), vbox72, TRUE, TRUE, 0);

	frame49 = gtk_frame_new(_("Search Type"));
	gtk_widget_show(frame49);
	gtk_box_pack_start(GTK_BOX(vbox72), frame49, TRUE, TRUE, 0);

	vbox63 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox63);
	gtk_container_add(GTK_CONTAINER(frame49), vbox63);

	rb_words =
	    gtk_radio_button_new_with_label(_301_group,
					    _("Multi Word"));
	_301_group = gtk_radio_button_group(GTK_RADIO_BUTTON(rb_words));
	gtk_widget_show(rb_words);
	gtk_box_pack_start(GTK_BOX(vbox63), rb_words, FALSE, FALSE, 0);

	tb_regexp =
	    gtk_radio_button_new_with_label(_301_group,
					    _("Regular Expression"));
	_301_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(tb_regexp));
	gtk_widget_show(tb_regexp);
	gtk_box_pack_start(GTK_BOX(vbox63), tb_regexp, FALSE, FALSE, 0);

	rb_exact_phrase =
	    gtk_radio_button_new_with_label(_301_group,
					    _("Exact Phrase"));
	_301_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_exact_phrase));
	gtk_widget_show(rb_exact_phrase);
	gtk_box_pack_start(GTK_BOX(vbox63), rb_exact_phrase, FALSE,
			   FALSE, 0);

	frame57 = gtk_frame_new(_("Options"));
	gtk_widget_show(frame57);
	gtk_box_pack_start(GTK_BOX(hbox74), frame57, TRUE, TRUE, 0);

	vbox64 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox64);
	gtk_container_add(GTK_CONTAINER(frame57), vbox64);

	cb_case_sensitive =
	    gtk_check_button_new_with_label(_("Case Sensitive"));
	gtk_widget_show(cb_case_sensitive);
	gtk_box_pack_start(GTK_BOX(vbox64), cb_case_sensitive, FALSE,
			   FALSE, 0);

	search.cb_include_strongs =
	    gtk_check_button_new_with_label(_
					    ("Include Strongs Numbers"));
	gtk_widget_show(search.cb_include_strongs);
	gtk_box_pack_start(GTK_BOX(vbox64), search.cb_include_strongs, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, search.cb_include_strongs,
			     _("In modules that have Strongs Numbers"),
			     NULL);

	search.cb_include_morphs =
	    gtk_check_button_new_with_label(_("Include Morph Tags"));
	gtk_widget_show(search.cb_include_morphs);
	gtk_box_pack_start(GTK_BOX(vbox64), search.cb_include_morphs, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, search.cb_include_morphs,
			     _("In modules that have Morph Tags"),
			     NULL);

	search.cb_include_footnotes =
	    gtk_check_button_new_with_label(_("Include Footnotes"));
	gtk_widget_show(search.cb_include_footnotes);
	gtk_box_pack_start(GTK_BOX(vbox64), search.cb_include_footnotes, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, search.cb_include_footnotes,
			     _("In modules that have Footnotes"), NULL);

	label204 = gtk_label_new(_("Search Criteria"));
	gtk_widget_show(label204);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook8),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook8), 0),
				   label204);


/*****************************************************page 3 */
	vbox70 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox70);
	gtk_container_add(GTK_CONTAINER(notebook8), vbox70);

	toolbar35 =
	    gtk_toolbar_new();
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar35), GTK_TOOLBAR_ICONS);
	gtk_widget_show(toolbar35);
	gtk_box_pack_start(GTK_BOX(vbox70), toolbar35, FALSE, FALSE, 0);
/*	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar35),
				      GTK_RELIEF_NONE);*/
				      
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-jump-to", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar35)));
	togglebutton_show_main =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar35),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Show Context"),
				       _("Show In Main window"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(togglebutton_show_main);

	vseparator22 = gtk_vseparator_new();
	gtk_widget_show(vseparator22);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar35), vseparator22,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator22, 6, 7);

	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-save-as", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar35)));
	button_save_results =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar35),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save Results"),
				       _
				       ("Save Search Results as Bookmarks"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(button_save_results);
	gtk_widget_set_sensitive(button_save_results, FALSE);

	vseparator23 = gtk_vseparator_new();
	gtk_widget_show(vseparator23);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar35), vseparator23,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator23, 6, 7);

	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-clear", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar35)));
	button_clear_results =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar35),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Clear"),
				       _("clear lists for new search"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(button_clear_results);

	hbox68 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox68);
	gtk_box_pack_start(GTK_BOX(vbox70), hbox68, TRUE, TRUE, 0);

	hpaned7 = gtk_hpaned_new();
	gtk_widget_show(hpaned7);
	gtk_box_pack_start(GTK_BOX(hbox68), hpaned7, TRUE, TRUE, 0);
	gtk_paned_set_position(GTK_PANED(hpaned7), 187);

	vpaned3 = gtk_vpaned_new();
	gtk_widget_show(vpaned3);
	gtk_paned_pack1(GTK_PANED(hpaned7), vpaned3, FALSE, TRUE);
	gtk_paned_set_position(GTK_PANED(vpaned3), 88);

	vbox65 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox65);
	gtk_paned_pack1(GTK_PANED(vpaned3), vbox65, FALSE, TRUE);

	frame_report = gtk_frame_new(_("Search Results"));
	gtk_widget_show(frame_report);
	gtk_box_pack_start(GTK_BOX(vbox65), frame_report, TRUE, TRUE,
			   0);
	gtk_widget_set_usize(frame_report, 212, -2);

	scrolledwindow_report = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_report);
	gtk_container_add(GTK_CONTAINER(frame_report),
			  scrolledwindow_report);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_report),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	frame_preview = gtk_frame_new(_("Preview"));
	gtk_widget_show(frame_preview);
	gtk_paned_pack2(GTK_PANED(vpaned3), frame_preview, TRUE, TRUE);

	scrolledwindow_preview = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_preview);
	gtk_container_add(GTK_CONTAINER(frame_preview),
			  scrolledwindow_preview);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_preview),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	frame_results = gtk_frame_new(_("Found"));
	gtk_widget_show(frame_results);
	gtk_paned_pack2(GTK_PANED(hpaned7), frame_results, TRUE, TRUE);

	scrolledwindow_results = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_results);
	gtk_container_add(GTK_CONTAINER(frame_results),
			  scrolledwindow_results);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_results),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	appbar_search =
	    gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gtk_widget_show(appbar_search);
	gtk_box_pack_start(GTK_BOX(vbox70), appbar_search, FALSE, FALSE,
			   0);

	label207 = gtk_label_new(_("Results"));
	gtk_widget_show(label207);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook8),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook8), 1),
				   label207);

/************************************************************* page 2 */
	frame_custom_ranges = gtk_frame_new(NULL);
	gtk_widget_show(frame_custom_ranges);
	gtk_container_add(GTK_CONTAINER(notebook8),
			  frame_custom_ranges);

	vbox78 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox78);
	gtk_container_add(GTK_CONTAINER(frame_custom_ranges), vbox78);

	toolbar38 =
	    gtk_toolbar_new();
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar38), GTK_TOOLBAR_ICONS);
	gtk_widget_show(toolbar38);
	gtk_box_pack_start(GTK_BOX(vbox78), toolbar38, FALSE, FALSE, 0);
/*	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar38),
				      GTK_RELIEF_NONE);*/
				      
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-new", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar38)));
	button_new_range =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar38),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("New"),
				       _("Create new custom range"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(button_new_range);

	tmp_toolbar_icon = tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-save", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar38)));
	button_save_range =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar38),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save List"),
				       _("Save custom range"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(button_save_range);

	vseparator26 = gtk_vseparator_new();
	gtk_widget_show(vseparator26);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar38), vseparator26,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator26, 6, 7);

	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-cut", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar38)));
	button_delete_range =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar38),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Delete"),
				       _("Delete selected range"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(button_delete_range);

	hbox76 = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox76);
	gtk_box_pack_start(GTK_BOX(vbox78), hbox76, TRUE, TRUE, 0);

	scrolledwindow67 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow67);
	gtk_box_pack_start(GTK_BOX(hbox76), scrolledwindow67, TRUE,
			   TRUE, 0);
	gtk_widget_set_usize(scrolledwindow67, 175, -2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow67),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	clist_custom_range = gtk_clist_new(2);
	gtk_widget_show(clist_custom_range);
	gtk_container_add(GTK_CONTAINER(scrolledwindow67),
			  clist_custom_range);
	gtk_clist_set_column_width(GTK_CLIST(clist_custom_range), 0,
				   194);
	gtk_clist_set_column_width(GTK_CLIST(clist_custom_range), 1,
				   80);
	gtk_clist_column_titles_hide(GTK_CLIST(clist_custom_range));

	label234 = gtk_label_new("");
	gtk_widget_show(label234);
	gtk_clist_set_column_widget(GTK_CLIST(clist_custom_range), 0,
				    label234);

	label235 = gtk_label_new("");
	gtk_widget_show(label235);
	gtk_clist_set_column_widget(GTK_CLIST(clist_custom_range), 1,
				    label235);

	frame66a = gtk_frame_new(NULL);
	gtk_widget_show(frame66a);
	gtk_box_pack_start(GTK_BOX(hbox76), frame66a, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame66a, 176, -2);

	vbox76 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox76);
	gtk_container_add(GTK_CONTAINER(frame66a), vbox76);

	label227 = gtk_label_new(_("Range Name"));
	gtk_widget_show(label227);
	gtk_box_pack_start(GTK_BOX(vbox76), label227, FALSE, FALSE, 0);
	gtk_widget_set_usize(label227, -2, 26);

	entry_range_name = gtk_entry_new();;
	gtk_widget_show(entry_range_name);
	gtk_box_pack_start(GTK_BOX(vbox76), entry_range_name, FALSE,
			   FALSE, 0);

	label228 = gtk_label_new(_("Range Text"));
	gtk_widget_show(label228);
	gtk_box_pack_start(GTK_BOX(vbox76), label228, FALSE, FALSE, 0);
	gtk_widget_set_usize(label228, -2, 26);

	entry_range_text = gtk_entry_new();
	gtk_widget_show(entry_range_text);
	gtk_box_pack_start(GTK_BOX(vbox76), entry_range_text, FALSE,
			   FALSE, 0);

	label229 = gtk_label_new(_("eg. Matt-John;Rev4"));
	gtk_widget_show(label229);
	gtk_box_pack_start(GTK_BOX(vbox76), label229, FALSE, FALSE, 0);
	gtk_widget_set_usize(label229, -2, 26);

	scrolledwindow65 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow65);
	gtk_box_pack_start(GTK_BOX(vbox76), scrolledwindow65, TRUE,
			   TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow65),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	clist_ranges = gtk_clist_new(1);
	gtk_widget_show(clist_ranges);
	gtk_container_add(GTK_CONTAINER(scrolledwindow65),
			  clist_ranges);
	gtk_clist_set_column_width(GTK_CLIST(clist_ranges), 0, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(clist_ranges));

	label226 = gtk_label_new("");
	gtk_widget_show(label226);
	gtk_clist_set_column_widget(GTK_CLIST(clist_ranges), 0,
				    label226);

	label206 = gtk_label_new(_("Custom Ranges"));
	gtk_widget_show(label206);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook8),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook8), 2),
				   label206);

/********************************************************** page 1 */
	frame67a = gtk_frame_new(NULL);
	gtk_widget_show(frame67a);
	gtk_container_add(GTK_CONTAINER(notebook8), frame67a);

	vbox73 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox73);
	gtk_container_add(GTK_CONTAINER(frame67a), vbox73);

	toolbar36 =
	    gtk_toolbar_new();
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar36), GTK_TOOLBAR_ICONS);
	gtk_widget_show(toolbar36);
	gtk_box_pack_start(GTK_BOX(vbox73), toolbar36, FALSE, FALSE, 0);
/*	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar36),
				      GTK_RELIEF_NONE);*/

	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-new", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar36)));
	button_modlist_new =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar36),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("New"),
				       _("Create new module list"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(button_modlist_new);

	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-clear", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar36)));
	button_clear_list =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar36),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Clear Module List"),
				       _("Clear Module List"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(button_clear_list);

	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-save", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar36)));
	button_save_mods =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar36),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save List"),
				       _("Save list of modules"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(button_save_mods);

	vseparator27 = gtk_vseparator_new();
	gtk_widget_show(vseparator27);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar36), vseparator27,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator27, 6, 7);

	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-cut", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar36)));
	button_delete_list =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar36),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Delete"),
				       _("Delete selected list"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(button_delete_list);

	hbox72 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox72);
	gtk_box_pack_start(GTK_BOX(vbox73), hbox72, TRUE, TRUE, 0);

	hbox75 = gtk_hbox_new(FALSE, 0);;
	gtk_widget_show(hbox75);
	gtk_box_pack_start(GTK_BOX(hbox72), hbox75, TRUE, TRUE, 0);

	hbox82 = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox82);
	gtk_box_pack_start(GTK_BOX(hbox75), hbox82, TRUE, TRUE, 0);

	scrolledwindow68 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow68);
	gtk_box_pack_start(GTK_BOX(hbox82), scrolledwindow68, TRUE,
			   TRUE, 0);
	gtk_widget_set_usize(scrolledwindow68, 175, -2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow68),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	search.module_lists = gtk_clist_new(2);
	gtk_widget_show(search.module_lists);
	gtk_container_add(GTK_CONTAINER(scrolledwindow68),
			  search.module_lists);
	gtk_clist_set_column_width(GTK_CLIST(search.module_lists), 0,
				   169);
	gtk_clist_set_column_width(GTK_CLIST(search.module_lists), 1,
				   80);
	gtk_clist_column_titles_hide(GTK_CLIST(search.module_lists));

	label236 = gtk_label_new("");
	gtk_widget_show(label236);
	gtk_clist_set_column_widget(GTK_CLIST(search.module_lists), 0,
				    label236);

	label237 = gtk_label_new("");
	gtk_widget_show(label237);
	gtk_clist_set_column_widget(GTK_CLIST(search.module_lists), 1,
				    label237);

	frame67 = gtk_frame_new(NULL);
	gtk_widget_show(frame67);
	gtk_box_pack_start(GTK_BOX(hbox82), frame67, FALSE, TRUE, 0);

	vbox80 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox80);
	gtk_container_add(GTK_CONTAINER(frame67), vbox80);

	frame69 = gtk_frame_new(_("List Name"));
	gtk_widget_show(frame69);
	gtk_box_pack_start(GTK_BOX(vbox80), frame69, FALSE, TRUE, 0);

	entry_modlist_name = gtk_entry_new();
	gtk_widget_show(entry_modlist_name);
	gtk_container_add(GTK_CONTAINER(frame69), entry_modlist_name);

	frame68 = gtk_frame_new(_("Modules"));
	gtk_widget_show(frame68);
	gtk_box_pack_start(GTK_BOX(vbox80), frame68, TRUE, TRUE, 0);

	vbox81 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox81);
	gtk_container_add(GTK_CONTAINER(frame68), vbox81);

	toolbar39 =
	    gtk_toolbar_new();
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar39), GTK_TOOLBAR_ICONS);
	gtk_widget_show(toolbar39);
	gtk_box_pack_start(GTK_BOX(vbox81), toolbar39, FALSE, FALSE, 0);
/*	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar39),
				      GTK_RELIEF_NONE);*/

	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-clear", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar39)));
	button28 =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar39),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Clear Module List"),
				       _("Clear all modules from list"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(button28);

	vseparator33 = gtk_vseparator_new();
	gtk_widget_show(vseparator33);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar39), vseparator33,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator33, 6, 7);

	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			"gtk-cut", 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar39)));
	button30 =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar39),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Delete"),
				       _
				       ("Remove selected module from list"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(button30);

	scrolledwindow66 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow66);
	gtk_box_pack_start(GTK_BOX(vbox81), scrolledwindow66, TRUE,
			   TRUE, 0);
	gtk_widget_set_usize(scrolledwindow66, 240, -2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow66),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	clist_modules = gtk_clist_new(2);
	gtk_widget_show(clist_modules);
	gtk_container_add(GTK_CONTAINER(scrolledwindow66),
			  clist_modules);
	gtk_clist_set_column_width(GTK_CLIST(clist_modules), 0, 300);
	gtk_clist_set_column_width(GTK_CLIST(clist_modules), 1, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(clist_modules));

	label232 = gtk_label_new("");
	gtk_widget_show(label232);
	gtk_clist_set_column_widget(GTK_CLIST(clist_modules), 0,
				    label232);

	label233 = gtk_label_new("");
	gtk_widget_show(label233);
	gtk_clist_set_column_widget(GTK_CLIST(clist_modules), 1,
				    label233);

	label205 = gtk_label_new(_("Custom Lists"));
	gtk_widget_show(label205);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook8),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook8), 3),
				   label205);
/************************************************************************/

/*******   ******/
	search.dialog = search_dialog;

	search.label_search_module = label_search_module;
	search.notebook = notebook8;
	search.togglebutton_show_main = togglebutton_show_main;
	search.rb_current_module = rb_one_mod;
	search.rb_mod_list = rb_mod_list;
	search.rb_custom_list = rb_custom_list;
	search.clist_modules = clist_modules;

	search.combo_list = combo31;
	search.combo_entry_list = combo_entry20;

	search.entry_list_name = entry_modlist_name;
	search.rb_last = rb_last;
	search.rb_no_scope = rb_no_scope;
	search.rb_custom_range = rb_custom_range;
	search.combo_range = combo_custom_range;
	search.combo_entry_range = combo_entry_custom_range;
	search.clist_range = clist_custom_range;
	search.clist_ranges = clist_ranges;
	search.entry_range_name = entry_range_name;
	search.entry_range_text = entry_range_text;
	search.search_entry = entry17;
	search.rb_words = rb_words;
	search.rb_regexp = tb_regexp;
	search.rb_exact_phrase = rb_exact_phrase;
	search.cb_case_sensitive = cb_case_sensitive;
	search.progressbar = appbar_search;
	search.label_mod_select = label_search_module;

	scrolledwindow_ctree = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_ctree);
	gtk_scrolled_window_set_policy((GtkScrolledWindow *)
				       scrolledwindow_ctree,
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	search.ctree = gtk_ctree_new(1, 0);
	gtk_widget_show(search.ctree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_ctree),
			  search.ctree);
	gtk_clist_set_column_width(GTK_CLIST(search.ctree), 0, 280);
	gtk_paned_pack1(GTK_PANED(hpaned8), scrolledwindow_ctree, TRUE,
			TRUE);

	search.which_scope = GTK_TOGGLE_BUTTON(search.rb_no_scope);

	/* add html widgets */

	search.report_html = gtk_html_new();
	gtk_widget_show(search.report_html);
	gtk_html_load_empty(GTK_HTML(search.report_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow_report),
			  search.report_html);

	search.preview_html = gtk_html_new();
	gtk_widget_show(search.preview_html);
	gtk_html_load_empty(GTK_HTML(search.preview_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow_preview),
			  search.preview_html);

	search.results_html = gtk_html_new();
	gtk_widget_show(search.results_html);
	gtk_html_load_empty(GTK_HTML(search.results_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow_results),
			  search.results_html);


	/* connect signals */

	gtk_signal_connect(GTK_OBJECT(search.ctree), "tree_select_row",
			   G_CALLBACK(on_ctree_select_row), NULL);
	gtk_signal_connect(GTK_OBJECT(search.rb_current_module),
			   "toggled",
			   G_CALLBACK(current_module_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(search.rb_no_scope), "toggled",
			   G_CALLBACK(scope_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(search.rb_mod_list), "toggled",
			   G_CALLBACK(mod_list_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(search.rb_custom_range),
			   "toggled", G_CALLBACK(scope_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(button_save_mods), "clicked",
			   G_CALLBACK(save_modlist), NULL);

	gtk_signal_connect(GTK_OBJECT(button_modlist_new), "clicked",
			   G_CALLBACK(new_modlist), NULL);

	gtk_signal_connect(GTK_OBJECT(button28), "clicked",
			   G_CALLBACK(clear_modules), NULL);
	gtk_signal_connect(GTK_OBJECT(button30), "clicked",
			   G_CALLBACK(delete_module), NULL);

	gtk_signal_connect(GTK_OBJECT(button_delete_list), "clicked",
			   G_CALLBACK(delete_list), NULL);
	gtk_signal_connect(GTK_OBJECT(button_new_range), "clicked",
			   G_CALLBACK(new_range), NULL);
	gtk_signal_connect(GTK_OBJECT(button_save_range), "clicked",
			   G_CALLBACK(save_range), NULL);
	gtk_signal_connect(GTK_OBJECT(button_delete_range), "clicked",
			   G_CALLBACK(delete_range), NULL);

	gtk_signal_connect(GTK_OBJECT(entry_range_name), "changed",
			   G_CALLBACK(range_name_changed), NULL);

	gtk_signal_connect(GTK_OBJECT(entry_range_text), "changed",
			   G_CALLBACK(range_text_changed), NULL);


	gtk_signal_connect(GTK_OBJECT(search.entry_list_name),
			   "changed",
			   G_CALLBACK(list_name_changed), NULL);


	gtk_signal_connect(GTK_OBJECT(search.clist_range), "select_row",
			   G_CALLBACK(custom_range_select_row),
			   NULL);

	gtk_signal_connect(GTK_OBJECT(search.module_lists),
			   "select_row",
			   G_CALLBACK(custom_list_select_row),
			   NULL);

	gtk_signal_connect(GTK_OBJECT(search.clist_modules),
			   "select_row",
			   G_CALLBACK(modules_select_row), NULL);

	gtk_signal_connect(GTK_OBJECT(search.togglebutton_show_main),
			   "toggled",
			   G_CALLBACK
			   (on_togglebutton_show_main), NULL);
	gtk_signal_connect(GTK_OBJECT(button_close), "clicked",
			   G_CALLBACK(on_button_close), NULL);

	gtk_signal_connect(GTK_OBJECT(button_begin_search), "clicked",
			   G_CALLBACK
			   (on_button_begin_search), NULL);
	gtk_signal_connect(GTK_OBJECT(search.results_html),
			   "link_clicked",
			   G_CALLBACK(link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(search.results_html), "on_url",
			   G_CALLBACK(url), NULL);
	gtk_signal_connect(GTK_OBJECT(search.report_html),
			   "link_clicked",
			   G_CALLBACK(report_link_clicked), NULL);

	gtk_signal_connect(GTK_OBJECT(button_clear_results), "clicked",
			   G_CALLBACK(button_clean), NULL);


	gtk_object_set_data(GTK_OBJECT(search.dialog), "tooltips",
			    tooltips);

	return search.dialog;
}


/******************************************************************************
 * Name
 *   gui_do_dialog_search
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void gui_do_dialog_search(void)
 *
 * Description
 *   create and setup the search dialog
 *
 * Return value
 *   void
 */

void gui_do_dialog_search(void)
{
	if (!is_running) {
		GtkWidget *dlg;
		gchar *buf[2], *text_cell;

		/* create and show search dialog */
		dlg = create_search_dialog();
		gtk_widget_show(dlg);

		/* initiate module count to 0 */
		search.module_count = 0;

		/* add custom ranges to clist_ranges */
		add_ranges();

		/* add custom modlist to module_list */
		add_modlist();

		/* add bibletext folder to module ctree and fill it */
		fill_group("Bibletext", get_list(TEXT_DESC_LIST));

		/* add commentary folder to module ctree and fill it */
		fill_group("Commentaries", get_list(COMM_DESC_LIST));

		/* add dictionary folder to module ctree and fill it */
		fill_group("Dictionaries", get_list(DICT_DESC_LIST));

		/* add book folder to module ctree and fill it */
		fill_group("General Books", get_list(GBS_DESC_LIST));

		/* set search module to current module 
		   and put in clist_modules */
		search.search_mod = settings.MainWindowModule;
		buf[0] =
		    get_module_description(settings.MainWindowModule);
		buf[1] = settings.MainWindowModule;
		change_mods_select_label(settings.MainWindowModule);
		gtk_clist_append(GTK_CLIST(search.clist_modules), buf);

		/* add one to module count */
		++search.module_count;

		text_cell = " ";
		gtk_clist_append((GtkCList *) search.clist_ranges,
				 &text_cell);
				 
		search.show_in_main = FALSE;
		is_running = TRUE;
	} else
		gdk_window_raise(GTK_WIDGET(search.dialog)->window);

}
