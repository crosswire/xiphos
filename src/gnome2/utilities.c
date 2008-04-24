/*
 * GnomeSword Bible Study Tool
 * utilities.c - support functions
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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
#include "gui/preferences_dialog.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/lists.h"
#include "main/settings.h"
#include "main/configs.h"
#include "main/sword.h"
 

gint gui_of2tf(const gchar * on_off)
{
	return (!strcmp(on_off, "On") ? TRUE : FALSE);
}

gchar *gui_tf2of(gint true_false)
{
	return (true_false ? "On" : "Off");
}

void gui_reassign_strdup(gchar **where, gchar *what)
{
	if (*where)
		g_free(*where);
	*where = g_strdup(what);
}


void gui_set_progressbar_text(GtkWidget * pb, gchar * text)
{
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pb), text);
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}  
}


void gui_set_progressbar_fraction(GtkWidget * pb, gdouble fraction)
{
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb), fraction);
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}  
}


void gui_set_combo_index(GtkWidget * combo, gint index)
{
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), index);
}


void gui_clear_combo(GtkWidget * combo)
{
	GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));	
	gtk_list_store_clear(GTK_LIST_STORE(model));
}

void gui_add_item_to_combo(GtkWidget * combo, gchar * item)
{
	GtkTreeIter iter;	
	GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
	gtk_list_store_append(GTK_LIST_STORE(model), &iter);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, (gchar*)item, -1);
}


/*
 * taken from galeon 
 * glade_signal_connect_func: used by glade_xml_signal_autoconnect_full
 */
void gui_glade_signal_connect_func (const gchar *cb_name, GObject *obj, 
			   const gchar *signal_name, const gchar *signal_data,
			   GObject *conn_obj, gboolean conn_after,
			   gpointer user_data)
{
	/** Module with all the symbols of the program */
	static GModule *mod_self = NULL;
	gpointer handler_func;

 	/* initialize gmodule */
	if (mod_self == NULL)
	{
		mod_self = g_module_open (NULL, 0);
		g_assert (mod_self != NULL);
	}

	/*g_print( "glade_signal_connect_func: cb_name = '%s', signal_name = '%s', signal_data = '%s'\n",
	  cb_name, signal_name, signal_data ); */
	
	if (g_module_symbol (mod_self, cb_name, &handler_func))
	{
		/* found callback */
		if (conn_obj)
		{
			if (conn_after)
			{
				g_signal_connect_object
                                        (obj, signal_name, 
                                         handler_func, conn_obj,
                                         G_CONNECT_AFTER);
			}
			else
			{
				g_signal_connect_object
                                        (obj, signal_name, 
                                         handler_func, conn_obj,
                                         G_CONNECT_SWAPPED);
			}
		}
		else
		{
			/* no conn_obj; use standard connect */
			gpointer data = NULL;
			
			data = user_data;
			
			if (conn_after)
			{
				g_signal_connect_after
					(obj, signal_name, 
					 handler_func, data);
			}
			else
			{
				g_signal_connect
					(obj, signal_name, 
					 handler_func, data);
			}
		}
	}
	else
	{
		GS_warning(("callback function not found: %s", cb_name));
	}
}


/**
 * taken form galeon-1.3.21
 * misc_general_user_file: returns the pathname of galeon shared files
 * (e.g., galeon.glade)
 *
 * fname: just the filename with no path information
 * critical: critical file? (halt if not found)
 */
gchar * gui_general_user_file (const char *fname, gboolean critical)
{
	static GHashTable *already_found = NULL;
	gchar *alternative[7];
	gchar *file;
	gint i;
	
	/* create cache hash table if it doesnt already exist */
	if (already_found == NULL)
	{
		already_found = g_hash_table_new_full (g_str_hash, g_str_equal,
						       g_free, g_free);
	}

        /* Have we already found this? */
	file = g_hash_table_lookup (already_found, fname);
	if (file != NULL)
	{
		if (g_file_test (file, G_FILE_TEST_EXISTS))
		{
			return g_strdup (file);
		}
		else
		{
			g_hash_table_remove (already_found, fname);
		}
	}

	/* try the default */
	file = g_build_filename (g_get_home_dir (), ".gnomesword", fname, NULL);
	
	/* success? */
	if (g_file_test (file, G_FILE_TEST_EXISTS))
	{
		/* add it to the set of found files */
		g_hash_table_insert (already_found, g_strdup (fname), 
				     g_strdup (file));
		return file;
	}
	g_free(file);

	/* specify alternate locations in order of precedence */
	i = 0;
	alternative[i++] = g_strdup (fname);
#ifdef MAINTAINER_MODE
	GS_message(("MAINTAINER_MODE"));
	/* generally only developers have any use for these */
	alternative[i++] = g_build_filename ("..", fname, NULL);
	alternative[i++] = g_build_filename ("ui", fname, NULL);
	alternative[i++] = g_build_filename ("..", "ui", fname, NULL);
	alternative[i++] = g_build_filename ("..", "..", "ui", fname, NULL);
	
#endif
	alternative[i++] = g_build_filename (SHARE_DIR, fname, NULL);
	alternative[i++] = NULL;  /* NULL terminator needed */
	
	/* select one of the alternatives */
	file = NULL;
	
	for (i = 0; alternative[i] != NULL; i++)
	{
		if (file == NULL &&  g_file_test (alternative[i], G_FILE_TEST_EXISTS)) 
		{
			file = alternative[i];
		}
		else
		{
			/* free unused string */
			g_free (alternative[i]);
		}
	}

	/* check for success */
	if (file != NULL)
	{
		/* warn if we're using other than the install default */
		if (!g_path_is_absolute (file))
		{
			gchar * cwd, *file1;
			GS_message( ("Using %s (usually OK)", file));

			/* Make it absolute */
			cwd = g_get_current_dir();
			file1 = g_build_filename (cwd, file, NULL);
			g_free (file);
			g_free (cwd);
			file = file1;
		}

		/* add it to the set of found files */
		g_hash_table_insert (already_found, g_strdup (fname), 
				     g_strdup (file));
	}
	/* if nothing then theres an error */
	else if (critical)
	{
		GS_error((_("%s not found"), fname));
	}

	/* return result */
	return file;
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
	gchar *buf;
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	gboolean valid;

	/* Check language */
	buf = strdup(language);
	if (!g_utf8_validate(buf,-1,NULL))
		language = _("Unknown");
	if (!g_unichar_isalnum(g_utf8_get_char(buf)) || (language == NULL))
		language = _("Unknown");
	g_free(buf);

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;
		buf = strdup(language);
		if(buf == NULL) {
			GS_print(("error: %s\n", error->message));
			g_error_free (error);
			return;
		}
		
		gtk_tree_model_get(model, &iter_iter, 0, &str_data, -1);
		if(!g_utf8_collate(g_utf8_casefold(buf,-1),
				      g_utf8_casefold(str_data,-1))) {
		/*if(!strcmp(buf,str_data)) {*/
			g_free(str_data);
			g_free(buf);
			return;
		}
		g_free(str_data);
		g_free(buf);
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
	buf = strdup(language);
	gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter, &iter);
	gtk_tree_store_set(GTK_TREE_STORE(model), 
			&child_iter, 
			0,
			(gchar *) buf, 
			-1);
	g_free(buf);
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
	gchar *buf;

	/* Check language */
	buf = strdup(language);
	if (!g_utf8_validate(buf,-1,NULL))
		language = _("Unknown");
	if (!g_unichar_isalnum(g_utf8_get_char(buf)) || (language == NULL))
		language = _("Unknown");
	g_free(buf);

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
	
	/*  Biblical Texts folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("Biblical Texts"), -1);

	if (is_sidebar) {
		gtk_tree_store_append(store, &child_iter, &iter);
		gtk_tree_store_set(store, &child_iter, 0,
				   _("Parallel View"), -1);
	}

	tmp = get_list(TEXT_LIST);
	while (tmp != NULL) {
		const gchar *buf = main_get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  Commentaries folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("Commentaries"), -1);

	tmp = get_list(COMM_LIST);
	while (tmp != NULL) {
		const gchar *buf = main_get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  Dictionaries folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("Dictionaries"), -1);

	tmp = get_list(DICT_LIST);
	while (tmp != NULL) {
		const gchar *buf = main_get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  Devotionals folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("Daily Devotionals"), -1);

	tmp = get_list(DEVOTION_LIST);
	while (tmp != NULL) {
		const gchar *buf = main_get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  General Books folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("General Books"), -1);

	tmp = get_list(GBS_LIST);
	while (tmp != NULL) {
		const gchar *buf = main_get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  Maps folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("Maps"), -1);

	tmp = get_list(MAP_LIST);
	while (tmp != NULL) {
		const gchar *buf = main_get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  Images folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 0, _("Images"), -1);

	tmp = get_list(IMAGE_LIST);
	while (tmp != NULL) {
		const gchar *buf = main_get_module_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
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
	
	return (key ? g_strdup(key) : NULL);
		
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
 *  ncr_to_utf8
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gchar * util_ncr2utf8(gchar * text)
 *
 * Description
 *   Converts a string containing numeric character references (NCR) to utf-8 encoding
 *
 *   A numeric character reference (NCR) is a common markup construct used in html
 *   pages. It consists of a short sequence of characters that represent a single
 *   character from the Universal Character Set (UCS) of Unicode.
 *   
 *   NCR markup : &#<unicode-value>;  ex: &#233;
 *
 * Return value
 *   gchar
 */

gchar * ncr_to_utf8(gchar * text)
{
	gchar *ncr;
	gunichar unicode;
	gchar utf8[7];
	gchar *result = NULL;
	gchar buf[strlen(text)+2];
	size_t count;
	guint len;
	GString *newtext;
	
	newtext = g_string_new(NULL);
	// add an extra char before text
	// to avoid a delimiter as first char
	strcpy(buf, " ");
	strcat(buf, text);
	// search for "&"
	result = strtok(buf, "&");
	newtext = g_string_append (newtext, result + 1);

	while( result != NULL ) {
       	result = strtok( NULL, "&" );
		GS_message(("result: %s",result));
       	if (result != NULL) {
       		// search for "#"
       		if ( strcspn(result, "#") == 0){
				// converts ncr value (string) to unicode (guint32)
   		       	count = strcspn(result + 1, ";"); 
 		      	ncr=g_strndup(result + 1, count );
 		     	unicode = 0;
				for (; *ncr != '\0' && *ncr >= '0' && *ncr <='9'; ncr++) 
 				  	unicode = (unicode * 10) + (*ncr - '0');
  				g_free(ncr - count);
				//  converts unicode char to utf8
  		  		if (g_unichar_validate(unicode)){
  		  			len = g_unichar_to_utf8(unicode, utf8);
  		  			utf8[len] = '\0';
  		  			newtext = g_string_append (newtext, utf8);
  		  		}
  		  		else {
  		  			g_string_append_printf(newtext,"&#%d;",unicode);
					GS_message(("src/gnome2/utilities.c ncr2utf8, invalid unicode char &#%d;\n", unicode));
				}
  		  		// remaining text added
  		  		if (strlen(result)>count)
  		  			g_string_append(newtext, result+count+2);
  			}
  			else {
				g_string_append_c(newtext, '&'); /* puts the '&' back for other escape codes */
  				g_string_append(newtext, result);
			}
  		}
   }
	return g_string_free (newtext, FALSE);

}

//
// for choosing variants, primary/secondary/all.
// perversely, this routine is called twice.
// it seems to be so that (first) old state can
// be undone, then (second) with new state.
// unfortunately, there is no argument provided which tells us so.
// since we have nothing to undo, we simply ignore it.
//
void reading_selector(char *modname,
		      char *key,
		      DIALOG_DATA *dialog,
		      GtkMenuItem *menuitem,
		      gpointer user_data)
{
	static gboolean this_time = FALSE;
	gchar *url;
	gboolean primary = 0, secondary = 0, all = 0;

	if (!this_time) {
		this_time = TRUE;	// next time, we'll do it.
		return;
	}

	switch ((int) GPOINTER_TO_INT(user_data))
	{
	case 0:
		primary = TRUE;  secondary = FALSE; all = FALSE;
		break;
	case 1:
		primary = FALSE; secondary = TRUE;  all = FALSE;
		break;
	case 2:
		primary = FALSE; secondary = FALSE; all = TRUE;
		break;
	default:
		g_message("invalid variant %d\n", (int) GPOINTER_TO_INT(user_data));
		gui_generic_warning("GnomeSword: invalid internal variant");
		break;
	}

	main_save_module_options(modname, "Primary Reading", primary);
	main_save_module_options(modname, "Secondary Reading", secondary);
	main_save_module_options(modname, "All Readings", all);

	url = g_strdup_printf("sword://%s/%s", modname, key);
	if (dialog)
		main_dialogs_url_handler(dialog, url, TRUE);
	else
		main_url_handler(url, TRUE);
	g_free(url);
	this_time = FALSE;	// next time, ignore.
}

/******   end of file   ******/
