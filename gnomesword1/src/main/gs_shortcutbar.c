/*
 * GnomeSword Bible Study Tool
 * gs_shortcutbar.c - SHORT DESCRIPTION
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/shortcut-bar/e-vscrolled-bar.h>
#include <gal/widgets/e-unicode.h>

/* frontend */
#include "shortcutbar_dialog.h"
#include "create_shortcutbar_search.h"
#include "create_shortcutbar_viewer.h"

/* main */ 
#include "gs_shortcutbar.h"
#include "gs_gnomesword.h"
#include "support.h"
#include "gs_bookmarks.h"
#include "gbs.h"
#include "gs_html.h"
#include "gs_editor.h"
#include "settings.h"
#include "lists.h"

/* backend */
#include "search_.h"
#include "sword.h"
#include "verselist.h"
#include "shortcutbar.h"

#define HTML_START "<html><head><meta http-equiv='content-type' content='text/html; charset=utf8'></head>"


GtkWidget *shortcut_bar;
EShortcutModel *shortcut_model;
SEARCH_OPT so, *p_so;

extern SB_VIEWER sb_v, *sv ;

extern gchar *shortcut_types[];
extern gboolean havedict;	/* let us know if we have at least one lex/dict module */
extern gboolean havecomm;	/* let us know if we have at least one commentary module */
extern gboolean havebible;	/* let us know if we have at least one Bible text module */

extern GList *sblist;			/* for saving search results to bookmarks  */
gint 
	groupnum0 = -1, 
	groupnum1 = -1, 
	groupnum2 = -1, 
	groupnum3 = -1, 
	groupnum4 = -1, 
	groupnum5 = -1, 
	groupnum6 = -1, 
	groupnum7 = -1, 
	groupnum8 = -1;	


static void on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
					  GdkEvent * event,
					  gint group_num,
					  gint item_num);
static void show_standard_popup(EShortcutBar * shortcut_bar,
				GdkEvent * event, gint group_num);
static void show_context_popup(EShortcutBar * shortcut_bar,
			       GdkEvent * event,
			       gint group_num, gint item_num);
static void set_small_icons(GtkWidget * menuitem,
			    EShortcutBar * shortcut_bar);
static void set_large_icons(GtkWidget * menuitem,
			    EShortcutBar * shortcut_bar);
static gint add_sb_group(EShortcutBar * shortcut_bar,
			 gchar * group_name);
static void on_item_added(EShortcutModel * shortcut_model,
			  gint group_num, gint item_num);
static void on_item_removed(EShortcutModel * shortcut_model,
			    gint group_num, gint item_num);
static void on_add_all_activate(GtkMenuItem * menuitem,
				gpointer user_data);
static void remove_all_items(gint group_num);



void search_module(SETTINGS * s, SEARCH_OPT * so)
{
	if (sblist)
		g_list_free(sblist);
	sblist = NULL;
	sblist = backend_do_search(s, (gpointer*)so);
	fill_search_results_clist(sblist, so, s);
}

gboolean display_dictlex_in_viewer(char * modName, char * key, 
						SETTINGS * s)
{
	if (s->showshortcutbar) {	
		gtk_notebook_set_page(GTK_NOTEBOOK(
			s->verse_list_notebook), 2);
		sprintf(s->groupName, "%s", "Viewer");
		showSBVerseList(s);
		backend_display_sb_dictlex(modName, key);
		return TRUE;
	}
	return FALSE;
}
void display_verse_list(gchar * module_name, gchar * verse_list,
				SETTINGS * s)
{
	GList * tmp = NULL;
	gboolean oddkey = TRUE;
	gchar 
		buf[256], 
		*utf8str,
		*colorkey;
	
	tmp = backend_get_verse_list(module_name, verse_list, s);
	
	beginHTML(s->vlsbhtml, TRUE);
	sprintf(buf,"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><font color=\"%s\"><b>[%s]</b><br></font>",
			s->bible_bg_color, 
			s->bible_text_color,
			s->link_color,
			s->bible_verse_num_color,
			module_name);
	utf8str = e_utf8_from_gtk_string(s->vlsbhtml, buf);
	displayHTML(s->vlsbhtml, utf8str, strlen(utf8str));
	
	while(tmp != NULL) {		
		if(oddkey){
			colorkey = s->link_color;
			oddkey = FALSE;
		}else{
			colorkey = s->bible_text_color;
			oddkey = TRUE;
		}
		sprintf(buf,"<a href=\"%s\"><font color=\"%s\"size=\"%s\">%s</font></a><br>",
					(const char *)tmp->data,
					colorkey,
					s->verselist_font_size,
					(const char *)tmp->data);
		utf8str = e_utf8_from_gtk_string(s->vlsbhtml, buf);
		displayHTML(s->vlsbhtml, utf8str, strlen(utf8str));
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	sprintf(buf,"</table></body</html>");	
	utf8str = e_utf8_from_gtk_string(s->vlsbhtml, buf);
	displayHTML(s->vlsbhtml, utf8str, strlen(utf8str));
	endHTML(s->vlsbhtml);
	
	showSBVerseList(s);
	gtk_notebook_set_page(GTK_NOTEBOOK(s->verse_list_notebook), 0);	
}

static void on_VLbutton_clicked(GtkButton * button, SETTINGS * s)
{

}

void change_viewer_page(GtkWidget *notebook, gint page)
{	
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), page);
	switch(page) {
		case 0:
			changegroupnameSB(&settings, _("Verse List"), groupnum7);
			break;
		case 1:
			changegroupnameSB(&settings, _("Search Results"), groupnum7);
			break;
		case 2:
			changegroupnameSB(&settings, _("Viewer"), groupnum7);
			break;
			
	}	
}

static void show_search_results_in_main(gboolean show, gchar *key)
{

	if (show) {
		if (GTK_TOGGLE_BUTTON(p_so->ckbCommentary)->active) {
			change_module_and_key(p_so->module_name, key);
		}

		else if (GTK_TOGGLE_BUTTON(p_so->ckbPerCom)->active) {
			change_module_and_key(p_so->module_name, key);
		}

		else if (GTK_TOGGLE_BUTTON(p_so->ckbGBS)->active) {
			change_module_and_key(p_so->module_name, key);
		}

		else
			change_verse(key);
	}
}

void fill_search_results_clist(GList *glist, SEARCH_OPT *so, SETTINGS *s)
{
	GList *tmp = NULL;
	gchar *utf8str, buf[256];
	gint i = 0;
	
	tmp = glist;
	gtk_clist_clear(GTK_CLIST(sv->clist));
	while (tmp != NULL) {	
		gchar *buf1 = (gchar*)tmp->data;
		gchar *token = strtok(buf1, "|");
		buf1 = token;
		token = strtok(NULL, "|");
		buf1 = token;
		gtk_clist_insert(GTK_CLIST(sv->clist), i, &buf1);
		++i;
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	sprintf(s->groupName,"%s","Search Results");
	sprintf(buf,"%d matches",i);
	gnome_appbar_set_status (GNOME_APPBAR (s->appbar), buf);
	gtk_notebook_set_page(GTK_NOTEBOOK(sv->notebook), 1);
	showSBVerseList(s);
	
	/* report results */
	beginHTML(s->htmlRP, TRUE);
	sprintf(buf,HTML_START 
	    "<body><center>%d Occurrences of <br><font color=\"%s\"><b>\"%s\"</b></font><br>found in <font color=\"%s\"><b>[%s]</b></font></center></body</html>", 
				i, s->found_color,s->searchText,
				s->bible_verse_num_color,so->module_name);	
	utf8str = e_utf8_from_gtk_string(s->htmlRP, buf);
	displayHTML(s->htmlRP, utf8str, strlen(utf8str));
	endHTML(s->htmlRP);	
	
	/* cleanup appbar progress */
	gnome_appbar_set_progress ((GnomeAppBar *)s->appbar, 0);
	/* display first item in list by selection row 0 */
	gtk_clist_select_row(GTK_CLIST(sv->clist), 0, 0);	
}

void search_results_select_row(GtkCList * clist,
				 gint row,
				 gint column,
				 GdkEvent * event, SETTINGS * s)
{
	gchar *buf;

	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &buf);
	s->displaySearchResults = TRUE;
	backend_search_results_item_display(buf);
	if (s->showinmain)
		show_search_results_in_main(s->showinmain,buf);
	s->displaySearchResults = FALSE;
}

/*** set shortcut bar to verse list group ***/
void showSBVerseList(SETTINGS * s)
{
	showSBGroup(s, groupnum7);
	changegroupnameSB(s, s->groupName, groupnum7);
}

/*** set shortcut bar to Daily Devotion group ***/
void set_sb_for_daily_devotion(SETTINGS * s)
{
	showSBGroup(s, groupnum7);
	changegroupnameSB(s, _("Daily Devotion"), groupnum7);
}

/*** set shortcut bar group ***/
void showSBGroup(SETTINGS * s, gint groupnum)
{
	EShortcutBar *bar1;
	if (!s->showshortcutbar) {
		gint biblepanesize;
		biblepanesize =
		    (settings.gs_width -
		     settings.shortcutbar_width) / 2;
		e_paned_set_position(E_PANED(s->epaned),
				     s->shortcutbar_width);
		e_paned_set_position(E_PANED
				     (lookup_widget
				      (s->app, "hpaned1")),
				     biblepanesize);
		s->showshortcutbar = TRUE;
	}
	bar1 = E_SHORTCUT_BAR(s->shortcut_bar);
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar1),
					  groupnum, TRUE);
}


void changegroupnameSB(SETTINGS * s, gchar * groupName, gint groupNum)
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

	group_name =
	    e_shortcut_model_get_group_name(E_SHORTCUT_BAR
					    (shortcut_bar)->model,
					    group_num);

	if (group_num == groupnum0) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum0) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Favorites.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("Favorites.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum1) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum1) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("BibleText.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("BibleText.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum2) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum2) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Commentaries.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("Commentaries.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum3) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum3) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Dictionaries.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("Dictionaries.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum8) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum8) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Books.conf", group_name,
					  group_num, "1");
		else
			backend_save_sb_group("Books.conf", group_name,
					  group_num, "0");
	}
}

static void
on_shortcut_dragged(EShortcutBar * shortcut_bar,
		    gint group_num, gint item_num)
{
	g_print("In on_shortcut_dragged Group:%i Item:%i\n",
		group_num, item_num);

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

	g_print("In on_shortcut_dropped Group:%i Item:%i\n",
		group_num, item_num);
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				       (shortcut_bar)->model,
				       group_num, item_num, NULL,
				       NULL, &icon_pixbuf);
	e_shortcut_model_add_item(shortcut_bar->model, group_num,
				  item_num, url, name, icon_pixbuf);
	savegroup((EShortcutBar *) shortcut_bar, group_num);
}

static void on_about_item_activate(GtkMenuItem * menuitem,
				   gpointer data)
{
	gint group_num, item_num;
	gchar *item_url, *item_name;
	EShortcutBar *bar1;
	gchar modname[16];

	item_num = GPOINTER_TO_INT(data);
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				       (shortcut_bar)->model,
				       group_num, item_num, &item_url,
				       &item_name, NULL);
	memset(modname, 0, 16);
	backend_module_name_from_description(modname, item_name);
	display_about_module_dialog(modname, FALSE);
	g_free(item_url);
	g_free(item_name);
}

static void remove_all_items(gint group_num)
{
	gint items, i;

	items = e_shortcut_model_get_num_items(shortcut_model,
					       group_num);
	for (i = items - 1; i > -1; i--) {
		e_shortcut_model_remove_item(shortcut_model,
					     group_num, i);
	}
}

static void on_remove_item_activate(GtkMenuItem * menuitem,
				    gpointer data)
{
	gint group_num, item_num;
	gchar *group_name;
	EShortcutBar *bar1;


	item_num = GPOINTER_TO_INT(data);
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));

	e_shortcut_model_remove_item(shortcut_model, group_num,
				     item_num);
	group_name =
	    e_shortcut_model_get_group_name(shortcut_model, group_num);

	if (group_num == groupnum0) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum0) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Favorites.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("Favorites.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum1) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum1) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("BibleText.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("BibleText.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum2) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum2) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Commentaries.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("Commentaries.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum3) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum3) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Dictionaries.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("Dictionaries.conf",
					  group_name, group_num, "0");
	}
}

void on_add_all_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GList *glist;
	gint number_of_items = 0, group_num, sbtype;
	EShortcutBar *bar1;
	gchar *group_name, modName[16];
	GdkPixbuf *icon_pixbuf = NULL;

	glist = NULL;
	switch(GPOINTER_TO_INT(user_data)) {
		case TEXT_TYPE:
			glist = get_list(TEXT_DESC_LIST);
		break;
		case COMMENTARY_TYPE:
			glist = get_list(COMM_DESC_LIST);
		break;
		case DICTIONARY_TYPE:
			glist = get_list(DICT_DESC_LIST);
		break;
		case BOOK_TYPE:
			glist = get_list(GBS_DESC_LIST);
		break;
	}
	
	//glist = backend_get_mod_description_list_SWORD((gchar *) user_data);

	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	group_name = "";
	group_name =
	    e_shortcut_model_get_group_name(E_SHORTCUT_BAR
					    (shortcut_bar)->model,
					    group_num);
	remove_all_items(group_num);

	while (glist != NULL) {
		memset(modName, 0, 16);
		backend_module_name_from_description(modName, (gchar *) glist->data);
		sbtype = 0;
		sbtype = backend_sb_type_from_modname(modName);
		if (sbtype < 0)
			sbtype = 0;

		switch (sbtype) {
		case 0:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-un.png");
			break;
		case 1:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-bl.png");
			break;
		case 2:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-green.png");
			break;
		case 3:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-gold.png");
			break;
		}

		e_shortcut_model_add_item(E_SHORTCUT_BAR
					  (shortcut_bar)->model,
					  group_num, -1, "not null",
					  (gchar *) glist->data,
					  icon_pixbuf);
		++number_of_items;
		glist = g_list_next(glist);
	}
	g_list_free(glist);
	savegroup(E_SHORTCUT_BAR(shortcut_bar), group_num);
	/****  we used g_strdup to fill the list so we need to g_free each item  ****/
	/*glist = g_list_first(glist);
	while (glist != NULL) {
		g_free((gchar *) glist->data);
		glist = g_list_next(glist);
	}
	g_list_free(glist);*/

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
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	memset(modName, 0, 16);
	backend_module_name_from_description(modName, (gchar *) user_data);
	sbtype = 0;
	sbtype = backend_sb_type_from_modname(modName);
	if (sbtype < 0)
		sbtype = 0;

	switch (sbtype) {
	case 0:
		icon_pixbuf =
		    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
					     "/book-un.png");
		break;
	case 1:
		icon_pixbuf =
		    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
					     "/book-bl.png");
		break;
	case 2:
		icon_pixbuf =
		    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
					     "/book-green.png");
		break;
	}
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->
				  model, group_num, -1, "not null",
				  (gchar *) user_data, icon_pixbuf);

	group_name = "";
	group_name = e_shortcut_model_get_group_name(E_SHORTCUT_BAR
						     (shortcut_bar)->
						     model, group_num);

	if (group_num == groupnum0) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum0) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Favorites.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("Favorites.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum1) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum1) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("BibleText.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("BibleText.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum2) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum2) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Commentaries.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("Commentaries.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum3) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum3) == E_ICON_BAR_LARGE_ICONS)
			backend_save_sb_group("Dictionaries.conf",
					  group_name, group_num, "1");
		else
			backend_save_sb_group("Dictionaries.conf",
					  group_name, group_num, "0");
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
static gint add_sb_group(EShortcutBar * shortcut_bar,
			 gchar * group_name)
{
	gint group_num;

	group_num =
	    e_shortcut_model_add_group(shortcut_bar->model, -1,
				       group_name);
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_SMALL_ICONS);
	return group_num;
}


/*** show hide shortcut bar ***/
void on_btnSB_clicked(GtkButton * button, SETTINGS * s)
{
	if (!s->docked) {
		gdk_window_raise(GTK_WIDGET(s->dockSB)->window);
		return;
	}

	if (settings.showshortcutbar) {
		settings.showshortcutbar = FALSE;
		s->biblepane_width = s->gs_width / 2;
		gtk_widget_hide(shortcut_bar);
		e_paned_set_position(E_PANED(s->epaned), 0);
		e_paned_set_position(E_PANED
				     (lookup_widget
				      (s->app, "hpaned1")),
				     s->biblepane_width);
	} else {
		s->showshortcutbar = TRUE;
		s->biblepane_width =
		    (s->gs_width - s->shortcutbar_width) / 2;
		e_paned_set_position(E_PANED(s->epaned),
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

	group_num =
	    GPOINTER_TO_INT(gtk_object_get_data
			    (GTK_OBJECT(menu), "group_num"));
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_LARGE_ICONS);
	if (group_num == groupnum0)
		backend_save_sb_iconsize("Favorites.conf", "1");
	if (group_num == groupnum1)
		backend_save_sb_iconsize("BibleText.conf", "1");
	if (group_num == groupnum2)
		backend_save_sb_iconsize("Commentaries.conf", "1");
	if (group_num == groupnum3)
		backend_save_sb_iconsize("Dictionaries.conf", "1");
}

static void
set_small_icons(GtkWidget * menuitem, EShortcutBar * shortcut_bar)
{
	GtkWidget *menu;
	gint group_num;

	menu = menuitem->parent;
	g_return_if_fail(GTK_IS_MENU(menu));

	group_num =
	    GPOINTER_TO_INT(gtk_object_get_data
			    (GTK_OBJECT(menu), "group_num"));
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_SMALL_ICONS);
	if (group_num == groupnum0)
		backend_save_sb_iconsize("Favorites.conf", "0");
	if (group_num == groupnum1)
		backend_save_sb_iconsize("BibleText.conf", "0");
	if (group_num == groupnum2)
		backend_save_sb_iconsize("Commentaries.conf", "0");
	if (group_num == groupnum3)
		backend_save_sb_iconsize("Dictionaries.conf", "0");
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
			   GTK_SIGNAL_FUNC(set_large_icons),
			   shortcut_bar);

	menuitem = gtk_menu_item_new_with_label(_("Small Icons"));
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC(set_small_icons),
			   shortcut_bar);

	menuitem = gtk_menu_item_new();
	gtk_widget_set_sensitive(menuitem, FALSE);
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);

	if (group_num == groupnum0) {
		menuitem =
		    gtk_menu_item_new_with_label(_("Add Bible Text"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		create_modlistmenu_sb(group_num, menuitem, menu_item_menu, TEXT_TYPE);

		menuitem =
		    gtk_menu_item_new_with_label(_("Add Commentary"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		create_modlistmenu_sb(group_num, menuitem, menu_item_menu, COMMENTARY_TYPE);


		menuitem =
		    gtk_menu_item_new_with_label(_("Add Dictionary"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		create_modlistmenu_sb(group_num, menuitem, menu_item_menu, DICTIONARY_TYPE);
		

		menuitem = gtk_menu_item_new_with_label(_("Add Book"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		create_modlistmenu_sb(group_num, menuitem, menu_item_menu, BOOK_TYPE);	
	}

	else {
		menuitem =
		    gtk_menu_item_new_with_label(_("Add Shortcut"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
	/*** add bookmark items ***/
		if (group_num == groupnum1)
			create_modlistmenu_sb(group_num, menuitem, menu_item_menu, TEXT_TYPE);

		if (group_num == groupnum2)
			create_modlistmenu_sb(group_num, menuitem, menu_item_menu, COMMENTARY_TYPE);

		if (group_num == groupnum3)
			create_modlistmenu_sb(group_num, menuitem, menu_item_menu, DICTIONARY_TYPE);	

		if (group_num == groupnum8)
			create_modlistmenu_sb(group_num, menuitem, menu_item_menu, BOOK_TYPE);

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
			   GTK_SIGNAL_FUNC
			   (on_remove_item_activate),
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

//      if(item_num == -1) {
//              if(group_num == groupnum2) /* change work space notebook to commentary page */
//                      gtk_notebook_set_page(GTK_NOTEBOOK(settings.workbook),0);
//              if(group_num == groupnum3) /* change Dictionayr - Books notebook to Dict page */
//                      gtk_notebook_set_page(GTK_NOTEBOOK(settings.workbook_lower),0);
//              if(group_num == groupnum8) /* change Dictionayr - Books notebook to Dict page */
//                      gtk_notebook_set_page(GTK_NOTEBOOK(settings.workbook_lower),1);
//      }

	if (event->button.button == 1) {
		if (item_num > -1) {
			app =
			    gtk_widget_get_toplevel(GTK_WIDGET
						    (shortcut_bar));
			e_shortcut_model_get_item_info(E_SHORTCUT_BAR
						       (shortcut_bar)->
						       model,
						       group_num,
						       item_num,
						       &type, &ref,
						       &icon_pixbuf);
			memset(modName, 0, 16);
			backend_module_name_from_description(modName, ref);

			if (group_num == groupnum0) {
				gint sbtype;
				sbtype = backend_sb_type_from_modname(modName);
				if (sbtype == 0 || sbtype == 1)
					change_module_and_key(modName,
							  settings.currentverse);
				else if (sbtype == 3) {
					gtk_notebook_set_page
					    (GTK_NOTEBOOK
					     (settings.workbook_lower), 1);
					change_module_and_key(modName,
							  NULL);
				}

				else
					change_module_and_key(modName,
							  settings.dictkey);
			}

			if (group_num == groupnum1) {
				if (havebible) {
					change_module_and_key(modName,
							  settings.currentverse);
				}
			}

			if (group_num == groupnum2) {
				if (havecomm) {
					change_module_and_key(modName,
							  settings.currentverse);
					gtk_notebook_set_page
					    (GTK_NOTEBOOK
					     (settings.workbook), 0);
				}
			}

			if (group_num == groupnum3) {
				if (havedict) {
					change_module_and_key(modName,
							  settings.dictkey);
					gtk_notebook_set_page
					    (GTK_NOTEBOOK
					     (settings.workbook_lower), 0);
				}
			}

			if (group_num == groupnum4) {
				change_verse(ref);
			}

			if (group_num == groupnum8) {
				change_module_and_key(modName, NULL);
				gtk_notebook_set_page(GTK_NOTEBOOK
						      (settings.workbook_lower),
						      1);
			}
			g_free(type);
			g_free(ref);
		}
	} else if (event->button.button == 3) {
		if (remItemNum == -1)
			show_standard_popup(shortcut_bar, event,
					    group_num);
		else
			show_context_popup(shortcut_bar, event,
					   group_num, item_num);
	}

}




void
srlink_clicked(GtkWidget *html_widget, const gchar * url, SETTINGS * s)
{
	gchar *buf, *modbuf = NULL, newmod[80], newref[80];
	gint i = 0, havemod = 0;
	GtkHTML *html;
	
	html = GTK_HTML(html_widget);
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
		change_module_and_key(modbuf, buf);
		g_free(buf);
	}
}

void save_verse_list_clicked(SETTINGS * s)
{	
	addverselistBM(s, sblist);
}

void vl_link_clicked(const gchar * url, SETTINGS * s)
{
	backend_verselist_change_verse(s, (gchar *) url);
	if(s->showinmain)
		change_verse((gchar *) url);	
}
void setupSB(SETTINGS * s)
{
	GList *tmp;
	GtkWidget
	    * button,
	    *searchbutton,
	    *ctree,
	    *scrolledwindow1,
	    *scrolledwindow2,
	    *vpSearch, *vboxVL, *vpVL, *html, *VLbutton;
	gint sbtype = 0, large_icons = 0;
	gchar *filename, group_name[256], icon_size[10], modName[16];
	GdkPixbuf *icon_pixbuf = NULL;

	/*
	 * shortcut bar 
	 */
	shortcut_model = e_shortcut_model_new();

	shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(shortcut_bar),
				 shortcut_model);
	if(s->showshortcutbar)
		gtk_widget_show(shortcut_bar);
	else
		gtk_widget_hide(shortcut_bar);

	s->shortcut_bar = shortcut_bar;

	e_paned_pack1(E_PANED(s->epaned), shortcut_bar, FALSE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(shortcut_bar), 4);
	/*
	 * end shortcut bar 
	*/
	

	tmp = NULL;
	if (s->showfavoritesgroup) {
		groupnum0 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Favorites"));
		filename = "Favorites.conf";
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum0,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			memset(modName, 0, 16);
			backend_module_name_from_description(modName, (gchar *) tmp->data);
			sbtype = 0;
			sbtype = backend_sb_type_from_modname(modName);
			if (sbtype < 0)
				sbtype = 0;
			switch (sbtype) {
			case 0:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-un.png");
				break;
			case 1:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-bl.png");
				break;
			case 2:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-green.png");
				break;
			case 3:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-gold.png");
				break;
			}
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum0,
						  -1, "favorite",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);

		}
	}
	if (s->showtextgroup) {
		groupnum1 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Bible Text"));
		filename = "BibleText.conf";
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum1,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-un.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum1,
						  -1, "bible text",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showcomgroup) {
		groupnum2 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Commentaries"));
		filename = "Commentaries.conf";
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum2,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-bl.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum2,
						  -1, "commentary",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showdictgroup) {
		groupnum3 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 e_utf8_from_locale_string(_
							   ("Dict/Lex")));
		filename = "Dictionaries.conf";
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum3,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-green.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum3,
						  -1, "dictionary",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	/* GBS */
	if (s->showbookgroup) {
		groupnum8 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Book"));
		filename = "Books.conf";
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum8,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-gold.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum8,
						  -1, "book",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	g_list_free(tmp);

	if (s->showhistorygroup) {
		groupnum4 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("History"));
	}

	/*** add bookmark group to shortcut bar ***/
	scrolledwindow1 = e_vscrolled_bar_new(NULL);
	gtk_widget_ref(scrolledwindow1);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "scrolledwindow1", scrolledwindow1,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);

	ctree = gtk_ctree_new(3, 0);
	gtk_widget_ref(ctree);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "ctree",
				 ctree, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(ctree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), ctree);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 0, 280);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 1, 80);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 2, 80);

	s->ctree_widget = ctree;

	button = gtk_button_new_with_label(_("Bookmarks"));
	gtk_widget_ref(button);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "button",
				 button, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(button);
	groupnum5 =
	    e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
				  scrolledwindow1, button, -1);

	loadtree(&settings);

	scrolledwindow2 = e_vscrolled_bar_new(NULL);
	gtk_widget_ref(scrolledwindow2);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "scrolledwindow2", scrolledwindow2,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindow2);

	vpSearch = gtk_viewport_new(NULL, NULL);
	gtk_widget_ref(vpSearch);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vpSearch",
				 vpSearch, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vpSearch);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), vpSearch);

	searchbutton = gtk_button_new_with_label(_("Search"));
	gtk_widget_ref(searchbutton);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "searchbutton", searchbutton,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(searchbutton);

	gui_create_shortcutbar_search(vpSearch, &settings);

	groupnum6 =
	    e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
				  scrolledwindow2, searchbutton, -1);
	s->searchbargroup = groupnum6;
/*************************************************************************************/
	vboxVL = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxVL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vboxVL",
				 vboxVL, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vboxVL);

	vpVL = gtk_viewport_new(NULL, NULL);
	gtk_widget_ref(vpVL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vpVL", vpVL,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vpVL);
	gtk_container_add(GTK_CONTAINER(vpVL), vboxVL);

	VLbutton = gtk_button_new_with_label(_("Verse List"));
	gtk_widget_ref(VLbutton);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "VLbutton",
				 VLbutton, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(VLbutton);

	/*** create viewer group and add to shortcut bar ****/
	html = gui_create_shortcutbar_viewer(vboxVL, &settings);

	
	/* setup shortcut bar verse list sword stuff */
	backend_setup_verselist(html,s);
	backend_setup_search_results_display(sv->html_widget);
	backend_setup_viewer(sv->html_viewer_widget);
	
	groupnum7 =
	    e_group_bar_add_group(E_GROUP_BAR(shortcut_bar), vpVL,
				  VLbutton, -1);
	e_shortcut_bar_set_enable_drags((EShortcutBar *)
					shortcut_bar, TRUE);
	gtk_signal_connect(GTK_OBJECT(VLbutton), "clicked",
			   GTK_SIGNAL_FUNC(on_VLbutton_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_bar),
			   "item_selected",
			   GTK_SIGNAL_FUNC
			   (on_shortcut_bar_item_selected), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_model),
			   "item_added",
			   GTK_SIGNAL_FUNC(on_item_added), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_model),
			   "item_removed",
			   GTK_SIGNAL_FUNC(on_item_removed), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_bar),
			   "shortcut_dragged",
			   GTK_SIGNAL_FUNC(on_shortcut_dragged), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_bar),
			   "shortcut_dropped",
			   GTK_SIGNAL_FUNC(on_shortcut_dropped), NULL);
}
/* FIXME: this messes up the group number for the manully added groups
          bookmarks, search and viewer */
void update_shortcut_bar(SETTINGS * s)
{
	gint count, i, large_icons, current_group, sbtype = 0;
	GList *tmp;
	EShortcutBar *bar;
	gchar *filename, group_name[256], icon_size[10];
	GdkPixbuf *icon_pixbuf = NULL;
	gchar modName[16];
	
	groupnum0 = -1;
	groupnum1 = -1; 
	groupnum2 = -1; 
	groupnum3 = -1; 
	groupnum4 = -1; 
	//groupnum5 = -1; 
	//groupnum6 = -1; 
	//groupnum7 = -1; 
	groupnum8 = -1;	
	
	bar = E_SHORTCUT_BAR(shortcut_bar);
	tmp = NULL;
	current_group =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar));
	count =
	    e_shortcut_model_get_num_groups(E_SHORTCUT_MODEL
					    (shortcut_model));
	/*** remove shortcutbar groups so we can load the one the user has sellected ***/
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
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum0,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			memset(modName, 0, 16);
			backend_module_name_from_description(modName, (gchar *) tmp->data);
			sbtype = 0;
			sbtype = backend_sb_type_from_modname(modName);
			if (sbtype < 0)
				sbtype = 0;
			switch (sbtype) {
			case 0:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-un.png");
				break;
			case 1:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-bl.png");
				break;
			case 2:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-green.png");
				break;
			case 3:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-gold.png");
				break;
			}
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum0,
						  -1, "favorites",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showtextgroup) {
		groupnum1 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Bible Text"));
		filename = "BibleText.conf";
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum1,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-un.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum1,
						  -1, "bible text",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showcomgroup) {
		groupnum2 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Commentaries"));
		filename = "Commentaries.conf";
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum2,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-bl.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum2,
						  -1, "commentary",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (s->showdictgroup) {
		groupnum3 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 e_utf8_from_locale_string(_
							   ("Dict/Lex")));
		filename = "Dictionaries.conf";
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum3,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-green.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum3,
						  -1, "dictionary",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}

	/* GBS */
	if (s->showbookgroup) {
		groupnum8 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Book"));
		filename = "Books.conf";
		tmp =
		    backend_load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum8,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-gold.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum8,
						  -1, "book",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}

	if (s->showhistorygroup) {
		groupnum4 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("History"));
	}
	g_list_free(tmp);
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar),
					  current_group, TRUE);
}

void
create_modlistmenu_sb(gint group_num, GtkWidget * menu,
		      GtkWidget * shortcut_menu_widget, gint mod_type)
{
	GtkWidget *item;
	GList *glist = NULL;
	if (group_num != groupnum0) {
		item =
		    gtk_menu_item_new_with_label(_("Add All Modules"));
		gtk_widget_ref(item);
		gtk_object_set_data_full(GTK_OBJECT(menu), "item",
					 item, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER
				  (shortcut_menu_widget), item);

		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC
				   (on_add_all_activate),
				   GINT_TO_POINTER(mod_type));
	}

	
	switch(mod_type) {
		case TEXT_TYPE:
			glist = get_list(TEXT_DESC_LIST);
		break;
		case COMMENTARY_TYPE:
			glist = get_list(COMM_DESC_LIST);
		break;
		case DICTIONARY_TYPE:
			glist = get_list(DICT_DESC_LIST);
		break;
		case BOOK_TYPE:
			glist = get_list(GBS_DESC_LIST);
		break;
	}
	
	//glist = backend_get_mod_description_list_SWORD(modtype);
	while (glist != NULL) {
		item =
		    gtk_menu_item_new_with_label((gchar *) glist->data);
		gtk_widget_ref(item);
		gtk_object_set_data_full(GTK_OBJECT(menu), "item",
					 item, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER
				  (shortcut_menu_widget), item);
		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC
				   (on_add_shortcut_activate),
				   (gchar *) glist->data);
		glist = g_list_next(glist);
	}
	g_list_free(glist);
	/****  we used g_strdup to fill the list so we need to g_free each item  ****/
	/*glist = g_list_first(glist);
	while (glist != NULL) {
		g_free((gchar *) glist->data);
		glist = g_list_next(glist);
	}
	g_list_free(glist);*/
}

void gs_shortcut_model_get_item_info(GtkWidget * shortcutbar_widget,
				     gint group_num,
				     gint item_num,
				     gchar ** item_url,
				     gchar ** item_name)
{
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				       (shortcutbar_widget)->model,
				       group_num,
				       item_num,
				       item_url, item_name, NULL);


}

gint gs_shortcut_model_get_num_items(GtkWidget * shortcutbar_widget,
				     gint group_num)
{
	return e_shortcut_model_get_num_items(E_SHORTCUT_BAR
					      (shortcutbar_widget)->
					      model, group_num);

}

/******   end of file   ******/
