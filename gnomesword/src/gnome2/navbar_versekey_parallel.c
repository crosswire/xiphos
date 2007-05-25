/*
 * GnomeSword Bible Study Tool
 * navbar_versekey_parallel.c - navigation bar for the parallel dialog
 *
 * Copyright (C) 2007 GnomeSword Developer Team
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

#include "gui/navbar_versekey_parallel.h"
#include "gui/utilities.h"


#include "main/navbar_versekey.h"
#include "main/parallel_view.h"
#include "main/settings.h"


enum  {
	BOOK_BUTTON,
	CHAPTER_BUTTON,
	VERSE_BUTTON
};

#ifndef OLD_NAVBAR
NAVBAR_VERSEKEY navbar_parallel;

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

static 
gboolean select_book_button_press_callback(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer user_data)
{
	GtkWidget *menu = NULL;
	
	menu = main_versekey_drop_down_book_menu(navbar_parallel, 
						NB_PARALLEL, 
						NULL, NULL);
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

static 
gboolean select_chapter_button_press_callback(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer user_data)
{
	GtkWidget *menu = NULL;
	
	menu = main_versekey_drop_down_chapter_menu(navbar_parallel, 
						NB_PARALLEL, 
						NULL, NULL);
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
	GtkWidget *menu = NULL;
	
	menu = main_versekey_drop_down_verse_menu(navbar_parallel, 
						NB_PARALLEL, 
						NULL, NULL);
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
	const gchar *buf = gtk_entry_get_text(entry);
	if(buf == NULL)
		return;
	settings.cvparallel = (gchar*)main_get_valid_key((gchar*)buf);
	if(settings.cvparallel == NULL)
		return;
	main_navbar_versekey_set(navbar_parallel, settings.cvparallel);
	main_update_parallel_page_detached();
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
	main_navbar_versekey_spin_verse(navbar_parallel,
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
	main_navbar_versekey_spin_chapter(navbar_parallel,
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
	main_navbar_versekey_spin_book(navbar_parallel,event->scroll.direction);
	return FALSE;
}

static void sync_with_main(void)
{
	gchar *url =
	    g_strdup_printf("gnomesword.url?action=showParallel&"
				"type=verse&value=%s",
				main_url_encode(xml_get_value("keys", "verse")));
	main_url_handler(url, TRUE);
	g_free(url);
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
			gtk_widget_hide(navbar_parallel.arrow_book_up);
			gtk_widget_show(navbar_parallel.arrow_book_up_box);			
		break;
		case CHAPTER_BUTTON:
			gtk_widget_hide(navbar_parallel.arrow_chapter_up);
			gtk_widget_show(navbar_parallel.arrow_chapter_up_box);			
		break;
		case VERSE_BUTTON:
			gtk_widget_hide(navbar_parallel.arrow_verse_up);
			gtk_widget_show(navbar_parallel.arrow_verse_up_box);			
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
			gtk_widget_hide(navbar_parallel.arrow_book_down);
			gtk_widget_show(navbar_parallel.arrow_book_down_box);			
		break;
		case CHAPTER_BUTTON:
			gtk_widget_hide(navbar_parallel.arrow_chapter_down);
			gtk_widget_show(navbar_parallel.arrow_chapter_down_box);			
		break;
		case VERSE_BUTTON:
			gtk_widget_hide(navbar_parallel.arrow_verse_down);
			gtk_widget_show(navbar_parallel.arrow_verse_down_box);			
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
			gtk_widget_hide(navbar_parallel.arrow_book_up_box);
			gtk_widget_show(navbar_parallel.arrow_book_up);			
		break;
		case CHAPTER_BUTTON:
			gtk_widget_hide(navbar_parallel.arrow_chapter_up_box);
			gtk_widget_show(navbar_parallel.arrow_chapter_up);			
		break;
		case VERSE_BUTTON:
			gtk_widget_hide(navbar_parallel.arrow_verse_up_box);
			gtk_widget_show(navbar_parallel.arrow_verse_up);			
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
			gtk_widget_hide(navbar_parallel.arrow_book_down_box);
			gtk_widget_show(navbar_parallel.arrow_book_down);			
		break;
		case CHAPTER_BUTTON:
			gtk_widget_hide(navbar_parallel.arrow_chapter_down_box);
			gtk_widget_show(navbar_parallel.arrow_chapter_down);			
		break;
		case VERSE_BUTTON:
			gtk_widget_hide(navbar_parallel.arrow_verse_down_box);
			gtk_widget_show(navbar_parallel.arrow_verse_down);			
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
			main_navbar_versekey_spin_book(navbar_parallel,0);
		break;
		case CHAPTER_BUTTON:
			main_navbar_versekey_spin_chapter(navbar_parallel,0);
		break;
		case VERSE_BUTTON:
			main_navbar_versekey_spin_verse(navbar_parallel,0);
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
			main_navbar_versekey_spin_book(navbar_parallel,1);
		break;
		case CHAPTER_BUTTON:
			main_navbar_versekey_spin_chapter(navbar_parallel,1);
		break;
		case VERSE_BUTTON:
			main_navbar_versekey_spin_verse(navbar_parallel,1);
		break;
	}
	return FALSE;
}


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
			 
	/*	
	g_signal_connect((gpointer) navbar.button_book_up,
			 "clicked", G_CALLBACK(on_book_button_up_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar.button_book_down,
			 "clicked", G_CALLBACK(on_book_button_down_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar.button_chapter_up,
			 "clicked", G_CALLBACK(on_chapter_button_up_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar.button_chapter_down,
			 "clicked", G_CALLBACK(on_chapter_button_down_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar.button_verse_up,
			 "clicked", G_CALLBACK(on_verse_button_up_clicked),
			 NULL);
	g_signal_connect((gpointer) navbar.button_verse_down,
			 "clicked", G_CALLBACK(on_verse_button_down_clicked),
			 NULL);*/
			 
			 
	g_signal_connect((gpointer) navbar.button_sync,
			 "clicked", G_CALLBACK(G_CALLBACK(sync_with_main)),
			 NULL);
	g_signal_connect((gpointer) navbar.button_book_menu,
			 "button_press_event",
			 G_CALLBACK(select_book_button_press_callback), 
			 NULL);
	g_signal_connect((gpointer) navbar.button_chapter_menu,
			 "button_press_event",
			 G_CALLBACK(select_chapter_button_press_callback), 
			 NULL);
	g_signal_connect((gpointer) navbar.button_verse_menu,
			 "button_press_event",
			 G_CALLBACK(select_verse_button_press_callback), 
			 NULL);	
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
 *  
 *
 * Synopsis
 *   #include "gui/navbar_versekey_editor.h"
 *
 *  GtkWidget *
 *
 * Description
 *   create a new Bible navigation toolbar and return it
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_navbar_versekey_parallel_new(void)
{
	gchar *glade_file;
	GladeXML *gxml;
	
	glade_file =
		    gui_general_user_file("navbar_versekey.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
#ifdef DEBUG
	g_message(glade_file);
#endif

	/* build the widget */
	gxml = glade_xml_new(glade_file, "navbar", NULL);
	navbar_parallel.dialog = TRUE;
	navbar_parallel.module_name = g_string_new(settings.MainWindowModule);
	navbar_parallel.key =  g_string_new(settings.currentverse);
	
	navbar_parallel.navbar = glade_xml_get_widget(gxml, "navbar");	
	navbar_parallel.button_history_back = glade_xml_get_widget(gxml, "button_history_back");	
	navbar_parallel.button_history_next = glade_xml_get_widget(gxml, "button_history_foward");	
	navbar_parallel.button_history_menu = glade_xml_get_widget(gxml, "togglebutton_history_list");
		
	navbar_parallel.button_sync = glade_xml_get_widget(gxml, "togglebutton_sync");
	gtk_widget_show(navbar_parallel.button_sync); 
	gtk_widget_hide(navbar_parallel.button_history_back); 
	gtk_widget_hide(navbar_parallel.button_history_next); 
	gtk_widget_hide(navbar_parallel.button_history_menu); 	
/*	
	navbar_parallel.button_book_up = glade_xml_get_widget(gxml, "button_book2");
	navbar_parallel.button_book_down = glade_xml_get_widget(gxml, "button_book1");
	navbar_parallel.button_chapter_up = glade_xml_get_widget(gxml, "button_chapter2");
	navbar_parallel.button_chapter_down = glade_xml_get_widget(gxml, "button_chapter1");
	navbar_parallel.button_verse_up = glade_xml_get_widget(gxml, "button_verse2");
	navbar_parallel.button_verse_down = glade_xml_get_widget(gxml, "button_verse1");
*/	
	navbar_parallel.button_book_up = glade_xml_get_widget(gxml, "eventbox9");
	navbar_parallel.button_book_down = glade_xml_get_widget(gxml, "eventbox6");
	navbar_parallel.button_chapter_up = glade_xml_get_widget(gxml, "eventbox8");
	navbar_parallel.button_chapter_down = glade_xml_get_widget(gxml, "eventbox4");
	navbar_parallel.button_verse_up = glade_xml_get_widget(gxml, "eventbox7");
	navbar_parallel.button_verse_down = glade_xml_get_widget(gxml, "eventbox1");	
	
	navbar_parallel.arrow_book_up_box = glade_xml_get_widget(gxml, "image13");
	navbar_parallel.arrow_book_up = glade_xml_get_widget(gxml, "image12");
	navbar_parallel.arrow_book_down_box = glade_xml_get_widget(gxml, "image15");
	navbar_parallel.arrow_book_down = glade_xml_get_widget(gxml, "image14");
	navbar_parallel.arrow_chapter_up_box = glade_xml_get_widget(gxml, "image9");
	navbar_parallel.arrow_chapter_up = glade_xml_get_widget(gxml, "image8");
	navbar_parallel.arrow_chapter_down_box = glade_xml_get_widget(gxml, "image11");
	navbar_parallel.arrow_chapter_down = glade_xml_get_widget(gxml, "image10");
	navbar_parallel.arrow_verse_up_box = glade_xml_get_widget(gxml, "image7");
	navbar_parallel.arrow_verse_up = glade_xml_get_widget(gxml, "image6");
	navbar_parallel.arrow_verse_down_box = glade_xml_get_widget(gxml, "image16");
	navbar_parallel.arrow_verse_down = glade_xml_get_widget(gxml, "image5");
		
	navbar_parallel.button_book_menu = glade_xml_get_widget(gxml, "togglebutton_book");
	navbar_parallel.button_chapter_menu = glade_xml_get_widget(gxml, "togglebutton_chapter");
	navbar_parallel.button_verse_menu = glade_xml_get_widget(gxml, "togglebutton_verse");
	navbar_parallel.lookup_entry = glade_xml_get_widget(gxml, "entry_lookup");
	navbar_parallel.label_book_menu = glade_xml_get_widget(gxml, "label_book");
	navbar_parallel.label_chapter_menu = glade_xml_get_widget(gxml, "label_chapter");
	navbar_parallel.label_verse_menu = glade_xml_get_widget(gxml, "label_verse");
	navbar_parallel.book_menu = gtk_menu_new();
	navbar_parallel.chapter_menu = gtk_menu_new();
	navbar_parallel.verse_menu = gtk_menu_new();
	_connect_signals(navbar_parallel);
	
	return navbar_parallel.navbar;
}
#endif
