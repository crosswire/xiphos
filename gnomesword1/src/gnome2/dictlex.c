/*
 * GnomeSword Bible Study Tool
 * _dictlex.c - gui for commentary modules
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
#  include <config.h>
#endif

#include <gnome.h>

#ifdef USE_MOZILLA
#include <gtkmozembed.h>
#endif

#include "gui/dictlex.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/dictlex_dialog.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/sidebar.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"

#include "main/embed.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"

/******************************************************************************
 * externs
 */
extern gboolean dict_display_change;
extern gboolean isrunningSD;	/* is the view dictionary dialog runing */




/******************************************************************************
 * static
 */



static void set_label(gchar * mod_name)
{
	gtk_label_set_text(GTK_LABEL(widgets.label_dict), mod_name);

}


/******************************************************************************
 * Name
 *   on_entryDictLookup_changed
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_entryDictLookup_changed(GtkEditable * editable,
						       DL_DATA * d)	
 *
 * Description
 *    look up text in dictionary entry widget
 *
 * Return value
 *   void
 */

void on_entryDictLookup_changed(GtkEditable * editable, gpointer data)
{
	main_dictionary_entery_changed(settings.DictWindowModule);
}



/******************************************************************************
 * Name
 *   gui_display_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void gui_display_dictlex(gchar * key)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_display_dictlex(gchar * key)
{
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);
}


/******************************************************************************
 * Name
 *  gui_set_dictlex_mod_and_key
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void gui_set_dictlex_mod_and_key(gchar *mod, gchar *key)	
 *
 * Description
 *   sets the dictionary module and key.  Primarily added for use in tabbed browsing
 *
 * Return value
 *   void
 */

void gui_set_dictlex_mod_and_key(gchar * mod_name, gchar * key)
{
	const gchar *old_key;

	//xml_set_value("GnomeSword", "modules", "dict", mod_name);
	//settings.DictWindowModule = xml_get_value("modules", "dict");
	//set_label(settings.DictWindowModule);
	if (key == NULL)
		key = "Grace";

	old_key = gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict));
	if (!strcmp(old_key, key))
		on_entryDictLookup_changed(NULL, NULL);
	else
		gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);

}


/******************************************************************************
 * Name
 *  html_button_pressed
 *
 * Synopsis
 *   #include "gui/dictlex.h"
 *
 *   gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * d)	
 *
 * Description
 *    mouse button pressed in dictionary / lexicon 
 *
 * Return value
 *   gint
 */

static gint html_button_pressed(GtkWidget * html,
				GdkEventButton * event, gpointer data)
{
	settings.whichwindow = DICTIONARY_WINDOW;

	gui_change_window_title(settings.DictWindowModule);

	switch (event->button) {
	case 1:

		break;
	case 2:
		/* 
		 * pass this for pasting 
		 */
		break;
	case 3:
		gui_create_pm_dictionary();
		break;
		/*gtk_signal_emit_stop_by_name(GTK_OBJECT(html),
		   "button_press_event"); */
		break;
	default:
		break;
	}

	return FALSE;
}


/******************************************************************************
 * Name
 *  list_button_released
 *
 * Synopsis
 *   #include "gui/dictlex.h"
 *
 *   gint list_button_released(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * d)	
 *
 * Description
 *    mouse button released in key list
 *
 * Return value
 *   gint
 */

static gint list_button_released(GtkWidget * treeview,
			 GdkEventButton * event, gpointer data)
{
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *buf = NULL;
	GtkTreeModel *model;

	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

	if (!gtk_tree_selection_get_selected
	    (selection, &model, &selected))
		return;

	switch (event->button) {
	case 1:
		gtk_tree_model_get(model, &selected, 0, &buf, -1);
		if (buf) {
			gtk_entry_set_text(GTK_ENTRY
					   (widgets.entry_dict), buf);
			g_free(buf);
		}
		break;
	case 2:
	case 3:
	default:
		break;
	}

	return FALSE;
}



static void add_columns(GtkTreeView * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);

	/* column for fixed toggles */
	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes("Keys",
							  renderer,
							  "text", 0,
							  NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);

	gtk_tree_view_append_column(treeview, column);
	/* get cell (row) height */
	gtk_cell_renderer_get_size(renderer,
				   GTK_WIDGET(treeview),
				   NULL,
				   NULL,
				   NULL, NULL, &settings.cell_height);
}


static gboolean on_enter_notify_event(GtkWidget * widget,
				      GdkEventCrossing * event,
				      gpointer user_data)
{
	//shift_key_presed = FALSE;
	gtk_widget_grab_focus (widgets.html_dict);
	settings.whichwindow = DICTIONARY_WINDOW;
	gui_change_window_title(settings.DictWindowModule);
  	return FALSE;
}


GtkWidget *gui_create_dictionary_pane(void)
{
	GtkWidget *box_dict;
	GtkWidget *hpaned;
	GtkWidget *vbox;
	GtkWidget *frame_entry;
	GtkWidget *toolbarDLKey;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *label205;
	GtkWidget *scrolledwindow;
	GtkListStore *model;
	GtkWidget *eventbox;	

	box_dict = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(box_dict);

	widgets.label_dict = gtk_label_new(settings.DictWindowModule);
	gtk_widget_show(widgets.label_dict);
	gtk_box_pack_start(GTK_BOX(box_dict),
			   widgets.label_dict, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(widgets.label_dict),
			      GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(widgets.label_dict), 0, 0.5);

	hpaned = gtk_hpaned_new();
	gtk_widget_show(hpaned);
	gtk_box_pack_start(GTK_BOX(box_dict), hpaned, TRUE, TRUE, 0);
	gtk_paned_set_position(GTK_PANED(hpaned), 195);

	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_paned_pack1(GTK_PANED(hpaned), vbox, TRUE, TRUE);
	
	widgets.entry_dict = gtk_entry_new();
	gtk_widget_show(widgets.entry_dict);
	gtk_box_pack_start(GTK_BOX(vbox), widgets.entry_dict, FALSE, TRUE, 0);

	frame_entry = gtk_frame_new(NULL);
	gtk_widget_show(frame_entry);
	gtk_frame_set_shadow_type((GtkFrame *)frame_entry,
                                    settings.shadow_type);
	gtk_box_pack_start(GTK_BOX(vbox), frame_entry, TRUE, TRUE, 0);
	
	/* create tree model */
	model = gtk_list_store_new(1, G_TYPE_STRING);

	/* create tree view */
	widgets.listview_dict =
	    gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	gtk_widget_show(widgets.listview_dict);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW
				     (widgets.listview_dict), TRUE);
	
	gtk_container_add(GTK_CONTAINER(frame_entry), widgets.listview_dict);
	
	
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW
					  (widgets.listview_dict),
					  FALSE);
	add_columns(GTK_TREE_VIEW(widgets.listview_dict));



#ifdef USE_MOZILLA	
	eventbox = gtk_event_box_new ();
	gtk_widget_show (eventbox);
	gtk_paned_pack2(GTK_PANED(hpaned), eventbox, TRUE, TRUE);
	widgets.html_dict = embed_new(DICTIONARY_TYPE);
	gtk_widget_show(widgets.html_dict);
	gtk_container_add(GTK_CONTAINER(eventbox),
			 widgets.html_dict);
	
	g_signal_connect ((gpointer) eventbox, "enter_notify_event",
		    G_CALLBACK (on_enter_notify_event),
		    NULL);

#else
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_paned_pack2(GTK_PANED(hpaned), scrolledwindow, TRUE, TRUE);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow,
                                             settings.shadow_type);

	widgets.html_dict = gtk_html_new();
	gtk_widget_show(widgets.html_dict);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_dict);

	g_signal_connect(GTK_OBJECT(widgets.html_dict),
			 "button_press_event",
			 G_CALLBACK(html_button_pressed), NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_dict),
			 "url_requested",
			 G_CALLBACK(url_requested), NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_dict), "on_url",
			 G_CALLBACK(gui_url),
			 GINT_TO_POINTER(DICTIONARY_TYPE));
	g_signal_connect(GTK_OBJECT(widgets.html_dict), "link_clicked",
			 G_CALLBACK(gui_link_clicked), NULL);
#endif
	g_signal_connect(GTK_OBJECT(widgets.entry_dict), "changed",
			 G_CALLBACK(on_entryDictLookup_changed), NULL);
	g_signal_connect(G_OBJECT(widgets.listview_dict),
			 "button_release_event",
			 G_CALLBACK(list_button_released), NULL);
	return box_dict;
}

/**
 **
 **
 **
 **
 **
 **
 **/


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

static void update_comm_global_ops(gchar * option, gboolean choice)
{
	/*g_warning("gui_update_text_global_ops");
	   save_module_options(settings.MainWindowModule, option, choice);
	   gui_display_text(settings.currentverse); */
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
/*	save_module_options(settings.MainWindowModule, (gchar *) data,
			    GTK_CHECK_MENU_ITEM(menuitem)->active);
	gui_display_text(settings.currentverse);*/
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



static void on_about_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
	gui_display_about_module_dialog(settings.DictWindowModule,
					FALSE);
}


static void on_item1_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{

}


static void on_print1_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gui_html_print(widgets.html_dict, FALSE);
}


static void on_copy2_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
#ifdef USE_MOZILLA
	embed_copy_selection(GTK_MOZ_EMBED(widgets.html_dict));
#else
	gui_copy_html(widgets.html_dict);
#endif
}


static void on_find1_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
	gui_find_dlg(widgets.html_dict, settings.DictWindowModule,
		     FALSE, NULL);
}


static void on_item2_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{

}


static void
on_set_module_font_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_set_module_font(settings.DictWindowModule);
	on_entryDictLookup_changed(NULL, NULL);
}


static void
on_use_current_dictionary_activate(GtkMenuItem * menuitem,
				   gpointer user_data)
{
	gchar *dict_key = NULL;
#ifdef USE_MOZILLA
	embed_copy_selection(GTK_MOZ_EMBED(widgets.html_dict));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);	
#else
	dict_key =
	    gui_get_word_or_selection(widgets.html_dict, FALSE);
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
#endif
}


static void
on_unlock_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *cipher_key;
	gchar *cipher_old;

//      cipher_old = get_cipher_key(settings.DictWindowModule);
	cipher_key =
	    gui_add_cipher_key(settings.DictWindowModule, cipher_old);
	if (cipher_key) {
		main_locked_module_display(widgets.html_dict,
					   settings.DictWindowModule,
					   cipher_key);
	}
}


static void on_show_tabs_activate(GtkMenuItem * menuitem,
				  gpointer user_data)
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



static void on_view_mod_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{

	gchar *module_name = NULL;

	module_name =
	    main_module_name_from_description((gchar *) user_data);
	if (module_name) {
		main_display_dictionary(module_name, settings.dictkey);
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


static GnomeUIInfo edit3_menu_uiinfo[] = {
	GNOMEUIINFO_MENU_COPY_ITEM(on_copy2_activate, NULL),
	GNOMEUIINFO_MENU_FIND_ITEM(on_find1_activate, NULL),
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

void gui_create_pm_dictionary(void)
{
	GtkWidget *menu1;
	GtkWidget *lookup_selection_menu;
	GtkWidget *usecurrent;
	GtkWidget *view_menu;
	GtkWidget *separator;
	GnomeUIInfo *menuitem;
	gchar *mod_name = settings.DictWindowModule;
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



	view_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM
				  (file3_menu_uiinfo[0].widget),
				  view_menu);

	gui_add_mods_2_gtk_menu(DICT_DESC_LIST, view_menu,
				(GCallback) on_view_mod_activate);


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
/*	
	gui_add_mods_2_gtk_menu(DICT_DESC_LIST, lookup_selection_menu,
				(GCallback)gui_lookup_dict_selection);
*/

	if ((main_check_for_global_option(mod_name,
					  "GBFRedLetterWords")) ||
	    (main_check_for_global_option(mod_name,
					  "OSISRedLetterWords"))) {
		gtk_widget_show(module_options_menu_uiinfo[2].widget);	//"words_in_red");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[2].
				    widget)->active = ops->words_in_red;
	}
	if ((main_check_for_global_option(mod_name, "GBFStrongs"))
	    || (main_check_for_global_option(mod_name, "ThMLStrongs"))
	    || (main_check_for_global_option(mod_name, "OSISStrongs"))) {
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
				    widget)->active =
		    ops->scripturerefs;
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
	if (main_has_cipher_tag(mod_name))
		gtk_widget_show(menu1_uiinfo[6].widget);

	gtk_menu_popup((GtkMenu*)menu1, NULL, NULL, NULL, NULL, 2,
		     			gtk_get_current_event_time());
	
	/*gnome_popup_menu_do_popup_modal(menu1, NULL,
					NULL, event, NULL,
					widgets.html_text);
	gtk_widget_destroy(menu1);*/
	g_free(ops);
}





//******  end of file  ******/
