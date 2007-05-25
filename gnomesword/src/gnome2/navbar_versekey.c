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

#include "editor/html-editor.h"

#include "gui/navbar_versekey.h"
#include "gui/bibletext_dialog.h"
#include "gui/tabbed_browser.h"
#include "gui/utilities.h"


#include "main/module_dialogs.h"
#include "main/navbar_versekey.h"
#include "main/settings.h"
#include "main/tab_history.h"


enum  {
	BOOK_BUTTON,
	CHAPTER_BUTTON,
	VERSE_BUTTON
};


NAVBAR_VERSEKEY navbar_versekey;

extern PASSAGE_TAB_INFO *cur_passage_tab;

#ifndef OLD_NAVBAR


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
	
	menu = main_versekey_drop_down_new(cur_passage_tab);
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
					     gpointer user_data)
{
	GtkWidget *menu;	
	
	menu = main_versekey_drop_down_book_menu(navbar_versekey, NB_MAIN,
						NULL,NULL);
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
					     gpointer user_data)
{
	GtkWidget *menu;
	
	menu = main_versekey_drop_down_chapter_menu(navbar_versekey, NB_MAIN,
						NULL,NULL);
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
					     gpointer user_data)
{
	GtkWidget *menu;
	
	menu = main_versekey_drop_down_verse_menu(navbar_versekey, NB_MAIN,
						NULL,NULL);
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
	DIALOG_DATA *dialog;
	
	const gchar *buf = gtk_entry_get_text(entry);
	if(buf == NULL)
		return;
	const gchar *gkey = main_get_valid_key((gchar*)buf);
	if(gkey == NULL)
		return;
	gchar *url = g_strdup_printf("sword:///%s", gkey);

	navbar_versekey.module_name = g_string_assign(navbar_versekey.module_name,settings.MainWindowModule);
	main_navbar_versekey_set(navbar_versekey, gkey);
	main_url_handler(url, TRUE);
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
	main_navbar_versekey_spin_verse(navbar_versekey, event->scroll.direction);
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
	main_navbar_versekey_spin_chapter(navbar_versekey,
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
	main_navbar_versekey_spin_book(navbar_versekey,
				event->scroll.direction);
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_up_enter_notify_event
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   gboolean on_up_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
 *                                       gpointer  user_data)
 *
 * Description
 *   mimic a button by hiding/showing arrow pixmaps when mouse enters spin
 *   buttons
 *
 * Return value
 *   gboolean
 */

static 
gboolean on_up_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        gpointer  user_data)
{
	switch(GPOINTER_TO_INT(user_data)) {
		case BOOK_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_book_up);
			gtk_widget_show(navbar_versekey.arrow_book_up_box);			
		break;
		case CHAPTER_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_chapter_up);
			gtk_widget_show(navbar_versekey.arrow_chapter_up_box);			
		break;
		case VERSE_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_verse_up);
			gtk_widget_show(navbar_versekey.arrow_verse_up_box);			
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_down_enter_notify_event
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   gboolean on_down_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        gpointer user_data)
 *
 * Description
 *   mimic a button by hiding/showing arrow pixmaps when mouse enters spin
 *   buttons
 *
 * Return value
 *   gboolean
 */

static 
gboolean on_down_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        gpointer user_data)
{
	switch(GPOINTER_TO_INT(user_data)) {
		case BOOK_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_book_down);
			gtk_widget_show(navbar_versekey.arrow_book_down_box);			
		break;
		case CHAPTER_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_chapter_down);
			gtk_widget_show(navbar_versekey.arrow_chapter_down_box);			
		break;
		case VERSE_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_verse_down);
			gtk_widget_show(navbar_versekey.arrow_verse_down_box);			
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_up_leave_notify_event
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   gboolean on_up_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        gpointer user_data)
 *
 * Description
 *   mimic a button by hiding/showing arrow pixmaps when mouse leaves spin
 *   buttons
 *
 * Return value
 *   gboolean
 */

static   
gboolean on_up_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        gpointer user_data)
{
	switch(GPOINTER_TO_INT(user_data)) {
		case BOOK_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_book_up_box);
			gtk_widget_show(navbar_versekey.arrow_book_up);			
		break;
		case CHAPTER_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_chapter_up_box);
			gtk_widget_show(navbar_versekey.arrow_chapter_up);			
		break;
		case VERSE_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_verse_up_box);
			gtk_widget_show(navbar_versekey.arrow_verse_up);			
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_down_leave_notify_event
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   gboolean on_down_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        gpointer user_data)
 *
 * Description
 *   mimic a button by hiding/showing arrow pixmaps when mouse leaves spin
 *   buttons
 *
 * Return value
 *   gboolean
 */

static 
gboolean on_down_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        gpointer user_data)
{
	switch(GPOINTER_TO_INT(user_data)) {
		case BOOK_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_book_down_box);
			gtk_widget_show(navbar_versekey.arrow_book_down);			
		break;
		case CHAPTER_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_chapter_down_box);
			gtk_widget_show(navbar_versekey.arrow_chapter_down);			
		break;
		case VERSE_BUTTON:
			gtk_widget_hide(navbar_versekey.arrow_verse_down_box);
			gtk_widget_show(navbar_versekey.arrow_verse_down);			
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_up_eventbox_button_release_event
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   gboolean on_up_eventbox_button_release_event (GtkWidget * widget,
 *                                       	GdkEventButton * event,
 *                                       	gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static 
gboolean on_up_eventbox_button_release_event (GtkWidget * widget,
                                        	GdkEventButton * event,
                                        	gpointer user_data)
{
	switch(GPOINTER_TO_INT(user_data)) {
		case BOOK_BUTTON:
			main_navbar_versekey_spin_book(navbar_versekey,0);
		break;
		case CHAPTER_BUTTON:
			main_navbar_versekey_spin_chapter(navbar_versekey,0);
		break;
		case VERSE_BUTTON:
			main_navbar_versekey_spin_verse(navbar_versekey,0);
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_down_eventbox_button_release_event
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   gboolean on_down_eventbox_button_release_event(GtkWidget * widget,
 *                                      	GdkEventButton * event,
 *                                      	gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static 
gboolean on_down_eventbox_button_release_event(GtkWidget * widget,
                                       	GdkEventButton * event,
                                       	gpointer user_data)
{
	switch(GPOINTER_TO_INT(user_data)) {
		case BOOK_BUTTON:
			main_navbar_versekey_spin_book(navbar_versekey,1);
		break;
		case CHAPTER_BUTTON:
			main_navbar_versekey_spin_chapter(navbar_versekey,1);
		break;
		case VERSE_BUTTON:
			main_navbar_versekey_spin_verse(navbar_versekey,1);
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   _connect_signals
 *
 * Synopsis
 *   #include "gui/navbar_versekey.h"
 *
 *   void _connect_signals(NAVBAR_VERSEKEY navbar)
 *
 * Description
 *   
 *
 * Return value
 *  void 
 */

static
void _connect_signals(NAVBAR_VERSEKEY navbar)
{
		
	g_signal_connect((gpointer) navbar.lookup_entry,
			 "activate", G_CALLBACK(on_entry_activate),
			 NULL);	
	g_signal_connect((gpointer) navbar.button_book_up,
			 "button_release_event", 
			G_CALLBACK(on_up_eventbox_button_release_event),
			 GINT_TO_POINTER(BOOK_BUTTON));
	g_signal_connect((gpointer) navbar.button_book_down,
			 "button_release_event", 
			G_CALLBACK(on_down_eventbox_button_release_event),
			 GINT_TO_POINTER(BOOK_BUTTON));
	g_signal_connect((gpointer) navbar.button_chapter_up,
			 "button_release_event", 
			G_CALLBACK(on_up_eventbox_button_release_event),
			GINT_TO_POINTER(CHAPTER_BUTTON) );
	g_signal_connect((gpointer) navbar.button_chapter_down,
			 "button_release_event",
			G_CALLBACK(on_down_eventbox_button_release_event),
			GINT_TO_POINTER(CHAPTER_BUTTON) );
	g_signal_connect((gpointer) navbar.button_verse_up,
			 "button_release_event", 
			 G_CALLBACK(on_up_eventbox_button_release_event),
			GINT_TO_POINTER(VERSE_BUTTON) );
	g_signal_connect((gpointer) navbar.button_verse_down,
			 "button_release_event", 
			 G_CALLBACK(on_down_eventbox_button_release_event),
			GINT_TO_POINTER(VERSE_BUTTON) );
	/*     */			
	g_signal_connect ((gpointer) navbar.button_book_up,
			"enter_notify_event",
		    	G_CALLBACK (on_up_enter_notify_event),
		    	GINT_TO_POINTER(BOOK_BUTTON));
	g_signal_connect ((gpointer) navbar.button_book_up,
			"leave_notify_event",
		    	G_CALLBACK (on_up_leave_notify_event),
		    	GINT_TO_POINTER(BOOK_BUTTON));
		    
	g_signal_connect ((gpointer)navbar.button_book_down , 
			"enter_notify_event",
		    	G_CALLBACK (on_down_enter_notify_event),
		    	GINT_TO_POINTER(BOOK_BUTTON));
	g_signal_connect ((gpointer)navbar.button_book_down ,
			"leave_notify_event",
		    	G_CALLBACK (on_down_leave_notify_event),
		    	GINT_TO_POINTER(BOOK_BUTTON));
		    
	/*    */		    
	g_signal_connect ((gpointer) navbar.button_chapter_up, "enter_notify_event",
		    G_CALLBACK (on_up_enter_notify_event),
		    GINT_TO_POINTER(CHAPTER_BUTTON));
	g_signal_connect ((gpointer)navbar.button_chapter_up , "leave_notify_event",
		    G_CALLBACK (on_up_leave_notify_event),
		    GINT_TO_POINTER(CHAPTER_BUTTON));	
	
	g_signal_connect ((gpointer) navbar.button_chapter_down, "enter_notify_event",
		    G_CALLBACK (on_down_enter_notify_event),
		    GINT_TO_POINTER(CHAPTER_BUTTON));
	g_signal_connect ((gpointer)navbar.button_chapter_down , "leave_notify_event",
		    G_CALLBACK (on_down_leave_notify_event),
		    GINT_TO_POINTER(CHAPTER_BUTTON));	
	
	/*    */		    
	g_signal_connect ((gpointer) navbar.button_verse_up, "enter_notify_event",
		    G_CALLBACK (on_up_enter_notify_event),
		    GINT_TO_POINTER(VERSE_BUTTON));
	g_signal_connect ((gpointer)navbar.button_verse_up , "leave_notify_event",
		    G_CALLBACK (on_up_leave_notify_event),
		    GINT_TO_POINTER(VERSE_BUTTON));
		    
	g_signal_connect ((gpointer) navbar.button_verse_down, "enter_notify_event",
		    G_CALLBACK (on_down_enter_notify_event),
		    GINT_TO_POINTER(VERSE_BUTTON));
	g_signal_connect ((gpointer)navbar.button_verse_down , "leave_notify_event",
		    G_CALLBACK (on_down_leave_notify_event),
		    GINT_TO_POINTER(VERSE_BUTTON));
			 
	g_signal_connect((gpointer) navbar.button_history_back,
			 "clicked", G_CALLBACK(on_button_history_back_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar.button_history_next,
			 "clicked", G_CALLBACK(on_button_history_next_clicked),
			 NULL);	
	g_signal_connect((gpointer) navbar.button_history_menu,
			 "button_press_event",
			 G_CALLBACK(select_button_press_callback), 
			 NULL);
	g_signal_connect((gpointer) navbar.button_book_menu,
			 "button_press_event",
			 G_CALLBACK(select_book_button_press_callback), 
			 NULL);
	g_signal_connect((gpointer) navbar.button_chapter_menu,
			 "button_press_event",
			 G_CALLBACK(select_chapter_button_press_callback), 
			NULL );
	g_signal_connect((gpointer) navbar.button_verse_menu,
			 "button_press_event",
			 G_CALLBACK(select_verse_button_press_callback), 
			NULL );	
	g_signal_connect ((gpointer)navbar.button_verse_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_verse_scroll_event),
		    	 NULL);			 
	g_signal_connect ((gpointer)navbar.button_chapter_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_chapter_scroll_event),
		    	 NULL);			 
	g_signal_connect ((gpointer)navbar.button_book_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_book_scroll_event),
		    	 NULL);	 
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
	
	glade_file = gui_general_user_file("navbar_versekey.glade", FALSE);
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
	
	navbar_versekey.button_book_up = glade_xml_get_widget(gxml, "eventbox9");
	navbar_versekey.button_book_down = glade_xml_get_widget(gxml, "eventbox6");
	navbar_versekey.button_chapter_up = glade_xml_get_widget(gxml, "eventbox8");
	navbar_versekey.button_chapter_down = glade_xml_get_widget(gxml, "eventbox4");
	navbar_versekey.button_verse_up = glade_xml_get_widget(gxml, "eventbox7");
	navbar_versekey.button_verse_down = glade_xml_get_widget(gxml, "eventbox1");	
	
	navbar_versekey.arrow_book_up_box = glade_xml_get_widget(gxml, "image13");
	navbar_versekey.arrow_book_up = glade_xml_get_widget(gxml, "image12");
	navbar_versekey.arrow_book_down_box = glade_xml_get_widget(gxml, "image15");
	navbar_versekey.arrow_book_down = glade_xml_get_widget(gxml, "image14");
	navbar_versekey.arrow_chapter_up_box = glade_xml_get_widget(gxml, "image9");
	navbar_versekey.arrow_chapter_up = glade_xml_get_widget(gxml, "image8");
	navbar_versekey.arrow_chapter_down_box = glade_xml_get_widget(gxml, "image11");
	navbar_versekey.arrow_chapter_down = glade_xml_get_widget(gxml, "image10");
	navbar_versekey.arrow_verse_up_box = glade_xml_get_widget(gxml, "image7");
	navbar_versekey.arrow_verse_up = glade_xml_get_widget(gxml, "image6");
	navbar_versekey.arrow_verse_down_box = glade_xml_get_widget(gxml, "image16");
	navbar_versekey.arrow_verse_down = glade_xml_get_widget(gxml, "image5");
	
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
	_connect_signals(navbar_versekey);

	return navbar_versekey.navbar;
}
#endif
