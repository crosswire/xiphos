/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_gui.c
    * -------------------
    * Wed June 27 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
  */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/e-paned/e-vpaned.h>

#include "gs_gui.h"
#include "gs_gui_cb.h"
#include "gs_editor.h"
#include "gs_gnomesword.h"
#include "support.h"
#include "gs_html.h"
#include "gs_shortcutbar.h"
#include "sword.h"


/*
 * gnome
 */
#include "main_menu.h"



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



static void on_btnSBDock_clicked(GtkButton * button, SETTINGS * s)
{
	dock_undock(s);
}


void create_mainwindow(SETTINGS *s)
{
	GtkWidget *dock1;
	GtkWidget *vbox_gs;
	GtkWidget *handleboxOptionsBar;
	GtkWidget *toolbarOptions;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSBDock;
	GtkWidget *btnSB;
	GtkWidget *cbBook;
	GList *cbBook_items = NULL;
	GtkObject *spbChapter_adj;
	GtkObject *spbVerse_adj;
	GtkWidget *btnLookup;
	GtkWidget *btnBack;
	GtkWidget *btnFoward;
	GtkWidget *vseparator13;
	GtkWidget *mainPanel;
	GtkWidget *vboxMain;
	GtkWidget *vpaned1;
	GtkWidget *hbox2;
	GtkWidget *handleboxNavBar;
	GtkWidget *toolbarNav;
	GtkWidget *hpaned1;
	GtkWidget *vbox_text;
	GtkWidget *notebook3;
	GtkWidget *vbox22;
	GtkWidget *label64;
	GtkWidget *vbox2;
	GtkWidget *label85;
	GtkWidget *swInt;
	GtkWidget *label41;
	GtkWidget *hbox8;
	GtkWidget *frame10;
	GtkWidget *label185;
	GtkWidget *label197;
	GtkWidget *hbox25;
	GtkWidget *appbar1;
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
	 * start nav toolbar 
	 */
	handleboxNavBar = gtk_handle_box_new();
	gtk_widget_ref(handleboxNavBar);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "handleboxNavBar", handleboxNavBar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handleboxNavBar);
	gtk_box_pack_start(GTK_BOX(vboxMain), handleboxNavBar, FALSE,
			   FALSE, 0);

	toolbarNav =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarNav);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbarNav",
				 toolbarNav,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarNav);
	gtk_container_add(GTK_CONTAINER(handleboxNavBar), toolbarNav);
	gtk_widget_set_usize(toolbarNav, -2, 34);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarNav),
				      GTK_RELIEF_NONE);

	cbBook = gtk_combo_new();
	gtk_widget_ref(cbBook);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "cbBook",
				 cbBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbBook);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), cbBook, NULL,
				  NULL);

	/*
	 * get and load books of the Bible 
	 */
	cbBook_items = backend_get_books();
	gtk_combo_set_popdown_strings(GTK_COMBO(cbBook), cbBook_items);
	g_list_free(cbBook_items);

	s->cbeBook = GTK_COMBO(cbBook)->entry;
	gtk_widget_ref(s->cbeBook);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->cbeBook",
				 s->cbeBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->cbeBook);
	gtk_entry_set_text(GTK_ENTRY(s->cbeBook), _("Romans"));

	spbChapter_adj = gtk_adjustment_new(8, -1, 151, 1, 10, 10);
	s->spbChapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbChapter_adj), 1, 0);
	gtk_widget_ref(s->spbChapter);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->spbChapter",
				 s->spbChapter,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->spbChapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), s->spbChapter,
				  NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(s->spbChapter), TRUE);

	spbVerse_adj = gtk_adjustment_new(28, -1, 180, 1, 10, 10);
	s->spbVerse = gtk_spin_button_new(GTK_ADJUSTMENT(spbVerse_adj), 1, 0);
	gtk_widget_ref(s->spbVerse);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->spbVerse",
				 s->spbVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->spbVerse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), s->spbVerse, NULL,
				  NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(s->spbVerse), TRUE);

	s->cbeFreeformLookup = gtk_entry_new();
	gtk_widget_ref(s->cbeFreeformLookup);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "s->cbeFreeformLookup", s->cbeFreeformLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->cbeFreeformLookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav),
				  s->cbeFreeformLookup, NULL, NULL);
	gtk_entry_set_text(GTK_ENTRY(s->cbeFreeformLookup), _("Romans 8:28"));

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	btnLookup =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Goto verse"),
				       _
				       ("Go to verse in free form lookup and add verse to history"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLookup);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnLookup",
				 btnLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLookup);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_BACK);
	btnBack =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Back"),
				       _
				       ("Go backward through history list"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnBack);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnBack",
				 btnBack,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnBack);
	gtk_widget_set_sensitive(btnBack, FALSE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_FORWARD);
	btnFoward =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Foward"),
				       _("Go foward through history list"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnFoward);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnFoward",
				 btnFoward,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnFoward);
	gtk_widget_set_sensitive(btnFoward, FALSE);
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
	
	

	notebook3 = gtk_notebook_new();
	gtk_widget_ref(notebook3);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "notebook3",
				 notebook3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook3);
	s->workbook = notebook3;
	
	e_paned_pack2(E_PANED(hpaned1), notebook3, TRUE, TRUE);

	gtk_container_set_border_width(GTK_CONTAINER(notebook3), 2);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook3), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook3));

	vbox22 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox22);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox22",
				 vbox22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox22);
	gtk_container_add(GTK_CONTAINER(notebook3), vbox22);

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
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook3),0), label64);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(notebook3),
                                    gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook3),0), _("Commentaries"));

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(notebook3), vbox2);

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
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(notebook3),
							1), label85);	
							     
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(notebook3),
                                gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(notebook3),
							1), _("Personal Comments"));
	
		
	/*
	 * studypad editor 
	 */
	s->htmlSP = studypad_control(notebook3, s);
	
				
	label41 = gtk_label_new(_("Study Pad"));
	gtk_widget_ref(label41);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label41",
				 label41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label41);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     2), label41);
	
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(notebook3),
                                gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(notebook3),
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
	frame10 = gtk_frame_new(NULL);
	gtk_widget_ref(frame10);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame10",
				 frame10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame10);
	gtk_container_add(GTK_CONTAINER(s->workbook_lower), frame10);
	gtk_container_set_border_width(GTK_CONTAINER(frame10), 2);
	
	s->notebookDL = gtk_notebook_new();
	gtk_widget_ref(s->notebookDL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->notebookDL",
				 s->notebookDL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->notebookDL);
	gtk_container_add(GTK_CONTAINER(frame10), s->notebookDL);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(s->notebookDL), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(s->notebookDL));	
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookDL),
					   s->dict_tabs);
					   
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->workbook_lower),
                                   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							    (s->workbook_lower),
							    0), _("Dict/Lex"));
 	
	/*
	 * end  dict/lex  
	 */
	
	/*
	 * GBS html display 
	 */
	
	s->notebookGBS = gtk_notebook_new ();
	gtk_widget_ref (s->notebookGBS);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "s->notebookGBS", s->notebookGBS,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (s->notebookGBS);
	gtk_container_add (GTK_CONTAINER (s->workbook_lower), s->notebookGBS);
	GTK_WIDGET_UNSET_FLAGS (s->notebookGBS, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookGBS), FALSE);	
	gtk_notebook_popup_enable(GTK_NOTEBOOK(s->notebookGBS));	
	
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
	 * end GBS html editor/display 
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
	 
	appbar1 = gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gtk_widget_ref(appbar1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "appbar1",
				 appbar1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(appbar1);
	gnome_app_set_statusbar(GNOME_APP(s->app), appbar1);

	s->appbar = lookup_widget(s->app, "appbar1");
	
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
	gtk_signal_connect(GTK_OBJECT(s->cbeBook), "changed",
			   GTK_SIGNAL_FUNC(on_cbeBook_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(s->spbChapter), "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_spbChapter_button_release_event), NULL);
	gtk_signal_connect(GTK_OBJECT(s->spbVerse), "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_spbVerse_button_release_event), NULL);
	gtk_signal_connect(GTK_OBJECT(s->cbeFreeformLookup),
			   "key_press_event",
			   GTK_SIGNAL_FUNC
			   (on_cbeFreeformLookup_key_press_event), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLookup), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLookup_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnBack), "clicked",
			   GTK_SIGNAL_FUNC(on_btnBack_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnFoward), "clicked",
			   GTK_SIGNAL_FUNC(on_btnFoward_clicked), NULL);
		   
	gtk_signal_connect(GTK_OBJECT(notebook3), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook3_switch_page),
			   NULL);	   
			   
	gtk_signal_connect(GTK_OBJECT(s->epaned), "button_release_event",
			   GTK_SIGNAL_FUNC(on_epaned_button_release_event),
			   (gchar *) "epaned");
	gtk_signal_connect(GTK_OBJECT(vpaned1), "button_release_event",
			   GTK_SIGNAL_FUNC(on_epaned_button_release_event),
			   (gchar *) "vpaned1");
	gtk_signal_connect(GTK_OBJECT(hpaned1), "button_release_event",
			   GTK_SIGNAL_FUNC(on_epaned_button_release_event),
			   (gchar *) "hpaned1");

	gtk_signal_connect(GTK_OBJECT(s->htmlInterlinear), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) s->app);
	gtk_signal_connect(GTK_OBJECT(s->htmlInterlinear), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	
	gtk_widget_grab_focus(s->app);

	gtk_widget_set_usize(s->app, s->gs_width, settings->gs_hight);
}
