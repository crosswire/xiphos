/*
 * GnomeSword Bible Study Tool
 * main_window.c - main window gui
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
#include <gtkhtml/gtkhtml.h>

#include "main/sword.h"
#include "main/settings.h"
#include "main/xml.h"

#include "gui/gnomesword.h"
#include "gui/main_window.h"
#include "gui/main_menu.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_dialog.h"
#include "gui/studypad.h"
#include "gui/toolbar_nav.h"
#include "gui/utilities.h"
#include "gui/html.h"
#include "gui/history.h"
#include "gui/bibletext.h"
#include "gui/parallel_view.h"
#include "gui/percomm.h"
#include "gui/commentary.h"
#include "gui/gbs.h"
#include "gui/dictlex.h"
#include "gui/search_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/bibletext_dialog.h"
#include "gui/widgets.h"

WIDGETS widgets;

extern HISTORY history_list[];	/* sturcture for storing history items */
extern gint history_items;


/******************************************************************************
 * Name
 *  gui_show_hide_texts
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_hide_texts(gboolean choice)
 *
 * Description
 *    Show/hide bible texts
 *
 * Return value
 *   void
 */

void gui_show_hide_texts(gboolean choice)
{
	settings.showtexts = choice;
	if (choice == FALSE) {
		gtk_widget_hide(widgets.vbox_text);
	} else {
		gtk_widget_show(widgets.vbox_text);
	}
	gui_set_bible_comm_layout();
}


/******************************************************************************
 * Name
 *  gui_show_hide_comms
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_hide_comms(gboolean choice)
 *
 * Description
 *    Show/hide Commentaries
 *
 * Return value
 *   void
 */

void gui_show_hide_comms(gboolean choice)
{
	settings.showcomms = choice;
	if (choice == FALSE) {
		gtk_widget_hide(widgets.box_comm);
	} else {
		gtk_widget_show(widgets.box_comm);
	}
	gui_set_bible_comm_layout();
}


/******************************************************************************
 * Name
 *  gui_show_hide_dicts
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_hide_dicts(gboolean choice)
 *
 * Description
 *    Show/hide Dictionaries-Lexicons
 *
 * Return value
 *   void
 */

void gui_show_hide_dicts(gboolean choice)
{
	settings.showdicts = choice;
	if (choice == FALSE) {
		gtk_widget_hide(widgets.box_dict);
	} else {
		gtk_widget_show(widgets.box_dict);
	}
	gui_set_bible_comm_layout();
}


/******************************************************************************
 * Name
 *  gui_set_bible_comm_layout
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_set_bible_comm_layout(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_set_bible_comm_layout(void)
{
	if ((settings.showtexts == FALSE)
	    && (settings.showcomms == FALSE)) {
		gtk_paned_set_position(GTK_PANED(widgets.vpaned), 0);
	} else if (settings.showdicts == FALSE) {
		gtk_paned_set_position(GTK_PANED
				       (widgets.vpaned),
				       settings.gs_hight);
	} else {
		gtk_paned_set_position(GTK_PANED
				       (widgets.vpaned),
				       settings.upperpane_hight);
	}

	if ((settings.showtexts == FALSE)
	    && (settings.showcomms == TRUE)) {
		gtk_paned_set_position(GTK_PANED(widgets.hpaned), 0);
	} else if (settings.showtexts == FALSE) {
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       settings.gs_width);
	} else if ((settings.showtexts == TRUE)
		   && (settings.showcomms == FALSE)) {
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       settings.gs_width);
	} else if ((settings.showtexts == TRUE)
		   && (settings.showcomms == TRUE)) {
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       settings.biblepane_width);
	} else if (settings.showtexts == TRUE) {
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       settings.gs_width);
	} else {
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       settings.biblepane_width);
	}
}


/******************************************************************************
 * Name
 *  gui_change_window_title
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_change_window_title(gchar * module_name)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_change_window_title(gchar * module_name)
{
	gchar title[200];
	/*
	 * set program title to current module name
	 */
	sprintf(title, "%s - %s", get_module_description(module_name),
						settings.program_title);
	gtk_window_set_title(GTK_WINDOW(widgets.app), title);
}




/******************************************************************************
 * Name
 *  gui_change_module_and_key
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_change_module_and_key(gchar * module_name, gchar * key)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_change_module_and_key(gchar * module_name, gchar * key)
{
	gint mod_type;
	gint page_num;
	gchar *val_key = NULL;

	mod_type = get_mod_type(module_name);

	switch (mod_type) {
	case TEXT_TYPE:
		if (settings.havebible) {
			if(settings.browsing)
				gui_update_tab_struct(module_name,
						      NULL,
						      NULL,
						      NULL,
						      NULL,
						      NULL,
						      settings.comm_showing);
			val_key = gui_update_nav_controls(key);
			main_display_bible(module_name, val_key);
			main_display_commentary(settings.CommWindowModule, val_key);
			free(val_key);
		}
		break;
	case COMMENTARY_TYPE:
		if (settings.havecomm) {
			if(settings.browsing)
				gui_update_tab_struct(NULL,
						      module_name,
						      NULL,
						      NULL,
						      NULL,
						      NULL,
						      TRUE);
			main_display_commentary(module_name, key);
		}
		break;
	case DICTIONARY_TYPE:
		if (settings.havedict) {
			if(settings.browsing)
				gui_update_tab_struct(NULL,
						      NULL,
						      module_name,
						      NULL,
						      key,
						      NULL,
						      settings.comm_showing);			
			//gui_set_dictlex_mod_and_key(module_name, key);
			main_display_dictionary(module_name, key);
		}
		break;
	case BOOK_TYPE:
		if (settings.havebook) {
			if(settings.browsing)
				gui_update_tab_struct(NULL,
						      NULL,
						      NULL,
						      module_name,
						      NULL,
						      key?key:NULL,
						      FALSE);
			main_set_book_mod(module_name, atol(key));
			gui_set_book_mod_and_key(module_name, key);
			gui_change_window_title(settings.book_mod);
		}
		break;
	}
}


/******************************************************************************
 * Name
 *  gui_change_verse
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_change_verse(gchar * key)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_change_verse(const gchar * key)
{
	gchar *val_key;

	settings.apply_change = FALSE;
	//g_warning(key);
	val_key = gui_update_nav_controls(key);
	if (settings.havebible) {
		/* add item to history */
		if (settings.addhistoryitem) {
			if (strcmp(settings.currentverse,
			     history_list[history_items - 1].verseref))
				gui_add_history_Item(widgets.app,
					       GTK_WIDGET
					       (widgets.shortcutbar),
					       settings.currentverse);
		}
		settings.addhistoryitem = TRUE;
		/* change main window */
		gui_display_text(val_key);
		gui_keep_bibletext_dialog_in_sync(val_key);
	}

	/*
	 * change parallel verses
	 */
	if (settings.dockedInt) {
		gui_update_parallel_page();
	}

	/*
	 * change personal notes editor   if not in edit mode
	 */
	if (!settings.editnote)
		if (settings.havepercomm)
			if (!settings.use_percomm_dialog)
				gui_display_percomm(val_key);

	/*
	 * set commentary module to current verse
	 */
	if (settings.havecomm && settings.comm_showing) {
		gui_display_commentary(val_key);
		gui_keep_comm_dialog_in_sync(val_key);
	}

	free(val_key);
}


/******************************************************************************
 * Name
 *   on_mainwindow_destroy
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void on_mainwindow_destroy(GtkObject * object, gpointer user_data)
 *
 * Description
 *    shutdown gnomesword
 *
 * Return value
 *   void
 */

static void on_mainwindow_destroy(GtkObject * object,
				  gpointer user_data)
{
	shutdown_frontend();
	/* shutdown the sword stuff */
	shutdown_backend();
	gtk_main_quit();
	gtk_exit(0);
}


/******************************************************************************
 * Name
 *   on_epaned_button_release_event
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   gboolean on_epaned_button_release_event(GtkWidget * widget,
 *			GdkEventButton * event, gpointer user_data)
 *
 * Description
 *    get and store pane sizes
 *
 * Return value
 *   void
 */

static gboolean epaned_button_release_event(GtkWidget * widget,
					    GdkEventButton * event,
					    gpointer user_data)
{
	gint panesize;
	gchar layout[80];

	panesize = gtk_paned_get_position(GTK_PANED(widget));

	if (panesize > 15) {
		if (!strcmp((gchar *) user_data, "epaned")) {
			settings.shortcutbar_width = panesize;
			sprintf(layout, "%d",
				settings.shortcutbar_width);
			xml_set_value("GnomeSword", "layout",
				      "shortcutbar", layout);
		}
		if (!strcmp((gchar *) user_data, "vpaned")) {
			settings.upperpane_hight = panesize;
			sprintf(layout, "%d", settings.upperpane_hight);
			xml_set_value("GnomeSword", "layout",
				      "uperpane", layout);
		}
		if (!strcmp((gchar *) user_data, "hpaned1")) {
			settings.biblepane_width = panesize;
			sprintf(layout, "%d", settings.biblepane_width);
			xml_set_value("GnomeSword", "layout",
				      "textpane", layout);
		}
		return FALSE;
	}
}


/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_search_appbar_update(char percent, void *userData)
{
	char maxHashes = *((char *) userData);
	float num;
	char buf[80];
	static char printed = 0;

	while (gtk_events_pending())
		gtk_main_iteration();
	while ((((float) percent) / 100) * maxHashes > printed) {
		sprintf(buf, "%f", (((float) percent) / 100));
		num = (float) percent / 100;
/*		gnome_appbar_set_progress((GnomeAppBar *) widgets.
					  appbar, num);*/
		printed++;
	}
	while (gtk_events_pending())
		gtk_main_iteration();
	printed = 0;
}


/******************************************************************************
 * Name
 *   on_configure_event
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   gboolean on_configure_event(GtkWidget * widget,
 *				   GdkEventConfigure * event,
 *				   gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   gboolean
 */

static gboolean on_configure_event(GtkWidget * widget,
				   GdkEventConfigure * event,
				   gpointer user_data)
{
	gchar layout[80];
	gint x;
	gint y;

 	gdk_window_get_root_origin(GDK_WINDOW(widgets.app->window), &x, &y);

	settings.gs_width = event->width;
	settings.gs_hight = event->height;
	settings.app_x = x;
	settings.app_y = y;

	sprintf(layout, "%d", settings.gs_width);
	xml_set_value("GnomeSword", "layout", "width", layout);

	sprintf(layout, "%d", settings.gs_hight);
	xml_set_value("GnomeSword", "layout", "hight", layout);

	sprintf(layout, "%d", settings.app_x);
	xml_set_value("GnomeSword", "layout", "app_x", layout);

	sprintf(layout, "%d", settings.app_y);
	xml_set_value("GnomeSword", "layout", "app_y", layout);
	xml_save_settings_doc(settings.fnconfigure);
	return FALSE;
}


/******************************************************************************
 * Name
 *   create_mainwindow
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void create_mainwindow(void)
 *
 * Description
 *    create gnomesword gui
 *
 * Return value
 *   void
 */

void create_mainwindow(void)
{
	GtkWidget *dock1;
	GtkWidget *vbox_gs;
	GtkWidget *mainPanel;
	GtkWidget *vboxMain;
	GtkWidget *hbox2;
	GtkWidget *nav_toolbar;
	GtkWidget *swInt;
	GtkWidget *hbox25;
	GtkWidget *hboxtb;
	GtkWidget *tab_button_icon;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new ();

	g_print("%s\n", "Building GnomeSword interface");

	widgets.studypad_dialog = NULL;

	widgets.app =
	    gnome_app_new("gnomesword",
			  _("GnomeSword - Bible Study Software"));
	gtk_object_set_data(GTK_OBJECT(widgets.app),
			    "widgets.app", widgets.app);
	gtk_widget_set_size_request(widgets.app, 680, 480);
	GTK_WIDGET_SET_FLAGS(widgets.app, GTK_CAN_FOCUS);
	gtk_window_set_policy(GTK_WINDOW(widgets.app), TRUE,
			      TRUE, TRUE);
	gnome_window_icon_set_default_from_file
	    (PACKAGE_PIXMAPS_DIR "/gs2-48x48.png");

	dock1 = GNOME_APP(widgets.app)->dock;
	gtk_widget_show(dock1);

	gui_create_main_menu(widgets.app);

	vbox_gs = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_gs);
	gnome_app_set_contents(GNOME_APP(widgets.app), vbox_gs);

	hbox25 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox25);
	gtk_box_pack_start(GTK_BOX(vbox_gs), hbox25, TRUE, TRUE, 0);

	widgets.epaned = gtk_hpaned_new();
	gtk_widget_show(widgets.epaned);
	gtk_box_pack_start(GTK_BOX(hbox25), widgets.epaned, TRUE, TRUE, 0);

	mainPanel = gtk_vbox_new(FALSE, 0);
	gtk_paned_pack2(GTK_PANED(widgets.epaned), mainPanel,
			TRUE, TRUE);
	gtk_widget_show(mainPanel);

	vboxMain = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vboxMain);
	gtk_box_pack_start(GTK_BOX(mainPanel), vboxMain, TRUE, TRUE, 0);

	/*
	 * notebook to have separate passages opened at once
	 * the passages are not actually open but are switched
	 * between similar to bookmarks
	 */
	widgets.hboxtb = gtk_hbox_new(FALSE, 0);
	if(settings.browsing)
		gtk_widget_show(widgets.hboxtb);
	gtk_box_pack_start(GTK_BOX(vboxMain), widgets.hboxtb, FALSE, FALSE, 0);

	widgets.button_new_tab = gtk_button_new();
	//don't show button here in case !settings.browsing

//	tab_button_icon = gtk_image_new_from_stock(GTK_STOCK_NEW,
//					GTK_ICON_SIZE_SMALL_TOOLBAR);
	tab_button_icon = gtk_image_new_from_file(PACKAGE_PIXMAPS_DIR
						"/new_tab_button.png");
	gtk_widget_show(tab_button_icon);
	gtk_container_add(GTK_CONTAINER(widgets.button_new_tab), tab_button_icon);
	gtk_button_set_relief(GTK_BUTTON(widgets.button_new_tab), GTK_RELIEF_NONE);
	gtk_box_pack_start(GTK_BOX(widgets.hboxtb), widgets.button_new_tab, FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, widgets.button_new_tab, _("Open a new tab"),
				NULL);

	widgets.notebook_main = gtk_notebook_new();gtk_widget_show(widgets.notebook_main);
	gtk_box_pack_start(GTK_BOX(widgets.hboxtb),
			   widgets.notebook_main, TRUE, TRUE, 0);
	gtk_widget_set_size_request(widgets.notebook_main, -1, 25);
	gtk_notebook_set_scrollable (GTK_NOTEBOOK(widgets.notebook_main),
                                             TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_main));
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.notebook_main), FALSE);
	/* main passage tabbed notebook end */

	/*
	 * nav toolbar
	 */
	nav_toolbar = gui_create_nav_toolbar(widgets.app);
	/* gtk_box_pack_start(GTK_BOX(vboxMain),nav_toolbar,FALSE,FALSE,0); */
	/*
	 * end nav toolbar
	 */

	widgets.vpaned = gtk_vpaned_new();
	gtk_widget_show(widgets.vpaned);
	gtk_box_pack_end(GTK_BOX(vboxMain), widgets.vpaned, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(widgets.vpaned), 1);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox2);
	gtk_paned_pack1(GTK_PANED(widgets.vpaned), hbox2, TRUE, TRUE);

	widgets.hpaned = gtk_hpaned_new();
	gtk_widget_show(widgets.hpaned);
	gtk_box_pack_start(GTK_BOX(hbox2), widgets.hpaned, TRUE, TRUE, 0);

	widgets.vbox_text = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(widgets.vbox_text);
	gtk_paned_pack1(GTK_PANED(widgets.hpaned),
					widgets.vbox_text, FALSE, TRUE);


	/*
	 * text/parallel notebook
	 */
	widgets.notebook_parallel_text = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_parallel_text);
	gtk_box_pack_start(GTK_BOX(widgets.vbox_text),
			   widgets.notebook_parallel_text, TRUE,
			   TRUE, 0);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.
				    notebook_parallel_text), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.
				      notebook_parallel_text), FALSE);

	/*
	 * text notebook
	 */	 
	widgets.notebook_text = gui_create_bible_pane();      
        gtk_container_add(GTK_CONTAINER(widgets.notebook_parallel_text),
			  widgets.notebook_text);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK
				    (widgets.notebook_text), TRUE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_text),FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.notebook_text),FALSE);
	gtk_notebook_popup_disable(GTK_NOTEBOOK(widgets.notebook_text));


	/*
	 * commentary and book pane
	 */
	widgets.box_comm = gui_create_commentary_pane();
	gtk_paned_pack2(GTK_PANED(widgets.hpaned),
			widgets.box_comm, TRUE, TRUE);

	/*
	 * lower_workbook
	 */
/*	widgets.workbook_lower = gtk_notebook_new();
	gtk_widget_show(widgets.workbook_lower);
	gtk_notebook_set_show_border(GTK_NOTEBOOK
				     (widgets.workbook_lower), FALSE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK
				   (widgets.workbook_lower), FALSE);

	gtk_paned_pack2(GTK_PANED(widgets.vpaned),
			widgets.workbook_lower, TRUE, TRUE);
*/
	/*
	 * dict/lex
         */
	widgets.box_dict = gui_create_dictionary_pane();        
        /*gtk_container_add(GTK_CONTAINER(widgets.workbook_lower),
			  widgets.box_dict);*/
	gtk_paned_pack2(GTK_PANED(widgets.vpaned),
			widgets.box_dict, TRUE, TRUE);
	/*
	 * end  dict/lex
	 */

	/*
	 * gbs notebook
	 */

/*	widgets.notebook_gbs = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_gbs);
	gtk_container_add(GTK_CONTAINER(widgets.workbook_lower),
			  widgets.notebook_gbs);
	GTK_WIDGET_UNSET_FLAGS(widgets.notebook_gbs, GTK_CAN_FOCUS);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK
				    (widgets.notebook_gbs), TRUE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK
				   (widgets.notebook_gbs), FALSE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_gbs));
	gtk_notebook_set_show_border(GTK_NOTEBOOK
				     (widgets.notebook_gbs), FALSE);
*/	
	/*
	 * end gbs
	 */


	widgets.appbar = gnome_appbar_new(FALSE, TRUE,
					  GNOME_PREFERENCES_NEVER);
	gtk_widget_show(widgets.appbar);
	gnome_app_set_statusbar(GNOME_APP(widgets.app), widgets.appbar);

	gui_install_menu_hints(widgets.app);


	g_signal_connect(GTK_OBJECT(widgets.app), "destroy",
			   G_CALLBACK(on_mainwindow_destroy), NULL);
	g_signal_connect((gpointer) widgets.app,
			 "configure_event",
			 G_CALLBACK(on_configure_event), NULL);
	g_signal_connect(GTK_OBJECT(widgets.epaned),
			   "button_release_event",
			   G_CALLBACK
			   (epaned_button_release_event),
			   (gchar *) "epaned");
	g_signal_connect(GTK_OBJECT(widgets.vpaned),
			   "button_release_event",
			   G_CALLBACK
			   (epaned_button_release_event),
			   (gchar *) "vpaned");
	g_signal_connect(GTK_OBJECT(widgets.hpaned),
			   "button_release_event",
			   G_CALLBACK
			   (epaned_button_release_event),
			   (gchar *) "hpaned1");

	gtk_widget_grab_focus(nav_bar.lookup_entry);

	gtk_widget_set_size_request(widgets.app, settings.gs_width,
			     settings.gs_hight);
}


/******************************************************************************
 * Name
 *   gui_show_main_window
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_main_window(void)
 *
 * Description
 *    display the app after all is created
 *
 * Return value
 *   void
 */

void gui_show_main_window(void)
{
	gtk_widget_show(widgets.app);
}
