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
 
#include <gnome.h>
#include "menustuff.h"
#include "callback.h"

//----------------------------------------------------------------------------------
void
additemtognomemenu(GtkWidget *MainFrm, gchar *itemname, gchar *menuname, GtkMenuCallback mycallback)
{
	GnomeUIInfo *menuitem;
	
	menuitem = g_new(GnomeUIInfo,2); 
	menuitem->type = GNOME_APP_UI_ITEM;
	menuitem->moreinfo=(gpointer)mycallback;
	menuitem->user_data = itemname;
	menuitem->label = itemname;
	menuitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
	menuitem->pixmap_info =GNOME_STOCK_MENU_BOOK_OPEN;
	menuitem->accelerator_key = 0;
	menuitem[1].type=GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm),menuname,menuitem,NULL);  
}

//----------------------------------------------------------------------------------
void
additemtopopupmenu(GtkWidget *MainFrm, GtkWidget *menu, gchar *itemname, GtkMenuCallback mycallback)
{
	GtkWidget *menuChoice;
	gchar menuName[64];
	int viewNumber(1);
		
	menuChoice = gtk_menu_item_new_with_label (itemname);	 //----------- popup menu		
	sprintf(menuName, "viewMod%d", viewNumber++);			
	gtk_object_set_data (GTK_OBJECT (MainFrm), menuName, menuChoice);			
	gtk_widget_show (menuChoice);	
	gtk_signal_connect(GTK_OBJECT (menuChoice), "activate",
			GTK_SIGNAL_FUNC (mycallback),
			      g_strdup(itemname));	
	gtk_container_add (GTK_CONTAINER (menu), menuChoice); 	
}

//-------------------------------------------------------------------------------------------
void
addsubtreeitem(GtkWidget *MainFrm, gchar *menulabel, gchar *subtreelabel)
{
	static GnomeUIInfo marks1_menu_uiinfo[] =   // this struct is here to keep from getting compiler error
	{  															  // 
  		GNOMEUIINFO_SEPARATOR,
  		GNOMEUIINFO_END
	};
	GnomeUIInfo *bookmarkitem;
	bookmarkitem = g_new(GnomeUIInfo,2);
	bookmarkitem->type = GNOME_APP_UI_SUBTREE;
	bookmarkitem->moreinfo=marks1_menu_uiinfo;
	bookmarkitem->user_data=NULL;
	bookmarkitem->label = subtreelabel;
	bookmarkitem->pixmap_type = GNOME_APP_PIXMAP_NONE;
	bookmarkitem->pixmap_info = NULL;
	bookmarkitem->accelerator_key = 0;
	bookmarkitem[1].type=GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm),menulabel,bookmarkitem,NULL);	
}

//-------------------------------------------------------------------------------------------
void
additemtosubtree(GtkWidget *MainFrm, gchar *subtreelabel, gchar *itemlabel )
{
	GnomeUIInfo *bookmarkitem;

	bookmarkitem = g_new(GnomeUIInfo,2);
	bookmarkitem->type = GNOME_APP_UI_ITEM;
	bookmarkitem->moreinfo=(gpointer)on_john_3_1_activate;
	bookmarkitem->user_data=g_strdup(itemlabel);
	bookmarkitem->label = itemlabel;
	bookmarkitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
	bookmarkitem->pixmap_info =GNOME_STOCK_MENU_BOOK_OPEN;
	bookmarkitem->accelerator_key = 0;
	bookmarkitem[1].type=GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm),subtreelabel,bookmarkitem,NULL);	
}

//-------------------------------------------------------------------------------------------
//----------------- we need to return a widget so we can use it later as a toggle menu item ----------------------//
GtkWidget *
additemtooptionmenu(GtkWidget *MainFrm, gchar *subtreelabel, gchar *itemlabel, GtkMenuCallback mycallback)
{
	GtkWidget *item;
	GnomeUIInfo *menuitem;

	menuitem = g_new(GnomeUIInfo,2);
	menuitem->type = GNOME_APP_UI_TOGGLEITEM;
	menuitem->user_data=NULL;
	menuitem->pixmap_type = GNOME_APP_PIXMAP_NONE;
	menuitem->pixmap_info =NULL;
	menuitem->accelerator_key = 0;
	menuitem->label =  itemlabel;
	menuitem->moreinfo=(gpointer)mycallback;
	menuitem[1].type=GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm),subtreelabel,menuitem,NULL);	
	item = menuitem[0].widget;

	return(item);
}

