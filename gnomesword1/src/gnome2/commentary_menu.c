/*
 * GnomeSword Bible Study Tool
 * commentary_menu.c - menu for commentary windows
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#include "gui/commentary.h"
#include "gui/commentary_dialog.h"
#include "gui/commentary_menu.h"
#include "gui/gnomesword.h"
#include "gui/gtkhtml_display.h"
#include "gui/cipher_key_dialog.h"
#include "gui/html.h"
#include "gui/dialog.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/main_window.h"
#include "gui/percomm.h"
#include "gui/dictlex.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/commentary.h"
#include "main/bibletext.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"


/******************************************************************************
 * Name
 *  on_copy_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void on_copy_activate(GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   copy selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_copy_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gui_copy_html(c->html);
}

/******************************************************************************
 * Name
 *  on_print_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void on_print_activate(GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   send contents of Bible pane (window) to the printer
 *
 * Return value
 *   void
 */

static void on_print_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gui_html_print(c->html, FALSE);
}


/******************************************************************************
 * Name
 *  on_find_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void on_find_activate(GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   opens find dialog
 *
 * Return value
 *   
 */

static void on_find_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gui_find_dlg(c->html, c->mod_name, FALSE, NULL);
}

/******************************************************************************
 * Name
 *  on_lookup_selection_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
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
/*	gchar *dict_key = NULL;
	gchar *mod_name = NULL;

	mod_name = module_name_from_description(dict_mod_description);
	if(!mod_name) 
		return;

	dict_key = gui_get_word_or_selection(cur_c->html, FALSE);
	if (dict_key && mod_name) {
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(mod_name,
						      dict_key);
		if (settings.inDictpane)
			gui_change_module_and_key(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
	}*/
}

/******************************************************************************
 * Name
 *  on_same_lookup_selection_activate				       
 *
 * Synopsis
 *  #include "gui/commentary_menu.h"
 *
 *  void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
 *                                         COMM_DATA * c) 	
 *
 * Description
 *   lookup seledtion in current dict/lex module
 *
 * Return value
 *   void
 */

static void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
					      COMM_DATA * c)
{
	gchar *dict_key = gui_get_word_or_selection(c->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(settings.
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
 *   #include "gui/commentary_menu.h"
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
	gchar *module_name = NULL;

	module_name = module_name_from_description((gchar *) user_data);
	if(module_name) {
		gui_change_module_and_key(module_name, settings.currentverse);
		g_free(module_name);
	}
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
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
	if (settings.use_percomm_dialog)
		gui_open_commentary_editor((gchar *) user_data);
/*	else {
		gtk_notebook_set_current_page(GTK_NOTEBOOK
				      (widgets.workbook_lower),
				      settings.percomm_page);
		gui_set_percomm_page((gchar *) user_data);
	}
*/
}


/******************************************************************************
 * Name
 *  on_view_new_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void on_view_new_activate(GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   opens view text dialog
 *
 * Return value
 *   void
 */

static void on_view_new_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gui_open_commentary_dialog(c->mod_name);
}

/******************************************************************************
 * Name
 *   gui_unlock_commentary 
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void gui_unlock_commentary(GtkMenuItem *menuitem, COMM_DATA *c)
 *
 * Description
 *   open cipher dialog to add cipher key to unlock module
 *
 * Return value
 *   void
 */

void gui_unlock_commentary(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gchar *cipher_key;

	cipher_key = gui_add_cipher_key(c->mod_name, c->cipher_old);
	if (cipher_key) {
		c->cipher_key = cipher_key;
		gui_module_is_locked_display(c->html,
					     c->mod_name,
					     c->cipher_key);
	}
}


/******************************************************************************
 * Name
 *  set_module_font_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void set_module_font_activate(GtkMenuItem * menuitem,
				     COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void set_module_font_activate(GtkMenuItem * menuitem,
				     COMM_DATA * c)
{
	gui_set_module_font(c->mod_name);

	if (c->dialog)
		gui_display_commentary_in_dialog(c->key);
}


/******************************************************************************
 * Name
 *  on_comm_showtabs_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void on_comm_showtabs_activate(GtkMenuItem * menuitem,
 *				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_comm_showtabs_activate(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	settings.comm_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
/*	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_comm),
				   settings.comm_tabs);*/
//	gui_set_comm_frame_label(c);
}

/******************************************************************************
 * Name
 *  on_sync_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void on_sync_activate(GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_sync_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	if (c->is_dialog)
		gui_display_commentary_in_dialog(settings.currentverse);
/*	else
		gui_display_commentary_with_struct(c,
						   settings.
						   currentverse);*/
}


/******************************************************************************
 * Name
 *  on_sync_toggle
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void on_sync_toggle(GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_sync_toggle(GtkCheckMenuItem * menuitem, COMM_DATA * c)
{
/*	c->sync = menuitem->active;
	if (c->sync)
		gui_display_commentary_with_struct(c,
						   settings.
						   currentverse);*/
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void (GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_prev_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gchar *key;

	main_set_commentary_key(c->mod_name, c->key);
	key = navigate_commentary(c->mod_name, 0);
	if (key) {
		if (c->is_dialog)
			gui_display_commentary_in_dialog(key);
/*		else
			gui_display_commentary_with_struct(c, key);*/
		free(key);
	}
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void (GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_next_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gchar *key;

	main_set_commentary_key(c->mod_name, c->key);
	key = navigate_commentary(c->mod_name, 1);
	if (key) {
		if (c->is_dialog)
			gui_display_commentary_in_dialog(key);
/*		else
			gui_display_commentary_with_struct(c, key);*/
		free(key);
	}
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void (GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_chapter_heading_activate(GtkMenuItem * menuitem,
					COMM_DATA * c)
{
	c->chapter_heading = TRUE;
	if (c->is_dialog)
		gui_display_commentary_in_dialog(c->key);
/*	else
		gui_display_commentary_with_struct(c,
						   settings.
						   currentverse);*/
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void (GtkMenuItem * menuitem, COMM_DATA * c)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_book_heading_activate(GtkMenuItem * menuitem,
				     COMM_DATA * c)
{
	c->book_heading = TRUE;
//	gui_display_commentary_with_struct(c, settings.currentverse);
}


/******************************************************************************
 * Name
 *  on_new_dialog_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void on_new_dialog_activate(GtkMenuItem * menuitem, 
						gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_new_dialog_activate(GtkMenuItem * menuitem,
				   gpointer user_data)
{
	gchar *module_name = NULL;

	module_name = module_name_from_description((gchar *) user_data);
	if(!module_name) 
		return;
	gui_open_commentary_dialog(module_name);
	if(module_name) g_free(module_name);
}



/******************************************************************************
 * Name
 *  on_close_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void on_close_activate(GtkMenuItem * menuitem, TEXT_DATA * t)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_close_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gui_close_comm_dialog(c);
}


/******************************************************************************
 * Name
 *  on_about_module_activate
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *  void on_about_module_activate(GtkMenuItem * menuitem, 
						TEXT_DATA * vt)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_about_module_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gui_display_about_module_dialog(c->mod_name, FALSE);
}

/******************************************************************************
 * Name
 *  gui_create_pm_text
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   GtkWidget *gui_create_pm_text(COMM_DATA * c)
 *
 * Description
 *   create popup menu for text module - attached to html widget
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_pm_comm(COMM_DATA * c)
{
	GtkWidget *pm;
	GtkAccelGroup *pm_accels;
	GtkWidget *about_module;
	GtkWidget *copy;
	GtkWidget *separator;
	GtkWidget *file;
	GtkWidget *file_menu;
	GtkWidget *navigate;
	GtkWidget *navigate_menu;
	GtkWidget *sync;
	GtkWidget *prev;
	GtkWidget *next;
	GtkWidget *chapter_heading;
	GtkWidget *book_heading;
	GtkWidget *print;
	GtkWidget *close;
	GtkWidget *show;
	GtkWidget *show_menu;
	GtkWidget *module_options;
	GtkWidget *edit;
	GtkWidget *edit_menu;
	GtkWidget *edit_note;
	GtkWidget *lookup_selection;
	GtkWidget *lookup_selection_menu;
	GtkAccelGroup *lookup_selection_menu_accels;
	GtkWidget *usecurrent;
	GtkWidget *view_new = NULL;
	GtkWidget *view_comm;
	GtkWidget *view_comm_menu;
	GtkAccelGroup *view_comm_menu_accels;
	GtkWidget *set_font;
	GtkWidget *find;
	GtkWidget *add_module_key = NULL;
	gchar buf[256];
	GString *str;
	GtkTooltips *tooltips;

	sprintf(buf, "%s %s %s", _("Open"), c->mod_name,
		_("in a dialog"));

	tooltips = gtk_tooltips_new();

	pm = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pm), "pm", pm);
/*	pm_accels = gtk_menu_ensure_uline_accel_group(GTK_MENU(pm));*/
	str = g_string_new(NULL);
	g_string_printf(str,"%s %s",_("About"),c->mod_name);
	
	about_module =
	    gtk_menu_item_new_with_label(str->str);
	gtk_widget_show(about_module);
	gtk_container_add(GTK_CONTAINER(pm),
				  about_module);
	g_string_free(str,TRUE);
	
	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm), separator);
	gtk_widget_set_sensitive(separator, FALSE);
	/*
	 * file menu
	 */
	file = gtk_menu_item_new_with_label(_("File"));
	gtk_widget_show(file);
	gtk_container_add(GTK_CONTAINER(pm), file);

	file_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu);
	/*
	 * navigate menu
	 */
	navigate = gtk_menu_item_new_with_label(_("Navigate"));
	gtk_widget_show(navigate);
	gtk_container_add(GTK_CONTAINER(file_menu), navigate);

	navigate_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(navigate),
				  navigate_menu);

	if (c->is_dialog) {
		sync =
		    gtk_menu_item_new_with_label(_
						 ("Sync with Main Window"));
		g_signal_connect(GTK_OBJECT(sync), "activate",
				   G_CALLBACK(on_sync_activate),
				   c);
	} else {
		sync =
		    gtk_check_menu_item_new_with_label(_
						       ("Sync with NavBar"));
		GTK_CHECK_MENU_ITEM(sync)->active = c->sync;
		g_signal_connect(GTK_OBJECT(sync), "toggled",
				   G_CALLBACK(on_sync_toggle), c);
	}
	gtk_widget_show(sync);
	gtk_container_add(GTK_CONTAINER(navigate_menu), sync);

	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(navigate_menu), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	prev = gtk_menu_item_new_with_label(_("Previous Comment"));
	gtk_widget_show(prev);
	gtk_container_add(GTK_CONTAINER(navigate_menu), prev);

	next = gtk_menu_item_new_with_label(_("Next Comment"));
	gtk_widget_show(next);
	gtk_container_add(GTK_CONTAINER(navigate_menu), next);

	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(navigate_menu), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	chapter_heading =
	    gtk_menu_item_new_with_label(_("Chapter Heading"));
	gtk_widget_show(chapter_heading);
	gtk_container_add(GTK_CONTAINER(navigate_menu),
			  chapter_heading);

	book_heading = gtk_menu_item_new_with_label(_("Book Heading"));
	gtk_widget_show(book_heading);
	gtk_container_add(GTK_CONTAINER(navigate_menu), book_heading);

	if (c->is_dialog) {
		view_comm =
		    gtk_menu_item_new_with_label(_("Open New Dialog"));
		gtk_widget_show(view_comm);
		gtk_container_add(GTK_CONTAINER(file_menu), view_comm);

		view_comm_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_comm),
					  view_comm_menu);
	/*	view_comm_menu_accels =
		    gtk_menu_ensure_uline_accel_group(GTK_MENU
						      (view_comm_menu));*/
		gui_add_mods_2_gtk_menu(COMM_DESC_LIST, view_comm_menu,
					(GCallback)
					on_new_dialog_activate);
	} else {

		view_comm =
		    gtk_menu_item_new_with_label(_("Open Module"));
		gtk_widget_show(view_comm);
		gtk_container_add(GTK_CONTAINER(file_menu), view_comm);

		view_comm_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_comm),
					  view_comm_menu);
/*		view_comm_menu_accels =
		    gtk_menu_ensure_uline_accel_group(GTK_MENU
						      (view_comm_menu));*/
		gui_add_mods_2_gtk_menu(COMM_DESC_LIST, view_comm_menu,
					(GCallback)
					on_view_mod_activate);

		view_new = gtk_menu_item_new_with_label(buf);
		gtk_widget_show(view_new);
		gtk_container_add(GTK_CONTAINER(file_menu), view_new);
		g_signal_connect(GTK_OBJECT(view_new), "activate",
				   G_CALLBACK
				   (on_view_new_activate), c);
	}
	print = gtk_menu_item_new_with_label(_("Print"));
	gtk_widget_show(print);
	gtk_container_add(GTK_CONTAINER(file_menu), print);

	if (c->is_dialog) {
		separator = gtk_menu_item_new();
		gtk_widget_show(separator);
		gtk_container_add(GTK_CONTAINER(file_menu), separator);
		gtk_widget_set_sensitive(separator, FALSE);

		close = gtk_menu_item_new_with_label(_("Close"));
		gtk_widget_show(close);
		gtk_container_add(GTK_CONTAINER(file_menu), close);
		g_signal_connect(GTK_OBJECT(close),
				   "activate",
				   G_CALLBACK
				   (on_close_activate), c);
	}

	/*
	 * edit menu
	 */
	edit = gtk_menu_item_new_with_label(_("Edit"));
	gtk_widget_show(edit);
	gtk_container_add(GTK_CONTAINER(pm), edit);

	edit_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit), edit_menu);

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(edit_menu), copy);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(edit_menu), find);

	if (c->is_percomm) {
		edit_note = gtk_menu_item_new_with_label(c->mod_name);
		gtk_widget_show(edit_note);
		gtk_container_add(GTK_CONTAINER(edit_menu), edit_note);

		g_signal_connect(GTK_OBJECT(edit_note),
				   "activate",
				   G_CALLBACK
				   (edit_percomm), c->mod_name);
	}

	/*
	 * module options menu
	 */
	module_options =
	    gtk_menu_item_new_with_label(_("Module Options"));
	gtk_widget_show(module_options);
	gtk_container_add(GTK_CONTAINER(pm), module_options);

	c->module_options_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_options),
				  c->module_options_menu);

	set_font = gtk_menu_item_new_with_label(_("Set Module Font"));
	gtk_widget_show(set_font);
	gtk_container_add(GTK_CONTAINER(c->module_options_menu),
			  set_font);
	gtk_tooltips_set_tip(tooltips, set_font,
			     _("Set font for this module"), NULL);

	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(c->module_options_menu),
			  separator);
	gtk_widget_set_sensitive(separator, FALSE);

/*	gui_add_global_option_items(c->mod_name, 
				c->module_options_menu, 
				1,
				c->ops);
	//add_global_option_items(c);
*/

	/*
	 * lookup menu
	 */
	lookup_selection =
	    gtk_menu_item_new_with_label(_("Lookup Selection"));
	gtk_widget_show(lookup_selection);
	gtk_container_add(GTK_CONTAINER(pm), lookup_selection);

	lookup_selection_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lookup_selection),
				  lookup_selection_menu);
/*	lookup_selection_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (lookup_selection_menu));*/

	usecurrent =
	    gtk_menu_item_new_with_label(_("Use Current Dictionary"));
	gtk_widget_show(usecurrent);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent);

	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  separator);
	gtk_widget_set_sensitive(separator, FALSE);

	if (c->is_dialog)
		gui_add_mods_2_gtk_menu(DICT_DESC_LIST,
					lookup_selection_menu,
					(GCallback)
					gui_on_lookup_commentary_dialog_selection);
	else
		gui_add_mods_2_gtk_menu(DICT_DESC_LIST,
					lookup_selection_menu,
					(GCallback)
					on_lookup_selection_activate);

	/*
	 * if module has cipher key include this item
	 */
	if (c->cipher_old || c->is_locked) {
		separator = gtk_menu_item_new();
		gtk_widget_show(separator);
		gtk_container_add(GTK_CONTAINER(pm), separator);
		gtk_widget_set_sensitive(separator, FALSE);

		add_module_key =
		    gtk_menu_item_new_with_label(_("Unlock This Module"));
		gtk_widget_show(add_module_key);
		gtk_container_add(GTK_CONTAINER(pm), add_module_key);

		g_signal_connect(GTK_OBJECT(add_module_key),
				   "activate",
				   G_CALLBACK
				   (gui_unlock_commentary), c);
	}

	/*
	 * show menu
	 */
	if (!c->is_dialog) {
		show = gtk_menu_item_new_with_label(_("Show"));
		gtk_widget_show(show);
		gtk_container_add(GTK_CONTAINER(pm), show);

		show_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(show),
					  show_menu);
		c->showtabs =
		    gtk_check_menu_item_new_with_label(_("Tabs"));
		gtk_widget_show(c->showtabs);
		gtk_container_add(GTK_CONTAINER(show_menu),
				  c->showtabs);
		g_signal_connect(GTK_OBJECT(c->showtabs), "activate",
				   G_CALLBACK
				   (on_comm_showtabs_activate), c);
	}
	
	g_signal_connect(GTK_OBJECT(about_module), "activate",
			   G_CALLBACK(on_about_module_activate), c);

	g_signal_connect(GTK_OBJECT(prev), "activate",
			   G_CALLBACK(on_prev_activate), c);
	g_signal_connect(GTK_OBJECT(next), "activate",
			   G_CALLBACK(on_next_activate), c);
	g_signal_connect(GTK_OBJECT(chapter_heading), "activate",
			   G_CALLBACK
			   (on_chapter_heading_activate), c);
	g_signal_connect(GTK_OBJECT(book_heading), "activate",
			   G_CALLBACK
			   (on_book_heading_activate), c);
	/*
	 * for using the current dictionary for lookup 
	 */
	g_signal_connect(GTK_OBJECT(usecurrent), "activate",
			   G_CALLBACK
			   (on_same_lookup_selection_activate), c);

	g_signal_connect(GTK_OBJECT(copy), "activate",
			   G_CALLBACK(on_copy_activate), c);
	g_signal_connect(GTK_OBJECT(print), "activate",
			   G_CALLBACK(on_print_activate), c);
	g_signal_connect(GTK_OBJECT(find), "activate",
			   G_CALLBACK(on_find_activate), c);
	g_signal_connect(GTK_OBJECT(set_font), "activate",
			   G_CALLBACK(set_module_font_activate),
			   c);

	gtk_object_set_data(GTK_OBJECT(pm), "tooltips", tooltips);
	return pm;
}
