/*
 * GnomeSword Bible Study Tool
 * gs_gui.c - main window gui
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

#include "main/gs_gui.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"
#include "main/settings.h"
#include "main/support.h"

#include "gui/main_menu.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_dialog.h"
#include "gui/studypad.h"
#include "gui/toolbar_nav.h"

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
	showSBGroup(settings.searchbargroup);
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
 *   void on_mainwindow_destroy(GtkObject * object, SETTINGS * s)
 *
 * Description
 *    shutdown gnomesword 
 *
 * Return value
 *   void
 */ 

static void on_mainwindow_destroy(GtkObject * object, SETTINGS * s)
{
	gnomesword_shutdown();
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
	extern gboolean havepercomm;
	
	if (!firsttime) {
		/*
		 * Store the page number so we can open to it the next
		 * time we start.
		 */
		settings.notebook3page = page_num;
	}
	firsttime = FALSE;	
	
	if(havepercomm)
		gtk_widget_hide(settings.toolbarComments);
	gtk_widget_hide(settings.toolbarStudypad);

	if (page_num == 1 && settings.editnote) {
		gtk_widget_show(settings.toolbarComments);
	}

	else if (page_num == 2) {
		gtk_widget_show(settings.toolbarStudypad);
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
		    e_paned_get_position(E_PANED(settings.epaned));
	else
		panesize =
		    e_paned_get_position(E_PANED(lookup_widget
				  (settings.app,
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
	settings.app =
	    gnome_app_new("gnomesword",
			  _("GnomeSWORD - Bible Study Software"));
	gtk_object_set_data(GTK_OBJECT(settings.app), "settings.app",
			    settings.app);
	gtk_widget_set_usize(settings.app, 680, 480);
	GTK_WIDGET_SET_FLAGS(settings.app, GTK_CAN_FOCUS);
	gtk_window_set_policy(GTK_WINDOW(settings.app), TRUE, TRUE, TRUE);

	dock1 = GNOME_APP(settings.app)->dock;
	gtk_widget_ref(dock1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "dock1", dock1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dock1);

	gui_create_main_menu(settings.app);


	vbox_gs = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_gs);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox_gs",
				 vbox_gs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_gs);
	gnome_app_set_contents(GNOME_APP(settings.app), vbox_gs);


	settings.hbox_toolbar = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(settings.hbox_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
			"settings.hbox_toolbar", settings.hbox_toolbar,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.hbox_toolbar);
	gtk_box_pack_start(GTK_BOX(vbox_gs), settings.hbox_toolbar,
			FALSE, TRUE, 0);


	/*
	 * begin options toolbar 
	 */

	handleboxOptionsBar = gtk_handle_box_new();
	gtk_widget_ref(handleboxOptionsBar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "handleboxOptionsBar",
				 handleboxOptionsBar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handleboxOptionsBar);
	gtk_box_pack_start(GTK_BOX(settings.hbox_toolbar),
			handleboxOptionsBar, TRUE, TRUE, 0);

	toolbarOptions =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarOptions);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
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
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnSBDock",
				 btnSBDock,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSBDock);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_INDEX);
	btnSB =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarOptions),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Shortcut Bar", 
				       _("Hide/Show Shortcut Bar"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSB);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnSB", btnSB,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSB);


	vseparator13 = gtk_vseparator_new();
	gtk_widget_ref(vseparator13);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vseparator13",
				 vseparator13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator13);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarOptions),
				  vseparator13, NULL, NULL);
	gtk_widget_set_usize(vseparator13, 7, 7);
	
	tmp_toolbar_icon = 
		gnome_stock_pixmap_widget (settings.app, 
			GNOME_STOCK_PIXMAP_SEARCH);
	btn_search = gtk_toolbar_append_element (GTK_TOOLBAR 
				(toolbarOptions),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("Search"),
				NULL, NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btn_search);
	gtk_object_set_data_full(GTK_OBJECT (settings.app), "btn_search",
			btn_search, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btn_search);
	
	

	hbox25 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox25);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "hbox25",
				 hbox25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox25);
	gtk_box_pack_start(GTK_BOX(vbox_gs), hbox25, TRUE, TRUE, 0);

	settings.epaned = e_hpaned_new();
	gtk_widget_ref(settings.epaned);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "settings.epaned",
				 settings.epaned,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.epaned);
	gtk_box_pack_start(GTK_BOX(hbox25), settings.epaned, TRUE, TRUE, 0);
	
	/*
	 * shortcut bar 
	 */

	mainPanel = gtk_vbox_new(FALSE, 0);
	e_paned_pack2(E_PANED(settings.epaned), mainPanel, TRUE, TRUE);
	gtk_widget_show(mainPanel);

	vboxMain = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxMain);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
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
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vpaned1",
				 vpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vpaned1);
	gtk_box_pack_end(GTK_BOX(vboxMain), vpaned1, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vpaned1), 1);


	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "hbox2", hbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	e_paned_pack1(E_PANED(vpaned1), hbox2, TRUE, TRUE);

	hpaned1 = e_hpaned_new();
	gtk_widget_ref(hpaned1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "hpaned1",
				 hpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned1);
	gtk_box_pack_start(GTK_BOX(hbox2), hpaned1, TRUE, TRUE, 0);


	vbox_text = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_text);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox_text",
				 vbox_text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_text);
	e_paned_pack1(E_PANED(hpaned1), vbox_text, FALSE, TRUE);
	
	/*
	 * text notebook
	 */
	settings.notebook_text = gtk_notebook_new();
	gtk_widget_ref(settings.notebook_text);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
			"settings.notebook_text", settings.notebook_text,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.notebook_text);
	gtk_box_pack_start(GTK_BOX(vbox_text), settings.notebook_text,
			TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(settings.notebook_text),
			TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(settings.notebook_text));
	
	

	settings.workbook_upper = gtk_notebook_new();
	gtk_widget_ref(settings.workbook_upper);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
			"settings.workbook_upper", settings.workbook_upper,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.workbook_upper);
	
	
	e_paned_pack2(E_PANED(hpaned1), settings.workbook_upper, TRUE, TRUE);

	gtk_container_set_border_width(GTK_CONTAINER(settings.workbook_upper),
			2);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(settings.workbook_upper), 
			TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(settings.workbook_upper));

	vbox22 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox22);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox22",
				 vbox22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox22);
	gtk_container_add(GTK_CONTAINER(settings.workbook_upper), vbox22);

	settings.notebook_comm = gtk_notebook_new();
	gtk_widget_ref(settings.notebook_comm);
	gtk_widget_show(settings.notebook_comm);
	gtk_box_pack_start(GTK_BOX(vbox22), settings.notebook_comm,
			TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(settings.notebook_comm), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(settings.notebook_comm));
	/**********************************************************************************/
	
	label64 = gtk_label_new(_("Commentaries"));
	gtk_widget_ref(label64);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label64",
				 label64,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label64);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.workbook_upper),
				   gtk_notebook_get_nth_page(
				   GTK_NOTEBOOK(settings.workbook_upper),0), 
				   label64);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings.workbook_upper),
                                    gtk_notebook_get_nth_page(
				    GTK_NOTEBOOK(settings.workbook_upper),0), 
				    _("Commentaries"));

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(settings.workbook_upper), vbox2);

	settings.notebook_percomm = gtk_notebook_new();
	gtk_widget_ref(settings.notebook_percomm);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
			"settings.notebook_percomm", settings.notebook_percomm,
			 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.notebook_percomm);
	gtk_box_pack_start(GTK_BOX(vbox2), settings.notebook_percomm,
			TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(settings.notebook_percomm),
			TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(settings.notebook_percomm));
	GTK_WIDGET_UNSET_FLAGS(settings.notebook_percomm, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebook_percomm),
			TRUE);
	
	/*
	 * personal comments editor goes here
	 */
	
	label85 = gtk_label_new(_("Personal Comments"));
	gtk_widget_ref(label85);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label85",
				 label85,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label85);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.workbook_upper),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(settings.workbook_upper),
				1), label85);	
							     
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings.workbook_upper),
                                gtk_notebook_get_nth_page(
					GTK_NOTEBOOK(settings.workbook_upper),
					1), _("Personal Comments"));
	
		
	/*
	 * studypad editor 
	 */
	settings.html_studypad =
		gui_create_studypad_control(settings.workbook_upper, &settings);
	
				
	label41 = gtk_label_new(_("Study Pad"));
	gtk_widget_ref(label41);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label41",
				 label41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label41);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.workbook_upper),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(settings.workbook_upper), 2),
			label41);
	
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings.workbook_upper),
                                gtk_notebook_get_nth_page(
					GTK_NOTEBOOK(settings.workbook_upper),
					2), _("Study Pad"));		
							
	/*
	 * end studypad editor 
	 */									     
	hbox8 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox8);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "hbox8", hbox8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox8);


	e_paned_pack2(E_PANED(vpaned1), hbox8, TRUE, TRUE);

	settings.workbook_lower = gtk_notebook_new ();
	gtk_widget_ref (settings.workbook_lower);
	gtk_object_set_data_full (GTK_OBJECT (settings.app),
			"settings.workbook_lower", settings.workbook_lower,
				(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.workbook_lower);
	gtk_box_pack_start(GTK_BOX (hbox8), settings.workbook_lower,
			TRUE, TRUE, 0);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(settings.workbook_lower));
	
	/*
	 * dict/lex  
	 */
	
	settings.notebookDL = gtk_notebook_new();
	gtk_widget_ref(settings.notebookDL);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
			"settings.notebookDL", settings.notebookDL,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.notebookDL);
	gtk_container_add(GTK_CONTAINER(settings.workbook_lower),
			settings.notebookDL);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(settings.notebookDL), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(settings.notebookDL));	
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebookDL),
					   settings.dict_tabs);
				
	label41 = gtk_label_new(_("Dict/Lex"));
	gtk_widget_ref(label41);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label41",
				 label41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label41);
	
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(settings.workbook_lower), 0),
			label41);				   
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(settings.workbook_lower), 0),
			_("Dict/Lex"));
 	
	/*
	 * end  dict/lex  
	 */
	
	/*
	 * gbs notebook 
	 */
	
	settings.notebook_gbs = gtk_notebook_new ();
	gtk_widget_ref(settings.notebook_gbs);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
			"settings.notebookGBS", settings.notebook_gbs,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.notebook_gbs);
	gtk_container_add(GTK_CONTAINER(settings.workbook_lower),
			settings.notebook_gbs);
	GTK_WIDGET_UNSET_FLAGS(settings.notebook_gbs, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebook_gbs), FALSE);	
	gtk_notebook_popup_enable(GTK_NOTEBOOK(settings.notebook_gbs));	
	
	label185 = gtk_label_new(_("Books"));
	gtk_widget_ref(label185);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label185",
			label185, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label185);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(settings.workbook_lower), 1),
			label185);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(settings.workbook_lower), 1),
			_("Books"));
	/*
	 * end gbs 
	 */
	
	
	
	/*
	 * interlinear page 
	 */
	settings.frameInt = gtk_frame_new (NULL);
	gtk_widget_ref (settings.frameInt);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
			"settings.frameInt", settings.frameInt,
			(GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(settings.frameInt);
	gtk_container_add(GTK_CONTAINER(settings.workbook_lower),
			settings.frameInt);
		
	swInt = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swInt);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
			"swInt", swInt, (GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(swInt);
	gtk_container_add(GTK_CONTAINER(settings.frameInt), swInt);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swInt),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
				       
	settings.dockedInt = TRUE;
	
	settings.htmlInterlinear = gtk_html_new();
	gtk_widget_ref(settings.htmlInterlinear);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
			"settings.htmlInterlinear", settings.htmlInterlinear,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings.htmlInterlinear);	
	gtk_html_load_empty(GTK_HTML(settings.htmlInterlinear));

	gtk_container_add(GTK_CONTAINER(swInt), settings.htmlInterlinear);
		
		
	label197 = gtk_label_new(_("Interlinear"));
	gtk_widget_ref(label197);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label197",
			label197, (GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(label197);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.workbook_lower), 
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(settings.workbook_lower), 2),
			label197);

	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings.workbook_lower),
			gtk_notebook_get_nth_page(
				GTK_NOTEBOOK(settings.workbook_lower), 2),
			_("Interlinear"));
	
	/*
	 * end Interlinear page 
	 */
	 
	settings.appbar = gnome_appbar_new(TRUE, TRUE,
			GNOME_PREFERENCES_NEVER);
	gtk_widget_ref(settings.appbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "settings.appbar",
			settings.appbar, (GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(settings.appbar);
	gnome_app_set_statusbar(GNOME_APP(settings.app), settings.appbar);

	gui_install_menu_hints(settings.app);

        /*
	 * create pixmaps for ctrees
	 */

	/* added to stop annoying gdk warning on startup */
	gtk_widget_realize(settings.app);
	/* when these pixmaps are created */
	pixmap1 = gdk_pixmap_create_from_xpm_d(settings.app->window, &mask1,
			&transparent, book_closed_xpm);
	pixmap2 = gdk_pixmap_create_from_xpm_d(settings.app->window, &mask2,
			&transparent, book_open_xpm);
	pixmap3 = gdk_pixmap_create_from_xpm_d(settings.app->window, &mask3,
			&transparent, mini_page_xpm);

	gtk_signal_connect(GTK_OBJECT(settings.app), "destroy",
			GTK_SIGNAL_FUNC(on_mainwindow_destroy), 
			(gpointer)&settings);
	gtk_signal_connect(GTK_OBJECT(settings.app), "size_allocate",
			GTK_SIGNAL_FUNC(on_mainwindow_size_allocate), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSBDock), "clicked",
			GTK_SIGNAL_FUNC(on_btnSBDock_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSB), "clicked",
			GTK_SIGNAL_FUNC(on_btnSB_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btn_search), "clicked",
			GTK_SIGNAL_FUNC(on_btn_search_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(settings.workbook_upper), "switch_page",
			GTK_SIGNAL_FUNC(workbook_upper_switch_page), NULL);

	gtk_signal_connect(GTK_OBJECT(settings.epaned), "button_release_event",
			GTK_SIGNAL_FUNC(epaned_button_release_event),
			(gchar *) "epaned");
	gtk_signal_connect(GTK_OBJECT(vpaned1), "button_release_event",
			GTK_SIGNAL_FUNC(epaned_button_release_event),
			(gchar *) "vpaned1");
	gtk_signal_connect(GTK_OBJECT(hpaned1), "button_release_event",
			GTK_SIGNAL_FUNC(epaned_button_release_event),
			(gchar *) "hpaned1");

	gtk_signal_connect(GTK_OBJECT(settings.htmlInterlinear), "on_url",
			GTK_SIGNAL_FUNC(on_url), (gpointer)settings.app);
	gtk_signal_connect(GTK_OBJECT(settings.htmlInterlinear),
			"link_clicked", GTK_SIGNAL_FUNC(on_link_clicked),
			NULL);
	gtk_signal_connect(GTK_OBJECT(settings.htmlInterlinear),
			"button_release_event",
			GTK_SIGNAL_FUNC(button_release_event), NULL);
	
	gtk_widget_grab_focus(settings.app);

	gtk_widget_set_usize(settings.app, settings.gs_width,
			settings.gs_hight);
}

