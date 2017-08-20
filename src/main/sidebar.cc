/*
 * Xiphos Bible Study Tool
 * sidebar.cc - sidebar interface to sword
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gtk/gtk.h>
#include <swmgr.h>
#include <swmodule.h>

#include "gui/xiphos.h"
#include "gui/main_menu.h"
#include "gui/widgets.h"
#include "gui/sidebar.h"
#include "gui/tabbed_browser.h"
#include "gui/utilities.h"
#include "gui/dialog.h"

#include "main/sidebar.h"
#include "main/configs.h"
#include "main/lists.h"
#include "main/mod_mgr.h"
#include "main/navbar.h"
#include "main/navbar_book.h"
#include "main/sword_treekey.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

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

void main_open_bookmark_in_new_tab(gchar *mod_name, gchar *key)
{
	gint module_type = backend->module_type(mod_name);

	switch (module_type) {
	case -1:
		return;
		break;
	case TEXT_TYPE:
	case COMMENTARY_TYPE:
		if (strcmp(settings.currentverse, key)) {
			xml_set_value("Xiphos", "keys", "verse",
				      key);
			settings.currentverse =
			    xml_get_value("keys", "verse");
		}
		break;
	case DICTIONARY_TYPE:
		xml_set_value("Xiphos", "keys", "dictionary", key);
		settings.dictkey = xml_get_value("keys", "dictionary");
		break;
	case BOOK_TYPE:
		xml_set_value("Xiphos", "keys", "offset", key);
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

void main_display_verse_list_in_sidebar(gchar *key,
					gchar *module_name,
					gchar *verse_list)
{
	GList *tmp = NULL;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreePath *path;
	GtkTreeIter iter;
	RESULTS *list_item;

	/* improper xref encoding, not BCV: gets null list from Sword. */
	if (verse_list == NULL) {
		gui_generic_warning(_("Module error: Unusable xref encoding.\n"
				      "Please report this, with module & verse"));
		return;
	}

	is_search_result = FALSE;

	XI_warning(("verse_list = %s", verse_list));
	list_of_verses = g_list_first(list_of_verses);
	if (list_of_verses) {
		GList *chaser = list_of_verses;
		while (chaser) {
			list_item = (RESULTS *)chaser->data;
			g_free(list_item->module);
			g_free(list_item->key);
			g_free(list_item);
			chaser = g_list_next(chaser);
		}
		g_list_free(list_of_verses);
		list_of_verses = NULL;
	}

	strcpy(settings.sb_search_mod, module_name);

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.results_list));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);

	strcpy(sidebar.mod_name, module_name);

	if (!strncmp(verse_list, "See ", 4))
		verse_list += 4;

	if ((*verse_list != '/') &&
	    ((tmp = backend->parse_verse_list(module_name, verse_list, key)) != NULL)) {
		// normal verse list.
		while (tmp != NULL) {
			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, 0,
					   (const char *)tmp->data, -1);

			list_item = g_new(RESULTS, 1);
			list_item->module = g_strdup(module_name);
			list_item->key = g_strdup((const char *)tmp->data);
			list_of_verses = g_list_append(list_of_verses,
						       (RESULTS *)list_item);

			tmp = g_list_next(tmp);
		}
	} else {
		// not normal verse list.  probably a genbook or dict key.
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, 0, verse_list, -1);

		list_item = g_new(RESULTS, 1);
		list_item->module = g_strdup(module_name);
		list_item->key = g_strdup((const char *)verse_list);
		list_of_verses = g_list_append(list_of_verses,
					       (RESULTS *)list_item);
	}

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(sidebar.results_list));
	if (!gtk_tree_model_get_iter_first(model, &iter))
		return;
	gtk_widget_set_sensitive(sidebar.menu_item_save_search, TRUE);
	path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_selection_select_path(selection, path);

	gtk_tree_path_free(path);

	gui_verselist_button_release_event(NULL, NULL, NULL);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar), 3);
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

static void add_children_to_tree(GtkTreeModel *model, GtkTreeIter iter,
				 gchar *mod_name, unsigned long offset)
{
	gchar buf[256];
	gchar *tmpbuf = NULL;
	GtkTreeIter child_iter;

	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   -1);

	XI_message(("offset: %ld", backend->get_treekey_offset()));
	XI_message(("%s", backend->display_mod->getName()));
	if (backend->treekey_first_child(offset)) {
		XI_message(("treekey_first_child1 %s", mod_name));

		offset = backend->get_treekey_offset();

		XI_message(("offset: %ld", offset));

		sprintf(buf, "%lu", offset);
		tmpbuf = backend->treekey_get_local_name(offset);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		if (backend->treekey_has_children(offset)) {
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
					   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CAPTION, (gchar *)tmpbuf,
					   COL_MODULE, (gchar *)mod_name,
					   COL_OFFSET, (gchar *)buf,
					   -1);
		} else {
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
					   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION, (gchar *)tmpbuf,
					   COL_MODULE, (gchar *)mod_name,
					   COL_OFFSET, (gchar *)buf,
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
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
					   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CAPTION, (gchar *)tmpbuf,
					   COL_MODULE, (gchar *)mod_name,
					   COL_OFFSET, (gchar *)buf,
					   -1);
		} else {
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
					   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION, (gchar *)tmpbuf,
					   COL_MODULE, (gchar *)mod_name,
					   COL_OFFSET, (gchar *)buf,
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
	GtkTextDirection dir = gtk_widget_get_direction(GTK_WIDGET(widgets.app));

	pixbufs = g_new0(TreePixbufs, 1);

	if (dir == GTK_TEXT_DIR_LTR) {
		pixbufs->pixbuf_closed =
		    pixbuf_finder("book_closed.png", 16, NULL);

		pixbufs->pixbuf_opened =
		    pixbuf_finder("book_open.png", 16, NULL);
	} else {
		pixbufs->pixbuf_closed =
		    pixbuf_finder("book_closed_rtol.png", 16, NULL);

		pixbufs->pixbuf_opened =
		    pixbuf_finder("book_open_rtol.png", 16, NULL);
	}

#if GTK_CHECK_VERSION(3, 0, 0)
#if GTK_CHECK_VERSION(3, 10, 0)
	GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
	pixbufs->pixbuf_helpdoc =
	    gtk_icon_theme_load_icon(icon_theme,
				     "gtk-dnd",
				     GTK_ICON_SIZE_MENU,
				     GTK_ICON_LOOKUP_FORCE_SIZE,
				     NULL);
#else
	pixbufs->pixbuf_helpdoc =
	    gtk_widget_render_icon_pixbuf(widgets.app,
					  GTK_STOCK_DND,
					  GTK_ICON_SIZE_MENU);
#endif
#else
	gtk_widget_render_icon(widgets.app,
			       GTK_STOCK_DND,
			       GTK_ICON_SIZE_MENU, NULL);
#endif
}

#ifdef ALLOW_BIBLE_NAVIGATION_FROM_SIDEBAR_TREE

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
static void add_verses_to_chapter(GtkTreeModel *model,
				  GtkTreeIter iter, const gchar *key)
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
	book = backend->key_get_book(settings.MainWindowModule, work_buf[3]);
	chapter = backend->key_get_chapter(settings.MainWindowModule, work_buf[3]);
	verses = backend->key_verse_count(settings.MainWindowModule, work_buf[3]);

	for (i = 1; i < (verses + 1); i++) {
		gchar *num = main_format_number(i);
		gchar *buf = g_strdup_printf("%s %s", _("verse"), num);
		g_free(num);
		gchar *key = g_strdup_printf("sword://%s/%s %d:%d",
					     work_buf[2],
					     book,
					     chapter,
					     i);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
				   COL_CLOSED_PIXBUF, NULL,
				   COL_CAPTION, (gchar *)buf,
				   COL_MODULE, (gchar *)work_buf[2],
				   COL_OFFSET, (gchar *)key,
				   -1);
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

static void add_chapters_to_book(GtkTreeModel *model, GtkTreeIter iter,
				 const gchar *key)
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
	book = backend->key_get_book(settings.MainWindowModule, work_buf[3]);
	chapters = backend->key_chapter_count(settings.MainWindowModule, work_buf[3]);

	for (i = 1; i < (chapters + 1); i++) {
		gchar *num = main_format_number(i);
		gchar *buf = g_strdup_printf("%s %s", _("chapter"), num);
		g_free(num);
		gchar *key = g_strdup_printf("chapter://%s/%s %d:1",
					     work_buf[2],
					     book,
					     i);
		gtk_tree_store_append(GTK_TREE_STORE(model),
				      &child_iter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CAPTION, (gchar *)buf,
				   COL_MODULE, (gchar *)work_buf[2],
				   COL_OFFSET, (gchar *)key,
				   -1);
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
 *				const gchar * mod_name)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void add_books_to_bible(GtkTreeModel *model, GtkTreeIter iter,
			       const gchar *mod_name)
{
	SWModule *mod = backend->get_SWModule(mod_name);
	if (!mod)
		return;

	VerseKey *vkey = (VerseKey *)mod->createKey();
	gint j = 0;
	GtkTreeIter child_iter;
	gchar *buf = NULL;

	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened, -1);
	if (backend->module_has_testament(mod_name, 1)) {
		while (j < vkey->BMAX[0]) {
			vkey->setTestament(1);
			vkey->setBook(j + 1);
			buf = strdup((gchar *)vkey->getBookName());
			gchar *key = g_strdup_printf("book://%s/%s 1:1",
						     mod_name, buf);
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter);
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
					   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CAPTION, (gchar *)buf,
					   COL_MODULE, (gchar *)mod_name,
					   COL_OFFSET, (gchar *)key,
					   -1);
			g_free(key);
			g_free(buf);
			++j;
		}
	}
	j = 0;
	if (backend->module_has_testament(mod_name, 2)) {
		while (j < vkey->BMAX[1]) {
			vkey->setTestament(2);
			vkey->setBook(j + 1);
			buf = strdup((gchar *)vkey->getBookName());
			gchar *key = g_strdup_printf("book://%s/%s 1:1",
						     mod_name, buf);
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter);
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
					   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CAPTION, (gchar *)buf,
					   COL_MODULE, (gchar *)mod_name,
					   COL_OFFSET, (gchar *)key,
					   -1);
			g_free(key);
			g_free(buf);
			++j;
		}
	}
	delete vkey;
}
#endif /* ALLOW_BIBLE_NAVIGATION_FROM_SIDEBAR_TREE */

#ifdef USE_TREEVIEW_PATH
gboolean main_expand_treeview_to_path(GtkTreeModel *model, GtkTreeIter iter)
{
	//gchar *cap = NULL;
	gchar *mod = NULL;
	gchar *key = NULL;
	//GtkTreePath *path;
	//gchar *path_str = NULL;

	//static int old_page = 0;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 3, &mod, 4, &key, -1);

	//backend->set_module(mod);
	//backend->set_treekey(key ? atoi(key) : 0);
	//path = gtk_tree_model_get_path(model, &iter);
	if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &iter) && !key) {
		add_children_to_tree(model,
				     iter,
				     mod,
				     0);
	}
	if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &iter) && backend->treekey_has_children(key ? atoi(key) : 0)) {
		add_children_to_tree(model,
				     iter, mod, atol(key));
	}

	/*gtk_tree_view_expand_row(GTK_TREE_VIEW
				 (sidebar.module_list), path,
				 FALSE);*/
	//gtk_tree_path_free(path);
	main_display_book(mod, (key ? key : (gchar *)"0"));
	main_setup_navbar_book(mod, (key ? atoi(key) : 0));
	return 1;
}
#endif /* USE_TREEVIEW_PATH */

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

void main_mod_treeview_button_one(GtkTreeModel *model,
				  GtkTreeIter selected)
{
	gint sbtype;
	gchar *cap = NULL;
	gchar *mod = NULL;
	gchar *key = NULL;
	GtkTreePath *path;

	static int old_page = 0;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 2, &cap, 3,
			   &mod, 4, &key, -1);
	if (!cap)
		return;

	if (!g_utf8_collate(cap, _("Parallel View"))) {
		if (settings.dockedInt) {
			gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
						      1);
		}
		if (settings.showparatab) {
			old_page = gtk_notebook_get_current_page(
			    GTK_NOTEBOOK(widgets.notebook_main));

			gtk_notebook_set_current_page(
			    GTK_NOTEBOOK(widgets.notebook_main),
			    gtk_notebook_page_num(
				GTK_NOTEBOOK(widgets.notebook_main),
				widgets.parallel_tab));
		}
	}

	if (!g_utf8_collate(cap, _("Standard View"))) {
		gtk_notebook_set_current_page(
		    GTK_NOTEBOOK(widgets.notebook_main),
		    old_page);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
					      0);
	}
	/* let's not do anything else if the parallel tab is showing */
	if (settings.paratab_showing)
		return;

	if (!g_utf8_collate(cap, _("Commentaries"))) {
		if (!settings.comm_showing) {
			settings.comm_showing = TRUE;
			gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book),
						      0);
		}
	}

	if (!g_utf8_collate(cap, _("General Books"))) {
		if (settings.comm_showing) {
			settings.comm_showing = FALSE;
			gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book),
						      1);
		}
	}

	if (!mod)
		return;

	sbtype = backend->module_type(mod);
	switch (sbtype) {
	case TEXT_TYPE:
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
					      0);
// MainWindowModule is set in main_bible_display(), not here.

#ifdef ALLOW_BIBLE_NAVIGATION_FROM_SIDEBAR_TREE
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected) && !key)
			add_books_to_bible(model, selected, mod);
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected) && strstr(key, "book:"))
			add_chapters_to_book(model, selected, key);
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected) && strstr(key, "chapter:"))
			add_verses_to_chapter(model, selected, key);
#endif

		if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widgets.viewtexts_item))) {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewtexts_item), 1);
			on_show_bible_text_activate(GTK_CHECK_MENU_ITEM(widgets.viewtexts_item), NULL);
		}

		if (key)
			main_url_handler(key, TRUE);
		else
			main_display_bible(mod, settings.currentverse);
		break;

	case COMMENTARY_TYPE:
	case PERCOM_TYPE:
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book), 0);
		settings.comm_showing = TRUE;

		if ((!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widgets.viewcomms_item))) ||
		    (!settings.comm_showing)) {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewcomms_item), 1);
			on_show_commentary_activate(GTK_CHECK_MENU_ITEM(widgets.viewcomms_item), NULL);
		}
		main_display_commentary(mod, settings.currentverse);
		break;

	case DICTIONARY_TYPE:
		if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widgets.viewdicts_item))) {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewdicts_item), 1);
			on_show_dictionary_lexicon_activate(GTK_CHECK_MENU_ITEM(widgets.viewcomms_item), NULL);
		}
		main_display_dictionary(mod, settings.dictkey);
		break;

	case BOOK_TYPE:
	case PRAYERLIST_TYPE:
		XI_message(("key %s", (key ? key : "-null-")));
		settings.comm_showing = FALSE;
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book), 1);
		backend->set_module(mod);
		backend->set_treekey(key ? atoi(key) : 0);
		path = gtk_tree_model_get_path(model, &selected);
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected) && !key) {
			add_children_to_tree(model, selected, mod, 0);
		}
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected) && backend->treekey_has_children(key ? atoi(key) : 0)) {
			add_children_to_tree(model, selected, mod, atol(key));
		}

#ifdef USE_TREEVIEW_PATH
		gchar *path_str = gtk_tree_path_to_string(path);
		XI_message(("path: %s", path_str));
		gui_save_treeview_path_string(path_str, mod);
		g_free(path_str);
#endif /* USE_TREEVIEW_PATH */

		gtk_tree_view_expand_row(GTK_TREE_VIEW(sidebar.module_list), path,
					 FALSE);
		gtk_tree_path_free(path);

		if ((!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widgets.viewcomms_item))) ||
		    (!settings.comm_showing)) {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewcomms_item), 1);
			on_show_commentary_activate(GTK_CHECK_MENU_ITEM(widgets.viewcomms_item), NULL);
		}

		main_display_book(mod, (key ? key : (gchar *)"0"));
		main_setup_navbar_book(mod, (key ? atoi(key) : 0));
		break;
	}
	g_free(cap);
	g_free(mod);
	g_free(key);
}

/******************************************************************************
 * Name
 *   language_add_folders
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void language_add_folders(GtkTreeModel * model,
 *			       GtkTreeIter iter, gchar * module_name)
 *
 * Description
 *   fast creation of the tree model's language folders
 *
 * Return value
 *   void
 */
static void
language_add_folders(GtkTreeModel *model,
		     GtkTreeIter iter,
		     gchar **languages)
{
	GtkTreeIter iter_iter;
	GtkTreeIter child_iter;
	int j;

	(void)gtk_tree_model_iter_children(model, &iter_iter, &iter);
	for (j = 0; languages[j]; ++j) {
		gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
				   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CAPTION, ((g_utf8_validate(languages[j], -1, NULL))
						     ? languages[j]
						     : _("Unknown")),
				   COL_MODULE, NULL,
				   COL_OFFSET, NULL,
				   -1);
	}
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
static void add_module_to_prayerlist_folder(GtkTreeModel *model,
					    GtkTreeIter iter,
					    gchar *module_name)
{

	GtkTreeIter child_iter;

	gtk_tree_store_append(GTK_TREE_STORE(model),
			      &child_iter, &iter);
	gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, (gchar *)module_name,
			   COL_MODULE, (gchar *)module_name,
			   COL_OFFSET, NULL,
			   -1);
}

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

static void add_module_to_language_folder(GtkTreeModel *model,
					  GtkTreeIter iter,
					  const gchar *language,
					  gchar *module_name,
					  const gchar *description)
{
	GtkTreeIter iter_iter;
	GtkTreeIter child_iter;
	gboolean valid;

	/* Check language */
	const gchar *buf = language;
	if (!g_utf8_validate(buf, -1, NULL))
		language = _("Unknown");
	if (!g_unichar_isalnum(g_utf8_get_char(buf)) || (language == NULL))
		language = _("Unknown");

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;
		const gchar *abbreviation = main_get_abbreviation(module_name);

		gtk_tree_model_get(model, &iter_iter, COL_CAPTION, &str_data, -1);
		if (!strcmp(language, str_data)) {
			gchar *content;
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter_iter);

			content = g_strdup_printf("%s: %s",
						  (abbreviation
						       ? abbreviation
						       : module_name),
						  description);
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
					   COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
					   COL_CAPTION, (gchar *)content,
					   COL_MODULE, (gchar *)module_name,
					   COL_OFFSET, NULL, -1);
			g_free(content);
			g_free(str_data);
			return;
		}
		g_free(str_data);
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

void main_load_module_tree(GtkWidget *tree)
{
	GtkTreeStore *store;
	GtkTreeIter text;
	GtkTreeIter commentary;
	GtkTreeIter dictionary;
	GtkTreeIter glossary;
	GtkTreeIter devotional;
	GtkTreeIter book;
	GtkTreeIter map;
	GtkTreeIter image;
	GtkTreeIter cult;
	GtkTreeIter prayerlist;
	GtkTreeIter child_iter;
	GList *tmp = NULL;
	GList *tmp2 = NULL;

	store = gtk_tree_store_new(N_COLUMNS,
				   GDK_TYPE_PIXBUF,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_store_clear(store);

	/*  Biblical Texts folders */
	gtk_tree_store_append(store, &text, NULL);
	gtk_tree_store_set(store, &text,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Biblical Texts"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Biblical Texts"), -1);

	gtk_tree_store_append(store, &child_iter, &text);
	gtk_tree_store_set(store, &child_iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
			   COL_CLOSED_PIXBUF, NULL,
			   COL_CAPTION, _("Parallel View"),
			   COL_MODULE, _("Parallel View"),
			   COL_OFFSET, _("Parallel View"), -1);

	gtk_tree_store_append(store, &child_iter, &text);
	gtk_tree_store_set(store, &child_iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
			   COL_CLOSED_PIXBUF, NULL,
			   COL_CAPTION, _("Standard View"),
			   COL_MODULE, _("Standard View"),
			   COL_OFFSET, _("Standard View"), -1);

	/*  Commentaries folders */
	gtk_tree_store_append(store, &commentary, NULL);
	gtk_tree_store_set(store, &commentary,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Commentaries"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Commentaries"), -1);

	/*  Dictionaries folders */
	gtk_tree_store_append(store, &dictionary, NULL);
	gtk_tree_store_set(store, &dictionary,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Dictionaries"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Dictionaries"), -1);

	/*  Glossaries folders */
	gtk_tree_store_append(store, &glossary, NULL);
	gtk_tree_store_set(store, &glossary,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Glossaries"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Glossaries"), -1);

	/*  Devotionals folders */
	gtk_tree_store_append(store, &devotional, NULL);
	gtk_tree_store_set(store, &devotional,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Daily Devotionals"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Daily Devotionals"), -1);

	/*  General Books folders */
	gtk_tree_store_append(store, &book, NULL);
	gtk_tree_store_set(store, &book,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("General Books"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("General Books"), -1);

	/*  Maps folders */
	gtk_tree_store_append(store, &map, NULL);
	gtk_tree_store_set(store, &map,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Maps"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Maps"), -1);

	/*  Images folders */
	gtk_tree_store_append(store, &image, NULL);
	gtk_tree_store_set(store, &image,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Images"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Images"), -1);

	/*  Cult/Unorthodox/Questionable folders */
	gtk_tree_store_append(store, &cult, NULL);
	gtk_tree_store_set(store, &cult,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Cult/Unorthodox"),
			   COL_MODULE, NULL,
			   COL_OFFSET, _("Cult/Unorthodox"), -1);

	/*  Prayer lists folder */
	if (settings.prayerlist) {
		gtk_tree_store_append(store, &prayerlist, NULL);
		gtk_tree_store_set(store, &prayerlist,
				   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
				   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				   COL_CAPTION, _("Prayer List/Journal"),
				   COL_MODULE, NULL,
				   COL_OFFSET, _("Prayer List/Journal"), -1);
	}

	tmp = mod_mgr_list_local_modules(settings.path_to_mods, TRUE);

	language_make_list(tmp, store,
			   text, commentary, map, image,
			   devotional, dictionary, glossary, book, cult,
			   NULL, NULL,
			   language_add_folders, FALSE);

	// fast-n-loose w/known string values to avoid pointless strcmp costs.
	// TEXT_MODS => 'B' ("Biblical Texts")
	// COMM_MODS => 'C' ("Commentaries")
	// DICT_MODS => 'L' ("Lexicons / Dictionaries")
	// BOOK_MODS => 'G' ("Generic Books")
	// see src/main/sword.h regarding these definitions.
	// see also similar code in src/gnome2/{mod_mgr,utilities}.c.
	// it is just necessary that we undo some of this inefficiency.

	tmp2 = tmp;
	while (tmp2 != NULL) {
		MOD_MGR *info = (MOD_MGR *)tmp2->data;

		if (info->is_cult) {
			add_module_to_language_folder(GTK_TREE_MODEL(store),
						      cult, info->language,
						      info->name, info->description);
		} else if (info->type[0] == 'B') {
			add_module_to_language_folder(GTK_TREE_MODEL(store),
						      text, info->language,
						      info->name, info->description);
		} else if (info->type[0] == 'C') {
			add_module_to_language_folder(GTK_TREE_MODEL(store),
						      commentary, info->language,
						      info->name, info->description);
		} else if (info->is_maps) {
			add_module_to_language_folder(GTK_TREE_MODEL(store),
						      map, info->language,
						      info->name, info->description);
		} else if (info->is_images) {
			add_module_to_language_folder(GTK_TREE_MODEL(store),
						      image, info->language,
						      info->name, info->description);
		} else if (info->is_devotional) {
			add_module_to_language_folder(GTK_TREE_MODEL(store),
						      devotional, info->language,
						      info->name, info->description);
		} else if (info->is_glossary) {
			add_module_to_language_folder(GTK_TREE_MODEL(store),
						      glossary, info->language,
						      info->name, info->description);
		} else if (info->type[0] == 'L') {
			add_module_to_language_folder(GTK_TREE_MODEL(store),
						      dictionary, info->language,
						      info->name, info->description);
		} else if (info->type[0] == 'G') {
			gchar *gstype = main_get_mod_config_entry(info->name, "GSType");
			if ((gstype == NULL) || strcmp(gstype, "PrayerList")) {
				add_module_to_language_folder(GTK_TREE_MODEL(store),
							      book, info->language,
							      info->name, info->description);
			}
		} else {
			XI_warning(("mod `%s' unknown type `%s'",
				    info->name, info->type));
		}

		g_free(info->name);
		g_free(info->type);
		g_free(info->new_version);
		g_free(info->old_version);
		g_free(info->installsize);
		g_free(info);
		tmp2 = g_list_next(tmp2);
	}
	g_list_free(tmp);

	/* prayer list folders */
	if (settings.prayerlist) {
		tmp = get_list(PRAYER_LIST);
		while (tmp != NULL) {
			add_module_to_prayerlist_folder(GTK_TREE_MODEL(store),
							prayerlist,
							(gchar *)tmp->data);
			tmp = g_list_next(tmp);
		}
	}
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

void main_add_mod_tree_columns(GtkTreeView *tree)
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

	column = gtk_tree_view_column_new();

	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, renderer, "pixbuf",
					    COL_OPEN_PIXBUF, "pixbuf-expander-open",
					    COL_OPEN_PIXBUF, "pixbuf-expander-closed",
					    COL_CLOSED_PIXBUF, NULL);

	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer, "text",
					    COL_CAPTION, NULL);
	gtk_tree_view_append_column(tree, column);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer, "text",
					    COL_MODULE, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer, "text",
					    COL_OFFSET, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);
}

/******   end of file   ******/
