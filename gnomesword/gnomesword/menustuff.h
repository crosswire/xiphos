/***************************************************************************
                          menustuff.h  -  description
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by  Terry Biggs
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
  
#ifdef __cplusplus
extern "C" {
#endif

#include "gs_sword.h"

void
additemtognomemenu(GtkWidget * MainFrm,
		   gchar * itemname,
		   gchar * itemdata,
		   gchar * submenuname, GtkMenuCallback mycallback);

void
additemtopopupmenu(GtkWidget * MainFrm,
		   GtkWidget * menu,
		   gchar * itemname, GtkMenuCallback mycallback);

void addseparator(GtkWidget * MainFrm, gchar * subtreelabel);

void
addsubtreeitem(GtkWidget * MainFrm,
	       gchar * menulabel, gchar * subtreelabel);

void
additemtosubtree(GtkWidget * MainFrm,
		 gchar * subtreelabel, gchar * itemlabel);

GtkWidget *additemtooptionmenu(GtkWidget * MainFrm,
			       gchar * subtreelabel,
			       gchar * itemlabel,
			       GtkMenuCallback mycallback);

void
removemenuitems(GtkWidget * MainFrm,
		gchar * startitem, gint numberofitems);
		
void createpopupmenus(GtkWidget *app, SETTINGS *settings,GList *biblelist, 
		GList *commentarylist, GList *dictionarylist,
		GList *percomlist);
		
void addmodstomenus(GtkWidget *app, SETTINGS *settings,GList *biblelist, 
		GList *commentarylist, GList *dictionarylist,
		GList *percomlist);		

#ifdef __cplusplus
}
#endif
