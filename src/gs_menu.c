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
#include "sw_gnomesword.h"
#include "gs_popup_cb.h"
#include "gs_html.h"
#include "support.h"
#include "gs_bookmarks.h"

GtkWidget *menuInt;
GtkWidget *lang_options_menu;

/*
 *******************************************************************************
 * static function prototypes
 *******************************************************************************
 */
static GtkWidget* create_pmInt(GList *mods, GList *options, gchar *intWindow);
static GtkWidget *create_pmCommentsHtml(GList *comDescription, 
				GList *dictDescription);
static GtkWidget *create_pmDict(GList *modsdesc);
static GtkWidget* create_pmBible(GList * bibleDescription,
				GList *dictDescription);
				/*
static GtkWidget *create_pmEditnote(GtkWidget *app, 
		GList *mods);
		*/
static void loadmenuformmodlist(GtkWidget *pmInt, 
		GList *mods, 
		gchar *labelGtkWidget, 
		GtkMenuCallback mycallback);
/******************************************************************************/
extern gint	greekpage,
        	hebrewpage;
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
		if(!strcmp((gchar *) tmp->data, "Hebrew Vowel Points")) {
			menu = "_Settings/Language Options/";
			shellmenu = lang_options_menu;
		}
		
		else if(!strcmp((gchar *) tmp->data, "Hebrew Cantillation")) {
			menu = "_Settings/Language Options/";
			shellmenu = lang_options_menu;
		}
		
		else if(!strcmp((gchar *) tmp->data, "Greek Accents")) {
			menu = "_Settings/Language Options/";
			shellmenu = lang_options_menu;
		}
		
		else {
			menu = "_Settings/";
			shellmenu =  menuInt;
		}
		
		menuitem = g_new(GnomeUIInfo, 2);
		menuitem->type = GNOME_APP_UI_TOGGLEITEM;
		menuitem->user_data = (gchar *)(gchar *) tmp->data;
		menuitem->pixmap_type = GNOME_APP_PIXMAP_NONE;
		menuitem->pixmap_info = NULL;
		menuitem->accelerator_key = 0;
		menuitem->label = (gchar *)(gchar *) tmp->data;
		menuitem->moreinfo = (gpointer) (GtkMenuCallback)on_global_options_activate;
		menuitem[1].type = GNOME_APP_UI_ENDOFINFO;
		gnome_app_insert_menus_with_data(GNOME_APP(settings->app), menu,
					 menuitem, NULL);
		item = menuitem[0].widget;
		
			
		/* add global option items to interlinear popup menu */
		menuChoice = gtk_check_menu_item_new_with_label((gchar *)(gchar *) tmp->data);	
		sprintf(menuName, "optionNum%d", viewNumber++);
		gtk_object_set_data(GTK_OBJECT(settings->app), menuName, menuChoice);
		gtk_widget_show(menuChoice);
		gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
			   GTK_SIGNAL_FUNC(on_int_global_options_activate),
			  (gchar *)(gchar *) tmp->data);  
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
                                             GTK_WIDGET(menuChoice),
                                             1);      
		/*gtk_menu_shell_append(GTK_MENU_SHELL(shellmenu),
			      GTK_WIDGET(menuChoice));	*/		      
		
		
		if(!strcmp((gchar *) tmp->data, "Strong's Numbers")) {
			s->strongsnum =  menuitem[0].widget;
			//GTK_CHECK_MENU_ITEM(s->strongsnum)->active = settings->strongs;	
			GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->strongsint;		
			/* set strongs toogle button */
			//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(settings->app,"btnStrongs")), settings->strongs);
		}
		
		if(!strcmp((gchar *) tmp->data,"Footnotes" )) {
			s->footnotes =  menuitem[0].widget;			
			//GTK_CHECK_MENU_ITEM(s->footnotes)->active = settings->footnotes;			
			GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->footnotesint;
			//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(settings->app,"btnFootnotes")), settings->footnotes);
		}
		
		if(!strcmp((gchar *) tmp->data, "Morphological Tags")) {
			s->morphs =  menuitem[0].widget;
			//GTK_CHECK_MENU_ITEM(s->morphs)->active = settings->morphs;
			GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->morphsint;
			//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(settings->app,"btnMorphs")), settings->morphs);
		}
		
		if(!strcmp((gchar *) tmp->data, "Hebrew Vowel Points")) {
			s->hebrewpoints =  menuitem[0].widget;
			//GTK_CHECK_MENU_ITEM(item)->active = settings->hebrewpoints;
			GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->hebrewpointsint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Hebrew Cantillation")) {
			s->cantillationmarks =  menuitem[0].widget;
			//GTK_CHECK_MENU_ITEM(item)->active = settings->cantillationmarks;
			GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->cantillationmarksint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Greek Accents")) {
			s->greekaccents =  menuitem[0].widget;
			//GTK_CHECK_MENU_ITEM(item)->active = settings->greekaccents;
			GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->greekaccentsint;
		}	
		
		if(!strcmp((gchar *) tmp->data, "Scripture Cross-references")) {
			s->crossrefs =  menuitem[0].widget;
			//GTK_CHECK_MENU_ITEM(item)->active = settings->greekaccents;
			//GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->greekaccentsint;
			GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->crossrefint;
		}	
		
		if(!strcmp((gchar *) tmp->data, "Lemmas")) {
			s->lemmas =  menuitem[0].widget;
			//GTK_CHECK_MENU_ITEM(item)->active = settings->greekaccents;
			//GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->greekaccentsint;
			GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->lemmasint;
		}		
		
		if(!strcmp((gchar *) tmp->data, "Headings")) {
			s->headings =  menuitem[0].widget;
			//GTK_CHECK_MENU_ITEM(item)->active = settings->greekaccents;
			//GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->greekaccentsint;
			GTK_CHECK_MENU_ITEM(menuChoice)->active = settings->headingsint;
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
void createpopupmenus(GtkWidget *app, 
				SETTINGS *settings,
				GList *bibleDescription,
				GList *comDescription,
				GList *dictDescription,
				GList *percomlist,
				GList *options) 
{
	GtkWidget 		
		//*menu5,
		*menuDict,
		*menuBible,
		*menuhtmlcom;	
	
	menuInt = create_pmInt(bibleDescription, options, "textComp1"); 
	//menu5 = create_pmEditnote(app, percomlist);
	menuDict = create_pmDict(dictDescription);
	/* create popup menu for Bible window */
	menuBible = create_pmBible(bibleDescription, dictDescription);	
	/* create popup menu for commentaries window */
	menuhtmlcom = create_pmCommentsHtml(comDescription, dictDescription);			
	/* attach popup menus and ajust checkmarks*/
	gnome_popup_menu_attach(menuInt,lookup_widget(app,"textComp1"),(gchar*)"1");
	//GTK_CHECK_MENU_ITEM (lookup_widget(menuInt,"show_strongs"))->active = settings->strongsint;
	//GTK_CHECK_MENU_ITEM (lookup_widget(menuInt,"show_morphs"))->active = settings->morphsint;
	//GTK_CHECK_MENU_ITEM (lookup_widget(menuInt,"show_footnotes"))->active = settings->footnotesint;	
	//gnome_popup_menu_attach(menu5,lookup_widget(app,"textComments"),(gchar*)"1");
	gnome_popup_menu_attach(menuBible,lookup_widget(app,"htmlTexts"),(gchar*)"1");
	GTK_CHECK_MENU_ITEM (lookup_widget(menuBible,"show_tabs"))->active = settings->comm_tabs;
	gnome_popup_menu_attach(menuhtmlcom,lookup_widget(app,"htmlCommentaries"),(gchar*)"1");
	GTK_CHECK_MENU_ITEM (lookup_widget(menuhtmlcom,"show_tabs1"))->active = settings->comm_tabs;
	gnome_popup_menu_attach(menuDict,lookup_widget(app,"htmlDict"),(gchar*)"1");	
	GTK_CHECK_MENU_ITEM (lookup_widget(menuDict,"show_tabs1"))->active = settings->dict_tabs;

}

/*
 *******************************************************************************
 * addmodsmenus(GList *, GList *, GList *) - add modules to about mods menus
 * app - main window
 * settings - gnomesword settings structure
 * biblelist - list of Bible modules - from initSword
 * commentarylist - list of commentary modules - from initSword
 * dictionarylist - list of dict/lex modules - from initSword 
 * percomlist - list of personal comments modules
 *******************************************************************************
 */
void addmodstomenus(GtkWidget *app, 
				SETTINGS *settings, 
				GList *biblelist, 
				GList *bibleDescription,
				GList *commentarylist, 
				GList *comDescription,
				GList *dictionarylist, 
				GList *dictDescription,
				GList *percomlist) 
{
	gchar	
		aboutrememberlastitem[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem2[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem3[80], //--  use to store last menu item so we can add the next item under it - gnome menus	
		mybuf[80];
	gint	pg = 0;
	GList *tmp = NULL;

	sprintf(aboutrememberlastitem,"%s","_Help/About Sword Modules/Bible Texts/<Separator>");
	sprintf(aboutrememberlastitem2,"%s","_Help/About Sword Modules/Commentaries/<Separator>");
	sprintf(aboutrememberlastitem3,"%s","_Help/About Sword Modules/Dictionaries-Lexicons/<Separator>");
//-- add textmods - Main Window menu
	tmp = biblelist;
	while (tmp != NULL) {	
	//-- add to menubar
		additemtognomemenu(app, (gchar *) tmp->data, (gchar *) tmp->data, aboutrememberlastitem , (GtkMenuCallback)on_kjv1_activate );
		//-- remember last item - so next item will be place below it   
		sprintf(aboutrememberlastitem,"%s%s","_Help/About Sword Modules/Bible Texts/", (gchar *) tmp->data);			
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
//-- add commmods - commentary window menu
	tmp = commentarylist;
	while (tmp != NULL) {
		additemtognomemenu(app,(gchar *) tmp->data, 
				(gchar *) tmp->data, aboutrememberlastitem2, 
				(GtkMenuCallback)on_kjv1_activate );			
		sprintf(aboutrememberlastitem2,"%s%s","_Help/About Sword Modules/Commentaries/",
				(gchar *) tmp->data);
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	pg = 0;
//-- add dictmods - dictionary window menu
	tmp = dictionarylist;
	while (tmp != NULL) {
		sprintf(mybuf,"%d",pg);
		if(!strcmp((gchar *) tmp->data,"StrongsHebrew")) hebrewpage = pg;
		if(!strcmp((gchar *) tmp->data,"StrongsGreek")) greekpage = pg;
		additemtognomemenu(app,(gchar *) tmp->data, 
				(gchar *) tmp->data, aboutrememberlastitem3,
				(GtkMenuCallback)on_kjv1_activate );
		sprintf(aboutrememberlastitem3,"%s%s","_Help/About Sword Modules/Dictionaries-Lexicons/",
				(gchar *) tmp->data);				
		++pg;
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

//-------------------------------------------------------------------------------------------
static GtkWidget *create_pmCommentsHtml(GList *comDescription, 
						GList *dictDescription)
{
	GtkWidget *pmCommentsHtml;
	GtkWidget *copy6;
	GtkWidget *goto_reference;
	GtkWidget *lookup_word;
	GtkWidget *lookup_word_menu;
	GtkWidget *usecurrent1;
	GtkWidget *separator1;
	GtkWidget *lookup_selection;
	GtkWidget *lookup_selection_menu;
	GtkWidget *usecurrent2;
	GtkWidget *separator3;	
	GtkWidget *add_bookmark;
	GtkWidget *add_bookmark_menu;
	GtkWidget *about_this_module6;
	GtkWidget *auto_scroll1;
	GtkWidget *show_tabs1;
	GtkWidget *view_in_new_window;
	GtkWidget *view_module1;
	GtkWidget *view_module1_menu;
	GtkWidget *print_item;
	GtkAccelGroup *view_module1_menu_accels;
	GtkWidget *separator22, *item1, *item3, *item4;
	GtkTooltips *tooltips;
	GList *tmp = NULL;
	//GList *tmp2 = NULL;
	gint i = 0;
		
	tooltips = gtk_tooltips_new();
	pmCommentsHtml = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmCommentsHtml), "pmCommentsHtml",
			    pmCommentsHtml);
			
	copy6 = gtk_menu_item_new_with_label("Copy");
	gtk_widget_ref(copy6);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "copy6", copy6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(copy6);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), copy6);


	print_item = gtk_menu_item_new_with_label("Print Comment");
	gtk_widget_ref(print_item);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml),
				 "print_item", print_item,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(print_item);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml),print_item);
		
	separator1 = gtk_menu_item_new ();
  	gtk_widget_ref (separator1);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "separator1", separator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator1);
  	gtk_container_add (GTK_CONTAINER (pmCommentsHtml), separator1);
  	gtk_widget_set_sensitive (separator1, FALSE);
	
	goto_reference = gtk_menu_item_new_with_label("Goto Reference");
	gtk_widget_ref(goto_reference);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml),
				 "goto_reference", goto_reference,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(goto_reference);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), goto_reference);
	
	lookup_word = gtk_menu_item_new_with_label ("Lookup Word");
	gtk_widget_ref (lookup_word);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "lookup_word", lookup_word,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (lookup_word);
  	gtk_container_add (GTK_CONTAINER (pmCommentsHtml), lookup_word);
	
	lookup_word_menu = gtk_menu_new ();
  	gtk_widget_ref (lookup_word_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "lookup_word_menu", lookup_word_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lookup_word), lookup_word_menu);
	
	usecurrent1 = gtk_menu_item_new_with_label ("Use Current Dictionary");
	gtk_widget_ref (usecurrent1);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "usecurrent1", usecurrent1,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (usecurrent1);
  	gtk_container_add (GTK_CONTAINER (lookup_word_menu), usecurrent1);
	
	separator1 = gtk_menu_item_new ();
  	gtk_widget_ref (separator1);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "separator1", separator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator1);
  	gtk_container_add (GTK_CONTAINER (lookup_word_menu), separator1);
  	gtk_widget_set_sensitive (separator1, FALSE);
	
	lookup_selection = gtk_menu_item_new_with_label ("Lookup Selection");
	gtk_widget_ref (lookup_selection);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "lookup_selection", lookup_selection,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (lookup_selection);
  	gtk_container_add (GTK_CONTAINER (pmCommentsHtml), lookup_selection);
	
	lookup_selection_menu = gtk_menu_new ();
  	gtk_widget_ref (lookup_selection_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "lookup_selection_menu", lookup_selection_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lookup_selection), lookup_selection_menu);
  	//view_module3_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (view_module3_menu));
	
	usecurrent2 = gtk_menu_item_new_with_label ("Use Current Dictionary");
	gtk_widget_ref (usecurrent2);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "usecurrent2", usecurrent2,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (usecurrent2);
  	gtk_container_add (GTK_CONTAINER (lookup_selection_menu), usecurrent2);
	
	separator3 = gtk_menu_item_new ();
  	gtk_widget_ref (separator3);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "separator3", separator3,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator3);
  	gtk_container_add (GTK_CONTAINER (lookup_selection_menu), separator3);
  	gtk_widget_set_sensitive (separator3, FALSE);	
	
	add_bookmark = gtk_menu_item_new_with_label("Add Bookmark");
	gtk_widget_ref(add_bookmark);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "add_bookmark",
				 add_bookmark, 
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(add_bookmark);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), add_bookmark);
	gtk_tooltips_set_tip(tooltips, add_bookmark, "Add current verse and module to Presonal Bookmarks",
			     NULL);
	
	add_bookmark_menu = gtk_menu_new ();
  	gtk_widget_ref (add_bookmark_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "add_bookmark_menu", add_bookmark_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (add_bookmark), add_bookmark_menu);
	/*** add bookmark items ***/
	create_addBookmarkMenuBM(pmCommentsHtml, add_bookmark_menu,settings);		
	
	about_this_module6 =
	    gtk_menu_item_new_with_label("About this module");
	gtk_widget_ref(about_this_module6);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml),
				 "about_this_module6", about_this_module6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(about_this_module6);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), about_this_module6);

	auto_scroll1 = gtk_check_menu_item_new_with_label("Auto Scroll");
	gtk_widget_ref(auto_scroll1);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "auto_scroll1",
				 auto_scroll1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(auto_scroll1);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), auto_scroll1);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(auto_scroll1),
				       TRUE);

	show_tabs1 = gtk_check_menu_item_new_with_label("Show Tabs");
	gtk_widget_ref(show_tabs1);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "show_tabs1",
				 show_tabs1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(show_tabs1);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), show_tabs1);
	gtk_tooltips_set_tip(tooltips, show_tabs1, "Show notebook tabs",
			     NULL);
	view_in_new_window = gtk_menu_item_new_with_label("View in New Window");
	gtk_widget_ref(view_in_new_window);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "view_in_new_window",
				 view_in_new_window,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(view_in_new_window);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), view_in_new_window);
	gtk_tooltips_set_tip(tooltips, view_in_new_window, "View this module in a new window",
			     NULL);
		
	separator22 = gtk_menu_item_new ();
  	gtk_widget_ref (separator22);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "separator22", separator22,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator22);
  	gtk_container_add (GTK_CONTAINER (pmCommentsHtml), separator22);
  	gtk_widget_set_sensitive (separator22, FALSE);		
			     
  	settings->unlockcommmod_item = gtk_menu_item_new_with_label ("Unlock This Module");
  	gtk_widget_ref (settings->unlockcommmod_item);
  	gtk_object_set_data_full (GTK_OBJECT (pmCommentsHtml), "settings->unlockcommmod_item",settings->unlockcommmod_item ,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (settings->unlockcommmod_item);
  	gtk_container_add (GTK_CONTAINER (pmCommentsHtml), settings->unlockcommmod_item);
  			     

	separator22 = gtk_menu_item_new();
	gtk_widget_ref(separator22);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "separator22",
				 separator22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(separator22);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), separator22);
	gtk_widget_set_sensitive(separator22, FALSE);
	
	view_module1 = gtk_menu_item_new_with_label("View Module");
	gtk_widget_ref(view_module1);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "view_module1",
				 view_module1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(view_module1);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), view_module1);

	view_module1_menu = gtk_menu_new();
	gtk_widget_ref(view_module1_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml),
				 "view_module1_menu", view_module1_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_module1),
				  view_module1_menu);
	view_module1_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(view_module1_menu));
	    
	i=0;
	//tmp = dictmods;
	tmp = dictDescription;
	while (tmp != NULL) {
		item3 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		item4 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_widget_ref(item4);
		gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "item3",
					 item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "item4",
					 item4,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);	
		gtk_widget_show(item4);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_word_activate),
				   GINT_TO_POINTER(i));
		gtk_signal_connect(GTK_OBJECT(item4), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_selection_activate),
				   GINT_TO_POINTER(i));
		gtk_container_add(GTK_CONTAINER(lookup_word_menu), item3);
		gtk_container_add(GTK_CONTAINER(lookup_selection_menu), item4);
		   
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	i=0;
	//tmp = mods;
	tmp = comDescription;	
	while (tmp != NULL) {
		item1 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item1);
		gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "item1",
					 item1,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item1);
		gtk_signal_connect(GTK_OBJECT(item1), "activate",
				   GTK_SIGNAL_FUNC(on_com_select_activate),
				   GINT_TO_POINTER(i));

		gtk_container_add(GTK_CONTAINER(view_module1_menu), item1);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	gtk_signal_connect(GTK_OBJECT(print_item), "activate",
			   GTK_SIGNAL_FUNC(on_print_item_activate),
			   (gchar *) "htmlCommentaries");
	gtk_signal_connect(GTK_OBJECT(usecurrent1), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_word_activate),
				   GINT_TO_POINTER(1001));
	gtk_signal_connect(GTK_OBJECT(usecurrent2), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_selection_activate),
				   GINT_TO_POINTER(1001));	
/*	gtk_signal_connect(GTK_OBJECT(add_bookmark), "activate",
			   GTK_SIGNAL_FUNC(on_add_bookmark_activate), (gchar *)"1");*/
	gtk_signal_connect(GTK_OBJECT(copy6), "activate",
			   GTK_SIGNAL_FUNC(on_copyhtml_activate),
			   (gchar *) "htmlCommentaries");
	gtk_signal_connect(GTK_OBJECT(goto_reference), "activate",
			   GTK_SIGNAL_FUNC(on_html_goto_reference_activate),
			   (gchar *) "htmlCommentaries");
	gtk_signal_connect(GTK_OBJECT(about_this_module6), "activate",
			   GTK_SIGNAL_FUNC(on_about_this_module_activate),
			   "commentary");
	gtk_signal_connect(GTK_OBJECT(auto_scroll1), "activate",
			   GTK_SIGNAL_FUNC(on_auto_scroll1_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(show_tabs1), "activate",
			   GTK_SIGNAL_FUNC(on_show_tabs_activate), 
			   (gchar *)"notebook1");
	gtk_signal_connect(GTK_OBJECT(view_in_new_window), "activate",
			   GTK_SIGNAL_FUNC(on_view_in_new_window2_activate),
			   NULL);	
	gtk_signal_connect (GTK_OBJECT (settings->unlockcommmod_item), "activate",
                      	GTK_SIGNAL_FUNC (on_unlock_key_activate),
                      	GINT_TO_POINTER(COMMENTARY_WINDOW));
	gtk_object_set_data(GTK_OBJECT(pmCommentsHtml), "tooltips", tooltips);
	return pmCommentsHtml;
}

//-------------------------------------------------------------------------------------------
static GtkWidget *create_pmDict(GList *modsdesc)
{
	GtkWidget *pmDict;
	GtkWidget *copy5;
	GtkWidget *goto_reference3;
	GtkWidget *lookup_word1;
	GtkWidget *about_this_module5;
	GtkWidget *add_bookmark;
	GtkWidget *add_bookmark_menu;
	GtkWidget *show_tabs1;
	GtkWidget *view_in_new_window;
	GtkWidget *separator23;
	GtkWidget *view_module2;
	GtkWidget *view_module2_menu;
	GtkAccelGroup *view_module2_menu_accels;
	GtkWidget *item3;
	GtkTooltips *tooltips;
	GList *tmp = NULL;
	gint i = 0;
	
	tooltips = gtk_tooltips_new();
	pmDict = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmDict), "pmDict", pmDict);

	copy5 = gtk_menu_item_new_with_label("Copy");
	gtk_widget_ref(copy5);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "copy5", copy5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(copy5);
	gtk_container_add(GTK_CONTAINER(pmDict), copy5);

	goto_reference3 = gtk_menu_item_new_with_label("Goto Reference");
	gtk_widget_ref(goto_reference3);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "goto_reference3",
				 goto_reference3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(goto_reference3);
	gtk_container_add(GTK_CONTAINER(pmDict), goto_reference3);

	lookup_word1 = gtk_menu_item_new_with_label("Lookup Word");
	gtk_widget_ref(lookup_word1);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "lookup_word1",
				 lookup_word1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lookup_word1);
	gtk_container_add(GTK_CONTAINER(pmDict), lookup_word1);

	add_bookmark = gtk_menu_item_new_with_label("Add Bookmark");
	gtk_widget_ref(add_bookmark);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "add_bookmark",
				 add_bookmark, 
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(add_bookmark);
	gtk_container_add(GTK_CONTAINER(pmDict), add_bookmark);
	gtk_tooltips_set_tip(tooltips, add_bookmark, "Add dict/lex key and module to Presonal Bookmarks",
			     NULL);	
	
	add_bookmark_menu = gtk_menu_new ();
  	gtk_widget_ref (add_bookmark_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmDict), "add_bookmark_menu", add_bookmark_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (add_bookmark), add_bookmark_menu);
	/*** add bookmark items ***/
	create_addBookmarkMenuBM(pmDict, add_bookmark_menu,settings);	
	
	about_this_module5 =
	    gtk_menu_item_new_with_label("About this module");
	gtk_widget_ref(about_this_module5);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "about_this_module5",
				 about_this_module5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(about_this_module5);
	gtk_container_add(GTK_CONTAINER(pmDict), about_this_module5);

	show_tabs1 = gtk_check_menu_item_new_with_label("Show Tabs");
	gtk_widget_ref(show_tabs1);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "show_tabs1",
				 show_tabs1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(show_tabs1);
	gtk_container_add(GTK_CONTAINER(pmDict), show_tabs1);
	gtk_tooltips_set_tip(tooltips, show_tabs1, "Show notebook tabs",
			     NULL);
			
        view_in_new_window = gtk_menu_item_new_with_label("View in New Window");
	gtk_widget_ref(view_in_new_window);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "view_in_new_window",
				 show_tabs1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(view_in_new_window);
	gtk_container_add(GTK_CONTAINER(pmDict), view_in_new_window);
	gtk_tooltips_set_tip(tooltips, view_in_new_window, "View this module in a new window",
			     NULL);

	separator23 = gtk_menu_item_new();
	gtk_widget_ref(separator23);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "separator23",
				 separator23,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(separator23);
	gtk_container_add(GTK_CONTAINER(pmDict), separator23);
	gtk_widget_set_sensitive(separator23, FALSE);

	view_module2 = gtk_menu_item_new_with_label("View Module");
	gtk_widget_ref(view_module2);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "view_module2",
				 view_module2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(view_module2);
	gtk_container_add(GTK_CONTAINER(pmDict), view_module2);

	view_module2_menu = gtk_menu_new();
	gtk_widget_ref(view_module2_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmDict), "view_module2_menu",
				 view_module2_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_module2),
				  view_module2_menu);
	view_module2_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(view_module2_menu));
	 
	tmp = modsdesc;
	while (tmp != NULL) {
		item3 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_object_set_data_full(GTK_OBJECT(pmDict), "item3",
					 item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_dict_select_activate),
				   GINT_TO_POINTER(i));

		gtk_container_add(GTK_CONTAINER(view_module2_menu), item3);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
  	gtk_signal_connect (GTK_OBJECT (copy5), "activate",
                      	GTK_SIGNAL_FUNC (on_copyhtml_activate),
                      	(gchar *)"htmlDict");
          gtk_signal_connect (GTK_OBJECT (goto_reference3), "activate",
                      	GTK_SIGNAL_FUNC (on_html_goto_reference_activate),
                      	(gchar *)"htmlDict");   
	gtk_signal_connect(GTK_OBJECT(lookup_word1), "activate",
			   GTK_SIGNAL_FUNC(on_html_lookup_selection_activate),
			   (gchar *)"htmlDict");    
	gtk_signal_connect(GTK_OBJECT(about_this_module5), "activate",
			   GTK_SIGNAL_FUNC(on_about_this_module_activate),
			   "dictionary");
	gtk_signal_connect(GTK_OBJECT(view_in_new_window), "activate",
			   GTK_SIGNAL_FUNC(on_view_in_new_window_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(show_tabs1), "activate",
			   GTK_SIGNAL_FUNC(on_show_tabs_activate), (gchar *)"notebook4");
	return pmDict;
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
	GtkWidget *lookup_word;
	GtkWidget *lookup_word_menu;
	GtkWidget *usecurrent1;
	GtkWidget *separator1;
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

	copy7 = gtk_menu_item_new_with_label ("Copy");
	gtk_widget_ref (copy7);
	gtk_object_set_data_full (GTK_OBJECT (pmBible), "copy7", copy7,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (copy7);
	gtk_container_add (GTK_CONTAINER (pmBible), copy7);
	
	lookup_word = gtk_menu_item_new_with_label ("Lookup Word");
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
	
	usecurrent1 = gtk_menu_item_new_with_label ("Use Current Dictionary");
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
	
	lookup_selection = gtk_menu_item_new_with_label ("Lookup Selection");
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
	
	usecurrent2 = gtk_menu_item_new_with_label ("Use Current Dictionary");
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
	add_bookmark = gtk_menu_item_new_with_label("Add Bookmark");
	gtk_widget_ref(add_bookmark);
	gtk_object_set_data_full(GTK_OBJECT(pmBible), "add_bookmark",
				 add_bookmark, 
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(add_bookmark);
	gtk_container_add(GTK_CONTAINER(pmBible), add_bookmark);
	gtk_tooltips_set_tip(tooltips, add_bookmark, "Add current verse to Presonal Bookmarks",
			     NULL);
	
	add_bookmark_menu = gtk_menu_new ();
  	gtk_widget_ref (add_bookmark_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "add_bookmark_menu", add_bookmark_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (add_bookmark), add_bookmark_menu);
	/*** add bookmark items ***/
	create_addBookmarkMenuBM(pmBible, add_bookmark_menu,settings);
	
	show_tabs = gtk_check_menu_item_new_with_label("Show Tabs");
	gtk_widget_ref(show_tabs);
	gtk_object_set_data_full(GTK_OBJECT(pmBible), "show_tabs",
				 show_tabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(show_tabs);
	gtk_container_add(GTK_CONTAINER(pmBible), show_tabs);
	gtk_tooltips_set_tip(tooltips, show_tabs, "Show notebook tabs",
			     NULL);
				     
  	about_this_module1 = gtk_menu_item_new_with_label ("About this module");
  	gtk_widget_ref (about_this_module1);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "about_this_module1", about_this_module1,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (about_this_module1);
  	gtk_container_add (GTK_CONTAINER (pmBible), about_this_module1);

	viewtext = gtk_menu_item_new_with_label ("View Text in new window");
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
			     
  	settings->unlocktextmod_item = gtk_menu_item_new_with_label ("Unlock This Module");
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

  	view_module3 = gtk_menu_item_new_with_label ("View Module");
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

	i=0;
	tmp = dictDescription;
	while (tmp != NULL) {
		item3 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		item4 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_widget_ref(item4);
		gtk_object_set_data_full(GTK_OBJECT(pmBible), "item3",
					 item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_object_set_data_full(GTK_OBJECT(pmBible), "item4",
					 item4,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);	
		gtk_widget_show(item4);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_word_activate),
				   GINT_TO_POINTER(i));
		gtk_signal_connect(GTK_OBJECT(item4), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_selection_activate),
				   GINT_TO_POINTER(i));
		gtk_container_add(GTK_CONTAINER(lookup_word_menu), item3);
		gtk_container_add(GTK_CONTAINER(lookup_selection_menu), item4);
		++i;
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
	gtk_signal_connect(GTK_OBJECT(usecurrent1), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_word_activate),
				   GINT_TO_POINTER(1001));
	gtk_signal_connect(GTK_OBJECT(usecurrent2), "activate",
				   GTK_SIGNAL_FUNC
				   (on_html_lookup_selection_activate),
				   GINT_TO_POINTER(1001));
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
	gtk_signal_connect (GTK_OBJECT (settings->unlocktextmod_item), "activate",
                      	GTK_SIGNAL_FUNC (on_unlock_key_activate),
                      	GINT_TO_POINTER(MAIN_TEXT_WINDOW));
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
create_pmInt(GList *mods, GList *options, gchar *intWindow)
{
	GtkWidget *pmInt;
	GtkAccelGroup *pmInt_accels;
	GtkWidget *copy7;
	GtkWidget *lang_options;
	GtkWidget *separator2;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();
	pmInt = gtk_menu_new ();
	gtk_object_set_data (GTK_OBJECT (pmInt), "pmInt", pmInt);
	pmInt_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (pmInt));

	copy7 = gtk_menu_item_new_with_label ("Copy");
	gtk_widget_ref (copy7);
	gtk_object_set_data_full (GTK_OBJECT (pmInt), "copy7", copy7,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (copy7);
	gtk_container_add (GTK_CONTAINER (pmInt), copy7);

	lang_options = gtk_menu_item_new_with_label ("Language Options");
	gtk_widget_ref(lang_options);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "lang_options",lang_options ,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (lang_options);
  	gtk_container_add (GTK_CONTAINER(pmInt), lang_options);
	
	lang_options_menu = gtk_menu_new ();
  	gtk_widget_ref(lang_options_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "lang_options_menu",lang_options_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lang_options), lang_options_menu);
	
  	separator2 = gtk_menu_item_new ();
  	gtk_widget_ref (separator2);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator2);
  	gtk_container_add (GTK_CONTAINER (pmInt), separator2);
  	gtk_widget_set_sensitive (separator2, FALSE);
  	/* build change interlinear modules submenu */
	loadmenuformmodlist(pmInt, mods, "Change Interlinear 1", (GtkMenuCallback)on_changeint1mod_activate );  	
	loadmenuformmodlist(pmInt, mods, "Change Interlinear 2", (GtkMenuCallback)on_changeint2mod_activate );	
	loadmenuformmodlist(pmInt, mods, "Change Interlinear 3", (GtkMenuCallback)on_changeint3mod_activate );  	
	loadmenuformmodlist(pmInt, mods, "Change Interlinear 4", (GtkMenuCallback)on_changeint4mod_activate );
	loadmenuformmodlist(pmInt, mods, "Change Interlinear 5", (GtkMenuCallback)on_changeint5mod_activate );

  	gtk_signal_connect (GTK_OBJECT (copy7), "activate",
                      	GTK_SIGNAL_FUNC (on_copyhtml_activate),
                      	(gchar *)intWindow);
			
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

/*
static void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer data)
{
		
}
static void
on_paste1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer data)
{
		
}

static GnomeUIInfo edit2_menu_uiinfo[] = {
	GNOMEUIINFO_MENU_CUT_ITEM(on_cut1_activate, NULL),
	GNOMEUIINFO_MENU_COPY_ITEM(on_copy4_activate, NULL),
	GNOMEUIINFO_MENU_PASTE_ITEM(on_paste1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo pmEditnote_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EDIT_TREE(edit2_menu_uiinfo),
	{
	 GNOME_APP_UI_ITEM, "Bold",
	 "Make selected text bold",
	 on_boldNE_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "Italic",
	 "Make selected text Italic",
	 on_italicNE_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "Reference",
	 "Make selected text a reference",
	 on_referenceNE_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "Underline",
	 "Underline selected text",
	 on_underlineNE_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "Greek",
	 "Use greek font",
	 on_greekNE_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, "Goto Reference",
	 "Go to selected reference",
	 on_goto_reference_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_JUMP_TO,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, "Lookup Selection",
	 NULL,
	 on_lookup_selection4_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_SEARCH,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GtkWidget *create_pmEditnote(GtkWidget *app, GList *mods)
{
	GtkWidget *pmEditnote;
	GList *tmp;
	
	pmEditnote = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmEditnote), "pmEditnote",
			    pmEditnote);
	gnome_app_fill_menu(GTK_MENU_SHELL(pmEditnote), pmEditnote_uiinfo,
			    NULL, FALSE, 0);

	gtk_widget_ref(pmEditnote_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "separator19",
				 pmEditnote_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmEditnote_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "edit2",
				 pmEditnote_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit2_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "cut1",
				 edit2_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit2_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "copy4",
				 edit2_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit2_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "paste1",
				 edit2_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmEditnote_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "boldNE",
				 pmEditnote_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmEditnote_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "italicNE",
				 pmEditnote_uiinfo[3].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmEditnote_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "referenceNE",
				 pmEditnote_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmEditnote_uiinfo[5].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "underlineNE",
				 pmEditnote_uiinfo[5].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmEditnote_uiinfo[6].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "greekNE",
				 pmEditnote_uiinfo[6].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmEditnote_uiinfo[7].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "separator18",
				 pmEditnote_uiinfo[7].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmEditnote_uiinfo[8].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote), "goto_reference",
				 pmEditnote_uiinfo[8].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmEditnote_uiinfo[9].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmEditnote),
				 "lookup_selection4",
				 pmEditnote_uiinfo[9].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	tmp = mods;	
	while (tmp != NULL) { 
		additemtopopupmenu(app, pmEditnote,(gchar *) tmp->data , 
			(GtkMenuCallback)on_change_module_activate); 
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	return pmEditnote;
}

*/

