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

#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/sidebar.h"
#include "main/lists.h"
#include "main/settings.h"
#include "main/configs.h"
#include "main/module.h"
#include "main/sword.h"

#include "backend/key.hh"
#include "backend/tree.hh"


enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_CAPTION,
	COL_MODULE,
	COL_OFFSET,
	N_COLUMNS
};

typedef struct {
	GdkPixbuf *pixbuf_opened;
	GdkPixbuf *pixbuf_closed;
	GdkPixbuf *pixbuf_helpdoc;
} TreePixbufs;

static TreePixbufs *pixbufs;


/******************************************************************************
 * Name
 *  add_children_to_root
 *
 * Synopsis
 *   #include "gui/gbs.h"
 *
 *   void add_children_to_root(gchar *bookname,
 *   				unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void main_add_children_to_tree(GtkTreeModel * model, GtkTreeIter iter,
				 gchar *mod_name, unsigned long offset)
{
	gchar buf[256];
	gchar *tmpbuf = NULL;
	GdkPixbuf *open;
	GdkPixbuf *closed;
	GtkTreeIter child_iter;


	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   -1);
  
	if (backend_gbs_treekey_first_child(offset)) {
		offset = backend_gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		tmpbuf = backend_gbs_get_treekey_local_name(offset);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		if (backend_gbs_treekey_has_children(offset)) {
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CAPTION, (gchar *) tmpbuf,
				   COL_MODULE, (gchar *) mod_name,
				   COL_OFFSET, (gchar *) buf, 
				   -1);
		} else {
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
				   COL_CLOSED_PIXBUF, NULL,
				   COL_CAPTION, (gchar *) tmpbuf,
				   COL_MODULE, (gchar *) mod_name,
				   COL_OFFSET, (gchar *) buf, 
				   -1);
		}
		free(tmpbuf);
	}

	while (backend_treekey_next_sibling(offset)) {
		offset = backend_gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		tmpbuf = backend_gbs_get_treekey_local_name(offset);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		if (gbs_treekey_has_children(offset)) {
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CAPTION, (gchar *) tmpbuf,
				   COL_MODULE, (gchar *) mod_name,
				   COL_OFFSET, (gchar *) buf, 
				   -1);
		} else {
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
				   COL_CLOSED_PIXBUF, NULL,
				   COL_CAPTION, (gchar *) tmpbuf,
				   COL_MODULE, (gchar *) mod_name,
				   COL_OFFSET, (gchar *) buf, 
				   -1);
		}
		free(tmpbuf);
	}
}

void main_create_pixbufs(void)
{
	pixbufs = g_new0(TreePixbufs, 1);
	pixbufs->pixbuf_closed = gtk_widget_render_icon(widgets.app,
                                             GNOME_STOCK_BOOK_BLUE, 
                                             GTK_ICON_SIZE_MENU,
                                             NULL);
	pixbufs->pixbuf_opened =
		gtk_widget_render_icon(widgets.app,
                                             GNOME_STOCK_BOOK_OPEN, 
                                             GTK_ICON_SIZE_MENU,
                                             NULL);
	pixbufs->pixbuf_helpdoc =
		gtk_widget_render_icon(widgets.app,
                                             GTK_STOCK_DND, 
                                             GTK_ICON_SIZE_MENU,
                                             NULL);
}


void main_add_verses_to_chapter(GtkTreeModel * model, GtkTreeIter iter, 
				const gchar * key)
{	
	gchar **work_buf = NULL;
	gchar *book = NULL;
	gint chapter;
	gint verses;
	gint i;
	GtkTreeIter child_iter;
	
	work_buf = g_strsplit (key,"/",4);
	if(!work_buf[2] || !work_buf[3]) {
		g_strfreev(work_buf);
		return;
	}
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
				COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
				-1);
	book = backend_get_book_from_key(work_buf[3]);
	chapter = backend_get_chapter_from_key (work_buf[3]);
	verses = backend_verse_count (work_buf[3]);
	
	for(i = 1;i < (verses + 1);i++) {
		gchar *buf = g_strdup_printf("%s %d", _("verse"), i);
		gchar *key = 
		       g_strdup_printf("sword://%s/%s %d:%d",
					work_buf[2],
					book,
					chapter,
					i);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
			   COL_CLOSED_PIXBUF, NULL,
			   COL_CAPTION, (gchar *) buf,
			   COL_MODULE, (gchar *) work_buf[2],
			   COL_OFFSET, (gchar *) key, 
			   -1);
		g_free(key);
		g_free(buf);
	}	
	g_strfreev(work_buf);
}



void main_add_chapters_to_book(GtkTreeModel * model, GtkTreeIter iter, 
				const gchar * key)
{	
	gchar **work_buf = NULL;
	gchar *book = NULL;
	gint chapters;
	gint i;
	GtkTreeIter child_iter;
	
	work_buf = g_strsplit (key,"/",4);
	if(!work_buf[2] || !work_buf[3]) {
		g_strfreev(work_buf);
		return;
	}
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
				COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
				-1);
	book = backend_get_book_from_key(work_buf[3]);
	chapters = backend_chapter_count (work_buf[3]);
	
	for(i = 1;i < (chapters + 1);i++) {
		gchar *buf = g_strdup_printf("%s %d", _("chapter"), i);
		gchar *key = 
		       g_strdup_printf("chapter://%s/%s %d:1",
					work_buf[2],
					book,
					i);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, (gchar *) buf,
			   COL_MODULE, (gchar *) work_buf[2],
			   COL_OFFSET, (gchar *) key, 
			   -1);
		g_free(key);
		g_free(buf);
	}	
	g_strfreev(work_buf);
}


void main_add_books_to_bible(GtkTreeModel * model, GtkTreeIter iter,
				const gchar * mod_name)
{
	GList *tmp = NULL;
	gint i;
	GtkTreeIter child_iter;
	
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
				COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
				-1);
	if(backend_module_has_testament(mod_name,  1)) {
		//g_warning("%s has OT",mod_name);
		tmp = get_list(BOOKS_LIST);
		for(i = 0;i < 39;i++) {
			gchar *buf = (gchar*) (g_list_nth(tmp,i))->data;
			gchar *key = 
			       g_strdup_printf("book://%s/%s 1:1",mod_name,buf);
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter);
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CAPTION, (gchar *) buf,
				   COL_MODULE, (gchar *) mod_name,
				   COL_OFFSET, (gchar *) key, 
				   -1);
			g_free(key);
			//g_warning(buf);
		}
	}
	if(backend_module_has_testament(mod_name,  2)) {
		//g_warning("%s has NT",mod_name);
		tmp = get_list(BOOKS_LIST);
		for(i = 39;i < 66;i++) {
			gchar *buf = (gchar*) (g_list_nth(tmp,i))->data;
			gchar *key = 
			       g_strdup_printf("book://%s/%s 1:1",mod_name,buf);
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter);
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CAPTION, (gchar *) buf,
				   COL_MODULE, (gchar *) mod_name,
				   COL_OFFSET, (gchar *) key, 
				   -1);
			g_free(key);
			//g_warning(buf);
		}

	}		
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/utilities.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_mod_treeview_button_one(GtkTreeModel *model, GtkTreeIter selected)
{
	gint sbtype;
	gchar *cap = NULL;
	gchar *mod = NULL;
	gchar *key = NULL;
	gint mod_type;
	
	
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 
				2, &cap, 
				3, &mod, 
				4, &key, 
				-1);
	if (!cap)
		return;
	
	if (!g_utf8_collate(cap, _("Parallel View"))) 
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_parallel_text),
						      1);
		/*gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (widgets. button_parallel_view),
					     TRUE);*/
	if (!g_utf8_collate(cap, _("Standard View"))) 
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_parallel_text),
						      0);
	/*
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (widgets. button_parallel_view),
					     FALSE);
	*/
	if (!mod)
		return;
	
	sbtype = get_mod_type(mod);
	switch (sbtype) {
	case 0:
		/*if (GTK_TOGGLE_BUTTON(widgets.button_parallel_view)->
		    active) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (widgets.
						      button_parallel_view),
						     FALSE);
		}
	*/
		gtk_notebook_set_current_page(GTK_NOTEBOOK(
					widgets.notebook_parallel_text), 0);
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)
			&& !key)
			main_add_books_to_bible(model, selected, mod);
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)
			&& strstr(key,"book:"))
			main_add_chapters_to_book(model, selected, key);
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)
			&& strstr(key,"chapter:"))
			main_add_verses_to_chapter(model, selected, key);
		if(key)
			gui_url_handler(key, TRUE);
		else
			gui_change_module_and_key(mod, settings.currentverse);
		break;
	case 1:
		settings.comm_showing = TRUE;
		gui_change_module_and_key(mod, settings.currentverse);
		break;
	case 2:
		gtk_notebook_set_current_page
		    (GTK_NOTEBOOK(widgets.workbook_lower), 0);
		gui_change_module_and_key(mod, settings.dictkey);
		break;
	case 3:
		main_set_book_mod(mod, (key)?atol(key):0);
		g_warning("main_mod_treeview_button_one = %s" ,key);
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)
			&& !key) {
			main_add_children_to_tree(model, 
					selected,
				 	mod, 
					gbs_get_treekey_offset());
		}
		if (!gtk_tree_model_iter_has_child (model, &selected)
			    && gbs_treekey_has_children(atoi(key))) {
				main_add_children_to_tree(model, 
				    		     selected,
						     mod,
				    		     atol(key));
				
		}
		if(key) 
			gui_change_module_and_key(mod, key);
		 else
			gui_change_module_and_key(mod, "0");
		break;
		
		
		break;
	}
	if(cap) g_free(cap);
	if(mod) g_free(mod);
	if(key) g_free(key);
	
	
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

		gtk_tree_model_get(model, &iter_iter, 2, &str_data, -1);
		if (!strcmp(language, str_data)) {
			g_free(str_data);
			return;
		}
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
	gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter,
			      &iter);
	gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, (gchar *) language,
			   COL_MODULE, NULL,
			   COL_OFFSET, NULL, 
			   -1);
	/*
	gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter, 0,
			   (gchar *) language, -1);
	*/

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

		gtk_tree_model_get(model, &iter_iter, 2, &str_data, -1);
		if (!strcmp(language, str_data)) {
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter_iter);
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CAPTION, (gchar *) module_name,
				   COL_MODULE, (gchar *) module_name,
				   COL_OFFSET, NULL, 
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

void main_load_module_tree(GtkWidget * tree)
{
	gint i;
	GtkTreeStore *store;
	GtkTreeIter iter;
	GtkTreeIter child_iter;
	GList *tmp = NULL;
	
	store = gtk_tree_store_new(	N_COLUMNS, 
					GDK_TYPE_PIXBUF,
					GDK_TYPE_PIXBUF,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING);
	gtk_tree_store_clear(store);
	
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Biblical Texts"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Biblical Texts"), 
			   -1);

	gtk_tree_store_append(store, &child_iter, &iter);
	gtk_tree_store_set(store, &child_iter, 
		   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
		   COL_CLOSED_PIXBUF, NULL,
		   COL_CAPTION, _("Parallel View"),
		   COL_MODULE, _("Parallel View"),
		   COL_OFFSET, _("Parallel View"), 
		   -1);
	gtk_tree_store_append(store, &child_iter, &iter);
	gtk_tree_store_set(store, &child_iter, 
		   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
		   COL_CLOSED_PIXBUF, NULL,
		   COL_CAPTION, _("Standard View"),
		   COL_MODULE, _("Standard View"),
		   COL_OFFSET, _("Standard View"), 
		   -1);

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
	gtk_tree_store_set(store, &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Commentaries"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Commentaries"), 
			   -1);

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
	gtk_tree_store_set(store, &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Dictionaries"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Dictionaries"), 
			   -1);

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
	gtk_tree_store_set(store, &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("General Books"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("General Books"), 
			   -1);

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


void main_add_mod_tree_columns(GtkTreeView * tree)
{
	GtkTreeViewColumn *column;
	GtkTreeViewColumn *column2;
	GtkCellRenderer *renderer;

	column = gtk_tree_view_column_new();

	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes
	    (column, renderer,
	     "pixbuf", COL_OPEN_PIXBUF,
	     "pixbuf-expander-open", COL_OPEN_PIXBUF,
	     "pixbuf-expander-closed", COL_CLOSED_PIXBUF, NULL);
	

	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_CAPTION, NULL);
	gtk_tree_view_append_column(tree, column);


	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_MODULE, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_OFFSET, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);
}


/******   end of file   ******/
