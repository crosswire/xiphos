/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          gs_menu.cpp  -  description
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
#include "interface.h"
#include "callback.h"
#include "gs_sword.h"
#include "gs_popup_cb.h"
#include "gs_html.h"
#include "support.h"

/******************************************************************************
 * function prototypes only visible to this file
 ******************************************************************************/
static GtkWidget* create_pmInt(GList *mods, gchar *intWindow, 
			GtkMenuCallback cbchangemod, 
			GtkMenuCallback mycallback);
static GtkWidget *create_pmComments2(GList * mods);
static GtkWidget *create_pmCommentsHtml(GList * mods,
				GList *comDescription);
static GtkWidget *create_pmDict(GList * mods, GList *modsdesc);
static GtkWidget* create_pmBible(GList *mods, 
				GList *bibleDescription);
static GtkWidget *create_pmEditnote(GtkWidget *app, 
		GList *mods);
static void loadmenuformmodlist(GtkWidget *pmInt, 
		GList *mods, 
		gchar *labelGtkWidget, 
		GtkMenuCallback mycallback);
/******************************************************************************/


extern gint	greekpage,
        	hebrewpage;
extern GS_TABS	*p_tabs;
		
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
				GList *biblelist, 
				GList *bibleDescription,
				GList *commentarylist,
				GList *comDescription, 
				GList *dictionarylist,
				GList *dictDescription,
				GList *percomlist) 
{
	GtkWidget 
		*menu2, 
		//*menu3, 
		//*menu4, 
		*menu5,
		*menuCom,
		*menuDict,
		*menuBible,
		*menuhtmlcom;	
	
	menu2 = create_pmInt(biblelist, "textComp1",
				(GtkMenuCallback)on_1st_interlinear_window1_activate,
				(GtkMenuCallback)on_about_this_module_activate); /*** not using this callback ***/
	/*menu3 =create_pmInt(biblelist, "textComp2",
				(GtkMenuCallback)on_2nd_interlinear_window1_activate,
				(GtkMenuCallback)on_about_this_module_activate); */
	/*menu4 =create_pmInt(biblelist, "textComp3",
				(GtkMenuCallback)on_3rd_interlinear_window1_activate,
				(GtkMenuCallback)on_about_this_module_activate); */
	menu5 = create_pmEditnote(app, percomlist);
	/* create pop menu for commentaries */
	menuCom = create_pmComments2(commentarylist);

	/* create popup menu for dict/lex window */
	menuDict = create_pmDict(dictionarylist, dictDescription);
	/* create popup menu for Bible window */
	menuBible = create_pmBible(biblelist, bibleDescription);	
		
	/* attach popup menus */
	gnome_popup_menu_attach(menu2,lookup_widget(app,"textComp1"),(gchar*)"1");	
	gnome_popup_menu_attach(menu5,lookup_widget(app,"textComments"),(gchar*)"1");	
	GTK_CHECK_MENU_ITEM (lookup_widget(menuDict,"show_tabs1"))->active = p_tabs->dictwindow;
	gnome_popup_menu_attach(menuCom,lookup_widget(app,"textCommentaries"),(gchar*)"1");
	GTK_CHECK_MENU_ITEM (lookup_widget(menuCom,"show_tabs1"))->active = p_tabs->commwindow;
	gnome_popup_menu_attach(menuBible,lookup_widget(app,"htmlTexts"),(gchar*)"1");
	menuhtmlcom = create_pmCommentsHtml(commentarylist, comDescription);	
	gnome_popup_menu_attach(menuhtmlcom,lookup_widget(app,"htmlCommentaries"),(gchar*)"1");
	gnome_popup_menu_attach(menuDict,lookup_widget(app,"htmlDict"),(gchar*)"1");
	GTK_CHECK_MENU_ITEM (lookup_widget(menuhtmlcom,"show_tabs1"))->active = p_tabs->commwindow;

}

/******************************************************************************
 * addmodsmenus(GList *, GList *, GList *) - add modules to view menus
 * app - main window
 * settings - gnomesword settings structure
 * biblelist - list of Bible modules - from initSword
 * commentarylist - list of commentary modules - from initSword
 * dictionarylist - list of dict/lex modules - from initSword 
 * percomlist - list of personal comments modules
 ******************************************************************************/
void addmodstomenus(GtkWidget *app, 
				SETTINGS *settings, 
				GList *biblelist, 
				GList *bibleDescription,
				GList *commentarylist, 
				GList *dictionarylist, 
				GList *comDescription,
				GList *dictDescription,
				GList *percomlist) 
{
	gchar	rememberlastitem[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem2[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem3[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		rememberlastitemCom[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		rememberlastitemDict[80], //--  use to store last menu item so we can add the next item under it - gnome menus	
		mybuf[80];
	gint	pg = 0;
	GList *tmp = NULL;
	//GList *tmp2 = NULL;

//-- set main window modules and add to menus	
	sprintf(rememberlastitem,"%s","_View/Main Window/");
	sprintf(rememberlastitemCom,"%s","_View/Commentary Window/");
	sprintf(rememberlastitemDict,"%s","_View/Dict-Lex Window/");
	sprintf(aboutrememberlastitem,"%s","_Help/About Sword Modules/Bible Texts/<Separator>");
	sprintf(aboutrememberlastitem2,"%s","_Help/About Sword Modules/Commentaries/<Separator>");
	sprintf(aboutrememberlastitem3,"%s","_Help/About Sword Modules/Dictionaries-Lexicons/<Separator>");
//-- add textmods - Main Window menu
	tmp = biblelist;
	//tmp2 = bibleDescription;
	while (tmp != NULL) {	
	//-- add to menubar
		additemtognomemenu(app, (gchar *) tmp->data, (gchar *) tmp->data, rememberlastitem , (GtkMenuCallback)on_mainText_activate );
		additemtognomemenu(app, (gchar *) tmp->data, (gchar *) tmp->data, aboutrememberlastitem , (GtkMenuCallback)on_kjv1_activate );
		//-- remember last item - so next item will be place below it       	
		sprintf(rememberlastitem,"%s%s","_View/Main Window/",(gchar *) tmp->data);	
		sprintf(aboutrememberlastitem,"%s%s","_Help/About Sword Modules/Bible Texts/", (gchar *) tmp->data);			
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
//-- add commmods - commentary window menu
	tmp = commentarylist;
	while (tmp != NULL) {		
		sprintf(mybuf,"%d",pg);
		additemtognomemenu(app, (gchar *) tmp->data, mybuf, rememberlastitemCom, 
				(GtkMenuCallback)on_com_select_activate);			
		sprintf(rememberlastitemCom,"%s%s","_View/Commentary Window/",
				(gchar *) tmp->data);	
		additemtognomemenu(app,(gchar *) tmp->data, 
				(gchar *) tmp->data, aboutrememberlastitem2, 
				(GtkMenuCallback)on_kjv1_activate );			
		sprintf(aboutrememberlastitem2,"%s%s","_Help/About Sword Modules/Commentaries/",
				(gchar *) tmp->data);
		++pg;
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
		additemtognomemenu(app, (gchar *) tmp->data, mybuf, rememberlastitemDict,
				(GtkMenuCallback)on_dict_select_activate );		
		sprintf(rememberlastitemDict,"%s%s","_View/Dict-Lex Window/",
				(gchar *) tmp->data);		
		additemtognomemenu(app,(gchar *) tmp->data, 
				(gchar *) tmp->data, aboutrememberlastitem3,
				(GtkMenuCallback)on_kjv1_activate );
		sprintf(aboutrememberlastitem3,"%s%s","_Help/About Sword Modules/Dictionaries-Lexicons/",
				(gchar *) tmp->data);				
		++pg;
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	
//-- add interlin1mods - interliniar1 window menu	
	sprintf(rememberlastitem,"%s","_View/Interlinear1 Window/");
	tmp = biblelist;
	while (tmp != NULL) {
		//-- add to menubar
		additemtognomemenu(app, (gchar *) tmp->data, (gchar *) tmp->data,rememberlastitem ,
					 (GtkMenuCallback)on_changeint1mod_activate );
		//-- remember last item - so next item will be place below it       	
		sprintf(rememberlastitem,"%s%s","_View/Interlinear1 Window/", (gchar *) tmp->data);		
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	
//-- add interlin2mods - interliniar2 window menu	
	sprintf(rememberlastitem,"%s","_View/Interlinear2 Window/");
	tmp = biblelist;
	while (tmp != NULL) {
		//-- add to menubar
		additemtognomemenu(app, (gchar *) tmp->data, (gchar *) tmp->data,rememberlastitem ,
					 (GtkMenuCallback)on_changeint2mod_activate);
		//-- remember last item - so next item will be place below it       	
		sprintf(rememberlastitem,"%s%s","_View/Interlinear2 Window/", (gchar *) tmp->data);		
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);	
//-- add interlin3mods - interliniar3 window menu	
	sprintf(rememberlastitem,"%s","_View/Interlinear3 Window/");
	tmp = biblelist;
	while (tmp != NULL) {
		//-- add to menubar
		additemtognomemenu(app, (gchar *) tmp->data, (gchar *) tmp->data,rememberlastitem ,
					 (GtkMenuCallback)on_changeint3mod_activate);
		//-- remember last item - so next item will be place below it       	
		sprintf(rememberlastitem,"%s%s","_View/Interlinear3 Window/", (gchar *) tmp->data);		
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);	
//-- add interlin4mods - interliniar4 window menu	
	sprintf(rememberlastitem,"%s","_View/Interlinear4 Window/");
	tmp = biblelist;
	while (tmp != NULL) {
		//-- add to menubar
		additemtognomemenu(app, (gchar *) tmp->data, (gchar *) tmp->data,rememberlastitem ,
					 (GtkMenuCallback)on_changeint4mod_activate);
		//-- remember last item - so next item will be place below it       	
		sprintf(rememberlastitem,"%s%s","_View/Interlinear4 Window/", (gchar *) tmp->data);		
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);	
//-- add interlin5mods - interliniar5 window menu	
	sprintf(rememberlastitem,"%s","_View/Interlinear5 Window/");
	tmp = biblelist;
	while (tmp != NULL) {
		//-- add to menubar
		additemtognomemenu(app, (gchar *) tmp->data, (gchar *) tmp->data,rememberlastitem ,
					 (GtkMenuCallback)on_changeint5mod_activate);
		//-- remember last item - so next item will be place below it       	
		sprintf(rememberlastitem,"%s%s","_View/Interlinear5 Window/", (gchar *) tmp->data);		
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
static GtkWidget *create_pmComments2(GList * mods)
{
	GtkWidget *pmComments2;
	GtkWidget *copy6;
	GtkWidget *goto_reference2;
	GtkWidget *lookup_selection2;
	GtkWidget *about_this_module6;
	GtkWidget *auto_scroll1;
	GtkWidget *show_tabs1;
	GtkWidget *view_module1;
	GtkWidget *view_module1_menu;
	GtkAccelGroup *view_module1_menu_accels;
	GtkWidget *separator22, *item1;
	GtkTooltips *tooltips;
	GList *tmp = NULL;
	gint i = 0;
	gchar buf[80];
	
	tooltips = gtk_tooltips_new();

	pmComments2 = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmComments2), "pmComments2",
			    pmComments2);
			
	copy6 = gtk_menu_item_new_with_label("Copy");
	gtk_widget_ref(copy6);
	gtk_object_set_data_full(GTK_OBJECT(pmComments2), "copy6", copy6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(copy6);
	gtk_container_add(GTK_CONTAINER(pmComments2), copy6);

	goto_reference2 = gtk_menu_item_new_with_label("Goto Reference");
	gtk_widget_ref(goto_reference2);
	gtk_object_set_data_full(GTK_OBJECT(pmComments2),
				 "goto_reference2", goto_reference2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(goto_reference2);
	gtk_container_add(GTK_CONTAINER(pmComments2), goto_reference2);

	lookup_selection2 =
	    gtk_menu_item_new_with_label("Lookup Selection");
	gtk_widget_ref(lookup_selection2);
	gtk_object_set_data_full(GTK_OBJECT(pmComments2),
				 "lookup_selection2", lookup_selection2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lookup_selection2);
	gtk_container_add(GTK_CONTAINER(pmComments2), lookup_selection2);

	about_this_module6 =
	    gtk_menu_item_new_with_label("About this module");
	gtk_widget_ref(about_this_module6);
	gtk_object_set_data_full(GTK_OBJECT(pmComments2),
				 "about_this_module6", about_this_module6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(about_this_module6);
	gtk_container_add(GTK_CONTAINER(pmComments2), about_this_module6);

	auto_scroll1 = gtk_check_menu_item_new_with_label("Auto Scroll");
	gtk_widget_ref(auto_scroll1);
	gtk_object_set_data_full(GTK_OBJECT(pmComments2), "auto_scroll1",
				 auto_scroll1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(auto_scroll1);
	gtk_container_add(GTK_CONTAINER(pmComments2), auto_scroll1);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(auto_scroll1),
				       TRUE);

	show_tabs1 = gtk_check_menu_item_new_with_label("Show Tabs");
	gtk_widget_ref(show_tabs1);
	gtk_object_set_data_full(GTK_OBJECT(pmComments2), "show_tabs1",
				 show_tabs1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(show_tabs1);
	gtk_container_add(GTK_CONTAINER(pmComments2), show_tabs1);
	gtk_tooltips_set_tip(tooltips, show_tabs1, "Show notebook tabs",
			     NULL);

	view_module1 = gtk_menu_item_new_with_label("View Module");
	gtk_widget_ref(view_module1);
	gtk_object_set_data_full(GTK_OBJECT(pmComments2), "view_module1",
				 view_module1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(view_module1);
	gtk_container_add(GTK_CONTAINER(pmComments2), view_module1);

	view_module1_menu = gtk_menu_new();
	gtk_widget_ref(view_module1_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmComments2),
				 "view_module1_menu", view_module1_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_module1),
				  view_module1_menu);
	view_module1_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(view_module1_menu));

	separator22 = gtk_menu_item_new();
	gtk_widget_ref(separator22);
	gtk_object_set_data_full(GTK_OBJECT(pmComments2), "separator22",
				 separator22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(separator22);
	gtk_container_add(GTK_CONTAINER(view_module1_menu), separator22);
	gtk_widget_set_sensitive(separator22, FALSE);


	tmp = mods;
	
	while (tmp != NULL) {
		item1 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item1);
		gtk_object_set_data_full(GTK_OBJECT(pmComments2), "item1",
					 item1,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item1);
		sprintf(buf, "%d", i);
		gtk_signal_connect(GTK_OBJECT(item1), "activate",
				   GTK_SIGNAL_FUNC(on_com_select_activate),
				   g_strdup((gchar *) buf));

		gtk_container_add(GTK_CONTAINER(view_module1_menu), item1);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);


	gtk_signal_connect(GTK_OBJECT(copy6), "activate",
			   GTK_SIGNAL_FUNC(on_copy3_activate),
			   (gchar *) "textCommentaries");
	gtk_signal_connect(GTK_OBJECT(goto_reference2), "activate",
			   GTK_SIGNAL_FUNC(on_goto_reference2_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(lookup_selection2), "activate",
			   GTK_SIGNAL_FUNC(on_lookup_selection2_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(about_this_module6), "activate",
			   GTK_SIGNAL_FUNC(on_about_this_module_activate),
			   "commentary");

	gtk_signal_connect(GTK_OBJECT(auto_scroll1), "activate",
			   GTK_SIGNAL_FUNC(on_auto_scroll1_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(show_tabs1), "activate",
			   GTK_SIGNAL_FUNC(on_show_tabs_activate), NULL);

	gtk_object_set_data(GTK_OBJECT(pmComments2), "tooltips", tooltips);
	return pmComments2;
}

//-------------------------------------------------------------------------------------------
static GtkWidget *create_pmCommentsHtml(GList * mods, GList *comDescription)
{
	GtkWidget *pmCommentsHtml;
	GtkWidget *copy6;
	GtkWidget *goto_reference;
	//GtkWidget *goto_reference_menu;
	//GtkAccelGroup *goto_reference_menu_accels;
	GtkWidget *lookup_selection2;
	GtkWidget *add_bookmark;
	GtkWidget *about_this_module6;
	GtkWidget *auto_scroll1;
	GtkWidget *show_tabs1;
	GtkWidget *view_in_new_window;
	GtkWidget *view_module1;
	GtkWidget *view_module1_menu;
	GtkAccelGroup *view_module1_menu_accels;
	GtkWidget *separator22, *item1;
	GtkTooltips *tooltips;
	GList *tmp = NULL;
	GList *tmp2 = NULL;
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

	goto_reference = gtk_menu_item_new_with_label("Goto Reference");
	gtk_widget_ref(goto_reference);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml),
				 "goto_reference", goto_reference,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(goto_reference);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), goto_reference);
	/*
	goto_reference_menu = gtk_menu_new();
	gtk_widget_ref(goto_reference_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml),
				 "goto_reference_menu", goto_reference_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(goto_reference),
				  goto_reference_menu);
	goto_reference_menu_accels=
	gtk_menu_ensure_uline_accel_group(GTK_MENU(goto_reference_menu_accels));
	*/

	lookup_selection2 =
	    gtk_menu_item_new_with_label("Lookup Selection");
	gtk_widget_ref(lookup_selection2);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml),
				 "lookup_selection2", lookup_selection2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lookup_selection2);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), lookup_selection2);

	add_bookmark = gtk_menu_item_new_with_label("Add Bookmark");
	gtk_widget_ref(add_bookmark);
	gtk_object_set_data_full(GTK_OBJECT(pmCommentsHtml), "add_bookmark",
				 add_bookmark, 
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(add_bookmark);
	gtk_container_add(GTK_CONTAINER(pmCommentsHtml), add_bookmark);
	gtk_tooltips_set_tip(tooltips, add_bookmark, "Add current verse and module to Presonal Bookmarks",
			     NULL);
			     
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

	tmp = mods;
	tmp2 = comDescription;	
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
		gtk_tooltips_set_tip(tooltips, item1, (gchar *) tmp2->data,
			     NULL);
		++i;
		tmp = g_list_next(tmp);
		tmp2 = g_list_next(tmp2);
	}
	g_list_free(tmp);
	g_list_free(tmp2);
	
	gtk_signal_connect(GTK_OBJECT(add_bookmark), "activate",
			   GTK_SIGNAL_FUNC(on_add_bookmark_activate), (gchar *)"1");
	gtk_signal_connect(GTK_OBJECT(copy6), "activate",
			   GTK_SIGNAL_FUNC(on_copyhtml_activate),
			   (gchar *) "htmlCommentaries");
	gtk_signal_connect(GTK_OBJECT(lookup_selection2), "activate",
			   GTK_SIGNAL_FUNC(on_html_lookup_selection_activate),
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
	gtk_object_set_data(GTK_OBJECT(pmCommentsHtml), "tooltips", tooltips);
	return pmCommentsHtml;
}

//-------------------------------------------------------------------------------------------
static GtkWidget *create_pmDict(GList * mods, GList *modsdesc)
{
	GtkWidget *pmDict;
	GtkWidget *copy5;
	GtkWidget *goto_reference3;
	GtkWidget *lookup_word1;
	GtkWidget *about_this_module5;
	GtkWidget *add_bookmark;
	GtkWidget *show_tabs1;
	GtkWidget *view_in_new_window;
	GtkWidget *separator23;
	GtkWidget *view_module2;
	GtkWidget *view_module2_menu;
	GtkAccelGroup *view_module2_menu_accels;
	GtkWidget *item3;
	GtkTooltips *tooltips;
	GList *tmp = NULL;
	GList *tmp2 = NULL;
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
	    
	tmp = mods;
	tmp2 = modsdesc;
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
		gtk_tooltips_set_tip(tooltips, item3, (gchar *) tmp2->data,
			     NULL);
		++i;
		tmp = g_list_next(tmp);
		tmp2 = g_list_next(tmp2);
	}
	g_list_free(tmp);
	g_list_free(tmp2);
  	gtk_signal_connect (GTK_OBJECT (copy5), "activate",
                      	GTK_SIGNAL_FUNC (on_copyhtml_activate),
                      	(gchar *)"htmlDict");
          gtk_signal_connect (GTK_OBJECT (goto_reference3), "activate",
                      	GTK_SIGNAL_FUNC (on_html_goto_reference_activate),
                      	(gchar *)"htmlDict");   
	gtk_signal_connect(GTK_OBJECT(lookup_word1), "activate",
			   GTK_SIGNAL_FUNC(on_html_lookup_selection_activate),
			   (gchar *)"htmlDict");                      	          	
	gtk_signal_connect(GTK_OBJECT(add_bookmark), "activate",
			   GTK_SIGNAL_FUNC(on_add_bookmark_activate), 
				(gchar *)"2");
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
static GtkWidget* create_pmBible(GList *mods, GList *bibleDescription)
{
	GtkWidget *pmBible;
	GtkAccelGroup *pmBible_accels;
	GtkWidget *copy7;
	GtkWidget *show_tabs;
	GtkWidget *lookup_selection;
	GtkWidget *about_this_module1;
	GtkWidget *add_bookmark;
	GtkWidget *separator2;
	GtkWidget *view_module3;
	GtkWidget *view_module3_menu;
	GtkAccelGroup *view_module3_menu_accels;
	GtkWidget *item3;
	GtkTooltips *tooltips;
	GtkWidget *viewtext;
	GList *tmp = NULL;
	GList *tmp2 = NULL;
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

	lookup_selection = gtk_menu_item_new_with_label ("Lookup Selection");
	gtk_widget_ref (lookup_selection);
  	gtk_object_set_data_full (GTK_OBJECT (pmBible), "lookup_selection", lookup_selection,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (lookup_selection);
  	gtk_container_add (GTK_CONTAINER (pmBible), lookup_selection);
	
	add_bookmark = gtk_menu_item_new_with_label("Add Bookmark");
	gtk_widget_ref(add_bookmark);
	gtk_object_set_data_full(GTK_OBJECT(pmBible), "add_bookmark",
				 add_bookmark, 
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(add_bookmark);
	gtk_container_add(GTK_CONTAINER(pmBible), add_bookmark);
	gtk_tooltips_set_tip(tooltips, add_bookmark, "Add current verse to Presonal Bookmarks",
			     NULL);
			     
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
	tmp = mods;
	tmp2 = bibleDescription;
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
		gtk_tooltips_set_tip(tooltips, item3, (gchar *) tmp2->data,
			     NULL);
		++i;
		tmp = g_list_next(tmp);
		tmp2 = g_list_next(tmp2);
	}
	g_list_free(tmp);
	g_list_free(tmp2);
	
	gtk_signal_connect(GTK_OBJECT(add_bookmark), "activate",
			   GTK_SIGNAL_FUNC(on_add_bookmark_activate), (gchar *)"0");
	gtk_signal_connect(GTK_OBJECT(show_tabs), "activate",
			   GTK_SIGNAL_FUNC(on_show_tabs_activate), (gchar *)"nbTextMods");
	
  	gtk_signal_connect (GTK_OBJECT (copy7), "activate",
                      	GTK_SIGNAL_FUNC (on_copyhtml_activate),
                      	(gchar *)"htmlTexts");
          	gtk_signal_connect (GTK_OBJECT (lookup_selection), "activate",
                      	GTK_SIGNAL_FUNC (on_html_lookup_selection_activate),
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
create_pmInt(GList *mods, gchar *intWindow, GtkMenuCallback cbchangemod, 
		GtkMenuCallback mycallback)
{
	GtkWidget *pmInt;
	GtkAccelGroup *pmInt_accels;
	GtkWidget *copy7;
	//GtkWidget *about_this_module1;
	GtkWidget *separator2;


	pmInt = gtk_menu_new ();
	gtk_object_set_data (GTK_OBJECT (pmInt), "pmInt", pmInt);
	pmInt_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (pmInt));

	copy7 = gtk_menu_item_new_with_label ("Copy");
	gtk_widget_ref (copy7);
	gtk_object_set_data_full (GTK_OBJECT (pmInt), "copy7", copy7,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (copy7);
	gtk_container_add (GTK_CONTAINER (pmInt), copy7);
/*
  	about_this_module1 = gtk_menu_item_new_with_label ("About this module");
  	gtk_widget_ref (about_this_module1);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "about_this_module1", about_this_module1,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (about_this_module1);
  	gtk_container_add (GTK_CONTAINER (pmInt), about_this_module1);
*/
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
/*  	gtk_signal_connect (GTK_OBJECT (about_this_module1), "activate",
                      	GTK_SIGNAL_FUNC (mycallback),
                      	NULL);*/
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
	while (tmp != NULL) { /* add personal comments modules to popup menu */
		additemtopopupmenu(app, pmEditnote,(gchar *) tmp->data , 
			(GtkMenuCallback)on_change_module_activate); 
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	return pmEditnote;
}

