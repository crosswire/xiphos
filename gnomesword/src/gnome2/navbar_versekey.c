/*
 * GnomeSword Bible Study Tool
 * navbar_verse.c - navigation bar for versekey modules
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
#include <glade/glade-xml.h>

#include "gui/navbar_versekey.h"
#include "gui/bibletext_dialog.h"
#include "gui/tabbed_browser.h"
#include "gui/utilities.h"


#include "main/module_dialogs.h"
#include "main/navbar_versekey.h"
#include "main/settings.h"
#include "main/tab_history.h"


enum  {
	HISTORY_BUTTON,
	BOOK_BUTTON,
	CHAPTER_BUTTON,
	VERSE_BUTTON
};

enum  {
	MAIN,
	DIALOG
};

NAVBAR_VERSEKEY navbar_versekey;

extern PASSAGE_TAB_INFO *cur_passage_tab;


/******************************************************************************
 * Name
 *   on_verse_button_up_clicked
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   void on_verse_button_up_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static 
void on_verse_button_up_clicked(GtkButton * button, gpointer user_data)
{
	DIALOG_DATA *dialog = (DIALOG_DATA*) user_data;
	main_navbar_versekey_spin_verse((dialog)?dialog->navbar:navbar_versekey,0);
}


/******************************************************************************
 * Name
 *   on_verse_button_down_clicked
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   void on_verse_button_down_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static 
void on_verse_button_down_clicked(GtkButton * button, gpointer user_data)
{
	DIALOG_DATA *dialog = (DIALOG_DATA*)user_data;
	main_navbar_versekey_spin_verse((dialog)?dialog->navbar:navbar_versekey,1);
}


/******************************************************************************
 * Name
 *   on_chapter_button_up_clicked
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   void on_chapter_button_up_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static 
void on_chapter_button_up_clicked(GtkButton * button, gpointer user_data)
{
	DIALOG_DATA *dialog = (DIALOG_DATA*)user_data;
	main_navbar_versekey_spin_chapter((dialog)?dialog->navbar:navbar_versekey,0);
	
}


/******************************************************************************
 * Name
 *   on_chapter_button_down_clicked
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   void on_chapter_button_down_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static 
void on_chapter_button_down_clicked(GtkButton * button, gpointer user_data)
{
	DIALOG_DATA *dialog = (DIALOG_DATA*)user_data;
	main_navbar_versekey_spin_chapter((dialog)?dialog->navbar:navbar_versekey,1);
}


/******************************************************************************
 * Name
 *   on_book_button_up_clicked
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   void on_book_button_up_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static 
void on_book_button_up_clicked(GtkButton * button, gpointer user_data)
{
	DIALOG_DATA *dialog = (DIALOG_DATA*)user_data;
	main_navbar_versekey_spin_book((dialog)?dialog->navbar:navbar_versekey,0);
}


/******************************************************************************
 * Name
 *   on_book_button_down_clicked
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   void on_book_button_down_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static 
void on_book_button_down_clicked(GtkButton * button, gpointer user_data)
{
	DIALOG_DATA *dialog = (DIALOG_DATA*)user_data;
	main_navbar_versekey_spin_book((dialog)?dialog->navbar:navbar_versekey,1);
}


/******************************************************************************
 * Name
 *   menu_deactivate_callback
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   void menu_deactivate_callback (GtkWidget *widget, gpointer user_data)
 *
 * Description
 *   return toogle button to normal
 *
 * Return value
 *   void
 */

static void menu_deactivate_callback(GtkWidget * widget, gpointer user_data)
{
	GtkWidget *menu_button;

	menu_button = GTK_WIDGET(user_data);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(menu_button), FALSE);
}


/******************************************************************************
 * Name
 *   menu_position_under
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   void menu_position_under(GtkMenu * menu, int * x, int * y,
 *				gboolean * push_in, gpointer user_data)
 *
 * Description
 *   position drop down menu under toogle button
 *   
 *
 * Return value
 *   void
 */

static 
void menu_position_under(GtkMenu * menu, int * x, int * y,
				gboolean * push_in, gpointer user_data)
{
	GtkWidget *widget;

	g_return_if_fail(GTK_IS_BUTTON(user_data));
	g_return_if_fail(GTK_WIDGET_NO_WINDOW(user_data));

	widget = GTK_WIDGET(user_data);

	gdk_window_get_origin(widget->window, x, y);

	*x += widget->allocation.x;
	*y += widget->allocation.y + widget->allocation.height;

	*push_in = FALSE;
}


/******************************************************************************
 * Name
 *   select_button_press_callback
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
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

static gboolean select_button_press_callback(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer user_data)
{
	GtkWidget *menu;
	
	switch(GPOINTER_TO_INT(user_data)) {
		case HISTORY_BUTTON:
			menu = main_versekey_drop_down_new(cur_passage_tab);
		break;
		case BOOK_BUTTON:
			menu = main_versekey_drop_down_book_menu(
						navbar_versekey ,
						NULL);
		break;
		case CHAPTER_BUTTON:
			menu = main_versekey_drop_down_chapter_menu(
						navbar_versekey,
						NULL);
		break;
		case VERSE_BUTTON:
			menu = main_versekey_drop_down_verse_menu(
						navbar_versekey,
						NULL);
		break;		
	}
	if(!menu)
		return 0;		
	g_signal_connect(menu, "deactivate",
			 G_CALLBACK(menu_deactivate_callback), widget);
	if ((event->type == GDK_BUTTON_PRESS) && event->button == 1) {
		gtk_widget_grab_focus(widget);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
			       menu_position_under, widget, event->button,
			       event->time);
		return TRUE;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   select_button_press_callback
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
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

static gboolean select_book_button_press_callback(GtkWidget * widget,
					     GdkEventButton * event,
					     DIALOG_DATA * dialog)
{
	GtkWidget *menu;
	
	menu = main_versekey_drop_down_book_menu(dialog->navbar, dialog);
	if(!menu)
		return 0;		
	g_signal_connect(menu, "deactivate",
			 G_CALLBACK(menu_deactivate_callback), widget);
	if ((event->type == GDK_BUTTON_PRESS) && event->button == 1) {
		gtk_widget_grab_focus(widget);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
			       menu_position_under, widget, event->button,
			       event->time);
		return TRUE;
	}
	return FALSE;
}



/******************************************************************************
 * Name
 *   select_button_press_callback
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
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

static gboolean select_chapter_button_press_callback(GtkWidget * widget,
					     GdkEventButton * event,
					     DIALOG_DATA * dialog)
{
	GtkWidget *menu;
	
	menu = main_versekey_drop_down_chapter_menu(dialog->navbar, dialog);
	if(!menu)
		return 0;		
	g_signal_connect(menu, "deactivate",
			 G_CALLBACK(menu_deactivate_callback), widget);
	if ((event->type == GDK_BUTTON_PRESS) && event->button == 1) {
		gtk_widget_grab_focus(widget);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
			       menu_position_under, widget, event->button,
			       event->time);
		return TRUE;
	}
	return FALSE;
}



/******************************************************************************
 * Name
 *   select_button_press_callback
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
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

static gboolean select_verse_button_press_callback(GtkWidget * widget,
					     GdkEventButton * event,
					     DIALOG_DATA * dialog)
{
	GtkWidget *menu;
	
	menu = main_versekey_drop_down_verse_menu(dialog->navbar, dialog);
	if(!menu)
		return 0;		
	g_signal_connect(menu, "deactivate",
			 G_CALLBACK(menu_deactivate_callback), widget);
	if ((event->type == GDK_BUTTON_PRESS) && event->button == 1) {
		gtk_widget_grab_focus(widget);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
			       menu_position_under, widget, event->button,
			       event->time);
		return TRUE;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_button_history_next_clicked
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *  void on_button_history_next_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static
void on_button_history_next_clicked(GtkButton * button, gpointer user_data)
{
	main_navigate_tab_history(1);
}


/******************************************************************************
 * Name
 *  on_button_history_back_clicked
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *  void on_button_history_back_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static
void on_button_history_back_clicked(GtkButton * button, gpointer user_data)
{
	main_navigate_tab_history(0);
}

/******************************************************************************
 * Name
 *   on_entry_activate
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void on_entry_activate(GtkEntry * entry, DIALOG_DATA * c)	
 *
 * Description
 *   go to verse in free form entry if user hit <enter>
 *
 * Return value
 *   void
 */

static void on_entry_activate(GtkEntry * entry, gpointer user_data)
{
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	DIALOG_DATA *dialog = (DIALOG_DATA*) user_data;
	const gchar *buf = gtk_entry_get_text(entry);
	if(buf == NULL)
		return;
	const gchar *gkey = main_get_valid_key((gchar*)buf);
	if(gkey == NULL)
		return;
	gchar *url = g_strdup_printf("sword:///%s", gkey);
	if(dialog) {
		dialog->navbar.module_name = g_string_assign(dialog->navbar.module_name,settings.MainWindowModule);
		main_navbar_versekey_set(dialog->navbar, gkey);
		main_dialogs_url_handler(dialog, url, TRUE);
		
	} else {
		navbar_versekey.module_name = g_string_assign(navbar_versekey.module_name,settings.MainWindowModule);
		main_navbar_versekey_set(navbar_versekey, gkey);
		main_url_handler(url, TRUE);
	}
	if(url)
		g_free(url);
	if(gkey)
		g_free((gchar*)gkey);
}


/******************************************************************************
 * Name
 *  on_button_verse_menu_verse_scroll_event
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *  gboolean on_button_verse_menu_verse_scroll_event(GtkWidget * widget,
 *                                           GdkEvent * event,
 *                                           gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static
gboolean on_button_verse_menu_verse_scroll_event(GtkWidget * widget,
                                            GdkEvent * event,
                                            gpointer user_data)
{
	DIALOG_DATA *dialog = (DIALOG_DATA*)user_data;
	main_navbar_versekey_spin_verse((dialog)?dialog->navbar:navbar_versekey,
					event->scroll.direction);
  return FALSE;
}



/******************************************************************************
 * Name
 *  on_button_verse_menu_chapter_scroll_event
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *  gboolean on_button_verse_menu_chapter_scroll_event(GtkWidget * widget,
 *                                           GdkEvent * event,
 *                                           gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static
gboolean on_button_verse_menu_chapter_scroll_event(GtkWidget * widget,
                                            GdkEvent * event,
                                            gpointer user_data)
{
	DIALOG_DATA *dialog = (DIALOG_DATA*)user_data;
	main_navbar_versekey_spin_chapter((dialog)?dialog->navbar:navbar_versekey,
					event->scroll.direction);
	return FALSE;
}



/******************************************************************************
 * Name
 *  on_button_verse_menu_book_scroll_event
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *  gboolean on_button_verse_menu_book_scroll_event(GtkWidget * widget,
 *                                           GdkEvent * event,
 *                                           gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static
gboolean on_button_verse_menu_book_scroll_event(GtkWidget * widget,
                                            GdkEvent * event,
                                            gpointer user_data)
{
	DIALOG_DATA *dialog = (DIALOG_DATA*)user_data;
	main_navbar_versekey_spin_book((dialog)?dialog->navbar:navbar_versekey,
					event->scroll.direction);
	return FALSE;
}


/******************************************************************************
 * Name
 *  gui_navbar_versekey_new
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *  GtkWidget *gui_navbar_book_new(void)
 *
 * Description
 *   create a new Bible navigation toolbar and return it
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_navbar_versekey_new(void)
{

	gchar *glade_file;
	GladeXML *gxml;
	GtkWidget *navbar;
	
	glade_file =
		    gui_general_user_file("navbar_versekey.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
#ifdef DEBUG
	g_message(glade_file);
#endif

	/* build the widget */
	gxml = glade_xml_new(glade_file, "navbar", NULL);
	navbar_versekey.dialog = FALSE;
	navbar_versekey.module_name = g_string_new(settings.MainWindowModule);
	navbar_versekey.key =  g_string_new(settings.currentverse);
	
	navbar_versekey.navbar = glade_xml_get_widget(gxml, "navbar");
	
	navbar_versekey.button_history_back = glade_xml_get_widget(gxml, "button_history_back");
	navbar_versekey.button_history_next = glade_xml_get_widget(gxml, "button_history_foward");
	navbar_versekey.button_history_menu = glade_xml_get_widget(gxml, "togglebutton_history_list");
	navbar_versekey.button_book_up = glade_xml_get_widget(gxml, "button_book2");
	navbar_versekey.button_book_down = glade_xml_get_widget(gxml, "button_book1");
	navbar_versekey.button_chapter_up = glade_xml_get_widget(gxml, "button_chapter2");
	navbar_versekey.button_chapter_down = glade_xml_get_widget(gxml, "button_chapter1");
	navbar_versekey.button_verse_up = glade_xml_get_widget(gxml, "button_verse2");
	navbar_versekey.button_verse_down = glade_xml_get_widget(gxml, "button_verse1");
	navbar_versekey.button_book_menu = glade_xml_get_widget(gxml, "togglebutton_book");
	navbar_versekey.button_chapter_menu = glade_xml_get_widget(gxml, "togglebutton_chapter");
	navbar_versekey.button_verse_menu = glade_xml_get_widget(gxml, "togglebutton_verse");
	navbar_versekey.lookup_entry = glade_xml_get_widget(gxml, "entry_lookup");
	navbar_versekey.label_book_menu = glade_xml_get_widget(gxml, "label_book");
	navbar_versekey.label_chapter_menu = glade_xml_get_widget(gxml, "label_chapter");
	navbar_versekey.label_verse_menu = glade_xml_get_widget(gxml, "label_verse");
	navbar_versekey.book_menu = gtk_menu_new();
	navbar_versekey.chapter_menu = gtk_menu_new();
	navbar_versekey.verse_menu = gtk_menu_new();
	
	g_signal_connect((gpointer) navbar_versekey.lookup_entry,
				 "activate", G_CALLBACK(on_entry_activate),
				 NULL);
		
	g_signal_connect((gpointer) navbar_versekey.button_book_up,
			 "clicked", G_CALLBACK(on_book_button_up_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar_versekey.button_book_down,
			 "clicked", G_CALLBACK(on_book_button_down_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar_versekey.button_chapter_up,
			 "clicked", G_CALLBACK(on_chapter_button_up_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar_versekey.button_chapter_down,
			 "clicked", G_CALLBACK(on_chapter_button_down_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar_versekey.button_verse_up,
			 "clicked", G_CALLBACK(on_verse_button_up_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar_versekey.button_verse_down,
			 "clicked", G_CALLBACK(on_verse_button_down_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar_versekey.button_history_back,
			 "clicked", G_CALLBACK(on_button_history_back_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar_versekey.button_history_next,
			 "clicked", G_CALLBACK(on_button_history_next_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar_versekey.button_history_menu,
			 "button_press_event",
			 G_CALLBACK(select_button_press_callback), 
			 GINT_TO_POINTER(HISTORY_BUTTON));
	g_signal_connect((gpointer) navbar_versekey.button_book_menu,
			 "button_press_event",
			 G_CALLBACK(select_button_press_callback), 
			 GINT_TO_POINTER(BOOK_BUTTON));
	g_signal_connect((gpointer) navbar_versekey.button_chapter_menu,
			 "button_press_event",
			 G_CALLBACK(select_button_press_callback), 
			 GINT_TO_POINTER(CHAPTER_BUTTON));
	g_signal_connect((gpointer) navbar_versekey.button_verse_menu,
			 "button_press_event",
			 G_CALLBACK(select_button_press_callback), 
			 GINT_TO_POINTER(VERSE_BUTTON));			 
	g_signal_connect ((gpointer)navbar_versekey.button_verse_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_verse_scroll_event),
		    	 NULL);			 
	g_signal_connect ((gpointer)navbar_versekey.button_chapter_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_chapter_scroll_event),
		    	 NULL);			 
	g_signal_connect ((gpointer)navbar_versekey.button_book_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_book_scroll_event),
		    	 NULL);
	return navbar_versekey.navbar;
	
}

/******************************************************************************
 * Name
 *  gui_navbar_versekey_new
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *  GtkWidget *gui_navbar_book_new(void)
 *
 * Description
 *   create a new Bible navigation toolbar and return it
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_navbar_versekey_dialog_new(gpointer d)
{

	gchar *glade_file;
	GladeXML *gxml;
	GtkWidget *navbar;
	//NAVBAR_VERSEKEY 
	DIALOG_DATA *dialog = (DIALOG_DATA *)d;
	glade_file =
		    gui_general_user_file("navbar_versekey.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
#ifdef DEBUG
	g_message(glade_file);
#endif

	/* build the widget */
	gxml = glade_xml_new(glade_file, "navbar", NULL);
	dialog->navbar.dialog = TRUE;
	dialog->navbar.module_name = g_string_new(settings.MainWindowModule);
	dialog->navbar.key =  g_string_new(settings.currentverse);
	
	dialog->navbar.navbar = glade_xml_get_widget(gxml, "navbar");
	
	dialog->navbar.button_sync = glade_xml_get_widget(gxml, "togglebutton_sync");
	gtk_widget_show(dialog->navbar.button_sync); 
	
	dialog->navbar.button_history_back = glade_xml_get_widget(gxml, "button_history_back");
	gtk_widget_hide(dialog->navbar.button_history_back); 
	
	dialog->navbar.button_history_next = glade_xml_get_widget(gxml, "button_history_foward");
	gtk_widget_hide(dialog->navbar.button_history_next); 
	
	dialog->navbar.button_history_menu = glade_xml_get_widget(gxml, "togglebutton_history_list");
	gtk_widget_hide(dialog->navbar.button_history_menu); 
	
	dialog->navbar.button_book_up = glade_xml_get_widget(gxml, "button_book2");
	dialog->navbar.button_book_down = glade_xml_get_widget(gxml, "button_book1");
	dialog->navbar.button_chapter_up = glade_xml_get_widget(gxml, "button_chapter2");
	dialog->navbar.button_chapter_down = glade_xml_get_widget(gxml, "button_chapter1");
	dialog->navbar.button_verse_up = glade_xml_get_widget(gxml, "button_verse2");
	dialog->navbar.button_verse_down = glade_xml_get_widget(gxml, "button_verse1");
	dialog->navbar.button_book_menu = glade_xml_get_widget(gxml, "togglebutton_book");
	dialog->navbar.button_chapter_menu = glade_xml_get_widget(gxml, "togglebutton_chapter");
	dialog->navbar.button_verse_menu = glade_xml_get_widget(gxml, "togglebutton_verse");
	dialog->navbar.lookup_entry = glade_xml_get_widget(gxml, "entry_lookup");
	dialog->navbar.label_book_menu = glade_xml_get_widget(gxml, "label_book");
	dialog->navbar.label_chapter_menu = glade_xml_get_widget(gxml, "label_chapter");
	dialog->navbar.label_verse_menu = glade_xml_get_widget(gxml, "label_verse");
	dialog->navbar.book_menu = gtk_menu_new();
	dialog->navbar.chapter_menu = gtk_menu_new();
	dialog->navbar.verse_menu = gtk_menu_new();
	
	g_signal_connect((gpointer) dialog->navbar.button_sync,
			 "clicked", G_CALLBACK(gui_bible_dialog_sync_toggled),
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.lookup_entry,
				 "activate", G_CALLBACK(on_entry_activate),
				 dialog);
		
	g_signal_connect((gpointer) dialog->navbar.button_book_up,
			 "clicked", G_CALLBACK(on_book_button_up_clicked),
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.button_book_down,
			 "clicked", G_CALLBACK(on_book_button_down_clicked),
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.button_chapter_up,
			 "clicked", G_CALLBACK(on_chapter_button_up_clicked),
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.button_chapter_down,
			 "clicked", G_CALLBACK(on_chapter_button_down_clicked),
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.button_verse_up,
			 "clicked", G_CALLBACK(on_verse_button_up_clicked),
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.button_verse_down,
			 "clicked", G_CALLBACK(on_verse_button_down_clicked),
			 dialog);/*
	g_signal_connect((gpointer) dialog->navbar.button_history_back,
			 "clicked", G_CALLBACK(on_button_history_back_clicked),
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.button_history_next,
			 "clicked", G_CALLBACK(on_button_history_next_clicked),
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.button_history_menu,
			 "button_press_event",
			 G_CALLBACK(select_button_press_callback), 
			 GINT_TO_POINTER(HISTORY_BUTTON));*/
	g_signal_connect((gpointer) dialog->navbar.button_book_menu,
			 "button_press_event",
			 G_CALLBACK(select_book_button_press_callback), 
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.button_chapter_menu,
			 "button_press_event",
			 G_CALLBACK(select_chapter_button_press_callback), 
			 dialog);
	g_signal_connect((gpointer) dialog->navbar.button_verse_menu,
			 "button_press_event",
			 G_CALLBACK(select_verse_button_press_callback), 
			 dialog);			 
	g_signal_connect ((gpointer)dialog->navbar.button_verse_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_verse_scroll_event),
		    	 dialog);			 
	g_signal_connect ((gpointer)dialog->navbar.button_chapter_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_chapter_scroll_event),
		    	 dialog);			 
	g_signal_connect ((gpointer)dialog->navbar.button_book_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_book_scroll_event),
		    	 dialog);
	return dialog->navbar.navbar;
	
}
