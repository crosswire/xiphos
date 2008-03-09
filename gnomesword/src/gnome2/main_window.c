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

#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif
#include "main/sword.h"
#include "main/settings.h"
#include "main/xml.h"

#include "gui/gnomesword.h"
#include "gui/main_window.h"
#include "gui/main_menu.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_dialog.h"
#include "gui/studypad.h"
#include "gui/sidebar.h"
#include "gui/toolbar_nav.h"
#include "gui/utilities.h"
#include "gui/bibletext.h"
#include "gui/parallel_view.h"
#include "gui/commentary.h"
#include "gui/gbs.h"
#include "gui/dictlex.h"
#include "gui/search_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/bibletext_dialog.h"
#include "gui/navbar_versekey.h"
#include "gui/widgets.h"


WIDGETS widgets;


extern gboolean shift_key_presed;

static gboolean ctrl_key_presed = FALSE;
static GtkWidget *nav_toolbar;
static main_window_created = FALSE;

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
	gui_tab_set_showtexts(choice);
	gui_set_tab_label(settings.currentverse, TRUE);
	if (choice == FALSE) {
		if (main_window_created)
			gtk_widget_hide(widgets.vpaned);
		xml_set_value("GnomeSword", "misc", "showtexts", "0");
	} else {
		if (main_window_created)
			gtk_widget_show(widgets.vpaned);
		xml_set_value("GnomeSword", "misc", "showtexts", "1");
	}
}


/******************************************************************************
 * Name
 *  gui_show_hide_preview
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_hide_preview(gboolean choice)
 *
 * Description
 *    Show/hide bible texts
 *
 * Return value
 *   void
 */

void gui_show_hide_preview(gboolean choice)
{
	settings.showpreview = choice;
	gui_tab_set_showpreview(choice);
	if (choice == FALSE) {
		if (main_window_created)
			gtk_widget_hide(widgets.vbox_previewer);
		xml_set_value("GnomeSword", "misc", "showpreview", "0");
	} else {
		if (main_window_created)
			gtk_widget_show(widgets.vbox_previewer);
		xml_set_value("GnomeSword", "misc", "showpreview", "1");
	}
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
	gui_tab_set_showcomms(choice);
	gui_set_tab_label(settings.currentverse, TRUE);
	if (choice == FALSE) {
		if (main_window_created)
			gtk_widget_hide(widgets.notebook_comm_book);
		xml_set_value("GnomeSword", "misc", "showcomms", "0");
	} else {
		if (main_window_created)
			gtk_widget_show(widgets.notebook_comm_book);
		xml_set_value("GnomeSword", "misc", "showcomms", "1");
	}
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
	gui_tab_set_showdicts(choice);
	gui_set_tab_label(settings.currentverse, TRUE);
	if (choice == FALSE) {
		if (main_window_created)
			gtk_widget_hide(widgets.box_dict);
		xml_set_value("GnomeSword", "misc", "showdicts", "0");
	} else {
		if (main_window_created)
			gtk_widget_show(widgets.box_dict);
		xml_set_value("GnomeSword", "misc", "showdicts", "1");
	}
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
	gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       settings.biblepane_width);
	gtk_paned_set_position(GTK_PANED(widgets.vpaned),
				       settings.biblepane_hight);
	gtk_paned_set_position(GTK_PANED(widgets.vpaned2),
				       settings.commpane_hight);
	
	if ((settings.showcomms == TRUE)  ||  (settings.showdicts == TRUE)) {
		gtk_widget_show(widgets.vpaned2);
	}
	
	gtk_paned_set_position(GTK_PANED(widgets.hpaned),
			       (settings.showtexts
				? settings.biblepane_width
				: 0));

	gtk_paned_set_position(GTK_PANED(widgets.vpaned2),
			       (settings.showcomms
				? settings.commpane_hight
				: 0));
	
	gtk_paned_set_position(GTK_PANED(widgets.vpaned2),
			       (settings.showdicts
				? settings.commpane_hight
				: settings.gs_hight));
	
	if ((settings.showcomms == FALSE)  && (settings.showdicts == FALSE)) {
		gtk_widget_hide(widgets.vpaned2);
		gtk_paned_set_position(GTK_PANED
				       (widgets.hpaned),
				       settings.gs_width);	   
	}
	
	if ((settings.showcomms == TRUE)
		   || (settings.showdicts == TRUE)) {
		gtk_paned_set_position(GTK_PANED
				       (widgets.hpaned),
				       settings.biblepane_width);	   
	}
	if((settings.showcomms == FALSE) && (settings.showtexts == FALSE))
		gtk_widget_hide(nav_toolbar);
	else
		gtk_widget_show(nav_toolbar);
		
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
	gchar *title;//[200];
	/*
	 * set program title to current module name
	 */
	title = g_strdup_printf("%s - %s", main_get_module_description(module_name),
						settings.program_title);
	gtk_window_set_title(GTK_WINDOW(widgets.app), title);
	g_free(title);
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
	main_shutdown_backend();
	gtk_main_quit();
	gtk_exit(0);
}


static gboolean  delete_event (GtkWidget *widget,
                                            GdkEvent *event,
                                            gpointer user_data)
{
	shutdown_frontend();
	/* shutdown the sword stuff */
	main_shutdown_backend();
	gtk_main_quit();
	gtk_exit(0);
	return TRUE;	
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
			settings.sidebar_width = panesize;
			sprintf(layout, "%d", settings.sidebar_width);
			xml_set_value("GnomeSword", "layout",
				      "shortcutbar", layout);
		}
		if (!strcmp((gchar *) user_data, "vpaned")) {
			settings.biblepane_hight = panesize;
			sprintf(layout, "%d", settings.biblepane_hight);
			xml_set_value("GnomeSword", "layout",
				      "biblehight", layout);
		}
		if (!strcmp((gchar *) user_data, "vpaned2")) {
			settings.commpane_hight = panesize;
			sprintf(layout, "%d", settings.commpane_hight);
			xml_set_value("GnomeSword", "layout",
				      "commentaryhight", layout);
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

static void on_notebook_bible_parallel_switch_page(GtkNotebook * notebook,
					GtkNotebookPage * page,
					gint page_num, GList **tl)
{ 
	if(page_num == 0)
		gui_set_drop_target(widgets.html_text);
	else
		gtk_drag_dest_unset(GTK_WIDGET(widgets.html_text));
}

static void on_notebook_comm_book_switch_page(GtkNotebook * notebook,
					GtkNotebookPage * page,
					gint page_num, GList **tl)
{ 
	gchar *url = NULL;
	
	if(page_num == 0) {
		settings.comm_showing = TRUE;
		gtk_drag_dest_unset(GTK_WIDGET(widgets.html_book));
		gui_set_drop_target(widgets.html_comm);
	} else {
		settings.comm_showing = FALSE;
		gtk_drag_dest_unset(GTK_WIDGET(widgets.html_comm));
		gui_set_drop_target(widgets.html_book);
	}
		
	gui_update_tab_struct(NULL, 
			      settings.CommWindowModule, 
			      NULL, 
			      NULL, 
			      NULL,
			      NULL,
			      settings.comm_showing,
			      settings.showtexts,
			      settings.showpreview,
			      settings.showcomms,
			      settings.showdicts);
	if(settings.comm_showing)
		main_display_commentary(settings.CommWindowModule, settings.currentverse);
	else {
		url = g_strdup_printf("sword://%s/%d",settings.book_mod,
						      settings.book_offset);
		main_url_handler(url);
		g_free(url);
	}
	gui_set_tab_label(settings.currentverse, TRUE);
}


static
gboolean on_vbox1_key_press_event(GtkWidget * widget, GdkEventKey * event,
				  gpointer user_data)
{
	/* these are the mods we actually use for global keys, we always only check for these set */
	guint state = event->state & (GDK_SHIFT_MASK  | GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_MOD4_MASK );

	switch(event->hardware_keycode) {
	case 40: // Alt-D  dictionary entry
		if (state == GDK_MOD1_MASK)
			gtk_widget_grab_focus(widgets.entry_dict);
		break;
	case 42: // Alt-G  genbook entry
		if (state == GDK_MOD1_MASK) {
			gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book),1);
			gtk_widget_grab_focus(navbar_book.lookup_entry);
		}
		break;
	case 46: // Ctrl-L  verse entry
		if (state == GDK_CONTROL_MASK)
#ifdef OLD_NAVBAR
			gtk_widget_grab_focus(nav_bar.lookup_entry);
#else
			gtk_widget_grab_focus(navbar_versekey.lookup_entry);
#endif
		break;
	case 41: // Ctrl-F  find in bible text
		if (state == GDK_CONTROL_MASK)
			// bible text only, *sigh*.
			gui_find_dlg(widgets.html_text,
				     settings.MainWindowModule,
				     FALSE, NULL);
		break;
	case 54: // Alt-C  commentary pane
		if (state == GDK_MOD1_MASK) {
#ifdef OLD_NAVBAR
			gtk_widget_grab_focus(nav_bar.lookup_entry);
#else
			gtk_widget_grab_focus(navbar_versekey.lookup_entry);
#endif
			gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book),0);
		}
		break;
	case 56: // Alt-B  bookmark
		if (state == GDK_MOD1_MASK) {
			gchar *label = g_strdup_printf("%s, %s",
						       settings.currentverse,
						       settings.MainWindowModule);
			gui_bookmark_dialog(label,
					    settings.MainWindowModule,
					    settings.currentverse);
			g_free(label);
		}
		break;
	case 33: // Alt-P  parallel detachment
		if (state == GDK_MOD1_MASK)
			on_undockInt_activate(NULL);
		break;
	}
	GS_message(("on_vbox1_key_press_event\nkeycode: %d, state: %d",
		    event->hardware_keycode, state));
	return FALSE;
}

static
gboolean on_vbox1_key_release_event(GtkWidget * widget,
                                        GdkEventKey * event,
                                        gpointer user_data)
{
        switch(event->hardware_keycode) {			
		case 50: 
		case 62: 
			shift_key_presed = FALSE;				
		break;
	}
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
	GtkWidget *vboxMain;
	GtkWidget *hbox2;
	GtkWidget *swInt;
	GtkWidget *hbox25;
	GtkWidget *hboxtb;
	GtkWidget *tab_button_icon;
	GtkTooltips *tooltips;
	GtkWidget *vbox;
	GtkWidget *label;
	GtkWidget *scrolledwindow;
	GtkWidget *box_book;
	GtkWidget *frame;
	/*
	GTK_SHADOW_NONE
  	GTK_SHADOW_IN
  	GTK_SHADOW_OUT
  	GTK_SHADOW_ETCHED_IN
  	GTK_SHADOW_ETCHED_OUT
	*/
	settings.shadow_type = GTK_SHADOW_IN;

	tooltips = gtk_tooltips_new ();
	GS_print(("%s gnomesword-%s\n", _("Starting"), VERSION));
	GS_print(("%s\n\n", _("Building GnomeSword interface")));

	widgets.studypad_dialog = NULL;

	widgets.app =
	    gnome_app_new("gnomesword",
			  _("GnomeSword - Bible Study Software"));
	gtk_object_set_data(GTK_OBJECT(widgets.app),
			    "widgets.app", widgets.app);
	gtk_widget_set_size_request(widgets.app, 680, 425);
	GTK_WIDGET_SET_FLAGS(widgets.app, GTK_CAN_FOCUS);
	gtk_window_set_resizable(GTK_WINDOW(widgets.app), TRUE);
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
	
/**widgets.epaned********/
	widgets.epaned = gtk_hpaned_new();
	gtk_widget_show(widgets.epaned);
	gtk_container_set_border_width (GTK_CONTAINER (widgets.epaned), 6);
	gtk_box_pack_start(GTK_BOX(hbox25), widgets.epaned, TRUE, TRUE, 0);

	vboxMain = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vboxMain);
	gtk_paned_pack2(GTK_PANED(widgets.epaned), vboxMain,
			TRUE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(vboxMain), 2);
	
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
#ifdef OLD_NAVBAR
	nav_toolbar = gui_create_nav_toolbar(vboxMain);
	gtk_box_pack_start(GTK_BOX(vboxMain), nav_toolbar, FALSE,
			   FALSE, 0);
#else
	nav_toolbar = gui_navbar_versekey_new();
	gtk_box_pack_start(GTK_BOX(vboxMain), nav_toolbar, FALSE,
			   FALSE, 0);
#endif
	/*
	 * end nav toolbar
	 */

/**widgets.hpaned********/
	widgets.hpaned = gtk_hpaned_new();
	gtk_widget_show(widgets.hpaned);
	gtk_box_pack_start(GTK_BOX(vboxMain), widgets.hpaned, TRUE, TRUE, 0);

/**widgets.vpaned********/
	widgets.vpaned = gtk_vpaned_new();
	gtk_widget_show(widgets.vpaned);
	gtk_paned_pack1(GTK_PANED(widgets.hpaned), widgets.vpaned, TRUE, TRUE);

	widgets.vpaned2 = gtk_vpaned_new();
	gtk_widget_show(widgets.vpaned2);
	gtk_paned_pack2(GTK_PANED(widgets.hpaned), widgets.vpaned2, TRUE, TRUE);

/**widgets.hpaned********/
	widgets.vbox_text = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(widgets.vbox_text);
	gtk_paned_pack1(GTK_PANED(widgets.vpaned),
					widgets.vbox_text, TRUE, TRUE);

	/*
	 * bible/parallel notebook
	 */
	widgets.notebook_bible_parallel = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_bible_parallel);
	gtk_box_pack_start(GTK_BOX(widgets.vbox_text),
			   widgets.notebook_bible_parallel, TRUE,
			   TRUE, 0);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                             GTK_POS_BOTTOM);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.
				    notebook_bible_parallel), TRUE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.
				      notebook_bible_parallel), FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (widgets.notebook_bible_parallel), 1);


	g_signal_connect(GTK_OBJECT(widgets.notebook_bible_parallel),
			   "switch_page",
			   G_CALLBACK
			   (on_notebook_bible_parallel_switch_page), 
			   NULL);
	/*
	 * text notebook
	 */	 
	widgets.notebook_text = gui_create_bible_pane();      
        gtk_container_add(GTK_CONTAINER(widgets.notebook_bible_parallel),
			  widgets.notebook_text);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK
				    (widgets.notebook_text), TRUE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_text),FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.notebook_text),FALSE);


	label = gtk_label_new(_("Standard View"));
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.notebook_bible_parallel), 
				    0),
				   label);

	/*
	 * previewer
	 */
	widgets.vbox_previewer = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(widgets.vbox_previewer);
	gtk_container_set_border_width (GTK_CONTAINER (widgets.vbox_previewer), 1);
	gtk_paned_pack2(GTK_PANED(widgets.vpaned), widgets.vbox_previewer, TRUE, TRUE);
	
#ifdef USE_GTKMOZEMBED 
	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(widgets.vbox_previewer), frame, 
				TRUE, TRUE,
			   	0);
	sidebar.html_viewer_widget = GTK_WIDGET(gecko_html_new(NULL, FALSE, VIEWER_TYPE));//embed_new(VIEWER_TYPE);
	gtk_container_add(GTK_CONTAINER(frame), sidebar.html_viewer_widget);
#else
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(widgets.vbox_previewer), scrolledwindow, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow,
					    settings.shadow_type);
	sidebar.html_viewer_widget = gtk_html_new();
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  sidebar.html_viewer_widget);
	g_signal_connect(GTK_OBJECT(sidebar.html_viewer_widget),
			 "link_clicked", G_CALLBACK(gui_link_clicked),
			 NULL);
#endif

	gtk_widget_show(sidebar.html_viewer_widget);
			  
	/*
	 * commentary/book notebook
	 */
	widgets.notebook_comm_book = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_comm_book);
	
	
	gtk_paned_pack1(GTK_PANED(widgets.vpaned2),
			widgets.notebook_comm_book, TRUE, TRUE);
	gtk_container_set_border_width (GTK_CONTAINER (widgets.notebook_comm_book), 1);
	
	
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(widgets.notebook_comm_book),
                                             GTK_POS_BOTTOM);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.
				    notebook_comm_book), TRUE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.
				     notebook_comm_book), FALSE);

	/*
	 * commentary pane
	 */
	widgets.box_comm = gui_create_commentary_pane();
	     
        gtk_container_add(GTK_CONTAINER(widgets.notebook_comm_book),
			  widgets.box_comm);
	

	label = gtk_label_new(_("Commentary View"));
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_comm_book),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.notebook_comm_book), 
				    0),
				   label);
			   
	/*
	 * book pane
     */   
	box_book = gui_create_book_pane();
        gtk_container_add(GTK_CONTAINER(widgets.notebook_comm_book),
			  box_book);
	

	label = gtk_label_new(_("Book View"));
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_comm_book),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.notebook_comm_book), 
				    1),
				   label);
				   
	/*
	 * dict/lex
     */
	widgets.box_dict = gui_create_dictionary_pane(); 
	gtk_paned_pack2(GTK_PANED(widgets.vpaned2),
			widgets.box_dict, TRUE, TRUE);
	/*
	 * end  dict/lex
	 */

	widgets.appbar = gnome_appbar_new(FALSE, TRUE,
					  GNOME_PREFERENCES_NEVER);
	gtk_widget_show(widgets.appbar);
	gnome_app_set_statusbar(GNOME_APP(widgets.app), widgets.appbar);

	gui_install_menu_hints(widgets.app);



  g_signal_connect ((gpointer) vbox_gs, "key_press_event",
                    G_CALLBACK (on_vbox1_key_press_event),
                    NULL);
  g_signal_connect ((gpointer) vbox_gs, "key_release_event",
                    G_CALLBACK (on_vbox1_key_release_event),
                    NULL);

	g_signal_connect(GTK_OBJECT(widgets.notebook_comm_book),
			   "switch_page",
			   G_CALLBACK
			   (on_notebook_comm_book_switch_page), 
			   NULL);

	g_signal_connect(GTK_OBJECT(widgets.app), "delete_event",
			   G_CALLBACK(delete_event), NULL);
			   
			   
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
	g_signal_connect(GTK_OBJECT(widgets.vpaned2),
			   "button_release_event",
			   G_CALLBACK
			   (epaned_button_release_event),
			   (gchar *) "vpaned2");
	g_signal_connect(GTK_OBJECT(widgets.hpaned),
			   "button_release_event",
			   G_CALLBACK
			   (epaned_button_release_event),
			   (gchar *) "hpaned1");

#ifdef OLD_NAVBAR
	gtk_widget_grab_focus(nav_bar.lookup_entry);
#else
	gtk_widget_grab_focus(navbar_versekey.lookup_entry);
#endif

	gtk_window_set_default_size((GtkWindow *)widgets.app,
                                             settings.gs_width,
                                             settings.gs_hight);
	main_window_created = TRUE;
}

/*
void new_main_window(void)
{
	gchar *glade_file;
	GladeXML *gxml;
	
	glade_file =
	    gui_general_user_file("main-window.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
	g_message(glade_file);

//	** build the widget **
	gxml = glade_xml_new(glade_file, "main-window", NULL);

	
	
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
