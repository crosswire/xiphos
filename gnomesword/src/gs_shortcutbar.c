/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * gs_shortcutbar.c
    * -------------------
    * Thu Jun 14 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
    *
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
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>
#include  <gal/shortcut-bar/e-vscrolled-bar.h>
#include <gal/widgets/e-unicode.h>

#include  "gs_shortcutbar.h"
#include  "gs_gnomesword.h"
#include  "support.h"
#include "gs_bookmarks.h"
#include "sw_search.h"
#include "sw_verselist_sb.h"
#include "sw_shortcutbar.h"
#include "sw_gbs.h"
#include "gs_detach_sb.h"
#include "gs_html.h"
#include "gs_editor.h"

GtkWidget *clistSearchResults;
GtkWidget *shortcut_bar;
EShortcutModel *shortcut_model;
SEARCH_OPT so,
	*p_so;


extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;

extern gchar *shortcut_types[];
extern SETTINGS *settings;
extern gboolean havedict;	/* let us know if we have at least one lex-dict module */
extern gboolean havecomm;	/* let us know if we have at least one commentary module */
extern gboolean havebible;	/* let us know if we have at least one Bible text module */
extern GList *bookmods;
extern GList *sbbookmods;


GList *sblist; /* for saving search results to bookmarks  */
gint groupnum0 = -1,
    groupnum1 = -1,
    groupnum2 = -1,
    groupnum3 = -1,
    groupnum4 = -1, 
    groupnum5 = -1, 
    groupnum6 = -1, 
    groupnum7 = -1,
    groupnum8 = -1; /* GBS books tree group */

static void setupSearchBar(GtkWidget * vp, 
					SETTINGS *s);
static GtkWidget *setupVerseListBar(GtkWidget * vboxVL, 
					SETTINGS *s);
static void on_srlink_clicked(GtkHTML * html, 
					const gchar * url,
					SETTINGS *s);
static void on_vllink_clicked(GtkHTML * html, 
					const gchar * url,
					SETTINGS *s);
static void on_tbtnSBViewMain_toggled(GtkToggleButton * togglebutton,
				        gpointer user_data);
static void on_btnSBSaveVL_clicked(GtkButton * button, 
					gpointer user_data);
static void on_btnSearch_clicked(GtkButton * button, 
					SETTINGS *s);
static void on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
					GdkEvent * event, 
					gint group_num,
					gint item_num);
static void show_standard_popup(EShortcutBar * shortcut_bar,
					GdkEvent * event, 
					gint group_num);
static void show_context_popup(EShortcutBar * shortcut_bar,
					GdkEvent * event,
					gint group_num, 
					gint item_num);
static void set_small_icons(GtkWidget * menuitem,
					EShortcutBar * shortcut_bar);
static void set_large_icons(GtkWidget * menuitem,
					EShortcutBar * shortcut_bar);
static gint add_sb_group(EShortcutBar * shortcut_bar, 
					gchar * group_name);
static void on_item_added(EShortcutModel * shortcut_model,
					gint group_num, 
					gint item_num);
static void on_item_removed(EShortcutModel * shortcut_model,
					gint group_num, 
					gint item_num);
static void on_clistSearchResults_select_row(GtkCList *clist,
                                        gint row,
                                        gint column,
                                        GdkEvent *event,
                                        SETTINGS *s);
static void on_add_all_activate(GtkMenuItem * menuitem, 
					gpointer user_data);
static void remove_all_items(gint group_num);



static void on_VLbutton_clicked(GtkButton * button, 
					SETTINGS *s)
{
		
}

void
on_clistSearchResults_select_row(GtkCList *clist,
                                        gint row,
                                        gint column,
                                        GdkEvent *event,
                                        SETTINGS *s)
{
	gchar *buf;
	
	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &buf);	
	s->displaySearchResults = TRUE;
	changesearchresultsSW_SEARCH(s, p_so, buf);
	if(s->showinmain) {
		if(GTK_TOGGLE_BUTTON(p_so->ckbGBS)->active) {		
			if(GTK_TOGGLE_BUTTON (p_so->rbPhraseSearch)->active){
				//searchgbsGS_EDITOR(s->searchText);
			}
		}
	}
	s->displaySearchResults = FALSE;
}

/*** set shortcut bar to verse list group ***/
void showSBVerseList(SETTINGS * s)
{
	showSBGroup(s, groupnum7);
	changegroupnameSB(s, s->groupName, groupnum7);
}

/*** set shortcut bar to Daily Devotion group ***/
void setupforDailyDevotion(SETTINGS * s)
{
	
	showSBGroup(s, groupnum7);
	changegroupnameSB(s, _("Daily Devotion"), groupnum7);
}

/*** set shortcut bar group ***/
void showSBGroup(SETTINGS *s, gint groupnum)
{
	EShortcutBar *bar1;
	if(!s->showshortcutbar) {
		gint biblepanesize;
		biblepanesize = (settings->gs_width - settings->shortcutbar_width) / 2;
		e_paned_set_position (E_PANED(lookup_widget(s->app,"epaned")), s->shortcutbar_width);
		e_paned_set_position(E_PANED(lookup_widget(s->app, "hpaned1")), biblepanesize);
		s->showshortcutbar = TRUE;	
	}
	bar1 = E_SHORTCUT_BAR(s->shortcut_bar);
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar1),
					  groupnum, TRUE);
}


void changegroupnameSB(SETTINGS *s, gchar * groupName, gint groupNum)
{
	GtkWidget *label;
	EShortcutBar *bar1;

	bar1 = E_SHORTCUT_BAR(s->shortcut_bar);
	//g_warning(groupName);
	label = gtk_label_new(groupName);
	gtk_widget_show(label);
	e_group_bar_set_group_button_label(E_GROUP_BAR(bar1),
					   groupNum, label);
}


static void savegroup(EShortcutBar * shortcut_bar, gint group_num)
{
	gchar *group_name;
	
	group_name = e_shortcut_model_get_group_name(E_SHORTCUT_BAR(shortcut_bar)->model, group_num);
	
	if (group_num == groupnum0) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum0) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Favorites.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Favorites.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum1) {
		if (e_shortcut_bar_get_view_type(E_SHORTCUT_BAR(shortcut_bar), groupnum1) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("BibleText.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("BibleText.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum2) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum2) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Commentaries.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Commentaries.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum3) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum3) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Dictionaries.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Dictionaries.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum8) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum8) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Books.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Books.conf", group_name,
					  group_num, "0");
	}
}

static void
on_shortcut_dragged(EShortcutBar * shortcut_bar,
		    gint group_num, gint item_num)
{
	g_print("In on_shortcut_dragged Group:%i Item:%i\n", group_num,
		item_num);

	e_shortcut_model_remove_item(shortcut_bar->model, group_num,
				     item_num);
	savegroup((EShortcutBar *) shortcut_bar, group_num);
}


static void
on_shortcut_dropped(EShortcutBar * shortcut_bar,
		    gint group_num,
		    gint item_num, gchar * url, gchar * name)
{
	GdkPixbuf *icon_pixbuf = NULL;

	g_print("In on_shortcut_dropped Group:%i Item:%i\n", group_num,
		item_num);
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				       (shortcut_bar)->model,
				       group_num,
				       item_num, NULL, NULL, &icon_pixbuf);
	e_shortcut_model_add_item(shortcut_bar->model,
				  group_num, item_num, url, name,
				  icon_pixbuf);
	savegroup((EShortcutBar *) shortcut_bar, group_num);
}

static void on_about_item_activate(GtkMenuItem * menuitem, gpointer data)
{
	gint group_num, item_num;
	gchar *item_url, *item_name;
	EShortcutBar *bar1;
	gchar modName[16];

	item_num = GPOINTER_TO_INT(data);
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num = e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				       (shortcut_bar)->model,
				       group_num,
				       item_num,
					&item_url, 
					&item_name, 
					NULL);
	memset(modName, 0, 16);
	modNameFromDesc(modName, item_name);
	showmoduleinfoSWORD(modName,FALSE);
	g_free(item_url);
	g_free(item_name);
}

static void remove_all_items(gint group_num)
{
	gint items,i;
	
	items =  e_shortcut_model_get_num_items(shortcut_model,
							 group_num);
	for (i = items-1; i > -1; i--) {
		e_shortcut_model_remove_item(shortcut_model, group_num, i);
	}	
}

static void on_remove_item_activate(GtkMenuItem * menuitem, gpointer data)
{
	gint group_num, item_num;
	gchar *group_name;
	EShortcutBar *bar1;


	item_num = GPOINTER_TO_INT(data);
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num = e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));

	e_shortcut_model_remove_item(shortcut_model, group_num, item_num);
	group_name = e_shortcut_model_get_group_name(shortcut_model,
						     group_num);
	
	if (group_num == groupnum0) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum0) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Favorites.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Favorites.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum1) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum1) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("BibleText.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("BibleText.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum2) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum2) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Commentaries.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Commentaries.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum3) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum3) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Dictionaries.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Dictionaries.conf", group_name,
					  group_num, "0");
	}	
}

void
on_add_all_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GList 
		*glist;
	gint 
		number_of_items = 0,
		group_num, 
		sbtype;	
	EShortcutBar *bar1;
	gchar 
		*group_name, 
		modName[16]; 
	GdkPixbuf 
		*icon_pixbuf = NULL;
	
	glist = NULL;
	glist = getModlistSW((gchar*)user_data);
	
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num = e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	group_name = "";
	group_name = e_shortcut_model_get_group_name(E_SHORTCUT_BAR(shortcut_bar)->model, group_num);	
	remove_all_items(group_num);
	
	while (glist != NULL) {
		memset(modName, 0, 16);
		modNameFromDesc(modName, (gchar*)glist->data);
		sbtype = 0;
		sbtype = sbtypefromModNameSBSW(modName);
		if (sbtype < 0)
			sbtype = 0;

		switch (sbtype) {
		case 0:
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-un.png");
			break;
		case 1:
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-bl.png");
			break;
		case 2:
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-green.png");
			break;
		case 3:
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-gold.png");
			break;
		}
		
		e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->model,
				  group_num, -1, "not null",
				  (gchar *) glist->data, icon_pixbuf);
		++number_of_items;
		glist = g_list_next(glist);
	}
	savegroup(E_SHORTCUT_BAR(shortcut_bar), group_num);
	/****  we used g_strdup to fill the list so we need to g_free each item  ****/
	glist = g_list_first(glist);
	while (glist != NULL) {
		g_free((gchar*)glist->data);
		glist = g_list_next(glist);		
	}
	g_list_free(glist);
	
}

static void
on_add_shortcut_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *group_name;
	gint group_num, sbtype;
	EShortcutBar *bar1;
	gchar modName[16];
	GdkPixbuf *icon_pixbuf = NULL;

	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num = e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	memset(modName, 0, 16);
	modNameFromDesc(modName, (gchar *) user_data);
	sbtype = 0;
	sbtype = sbtypefromModNameSBSW(modName);
	if (sbtype < 0)
		sbtype = 0;

	switch (sbtype) {
	case 0:
		icon_pixbuf =
      gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-un.png");
		break;
	case 1:
		icon_pixbuf =
      gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-bl.png");
		break;
	case 2:
		icon_pixbuf =
      gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-green.png");
		break;
	}
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->model,
				  group_num, -1, "not null",
				  (gchar *) user_data, icon_pixbuf);
	
	group_name = "";
	group_name = e_shortcut_model_get_group_name(E_SHORTCUT_BAR
						     (shortcut_bar)->model,
						     group_num);
	
	if (group_num == groupnum0) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum0) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Favorites.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Favorites.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum1) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum1) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("BibleText.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("BibleText.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum2) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum2) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Commentaries.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Commentaries.conf", group_name,
					  group_num, "0");
	}
	
	if (group_num == groupnum3) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum3) == E_ICON_BAR_LARGE_ICONS)
			saveshortcutbarSW("Dictionaries.conf", group_name,
					  group_num, "1");
		else
			saveshortcutbarSW("Dictionaries.conf", group_name,
					  group_num, "0");
	}
}

static void
on_item_added(EShortcutModel * model, gint group_num, gint item_num)
{

}


static void
on_item_removed(EShortcutModel * shortcut_model,
		gint group_num, gint item_num)
{

	g_print("In on_item_removed Group:%i Item:%i\n", group_num,
		item_num);
	
}

/******************************************************************************
 * add_sb_group - add group to shourtcut bar
 * shortcut_bar - shortcut bar to add group to
 * group_name - name of the group to be added
 *****************************************************************************/
static gint add_sb_group(EShortcutBar * shortcut_bar, gchar * group_name)
{
	gint group_num;

	group_num =
	    e_shortcut_model_add_group(shortcut_bar->model, -1,
				       group_name);
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_SMALL_ICONS);
	return group_num;
}

/*** dock/undock shortcut bar ***/
void on_btnSBDock_clicked(GtkButton * button, SETTINGS *s)
{
	gint biblepanesize;
	
	if (s->docked) {		
		//s->showshortcutbar = FALSE;
		s->docked = FALSE;
		biblepanesize = s->gs_width / 2;
		s->dockSB = create_dlgDock(s);
		 gtk_widget_reparent(shortcut_bar,
                                            s->vboxDock );
		settings->showshortcutbar = TRUE;
		gtk_widget_show(shortcut_bar);
		e_paned_set_position(E_PANED
				     (lookup_widget(s->app, "epaned")),
				     0);
		e_paned_set_position(E_PANED
				     (lookup_widget(s->app, "hpaned1")),
				     biblepanesize);
		gtk_widget_show(s->dockSB);
	} else {
		//s->showshortcutbar = TRUE;
		s->docked = TRUE;
		biblepanesize = (s->gs_width - s->shortcutbar_width) / 2;
		e_paned_set_position(E_PANED
				     (lookup_widget(s->app, "epaned")),
				     s->shortcutbar_width);
		e_paned_set_position(E_PANED
				     (lookup_widget(s->app, "hpaned1")),
				     biblepanesize);
		 gtk_widget_reparent(shortcut_bar,
                                             lookup_widget(s->app, "epaned"));		
		gtk_widget_destroy(s->dockSB);
	}
}

/*** show hide shortcut bar ***/
void on_btnSB_clicked(GtkButton * button, SETTINGS *s)
{
	if(!s->docked) {
		gdk_window_raise(GTK_WIDGET(s->dockSB)->window);
		return;
	}
	
	if (settings->showshortcutbar) {
		settings->showshortcutbar = FALSE;
		s->biblepane_width = s->gs_width / 2;
		gtk_widget_hide(shortcut_bar);
		e_paned_set_position(E_PANED
			     (lookup_widget(s->app, "epaned")),
			     0);
		e_paned_set_position(E_PANED
			     (lookup_widget(s->app, "hpaned1")),
			     s->biblepane_width);
	} else {
		s->showshortcutbar = TRUE;
		s->biblepane_width = (s->gs_width - s->shortcutbar_width) / 2;
		e_paned_set_position(E_PANED
			     (lookup_widget(s->app, "epaned")),
			     s->shortcutbar_width);
		e_paned_set_position(E_PANED
			     (lookup_widget(s->app, "hpaned1")),
			     s->biblepane_width);
		gtk_widget_show(shortcut_bar);
	}
}

static void
set_large_icons(GtkWidget * menuitem, EShortcutBar * shortcut_bar)
{
	GtkWidget *menu;
	gint group_num;

	menu = menuitem->parent;
	g_return_if_fail(GTK_IS_MENU(menu));

	group_num = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(menu),
							"group_num"));
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_LARGE_ICONS);
	if (group_num == groupnum0)
		save_iconsizeSW("Favorites.conf", "1");
	if (group_num == groupnum1)
		save_iconsizeSW("BibleText.conf", "1");
	if (group_num == groupnum2)
		save_iconsizeSW("Commentaries.conf", "1");
	if (group_num == groupnum3)
		save_iconsizeSW("Dictionaries.conf", "1");
}

static void
set_small_icons(GtkWidget * menuitem, EShortcutBar * shortcut_bar)
{
	GtkWidget *menu;
	gint group_num;

	menu = menuitem->parent;
	g_return_if_fail(GTK_IS_MENU(menu));

	group_num = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(menu),
							"group_num"));
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_SMALL_ICONS);
	if (group_num == groupnum0)
		save_iconsizeSW("Favorites.conf", "0");
	if (group_num == groupnum1)
		save_iconsizeSW("BibleText.conf", "0");
	if (group_num == groupnum2)
		save_iconsizeSW("Commentaries.conf", "0");
	if (group_num == groupnum3)
		save_iconsizeSW("Dictionaries.conf", "0");
}

static void
show_standard_popup(EShortcutBar * shortcut_bar,
		    GdkEvent * event, gint group_num)
{
	GtkWidget *menu, *menuitem, *menu_item_menu;

	/* We don't have any commands if there aren't any groups yet. */
	if (group_num == -1)
		return;

	menu = gtk_menu_new();

	menuitem = gtk_menu_item_new_with_label(_("Large Icons"));
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC(set_large_icons), shortcut_bar);

	menuitem = gtk_menu_item_new_with_label(_("Small Icons"));
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC(set_small_icons), shortcut_bar);

	menuitem = gtk_menu_item_new();
	gtk_widget_set_sensitive(menuitem, FALSE);
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);

	if (group_num == groupnum0) {
		menuitem = gtk_menu_item_new_with_label(_("Add Bible Text"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu", menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		create_modlistmenu_sb(group_num, menuitem, menu_item_menu,
				      "Biblical Texts"); /* this string is used internally */

		menuitem = gtk_menu_item_new_with_label(_("Add Commentary"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu", menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		create_modlistmenu_sb(group_num, menuitem, menu_item_menu,
				      "Commentaries"); /* this string is used internally */


		menuitem = gtk_menu_item_new_with_label(_("Add Dictionary"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu", menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		create_modlistmenu_sb(group_num, menuitem, menu_item_menu,
				      "Lexicons / Dictionaries"); /* this string is used internally */

		
		menuitem = gtk_menu_item_new_with_label(_("Add Book"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu", menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		create_modlistmenu_sb(group_num, menuitem, menu_item_menu,
				      "Generic Books");	 /* this string is used internally */
	      
	} 
	
	else {
		menuitem = gtk_menu_item_new_with_label(_("Add Shortcut"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu", menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
	/*** add bookmark items ***/
		if (group_num == groupnum1)
			create_modlistmenu_sb(group_num, menuitem, menu_item_menu,
					      "Biblical Texts"); /* this string is used internally */

		if (group_num == groupnum2)
			create_modlistmenu_sb(group_num, menuitem, menu_item_menu,
					      "Commentaries"); /* this string is used internally */

		if (group_num == groupnum3)
			create_modlistmenu_sb(group_num, menuitem, menu_item_menu,
					      "Lexicons / Dictionaries"); /* this string is used internally */

		if (group_num == groupnum8)
			create_modlistmenu_sb(group_num, menuitem, menu_item_menu,
					      "Generic Books"); /* this string is used internally */

	}
	/* Save the group num so we can get it in the callbacks. */
	gtk_object_set_data(GTK_OBJECT(menu), "group_num",
			    GINT_TO_POINTER(group_num));

	/* FIXME: Destroy menu when finished with it somehow? */
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
		       event->button.button, event->button.time);
}

static void
show_context_popup(EShortcutBar * shortcut_bar,
		   GdkEvent * event, gint group_num, gint item_num)
{
	GtkWidget *menu, *menuitem;

	menu = gtk_menu_new();

	menuitem = gtk_menu_item_new_with_label(_("About this Module"));
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC(on_about_item_activate),
			   GINT_TO_POINTER(item_num));

	menuitem = gtk_menu_item_new();
	gtk_widget_set_sensitive(menuitem, FALSE);
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);

	menuitem =
	    gtk_menu_item_new_with_label(_("Remove from Shortcut Bar"));
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC(on_remove_item_activate),
			   GINT_TO_POINTER(item_num));

	/* Save the group & item nums so we can get them in the callbacks. */
	gtk_object_set_data(GTK_OBJECT(menu), "group_num",
			    GINT_TO_POINTER(group_num));
	gtk_object_set_data(GTK_OBJECT(menu), "item_num",
			    GINT_TO_POINTER(item_num));

	/* FIXME: Destroy menu when finished with it somehow? */
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
		       event->button.button, event->button.time);
}

/*****************************************************************************
 *      for any shortcut bar item clicked or group button
 *****************************************************************************/
static void
on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
			      GdkEvent * event,
			      gint group_num, gint item_num)
{
	GtkWidget *app;
	gchar *type, *ref;
	gchar modName[16];
	gint remItemNum = 0;
	GdkPixbuf *icon_pixbuf = NULL;
	
	remItemNum = item_num;
	
//	if(item_num == -1) {
//		if(group_num == groupnum2) /* change work space notebook to commentary page */
//			gtk_notebook_set_page(GTK_NOTEBOOK(settings->workbook),0);
//		if(group_num == groupnum3) /* change Dictionayr - Books notebook to Dict page */
//			gtk_notebook_set_page(GTK_NOTEBOOK(settings->workbook_lower),0);
//		if(group_num == groupnum8) /* change Dictionayr - Books notebook to Dict page */
//			gtk_notebook_set_page(GTK_NOTEBOOK(settings->workbook_lower),1);
//	}
	
	if (event->button.button == 1) {
		if(item_num > -1) {
			app = gtk_widget_get_toplevel(GTK_WIDGET(shortcut_bar));
			e_shortcut_model_get_item_info(E_SHORTCUT_BAR
					       (shortcut_bar)->model,
					       group_num, item_num, &type,
					       &ref, &icon_pixbuf);
			memset(modName, 0, 16);
			modNameFromDesc(modName, ref);
			
			if (group_num == groupnum0) {
				gint sbtype;
				sbtype = sbtypefromModNameSBSW(modName);
				if (sbtype == 0 || sbtype == 1)
					gotoBookmarkSWORD(modName,
						  settings->currentverse);
				else if (sbtype == 3) {
					gtk_notebook_set_page(GTK_NOTEBOOK(settings->workbook_lower),1);
					gotoBookmarkSWORD(modName,NULL);
				}
					
				else
					gotoBookmarkSWORD(modName,
						  settings->dictkey);
			}
			
			if (group_num == groupnum1) {
				if (havebible) {
					gotoBookmarkSWORD(modName,
						  settings->currentverse);
				}
			}
			
			if (group_num == groupnum2) {
				if (havecomm) {
					gotoBookmarkSWORD(modName,
						  settings->currentverse);
					gtk_notebook_set_page(GTK_NOTEBOOK(settings->workbook),0);
				}
			}
			
			if (group_num == groupnum3) {
				if (havedict) {
					gotoBookmarkSWORD(modName,
						  settings->dictkey);
					gtk_notebook_set_page(GTK_NOTEBOOK(settings->workbook_lower),0);
				}
			}
			
			if (group_num == groupnum4) {
				changeVerseSWORD(ref);
			}
			
			if (group_num == groupnum8) {
				gotoBookmarkSWORD(modName,NULL);
				gtk_notebook_set_page(GTK_NOTEBOOK(settings->workbook_lower),1);
			}
			g_free(type);
			g_free(ref);
		}
	} else if (event->button.button == 3) {
		if (remItemNum == -1)
			show_standard_popup(shortcut_bar, event,
					    group_num);
		else
			show_context_popup(shortcut_bar, event, group_num,
					   item_num);
	}
	
}

static void on_btnSearch_clicked(GtkButton * button, SETTINGS * s)
{
	sblist = searchSWORD(s, p_so);
}

/*** save verse list as bookmarks ***/
static void on_btnSBSaveVL_clicked(GtkButton * button, gpointer user_data)
{
	SETTINGS *s;

	s = (SETTINGS *) user_data;
	addverselistBM(s, sblist);
}


static void on_tbtnSBViewMain_toggled(GtkToggleButton * togglebutton,
				      gpointer user_data) 
{
	SETTINGS *s;

	s = (SETTINGS *) user_data;
	s->showinmain = togglebutton->active;
}


static void on_btnViewVL_clicked(GtkButton * button, gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(GTK_WIDGET(user_data)), 0);
	changegroupnameSB(settings, _("Verse List"), groupnum7);
}


static void on_btnViewSR_clicked(GtkButton * button, gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(GTK_WIDGET(user_data)), 1);
	changegroupnameSB(settings, _("Search Results"), groupnum7);
}


static void on_btnViewer_clicked(GtkButton * button, gpointer user_data)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(GTK_WIDGET(user_data)), 2);
	changegroupnameSB(settings, _("Viewer"), groupnum7);
}


static void
on_vllink_clicked(GtkHTML * html, const gchar * url, SETTINGS *s)
{
	changeVerseListSBSWORD(s, (gchar *) url);
}

static void
on_srlink_clicked(GtkHTML * html, const gchar * url, SETTINGS *s)
{
	gchar 
		*buf, 
		*modbuf = NULL,
		newmod[80], 
		newref[80];
	gint 
		i = 0, 
		havemod = 0;
	
	if (!strncmp(url, "version=", 7)
		   || !strncmp(url, "passage=", 7)) {
		gchar *mybuf = NULL;
		mybuf = strstr(url, "version=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			i = 0;
			while (mybuf[i] != ' ') {
				newmod[i] = mybuf[i];
				newmod[i + 1] = '\0';
				++i;
				++havemod;
			}
		}
		mybuf = NULL;
		mybuf = strstr(url, "passage=");
		i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			while (i < strlen(mybuf)) {
				newref[i] = mybuf[i];
				newref[i + 1] = '\0';
				++i;
			}
		}
		if (havemod > 2) {
			modbuf = newmod;
		} else {
			modbuf = s->MainWindowModule;
		}
		buf = g_strdup(newref);
		gotoBookmarkSWORD(modbuf, buf);
		g_free(buf);	
	}
}

/*
static void
on_search_results_link_clicked(GtkHTML * html, const gchar * url,
			       gpointer data)
{
	SETTINGS *s;

	s = (SETTINGS *) data;
	s->displaySearchResults = TRUE;
	changesearchresultsSBSW(s, (gchar *) url);
	s->displaySearchResults = FALSE;
	
}
*/

static void
on_nbVL_switch_page                    (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data)
{
	SETTINGS *s;
	
	s = (SETTINGS*) user_data;	
	switch(page_num){
		case 0:
			gtk_widget_set_sensitive(lookup_widget(s->app, "btnSBSaveVL"), FALSE);
			gtk_widget_set_sensitive( lookup_widget(s->app, "tbtnSBViewMain"), TRUE);
		break;
		case 1:
			gtk_widget_set_sensitive(lookup_widget(s->app, "btnSBSaveVL"), TRUE);
			gtk_widget_set_sensitive( lookup_widget(s->app, "tbtnSBViewMain"), TRUE);			
		break;
		case 2:
			gtk_widget_set_sensitive(lookup_widget(s->app, "btnSBSaveVL"), FALSE);
			gtk_widget_set_sensitive( lookup_widget(s->app, "tbtnSBViewMain"), FALSE);			
		break;
	}
}

static GtkWidget *setupVerseListBar(GtkWidget * vboxVL, SETTINGS * s)
{
	GtkWidget *toolbarVL;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSBSaveVL;
	GtkWidget *tbtnSBViewMain;
	//GtkWidget *btnSBShowCV;
	GtkWidget *vseparator1;
	GtkWidget *btnViewVL;
	GtkWidget *btnViewSR;
	GtkWidget *btnViewer;
	GtkWidget *nbVL;
	GtkWidget *vbox2;
	GtkWidget *frame1;
	GtkWidget *scrolledwindow1;
	GtkWidget *frame2;
	GtkWidget *scrolledwindow2;
	GtkWidget *label1;
	GtkWidget *vbox3;
	GtkWidget *vpaned_srch_rslt;
	GtkWidget *frame3;
	GtkWidget *scrolledwindow3;
	GtkWidget *frame4;
	GtkWidget *scrolledwindow4;	
	GtkWidget *label2;
	GtkWidget *frame5;
	GtkWidget *label3;
	GtkWidget *htmlshow;
	GtkWidget *htmlshow2;
	GtkWidget *scrolledwindow5;
	GtkWidget *htmlviewer;
	GtkWidget *frameTB;
	GtkWidget *frameRP;
	GtkWidget *scrolledwindowRP;

	frameTB = gtk_frame_new(NULL);
	gtk_widget_ref(frameTB);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frameTB", frameTB,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameTB);
	gtk_box_pack_start(GTK_BOX(vboxVL), frameTB, FALSE, TRUE, 0);

	toolbarVL =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarVL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbarVL", toolbarVL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarVL);
	gtk_container_add(GTK_CONTAINER(frameTB), toolbarVL);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarVL),
				      GTK_RELIEF_NONE);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_SAVE);
	btnSBSaveVL =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save List"),
				       _("Save the current verse list as a bookmark file"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSBSaveVL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnSBSaveVL",
				 btnSBSaveVL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSBSaveVL);
	gtk_widget_set_sensitive(btnSBSaveVL, FALSE);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_JUMP_TO);
	tbtnSBViewMain =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Main Form"),
				       _("Toggle to show results in Main Form"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(tbtnSBViewMain);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "tbtnSBViewMain",
				 tbtnSBViewMain,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tbtnSBViewMain);
/*
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_REFRESH);
	btnSBShowCV =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("UpdateMain"),
				       _("Show current verse list verse in Main Form"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSBShowCV);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnSBShowCV",
				 btnSBShowCV,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSBShowCV);
	gtk_widget_set_sensitive(btnSBShowCV, FALSE);
*/	
	vseparator1 = gtk_vseparator_new();
	gtk_widget_ref(vseparator1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vseparator1",
				 vseparator1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator1);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarVL), vseparator1, NULL,
				  NULL);
	gtk_widget_set_usize(vseparator1, 12, 12);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_BOOK_RED);
	btnViewVL =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("VerseList"),
				       _("View Verse List"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnViewVL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnViewVL",
				 btnViewVL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnViewVL);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_BOOK_GREEN);
	btnViewSR =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("SearchResults"),
				       _("View Search Results"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnViewSR);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnViewSR",
				 btnViewSR,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnViewSR);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_BOOK_BLUE);
	btnViewer =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Viewer"), _("Viewer"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnViewer);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnViewer",
				 btnViewer,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnViewer);

	nbVL = gtk_notebook_new();
	gtk_widget_ref(nbVL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "nbVL", nbVL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(nbVL);
	gtk_box_pack_start(GTK_BOX(vboxVL), nbVL, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(nbVL, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nbVL), FALSE);

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(nbVL), vbox2);

	frame1 = gtk_frame_new(NULL);
	gtk_widget_ref(frame1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame1", frame1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame1);
	gtk_box_pack_start(GTK_BOX(vbox2), frame1, FALSE, TRUE, 0);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "scrolledwindow1",
				 scrolledwindow1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);
	gtk_container_add(GTK_CONTAINER(frame1), scrolledwindow1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_widget_set_usize(scrolledwindow1, -2, 75);

	s->vlsbhtml = gtk_html_new();
	gtk_widget_ref(s->vlsbhtml);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "s->vlsbhtml", s->vlsbhtml,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->vlsbhtml);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), s->vlsbhtml);
	gtk_html_load_empty(GTK_HTML(s->vlsbhtml));

	frame2 = gtk_frame_new(NULL);
	gtk_widget_ref(frame2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame2", frame2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox2), frame2, TRUE, TRUE, 0);

	scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "scrolledwindow2",
				 scrolledwindow2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow2);
	gtk_container_add(GTK_CONTAINER(frame2), scrolledwindow2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow2), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	htmlshow = gtk_html_new();
	gtk_widget_ref(htmlshow);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "htmlshow", htmlshow,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlshow);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), htmlshow);
	gtk_html_load_empty(GTK_HTML(htmlshow));

	label1 = gtk_label_new(_("label1"));
	gtk_widget_ref(label1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label1", label1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label1);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbVL),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbVL), 0),
				   label1);

//-------------------------------------------------------------------------

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox3", vbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(nbVL), vbox3);
	
	frameRP = gtk_frame_new(NULL);
	gtk_widget_ref(frameRP);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frameRP",frameRP ,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameRP);
	gtk_box_pack_start(GTK_BOX(vbox3), frameRP, FALSE, TRUE, 0);

	scrolledwindowRP = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowRP);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "scrolledwindowRP",
				 scrolledwindowRP,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindowRP);
	gtk_container_add(GTK_CONTAINER(frameRP), scrolledwindowRP);
	gtk_widget_set_usize(scrolledwindowRP, -2, 65);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowRP), GTK_POLICY_NEVER,
				      GTK_POLICY_NEVER );
				       
	s->htmlRP = gtk_html_new();
	gtk_widget_ref(s->htmlRP);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "s->htmlRP", s->htmlRP,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->htmlRP);
	gtk_container_add(GTK_CONTAINER(scrolledwindowRP), s->htmlRP);
	gtk_html_load_empty(GTK_HTML(s->htmlRP));

	vpaned_srch_rslt = gtk_vpaned_new();
	gtk_widget_ref(vpaned_srch_rslt);
	gtk_object_set_data_full(GTK_OBJECT(settings->app), "vpaned_srch_rslt",
				 vpaned_srch_rslt, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vpaned_srch_rslt);
	gtk_box_pack_start(GTK_BOX(vbox3), vpaned_srch_rslt, TRUE, TRUE, 0);
	gtk_paned_set_position(GTK_PANED(vpaned_srch_rslt), 100);

	frame3 = gtk_frame_new(NULL);
	gtk_widget_ref(frame3);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame3", frame3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame3);
	gtk_paned_pack1(GTK_PANED(vpaned_srch_rslt), frame3, TRUE, TRUE);

	scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow3);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "scrolledwindow3",
				 scrolledwindow3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow3);
	gtk_container_add(GTK_CONTAINER(frame3), scrolledwindow3);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow3), GTK_POLICY_AUTOMATIC,
				      GTK_POLICY_AUTOMATIC);
				       
	clistSearchResults = gtk_clist_new (1);
	gtk_widget_ref (clistSearchResults);
	gtk_object_set_data_full (GTK_OBJECT (s->app), "clistSearchResults", clistSearchResults,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (clistSearchResults);
	gtk_container_add (GTK_CONTAINER (scrolledwindow3), clistSearchResults);
	gtk_clist_set_column_width (GTK_CLIST (clistSearchResults), 0, 100); 
	gtk_clist_column_titles_hide (GTK_CLIST (clistSearchResults));

	frame4 = gtk_frame_new(NULL);
	gtk_widget_ref(frame4);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame4", frame4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame4);
	gtk_paned_pack2(GTK_PANED(vpaned_srch_rslt), frame4, TRUE, TRUE);

	scrolledwindow4 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow4);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "scrolledwindow4",
				 scrolledwindow4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow4);
	gtk_container_add(GTK_CONTAINER(frame4), scrolledwindow4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow4), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	

	htmlshow2 = gtk_html_new();
	gtk_widget_ref(htmlshow2);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "htmlshow2", htmlshow2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlshow2);
	gtk_container_add(GTK_CONTAINER(scrolledwindow4), htmlshow2);
	gtk_html_load_empty(GTK_HTML(htmlshow2));
	setupsearchresultsSBSW(htmlshow2);
	
	label2 = gtk_label_new(_("label2"));
	gtk_widget_ref(label2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label2", label2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label2);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbVL),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbVL), 1),
				   label2);

//-------------------------------------------------------------------------

	frame5 = gtk_frame_new(NULL);
	gtk_widget_ref(frame5);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame5", frame5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame5);
	gtk_container_add(GTK_CONTAINER(nbVL), frame5);

	scrolledwindow5 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow5);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "scrolledwindow5",
				 scrolledwindow5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow5);
	gtk_container_add(GTK_CONTAINER(frame5), scrolledwindow5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow5), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	htmlviewer = gtk_html_new();
	gtk_widget_ref(htmlviewer);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "htmlviewer", htmlviewer,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlviewer);
	gtk_container_add(GTK_CONTAINER(scrolledwindow5), htmlviewer);
	gtk_html_load_empty(GTK_HTML(htmlviewer));
	setupviewerSBSW(htmlviewer);

	label3 = gtk_label_new(_("label3"));
	gtk_widget_ref(label3);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label3", label3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label3);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbVL),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbVL), 2),
				   label3);
	
	gtk_signal_connect(GTK_OBJECT(s->vlsbhtml), "link_clicked",
			   GTK_SIGNAL_FUNC(on_vllink_clicked), s);
	gtk_signal_connect(GTK_OBJECT(btnSBSaveVL), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSBSaveVL_clicked), s);
	gtk_signal_connect(GTK_OBJECT(tbtnSBViewMain), "toggled",
			   GTK_SIGNAL_FUNC(on_tbtnSBViewMain_toggled), s);
	gtk_signal_connect(GTK_OBJECT(btnViewVL), "clicked",
			   GTK_SIGNAL_FUNC(on_btnViewVL_clicked),
			   GTK_WIDGET(nbVL));
	gtk_signal_connect(GTK_OBJECT(btnViewSR), "clicked",
			   GTK_SIGNAL_FUNC(on_btnViewSR_clicked),
			   GTK_WIDGET(nbVL));
	gtk_signal_connect(GTK_OBJECT(btnViewer), "clicked",
			   GTK_SIGNAL_FUNC(on_btnViewer_clicked),
			   GTK_WIDGET(nbVL));
  	gtk_signal_connect (GTK_OBJECT (nbVL), "switch_page",
                      		GTK_SIGNAL_FUNC (on_nbVL_switch_page),
				s);
	gtk_signal_connect (GTK_OBJECT (clistSearchResults), "select_row",
                      GTK_SIGNAL_FUNC (on_clistSearchResults_select_row),
			      s);
	gtk_signal_connect(GTK_OBJECT(htmlshow2), "link_clicked",
			   GTK_SIGNAL_FUNC(on_srlink_clicked), s);
	gtk_signal_connect(GTK_OBJECT(htmlshow2), "on_url",
			   GTK_SIGNAL_FUNC(on_url), s->app);
	return htmlshow;
}


/******************************************************************************
* 
*******************************************************************************/
static void setupSearchBar(GtkWidget * vp, SETTINGS * s)
{
	GtkWidget *frame1;
	GtkWidget *vbox1;
	GtkWidget *frame7;
	GtkWidget *vbox5;
	GtkWidget *entrySearch;
	GtkWidget *btnSearch;
	GtkWidget *frame2;
	GtkWidget *vbox2;
	GSList *vbox2_group = NULL;
	GtkWidget *rbMultiword;
	GtkWidget *rbRegExp;
	GtkWidget *frame3;
	GtkWidget *vbox3;
	GtkWidget *ckbCaseSensitive;
	GtkWidget *frame4;
	GtkWidget *vbox4;
	GSList *vbox4_group = NULL;
	GtkWidget *rbNoScope;
	GtkWidget *rrbUseBounds;
	GtkWidget *rbLastSearch;
	GtkWidget *frame5;
	GtkWidget *table1;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *entryLower;
	GtkWidget *entryUpper;
	GtkTooltips *tooltips;
		
	p_so = &so;
	
	tooltips = gtk_tooltips_new();

	frame1 = gtk_frame_new(NULL);
	gtk_widget_ref(frame1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame1", frame1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame1);
	gtk_container_add(GTK_CONTAINER(vp), frame1);
	//gtk_widget_set_usize(frame1, 162, 360);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox1", vbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(frame1), vbox1);

	frame7 = gtk_frame_new(NULL);
	gtk_widget_ref(frame7);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame7", frame7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame7);
	gtk_box_pack_start(GTK_BOX(vbox1), frame7, FALSE, TRUE, 0);

	vbox5 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox5);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox5", vbox5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox5);
	gtk_container_add(GTK_CONTAINER(frame7), vbox5);

	entrySearch = gtk_entry_new();
	gtk_widget_ref(entrySearch);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "entrySearch",
				 entrySearch,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entrySearch);
	gtk_box_pack_start(GTK_BOX(vbox5), entrySearch, TRUE, TRUE, 0);
	gtk_widget_set_usize(entrySearch, 130, -2);

	btnSearch = gnome_stock_button(GNOME_STOCK_BUTTON_OK);
	gtk_widget_ref(btnSearch);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnSearch",
				 btnSearch,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSearch);
	gtk_box_pack_start(GTK_BOX(vbox5), btnSearch, TRUE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, btnSearch, _("Begin Search"), NULL);
	gtk_button_set_relief(GTK_BUTTON(btnSearch), GTK_RELIEF_HALF);

	frame2 = gtk_frame_new(_("Search Type"));
	gtk_widget_ref(frame2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame2", frame2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox1), frame2, FALSE, TRUE, 0);

	vbox2 = gtk_vbox_new(TRUE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox2", vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(frame2), vbox2);

	rbMultiword =
	    gtk_radio_button_new_with_label(vbox2_group, _("Multi Word"));
	vbox2_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbMultiword));
	gtk_widget_ref(rbMultiword);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "rbMultiword",
				 rbMultiword,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rbMultiword);
	gtk_box_pack_start(GTK_BOX(vbox2), rbMultiword, FALSE, FALSE, 0);
	gtk_widget_set_usize(rbMultiword, -2, 20);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbMultiword), TRUE);

	rbRegExp =
	    gtk_radio_button_new_with_label(vbox2_group,
					    _("Regular Expression"));
	vbox2_group = gtk_radio_button_group(GTK_RADIO_BUTTON(rbRegExp));
	gtk_widget_ref(rbRegExp);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "rbRegExp", rbRegExp,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rbRegExp);
	gtk_box_pack_start(GTK_BOX(vbox2), rbRegExp, FALSE, FALSE, 0);
	gtk_widget_set_usize(rbRegExp, -2, 20);

	p_so->rbPhraseSearch =
	    gtk_radio_button_new_with_label(vbox2_group,
					    _("Exact Phrase"));
	vbox2_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(p_so->rbPhraseSearch));
	gtk_widget_ref(p_so->rbPhraseSearch);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "p_so->rbPhraseSearch",
				 p_so->rbPhraseSearch,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p_so->rbPhraseSearch);
	gtk_box_pack_start(GTK_BOX(vbox2), p_so->rbPhraseSearch, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(p_so->rbPhraseSearch, -2, 20);

	frame3 = gtk_frame_new(_("Search Options"));
	gtk_widget_ref(frame3);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame3", frame3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame3);
	gtk_box_pack_start(GTK_BOX(vbox1), frame3, FALSE, TRUE, 0);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox3", vbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(frame3), vbox3);

	ckbCaseSensitive =
	    gtk_check_button_new_with_label(_("Case Sensitive"));
	gtk_widget_ref(ckbCaseSensitive);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "ckbCaseSensitive",
				 ckbCaseSensitive,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ckbCaseSensitive);
	gtk_box_pack_start(GTK_BOX(vbox3), ckbCaseSensitive, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(ckbCaseSensitive, -2, 20);

	p_so->ckbCommentary =
	    gtk_check_button_new_with_label(_("Search Commentary"));
	gtk_widget_ref(p_so->ckbCommentary);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "p_so->ckbCommentary",
				 p_so->ckbCommentary,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p_so->ckbCommentary);
	gtk_box_pack_start(GTK_BOX(vbox3), p_so->ckbCommentary, FALSE, FALSE, 0);
	gtk_widget_set_usize(p_so->ckbCommentary, -2, 20);

	p_so->ckbPerCom = gtk_check_button_new_with_label(_("Search Personal"));
	gtk_widget_ref(p_so->ckbPerCom);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "p_so->ckbPerCom",
				 p_so->ckbPerCom,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p_so->ckbPerCom);
	gtk_box_pack_start(GTK_BOX(vbox3), p_so->ckbPerCom, FALSE, FALSE, 0);
	gtk_widget_set_usize(p_so->ckbPerCom, -2, 20);

	p_so->ckbGBS = gtk_check_button_new_with_label(_("Search Book"));
	gtk_widget_ref(p_so->ckbGBS);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "p_so->ckbGBS",
				 p_so->ckbGBS,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p_so->ckbGBS);	
	gtk_tooltips_set_tip (tooltips, p_so->ckbGBS, _("Search Current Book"), NULL);
	gtk_box_pack_start(GTK_BOX(vbox3), p_so->ckbGBS, FALSE, FALSE, 0);
	gtk_widget_set_usize(p_so->ckbGBS, -2, 20);
	
	frame4 = gtk_frame_new(_("Search Scope"));
	gtk_widget_ref(frame4);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame4", frame4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame4);
	gtk_box_pack_start(GTK_BOX(vbox1), frame4, FALSE, TRUE, 0);

	vbox4 = gtk_vbox_new(TRUE, 0);
	gtk_widget_ref(vbox4);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox4", vbox4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(frame4), vbox4);

	rbNoScope =
	    gtk_radio_button_new_with_label(vbox4_group, _("No Scope"));
	vbox4_group = gtk_radio_button_group(GTK_RADIO_BUTTON(rbNoScope));
	gtk_widget_ref(rbNoScope);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "rbNoScope",
				 rbNoScope,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rbNoScope);
	gtk_box_pack_start(GTK_BOX(vbox4), rbNoScope, FALSE, FALSE, 0);
	gtk_widget_set_usize(rbNoScope, -2, 20);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbNoScope), TRUE);

	rrbUseBounds =
	    gtk_radio_button_new_with_label(vbox4_group, _("Use Bounds"));
	vbox4_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rrbUseBounds));
	gtk_widget_ref(rrbUseBounds);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "rrbUseBounds",
				 rrbUseBounds,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rrbUseBounds);
	gtk_box_pack_start(GTK_BOX(vbox4), rrbUseBounds, FALSE, FALSE, 0);
	gtk_widget_set_usize(rrbUseBounds, -2, 20);

	rbLastSearch =
	    gtk_radio_button_new_with_label(vbox4_group, _("Last Search"));
	vbox4_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbLastSearch));
	gtk_widget_ref(rbLastSearch);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "rbLastSearch",
				 rbLastSearch,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rbLastSearch);
	gtk_box_pack_start(GTK_BOX(vbox4), rbLastSearch, FALSE, FALSE, 0);
	gtk_widget_set_usize(rbLastSearch, -2, 20);

	frame5 = gtk_frame_new(_("Bounds"));
	gtk_widget_ref(frame5);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame5", frame5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame5);
	gtk_box_pack_start(GTK_BOX(vbox1), frame5, TRUE, TRUE, 0);

	table1 = gtk_table_new(2, 2, FALSE);
	gtk_widget_ref(table1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "table1", table1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table1);
	gtk_container_add(GTK_CONTAINER(frame5), table1);

	label1 = gtk_label_new(_("Lower"));
	gtk_widget_ref(label1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label1", label1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label1);
	gtk_table_attach(GTK_TABLE(table1), label1, 0, 1, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

	label2 = gtk_label_new(_("Upper "));
	gtk_widget_ref(label2);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label2", label2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label2);
	gtk_table_attach(GTK_TABLE(table1), label2, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

	entryLower = gtk_entry_new();
	gtk_widget_ref(entryLower);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "entryLower",
				 entryLower,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryLower);
	gtk_table_attach(GTK_TABLE(table1), entryLower, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(entryLower, 114, 22);
	gtk_entry_set_text(GTK_ENTRY(entryLower), _("Genesis"));

	entryUpper = gtk_entry_new();
	gtk_widget_ref(entryUpper);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "entryUpper",
				 entryUpper,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryUpper);
	gtk_table_attach(GTK_TABLE(table1), entryUpper, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(entryUpper, 114, 22);
	gtk_entry_set_text(GTK_ENTRY(entryUpper), _("Revelation"));

	gtk_signal_connect(GTK_OBJECT(btnSearch), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSearch_clicked), s);
	gtk_object_set_data(GTK_OBJECT(s->app), "tooltips", tooltips);
}

void setupSB(SETTINGS * s)
{
	GList 
		*tmp;
	GtkWidget
		*button,
		*searchbutton,
		*ctree,
		*scrolledwindow1,
		*scrolledwindow2, 
		*vpSearch, 
		*vboxVL, 
		*vpVL, 
		*html, 
		*VLbutton;
	gint 
		sbtype = 0, 
		large_icons = 0;
	gchar 
		*filename, 
		group_name[256], 
		icon_size[10],
		modName[16]; 
	GdkPixbuf 
		*icon_pixbuf = NULL;


	tmp = NULL;
	if (s->showfavoritesgroup) {
		groupnum0 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Favorites"));
		filename = "Favorites.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);		
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum0,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			memset(modName, 0, 16);
			modNameFromDesc(modName, (gchar *) tmp->data);
			sbtype = 0;
			sbtype = sbtypefromModNameSBSW(modName);
			if (sbtype < 0)
				sbtype = 0;
			switch (sbtype) {
			case 0:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-un.png");
				break;
			case 1:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-bl.png");
				break;
			case 2:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-green.png");
				break;
			case 3:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-gold.png");
				break;
			}
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum0, -1,
						  "favorite",
						  (gchar *) tmp->data,
						  icon_pixbuf);
			tmp = g_list_next(tmp);

		}
	}
	if (s->showtextgroup) {
		groupnum1 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Bible Text"));
		filename = "BibleText.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum1,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-un.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum1, -1,
						  "bible text",
						  (gchar *) tmp->data,
						  icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showcomgroup) {
		groupnum2 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Commentaries"));
		filename = "Commentaries.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum2,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-bl.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum2, -1,
						  "commentary",
						  (gchar *) tmp->data,
						  icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showdictgroup) {
		groupnum3 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 e_utf8_from_locale_string(_("Dict/Lex")));
		filename = "Dictionaries.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum3,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-green.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum3, -1,
						  "dictionary",
						  (gchar *) tmp->data,
						  icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	/* GBS */
	if (s->showbookgroup) {
		groupnum8 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Book"));
		filename = "Books.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum8,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-gold.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum8, -1,
						  "book",
						  (gchar *) tmp->data,
						  icon_pixbuf);	
			tmp = g_list_next(tmp);
		}
	}
	g_list_free(tmp);
	
	if (s->showhistorygroup) {
		groupnum4 =
		    add_sb_group((EShortcutBar *) shortcut_bar, _("History"));
	}
	
	/*** add bookmark group to shortcut bar ***/
	scrolledwindow1 = e_vscrolled_bar_new(NULL);
	gtk_widget_ref(scrolledwindow1);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "scrolledwindow1",
				 scrolledwindow1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);
	
	ctree = gtk_ctree_new(3, 0);
	gtk_widget_ref(ctree);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "ctree", ctree,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ctree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), ctree);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 0, 280);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 1, 80);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 2, 80);
	
	s->ctree_widget =  ctree;
	
	button = gtk_button_new_with_label(_("Bookmarks"));
	gtk_widget_ref(button);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "button",
				 button,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button);
	groupnum5 = e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
					  scrolledwindow1, button, -1);

	loadtree(settings);

	scrolledwindow2 = e_vscrolled_bar_new(NULL);	
	gtk_widget_ref(scrolledwindow2);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "scrolledwindow2", scrolledwindow2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow2);

	vpSearch = gtk_viewport_new(NULL, NULL);
	gtk_widget_ref(vpSearch);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vpSearch",
				 vpSearch,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vpSearch);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), vpSearch);

	searchbutton = gtk_button_new_with_label(_("Search"));
	gtk_widget_ref(searchbutton);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "searchbutton",
				 searchbutton,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(searchbutton);

	setupSearchBar(vpSearch, settings);

	groupnum6 = e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
					  scrolledwindow2,
					  searchbutton, -1);
	s->searchbargroup = groupnum6;
/*************************************************************************************/
	vboxVL = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxVL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vboxVL",
				 vboxVL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vboxVL);

	vpVL = gtk_viewport_new(NULL, NULL);
	gtk_widget_ref(vpVL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vpVL", vpVL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vpVL);
	gtk_container_add(GTK_CONTAINER(vpVL), vboxVL);

	VLbutton = gtk_button_new_with_label(_("Verse List"));
	gtk_widget_ref(VLbutton);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "VLbutton", VLbutton,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(VLbutton);

	html = setupVerseListBar(vboxVL, settings);
	
	/* setup shortcut bar verse list sword stuff */
	setupVerseListSBSWORD(html);
	
	groupnum7 = e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
					 vpVL , VLbutton, -1);
	e_shortcut_bar_set_enable_drags((EShortcutBar *) shortcut_bar,
					TRUE);
	gtk_signal_connect(GTK_OBJECT(VLbutton), "clicked",
			   GTK_SIGNAL_FUNC(on_VLbutton_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_bar), "item_selected",
			   GTK_SIGNAL_FUNC(on_shortcut_bar_item_selected),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_model), "item_added",
			   GTK_SIGNAL_FUNC(on_item_added), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_model), "item_removed",
			   GTK_SIGNAL_FUNC(on_item_removed), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_bar), "shortcut_dragged",
			   GTK_SIGNAL_FUNC(on_shortcut_dragged), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_bar), "shortcut_dropped",
			   GTK_SIGNAL_FUNC(on_shortcut_dropped), NULL);
}

void update_shortcut_bar(SETTINGS * s)
{
	gint count, i, large_icons, current_group, sbtype = 0;
	GList *tmp;
	EShortcutBar *bar;
	gchar *filename, group_name[256], icon_size[10];
	GdkPixbuf *icon_pixbuf = NULL;
	gchar modName[16];

	bar = E_SHORTCUT_BAR(shortcut_bar);
	tmp = NULL;
	current_group =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar));
	count =
	    e_shortcut_model_get_num_groups(E_SHORTCUT_MODEL
					    (shortcut_model));
	//*** remove shortcutbar groups so we can load the one the user has sellected
	for (i = count - 1; i > -1; i--) {
		e_shortcut_model_remove_group(E_SHORTCUT_MODEL
					      (shortcut_model), i);
	}
	tmp = NULL;
	if (s->showfavoritesgroup) {
		groupnum0 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Favorites"));
		filename = "Favorites.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum0,
						     E_ICON_BAR_LARGE_ICONS);		
		while (tmp != NULL) {
			memset(modName, 0, 16);
			modNameFromDesc(modName, (gchar *) tmp->data);
			sbtype = 0;
			sbtype = sbtypefromModNameSBSW(modName);
			if (sbtype < 0)
				sbtype = 0;
			switch (sbtype) {
			case 0:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-un.png");
				break;
			case 1:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-bl.png");
				break;
			case 2:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-green.png");
				break;
			case 3:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-gold.png");
				break;
			}
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum0, -1,
						  "favorites",
						  (gchar *) tmp->data,
						  icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showtextgroup) {
		groupnum1 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Bible Text"));
		filename = "BibleText.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum1,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-un.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum1, -1,
						  "bible text",
						  (gchar *) tmp->data,
						  icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showcomgroup) {
		groupnum2 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Commentaries"));
		filename = "Commentaries.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum2,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-bl.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum2, -1,
						  "commentary",
						  (gchar *) tmp->data,
						  icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showdictgroup) {
		groupnum3 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 e_utf8_from_locale_string(_("Dict/Lex")));
		filename = "Dictionaries.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum3,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-green.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum3, -1,
						  "dictionary",
						  (gchar *) tmp->data,
						  icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	
	/* GBS */
	if (s->showbookgroup) {
		groupnum8 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Book"));
		filename = "Books.conf";
		tmp = loadshortcutbarSW(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum8,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
        gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/book-gold.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->model,
						  groupnum8, -1,
						  "book",
						  (gchar *) tmp->data,
						  icon_pixbuf);	
			tmp = g_list_next(tmp);
		}
	}
	
	if (s->showhistorygroup) {
		groupnum4 =
		    add_sb_group((EShortcutBar *) shortcut_bar, _("History"));
	}
	g_list_free(tmp);
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar),
					  current_group, TRUE);
	//g_list_free(tmp);
}

void
create_modlistmenu_sb(gint group_num, GtkWidget * menu,
		      GtkWidget * shortcut_menu_widget, gchar * modtype)
{
	GtkWidget *item;
	GList *glist;
	if(group_num !=  groupnum0) {
	item = gtk_menu_item_new_with_label(_("Add All Modules"));
	gtk_widget_ref(item);
	gtk_object_set_data_full(GTK_OBJECT(menu), "item",
				 item, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(shortcut_menu_widget),
			  item);
	
	gtk_signal_connect(GTK_OBJECT(item), "activate",
			   GTK_SIGNAL_FUNC
			   (on_add_all_activate),
			   (gchar*)modtype);
	}
	
	glist = NULL;
	glist = getModlistSW(modtype);
	while (glist != NULL) {
		item = gtk_menu_item_new_with_label((gchar *) glist->data);
		gtk_widget_ref(item);
		gtk_object_set_data_full(GTK_OBJECT(menu), "item",
					 item, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(shortcut_menu_widget),
				  item);
		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC
				   (on_add_shortcut_activate),
				   (gchar *) glist->data);
		glist = g_list_next(glist);
	}
	/****  we used g_strdup to fill the list so we need to g_free each item  ****/
	glist = g_list_first(glist);
	while (glist != NULL) {
		g_free((gchar*)glist->data);
		glist = g_list_next(glist);		
	}
	g_list_free(glist);
}

void gs_shortcut_model_get_item_info(GtkWidget *shortcutbar_widget,
					gint group_num,
					gint item_num,
					gchar **item_url,
					gchar **item_name)
{
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
					       (shortcutbar_widget)->model,
					       group_num,
					       item_num, 
						item_url, 
						item_name,
					       NULL);

	
}

gint gs_shortcut_model_get_num_items(GtkWidget *shortcutbar_widget, 
						gint group_num)
{
	return e_shortcut_model_get_num_items(E_SHORTCUT_BAR
			(shortcutbar_widget)->model, group_num);
	
}	

/******   end of file   ******/
