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

#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

#include "gui/dictlex.h"
#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/dictlex_dialog.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/sidebar.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"

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
	main_dictionary_entry_changed(settings.DictWindowModule);
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

	//gui_change_window_title(settings.DictWindowModule);

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
 *  html_button_released
 *
 * Synopsis
 *   #include "gui/dictlex.h"
 *
 *   gint html_button_released(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * d)	
 *
 * Description
 *    mouse button released in dictionary / lexicon 
 *
 * Return value
 *   gint
 */

static gint html_button_released(GtkWidget * html,
				GdkEventButton * event, gpointer data)
{
	extern gboolean in_url;
	gchar *key;
	const gchar *url;
	//gchar *buf = NULL;
	
	settings.whichwindow = DICTIONARY_WINDOW;

	//gui_change_window_title(settings.DictWindowModule);

	switch (event->button) {
	case 1:
		if (in_url) 
			break;
		key = gui_button_press_lookup(widgets.html_dict);
		if (key) {
			if(g_strstr_len(key,strlen(key),"*")) {
				key = g_strdelimit(key, "*", ' ');
				key = g_strstrip(key);
				url = g_strdup_printf(
					"gnomesword.url?action=showModInfo&value=1&module=%s",
					key);
				main_url_handler(url,TRUE);
				g_free((gchar*)url);
				g_free(key);
				break;
			}
		}
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
	//gtk_widget_grab_focus (widgets.html_dict);
	//settings.whichwindow = DICTIONARY_WINDOW;
	//gui_change_window_title(settings.DictWindowModule);
  	return FALSE;
}

void dict_key_entry_changed(GtkEntry * entry, gpointer data)
{
	gchar *buf = NULL;
	
	buf = (gchar*)gtk_entry_get_text(entry);
	GS_message(("dict_key_entry_changed: %s",buf));
	if(strlen(buf) < 2 )
		return;
	
	main_display_dictionary(settings.DictWindowModule, buf);
	//gtk_widget_grab_focus(widgets.entry_dict);
}

void button_back_clicked(GtkButton * button, gpointer user_data)
{
	if(settings.havedict) main_dictionary_button_clicked(0);
}

void button_forward_clicked(GtkButton * button, gpointer user_data)
{
	if(settings.havedict) main_dictionary_button_clicked(1);
}

static void on_entry_activate(GtkEntry * entry,
				   gpointer user_data)
{
	GS_message(("on_entry_activate"));
}


/******************************************************************************
 * Name
 *   menu_deactivate_callback
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void menu_deactivate_callback (GtkWidget *widget, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void menu_deactivate_callback (GtkWidget *widget, gpointer user_data)
{
	GtkWidget *menu_button;
	
	menu_button = GTK_WIDGET (user_data);
		
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (menu_button), FALSE);
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
 *
 *
 * Return value
 *   
 */

static void menu_position_under (GtkMenu *menu, 
		     int *x, 
		     int *y,
		     gboolean *push_in,
		     gpointer user_data)
{
	GtkWidget *widget;
	
	g_return_if_fail (GTK_IS_BUTTON (user_data));
	g_return_if_fail (GTK_WIDGET_NO_WINDOW (user_data));

	widget = GTK_WIDGET (user_data);
	
	gdk_window_get_origin (widget->window, x, y);
	
	*x += widget->allocation.x;
	*y += widget->allocation.y + widget->allocation.height;

	*push_in = FALSE;
}


/******************************************************************************
 * Name
 *   select_button_press_callback
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   gboolean select_button_press_callback (GtkWidget *widget,
 *			      GdkEventButton *event,
 *			      gpointer user_data)
 *
 * Description
 *    make the tooglebutton act like a gtk optionmenu by dropping a popup
 *    under the button
 *
 * Return value
 *   gboolean
 */

static gboolean select_button_press_callback (GtkWidget *widget,
			      GdkEventButton *event,
			      gpointer user_data)
{
	GtkWidget *menu = main_dictionary_drop_down_new(settings.DictWindowModule,
						settings.dictkey);
	
	g_signal_connect (menu, "deactivate",
			  G_CALLBACK (menu_deactivate_callback),
			  widget);
	if ((event->type == GDK_BUTTON_PRESS) && event->button == 1) {
		gtk_widget_grab_focus (widget);
		
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
		gtk_menu_popup (GTK_MENU (menu),
				NULL, NULL, menu_position_under, widget, 
				event->button, event->time);

		return TRUE;
	}
	return FALSE;
}
#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb (GeckoHtml *html,
			     gchar *uri,
			     gpointer user_data)
{	
	gui_create_pm_dictionary(); 
}
#endif

GtkWidget *gui_create_dictionary_pane(void)
{
	GtkWidget *box_dict;
	GtkWidget *hpaned;
	GtkWidget *vbox;
	GtkWidget *hbox2;
	GtkWidget *button10;
	GtkWidget *image1;
	GtkWidget *comboboxentry1;
	GtkWidget *button11;
	GtkWidget *image2;
	GtkWidget *arrow1;
	GtkWidget *frame_entry;
	GtkWidget *toolbarDLKey;
	GtkWidget *dict_drop_down;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *label205;
	GtkWidget *scrolledwindow;
	GtkWidget *frame;
	GtkListStore *model;
	GtkWidget *eventbox;
	GtkListStore *store;

	box_dict = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(box_dict);
	
	gtk_container_set_border_width (GTK_CONTAINER (box_dict), 1);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox2);
	gtk_box_pack_start(GTK_BOX(box_dict), hbox2, FALSE, FALSE, 0);

  	widgets.entry_dict = gtk_entry_new ();
  	gtk_widget_show (widgets.entry_dict);
	gtk_box_pack_start(GTK_BOX(hbox2), widgets.entry_dict, TRUE, TRUE, 0);
	
		   
  	dict_drop_down = gtk_toggle_button_new ();
 	 gtk_widget_show (dict_drop_down);
	gtk_box_pack_start(GTK_BOX(hbox2), dict_drop_down, FALSE, TRUE, 0);

  	arrow1 = gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_OUT);
  	gtk_widget_show (arrow1);
  	gtk_container_add (GTK_CONTAINER (dict_drop_down), arrow1);
	
	button10 = gtk_button_new();
	gtk_widget_show(button10);
	gtk_box_pack_start(GTK_BOX(hbox2), button10, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(button10), GTK_RELIEF_NONE);

  	image1 = gtk_image_new_from_icon_name ("stock_up", GTK_ICON_SIZE_BUTTON);
	/*image1 =
	    gtk_image_new_from_stock("gtk-go-back",
				     GTK_ICON_SIZE_BUTTON);*/
	gtk_widget_show(image1);
	gtk_container_add(GTK_CONTAINER(button10), image1);
	
	button11 = gtk_button_new();
	gtk_widget_show(button11);
	gtk_box_pack_start(GTK_BOX(hbox2), button11, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(button11), GTK_RELIEF_NONE);

	image2 = gtk_image_new_from_icon_name ("stock_down", GTK_ICON_SIZE_BUTTON);
	   /* gtk_image_new_from_stock("gtk-go-forward",
				     GTK_ICON_SIZE_BUTTON);*/
	gtk_widget_show(image2);
	gtk_container_add(GTK_CONTAINER(button11), image2);

#ifdef USE_GTKMOZEMBED
	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), settings.shadow_type);
	gtk_box_pack_start(GTK_BOX(box_dict), frame, TRUE, TRUE, 0);
	gtk_widget_show(frame);
	
	eventbox = gtk_event_box_new ();
	gtk_container_add(GTK_CONTAINER(frame), eventbox);
	gtk_widget_show (eventbox);
	
	widgets.html_dict = GTK_WIDGET(gecko_html_new(NULL, FALSE, DICTIONARY_TYPE));
	gtk_widget_show(widgets.html_dict);
	gtk_container_add(GTK_CONTAINER(eventbox),
			 widgets.html_dict);
	g_signal_connect((gpointer)widgets.html_dict,
		      "popupmenu_requested",
		      G_CALLBACK (_popupmenu_requested_cb),
		      NULL);
	g_signal_connect ((gpointer) eventbox, "enter_notify_event",
		    G_CALLBACK (on_enter_notify_event),
		    NULL);
#else
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(box_dict), scrolledwindow, TRUE, TRUE, 0);

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
			 "button_release_event",
			 G_CALLBACK(html_button_released), NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_dict),
			 "url_requested",
			 G_CALLBACK(url_requested), NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_dict), "on_url",
			 G_CALLBACK(gui_url),
			 GINT_TO_POINTER(DICTIONARY_TYPE));
	g_signal_connect(GTK_OBJECT(widgets.html_dict), "link_clicked",
			 G_CALLBACK(gui_link_clicked), NULL);
#endif
 
	g_signal_connect (dict_drop_down, 
			  "button_press_event",
			  G_CALLBACK (select_button_press_callback),
			  NULL);
	g_signal_connect(G_OBJECT(widgets.entry_dict), "activate",
			 G_CALLBACK(dict_key_entry_changed), NULL);
			 
	g_signal_connect ((gpointer) button10, "clicked",
		    G_CALLBACK (button_back_clicked),
		    NULL);
	g_signal_connect ((gpointer) button11, "clicked",
		    G_CALLBACK (button_forward_clicked),
		    NULL);
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
	main_save_module_options(settings.DictWindowModule, (gchar *) data,
			    GTK_CHECK_MENU_ITEM(menuitem)->active);
	main_display_dictionary(settings.DictWindowModule, settings.dictkey);
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
#ifdef USE_GTKMOZEMBED
	gecko_html_print_document (GTK_WINDOW(widgets.app), 
				   settings.DictWindowModule, 
				   GECKO_HTML(widgets.html_dict));
#else	
	gui_html_print(widgets.html_dict, FALSE);
#endif
}


static void on_copy2_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(widgets.html_dict));
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
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(widgets.html_dict));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);		
#else
	dict_key =
	    gui_get_word_or_selection(widgets.html_dict, FALSE);
	if (dict_key) {
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
	if (cipher_key)
		main_display_dictionary(settings.DictWindowModule, settings.dictkey);
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



static void on_add_bookmark_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{
	
	gchar *label = g_strdup_printf("%s, %s",settings.dictkey,
					settings.DictWindowModule);

	gui_bookmark_dialog(label,settings.DictWindowModule, settings.dictkey);
	g_free(label);	
	
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
	//GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Bookmark"),
	 NULL,
	 (gpointer) on_add_bookmark_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-add",
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
	 GNOME_APP_UI_TOGGLEITEM, N_("Show Tabs"),
	 NULL,
	 (gpointer) on_show_tabs_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Unlock This Module"),
	 NULL,
	 (gpointer) on_unlock_module_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-authentication",
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
	gtk_widget_hide(menu1_uiinfo[7].widget);	//"unlock_module" 
	gtk_widget_hide(menu1_uiinfo[8].widget);
	gtk_widget_hide(menu1_uiinfo[9].widget);



	view_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM
				  (file3_menu_uiinfo[0].widget),
				  view_menu);

	gui_add_mods_2_gtk_menu(DICT_DESC_LIST, view_menu,
				(GCallback) on_view_mod_activate);


	lookup_selection_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu1_uiinfo[6].widget),
				  lookup_selection_menu);

	usecurrent =
	    gtk_menu_item_new_with_label(_("Use Current Dictionary"));
	gtk_widget_show(usecurrent);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent);
	g_signal_connect(GTK_OBJECT(usecurrent),
			   "activate",
			   G_CALLBACK(on_use_current_dictionary_activate), 
			   NULL);

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
		gtk_widget_show(menu1_uiinfo[7].widget);

	gtk_menu_popup((GtkMenu*)menu1, NULL, NULL, NULL, NULL, 2,
		     			gtk_get_current_event_time());
	
	g_free(ops);
}





//******  end of file  ******/
