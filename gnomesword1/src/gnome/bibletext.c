/*
 * GnomeSword Bible Study Tool
 * bibletext.c - gui for Bible text modules
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

#include "gui/gnomesword.h"
#include "gui/gtkhtml_display.h"
#include "gui/bibletext.h"
#include "gui/bibletext_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/cipher_key_dialog.h"
#include "gui/html.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/dialog.h"
#include "gui/font_dialog.h"
#include "gui/dictlex.h"

#include "main/bibletext.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"

static void on_notebook_text_switch_page(GtkNotebook * notebook,
					 GtkNotebookPage * page,
					 gint page_num, GList * tl);
/******************************************************************************
 * externs
 */

extern gboolean gsI_isrunning;

/******************************************************************************
 * globals to this file only 
 */
static GList *text_list;
static TEXT_DATA *cur_t;
static gboolean display_change = TRUE;


/******************************************************************************
 * Name
 *  gui_get_module_global_options
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  static void gui_get_module_global_options(TEXT_DATA * t)	
 *
 * Description
 *    get global options for a module
 *
 * Return value
 *   void
 */

void gui_get_module_global_options(TEXT_DATA * t)
{
	t->tgs->strongs =
	    check_for_global_option(t->mod_name, "GBFStrongs");
	if(!t->tgs->strongs)
		t->tgs->strongs =
	    	    check_for_global_option(t->mod_name, "ThMLStrongs");
	if(!t->tgs->strongs)
		t->tgs->strongs =
	   	    check_for_global_option(t->mod_name, "OSISStrongs");
	t->tgs->morphs =
	    check_for_global_option(t->mod_name, "GBFMorph");
	if(!t->tgs->morphs)
		t->tgs->morphs =
		    check_for_global_option(t->mod_name, "OSISSMorph");
	if(!t->tgs->morphs)
		t->tgs->morphs =
	    	    check_for_global_option(t->mod_name, "ThMLMorph");
	t->tgs->gbffootnotes =
	    check_for_global_option(t->mod_name, "GBFFootnotes");
	t->tgs->thmlfootnotes =
	    check_for_global_option(t->mod_name, "ThMLFootnotes");
	t->tgs->greekaccents =
	    check_for_global_option(t->mod_name, "UTF8GreekAccents");
	t->tgs->lemmas =
	    check_for_global_option(t->mod_name, "ThMLLemma");
	t->tgs->scripturerefs =
	    check_for_global_option(t->mod_name, "ThMLScripref");
	t->tgs->hebrewpoints =
	    check_for_global_option(t->mod_name, "UTF8HebrewPoints");
	t->tgs->hebrewcant =
	    check_for_global_option(t->mod_name, "UTF8Cantillation");
	t->tgs->headings =
	    check_for_global_option(t->mod_name, "ThMLHeadings");
	t->tgs->variants =
	    check_for_global_option(t->mod_name, "ThMLVariants");
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
	if (page)
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_text), page);
	else
		on_notebook_text_switch_page(GTK_NOTEBOOK
					     (widgets.notebook_text),
					     NULL, page, text_list);
	settings.text_last_page = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_text),
				   settings.text_tabs);
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

static void text_page_changed(gint page_num, TEXT_DATA * t)
{
	/*
	 * remember new module name
	 */
	sprintf(settings.MainWindowModule, "%s", t->mod_name);
	/*
	 * remember page number
	 */
	settings.text_last_page = page_num;
	/*
	 * display new module with current verse
	 */
	if (display_change) {
		gui_set_text_page_and_key(page_num,
					  settings.currentverse);
	}
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

static void set_text_variant_global_option(gchar * option,
					   gchar * choice)
{
	set_text_global_option(option, choice);
	chapter_display(cur_t->html, cur_t->mod_name,
			cur_t->tgs, settings.currentverse, TRUE);
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

static void on_notebook_text_switch_page(GtkNotebook * notebook,
					 GtkNotebookPage * page,
					 gint page_num, GList * tl)
{
	TEXT_DATA *t;
	/*
	 * get data structure for new module 
	 */
	t = (TEXT_DATA *) g_list_nth_data(tl, page_num);
	/*
	 * create pane if it does not already exist
	 */
	if (!t->frame) {
		gui_add_new_text_pane(t);
	}
	/*
	 * point TEXT_DATA *cur_t to t - cur_t is global to this file
	 */
	cur_t = t;
	/*
	 * do work that's non gui
	 */
	text_page_changed(page_num, t);
	/*
	 * set program title current text module name 
	 */
	gui_change_window_title(t->mod_name);
	/*
	 * set search module to current text module 
	 */
	strcpy(settings.sb_search_mod, t->mod_name);
	/*
	 * set search label to current text module 
	 */
	gui_set_search_label(t->mod_name);
	/*
	 *  hide/show toolbar 
	 */
	if (settings.text_tool)
		gtk_widget_show(t->frame_toolbar);
	else
		gtk_widget_hide(t->frame_toolbar);
	GTK_CHECK_MENU_ITEM(t->showtoolbar)->active =
	    settings.text_tool;
	/*
	 *  set words_in_red
	 */
	settings.strip_words_in_red = t->strip_words_in_red;
	/*
	 *  keep showtabs menu item current 
	 */
	GTK_CHECK_MENU_ITEM(t->showtabs)->active = settings.text_tabs;

	gui_set_text_frame_label();
	widgets.html_text = t->html;
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
	gui_copy_html(t->html);
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

	dict_key = gui_get_word_or_selection(cur_t->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(mod_name,
						      dict_key);
		if (settings.inDictpane)
			gui_change_module_and_key(mod_name, dict_key);
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
	gchar *dict_key = gui_get_word_or_selection(t->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(settings.
						      DictWindowModule,
						      dict_key);
		if (settings.inDictpane)
			gui_change_module_and_key(settings.
						  DictWindowModule,
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
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_text),
			      page);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void (GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void edit_percomm(GtkMenuItem * menuitem, gpointer user_data)
{
	if(settings.use_percomm_dialog)
		gui_open_commentary_editor((gchar *) user_data);
	else {
		gtk_notebook_set_page(GTK_NOTEBOOK(
			widgets.workbook_lower),settings.percomm_page);
		gui_set_percomm_page((gchar *) user_data);		
	}
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

static void on_text_showtabs_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	settings.text_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_text),
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
	if (settings.text_tool)
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
 *  void on_view_new_activate(GtkMenuItem * menuitem, TEXT_DATA * t)	
 *
 * Description
 *   opens view text dialog
 *
 * Return value
 *   void
 */

static void on_view_new_activate(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	gui_open_bibletext_dialog(t->mod_name);
}

/******************************************************************************
 * Name
 *  gui_unlock_bibletext
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  void gui_unlock_bibletext(GtkMenuItem * menuitem,TEXT_DATA * t) 	
 *
 * Description
 *   opens add cipher key dialog
 *
 * Return value
 *   void
 */

void gui_unlock_bibletext(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	gchar *cipher_key;

//      g_warning(t->cipher_old);

	cipher_key = gui_add_cipher_key(t->mod_name, t->cipher_old);
	if (cipher_key) {
		t->cipher_key = cipher_key;
		cur_t = t;
		gui_module_is_locked_display(t->html,
					     t->mod_name,
					     t->cipher_key);
	}
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void set_module_font_activate(GtkMenuItem * menuitem,
				     TEXT_DATA * t)
{
	gui_set_module_font(t->mod_name);
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

static GtkWidget *create_pm_text(TEXT_DATA * t)
{
	GtkWidget *pm_text;
	GtkAccelGroup *pm_text_accels;
	GtkWidget *copy;
	GtkWidget *separator;
	GtkWidget *edit_note;
	GtkWidget *edit_note_menu;
	GtkAccelGroup *edit_note_menu_accels;
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
	GtkWidget *set_font;
	GtkWidget *find;
	GList *tmp;
	gint i;
	GtkTooltips *tooltips;

	tmp = NULL;

	tooltips = gtk_tooltips_new();

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

	if (settings.havepercomm) {
		separator = gtk_menu_item_new();
		gtk_widget_ref(separator);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "separator", separator,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(separator);
		gtk_container_add(GTK_CONTAINER(pm_text), separator);
		gtk_widget_set_sensitive(separator, FALSE);

		edit_note =
		    gtk_menu_item_new_with_label(_("Edit Note"));
		gtk_widget_ref(edit_note);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "edit_note", edit_note,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(edit_note);
		gtk_container_add(GTK_CONTAINER(pm_text), edit_note);

		edit_note_menu = gtk_menu_new();
		gtk_widget_ref(edit_note_menu);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "edit_note_menu",
					 edit_note_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_note),
					  edit_note_menu);
		edit_note_menu_accels =
		    gtk_menu_ensure_uline_accel_group(GTK_MENU
						      (edit_note_menu));
	}

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


	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "separator",
				 separator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm_text), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	set_font = gtk_menu_item_new_with_label(_("Set Module Font"));
	gtk_widget_ref(set_font);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "set_font",
				 set_font,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(set_font);
	gtk_container_add(GTK_CONTAINER(pm_text), set_font);
	gtk_tooltips_set_tip(tooltips, set_font,
			     _("Set font for this module"), NULL);

	/*
	 * if module has cipher key include this item
	 */
	if (t->cipher_old || t->is_locked) {
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
				   (gui_unlock_bibletext), t);
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

	if (settings.havepercomm) {
		i = 0;
		tmp = get_list(PERCOMM_LIST);
		while (tmp != NULL) {
			item3 =
			    gtk_menu_item_new_with_label((gchar *) tmp->
							 data);
			gtk_widget_ref(item3);
			gtk_object_set_data_full(GTK_OBJECT(pm_text),
						 "item3", item3,
						 (GtkDestroyNotify)
						 gtk_widget_unref);
			gtk_widget_show(item3);

			gtk_signal_connect(GTK_OBJECT(item3),
					   "activate",
					   GTK_SIGNAL_FUNC
					   (edit_percomm),
					   (gchar *) tmp->data);

			gtk_container_add(GTK_CONTAINER(edit_note_menu),
					  item3);
			++i;
			tmp = g_list_next(tmp);
		}
		g_list_free(tmp);
	}
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
			   GTK_SIGNAL_FUNC(on_view_new_activate), t);


	gtk_signal_connect(GTK_OBJECT(set_font), "activate",
			   GTK_SIGNAL_FUNC(set_module_font_activate),
			   t);

	gtk_object_set_data(GTK_OBJECT(pm_text), "tooltips", tooltips);
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
	/*
	 * set program title to current text module name 
	 */
	gui_change_window_title(t->mod_name);
	/*
	 * set search module to current text module 
	 */
	strcpy(settings.sb_search_mod, t->mod_name);
	/*
	 * set search frame label to current text module 
	 */
	gui_set_search_label(t->mod_name);

	switch (event->button) {
	case 1:
		if (!in_url) {
			key = gui_button_press_lookup(t->html);
			if (key) {
				gchar *dict = NULL;
				if (settings.useDefaultDict)
					dict =
					    g_strdup(settings.
						     DefaultDict);
				else
					dict =
					    g_strdup(settings.
						     DictWindowModule);
				if (settings.inViewer)
					gui_display_dictlex_in_viewer
					    (dict, key);
				if (settings.inDictpane)
					gui_change_module_and_key(dict,
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
	save_module_options(cur_t->mod_name, option,
			    togglebutton->active);
	chapter_display(cur_t->html, cur_t->mod_name, cur_t->tgs,
			settings.currentverse, TRUE);
}

/******************************************************************************
 * Name
 *   print_clicked
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void print_clicked(GtkButton * button, COMM_DATA * c)
 *
 * Description
 *   send comment to the printer
 *
 * Return value
 *   void
 */

static void print_clicked(GtkButton * button, TEXT_DATA * t)
{
	gui_html_print(t->html);
}


/******************************************************************************
 * Name
 *   font_clicked
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void font_clicked(GtkButton * button, COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void font_clicked(GtkButton * button, TEXT_DATA * t)
{
	gui_set_module_font(t->mod_name);
}


/******************************************************************************
 * Name
 *   on_primary_reading_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_primary_reading_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_primary_reading_activate(GtkMenuItem * menuitem,
					gpointer user_data)
{
	set_text_variant_global_option("Textual Variants",
				       "Primary Reading");
}

/******************************************************************************
 * Name
 *   on_secondary_reading_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_secondary_reading_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_secondary_reading_activate(GtkMenuItem * menuitem,
					  gpointer user_data)
{
	set_text_variant_global_option("Textual Variants",
				       "Secondary Reading");
}

/******************************************************************************
 * Name
 *   on_all_readings_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_all_readings_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_all_readings_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
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
 *   add_global_option_buttons 
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void add_global_option_buttons(GtkWidget * toolbar, 
 *				gchar * mod_name, 
 *				TEXT_GLOBALS * tgs, 
 *				GtkMenuCallback callback)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_add_global_option_buttons(GtkWidget * toolbar,
				   gchar * mod_name,
				   TEXT_GLOBALS * tgs,
				   GtkMenuCallback callback)
{
	gint active = 0;
	GtkWidget *variant_menu;
	GtkWidget *tmp_toolbar_icon;

	if (tgs->strongs) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/s.xpm");
		tgs->t_btn_strongs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("S"),
					       _
					       ("Toggle Strongs Numbers"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_ref(tgs->t_btn_strongs);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "tgs->t_btn_strongs",
					 tgs->t_btn_strongs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tgs->t_btn_strongs);
		gtk_widget_set_usize(tgs->t_btn_strongs, 24, 24);

		active =
		    load_module_options(mod_name, "Strong's Numbers");

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (tgs->t_btn_strongs),
					     active);

		gtk_signal_connect(GTK_OBJECT(tgs->t_btn_strongs),
				   "toggled",
				   GTK_SIGNAL_FUNC(callback),
				   "Strong's Numbers");
	}
	if (tgs->morphs) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/m.xpm");
		tgs->t_btn_morphs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("M"),
					       _("Toggle Morph Tags"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_ref(tgs->t_btn_morphs);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "tgs->t_btn_morphs",
					 tgs->t_btn_morphs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tgs->t_btn_morphs);
		gtk_widget_set_usize(tgs->t_btn_morphs, 24, 24);

		active =
		    load_module_options(mod_name, "Morphological Tags");

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (tgs->t_btn_morphs),
					     active);

		gtk_signal_connect(GTK_OBJECT(tgs->t_btn_morphs),
				   "toggled",
				   GTK_SIGNAL_FUNC(callback),
				   "Morphological Tags");
	}
	if (tgs->gbffootnotes || tgs->thmlfootnotes) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/f.xpm");
		tgs->t_btn_footnotes =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("F"),
					       _("Toggle Footnotes"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_ref(tgs->t_btn_footnotes);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "tgs->t_btn_footnotes",
					 tgs->t_btn_footnotes,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tgs->t_btn_footnotes);
		gtk_widget_set_usize(tgs->t_btn_footnotes, 24, 24);

		active = load_module_options(mod_name, "Footnotes");

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (tgs->t_btn_footnotes),
					     active);

		gtk_signal_connect(GTK_OBJECT(tgs->t_btn_footnotes),
				   "toggled",
				   GTK_SIGNAL_FUNC(callback),
				   "Footnotes");
	}
	if (tgs->greekaccents) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/a.xpm");
		tgs->t_btn_accents =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("A"),
					       _
					       ("Toggle Greek Accents"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_ref(tgs->t_btn_accents);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "tgs->t_btn_accents",
					 tgs->t_btn_accents,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tgs->t_btn_accents);
		gtk_widget_set_usize(tgs->t_btn_accents, 24, 24);

		active = load_module_options(mod_name, "Greek Accents");

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (tgs->t_btn_accents),
					     active);

		gtk_signal_connect(GTK_OBJECT(tgs->t_btn_accents),
				   "toggled",
				   GTK_SIGNAL_FUNC(callback),
				   "Greek Accents");

	}
	if (tgs->lemmas) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/l.xpm");
		tgs->t_btn_lemmas =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("L"),
					       _("Toggle Lemmas"), NULL,
					       tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_ref(tgs->t_btn_lemmas);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "tgs->t_btn_lemmas",
					 tgs->t_btn_lemmas,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tgs->t_btn_lemmas);
		gtk_widget_set_usize(tgs->t_btn_lemmas, 24, 24);

		active = load_module_options(mod_name, "Lemmas");

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (tgs->t_btn_lemmas),
					     active);

		gtk_signal_connect(GTK_OBJECT(tgs->t_btn_lemmas),
				   "toggled",
				   GTK_SIGNAL_FUNC(callback), "Lemmas");

	}
	if (tgs->scripturerefs) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/r.xpm");
		tgs->t_btn_scripturerefs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("R"),
					       _
					       ("Toggle Scripture References"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_ref(tgs->t_btn_scripturerefs);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "tgs->t_btn_scripturerefs",
					 tgs->t_btn_scripturerefs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tgs->t_btn_scripturerefs);
		gtk_widget_set_usize(tgs->t_btn_scripturerefs, 24, 24);

		active =
		    load_module_options(mod_name,
					"Scripture Cross-references");

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (tgs->t_btn_scripturerefs),
					     active);


		gtk_signal_connect(GTK_OBJECT(tgs->t_btn_scripturerefs),
				   "toggled",
				   GTK_SIGNAL_FUNC(callback),
				   "Scripture Cross-references");

	}
	if (tgs->hebrewpoints) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/p.xpm");
		tgs->t_btn_points =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("P"),
					       _
					       ("Toggle Hebrew Vowel Points"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_ref(tgs->t_btn_points);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "tgs->t_btn_points",
					 tgs->t_btn_points,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tgs->t_btn_points);
		gtk_widget_set_usize(tgs->t_btn_points, 24, 24);

		active =
		    load_module_options(mod_name,
					"Hebrew Vowel Points");

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (tgs->t_btn_points),
					     active);

		gtk_signal_connect(GTK_OBJECT(tgs->t_btn_points),
				   "toggled",
				   GTK_SIGNAL_FUNC(callback),
				   "Hebrew Vowel Points");

	}
	if (tgs->hebrewcant) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/c.xpm");
		tgs->t_btn_cant =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("C"),
					       _
					       ("Toggle Hebrew Cantillation"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_ref(tgs->t_btn_cant);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "tgs->t_btn_cant",
					 tgs->t_btn_cant,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tgs->t_btn_cant);
		gtk_widget_set_usize(tgs->t_btn_cant, 24, 24);

		active =
		    load_module_options(mod_name,
					"Hebrew Cantillation");

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (tgs->t_btn_cant), active);

		gtk_signal_connect(GTK_OBJECT(tgs->t_btn_cant),
				   "toggled", GTK_SIGNAL_FUNC(callback),
				   "Hebrew Cantillation");

	}
	if (tgs->headings) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/h.xpm");
		tgs->t_btn_headings =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("H"),
					       _("Toggle Headings"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_ref(tgs->t_btn_headings);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "tgs->t_btn_headings",
					 tgs->t_btn_headings,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(tgs->t_btn_headings);
		gtk_widget_set_usize(tgs->t_btn_headings, 24, 24);

		active = load_module_options(mod_name, "Headings");

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (tgs->t_btn_headings),
					     active);

		gtk_signal_connect(GTK_OBJECT(tgs->t_btn_headings),
				   "toggled",
				   GTK_SIGNAL_FUNC(callback),
				   "Headings");
	}
	if (tgs->variants) {
		variant_menu = gtk_menu_bar_new();
		gtk_widget_ref(variant_menu);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
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
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "textual_variants",
					 variant_menu_uiinfo[0].widget,
					 (GtkDestroyNotify)
					 gtk_widget_unref);

		gtk_widget_ref(primary_reading_uiinfo[0].widget);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "primary_reading",
					 primary_reading_uiinfo[0].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
					       (primary_reading_uiinfo
						[0].widget), TRUE);

		tgs->t_btn_primary = primary_reading_uiinfo[0].widget;


		gtk_widget_ref(primary_reading_uiinfo[1].widget);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "secondary_reading",
					 primary_reading_uiinfo[1].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);

		tgs->t_btn_secondary = primary_reading_uiinfo[1].widget;

		gtk_widget_ref(primary_reading_uiinfo[2].widget);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "all_readings",
					 primary_reading_uiinfo[2].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);

		tgs->t_btn_all = primary_reading_uiinfo[2].widget;
	}

}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "bibletext.h"
 *   void create_toolbar(TEXT_DATA * t)
 *  	
 *
 * Description
 *   create a toolbar for each text pane
 *
 * Return value
 *   void
 */

static void create_toolbar(TEXT_DATA * t)
{
	GtkWidget *toolbar;
	GtkWidget *print;
	GtkWidget *font;
	GtkWidget *tmp_toolbar_icon;

	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "toolbar",
				 toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_container_add(GTK_CONTAINER(t->frame_toolbar), toolbar);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);


	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_PRINT);
	print =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"),
				       _("Print Current Chapter"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(print);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "print", print, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(print);
	//gtk_widget_set_usize(tgs->t_btn_headings, 24, 24);    

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_FONT);
	font =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Font"),
				       _
				       ("Set font face or size for current module"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(font);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "font", font, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(font);


	/* add global option buttons to toolbar */
	gui_add_global_option_buttons(toolbar, t->mod_name, t->tgs,
				      (GtkMenuCallback)
				      on_t_btn_toggled);

	gtk_signal_connect(GTK_OBJECT(print),
			   "clicked",
			   GTK_SIGNAL_FUNC(print_clicked), t);
	gtk_signal_connect(GTK_OBJECT(font),
			   "clicked", GTK_SIGNAL_FUNC(font_clicked), t);

}

/******************************************************************************
 * Name
 *  create_pane
 *
 * Synopsis
 *   #include "bibletext.h"
 *   void create_pane(TEXT_DATA * t)
 *  	
 *
 * Description
 *   create a text pane(window) for each text module
 *
 * Return value
 *   void
 */

static void create_pane(TEXT_DATA * t)
{
	GtkWidget *vbox;
	GtkWidget *frame_text;
	GtkWidget *scrolledwindow;
	
	t->frame = gtk_frame_new(NULL);
	gtk_widget_ref(t->frame);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "t->frame",
				 t->frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(t->frame);
	gtk_container_add(GTK_CONTAINER(t->vbox), t->frame);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vbox", vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(t->frame), vbox);

	t->frame_toolbar = gtk_frame_new(NULL);
	gtk_widget_ref(t->frame_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "t->frame_toolbar", t->frame_toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_frame_set_shadow_type(GTK_FRAME(t->frame_toolbar),
				  GTK_SHADOW_NONE);
	//gtk_widget_show(t->frame_toolbar);
	gtk_box_pack_start(GTK_BOX(vbox), t->frame_toolbar, FALSE, TRUE,
			   0);

	create_toolbar(t);


	frame_text = gtk_frame_new(NULL);
	gtk_widget_ref(frame_text);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "frame_text", frame_text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_text);
	gtk_box_pack_start(GTK_BOX(vbox), frame_text, TRUE, TRUE, 0);

	
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "scrolledwindow", scrolledwindow,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow);
	gtk_container_add(GTK_CONTAINER(frame_text), scrolledwindow);
	/*gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE,
	   0); */
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	t->html = gtk_html_new();
	gtk_widget_ref(t->html);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "t->html", t->html, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(t->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), t->html);
	gtk_html_load_empty(GTK_HTML(t->html));


	gtk_signal_connect(GTK_OBJECT(t->html), "link_clicked",
			   GTK_SIGNAL_FUNC(gui_link_clicked),
			   (TEXT_DATA *) t);
	gtk_signal_connect(GTK_OBJECT(t->html), "on_url",
			   GTK_SIGNAL_FUNC(gui_url),
			   (TEXT_DATA *) t);
	gtk_signal_connect(GTK_OBJECT(t->html), "button_release_event",
			   GTK_SIGNAL_FUNC(on_button_release_event),
			   (TEXT_DATA *) t);
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
		gtk_frame_set_label(GTK_FRAME(cur_t->frame),
				    cur_t->mod_name);
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
				      (widgets.notebook_text),
				      page_num);
	}

	if (!cur_t->is_locked)
		chapter_display(cur_t->html, cur_t->mod_name,
				cur_t->tgs, key, TRUE);
	display_change = TRUE;
}

/******************************************************************************
 * Name
 *  gui_display_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   void gui_display_text(gchar * key)
 *  	
 *
 * Description
 *   call chapter_display and pass html wigdet, module name and key
 *
 * Return value
 *   void
 */

void gui_display_text(gchar * key)
{
	if (!cur_t->is_locked)
		chapter_display(cur_t->html, cur_t->mod_name,
				cur_t->tgs, key, TRUE);
	else if (cur_t->cipher_key) {
//              g_warning(cur_t->cipher_key);
		gui_module_is_locked_display(cur_t->html,
					     cur_t->mod_name,
					     cur_t->cipher_key);

	}
}

/******************************************************************************
 * Name
 *  gui_add_new_pane
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void gui_add_new_pane(TEXT_DATA * t)
 *
 * Description
 *   creates a text pane when user selects a new text module
 *
 * Return value
 *   void
 */

void gui_add_new_text_pane(TEXT_DATA * t)
{
	GtkWidget *popupmenu;

	gui_get_module_global_options(t);
	create_pane(t);
	if (t->is_locked)
		gui_module_is_locked_display(t->html, t->mod_name,
					     t->cipher_key);
	popupmenu = create_pm_text(t);
	gnome_popup_menu_attach(popupmenu, t->html, NULL);
}

/******************************************************************************
 * Name
 *  add_vbox_to_notebook
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void add_vbox_to_notebook(TEXT_DATA * t)
 *
 * Description
 *   adds a vbox and label to the text notebook for each text module
 *
 * Return value
 *   void
 */

static void add_vbox_to_notebook(TEXT_DATA * t)
{
	GtkWidget *label;

	t->vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(t->vbox);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "t->vbox", t->vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(t->vbox);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_text),
			  t->vbox);


	label = gtk_label_new(t->mod_name);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label",
				 label, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_text),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(widgets.notebook_text),
				    t->mod_num), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.notebook_text),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.notebook_text),
					  t->mod_num),
					 (gchar *) t->mod_name);
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

void gui_setup_text(GList * mods)
{

	GList *tmp = NULL;
	gchar *modbuf;
	TEXT_DATA *t;
	gint count = 0;

	text_list = NULL;
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		t = g_new(TEXT_DATA, 1);
		t->tgs = g_new(TEXT_GLOBALS, 1);
		t->mod_name = (gchar *) tmp->data;
		t->mod_num = count;
		t->search_string = NULL;
		t->key = NULL;
		t->cipher_key = NULL;
		t->find_dialog = NULL;
		t->strip_words_in_red 
			= load_module_options(t->mod_name, 
				"Strip Words in Red");
		if (has_cipher_tag(t->mod_name)) {
			t->is_locked = module_is_locked(t->mod_name);
			t->cipher_old = get_cipher_key(t->mod_name);
		}

		else {

			t->is_locked = 0;
			t->cipher_old = NULL;
		}

		t->frame = NULL;
		add_vbox_to_notebook(t);
		text_list = g_list_append(text_list, (TEXT_DATA *) t);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(widgets.notebook_text),
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
		 * free any cipher keys 
		 */
		if (t->cipher_key)
			g_free(t->cipher_key);
		if (t->cipher_old)
			g_free(t->cipher_old);
		/* 
		 * free global options 
		 */
		g_free(t->tgs);
		/* 
		 * free each TEXT_DATA item created 
		 */
		g_free((TEXT_DATA *) text_list->data);
		text_list = g_list_next(text_list);
	}
	g_list_free(text_list);
}
