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
#include <swmgr.h>
#include <swmodule.h>

#include "gui/gnomesword.h"
#include "gui/widgets.h"
#include "gui/sidebar.h"
#include "gui/tabbed_browser.h"

#include "main/sidebar.h"
#include "main/lists.h"
#include "main/navbar.h"
#include "main/navbar_book.h"
#include "main/prayer_list.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"

#include "backend/sword_main.hh"


enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_CAPTION,
	COL_MODULE,
	COL_OFFSET,
	N_COLUMNS
};

TreePixbufs *pixbufs;



/******************************************************************************
 * Name
 *   main_open_bookmark_in_new_tab
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_open_bookmark_in_new_tab(gchar * mod_name, gchar * key)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_open_bookmark_in_new_tab(gchar * mod_name, gchar * key)
{
	gint module_type = backend->module_type(mod_name);

	switch (module_type) {
	case -1:
		return;
		break;
	case TEXT_TYPE:
	case COMMENTARY_TYPE:
		if (strcmp(settings.currentverse, key)) {
			xml_set_value("GnomeSword", "keys", "verse",
				      key);
			settings.currentverse =
			    xml_get_value("keys", "verse");
		}
		break;
	case DICTIONARY_TYPE:
		xml_set_value("GnomeSword", "keys", "dictionary", key);
		settings.dictkey = xml_get_value("keys", "dictionary");
		break;
	case BOOK_TYPE:
		xml_set_value("GnomeSword", "keys", "offset", key);
		settings.book_offset =
		    atol(xml_get_value("keys", "offset"));
		break;
	}
	gui_open_module_in_new_tab(mod_name);
}


/******************************************************************************
 * Name
 *   main_display_verse_list_in_sidebar
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_display_verse_list_in_sidebar(gchar * key, gchar * module_name,
 *				       gchar * verse_list)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_display_verse_list_in_sidebar(gchar * key,
					gchar * module_name,
					gchar * verse_list)
{
	GList *tmp = NULL;
	gint i = 0;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreePath *path;
	GtkTreeIter iter;
	RESULTS *list_item;

	GS_warning(("verse_list = %s",verse_list));
	list_of_verses = g_list_first(list_of_verses);
	if (list_of_verses) {
		while (list_of_verses) {
			list_item = (RESULTS *) list_of_verses->data;
			g_free(list_item->module);
			g_free(list_item->key);
			g_free(list_item);
			list_of_verses = g_list_next(list_of_verses);
		}
		g_list_free(list_of_verses);
	}
	list_of_verses = NULL;

	strcpy(settings.sb_search_mod, module_name);

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (sidebar.results_list));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);

	strcpy(sidebar.mod_name, module_name);

	if (verse_list[0] == 'S' && verse_list[1] == 'e'
	    && verse_list[2] == 'e') {
		verse_list += 4;
	}
	i = 0;
	tmp = backend->parse_verse_list(verse_list, key);
	while (tmp != NULL) {
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, 0,
				   (const char *) tmp->data, -1);
		++i;
		tmp = g_list_next(tmp);
	}

	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(sidebar.results_list));
	if (!gtk_tree_model_get_iter_first(model, &iter))
		return;
	gtk_widget_set_sensitive(sidebar.menu_item_save_search,TRUE);
	path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_selection_select_path(selection, path);

	gtk_tree_path_free(path);

	gui_verselist_button_release_event(NULL, NULL, NULL);
	gtk_notebook_set_current_page(GTK_NOTEBOOK
				      (widgets.notebook_sidebar), 3);
	gtk_widget_grab_focus(GTK_WIDGET(sidebar.results_list));
}


/******************************************************************************
 * Name
 *  main_add_children_to_tree
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_add_children_to_tree(GtkTreeModel * model, GtkTreeIter iter,
 *				 gchar *mod_name, unsigned long offset)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void add_children_to_tree(GtkTreeModel * model, GtkTreeIter iter,
				 gchar * mod_name, unsigned long offset)
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

	GS_message(("offset: %d",backend->get_treekey_offset()));
	GS_message((backend->display_mod->Name()));
	if (backend->treekey_first_child(offset)) {
		GS_message(("treekey_first_child1 %s",mod_name));
		
		offset = backend->get_treekey_offset();
		
		GS_message(("offset: %d",offset));
		
		sprintf(buf, "%lu", offset);
		tmpbuf = backend->treekey_get_local_name(offset);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		if (backend->treekey_has_children(offset)) {
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CAPTION,
					   (gchar *) tmpbuf, COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) buf,
					   -1);
		} else {
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION,
					   (gchar *) tmpbuf, COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) buf,
					   -1);
		}
		free(tmpbuf);
	}

	while (backend->treekey_next_sibling(offset)) {
		offset = backend->get_treekey_offset();
		sprintf(buf, "%lu", offset);
		tmpbuf = backend->treekey_get_local_name(offset);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		if (backend->treekey_has_children(offset)) {
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CAPTION,
					   (gchar *) tmpbuf, COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) buf,
					   -1);
		} else {
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION,
					   (gchar *) tmpbuf, COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) buf,
					   -1);
		}
		free(tmpbuf);
	}
}


/******************************************************************************
 * Name
 *  main_create_pixbufs
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_create_pixbufs(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

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
				   GTK_ICON_SIZE_MENU, NULL);
	pixbufs->pixbuf_helpdoc =
	    gtk_widget_render_icon(widgets.app,
				   GTK_STOCK_DND,
				   GTK_ICON_SIZE_MENU, NULL);
}


/******************************************************************************
 * Name
 *  main_add_verses_to_chapter
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_add_verses_to_chapter(GtkTreeModel * model, GtkTreeIter iter, 
 *				const gchar * key)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void add_verses_to_chapter(GtkTreeModel * model,
				  GtkTreeIter iter, const gchar * key)
{
	gchar **work_buf = NULL;
	gchar *book = NULL;
	gint chapter;
	gint verses;
	gint i;
	GtkTreeIter child_iter;

	work_buf = g_strsplit(key, "/", 4);
	if (!work_buf[2] || !work_buf[3]) {
		g_strfreev(work_buf);
		return;
	}
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened, -1);
	book = backend->key_get_book(work_buf[3]);
	chapter = backend->key_get_chapter(work_buf[3]);
	verses = backend->key_verse_count(work_buf[3]);

	for (i = 1; i < (verses + 1); i++) {
		gchar *buf = g_strdup_printf("%s %d", _("verse"), i);
		gchar *key = g_strdup_printf("sword://%s/%s %d:%d",
					     work_buf[2],
					     book,
					     chapter,
					     i);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF,
				   pixbufs->pixbuf_helpdoc,
				   COL_CLOSED_PIXBUF, NULL, COL_CAPTION,
				   (gchar *) buf, COL_MODULE,
				   (gchar *) work_buf[2], COL_OFFSET,
				   (gchar *) key, -1);
		g_free(key);
		g_free(buf);
	}
	g_strfreev(work_buf);
}



/******************************************************************************
 * Name
 *  main_add_chapters_to_book
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_add_chapters_to_book(GtkTreeModel * model, GtkTreeIter iter, 
				const gchar * key)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void add_chapters_to_book(GtkTreeModel * model, GtkTreeIter iter,
				 const gchar * key)
{
	gchar **work_buf = NULL;
	gchar *book = NULL;
	gint chapters;
	gint i;
	GtkTreeIter child_iter;

	work_buf = g_strsplit(key, "/", 4);
	if (!work_buf[2] || !work_buf[3]) {
		g_strfreev(work_buf);
		return;
	}
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened, -1);
	book = backend->key_get_book(work_buf[3]);
	chapters = backend->key_chapter_count(work_buf[3]);

	for (i = 1; i < (chapters + 1); i++) {
		gchar *buf = g_strdup_printf("%s %d", _("chapter"), i);
		gchar *key = g_strdup_printf("chapter://%s/%s %d:1",
					     work_buf[2],
					     book,
					     i);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF,
				   pixbufs->pixbuf_closed,
				   COL_CLOSED_PIXBUF,
				   pixbufs->pixbuf_closed, COL_CAPTION,
				   (gchar *) buf, COL_MODULE,
				   (gchar *) work_buf[2], COL_OFFSET,
				   (gchar *) key, -1);
		g_free(key);
		g_free(buf);
	}
	g_strfreev(work_buf);
}


/******************************************************************************
 * Name
 *  main_add_books_to_bible
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_add_books_to_bible(GtkTreeModel * model, GtkTreeIter iter,
				const gchar * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void add_books_to_bible(GtkTreeModel * model, GtkTreeIter iter,
			       const gchar * mod_name)
{
	GList *tmp = NULL;
	VerseKey key;
	gint j = 0;
	GtkTreeIter child_iter;
/*	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;    */
	gchar *buf = NULL;

	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened, -1);
	if (backend->module_has_testament(mod_name, 1)) {
		while (j < key.BMAX[0]) {
/*			buf =
//			    g_convert((const char *) key.books[0][j].
				      name, -1, UTF_8, OLD_CODESET,
				      &bytes_read, &bytes_written,
				      &error);
			if (buf == NULL) {
				GS_print(("error: %s\n", error->message));
				g_error_free(error);
				continue;
			}    */
			buf = strdup((gchar *) key.books[0][j].name);
			gchar *key = g_strdup_printf("book://%s/%s 1:1",
						     mod_name, buf);
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter);
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CAPTION, (gchar *) buf,
					   COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) key,
					   -1);
			g_free(key);
			g_free(buf);
			++j;
		}
	}
	j = 0;
	if (backend->module_has_testament(mod_name, 2)) {
		while (j < key.BMAX[1]) {
/*			buf =
//			    g_convert((const char *) key.books[1][j].
				      name, -1, UTF_8, OLD_CODESET,
				      &bytes_read, &bytes_written,
				      &error);
			if (buf == NULL) {
				GS_print(("error: %s\n", error->message));
				g_error_free(error);
				continue;
			}    */
			buf = strdup((gchar *) key.books[1][j].name);
			gchar *key = g_strdup_printf("book://%s/%s 1:1",
						     mod_name, buf);
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter);
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CAPTION, (gchar *) buf,
					   COL_MODULE,
					   (gchar *) mod_name,
					   COL_OFFSET, (gchar *) key,
					   -1);
			g_free(key);
			g_free(buf);
			++j;
		}
	}
}


/******************************************************************************
 * Name
 *   main_mod_treeview_button_one
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_mod_treeview_button_one(GtkTreeModel *model, GtkTreeIter selected)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_mod_treeview_button_one(GtkTreeModel * model,
				  GtkTreeIter selected)
{
	gint sbtype;
	gchar *cap = NULL;
	gchar *mod = NULL;
	gchar *key = NULL;
	gchar *offset = NULL;
	gint mod_type;
	GtkTreePath *path;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 2, &cap, 3,
			   &mod, 4, &key, -1);
	if (!cap)
		return;
	if (!g_utf8_collate(cap, _("Parallel View")))
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.
					       notebook_bible_parallel),
					      1);

	if (!g_utf8_collate(cap, _("Standard View")))
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.
					       notebook_bible_parallel),
					      0);

	if (!g_utf8_collate(cap, _("Commentaries"))) {
		if (!settings.comm_showing) {
			settings.comm_showing = TRUE;
			gtk_notebook_set_current_page(GTK_NOTEBOOK
						      (widgets.
						       notebook_comm_book),
						      0);
			/*main_display_commentary(settings.
			   CommWindowModule,
			   settings.currentverse); */
		}
	}

	if (!g_utf8_collate(cap, _("General Books"))) {
		if (settings.comm_showing) {
			settings.comm_showing = FALSE;
			gtk_notebook_set_current_page(GTK_NOTEBOOK
						      (widgets.
						       notebook_comm_book),
						      1);
			/*offset =
			   g_strdup_printf("%lu",
			   settings.book_offset);
			   main_display_book(settings.book_mod, offset);
			   g_free(offset); */
		}
	}

	if (!mod)
		return;

	sbtype = backend->module_type(mod);
	switch (sbtype) {
	case TEXT_TYPE:
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.
					       notebook_bible_parallel),
					      0);
		if(strcmp(settings.MainWindowModule, mod)) {
			xml_set_value("GnomeSword", "modules", "bible",
						mod);
			settings.MainWindowModule = xml_get_value(
					"modules", "bible");
		}	
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected)
		    && !key)
			add_books_to_bible(model, selected, mod);
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected)
		    && strstr(key, "book:"))
			add_chapters_to_book(model, selected, key);
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected)
		    && strstr(key, "chapter:"))
			add_verses_to_chapter(model, selected, key);

		if (key)
			main_url_handler(key, TRUE);
		else {
#ifdef OLD_NAVBAR
			if (navbar_main.module_name)
				g_free(navbar_main.module_name);
			navbar_main.module_name = g_strdup(mod);
			main_navbar_fill_book_combo(navbar_main);
#else
#endif
			main_display_bible(mod, settings.currentverse);
		}

		break;
	case COMMENTARY_TYPE:
	case PERCOM_TYPE:
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.
					       notebook_comm_book), 0);
		settings.comm_showing = TRUE;
		main_display_commentary(mod, settings.currentverse);
		break;
	case DICTIONARY_TYPE:
		main_display_dictionary(mod, settings.dictkey);
		break;
	case BOOK_TYPE:
#ifdef PRAYERLIST
	case PRAYERLIST_TYPE:
#endif
		GS_message(("key %s",key));
		settings.comm_showing = FALSE;
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.
					       notebook_comm_book), 1);
		backend->set_module(mod);
		backend->set_treekey(key ? atoi(key) : 0);
		path = gtk_tree_model_get_path(model, &selected);
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected)
		    && !key) {
			add_children_to_tree(model,
					     selected,
					     mod,
					     0);
		}
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected)
		    && backend->treekey_has_children(atoi(key))) {
			add_children_to_tree(model,
					     selected, mod, atol(key));
		}
		gtk_tree_view_expand_row(GTK_TREE_VIEW
					 (sidebar.module_list), path,
					 FALSE);
		gtk_tree_path_free(path);
		main_display_book(mod, (key ? key : (gchar *) "0"));
		main_setup_navbar_book(mod, (key ? atoi(key) : 0));
		break;
	}
	if (cap)
		g_free(cap);
	if (mod)
		g_free(mod);
	if (key)
		g_free(key);


}


/******************************************************************************
 * Name
 *   add_language_folder
 *
 * Synopsis
 *   #include "main/sidebar.h"
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
	const gchar *buf;

	/* Check language */
	buf = language;
	if (!g_utf8_validate(buf,-1,NULL))
		language = _("Unknown");
	if (!g_unichar_isalnum(g_utf8_get_char(buf)) || (language == NULL))
		language = _("Unknown");

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
			   COL_MODULE, NULL, COL_OFFSET, NULL, -1);
}


/******************************************************************************
 * Name
 *   add_module_to_prayerlist_folder
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void add_module_to_prayerlist_folder(GtkTreeModel * model,
 *		      GtkTreeIter iter, gchar * module_name)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
#ifdef PRAYERLIST
static void add_module_to_prayerlist_folder(GtkTreeModel * model,
					  GtkTreeIter iter,
					  gchar * module_name)
{
	
	GtkTreeIter child_iter;
	
	gtk_tree_store_append(GTK_TREE_STORE(model),
			      &child_iter, &iter);
	gtk_tree_store_set(GTK_TREE_STORE(model),
			   &child_iter, COL_OPEN_PIXBUF,
			   pixbufs->pixbuf_closed,
			   COL_CLOSED_PIXBUF,
			   pixbufs->pixbuf_closed,
			   COL_CAPTION,
			   (gchar *) module_name,
			   COL_MODULE,
			   (gchar *) module_name,
			   COL_OFFSET, NULL, -1);
}
#endif


/******************************************************************************
 * Name
 *   add_module_to_language_folder
 *
 * Synopsis
 *   #include "main/sidebar.h"
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
	const gchar *buf;

	/* Check language */
	buf = language;
	if (!g_utf8_validate(buf,-1,NULL))
		language = _("Unknown");
	if (!g_unichar_isalnum(g_utf8_get_char(buf)) || (language == NULL))
		language = _("Unknown");

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;

		gtk_tree_model_get(model, &iter_iter, 2, &str_data, -1);
		if (!strcmp(language, str_data)) {
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter_iter);
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CAPTION,
					   (gchar *) module_name,
					   COL_MODULE,
					   (gchar *) module_name,
					   COL_OFFSET, NULL, -1);
			g_free(str_data);
			return;
		}
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
}


/******************************************************************************
 * Name
 *   main_load_module_tree
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_load_module_tree(GtkWidget * tree)
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

	store = gtk_tree_store_new(N_COLUMNS,
				   GDK_TYPE_PIXBUF,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_store_clear(store);

	/*  Biblical Texts folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Biblical Texts"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Biblical Texts"), -1);

	gtk_tree_store_append(store, &child_iter, &iter);
	gtk_tree_store_set(store, &child_iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
			   COL_CLOSED_PIXBUF, NULL,
			   COL_CAPTION, _("Parallel View"),
			   COL_MODULE, _("Parallel View"),
			   COL_OFFSET, _("Parallel View"), -1);

	gtk_tree_store_append(store, &child_iter, &iter);
	gtk_tree_store_set(store, &child_iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
			   COL_CLOSED_PIXBUF, NULL,
			   COL_CAPTION, _("Standard View"),
			   COL_MODULE, _("Standard View"),
			   COL_OFFSET, _("Standard View"), -1);

	tmp = get_list(TEXT_LIST);
	while (tmp != NULL) {
		const gchar *buf =
		    backend->module_get_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  Commentaries folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Commentaries"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Commentaries"), -1);

	tmp = get_list(COMM_LIST);
	while (tmp != NULL) {
		const gchar *buf =
		    backend->module_get_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  Dictionaries folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Dictionaries"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Dictionaries"), -1);

	tmp = get_list(DICT_LIST);
	while (tmp != NULL) {
		const gchar *buf =
		    backend->module_get_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

	/*  Devotionals folders */
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Daily Devotionals"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Daily Devotionals"), -1);

	tmp = get_list(DEVOTION_LIST);
	while (tmp != NULL) {
		const gchar *buf =
		    backend->module_get_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
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
			   COL_OFFSET, _("General Books"), -1);

	/*  General Books folders */
	tmp = get_list(GBS_LIST);
	while (tmp != NULL) {
		const gchar *buf =
		    backend->module_get_language((gchar *) tmp->data);
		add_language_folder(GTK_TREE_MODEL(store), iter, buf);
		add_module_to_language_folder(GTK_TREE_MODEL(store),
					      iter, buf,
					      (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}

#ifdef PRAYERLIST
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Prayer List"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Prayer List"), -1);
	tmp = get_list(PRAYER_LIST);
	while (tmp != NULL) {
		add_module_to_prayerlist_folder(GTK_TREE_MODEL(store),
					      iter, (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}
#endif	
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree),
				GTK_TREE_MODEL(store));
}


/******************************************************************************
 * Name
 *  main_add_mod_tree_columns
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void main_add_mod_tree_columns(GtkTreeView * tree)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

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
