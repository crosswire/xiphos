/*
 * GnomeSword Bible Study Tool
 * _bibletext.c - gui for Bible text modules
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

#include "gui/_bibletext.h"
#include "gui/bibletext_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/cipher_key_dialog.h"

#include "main/bibletext.h"
#include "main/gs_html.h"
#include "main/settings.h"
#include "main/lists.h"



/******************************************************************************
 * externs
 */

extern gboolean isrunningVT;
extern gboolean gsI_isrunning;
//extern GList *options;

/******************************************************************************
 * globals to this file only 
 */
static GList *text_list;
static TEXT_DATA *cur_t;
static gboolean display_change = TRUE;


/******************************************************************************
 * Name
 *  get_module_global_options
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  static void get_module_global_options(TEXT_DATA * t)	
 *
 * Description
 *    get global options for a module
 *
 * Return value
 *   void
 */

static void get_module_global_options(TEXT_DATA * t)
{
	t->gbfstrongs =
	    check_for_global_option(t->mod_num, "GBFStrongs");
	t->thmlstrongs =
	    check_for_global_option(t->mod_num, "ThMLStrongs");
	t->gbfmorphs =
	    check_for_global_option(t->mod_num, "GBFMorph");
	t->thmlmorphs =
	    check_for_global_option(t->mod_num, "ThMLMorph");
	t->gbffootnotes =
	    check_for_global_option(t->mod_num, "GBFFootnotes");
	t->thmlfootnotes =
	    check_for_global_option(t->mod_num,"ThMLFootnotes");
	t->greekaccents =
	    check_for_global_option(t->mod_num,
					    "UTF8GreekAccents");
	t->lemmas =
	    check_for_global_option(t->mod_num, "ThMLLemma");
	t->scripturerefs =
	    check_for_global_option(t->mod_num, "ThMLScripref");
	t->hebrewpoints =
	    check_for_global_option(t->mod_num,
					    "UTF8HebrewPoints");
	t->hebrewcant =
	    check_for_global_option(t->mod_num,
					    "UTF8Cantillation");
	t->headings =
	    check_for_global_option(t->mod_num, "ThMLHeadings");
	t->variants =
	    check_for_global_option(t->mod_num, "ThMLVariants");
}

/******************************************************************************
 * Name
 *  set_page_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void set_page_text(gchar * modname, GList * text_list)	
 *
 * Description
 *   change text module by finding page number from module name
 *
 * Return value
 *   void
 */

static void set_page_text(gchar * modname, GList * text_list) 
{
	gint page = 0;
	TEXT_DATA *t = NULL;

	text_list = g_list_first(text_list);
	while (text_list != NULL) {
		t = (TEXT_DATA *) text_list->data;
		if (!strcmp(t->mod_name, modname))
			break;
		++page;
		text_list = g_list_next(text_list);
	}
	cur_t = t;
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_text), page);
	settings.text_last_page = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebook_text),
			settings.text_tabs);
}

/******************************************************************************
 * Name
 *  gui_set_text_frame_label
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void gui_set_text_frame_label(void)	
 *
 * Description
 *   sets text frame label to module name or null
 *
 * Return value
 *   void
 */

void gui_set_text_frame_label(void)
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module name
	 */	
	if (settings.text_tabs)
		gtk_frame_set_label(GTK_FRAME(cur_t->frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(cur_t->frame), cur_t->mod_name);
	
}

/******************************************************************************
 * Name
 *  gui_set_text_page_and_key
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void gui_set_text_page_and_key(gint page_num, gchar * key)	
 *
 * Description
 *   change text module notebook page and display new key (reference)
 *
 * Return value
 *   void
 */

void gui_set_text_page_and_key(gint page_num, gchar * key) 
{
	/*
	 * we don't want backend_dispaly_text to be
	 * called by on_notebook_text_switch_page
	 */
	display_change = FALSE;
	if (settings.text_last_page != page_num) {
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (settings.notebook_text),
				      page_num);
	}

	if (cur_t->is_locked) {
		GtkWidget *dlg;	
		dlg = gui_create_cipher_key_dialog(cur_t->mod_name);
		gtk_widget_show(dlg);

	} else
		display_text(key);
	display_change = TRUE;
}

/******************************************************************************
 * Name
 *  text_page_changed
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void text_page_changed(gint page_num, TEXT_DATA *t)	
 *
 * Description
 *    take care of non gui stuff on notebook page change
 *
 * Return value
 *   void
 */
 
void text_page_changed(gint page_num, TEXT_DATA *t)
{
	/*
	 * remember new module name
	 */
	sprintf(settings.MainWindowModule, "%s", t->mod_name);
	/*
	 * point TEXT_DATA *cur_t to t - cur_t is global to this file
	 */
	cur_t = t;
	/*
	 * remember page number
	 */
	settings.text_last_page = page_num;
	/*
	 * display new module with current verse
	 */
	if (display_change) {
		gui_set_text_page_and_key(page_num,settings.currentverse);
	}
	/*
	 * set global options for current module 
	 */
	set_options_on_page_change(t);		
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void set_text_variant_global_option(gchar * option, gchar * choice)	
 *
 * Description
 *   set sword global option (THMLVariant)
 *
 * Return value
 *   void
 */

void set_text_variant_global_option(gchar * option, gchar * choice)
{
	set_text_global_option(option, choice);
	display_text(settings.currentverse);
}

/******************************************************************************
 * Name
 *  set_options_on_page_change
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void set_options_on_page_change(TEXT_DATA * t)	
 *
 * Description
 *   set module global options on page change
 *
 * Return value
 *   void
 */

void set_options_on_page_change(TEXT_DATA * t)
{
	if (t->gbfstrongs || t->thmlstrongs)
		set_text_module_global_option("Strong's Numbers",
				GTK_TOGGLE_BUTTON(t->t_btn_strongs)->
					       active);
	if (t->gbfmorphs || t->thmlmorphs)
		set_text_module_global_option("Morphological Tags",
				GTK_TOGGLE_BUTTON(t->t_btn_morphs)->
						active);
	if (t->gbffootnotes || t->thmlfootnotes)
		set_text_module_global_option("Footnotes",
				GTK_TOGGLE_BUTTON(t->t_btn_footnotes)->
					       active);
	if (t->greekaccents)
		set_text_module_global_option("Greek Accents",
				GTK_TOGGLE_BUTTON(t->t_btn_accents)->
					       active);
	if (t->lemmas)
		set_text_module_global_option("Lemmas",
				GTK_TOGGLE_BUTTON(t->t_btn_lemmas)->
					       active);
	if (t->scripturerefs)
		set_text_module_global_option("Scripture Cross-references",
				GTK_TOGGLE_BUTTON(t->t_btn_scripturerefs)->
						active);
	if (t->hebrewpoints)
		set_text_module_global_option("Hebrew Vowel Points",
				GTK_TOGGLE_BUTTON(t->t_btn_points)->
					       active);
	if (t->hebrewcant)
		set_text_module_global_option("Hebrew Cantillation",
				GTK_TOGGLE_BUTTON(t->t_btn_cant)->
					       active);
	if (t->headings)
		set_text_module_global_option("Headings",
				GTK_TOGGLE_BUTTON(t->t_btn_headings)->
					       active);
	if (t->variants) {
		if(GTK_RADIO_MENU_ITEM(t->t_btn_primary)->check_menu_item.active)
			set_text_global_option("Textual Variants", "Primary Reading");
		else if(GTK_RADIO_MENU_ITEM(t->t_btn_secondary)->check_menu_item.active)
			set_text_global_option("Textual Variants", "Secondary Reading");
		else if(GTK_RADIO_MENU_ITEM(t->t_btn_all)->check_menu_item.active)
			set_text_global_option("Textual Variants", "All Readings");
		display_text(settings.currentverse);
	}
}

/******************************************************************************
 * Name
 *  on_notebook_text_switch_page
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_notebook_text_switch_page(GtkNotebook * notebook,
 *				  GtkNotebookPage * page,
 *				  gint page_num, GList * tl)	
 *
 * Description
 *   sets gui to new sword module
 *
 * Return value
 *   void
 */

void on_notebook_text_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, GList * tl)
{
	TEXT_DATA *t;
	gchar title[200];
	/*
	 * get data structure for new module 
	 */
	t = (TEXT_DATA *) g_list_nth_data(tl, page_num);
	/*
	 * do work that's non gui
	 */
	text_page_changed(page_num, t);
	/*
	 * set program title to GnomeSWORD + current text module name 
	 */
	sprintf(title, "GnomeSWORD - %s", text_get_description(t->mod_num));
	gtk_window_set_title(GTK_WINDOW(settings.app), title);
	/*
	 *  hide/show toolbar 
	 */
	if(settings.text_tool)
		gtk_widget_show(t->frame_toolbar);
	else
		gtk_widget_hide(t->frame_toolbar);
	GTK_CHECK_MENU_ITEM(t->showtoolbar)->active = settings.text_tool;
	/*
	 *  keep showtabs menu item current 
	 */
	GTK_CHECK_MENU_ITEM(t->showtabs)->active = settings.text_tabs;
	
	gui_set_text_frame_label();
	settings.html_text = t->html;
}

/******************************************************************************
 * Name
 *  on_copy_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_copy_activate(GtkMenuItem * menuitem, TEXT_DATA * t)	
 *
 * Description
 *   copy selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_copy_activate(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	copyGS_HTML(t->html);
}

/******************************************************************************
 * Name
 *  on_find_activate
 *
 * Synopsis
 *   #include "gs_bibletext.h"
 *
 *  void on_find_activate(GtkMenuItem * menuitem, TEXT_DATA * t)	
 *
 * Description
 *   opens find dialog
 *
 * Return value
 *   
 */

static void on_find_activate(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	//searchGS_FIND_DLG(c, FALSE, NULL);
}

/******************************************************************************
 * Name
 *  on_lookup_selection_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *   void on_lookup_selection_activate(GtkMenuItem * menuitem,
 *				  gchar * modDescription)   	
 *
 * Description
 *   lookup seledtion in a dict/lex module
 *
 * Return value
 *   void
 */

static void on_lookup_selection_activate(GtkMenuItem * menuitem,
				  gchar * dict_mod_description)
{
	gchar *dict_key, mod_name[16];

	memset(mod_name, 0, 16);
	module_name_from_description(mod_name, dict_mod_description);
	
	dict_key = get_word_or_selection(cur_t->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(mod_name, dict_key);
		if (settings.inDictpane)
			change_module_and_key(mod_name, dict_key);
		g_free(dict_key);
	}
}

/******************************************************************************
 * Name
 *  on_same_lookup_selection_activate				       
 *
 * Synopsis
 *  #include "_bibletext.h"
 *
 *  void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
 *                                         TEXT_DATA * t) 	
 *
 * Description
 *   lookup seledtion in current dict/lex module
 *
 * Return value
 *   void
 */

static void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
				       TEXT_DATA * t)
{
	gchar *dict_key = get_word_or_selection(t->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(settings.DictWindowModule,
						  dict_key);
		if (settings.inDictpane)
			change_module_and_key(settings.DictWindowModule,
					      dict_key);
		g_free(dict_key);
	}
}

/******************************************************************************
 * Name
 *  on_view_mod_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_view_mod_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   show a different text module by changing the notebook page
 *
 * Return value
 *   void
 */

static void on_view_mod_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{
	gint page;

	page = GPOINTER_TO_INT(user_data);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_text),
			      page);
}

/******************************************************************************
 * Name
 *  on_text_showtabs_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  void on_text_showtabs_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   display text module notebook tabs
 *
 * Return value
 *   void
 */

static void on_text_showtabs_activate(GtkMenuItem *menuitem,
		gpointer user_data)
{
	settings.text_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebook_text),
				   settings.text_tabs); 
	gui_set_text_frame_label();
}

/******************************************************************************
 * Name
 *   on_text_showtoolbar_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_text_showtoolbar_activate(GtkMenuItem * menuitem,
 *						TEXT_DATA * t)
 *
 * Description
 *   display/hide text toolbar
 *
 * Return value
 *   void
 */

static void on_text_showtoolbar_activate(GtkMenuItem * menuitem,
						TEXT_DATA * t)
{
	settings.text_tool = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if(settings.text_tool)
		gtk_widget_show(t->frame_toolbar);
	else
		gtk_widget_hide(t->frame_toolbar);
}

/******************************************************************************
 * Name
 *  on_view_new_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  void on_view_new_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   opens view text dialog
 *
 * Return value
 *   void
 */

static void on_view_new_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	static GtkWidget *dlg;
	GdkCursor *cursor;

	gtk_widget_show(settings.app);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(settings.app->window, cursor);

	if (!isrunningVT) {
		dlg = gui_create_bibletext_dialog();
		isrunningVT = TRUE;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(settings.app);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(settings.app->window, cursor);
}

/******************************************************************************
 * Name
 *  on_unlock_key_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  void on_unlock_key_activate(GtkMenuItem * menuitem,TEXT_DATA * t) 	
 *
 * Description
 *   opens add cipher key dialog
 *
 * Return value
 *   void
 */

static void on_unlock_key_activate(GtkMenuItem * menuitem,
				   TEXT_DATA * t)
{
	GtkWidget *dlg;
	dlg = gui_create_cipher_key_dialog(t->mod_name);
	gnome_dialog_set_default(GNOME_DIALOG(dlg), 2);
	gnome_dialog_run_and_close(GNOME_DIALOG(dlg));
}

/******************************************************************************
 * Name
 *  gui_create_pm_text
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   GtkWidget *gui_create_pm_text(TEXT_DATA * t)
 *
 * Description
 *   create popup menu for text module - attached to html widget
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_pm_text(TEXT_DATA * t)
{
	GtkWidget *pm_text;
	GtkAccelGroup *pm_text_accels;
	GtkWidget *copy;
	GtkWidget *separator;
	GtkWidget *lookup_selection;
	GtkWidget *lookup_selection_menu;
	GtkAccelGroup *lookup_selection_menu_accels;
	GtkWidget *usecurrent;
	GtkWidget *item3;
	GtkWidget *item4;
	GtkWidget *view_new;
	GtkWidget *view_text;
	GtkWidget *view_text_menu;
	GtkAccelGroup *view_text_menu_accels;
	GtkWidget *find;
	GList *tmp;
	gint i;

	tmp = NULL;

	pm_text = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pm_text), "pm_text", pm_text);
	pm_text_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(pm_text));

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_ref(copy);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "copy", copy,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(pm_text), copy);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_ref(find);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "find", find,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(pm_text), find);

	t->showtabs =
	    gtk_check_menu_item_new_with_label(_("Show Tabs"));
	gtk_widget_ref(t->showtabs);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "t->showtabs",
				 t->showtabs, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(t->showtabs);
	gtk_container_add(GTK_CONTAINER(pm_text), t->showtabs);

	t->showtoolbar =
	    gtk_check_menu_item_new_with_label(_("Show Toolbar"));
	gtk_widget_ref(t->showtoolbar);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "t->showtoolbar",
				 t->showtoolbar, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(t->showtoolbar);
	gtk_container_add(GTK_CONTAINER(pm_text), t->showtoolbar);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm_text), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	view_new = gtk_menu_item_new_with_label("View in new window");
	gtk_widget_ref(view_new);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "view_new",
				 view_new, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(view_new);
	gtk_container_add(GTK_CONTAINER(pm_text), view_new);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm_text), separator);
	gtk_widget_set_sensitive(separator, FALSE);


	lookup_selection =
	    gtk_menu_item_new_with_label(_("Lookup Selection"));
	gtk_widget_ref(lookup_selection);
	gtk_object_set_data_full(GTK_OBJECT(pm_text),
				 "lookup_selection", lookup_selection,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(lookup_selection);
	gtk_container_add(GTK_CONTAINER(pm_text), lookup_selection);

	lookup_selection_menu = gtk_menu_new();
	gtk_widget_ref(lookup_selection_menu);
	gtk_object_set_data_full(GTK_OBJECT(pm_text),
				 "lookup_selection_menu",
				 lookup_selection_menu,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lookup_selection),
				  lookup_selection_menu);
	lookup_selection_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (lookup_selection_menu));

	usecurrent =
	    gtk_menu_item_new_with_label("Use Current Dictionary");
	gtk_widget_ref(usecurrent);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "usecurrent",
				 usecurrent, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(usecurrent);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent);


	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm_text), separator);
	gtk_widget_set_sensitive(separator, FALSE);


	view_text = gtk_menu_item_new_with_label(_("View Text"));
	gtk_widget_ref(view_text);
	gtk_object_set_data_full(GTK_OBJECT(pm_text),
				 "view_text", view_text,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(view_text);
	gtk_container_add(GTK_CONTAINER(pm_text), view_text);

	view_text_menu = gtk_menu_new();
	gtk_widget_ref(view_text_menu);
	gtk_object_set_data_full(GTK_OBJECT(pm_text),
				 "view_text_menu",
				 view_text_menu, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_text),
				  view_text_menu);
	view_text_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(view_text_menu));
	/*
	 * if module has cipher key include this item
	 */
	if (t->is_locked) {
		GtkWidget *add_module_key;
		separator = gtk_menu_item_new();
		gtk_widget_ref(separator);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "separator", separator,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(separator);
		gtk_container_add(GTK_CONTAINER(pm_text), separator);
		gtk_widget_set_sensitive(separator, FALSE);

		add_module_key =
		    gtk_menu_item_new_with_label(_
						 ("Unlock This Module"));
		gtk_widget_ref(add_module_key);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "add_module_key",
					 add_module_key,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(add_module_key);
		gtk_container_add(GTK_CONTAINER(pm_text),
				  add_module_key);

		gtk_signal_connect(GTK_OBJECT(add_module_key),
				   "activate",
				   GTK_SIGNAL_FUNC
				   (on_unlock_key_activate), t);
	}

	tmp = get_list(DICT_DESC_LIST);
	while (tmp != NULL) {
		item4 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item4);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "item4", item4,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item4);
		gtk_signal_connect(GTK_OBJECT(item4), "activate",
				   GTK_SIGNAL_FUNC
				   (on_lookup_selection_activate),
				   (gchar *) tmp->data);
		gtk_container_add(GTK_CONTAINER
				  (lookup_selection_menu), item4);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	i = 0;
	tmp = get_list(TEXT_DESC_LIST);
	while (tmp != NULL) {
		item3 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "item3", item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_view_mod_activate),
				   GINT_TO_POINTER(i));
		gtk_container_add(GTK_CONTAINER(view_text_menu), item3);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	/*
	 * for using the current dictionary for lookup 
	 */
	gtk_signal_connect(GTK_OBJECT(usecurrent), "activate",
			   GTK_SIGNAL_FUNC
			   (on_same_lookup_selection_activate), t);

	gtk_signal_connect(GTK_OBJECT(copy), "activate",
			   GTK_SIGNAL_FUNC(on_copy_activate), t);
	gtk_signal_connect(GTK_OBJECT(find), "activate",
			   GTK_SIGNAL_FUNC(on_find_activate), t);
	gtk_signal_connect(GTK_OBJECT(t->showtabs), "activate",
			   GTK_SIGNAL_FUNC
			   (on_text_showtabs_activate), NULL);
	
	gtk_signal_connect(GTK_OBJECT(t->showtoolbar), "activate",
			   GTK_SIGNAL_FUNC
			   (on_text_showtoolbar_activate), t);
	gtk_signal_connect(GTK_OBJECT(view_new), "activate",
			   GTK_SIGNAL_FUNC
			   (on_view_new_activate), NULL);
	return pm_text;
}

/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  gboolean on_button_release_event(GtkWidget * widget,
			    GdkEventButton * event, TEXT_DATA * t)	
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

static gboolean on_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					TEXT_DATA * t)
{
	extern gboolean in_url;
	gchar *key;

	settings.whichwindow = MAIN_TEXT_WINDOW;

	switch (event->button) {
	case 1:
		if (!in_url) {
			key = buttonpresslookupGS_HTML(t->html);
			if (key) {
				gchar *dict = NULL;
				if (settings.useDefaultDict)
					dict = g_strdup(settings.DefaultDict);
				else
					dict = g_strdup(settings.DictWindowModule);
				if (settings.inViewer)
					gui_display_dictlex_in_viewer(dict,
								  key);
				if (settings.inDictpane)
					change_module_and_key(dict,
							      key);
				g_free(key);
				if (dict)
					g_free(dict);
			}
		}
		return TRUE;
		break;
	case 2:
		return TRUE;
		break;
	case 3:
		return TRUE;
		break;
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *  on_t_btn_toggled
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  void on_t_btn_toggled(GtkToggleButton *togglebutton, gchar *option)	
 *
 * Description
 *   called when global option toggle button is press
 *
 * Return value
 *   void
 */

static void on_t_btn_toggled(GtkToggleButton * togglebutton,
			     gchar * option)
{
	set_text_module_global_option(option, togglebutton->active);
}

void
on_primary_reading_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	set_text_variant_global_option("Textual Variants",
				       "Primary Reading");
}

void
on_secondary_reading_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
	set_text_variant_global_option("Textual Variants",
				       "Secondary Reading");
}

void
on_all_readings_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	set_text_variant_global_option("Textual Variants",
				       "All Readings");
}

static GnomeUIInfo primary_reading_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Primary Reading"),
	 NULL,
	 (gpointer) on_primary_reading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Secondary Reading"),
	 NULL,
	 (gpointer) on_secondary_reading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("All Readings"),
	 NULL,
	 (gpointer) on_all_readings_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo textual_variants_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_RADIOITEMS, NULL, NULL, primary_reading_uiinfo,
	 NULL, NULL, GNOME_APP_PIXMAP_NONE, NULL, 0,
	 (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo variant_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Textual Variants"),
	 NULL,
	 textual_variants_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

/******************************************************************************
 * Name
 *  gui_create_text_pane
 *
 * Synopsis
 *   #include "_bibletext.h"
 *   void gui_create_text_pane(TEXT_DATA * t)
 *  	
 *
 * Description
 *   create a text pane(window) for each text module
 *
 * Return value
 *   void
 */

void gui_create_text_pane(TEXT_DATA * t)
{
	GtkWidget *vbox;
	GtkWidget *toolbar;
	GtkWidget *scrolledwindow;
	GtkWidget *label;
	GtkWidget *variant_menu;

	t->frame = gtk_frame_new(NULL);
	gtk_widget_ref(t->frame);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "t->frame",
				 t->frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(t->frame);
	gtk_container_add(GTK_CONTAINER(settings.notebook_text), t->frame);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox", vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(t->frame), vbox);

	t->frame_toolbar = gtk_frame_new(NULL);
	gtk_widget_ref(t->frame_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "t->frame_toolbar", t->frame_toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	//gtk_widget_show(t->frame_toolbar);
	gtk_box_pack_start(GTK_BOX(vbox), t->frame_toolbar, FALSE, TRUE, 0);

	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_TEXT);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "toolbar",
				 toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_container_add(GTK_CONTAINER(t->frame_toolbar), toolbar);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);

	if (t->gbfstrongs || t->thmlstrongs) {
		t->t_btn_strongs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("S"),
					       _
					       ("Toggle Strongs Numbers"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_strongs);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "t->t_btn_strongs",
					 t->t_btn_strongs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_strongs);
		gtk_widget_set_usize(t->t_btn_strongs, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_strongs),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Strong's Numbers");
	}
	if (t->gbfmorphs || t->thmlmorphs) {
		t->t_btn_morphs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("M"),
					       _("Toggle Morph Tags"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_morphs);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "t->t_btn_morphs",
					 t->t_btn_morphs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_morphs);
		gtk_widget_set_usize(t->t_btn_morphs, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_morphs),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Morphological Tags");
	}
	if (t->gbffootnotes || t->thmlfootnotes) {
		t->t_btn_footnotes =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("F"),
					       _("Toggle Footnotes"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_footnotes);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "t->t_btn_footnotes",
					 t->t_btn_footnotes,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_footnotes);
		gtk_widget_set_usize(t->t_btn_footnotes, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_footnotes),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Footnotes");
	}
	if (t->greekaccents) {
		t->t_btn_accents =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("A"),
					       _
					       ("Toggle Greek Accents"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_accents);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "t->t_btn_accents",
					 t->t_btn_accents,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_accents);
		gtk_widget_set_usize(t->t_btn_accents, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_accents),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Greek Accents");

	}
	if (t->lemmas) {
		t->t_btn_lemmas =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("L"),
					       _("Toggle Lemmas"), NULL,
					       NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_lemmas);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "t->t_btn_lemmas",
					 t->t_btn_lemmas,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_lemmas);
		gtk_widget_set_usize(t->t_btn_lemmas, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_lemmas),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Lemmas");

	}
	if (t->scripturerefs) {
		t->t_btn_scripturerefs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("R"),
					       _
					       ("Toggle Scripture References"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_scripturerefs);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "t->t_btn_scripturerefs",
					 t->t_btn_scripturerefs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_scripturerefs);
		gtk_widget_set_usize(t->t_btn_scripturerefs, 24, 24);


		gtk_signal_connect(GTK_OBJECT(t->t_btn_scripturerefs),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Scripture Cross-references");

	}
	if (t->hebrewpoints) {
		t->t_btn_points =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("P"),
					       _
					       ("Toggle Hebrew Vowel Points"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_points);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "t->t_btn_points",
					 t->t_btn_points,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_points);
		gtk_widget_set_usize(t->t_btn_points, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_points),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Hebrew Vowel Points");

	}
	if (t->hebrewcant) {
		t->t_btn_cant =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("C"),
					       _
					       ("Toggle Hebrew Cantillation"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_cant);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "t->t_btn_cant", t->t_btn_cant,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_cant);
		gtk_widget_set_usize(t->t_btn_cant, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_cant), "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Hebrew Cantillation");

	}
	if (t->headings) {
		t->t_btn_headings =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("H"),
					       _("Toggle Headings"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_headings);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "t->t_btn_headings",
					 t->t_btn_headings,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_headings);
		gtk_widget_set_usize(t->t_btn_headings, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_headings),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Headings");
	}
	if (t->variants) {
		variant_menu = gtk_menu_bar_new();
		gtk_widget_ref(variant_menu);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "variant_menu", variant_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(variant_menu);
		gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar),
					  variant_menu, NULL, NULL);
		gnome_app_fill_menu(GTK_MENU_SHELL(variant_menu),
				    variant_menu_uiinfo, NULL, FALSE,
				    0);

		gtk_widget_ref(variant_menu_uiinfo[0].widget);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "textual_variants",
					 variant_menu_uiinfo[0].widget,
					 (GtkDestroyNotify)
					 gtk_widget_unref);

		gtk_widget_ref(primary_reading_uiinfo[0].widget);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "primary_reading",
					 primary_reading_uiinfo[0].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
					       (primary_reading_uiinfo
						[0].widget), TRUE);

		t->t_btn_primary = primary_reading_uiinfo[0].widget;

		gtk_widget_ref(primary_reading_uiinfo[1].widget);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "secondary_reading",
					 primary_reading_uiinfo[1].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);

		t->t_btn_secondary = primary_reading_uiinfo[1].widget;

		gtk_widget_ref(primary_reading_uiinfo[2].widget);
		gtk_object_set_data_full(GTK_OBJECT(settings.app),
					 "all_readings",
					 primary_reading_uiinfo[2].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);

		t->t_btn_all = primary_reading_uiinfo[2].widget;
	}

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "scrolledwindow",
				 scrolledwindow,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	t->html = gtk_html_new();
	gtk_widget_ref(t->html);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "t->html", t->html, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(t->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), t->html);
	gtk_html_load_empty(GTK_HTML(t->html));

	label = gtk_label_new(t->mod_name);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label",
				 label, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.notebook_text),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(settings.notebook_text),
				    t->mod_num), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (settings.notebook_text),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (settings.notebook_text),
					  t->mod_num),
					 (gchar *) t->mod_name);

	gtk_signal_connect(GTK_OBJECT(t->html), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(t->html), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) settings.app);
	gtk_signal_connect(GTK_OBJECT(t->html),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_button_release_event), (TEXT_DATA *) t);
}
/******************************************************************************
 * Name
 *  gui_setup_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void gui_setup_text(GList *mods)
 *
 * Description
 *   set up gui for sword text modules - return list of text module names
 *
 * Return value
 *   void
 */

void gui_setup_text(GList *mods) 
{
	GtkWidget *popupmenu;
	GList *tmp = NULL;
	gchar *modname;
	gchar *modbuf;
	TEXT_DATA *t;
	gint count = 0;

	text_list = NULL;
	
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		t = g_new(TEXT_DATA, 1);
		t->mod_name = modname;
		t->mod_num = count;
		t->search_string = NULL;
		t->key = NULL;
		t->find_dialog = NULL;
		t->is_locked = module_is_locked(t->mod_name);
		get_module_global_options(t);
		gui_create_text_pane(t);
		popupmenu = gui_create_pm_text(t);
		gnome_popup_menu_attach(popupmenu, t->html, NULL);
		new_text_display(t->html, t->mod_name);
		text_list = g_list_append(text_list, (TEXT_DATA *) t);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(settings.notebook_text),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebook_text_switch_page), text_list);

	modbuf = g_strdup(settings.MainWindowModule);

	set_page_text(modbuf, text_list);

	g_free(modbuf);
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *  gui_shutdown_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void gui_shutdown_text(void)	
 *
 * Description
 *   shut down text module support clean mem
 *
 * Return value
 *   void
 */

void gui_shutdown_text(void) 
{
	text_list = g_list_first(text_list);
	while (text_list != NULL) {
		TEXT_DATA *t = (TEXT_DATA *) text_list->data;
		/* 
		 * free any search dialogs created 
		 */
		if (t->find_dialog)
			 g_free(t->find_dialog);
		/* 
		 * free each TEXT_DATA item created 
		 */
		 g_free((TEXT_DATA *) text_list->data);
		 text_list = g_list_next(text_list);
	} g_list_free(text_list);
}





