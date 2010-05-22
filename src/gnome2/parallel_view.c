/*
 * Xiphos Bible Study Tool
 * gs_parallel.c - support for displaying multiple modules
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

#ifdef USE_GTKMOZEMBED
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "gecko/gecko-html.h"
#endif
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

#include "gui/parallel_view.h"
#include "gui/parallel_dialog.h"
#include "gui/main_window.h"
#include "gui/xiphos.h"
#include "gui/widgets.h"
#include "gui/tabbed_browser.h"

#include "main/parallel_view.h"
#include "main/global_ops.hh"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"

extern gboolean shift_key_pressed;

/******************************************************************************
 * static
 */

/******************************************************************************
 * Name
 *   on_undockInt_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_undockInt_activate(GtkMenuItem *menuitem)
 *
 * Description
 *   undock/dock parallel page
 *
 * Return value
 *   void
 */

void on_undockInt_activate(GtkMenuItem * menuitem)
{
	if (settings.dockedInt) {
		settings.dockedInt = FALSE;
		gui_undock_parallel_page();

	} else {
		settings.dockedInt = TRUE;
		gui_btnDockInt_clicked(NULL, NULL);
	}
}



/******************************************************************************
 * Name
 *   on_paratab_activate
 *
 * Synopsis
 *   #include "gui/parallel_view.h
 *
 *   void on_paratab_activate(GtkMenuItem *menuitem)
 *
 * Description
 *   open parallel view in a tab
 *
 * Return value
 *   void
 */

void on_paratab_activate(GtkMenuItem * menuitem)
{
	gui_open_parallel_view_in_new_tab();
}



/******************************************************************************
 * Name
 *   on_changeint1mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint1mod_activate(GtkMenuItem * menuitem,
 *			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void on_changeint1mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	
	gchar *mod_name = NULL;
//	GS_message(("mod_name",(gchar *) user_data));
	mod_name = main_module_name_from_description((gchar *) user_data);
	
	if(!mod_name) 
		return;
	main_change_parallel_module(PARALLEL1, mod_name);
}




/******************************************************************************
 * Name
 *   on_changeint2mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint2mod_activate(GtkMenuItem * menuitem,
 *			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint2mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description((gchar *) user_data);
	if(!mod_name) 
		return;
	main_change_parallel_module(PARALLEL2, mod_name);
}


/******************************************************************************
 * Name
 *   on_changeint3mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint3mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint3mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description((gchar *) user_data);
	if(!mod_name) 
		return;
	main_change_parallel_module(PARALLEL3, mod_name);
}


/******************************************************************************
 * Name
 *   on_changeint4mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint4mod_activate(GtkMenuItem * menuitem,
 *			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint4mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description((gchar *) user_data);
	if(!mod_name) 
		return;
	main_change_parallel_module(PARALLEL4, mod_name);
}


/******************************************************************************
 * Name
 *   on_changeint5mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint5mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint5mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description((gchar *) user_data);

	if (!mod_name) 
		return;
	main_change_parallel_module(PARALLEL5, mod_name);
}


void gui_popup_menu_parallel(void)
{	
	GtkWidget *menu;
	GtkWidget *copy7;
	GtkWidget *undockInt = NULL;
	GtkWidget *module_options;
	GtkWidget *separator2;
	GtkWidget *module_options_menu;
	
	menu = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(menu), "pmInt", menu);

	copy7 = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_hide(copy7);
	gtk_container_add(GTK_CONTAINER(menu), copy7);

	separator2 = gtk_menu_item_new();
	gtk_widget_hide(separator2);
	gtk_container_add(GTK_CONTAINER(menu), separator2);
	gtk_widget_set_sensitive(separator2, FALSE);
	
	if (!settings.showparatab) {
		undockInt = gtk_menu_item_new_with_label(_("Detach/Attach"));
		gtk_widget_show(undockInt);
		gtk_container_add(GTK_CONTAINER(menu), undockInt);
	}

	module_options =
	    gtk_menu_item_new_with_label(_("Module Options"));
	gtk_widget_show(module_options);
	gtk_container_add(GTK_CONTAINER(menu), module_options);

	module_options_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_options),
				  module_options_menu);
				  
	main_load_g_ops_parallel(module_options_menu);

	separator2 = gtk_menu_item_new();
	gtk_widget_show(separator2);
	gtk_container_add(GTK_CONTAINER(menu), separator2);
	gtk_widget_set_sensitive(separator2, FALSE);
	/* build change parallel modules submenu */
	main_load_menu_form_mod_list(menu, _("Change parallel 1"),			       
				     G_CALLBACK(on_changeint1mod_activate));
	main_load_menu_form_mod_list(menu, _("Change parallel 2"),
				     G_CALLBACK(on_changeint2mod_activate));
	main_load_menu_form_mod_list(menu, _("Change parallel 3"),
				     G_CALLBACK(on_changeint3mod_activate));
	main_load_menu_form_mod_list(menu, _("Change parallel 4"),
				     G_CALLBACK(on_changeint4mod_activate));
	main_load_menu_form_mod_list(menu, _("Change parallel 5"),
				     G_CALLBACK(on_changeint5mod_activate));

/*	g_signal_connect(GTK_OBJECT(copy7), "activate",
			   G_CALLBACK(gui_copyhtml_activate),
			   NULL);*/
	
	if (!settings.showparatab) {
		if (undockInt) {
			g_signal_connect(GTK_OBJECT(undockInt), "activate",
					 G_CALLBACK(on_undockInt_activate),
					 &settings);
		} else {
			GS_warning(("undockInt is NULL?"));
		}
	}
	
	gtk_menu_popup((GtkMenu*)menu, NULL, NULL, NULL, NULL, 0,
		       gtk_get_current_event_time());
}


static gboolean
on_enter_notify_event        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
	gtk_widget_grab_focus (widgets.html_parallel);
	//settings.whichwindow = MAIN_TEXT_WINDOW;
	//gui_change_window_title(settings.MainWindowModule);
  	return FALSE;
}

static gboolean on_key_press_event           (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
	switch(event->hardware_keycode) {
		case 50:
		case 62:
			shift_key_pressed = TRUE;
		break;
	}
  	return FALSE;
}


static gboolean on_key_release_event         (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
	switch(event->hardware_keycode) {
		case 50:
		case 62:
			shift_key_pressed = FALSE;
		break;
	}
  	return FALSE;
}


#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb(GeckoHtml *html,
			gchar *uri,
			gpointer user_data)
{
	gui_popup_menu_parallel();
}
#else
static gboolean
_popupmenu_requested_cb(GtkHTML *html,
			GdkEventButton * event,
			gpointer date)
{
	if (event->button == 3) {
		gui_popup_menu_parallel();
		return 1;
	}
	return 0;
}
#endif

/******************************************************************************
 * Name
 *   gui_create_parallel_page
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_create_parallel_page(guint page_num)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_create_parallel_page(void)
{
	GtkWidget *label;
#ifdef USE_GTKMOZEMBED
	GtkWidget *eventbox;
#else
	GtkWidget *scrolled_window;
#endif

	/*
	 * parallel page 
	 */
	settings.dockedInt = TRUE;
#ifdef USE_GTKMOZEMBED
	eventbox = gtk_event_box_new ();
	gtk_widget_show (eventbox);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_bible_parallel), eventbox);

	widgets.frame_parallel = eventbox;
	widgets.html_parallel = GTK_WIDGET(gecko_html_new(NULL, FALSE, PARALLEL_TYPE)); //embed_new(PARALLEL_TYPE); //gtk_moz_embed_new();
	gtk_widget_show(widgets.html_parallel);
	gtk_container_add(GTK_CONTAINER(eventbox),
			  widgets.html_parallel);

	g_signal_connect((gpointer)widgets.html_parallel,
			 "popupmenu_requested",
			 G_CALLBACK (_popupmenu_requested_cb),
			 NULL);
#else
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolled_window);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);
/*	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolled_window,
                                             settings.shadow_type);*/
	widgets.frame_parallel = scrolled_window;

	gtk_container_add(GTK_CONTAINER(widgets.notebook_bible_parallel),
			  scrolled_window);
	widgets.html_parallel = gtk_html_new();
	gtk_widget_show(widgets.html_parallel);
	gtk_html_load_empty(GTK_HTML(widgets.html_parallel));
	gtk_container_add(GTK_CONTAINER(scrolled_window),
			  widgets.html_parallel);

	g_signal_connect(GTK_OBJECT(widgets.html_parallel),
			 "on_url", G_CALLBACK(gui_url),
			 (gpointer) widgets.app);
	g_signal_connect(GTK_OBJECT(widgets.html_parallel),
			 "link_clicked",
			 G_CALLBACK(gui_link_clicked), NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_parallel),
			 "button_release_event",
			 G_CALLBACK (_popupmenu_requested_cb),
			 NULL);
#endif

	label = gtk_label_new(_("Parallel View"));
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.notebook_bible_parallel),
				    1),
				   label);

	g_signal_connect ((gpointer) widgets.html_parallel, "enter_notify_event",
		    G_CALLBACK (on_enter_notify_event),
		    NULL);
	g_signal_connect ((gpointer) widgets.html_parallel, "key_press_event",
		    G_CALLBACK (on_key_press_event),
		    NULL);
	g_signal_connect ((gpointer) widgets.html_parallel, "key_release_event",
		    G_CALLBACK (on_key_release_event),
		    NULL);
}
