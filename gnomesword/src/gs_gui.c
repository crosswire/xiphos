/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            		gs_gui.c
                             -------------------
  				Wed June 27 2001
    			copyright (C) 2001 by Terry Biggs
   			   tbiggs@users.sourceforge.net
 ***************************************************************************/

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
#  include <config.h>
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


#include "gs_gui.h"
#include "callback.h"
#include "gs_studypad.h"
#include "gs_gnomesword.h"
#include "support.h"
#include "gs_shortcutbar.h"
#include "sw_gnomesword.h"
#include "e-splash.h"


#ifdef USE_SPELL
#include "spell.h"
#include "spell_gui.h"
#endif				/* USE_SPELL */


GtkWidget *appbar1;
GtkWidget *main_label;

extern GtkWidget *shortcut_bar;
extern EShortcutModel *shortcut_model;
extern SETTINGS *settings;


static GnomeUIInfo file1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EXIT_ITEM(on_exit1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo edit1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, "Copy",
	 "Copy high lighted text form main window",
	 on_copy3_activate, "moduleText", NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, "Search",
	 "Open search dialog",
	 on_search1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo quickmarks_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, "Add Quickmark",
	 "Add current verse to Quickmarks menu",
	 on_add_quickmark_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "Edit Quickmarks",
	 "Edit Quickmarks",
	 on_edit_quickmarks_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_GREEN,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo history1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, "C_lear",
	 "Clear history list",
	 on_clear1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo settings1_menu_uiinfo[] = {
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

static GnomeUIInfo about_sword_modules1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, "Bible Texts",
	 "Information about Bible texts",
	 bible_texts1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "Commentaries",
	 NULL,
	 commentaries1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "Dictionaries-Lexicons",
	 NULL,
	 dictionaries_lexicons1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo help1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, "Contents",
	 "Help Contents",
	 on_help_contents_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_HELP,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "About the Sword Project...",
	 "About The Sword Project",
	 on_about_the_sword_project1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "About Sword Modules",
	 "Information about the installed modules",
	 about_sword_modules1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, "About GnomeSword...",
	 "About GnomeSword",
	 on_about_gnomesword1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] = {
	GNOMEUIINFO_MENU_FILE_TREE(file1_menu_uiinfo),
	GNOMEUIINFO_MENU_EDIT_TREE(edit1_menu_uiinfo),
	{
	 GNOME_APP_UI_SUBTREE, "_Quickmarks",
	 NULL,
	 quickmarks_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "_History",
	 NULL,
	 history1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_MENU_SETTINGS_TREE(settings1_menu_uiinfo),
	GNOMEUIINFO_MENU_HELP_TREE(help1_menu_uiinfo),
	GNOMEUIINFO_END
};

GtkWidget *create_mainwindow(GtkWidget * splash)
{
//      GtkWidget *mainwindow;
	GtkWidget *dock1;
	GtkWidget *toolbar20;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSearch;
	GtkWidget *btnStrongs;
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
	GtkWidget *btnExit;
	GtkWidget *mainPanel;
	GtkWidget *epaned;
	GtkWidget *vpaned1;
	GtkWidget *hbox2;
	GtkWidget *hpaned1;
	GtkWidget *vbox32;
	GtkWidget *nbTextMods;
	GtkWidget *frame9;
	GtkWidget *nbText;
	GtkWidget *swHtmlBible;
	GtkWidget *vbox19;
	GtkWidget *scrolledwindow18;
	GtkWidget *moduleText;
	GtkWidget *notebook3;
	GtkWidget *vbox22;
	GtkWidget *notebook1;
	GtkWidget *hbox18;
	GtkWidget *framecom;
	GtkWidget *nbCom;
	GtkWidget *swHtmlCom;
	GtkWidget *label;
	GtkWidget *vbox23;
	GtkWidget *scrolledwindow28;
	GtkWidget *textCommentaries;
	GtkWidget *handlebox17;
	GtkWidget *toolbar26;
	GtkWidget *btnComPrev;
	GtkWidget *btnComNext;
	GtkWidget *label64;
	GtkWidget *vbox2;
	GtkWidget *hbox11;
	GtkWidget *vbox8;
	GtkWidget *nbPerCom;
	GtkWidget *swHtmlPerCom;
	GtkWidget *scrolledwindow11;
	GtkWidget *textComments;
	GtkWidget *sbNotes;
	GtkWidget *handlebox16;
	GtkWidget *tbNotes;
	GtkWidget *tbtnFollow;
	GtkWidget *btnEditNote;
	GtkWidget *btnSaveNote;
	GtkWidget *btnDeleteNote;
	GtkWidget *btnSpellNotes;
	GtkWidget *label85;
	GtkWidget *vbox3;
	GtkWidget *frame2;
	GtkWidget *vbox4;
	GtkWidget *scrolledwindow15;
	GtkWidget *framedict;
	GtkWidget *label12;
	GtkWidget *frame12;
	GtkWidget *vbox6;
	GtkWidget *handlebox10;
	GtkWidget *toolbar5;
	GtkWidget *btnSPnew;
	GtkWidget *btnOpenFile;
	GtkWidget *btnSaveFile;
	GtkWidget *btnSaveFileAs;
	GtkWidget *vseparator9;
	GtkWidget *btnPrint;
	GtkWidget *vseparator10;
	GtkWidget *btnCut;
	GtkWidget *btnCopy;
	GtkWidget *btnPaste;
	GtkWidget *vseparator11;
	GtkWidget *btnSpell;
	GtkWidget *scrolledwindow17;
	GtkWidget *text3;
	GtkWidget *statusbar2;
	GtkWidget *label41;
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
	GtkWidget *hbox25;
	gchar *pathname;
	GdkPixbuf *icon_pixbuf;

	g_print("Building GnomeSword interface\n");
	if (settings->showsplash) {
		pathname = gnome_pixmap_file("gnomesword/GnomeSword.xpm");
		icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);
		pathname = gnome_pixmap_file("gnomesword/sword.xpm");
		icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);
		pathname = gnome_pixmap_file("gnomesword/sword.xpm");
		icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);
		pathname = gnome_pixmap_file("gnomesword/GnomeSword.xpm");
		icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);
		while (gtk_events_pending())
			gtk_main_iteration();
	}

	settings->app =
	    gnome_app_new("gnomesword",
			  "GnomeSword - Bible Study Software");
	gtk_object_set_data(GTK_OBJECT(settings->app), "settings->app",
			    settings->app);
	gtk_widget_set_usize(settings->app, 680, 480);
	GTK_WIDGET_SET_FLAGS(settings->app, GTK_CAN_FOCUS);

	dock1 = GNOME_APP(settings->app)->dock;
	gtk_widget_ref(dock1);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "dock1", dock1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dock1);

	gnome_app_create_menus(GNOME_APP(settings->app), menubar1_uiinfo);

	gtk_widget_ref(menubar1_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "file1",
				 menubar1_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(file1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "separator4",
				 file1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(file1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "exit1",
				 file1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "edit1",
				 menubar1_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "copy1",
				 edit1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "separator3",
				 edit1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "search1",
				 edit1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "quickmarks",
				 menubar1_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(quickmarks_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "add_bookmark1",
				 quickmarks_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(quickmarks_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "edit_quickmarks",
				 quickmarks_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(quickmarks_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "separator4",
				 quickmarks_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "history1",
				 menubar1_uiinfo[3].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(history1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "clear1",
				 history1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(history1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "separator5",
				 history1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "settings1",
				 menubar1_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(settings1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "preferences1",
				 settings1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[5].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "help1",
				 menubar1_uiinfo[5].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_right_justify(GTK_MENU_ITEM
				    (menubar1_uiinfo[5].widget));

	gtk_widget_ref(help1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "about_the_sword_project1",
				 help1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(help1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "about_gnomesword1",
				 help1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(help1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "about_sword_modules1",
				 help1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "bible_texts1",
				 about_sword_modules1_menu_uiinfo
				 [0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(bible_texts1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "separator15",
				 bible_texts1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "commentaries1",
				 about_sword_modules1_menu_uiinfo[1].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(commentaries1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "separator16",
				 commentaries1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "dictionaries_lexicons1",
				 about_sword_modules1_menu_uiinfo
				 [2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(dictionaries_lexicons1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "separator17",
				 dictionaries_lexicons1_menu_uiinfo
				 [0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	toolbar20 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar20);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "toolbar20",
				 toolbar20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar20);
	gnome_app_add_toolbar(GNOME_APP(settings->app),
			      GTK_TOOLBAR(toolbar20), "toolbar20",
			      GNOME_DOCK_ITEM_BEH_NEVER_VERTICAL,
			      GNOME_DOCK_TOP, 1, 0, 0);
	gtk_widget_set_usize(toolbar20, -2, 34);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar20),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_SEARCH);
	btnSearch =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Search",
				       "Search current text or commentary module",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSearch);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnSearch",
				 btnSearch,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSearch);

	tmp_toolbar_icon =
	    create_pixmap(settings->app, "gnomesword/strongs2.xpm", TRUE);
	btnStrongs =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, "Strongs",
				       "Toogle Strongs Numbers", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnStrongs);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnStrongs",
				 btnStrongs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnStrongs);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_INDEX);
	btnSB =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Shortcut Bar", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSB);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnSB", btnSB,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSB);

	cbBook = gtk_combo_new();
	gtk_widget_ref(cbBook);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "cbBook",
				 cbBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbBook);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20), cbBook, NULL,
				  NULL);
	gtk_widget_set_usize(cbBook, 154, -2);

	/*** get and load books of the Bible ***/
	cbBook_items = getBibleBooks();
	gtk_combo_set_popdown_strings(GTK_COMBO(cbBook), cbBook_items);
	g_list_free(cbBook_items);

	cbeBook = GTK_COMBO(cbBook)->entry;
	gtk_widget_ref(cbeBook);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "cbeBook",
				 cbeBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbeBook);
	gtk_entry_set_text(GTK_ENTRY(cbeBook), "Romans");

	spbChapter_adj = gtk_adjustment_new(8, 0, 151, 1, 10, 10);
	spbChapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbChapter_adj), 1, 0);
	gtk_widget_ref(spbChapter);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "spbChapter",
				 spbChapter,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spbChapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20), spbChapter, NULL,
				  NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spbChapter), TRUE);

	spbVerse_adj = gtk_adjustment_new(28, 0, 180, 1, 10, 10);
	spbVerse = gtk_spin_button_new(GTK_ADJUSTMENT(spbVerse_adj), 1, 0);
	gtk_widget_ref(spbVerse);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "spbVerse",
				 spbVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spbVerse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20), spbVerse, NULL,
				  NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spbVerse), TRUE);

	cbeFreeformLookup = gtk_entry_new();
	gtk_widget_ref(cbeFreeformLookup);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "cbeFreeformLookup", cbeFreeformLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbeFreeformLookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20),
				  cbeFreeformLookup, NULL, NULL);
	gtk_widget_set_usize(cbeFreeformLookup, 190, -2);
	gtk_entry_set_text(GTK_ENTRY(cbeFreeformLookup), "Romans 8:28");

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	btnLookup =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Goto verse",
				       "Go to verse in free form lookup and add verse to history",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLookup);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnLookup",
				 btnLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLookup);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_BACK);
	btnBack =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Back",
				       "Go backward through history list",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnBack);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnBack",
				 btnBack,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnBack);
	gtk_widget_set_sensitive(btnBack, FALSE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_FORWARD);
	btnFoward =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Foward",
				       "Go foward through history list",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnFoward);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnFoward",
				 btnFoward,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnFoward);
	gtk_widget_set_sensitive(btnFoward, FALSE);

	vseparator13 = gtk_vseparator_new();
	gtk_widget_ref(vseparator13);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vseparator13",
				 vseparator13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator13);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20), vseparator13,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator13, 7, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_EXIT);
	btnExit =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Exit", "Exit GnomeSword", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnExit);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnExit",
				 btnExit,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnExit);

//-------------------------------------------------------------
	hbox25 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox25);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "hbox25",
				 hbox25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox25);
	gnome_app_set_contents(GNOME_APP(settings->app), hbox25);

	epaned = e_hpaned_new();
	gtk_widget_ref(epaned);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "epaned",
				 epaned,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(epaned);
	gtk_box_pack_start(GTK_BOX(hbox25), epaned, TRUE, TRUE, 0);
	e_paned_set_position(E_PANED(epaned), 120);

	shortcut_model = e_shortcut_model_new();

	shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(shortcut_bar),
				 shortcut_model);
	gtk_widget_set_usize(shortcut_bar, 150, 250);
	gtk_widget_show(shortcut_bar);

	settings->shortcut_bar = shortcut_bar;	//lookup_widget(settings->app,"shortcut_bar");

	e_paned_pack1(E_PANED(epaned), shortcut_bar, FALSE, TRUE);

//#if 0
	gtk_container_set_border_width(GTK_CONTAINER(shortcut_bar), 4);
//#endif

	mainPanel = gtk_vbox_new(FALSE, 0);
	e_paned_pack2(E_PANED(epaned), mainPanel, TRUE, TRUE);
	gtk_widget_show(mainPanel);
//-----------------------------------------------------------------
	vpaned1 = e_vpaned_new();
	gtk_widget_ref(vpaned1);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vpaned1",
				 vpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vpaned1);
	gtk_box_pack_start(GTK_BOX(mainPanel), vpaned1, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vpaned1), 1);
	e_paned_set_position(E_PANED(vpaned1), 236);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "hbox2", hbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	e_paned_pack1(E_PANED(vpaned1), hbox2, TRUE, TRUE);

	hpaned1 = e_hpaned_new();
	gtk_widget_ref(hpaned1);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "hpaned1",
				 hpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_set_usize(hpaned1, -2, 380);
	gtk_widget_show(hpaned1);
	gtk_box_pack_end(GTK_BOX(hbox2), hpaned1, TRUE, TRUE, 0);


	vbox32 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox32);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox32",
				 vbox32,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox32);
	//gtk_paned_pack1 (GTK_PANED (hpaned1), vbox32, FALSE, TRUE);
	e_paned_pack1(E_PANED(hpaned1), vbox32, FALSE, TRUE);

	nbTextMods = gtk_notebook_new();
	gtk_widget_ref(nbTextMods);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "nbTextMods",
				 nbTextMods,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(nbTextMods);
	gtk_box_pack_start(GTK_BOX(vbox32), nbTextMods, FALSE, TRUE, 0);
	gtk_widget_set_usize(nbTextMods, -2, 31);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(nbTextMods), TRUE);


	frame9 = gtk_frame_new(NULL);
	gtk_widget_ref(frame9);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "frame9",
				 frame9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame9);
	gtk_box_pack_start(GTK_BOX(vbox32), frame9, TRUE, TRUE, 0);

	gtk_widget_set_usize(frame9, 380, -2);
	gtk_container_set_border_width(GTK_CONTAINER(frame9), 2);

	nbText = gtk_notebook_new();
	gtk_widget_ref(nbText);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "nbText",
				 nbText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(nbText);
	gtk_container_add(GTK_CONTAINER(frame9), nbText);
	GTK_WIDGET_UNSET_FLAGS(nbText, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nbText), FALSE);

	vbox19 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox19);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox19",
				 vbox19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox19);
	gtk_container_add(GTK_CONTAINER(nbText), vbox19);

	scrolledwindow18 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow18);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "scrolledwindow18", scrolledwindow18,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow18);
	gtk_box_pack_start(GTK_BOX(vbox19), scrolledwindow18, TRUE, TRUE,
			   0);
	gtk_container_set_border_width(GTK_CONTAINER(scrolledwindow18), 2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow18),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	moduleText = gtk_text_new(NULL, NULL);
	gtk_widget_ref(moduleText);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "moduleText",
				 moduleText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(moduleText);
	gtk_container_add(GTK_CONTAINER(scrolledwindow18), moduleText);

	label = gtk_label_new("label");
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbText),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbText), 0),
				   label);

	swHtmlBible = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swHtmlBible);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "swHtmlBible",
				 swHtmlBible,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swHtmlBible);
	gtk_container_add(GTK_CONTAINER(nbText), swHtmlBible);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swHtmlBible),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	label = gtk_label_new("label");
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbText),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbText), 1),
				   label);

	notebook3 = gtk_notebook_new();
	gtk_widget_ref(notebook3);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "notebook3",
				 notebook3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook3);

	e_paned_pack2(E_PANED(hpaned1), notebook3, TRUE, TRUE);

	gtk_container_set_border_width(GTK_CONTAINER(notebook3), 2);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook3), TRUE);

	vbox22 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox22);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox22",
				 vbox22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox22);
	gtk_container_add(GTK_CONTAINER(notebook3), vbox22);

	notebook1 = gtk_notebook_new();
	gtk_widget_ref(notebook1);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "notebook1",
				 notebook1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook1);
	gtk_box_pack_start(GTK_BOX(vbox22), notebook1, FALSE, TRUE, 0);
	gtk_widget_set_usize(notebook1, -2, 31);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook1), TRUE);

	hbox18 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox18);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "hbox18",
				 hbox18,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox18);
	gtk_box_pack_start(GTK_BOX(vbox22), hbox18, TRUE, TRUE, 0);

	framecom = gtk_frame_new(NULL);
	gtk_widget_ref(framecom);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "framecom",
				 framecom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(framecom);
	gtk_box_pack_start(GTK_BOX(hbox18), framecom, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(framecom), 2);


	nbCom = gtk_notebook_new();
	gtk_widget_ref(nbCom);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "nbCom", nbCom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(nbCom);
	gtk_container_add(GTK_CONTAINER(framecom), nbCom);
	//gtk_container_set_border_width (GTK_CONTAINER (nbCom), 2);
	//gtk_container_add(GTK_CONTAINER(framecom), nbCom);
	//gtk_box_pack_start (GTK_BOX (framecom), nbCom, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(nbCom, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nbCom), FALSE);

	swHtmlCom = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swHtmlCom);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "swHtmlCom",
				 swHtmlCom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swHtmlCom);
	gtk_container_add(GTK_CONTAINER(nbCom), swHtmlCom);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swHtmlCom),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	/* htmlCommentaries goes here */

	label = gtk_label_new("label");
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbCom),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbCom), 0),
				   label);

	vbox23 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox23);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox23",
				 vbox23,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox23);
	gtk_container_add(GTK_CONTAINER(nbCom), vbox23);

	scrolledwindow28 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow28);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "scrolledwindow28", scrolledwindow28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow28);
	gtk_box_pack_start(GTK_BOX(vbox23), scrolledwindow28, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow28),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	textCommentaries = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textCommentaries);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "textCommentaries", textCommentaries,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textCommentaries);
	gtk_container_add(GTK_CONTAINER(scrolledwindow28),
			  textCommentaries);

	label = gtk_label_new("label");
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbCom),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbCom), 1),
				   label);

	handlebox17 = gtk_handle_box_new();
	gtk_widget_ref(handlebox17);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "handlebox17",
				 handlebox17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(hbox18), handlebox17, FALSE, FALSE, 0);
	gtk_handle_box_set_handle_position(GTK_HANDLE_BOX(handlebox17),
					   GTK_POS_TOP);

	toolbar26 =
	    gtk_toolbar_new(GTK_ORIENTATION_VERTICAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar26);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "toolbar26",
				 toolbar26,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar26);
	gtk_container_add(GTK_CONTAINER(handlebox17), toolbar26);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_UP);
	btnComPrev =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar26),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button2", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnComPrev);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnComPrev",
				 btnComPrev,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnComPrev);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_DOWN);
	btnComNext =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar26),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button3", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnComNext);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnComNext",
				 btnComNext,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnComNext);

	label64 = gtk_label_new("Commentaries");
	gtk_widget_ref(label64);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label64",
				 label64,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label64);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     0), label64);

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_container_add(GTK_CONTAINER(notebook3), vbox2);

	hbox11 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox11);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "hbox11",
				 hbox11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox11);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox11, TRUE, TRUE, 0);

	vbox8 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox8);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox8", vbox8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox8);
	gtk_box_pack_start(GTK_BOX(hbox11), vbox8, TRUE, TRUE, 0);


	nbPerCom = gtk_notebook_new();
	gtk_widget_ref(nbPerCom);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "nbPerCom",
				 nbPerCom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(nbPerCom);
	gtk_box_pack_start(GTK_BOX(vbox8), nbPerCom, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(nbPerCom, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nbPerCom), FALSE);

	swHtmlPerCom = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swHtmlPerCom);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "swHtmlPerCom",
				 swHtmlPerCom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swHtmlPerCom);
	gtk_container_add(GTK_CONTAINER(nbPerCom), swHtmlPerCom);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swHtmlPerCom),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);




	label = gtk_label_new("label");
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbPerCom),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbPerCom),
							     0), label);

	scrolledwindow11 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow11);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
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
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "textComments",
				 textComments,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComments);
	gtk_container_add(GTK_CONTAINER(scrolledwindow11), textComments);

	label = gtk_label_new("label");
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbPerCom),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbPerCom),
							     1), label);


	sbNotes = gtk_statusbar_new();
	gtk_widget_ref(sbNotes);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "sbNotes",
				 sbNotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(vbox8), sbNotes, FALSE, FALSE, 0);

	handlebox16 = gtk_handle_box_new();
	gtk_widget_ref(handlebox16);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "handlebox16",
				 handlebox16,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handlebox16);
	gtk_box_pack_start(GTK_BOX(hbox11), handlebox16, FALSE, FALSE, 0);
	gtk_handle_box_set_shadow_type(GTK_HANDLE_BOX(handlebox16),
				       GTK_SHADOW_NONE);
	gtk_handle_box_set_handle_position(GTK_HANDLE_BOX(handlebox16),
					   GTK_POS_TOP);

	tbNotes =
	    gtk_toolbar_new(GTK_ORIENTATION_VERTICAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(tbNotes);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "tbNotes",
				 tbNotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tbNotes);
	gtk_container_add(GTK_CONTAINER(handlebox16), tbNotes);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(tbNotes),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	tbtnFollow =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, "togglebutton1",
				       "Follow Main Text Window", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(tbtnFollow);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "tbtnFollow",
				 tbtnFollow,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tbtnFollow);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tbtnFollow), TRUE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_PROPERTIES);
	btnEditNote =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, "Edit", "Edit note", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEditNote);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnEditNote",
				 btnEditNote,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEditNote);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_ADD);
	btnSaveNote =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Save", "Save note", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSaveNote);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnSaveNote",
				 btnSaveNote,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSaveNote);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_REMOVE);
	btnDeleteNote =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Delete", "Delete note", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnDeleteNote);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "btnDeleteNote", btnDeleteNote,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnDeleteNote);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	btnSpellNotes =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button1", "Start Spell Check",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSpellNotes);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "btnSpellNotes", btnSpellNotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSpellNotes);

	label85 = gtk_label_new("Personal Comments");
	gtk_widget_ref(label85);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label85",
				 label85,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label85);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     1), label85);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox3", vbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(notebook3), vbox3);

	frame2 = gtk_frame_new(NULL);
	gtk_widget_ref(frame2);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "frame2",
				 frame2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox3), frame2, TRUE, TRUE, 0);
	//gtk_widget_set_usize(frame2, -2, 67);

	vbox4 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox4);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox4", vbox4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(frame2), vbox4);



	scrolledwindow15 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow15);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "scrolledwindow15", scrolledwindow15,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow15);
	gtk_box_pack_start(GTK_BOX(vbox4), scrolledwindow15, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow15),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);

	label12 = gtk_label_new("Interlinear");
	gtk_widget_ref(label12);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label12",
				 label12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label12);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     2), label12);

	frame12 = gtk_frame_new(NULL);
	gtk_widget_ref(frame12);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "frame12",
				 frame12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame12);
	gtk_container_add(GTK_CONTAINER(notebook3), frame12);

	vbox6 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox6);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox6", vbox6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox6);
	gtk_container_add(GTK_CONTAINER(frame12), vbox6);

	handlebox10 = gtk_handle_box_new();
	gtk_widget_ref(handlebox10);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "handlebox10",
				 handlebox10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handlebox10);
	gtk_box_pack_start(GTK_BOX(vbox6), handlebox10, FALSE, TRUE, 0);
	gtk_handle_box_set_shadow_type(GTK_HANDLE_BOX(handlebox10),
				       GTK_SHADOW_NONE);
	gtk_handle_box_set_snap_edge(GTK_HANDLE_BOX(handlebox10),
				     GTK_POS_LEFT);

	toolbar5 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar5);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "toolbar5",
				 toolbar5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar5);
	gtk_container_add(GTK_CONTAINER(handlebox10), toolbar5);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar5),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_NEW);
	btnSPnew =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button1", "Start new document",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSPnew);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnSPnew",
				 btnSPnew,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSPnew);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_OPEN);
	btnOpenFile =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Open", "Open File", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnOpenFile);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnOpenFile",
				 btnOpenFile,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnOpenFile);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_SAVE);
	btnSaveFile =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Save", "Save", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSaveFile);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnSaveFile",
				 btnSaveFile,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSaveFile);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_SAVE_AS);
	btnSaveFileAs =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Save As", "Save as", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSaveFileAs);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "btnSaveFileAs", btnSaveFileAs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSaveFileAs);

	vseparator9 = gtk_vseparator_new();
	gtk_widget_ref(vseparator9);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vseparator9",
				 vseparator9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator9);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar5), vseparator9, NULL,
				  NULL);
	gtk_widget_set_usize(vseparator9, 7, 17);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_PRINT);
	btnPrint =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Print", "Print ", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnPrint);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnPrint",
				 btnPrint,
				 (GtkDestroyNotify) gtk_widget_unref);

	vseparator10 = gtk_vseparator_new();
	gtk_widget_ref(vseparator10);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vseparator10",
				 vseparator10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator10);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar5), vseparator10,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator10, 7, 17);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_CUT);
	btnCut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Cut", "Cut to clipboard", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnCut);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnCut",
				 btnCut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnCut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_COPY);
	btnCopy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Copy", "Copy to clipboard", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnCopy);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnCopy",
				 btnCopy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnCopy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_PASTE);
	btnPaste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Paste", "Paste from clipborad",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnPaste);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnPaste",
				 btnPaste,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPaste);

	vseparator11 = gtk_vseparator_new();
	gtk_widget_ref(vseparator11);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vseparator11",
				 vseparator11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator11);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar5), vseparator11,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator11, -2, 17);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	btnSpell =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button8", "Start Spell Check",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSpell);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnSpell",
				 btnSpell,
				 (GtkDestroyNotify) gtk_widget_unref);

	scrolledwindow17 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow17);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "scrolledwindow17", scrolledwindow17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow17);
	gtk_box_pack_start(GTK_BOX(vbox6), scrolledwindow17, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow17),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	text3 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(text3);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "text3", text3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(text3);
	gtk_container_add(GTK_CONTAINER(scrolledwindow17), text3);
	gtk_text_set_editable(GTK_TEXT(text3), TRUE);

	statusbar2 = gtk_statusbar_new();
	gtk_widget_ref(statusbar2);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "statusbar2",
				 statusbar2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(statusbar2);
	gtk_box_pack_start(GTK_BOX(vbox6), statusbar2, FALSE, TRUE, 0);

	label41 = gtk_label_new("Study Pad");
	gtk_widget_ref(label41);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label41",
				 label41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label41);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     3), label41);

	hbox8 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox8);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "hbox8", hbox8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox8);


	e_paned_pack2(E_PANED(vpaned1), hbox8, TRUE, TRUE);


	gtk_widget_set_usize(hbox8, -2, 115);

	frame10 = gtk_frame_new(NULL);
	gtk_widget_ref(frame10);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "frame10",
				 frame10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame10);
	gtk_box_pack_start(GTK_BOX(hbox8), frame10, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame10, -2, 205);
	gtk_container_set_border_width(GTK_CONTAINER(frame10), 2);

	hbox13 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox13);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "hbox13",
				 hbox13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox13);
	gtk_container_add(GTK_CONTAINER(frame10), hbox13);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox1", vbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_box_pack_start(GTK_BOX(hbox13), vbox1, TRUE, TRUE, 0);
	gtk_widget_set_usize(vbox1, 427, -2);

	notebook4 = gtk_notebook_new();
	gtk_widget_ref(notebook4);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "notebook4",
				 notebook4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook4);
	gtk_box_pack_start(GTK_BOX(vbox1), notebook4, FALSE, TRUE, 0);
	gtk_widget_set_usize(notebook4, -2, 31);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook4), TRUE);

	framedict = gtk_frame_new(NULL);
	gtk_widget_ref(framecom);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "framedict",
				 framedict,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(framedict);
	gtk_box_pack_start(GTK_BOX(vbox1), framedict, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(framedict), 2);

	scrolledwindow8 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow8);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "scrolledwindow8", scrolledwindow8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow8);
	gtk_container_add(GTK_CONTAINER(framedict), scrolledwindow8);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow8), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	vbox14 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox14);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vbox14",
				 vbox14,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox14);
	gtk_box_pack_start(GTK_BOX(hbox13), vbox14, FALSE, TRUE, 0);
	gtk_widget_set_usize(vbox14, 208, -2);

	hbox19 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox19);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "hbox19",
				 hbox19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox19);
	gtk_box_pack_start(GTK_BOX(vbox14), hbox19, FALSE, FALSE, 0);

	toolbar25 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar25);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "toolbar25",
				 toolbar25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar25);
	gtk_box_pack_start(GTK_BOX(hbox19), toolbar25, TRUE, TRUE, 0);
	gtk_widget_set_usize(toolbar25, 132, -2);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar25),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_BACK);
	btnKeyPrev =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar25),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button2", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnKeyPrev);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnKeyPrev",
				 btnKeyPrev,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnKeyPrev);

	dictionarySearchText = gtk_entry_new();
	gtk_widget_ref(dictionarySearchText);
	gtk_object_set_data_full(GTK_OBJECT(settings->app),
				 "dictionarySearchText",
				 dictionarySearchText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dictionarySearchText);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar25),
				  dictionarySearchText, NULL, NULL);
	gtk_widget_set_usize(dictionarySearchText, 145, -2);
	gtk_entry_set_text(GTK_ENTRY(dictionarySearchText), "GRACE");

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings->app,
				      GNOME_STOCK_PIXMAP_FORWARD);
	btnKeyNext =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar25),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button3", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnKeyNext);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "btnKeyNext",
				 btnKeyNext,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnKeyNext);

	list1 = gtk_clist_new(1);
	gtk_widget_ref(list1);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "list1", list1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(list1);
	gtk_box_pack_start(GTK_BOX(vbox14), list1, TRUE, TRUE, 0);
	gtk_widget_set_usize(list1, 126, -2);
	gtk_clist_set_column_width(GTK_CLIST(list1), 0, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(list1));

	label58 = gtk_label_new("label58");
	gtk_widget_ref(label58);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "label58",
				 label58,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label58);
	gtk_clist_set_column_widget(GTK_CLIST(list1), 0, label58);

	appbar1 = gnome_appbar_new(TRUE, TRUE, GNOME_PREFERENCES_NEVER);
	gtk_widget_ref(appbar1);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "appbar1",
				 appbar1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(appbar1);
	gnome_app_set_statusbar(GNOME_APP(settings->app), appbar1);

	settings->appbar = lookup_widget(settings->app, "appbar1");

	gtk_signal_connect(GTK_OBJECT(settings->app), "destroy",
			   GTK_SIGNAL_FUNC(on_mainwindow_destroy), NULL);
	gnome_app_install_menu_hints(GNOME_APP(settings->app),
				     menubar1_uiinfo);
	gtk_signal_connect(GTK_OBJECT(btnSearch), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSearch_clicked),
			   settings);
	gtk_signal_connect(GTK_OBJECT(btnStrongs), "toggled",
			   GTK_SIGNAL_FUNC(on_btnStrongs_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSB), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSB_clicked), 
			   settings);
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
	gtk_signal_connect(GTK_OBJECT(btnExit), "clicked",
			   GTK_SIGNAL_FUNC(on_btnExit_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(moduleText), "enter_notify_event",
			   GTK_SIGNAL_FUNC
			   (on_moduleText_enter_notify_event), NULL);
	gtk_signal_connect(GTK_OBJECT(notebook3), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook3_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(textCommentaries),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_textComments_button_release_event), NULL);
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
	gtk_signal_connect(GTK_OBJECT(tbtnFollow), "toggled",
			   GTK_SIGNAL_FUNC(on_tbtnFollow_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEditNote), "toggled",
			   GTK_SIGNAL_FUNC(on_btnEditNote_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSaveNote), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSaveNote_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnDeleteNote), "clicked",
			   GTK_SIGNAL_FUNC(on_btnDeleteNote_clicked),
			   NULL);
#ifdef USE_SPELL
	gtk_signal_connect(GTK_OBJECT(btnSpellNotes), "clicked",
			   GTK_SIGNAL_FUNC(spell_check_cb),
			   (gchar *) "textComments");
#endif				/* USE_SPELL */

	gtk_signal_connect(GTK_OBJECT(moduleText), "enter_notify_event",
			   GTK_SIGNAL_FUNC
			   (on_moduleText_enter_notify_event), NULL);

	gtk_signal_connect(GTK_OBJECT(notebook3), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook3_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(notebook1), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook1_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(textCommentaries),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_textComments_button_release_event), NULL);
	gtk_signal_connect(GTK_OBJECT(btnComPrev), "clicked",
			   GTK_SIGNAL_FUNC(on_btnComPrev_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnComNext), "clicked",
			   GTK_SIGNAL_FUNC(on_btnComNext_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSPnew), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSPnew_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnOpenFile), "clicked",
			   GTK_SIGNAL_FUNC(on_btnOpenFile_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSaveFile), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSaveFile_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSaveFileAs), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSaveFileAs_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnPrint), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPrint_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnCut), "clicked",
			   GTK_SIGNAL_FUNC(on_btnCut_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnCopy), "clicked",
			   GTK_SIGNAL_FUNC(on_btnCopy_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnPaste), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPaste_clicked), NULL);

#ifdef USE_SPELL
	gtk_signal_connect(GTK_OBJECT(btnSpell), "clicked",
			   GTK_SIGNAL_FUNC(spell_check_cb),
			   (gchar *) "text3");
#endif				/* USE_SPELL */

	gtk_signal_connect(GTK_OBJECT(text3), "changed",
			   GTK_SIGNAL_FUNC(on_text3_changed), NULL);
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

	gtk_widget_grab_focus(settings->app);
	if (settings->showsplash) {
		e_splash_set_icon_highlight(E_SPLASH(splash), 0, TRUE);
		while (gtk_events_pending())
			gtk_main_iteration();
	}

	return settings->app;
}
