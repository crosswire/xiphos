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
#include "main/lists.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/commentary.h"
#include "main/bibletext.h"

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
	GtkWidget *progressbar;
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

	gchar *search_mod;

	GList *rangelist;
	GList *modlists;


};

static gchar *get_modlist_string(GList * mods);

static SEARCH_DIALOG search;
static gboolean is_running = FALSE;


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
//	g_warning(mod_list);
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
		gnome_appbar_set_progress((GnomeAppBar *) search.
					  progressbar, num);
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
 *   get_custom_range_from_name
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   gchar *get_custom_range_from_name(gchar * name)
 *
 * Description
 *   used by setup_search() - returns the search range (scope)
 *
 * Return value
 *   gchar *
 */

static gchar *get_custom_range_from_name(gchar * name)
{
	GList *tmp = NULL;
	gchar *buf;
	CUSTOM_RANGE *r;

	tmp = search.rangelist;
	while (tmp != NULL) {
		r = (CUSTOM_RANGE *) tmp->data;
		buf = r->label;
		if (!strcmp(buf, name))
			return g_strdup(r->range);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return NULL;

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

static GList *get_custom_list_from_name(gchar * name)
{
	GList *tmp = NULL;
	GList *items = NULL;
	gchar *buf, *mod_list = NULL;
	CUSTOM_MODLIST *r;
	gchar *t;
	gchar *token;

	search.module_count = 0;
	t = ",";
	tmp = search.modlists;
	while (tmp != NULL) {
		r = (CUSTOM_MODLIST *) tmp->data;
		//g_warning("mod list = %s",r->modules);
		buf = r->label;
		mod_list = g_strdup(r->modules);
		if (!strcmp(buf, name)) {
			token = strtok(mod_list, t);
			++search.module_count;
			while (token != NULL) {
				//g_warning("token = %s",token);
				++search.module_count;
				items = g_list_append(items, (gchar *)
						      g_strdup(token));
				token = strtok(NULL, t);
			}
			if (mod_list)
				g_free(mod_list);
			return items;
		}
		if (mod_list)
			g_free(mod_list);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
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
	GList *tmp = NULL;
	GList *items = NULL;
	gchar *buf[2];
	CUSTOM_RANGE *r;

	gtk_clist_clear(GTK_CLIST(search.clist_range));
	tmp = search.rangelist;
	while (tmp != NULL) {
		r = (CUSTOM_RANGE *) tmp->data;
		//g_warning("on load %s = %s", r->label, r->range);
		buf[0] = r->label;
		buf[1] = r->range;

		items = g_list_append(items, (gchar *) r->label);
		search.range_rows =
		    gtk_clist_append(GTK_CLIST(search.clist_range),
				     buf);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	if (items != NULL)
		gtk_combo_set_popdown_strings(GTK_COMBO
					      (search.combo_range),
					      items);
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
	GList *tmp = NULL;
	GList *items = NULL;
	gchar *buf[2];
	CUSTOM_MODLIST *r;

	gtk_clist_clear(GTK_CLIST(search.module_lists));
	tmp = search.modlists;
	while (tmp != NULL) {
		r = (CUSTOM_MODLIST *) tmp->data;
		//g_warning("on load %s = %s", r->label, r->range);
		buf[0] = r->label;
		buf[1] = r->modules;

		items = g_list_append(items, (gchar *) r->label);
		search.list_rows =
		    gtk_clist_append(GTK_CLIST(search.module_lists),
				     buf);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	if (items != NULL)
		gtk_combo_set_popdown_strings(GTK_COMBO
					      (search.combo_list),
					      items);
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

static void fill_group(gint group_num, GList * mod_lists)
{
	GList *tmp = NULL;
	gchar *mod_name, *type;
	GdkPixbuf *icon_pixbuf = NULL;

	tmp = mod_lists;
	while (tmp != NULL) {
		mod_name = (gchar *) tmp->data;

		switch (group_num) {
		case 0:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-un.png");
			type = "text";
			break;
		case 1:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-bl.png");
			type = "comm";
			break;
		case 2:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-green.png");
			type = "dict";
			break;
		case 3:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-gold.png");
			type = "book";
			break;
		default:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-un.png");
			type = "default";
		}

		e_shortcut_model_add_item(E_SHORTCUT_BAR
					  (search.shortcut_bar)->
					  model, group_num,
					  -1, "favorite",
					  (gchar *) tmp->
					  data, icon_pixbuf);
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
	gchar *name;

	gui_begin_html(search.results_html, TRUE);
	clear_scope();
	if (GTK_TOGGLE_BUTTON(search.rb_custom_range)->active) {
		clear_search_list();
		name =
		    gtk_entry_get_text(GTK_ENTRY
				       (search.combo_entry_range));
		set_range(get_custom_range_from_name(name));
		set_scope2range();
	} else if (GTK_TOGGLE_BUTTON(search.rb_last)->active) {
		set_scope2last_search();
	} else {
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
	CUSTOM_RANGE *r;
	CUSTOM_MODLIST *m;
	GList *tmp = NULL;

	is_running = FALSE;

	tmp = g_list_first(search.rangelist);
	while (tmp != NULL) {
		r = (CUSTOM_RANGE *) tmp->data;
		g_free(r->label);
		g_free(r->range);
		g_free((CUSTOM_RANGE *) tmp->data);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	g_list_free(search.rangelist);

	tmp = g_list_first(search.modlists);
	while (tmp != NULL) {
		m = (CUSTOM_MODLIST *) tmp->data;
		g_free(m->label);
		g_free(m->modules);
		g_free((CUSTOM_MODLIST *) tmp->data);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	g_list_free(search.modlists);


	gtk_widget_destroy(search.dialog);
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
	gchar *search_string, *module;
	gchar buf[256], *utf8str;
	gint utf8len;
	GList *search_mods = NULL;

	GtkHTMLStreamStatus status2 = 0;
	GtkHTML *html;
	GtkHTMLStream *htmlstream2;

	html = GTK_HTML(search.report_html);
	htmlstream2 =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");
	sprintf(buf,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);
	utf8str = e_utf8_from_gtk_string(search.report_html, buf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream2, utf8str,
			       utf8len);
	}

	set_up_search();

	gtk_notebook_set_page(GTK_NOTEBOOK(search.notebook), 3);

	search_type = GTK_TOGGLE_BUTTON
	    (search.rb_regexp)->active ? 0 :
	    GTK_TOGGLE_BUTTON(search.rb_exact_phrase)->active ? -1 : -2;

	settings.searchType = search_type;

	search_params = GTK_TOGGLE_BUTTON
	    (search.cb_case_sensitive)->active ? 0 : REG_ICASE;

	search_string =
	    gtk_entry_get_text(GTK_ENTRY(search.search_entry));

	if (GTK_TOGGLE_BUTTON(search.rb_custom_list)->active) {
		gchar *name;
		name =
		    gtk_entry_get_text(GTK_ENTRY
				       (search.combo_entry_list));
		search_mods = get_custom_list_from_name(name);
	} else if (GTK_TOGGLE_BUTTON(search.rb_mod_list)->active) {
		search_mods = get_current_list();
	} else
		search_mods = get_current_search_mod();

	search_mods = g_list_first(search_mods);
	while (search_mods != NULL) {
		module = (gchar *) search_mods->data;

		sprintf(buf, "%s %s %s", SEARCHING, module, SMODULE);
		gnome_appbar_set_status(GNOME_APPBAR
					(search.progressbar), buf);
		set_search_module(module);

		finds = do_dialog_search(search_string,
					 search_type, search_params);

		sprintf(buf, "%d %s <A HREF=\"%s\">%s</A><br>",
			finds, FINDS, module, module);
		utf8str =
		    e_utf8_from_gtk_string(search.report_html, buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream2,
				       utf8str, utf8len);
		}

		g_free(module);
		search_mods = g_list_next(search_mods);
	}
	g_list_free(search_mods);

	gtk_html_end(GTK_HTML(html), htmlstream2, status2);
	gui_end_html(search.results_html);
	gnome_appbar_set_progress((GnomeAppBar *) search.progressbar,
				  (gfloat) 0);
	gnome_appbar_set_status(GNOME_APPBAR(search.progressbar),
				_("Search Finished"));
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

}


/******************************************************************************
 * Name
 *   add_sb_group
 *
 * Synopsis
 *   #include "Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   gint add_group_to_sb_search(gchar * group_name)	
 *
 * Description
 *   add group to perferences dialog shourtcut bar
 *
 * Return value
 *   gint
 */

static gint add_group_to_sb_search(EShortcutBar * shortcut_bar,
				   gchar * group_name)
{
	gint group_num;

	group_num =
	    e_shortcut_model_add_group(shortcut_bar->model, -1,
				       group_name);
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_SMALL_ICONS);
	return group_num;
}


/******************************************************************************
 * Name
 *   shortcut_bar_item_selected
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
 *			GdkEvent * event, gint group_num, gint item_num)
 *
 * Description
 *   changes search module or adds a new one to the list
 *   depending on setting rb_current_module (radio button)
 *
 * Return value
 *   void
 */

static void shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
				       GdkEvent * event, gint group_num,
				       gint item_num)
{
	gchar *type, *ref;
	gchar *text[2], *mod_list;
	GdkPixbuf *icon_pixbuf = NULL;
	gchar mod_name[16];
	GList *mods = NULL;


	if (event->button.button == 1) {
		if (item_num > -1) {
			e_shortcut_model_get_item_info(E_SHORTCUT_BAR
						       (shortcut_bar)->
						       model,
						       group_num,
						       item_num,
						       &type, &ref,
						       &icon_pixbuf);
			text[0] = ref;
			memset(mod_name, 0, 16);
			module_name_from_description(mod_name, ref);
			text[1] = mod_name;
			if (GTK_TOGGLE_BUTTON
			    (search.rb_current_module)->active) {
				search.search_mod = g_strdup(mod_name);
				search.module_count = 1;
			} else {
				gtk_clist_append(GTK_CLIST
						 (search.clist_modules),
						 text);

				mods = get_current_list();
				mod_list = get_modlist_string(mods);
				//g_warning("mod string = %s", mod_list);
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
			g_free(type);
			g_free(ref);
			change_mods_select_label(mod_name);
		}
	}
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
				text = get_striptext(modbuf, buf);
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
		text = get_module_text(modbuf, buf);
		entry_display(search.preview_html, modbuf,
			      text, buf, TRUE);
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
	gchar *text;

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
	gchar *text;

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
	gchar *entry;
	gchar *text;
	GList *range = NULL;
	GList *tmp = NULL;

	entry = gtk_entry_get_text(GTK_ENTRY(editable));
	range = get_element(entry);
	gtk_clist_clear((GtkCList *) search.clist_ranges);
	tmp = g_list_first(range);
	while (tmp != NULL) {
		text = (gchar *) tmp->data;
		gtk_clist_append((GtkCList *) search.clist_ranges,
				 &text);
		g_free((gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	g_list_free(range);
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
	info->label_top = N_("Do you really want to CLEAR the module list?");
	info->yes = TRUE;
	info->no = TRUE;

	test = gui_gs_dialog(info);
	if (test != GS_YES)
		return;
	
	gtk_clist_clear((GtkCList *) search.clist_modules);
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

	info = gui_new_dialog();
	info->label_top = N_("Do you really want to REMOVE the selected item?");
	info->yes = TRUE;
	info->no = TRUE;

	test = gui_gs_dialog(info);
	if (test != GS_YES)
		return;
	gtk_clist_remove((GtkCList *) search.clist_modules,
				 search.modules_row);
	
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
	GList *tmp = NULL;
	gint i = 0;
	gchar *text1;
	gchar *text2;
	CUSTOM_MODLIST *r;

	tmp = g_list_first(search.modlists);
	while (tmp != NULL) {
		r = (CUSTOM_MODLIST *) tmp->data;
		//g_warning("on save1 %s = %s", r->label, r->modules);
		g_free(r->label);
		g_free(r->modules);
		g_free((CUSTOM_MODLIST *) tmp->data);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	g_list_free(search.modlists);
	search.modlists = NULL;

	for (i = 0; i < (search.list_rows + 1); i++) {

		gtk_clist_get_text((GtkCList *) search.module_lists,
				   i, 0, &text1);
		gtk_clist_get_text((GtkCList *) search.module_lists,
				   i, 1, &text2);

		r = g_new(CUSTOM_MODLIST, 1);
		r->label = g_strdup(text1);
		r->modules = g_strdup(text2);
		search.modlists =
		    g_list_append(search.modlists,
				  (CUSTOM_MODLIST *) r);
	}
	save_custom_modlist(search.modlists);
	add_modlist();
}


/******************************************************************************
 * Name
 *   save_modlist_as
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   
 *
 * Description
 *   saves the current module list as...
 *
 * Return value
 *   void
 */

static void save_modlist_as(GtkButton * button, gpointer user_data)
{
	gint test;
	CUSTOM_MODLIST *m;
	
	GS_DIALOG *info;

	
	info = gui_new_dialog();
	info->label_top = N_("Save List");
	info->text1 = g_strdup("");
	info->label1 = N_("Enter List Name: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	/*** open dialog to get name for list ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {	
		if (strlen(info->text1) > 0) {
			GList *mods = NULL;
			gchar *text[2], *mods_string;
			mods = get_current_list();
			mods_string = get_modlist_string(mods);
			text[0] = info->text1;
			text[1] = mods_string;
			
			search.custom_list_row =
			    gtk_clist_append((GtkCList *) search.
					     module_lists, text);
			search.list_rows = search.custom_list_row;
			m = g_new(CUSTOM_MODLIST, 1);
			m->label = g_strdup(info->text1);
			m->modules = g_strdup(mods_string);
			search.modlists =
			    g_list_append(search.modlists,
					  (CUSTOM_MODLIST *) m);
			save_custom_modlist(search.modlists);
			gtk_clist_select_row((GtkCList *)
					     search.module_lists,
					     search.custom_list_row, 0);
		}
	}
	g_free(info->text1);
	g_free(info);
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
	GList *tmp = NULL;
	gint i = 0;
	gchar *text1;
	gchar *text2;
	CUSTOM_RANGE *r;

	tmp = g_list_first(search.rangelist);
	while (tmp != NULL) {
		r = (CUSTOM_RANGE *) tmp->data;
		//g_warning("on save1 %s = %s", r->label, r->range);
		g_free(r->label);
		g_free(r->range);
		g_free((CUSTOM_RANGE *) tmp->data);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	g_list_free(search.rangelist);
	search.rangelist = NULL;

	for (i = 0; i < (search.range_rows + 1); i++) {

		gtk_clist_get_text((GtkCList *) search.clist_range,
				   i, 0, &text1);
		gtk_clist_get_text((GtkCList *) search.clist_range,
				   i, 1, &text2);

		r = g_new(CUSTOM_RANGE, 1);
		r->label = g_strdup(text1);
		r->range = g_strdup(text2);
		search.rangelist =
		    g_list_append(search.rangelist, (CUSTOM_RANGE *) r);
	}
	save_custom_ranges(search.rangelist);
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
	info->label_top = N_("Are you sure you want ot delete this range?");
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
	info->label_top = N_("Are you sure you want ot delete this list?");
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
	GtkWidget *cb_include_strongs;
	GtkWidget *cb_include_morphs;
	GtkWidget *cb_include_footnotes;
	GtkWidget *label204;
	GtkWidget *frame67a;
	GtkWidget *vbox73;
	GtkWidget *toolbar36;
	GtkWidget *button_modlist_new;
	GtkWidget *button_clear_list;
	GtkWidget *button_save_mods;
	GtkWidget *button_save_mods_as;
	GtkWidget *vseparator27;
	GtkWidget *button_delete_list;
	GtkWidget *hbox72;
	GtkWidget *hbox75;
	GtkWidget *hbox82;
	GtkWidget *scrolledwindow68;
	GtkWidget *clist_mod_lists;
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
	
	sprintf(title,"%s - %s", settings.program_title, _("Search"));

	tooltips = gtk_tooltips_new();

	search_dialog = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_object_set_data(GTK_OBJECT(search_dialog), "search_dialog",
			    search_dialog);
	gtk_window_set_title(GTK_WINDOW(search_dialog),
			     title);
	gtk_window_set_default_size(GTK_WINDOW(search_dialog), 598,
				    337);

	hbox_search = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox_search);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "hbox_search", hbox_search,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox_search);
	gtk_container_add(GTK_CONTAINER(search_dialog), hbox_search);

	hpaned8 = gtk_hpaned_new();
	gtk_widget_ref(hpaned8);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hpaned8",
				 hpaned8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned8);
	gtk_box_pack_start(GTK_BOX(hbox_search), hpaned8, TRUE, TRUE,
			   0);
	gtk_paned_set_position(GTK_PANED(hpaned8), 150);

	vbox57 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox57);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox57",
				 vbox57,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox57);
	gtk_paned_pack2(GTK_PANED(hpaned8), vbox57, TRUE, TRUE);

	frame_search = gtk_frame_new(_("Search"));
	gtk_widget_ref(frame_search);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "frame_search", frame_search,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_search);
	gtk_box_pack_start(GTK_BOX(vbox57), frame_search, FALSE, TRUE,
			   0);

	vbox82 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox82);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox82",
				 vbox82,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox82);
	gtk_container_add(GTK_CONTAINER(frame_search), vbox82);

	label_search_module = gtk_label_new(_("<Module>"));
	gtk_widget_ref(label_search_module);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "label_search_module",
				 label_search_module,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label_search_module);
	gtk_box_pack_start(GTK_BOX(vbox82), label_search_module, TRUE,
			   TRUE, 0);
	gtk_widget_set_usize(label_search_module, 80, -2);
	gtk_label_set_justify(GTK_LABEL(label_search_module),
			      GTK_JUSTIFY_LEFT);

	hbox71 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox71);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox71",
				 hbox71,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox71);
	gtk_box_pack_start(GTK_BOX(vbox82), hbox71, TRUE, TRUE, 0);

	toolbar37 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar37);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "toolbar37",
				 toolbar37,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar37);
	gtk_box_pack_start(GTK_BOX(hbox71), toolbar37, TRUE, TRUE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar37),
				      GTK_RELIEF_NONE);

	label230 = gtk_label_new(_(" for "));
	gtk_widget_ref(label230);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label230",
				 label230,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label230);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar37), label230,
				  NULL, NULL);

	vbox_entry = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_entry);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "vbox_entry", vbox_entry,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_entry);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar37), vbox_entry,
				  NULL, NULL);

	entry17 = gtk_entry_new();
	gtk_widget_ref(entry17);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "entry17",
				 entry17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry17);
	gtk_box_pack_start(GTK_BOX(vbox_entry), entry17, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(entry17, 350, -2);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_SEARCH);
	button_begin_search =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar37),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button23"), _("Begin search"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_begin_search);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_begin_search",
				 button_begin_search,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_begin_search);

	vseparator25 = gtk_vseparator_new();
	gtk_widget_ref(vseparator25);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "vseparator25", vseparator25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator25);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar37), vseparator25,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator25, 6, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_EXIT);
	button_close =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar37),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Close"),
				       _("Close search dialog"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_close);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_close", button_close,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_close);

	frame66 = gtk_frame_new(NULL);
	gtk_widget_ref(frame66);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "frame66",
				 frame66,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame66);
	gtk_box_pack_start(GTK_BOX(vbox57), frame66, TRUE, TRUE, 0);

	notebook8 = gtk_notebook_new();
	gtk_widget_ref(notebook8);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "notebook8",
				 notebook8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook8);
	gtk_container_add(GTK_CONTAINER(frame66), notebook8);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook8), TRUE);
	gtk_notebook_set_tab_hborder(GTK_NOTEBOOK(notebook8), 8);
	gtk_notebook_set_tab_vborder(GTK_NOTEBOOK(notebook8), 1);

	frame47 = gtk_frame_new(NULL);
	gtk_widget_ref(frame47);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "frame47",
				 frame47,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame47);
	gtk_container_add(GTK_CONTAINER(notebook8), frame47);

	vbox58 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox58);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox58",
				 vbox58,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox58);
	gtk_container_add(GTK_CONTAINER(frame47), vbox58);

	frame_scope = gtk_frame_new(_("Scope"));
	gtk_widget_ref(frame_scope);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "frame_scope", frame_scope,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_scope);
	gtk_box_pack_start(GTK_BOX(vbox58), frame_scope, FALSE, TRUE,
			   0);

	vbox77 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox77);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox77",
				 vbox77,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox77);
	gtk_container_add(GTK_CONTAINER(frame_scope), vbox77);

	hbox77 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox77);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox77",
				 hbox77,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox77);
	gtk_box_pack_start(GTK_BOX(vbox77), hbox77, TRUE, TRUE, 0);

	rb_no_scope =
	    gtk_radio_button_new_with_label(hbox77_group,
					    _("Entire Module   "));
	hbox77_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_no_scope));
	gtk_widget_ref(rb_no_scope);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "rb_no_scope", rb_no_scope,
				 (GtkDestroyNotify) gtk_widget_unref);
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
	gtk_widget_ref(rb_last);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "rb_last",
				 rb_last,
				 (GtkDestroyNotify) gtk_widget_unref);
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
	gtk_widget_ref(rb_custom_range);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "rb_custom_range", rb_custom_range,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rb_custom_range);
	gtk_box_pack_start(GTK_BOX(hbox77), rb_custom_range, TRUE, TRUE,
			   0);
	gtk_tooltips_set_tip(tooltips, rb_custom_range,
			     _("Use a custom search range"), NULL);

	hbox78 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox78);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox78",
				 hbox78,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox78);
	gtk_box_pack_start(GTK_BOX(vbox77), hbox78, TRUE, TRUE, 0);

	combo_custom_range = gtk_combo_new();
	gtk_widget_ref(combo_custom_range);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "combo_custom_range",
				 combo_custom_range,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_custom_range);
	gtk_box_pack_start(GTK_BOX(hbox78), combo_custom_range, TRUE,
			   TRUE, 0);
	combo_custom_range_items =
	    g_list_append(combo_custom_range_items, (gpointer) "");
	gtk_combo_set_popdown_strings(GTK_COMBO(combo_custom_range),
				      combo_custom_range_items);
	g_list_free(combo_custom_range_items);

	combo_entry_custom_range = GTK_COMBO(combo_custom_range)->entry;
	gtk_widget_ref(combo_entry_custom_range);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "combo_entry_custom_range",
				 combo_entry_custom_range,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_entry_custom_range);

	frame_modlists = gtk_frame_new(_("Modules"));
	gtk_widget_ref(frame_modlists);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "frame_modlists", frame_modlists,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_modlists);
	gtk_box_pack_start(GTK_BOX(vbox58), frame_modlists, FALSE, TRUE,
			   0);

	vbox79 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox79);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox79",
				 vbox79,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox79);
	gtk_container_add(GTK_CONTAINER(frame_modlists), vbox79);

	hbox79 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox79);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox79",
				 hbox79,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox79);
	gtk_box_pack_start(GTK_BOX(vbox79), hbox79, TRUE, TRUE, 0);

	rb_one_mod =
	    gtk_radio_button_new_with_label(hbox79_group,
					    _("Single Module"));
	hbox79_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_one_mod));
	gtk_widget_ref(rb_one_mod);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "rb_one_mod", rb_one_mod,
				 (GtkDestroyNotify) gtk_widget_unref);
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
	gtk_widget_ref(rb_mod_list);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "rb_mod_list", rb_mod_list,
				 (GtkDestroyNotify) gtk_widget_unref);
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
	gtk_widget_ref(rb_custom_list);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "rb_custom_list", rb_custom_list,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rb_custom_list);
	gtk_box_pack_start(GTK_BOX(hbox79), rb_custom_list, FALSE, TRUE,
			   0);
	gtk_tooltips_set_tip(tooltips, rb_custom_list,
			     _
			     ("Use a custom module list for the search"),
			     NULL);

	hbox80 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox80);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox80",
				 hbox80,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox80);
	gtk_box_pack_start(GTK_BOX(vbox79), hbox80, TRUE, TRUE, 0);

	combo31 = gtk_combo_new();
	gtk_widget_ref(combo31);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "combo31",
				 combo31,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo31);
	gtk_box_pack_start(GTK_BOX(hbox80), combo31, TRUE, TRUE, 0);

	combo_entry20 = GTK_COMBO(combo31)->entry;
	gtk_widget_ref(combo_entry20);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "combo_entry20", combo_entry20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_entry20);

	hbox74 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox74);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox74",
				 hbox74,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox74);
	gtk_box_pack_start(GTK_BOX(vbox58), hbox74, TRUE, TRUE, 0);

	vbox72 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox72);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox72",
				 vbox72,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox72);
	gtk_box_pack_start(GTK_BOX(hbox74), vbox72, TRUE, TRUE, 0);

	frame49 = gtk_frame_new(_("Search Type"));
	gtk_widget_ref(frame49);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "frame49",
				 frame49,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame49);
	gtk_box_pack_start(GTK_BOX(vbox72), frame49, TRUE, TRUE, 0);

	vbox63 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox63);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox63",
				 vbox63,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox63);
	gtk_container_add(GTK_CONTAINER(frame49), vbox63);

	rb_words =
	    gtk_radio_button_new_with_label(_301_group,
					    _("Multi Word"));
	_301_group = gtk_radio_button_group(GTK_RADIO_BUTTON(rb_words));
	gtk_widget_ref(rb_words);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "rb_words",
				 rb_words,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rb_words);
	gtk_box_pack_start(GTK_BOX(vbox63), rb_words, FALSE, FALSE, 0);

	tb_regexp =
	    gtk_radio_button_new_with_label(_301_group,
					    _("Regular Expression"));
	_301_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(tb_regexp));
	gtk_widget_ref(tb_regexp);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "tb_regexp",
				 tb_regexp,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tb_regexp);
	gtk_box_pack_start(GTK_BOX(vbox63), tb_regexp, FALSE, FALSE, 0);

	rb_exact_phrase =
	    gtk_radio_button_new_with_label(_301_group,
					    _("Exact Phrase"));
	_301_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rb_exact_phrase));
	gtk_widget_ref(rb_exact_phrase);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "rb_exact_phrase", rb_exact_phrase,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rb_exact_phrase);
	gtk_box_pack_start(GTK_BOX(vbox63), rb_exact_phrase, FALSE,
			   FALSE, 0);

	frame57 = gtk_frame_new(_("Options"));
	gtk_widget_ref(frame57);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "frame57",
				 frame57,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame57);
	gtk_box_pack_start(GTK_BOX(hbox74), frame57, TRUE, TRUE, 0);

	vbox64 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox64);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox64",
				 vbox64,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox64);
	gtk_container_add(GTK_CONTAINER(frame57), vbox64);

	cb_case_sensitive =
	    gtk_check_button_new_with_label(_("Case Sensitive"));
	gtk_widget_ref(cb_case_sensitive);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "cb_case_sensitive", cb_case_sensitive,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cb_case_sensitive);
	gtk_box_pack_start(GTK_BOX(vbox64), cb_case_sensitive, FALSE,
			   FALSE, 0);

	cb_include_strongs =
	    gtk_check_button_new_with_label(_
					    ("Include Strongs Numbers"));
	gtk_widget_ref(cb_include_strongs);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "cb_include_strongs",
				 cb_include_strongs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cb_include_strongs);
	gtk_box_pack_start(GTK_BOX(vbox64), cb_include_strongs, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, cb_include_strongs,
			     _("In modules that have Strongs Numbers"),
			     NULL);

	cb_include_morphs =
	    gtk_check_button_new_with_label(_("Include Morph Tags"));
	gtk_widget_ref(cb_include_morphs);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "cb_include_morphs", cb_include_morphs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cb_include_morphs);
	gtk_box_pack_start(GTK_BOX(vbox64), cb_include_morphs, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, cb_include_morphs,
			     _("In modules that have Morph Tags"),
			     NULL);

	cb_include_footnotes =
	    gtk_check_button_new_with_label(_("Include Footnotes"));
	gtk_widget_ref(cb_include_footnotes);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "cb_include_footnotes",
				 cb_include_footnotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cb_include_footnotes);
	gtk_box_pack_start(GTK_BOX(vbox64), cb_include_footnotes, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, cb_include_footnotes,
			     _("In modules that have Footnotes"), NULL);

	label204 = gtk_label_new(_("Search Criteria"));
	gtk_widget_ref(label204);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label204",
				 label204,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label204);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook8),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook8), 0),
				   label204);

	frame67a = gtk_frame_new(NULL);
	gtk_widget_ref(frame67a);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "frame67a",
				 frame67a,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame67a);
	gtk_container_add(GTK_CONTAINER(notebook8), frame67a);

	vbox73 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox73);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox73",
				 vbox73,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox73);
	gtk_container_add(GTK_CONTAINER(frame67a), vbox73);

	toolbar36 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar36);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "toolbar36",
				 toolbar36,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar36);
	gtk_box_pack_start(GTK_BOX(vbox73), toolbar36, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar36),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_NEW);
	button_modlist_new =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar36),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("New"),
				       _("Create new module list"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_modlist_new);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_modlist_new",
				 button_modlist_new,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_modlist_new);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_CLEAR);
	button_clear_list =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar36),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Clear Module List"),
				       _("Clear Module List"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_clear_list);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_clear_list", button_clear_list,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_clear_list);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_SAVE);
	button_save_mods =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar36),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save List"),
				       _("Save list of modules"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_save_mods);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_save_mods", button_save_mods,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_save_mods);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_SAVE_AS);
	button_save_mods_as =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar36),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save As"),
				       _
				       ("Save current list of modules as..."),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_save_mods_as);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_save_mods_as",
				 button_save_mods_as,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_save_mods_as);

	vseparator27 = gtk_vseparator_new();
	gtk_widget_ref(vseparator27);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "vseparator27", vseparator27,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator27);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar36), vseparator27,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator27, 6, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_CUT);
	button_delete_list =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar36),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Delete"),
				       _("Delete selected list"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_delete_list);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_delete_list",
				 button_delete_list,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_delete_list);

	hbox72 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox72);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox72",
				 hbox72,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox72);
	gtk_box_pack_start(GTK_BOX(vbox73), hbox72, TRUE, TRUE, 0);

	hbox75 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox75);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox75",
				 hbox75,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox75);
	gtk_box_pack_start(GTK_BOX(hbox72), hbox75, TRUE, TRUE, 0);

	hbox82 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox82);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox82",
				 hbox82,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox82);
	gtk_box_pack_start(GTK_BOX(hbox75), hbox82, TRUE, TRUE, 0);

	scrolledwindow68 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow68);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "scrolledwindow68", scrolledwindow68,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow68);
	gtk_box_pack_start(GTK_BOX(hbox82), scrolledwindow68, TRUE,
			   TRUE, 0);
	gtk_widget_set_usize(scrolledwindow68, 175, -2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow68),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	clist_mod_lists = gtk_clist_new(2);
	gtk_widget_ref(clist_mod_lists);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "clist_mod_lists", clist_mod_lists,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(clist_mod_lists);
	gtk_container_add(GTK_CONTAINER(scrolledwindow68),
			  clist_mod_lists);
	gtk_clist_set_column_width(GTK_CLIST(clist_mod_lists), 0, 169);
	gtk_clist_set_column_width(GTK_CLIST(clist_mod_lists), 1, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(clist_mod_lists));

	label236 = gtk_label_new(_("label236"));
	gtk_widget_ref(label236);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label236",
				 label236,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label236);
	gtk_clist_set_column_widget(GTK_CLIST(clist_mod_lists), 0,
				    label236);

	label237 = gtk_label_new(_("label237"));
	gtk_widget_ref(label237);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label237",
				 label237,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label237);
	gtk_clist_set_column_widget(GTK_CLIST(clist_mod_lists), 1,
				    label237);

	frame67 = gtk_frame_new(NULL);
	gtk_widget_ref(frame67);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "frame67",
				 frame67,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame67);
	gtk_box_pack_start(GTK_BOX(hbox82), frame67, FALSE, TRUE, 0);

	vbox80 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox80);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox80",
				 vbox80,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox80);
	gtk_container_add(GTK_CONTAINER(frame67), vbox80);

	frame69 = gtk_frame_new(_("List Name"));
	gtk_widget_ref(frame69);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "frame69",
				 frame69,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame69);
	gtk_box_pack_start(GTK_BOX(vbox80), frame69, FALSE, TRUE, 0);

	entry_modlist_name = gtk_entry_new();
	gtk_widget_ref(entry_modlist_name);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "entry_modlist_name",
				 entry_modlist_name,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry_modlist_name);
	gtk_container_add(GTK_CONTAINER(frame69), entry_modlist_name);

	frame68 = gtk_frame_new(_("Modules"));
	gtk_widget_ref(frame68);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "frame68",
				 frame68,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame68);
	gtk_box_pack_start(GTK_BOX(vbox80), frame68, TRUE, TRUE, 0);

	vbox81 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox81);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox81",
				 vbox81,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox81);
	gtk_container_add(GTK_CONTAINER(frame68), vbox81);

	toolbar39 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar39);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "toolbar39",
				 toolbar39,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar39);
	gtk_box_pack_start(GTK_BOX(vbox81), toolbar39, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar39),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_CLEAR);
	button28 =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar39),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Clear Module List"),
				       _("Clear all modules from list"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button28);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "button28",
				 button28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button28);

	vseparator33 = gtk_vseparator_new();
	gtk_widget_ref(vseparator33);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "vseparator33", vseparator33,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator33);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar39), vseparator33,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator33, 6, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_CUT);
	button30 =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar39),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Delete"),
				       _
				       ("Remove selected module from list"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button30);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "button30",
				 button30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button30);

	scrolledwindow66 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow66);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "scrolledwindow66", scrolledwindow66,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow66);
	gtk_box_pack_start(GTK_BOX(vbox81), scrolledwindow66, TRUE,
			   TRUE, 0);
	gtk_widget_set_usize(scrolledwindow66, 240, -2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow66),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	clist_modules = gtk_clist_new(2);
	gtk_widget_ref(clist_modules);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "clist_modules", clist_modules,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(clist_modules);
	gtk_container_add(GTK_CONTAINER(scrolledwindow66),
			  clist_modules);
	gtk_clist_set_column_width(GTK_CLIST(clist_modules), 0, 300);
	gtk_clist_set_column_width(GTK_CLIST(clist_modules), 1, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(clist_modules));

	label232 = gtk_label_new(_("label232"));
	gtk_widget_ref(label232);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label232",
				 label232,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label232);
	gtk_clist_set_column_widget(GTK_CLIST(clist_modules), 0,
				    label232);

	label233 = gtk_label_new(_("label233"));
	gtk_widget_ref(label233);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label233",
				 label233,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label233);
	gtk_clist_set_column_widget(GTK_CLIST(clist_modules), 1,
				    label233);

	label205 = gtk_label_new(_("Custom Lists"));
	gtk_widget_ref(label205);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label205",
				 label205,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label205);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook8),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook8), 1),
				   label205);

	frame_custom_ranges = gtk_frame_new(NULL);
	gtk_widget_ref(frame_custom_ranges);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "frame_custom_ranges",
				 frame_custom_ranges,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_custom_ranges);
	gtk_container_add(GTK_CONTAINER(notebook8),
			  frame_custom_ranges);

	vbox78 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox78);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox78",
				 vbox78,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox78);
	gtk_container_add(GTK_CONTAINER(frame_custom_ranges), vbox78);

	toolbar38 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar38);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "toolbar38",
				 toolbar38,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar38);
	gtk_box_pack_start(GTK_BOX(vbox78), toolbar38, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar38),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_NEW);
	button_new_range =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar38),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("New"),
				       _("Create new custom range"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_new_range);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_new_range", button_new_range,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_new_range);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_SAVE);
	button_save_range =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar38),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save List"),
				       _("Save custom range"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_save_range);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_save_range", button_save_range,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_save_range);

	vseparator26 = gtk_vseparator_new();
	gtk_widget_ref(vseparator26);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "vseparator26", vseparator26,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator26);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar38), vseparator26,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator26, 6, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_CUT);
	button_delete_range =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar38),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Delete"),
				       _("Delete selected range"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_delete_range);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_delete_range",
				 button_delete_range,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_delete_range);

	hbox76 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox76);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox76",
				 hbox76,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox76);
	gtk_box_pack_start(GTK_BOX(vbox78), hbox76, TRUE, TRUE, 0);

	scrolledwindow67 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow67);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "scrolledwindow67", scrolledwindow67,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow67);
	gtk_box_pack_start(GTK_BOX(hbox76), scrolledwindow67, TRUE,
			   TRUE, 0);
	gtk_widget_set_usize(scrolledwindow67, 175, -2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow67),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	clist_custom_range = gtk_clist_new(2);
	gtk_widget_ref(clist_custom_range);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "clist_custom_range",
				 clist_custom_range,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(clist_custom_range);
	gtk_container_add(GTK_CONTAINER(scrolledwindow67),
			  clist_custom_range);
	gtk_clist_set_column_width(GTK_CLIST(clist_custom_range), 0,
				   194);
	gtk_clist_set_column_width(GTK_CLIST(clist_custom_range), 1,
				   80);
	gtk_clist_column_titles_hide(GTK_CLIST(clist_custom_range));

	label234 = gtk_label_new(_("label234"));
	gtk_widget_ref(label234);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label234",
				 label234,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label234);
	gtk_clist_set_column_widget(GTK_CLIST(clist_custom_range), 0,
				    label234);

	label235 = gtk_label_new(_("label235"));
	gtk_widget_ref(label235);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label235",
				 label235,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label235);
	gtk_clist_set_column_widget(GTK_CLIST(clist_custom_range), 1,
				    label235);

	frame66a = gtk_frame_new(NULL);
	gtk_widget_ref(frame66a);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "frame66a",
				 frame66a,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame66a);
	gtk_box_pack_start(GTK_BOX(hbox76), frame66a, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame66a, 176, -2);

	vbox76 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox76);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox76",
				 vbox76,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox76);
	gtk_container_add(GTK_CONTAINER(frame66a), vbox76);

	label227 = gtk_label_new(_("Range Name"));
	gtk_widget_ref(label227);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label227",
				 label227,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label227);
	gtk_box_pack_start(GTK_BOX(vbox76), label227, FALSE, FALSE, 0);
	gtk_widget_set_usize(label227, -2, 26);

	entry_range_name = gtk_entry_new();
	gtk_widget_ref(entry_range_name);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "entry_range_name", entry_range_name,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry_range_name);
	gtk_box_pack_start(GTK_BOX(vbox76), entry_range_name, FALSE,
			   FALSE, 0);

	label228 = gtk_label_new(_("Range Text"));
	gtk_widget_ref(label228);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label228",
				 label228,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label228);
	gtk_box_pack_start(GTK_BOX(vbox76), label228, FALSE, FALSE, 0);
	gtk_widget_set_usize(label228, -2, 26);

	entry_range_text = gtk_entry_new();
	gtk_widget_ref(entry_range_text);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "entry_range_text", entry_range_text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry_range_text);
	gtk_box_pack_start(GTK_BOX(vbox76), entry_range_text, FALSE,
			   FALSE, 0);

	label229 = gtk_label_new(_("eg. Matt-John;Rev4"));
	gtk_widget_ref(label229);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label229",
				 label229,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label229);
	gtk_box_pack_start(GTK_BOX(vbox76), label229, FALSE, FALSE, 0);
	gtk_widget_set_usize(label229, -2, 26);

	scrolledwindow65 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow65);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "scrolledwindow65", scrolledwindow65,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow65);
	gtk_box_pack_start(GTK_BOX(vbox76), scrolledwindow65, TRUE,
			   TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow65),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	clist_ranges = gtk_clist_new(1);
	gtk_widget_ref(clist_ranges);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "clist_ranges", clist_ranges,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(clist_ranges);
	gtk_container_add(GTK_CONTAINER(scrolledwindow65),
			  clist_ranges);
	gtk_clist_set_column_width(GTK_CLIST(clist_ranges), 0, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(clist_ranges));

	label226 = gtk_label_new(_("label226"));
	gtk_widget_ref(label226);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label226",
				 label226,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label226);
	gtk_clist_set_column_widget(GTK_CLIST(clist_ranges), 0,
				    label226);

	label206 = gtk_label_new(_("Custom Ranges"));
	gtk_widget_ref(label206);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label206",
				 label206,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label206);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook8),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook8), 2),
				   label206);

	vbox70 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox70);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox70",
				 vbox70,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox70);
	gtk_container_add(GTK_CONTAINER(notebook8), vbox70);

	toolbar35 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar35);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "toolbar35",
				 toolbar35,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar35);
	gtk_box_pack_start(GTK_BOX(vbox70), toolbar35, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar35),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	togglebutton_show_main =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar35),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Show Context"),
				       _("Show In Main window"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(togglebutton_show_main);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "togglebutton_show_main",
				 togglebutton_show_main,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(togglebutton_show_main);

	vseparator22 = gtk_vseparator_new();
	gtk_widget_ref(vseparator22);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "vseparator22", vseparator22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator22);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar35), vseparator22,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator22, 6, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_SAVE_AS);
	button_save_results =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar35),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save Results"),
				       _
				       ("Save Search Results as Bookmarks"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_save_results);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_save_results",
				 button_save_results,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_save_results);
	gtk_widget_set_sensitive(button_save_results, FALSE);

	vseparator23 = gtk_vseparator_new();
	gtk_widget_ref(vseparator23);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "vseparator23", vseparator23,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator23);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar35), vseparator23,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator23, 6, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(search_dialog,
				      GNOME_STOCK_PIXMAP_CLEAR);
	button_clear_results =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar35),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Clear"),
				       _("clear lists for new search"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_clear_results);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "button_clear_results",
				 button_clear_results,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_clear_results);

	hbox68 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox68);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hbox68",
				 hbox68,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox68);
	gtk_box_pack_start(GTK_BOX(vbox70), hbox68, TRUE, TRUE, 0);

	hpaned7 = gtk_hpaned_new();
	gtk_widget_ref(hpaned7);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "hpaned7",
				 hpaned7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned7);
	gtk_box_pack_start(GTK_BOX(hbox68), hpaned7, TRUE, TRUE, 0);
	gtk_paned_set_position(GTK_PANED(hpaned7), 187);

	vpaned3 = gtk_vpaned_new();
	gtk_widget_ref(vpaned3);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vpaned3",
				 vpaned3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vpaned3);
	gtk_paned_pack1(GTK_PANED(hpaned7), vpaned3, FALSE, TRUE);
	gtk_paned_set_position(GTK_PANED(vpaned3), 88);

	vbox65 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox65);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "vbox65",
				 vbox65,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox65);
	gtk_paned_pack1(GTK_PANED(vpaned3), vbox65, FALSE, TRUE);

	frame_report = gtk_frame_new(_("Search Results"));
	gtk_widget_ref(frame_report);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "frame_report", frame_report,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_report);
	gtk_box_pack_start(GTK_BOX(vbox65), frame_report, TRUE, TRUE,
			   0);
	gtk_widget_set_usize(frame_report, 212, -2);

	scrolledwindow_report = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow_report);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "scrolledwindow_report",
				 scrolledwindow_report,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow_report);
	gtk_container_add(GTK_CONTAINER(frame_report),
			  scrolledwindow_report);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_report),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	frame_preview = gtk_frame_new(_("Preview"));
	gtk_widget_ref(frame_preview);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "frame_preview", frame_preview,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_preview);
	gtk_paned_pack2(GTK_PANED(vpaned3), frame_preview, TRUE, TRUE);

	scrolledwindow_preview = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow_preview);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "scrolledwindow_preview",
				 scrolledwindow_preview,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow_preview);
	gtk_container_add(GTK_CONTAINER(frame_preview),
			  scrolledwindow_preview);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_preview),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	frame_results = gtk_frame_new(_("Found"));
	gtk_widget_ref(frame_results);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "frame_results", frame_results,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_results);
	gtk_paned_pack2(GTK_PANED(hpaned7), frame_results, TRUE, TRUE);

	scrolledwindow_results = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow_results);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "scrolledwindow_results",
				 scrolledwindow_results,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow_results);
	gtk_container_add(GTK_CONTAINER(frame_results),
			  scrolledwindow_results);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_results),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	appbar_search =
	    gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gtk_widget_ref(appbar_search);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog),
				 "appbar_search", appbar_search,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(appbar_search);
	gtk_box_pack_start(GTK_BOX(vbox70), appbar_search, FALSE, FALSE,
			   0);

	label207 = gtk_label_new(_("Results"));
	gtk_widget_ref(label207);
	gtk_object_set_data_full(GTK_OBJECT(search_dialog), "label207",
				 label207,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label207);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook8),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook8), 3),
				   label207);



/*******   ******/
	search.dialog = search_dialog;
	search.label_search_module = label_search_module;
	search.notebook = notebook8;
	search.togglebutton_show_main = togglebutton_show_main;
	search.rb_current_module = rb_one_mod;
	search.rb_mod_list = rb_mod_list;
	search.rb_custom_list = rb_custom_list;
	search.module_lists = clist_mod_lists;
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
	search.shortcut_model = e_shortcut_model_new();

	search.shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(search.shortcut_bar),
				 search.shortcut_model);
	gtk_widget_set_usize(search.shortcut_bar, 150, 250);
	gtk_widget_show(search.shortcut_bar);
	gtk_paned_pack1(GTK_PANED(hpaned8), search.shortcut_bar, TRUE,
			TRUE);

	search.which_scope = GTK_TOGGLE_BUTTON(search.rb_no_scope);

	/* add html widgets */

	search.report_html = gtk_html_new();
	gtk_widget_ref(search.report_html);
	gtk_object_set_data_full(GTK_OBJECT(search.dialog),
				 "search.report_html",
				 search.report_html, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(search.report_html);
	gtk_html_load_empty(GTK_HTML(search.report_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow_report),
			  search.report_html);

	search.preview_html = gtk_html_new();
	gtk_widget_ref(search.preview_html);
	gtk_object_set_data_full(GTK_OBJECT(search.dialog),
				 "search.preview_html",
				 search.preview_html, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(search.preview_html);
	gtk_html_load_empty(GTK_HTML(search.preview_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow_preview),
			  search.preview_html);

	search.results_html = gtk_html_new();
	gtk_widget_ref(search.results_html);
	gtk_object_set_data_full(GTK_OBJECT(search.dialog),
				 "search.results_html",
				 search.results_html, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(search.results_html);
	gtk_html_load_empty(GTK_HTML(search.results_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow_results),
			  search.results_html);


	/* connect signals */

	gtk_signal_connect(GTK_OBJECT(button_save_mods_as), "clicked",
			   GTK_SIGNAL_FUNC(save_modlist_as), NULL);
	gtk_signal_connect(GTK_OBJECT(search.rb_current_module),
			   "toggled",
			   GTK_SIGNAL_FUNC(current_module_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(search.rb_no_scope), "toggled",
			   GTK_SIGNAL_FUNC(scope_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(search.rb_mod_list), "toggled",
			   GTK_SIGNAL_FUNC(mod_list_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(search.rb_custom_range),
			   "toggled", GTK_SIGNAL_FUNC(scope_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(button_save_mods), "clicked",
			   GTK_SIGNAL_FUNC(save_modlist), NULL);

	gtk_signal_connect(GTK_OBJECT(button_modlist_new), "clicked",
			   GTK_SIGNAL_FUNC(new_modlist), NULL);

	gtk_signal_connect(GTK_OBJECT(button28), "clicked",
			   GTK_SIGNAL_FUNC(clear_modules), NULL);
	gtk_signal_connect(GTK_OBJECT(button30), "clicked",
			   GTK_SIGNAL_FUNC(delete_module), NULL);

	gtk_signal_connect(GTK_OBJECT(button_delete_list), "clicked",
			   GTK_SIGNAL_FUNC(delete_list), NULL);
	gtk_signal_connect(GTK_OBJECT(button_new_range), "clicked",
			   GTK_SIGNAL_FUNC(new_range), NULL);
	gtk_signal_connect(GTK_OBJECT(button_save_range), "clicked",
			   GTK_SIGNAL_FUNC(save_range), NULL);
	gtk_signal_connect(GTK_OBJECT(button_delete_range), "clicked",
			   GTK_SIGNAL_FUNC(delete_range), NULL);

	gtk_signal_connect(GTK_OBJECT(entry_range_name), "changed",
			   GTK_SIGNAL_FUNC(range_name_changed), NULL);

	gtk_signal_connect(GTK_OBJECT(entry_range_text), "changed",
			   GTK_SIGNAL_FUNC(range_text_changed), NULL);


	gtk_signal_connect(GTK_OBJECT(search.entry_list_name),
			   "changed",
			   GTK_SIGNAL_FUNC(list_name_changed), NULL);


	gtk_signal_connect(GTK_OBJECT(search.clist_range), "select_row",
			   GTK_SIGNAL_FUNC(custom_range_select_row),
			   NULL);

	gtk_signal_connect(GTK_OBJECT(search.module_lists),
			   "select_row",
			   GTK_SIGNAL_FUNC(custom_list_select_row),
			   NULL);

	gtk_signal_connect(GTK_OBJECT(search.clist_modules),
			   "select_row",
			   GTK_SIGNAL_FUNC(modules_select_row), NULL);

	gtk_signal_connect(GTK_OBJECT(search.togglebutton_show_main),
			   "toggled",
			   GTK_SIGNAL_FUNC
			   (on_togglebutton_show_main), NULL);
	gtk_signal_connect(GTK_OBJECT(button_close), "clicked",
			   GTK_SIGNAL_FUNC(on_button_close), NULL);

	gtk_signal_connect(GTK_OBJECT(button_begin_search), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_button_begin_search), NULL);
	gtk_signal_connect(GTK_OBJECT(search.results_html),
			   "link_clicked",
			   GTK_SIGNAL_FUNC(link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(search.results_html), "on_url",
			   GTK_SIGNAL_FUNC(url), NULL);
	gtk_signal_connect(GTK_OBJECT(search.report_html),
			   "link_clicked",
			   GTK_SIGNAL_FUNC(report_link_clicked), NULL);

	gtk_signal_connect(GTK_OBJECT(button_clear_results), "clicked",
			   GTK_SIGNAL_FUNC(button_clean), NULL);


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

		/* load custom ranges in glist and add to clist_ranges */
		search.rangelist = load_custom_ranges();
		add_ranges();
		/* load custom modlist in glist and add to module_list */
		search.modlists = load_custom_modlist();
		add_modlist();

		/* add bibletext group to shortcut bar and fill it */
		search.text_group = add_group_to_sb_search((EShortcutBar
							    *) search.
							   shortcut_bar,
							   _
							   ("Bible Texts"));
		fill_group(search.text_group, get_list(TEXT_DESC_LIST));

		/* add commentary group to shortcut bar and fill it */
		search.comm_group = add_group_to_sb_search((EShortcutBar
							    *) search.
							   shortcut_bar,
							   _
							   ("Commentary"));
		fill_group(search.comm_group, get_list(COMM_DESC_LIST));

		/* add dictionary group to shortcut bar and fill it */
		search.dict_group = add_group_to_sb_search((EShortcutBar
							    *) search.
							   shortcut_bar,
							   _
							   ("Dict/lex"));
		fill_group(search.dict_group, get_list(DICT_DESC_LIST));

		/* add book group to shortcut bar and fill it */
		search.book_group = add_group_to_sb_search((EShortcutBar
							    *) search.
							   shortcut_bar,
							   _("Books"));
		fill_group(search.book_group, get_list(GBS_DESC_LIST));

		/* connect shortcut bar items to callback */
		gtk_signal_connect(GTK_OBJECT(search.shortcut_bar),
				   "item_selected",
				   GTK_SIGNAL_FUNC
				   (shortcut_bar_item_selected), NULL);

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

		is_running = TRUE;
	} else
		gdk_window_raise(GTK_WIDGET(search.dialog)->window);

}


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

void gui_add_to_found_list(gchar * result_text, gchar * module)
{
	gchar buf[256], *utf8str;
	gint utf8len;

	sprintf(buf,
		"&nbsp; <A HREF=\"version=%s passage=%s\"NAME=\"%s\" >%s, %s</A><br>",
		module, result_text, module, module, result_text);
	utf8str = e_utf8_from_gtk_string(search.results_html, buf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gui_display_html(search.results_html, utf8str, utf8len);
	}
}
