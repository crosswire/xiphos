/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_menu.c
    * -------------------
    * Mon May 8 2000
    * copyright (C) 2000 by Terry Biggs
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
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include "gs_gnomesword.h"
#include "gs_menu.h"
#include "gs_gui_cb.h"
#include "gs_mainmenu_cb.h"
#include "sword.h"
#include "gs_popup_cb.h"
#include "gs_html.h"
#include "support.h"
#include "gs_bookmarks.h"

GtkWidget *module_options_menu;
gint	greekpage,
        hebrewpage;
/*
 *******************************************************************************
 * static function prototypes
 *******************************************************************************
 */
static GtkWidget* create_pmInt(GList *mods, gchar *intWindow);
static GtkWidget* create_pmBible(GList * bibleDescription,
				GList *dictDescription);
static void loadmenuformmodlist(GtkWidget *pmInt, 
		GList *mods, 
		gchar *labelGtkWidget, 
		GtkMenuCallback mycallback);
/******************************************************************************/

extern SETTINGS *settings;

		
/*** add sword global options to menus ***/	
void
additemstooptionsmenu(GList *options, SETTINGS *s)
{
	GtkWidget *item;
	gchar *menu;
	GnomeUIInfo *menuitem;		
	GtkWidget 
		*menuChoice,
		*shellmenu;
	gchar menuName[64];
	int viewNumber = 0;
	GList *tmp;
	
	tmp = NULL;

	tmp = options;
	while (tmp != NULL) {
		shellmenu =  module_options_menu;		
			
		/* add global option items to interlinear popup menu */
		menuChoice = gtk_check_menu_item_new_with_label((gchar *)(gchar *) tmp->data);	
		sprintf(menuName, "optionNum%d", viewNumber++);
		gtk_object_set_data(GTK_OBJECT(s->app), menuName, menuChoice);
		gtk_widget_show(menuChoice);
		gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
			   GTK_SIGNAL_FUNC(on_int_global_options_activate),
			  (gchar *)(gchar *) tmp->data);  
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
                                             GTK_WIDGET(menuChoice),
                                             1);  	      
				
		if(!strcmp((gchar *) tmp->data, "Strong's Numbers")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active = s->strongsint;		
		}
		
		if(!strcmp((gchar *) tmp->data, "Footnotes")) {		
			GTK_CHECK_MENU_ITEM(menuChoice)->active = s->footnotesint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Morphological Tags")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active = s->morphsint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Hebrew Vowel Points")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active = s->hebrewpointsint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Hebrew Cantillation")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active = s->cantillationmarksint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Greek Accents")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active = s->greekaccentsint;
		}	
		
		if(!strcmp((gchar *) tmp->data, "Scripture Cross-references")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active = s->crossrefint;
		}	
		
		if(!strcmp((gchar *) tmp->data, "Lemmas")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active = s->lemmasint;
		}		
		
		if(!strcmp((gchar *) tmp->data, "Headings")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active = s->headingsint;
		}	
		
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}


/******************************************************************************
 * additemtognomemenu - add item to gnome menu 
 * adds an item to the main menu bar
 * MainFrm - the main window - app widget
 * itemname - the item label and also used for user data
 * menuname - the gnome menu path 
 * mycallback - the callback to call when item is selected 
 ******************************************************************************/
void
additemtognomemenu(GtkWidget * MainFrm, gchar * itemname, gchar * itemdata,
		   gchar * menuname, GtkMenuCallback mycallback)
{
	GnomeUIInfo *menuitem;

	menuitem = g_new(GnomeUIInfo, 2);
	menuitem->type = GNOME_APP_UI_ITEM;
	menuitem->moreinfo = (gpointer) mycallback;
	menuitem->user_data = g_strdup(itemdata);
	menuitem->label = itemname;
	menuitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
	menuitem->pixmap_info = GNOME_STOCK_MENU_BOOK_OPEN;
	menuitem->accelerator_key = 0;
	menuitem[1].type = GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm), menuname,
					 menuitem, NULL);
	//g_free(menuitem->user_data); 
	//g_free(menuitem);
}


/******************************************************************************
 * additemtopopupmenu - add item to popup menu
 * add one item to a popup menu
 * cannot be used to add an item to a submenu item ????
 * MainFrm - main window - app
 * menu - popup menu to add item to
 * itemname - menu item -used for label and user data
 * mycallback - callback function to call when menu item pressed
 ******************************************************************************/
void
additemtopopupmenu(GtkWidget * MainFrm, GtkWidget * menu, gchar * itemname,
		   GtkMenuCallback mycallback)
{
	GtkWidget *menuChoice;
	gchar menuName[64];
	int viewNumber = 0;

	menuChoice = gtk_menu_item_new_with_label(itemname);	/* popup menu */
	sprintf(menuName, "viewMod%d", viewNumber++);
	gtk_object_set_data(GTK_OBJECT(MainFrm), menuName, menuChoice);
	gtk_widget_show(menuChoice);
	gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
			   GTK_SIGNAL_FUNC(mycallback),
			   g_strdup(itemname));        
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),
			      GTK_WIDGET(menuChoice));
}

/******************************************************************************
 * addseparato - add separator line to menu
 * MainFrm - main window - app
 * subtreelabel - path to menu item
 ******************************************************************************/ 
void addseparator(GtkWidget * MainFrm, gchar * subtreelabel)
{
	GnomeUIInfo *bookmarkitem;
	bookmarkitem = g_new(GnomeUIInfo, 2);
	bookmarkitem->type = GNOME_APP_UI_SEPARATOR;
	bookmarkitem->pixmap_type = GNOME_APP_PIXMAP_NONE;
	bookmarkitem->accelerator_key = 0;
	bookmarkitem[1].type = GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm), subtreelabel,
					 bookmarkitem, NULL);
	//g_free(bookmarkitem); 
}

//-------------------------------------------------------------------------------------------
void
addsubtreeitem(GtkWidget * MainFrm, gchar * menulabel,
	       gchar * subtreelabel)
{
	static GnomeUIInfo marks1_menu_uiinfo[] = { /* this struct is here to keep from getting compiler error */                                                                                                                            //
		GNOMEUIINFO_SEPARATOR,
		GNOMEUIINFO_END
	};
	GnomeUIInfo *bookmarkitem;
	bookmarkitem = g_new(GnomeUIInfo, 2);
	bookmarkitem->type = GNOME_APP_UI_SUBTREE;
	bookmarkitem->moreinfo = marks1_menu_uiinfo;
	bookmarkitem->user_data = NULL;
	bookmarkitem->label = subtreelabel;
	bookmarkitem->pixmap_type = GNOME_APP_PIXMAP_NONE;
	bookmarkitem->pixmap_info = NULL;
	bookmarkitem->accelerator_key = 0;
	bookmarkitem[1].type = GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm), menulabel,
					 bookmarkitem, NULL);
	//g_free(bookmarkitem);         
}

/******************************************************************************
 * createpopupmenus(GList *, GList *, GList *) - create popup menus
 * app - main window
 * settings - gnomesword settings structure
 * biblelist - list of Bible modules - from initSword
 * commentarylist - list of commentary modules - from initSword
 * dictionarylist - list of dict/lex modules - from initSword 
 * percomlist - list of personal comments modules
 ******************************************************************************/
void createpopupmenus(SETTINGS *s,
		GList *bibleDescription,
		GList *options) 
{
	/* create popup menu for interlinear window */
	s->menuInt = create_pmInt(bibleDescription, "textComp1"); 
	
	/* attach popup menus */
	gnome_popup_menu_attach(s->menuInt,s->htmlInterlinear,(gchar*)"1");
	additemstooptionsmenu(options, s);
}

/*
 *******************************************************************************
 * addmodsmenus(GList *, GList *, GList *) - add modules to about mods menus
 * settings - gnomesword settings structure
 * biblelist - list of Bible modules - from initSword
 * commentarylist - list of commentary modules - from initSword
 * dictionarylist - list of dict/lex modules - from initSword 
 * percomlist - list of personal comments modules
 *******************************************************************************
 */
void addmodstomenus(SETTINGS *s, 
		GList *biblelist, 
		GList *commentarylist, 
		GList *dictionarylist, 
		GList *booklist) 
{
	gchar	
		aboutrememberlastitem[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem2[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem3[80], //--  use to store last menu item so we can add the next item under it - gnome menus	
		mybuf[80];
	gint	pg = 0;
	GList 	*tmp = NULL;

	sprintf(aboutrememberlastitem,"%s", _("_Help/About Sword Modules/Bible Texts/<Separator>"));
	sprintf(aboutrememberlastitem2,"%s", _("_Help/About Sword Modules/Commentaries/<Separator>"));
	sprintf(aboutrememberlastitem3,"%s", _("_Help/About Sword Modules/Dictionaries-Lexicons/<Separator>"));
//-- add textmods - Main Window menu
	tmp = biblelist;
	while (tmp != NULL) {	
	//-- add to menubar
		additemtognomemenu(s->app, 
				(gchar *) tmp->data, 
				(gchar *) tmp->data, 
				aboutrememberlastitem , 
				(GtkMenuCallback)on_kjv1_activate );
		//-- remember last item - so next item will be place below it   
		sprintf(aboutrememberlastitem,"%s%s", 
				_("_Help/About Sword Modules/Bible Texts/"), 
				(gchar *) tmp->data);			
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
//-- add commmods - commentary window menu
	tmp = commentarylist;
	while (tmp != NULL) {
		additemtognomemenu(s->app,(gchar *) 
				tmp->data, 
				(gchar *) tmp->data, 
				aboutrememberlastitem2, 
				(GtkMenuCallback)on_kjv1_activate );			
		sprintf(aboutrememberlastitem2,"%s%s", _("_Help/About Sword Modules/Commentaries/"),
				(gchar *) tmp->data);
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	pg = 0;
//-- add dictmods - dictionary window menu
	tmp = dictionarylist;
	while (tmp != NULL) {
		sprintf(mybuf,"%d",pg);
		if(!strcmp((gchar *) tmp->data, _("StrongsHebrew"))) hebrewpage = pg;
		if(!strcmp((gchar *) tmp->data, _("StrongsGreek"))) greekpage = pg;
		additemtognomemenu(s->app,(gchar *) tmp->data, 
				(gchar *) tmp->data, aboutrememberlastitem3,
				(GtkMenuCallback)on_kjv1_activate );
		sprintf(aboutrememberlastitem3,"%s%s", _("_Help/About Sword Modules/Dictionaries-Lexicons/"),
				(gchar *) tmp->data);				
		++pg;
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	/** book modules **/
	sprintf(aboutrememberlastitem3,"%s", _("_Help/About Sword Modules/Books/<Separator>"));
	tmp = booklist;
	while (tmp != NULL) {
		additemtognomemenu(s->app,(gchar *) tmp->data, 
				(gchar *) tmp->data, aboutrememberlastitem3, 
				(GtkMenuCallback)on_kjv1_activate );			
		sprintf(aboutrememberlastitem3,"%s%s", _("_Help/About Sword Modules/Books/"),
				(gchar *) tmp->data);
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
}			

/********************************************************************************
 * additemtosubtree
 * GtkWidget * MainFrm - application main window
 * gchar * subtreelabel - path to menu subtree
 * gchar * itemlabel - item to add
 *******************************************************************************/
void
additemtosubtree(GtkWidget * MainFrm, gchar * subtreelabel,
		 gchar * itemlabel)
{
	GnomeUIInfo *bookmarkitem;
	
	bookmarkitem = g_new(GnomeUIInfo, 2);
	bookmarkitem->type = GNOME_APP_UI_ITEM;
	bookmarkitem->moreinfo = (gpointer) on_john_3_1_activate;
	bookmarkitem->user_data = g_strdup(itemlabel);
	bookmarkitem->label = itemlabel;
	bookmarkitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
	bookmarkitem->pixmap_info = GNOME_STOCK_MENU_BOOK_OPEN;
	bookmarkitem->accelerator_key = 0;
	bookmarkitem[1].type = GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm), subtreelabel,
					 bookmarkitem, NULL);
//      g_free(bookmarkitem->user_data);
//      g_free(bookmarkitem);   
}

//-------------------------------------------------------------------------------------------
//-- we need to return a widget so we can use it later as a toggle menu item
GtkWidget *additemtooptionmenu(GtkWidget * MainFrm, gchar * subtreelabel,
			       gchar * itemlabel,
			       GtkMenuCallback mycallback)
{
	GtkWidget *item;
	GnomeUIInfo *menuitem;

	menuitem = g_new(GnomeUIInfo, 2);
	menuitem->type = GNOME_APP_UI_TOGGLEITEM;
	menuitem->user_data = NULL;
	menuitem->pixmap_type = GNOME_APP_PIXMAP_NONE;
	menuitem->pixmap_info = NULL;
	menuitem->accelerator_key = 0;
	menuitem->label = itemlabel;
	menuitem->moreinfo = (gpointer) mycallback;
	menuitem[1].type = GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm), subtreelabel,
					 menuitem, NULL);
	item = menuitem[0].widget;

	return (item);
}

//-------------------------------------------------------------------------------------------
void
removemenuitems(GtkWidget * MainFrm, gchar * startitem, gint numberofitems)
/* remove a number(numberofitems) of items form a menu or submenu(startitem)   */
{				
	gnome_app_remove_menus(GNOME_APP(MainFrm), startitem,
			       numberofitems);
}

/*****************************************************************************
 *  popup menu for main bible window
 *****************************************************************************/
static GtkWidget* create_pmBible(GList *bibleDescription, 
				GList *dictDescription)
{
	GtkWidget *pmBible;
	GtkAccelGroup *pmBible_accels;
	GtkWidget *copy7;
	GtkWidget *show_tabs;
    /*
	GtkWidget *lookup_word;
	GtkWidget *lookup_word_menu;
	GtkWidget *usecurrent1;
	GtkWidget *separator1;
    */
	GtkWidget *lookup_selection;
	GtkWidget *lookup_selection_menu;
	GtkWidget *usecurrent2;
	GtkWidget *separator3;
	GtkWidget *about_this_module1;
	GtkWidget *add_bookmark;
	GtkWidget *add_bookmark_menu;
	GtkWidget *separator4;
	GtkWidget *separator2;
	GtkWidget *view_module3;
	GtkWidget *view_module3_menu;
	GtkAccelGroup *view_module3_menu_accels;
	GtkWidget *item3;
	GtkWidget *item4;
	GtkTooltips *tooltips;
	GtkWidget *viewtext;
	GList *tmp = NULL;
	gint i = 0;
	
	tooltips = gtk_tooltips_new();
	pmBible = gtk_menu_new ();
	gtk_object_set_data (GTK_OBJECT (pmBible), "pmBible", pmBible);
	pmBible_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (pmBible));

	copy7 = gtk_menu_item_new_with_label (_("Copy"));
	gtk_widget_ref (copy7);
	gtk_object_set_data_full (GTK_OBJECT (pmBible), "copy7", copy7,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (copy7);
	gtk_container_add (GTK_CONTAINER (pmBible), copy7);
	/*
	lookup_word = gtk_menu_item_new_with_label (_("Lookup Word"));
	gtk_widget_ref (lookup_word);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "lookup_word", lookup_word,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (lookup_word);
  	gtk_container_add (GTK_CONTAINER (pmBible), lookup_word);
	
	lookup_word_menu = gtk_menu_new ();
  	gtk_widget_ref (lookup_word_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "lookup_word_menu", lookup_word_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lookup_word), lookup_word_menu);
	
	usecurrent1 = gtk_menu_item_new_with_label (_("Use Current Dictionary"));
	gtk_widget_ref (usecurrent1);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "usecurrent1", usecurrent1,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (usecurrent1);
  	gtk_container_add (GTK_CONTAINER (lookup_word_menu), usecurrent1);
	
	separator1 = gtk_menu_item_new ();
  	gtk_widget_ref (separator1);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "separator1", separator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator1);
  	gtk_container_add (GTK_CONTAINER (lookup_word_menu), separator1);
  	gtk_widget_set_sensitive (separator1, FALSE);
	*/
	lookup_selection = gtk_menu_item_new_with_label (_("Lookup Selection"));
	gtk_widget_ref (lookup_selection);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "lookup_selection", lookup_selection,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (lookup_selection);
  	gtk_container_add (GTK_CONTAINER (pmBible), lookup_selection);
	
	lookup_selection_menu = gtk_menu_new ();
  	gtk_widget_ref (lookup_selection_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "lookup_selection_menu", lookup_selection_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lookup_selection), lookup_selection_menu);
  	//view_module3_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (view_module3_menu));
	
	usecurrent2 = gtk_menu_item_new_with_label (_("Use Current Dictionary"));
	gtk_widget_ref (usecurrent2);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "usecurrent2", usecurrent2,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (usecurrent2);
  	gtk_container_add (GTK_CONTAINER (lookup_selection_menu), usecurrent2);
	
	separator3 = gtk_menu_item_new ();
  	gtk_widget_ref (separator3);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "separator3", separator3,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator3);
  	gtk_container_add (GTK_CONTAINER (lookup_selection_menu), separator3);
  	gtk_widget_set_sensitive (separator3, FALSE);	
	
	/*** add bookmark menu ***/
	add_bookmark = gtk_menu_item_new_with_label(_("Add Bookmark"));
	gtk_widget_ref(add_bookmark);
	gtk_object_set_data_full(GTK_OBJECT(pmBible), "add_bookmark",
				 add_bookmark, 
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(add_bookmark);
	gtk_container_add(GTK_CONTAINER(pmBible), add_bookmark);
	gtk_tooltips_set_tip(tooltips, add_bookmark, _("Add current verse to Presonal Bookmarks"),
			     NULL);
	
	add_bookmark_menu = gtk_menu_new ();
  	gtk_widget_ref (add_bookmark_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "add_bookmark_menu", add_bookmark_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (add_bookmark), add_bookmark_menu);
	
	/*** add bookmark items ***/
	create_addBookmarkMenuBM(pmBible, add_bookmark_menu, settings);
	
	show_tabs = gtk_check_menu_item_new_with_label(_("Show Tabs"));
	gtk_widget_ref(show_tabs);
	gtk_object_set_data_full(GTK_OBJECT(pmBible), "show_tabs",
				 show_tabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(show_tabs);
	gtk_container_add(GTK_CONTAINER(pmBible), show_tabs);
	gtk_tooltips_set_tip(tooltips, show_tabs, _("Show notebook tabs"),
			     NULL);
				     
  	about_this_module1 = gtk_menu_item_new_with_label (_("About this module"));
  	gtk_widget_ref (about_this_module1);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "about_this_module1", about_this_module1,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (about_this_module1);
  	gtk_container_add (GTK_CONTAINER (pmBible), about_this_module1);

	viewtext = gtk_menu_item_new_with_label (_("View Text in new window"));
  	gtk_widget_ref (viewtext);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "viewtext", viewtext,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (viewtext);
  	gtk_container_add (GTK_CONTAINER (pmBible), viewtext);
	
	separator4 = gtk_menu_item_new ();
  	gtk_widget_ref (separator4);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "separator4", separator4,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator4);
  	gtk_container_add (GTK_CONTAINER (pmBible), separator4);
  	gtk_widget_set_sensitive (separator4, FALSE);		
			     
  	settings->unlocktextmod_item = gtk_menu_item_new_with_label (_("Unlock This Module"));
  	gtk_widget_ref (settings->unlocktextmod_item);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "settings->unlocktextmod_item",settings->unlocktextmod_item ,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (settings->unlocktextmod_item);
  	gtk_container_add (GTK_CONTAINER (pmBible), settings->unlocktextmod_item);
  	
  	
  	separator2 = gtk_menu_item_new ();
  	gtk_widget_ref (separator2);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator2);
  	gtk_container_add (GTK_CONTAINER (pmBible), separator2);
  	gtk_widget_set_sensitive (separator2, FALSE);

  	view_module3 = gtk_menu_item_new_with_label (_("View Module"));
  	gtk_widget_ref (view_module3);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "view_module3", view_module3,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (view_module3);
  	gtk_container_add (GTK_CONTAINER (pmBible), view_module3);

  	view_module3_menu = gtk_menu_new ();
  	gtk_widget_ref (view_module3_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "view_module3_menu", view_module3_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (view_module3), view_module3_menu);
  	view_module3_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (view_module3_menu));

	tmp = dictDescription;
	while (tmp != NULL) {
		//item3 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		item4 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		//gtk_widget_ref(item3);
		gtk_widget_ref(item4);
		/*gtk_object_set_data_full(GTK_OBJECT(pmBible), "item3",
					 item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);*/
		gtk_object_set_data_full(GTK_OBJECT(pmBible), "item4",
					 item4,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		//gtk_widget_show(item3);	
		gtk_widget_show(item4);
		/*gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_word_activate),
				   (gchar *) tmp->data);*/
		gtk_signal_connect(GTK_OBJECT(item4), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_selection_activate),
				   (gchar *) tmp->data);
		//gtk_container_add(GTK_CONTAINER(lookup_word_menu), item3);
		gtk_container_add(GTK_CONTAINER(lookup_selection_menu), item4);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	i=0;
	tmp = bibleDescription;
	while (tmp != NULL) {
		item3 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_object_set_data_full(GTK_OBJECT(pmBible), "item3",
					 item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);		
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_mainText_activate),
				   GINT_TO_POINTER(i));

		gtk_container_add(GTK_CONTAINER(view_module3_menu), item3);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);	
	/*** these two are for using the current dictionary for lookup ***/
	/*gtk_signal_connect(GTK_OBJECT(usecurrent1), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_word_activate),
				   "current");*/
	gtk_signal_connect(GTK_OBJECT(usecurrent2), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_selection_activate),
				   "current");
	gtk_signal_connect(GTK_OBJECT(show_tabs), "activate",
			   GTK_SIGNAL_FUNC(on_show_tabs_activate), 
			(gchar *)"nbTextMods");	
  	gtk_signal_connect (GTK_OBJECT (copy7), "activate",
                      	GTK_SIGNAL_FUNC (on_copyhtml_activate),
                      	(gchar *)"htmlTexts");
  	gtk_signal_connect (GTK_OBJECT (about_this_module1), "activate",
                      	GTK_SIGNAL_FUNC (on_about_this_module_activate),
                      	"Bible");
	gtk_signal_connect (GTK_OBJECT (viewtext), "activate",
                      	GTK_SIGNAL_FUNC (on_viewtext_activate),
                      	NULL);
  return pmBible;
}


/******************************************************************************
 * create_pmInt1(GList *mods) - create popup menu for first interlinear window
 * GList *mods - list of Bible modules
 * gchar *intWindow - the interlinear window to build the menu for
 * GtkMenuCallback cbchangemod - callback to change modules form view submenu
 * GtkMenuCallback mycallback - callback for copy text
 ******************************************************************************/
static GtkWidget*
create_pmInt(GList *mods, gchar *intWindow)
{
	GtkWidget *pmInt;
	GtkAccelGroup *pmInt_accels;
	GtkWidget *copy7;
	GtkWidget *undockInt;
	GtkWidget *module_options;
	GtkWidget *separator2;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();
	pmInt = gtk_menu_new ();
	gtk_object_set_data (GTK_OBJECT (pmInt), "pmInt", pmInt);
	pmInt_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (pmInt));

	copy7 = gtk_menu_item_new_with_label (_("Copy"));
	gtk_widget_ref (copy7);
	gtk_object_set_data_full (GTK_OBJECT (pmInt), "copy7", copy7,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (copy7);
	gtk_container_add (GTK_CONTAINER (pmInt), copy7);

	separator2 = gtk_menu_item_new ();
  	gtk_widget_ref (separator2);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator2);
  	gtk_container_add (GTK_CONTAINER (pmInt), separator2);
  	gtk_widget_set_sensitive (separator2, FALSE);
	
	undockInt = gtk_menu_item_new_with_label (_("Detach/Attach"));
	gtk_widget_ref (undockInt);
	gtk_object_set_data_full (GTK_OBJECT (pmInt), "undockInt",undockInt ,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (undockInt);
	gtk_container_add (GTK_CONTAINER (pmInt), undockInt);
	
	module_options = gtk_menu_item_new_with_label (_("Module Options"));
	gtk_widget_ref(module_options);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "module_options", module_options,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (module_options);
  	gtk_container_add (GTK_CONTAINER(pmInt), module_options);
	
	module_options_menu = gtk_menu_new ();
  	gtk_widget_ref(module_options_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "module_options_menu",module_options_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (module_options), module_options_menu);
	
  	separator2 = gtk_menu_item_new ();
  	gtk_widget_ref (separator2);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator2);
  	gtk_container_add (GTK_CONTAINER (pmInt), separator2);
  	gtk_widget_set_sensitive (separator2, FALSE);
  	/* build change interlinear modules submenu */
	loadmenuformmodlist(pmInt, mods, _("Change Interlinear 1"), (GtkMenuCallback)on_changeint1mod_activate );  	
	loadmenuformmodlist(pmInt, mods, _("Change Interlinear 2"), (GtkMenuCallback)on_changeint2mod_activate );	
	loadmenuformmodlist(pmInt, mods, _("Change Interlinear 3"), (GtkMenuCallback)on_changeint3mod_activate );  	
	loadmenuformmodlist(pmInt, mods, _("Change Interlinear 4"), (GtkMenuCallback)on_changeint4mod_activate );
	loadmenuformmodlist(pmInt, mods, _("Change Interlinear 5"), (GtkMenuCallback)on_changeint5mod_activate );

  	gtk_signal_connect (GTK_OBJECT (copy7), "activate",
                      	GTK_SIGNAL_FUNC (on_copyhtml_activate),
                      	(gchar *)intWindow);
	gtk_signal_connect (GTK_OBJECT (undockInt), "activate",
                      	GTK_SIGNAL_FUNC (on_undockInt_activate), 
                      	settings);
			
  return pmInt;
}

static
void loadmenuformmodlist(GtkWidget *pmInt, GList *mods,  gchar *label, GtkMenuCallback mycallback)
{
	GList *tmp;
	GtkWidget *item;
	GtkWidget *view_module;
	GtkWidget *view_module_menu;
	GtkAccelGroup *view_module_menu_accels;	
	
	view_module = gtk_menu_item_new_with_label (label);
  	gtk_widget_ref (view_module);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "view_module", view_module,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (view_module);
  	gtk_container_add (GTK_CONTAINER (pmInt), view_module);

  	view_module_menu = gtk_menu_new ();
  	gtk_widget_ref (view_module_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "view_module_menu", view_module_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (view_module), view_module_menu);
  	view_module_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (view_module_menu));
	tmp = mods;
	while (tmp != NULL) {
		item = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item);
		gtk_object_set_data_full(GTK_OBJECT(pmInt), "item",
					 item,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item);		
		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC(mycallback),
				   g_strdup((gchar *)tmp->data ));

		gtk_container_add(GTK_CONTAINER(view_module_menu), item);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp); 
} 


