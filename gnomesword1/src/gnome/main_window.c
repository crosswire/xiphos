/*
 * GnomeSword Bible Study Tool
 * main_window.c - main window gui
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
#include <config.h>
#endif

#include <gnome.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/e-paned/e-vpaned.h>
#include "main/sword.h"
#include "main/settings.h"

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
#include "gui/interlinear.h"
#include "gui/percomm.h"
#include "gui/commentary.h"
#include "gui/gbs.h"
#include "gui/dictlex.h"
#include "gui/search_dialog.h"

WIDGETS widgets;

static char *book_open_xpm[] = {
	"16 16 4 1",
	"       c None s None",
	".      c black",
	"X      c #808080",
	"o      c white",
	"                ",
	"  ..            ",
	" .Xo.    ...    ",
	" .Xoo. ..oo.    ",
	" .Xooo.Xooo...  ",
	" .Xooo.oooo.X.  ",
	" .Xooo.Xooo.X.  ",
	" .Xooo.oooo.X.  ",
	" .Xooo.Xooo.X.  ",
	" .Xooo.oooo.X.  ",
	"  .Xoo.Xoo..X.  ",
	"   .Xo.o..ooX.  ",
	"    .X..XXXXX.  ",
	"    ..X.......  ",
	"     ..         ",
	"                "
};

static char *book_closed_xpm[] = {
	"16 16 6 1",
	"       c None s None",
	".      c black",
	"X      c red",
	"o      c yellow",
	"O      c #808080",
	"#      c white",
	"                ",
	"       ..       ",
	"     ..XX.      ",
	"   ..XXXXX.     ",
	" ..XXXXXXXX.    ",
	".ooXXXXXXXXX.   ",
	"..ooXXXXXXXXX.  ",
	".X.ooXXXXXXXXX. ",
	".XX.ooXXXXXX..  ",
	" .XX.ooXXX..#O  ",
	"  .XX.oo..##OO. ",
	"   .XX..##OO..  ",
	"    .X.#OO..    ",
	"     ..O..      ",
	"      ..        ",
	"                "
};

static char *mini_page_xpm[] = {
	"16 16 4 1",
	"       c None s None",
	".      c black",
	"X      c white",
	"o      c #808080",
	"                ",
	"   .......      ",
	"   .XXXXX..     ",
	"   .XoooX.X.    ",
	"   .XXXXX....   ",
	"   .XooooXoo.o  ",
	"   .XXXXXXXX.o  ",
	"   .XooooooX.o  ",
	"   .XXXXXXXX.o  ",
	"   .XooooooX.o  ",
	"   .XXXXXXXX.o  ",
	"   .XooooooX.o  ",
	"   .XXXXXXXX.o  ",
	"   ..........o  ",
	"    oooooooooo  ",
	"                "
};

GdkPixmap *pixmap1;
GdkPixmap *pixmap2;
GdkPixmap *pixmap3;
GdkBitmap *mask1;
GdkBitmap *mask2;
GdkBitmap *mask3;

extern HISTORY historylist[];	/* sturcture for storing history items */
extern gint historyitems;

/* Show/hide bible texts */
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


/* Show/hide Commentaries */
void gui_show_hide_comms(gboolean choice)
{
	settings.showcomms = choice;
	if (choice == FALSE) {
		gtk_widget_hide(widgets.notebook_comm);
	} else {
		gtk_widget_show(widgets.notebook_comm);
	}
	gui_set_bible_comm_layout();
}

/* Show/hide Dictionaries-Lexicons */
void gui_show_hide_dicts(gboolean choice)
{
	settings.showdicts = choice;
	if (choice == FALSE) {
		gtk_widget_hide(widgets.workbook_lower);
	} else {
		gtk_widget_show(widgets.workbook_lower);
	}
	gui_set_bible_comm_layout();
}

void gui_set_bible_comm_layout(void)
{
	if ((settings.showtexts == FALSE)
	    && (settings.showcomms == FALSE)) {
		e_paned_set_position(E_PANED(widgets.vpaned), 0);
	} else if (settings.showdicts == FALSE) {
		e_paned_set_position(E_PANED
				     (widgets.vpaned),
				     settings.gs_hight);
	} else {
		e_paned_set_position(E_PANED
				     (widgets.vpaned),
				     settings.upperpane_hight);
	}

	if ((settings.showtexts == FALSE)
	    && (settings.showcomms == TRUE)) {
		e_paned_set_position(E_PANED(widgets.hpaned), 0);
	} else if (settings.showtexts == FALSE) {
		e_paned_set_position(E_PANED(widgets.hpaned),
				     settings.gs_width);
	} else if ((settings.showtexts == TRUE)
		   && (settings.showcomms == FALSE)) {
		e_paned_set_position(E_PANED(widgets.hpaned),
				     settings.gs_width);
	} else if ((settings.showtexts == TRUE)
		   && (settings.showcomms == TRUE)) {
		e_paned_set_position(E_PANED(widgets.hpaned),
				     settings.biblepane_width);
	} else if (settings.showtexts == TRUE) {
		e_paned_set_position(E_PANED(widgets.hpaned),
				     settings.gs_width);
	} else {
		e_paned_set_position(E_PANED(widgets.hpaned),
				     settings.biblepane_width);
	}
}

/******************************************************************************
 * Name
 *  gui_change_window_title
 *
 * Synopsis
 *   #include "main_window.h"
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
	sprintf(title, "%s - %s", settings.program_title,
		get_module_description(module_name));
	gtk_window_set_title(GTK_WINDOW(widgets.app), title);
}




/******************************************************************************
 * Name
 *  gui_change_module_and_key
 *
 * Synopsis
 *   #include "main_window.h"
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
			page_num =
			    get_module_number(module_name, TEXT_MODS);
			val_key = gui_update_nav_controls(key);
			gui_set_text_page_and_key(page_num, val_key);
			free(val_key);
		}
		break;
	case COMMENTARY_TYPE:
		if (settings.havecomm) {
			page_num =
			    get_module_number(module_name, COMM_MODS);
			gui_set_commentary_page_and_key(page_num, key);
		}
		break;
	case DICTIONARY_TYPE:
		if (settings.havedict) {
			page_num =
			    get_module_number(module_name, DICT_MODS);
			gui_set_dictionary_page_and_key(page_num, key);
		}
		break;
	case BOOK_TYPE:
		if (settings.havebook) {
			page_num =
			    get_module_number(module_name, BOOK_MODS);
			if (key)
				gui_set_book_page_and_key(page_num,
							  key);
			else {
				gtk_notebook_set_page(GTK_NOTEBOOK
						      (widgets.
						       notebook_gbs),
						      page_num);
			}
		}
		break;
	}
}


/******************************************************************************
 * Name
 *  gui_change_verse
 *
 * Synopsis
 *   #include "main_window.h"
 *
 *   void gui_change_verse(gchar * key)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_change_verse(gchar * key)
{
	gchar *val_key;

	settings.apply_change = FALSE;
	val_key = gui_update_nav_controls(key);

	if (settings.havebible) {
		/* add item to history */
		if (settings.addhistoryitem) {
			if (strcmp
			    (settings.currentverse,
			     historylist[historyitems - 1].verseref))
				addHistoryItem(widgets.app,
					       GTK_WIDGET
					       (widgets.shortcutbar),
					       settings.currentverse);
		}
		settings.addhistoryitem = TRUE;

		/* change main window */
		gui_display_text(val_key);
	}

	/* 
	 * change interlinear verses 
	 */
	if (settings.dockedInt) {
		gui_update_interlinear_page();
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
	if (settings.havecomm)
		gui_display_commentary(val_key);

	free(val_key);
}


/******************************************************************************
 * Name
 *  html_button_pressed
 *
 * Synopsis
 *   #include "gs_gui.h"
 *
 *   gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
 *					gpointer *data)	
 *
 * Description
 *    mouse button pressed in interlinear window 
 *
 * Return value
 *   gint
 */

static gint button_release_event(GtkWidget * html,
				 GdkEventButton * event, gpointer data)
{
	settings.whichwindow = INTERLINEAR_WINDOW;

	switch (event->button) {
	case 1:
		return TRUE;
		break;
	case 2:
		/* 
		 * pass this for pasting 
		 */
		return TRUE;
		break;
	case 3:
		gtk_signal_emit_stop_by_name(GTK_OBJECT(html),
					     "button_press_event");
		return TRUE;
		break;
	default:
		return FALSE;
	}

	return FALSE;
}

/******************************************************************************
 * Name
 *   on_mainwindow_size_allocate
 *
 * Synopsis
 *   #include "gs_gui.h"
 *
 *   void on_mainwindow_size_allocate(GtkWidget * widget,
 *			GtkAllocation * allocation, gpointer user_data)
 *
 * Description
 *    store the app size when size is changed 
 *
 * Return value
 *   void
 */

static void on_mainwindow_size_allocate(GtkWidget * widget,
					GtkAllocation * allocation,
					gpointer user_data)
{
	settings.gs_width = allocation->width;
	settings.gs_hight = allocation->height;
}

/******************************************************************************
 * Name
 *   on_mainwindow_destroy
 *
 * Synopsis
 *   #include "gs_gui.h"
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
	shutdown_gnomesword();
	gtk_exit(0);
}

/******************************************************************************
 * Name
 *   on_epaned_button_release_event
 *
 * Synopsis
 *   #include "gs_gui.h"
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

	panesize = e_paned_get_position(E_PANED(widget));

	if (panesize > 15) {
		if (!strcmp((gchar *) user_data, "epaned"))
			settings.shortcutbar_width = panesize;
		if (!strcmp((gchar *) user_data, "vpaned1"))
			settings.upperpane_hight = panesize;
		if (!strcmp((gchar *) user_data, "hpaned1"))
			settings.biblepane_width = panesize;
	}
	return TRUE;
}


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
		gnome_appbar_set_progress((GnomeAppBar *) widgets.
					  appbar, num);
		printed++;
	}
	while (gtk_events_pending())
		gtk_main_iteration();
	printed = 0;
}

/******************************************************************************
 * Name
 *   create_mainwindow
 *
 * Synopsis
 *   #include "gs_gui.h"
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
	GtkWidget *label41;
	GtkWidget *label185;
	GtkWidget *label197;
	GtkWidget *hbox25;
	GdkColor transparent = { 0 };
	gint page_num = 0;

	g_print("%s\n", "Building GnomeSword interface");

	widgets.studypad_dialog = NULL;

	widgets.app =
	    gnome_app_new("gnomesword",
			  _("GnomeSword - Bible Study Software"));
	gtk_object_set_data(GTK_OBJECT(widgets.app), "widgets.app",
			    widgets.app);
	gtk_widget_set_usize(widgets.app, 680, 480);
	GTK_WIDGET_SET_FLAGS(widgets.app, GTK_CAN_FOCUS);
	gtk_window_set_policy(GTK_WINDOW(widgets.app), TRUE, TRUE,
			      TRUE);

	dock1 = GNOME_APP(widgets.app)->dock;
	gtk_widget_ref(dock1);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "dock1",
				 dock1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dock1);

	gui_create_main_menu(widgets.app);

	vbox_gs = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_gs);
	gnome_app_set_contents(GNOME_APP(widgets.app), vbox_gs);

	hbox25 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox25);
	gtk_box_pack_start(GTK_BOX(vbox_gs), hbox25, TRUE, TRUE, 0);

	widgets.epaned = e_hpaned_new();
	gtk_widget_show(widgets.epaned);
	gtk_box_pack_start(GTK_BOX(hbox25), widgets.epaned, TRUE, TRUE,
			   0);

	/*
	 * shortcut bar 
	 */

	mainPanel = gtk_vbox_new(FALSE, 0);
	e_paned_pack2(E_PANED(widgets.epaned), mainPanel, TRUE, TRUE);
	gtk_widget_show(mainPanel);

	vboxMain = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vboxMain);
	gtk_box_pack_start(GTK_BOX(mainPanel), vboxMain, TRUE, TRUE, 0);

	/*
	 * nav toolbar 
	 */
	 
	nav_toolbar = gui_create_nav_toolbar();
	gtk_box_pack_start(GTK_BOX(vboxMain), nav_toolbar, FALSE,
			   FALSE, 0);

	/*
	 * end nav toolbar 
	 */


	widgets.vpaned = e_vpaned_new();
	gtk_widget_show(widgets.vpaned);
	gtk_box_pack_end(GTK_BOX(vboxMain), widgets.vpaned, TRUE, TRUE,
			 0);
	gtk_container_set_border_width(GTK_CONTAINER(widgets.vpaned),
				       1);


	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox2);
	e_paned_pack1(E_PANED(widgets.vpaned), hbox2, TRUE, TRUE);

	widgets.hpaned = e_hpaned_new();
	gtk_widget_show(widgets.hpaned);
	gtk_box_pack_start(GTK_BOX(hbox2), widgets.hpaned, TRUE, TRUE,
			   0);


	widgets.vbox_text = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(widgets.vbox_text);
	e_paned_pack1(E_PANED(widgets.hpaned), widgets.vbox_text, FALSE,
		      TRUE);

	/*
	 * text notebook
	 */
	widgets.notebook_text = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_text);
	gtk_box_pack_start(GTK_BOX(widgets.vbox_text),
			   widgets.notebook_text, TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.notebook_text),
				    TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_text));
	gtk_notebook_set_show_border(GTK_NOTEBOOK
				     (widgets.notebook_text), FALSE);

	/*
	 * commentary notebook
	 */
	widgets.notebook_comm = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_comm);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.notebook_comm),
				    TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_comm));
	gtk_notebook_set_show_border(GTK_NOTEBOOK
				     (widgets.notebook_comm), FALSE);

	e_paned_pack2(E_PANED(widgets.hpaned), widgets.notebook_comm,
		      TRUE, TRUE);

	/*
	 * lower_workbook
	 */
	widgets.workbook_lower = gtk_notebook_new();
	gtk_widget_show(widgets.workbook_lower);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.workbook_lower));
	gtk_notebook_set_show_border(GTK_NOTEBOOK
				     (widgets.workbook_lower), FALSE);

	e_paned_pack2(E_PANED(widgets.vpaned), widgets.workbook_lower,
		      TRUE, TRUE);

	/*
	 * dict/lex  
	 */
	widgets.notebook_dict = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_dict);
	gtk_container_add(GTK_CONTAINER(widgets.workbook_lower),
			  widgets.notebook_dict);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.notebook_dict),
				    TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_dict));
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_dict),
				   settings.dict_tabs);
	gtk_notebook_set_show_border(GTK_NOTEBOOK
				     (widgets.notebook_dict), FALSE);

	label41 = gtk_label_new(_("Dict/Lex"));
	gtk_widget_show(label41);

	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.workbook_lower),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.workbook_lower), page_num),
				   label41);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.workbook_lower),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.workbook_lower),
					  page_num), _("Dict/Lex"));
	++page_num;
	/*
	 * end  dict/lex  
	 */

	/*
	 * gbs notebook 
	 */

	widgets.notebook_gbs = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_gbs);
	gtk_container_add(GTK_CONTAINER(widgets.workbook_lower),
			  widgets.notebook_gbs);
	GTK_WIDGET_UNSET_FLAGS(widgets.notebook_gbs, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_gbs),
				   FALSE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_gbs));
	gtk_notebook_set_show_border(GTK_NOTEBOOK
				     (widgets.notebook_gbs), FALSE);

	label185 = gtk_label_new(_("Books"));
	gtk_widget_show(label185);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.workbook_lower),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.workbook_lower), page_num),
				   label185);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.workbook_lower),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.workbook_lower),
					  page_num), _("Books"));
	++page_num;
	/*
	 * end gbs 
	 */

	/*
	 * percomm 
	 */
	if (!settings.use_percomm_dialog) {
		gui_percomm_in_workbook(widgets.workbook_lower,
					page_num);
		++page_num;
	}

	/*
	 * studypad editor 
	 */
	if (settings.use_studypad) {
		if (!settings.use_studypad_dialog) {
			settings.studypad_dialog_exist = FALSE;
			gui_open_studypad(widgets.workbook_lower,
					  settings.studypadfilename,
					  page_num);
			++page_num;
		}
	}
	/*
	 * end studypad editor 
	 */


	/*
	 * interlinear page 
	 */
	widgets.frame_interlinear = gtk_frame_new(NULL);
	gtk_widget_show(widgets.frame_interlinear);
	gtk_container_add(GTK_CONTAINER(widgets.workbook_lower),
			  widgets.frame_interlinear);

	swInt = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(swInt);
	gtk_container_add(GTK_CONTAINER(widgets.frame_interlinear),
			  swInt);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swInt),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);

	settings.dockedInt = TRUE;

	widgets.html_interlinear = gtk_html_new();
	gtk_widget_show(widgets.html_interlinear);
	gtk_html_load_empty(GTK_HTML(widgets.html_interlinear));
	gtk_container_add(GTK_CONTAINER(swInt),
			  widgets.html_interlinear);


	label197 = gtk_label_new(_("Interlinear"));
	gtk_widget_show(label197);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.workbook_lower),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.workbook_lower), page_num),
				   label197);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.workbook_lower),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.workbook_lower),
					  page_num), _("Interlinear"));
	settings.interlinear_page = page_num;

	/*
	 * end Interlinear page 
	 */

	widgets.appbar = gnome_appbar_new(FALSE, TRUE,
					  GNOME_PREFERENCES_NEVER);
	gtk_widget_show(widgets.appbar);
	gnome_app_set_statusbar(GNOME_APP(widgets.app), widgets.appbar);

	gui_install_menu_hints(widgets.app);

	/*
	 * create pixmaps for ctrees
	 */

	/* added to stop annoying gdk warning on startup */
	gtk_widget_realize(widgets.app);
	/* when these pixmaps are created */
	pixmap1 =
	    gdk_pixmap_create_from_xpm_d(widgets.app->window, &mask1,
					 &transparent, book_closed_xpm);
	pixmap2 =
	    gdk_pixmap_create_from_xpm_d(widgets.app->window, &mask2,
					 &transparent, book_open_xpm);
	pixmap3 =
	    gdk_pixmap_create_from_xpm_d(widgets.app->window, &mask3,
					 &transparent, mini_page_xpm);

	gtk_signal_connect(GTK_OBJECT(widgets.app), "destroy",
			   GTK_SIGNAL_FUNC(on_mainwindow_destroy),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(widgets.app), "size_allocate",
			   GTK_SIGNAL_FUNC(on_mainwindow_size_allocate),
			   NULL);

	gtk_signal_connect(GTK_OBJECT(widgets.epaned),
			   "button_release_event",
			   GTK_SIGNAL_FUNC(epaned_button_release_event),
			   (gchar *) "epaned");
	gtk_signal_connect(GTK_OBJECT(widgets.vpaned),
			   "button_release_event",
			   GTK_SIGNAL_FUNC(epaned_button_release_event),
			   (gchar *) "vpaned");
	gtk_signal_connect(GTK_OBJECT(widgets.hpaned),
			   "button_release_event",
			   GTK_SIGNAL_FUNC(epaned_button_release_event),
			   (gchar *) "hpaned1");

	gtk_signal_connect(GTK_OBJECT(widgets.html_interlinear),
			   "on_url", GTK_SIGNAL_FUNC(gui_url),
			   (gpointer) widgets.app);
	gtk_signal_connect(GTK_OBJECT(widgets.html_interlinear),
			   "link_clicked",
			   GTK_SIGNAL_FUNC(gui_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(widgets.html_interlinear),
			   "button_release_event",
			   GTK_SIGNAL_FUNC(button_release_event), NULL);

	gtk_widget_grab_focus(widgets.app);

	gtk_widget_set_usize(widgets.app, settings.gs_width,
			     settings.gs_hight);
}


/******************************************************************************
 * Name
 *   gui_show_main_window
 *
 * Synopsis
 *   #include "main_window.h"
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
