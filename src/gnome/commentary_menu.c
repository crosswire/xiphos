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
#include "gui/shortcutbar_viewer.h"
#include "gui/utilities.h"

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
	gui_html_print(c->html);
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
	gchar *dict_key, mod_name[16];

	memset(mod_name, 0, 16);
	module_name_from_description(mod_name, dict_mod_description);

	dict_key = gui_get_word_or_selection(cur_c->html, FALSE);
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
	gchar module_name[16];
	
	memset(module_name, 0, 16);
	module_name_from_description(module_name, (gchar *) user_data);
	gui_change_module_and_key(module_name, 
				settings.currentverse);
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

void gui_unlock_commentary(GtkMenuItem *menuitem, COMM_DATA *c)
{
	gchar *cipher_key;
	
//	g_warning(c->cipher_old);
	
	cipher_key = gui_add_cipher_key(c->mod_name, c->cipher_old);
	if(cipher_key){
		c->cipher_key = cipher_key;
		//cur_c = c;
		gui_module_is_locked_display(c->html, 
				c->mod_name, c->cipher_key);		
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
}


/******************************************************************************
 * Name
 *  global_option_red_words
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_red_words(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_red_words(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->words_in_red = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_strongs
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_strongs(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_strongs(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->strongs = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_morph
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_morph(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_morph(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->morphs = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_footnotes
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_footnotes(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_footnotes(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->footnotes = GTK_CHECK_MENU_ITEM(menuitem)->active;
}

/******************************************************************************
 * Name
 *  global_option_greekaccents
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_greekaccents(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_greekaccents(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->greekaccents = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_lemmas
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_lemmas(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_lemmas(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->lemmas = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_scripturerefs
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_scripturerefs(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_scripturerefs(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->scripturerefs = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_hebrewpoints
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_hebrewpoints(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_hebrewpoints(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->hebrewpoints = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_hebrewcant
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_hebrewcant(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_hebrewcant(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->hebrewcant = GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/******************************************************************************
 * Name
 *  global_option_headings
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_headings(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_headings(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->headings = GTK_CHECK_MENU_ITEM(menuitem)->active;
}



/******************************************************************************
 * Name
 *  global_option_variants_all
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void global_option_variants_all(GtkMenuItem * menuitem,
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_variants_all(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->variants_all = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if(c->cgs->variants_all){
		
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
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_variants_primary(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	//gchar *text_str;
	
	c->cgs->variants_primary = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if(c->cgs->variants_primary) {
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
				      COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_variants_scondary(GtkMenuItem * menuitem,
				      COMM_DATA * c)
{
	c->cgs->variants_secondary = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if(c->cgs->variants_secondary){
		
	}
}


/******************************************************************************
 * Name
 *    add_global_option_items
 *
 * Synopsis
 *   #include "gui/commentary_menu.h"
 *
 *   void add_global_option_items(COMM_DATA * c)
 *
 * Description
 *   add global module options to popup menus (Bible texts)
 *
 * Return value
 *   void
 */

static void add_global_option_items(COMM_DATA * c)
{
	GtkWidget * item;
	GtkWidget * menu;
	GSList *group = NULL;	
	
	if(check_for_global_option(c->mod_name, "GBFRedLetterWords")) {
		c->cgs->words_in_red = 
			load_module_options(c->mod_name, "Red letter words");
		
	
		item =
		    gtk_check_menu_item_new_with_label(_("Red letter words"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->words_in_red;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_red_words),
				   c);
	}
	
	if((check_for_global_option(c->mod_name, "GBFStrongs")) ||
		(check_for_global_option(c->mod_name, "ThMLStrongs")) ||
		(check_for_global_option(c->mod_name, "OSISStrongs"))) {
		c->cgs->strongs = 
			load_module_options(c->mod_name, "Strong's Numbers");
		
		item =
		    gtk_check_menu_item_new_with_label(_("Strong's Numbers"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->strongs;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_strongs),
				   c);
	}
	
	if((check_for_global_option(c->mod_name, "GBFMorph")) ||
		(check_for_global_option(c->mod_name, "ThMLMorph")) ||
		(check_for_global_option(c->mod_name, "OSISSMorph"))) {
		c->cgs->morphs = 
			load_module_options(c->mod_name, "Morphological Tags");
		
		item =
		    gtk_check_menu_item_new_with_label(_("Morphological Tags"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->morphs;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_morph),
				   c);
	}
	
	if((check_for_global_option(c->mod_name, "GBFFootnotes")) ||
		(check_for_global_option(c->mod_name, "ThMLFootnotes"))) {
		c->cgs->footnotes = 
			load_module_options(c->mod_name, "Footnotes");
		
		item =
		    gtk_check_menu_item_new_with_label(_("Footnotes"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->footnotes;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_footnotes),
				   c);
	}
	
	if(check_for_global_option(c->mod_name, "UTF8GreekAccents")) {
		c->cgs->greekaccents = 
			load_module_options(c->mod_name, "Greek Accents");
		
		item =
		    gtk_check_menu_item_new_with_label(_("Greek Accents"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->greekaccents;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_greekaccents),
				   c);
	}
	
	if(check_for_global_option(c->mod_name, "ThMLLemma")) {
		c->cgs->lemmas = 
			load_module_options(c->mod_name, "Lemmas");
		
		item =
		    gtk_check_menu_item_new_with_label(_("Lemmas"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->lemmas;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_lemmas),
				   c);
	}
	
	if(check_for_global_option(c->mod_name, "ThMLScripref")) {
		c->cgs->scripturerefs = 
			load_module_options(c->mod_name, "Scripture Cross-references");
		
		item =
		    gtk_check_menu_item_new_with_label(_("Scripture Cross-references"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->scripturerefs;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_scripturerefs),
				   c);
	}
	
	if(check_for_global_option(c->mod_name, "UTF8HebrewPoints")) {
		c->cgs->hebrewpoints = 
			load_module_options(c->mod_name, "Hebrew Vowel Points");
		
		item =
		    gtk_check_menu_item_new_with_label(_("Hebrew Vowel Points"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->hebrewpoints;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_hebrewpoints),
				   c);
	}
	
	if(check_for_global_option(c->mod_name, "UTF8Cantillation")) {
		c->cgs->hebrewcant = 
			load_module_options(c->mod_name, "Hebrew Cantillation");
		
		item =
		    gtk_check_menu_item_new_with_label(_("Hebrew Cantillation"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->hebrewcant;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_hebrewcant),
				   c);
	}
	
	if(check_for_global_option(c->mod_name, "ThMLHeadings")) {
		c->cgs->headings = 
			load_module_options(c->mod_name, "Headings");
		
		item =
		    gtk_check_menu_item_new_with_label(_("Headings"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
			
		GTK_CHECK_MENU_ITEM(item)->active = c->cgs->headings;
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_headings),
				   c);
	}
	
	if(check_for_global_option(c->mod_name, "ThMLVariants")) {
		c->cgs->variants_all = 
			load_module_options(c->mod_name, "All Readings");
		c->cgs->variants_primary = 
			load_module_options(c->mod_name, "Primary Reading");
		c->cgs->variants_secondary = 
			load_module_options(c->mod_name, "Secondary Reading");
		item =
		    gtk_menu_item_new_with_label(_("Variants"));
		gtk_container_add(GTK_CONTAINER(c->module_options_menu), item);
		gtk_widget_show(item);
		
		menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(item),
				  menu);
		
		item = gtk_radio_menu_item_new_with_label (group, _("All Readings"));
  		group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (item));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		if(c->cgs->variants_all)		   
			gtk_check_menu_item_set_active (
				GTK_CHECK_MENU_ITEM (item), 
					c->cgs->variants_all); 
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_variants_all),
				   c);
				   
		item = gtk_radio_menu_item_new_with_label (group, _("Primary Readings"));
  		group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (item));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		if(c->cgs->variants_primary)		   
			gtk_check_menu_item_set_active (
				GTK_CHECK_MENU_ITEM (item), 
					c->cgs->variants_primary);  
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_variants_primary),
				   c);
		
		item = gtk_radio_menu_item_new_with_label (group, _("Secondary Readings"));
  		group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (item));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		gtk_signal_connect(GTK_OBJECT(item), "toggled",
				   GTK_SIGNAL_FUNC
				   (global_option_variants_scondary),
				   c);	
		if(c->cgs->variants_secondary)		   
			gtk_check_menu_item_set_active (
				GTK_CHECK_MENU_ITEM (item), 
					c->cgs->variants_secondary);   
	}
	
}

static void on_comm_showtabs_activate(GtkMenuItem * menuitem,
						COMM_DATA *c)
{
	settings.comm_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_comm),
				   settings.comm_tabs);
	gui_set_comm_frame_label();
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

static void on_sync_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	if(c->is_dialog)
		gui_display_commentary_in_dialog(settings.currentverse);
	else
		gui_display_commentary_with_struct(c, settings.currentverse);
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
	
	set_commentary_key(c->mod_name, c->key);	
	key = navigate_commentary(c->mod_name, 0);
	if(key) {
		if(c->is_dialog)
			gui_display_commentary_in_dialog(key);
		else
			gui_display_commentary_with_struct(c, key);
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
	
	set_commentary_key(c->mod_name, c->key);	
	key = navigate_commentary(c->mod_name, 1);
	if(key) {
		if(c->is_dialog)
			gui_display_commentary_in_dialog(key);
		else
			gui_display_commentary_with_struct(c, key);
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

static void on_chapter_heading_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	c->chapter_heading = TRUE;
	if(c->is_dialog)
		gui_display_commentary_in_dialog(c->key);
	else
		gui_display_commentary_with_struct(c, settings.currentverse);
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

static void on_book_heading_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	c->book_heading = TRUE;
	gui_display_commentary_with_struct(c, settings.currentverse);
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
	gchar module_name[16];
	
	memset(module_name, 0, 16);
	module_name_from_description(module_name, (gchar *) user_data);	
	gui_open_commentary_dialog(module_name);
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

static void on_close_activate(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	gui_close_comm_dialog();
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
	GtkTooltips *tooltips;

	sprintf(buf,"%s %s %s",_("Open"), c->mod_name, _("in a dialog"));
	
	tooltips = gtk_tooltips_new();

	pm = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pm), "pm", pm);
	pm_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(pm));
	/*
	 * file menu
	 */
	file =
	    gtk_menu_item_new_with_label(_("File"));
	gtk_widget_show(file);
	gtk_container_add(GTK_CONTAINER(pm), file);

	file_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file),
				  file_menu);	    
	/*
	 * navigate menu
	 */
	navigate =
	    gtk_menu_item_new_with_label(_("Navigate"));
	gtk_widget_show(navigate);
	gtk_container_add(GTK_CONTAINER(file_menu), navigate);

	navigate_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(navigate),
				  navigate_menu); 
				  
	sync = gtk_menu_item_new_with_label(_("Sync with NavBar"));
	gtk_widget_show(sync);
	gtk_container_add(GTK_CONTAINER(navigate_menu), sync);
		
	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(navigate_menu), separator);
	gtk_widget_set_sensitive(separator, FALSE);				  		  
				  
	prev = gtk_menu_item_new_with_label(_("Prev Comment"));
	gtk_widget_show(prev);
	gtk_container_add(GTK_CONTAINER(navigate_menu), prev);
				  
	next = gtk_menu_item_new_with_label(_("Next Comment"));
	gtk_widget_show(next);
	gtk_container_add(GTK_CONTAINER(navigate_menu), next);
		
	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(navigate_menu), separator);
	gtk_widget_set_sensitive(separator, FALSE);				  
				  
	chapter_heading = gtk_menu_item_new_with_label(_("Chapter Heading"));
	gtk_widget_show(chapter_heading);
	gtk_container_add(GTK_CONTAINER(navigate_menu), chapter_heading);
				  
	book_heading = gtk_menu_item_new_with_label(_("Book Heading"));
	gtk_widget_show(book_heading);
	gtk_container_add(GTK_CONTAINER(navigate_menu), book_heading);
	
	if (c->is_dialog) {
		view_comm = gtk_menu_item_new_with_label(_("Open New Dialog"));
		gtk_widget_show(view_comm);
		gtk_container_add(GTK_CONTAINER(file_menu), view_comm);
	
		view_comm_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_comm),
					  view_comm_menu);
		view_comm_menu_accels =
		    gtk_menu_ensure_uline_accel_group(GTK_MENU(view_comm_menu));
		gui_add_mods_2_gtk_menu(COMM_DESC_LIST, view_comm_menu,
				(GtkMenuCallback) on_new_dialog_activate);
	}
	else {	
		
		view_comm = gtk_menu_item_new_with_label(_("Open Module"));		
		gtk_widget_show(view_comm);
		gtk_container_add(GTK_CONTAINER(file_menu), view_comm);
	
		view_comm_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_comm),
					  view_comm_menu);
		view_comm_menu_accels =
		    gtk_menu_ensure_uline_accel_group(GTK_MENU(view_comm_menu));
		gui_add_mods_2_gtk_menu(COMM_DESC_LIST, view_comm_menu,
				(GtkMenuCallback) on_view_mod_activate);
		
		view_new = gtk_menu_item_new_with_label(buf);
		gtk_widget_show(view_new);
		gtk_container_add(GTK_CONTAINER(file_menu), view_new);		
		gtk_signal_connect(GTK_OBJECT(view_new), "activate",
			   GTK_SIGNAL_FUNC(on_view_new_activate), c);
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
		gtk_signal_connect(GTK_OBJECT(close),
				   "activate",
				   GTK_SIGNAL_FUNC
				   (on_close_activate), c);
	}

	/*
	 * edit menu
	 */
	edit =
	    gtk_menu_item_new_with_label(_("Edit"));
	gtk_widget_show(edit);
	gtk_container_add(GTK_CONTAINER(pm), edit);

	edit_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit),
				  edit_menu);
	
	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(edit_menu), copy);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(edit_menu), find);

	if (c->is_percomm) {
		edit_note =
		    gtk_menu_item_new_with_label(c->mod_name);
		gtk_widget_show(edit_note);
		gtk_container_add(GTK_CONTAINER(edit_menu), edit_note);
		
		gtk_signal_connect(GTK_OBJECT(edit_note),
				   "activate",
				   GTK_SIGNAL_FUNC
				   (edit_percomm),
				   c->mod_name);
	}
	
	/*
	 * module options menu
	 */
	module_options = gtk_menu_item_new_with_label(_("Module Options"));
	gtk_widget_show(module_options);
	gtk_container_add(GTK_CONTAINER(pm), module_options);
	
	c->module_options_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_options),
				  c->module_options_menu);

	set_font = gtk_menu_item_new_with_label(_("Set Module Font"));
	gtk_widget_show(set_font);
	gtk_container_add(GTK_CONTAINER(c->module_options_menu), set_font);
	gtk_tooltips_set_tip(tooltips, set_font,
			     _("Set font for this module"), NULL);
			     
	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(c->module_options_menu), separator);
	gtk_widget_set_sensitive(separator, FALSE);
	
	add_global_option_items(c);
	
	
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
	lookup_selection_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (lookup_selection_menu));

	usecurrent =
	    gtk_menu_item_new_with_label("Use Current Dictionary");
	gtk_widget_show(usecurrent);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent);
			     
	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu), separator);
	gtk_widget_set_sensitive(separator, FALSE);
	
	if(c->is_dialog)
		gui_add_mods_2_gtk_menu(DICT_DESC_LIST, 
				lookup_selection_menu,
				(GtkMenuCallback) 
				gui_on_lookup_commentary_dialog_selection);
	else
		gui_add_mods_2_gtk_menu(DICT_DESC_LIST, 
				lookup_selection_menu,
				(GtkMenuCallback) 
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
		    gtk_menu_item_new_with_label(_
						 ("Unlock This Module"));
		gtk_widget_show(add_module_key);
		gtk_container_add(GTK_CONTAINER(pm),
				  add_module_key);

		gtk_signal_connect(GTK_OBJECT(add_module_key),
				   "activate",
				   GTK_SIGNAL_FUNC
				   (gui_unlock_commentary), c);
	}

	/*
	 * show menu
	 */	
	if(!c->is_dialog) {
		show =
		    gtk_menu_item_new_with_label(_("Show"));
		gtk_widget_show(show);
		gtk_container_add(GTK_CONTAINER(pm), show);
	
		show_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(show),
					  show_menu);
		c->showtabs =
		    gtk_check_menu_item_new_with_label(_("Tabs"));
		gtk_widget_show(c->showtabs);
		gtk_container_add(GTK_CONTAINER(show_menu), c->showtabs);
		gtk_signal_connect(GTK_OBJECT(c->showtabs), "activate",
			   GTK_SIGNAL_FUNC
			   (on_comm_showtabs_activate), NULL);
	}
	
	gtk_signal_connect(GTK_OBJECT(sync), "activate",
			   GTK_SIGNAL_FUNC
			   (on_sync_activate), c);
	gtk_signal_connect(GTK_OBJECT(prev), "activate",
			   GTK_SIGNAL_FUNC
			   (on_prev_activate), c);
	gtk_signal_connect(GTK_OBJECT(next), "activate",
			   GTK_SIGNAL_FUNC
			   (on_next_activate), c);
	gtk_signal_connect(GTK_OBJECT(chapter_heading), "activate",
			   GTK_SIGNAL_FUNC
			   (on_chapter_heading_activate), c);
	gtk_signal_connect(GTK_OBJECT(book_heading), "activate",
			   GTK_SIGNAL_FUNC
			   (on_book_heading_activate), c);
	
	/*
	 * for using the current dictionary for lookup 
	 */
	gtk_signal_connect(GTK_OBJECT(usecurrent), "activate",
			   GTK_SIGNAL_FUNC
			   (on_same_lookup_selection_activate), c);

	gtk_signal_connect(GTK_OBJECT(copy), "activate",
			   GTK_SIGNAL_FUNC(on_copy_activate), c);
	gtk_signal_connect(GTK_OBJECT(print), "activate",
			   GTK_SIGNAL_FUNC(on_print_activate), c);
	gtk_signal_connect(GTK_OBJECT(find), "activate",
			   GTK_SIGNAL_FUNC(on_find_activate), c);



	gtk_signal_connect(GTK_OBJECT(set_font), "activate",
			   GTK_SIGNAL_FUNC(set_module_font_activate),
			   c);

	gtk_object_set_data(GTK_OBJECT(pm), "tooltips", tooltips);
	return pm;
}
