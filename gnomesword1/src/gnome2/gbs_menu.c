/*
 * GnomeSword Bible Study Tool
 * gbs_menu.c - menu for Bible texts windows
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

#include "gui/gbs.h"
#include "gui/gbs_dialog.h"
#include "gui/gbs_menu.h"
#include "gui/gnomesword.h"
#include "gui/gtkhtml_display.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/cipher_key_dialog.h"
#include "gui/html.h"
#include "gui/dialog.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/gbs.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"

/******************************************************************************
 * Name
 *  global_option_red_words
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_red_words(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_red_words(GtkMenuItem * menuitem,
				    GBS_DATA * g)
{
	g->bgo->words_in_red = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_strongs
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_strongs(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_strongs(GtkMenuItem * menuitem, GBS_DATA * g)
{
	g->bgo->strongs = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_morph
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_morph(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_morph(GtkMenuItem * menuitem, GBS_DATA * g)
{
	g->bgo->morphs = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_footnotes
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_footnotes(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_footnotes(GtkMenuItem * menuitem,
				    GBS_DATA * g)
{
	g->bgo->footnotes = GTK_CHECK_MENU_ITEM(menuitem)->active;
}

/******************************************************************************
 * Name
 *  global_option_greekaccents
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_greekaccents(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_greekaccents(GtkMenuItem * menuitem,
				       GBS_DATA * g)
{
	g->bgo->greekaccents = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_lemmas
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_lemmas(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_lemmas(GtkMenuItem * menuitem, GBS_DATA * g)
{
	g->bgo->lemmas = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_scripturerefs
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_scripturerefs(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_scripturerefs(GtkMenuItem * menuitem,
					GBS_DATA * g)
{
	g->bgo->scripturerefs = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_hebrewpoints
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_hebrewpoints(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_hebrewpoints(GtkMenuItem * menuitem,
				       GBS_DATA * g)
{
	g->bgo->hebrewpoints = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_hebrewcant
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_hebrewcant(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_hebrewcant(GtkMenuItem * menuitem,
				     GBS_DATA * g)
{
	g->bgo->hebrewcant = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_headings
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_headings(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_headings(GtkMenuItem * menuitem,
				   GBS_DATA * g)
{
	g->bgo->headings = GTK_CHECK_MENU_ITEM(menuitem)->active;
}



/******************************************************************************
 * Name
 *  global_option_variants_all
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_variants_all(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_variants_all(GtkMenuItem * menuitem,
				       GBS_DATA * g)
{
	g->bgo->variants_all = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (g->bgo->variants_all) {

	}
}


/******************************************************************************
 * Name
 *  global_option_variants_primary
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_variants_primary(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_variants_primary(GtkMenuItem * menuitem,
					   GBS_DATA * g)
{
	//gchar *text_str;

	g->bgo->variants_primary =
	    GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (g->bgo->variants_primary) {
		/*
		   text_str = get_commentary_text(c->mod_name, c->key);
		   entry_display(c->html, c->mod_name, text_str, c->key, TRUE);
		   free(text_str);
		 */
	}
}


/******************************************************************************
 * Name
 *    global_option_variants_scondary
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_variants_scondary(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_variants_scondary(GtkMenuItem * menuitem,
					    GBS_DATA * g)
{
	g->bgo->variants_secondary =
	    GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (g->bgo->variants_secondary) {

	}
}




/******************************************************************************
 * Name
 *    add_global_option_items
 *
 * Synopsis
 *   #include "gui/gbs_menu.h"
 *
 *   void add_global_option_items(GBS_DATA * c)
 *
 * Description
 *   add global module options to popup menus (book texts)
 *
 * Return value
 *   void
 */

static void add_global_option_items(GBS_DATA * g)
{
	GtkWidget *item;
	GtkWidget *menu;
	GSList *group = NULL;

	if (check_for_global_option(g->mod_name, "GBFRedLetterWords")) {
		g->bgo->words_in_red =
		    load_module_options(g->mod_name,
					"Words of Christ in Red");


		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Words of Christ in Red"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active =
		    g->bgo->words_in_red;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_red_words), g);
	}

	if ((check_for_global_option(g->mod_name, "GBFStrongs")) ||
	    (check_for_global_option(g->mod_name, "ThMLStrongs")) ||
	    (check_for_global_option(g->mod_name, "OSISStrongs"))) {
		g->bgo->strongs =
		    load_module_options(g->mod_name,
					"Strong's Numbers");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Strong's Numbers"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = g->bgo->strongs;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_strongs), g);
	}

	if ((check_for_global_option(g->mod_name, "GBFMorph")) ||
	    (check_for_global_option(g->mod_name, "ThMLMorph")) ||
	    (check_for_global_option(g->mod_name, "OSISSMorph"))) {
		g->bgo->morphs =
		    load_module_options(g->mod_name,
					"Morphological Tags");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Morphological Tags"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = g->bgo->morphs;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_morph), g);
	}

	if ((check_for_global_option(g->mod_name, "GBFFootnotes")) ||
	    (check_for_global_option(g->mod_name, "ThMLFootnotes"))) {
		g->bgo->footnotes =
		    load_module_options(g->mod_name, "Footnotes");

		item =
		    gtk_check_menu_item_new_with_label(_("Footnotes"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = g->bgo->footnotes;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_footnotes), g);
	}

	if (check_for_global_option(g->mod_name, "UTF8GreekAccents")) {
		g->bgo->greekaccents =
		    load_module_options(g->mod_name, "Greek Accents");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Greek Accents"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active =
		    g->bgo->greekaccents;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_greekaccents), g);
	}

	if (check_for_global_option(g->mod_name, "ThMLLemma")) {
		g->bgo->lemmas =
		    load_module_options(g->mod_name, "Lemmas");

		item = gtk_check_menu_item_new_with_label(_("Lemmas"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = g->bgo->lemmas;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_lemmas), g);
	}

	if (check_for_global_option(g->mod_name, "ThMLScripref")) {
		g->bgo->scripturerefs =
		    load_module_options(g->mod_name,
					"Scripture Cross-references");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Scripture Cross-references"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active =
		    g->bgo->scripturerefs;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_scripturerefs), g);
	}

	if (check_for_global_option(g->mod_name, "UTF8HebrewPoints")) {
		g->bgo->hebrewpoints =
		    load_module_options(g->mod_name,
					"Hebrew Vowel Points");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Hebrew Vowel Points"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active =
		    g->bgo->hebrewpoints;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_hebrewpoints), g);
	}

	if (check_for_global_option(g->mod_name, "UTF8Cantillation")) {
		g->bgo->hebrewcant =
		    load_module_options(g->mod_name,
					"Hebrew Cantillation");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Hebrew Cantillation"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = g->bgo->hebrewcant;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_hebrewcant), g);
	}

	if (check_for_global_option(g->mod_name, "ThMLHeadings")) {
		g->bgo->headings =
		    load_module_options(g->mod_name, "Headings");

		item =
		    gtk_check_menu_item_new_with_label(_("Headings"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = g->bgo->headings;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_headings), g);
	}

	if (check_for_global_option(g->mod_name, "ThMLVariants")) {
		g->bgo->variants_all =
		    load_module_options(g->mod_name, "All Readings");
		g->bgo->variants_primary =
		    load_module_options(g->mod_name, "Primary Reading");
		g->bgo->variants_secondary =
		    load_module_options(g->mod_name,
					"Secondary Reading");
		item = gtk_menu_item_new_with_label(_("Variants"));
		gtk_container_add(GTK_CONTAINER(g->module_options_menu),
				  item);
		gtk_widget_show(item);

		menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

		item =
		    gtk_radio_menu_item_new_with_label(group,
						       _
						       ("All Readings"));
/*		group =
		    gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM
					      (item));*/
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		if (g->bgo->variants_all)
			gtk_check_menu_item_set_active
			    (GTK_CHECK_MENU_ITEM(item),
			     g->bgo->variants_all);
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_variants_all), g);

		item =
		    gtk_radio_menu_item_new_with_label(group,
						       _
						       ("Primary Readings"));
		/*group =
		    gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM
					      (item));*/
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		if (g->bgo->variants_primary)
			gtk_check_menu_item_set_active
			    (GTK_CHECK_MENU_ITEM(item),
			     g->bgo->variants_primary);
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_variants_primary), g);

		item =
		    gtk_radio_menu_item_new_with_label(group,
						       _
						       ("Secondary Readings"));
		/*group =
		    gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM
					      (item));*/
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		g_signal_connect(GTK_OBJECT(item), "toggled",
				   G_CALLBACK
				   (global_option_variants_scondary),
				   g);
		if (g->bgo->variants_secondary)
			gtk_check_menu_item_set_active
			    (GTK_CHECK_MENU_ITEM(item),
			     g->bgo->variants_secondary);
	}

}
/******************************************************************************
 * Name
 *  on_copy_activate
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
 *
 *   void on_copy_activate(GtkMenuItem * menuitem, GBS_DATA * t)	
 *
 * Description
 *   copy selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_copy_activate(GtkMenuItem * menuitem, GBS_DATA * t)
{
	gui_copy_html(t->html);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/bibletext_menu.h"
 *
 *   void (GtkMenuItem * menuitem, GBS_DATA * t)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_close_activate(GtkMenuItem * menuitem, GBS_DATA * dlg)
{
	gui_close_gbs_dialog(dlg);
}


/******************************************************************************
 * Name
 *  on_print_activate
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
 *
 *   void on_print_activate(GtkMenuItem * menuitem, GBS_DATA * t)	
 *
 * Description
 *   send contents of Bible pane (window) to the printer
 *
 * Return value
 *   void
 */

static void on_print_activate(GtkMenuItem * menuitem, GBS_DATA * t)
{
	gui_html_print(t->html, FALSE);
}


/******************************************************************************
 * Name
 *  on_find_activate
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
 *
 *  void on_find_activate(GtkMenuItem * menuitem, GBS_DATA * t)	
 *
 * Description
 *   opens find dialog
 *
 * Return value
 *   
 */

static void on_find_activate(GtkMenuItem * menuitem, GBS_DATA * t)
{
	gui_find_dlg(t->html, t->mod_name, FALSE, NULL);
}

/******************************************************************************
 * Name
 *  on_same_lookup_selection_activate				       
 *
 * Synopsis
 *  #include "gui/dictionary_menu.h"
 *
 *  void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
 *                                         GBS_DATA * t) 	
 *
 * Description
 *   lookup seledtion in current dict/lex module
 *
 * Return value
 *   void
 */

static void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
					      GBS_DATA * t)
{
	gchar *dict_key = gui_get_word_or_selection(t->html, FALSE);
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
 *  	on_lookup_dialog_selection_activate			       
 *
 * Synopsis
 *  #include "gui/gbs_menu.h"
 *
 *  void on_lookup_dialog_selection_activate(GtkMenuItem * menuitem,
 *                                         GBS_DATA * t) 	
 *
 * Description
 *   lookup seledtion in current dict/lex module
 *
 * Return value
 *   void
 */

static void on_lookup_dialog_selection_activate(GtkMenuItem * menuitem,
					      gpointer user_data)
{
	
}



/******************************************************************************
 * Name
 *  	on_lookup_selection_activate			       
 *
 * Synopsis
 *  #include "gui/gbs_menu.h"
 *
 *  void on_lookup_selection_activate(GtkMenuItem * menuitem,
 *                                         GBS_DATA * t) 	
 *
 * Description
 *   lookup seledtion in current dict/lex module
 *
 * Return value
 *   void
 */

static void on_lookup_selection_activate(GtkMenuItem * menuitem,
					      gpointer user_data)
{
	
}

/******************************************************************************
 * Name
 *  on_view_mod_activate
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
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
	if(!module_name) 
		return;
	gui_change_module_and_key(module_name, NULL);
	if(module_name) g_free(module_name);
}


/******************************************************************************
 * Name
 *  on_showtabs_activate
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
 *
 *  void on_showtabs_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   display notebook tabs
 *
 * Return value
 *   void
 */

static void on_showtabs_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	settings.book_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_gbs),
				   settings.book_tabs);
	gui_set_gbs_frame_label();
}


/******************************************************************************
 * Name
 *  on_view_new_activate
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
 *
 *  void on_view_new_activate(GtkMenuItem * menuitem, GBS_DATA * t)	
 *
 * Description
 *   opens view text dialog
 *
 * Return value
 *   void
 */

static void on_view_new_activate(GtkMenuItem * menuitem, GBS_DATA * t)
{
	gui_open_gbs_dialog(t->mod_name);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
 *
 *  void (GtkMenuItem * menuitem, GBS_DATA * t)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_view_current_dict_activate(GtkMenuItem * menuitem, GBS_DATA * t)
{
	gtk_notebook_set_current_page(GTK_NOTEBOOK
				      (widgets.workbook_lower), 0);
}

/******************************************************************************
 * Name
 *  gui_unlock_dictionary
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
 *
 *  void gui_unlock_dictionary(GtkMenuItem * menuitem,GBS_DATA * t) 	
 *
 * Description
 *   opens add cipher key dialog
 *
 * Return value
 *   void
 */

void gui_unlock_gbs(GtkMenuItem * menuitem, GBS_DATA * t)
{
	gchar *cipher_key;

	cipher_key = gui_add_cipher_key(t->mod_name, t->cipher_old);
	if (cipher_key) {
		t->cipher_key = cipher_key;
		//cur_t = t;
		gui_module_is_locked_display(t->html,
					     t->mod_name,
					     t->cipher_key);
	}
}


/******************************************************************************
 * Name
 *  set_module_font_activate
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
 *
 *   void set_module_font_activate(GtkMenuItem * menuitem,
				     GBS_DATA * t)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void set_module_font_activate(GtkMenuItem * menuitem,
				     GBS_DATA * t)
{
	gui_set_module_font(t->mod_name);
}


/******************************************************************************
 * Name
 *  on_new_dialog_activate
 *
 * Synopsis
 *   #include "gui/bibletext_menu.h"
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
	gui_open_gbs_dialog(module_name);
	if(module_name) g_free(module_name);
}


/******************************************************************************
 * Name
 *  on_about_module_activate
 *
 * Synopsis
 *   #include "gui/gbs_menu.h"
 *
 *  void on_about_module_activate(GtkMenuItem * menuitem, 
						GBS_DATA * t)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_about_module_activate(GtkMenuItem * menuitem, GBS_DATA * t)
{
	gui_display_about_module_dialog(t->mod_name, FALSE);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/gbs_menu.h"
 *
 *  void on_sync_activate(GtkMenuItem * menuitem, 
						gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
/*
static void on_sync_activate(GtkMenuItem * menuitem, GBS_DATA * dlg)
{
	//gui_sync_bibletext_dialog_with_main();
}
*/

/******************************************************************************
 * Name
 *  gui_create_pm_dict
 *
 * Synopsis
 *   #include "gui/dictionary_menu.h"
 *
 *   GtkWidget *gui_create_pm_dict(GBS_DATA * t)
 *
 * Description
 *   create popup menu for dict/lex module - attached to html widget
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_pm_gbs(GBS_DATA * t)
{
	GtkWidget *pm_text;
	GtkAccelGroup *pm_text_accels;
	GtkWidget *about_module;
	GtkWidget *separator;
	GtkWidget *file;
	GtkWidget *file_menu;
	GtkWidget *current_dict;
	GtkWidget *print;
	GtkWidget *close;
	GtkWidget *show;
	GtkWidget *show_menu;
	GtkWidget *module_options;
	GtkWidget *copy;
	GtkWidget *edit;
	GtkWidget *edit_menu;
	GtkWidget *lookup_selection;
	GtkWidget *lookup_selection_menu;
	GtkAccelGroup *lookup_selection_menu_accels;
	GtkWidget *usecurrent;
	GtkWidget *view_new = NULL;
	GtkWidget *view_text;
	GtkWidget *view_text_menu = NULL;
	GtkAccelGroup *view_text_menu_accels;
	GtkWidget *set_font;
	GtkWidget *find;
	GtkWidget *add_module_key = NULL;
	gchar buf[256];
	GString *str;
	GtkTooltips *tooltips;

	sprintf(buf, "%s %s %s", _("Open"), t->mod_name,
		_("in a dialog"));

	tooltips = gtk_tooltips_new();

	pm_text = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pm_text), "pm_text", pm_text);
	
	str = g_string_new(NULL);
	g_string_printf(str,"%s %s",_("About"),t->mod_name);
	
	about_module =
	    gtk_menu_item_new_with_label(str->str);
	gtk_widget_show(about_module);
	gtk_container_add(GTK_CONTAINER(pm_text),
				  about_module);
	g_string_free(str,TRUE);
	
	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm_text), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	
	/*
	 * file menu
	 */
	file = gtk_menu_item_new_with_label(_("File"));
	gtk_widget_show(file);
	gtk_container_add(GTK_CONTAINER(pm_text), file);

	file_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu);

	if (t->is_dialog) {
		view_text =
		    gtk_menu_item_new_with_label(_("Open New Dialog"));
		gtk_widget_show(view_text);
		gtk_container_add(GTK_CONTAINER(file_menu), view_text);

		view_text_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_text),
					  view_text_menu);
					  
		gui_add_mods_2_gtk_menu(GBS_DESC_LIST, view_text_menu,
					(GCallback)
					on_new_dialog_activate);

	} else {
		current_dict =
		    gtk_menu_item_new_with_label(_("View Current Dict/Lex"));
		gtk_widget_show(current_dict);
		gtk_container_add(GTK_CONTAINER(file_menu), current_dict);
		if(!settings.havedict)
			gtk_widget_set_sensitive(current_dict, FALSE);
		view_text =
		    gtk_menu_item_new_with_label(_("Open Module"));
		gtk_widget_show(view_text);
		gtk_container_add(GTK_CONTAINER(file_menu), view_text);

		view_text_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_text),
					  view_text_menu);
		
		gui_add_mods_2_gtk_menu(GBS_DESC_LIST, view_text_menu,
					(GCallback)
					on_view_mod_activate);

		view_new = gtk_menu_item_new_with_label(buf);
		gtk_widget_show(view_new);
		gtk_container_add(GTK_CONTAINER(file_menu), view_new);

		g_signal_connect(GTK_OBJECT(current_dict), "activate",
				   G_CALLBACK
				   (on_view_current_dict_activate), t);
		g_signal_connect(GTK_OBJECT(view_new), "activate",
				   G_CALLBACK
				   (on_view_new_activate), t);

	}

	print = gtk_menu_item_new_with_label(_("Print"));
	gtk_widget_show(print);
	gtk_container_add(GTK_CONTAINER(file_menu), print);

	if (t->is_dialog) {
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
				   (on_close_activate), t);
	}

	/*
	 * edit menu
	 */
	edit = gtk_menu_item_new_with_label(_("Edit"));
	gtk_widget_show(edit);
	gtk_container_add(GTK_CONTAINER(pm_text), edit);

	edit_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit), edit_menu);

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(edit_menu), copy);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(edit_menu), find);

	/*
	 * module options menu
	 */
	module_options =
	    gtk_menu_item_new_with_label(_("Module Options"));
	gtk_widget_show(module_options);
	gtk_container_add(GTK_CONTAINER(pm_text), module_options);

	t->module_options_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_options),
				  t->module_options_menu);

	set_font = gtk_menu_item_new_with_label(_("Set Module Font"));
	gtk_widget_show(set_font);
	gtk_container_add(GTK_CONTAINER(t->module_options_menu),
			  set_font);
	gtk_tooltips_set_tip(tooltips, set_font,
			     _("Set font for this module"), NULL);
			     

	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(t->module_options_menu),
			  separator);
	gtk_widget_set_sensitive(separator, FALSE);

	add_global_option_items(t);

/*
	add_global_option_items(t);
*/
	/*
	 * lookup menu
	 */
	lookup_selection =
	    gtk_menu_item_new_with_label(_("Lookup Selection"));
	gtk_widget_show(lookup_selection);
	gtk_container_add(GTK_CONTAINER(pm_text), lookup_selection);

	lookup_selection_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lookup_selection),
				  lookup_selection_menu);

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

	if (t->is_dialog)
		gui_add_mods_2_gtk_menu(DICT_DESC_LIST,
			lookup_selection_menu,
			(GCallback)
			on_lookup_dialog_selection_activate);
	else
		gui_add_mods_2_gtk_menu(DICT_DESC_LIST,
			lookup_selection_menu,
			(GCallback)
			on_lookup_selection_activate);

	/*
	 * if module has cipher key include this item
	 */
	if (t->cipher_old || t->is_locked) {
		separator = gtk_menu_item_new();
		gtk_widget_show(separator);
		gtk_container_add(GTK_CONTAINER(pm_text), separator);
		gtk_widget_set_sensitive(separator, FALSE);

		add_module_key =
		    gtk_menu_item_new_with_label(_
						 ("Unlock This Module"));
		gtk_widget_show(add_module_key);
		gtk_container_add(GTK_CONTAINER(pm_text),
				  add_module_key);

		g_signal_connect(GTK_OBJECT(add_module_key),
				   "activate",
				   G_CALLBACK
				   (gui_unlock_gbs), t);

	}

	/*
	 * show menu
	 */
	if (!t->is_dialog) {
		show = gtk_menu_item_new_with_label(_("Show"));
		gtk_widget_show(show);
		gtk_container_add(GTK_CONTAINER(pm_text), show);

		show_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(show),
					  show_menu);
		t->showtabs =
		    gtk_check_menu_item_new_with_label(_("Tabs"));
		gtk_widget_show(t->showtabs);
		gtk_container_add(GTK_CONTAINER(show_menu),
				  t->showtabs);
		g_signal_connect(GTK_OBJECT(t->showtabs), "activate",
				   G_CALLBACK
				   (on_showtabs_activate), 
				   NULL);
	}

	/*
	 * for using the current dictionary for lookup 
	 */
	g_signal_connect(GTK_OBJECT(usecurrent), "activate",
			   G_CALLBACK
			   (on_same_lookup_selection_activate), t);
	g_signal_connect(GTK_OBJECT(about_module), "activate",
			   G_CALLBACK
			   (on_about_module_activate), t);

	g_signal_connect(GTK_OBJECT(copy), "activate",
			   G_CALLBACK(on_copy_activate), t);
	g_signal_connect(GTK_OBJECT(print), "activate",
			   G_CALLBACK(on_print_activate), t);
	g_signal_connect(GTK_OBJECT(find), "activate",
			   G_CALLBACK(on_find_activate), t);

	g_signal_connect(GTK_OBJECT(set_font), "activate",
			   G_CALLBACK(set_module_font_activate),
			   t);

	gtk_object_set_data(GTK_OBJECT(pm_text), "tooltips", tooltips);
	return pm_text;
}
