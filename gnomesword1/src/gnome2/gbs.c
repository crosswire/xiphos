/*
 * GnomeSword Bible Study Tool
 * gbs.c - generic book support - the gui
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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


//#include "gui/gtkhtml_display.h"
#include "gui/gbs.h"
#include "gui/gbs_dialog.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/find_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/bookmarks.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/lists.h"
#include "main/global_ops.hh"
#include "main/sword.h"
#include "main/xml.h"

static void create_menu(GdkEventButton * event);



/******************************************************************************
 * Name
 *    
 *
 * Synopsis
 *   #include "gui/gbs.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_popup_menu_gbs(GdkEventButton * event)
{
	create_menu(event);
}

/******************************************************************************
 * Name
 *  gui_set_gbs_frame_label
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_set_gbs_frame_label(void)	
 *
 * Description
 *   sets gbs label to module name
 *
 * Return value
 *   void
 */

static void set_gbs_label(gchar * mod_name)
{
	/*
	 * set label to module name
	 */
	gtk_label_set_text (GTK_LABEL(widgets.label_comm),mod_name);
}

/******************************************************************************
 * Name
 *  gui_set_book_page_and_key
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_set_book_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_set_book_mod_and_key(gchar * mod_name, gchar * key)
{
	main_display_book(mod_name, key);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include ".h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_update_gbs_global_ops(gchar * option, gboolean choice)
{
	/*save_module_options(cur_t->mod_name, option, 
				    choice);*/
	//gbs_display(cur_g, tree_level);
}




/**  Main Book menu stuff
 **
 **
 **
 **
 **
 **
 **/

/******************************************************************************
 * Name
 *  on_global_option
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_global_option(GtkMenuItem * menuitem, gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_global_option(GtkMenuItem * menuitem, gpointer data)
{
	main_save_module_options(settings.book_mod, (gchar *) data,
			    GTK_CHECK_MENU_ITEM(menuitem)->active);
	main_display_book(settings.book_mod,settings.book_key);
	//gui_set_book_mod_and_key(settings.book_mod, settings.book_key);
}



static void on_about_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_display_about_module_dialog(settings.book_mod, FALSE);
}


static void on_item1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void on_print1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_html_print(widgets.html_comm, FALSE);
}


static void on_copy2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_copy_html(widgets.html_comm);
}


static void on_find1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_find_dlg(widgets.html_comm, settings.book_mod, FALSE, NULL);
}


static void on_item2_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_set_module_font_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_set_module_font(settings.book_mod);
	//gui_display_text(settings.currentverse);
}


static void
on_use_current_dictionary_activate(GtkMenuItem * menuitem,
				   gpointer user_data)
{
	gchar *dict_key = gui_get_word_or_selection(widgets.html_comm, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			main_sidebar_display_dictlex(settings.
						      DictWindowModule,
						      dict_key);
		if (settings.inDictpane)
			main_display_dictionary(settings.
						  DictWindowModule,
						  dict_key);
		g_free(dict_key);
	}
}


static void
on_unlock_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *cipher_key;
	gchar *cipher_old;
	
//	cipher_old = get_cipher_key(settings.book_mod);
	cipher_key = gui_add_cipher_key(settings.book_mod, cipher_old);
	if (cipher_key) {
		main_locked_module_display(widgets.html_text,
				settings.book_mod, cipher_key);
	}
}


static void on_show_tabs_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_all_readings_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_primary_reading_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
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

void gui_lookup_gbs_selection(GtkMenuItem * menuitem,
				    gchar * dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;
	
	//if(!cur_t->html) return;
	
	mod_name = main_module_name_from_description(dict_mod_description);
	dict_key = gui_get_word_or_selection(widgets.html_comm, FALSE);
	if (dict_key && mod_name) {
		if (settings.inViewer)
			main_sidebar_display_dictlex(mod_name,
						      dict_key);
		if (settings.inDictpane)
			main_display_dictionary(mod_name, dict_key);
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
	//gui_open_commentary_editor((gchar *) user_data);
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

	module_name = main_module_name_from_description((gchar *) user_data);
	if(module_name) {
		main_display_book(module_name, "0");
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
	gchar *mod_name = settings.book_mod;
	GLOBAL_OPS *ops = main_new_globals(mod_name);
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
	gtk_widget_hide(menu1_uiinfo[7].widget);
	gtk_widget_hide(menu1_uiinfo[8].widget);
	gtk_widget_hide(file3_menu_uiinfo[0].widget);
	gtk_widget_hide(file3_menu_uiinfo[1].widget);
	gtk_widget_hide(edit3_menu_uiinfo[2].widget);
				
								
	
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
				(GCallback)gui_lookup_gbs_selection);
			
				
	if ((main_check_for_global_option(mod_name,
				     "GBFRedLetterWords")) ||
	    (main_check_for_global_option(mod_name,
				     "OSISRedLetterWords"))) {
		gtk_widget_show(module_options_menu_uiinfo[2].widget);	//"words_in_red");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[2].
				    widget)->active = ops->words_in_red;
	}
	if ((main_check_for_global_option
	     (mod_name, "GBFStrongs"))
	    ||
	    (main_check_for_global_option
	     (mod_name, "ThMLStrongs"))
	    ||
	    (main_check_for_global_option
	     (mod_name, "OSISStrongs"))) {
		gtk_widget_show(module_options_menu_uiinfo[3].widget);	//"strongs_numbers");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[3].
				    widget)->active = ops->strongs;
	}
	if ((main_check_for_global_option(mod_name, "GBFMorph")) ||
	    (main_check_for_global_option(mod_name, "ThMLMorph")) ||
	    (main_check_for_global_option(mod_name, "OSISMorph"))) {
		gtk_widget_show(module_options_menu_uiinfo[4].widget);	//"/morph_tags");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[4].
				    widget)->active = ops->morphs;
	}
	if ((main_check_for_global_option(mod_name, "GBFFootnotes")) ||
	    (main_check_for_global_option(mod_name, "ThMLFootnotes")) ||
	    (main_check_for_global_option(mod_name, "OSISFootnotes"))) {
		gtk_widget_show(module_options_menu_uiinfo[5].widget);	//"footnotes");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[5].
				    widget)->active = ops->footnotes;
	}
	if (main_check_for_global_option(mod_name, "UTF8GreekAccents")) {
		gtk_widget_show(module_options_menu_uiinfo[6].widget);	// "greek_accents");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[6].
				    widget)->active = ops->greekaccents;
	}
	if (main_check_for_global_option(mod_name, "ThMLLemma")) {
		gtk_widget_show(module_options_menu_uiinfo[7].widget);	//"lemmas");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[7].
				    widget)->active = ops->lemmas;
	}
	if (main_check_for_global_option(mod_name, "ThMLScripref") ||
	    (main_check_for_global_option(mod_name, "OSISScripref"))) {
		gtk_widget_show(module_options_menu_uiinfo[8].widget);	//"cross_references");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[8].
				    widget)->active = ops->scripturerefs;
	}
	if (main_check_for_global_option(mod_name, "UTF8HebrewPoints")) {
		gtk_widget_show(module_options_menu_uiinfo[9].widget);	//"hebrew_vowel_points");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[9].
				    widget)->active = ops->hebrewpoints;
	}
	if (main_check_for_global_option(mod_name, "UTF8Cantillation")) {
		gtk_widget_show(module_options_menu_uiinfo[10].widget);	//"hebrew_cantillation");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[10].
				    widget)->active = ops->hebrewcant;
	}
	if (main_check_for_global_option(mod_name, "ThMLHeadings") ||
	    (main_check_for_global_option(mod_name, "OSISHeadings"))) {
		gtk_widget_show(module_options_menu_uiinfo[11].widget);	//"headings");
		 GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[11].
				    widget)->active = ops->headings;   
	}
	if (main_check_for_global_option(mod_name, "ThMLVariants")) {
		gtk_widget_show(module_options_menu_uiinfo[12].widget);	//"variants");

		gtk_widget_show(all_readings_uiinfo[0].widget);	//"all_readings");

		gtk_widget_show(all_readings_uiinfo[1].widget);	//"primary_reading");

		gtk_widget_show(all_readings_uiinfo[2].widget);	//"secondary_reading");
		
	}
	if(main_has_cipher_tag(mod_name))
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



//******  end of file  ******/
