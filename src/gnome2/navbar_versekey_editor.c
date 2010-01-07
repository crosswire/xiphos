/*
 * Xiphos Bible Study Tool
 * navbar_verse_dialog.c - navigation bar for versekey modules in dialogs
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
#include <glade/glade-xml.h>


#include "editor/slib-editor.h"

#include "gui/navbar_versekey_editor.h"
#include "gui/bibletext_dialog.h"
#include "gui/tabbed_browser.h"
#include "gui/utilities.h"

//#include "main/module_dialogs.h"
#include "main/navbar_versekey.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/tab_history.h"

#include "gui/debug_glib_null.h"


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
gboolean on_verse_up_button_release_event(GtkWidget * widget,
                                       	GdkEventButton * event,
                                       	EDITOR * editor)
{
	main_navbar_versekey_spin_verse(editor->navbar, 0);
	return FALSE;
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
gboolean on_verse_down_button_release_event(GtkWidget * widget,
                                       	GdkEventButton * event,
                                       	EDITOR * editor)
{
	main_navbar_versekey_spin_verse(editor->navbar, 1);
	return FALSE;
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
gboolean on_chapter_up_button_release_event(GtkWidget * widget,
                                       	GdkEventButton * event,
                                       	EDITOR * editor)
{
	main_navbar_versekey_spin_chapter(editor->navbar, 0);
	return FALSE;
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
gboolean on_chapter_down_button_release_event(GtkWidget * widget,
                                       	GdkEventButton * event,
                                       EDITOR * editor)
{
	main_navbar_versekey_spin_chapter(editor->navbar, 1);
	return FALSE;
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
gboolean on_book_up_button_release_event(GtkWidget * widget,
                                       	GdkEventButton * event,
                                       	EDITOR * editor)
{
	main_navbar_versekey_spin_book(editor->navbar, 0);
	return FALSE;
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
gboolean on_book_down_button_release_event(GtkWidget * widget,
                                       	GdkEventButton * event,
                                       	EDITOR * editor)
{
	main_navbar_versekey_spin_book(editor->navbar, 1);
	return FALSE;
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

static 
gboolean select_book_button_press_callback(GtkWidget * widget,
					     GdkEventButton * event,
					     EDITOR * editor)
{
	GtkWidget *menu;
	
	menu = main_versekey_drop_down_book_menu(editor->navbar, NB_EDITOR, NULL, editor);
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
					     EDITOR * editor)
{
	GtkWidget *menu;
	
	menu = main_versekey_drop_down_chapter_menu(editor->navbar, NB_EDITOR, NULL, editor);
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
					     EDITOR * editor)
{
	GtkWidget *menu;
	
	menu = main_versekey_drop_down_verse_menu(editor->navbar, NB_EDITOR, NULL, editor);
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

static void on_entry_activate(GtkEntry * entry, EDITOR * editor)
{
	const gchar *buf = gtk_entry_get_text(entry);
	if(buf == NULL)
		return;
	const gchar *gkey = main_get_valid_key((gchar*)buf);
	if(gkey == NULL)
		return;
	gchar *url = g_strdup_printf("sword:///%s", gkey);
	
	editor->navbar.module_name = g_string_assign(editor->navbar.module_name,settings.MainWindowModule);
	main_navbar_versekey_set(editor->navbar, gkey);
	
	editor_load_note(editor, NULL, gkey);

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
                                            EDITOR * editor)
{
	main_navbar_versekey_spin_verse(editor->navbar,
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
                                            EDITOR * editor)
{
	main_navbar_versekey_spin_chapter(editor->navbar,
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
                                            EDITOR * editor)
{
	main_navbar_versekey_spin_book(editor->navbar,event->scroll.direction);
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
gboolean on_book_up_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_book_up_box);
	gtk_widget_show(editor->navbar.arrow_book_up);	
	return FALSE;
}

static   
gboolean on_chapter_up_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_chapter_up_box);
	gtk_widget_show(editor->navbar.arrow_chapter_up);	
	return FALSE;
}

static   
gboolean on_verse_up_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_verse_up_box);
	gtk_widget_show(editor->navbar.arrow_verse_up);	
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
gboolean on_book_down_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	
	gtk_widget_hide(editor->navbar.arrow_book_down_box);
	gtk_widget_show(editor->navbar.arrow_book_down);
	return FALSE;
}

static 
gboolean on_chapter_down_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_chapter_down_box);
	gtk_widget_show(editor->navbar.arrow_chapter_down);
	return FALSE;
}

static 
gboolean on_verse_down_leave_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_verse_down_box);
	gtk_widget_show(editor->navbar.arrow_verse_down);
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
gboolean on_book_up_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_book_up);
	gtk_widget_show(editor->navbar.arrow_book_up_box);
	return FALSE;
}


static 
gboolean on_chapter_up_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_chapter_up);
	gtk_widget_show(editor->navbar.arrow_chapter_up_box);
	return FALSE;
}

static 
gboolean on_verse_up_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_verse_up);
	gtk_widget_show(editor->navbar.arrow_verse_up_box);
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
gboolean on_book_down_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_book_down);
	gtk_widget_show(editor->navbar.arrow_book_down_box);
	return FALSE;
}

static 
gboolean on_chapter_down_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_chapter_down);
	gtk_widget_show(editor->navbar.arrow_chapter_down_box);
	return FALSE;
}

static 
gboolean on_verse_down_enter_notify_event(GtkWidget * widget, GdkEventCrossing * event,
                                        EDITOR * editor)
{
	gtk_widget_hide(editor->navbar.arrow_verse_down);
	gtk_widget_show(editor->navbar.arrow_verse_down_box);
	return FALSE;
}


static
void _connect_signals(NAVBAR_VERSEKEY navbar, EDITOR * editor)
{		
	g_signal_connect((gpointer) navbar.lookup_entry,
			 "activate", G_CALLBACK(on_entry_activate),
			 editor);
	
	g_signal_connect((gpointer) navbar.button_book_up,
			 "button_release_event", 
			 G_CALLBACK(on_book_up_button_release_event),
			 editor);
	g_signal_connect((gpointer) navbar.button_book_down,
			 "button_release_event", 
			 G_CALLBACK(on_book_down_button_release_event),
			 editor);
	g_signal_connect((gpointer) navbar.button_chapter_up,
			 "button_release_event", 
			 G_CALLBACK(on_chapter_up_button_release_event),
			 editor);
	g_signal_connect((gpointer) navbar.button_chapter_down,
			 "button_release_event", 
			 G_CALLBACK(on_chapter_down_button_release_event),
			 editor);
	g_signal_connect((gpointer) navbar.button_verse_up,
			 "button_release_event", 
			 G_CALLBACK(on_verse_up_button_release_event),
			 editor );
	g_signal_connect((gpointer) navbar.button_verse_down,
			 "button_release_event", 
			 G_CALLBACK(on_verse_down_button_release_event),
			 editor);
			 
	/*     */			
	g_signal_connect ((gpointer) navbar.button_book_up,
			"enter_notify_event",
		    	G_CALLBACK (on_book_up_enter_notify_event),
		    	editor);
	g_signal_connect ((gpointer) navbar.button_book_up,
			"leave_notify_event",
		    	G_CALLBACK (on_book_up_leave_notify_event),
		    	editor);
		    
	g_signal_connect ((gpointer)navbar.button_book_down , 
			"enter_notify_event",
		    	G_CALLBACK (on_book_down_enter_notify_event),
		    	editor);
	g_signal_connect ((gpointer)navbar.button_book_down ,
			"leave_notify_event",
		    	G_CALLBACK (on_book_down_leave_notify_event),
		    	editor);
		    
	/*    */		    
	g_signal_connect ((gpointer) navbar.button_chapter_up, "enter_notify_event",
		    G_CALLBACK (on_chapter_up_enter_notify_event),
		    editor);
	g_signal_connect ((gpointer)navbar.button_chapter_up , "leave_notify_event",
		    G_CALLBACK (on_chapter_up_leave_notify_event),
		    editor);	
	
	g_signal_connect ((gpointer) navbar.button_chapter_down, "enter_notify_event",
		    G_CALLBACK (on_chapter_down_enter_notify_event),
		    editor);
	g_signal_connect ((gpointer)navbar.button_chapter_down , "leave_notify_event",
		    G_CALLBACK (on_chapter_down_leave_notify_event),
		    editor);	
	
	/*    */		    
	g_signal_connect ((gpointer) navbar.button_verse_up, "enter_notify_event",
		    G_CALLBACK (on_verse_up_enter_notify_event),
		    editor);
	g_signal_connect ((gpointer)navbar.button_verse_up , "leave_notify_event",
		    G_CALLBACK (on_verse_up_leave_notify_event),
		    editor);
		    
	g_signal_connect ((gpointer) navbar.button_verse_down, "enter_notify_event",
		    G_CALLBACK (on_verse_down_enter_notify_event),
		    editor);
	g_signal_connect ((gpointer)navbar.button_verse_down , "leave_notify_event",
		    G_CALLBACK (on_verse_down_leave_notify_event),
		    editor);		 
			 
			 
/*			
	g_signal_connect((gpointer) navbar.button_book_up,
			 "clicked", G_CALLBACK(on_book_button_up_clicked),
			 editor);
	g_signal_connect((gpointer) navbar.button_book_down,
			 "clicked", G_CALLBACK(on_book_button_down_clicked),
			 editor);
	g_signal_connect((gpointer) navbar.button_chapter_up,
			 "clicked", G_CALLBACK(on_chapter_button_up_clicked),
			 editor);
	g_signal_connect((gpointer) navbar.button_chapter_down,
			 "clicked", G_CALLBACK(on_chapter_button_down_clicked),
			 editor);
	g_signal_connect((gpointer) navbar.button_verse_up,
			 "clicked", G_CALLBACK(on_verse_button_up_clicked),
			 editor);
	g_signal_connect((gpointer) navbar.button_verse_down,
			 "clicked", G_CALLBACK(on_verse_button_down_clicked),
			 editor);
			 */
	g_signal_connect((gpointer) navbar.button_book_menu,
			 "button_press_event",
			 G_CALLBACK(select_book_button_press_callback), 
			 editor);
	g_signal_connect((gpointer) navbar.button_chapter_menu,
			 "button_press_event",
			 G_CALLBACK(select_chapter_button_press_callback), 
			 editor);
	g_signal_connect((gpointer) navbar.button_verse_menu,
			 "button_press_event",
			 G_CALLBACK(select_verse_button_press_callback), 
			 editor);	
	g_signal_connect ((gpointer)navbar.button_verse_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_verse_scroll_event),
		    	 editor);			 
	g_signal_connect ((gpointer)navbar.button_chapter_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_chapter_scroll_event),
		    	 editor);			 
	g_signal_connect ((gpointer)navbar.button_book_menu , "scroll_event",
		    	 G_CALLBACK (on_button_verse_menu_book_scroll_event),
		    	 editor);	 
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

GtkWidget *gui_navbar_versekey_editor_new(EDITOR * editor)
{
	gchar *glade_file;
	GladeXML *gxml;
	
	glade_file =
		    gui_general_user_file("navbar_versekey.glade", FALSE);
	g_return_val_if_fail((glade_file != NULL), NULL);
	GS_message(("%s",glade_file));

	/* build the widget */
	gxml = glade_xml_new(glade_file, "navbar", NULL);
	editor->navbar.dialog = TRUE;
	editor->navbar.module_name = g_string_new(settings.MainWindowModule);
	editor->navbar.key =  g_string_new(settings.currentverse);
	
	editor->navbar.navbar = glade_xml_get_widget(gxml, "navbar");	
	editor->navbar.button_history_back = glade_xml_get_widget(gxml, "button_history_back");	
	editor->navbar.button_history_next = glade_xml_get_widget(gxml, "button_history_foward");	
	editor->navbar.button_history_menu = glade_xml_get_widget(gxml, "togglebutton_history_list");
		
	editor->navbar.button_sync = glade_xml_get_widget(gxml, "togglebutton_sync");
	gtk_widget_show(editor->navbar.button_sync); 
	gtk_widget_set_tooltip_text(editor->navbar.button_sync,
				    _("Synchronize this window from main window"));
	
	editor->navbar.button_book_up = glade_xml_get_widget(gxml, "eventbox9");
	editor->navbar.button_book_down = glade_xml_get_widget(gxml, "eventbox6");
	editor->navbar.button_chapter_up = glade_xml_get_widget(gxml, "eventbox8");
	editor->navbar.button_chapter_down = glade_xml_get_widget(gxml, "eventbox4");
	editor->navbar.button_verse_up = glade_xml_get_widget(gxml, "eventbox7");
	editor->navbar.button_verse_down = glade_xml_get_widget(gxml, "eventbox1");
		
	editor->navbar.arrow_book_up_box = glade_xml_get_widget(gxml, "image13");
	editor->navbar.arrow_book_up = glade_xml_get_widget(gxml, "image12");
	editor->navbar.arrow_book_down_box = glade_xml_get_widget(gxml, "image15");
	editor->navbar.arrow_book_down = glade_xml_get_widget(gxml, "image14");
	editor->navbar.arrow_chapter_up_box = glade_xml_get_widget(gxml, "image9");
	editor->navbar.arrow_chapter_up = glade_xml_get_widget(gxml, "image8");
	editor->navbar.arrow_chapter_down_box = glade_xml_get_widget(gxml, "image11");
	editor->navbar.arrow_chapter_down = glade_xml_get_widget(gxml, "image10");
	editor->navbar.arrow_verse_up_box = glade_xml_get_widget(gxml, "image7");
	editor->navbar.arrow_verse_up = glade_xml_get_widget(gxml, "image6");
	editor->navbar.arrow_verse_down_box = glade_xml_get_widget(gxml, "image16");
	editor->navbar.arrow_verse_down = glade_xml_get_widget(gxml, "image5");
		
	editor->navbar.button_book_menu = glade_xml_get_widget(gxml, "togglebutton_book");
	editor->navbar.button_chapter_menu = glade_xml_get_widget(gxml, "togglebutton_chapter");
	editor->navbar.button_verse_menu = glade_xml_get_widget(gxml, "togglebutton_verse");
	editor->navbar.lookup_entry = glade_xml_get_widget(gxml, "entry_lookup");
	editor->navbar.label_book_menu = glade_xml_get_widget(gxml, "label_book");
	editor->navbar.label_chapter_menu = glade_xml_get_widget(gxml, "label_chapter");
	editor->navbar.label_verse_menu = glade_xml_get_widget(gxml, "label_verse");
	editor->navbar.book_menu = gtk_menu_new();
	editor->navbar.chapter_menu = gtk_menu_new();
	editor->navbar.verse_menu = gtk_menu_new();
	_connect_signals(editor->navbar, editor);
	
	return editor->navbar.navbar;
}
