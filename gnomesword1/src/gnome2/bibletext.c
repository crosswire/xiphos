/*
 * GnomeSword Bible Study Tool
 * bibletext.c - gui for Bible text modules
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
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine.h>



#include "gui/gnomesword.h"
#include "gui/gtkhtml_display.h"
#include "gui/bibletext.h"
#include "gui/bibletext_menu.h"
#include "gui/bibletext_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/cipher_key_dialog.h"
#include "gui/html.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/dialog.h"
#include "gui/font_dialog.h"
#include "gui/dictlex.h"
#include "gui/tabbed_browser.h"
#include "gui/widgets.h"

#include "main/bibletext.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"

static void create_menu(GdkEventButton * event);

/******************************************************************************
 * Name
 *  update_text_global_ops
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   void update_text_global_ops(gchar * option, gboolean choice)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_update_text_global_ops(gchar * option, gboolean choice)
{
	/*g_warning("gui_update_text_global_ops");
	save_module_options(settings.MainWindowModule, option, choice);
	gui_display_text(settings.currentverse);*/
}

/******************************************************************************
 * Name
 *  global_option_red_words
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_global_option(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_global_option(GtkMenuItem * menuitem, gpointer data)
{
	save_module_options(settings.MainWindowModule, (gchar *) data,
			    GTK_CHECK_MENU_ITEM(menuitem)->active);
	gui_display_text(settings.currentverse);
}


/******************************************************************************
 * Name
 *    
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   
 *
 * Description
 *   add global module options to popup menus
 *
 * Return value
 *   void
 */

void gui_popup_pm_text(gchar * mod_name, GdkEventButton * event)
{
	GLOBAL_OPS *ops = gui_new_globals(settings.MainWindowModule);
	create_menu(event);	//create_pm_text(ops,event);
	g_free(ops);

}




void gui_set_text_mod_and_key(gchar * mod_name, gchar * val_key)
{
/*	if(cur_t_new->mod_name)
		g_free(cur_t_new->mod_name);			
		cur_t_new->mod_name = g_strdup(settings.MainWindowModule);			
			gui_display_text(val_key);			
			break;	
		}
		tmp = g_list_next(tmp);
	}*/
	gui_display_text(val_key);

}


void gui_display_text(const gchar * val_key)
{
	GLOBAL_OPS *ops = gui_new_globals(settings.MainWindowModule);
/*
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_parallel_text),
				1);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_text),
				0);*/
	chapter_display(widgets.html_text,
			settings.MainWindowModule,
			ops, (gchar *) val_key, TRUE);
	g_free(ops);
}



/**
 **
 **
 **
 **
 **
 **
 **/

void on_about_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_display_about_module_dialog(settings.MainWindowModule, FALSE);
}


void on_item1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


void on_print1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_html_print(widgets.html_text, FALSE);
}


void on_copy2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_copy_html(widgets.html_text);
}


void on_find1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_find_dlg(widgets.html_text, settings.MainWindowModule, FALSE, NULL);
}


void on_item2_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


void
on_set_module_font_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_set_module_font(settings.MainWindowModule);
	gui_display_text(settings.currentverse);
}


void
on_use_current_dictionary_activate(GtkMenuItem * menuitem,
				   gpointer user_data)
{
	gchar *dict_key = gui_get_word_or_selection(widgets.html_text, FALSE);
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


void
on_unlock_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *cipher_key;
	gchar *cipher_old;
	
	cipher_old = get_cipher_key(settings.MainWindowModule);
	cipher_key = gui_add_cipher_key(settings.MainWindowModule, cipher_old);
	if (cipher_key) {
		gui_module_is_locked_display(widgets.html_text,
					     settings.MainWindowModule,
					     cipher_key);
	}
}


void on_show_tabs_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


void
on_all_readings_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


void
on_primary_reading_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


void
on_secondary_reading_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{

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
 *   lookup seledtion in a dict/lex module
 *
 * Return value
 *   void
 */

void gui_lookup_bibletext_selection(GtkMenuItem * menuitem,
				    gchar * dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;
	
	//if(!cur_t->html) return;
	
	mod_name = module_name_from_description(dict_mod_description);
	dict_key = gui_get_word_or_selection(widgets.html_text, FALSE);
	if (dict_key && mod_name) {
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(mod_name,
						      dict_key);
		if (settings.inDictpane)
			gui_change_module_and_key(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
	}
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/bibletext.h"
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
	else {
		gtk_notebook_set_current_page(GTK_NOTEBOOK
				      (widgets.workbook_lower),
				      settings.percomm_page);
		gui_set_percomm_page((gchar *) user_data);
	}
}

/******************************************************************************
 * Name
 *  on_view_mod_activate
 *
 * Synopsis
 *   #include "gui/bibletext.h"
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


static GnomeUIInfo view_text_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("item1"),
	 NULL,
	 (gpointer) on_item1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-book-green",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo file3_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Open Module"),
	 NULL,
	 view_text_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_PRINT_ITEM(on_print1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo note_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("item2"),
	 NULL,
	 (gpointer) on_item2_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo edit3_menu_uiinfo[] = {
	GNOMEUIINFO_MENU_COPY_ITEM(on_copy2_activate, NULL),
	GNOMEUIINFO_MENU_FIND_ITEM(on_find1_activate, NULL),
	{
	 GNOME_APP_UI_SUBTREE, N_("Note"),
	 NULL,
	 note_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-dnd",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo all_readings_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("All Readings"),
	 NULL,
	 (gpointer) on_all_readings_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
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
	GNOMEUIINFO_END
};

static GnomeUIInfo variants_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_RADIOITEMS, NULL, NULL, all_readings_uiinfo,
	 NULL, NULL, GNOME_APP_PIXMAP_NONE, NULL, 0,
	 (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo module_options_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Set Module Font"),
	 NULL,
	 (gpointer) on_set_module_font_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-select-font",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Words of Christ in Red"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Words of Christ in Red",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM,
	 N_("Strong's Numbers"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Strong's Numbers",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE,
	 NULL,
	 0,
	 (GdkModifierType) 0,
	 NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Morphological Tags"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Morphological Tags",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Footnotes"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Footnotes",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Greek Accents"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Greek Accents",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Lemmas"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Lemmas",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Scripture Cross-references"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Scripture Cross-references",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Vowel Points"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Hebrew Vowel Points",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Cantillation"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Hebrew Cantillation",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Headings"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Headings",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Variants"),
	 NULL,
	 variants_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo lookup_selection_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Use Current Dictionary"),
	 NULL,
	 (gpointer) on_use_current_dictionary_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-find",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo menu1_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("About"),
	 NULL,
	 (gpointer) on_about_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-about",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_SUBTREE, N_("File"),
	 NULL,
	 file3_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-open",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Edit"),
	 NULL,
	 edit3_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-dnd",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("_Module Options"),
	 NULL,
	 module_options_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-ok",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Lookup Selection"),
	 NULL,
	 lookup_selection_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-find",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Unlock This Module"),
	 NULL,
	 (gpointer) on_unlock_module_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-authentication",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Show Tabs"),
	 NULL,
	 (gpointer) on_show_tabs_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static void create_menu(GdkEventButton * event)
{
	GtkWidget *menu1;
	GtkWidget *lookup_selection_menu;
	GtkWidget *usecurrent;
	GtkWidget *view_menu;
	GtkWidget *separator;
	GtkWidget *edit_per_menu;
	GnomeUIInfo *menuitem;
	gchar *mod_name = settings.MainWindowModule;
	GLOBAL_OPS *ops = gui_new_globals(settings.MainWindowModule);
	menu1 = gtk_menu_new();
	gnome_app_fill_menu(GTK_MENU_SHELL(menu1), menu1_uiinfo,
			    NULL, FALSE, 0);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (all_readings_uiinfo[0].widget),
				       TRUE);
	gtk_widget_hide(module_options_menu_uiinfo[2].widget);	//"words_in_red"         
	gtk_widget_hide(module_options_menu_uiinfo[3].widget);	//"strongs_numbers"      
	gtk_widget_hide(module_options_menu_uiinfo[4].widget);	//"/morph_tags"  
	gtk_widget_hide(module_options_menu_uiinfo[5].widget);	//"footnotes"    
	gtk_widget_hide(module_options_menu_uiinfo[6].widget);	// "greek_accents"       
	gtk_widget_hide(module_options_menu_uiinfo[7].widget);	//"lemmas"       
	gtk_widget_hide(module_options_menu_uiinfo[8].widget);	//"cross_references"    
	gtk_widget_hide(module_options_menu_uiinfo[9].widget);	//"hebrew_vowel_points" 
	gtk_widget_hide(module_options_menu_uiinfo[10].widget);	//"hebrew_cantillation"        
	gtk_widget_hide(module_options_menu_uiinfo[11].widget);	//"headings"    
	gtk_widget_hide(module_options_menu_uiinfo[12].widget);	//"variants"   
	gtk_widget_hide(menu1_uiinfo[6].widget);	//"unlock_module"



	view_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file3_menu_uiinfo[0].widget),
				  view_menu);
	
	gui_add_mods_2_gtk_menu(TEXT_DESC_LIST, view_menu,
				(GCallback) on_view_mod_activate);
	
	edit_per_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit3_menu_uiinfo[2].widget),
				  edit_per_menu);
				  
	gui_add_mods_2_gtk_menu(PERCOMM_LIST, edit_per_menu,
				(GCallback) edit_percomm);
				
								
	
	lookup_selection_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu1_uiinfo[5].widget),
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
	
	gui_add_mods_2_gtk_menu(DICT_DESC_LIST, lookup_selection_menu,
				(GCallback)gui_lookup_bibletext_selection);
			
				
	if ((check_for_global_option(settings.MainWindowModule,
				     "GBFRedLetterWords")) ||
	    (check_for_global_option(settings.MainWindowModule,
				     "OSISRedLetterWords"))) {
		gtk_widget_show(module_options_menu_uiinfo[2].widget);	//"words_in_red");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[2].
				    widget)->active = ops->words_in_red;
	}
	if ((check_for_global_option
	     (settings.MainWindowModule, "GBFStrongs"))
	    ||
	    (check_for_global_option
	     (settings.MainWindowModule, "ThMLStrongs"))
	    ||
	    (check_for_global_option
	     (settings.MainWindowModule, "OSISStrongs"))) {
		gtk_widget_show(module_options_menu_uiinfo[3].widget);	//"strongs_numbers");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[3].
				    widget)->active = ops->strongs;
	}
	if ((check_for_global_option(mod_name, "GBFMorph")) ||
	    (check_for_global_option(mod_name, "ThMLMorph")) ||
	    (check_for_global_option(mod_name, "OSISMorph"))) {
		gtk_widget_show(module_options_menu_uiinfo[4].widget);	//"/morph_tags");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[4].
				    widget)->active = ops->morphs;
	}
	if ((check_for_global_option(mod_name, "GBFFootnotes")) ||
	    (check_for_global_option(mod_name, "ThMLFootnotes")) ||
	    (check_for_global_option(mod_name, "OSISFootnotes"))) {
		gtk_widget_show(module_options_menu_uiinfo[5].widget);	//"footnotes");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[5].
				    widget)->active = ops->footnotes;
	}
	if (check_for_global_option(mod_name, "UTF8GreekAccents")) {
		gtk_widget_show(module_options_menu_uiinfo[6].widget);	// "greek_accents");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[6].
				    widget)->active = ops->greekaccents;
	}
	if (check_for_global_option(mod_name, "ThMLLemma")) {
		gtk_widget_show(module_options_menu_uiinfo[7].widget);	//"lemmas");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[7].
				    widget)->active = ops->lemmas;
	}
	if (check_for_global_option(mod_name, "ThMLScripref") ||
	    (check_for_global_option(mod_name, "OSISScripref"))) {
		gtk_widget_show(module_options_menu_uiinfo[8].widget);	//"cross_references");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[8].
				    widget)->active = ops->scripturerefs;
	}
	if (check_for_global_option(mod_name, "UTF8HebrewPoints")) {
		gtk_widget_show(module_options_menu_uiinfo[9].widget);	//"hebrew_vowel_points");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[9].
				    widget)->active = ops->hebrewpoints;
	}
	if (check_for_global_option(mod_name, "UTF8Cantillation")) {
		gtk_widget_show(module_options_menu_uiinfo[10].widget);	//"hebrew_cantillation");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[10].
				    widget)->active = ops->hebrewcant;
	}
	if (check_for_global_option(mod_name, "ThMLHeadings") ||
	    (check_for_global_option(mod_name, "OSISHeadings"))) {
		gtk_widget_show(module_options_menu_uiinfo[11].widget);	//"headings");
		 GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[11].
				    widget)->active = ops->headings;   
	}
	if (check_for_global_option(mod_name, "ThMLVariants")) {
		gtk_widget_show(module_options_menu_uiinfo[12].widget);	//"variants");

		gtk_widget_show(all_readings_uiinfo[0].widget);	//"all_readings");

		gtk_widget_show(all_readings_uiinfo[1].widget);	//"primary_reading");

		gtk_widget_show(all_readings_uiinfo[2].widget);	//"secondary_reading");
		
	}
	if(has_cipher_tag(mod_name))
		gtk_widget_show(menu1_uiinfo[6].widget);
	
	
	/* 
	 * menu1_uiinfo[0].widget, "about");
	 * menu1_uiinfo[1].widget, "separator4");
	 * menu1_uiinfo[2].widget, "file3");
	 * file3_menu_uiinfo[0].widget, "view_text");
	 * view_text_menu_uiinfo[0].widget, "item1");
	 * file3_menu_uiinfo[1].widget, "separator8");
	 * file3_menu_uiinfo[2].widget, "print1");
	 * menu1_uiinfo[3].widget, "edit3");
	 * edit3_menu_uiinfo[0].widget, "copy2");
	 * edit3_menu_uiinfo[1].widget, "find1");
	 * edit3_menu_uiinfo[2].widget, "note");
	 * note_menu_uiinfo[0].widget, "item2");
	 * menu1_uiinfo[4].widget, "module_options");
	 * module_options_menu_uiinfo[0].widget, "set_module_font");
	 * module_options_menu_uiinfo[1].widget, "separator5");
	 * menu1_uiinfo[5].widget, "lookup_selection");
	 * lookup_selection_menu_uiinfo[0].widget, "use_current_dictionary");
	 * lookup_selection_menu_uiinfo[1].widget, "separator6");
	 * menu1_uiinfo[7].widget, "separator7");
	 * menu1_uiinfo[8].widget, "show_tabs");
	 */
	gnome_popup_menu_do_popup_modal(menu1, NULL,
					NULL, event, NULL,
					widgets.html_text);
	gtk_widget_destroy(menu1);
	g_free(ops);
}



/* end of file */
