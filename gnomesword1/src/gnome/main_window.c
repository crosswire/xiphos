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
	 * set program title to GnomeSWORD + current module name 
	 */
	sprintf(title, "GnomeSWORD - %s", get_module_description(module_name));
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
		if(settings.havebible) {
			page_num =
			    get_module_number(module_name, 
							TEXT_MODS);
			val_key = gui_update_nav_controls(key);
			gui_set_text_page_and_key(page_num, val_key);
			free(val_key);
		}
		break;
	case COMMENTARY_TYPE:
		if(settings.havecomm) {
			page_num =
			    get_module_number(module_name, 
							COMM_MODS);
			gui_set_commentary_page_and_key(page_num, key);
		}
		break;
	case DICTIONARY_TYPE:
		if(settings.havedict) {
			page_num =
			    get_module_number(module_name, 
							DICT_MODS);
			gui_set_dictionary_page_and_key(page_num, key);
		}
		break;
	case BOOK_TYPE:
		if(settings.havebook) {
			page_num =
			    get_module_number(module_name, 
							BOOK_MODS);
			if(key)
				gui_set_book_page_and_key(page_num, key);
			else {
				gtk_notebook_set_page(GTK_NOTEBOOK(
					widgets.notebook_gbs),
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

	val_key = gui_update_nav_controls(key);

	settings.apply_change = FALSE;
	
	if(settings.havebible) {
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
	if (settings.notefollow) {	                  
		if (!settings.editnote)
			if (settings.havepercomm)
				gui_display_percomm(val_key);
	}
	/* 
	 * set commentary module to current verse 
	 */
	if (settings.havecomm)
		gui_display_commentary(val_key);
	
	free(val_key);
	settings.apply_change = TRUE;
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

static gint button_release_event(GtkWidget * html, GdkEventButton * event,
					gpointer data)
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
	}

	return FALSE;
}

/******************************************************************************
 * Name
 *   on_btnSBDock_clicked
 *
 * Synopsis
 *   #include "gs_gui.h"
 *
 *   void on_btnSBDock_clicked(GtkButton *button, gpointer user_data)
 *
 * Description
 *    toogle shortcut bar - attached/detached 
 *
 * Return value
 *   void
 */ 

static void on_btnSBDock_clicked(GtkButton * button, gpointer user_data)
{
	gui_attach_detach_shortcutbar();
}

/******************************************************************************
 * Name
 *    on_btnSB_clicked
 *
 * Synopsis
 *   #include "gs_gui.h"
 *
 *   void on_btnSB_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   show hide shortcut bar
 *
 * Return value
 *   void
 */

static void on_btnSB_clicked(GtkButton * button, gpointer user_data)
{
	gui_shortcutbar_showhide();
}

/******************************************************************************
 * Name
 *   on_btn_search_clicked
 *
 * Synopsis
 *   #include "gs_gui.h"
 *
 *   void on_btn_search_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *    displays the shortcut bar search group 
 *    will open the new search dialog (FIXME: build new search dialog)
 *
 * Return value
 *   void
 */ 


static void on_btn_search_clicked(GtkButton *button, gpointer user_data)
{
	gui_do_dialog_search();
	//showSBGroup(settings.searchbargroup);
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
			GtkAllocation * allocation, gpointer user_data)
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

static void on_mainwindow_destroy(GtkObject *object, gpointer user_data)
{
	shutdown_gnomesword();
	gtk_exit(0);
}

/******************************************************************************
 * Name
 *   workbook_upper_switch_page
 *
 * Synopsis
 *   #include "gs_gui.h"
 *
 *   void workbook_upper_switch_page(GtkNotebook * notebook,
 *			 GtkNotebookPage * page,
 *			 gint page_num, gpointer user_data)
 *
 * Description
 *    notebook page changed (commentary, percomm and studypad pages) 
 *
 * Return value
 *   void
 */ 

static void workbook_upper_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 gint page_num, gpointer user_data)
{
	static gboolean firsttime = TRUE;
	
	if (!firsttime) {
		/*
		 * Store the page number so we can open to it the next
		 * time we start.
		 */
		settings.notebook3page = page_num;
	}
	firsttime = FALSE;	
	
	if(settings.havepercomm)
		gtk_widget_hide(widgets.toolbar_comments);
	gtk_widget_hide(widgets.toolbar_studypad);

	if (page_num == 1 && settings.editnote) {
		gtk_widget_show(widgets.toolbar_comments);
	}

	else if (page_num == 2) {
		gtk_widget_show(widgets.toolbar_studypad);
	}
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
			GdkEventButton * event, gpointer user_data)
{
	gint panesize;
	if(!strcmp((gchar*)user_data,"epaned")) 
		panesize = 
		    e_paned_get_position(E_PANED(widgets.epaned));
	else
		panesize =
		    e_paned_get_position(E_PANED(gui_lookup_widget
				  (widgets.app,
				   (gchar *) user_data)));

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

static void on_mainwindow_set_focus(GtkWindow * window, GtkWidget * widget,
						gpointer user_data)
{
	//g_warning("focus set to main window");
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
		gnome_appbar_set_progress((GnomeAppBar *) widgets.appbar, num);
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
	GtkWidget *handleboxOptionsBar;
	GtkWidget *toolbarOptions;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSBDock;
	GtkWidget *btnSB;
	GtkWidget *btn_search;
	GtkWidget *mainPanel;
	GtkWidget *vboxMain;
	GtkWidget *vpaned1;
	GtkWidget *hbox2;
	GtkWidget *nav_toolbar;
	GtkWidget *hpaned1;
	GtkWidget *vbox_text;
	GtkWidget *vbox22;
	GtkWidget *label64;
	GtkWidget *vbox2;
	GtkWidget *label85;
	GtkWidget *swInt;
	GtkWidget *label41;
	GtkWidget *hbox8;
	GtkWidget *label185;
	GtkWidget *label197;
	GtkWidget *hbox25;
	GtkWidget *vseparator13;
	GdkColor transparent = { 0 };
	
	g_print("%s\n", "Building GnomeSword interface");
	widgets.app =
	    gnome_app_new("gnomesword",
			  _("GnomeSWORD - Bible Study Software"));
	gtk_object_set_data(GTK_OBJECT(widgets.app), "widgets.app",
			    widgets.app);
	gtk_widget_set_usize(widgets.app, 680, 480);
	GTK_WIDGET_SET_FLAGS(widgets.app, GTK_CAN_FOCUS);
	gtk_window_set_policy(GTK_WINDOW(widgets.app), TRUE, TRUE, TRUE);

	dock1 = GNOME_APP(widgets.app)->dock;
	gtk_widget_ref(dock1);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "dock1", dock1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dock1);

	gui_create_main_menu(widgets.app);


	vbox_gs = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_gs);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vbox_gs",
				 vbox_gs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_gs);
	gnome_app_set_contents(GNOME_APP(widgets.app), vbox_gs);


	widgets.hbox_toolbar = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(widgets.hbox_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
			"widgets.hbox_toolbar", widgets.hbox_toolbar,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.hbox_toolbar);
	gtk_box_pack_start(GTK_BOX(vbox_gs), widgets.hbox_toolbar,
			FALSE, TRUE, 0);


	/*
	 * begin options toolbar 
	 */

	handleboxOptionsBar = gtk_handle_box_new();
	gtk_widget_ref(handleboxOptionsBar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "handleboxOptionsBar",
				 handleboxOptionsBar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handleboxOptionsBar);
	gtk_box_pack_start(GTK_BOX(widgets.hbox_toolbar),
			handleboxOptionsBar, TRUE, TRUE, 0);

	toolbarOptions =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarOptions);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "toolbarOptions", toolbarOptions,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarOptions);
	gtk_container_add(GTK_CONTAINER(handleboxOptionsBar),toolbarOptions);
	gtk_widget_set_usize(toolbarOptions, -2, 34);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarOptions),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
		gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR "/dock.xpm");
    
	btnSBDock =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarOptions),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Detach",
				       _("Detach/Attach Shortcut Bar"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSBDock);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "btnSBDock",
				 btnSBDock,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSBDock);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_INDEX);
	btnSB =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarOptions),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Shortcut Bar", 
				       _("Hide/Show Shortcut Bar"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSB);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "btnSB", btnSB,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSB);


	vseparator13 = gtk_vseparator_new();
	gtk_widget_ref(vseparator13);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vseparator13",
				 vseparator13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator13);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarOptions),
				  vseparator13, NULL, NULL);
	gtk_widget_set_usize(vseparator13, 7, 7);
	
	tmp_toolbar_icon = 
		gnome_stock_pixmap_widget (widgets.app, 
			GNOME_STOCK_PIXMAP_SEARCH);
	btn_search = gtk_toolbar_append_element (GTK_TOOLBAR 
				(toolbarOptions),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("Search"),
				NULL, NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btn_search);
	gtk_object_set_data_full(GTK_OBJECT (widgets.app), "btn_search",
			btn_search, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btn_search);
	
	

	hbox25 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox25);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "hbox25",
				 hbox25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox25);
	gtk_box_pack_start(GTK_BOX(vbox_gs), hbox25, TRUE, TRUE, 0);

	widgets.epaned = e_hpaned_new();
	gtk_widget_ref(widgets.epaned);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "widgets.epaned",
				 widgets.epaned,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.epaned);
	gtk_box_pack_start(GTK_BOX(hbox25), widgets.epaned, TRUE, TRUE, 0);
	
	/*
	 * shortcut bar 
	 */

	mainPanel = gtk_vbox_new(FALSE, 0);
	e_paned_pack2(E_PANED(widgets.epaned), mainPanel, TRUE, TRUE);
	gtk_widget_show(mainPanel);

	vboxMain = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxMain);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "vboxMain", vboxMain,
				 (GtkDestroyNotify) gtk_widget_unref);
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


	vpaned1 = e_vpaned_new();
	gtk_widget_ref(vpaned1);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vpaned1",
				 vpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vpaned1);
	gtk_box_pack_end(GTK_BOX(vboxMain), vpaned1, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vpaned1), 1);


	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "hbox2", hbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	e_paned_pack1(E_PANED(vpaned1), hbox2, TRUE, TRUE);

	hpaned1 = e_hpaned_new();
	gtk_widget_ref(hpaned1);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "hpaned1",
				 hpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned1);
	gtk_box_pack_start(GTK_BOX(hbox2), hpaned1, TRUE, TRUE, 0);


	vbox_text = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_text);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vbox_text",
				 vbox_text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_text);
	e_paned_pack1(E_PANED(hpaned1), vbox_text, FALSE, TRUE);
	
	/*
	 * text notebook
	 */
	widgets.notebook_text = gtk_notebook_new();
	gtk_widget_ref(widgets.notebook_text);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
			"widgets.notebook_text", widgets.notebook_text,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.notebook_text);
	gtk_box_pack_start(GTK_BOX(vbox_text), widgets.notebook_text,
			TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.notebook_text),
			TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_text));
	
	

	widgets.workbook_upper = gtk_notebook_new();
	gtk_widget_ref(widgets.workbook_upper);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
			"widgets.workbook_upper", widgets.workbook_upper,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.workbook_upper);
	
	
	e_paned_pack2(E_PANED(hpaned1), widgets.workbook_upper, TRUE, TRUE);

	gtk_container_set_border_width(GTK_CONTAINER(widgets.workbook_upper),
			2);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.workbook_upper), 
			TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.workbook_upper));

	vbox22 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox22);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vbox22",
				 vbox22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox22);
	gtk_container_add(GTK_CONTAINER(widgets.workbook_upper), vbox22);

	widgets.notebook_comm = gtk_notebook_new();
	gtk_widget_ref(widgets.notebook_comm);
	gtk_widget_show(widgets.notebook_comm);
	gtk_box_pack_start(GTK_BOX(vbox22), widgets.notebook_comm,
			TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.notebook_comm), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_comm));
	/**********************************************************************************/
	
	label64 = gtk_label_new(_("Commentaries"));
	gtk_widget_ref(label64);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label64",
				 label64,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label64);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.workbook_upper),
				   gtk_notebook_get_nth_page(
				   GTK_NOTEBOOK(widgets.workbook_upper),0), 
				   label64);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.workbook_upper),
                                    gtk_notebook_get_nth_page(
				    GTK_NOTEBOOK(widgets.workbook_upper),0), 
				    _("Commentaries"));

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(widgets.workbook_upper), vbox2);

	widgets.notebook_percomm = gtk_notebook_new();
	gtk_widget_ref(widgets.notebook_percomm);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
			"widgets.notebook_percomm", widgets.notebook_percomm,
			 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.notebook_percomm);
	gtk_box_pack_start(GTK_BOX(vbox2), widgets.notebook_percomm,
			TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.notebook_percomm),
			TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_percomm));
	GTK_WIDGET_UNSET_FLAGS(widgets.notebook_percomm, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_percomm),
			TRUE);
	
	/*
	 * personal comments editor goes here
	 */
	
	label85 = gtk_label_new(_("Personal Comments"));
	gtk_widget_ref(label85);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label85",
				 label85,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label85);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.workbook_upper),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(widgets.workbook_upper),
				1), label85);	
							     
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.workbook_upper),
                                gtk_notebook_get_nth_page(
					GTK_NOTEBOOK(widgets.workbook_upper),
					1), _("Personal Comments"));
	
		
	/*
	 * studypad editor 
	 */
	widgets.html_studypad =
		gui_create_studypad_control(widgets.workbook_upper);
	
				
	label41 = gtk_label_new(_("Study Pad"));
	gtk_widget_ref(label41);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label41",
				 label41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label41);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.workbook_upper),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(widgets.workbook_upper), 2),
			label41);
	
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.workbook_upper),
                                gtk_notebook_get_nth_page(
					GTK_NOTEBOOK(widgets.workbook_upper),
					2), _("Study Pad"));		
							
	/*
	 * end studypad editor 
	 */									     
	hbox8 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox8);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "hbox8", hbox8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox8);


	e_paned_pack2(E_PANED(vpaned1), hbox8, TRUE, TRUE);

	widgets.workbook_lower = gtk_notebook_new ();
	gtk_widget_ref (widgets.workbook_lower);
	gtk_object_set_data_full (GTK_OBJECT (widgets.app),
			"widgets.workbook_lower", widgets.workbook_lower,
				(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.workbook_lower);
	gtk_box_pack_start(GTK_BOX (hbox8), widgets.workbook_lower,
			TRUE, TRUE, 0);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.workbook_lower));
	
	/*
	 * dict/lex  
	 */
	
	widgets.notebook_dict = gtk_notebook_new();
	gtk_widget_ref(widgets.notebook_dict);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
			"widgets.notebook_dict", widgets.notebook_dict,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.notebook_dict);
	gtk_container_add(GTK_CONTAINER(widgets.workbook_lower),
			widgets.notebook_dict);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.notebook_dict), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_dict));	
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_dict),
					   settings.dict_tabs);
				
	label41 = gtk_label_new(_("Dict/Lex"));
	gtk_widget_ref(label41);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label41",
				 label41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label41);
	
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(widgets.workbook_lower), 0),
			label41);				   
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(widgets.workbook_lower), 0),
			_("Dict/Lex"));
 	
	/*
	 * end  dict/lex  
	 */
	
	/*
	 * gbs notebook 
	 */
	
	widgets.notebook_gbs = gtk_notebook_new ();
	gtk_widget_ref(widgets.notebook_gbs);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
			"settings.notebookGBS", widgets.notebook_gbs,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.notebook_gbs);
	gtk_container_add(GTK_CONTAINER(widgets.workbook_lower),
			widgets.notebook_gbs);
	GTK_WIDGET_UNSET_FLAGS(widgets.notebook_gbs, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_gbs), FALSE);	
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_gbs));	
	
	label185 = gtk_label_new(_("Books"));
	gtk_widget_ref(label185);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label185",
			label185, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label185);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(widgets.workbook_lower), 1),
			label185);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(widgets.workbook_lower), 1),
			_("Books"));
	/*
	 * end gbs 
	 */
	
	
	
	/*
	 * interlinear page 
	 */
	widgets.frame_interlinear = gtk_frame_new (NULL);
	gtk_widget_ref (widgets.frame_interlinear);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
			"widgets.frame_interlinear", widgets.frame_interlinear,
			(GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(widgets.frame_interlinear);
	gtk_container_add(GTK_CONTAINER(widgets.workbook_lower),
			widgets.frame_interlinear);
		
	swInt = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swInt);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
			"swInt", swInt, (GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(swInt);
	gtk_container_add(GTK_CONTAINER(widgets.frame_interlinear), swInt);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swInt),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
				       
	settings.dockedInt = TRUE;
	
	widgets.html_interlinear = gtk_html_new();
	gtk_widget_ref(widgets.html_interlinear);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
			"widgets.html_interlinear", widgets.html_interlinear,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.html_interlinear);	
	gtk_html_load_empty(GTK_HTML(widgets.html_interlinear));

	gtk_container_add(GTK_CONTAINER(swInt), widgets.html_interlinear);
		
		
	label197 = gtk_label_new(_("Interlinear"));
	gtk_widget_ref(label197);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label197",
			label197, (GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(label197);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.workbook_lower), 
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(widgets.workbook_lower), 2),
			label197);

	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(widgets.workbook_lower), 2),
			_("Interlinear"));
	
	/*
	 * end Interlinear page 
	 */
	 
	widgets.appbar = gnome_appbar_new(TRUE, TRUE,
			GNOME_PREFERENCES_NEVER);
	gtk_widget_ref(widgets.appbar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "widgets.appbar",
			widgets.appbar, (GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(widgets.appbar);
	gnome_app_set_statusbar(GNOME_APP(widgets.app), widgets.appbar);

	gui_install_menu_hints(widgets.app);

        /*
	 * create pixmaps for ctrees
	 */

	/* added to stop annoying gdk warning on startup */
	gtk_widget_realize(widgets.app);
	/* when these pixmaps are created */
	pixmap1 = gdk_pixmap_create_from_xpm_d(widgets.app->window, &mask1,
			&transparent, book_closed_xpm);
	pixmap2 = gdk_pixmap_create_from_xpm_d(widgets.app->window, &mask2,
			&transparent, book_open_xpm);
	pixmap3 = gdk_pixmap_create_from_xpm_d(widgets.app->window, &mask3,
			&transparent, mini_page_xpm);

	gtk_signal_connect(GTK_OBJECT(widgets.app), "destroy",
			GTK_SIGNAL_FUNC(on_mainwindow_destroy), NULL);
	gtk_signal_connect(GTK_OBJECT(widgets.app), "size_allocate",
			GTK_SIGNAL_FUNC(on_mainwindow_size_allocate), NULL);
	gtk_signal_connect (GTK_OBJECT (widgets.app), "set_focus",
                        GTK_SIGNAL_FUNC(on_mainwindow_set_focus),
                        NULL);
	gtk_signal_connect(GTK_OBJECT(btnSBDock), "clicked",
			GTK_SIGNAL_FUNC(on_btnSBDock_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSB), "clicked",
			GTK_SIGNAL_FUNC(on_btnSB_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btn_search), "clicked",
			GTK_SIGNAL_FUNC(on_btn_search_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(widgets.workbook_upper), "switch_page",
			GTK_SIGNAL_FUNC(workbook_upper_switch_page), NULL);

	gtk_signal_connect(GTK_OBJECT(widgets.epaned), "button_release_event",
			GTK_SIGNAL_FUNC(epaned_button_release_event),
			(gchar *) "epaned");
	gtk_signal_connect(GTK_OBJECT(vpaned1), "button_release_event",
			GTK_SIGNAL_FUNC(epaned_button_release_event),
			(gchar *) "vpaned1");
	gtk_signal_connect(GTK_OBJECT(hpaned1), "button_release_event",
			GTK_SIGNAL_FUNC(epaned_button_release_event),
			(gchar *) "hpaned1");

	gtk_signal_connect(GTK_OBJECT(widgets.html_interlinear), "on_url",
			GTK_SIGNAL_FUNC(gui_url), (gpointer)widgets.app);
	gtk_signal_connect(GTK_OBJECT(widgets.html_interlinear),
			"link_clicked", GTK_SIGNAL_FUNC(gui_link_clicked),
			NULL);
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
