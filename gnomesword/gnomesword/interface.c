/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            interface.c
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
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


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
//#include <gtkhtml.h>

#include <gnome.h>
#include <gtk/gtk.h>

#include "callback.h"
#include "gs_studypad.h"
#include "interface.h"
#include "support.h"

//#ifdef  USE_SHORTCUTBAR 
#include  <widgets/e-paned/e-paned.h>
#include  <widgets/e-paned/e-hpaned.h>
#include  <widgets/e-paned/e-vpaned.h>
#include  <widgets/shortcut-bar/e-shortcut-bar.h>
//#endif
//GdkColor   bgColor = {0, 0xdfff, 0xdfff, 0xffff};

#define NUM_SHORTCUT_TYPES 4
gchar *shortcut_types[NUM_SHORTCUT_TYPES] = {
	"bible:", "commentary:", "dict/lex:",
	"history:"
};
gchar *icon_filenames[NUM_SHORTCUT_TYPES] = {
	"gnomesword/GnomeSword.png", "gnome-word.png",
	    "gnomesword/GnomeSword.png",
	    "gnome-folder.png"
};

GdkPixbuf *icon_pixbufs[NUM_SHORTCUT_TYPES];
GtkWidget *main_label, *MainFrm, *shortcut_bar;
EShortcutModel *shortcut_model;
static GdkPixbuf *icon_callback(EShortcutBar * shortcut_bar,
				const gchar * url, gpointer data);
			    			     
static GdkPixbuf *icon_callback(EShortcutBar * shortcut_bar,
				const gchar * url, gpointer data)
{
	gint i;

	for (i = 0; i < NUM_SHORTCUT_TYPES; i++) {
		if (!strncmp(url, shortcut_types[i],
			     strlen(shortcut_types[i]))) {
			gdk_pixbuf_ref(icon_pixbufs[i]);
			return icon_pixbufs[i];
		}
	}

	return NULL;
}

GtkWidget *create_fileselection1(void)
{
	GtkWidget *fileselection1;
	GtkWidget *ok_button1;
	GtkWidget *cancel_button1;

	fileselection1 =
	    gtk_file_selection_new("GtkSword - Open Note File");
	gtk_object_set_data(GTK_OBJECT(fileselection1), "fileselection1",
			    fileselection1);
	gtk_container_set_border_width(GTK_CONTAINER(fileselection1), 10);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION
					       (fileselection1));

	ok_button1 = GTK_FILE_SELECTION(fileselection1)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselection1), "ok_button1",
			    ok_button1);
	gtk_widget_show(ok_button1);
	GTK_WIDGET_SET_FLAGS(ok_button1, GTK_CAN_DEFAULT);

	cancel_button1 = GTK_FILE_SELECTION(fileselection1)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselection1), "cancel_button1",
			    cancel_button1);
	gtk_widget_show(cancel_button1);
	GTK_WIDGET_SET_FLAGS(cancel_button1, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(ok_button1), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button1_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(cancel_button1), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button1_clicked),
			   NULL);

	return fileselection1;
}

GtkWidget *create_fileselectionSave(void)
{
	GtkWidget *fileselectionSave;
	GtkWidget *ok_button2;
	GtkWidget *cancel_button2;

	fileselectionSave =
	    gtk_file_selection_new("GtkSword - Save Note File");
	gtk_object_set_data(GTK_OBJECT(fileselectionSave),
			    "fileselectionSave", fileselectionSave);
	gtk_container_set_border_width(GTK_CONTAINER(fileselectionSave),
				       10);

	ok_button2 = GTK_FILE_SELECTION(fileselectionSave)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselectionSave), "ok_button2",
			    ok_button2);
	gtk_widget_show(ok_button2);
	GTK_WIDGET_SET_FLAGS(ok_button2, GTK_CAN_DEFAULT);

	cancel_button2 =
	    GTK_FILE_SELECTION(fileselectionSave)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselectionSave),
			    "cancel_button2", cancel_button2);
	gtk_widget_show(cancel_button2);
	GTK_WIDGET_SET_FLAGS(cancel_button2, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button2_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(cancel_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button2_clicked),
			   NULL);

	return fileselectionSave;
}

static GnomeUIInfo menu1_uiinfo[] = {
	GNOMEUIINFO_MENU_COPY_ITEM(on_copy3_activate, "moduleText"),
	{
	 GNOME_APP_UI_ITEM, "Lookup Selection",
	 NULL,
	 on_lookup_selection_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_SEARCH,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "About this module",
	 NULL,
	 on_about_this_module1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

GtkWidget *create_menu1(void)
{
	GtkWidget *menu1;

	menu1 = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(menu1), "menu1", menu1);
	gnome_app_fill_menu(GTK_MENU_SHELL(menu1), menu1_uiinfo,
			    NULL, FALSE, 0);

	gtk_widget_ref(menu1_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(menu1), "copy1",
				 menu1_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menu1_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(menu1), "lookup_selection1",
				 menu1_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menu1_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(menu1), "about_this_module1",
				 menu1_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menu1_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(menu1), "separator2",
				 menu1_uiinfo[3].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	return menu1;
}

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

static GnomeUIInfo bookmarks1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, "Add Bookmark",
	 "Add current verse to bookmarks menu",
	 on_add_bookmark1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "Edit Bookmarks",
	 "Edit bookmarks list",
	 on_edit_bookmarks1_activate, NULL, NULL,
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

static GnomeUIInfo main_window1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo interlinear1_window1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo interlinear2_window1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo interlinear3_window1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo commentary_window1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo dict_lex_window1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo view1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, "Main Window",
	 "Select text module for main window",
	 main_window1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "Interlinear1 Window",
	 "Select text module for 1st Interlinear window",
	 interlinear1_window1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "Interlinear2 Window",
	 "Select text module for 2nd Interlinear window",
	 interlinear2_window1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "Interlinear3 Window",
	 "Select text module for 3rd Interlinear window",
	 interlinear3_window1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "Commentary Window",
	 "Seledect commentary module",
	 commentary_window1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "Dict-Lex Window",
	 "Select Dict-Lex module",
	 dict_lex_window1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
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
	 GNOME_APP_UI_ITEM, "About the Sword Project...",
	 "About The Sword Project",
	 on_about_the_sword_project1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "About GnomeSword...",
	 "About GnomeSword",
	 on_about_gnomesword1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "About Sword Modules",
	 "Information about the installed modules",
	 about_sword_modules1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] = {
	GNOMEUIINFO_MENU_FILE_TREE(file1_menu_uiinfo),
	GNOMEUIINFO_MENU_EDIT_TREE(edit1_menu_uiinfo),
	{
	 GNOME_APP_UI_SUBTREE, "_Bookmarks",
	 NULL,
	 bookmarks1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, "_History",
	 NULL,
	 history1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_MENU_VIEW_TREE(view1_menu_uiinfo),
	GNOMEUIINFO_MENU_SETTINGS_TREE(settings1_menu_uiinfo),
	GNOMEUIINFO_MENU_HELP_TREE(help1_menu_uiinfo),
	GNOMEUIINFO_END
};

GtkWidget *create_mainwindow(void)
{
	GtkWidget *mainwindow;
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
	GtkWidget *epaned;
	GtkWidget *mainPanel;
	GtkWidget *vpaned1;
	GtkWidget *hbox2;
	GtkWidget *hpaned1;
	GtkWidget *frame9;
	GtkWidget *vbox19;
	GtkWidget *scrolledwindow18;
	GtkWidget *moduleText;
	//GtkWidget *frame19;
	//GtkWidget *lbText;
	GtkWidget *notebook3;
	GtkWidget *vbox22;
	GtkWidget *notebook1;
	GtkWidget *hbox18;
	GtkWidget *framecom;
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
	GtkWidget *textComp1;
	GtkWidget *frame17;
	GtkWidget *scrolledwindow16;
	GtkWidget *textComp2;
	GtkWidget *frame18;
	GtkWidget *scrolledwindow21;
	GtkWidget *textComp3;
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
	GtkWidget *textDict;
	GtkWidget *vbox14;
	GtkWidget *hbox19;
	GtkWidget *toolbar25;
	GtkWidget *btnKeyPrev;
	GtkWidget *dictionarySearchText;
	GtkWidget *btnKeyNext;
	GtkWidget *list1;
	GtkWidget *label58;
	GtkWidget *appbar1, *hbox25;


	gint i;
	gchar *pathname;

	mainwindow =
	    gnome_app_new("gnomesword",
			  "GnomeSword - Bible Study Software");
	gtk_object_set_data(GTK_OBJECT(mainwindow), "mainwindow",
			    mainwindow);
	gtk_widget_set_usize(mainwindow, 680, 480);
	GTK_WIDGET_SET_FLAGS(mainwindow, GTK_CAN_FOCUS);
	gtk_window_set_default_size(GTK_WINDOW(mainwindow), 680, 480);

	dock1 = GNOME_APP(mainwindow)->dock;
	gtk_widget_ref(dock1);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "dock1", dock1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dock1);

	gnome_app_create_menus(GNOME_APP(mainwindow), menubar1_uiinfo);

	gtk_widget_ref(menubar1_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "file1",
				 menubar1_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(file1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator4",
				 file1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(file1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "exit1",
				 file1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "edit1",
				 menubar1_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "copy1",
				 edit1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator3",
				 edit1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "search1",
				 edit1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "bookmarks1",
				 menubar1_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(bookmarks1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "add_bookmark1",
				 bookmarks1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(bookmarks1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "edit_bookmarks1",
				 bookmarks1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(bookmarks1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator4",
				 bookmarks1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "history1",
				 menubar1_uiinfo[3].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(history1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "clear1",
				 history1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(history1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator5",
				 history1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "view1",
				 menubar1_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(view1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "main_window1",
				 view1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(main_window1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator8",
				 main_window1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(view1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "interlinear1_window1",
				 view1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(interlinear1_window1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator10",
				 interlinear1_window1_menu_uiinfo[0].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(view1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "interlinear2_window1",
				 view1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(interlinear2_window1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator9",
				 interlinear2_window1_menu_uiinfo[0].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(view1_menu_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "interlinear3_window1",
				 view1_menu_uiinfo[3].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(interlinear3_window1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator11",
				 interlinear3_window1_menu_uiinfo[0].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(view1_menu_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "commentary_window1",
				 view1_menu_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(commentary_window1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator20",
				 commentary_window1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(view1_menu_uiinfo[5].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "dict_lex_window1",
				 view1_menu_uiinfo[5].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(dict_lex_window1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator21",
				 dict_lex_window1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);



	gtk_widget_ref(menubar1_uiinfo[5].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "settings1",
				 menubar1_uiinfo[5].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(settings1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "preferences1",
				 settings1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[6].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "help1",
				 menubar1_uiinfo[6].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(help1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "about_the_sword_project1",
				 help1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(help1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "about_gnomesword1",
				 help1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(help1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "about_sword_modules1",
				 help1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "bible_texts1",
				 about_sword_modules1_menu_uiinfo[0].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(bible_texts1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator15",
				 bible_texts1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "commentaries1",
				 about_sword_modules1_menu_uiinfo[1].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(commentaries1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator16",
				 commentaries1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "dictionaries_lexicons1",
				 about_sword_modules1_menu_uiinfo[2].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(dictionaries_lexicons1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "separator17",
				 dictionaries_lexicons1_menu_uiinfo[0].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	toolbar20 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar20);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "toolbar20",
				 toolbar20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar20);
	gnome_app_add_toolbar(GNOME_APP(mainwindow),
			      GTK_TOOLBAR(toolbar20), "toolbar20",
			      GNOME_DOCK_ITEM_BEH_NEVER_VERTICAL,
			      GNOME_DOCK_TOP, 1, 0, 0);
	gtk_widget_set_usize(toolbar20, -2, 34);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar20),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_SEARCH);
	btnSearch =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Search",
				       "Search current text or commentary module",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSearch);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnSearch",
				 btnSearch,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSearch);

	tmp_toolbar_icon =
	    create_pixmap(mainwindow, "gnomesword/strongs2.xpm", TRUE);
	btnStrongs =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, "Strongs",
				       "Toogle Strongs Numbers", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnStrongs);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnStrongs",
				 btnStrongs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnStrongs);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_INDEX);
	btnSB =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Shortcut Bar", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSB);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnSB", btnSB,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSB);

	cbBook = gtk_combo_new();
	gtk_widget_ref(cbBook);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "cbBook", cbBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbBook);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20), cbBook, NULL,
				  NULL);
	gtk_widget_set_usize(cbBook, 154, -2);
	cbBook_items = g_list_append(cbBook_items, "Genesis");
	cbBook_items = g_list_append(cbBook_items, "Exodus");
	cbBook_items = g_list_append(cbBook_items, "Leviticus");
	cbBook_items = g_list_append(cbBook_items, "Numbers");
	cbBook_items = g_list_append(cbBook_items, "Deuteronomy");
	cbBook_items = g_list_append(cbBook_items, "Joshua");
	cbBook_items = g_list_append(cbBook_items, "Judges");
	cbBook_items = g_list_append(cbBook_items, "Ruth");
	cbBook_items = g_list_append(cbBook_items, "I Samuel");
	cbBook_items = g_list_append(cbBook_items, "II Samuel");
	cbBook_items = g_list_append(cbBook_items, "I Kings");
	cbBook_items = g_list_append(cbBook_items, "II Kings");
	cbBook_items = g_list_append(cbBook_items, "I Chronicles");
	cbBook_items = g_list_append(cbBook_items, "II Chronicles");
	cbBook_items = g_list_append(cbBook_items, "Ezra");
	cbBook_items = g_list_append(cbBook_items, "Nehemiah");
	cbBook_items = g_list_append(cbBook_items, "Esther");
	cbBook_items = g_list_append(cbBook_items, "Job");
	cbBook_items = g_list_append(cbBook_items, "Psalms");
	cbBook_items = g_list_append(cbBook_items, "Proverbs");
	cbBook_items = g_list_append(cbBook_items, "Ecclesiastes");
	cbBook_items = g_list_append(cbBook_items, "Song of Solomon");
	cbBook_items = g_list_append(cbBook_items, "Isaiah");
	cbBook_items = g_list_append(cbBook_items, "Jeremiah");
	cbBook_items = g_list_append(cbBook_items, "Lamentations");
	cbBook_items = g_list_append(cbBook_items, "Ezekiel");
	cbBook_items = g_list_append(cbBook_items, "Daniel");
	cbBook_items = g_list_append(cbBook_items, "Hosea");
	cbBook_items = g_list_append(cbBook_items, "Joel");
	cbBook_items = g_list_append(cbBook_items, "Amos");
	cbBook_items = g_list_append(cbBook_items, "Obadiah");
	cbBook_items = g_list_append(cbBook_items, "Jonah");
	cbBook_items = g_list_append(cbBook_items, "Micah");
	cbBook_items = g_list_append(cbBook_items, "Nahum");
	cbBook_items = g_list_append(cbBook_items, "Habakkuk");
	cbBook_items = g_list_append(cbBook_items, "Zephaniah");
	cbBook_items = g_list_append(cbBook_items, "Haggai");
	cbBook_items = g_list_append(cbBook_items, "Zechariah");
	cbBook_items = g_list_append(cbBook_items, "Malachi");
	cbBook_items = g_list_append(cbBook_items, "Matthew");
	cbBook_items = g_list_append(cbBook_items, "Mark");
	cbBook_items = g_list_append(cbBook_items, "Luke");
	cbBook_items = g_list_append(cbBook_items, "John");
	cbBook_items = g_list_append(cbBook_items, "Acts");
	cbBook_items = g_list_append(cbBook_items, "Romans");
	cbBook_items = g_list_append(cbBook_items, "I Corinthians");
	cbBook_items = g_list_append(cbBook_items, "II Corinthians");
	cbBook_items = g_list_append(cbBook_items, "Galatians");
	cbBook_items = g_list_append(cbBook_items, "Ephesians");
	cbBook_items = g_list_append(cbBook_items, "Philippians");
	cbBook_items = g_list_append(cbBook_items, "Colossians");
	cbBook_items = g_list_append(cbBook_items, "I Thessalonians");
	cbBook_items = g_list_append(cbBook_items, "II Thessalonians");
	cbBook_items = g_list_append(cbBook_items, "I Timothy");
	cbBook_items = g_list_append(cbBook_items, "II Timothy");
	cbBook_items = g_list_append(cbBook_items, "Titus");
	cbBook_items = g_list_append(cbBook_items, "Philemon");
	cbBook_items = g_list_append(cbBook_items, "Hebrews");
	cbBook_items = g_list_append(cbBook_items, "James");
	cbBook_items = g_list_append(cbBook_items, "I Peter");
	cbBook_items = g_list_append(cbBook_items, "II Peter");
	cbBook_items = g_list_append(cbBook_items, "I John");
	cbBook_items = g_list_append(cbBook_items, "II John");
	cbBook_items = g_list_append(cbBook_items, "III John");
	cbBook_items = g_list_append(cbBook_items, "Jude");
	cbBook_items = g_list_append(cbBook_items, "Revelation");
	gtk_combo_set_popdown_strings(GTK_COMBO(cbBook), cbBook_items);
	g_list_free(cbBook_items);

	cbeBook = GTK_COMBO(cbBook)->entry;
	gtk_widget_ref(cbeBook);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "cbeBook",
				 cbeBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbeBook);
	gtk_entry_set_text(GTK_ENTRY(cbeBook), "Romans");

	spbChapter_adj = gtk_adjustment_new(8, 0, 151, 1, 10, 10);
	spbChapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbChapter_adj), 1, 0);
	gtk_widget_ref(spbChapter);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "spbChapter",
				 spbChapter,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spbChapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20), spbChapter, NULL,
				  NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spbChapter), TRUE);

	spbVerse_adj = gtk_adjustment_new(28, 0, 180, 1, 10, 10);
	spbVerse = gtk_spin_button_new(GTK_ADJUSTMENT(spbVerse_adj), 1, 0);
	gtk_widget_ref(spbVerse);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "spbVerse",
				 spbVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spbVerse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20), spbVerse, NULL,
				  NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spbVerse), TRUE);

	cbeFreeformLookup = gtk_entry_new();
	gtk_widget_ref(cbeFreeformLookup);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "cbeFreeformLookup", cbeFreeformLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbeFreeformLookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20),
				  cbeFreeformLookup, NULL, NULL);
	gtk_widget_set_usize(cbeFreeformLookup, 190, -2);
	gtk_entry_set_text(GTK_ENTRY(cbeFreeformLookup), "Romans 8:28");

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	btnLookup =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Goto verse",
				       "Go to verse in free form lookup and add verse to history",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLookup);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnLookup",
				 btnLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLookup);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_BACK);
	btnBack =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Back",
				       "Go backward through history list",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnBack);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnBack",
				 btnBack,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnBack);
	gtk_widget_set_sensitive(btnBack, FALSE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_FORWARD);
	btnFoward =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Foward",
				       "Go foward through history list",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnFoward);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnFoward",
				 btnFoward,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnFoward);
	gtk_widget_set_sensitive(btnFoward, FALSE);

	vseparator13 = gtk_vseparator_new();
	gtk_widget_ref(vseparator13);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vseparator13",
				 vseparator13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator13);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar20), vseparator13,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator13, 7, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_EXIT);
	btnExit =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar20),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Exit", "Exit GnomeSword", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnExit);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnExit",
				 btnExit,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnExit);

//---------------------------------------------------------------------------------------------------
	hbox25 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox25);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "hbox25", hbox25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox25);
	gnome_app_set_contents(GNOME_APP(mainwindow), hbox25);
	
        epaned = e_hpaned_new ();
        gtk_widget_ref (epaned);
        gtk_object_set_data_full (GTK_OBJECT (mainwindow), "epaned", epaned,
                            (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (epaned);
        gtk_box_pack_start (GTK_BOX (hbox25), epaned, TRUE, TRUE, 0);
        e_paned_set_position (E_PANED(epaned), 120);
  
	shortcut_model = e_shortcut_model_new();

	shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(shortcut_bar),
				 shortcut_model);
	gtk_widget_set_usize(shortcut_bar, 150, 250);
	gtk_widget_show(shortcut_bar);

        e_paned_pack1(E_PANED (epaned), shortcut_bar, FALSE, TRUE);

	e_shortcut_bar_set_icon_callback(E_SHORTCUT_BAR(shortcut_bar),
					 icon_callback, NULL);

#if 0
	gtk_container_set_border_width(GTK_CONTAINER(shortcut_bar), 4);
#endif

	mainPanel = gtk_vbox_new(FALSE, 0);
	e_paned_pack2(E_PANED (epaned), mainPanel, TRUE, TRUE);
	//gtk_box_pack_start(GTK_BOX(hbox25), mainPanel, TRUE, TRUE, 0);
	gtk_widget_show(mainPanel);
//-----------------------------------------------------------------------------------------------------
	vpaned1 = e_vpaned_new();
	gtk_widget_ref(vpaned1);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vpaned1",
				 vpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vpaned1);
	gtk_box_pack_start(GTK_BOX(mainPanel), vpaned1, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vpaned1), 1);
	e_paned_set_position(E_PANED(vpaned1), 236);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "hbox2", hbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	e_paned_pack1(E_PANED(vpaned1), hbox2, TRUE, TRUE);

	hpaned1 = e_hpaned_new();
	gtk_widget_ref(hpaned1);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "hpaned1",
				 hpaned1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned1);
	gtk_box_pack_end(GTK_BOX(hbox2), hpaned1, TRUE, TRUE, 0);

	frame9 = gtk_frame_new("KJV");
	gtk_widget_ref(frame9);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "frame9", frame9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame9);
	e_paned_pack1(E_PANED(hpaned1), frame9, FALSE, TRUE);
	gtk_widget_set_usize(frame9, 325, -2);
	gtk_container_set_border_width(GTK_CONTAINER(frame9), 2);

	vbox19 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox19);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vbox19", vbox19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox19);
	gtk_container_add(GTK_CONTAINER(frame9), vbox19);

	scrolledwindow18 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow18);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
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
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "moduleText",
				 moduleText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(moduleText);
	gtk_container_add(GTK_CONTAINER(scrolledwindow18), moduleText);
/*
	frame19 = gtk_frame_new(NULL);
	gtk_widget_ref(frame19);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "frame19",
				 frame19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame19);
	gtk_box_pack_start(GTK_BOX(vbox19), frame19, FALSE, FALSE, 0);

	lbText = gtk_label_new("[KJV]");
	gtk_widget_ref(lbText);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "lbText", lbText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbText);
	gtk_container_add(GTK_CONTAINER(frame19), lbText);
	gtk_misc_set_padding(GTK_MISC(lbText), 1, 1);
*/
	notebook3 = gtk_notebook_new();
	gtk_widget_ref(notebook3);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "notebook3",
				 notebook3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook3);
	e_paned_pack2(E_PANED(hpaned1), notebook3, TRUE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(notebook3), 2);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook3), TRUE);

	vbox22 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox22);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vbox22", vbox22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox22);
	gtk_container_add(GTK_CONTAINER(notebook3), vbox22);

	notebook1 = gtk_notebook_new();
	gtk_widget_ref(notebook1);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "notebook1",
				 notebook1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook1);
	gtk_box_pack_start(GTK_BOX(vbox22), notebook1, FALSE, TRUE, 0);
	gtk_widget_set_usize(notebook1, -2, 31);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook1), TRUE);

	hbox18 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox18);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "hbox18", hbox18,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox18);
	gtk_box_pack_start(GTK_BOX(vbox22), hbox18, TRUE, TRUE, 0);

	framecom = gtk_frame_new("Commentary");
	gtk_widget_ref(framecom);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "framecom",
				 framecom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(framecom);
	gtk_box_pack_start(GTK_BOX(hbox18), framecom, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(framecom), 2);

	scrolledwindow28 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow28);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "scrolledwindow28", scrolledwindow28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow28);
	gtk_container_add(GTK_CONTAINER(framecom), scrolledwindow28);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow28),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	textCommentaries = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textCommentaries);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "textCommentaries", textCommentaries,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textCommentaries);
	gtk_container_add(GTK_CONTAINER(scrolledwindow28),
			  textCommentaries);

	handlebox17 = gtk_handle_box_new();
	gtk_widget_ref(handlebox17);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "handlebox17",
				 handlebox17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(hbox18), handlebox17, FALSE, FALSE, 0);
	gtk_handle_box_set_handle_position(GTK_HANDLE_BOX(handlebox17),
					   GTK_POS_TOP);

	toolbar26 =
	    gtk_toolbar_new(GTK_ORIENTATION_VERTICAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar26);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "toolbar26",
				 toolbar26,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar26);
	gtk_container_add(GTK_CONTAINER(handlebox17), toolbar26);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_UP);
	btnComPrev =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar26),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button2", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnComPrev);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnComPrev",
				 btnComPrev,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnComPrev);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_DOWN);
	btnComNext =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar26),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button3", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnComNext);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnComNext",
				 btnComNext,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnComNext);

	label64 = gtk_label_new("Commentaries");
	gtk_widget_ref(label64);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "label64",
				 label64,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label64);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     0), label64);

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_container_add(GTK_CONTAINER(notebook3), vbox2);

	hbox11 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox11);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "hbox11", hbox11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox11);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox11, TRUE, TRUE, 0);

	vbox8 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox8);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vbox8", vbox8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox8);
	gtk_box_pack_start(GTK_BOX(hbox11), vbox8, TRUE, TRUE, 0);

	scrolledwindow11 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow11);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "scrolledwindow11", scrolledwindow11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow11);
	gtk_box_pack_start(GTK_BOX(vbox8), scrolledwindow11, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow11),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	textComments = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textComments);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "textComments",
				 textComments,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComments);
	gtk_container_add(GTK_CONTAINER(scrolledwindow11), textComments);

	sbNotes = gtk_statusbar_new();
	gtk_widget_ref(sbNotes);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "sbNotes",
				 sbNotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(vbox8), sbNotes, FALSE, FALSE, 0);

	handlebox16 = gtk_handle_box_new();
	gtk_widget_ref(handlebox16);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "handlebox16",
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
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "tbNotes",
				 tbNotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tbNotes);
	gtk_container_add(GTK_CONTAINER(handlebox16), tbNotes);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(tbNotes),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	tbtnFollow =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, "togglebutton1",
				       "Follow Main Text Window", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(tbtnFollow);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "tbtnFollow",
				 tbtnFollow,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tbtnFollow);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tbtnFollow), TRUE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_PROPERTIES);
	btnEditNote =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, "Edit", "Edit note", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEditNote);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnEditNote",
				 btnEditNote,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEditNote);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_ADD);
	btnSaveNote =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Save", "Save note", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSaveNote);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnSaveNote",
				 btnSaveNote,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSaveNote);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_REMOVE);
	btnDeleteNote =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Delete", "Delete note", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnDeleteNote);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnDeleteNote",
				 btnDeleteNote,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnDeleteNote);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	btnSpellNotes =
	    gtk_toolbar_append_element(GTK_TOOLBAR(tbNotes),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button1", "Start Spell Check",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSpellNotes);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnSpellNotes",
				 btnSpellNotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSpellNotes);

	label85 = gtk_label_new("Personal Comments");
	gtk_widget_ref(label85);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "label85",
				 label85,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label85);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     1), label85);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vbox3", vbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(notebook3), vbox3);

	frame2 = gtk_frame_new(NULL);
	gtk_widget_ref(frame2);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "frame2", frame2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox3), frame2, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame2, -2, 67);

	vbox4 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox4);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vbox4", vbox4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(frame2), vbox4);

	scrolledwindow15 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow15);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "scrolledwindow15", scrolledwindow15,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow15);
	gtk_box_pack_start(GTK_BOX(vbox4), scrolledwindow15, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow15),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	textComp1 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textComp1);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "textComp1",
				 textComp1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp1);
	gtk_container_add(GTK_CONTAINER(scrolledwindow15), textComp1);

	frame17 = gtk_frame_new(NULL);
	gtk_widget_ref(frame17);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "frame17",
				 frame17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame17);
	gtk_box_pack_start(GTK_BOX(vbox3), frame17, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame17, -2, 73);

	scrolledwindow16 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow16);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "scrolledwindow16", scrolledwindow16,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow16);
	gtk_container_add(GTK_CONTAINER(frame17), scrolledwindow16);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow16),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	textComp2 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textComp2);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "textComp2",
				 textComp2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp2);
	gtk_container_add(GTK_CONTAINER(scrolledwindow16), textComp2);

	frame18 = gtk_frame_new(NULL);
	gtk_widget_ref(frame18);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "frame18",
				 frame18,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame18);
	gtk_box_pack_start(GTK_BOX(vbox3), frame18, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame18, -2, 76);

	scrolledwindow21 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow21);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "scrolledwindow21", scrolledwindow21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow21);
	gtk_container_add(GTK_CONTAINER(frame18), scrolledwindow21);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow21),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	textComp3 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textComp3);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "textComp3",
				 textComp3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp3);
	gtk_container_add(GTK_CONTAINER(scrolledwindow21), textComp3);

	label12 = gtk_label_new("Interlinear");
	gtk_widget_ref(label12);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "label12",
				 label12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label12);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     2), label12);

	frame12 = gtk_frame_new(NULL);
	gtk_widget_ref(frame12);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "frame12",
				 frame12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame12);
	gtk_container_add(GTK_CONTAINER(notebook3), frame12);

	vbox6 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox6);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vbox6", vbox6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox6);
	gtk_container_add(GTK_CONTAINER(frame12), vbox6);

	handlebox10 = gtk_handle_box_new();
	gtk_widget_ref(handlebox10);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "handlebox10",
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
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "toolbar5",
				 toolbar5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar5);
	gtk_container_add(GTK_CONTAINER(handlebox10), toolbar5);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar5),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_NEW);
	btnSPnew =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button1", "Start new document",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSPnew);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnSPnew",
				 btnSPnew,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSPnew);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_OPEN);
	btnOpenFile =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Open", "Open File", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnOpenFile);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnOpenFile",
				 btnOpenFile,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnOpenFile);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_SAVE);
	btnSaveFile =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Save", "Save", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSaveFile);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnSaveFile",
				 btnSaveFile,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSaveFile);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_SAVE_AS);
	btnSaveFileAs =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Save As", "Save as", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSaveFileAs);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnSaveFileAs",
				 btnSaveFileAs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSaveFileAs);

	vseparator9 = gtk_vseparator_new();
	gtk_widget_ref(vseparator9);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vseparator9",
				 vseparator9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator9);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar5), vseparator9, NULL,
				  NULL);
	gtk_widget_set_usize(vseparator9, 7, 17);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_PRINT);
	btnPrint =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Print", "Print ", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnPrint);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnPrint",
				 btnPrint,
				 (GtkDestroyNotify) gtk_widget_unref);

	vseparator10 = gtk_vseparator_new();
	gtk_widget_ref(vseparator10);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vseparator10",
				 vseparator10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator10);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar5), vseparator10,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator10, 7, 17);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_CUT);
	btnCut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Cut", "Cut to clipboard", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnCut);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnCut", btnCut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnCut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_COPY);
	btnCopy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Copy", "Copy to clipboard", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnCopy);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnCopy",
				 btnCopy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnCopy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_PASTE);
	btnPaste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Paste", "Paste from clipborad",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnPaste);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnPaste",
				 btnPaste,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPaste);

	vseparator11 = gtk_vseparator_new();
	gtk_widget_ref(vseparator11);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vseparator11",
				 vseparator11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator11);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar5), vseparator11,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator11, -2, 17);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	btnSpell =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar5),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button8", "Start Spell Check",
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSpell);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnSpell",
				 btnSpell,
				 (GtkDestroyNotify) gtk_widget_unref);

	scrolledwindow17 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow17);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
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
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "text3", text3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(text3);
	gtk_container_add(GTK_CONTAINER(scrolledwindow17), text3);
	gtk_text_set_editable(GTK_TEXT(text3), TRUE);

	statusbar2 = gtk_statusbar_new();
	gtk_widget_ref(statusbar2);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "statusbar2",
				 statusbar2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(statusbar2);
	gtk_box_pack_start(GTK_BOX(vbox6), statusbar2, FALSE, TRUE, 0);

	label41 = gtk_label_new("Study Pad");
	gtk_widget_ref(label41);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "label41",
				 label41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label41);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook3),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook3),
							     3), label41);

	hbox8 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox8);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "hbox8", hbox8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox8);
	e_paned_pack2(E_PANED(vpaned1), hbox8, TRUE, TRUE);
	gtk_widget_set_usize(hbox8, -2, 115);

	frame10 = gtk_frame_new("Dictionaries - Lexicons");
	gtk_widget_ref(frame10);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "frame10",
				 frame10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame10);
	gtk_box_pack_start(GTK_BOX(hbox8), frame10, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame10, -2, 205);
	gtk_container_set_border_width(GTK_CONTAINER(frame10), 2);

	hbox13 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox13);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "hbox13", hbox13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox13);
	gtk_container_add(GTK_CONTAINER(frame10), hbox13);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vbox1", vbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_box_pack_start(GTK_BOX(hbox13), vbox1, TRUE, TRUE, 0);
	gtk_widget_set_usize(vbox1, 427, -2);

	notebook4 = gtk_notebook_new();
	gtk_widget_ref(notebook4);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "notebook4",
				 notebook4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook4);
	gtk_box_pack_start(GTK_BOX(vbox1), notebook4, FALSE, TRUE, 0);
	gtk_widget_set_usize(notebook4, -2, 31);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook4), TRUE);

	scrolledwindow8 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow8);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "scrolledwindow8",
				 scrolledwindow8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow8);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow8, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow8), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	textDict = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textDict);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "textDict",
				 textDict,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textDict);
	gtk_container_add(GTK_CONTAINER(scrolledwindow8), textDict);

	vbox14 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox14);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "vbox14", vbox14,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox14);
	gtk_box_pack_start(GTK_BOX(hbox13), vbox14, FALSE, TRUE, 0);
	gtk_widget_set_usize(vbox14, 208, -2);

	hbox19 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox19);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "hbox19", hbox19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox19);
	gtk_box_pack_start(GTK_BOX(vbox14), hbox19, FALSE, FALSE, 0);

	toolbar25 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar25);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "toolbar25",
				 toolbar25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar25);
	gtk_box_pack_start(GTK_BOX(hbox19), toolbar25, TRUE, TRUE, 0);
	gtk_widget_set_usize(toolbar25, 132, -2);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar25),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow, GNOME_STOCK_PIXMAP_BACK);
	btnKeyPrev =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar25),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button2", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnKeyPrev);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnKeyPrev",
				 btnKeyPrev,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnKeyPrev);

	dictionarySearchText = gtk_entry_new();
	gtk_widget_ref(dictionarySearchText);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow),
				 "dictionarySearchText",
				 dictionarySearchText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dictionarySearchText);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar25),
				  dictionarySearchText, NULL, NULL);
	gtk_widget_set_usize(dictionarySearchText, 145, -2);
	gtk_entry_set_text(GTK_ENTRY(dictionarySearchText), "GRACE");

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(mainwindow,
				      GNOME_STOCK_PIXMAP_FORWARD);
	btnKeyNext =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar25),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button3", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnKeyNext);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "btnKeyNext",
				 btnKeyNext,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnKeyNext);

	list1 = gtk_clist_new(1);
	gtk_widget_ref(list1);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "list1", list1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(list1);
	gtk_box_pack_start(GTK_BOX(vbox14), list1, TRUE, TRUE, 0);
	gtk_widget_set_usize(list1, 126, -2);
	gtk_clist_set_column_width(GTK_CLIST(list1), 0, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(list1));

	label58 = gtk_label_new("label58");
	gtk_widget_ref(label58);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "label58",
				 label58,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label58);
	gtk_clist_set_column_widget(GTK_CLIST(list1), 0, label58);

	appbar1 = gnome_appbar_new(FALSE, TRUE, GNOME_PREFERENCES_NEVER);
	gtk_widget_ref(appbar1);
	gtk_object_set_data_full(GTK_OBJECT(mainwindow), "appbar1",
				 appbar1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(appbar1);
	gnome_app_set_statusbar(GNOME_APP(mainwindow), appbar1);

	gtk_signal_connect(GTK_OBJECT(mainwindow), "destroy",
			   GTK_SIGNAL_FUNC(on_mainwindow_destroy), NULL);
	gnome_app_install_menu_hints(GNOME_APP(mainwindow),
				     menubar1_uiinfo);
	gtk_signal_connect(GTK_OBJECT(btnSearch), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSearch_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnStrongs), "toggled",
			   GTK_SIGNAL_FUNC(on_btnStrongs_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSB), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSB_clicked), NULL);
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
	gtk_signal_connect(GTK_OBJECT(cbeFreeformLookup), "drag_drop",
			   GTK_SIGNAL_FUNC(on_cbeFreeformLookup_drag_drop),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnLookup), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLookup_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnBack), "clicked",
			   GTK_SIGNAL_FUNC(on_btnBack_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnFoward), "clicked",
			   GTK_SIGNAL_FUNC(on_btnFoward_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnExit), "clicked",
			   GTK_SIGNAL_FUNC(on_btnExit_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(moduleText), "button_press_event",
			   GTK_SIGNAL_FUNC
			   (on_moduleText_button_press_event), NULL);
	gtk_signal_connect(GTK_OBJECT(moduleText), "drag_begin",
			   GTK_SIGNAL_FUNC(on_moduleText_drag_begin),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(moduleText), "enter_notify_event",
			   GTK_SIGNAL_FUNC
			   (on_moduleText_enter_notify_event), NULL);
	gtk_signal_connect(GTK_OBJECT(notebook3), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook3_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(notebook1), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook1_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(textCommentaries), "changed",
			   GTK_SIGNAL_FUNC(on_textComments_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(textCommentaries),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_textComments_button_release_event), NULL);
	gtk_signal_connect(GTK_OBJECT(textCommentaries), "drag_begin",
			   GTK_SIGNAL_FUNC(on_textCommentaries_drag_begin),
			   NULL);
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
	gtk_signal_connect(GTK_OBJECT(textComments), "drag_begin",
			   GTK_SIGNAL_FUNC(on_textComments_drag_begin),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(textComments), "drag_data_get",
			   GTK_SIGNAL_FUNC(on_textComments_drag_data_get),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(textComments), "drag_data_received",
			   GTK_SIGNAL_FUNC
			   (on_textComments_drag_data_received), NULL);
	gtk_signal_connect(GTK_OBJECT(textComments), "drag_drop",
			   GTK_SIGNAL_FUNC(on_textComments_drag_drop),
			   NULL);
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
	gtk_signal_connect(GTK_OBJECT(btnSpellNotes), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSpellNotes_clicked),
			   NULL);

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
	gtk_signal_connect(GTK_OBJECT(btnSpell), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSpell_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(text3), "changed",
			   GTK_SIGNAL_FUNC(on_text3_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(notebook4), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook4_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(textDict), "drag_begin",
			   GTK_SIGNAL_FUNC(on_textDict_drag_begin), NULL);
	gtk_signal_connect (GTK_OBJECT (textDict), "button_press_event",
                           GTK_SIGNAL_FUNC (on_textDict_button_press_event),
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
	  gtk_signal_connect (GTK_OBJECT (epaned), "button_release_event",
                      GTK_SIGNAL_FUNC (on_epaned_button_release_event),
                      NULL);

//-------------------------------------------------------------------------------------------
	gtk_widget_pop_visual();
	gtk_widget_pop_colormap();

	/* Load our default icons. */
	for (i = 0; i < NUM_SHORTCUT_TYPES; i++) {
		pathname = gnome_pixmap_file(icon_filenames[i]);
		if (pathname)
			icon_pixbufs[i] =
			    gdk_pixbuf_new_from_file(pathname);
		else
			icon_pixbufs[i] = NULL;
	}
	gtk_signal_connect(GTK_OBJECT(shortcut_bar), "item_selected",
			   GTK_SIGNAL_FUNC(on_shortcut_bar_item_selected),
			   NULL);
//-------------------------------------------------------------------------------------------
	gtk_widget_grab_focus(mainwindow);
	return mainwindow;
}

GtkWidget *create_about2(void)
{
	const gchar *authors[] = {
		"\t\t\t  Terry Biggs    tbiggs@users.sourceforge.net",
		NULL
	};
	GtkWidget *about2;

	about2 = gnome_about_new("gnomesword", VERSION,
				 "                Copyright                 2000",
				 authors,
				 "This is a faithful saying,and worthy of all acceptation, that Christ Jesus came into the world to save sinners; of whom I am chief. - I Timothy 1:15",
				 "gnomesword/GnomeSword.png");
	gtk_object_set_data(GTK_OBJECT(about2), "about2", about2);
	gtk_container_set_border_width(GTK_CONTAINER(about2), 4);
	gtk_window_set_modal(GTK_WINDOW(about2), TRUE);

	return about2;
}


GtkWidget *create_InfoBox(void)
{
	GtkWidget *InfoBox;
	GtkWidget *dialog_vbox6;
	GtkWidget *hbox20;
	GtkWidget *pixmap2;
	GtkWidget *vbox24;
	GtkWidget *lbInfoBox1;
	GtkWidget *lbInfoBox2;
	GtkWidget *lbInfoBox3;
	GtkWidget *dialog_action_area6;
	GtkWidget *btnInfoBoxYes;
	GtkWidget *btnInfoBoxNo;

	InfoBox = gnome_dialog_new("GnomeSword - InfoBox", NULL);
	gtk_object_set_data(GTK_OBJECT(InfoBox), "InfoBox", InfoBox);
	gtk_widget_set_usize(InfoBox, 216, -2);
	gtk_container_set_border_width(GTK_CONTAINER(InfoBox), 4);
	gtk_window_set_modal(GTK_WINDOW(InfoBox), TRUE);

	dialog_vbox6 = GNOME_DIALOG(InfoBox)->vbox;
	gtk_object_set_data(GTK_OBJECT(InfoBox), "dialog_vbox6",
			    dialog_vbox6);
	gtk_widget_show(dialog_vbox6);

	hbox20 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox20);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "hbox20", hbox20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox20);
	gtk_box_pack_start(GTK_BOX(dialog_vbox6), hbox20, TRUE, TRUE, 0);

	pixmap2 =
	    create_pixmap(InfoBox, "gnomesword/GnomeSword.xpm", FALSE);
	gtk_widget_ref(pixmap2);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "pixmap2", pixmap2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap2);
	gtk_box_pack_start(GTK_BOX(hbox20), pixmap2, TRUE, TRUE, 0);

	vbox24 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox24);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "vbox24", vbox24,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox24);
	gtk_box_pack_start(GTK_BOX(hbox20), vbox24, TRUE, TRUE, 0);

	lbInfoBox1 = gtk_label_new("File");
	gtk_widget_ref(lbInfoBox1);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "lbInfoBox1",
				 lbInfoBox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbInfoBox1);
	gtk_box_pack_start(GTK_BOX(vbox24), lbInfoBox1, FALSE, FALSE, 0);

	lbInfoBox2 = gtk_label_new("in StudyPad is not saved!");
	gtk_widget_ref(lbInfoBox2);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "lbInfoBox2",
				 lbInfoBox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbInfoBox2);
	gtk_box_pack_start(GTK_BOX(vbox24), lbInfoBox2, FALSE, FALSE, 0);

	lbInfoBox3 = gtk_label_new("Shall I save it?");
	gtk_widget_ref(lbInfoBox3);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "lbInfoBox3",
				 lbInfoBox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbInfoBox3);
	gtk_box_pack_start(GTK_BOX(vbox24), lbInfoBox3, FALSE, FALSE, 0);

	dialog_action_area6 = GNOME_DIALOG(InfoBox)->action_area;
	gtk_object_set_data(GTK_OBJECT(InfoBox), "dialog_action_area6",
			    dialog_action_area6);
	gtk_widget_show(dialog_action_area6);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area6),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area6), 8);

	gnome_dialog_append_button(GNOME_DIALOG(InfoBox),
				   GNOME_STOCK_BUTTON_YES);
	btnInfoBoxYes = g_list_last(GNOME_DIALOG(InfoBox)->buttons)->data;
	gtk_widget_ref(btnInfoBoxYes);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "btnInfoBoxYes",
				 btnInfoBoxYes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnInfoBoxYes);
	GTK_WIDGET_SET_FLAGS(btnInfoBoxYes, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(InfoBox),
				   GNOME_STOCK_BUTTON_NO);
	btnInfoBoxNo = g_list_last(GNOME_DIALOG(InfoBox)->buttons)->data;
	gtk_widget_ref(btnInfoBoxNo);
	gtk_object_set_data_full(GTK_OBJECT(InfoBox), "btnInfoBoxNo",
				 btnInfoBoxNo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnInfoBoxNo);
	GTK_WIDGET_SET_FLAGS(btnInfoBoxNo, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(btnInfoBoxYes), "clicked",
			   GTK_SIGNAL_FUNC(on_btnInfoBoxYes_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnInfoBoxNo), "clicked",
			   GTK_SIGNAL_FUNC(on_btnInfoBoxNo_clicked), NULL);

	return InfoBox;
}

GtkWidget *create_AboutSword(void)
{
	GtkWidget *AboutSword;
	GtkWidget *dialog_vbox7;
	GtkWidget *vbox21;
	GtkWidget *pixmap1;
	GtkWidget *scrolledwindow26;
	GtkWidget *txtAboutSword;
	GtkWidget *label95;
	GtkWidget *href1;
	GtkWidget *label96;
	GtkWidget *text6;
	GtkWidget *dialog_action_area7;
	GtkWidget *btnAboutSwordOK;

	AboutSword = gnome_dialog_new("About The Sword Project", NULL);
	gtk_object_set_data(GTK_OBJECT(AboutSword), "AboutSword",
			    AboutSword);
	gtk_container_set_border_width(GTK_CONTAINER(AboutSword), 4);
	gtk_window_set_policy(GTK_WINDOW(AboutSword), FALSE, FALSE, FALSE);

	dialog_vbox7 = GNOME_DIALOG(AboutSword)->vbox;
	gtk_object_set_data(GTK_OBJECT(AboutSword), "dialog_vbox7",
			    dialog_vbox7);
	gtk_widget_show(dialog_vbox7);

	vbox21 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox21);
	gtk_object_set_data_full(GTK_OBJECT(AboutSword), "vbox21", vbox21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox21);
	gtk_box_pack_start(GTK_BOX(dialog_vbox7), vbox21, TRUE, TRUE, 0);

	pixmap1 = create_pixmap(AboutSword, "gnomesword/sword.xpm", FALSE);
	gtk_widget_ref(pixmap1);
	gtk_object_set_data_full(GTK_OBJECT(AboutSword), "pixmap1",
				 pixmap1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap1);
	gtk_box_pack_start(GTK_BOX(vbox21), pixmap1, FALSE, FALSE, 0);
	gtk_misc_set_padding(GTK_MISC(pixmap1), 4, 4);

	scrolledwindow26 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow26);
	gtk_object_set_data_full(GTK_OBJECT(AboutSword),
				 "scrolledwindow26", scrolledwindow26,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow26);
	gtk_box_pack_start(GTK_BOX(vbox21), scrolledwindow26, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow26),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	txtAboutSword = gtk_text_new(NULL, NULL);
	gtk_widget_ref(txtAboutSword);
	gtk_object_set_data_full(GTK_OBJECT(AboutSword), "txtAboutSword",
				 txtAboutSword,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(txtAboutSword);
	gtk_container_add(GTK_CONTAINER(scrolledwindow26), txtAboutSword);
	gtk_widget_set_usize(txtAboutSword, -2, 103);
	gtk_text_insert(GTK_TEXT(txtAboutSword), NULL, NULL, NULL,
			"The SWORD Project is an effort to create an ever expanding software package for research and study of God and His Word.  The SWORD Bible Framework allows easy manipulation of Bible texts, commentaries, lexicons, dictionaries, etc.  Many frontends are build using this framework.  An installed module set may be shared between any frontend using the framework.",
			359);

	label95 =
	    gtk_label_new
	    ("Modules can be downloaded from the Sword Project ");
	gtk_widget_ref(label95);
	gtk_object_set_data_full(GTK_OBJECT(AboutSword), "label95",
				 label95,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label95);
	gtk_box_pack_start(GTK_BOX(vbox21), label95, FALSE, FALSE, 0);
	gtk_widget_set_usize(label95, -2, 24);

	href1 = gnome_href_new("www.crosswire.org", "The Sword Project");
	gtk_widget_ref(href1);
	gtk_object_set_data_full(GTK_OBJECT(AboutSword), "href1", href1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(href1);
	gtk_box_pack_start(GTK_BOX(vbox21), href1, FALSE, FALSE, 0);

	label96 = gtk_label_new("Current Module");
	gtk_widget_ref(label96);
	gtk_object_set_data_full(GTK_OBJECT(AboutSword), "label96",
				 label96,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label96);
	gtk_box_pack_start(GTK_BOX(vbox21), label96, FALSE, FALSE, 0);
	gtk_widget_set_usize(label96, -2, 19);

	text6 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(text6);
	gtk_object_set_data_full(GTK_OBJECT(AboutSword), "text6", text6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(text6);
	gtk_box_pack_start(GTK_BOX(vbox21), text6, TRUE, TRUE, 0);

	dialog_action_area7 = GNOME_DIALOG(AboutSword)->action_area;
	gtk_object_set_data(GTK_OBJECT(AboutSword), "dialog_action_area7",
			    dialog_action_area7);
	gtk_widget_show(dialog_action_area7);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area7),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area7), 8);

	gnome_dialog_append_button(GNOME_DIALOG(AboutSword),
				   GNOME_STOCK_BUTTON_OK);
	btnAboutSwordOK =
	    g_list_last(GNOME_DIALOG(AboutSword)->buttons)->data;
	gtk_widget_ref(btnAboutSwordOK);
	gtk_object_set_data_full(GTK_OBJECT(AboutSword), "btnAboutSwordOK",
				 btnAboutSwordOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnAboutSwordOK);
	GTK_WIDGET_SET_FLAGS(btnAboutSwordOK, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(btnAboutSwordOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnAboutSwordOK_clicked),
			   NULL);

	return AboutSword;
}

GtkWidget *create_aboutmodules(void)
{
	GtkWidget *aboutmodules;
	GtkWidget *dialog_vbox8;
	GtkWidget *vbox25;
	GtkWidget *hbox21;
	GtkWidget *pixmap3;
	GtkWidget *label97;
	GtkWidget *pixmap4;
	GtkWidget *lbModName;
	GtkWidget *scrolledwindow30;
	GtkWidget *textModAbout;
	GtkWidget *dialog_action_area8;
	GtkWidget *btnAboutModuleOK;

	aboutmodules = gnome_dialog_new("About Sword Modules", NULL);
	gtk_object_set_data(GTK_OBJECT(aboutmodules), "aboutmodules",
			    aboutmodules);
	gtk_container_set_border_width(GTK_CONTAINER(aboutmodules), 4);
	gtk_window_set_policy(GTK_WINDOW(aboutmodules), FALSE, FALSE,
			      FALSE);

	dialog_vbox8 = GNOME_DIALOG(aboutmodules)->vbox;
	gtk_object_set_data(GTK_OBJECT(aboutmodules), "dialog_vbox8",
			    dialog_vbox8);
	gtk_widget_show(dialog_vbox8);

	vbox25 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox25);
	gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "vbox25",
				 vbox25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox25);
	gtk_box_pack_start(GTK_BOX(dialog_vbox8), vbox25, TRUE, TRUE, 0);

	hbox21 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox21);
	gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "hbox21",
				 hbox21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox21);
	gtk_box_pack_start(GTK_BOX(vbox25), hbox21, TRUE, TRUE, 0);

	pixmap3 =
	    create_pixmap(aboutmodules, "gnomesword/sword.xpm", FALSE);
	gtk_widget_ref(pixmap3);
	gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "pixmap3",
				 pixmap3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap3);
	gtk_box_pack_start(GTK_BOX(hbox21), pixmap3, TRUE, TRUE, 0);

	label97 = gtk_label_new("The Sword Project");
	gtk_widget_ref(label97);
	gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "label97",
				 label97,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label97);
	gtk_box_pack_start(GTK_BOX(hbox21), label97, FALSE, FALSE, 0);

	pixmap4 =
	    create_pixmap(aboutmodules, "gnomesword/GnomeSword.xpm",
			  FALSE);
	gtk_widget_ref(pixmap4);
	gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "pixmap4",
				 pixmap4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap4);
	gtk_box_pack_start(GTK_BOX(hbox21), pixmap4, TRUE, TRUE, 0);

	lbModName = gtk_label_new("KJV");
	gtk_widget_ref(lbModName);
	gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "lbModName",
				 lbModName,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbModName);
	gtk_box_pack_start(GTK_BOX(vbox25), lbModName, FALSE, FALSE, 0);
	gtk_widget_set_usize(lbModName, -2, 21);

	scrolledwindow30 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow30);
	gtk_object_set_data_full(GTK_OBJECT(aboutmodules),
				 "scrolledwindow30", scrolledwindow30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow30);
	gtk_box_pack_start(GTK_BOX(vbox25), scrolledwindow30, TRUE, TRUE,
			   0);
	gtk_widget_set_usize(scrolledwindow30, 304, 183);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow30),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);

	textModAbout = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textModAbout);
	gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "textModAbout",
				 textModAbout,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textModAbout);
	gtk_container_add(GTK_CONTAINER(scrolledwindow30), textModAbout);

	dialog_action_area8 = GNOME_DIALOG(aboutmodules)->action_area;
	gtk_object_set_data(GTK_OBJECT(aboutmodules),
			    "dialog_action_area8", dialog_action_area8);
	gtk_widget_show(dialog_action_area8);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area8),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area8), 8);

	gnome_dialog_append_button(GNOME_DIALOG(aboutmodules),
				   GNOME_STOCK_BUTTON_OK);
	btnAboutModuleOK =
	    g_list_last(GNOME_DIALOG(aboutmodules)->buttons)->data;
	gtk_widget_ref(btnAboutModuleOK);
	gtk_object_set_data_full(GTK_OBJECT(aboutmodules),
				 "btnAboutModuleOK", btnAboutModuleOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnAboutModuleOK);
	GTK_WIDGET_SET_FLAGS(btnAboutModuleOK, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(btnAboutModuleOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnAboutModuleOK_clicked),
			   NULL);

	return aboutmodules;
}

GtkWidget *create_dlgSettings(void)
{
	GtkWidget *dlgSettings;
	GtkWidget *dialog_vbox9;
	GtkWidget *hbox22;
	GtkWidget *pixmap5;
	GtkWidget *notebook7;
	GtkWidget *hbox23;
	GtkWidget *label103;
	GtkWidget *cpfgCurrentverse;
	GtkWidget *label98;
	GtkWidget *vbox28;
	GtkWidget *cbtnShowSCB;
	GtkWidget *cbtnShowCOMtabs;
	GtkWidget *cbtnShowDLtabs;
	GtkWidget *label123;
	GtkWidget *vbox29;
	GtkWidget *cbtnShowTextgroup;
	GtkWidget *cbtnShowComGroup;
	GtkWidget *cbtnShowDictGroup;
	GtkWidget *cbtnShowHistoryGroup;
	GtkWidget *label124;
	GtkWidget *hbox24;
	GtkWidget *cbtnPNformat;
	GtkWidget *label122;
	GtkWidget *dialog_action_area9;
	GtkWidget *btnPropertyboxOK;
	GtkWidget *btnPropertyboxApply;
	GtkWidget *btnPropertyboxCancel;

	dlgSettings = gnome_dialog_new("GnomeSword - Settings", NULL);
	gtk_object_set_data(GTK_OBJECT(dlgSettings), "dlgSettings",
			    dlgSettings);
	gtk_window_set_policy(GTK_WINDOW(dlgSettings), FALSE, FALSE,
			      FALSE);

	dialog_vbox9 = GNOME_DIALOG(dlgSettings)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgSettings), "dialog_vbox9",
			    dialog_vbox9);
	gtk_widget_show(dialog_vbox9);

	hbox22 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox22);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox22", hbox22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox22);
	gtk_box_pack_start(GTK_BOX(dialog_vbox9), hbox22, TRUE, TRUE, 0);

	pixmap5 =
	    create_pixmap(dlgSettings, "gnomesword/GnomeSword.xpm", FALSE);
	gtk_widget_ref(pixmap5);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "pixmap5",
				 pixmap5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap5);
	gtk_box_pack_start(GTK_BOX(hbox22), pixmap5, TRUE, TRUE, 0);

	notebook7 = gtk_notebook_new();
	gtk_widget_ref(notebook7);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "notebook7",
				 notebook7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook7);
	gtk_box_pack_start(GTK_BOX(hbox22), notebook7, TRUE, TRUE, 0);

	hbox23 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox23);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox23", hbox23,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox23);
	gtk_container_add(GTK_CONTAINER(notebook7), hbox23);

	label103 = gtk_label_new("Current Verse Color");
	gtk_widget_ref(label103);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label103",
				 label103,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label103);
	gtk_box_pack_start(GTK_BOX(hbox23), label103, FALSE, FALSE, 0);
	gtk_widget_set_usize(label103, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label103), GTK_JUSTIFY_LEFT);

	cpfgCurrentverse = gnome_color_picker_new();
	gtk_widget_ref(cpfgCurrentverse);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cpfgCurrentverse", cpfgCurrentverse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cpfgCurrentverse);
	gtk_box_pack_start(GTK_BOX(hbox23), cpfgCurrentverse, TRUE, FALSE,
			   0);

	label98 = gtk_label_new("Main window");
	gtk_widget_ref(label98);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label98",
				 label98,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label98);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     0), label98);

	vbox28 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox28);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox28", vbox28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox28);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox28);

	cbtnShowSCB = gtk_check_button_new_with_label("Show Shortcut bar");
	gtk_widget_ref(cbtnShowSCB);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "cbtnShowSCB",
				 cbtnShowSCB,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowSCB);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowSCB, FALSE, FALSE, 0);

	cbtnShowCOMtabs =
	    gtk_check_button_new_with_label
	    ("Show Commentary notebook tabs");
	gtk_widget_ref(cbtnShowCOMtabs);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowCOMtabs", cbtnShowCOMtabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowCOMtabs);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowCOMtabs, FALSE, FALSE,
			   0);

	cbtnShowDLtabs =
	    gtk_check_button_new_with_label("Show Dict/Lex notebook tabs");
	gtk_widget_ref(cbtnShowDLtabs);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "cbtnShowDLtabs",
				 cbtnShowDLtabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowDLtabs);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowDLtabs, FALSE, FALSE,
			   0);

	label123 = gtk_label_new("Interface");
	gtk_widget_ref(label123);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label123",
				 label123,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label123);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     1), label123);

	vbox29 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox29);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox29", vbox29,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox29);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox29);

	cbtnShowTextgroup =
	    gtk_check_button_new_with_label
	    ("Show Bible Text Group in Shortcut Bar");
	gtk_widget_ref(cbtnShowTextgroup);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowTextgroup", cbtnShowTextgroup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowTextgroup);
	gtk_box_pack_start(GTK_BOX(vbox29), cbtnShowTextgroup, FALSE,
			   FALSE, 0);

  	cbtnShowComGroup = gtk_check_button_new_with_label ("Show Commentary Group in Shortcut Bar");
  	gtk_widget_ref (cbtnShowComGroup);
  	gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowComGroup", cbtnShowComGroup,
                            (GtkDestroyNotify) gtk_widget_unref);
 	 gtk_widget_show (cbtnShowComGroup);
 	 gtk_box_pack_start (GTK_BOX (vbox29), cbtnShowComGroup, FALSE, FALSE, 0);

	  cbtnShowDictGroup = gtk_check_button_new_with_label ("Show Dict/Lex Group in Shortcut bar");
 	 gtk_widget_ref (cbtnShowDictGroup);
 	 gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowDictGroup", cbtnShowDictGroup,
                            (GtkDestroyNotify) gtk_widget_unref);
 	 gtk_widget_show (cbtnShowDictGroup);
 	 gtk_box_pack_start (GTK_BOX (vbox29), cbtnShowDictGroup, FALSE, FALSE, 0);
  
  	cbtnShowHistoryGroup = gtk_check_button_new_with_label ("Show History Group in Shortcut Bar");
  	gtk_widget_ref (cbtnShowHistoryGroup);
  	gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowHistoryGroup", cbtnShowHistoryGroup,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (cbtnShowHistoryGroup);
  	gtk_box_pack_start (GTK_BOX (vbox29), cbtnShowHistoryGroup, FALSE, FALSE, 0);

	label124 = gtk_label_new("Shortcut Bar");
	gtk_widget_ref(label124);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label124",
				 label124,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label124);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     2), label124);

	hbox24 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox24);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox24", hbox24,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox24);
	gtk_container_add(GTK_CONTAINER(notebook7), hbox24);

	cbtnPNformat = gtk_check_button_new_with_label("Use Formatting");
	gtk_widget_ref(cbtnPNformat);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "cbtnPNformat",
				 cbtnPNformat,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnPNformat);
	gtk_box_pack_start(GTK_BOX(hbox24), cbtnPNformat, FALSE, FALSE, 0);

	label122 = gtk_label_new("Personal Notes");
	gtk_widget_ref(label122);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label122",
				 label122,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label122);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     3), label122);

	dialog_action_area9 = GNOME_DIALOG(dlgSettings)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgSettings), "dialog_action_area9",
			    dialog_action_area9);
	gtk_widget_show(dialog_action_area9);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area9),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area9), 8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSettings),
				   GNOME_STOCK_BUTTON_OK);
	btnPropertyboxOK =
	    g_list_last(GNOME_DIALOG(dlgSettings)->buttons)->data;
	gtk_widget_ref(btnPropertyboxOK);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnPropertyboxOK", btnPropertyboxOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPropertyboxOK);
	gtk_widget_set_sensitive(btnPropertyboxOK, FALSE);
	GTK_WIDGET_SET_FLAGS(btnPropertyboxOK, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSettings),
				   GNOME_STOCK_BUTTON_APPLY);
	btnPropertyboxApply =
	    g_list_last(GNOME_DIALOG(dlgSettings)->buttons)->data;
	gtk_widget_ref(btnPropertyboxApply);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnPropertyboxApply",
				 btnPropertyboxApply,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPropertyboxApply);
	gtk_widget_set_sensitive(btnPropertyboxApply, FALSE);
	GTK_WIDGET_SET_FLAGS(btnPropertyboxApply, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSettings),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnPropertyboxCancel =
	    g_list_last(GNOME_DIALOG(dlgSettings)->buttons)->data;
	gtk_widget_ref(btnPropertyboxCancel);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnPropertyboxCancel",
				 btnPropertyboxCancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPropertyboxCancel);
	GTK_WIDGET_SET_FLAGS(btnPropertyboxCancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(cpfgCurrentverse), "color_set",
			   GTK_SIGNAL_FUNC(on_cpfgCurrentverse_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowSCB), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowSCB_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowCOMtabs), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowCOMtabs_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowDLtabs), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowDLtabs_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowTextgroup), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnShowTextgroup_toggled), NULL);
	gtk_signal_connect (GTK_OBJECT (cbtnShowComGroup), "toggled",
	                   GTK_SIGNAL_FUNC (on_cbtnShowComGroup_toggled),
		           NULL);
	gtk_signal_connect (GTK_OBJECT (cbtnShowDictGroup), "toggled",
				GTK_SIGNAL_FUNC (on_cbtnShowDictGroup_toggled),
				NULL);
  	gtk_signal_connect (GTK_OBJECT (cbtnShowHistoryGroup), "toggled",
                      	GTK_SIGNAL_FUNC (on_cbtnShowHistoryGroup_toggled),
                     	 NULL);			
	gtk_signal_connect(GTK_OBJECT(cbtnPNformat), "toggled",
			   GTK_SIGNAL_FUNC(on_cbtnPNformat_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPropertyboxOK_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxApply), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPropertyboxApply_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxCancel), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btnPropertyboxCancel_clicked), NULL);

	return dlgSettings;
}

GtkWidget *create_dlgSetup(void)
{
	GtkWidget *dlgSetup;
	GtkWidget *dialog_vbox10;
	GtkWidget *druid1;
	GtkWidget *druidpagestart1;
	GdkColor druidpagestart1_bg_color = { 0, 6168, 6168, 28527 };
	GdkColor druidpagestart1_textbox_color =
	    { 0, 65535, 65535, 65535 };
	GdkColor druidpagestart1_logo_bg_color = { 0, 6425, 6682, 29298 };
	GdkColor druidpagestart1_title_color = { 0, 65535, 65535, 65535 };
	GtkWidget *druidpagestandard3;
	GdkColor druidpagestandard3_bg_color = { 0, 46260, 7967, 11308 };
	GdkColor druidpagestandard3_logo_bg_color =
	    { 0, 45232, 8481, 11822 };
	GdkColor druidpagestandard3_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkWidget *druid_vbox3;
	GtkWidget *vbox27;
	GtkWidget *label112;
	GtkWidget *label113;
	GtkWidget *label114;
	GtkWidget *label115;
	GtkWidget *label116;
	GtkWidget *label117;
	GtkWidget *label118;
	GtkWidget *label119;
	GtkWidget *label120;
	GtkWidget *label121;
	GtkWidget *druidpagestandard1;
	GdkColor druidpagestandard1_bg_color = { 0, 6168, 6168, 28527 };
	GdkColor druidpagestandard1_logo_bg_color =
	    { 0, 8481, 6425, 31354 };
	GdkColor druidpagestandard1_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkWidget *druid_vbox1;
	GtkWidget *vbox26;
	GSList *vbox26_group = NULL;
	GtkWidget *radiobutton1;
	GtkWidget *radiobutton2;
	GtkWidget *druidpagestandard2;
	GdkColor druidpagestandard2_bg_color = { 0, 46260, 13621, 3341 };
	GdkColor druidpagestandard2_logo_bg_color =
	    { 0, 44204, 17476, 17990 };
	GdkColor druidpagestandard2_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkWidget *druid_vbox2;
	GtkWidget *table7;
	GtkWidget *label104;
	GtkWidget *label105;
	GtkWidget *label106;
	GtkWidget *label107;
	GtkWidget *combo1;
	GtkWidget *combo_entry1;
	GtkWidget *combo2;
	GtkWidget *combo_entry2;
	GtkWidget *combo3;
	GtkWidget *combo_entry3;
	GtkWidget *combo4;
	GtkWidget *combo_entry4;
	GtkWidget *checkbutton1;
	GtkWidget *checkbutton2;
	GtkWidget *checkbutton3;
	GtkWidget *checkbutton4;
	GtkWidget *label111;
	GtkWidget *druidpagefinish1;
	GdkColor druidpagefinish1_bg_color = { 0, 6425, 6425, 28784 };
	GdkColor druidpagefinish1_textbox_color =
	    { 0, 65535, 65535, 65535 };
	GdkColor druidpagefinish1_logo_bg_color =
	    { 0, 65535, 65535, 65535 };
	GdkColor druidpagefinish1_title_color = { 0, 65535, 65535, 65535 };
	GtkWidget *dialog_action_area10;

	dlgSetup = gnome_dialog_new("GnomeSword - Setup", NULL);
	gtk_object_set_data(GTK_OBJECT(dlgSetup), "dlgSetup", dlgSetup);
	gtk_container_set_border_width(GTK_CONTAINER(dlgSetup), 2);
	gtk_window_set_policy(GTK_WINDOW(dlgSetup), FALSE, FALSE, FALSE);

	dialog_vbox10 = GNOME_DIALOG(dlgSetup)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgSetup), "dialog_vbox10",
			    dialog_vbox10);
	gtk_widget_show(dialog_vbox10);

	druid1 = gnome_druid_new();
	gtk_widget_ref(druid1);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "druid1", druid1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid1);
	gtk_box_pack_start(GTK_BOX(dialog_vbox10), druid1, TRUE, TRUE, 0);

	druidpagestart1 = gnome_druid_page_start_new();
	gtk_widget_ref(druidpagestart1);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "druidpagestart1",
				 druidpagestart1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druidpagestart1);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagestart1));
	gnome_druid_set_page(GNOME_DRUID(druid1),
			     GNOME_DRUID_PAGE(druidpagestart1));
	gnome_druid_page_start_set_bg_color(GNOME_DRUID_PAGE_START
					    (druidpagestart1),
					    &druidpagestart1_bg_color);
	gnome_druid_page_start_set_textbox_color(GNOME_DRUID_PAGE_START
						 (druidpagestart1),
						 &druidpagestart1_textbox_color);
	gnome_druid_page_start_set_logo_bg_color(GNOME_DRUID_PAGE_START
						 (druidpagestart1),
						 &druidpagestart1_logo_bg_color);
	gnome_druid_page_start_set_title_color(GNOME_DRUID_PAGE_START
					       (druidpagestart1),
					       &druidpagestart1_title_color);
	gnome_druid_page_start_set_title(GNOME_DRUID_PAGE_START
					 (druidpagestart1),
					 "Welcome To GnomeSword");
	gnome_druid_page_start_set_text(GNOME_DRUID_PAGE_START
					(druidpagestart1),
					"This guide will take you through the initial setup of GnomeSword");
	gnome_druid_page_start_set_logo(GNOME_DRUID_PAGE_START
					(druidpagestart1),
					create_image
					("gnomesword/GnomeSword.xpm"));

	druidpagestandard3 =
	    gnome_druid_page_standard_new_with_vals("", NULL);
	gtk_widget_ref(druidpagestandard3);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup),
				 "druidpagestandard3", druidpagestandard3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show_all(druidpagestandard3);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagestandard3));
	gnome_druid_page_standard_set_bg_color(GNOME_DRUID_PAGE_STANDARD
					       (druidpagestandard3),
					       &druidpagestandard3_bg_color);
	gnome_druid_page_standard_set_logo_bg_color
	    (GNOME_DRUID_PAGE_STANDARD(druidpagestandard3),
	     &druidpagestandard3_logo_bg_color);
	gnome_druid_page_standard_set_title_color(GNOME_DRUID_PAGE_STANDARD
						  (druidpagestandard3),
						  &druidpagestandard3_title_color);
	gnome_druid_page_standard_set_logo(GNOME_DRUID_PAGE_STANDARD
					   (druidpagestandard3),
					   create_image
					   ("gnomesword/GnomeSword.xpm"));

	druid_vbox3 = GNOME_DRUID_PAGE_STANDARD(druidpagestandard3)->vbox;
	gtk_widget_ref(druid_vbox3);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "druid_vbox3",
				 druid_vbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid_vbox3);

	vbox27 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox27);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "vbox27", vbox27,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox27);
	gtk_box_pack_start(GTK_BOX(druid_vbox3), vbox27, TRUE, TRUE, 0);

	label112 = gtk_label_new("Your Home Directory:");
	gtk_widget_ref(label112);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label112",
				 label112,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label112);
	gtk_box_pack_start(GTK_BOX(vbox27), label112, FALSE, FALSE, 0);

	label113 = gtk_label_new("/home/");
	gtk_widget_ref(label113);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label113",
				 label113,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label113);
	gtk_box_pack_start(GTK_BOX(vbox27), label113, FALSE, FALSE, 0);

	label114 = gtk_label_new("Sword Directory:");
	gtk_widget_ref(label114);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label114",
				 label114,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label114);
	gtk_box_pack_start(GTK_BOX(vbox27), label114, FALSE, FALSE, 0);

	label115 = gtk_label_new("/usr/share/sword");
	gtk_widget_ref(label115);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label115",
				 label115,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label115);
	gtk_box_pack_start(GTK_BOX(vbox27), label115, FALSE, FALSE, 0);

	label116 = gtk_label_new("Number of Bible Text Modules:");
	gtk_widget_ref(label116);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label116",
				 label116,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label116);
	gtk_box_pack_start(GTK_BOX(vbox27), label116, FALSE, FALSE, 0);

	label117 = gtk_label_new("0");
	gtk_widget_ref(label117);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label117",
				 label117,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label117);
	gtk_box_pack_start(GTK_BOX(vbox27), label117, FALSE, FALSE, 0);

	label118 = gtk_label_new("Number of Commentary Modules:");
	gtk_widget_ref(label118);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label118",
				 label118,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label118);
	gtk_box_pack_start(GTK_BOX(vbox27), label118, FALSE, FALSE, 0);

	label119 = gtk_label_new("0");
	gtk_widget_ref(label119);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label119",
				 label119,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label119);
	gtk_box_pack_start(GTK_BOX(vbox27), label119, FALSE, FALSE, 0);

	label120 = gtk_label_new("Number of Lexicon / Dictionary Modules");
	gtk_widget_ref(label120);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label120",
				 label120,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label120);
	gtk_box_pack_start(GTK_BOX(vbox27), label120, FALSE, FALSE, 0);

	label121 = gtk_label_new("0");
	gtk_widget_ref(label121);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label121",
				 label121,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label121);
	gtk_box_pack_start(GTK_BOX(vbox27), label121, FALSE, FALSE, 0);

	druidpagestandard1 =
	    gnome_druid_page_standard_new_with_vals("", NULL);
	gtk_widget_ref(druidpagestandard1);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup),
				 "druidpagestandard1", druidpagestandard1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show_all(druidpagestandard1);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagestandard1));
	gnome_druid_page_standard_set_bg_color(GNOME_DRUID_PAGE_STANDARD
					       (druidpagestandard1),
					       &druidpagestandard1_bg_color);
	gnome_druid_page_standard_set_logo_bg_color
	    (GNOME_DRUID_PAGE_STANDARD(druidpagestandard1),
	     &druidpagestandard1_logo_bg_color);
	gnome_druid_page_standard_set_title_color(GNOME_DRUID_PAGE_STANDARD
						  (druidpagestandard1),
						  &druidpagestandard1_title_color);
	gnome_druid_page_standard_set_title(GNOME_DRUID_PAGE_STANDARD
					    (druidpagestandard1),
					    "Do you want GnomeSword to open with");
	gnome_druid_page_standard_set_logo(GNOME_DRUID_PAGE_STANDARD
					   (druidpagestandard1),
					   create_image
					   ("gnomesword/GnomeSword.xpm"));

	druid_vbox1 = GNOME_DRUID_PAGE_STANDARD(druidpagestandard1)->vbox;
	gtk_widget_ref(druid_vbox1);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "druid_vbox1",
				 druid_vbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid_vbox1);

	vbox26 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox26);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "vbox26", vbox26,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox26);
	gtk_box_pack_start(GTK_BOX(druid_vbox1), vbox26, TRUE, TRUE, 0);

	radiobutton1 =
	    gtk_radio_button_new_with_label(vbox26_group,
					    "Settings you choose as default");
	vbox26_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(radiobutton1));
	gtk_widget_ref(radiobutton1);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "radiobutton1",
				 radiobutton1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(radiobutton1);
	gtk_box_pack_start(GTK_BOX(vbox26), radiobutton1, FALSE, FALSE, 0);

	radiobutton2 =
	    gtk_radio_button_new_with_label(vbox26_group,
					    "Settings used the last time GnomeSword ran");
	vbox26_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(radiobutton2));
	gtk_widget_ref(radiobutton2);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "radiobutton2",
				 radiobutton2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(radiobutton2);
	gtk_box_pack_start(GTK_BOX(vbox26), radiobutton2, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2),
				     TRUE);

	druidpagestandard2 =
	    gnome_druid_page_standard_new_with_vals("", NULL);
	gtk_widget_ref(druidpagestandard2);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup),
				 "druidpagestandard2", druidpagestandard2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show_all(druidpagestandard2);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagestandard2));
	gnome_druid_page_standard_set_bg_color(GNOME_DRUID_PAGE_STANDARD
					       (druidpagestandard2),
					       &druidpagestandard2_bg_color);
	gnome_druid_page_standard_set_logo_bg_color
	    (GNOME_DRUID_PAGE_STANDARD(druidpagestandard2),
	     &druidpagestandard2_logo_bg_color);
	gnome_druid_page_standard_set_title_color(GNOME_DRUID_PAGE_STANDARD
						  (druidpagestandard2),
						  &druidpagestandard2_title_color);
	gnome_druid_page_standard_set_title(GNOME_DRUID_PAGE_STANDARD
					    (druidpagestandard2),
					    "Settings to use on first program run");
	gnome_druid_page_standard_set_logo(GNOME_DRUID_PAGE_STANDARD
					   (druidpagestandard2),
					   create_image
					   ("gnomesword/GnomeSword.xpm"));

	druid_vbox2 = GNOME_DRUID_PAGE_STANDARD(druidpagestandard2)->vbox;
	gtk_widget_ref(druid_vbox2);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "druid_vbox2",
				 druid_vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid_vbox2);

	table7 = gtk_table_new(8, 2, FALSE);
	gtk_widget_ref(table7);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "table7", table7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table7);
	gtk_box_pack_start(GTK_BOX(druid_vbox2), table7, TRUE, TRUE, 0);

	label104 = gtk_label_new("Main Text Module");
	gtk_widget_ref(label104);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label104",
				 label104,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label104);
	gtk_table_attach(GTK_TABLE(table7), label104, 0, 1, 0, 1,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	label105 = gtk_label_new("Interlinear 1");
	gtk_widget_ref(label105);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label105",
				 label105,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label105);
	gtk_table_attach(GTK_TABLE(table7), label105, 0, 1, 1, 2,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	label106 = gtk_label_new("Interlinear 2");
	gtk_widget_ref(label106);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label106",
				 label106,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label106);
	gtk_table_attach(GTK_TABLE(table7), label106, 0, 1, 2, 3,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	label107 = gtk_label_new("Interlinear 3");
	gtk_widget_ref(label107);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label107",
				 label107,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label107);
	gtk_table_attach(GTK_TABLE(table7), label107, 0, 1, 3, 4,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	combo1 = gtk_combo_new();
	gtk_widget_ref(combo1);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "combo1", combo1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo1);
	gtk_table_attach(GTK_TABLE(table7), combo1, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	combo_entry1 = GTK_COMBO(combo1)->entry;
	gtk_widget_ref(combo_entry1);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "combo_entry1",
				 combo_entry1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_entry1);

	combo2 = gtk_combo_new();
	gtk_widget_ref(combo2);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "combo2", combo2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo2);
	gtk_table_attach(GTK_TABLE(table7), combo2, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	combo_entry2 = GTK_COMBO(combo2)->entry;
	gtk_widget_ref(combo_entry2);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "combo_entry2",
				 combo_entry2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_entry2);

	combo3 = gtk_combo_new();
	gtk_widget_ref(combo3);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "combo3", combo3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo3);
	gtk_table_attach(GTK_TABLE(table7), combo3, 1, 2, 2, 3,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	combo_entry3 = GTK_COMBO(combo3)->entry;
	gtk_widget_ref(combo_entry3);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "combo_entry3",
				 combo_entry3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_entry3);

	combo4 = gtk_combo_new();
	gtk_widget_ref(combo4);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "combo4", combo4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo4);
	gtk_table_attach(GTK_TABLE(table7), combo4, 1, 2, 3, 4,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	combo_entry4 = GTK_COMBO(combo4)->entry;
	gtk_widget_ref(combo_entry4);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "combo_entry4",
				 combo_entry4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_entry4);

	checkbutton1 =
	    gtk_check_button_new_with_label
	    ("Auto Save Personal comments                                                                                 ");
	gtk_widget_ref(checkbutton1);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "checkbutton1",
				 checkbutton1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(checkbutton1);
	gtk_table_attach(GTK_TABLE(table7), checkbutton1, 1, 2, 5, 6,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton1),
				     TRUE);

	checkbutton2 =
	    gtk_check_button_new_with_label
	    ("Use Verse Style                                                                                                       ");
	gtk_widget_ref(checkbutton2);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "checkbutton2",
				 checkbutton2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(checkbutton2);
	gtk_table_attach(GTK_TABLE(table7), checkbutton2, 1, 2, 4, 5,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton2),
				     TRUE);

	checkbutton3 =
	    gtk_check_button_new_with_label
	    ("Show Interlinear Page                                                                                              ");
	gtk_widget_ref(checkbutton3);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "checkbutton3",
				 checkbutton3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(checkbutton3);
	gtk_table_attach(GTK_TABLE(table7), checkbutton3, 1, 2, 6, 7,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton3),
				     TRUE);

	checkbutton4 =
	    gtk_check_button_new_with_label
	    ("Show footnotes - with modules that support this feature                                         ");
	gtk_widget_ref(checkbutton4);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "checkbutton4",
				 checkbutton4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(checkbutton4);
	gtk_table_attach(GTK_TABLE(table7), checkbutton4, 1, 2, 7, 8,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	label111 =
	    gtk_label_new
	    ("These setting will be the default settings if you chose to use default settings. ");
	gtk_widget_ref(label111);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "label111",
				 label111,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label111);
	gtk_box_pack_start(GTK_BOX(druid_vbox2), label111, FALSE, FALSE,
			   0);
	gtk_label_set_justify(GTK_LABEL(label111), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label111), TRUE);

	druidpagefinish1 = gnome_druid_page_finish_new();
	gtk_widget_ref(druidpagefinish1);
	gtk_object_set_data_full(GTK_OBJECT(dlgSetup), "druidpagefinish1",
				 druidpagefinish1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druidpagefinish1);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagefinish1));
	gnome_druid_page_finish_set_bg_color(GNOME_DRUID_PAGE_FINISH
					     (druidpagefinish1),
					     &druidpagefinish1_bg_color);
	gnome_druid_page_finish_set_textbox_color(GNOME_DRUID_PAGE_FINISH
						  (druidpagefinish1),
						  &druidpagefinish1_textbox_color);
	gnome_druid_page_finish_set_logo_bg_color(GNOME_DRUID_PAGE_FINISH
						  (druidpagefinish1),
						  &druidpagefinish1_logo_bg_color);
	gnome_druid_page_finish_set_title_color(GNOME_DRUID_PAGE_FINISH
						(druidpagefinish1),
						&druidpagefinish1_title_color);

	dialog_action_area10 = GNOME_DIALOG(dlgSetup)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgSetup), "dialog_action_area10",
			    dialog_action_area10);
	gtk_widget_show(dialog_action_area10);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area10),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area10),
				   8);

	return dlgSetup;
}
