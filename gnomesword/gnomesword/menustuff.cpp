/***************************************************************************
                          menustuff.cpp  -  description
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
#include <iostream.h>
#include <gnome.h>
#include "menustuff.h"
#include "interface.h"
#include "callback.h"
#include "listeditor.h"

//----------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------
void
additemtopopupmenu(GtkWidget * MainFrm, GtkWidget * menu, gchar * itemname,
		   GtkMenuCallback mycallback)
{
	GtkWidget *menuChoice;
	gchar menuName[64];
	int viewNumber(1);

	menuChoice = gtk_menu_item_new_with_label(itemname);	//----------- popup menu               
	sprintf(menuName, "viewMod%d", viewNumber++);
	gtk_object_set_data(GTK_OBJECT(MainFrm), menuName, menuChoice);
	gtk_widget_show(menuChoice);
	gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
			   GTK_SIGNAL_FUNC(mycallback),
			   g_strdup(itemname));
	//gtk_container_add (GTK_CONTAINER (menu), menuChoice);         
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),
			      GTK_WIDGET(menuChoice));
}

//-------------------------------------------------------------------------------------------
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
	static GnomeUIInfo marks1_menu_uiinfo[] = {	// this struct is here to keep from getting compiler error                                                                                                                            //
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

//-------------------------------------------------------------------------------------------
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
//----------------- we need to return a widget so we can use it later as a toggle menu item ----------------------//
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
//-- remove a number(numberofitems) of items form-----------------------------------------------------------------//                                                                  //
{				//-- a menu or submenu(startitem)
	gnome_app_remove_menus(GNOME_APP(MainFrm), startitem,
			       numberofitems);
}

//-------------------------------------------------------------------------------------------
GtkWidget *create_pmComments2(GList * mods)
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

	GList *tmp;
	tmp = mods;
	gint i = 0;
	gchar buf[80];
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
			   GTK_SIGNAL_FUNC(on_about_this_module6_activate),
			   NULL);

	gtk_signal_connect(GTK_OBJECT(auto_scroll1), "activate",
			   GTK_SIGNAL_FUNC(on_auto_scroll1_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(show_tabs1), "activate",
			   GTK_SIGNAL_FUNC(on_show_tabs1_activate), NULL);

	gtk_object_set_data(GTK_OBJECT(pmComments2), "tooltips", tooltips);


	return pmComments2;
}

//-------------------------------------------------------------------------------------------
GtkWidget *create_pmDict(GList * mods)
{
	GtkWidget *pmDict;
	GtkWidget *copy5;
	GtkWidget *goto_reference3;
	GtkWidget *lookup_word1;
	GtkWidget *about_this_module5;
	GtkWidget *show_tabs1;
	GtkWidget *separator23;
	GtkWidget *view_module2;
	GtkWidget *view_module2_menu;
	GtkAccelGroup *view_module2_menu_accels;
	GtkWidget *item3;
	GtkTooltips *tooltips;

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
	GList *tmp;
	tmp = mods;
	gint i = 0;
	gchar buf[80];
	while (tmp != NULL) {
		item3 = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_object_set_data_full(GTK_OBJECT(pmDict), "item3",
					 item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);
		sprintf(buf, "%d", i);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_dict_select_activate),
				   g_strdup((gchar *) buf));

		gtk_container_add(GTK_CONTAINER(view_module2_menu), item3);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	gtk_signal_connect(GTK_OBJECT(copy5), "activate",
			   GTK_SIGNAL_FUNC(on_copy3_activate),
			   (gchar *) "textDict");
	gtk_signal_connect(GTK_OBJECT(goto_reference3), "activate",
			   GTK_SIGNAL_FUNC(on_goto_reference3_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(lookup_word1), "activate",
			   GTK_SIGNAL_FUNC(on_lookup_word1_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(about_this_module5), "activate",
			   GTK_SIGNAL_FUNC(on_about_this_module5_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(show_tabs1), "activate",
			   GTK_SIGNAL_FUNC(on_show_tabs2_activate), NULL);
	return pmDict;
}
