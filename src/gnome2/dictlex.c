/*
 * Xiphos Bible Study Tool
 * dictlex.c - gui for commentary modules
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
#include <config.h>
#endif

#include <gtk/gtk.h>

#include "xiphos_html/xiphos_html.h"

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
#include "gui/utilities.h"

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
extern gboolean isrunningSD; /* is the view dictionary dialog runing */

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

void gui_get_clipboard_text_for_lookup(GtkClipboard *clipboard,
				       const gchar *text, gpointer data)
{
	char *key = NULL;
	gchar *dict = NULL;
	int len = 0;

	if (text == NULL)
		return;
	XI_message(("src/gnome2/dictlex.c: text =>%s<", text));

	key = g_strdelimit((char *)text, "&.,\"<>;:?", ' ');
	key = g_strstrip((char *)key);
	len = strlen(key);

	if (key[len - 1] == 's' || key[len - 1] == 'd')
		key[len - 1] = '\0';
	if (key[len - 1] == 'h' && key[len - 2] == 't' && key[len - 3] == 'e')
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

void on_entryDictLookup_changed(GtkEditable *editable, gpointer data)
{
	main_dictionary_entry_changed(settings.DictWindowModule);
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
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

void gui_display_dictlex(gchar *key)
{
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);
}
#endif

#if 0
// unneeded at this time.  disabled to silence cppcheck.
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

void gui_set_dictlex_mod_and_key(gchar *mod_name, gchar *key)
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
#endif

void dict_key_entry_changed(GtkEntry *entry, gpointer data)
{
	gchar *buf = NULL;

	buf = (gchar *)gtk_entry_get_text(entry);
	XI_message(("dict_key_entry_changed: %s", buf));
	if (strlen(buf) < 2)
		return;

	main_display_dictionary(settings.DictWindowModule, buf);
	//gtk_widget_grab_focus(widgets.entry_dict);
}

void button_back_clicked(GtkButton *button, gpointer user_data)
{
	if (settings.havedict)
		main_dictionary_button_clicked(0);
}

void button_forward_clicked(GtkButton *button, gpointer user_data)
{
	if (settings.havedict)
		main_dictionary_button_clicked(1);
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

static void menu_deactivate_callback(GtkWidget *widget,
				     gpointer user_data)
{
	GtkWidget *menu_button;

	menu_button = GTK_WIDGET(user_data);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(menu_button),
				     FALSE);
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

static void menu_position_under(GtkMenu *menu,
				int *x,
				int *y,
				gboolean *push_in, gpointer user_data)
{
	GtkWidget *widget;
	GtkAllocation allocation;

	g_return_if_fail(GTK_IS_BUTTON(user_data));
#if GTK_CHECK_VERSION(2, 20, 0)
	g_return_if_fail(gtk_widget_get_window(user_data));
#else
	g_return_if_fail(GTK_WIDGET_NO_WINDOW(user_data));
#endif
	widget = GTK_WIDGET(user_data);

	gdk_window_get_origin(gtk_widget_get_window(widget), x, y);
	gtk_widget_get_allocation(widget, &allocation);
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

static gboolean select_button_press_callback(GtkWidget *widget,
					     GdkEventButton *event,
					     gpointer user_data)
{
	if (!settings.DictWindowModule ||
	    (*settings.DictWindowModule == '\0'))
		return 0;

	GtkWidget *menu =
	    main_dictionary_drop_down_new(settings.DictWindowModule,
					  settings.dictkey);

	g_signal_connect(menu, "deactivate",
			 G_CALLBACK(menu_deactivate_callback), widget);
	if ((event->type == GDK_BUTTON_PRESS) && event->button == 1) {
		gtk_widget_grab_focus(widget);

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),
					     TRUE);
#if GTK_CHECK_VERSION(3, 22, 0)
		gtk_menu_popup_at_widget(GTK_MENU(menu), widget, 0, 0, NULL);
#else
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
			       menu_position_under, widget, event->button,
			       event->time);
#endif

		return TRUE;
	}
	return FALSE;
}

static void
_popupmenu_requested_cb(XiphosHtml *html, gchar *uri, gpointer user_data)
{
	gui_menu_popup(html, settings.DictWindowModule, NULL);
	//gui_create_pm_dictionary();
}

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
#ifndef USE_WEBKIT2
	GtkWidget *scrolledwindow;
#endif

	UI_VBOX(box_dict, FALSE, 0);
	gtk_widget_show(box_dict);

	gtk_container_set_border_width(GTK_CONTAINER(box_dict), 1);

	UI_HBOX(hbox2, FALSE, 0);
	gtk_widget_show(hbox2);
	gtk_box_pack_start(GTK_BOX(box_dict), hbox2, FALSE, FALSE, 0);

	widgets.entry_dict = gtk_entry_new();
	gtk_widget_show(widgets.entry_dict);
	gtk_box_pack_start(GTK_BOX(hbox2), widgets.entry_dict, TRUE, TRUE,
			   0);

	dict_drop_down = gtk_toggle_button_new();
	gtk_widget_show(dict_drop_down);
	gtk_box_pack_start(GTK_BOX(hbox2), dict_drop_down, FALSE, TRUE, 0);

#if GTK_CHECK_VERSION(3, 14, 0)
	arrow1 =
	    gtk_image_new_from_icon_name("open-menu-symbolic",
					 GTK_ICON_SIZE_BUTTON);
#else
	arrow1 = gtk_arrow_new(GTK_ARROW_DOWN, GTK_SHADOW_OUT);
#endif
	gtk_widget_show(arrow1);
	gtk_container_add(GTK_CONTAINER(dict_drop_down), arrow1);

	button10 = gtk_button_new();
	gtk_widget_show(button10);
	gtk_box_pack_start(GTK_BOX(hbox2), button10, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(button10), GTK_RELIEF_NONE);

	image1 =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_image_new_from_icon_name("go-up-symbolic",
					 GTK_ICON_SIZE_BUTTON);
#else
	    gtk_image_new_from_stock(GTK_STOCK_GO_UP,
				     GTK_ICON_SIZE_BUTTON);
#endif
	gtk_widget_show(image1);
	gtk_container_add(GTK_CONTAINER(button10), image1);

	button11 = gtk_button_new();
	gtk_widget_show(button11);
	gtk_box_pack_start(GTK_BOX(hbox2), button11, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(button11), GTK_RELIEF_NONE);

	image2 =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_image_new_from_icon_name("go-down-symbolic",
					 GTK_ICON_SIZE_BUTTON);
#else
	    gtk_image_new_from_stock(GTK_STOCK_GO_DOWN,
				     GTK_ICON_SIZE_BUTTON);
#endif
	gtk_widget_show(image2);
	gtk_container_add(GTK_CONTAINER(button11), image2);

#ifndef USE_WEBKIT2
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(box_dict), scrolledwindow, TRUE, TRUE,
			   0);

	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow,
					    settings.shadow_type);
#endif

	widgets.html_dict =
	    GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, DICTIONARY_TYPE));
	gtk_widget_show(widgets.html_dict);
#ifdef USE_WEBKIT2
	gtk_box_pack_start(GTK_BOX(box_dict), widgets.html_dict, TRUE, TRUE, 0);
#else
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_dict);
#endif
	g_signal_connect((gpointer)widgets.html_dict,
			 "popupmenu_requested",
			 G_CALLBACK(_popupmenu_requested_cb), NULL);

	g_signal_connect(dict_drop_down,
			 "button_press_event",
			 G_CALLBACK(select_button_press_callback), NULL);
	g_signal_connect(G_OBJECT(widgets.entry_dict), "activate",
			 G_CALLBACK(dict_key_entry_changed), NULL);

	g_signal_connect((gpointer)button10, "clicked",
			 G_CALLBACK(button_back_clicked), NULL);
	g_signal_connect((gpointer)button11, "clicked",
			 G_CALLBACK(button_forward_clicked), NULL);
	return box_dict;
}

//******  end of file  ******/
