/*
 * GnomeSword Bible Study Tool
 * utilities.c - support functions
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
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>

#include "gui/utilities.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/lists.h"
#include "main/settings.h"
#include "main/configs.h"
#include "main/module.h"
#include "main/sword.h"



/******************************************************************************
 * Name
 *   sword_url_handler
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint sword_url_handler(const gchar * url)
 *
 * Description
 *   display a sword uri in the form 'sword://KJV/1John5:8'
 *                                   'sword://MHC/Genesis1:1'
 *   in the appropriate pane
 *
 * Return value
 *   gint
 */

gint sword_url_handler(const gchar * url)
{
	gchar *buf = NULL;
	gchar *module = NULL;
	const gchar *key = NULL;
	static gchar **work_buf = NULL;
	gint mod_type;
	gboolean change_verse = FALSE;
	
	if(!strstr(url,"sword://"))
		return 0;
	
	work_buf = g_strsplit (url,"/",4);
	if(check_for_module(work_buf[2])) {
		mod_type = get_mod_type(work_buf[2]);
		switch(mod_type) {
			case TEXT_TYPE:			
				key = get_valid_key(work_buf[3]);
				xml_set_value("GnomeSword", "modules", "bible",
				      work_buf[2]);
				settings.MainWindowModule = work_buf[2];
				if(strcmp(key,settings.currentverse))
					change_verse = TRUE;
				gui_change_module_and_key(
					settings.MainWindowModule, 
					key);
				if(change_verse) {
					settings.currentverse = (gchar*)key;
					gui_change_verse(settings.currentverse);
				}
				if(key) g_free((gchar*)key);
				return 1;	
			break;
			case COMMENTARY_TYPE:				
				key = get_valid_key(work_buf[3]);
				xml_set_value("GnomeSword", "modules", "comm",
				      work_buf[2]);
				settings.CommWindowModule = work_buf[2];
				if(strcmp(key,settings.currentverse))
					change_verse = TRUE;
				gui_change_module_and_key(
					settings.CommWindowModule,
					key);
				if(change_verse) {
					settings.currentverse = (gchar*)key;
					gui_change_verse(settings.currentverse);
				}
				if(key) g_free((gchar*)key);
				return 1;				
			break;
			case DICTIONARY_TYPE:
				settings.dictkey = work_buf[3];
				xml_set_value("GnomeSword", "modules", "dict",
				      work_buf[2]);
				settings.DictWindowModule = work_buf[2];
				gui_change_module_and_key(
					settings.DictWindowModule,
					settings.dictkey);
				gtk_notebook_set_current_page(
					GTK_NOTEBOOK(widgets.workbook_lower),0);
				return 1;	
			break;
			case BOOK_TYPE:
				settings.book_key = work_buf[3];
				xml_set_value("GnomeSword", "modules", "book",
				      work_buf[2]);
				settings.BookWindowModule = work_buf[2];
				gui_change_module_and_key(
					settings.BookWindowModule,
					settings.book_key);
				gtk_notebook_set_current_page(
					GTK_NOTEBOOK(widgets.workbook_lower),1);
				return 1;	
			break;
		}
	}
	return 0;
}


/******************************************************************************
 * Name
 *   add_language_folder
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   void add_language_folder(GtkTreeModel * model, GtkTreeIter iter,
 *			 gchar * language)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_language_folder(GtkTreeModel * model, GtkTreeIter iter,
				const gchar * language)
{
	GtkTreeIter iter_iter;
	GtkTreeIter parent;
	GtkTreeIter child_iter;
	gboolean valid;

	if ((!g_ascii_isalnum(language[0])) || (language == NULL))
		language = N_("Unknown");

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;

		gtk_tree_model_get(model, &iter_iter, 0, &str_data, -1);
		if (!strcmp(language, str_data)) {
			g_free(str_data);
			return;
		}
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
	gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter,
			      &iter);
	gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter, 0,
			   (gchar *) language, -1);

}


/******************************************************************************
 * Name
 *   add_module_to_language_folder
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   void add_module_to_language_folder(GtkTreeModel * model,
 *		      GtkTreeIter iter, gchar * language, gchar * module_name)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_module_to_language_folder(GtkTreeModel * model,
					  GtkTreeIter iter,
					  const gchar * language,
					  gchar * module_name)
{
	GtkTreeIter iter_iter;
	GtkTreeIter parent;
	GtkTreeIter child_iter;
	gboolean valid;

	if ((!g_ascii_isalnum(language[0])) || (language == NULL))
		language = N_("Unknown");

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;

		gtk_tree_model_get(model, &iter_iter, 0, &str_data, -1);
		if (!strcmp(language, str_data)) {
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter_iter);
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, 
					   0, (gchar *) module_name, 
					   -1);
			g_free(str_data);
			return;
		}
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
}


/******************************************************************************
 * Name
 *   load_module_tree
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   void load_module_tree(GtkWidget * tree)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_load_module_tree(GtkWidget * tree, gboolean is_sidebar)
{
	gint i;
	GtkTreeStore *store;
	GtkTreeIter iter;
	GtkTreeIter child_iter;
	GList *tmp = NULL;


	store = gtk_tree_store_new(1, G_TYPE_STRING);
	gtk_tree_store_clear(store);
	
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("Biblical Texts"), -1);

	if(is_sidebar) {
		
		gtk_tree_store_append(store, &child_iter, &iter);
		gtk_tree_store_set(store, &child_iter, 0,
				   _("Parallel View"), -1);
	}
	/*  add language folders Biblical Texts folder */
	tmp = get_list(TEXT_LIST);
	while (tmp != NULL) {
		const gchar *buf = get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		tmp = g_list_next(tmp);
	}

	/*  add modules to Biblical Texts language folders */
	tmp = get_list(TEXT_LIST);
	while (tmp != NULL) {
		const gchar *buf = get_module_language((gchar *) tmp->data);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  add language folders Commentaries folder */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("Commentaries"), -1);

	tmp = get_list(COMM_LIST);
	while (tmp != NULL) {
		const gchar *buf = get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		tmp = g_list_next(tmp);
	}

	/*  add modules to Commentaries language folders */
	tmp = get_list(COMM_LIST);
	while (tmp != NULL) {
		const gchar *buf = get_module_language((gchar *) tmp->data);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  add language folders Dictionaries folder */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("Dictionaries"), -1);

	tmp = get_list(DICT_LIST);
	while (tmp != NULL) {
		const gchar *buf = get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		tmp = g_list_next(tmp);

	}

	/*  add modules to Dict/Lex language folders */
	tmp = get_list(DICT_LIST);
	while (tmp != NULL) {
		const gchar *buf = get_module_language((gchar *) tmp->data);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}


	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("General Books"), -1);

	/*  add language folders Books folder */
	tmp = get_list(GBS_LIST);
	while (tmp != NULL) {
		const gchar *buf = get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		tmp = g_list_next(tmp);
	}

	/*  add modules to Books language folders */
	tmp = get_list(GBS_LIST);
	while (tmp != NULL) {
		const gchar *buf = get_module_language((gchar *) tmp->data);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree),
				GTK_TREE_MODEL(store));
}

/******************************************************************************
 * Name
 *  get_font
 *
 * Synopsis
 *   #include ".h"
 *
 *   MOD_FONT * get_font(gchar *mod_name)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

MOD_FONT *get_font(gchar * mod_name)
{
	MOD_FONT *mf;
	gchar file[250];

	sprintf(file, "%s/fonts.conf", settings.gSwordDir);

	mf = g_new(MOD_FONT, 1);
	mf->mod_name = mod_name;
	mf->old_font = NULL;
	mf->old_gdk_font = NULL;
	mf->old_font_size = NULL;
	mf->new_font = NULL;
	mf->new_gdk_font = NULL;
	mf->new_font_size = NULL;
	mf->no_font = 0;
	
	mf->old_font = get_conf_file_item(file, mod_name, "Font");
	mf->old_gdk_font = get_conf_file_item(file, mod_name, "GdkFont");
	mf->old_font_size = get_conf_file_item(file, mod_name, "Fontsize");
	//g_warning("mf->old_font = %s",mf->old_font);
	if (mf->old_font == NULL)
		mf->old_font = g_strdup("none");
	if(mf->old_font_size == NULL)
		mf->old_font_size = g_strdup("+0");
	
	return mf;
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include ".h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void free_font(MOD_FONT *mf)
{
	
	if(mf->old_font) g_free(mf->old_font);
	if(mf->old_gdk_font) g_free(mf->old_gdk_font);
	if(mf->old_font_size) g_free(mf->old_font_size);
	//mf->new_font = NULL;
	//mf->new_gdk_font = NULL;
	//mf->new_font_size = NULL;
	//mf->no_font = 0;
	g_free(mf);
}
/******************************************************************************
 * Name
 *   remove_linefeeds
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gchar * remove_linefeeds(gchar * buf)
 *
 * Description
 *   remove line feeds so we can display string in appbar
 *
 * Return value
 *   gchar * 
 */
 
gchar * remove_linefeeds(gchar * buf)
{	
	gchar * key = NULL;
	
	key = g_strdelimit(buf, "\n", ' ');
	
	if(key)
		return g_strdup(key);
	else
		return NULL;
		
}


/******************************************************************************
 * Name
 *   gui_add_item2gnome_menu
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   void gui_add_item2gnome_menu(GtkWidget * MainFrm, gchar * itemname, 
 *	gchar * itemdata, gchar * menuname, GCallback mycallback)
 *
 * Description
 *   adds an item to the main menu bar
 *
 * Return value
 *   void
 */

void gui_add_item2gnome_menu(GtkWidget * MainFrm, gchar * itemname, 
	gchar * itemdata, gchar * menuname, GCallback mycallback)
{
	GnomeUIInfo *menuitem;

	menuitem = g_new(GnomeUIInfo, 2);
	menuitem->type = GNOME_APP_UI_ITEM;
	menuitem->moreinfo = (gpointer) mycallback;
	menuitem->user_data = itemdata;
	menuitem->label = itemname;
	menuitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
	menuitem->pixmap_info = GNOME_STOCK_BOOK_OPEN;
	menuitem->accelerator_key = 0;
	menuitem[1].type = GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm), menuname,
					 menuitem, NULL);
}


/******************************************************************************
 * Name
 *   gui_add_separator2menu
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   void gui_add_separator2menu(GtkWidget * MainFrm, gchar * subtreelabel)
 *
 * Description
 *   add separator line to menu
 *
 * Return value
 *   void
 */

void gui_add_separator2menu(GtkWidget * MainFrm, gchar * subtreelabel)
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


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   void add_mods_to_menus(GList * modlist, gchar * menu,
 *				GCallback callback) 
 *
 * Description
 *   add a list of modules to a menu
 *
 * Return value
 *   void
 */

void gui_add_mods_to_menus(GList * modlist, gchar * menu,
				GCallback callback) 
{	
	gchar	
		view_remember_last_item[80];
	//gint	i = 0;
	GList 	*tmp = NULL;

	sprintf(view_remember_last_item,"%s", menu);
	
	
	tmp = modlist;
	while (tmp != NULL) {	
		gui_add_item2gnome_menu(widgets.app, 
			(gchar *) tmp->data, 
			(gchar *) tmp->data,
			view_remember_last_item, 
			callback);
		/* remember last item - so next item will be placed below it */
		sprintf(view_remember_last_item,"%s%s", 
				menu, 
				(gchar *) tmp->data);	
		//++i;
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
}			


/******************************************************************************
 * Name
 *   gui_remove_menu_items
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   void gui_remove_menu_items(gchar * startitem, gint numberofitems)
 *
 * Description
 *   remove a number(numberofitems) of items form a menu or submenu(startitem)
 *
 * Return value
 *   void
 */

void gui_remove_menu_items(gchar * startitem, gint numberofitems)
{				
	gnome_app_remove_menus(GNOME_APP(widgets.app), startitem,
			       numberofitems);
}			

/******************************************************************************
 * Name
 *  add_mods_2_gtk_menu
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   void add_mods_2_gtk_menu(gchar * mod_type, GtkMenu * menu,
				GCallback callback)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_add_mods_2_gtk_menu(gint mod_type, GtkWidget * menu,
				GCallback callback)
{
	GList 	*tmp = NULL;
	GtkWidget * item;
	
	tmp = get_list(mod_type);
	while (tmp != NULL) {	
		item =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_show(item);
		g_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK
				   (callback),
				   (gchar *) tmp->data);		
		gtk_container_add(GTK_CONTAINER(menu), item); 
		tmp = g_list_next(tmp);	
	}	
}



/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

GList *gui_fill_count_list(int count)
{
	GList *glist = NULL;
	gint start;
	gchar buf[32];
	
	for(start = 1; start <= count ; start++) {
		sprintf(buf, "%d", start);
		glist = g_list_append(glist,
				 (gchar *) g_strdup(buf));
	}
	return glist;
}			


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

void gui_free_count_list(GList *glist)
{
	glist = g_list_first(glist);
	while (glist != NULL) {
		gchar *buf = (gchar *) glist->data;
		g_free(buf);
		glist = g_list_next(glist);
	}
	g_list_free(glist);
}

/******   end of file   ******/
