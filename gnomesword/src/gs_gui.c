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
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/e-paned/e-vpaned.h>
#include <gal/e-paned/e-vpaned.h>
#include <gtkhtml/gtkhtml.h>

#include "gs_gui.h"
#include "gs_html.h"
#include "gs_mainmenu_cb.h"
#include "gs_gui_cb.h"
#include "gs_editor.h"
#include "gs_gnomesword.h"
#include "support.h"
#include "gs_shortcutbar.h"
#include "sw_sword.h"
#include "e-splash.h"
#include "gs_html.h"


GtkWidget *appbar1;
GtkWidget *main_label;


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

extern GtkWidget *shortcut_bar;
extern EShortcutModel *shortcut_model;
//extern SETTINGS *settings;


static GnomeUIInfo file1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EXIT_ITEM(on_exit1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo edit1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Copy"),
	 N_("Copy highlighted text form main window"),
	 on_copyhtml_activate, "htmlTexts", NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Search"),
	 N_("Open search dialog"),
	 on_search1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo quickmarks_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Add Quickmark"),
	 N_("Add current verse to Quickmarks menu"),
	 on_add_quickmark_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Clear Quickmarks"),
	 N_("Edit Quickmarks"),
	 on_clear_quickmarks_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CUT,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo history1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("C_lear"),
	 N_("Clear history list"),
	 on_clear1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo view1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Daily Devotion"),
	 N_("Show the Daily Devotion for Today"),
	 (gpointer) on_daily_devotion1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_OPEN,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo module_options_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo settings1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Module Options"),
	 NULL,
	 module_options_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CONVERT,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_MENU_PREFERENCES_ITEM(on_preferences1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo bible_texts1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo commentaries1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo dictionaries_lexicons1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo books_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo about_sword_modules1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Bible Texts"),
	 N_("Information about Bible texts"),
	 bible_texts1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Commentaries"),
	 NULL,
	 commentaries1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Dictionaries-Lexicons"),
	 NULL,
	 dictionaries_lexicons1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Books"),
	 NULL,
	 books_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo help1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Contents"),
	 N_("Help Contents"),
	 on_help_contents_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_HELP,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("About the Sword Project..."),
	 N_("Infotmation About The Sword Project"),
	 on_about_the_sword_project1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("About Sword Modules"),
	 N_("Information about the installed modules"),
	 about_sword_modules1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("About GnomeSword..."),
	 N_("About GnomeSword"),
	 on_about_gnomesword1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] = {
	GNOMEUIINFO_MENU_FILE_TREE(file1_menu_uiinfo),
	GNOMEUIINFO_MENU_EDIT_TREE(edit1_menu_uiinfo),
	{
	 GNOME_APP_UI_SUBTREE, N_("_Quickmarks"),
	 NULL,
	 quickmarks_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("_History"),
	 NULL,
	 history1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_MENU_VIEW_TREE(view1_menu_uiinfo),
	GNOMEUIINFO_MENU_SETTINGS_TREE(settings1_menu_uiinfo),
	GNOMEUIINFO_MENU_HELP_TREE(help1_menu_uiinfo),
	GNOMEUIINFO_END
};

GtkWidget *create_mainwindow(GtkWidget * splash, SETTINGS *s)
{
	GtkWidget *dock1;
	GtkWidget *vbox_gs;
	GtkWidget *hboxToolbar;
	GtkWidget *handleboxOptionsBar;
	GtkWidget *toolbarOptions;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSBDock;
	GtkWidget *btnStrongs;
	GtkWidget *btnMorphs;
	GtkWidget *btnFootnotes;
	GtkWidget *btnSB;
	GtkWidget *cbBook;
	GList *cbBook_items = NULL;
	GtkWidget *cbeBook;
	GtkObject *spbChapter_adj;
	GtkWidget *spbChapter;
	GtkObject *spbVerse_adj;
	GtkWidget *spbVerse;
	GtkWidget *cbeFreeformLookup;
	GtkWidget *btnLookup;
	GtkWidget *btnBack;
	GtkWidget *btnFoward;
	GtkWidget *vseparator13;
	GtkWidget *mainPanel;
	GtkWidget *vboxMain;
	GtkWidget *epaned;
	GtkWidget *vpaned1;
	GtkWidget *hbox2;
	GtkWidget *handleboxNavBar;
	GtkWidget *toolbarNav;
	GtkWidget *hpaned1;
	GtkWidget *vbox32;
	GtkWidget *nbTextMods;
	GtkWidget *frame9;
	GtkWidget *frameBible;
	GtkWidget *swHtmlBible;
	GtkWidget *notebook3;
	GtkWidget *vbox22;
	GtkWidget *notebook1;
	GtkWidget *vbox43;
	GtkWidget *btnCommSyn;
	GtkWidget *vseparator14;
	GtkWidget *btnCommPrint;
	GtkWidget *hbox18;
	GtkWidget *frameCom;
	GtkWidget *frameCom2;
	GtkWidget *swHtmlCom;
	GtkWidget *handlebox17;
	GtkWidget *toolbar26;
	GtkWidget *btnComPrev;
	GtkWidget *btnComNext;
	GtkWidget *label64;
	GtkWidget *vbox2;
	GtkWidget *hbox11;
	GtkWidget *vbox8;
	GtkWidget *nbPerCom;
	GtkWidget *vboxPC;
	GtkWidget *scrolledwindow11;
	GtkWidget *textComments;
	GtkWidget *sbNotes;
	GtkWidget *label85;
	GtkWidget *swInt;
	GtkWidget *framedict;
	GtkWidget *label41;
	GtkWidget *label;
	GtkWidget *hbox8;
	GtkWidget *frame10;
	GtkWidget *hbox13;
	GtkWidget *vbox1;
	GtkWidget *notebook4;
	GtkWidget *scrolledwindow8;
	GtkWidget *vbox14;
	GtkWidget *hbox19;
	GtkWidget *toolbar25;
	GtkWidget *btnKeyPrev;
	GtkWidget *dictionarySearchText;
	GtkWidget *btnKeyNext;
	GtkWidget *list1;
	GtkWidget *label58;
	GtkWidget *label185;
	GtkWidget *label197;
	GtkWidget *labelDict;
	GtkWidget *hbox25;
	gchar *pathname;
	GdkPixbuf *icon_pixbuf;
	GdkColor transparent = { 0 };
	
	g_print("%s\n", "Building GnomeSword interface");
	if (s->showsplash) {
		pathname = gnome_pixmap_file("gnomesword/GnomeSword.xpm");
		icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);
		pathname = gnome_pixmap_file("gnomesword/sword.xpm");
		icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);
		pathname =
		    gnome_pixmap_file("gnomesword/gnome-fontsel.xpm");
		icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);
		pathname = gnome_pixmap_file("gnomesword/GnomeSword.xpm");
		icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);
		while (gtk_events_pending())
			gtk_main_iteration();
		e_splash_set_icon_highlight(E_SPLASH(splash), 1, TRUE);
	}

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

	gnome_app_create_menus(GNOME_APP(s->app), menubar1_uiinfo);

	gtk_widget_ref(menubar1_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "file1",
				 menubar1_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(file1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "separator4",
				 file1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(file1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "exit1",
				 file1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "edit1",
				 menubar1_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "copy1",
				 edit1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "separator3",
				 edit1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "search1",
				 edit1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "quickmarks",
				 menubar1_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(quickmarks_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "add_bookmark1",
				 quickmarks_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(quickmarks_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "edit_quickmarks",
				 quickmarks_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(quickmarks_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "separator4",
				 quickmarks_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "history1",
				 menubar1_uiinfo[3].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(history1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "clear1",
				 history1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(history1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "separator5",
				 history1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "view1",
				 menubar1_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(view1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "daily_devotion1",
				 view1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "settings1",
				 menubar1_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(settings1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "preferences1",
				 settings1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[5].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "help1",
				 menubar1_uiinfo[5].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_right_justify(GTK_MENU_ITEM
				    (menubar1_uiinfo[5].widget));

	gtk_widget_ref(help1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "about_the_sword_project1",
				 help1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(help1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "about_gnomesword1",
				 help1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(help1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "about_sword_modules1",
				 help1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "bible_texts1",
				 about_sword_modules1_menu_uiinfo
				 [0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(bible_texts1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "separator15",
				 bible_texts1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "commentaries1",
				 about_sword_modules1_menu_uiinfo[1].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(commentaries1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "separator16",
				 commentaries1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "dictionaries_lexicons1",
				 about_sword_modules1_menu_uiinfo
				 [2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(dictionaries_lexicons1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "separator17",
				 dictionaries_lexicons1_menu_uiinfo
				 [0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "books",
				 about_sword_modules1_menu_uiinfo[3].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(books_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "separator17",
				 books_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

//-------------------------------------------------------------
	vbox_gs = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_gs);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox_gs",
				 vbox_gs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_gs);
	gnome_app_set_contents(GNOME_APP(s->app), vbox_gs);


	hboxToolbar = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hboxToolbar);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hboxToolbar",
				 hboxToolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hboxToolbar);
	gtk_box_pack_start(GTK_BOX(vbox_gs), hboxToolbar, FALSE, TRUE, 0);


 /*************************************************************************************** begin options toolbar */

	handleboxOptionsBar = gtk_handle_box_new();
	gtk_widget_ref(handleboxOptionsBar);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "handleboxOptionsBar",
				 handleboxOptionsBar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handleboxOptionsBar);
	gtk_box_pack_start(GTK_BOX(hboxToolbar), handleboxOptionsBar, TRUE,
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
	    create_pixmap(s->app, "gnomesword/dock.xpm", TRUE);
	/*tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				     GNOME_STOCK_PIXMAP_INDEX );*/
	btnSBDock =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarOptions),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Detach",
				       "Detach/Attach Shortcut Bar",
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
				       "Hide/Show Shortcut Bar", NULL,
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
	    create_pixmap(s->app, "gnomesword/strongs.xpm", TRUE);
	btnStrongs =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarOptions),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, "Strongs",
				       "Toogle Strongs Numbers", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnStrongs);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnStrongs",
				 btnStrongs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnStrongs);

	tmp_toolbar_icon =
	    create_pixmap(s->app, "gnomesword/morphs.xpm", TRUE);
	btnMorphs =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarOptions),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, "Morphological",
				       "Toogle Morphological Tags", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnMorphs);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnMorphs",
				 btnMorphs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnMorphs);

	tmp_toolbar_icon =
	    create_pixmap(s->app, "gnomesword/footnote3.xpm", TRUE);
	btnFootnotes =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarOptions),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, "Footnotes",
				       "Toogle Footnotes", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnFootnotes);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnFootnotes",
				 btnFootnotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnFootnotes);


	hbox25 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox25);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hbox25",
				 hbox25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox25);
	gtk_box_pack_start(GTK_BOX(vbox_gs), hbox25, TRUE, TRUE, 0);

	epaned = e_hpaned_new();
	gtk_widget_ref(epaned);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "epaned",
				 epaned,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(epaned);
	gtk_box_pack_start(GTK_BOX(hbox25), epaned, TRUE, TRUE, 0);
	/****************************************************************************** shortcut bar **/
	shortcut_model = e_shortcut_model_new();

	shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(shortcut_bar),
				 shortcut_model);
	if(s->showshortcutbar)
		gtk_widget_show(shortcut_bar);
	else
		gtk_widget_hide(shortcut_bar);

	s->shortcut_bar = shortcut_bar;

	e_paned_pack1(E_PANED(epaned), shortcut_bar, FALSE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(shortcut_bar), 4);
	/************************************************************************ end shortcut bar **/

	mainPanel = gtk_vbox_new(FALSE, 0);
	e_paned_pack2(E_PANED(epaned), mainPanel, TRUE, TRUE);
	gtk_widget_show(mainPanel);

	vboxMain = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxMain);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "vboxMain", vboxMain,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vboxMain);
	gtk_box_pack_start(GTK_BOX(mainPanel), vboxMain, TRUE, TRUE, 0);

	/************************************************************************************ start nav toolbar */
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

	/*** get and load books of the Bible ***/
	cbBook_items = getBibleBooks();
	gtk_combo_set_popdown_strings(GTK_COMBO(cbBook), cbBook_items);
	g_list_free(cbBook_items);

	cbeBook = GTK_COMBO(cbBook)->entry;
	gtk_widget_ref(cbeBook);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "cbeBook",
				 cbeBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbeBook);
	gtk_entry_set_text(GTK_ENTRY(cbeBook), _("Romans"));

	spbChapter_adj = gtk_adjustment_new(8, -1, 151, 1, 10, 10);
	spbChapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbChapter_adj), 1, 0);
	gtk_widget_ref(spbChapter);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "spbChapter",
				 spbChapter,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spbChapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), spbChapter,
				  NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spbChapter), TRUE);

	spbVerse_adj = gtk_adjustment_new(28, -1, 180, 1, 10, 10);
	spbVerse = gtk_spin_button_new(GTK_ADJUSTMENT(spbVerse_adj), 1, 0);
	gtk_widget_ref(spbVerse);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "spbVerse",
				 spbVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spbVerse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), spbVerse, NULL,
				  NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spbVerse), TRUE);

	cbeFreeformLookup = gtk_entry_new();
	gtk_widget_ref(cbeFreeformLookup);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "cbeFreeformLookup", cbeFreeformLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbeFreeformLookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav),
				  cbeFreeformLookup, NULL, NULL);
	gtk_entry_set_text(GTK_ENTRY(cbeFreeformLookup), _("Romans 8:28"));

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
	/************************************************************************************ end nav toolbar */


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


	vbox32 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox32);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox32",
				 vbox32,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox32);
	e_paned_pack1(E_PANED(hpaned1), vbox32, FALSE, TRUE);

	nbTextMods = gtk_notebook_new();
	gtk_widget_ref(nbTextMods);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "nbTextMods",
				 nbTextMods,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(nbTextMods);
	gtk_box_pack_start(GTK_BOX(vbox32), nbTextMods, FALSE, TRUE, 0);
	gtk_widget_set_usize(nbTextMods, -2, 31);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(nbTextMods), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(nbTextMods));

	frame9 = gtk_frame_new(NULL);
	gtk_widget_ref(frame9);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame9",
				 frame9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame9);
	gtk_box_pack_start(GTK_BOX(vbox32), frame9, TRUE, TRUE, 0);

	frameBible = gtk_frame_new(NULL);
	gtk_widget_ref(frameBible);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frameBible",
				 frameBible,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameBible);
	gtk_container_add(GTK_CONTAINER(frame9), frameBible);


	gtk_container_set_border_width(GTK_CONTAINER(frame9), 2);

	swHtmlBible = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swHtmlBible);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "swHtmlBible",
				 swHtmlBible,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swHtmlBible);
	gtk_container_add(GTK_CONTAINER(frameBible), swHtmlBible);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swHtmlBible),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	/***  htmlTexts goes here  ***/

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

	notebook1 = gtk_notebook_new();
	gtk_widget_ref(notebook1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "notebook1",
				 notebook1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook1);
	gtk_box_pack_start(GTK_BOX(vbox22), notebook1, FALSE, TRUE, 0);
	gtk_widget_set_usize(notebook1, -2, 31);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook1), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook1));
	/**********************************************************************************/

	/**********************************************************************************/
	vbox43 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox43);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox43",
				 vbox43,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox43);
	gtk_box_pack_start(GTK_BOX(vbox22), vbox43, TRUE, TRUE, 0);

	handlebox17 = gtk_handle_box_new();
	gtk_widget_ref(handlebox17);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "handlebox17",
				 handlebox17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(vbox43), handlebox17, FALSE, TRUE, 0);

	toolbar26 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar26);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbar26",
				 toolbar26,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar26);
	gtk_container_add(GTK_CONTAINER(handlebox17), toolbar26);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_REFRESH);
	btnCommSyn =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar26),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Synchronize"),
				       _
				       ("Synchronize Commentary with Bible Text"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnCommSyn);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnCommSyn",
				 btnCommSyn,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnCommSyn);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_BACK);
	btnComPrev =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar26),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button2"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnComPrev);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnComPrev",
				 btnComPrev,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnComPrev);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_FORWARD);
	btnComNext =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar26),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button3"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnComNext);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnComNext",
				 btnComNext,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnComNext);

	vseparator14 = gtk_vseparator_new();
	gtk_widget_ref(vseparator14);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vseparator14",
				 vseparator14,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator14);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar26), vseparator14,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator14, 6, 15);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_PRINT);
	btnCommPrint =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar26),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"), _("Print this Comment"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnCommPrint);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnCommPrint",
				 btnCommPrint,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnCommPrint);

	hbox18 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox18);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hbox18",
				 hbox18,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox18);
	gtk_box_pack_start(GTK_BOX(vbox43), hbox18, TRUE, TRUE, 0);

	frameCom = gtk_frame_new(NULL);
	gtk_widget_ref(frameCom);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frameCom",
				 frameCom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameCom);
	gtk_box_pack_start(GTK_BOX(hbox18), frameCom, TRUE, TRUE, 0);

	frameCom2 = gtk_frame_new(NULL);
	gtk_widget_ref(frameCom2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frameCom2",
				 frameCom2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameCom2);
	gtk_container_add(GTK_CONTAINER(frameCom), frameCom2);

	swHtmlCom = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swHtmlCom);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "swHtmlCom",
				 swHtmlCom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swHtmlCom);
	gtk_container_add(GTK_CONTAINER(frameCom2), swHtmlCom);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swHtmlCom),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	label64 = gtk_label_new(_("Commentaries"));
	gtk_widget_ref(label64);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label64",
				 label64,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label64);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook3),0), label64);
	
	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_container_add(GTK_CONTAINER(notebook3), vbox2);

	hbox11 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox11);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hbox11",
				 hbox11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox11);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox11, TRUE, TRUE, 0);

	vbox8 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox8);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox8", vbox8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox8);
	gtk_box_pack_start(GTK_BOX(hbox11), vbox8, TRUE, TRUE, 0);


	nbPerCom = gtk_notebook_new();
	gtk_widget_ref(nbPerCom);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "nbPerCom",
				 nbPerCom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(nbPerCom);
	gtk_box_pack_start(GTK_BOX(vbox8), nbPerCom, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(nbPerCom, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nbPerCom), FALSE);

	vboxPC = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxPC);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vboxPC",
				 vboxPC,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vboxPC);
	gtk_container_add(GTK_CONTAINER(nbPerCom), vboxPC);

	/************************************************************************** percom html editor/display */
	percom_control(vboxPC, s);	
	gtk_box_pack_start(GTK_BOX(hboxToolbar), s->toolbarComments, TRUE,
			   TRUE, 0);
/*
	gnome_app_add_toolbar (GNOME_APP (s->app), 
				GTK_TOOLBAR (s->toolbarComments),
			        GS_DOCK_PC,
			        GNOME_DOCK_ITEM_BEH_NORMAL,
			        GNOME_DOCK_TOP, 1, 2, 0);
*/	
	label = gtk_label_new("label");
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbPerCom),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbPerCom),
							     0), label);
	/***************************************************************** end percom html editor/display */
	scrolledwindow11 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow11);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "scrolledwindow11", scrolledwindow11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow11);
	gtk_container_add(GTK_CONTAINER(nbPerCom), scrolledwindow11);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow11),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	textComments = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textComments);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "textComments",
				 textComments,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComments);
	gtk_container_add(GTK_CONTAINER(scrolledwindow11), textComments);

	label = gtk_label_new("label");
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbPerCom),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbPerCom),
							     1), label);


	sbNotes = gtk_statusbar_new();
	gtk_widget_ref(sbNotes);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "sbNotes",
				 sbNotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(vbox8), sbNotes, FALSE, FALSE, 0);


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
	/********************************************************************** studypad editor */
	s->htmlSP = studypad_control(notebook3, s);
	gtk_box_pack_start(GTK_BOX(hboxToolbar), s->toolbarStudypad, TRUE,
			   TRUE, 0);
/*	
	gnome_app_add_toolbar (GNOME_APP (s->app), 
				GTK_TOOLBAR (s->toolbarStudypad),
			        GS_DOCK_SP,
			        GNOME_DOCK_ITEM_BEH_NORMAL,
			        GNOME_DOCK_TOP, 1, 3, 0);
*/				
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
	/************************************************************** end studypad editor */
	
	/******************************************************************** interlinear page */
	/*s->frameInt = gtk_frame_new(NULL);
	gtk_widget_ref(s->frameInt);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "s->frameInt",
				 s->frameInt,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->frameInt);
	gtk_container_add(GTK_CONTAINER(notebook3), s->frameInt); 

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
	
	gtk_container_add(GTK_CONTAINER
			  (swInt),
			  s->htmlInterlinear);
			  
	label12 = gtk_label_new(_("Interlinear"));
	gtk_widget_ref(label12);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label12",
				 label12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label12);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     4), label12);
*/
	/********************************************************************** end Interlinear page */									     
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

	frame10 = gtk_frame_new(NULL);
	gtk_widget_ref(frame10);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame10",
				 frame10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame10);
	//gtk_box_pack_start(GTK_BOX(s->workbook_lower), frame10, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(s->workbook_lower), frame10);
	gtk_container_set_border_width(GTK_CONTAINER(frame10), 2);

	hbox13 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox13);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hbox13",
				 hbox13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox13);
	gtk_container_add(GTK_CONTAINER(frame10), hbox13);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox1", vbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_box_pack_start(GTK_BOX(hbox13), vbox1, TRUE, TRUE, 0);

	notebook4 = gtk_notebook_new();
	gtk_widget_ref(notebook4);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "notebook4",
				 notebook4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook4);
	gtk_box_pack_start(GTK_BOX(vbox1), notebook4, FALSE, TRUE, 0);
	gtk_widget_set_usize(notebook4, -2, 31);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook4), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook4));

	framedict = gtk_frame_new(NULL);
	gtk_widget_ref(framedict);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "framedict",
				 framedict,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(framedict);
	gtk_box_pack_start(GTK_BOX(vbox1), framedict, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(framedict), 2);

	scrolledwindow8 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow8);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "scrolledwindow8", scrolledwindow8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow8);
	gtk_container_add(GTK_CONTAINER(framedict), scrolledwindow8);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow8), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	vbox14 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox14);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox14",
				 vbox14,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox14);
	gtk_box_pack_start(GTK_BOX(hbox13), vbox14, FALSE, TRUE, 0);

	hbox19 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox19);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hbox19",
				 hbox19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox19);
	gtk_box_pack_start(GTK_BOX(vbox14), hbox19, FALSE, FALSE, 0);

	toolbar25 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar25);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbar25",
				 toolbar25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar25);
	gtk_box_pack_start(GTK_BOX(hbox19), toolbar25, TRUE, TRUE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar25),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_BACK);
	btnKeyPrev =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar25),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button2", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnKeyPrev);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnKeyPrev",
				 btnKeyPrev,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnKeyPrev);

	dictionarySearchText = gtk_entry_new();
	gtk_widget_ref(dictionarySearchText);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "dictionarySearchText",
				 dictionarySearchText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dictionarySearchText);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar25),
				  dictionarySearchText, NULL, NULL);
	gtk_widget_set_usize(dictionarySearchText, 145, -2);
	gtk_entry_set_text(GTK_ENTRY(dictionarySearchText), "GRACE");

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_FORWARD);
	btnKeyNext =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar25),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button3", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnKeyNext);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnKeyNext",
				 btnKeyNext,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnKeyNext);

	list1 = gtk_clist_new(1);
	gtk_widget_ref(list1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "list1", list1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(list1);
	gtk_box_pack_start(GTK_BOX(vbox14), list1, TRUE, TRUE, 0);
	gtk_widget_set_usize(list1, 126, -2);
	gtk_clist_set_column_width(GTK_CLIST(list1), 0, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(list1));

	label58 = gtk_label_new("label58");
	gtk_widget_ref(label58);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label58",
				 label58,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label58);
	gtk_clist_set_column_widget(GTK_CLIST(list1), 0, label58);

	  labelDict = gtk_label_new (_("Dict/Lex"));
	  gtk_widget_ref (labelDict);
	  gtk_object_set_data_full (GTK_OBJECT (s->app), "labelDict", labelDict,
				    (GtkDestroyNotify) gtk_widget_unref);
	  gtk_widget_show (labelDict);
	  gtk_notebook_set_tab_label (GTK_NOTEBOOK (s->workbook_lower), 
			gtk_notebook_get_nth_page (GTK_NOTEBOOK (s->workbook_lower), 0), labelDict);
	
	
	/************************************************************************** GBS html editor/display */
	
	s->notebookGBS = gtk_notebook_new ();
	gtk_widget_ref (s->notebookGBS);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "s->notebookGBS", s->notebookGBS,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (s->notebookGBS);
	gtk_container_add (GTK_CONTAINER (s->workbook_lower), s->notebookGBS);
	GTK_WIDGET_UNSET_FLAGS (s->notebookGBS, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (s->notebookGBS), FALSE);
	
	
	//s->htmlBook = gbs_control(s->workbook_lower, s);	
	//gtk_box_pack_start(GTK_BOX(hboxToolbar), s->toolbarBooks, TRUE,TRUE, 0);
			   
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
	/*********************************************************************** end GBS html editor/display */
	
	
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



	appbar1 = gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gtk_widget_ref(appbar1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "appbar1",
				 appbar1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(appbar1);
	gnome_app_set_statusbar(GNOME_APP(s->app), appbar1);

	s->appbar = lookup_widget(s->app, "appbar1");

        /**** create pixmaps of ctrees  ****/
		
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
			   GTK_SIGNAL_FUNC(on_mainwindow_destroy), NULL);
	gtk_signal_connect(GTK_OBJECT(s->app), "size_allocate",
			   GTK_SIGNAL_FUNC(on_mainwindow_size_allocate),
			   NULL);
	gnome_app_install_menu_hints(GNOME_APP(s->app),
				     menubar1_uiinfo);
	gtk_signal_connect(GTK_OBJECT(btnSBDock), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSBDock_clicked),
			   s);
	gtk_signal_connect(GTK_OBJECT(btnStrongs), "toggled",
			   GTK_SIGNAL_FUNC(on_btnStrongs_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnMorphs), "toggled",
			   GTK_SIGNAL_FUNC(on_btnMorphs_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnFootnotes), "toggled",
			   GTK_SIGNAL_FUNC(on_btnFootnotes_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSB), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSB_clicked), s);
	gtk_signal_connect(GTK_OBJECT(cbeBook), "changed",
			   GTK_SIGNAL_FUNC(on_cbeBook_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(spbChapter), "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_spbChapter_button_release_event), NULL);
	gtk_signal_connect(GTK_OBJECT(spbVerse), "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_spbVerse_button_release_event), NULL);
	gtk_signal_connect(GTK_OBJECT(cbeFreeformLookup),
			   "key_press_event",
			   GTK_SIGNAL_FUNC
			   (on_cbeFreeformLookup_key_press_event), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLookup), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLookup_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnBack), "clicked",
			   GTK_SIGNAL_FUNC(on_btnBack_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnFoward), "clicked",
			   GTK_SIGNAL_FUNC(on_btnFoward_clicked), NULL);
/*	gtk_signal_connect(GTK_OBJECT(btnExit), "clicked",
			   GTK_SIGNAL_FUNC(on_btnExit_clicked), NULL);*/
	gtk_signal_connect(GTK_OBJECT(notebook3), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook3_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnCommSyn), "clicked",
			   GTK_SIGNAL_FUNC(on_btnCommSyn_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnCommPrint), "clicked",
			   GTK_SIGNAL_FUNC(on_btnCommPrint_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnComPrev), "clicked",
			   GTK_SIGNAL_FUNC(on_btnComPrev_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnComNext), "clicked",
			   GTK_SIGNAL_FUNC(on_btnComNext_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(textComments), "changed",
			   GTK_SIGNAL_FUNC(on_textComments_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(textComments),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_textComments_button_release_event), NULL);
	gtk_signal_connect(GTK_OBJECT(textComments), "key_press_event",
			   GTK_SIGNAL_FUNC
			   (on_textComments_key_press_event), NULL);

	gtk_signal_connect(GTK_OBJECT(notebook1), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook1_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnComPrev), "clicked",
			   GTK_SIGNAL_FUNC(on_btnComPrev_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnComNext), "clicked",
			   GTK_SIGNAL_FUNC(on_btnComNext_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(notebook4), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook4_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnKeyPrev), "clicked",
			   GTK_SIGNAL_FUNC(on_btnKeyPrev_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(dictionarySearchText), "changed",
			   GTK_SIGNAL_FUNC
			   (on_dictionarySearchText_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(dictionarySearchText),
			   "key_press_event",
			   GTK_SIGNAL_FUNC
			   (on_dictionarySearchText_key_press_event),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnKeyNext), "clicked",
			   GTK_SIGNAL_FUNC(on_btnKeyNext_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(list1), "select_row",
			   GTK_SIGNAL_FUNC(on_list1_select_row), NULL);
	gtk_signal_connect(GTK_OBJECT(list1), "button_press_event",
			   GTK_SIGNAL_FUNC(on_list1_button_press_event),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(epaned), "button_release_event",
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
	if (s->showsplash) {
		e_splash_set_icon_highlight(E_SPLASH(splash), 0, TRUE);
		while (gtk_events_pending())
			gtk_main_iteration();
	}

	return s->app;
}
