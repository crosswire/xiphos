/***************************************************************************
                                    gs_shortcutbar.c
                             -------------------
    begin                : Thu Jun 14 2001
    copyright            : (C) 2001 by tbiggs
    email                : tbiggs@users.sf.net
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
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>
#include  "gs_shortcutbar.h"
#include  "gs_gnomesword.h"
#include  "support.h"

extern gchar *shortcut_types[];
extern GtkWidget *shortcut_bar;
extern SETTINGS *settings;
extern GtkWidget *MainFrm;

gint groupnum1 = 0,
        groupnum2 = 0,
        groupnum3 = 0,
        groupnum4 = 0;


//----------------------------------------------------------------------------------------------
void on_btnSB_clicked(GtkButton * button, gpointer user_data)
{
	if (settings->showshortcutbar) {
		settings->showshortcutbar = FALSE;
		e_paned_set_position (E_PANED(lookup_widget(MainFrm,"epaned")), 0);
	} else {
		settings->showshortcutbar = TRUE;
		e_paned_set_position (E_PANED(lookup_widget(MainFrm,"epaned")), settings->shortcutbarsize);
	}
}

/*****************************************************************************
 *      for any shortcut bar item clicked
 *****************************************************************************/
void
on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
			      GdkEvent * event,
			      gint group_num, gint item_num)
{
	sbchangeModSword(gtk_widget_get_toplevel(GTK_WIDGET(shortcut_bar)),
			GTK_WIDGET(shortcut_bar), group_num, item_num);
}

void setupSB(GList *textlist, GList *commentarylist, GList *dictionarylist)
{
	GList *tmp;
	
	tmp = NULL;
	if(settings->showtextgroup){
	    	groupnum1 = add_sb_group((EShortcutBar *)shortcut_bar, "Bible Text");
	}
	if(settings->showcomgroup){
	    	groupnum2 = add_sb_group((EShortcutBar *)shortcut_bar, "Commentaries");
	}
	if(settings->showdictgroup){
		groupnum3 = add_sb_group((EShortcutBar *)shortcut_bar, "Dict/Lex");
	}
	if(settings->showhistorygroup){
		groupnum4 = add_sb_group((EShortcutBar *)shortcut_bar, "History");	
	}
	
	//--  add modules to shortcut bar
	if(settings->showtextgroup){ 	
		tmp = textlist;
		while (tmp != NULL) {
		 	e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum1, -1,
					shortcut_types[0],
					(gchar *) tmp->data);
			tmp = g_list_next(tmp);
		}	
	}
	g_list_free(tmp);	
	if(settings->showcomgroup){
	                tmp = commentarylist;
	                 while (tmp != NULL) {	
			e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum2, -1,
					shortcut_types[1],
					(gchar *) tmp->data);
	                                  tmp = g_list_next(tmp);
		}	
	}	
	g_list_free(tmp);
	if(settings->showdictgroup){
	                 tmp = dictionarylist;
	                 while (tmp != NULL) { 	
			e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum3, -1,
					shortcut_types[2],
					(gchar *) tmp->data);
			tmp = g_list_next(tmp);
		}
	}	
                 g_list_free(tmp);
}

/******************************************************************************
 * add_sb_group - add group to shourtcut bar
 * shortcut_bar - shortcut bar to add group to
 * group_name - name of the group to be added
 *****************************************************************************/
gint
add_sb_group(EShortcutBar *shortcut_bar,gchar *group_name)
{
	gint group_num;
	
	group_num = e_shortcut_model_add_group (shortcut_bar->model, -1, group_name); 	
	e_shortcut_bar_set_view_type (shortcut_bar, group_num, E_ICON_BAR_SMALL_ICONS);
        	return group_num;
}

