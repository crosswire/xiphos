/*
 * GnomeSword Bible Study Tool
 * gs_gui.c - SHORT DESCRIPTION
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

/* main */ 
#include "gs_gui.h"
#include "gs_gnomesword.h"
#include "gs_html.h"
#include "gs_shortcutbar.h"
#include "settings.h"
#include "support.h"

/* frontend */
#include "main_menu.h"
#include "shortcutbar_dialog.h"
#include "studypad.h"
#include "toolbar_nav.h"


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
 *   void on_btnSBDock_clicked(GtkButton * button, SETTINGS * s)
 *
 * Description
 *    toogle shortcut bar - attached/detached 
 *
 * Return value
 *   void
 */ 

static void on_btnSBDock_clicked(GtkButton * button, SETTINGS * s)
{
	gui_attach_detach_shortcutbar(s);
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
	showSBGroup(&settings, settings.searchbargroup);
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
	gnomesword_shutdown(s);
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
		changepagenotebook(notebook, page_num);	
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
 *   void create_mainwindow(SETTINGS *s)
 *
 * Description
 *    create gnomesword gui 
 *
 * Return value
 *   void
 */ 

void create_mainwindow(SETTINGS *s)
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
	s->app =
	    gnome_app_new("gnomesword",
			  _("GnomeSWORD - Bible Study Software"));
	gtk_object_set_data(GTK_OBJECT(s->app), "s->app",
			    s->app);
	gtk_widget_set_usize(s->app, 680, 480);
	GTK_WIDGET_SET_FLAGS(s->app, GTK_CAN_FOCUS);
	gtk_window_set_policy(GTK_WINDOW(s->app), TRUE, TRUE, TRUE);

	dock1 = GNOME_APP(s->app)->dock;
	gtk_widget_ref(dock1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "dock1", dock1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dock1);

	gui_create_main_menu(s->app);


	vbox_gs = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_gs);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox_gs",
				 vbox_gs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_gs);
	gnome_app_set_contents(GNOME_APP(s->app), vbox_gs);


	s->hbox_toolbar = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(s->hbox_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->hbox_toolbar",
				 s->hbox_toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->hbox_toolbar);
	gtk_box_pack_start(GTK_BOX(vbox_gs), s->hbox_toolbar, FALSE, TRUE, 0);


	/*
	 * begin options toolbar 
	 */

	handleboxOptionsBar = gtk_handle_box_new();
	gtk_widget_ref(handleboxOptionsBar);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "handleboxOptionsBar",
				 handleboxOptionsBar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handleboxOptionsBar);
	gtk_box_pack_start(GTK_BOX(s->hbox_toolbar), handleboxOptionsBar, TRUE,
			   TRUE, 0);

	toolbarOptions =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarOptions);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
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
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnSBDock",
				 btnSBDock,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSBDock);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_INDEX);
	btnSB =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarOptions),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Shortcut Bar", 
				       _("Hide/Show Shortcut Bar"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSB);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnSB", btnSB,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSB);


	vseparator13 = gtk_vseparator_new();
	gtk_widget_ref(vseparator13);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vseparator13",
				 vseparator13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator13);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarOptions),
				  vseparator13, NULL, NULL);
	gtk_widget_set_usize(vseparator13, 7, 7);
	
	tmp_toolbar_icon = 
		gnome_stock_pixmap_widget (s->app, 
			GNOME_STOCK_PIXMAP_SEARCH);
	btn_search = gtk_toolbar_append_element (GTK_TOOLBAR 
				(toolbarOptions),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("Search"),
				NULL, NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref (btn_search);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "btn_search", btn_search,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (btn_search);
	
	

	hbox25 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox25);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hbox25",
				 hbox25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox25);
	gtk_box_pack_start(GTK_BOX(vbox_gs), hbox25, TRUE, TRUE, 0);

	s->epaned = e_hpaned_new();
	gtk_widget_ref(s->epaned);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->epaned",
				 s->epaned,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->epaned);
	gtk_box_pack_start(GTK_BOX(hbox25), s->epaned, TRUE, TRUE, 0);
	
	/*
	 * shortcut bar 
	 */

	mainPanel = gtk_vbox_new(FALSE, 0);
	e_paned_pack2(E_PANED(s->epaned), mainPanel, TRUE, TRUE);
	gtk_widget_show(mainPanel);

	vboxMain = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxMain);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
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
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vpaned1",
				 vpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vpaned1);
	gtk_box_pack_end(GTK_BOX(vboxMain), vpaned1, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vpaned1), 1);


	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hbox2", hbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	e_paned_pack1(E_PANED(vpaned1), hbox2, TRUE, TRUE);

	hpaned1 = e_hpaned_new();
	gtk_widget_ref(hpaned1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hpaned1",
				 hpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned1);
	gtk_box_pack_start(GTK_BOX(hbox2), hpaned1, TRUE, TRUE, 0);


	vbox_text = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_text);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox_text",
				 vbox_text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_text);
	e_paned_pack1(E_PANED(hpaned1), vbox_text, FALSE, TRUE);
	
	/*
	 * text notebook
	 */
	s->notebook_text = gtk_notebook_new();
	gtk_widget_ref(s->notebook_text);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->notebook_text",
				 s->notebook_text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->notebook_text);
	gtk_box_pack_start(GTK_BOX(vbox_text), s->notebook_text, TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(s->notebook_text), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(s->notebook_text));
	
	

	s->workbook_upper = gtk_notebook_new();
	gtk_widget_ref(s->workbook_upper);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->workbook_upper",
				 s->workbook_upper,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->workbook_upper);
	
	
	e_paned_pack2(E_PANED(hpaned1), s->workbook_upper, TRUE, TRUE);

	gtk_container_set_border_width(GTK_CONTAINER(s->workbook_upper), 2);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(s->workbook_upper), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(s->workbook_upper));

	vbox22 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox22);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox22",
				 vbox22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox22);
	gtk_container_add(GTK_CONTAINER(s->workbook_upper), vbox22);

	s->notebook_comm = gtk_notebook_new();
	gtk_widget_ref(s->notebook_comm);
	gtk_widget_show(s->notebook_comm);
	gtk_box_pack_start(GTK_BOX(vbox22), s->notebook_comm, TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(s->notebook_comm), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(s->notebook_comm));
	/**********************************************************************************/
	
	label64 = gtk_label_new(_("Commentaries"));
	gtk_widget_ref(label64);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label64",
				 label64,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label64);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->workbook_upper),
				   gtk_notebook_get_nth_page(
				   GTK_NOTEBOOK(s->workbook_upper),0), 
				   label64);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->workbook_upper),
                                    gtk_notebook_get_nth_page(
				    GTK_NOTEBOOK(s->workbook_upper),0), 
				    _("Commentaries"));

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(s->workbook_upper), vbox2);

	s->notebook_percomm = gtk_notebook_new();
	gtk_widget_ref(s->notebook_percomm);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->notebook_percomm",
				 s->notebook_percomm,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->notebook_percomm);
	gtk_box_pack_start(GTK_BOX(vbox2), s->notebook_percomm, TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(s->notebook_percomm), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(s->notebook_percomm));
	GTK_WIDGET_UNSET_FLAGS(s->notebook_percomm, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebook_percomm), TRUE);
	
	/*
	 * personal comments editor goes here
	 */
	
	label85 = gtk_label_new(_("Personal Comments"));
	gtk_widget_ref(label85);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label85",
				 label85,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label85);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->workbook_upper),
				gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(s->workbook_upper),
							1), label85);	
							     
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->workbook_upper),
                                gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(s->workbook_upper),
							1), _("Personal Comments"));
	
		
	/*
	 * studypad editor 
	 */
	s->html_studypad = studypad_control(s->workbook_upper, s);
	
				
	label41 = gtk_label_new(_("Study Pad"));
	gtk_widget_ref(label41);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label41",
				 label41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label41);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->workbook_upper),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (s->workbook_upper),
							     2), label41);
	
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->workbook_upper),
                                gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(s->workbook_upper),
							2), _("Study Pad"));		
							
	/*
	 * end studypad editor 
	 */									     
	hbox8 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox8);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hbox8", hbox8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox8);


	e_paned_pack2(E_PANED(vpaned1), hbox8, TRUE, TRUE);

	s->workbook_lower = gtk_notebook_new ();
	gtk_widget_ref (s->workbook_lower);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "s->workbook_lower", s->workbook_lower,
				(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (s->workbook_lower);
	gtk_box_pack_start (GTK_BOX (hbox8), s->workbook_lower, TRUE, TRUE, 0);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(s->workbook_lower));
	
	/*
	 * dict/lex  
	 */
	 /*
	frame10 = gtk_frame_new(NULL);
	gtk_widget_ref(frame10);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame10",
				 frame10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame10);
	gtk_container_add(GTK_CONTAINER(s->workbook_lower), frame10);
	*/
	
	
	s->notebookDL = gtk_notebook_new();
	gtk_widget_ref(s->notebookDL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->notebookDL",
				 s->notebookDL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->notebookDL);
	gtk_container_add(GTK_CONTAINER(s->workbook_lower), s->notebookDL);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(s->notebookDL), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(s->notebookDL));	
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookDL),
					   s->dict_tabs);
				
	label41 = gtk_label_new(_("Dict/Lex"));
	gtk_widget_ref(label41);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label41",
				 label41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label41);
	
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->workbook_lower),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (s->workbook_lower),
							     0), label41);				   
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->workbook_lower),
                                   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							    (s->workbook_lower),
							    0), _("Dict/Lex"));
 	
	/*
	 * end  dict/lex  
	 */
	
	/*
	 * gbs notebook 
	 */
	 /*
	frame10 = gtk_frame_new(NULL);
	gtk_widget_ref(frame10);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame10",
				 frame10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame10);
	gtk_container_add(GTK_CONTAINER(s->workbook_lower), frame10);
	*/
	
	s->notebook_gbs = gtk_notebook_new ();
	gtk_widget_ref (s->notebook_gbs);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "s->notebookGBS", s->notebook_gbs,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (s->notebook_gbs);
	gtk_container_add (GTK_CONTAINER (s->workbook_lower), s->notebook_gbs);
	GTK_WIDGET_UNSET_FLAGS (s->notebook_gbs, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebook_gbs), FALSE);	
	gtk_notebook_popup_enable(GTK_NOTEBOOK(s->notebook_gbs));	
	
	label185 = gtk_label_new(_("Books"));
	gtk_widget_ref(label185);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label185",
				 label185,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label185);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->workbook_lower),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (s->workbook_lower),
							     1), label185);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->workbook_lower),
                                  gtk_notebook_get_nth_page(GTK_NOTEBOOK
							    (s->workbook_lower),
							    1), _("Books"));
	/*
	 * end gbs 
	 */
	
	
	
	/*
	 * interlinear page 
	 */
	 s->frameInt = gtk_frame_new (NULL);
	  gtk_widget_ref (s->frameInt);
	  gtk_object_set_data_full (GTK_OBJECT (s->app), "s->frameInt", s->frameInt,
				    (GtkDestroyNotify) gtk_widget_unref);
	  gtk_widget_show (s->frameInt);
	  gtk_container_add (GTK_CONTAINER (s->workbook_lower), s->frameInt);
		
	swInt = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swInt);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "swInt", swInt,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swInt);
	gtk_container_add(GTK_CONTAINER(s->frameInt), swInt);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (swInt),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);
				       
	s->dockedInt = TRUE;
	
	s->htmlInterlinear = gtk_html_new();
	gtk_widget_ref(s->htmlInterlinear);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->htmlInterlinear",
				 s->htmlInterlinear,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->htmlInterlinear);	
	gtk_html_load_empty(GTK_HTML(s->htmlInterlinear));

	gtk_container_add(GTK_CONTAINER
			  (swInt),
			  s->htmlInterlinear);
		
		
	label197 = gtk_label_new (_("Interlinear"));
	gtk_widget_ref (label197);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "label197", label197,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label197);
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (s->workbook_lower), 
	gtk_notebook_get_nth_page (GTK_NOTEBOOK (s->workbook_lower), 2), label197);

	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->workbook_lower),
                                    gtk_notebook_get_nth_page(GTK_NOTEBOOK
							    (s->workbook_lower),
							    2), _("Interlinear"));
	
	/*
	 * end Interlinear page 
	 */
	 
	s->appbar = gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gtk_widget_ref(s->appbar);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->appbar",
				 s->appbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->appbar);
	gnome_app_set_statusbar(GNOME_APP(s->app), s->appbar);

		
	gui_install_menu_hints(s->app);
        /*
	 * create pixmaps for ctrees
	 */
		
	gtk_widget_realize(s->app); /** added to stop annoying gdk warning on startup **/
	pixmap1 =                   /** when these pixmaps are created **/
	    gdk_pixmap_create_from_xpm_d(s->app->window, &mask1,
					 &transparent, book_closed_xpm);
	pixmap2 =
	    gdk_pixmap_create_from_xpm_d(s->app->window, &mask2,
					 &transparent, book_open_xpm);
	pixmap3 =
	    gdk_pixmap_create_from_xpm_d(s->app->window, &mask3,
					 &transparent, mini_page_xpm);



	gtk_signal_connect(GTK_OBJECT(s->app), "destroy",
			   GTK_SIGNAL_FUNC(on_mainwindow_destroy), 
			   (gpointer) s);
	gtk_signal_connect(GTK_OBJECT(s->app), "size_allocate",
			   GTK_SIGNAL_FUNC(on_mainwindow_size_allocate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnSBDock), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSBDock_clicked),
			   s);
	gtk_signal_connect(GTK_OBJECT(btnSB), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSB_clicked), s);
	gtk_signal_connect (GTK_OBJECT (btn_search), "clicked",
                      GTK_SIGNAL_FUNC (on_btn_search_clicked),
                      NULL);
	gtk_signal_connect(GTK_OBJECT(s->workbook_upper), "switch_page",
			   GTK_SIGNAL_FUNC(workbook_upper_switch_page),
			   NULL);	   
			   
	gtk_signal_connect(GTK_OBJECT(s->epaned), "button_release_event",
			   GTK_SIGNAL_FUNC(epaned_button_release_event),
			   (gchar *) "epaned");
	gtk_signal_connect(GTK_OBJECT(vpaned1), "button_release_event",
			   GTK_SIGNAL_FUNC(epaned_button_release_event),
			   (gchar *) "vpaned1");
	gtk_signal_connect(GTK_OBJECT(hpaned1), "button_release_event",
			   GTK_SIGNAL_FUNC(epaned_button_release_event),
			   (gchar *) "hpaned1");

	gtk_signal_connect(GTK_OBJECT(s->htmlInterlinear), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) s->app);
	gtk_signal_connect(GTK_OBJECT(s->htmlInterlinear), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(s->htmlInterlinear), "button_release_event",
			   GTK_SIGNAL_FUNC(button_release_event), 
			   NULL);
	
	gtk_widget_grab_focus(s->app);

	gtk_widget_set_usize(s->app, s->gs_width, s->gs_hight);
}
