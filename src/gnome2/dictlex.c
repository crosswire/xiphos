/*
 * Xiphos Bible Study Tool
 * _dictlex.c - gui for commentary modules
 *
 * Copyright (C) 2000-2011 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#ifdef GTKHTML
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

#include "../xiphos_html/xiphos_html.h"

#include "gui/dictlex.h"
#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/xiphos.h"
#include "gui/cipher_key_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/main_window.h"
#include "gui/menu_popup.h"
#include "gui/sidebar.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/url.hh"
#include "main/xml.h"

#include "gui/debug_glib_null.h"

/******************************************************************************
 * externs
 */
extern gboolean dict_display_change;
extern gboolean isrunningSD;	/* is the view dictionary dialog runing */




/******************************************************************************
 * static
 */


/******************************************************************************
 * Name
 *   gui_get_clipboard_text_for_lookup
 *
 * Synopsis
 *   #include "gui/dictlex.h"
 *
 *   void gui_get_clipboard_text_for_lookup (GtkClipboard *clipboard,
 *					     const gchar *text,
 *					     gpointer data)
 *
 * Description
 *    an ugly hack to get the selection from widget on a dbl click
 *    and display text in dictionary pane using default dictionary if set or
 *    current dictionary - this called by (gecko|wk)/Yelper.cpp 
 *    Yelper::ProcessMouseDblClickEvent (void* aEvent)
 *    also called by wk-html.c   button_press_handler() -- still ugly
 *
 * Return value
 *   void
 */

void gui_get_clipboard_text_for_lookup (GtkClipboard *clipboard,
					const gchar *text,
					gpointer data)
{
	char *key = NULL;
	gchar *dict = NULL;
	int len = 0;

	if (text == NULL) return;
	GS_message(("src/gnome2/dictlex.c: text =>%s<",text));

	key = g_strdelimit((char*)text, "&.,\"<>;:?", ' ');
	key = g_strstrip((char*)key);
	len = strlen(key);

	if (key[len - 1] == 's' || key[len - 1] == 'd')
		key[len - 1] = '\0';
	if (key[len - 1] == 'h' && key[len - 2] == 't'
	    && key[len - 3] == 'e')
		key[len - 3] = '\0';

	if (settings.useDefaultDict)
		dict = g_strdup(settings.DefaultDict);
	else
		dict = g_strdup(settings.DictWindowModule);

	main_display_dictionary(dict, key);

	if (dict)
		g_free(dict);
}

#if 0
static void set_label(gchar * mod_name)
{
	gtk_label_set_text(GTK_LABEL(widgets.label_dict), mod_name);

}
#endif

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

	//xml_set_value("Xiphos", "modules", "dict", mod_name);
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

#ifndef USE_XIPHOS_HTML
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
		//gui_create_pm_dictionary();
		gui_menu_popup (NULL, settings.DictWindowModule, NULL);
		break;
		/*gtk_signal_emit_stop_by_name(G_OBJECT(html),
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

extern gboolean in_url;

static gint html_button_released(GtkWidget * html,
				GdkEventButton * event, gpointer data)
{
#ifdef GTKHTML
	gchar *key;
	const gchar *url;
#endif

	settings.whichwindow = DICTIONARY_WINDOW;

	//gui_change_window_title(settings.DictWindowModule);

#ifdef GTKHTML
	switch (event->button) {
	case 1:
		if (in_url)
			break;
		key = gui_button_press_lookup(widgets.html_dict);
		if (key) {
			if (g_strstr_len(key,strlen(key),"*")) {
				key = g_strdelimit(key, "*", ' ');
				key = g_strstrip(key);
				url = g_strdup_printf(
					"passagestudy.jsp?action=showModInfo&value=1&module=%s",
					key);
				main_url_handler(url,TRUE);
				g_free((gchar*)url);
				g_free(key);
				break;
			}
		}
		break;
	}
#endif /* GTKHTML */
	return FALSE;
}

#endif /* !USE_XIPHOS_HTML */

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
#if 0
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
		return 0;

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
//	GtkTreeModel *model = gtk_tree_view_get_model(treeview);

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
#endif /* 0 */


void dict_key_entry_changed(GtkEntry * entry, gpointer data)
{
	gchar *buf = NULL;

	buf = (gchar*)gtk_entry_get_text(entry);
	GS_message(("dict_key_entry_changed: %s",buf));
	if (strlen(buf) < 2 )
		return;

	main_display_dictionary(settings.DictWindowModule, buf);
	//gtk_widget_grab_focus(widgets.entry_dict);
}

void button_back_clicked(GtkButton * button, gpointer user_data)
{
	if (settings.havedict) main_dictionary_button_clicked(0);
}

void button_forward_clicked(GtkButton * button, gpointer user_data)
{
	if (settings.havedict) main_dictionary_button_clicked(1);
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
	GtkAllocation allocation;
    
	g_return_if_fail (GTK_IS_BUTTON (user_data));
#if defined(HAVE_GTK_220) || defined(USE_GTK_3)
        g_return_if_fail (gtk_widget_get_has_window(user_data));
#else
	g_return_if_fail (GTK_WIDGET_NO_WINDOW (user_data));
#endif
	widget = GTK_WIDGET (user_data);

	gdk_window_get_origin (gtk_widget_get_window(widget), x, y);
	gtk_widget_get_allocation (widget,&allocation);
	*x += allocation.x;
	*y += allocation.y + allocation.height;

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
	if (!settings.DictWindowModule ||
	    (*settings.DictWindowModule == '\0'))
		return 0;

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
#ifdef USE_XIPHOS_HTML
static void
_popupmenu_requested_cb (XiphosHtml *html,
			     gchar *uri,
			     gpointer user_data)
{
	gui_menu_popup (html, settings.DictWindowModule, NULL);
	//gui_create_pm_dictionary();
}
#endif

GtkWidget *gui_create_dictionary_pane(void)
{
	GtkWidget *box_dict;
	GtkWidget *hbox2;
	GtkWidget *button10;
	GtkWidget *image1;
	GtkWidget *button11;
	GtkWidget *image2;
	GtkWidget *arrow1;
	GtkWidget *dict_drop_down;
	GtkWidget *scrolledwindow;

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

	image1 = gtk_image_new_from_stock(GTK_STOCK_GO_UP, GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image1);
	gtk_container_add(GTK_CONTAINER(button10), image1);

	button11 = gtk_button_new();
	gtk_widget_show(button11);
	gtk_box_pack_start(GTK_BOX(hbox2), button11, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(button11), GTK_RELIEF_NONE);

	image2 = gtk_image_new_from_stock(GTK_STOCK_GO_DOWN, GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image2);
	gtk_container_add(GTK_CONTAINER(button11), image2);

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(box_dict), scrolledwindow, TRUE, TRUE, 0);
	
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow,
                                             settings.shadow_type);
#ifdef USE_XIPHOS_HTML
	widgets.html_dict = GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, DICTIONARY_TYPE));
	gtk_widget_show(widgets.html_dict);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			 widgets.html_dict);
	g_signal_connect((gpointer)widgets.html_dict,
		      "popupmenu_requested",
		      G_CALLBACK (_popupmenu_requested_cb),
		      NULL);
#else

	widgets.html_dict = gtk_html_new();
	gtk_widget_show(widgets.html_dict);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_dict);

	g_signal_connect(G_OBJECT(widgets.html_dict),
			 "button_press_event",
			 G_CALLBACK(html_button_pressed), NULL);
	g_signal_connect(G_OBJECT(widgets.html_dict),
			 "button_release_event",
			 G_CALLBACK(html_button_released), NULL);
	g_signal_connect(G_OBJECT(widgets.html_dict),
			 "url_requested",
			 G_CALLBACK(url_requested), NULL);
	g_signal_connect(G_OBJECT(widgets.html_dict), "on_url",
			 G_CALLBACK(gui_url),
			 GINT_TO_POINTER(DICTIONARY_TYPE));
	g_signal_connect(G_OBJECT(widgets.html_dict), "link_clicked",
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


//******  end of file  ******/
