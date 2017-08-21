/*
 * Xiphos Bible Study Tool
 * module_dialogs.cc - view module in a dialog
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

#include <gtk/gtk.h>
#include <swmgr.h>
#include <swmodule.h>
#include <url.h>

#include <string.h>

#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/display_info.h"
#include "gui/font_dialog.h"
#include "gui/navbar_versekey.h"
#include "gui/sidebar.h"
#include "gui/main_window.h"
#include "gui/xiphos.h"
#include "gui/utilities.h"
#include "gui/widgets.h"
#include "gui/about_modules.h"

#include "main/navbar_book_dialog.h"
#include "main/module_dialogs.h"
#include "main/navbar_versekey.h"
#include "main/sword.h"
#include "main/sidebar.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"
#include "main/display.hh"
#include "main/global_ops.hh"
#include "main/url.hh"

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\"><!-- A { text-decoration:none } *[dir=rtl] { text-align: right; } --></style></head>"

typedef struct _treeitem TreeItem;
struct _treeitem
{
	gchar *module_name;
	gchar *item_name;
	gchar *offset;
	gboolean is_leaf;
	gboolean expanded;
	GdkPixbuf *pixbuf_opened;
	GdkPixbuf *pixbuf_closed;
};

enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_TITLE,
	COL_BOOK,
	COL_OFFSET,
	N_COLUMNS
};

gboolean bible_freed;
gboolean dialog_freed;
gboolean bible_apply_change;

/******************************************************************************
 * static - global to this file only
 */
static DIALOG_DATA *dlg_bible;
static DIALOG_DATA *dlg_percom;
static GList *list_dialogs;

/******************************************************************************
 * externs
 */
extern gboolean gsI_isrunning; /* information dialog */
extern gboolean do_display;

extern gboolean valid_scripture_key;

/******************************************************************************
 * Name
 *   main_dialogs_book_heading
 *
 * Synopsis
 *   #include "main/module_dialogs.h"
 *
 *   void main_dialogs_book_heading(DIALOG_DATA *d)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_dialogs_book_heading(DIALOG_DATA *d)
{
	BackEnd *be = (BackEnd *)d->backend;
	SWMgr *mgr = be->get_mgr();

	be->display_mod = mgr->Modules[d->mod_name];
	XI_message(("%s", be->display_mod->renderText().c_str()));
	VerseKey *vkey = (VerseKey *)(SWKey *)(*be->display_mod);
	vkey->setIntros(1);
	vkey->setAutoNormalize(0);
	vkey->setVerse(0);
	vkey->setChapter(0);
	be->display_mod->display();
}

/******************************************************************************
 * Name
 *   main_dialogs_chapter_heading
 *
 * Synopsis
 *   #include "main/module_dialogs.h"
 *
 *   void main_dialogs_chapter_heading(DIALOG_DATA *d)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_dialogs_chapter_heading(DIALOG_DATA *d)
{
	BackEnd *be = (BackEnd *)d->backend;
	SWMgr *mgr = be->get_mgr();

	be->display_mod = mgr->Modules[d->mod_name];
	backend->display_mod->setKey(d->key);
	VerseKey *vkey = (VerseKey *)(SWKey *)(*be->display_mod);
	vkey->setIntros(1);
	vkey->setAutoNormalize(0);
	vkey->setVerse(0);
	//vkey->Chapter(0);
	be->display_mod->display();
}

/******************************************************************************
 * Name
 *   main_dialogs_clear_viewer
 *
 * Synopsis
 *   #include "main/module_dialogs.h"
 *
 *   void main_dialogs_clear_viewer(DIALOG_DATA *d)
 *
 * Description
 *   clear the information viewer
 *
 * Return value
 *   void
 */

void main_dialogs_clear_viewer(DIALOG_DATA *d)
{
	GString *str = g_string_new(NULL);
	const char *buf = N_("Previewer");
	MOD_FONT *mf = get_font(settings.MainWindowModule);

	g_string_printf(str,
			HTML_START
			"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			"<b><font color=\"grey\" face=\"%s\" size=\"%+d\">%s</font></b><hr/></body></html>",
			settings.bible_bg_color, settings.bible_text_color,
			settings.link_color,
			((mf->old_font) ? mf->old_font : ""),
			mf->old_font_size_value - 1,
			buf);
	free_font(mf);
	HtmlOutput(str->str, d->previewer, NULL, NULL);
	g_string_free(str, TRUE);
}

/******************************************************************************
 * Name
 *   main_dialogs_information_viewer
 *
 * Synopsis
 *   #include "main/previewer.h"
 *
 *   void main_dialogs_information_viewer(GtkWidget * html_widget, gchar * mod_name,
 *		    gchar * text, gchar *key, gchar * type)
 *
 * Description
 *   display information in the information previewer
 *
 * Return value
 *   void
 */

void main_dialogs_information_viewer(DIALOG_DATA *d, gchar *mod_name,
				     gchar *text, gchar *key,
				     gchar *action, gchar *type,
				     gchar *morph_text, gchar *morph)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	MOD_FONT *mf = get_font(mod_name);

	g_string_printf(tmp_str,
			HTML_START
			"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
			settings.bible_bg_color, settings.bible_text_color,
			settings.link_color);

	str = g_string_new(tmp_str->str);
	if (type) {
		if (!strcmp(type, "n")) {
			g_string_printf(tmp_str,
					"<font color=\"grey\">%s<hr/></font><br/>",
					_("Footnote"));
			str = g_string_append(str, tmp_str->str);
		}
		if (!strcmp(type, "x")) {
			g_string_printf(tmp_str,
					"<font color=\"grey\">%s<hr/></font><br/>",
					_("Cross Reference"));
			str = g_string_append(str, tmp_str->str);
		}
		if (!strcmp(action, "showStrongs")) { //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str,
					"<font color=\"grey\">%s: %s<hr/></font><br/>",
					_("Strongs"), key);
			str = g_string_append(str, tmp_str->str);
		}
		if (!strcmp(action, "showMorph")) { //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str,
					"<font color=\"grey\">%s: %s<hr/></font><br/>",
					_("Morphology"), key);
			str = g_string_append(str, tmp_str->str);
		}
	}

	if (!strcmp(action, "showStrongsMorph")) { //&& !strcmp(type,"Greek")
		g_string_printf(tmp_str,
				"<font color=\"grey\">%s: %s<hr/></font><br/>",
				_("Strongs"), key);
		str = g_string_append(str, tmp_str->str);
		g_string_printf(tmp_str,
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font) ? mf->old_font : "none",
				(mf->old_font_size) ? mf->old_font_size : "+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);

		g_string_printf(tmp_str,
				"<font color=\"grey\"><br/><br/>%s: %s<hr/></font><br/>",
				_("Morphology"), morph);
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, morph_text);
		g_string_printf(tmp_str, " %s<br/>", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);

	} else {
		g_string_printf(tmp_str,
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font) ? mf->old_font : "none",
				(mf->old_font_size) ? mf->old_font_size : "+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);

		g_string_printf(tmp_str, " %s", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);
	}

	HtmlOutput(str->str, sidebar.html_viewer_widget, NULL, NULL);
	free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}

/******************************************************************************
 * Name
 *   main_dialog_information_viewer
 *
 * Synopsis
 *   #include "main/sword.h.h"
 *
 *   void main_dialog_information_viewer(GtkWidget * html_widget, gchar * mod_name,
 *		    gchar * text, gchar *key, gchar * type, gboolean show_key)
 *
 * Description
 *   display information in the information viewer
 *
 * Return value
 *   void
 */

void main_dialog_information_viewer(const gchar *mod_name,
				    const gchar *text,
				    const gchar *key,
				    const gchar *action,
				    const gchar *type,
				    const gchar *morph_text,
				    const gchar *morph,
				    DIALOG_DATA *d)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	MOD_FONT *mf = get_font((gchar *)mod_name);

	if (!d->previewer)
		return;

	g_string_printf(tmp_str,
			HTML_START
			"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
			settings.bible_bg_color, settings.bible_text_color,
			settings.link_color);

	str = g_string_new(tmp_str->str);
	if (type) {
		if (!strcmp(type, "n")) {
			g_string_printf(tmp_str, "<font color=\"grey\">%s<hr/></font><br/>",
					_("Footnote"));
			str = g_string_append(str, tmp_str->str);
		}
		if (!strcmp(type, "x")) {
			g_string_printf(tmp_str, "<font color=\"grey\">%s<hr/></font><br/>",
					_("Cross Reference"));
			str = g_string_append(str, tmp_str->str);
		}
		if (!strcmp(action, "showStrongs")) { //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str, "<font color=\"grey\">%s: %s<hr/></font><br/>",
					_("Strongs"), key);
			str = g_string_append(str, tmp_str->str);
		}
		if (!strcmp(action, "showMorph")) { //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str, "<font color=\"grey\">%s: %s<hr/></font><br/>",
					_("Morphology"), key);
			str = g_string_append(str, tmp_str->str);
		}
	}

	if (!strcmp(action, "showStrongsMorph")) { //&& !strcmp(type,"Greek")
		g_string_printf(tmp_str, "<font color=\"grey\">%s: %s<hr/></font><br/>",
				_("Strongs"), key);
		str = g_string_append(str, tmp_str->str);
		g_string_printf(tmp_str,
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font) ? mf->old_font : "none",
				(mf->old_font_size) ? mf->old_font_size : "+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);

		g_string_printf(tmp_str, "<font color=\"grey\"><br/><br/>%s: %s<hr/></font><br/>",
				_("Morphology"), morph);
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, morph_text);
		g_string_printf(tmp_str, " %s<br/>", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);

	} else {
		g_string_printf(tmp_str,
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font) ? mf->old_font : "none",
				(mf->old_font_size) ? mf->old_font_size : "+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);

		g_string_printf(tmp_str, " %s", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);
	}

	HtmlOutput(str->str, d->previewer, NULL, NULL);
	free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}

/******************************************************************************
 * Name
 *  add_tree_item
 *
 * Synopsis
 *   #include "gui/gbs.h"
 *
 *   void add_tree_item()
 *
 * Description
 *
 *
 * Return value
 *   GtkCTreeNode*
 */

static void add_tree_item(GtkTreeModel *model, TreeItem *item, GtkTreeIter parent)
{

	GtkTreeIter iter;

	gtk_tree_store_append(GTK_TREE_STORE(model), &iter, &parent);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, item->pixbuf_opened,
			   COL_CLOSED_PIXBUF, item->pixbuf_closed,
			   COL_TITLE, item->item_name,
			   COL_BOOK, item->module_name,
			   COL_OFFSET, item->offset,
			   -1);
}

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

void main_dialogs_add_children_to_tree(GtkTreeModel *model, GtkTreeIter iter,
				       unsigned long offset, gboolean is_dialog, DIALOG_DATA *d)
{
	gchar buf[256];
	gchar *tmpbuf;
	TreeItem treeitem, *p_treeitem;
	BackEnd *be = (BackEnd *)d->backend;

	p_treeitem = &treeitem;
	p_treeitem->module_name = d->mod_name;

	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   -1);

	if (be->treekey_first_child(offset)) {
		offset = be->get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_treeitem->offset = buf;
		tmpbuf = be->treekey_get_local_name(offset);
		p_treeitem->item_name = (gchar *)tmpbuf;
		if (be->treekey_has_children(offset)) {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_closed;
			p_treeitem->pixbuf_closed = pixbufs->pixbuf_opened;
			p_treeitem->is_leaf = FALSE;
			p_treeitem->expanded = FALSE;
		} else {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_helpdoc;
			p_treeitem->pixbuf_closed = NULL;
			p_treeitem->is_leaf = TRUE;
			p_treeitem->expanded = FALSE;
		}
		add_tree_item(model, p_treeitem, iter);
		free(tmpbuf);
	}

	while (be->treekey_next_sibling(offset)) {
		offset = be->get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_treeitem->offset = buf;
		tmpbuf = be->treekey_get_local_name(offset);
		p_treeitem->item_name = (gchar *)tmpbuf;
		if (be->treekey_has_children(offset)) {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_closed;
			p_treeitem->pixbuf_closed = pixbufs->pixbuf_opened;
			p_treeitem->is_leaf = FALSE;
			p_treeitem->expanded = FALSE;
		} else {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_helpdoc;
			p_treeitem->pixbuf_closed = NULL;
			p_treeitem->is_leaf = TRUE;
			p_treeitem->expanded = FALSE;
		}
		add_tree_item(model, p_treeitem, iter);
		free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *  add_book_to_tree
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void add_book_to_tree(GtkWidget * tree, gchar * mod_name)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_dialogs_add_book_to_tree(GtkWidget *tree, gchar *mod_name,
				   gboolean is_dialog, DIALOG_DATA *d)
{
	GtkTreeIter iter;
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
	BackEnd *be = (BackEnd *)d->backend;

	gtk_tree_store_append(GTK_TREE_STORE(model), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_TITLE, d->mod_name,
			   COL_BOOK, d->mod_name,
			   COL_OFFSET, NULL,
			   -1);
	be->set_module(d->mod_name);
	be->set_treekey(0);
	main_dialogs_add_children_to_tree(model, iter, be->get_treekey_offset(),
					  is_dialog, d);
}

/******************************************************************************
 * Name
 *   tree_selection_changed
 *
 * Synopsis
 *   #include "main/gbs_main.h"
 *
 *   void tree_selection_changed(GtkTreeModel * model,
 *        GtkTreeSelection * selection, gboolean is_dialog, DIALOG_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_dialogs_tree_selection_changed(GtkTreeModel *model,
					 GtkTreeSelection *selection, gboolean is_dialog, DIALOG_DATA *g)
{
	GtkTreeIter selected;
	gchar *name = NULL;
	gchar *book = NULL;
	gchar *offset = NULL;
	BackEnd *be = (BackEnd *)g->backend;

	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		GtkTreePath *path = gtk_tree_model_get_path(model, &selected);
		//tree_level = gtk_tree_path_get_depth(path);
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &name,
				   3, &book,
				   4, &offset,
				   -1);
		gtk_tree_path_free(path);

		if (offset) {
			unsigned long l_offset = strtoul(offset, NULL, 0);
			g->offset = l_offset;
			be->set_module(book);
			be->set_treekey(l_offset);
			settings.book_key = be->treekey_get_local_name(l_offset);
			if (!gtk_tree_model_iter_has_child(model, &selected) &&
			    be->treekey_has_children(l_offset)) {
				main_dialogs_add_children_to_tree(model, selected,
								  l_offset, is_dialog, g);
			}
			//is_leaf = gtk_tree_model_iter_has_child(model, &selected);
			be->display_mod->display();
			g_free(name);
			g_free(book);
			g_free(offset);
		}
	}
}

/******************************************************************************
 * Name
 *   main_dialogs_dictionary_entry_changed
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_dialogs_dictionary_entry_changed(DIALOG_DATA * d)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_dialogs_dictionary_entry_changed(DIALOG_DATA *d)
{
	gint count = 10, i;
	gchar *key = NULL;
	static gboolean firsttime = TRUE;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	gint height;
	gchar *mod_name = d->mod_name;
	BackEnd *be = (BackEnd *)d->backend;

	if (!backend->is_module(mod_name))
		return;

	//key = (gchar*)gtk_entry_get_text(GTK_ENTRY(d->entry));
	be->set_module_key(mod_name, d->key);
	key = be->get_module_key();

	//	xml_set_value("Xiphos", "keys", "dictionary", key);
	//	settings.dictkey = xml_get_value("keys", "dictionary");

	be->set_module_key(mod_name, key);
	be->display_mod->display();

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(d->listview));
	list_store = GTK_LIST_STORE(model);

	if (!firsttime) {
#ifdef USE_GTK_3
		height = gdk_window_get_height(GDK_WINDOW(d->listview));
#else
		gdk_drawable_get_size((GdkDrawable *)d->listview->window,
				      NULL,
				      &height);
#endif
		count = height / settings.cell_height;
	}

	if (count) {
		gchar *new_key = be->navigate_module(-1);

		gtk_list_store_clear(list_store);

		for (i = 0; i < (count / 2); i++) {
			free(new_key);
			new_key = be->navigate_module(0);
		}

		for (i = 0; i < count; i++) {
			free(new_key);
			new_key = be->navigate_module(1);
			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, 0,
					   new_key, -1);
		}
		free(new_key);
	}
	firsttime = FALSE;
}

/******************************************************************************
 * Name
 *   main_free_on_destroy
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_free_on_destroy(DIALOG_DATA * vt)
 *
 * Description
 *   removes dialog from dialog_list when dialog is destroyed other than
 *   program shut down
 *
 * Return value
 *   void
 */

void main_free_on_destroy(DIALOG_DATA *d)
{
	list_dialogs = g_list_remove(list_dialogs, (DIALOG_DATA *)d);
	//	g_free(d->ops);
	g_free(d->key);
	g_free(d->mod_name);

	if (d->navbar.module_name)
		g_string_free(d->navbar.module_name, TRUE);
	if (d->navbar.key)
		g_string_free(d->navbar.key, TRUE);

	if ((BackEnd *)d->backend) {
		BackEnd *be = (BackEnd *)d->backend;
		delete be;
		be = NULL;
	}
	g_free(d);
}

/******************************************************************************
 * Name
 *   gui_bibletext_dialog_goto_bookmark
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_bibletext_dialog_goto_bookmark(gchar * mod_name, gchar * key)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_dialog_goto_bookmark(const gchar *module, const gchar *key)
{
	DIALOG_DATA *t = NULL;
	GList *tmp = NULL;

	tmp = g_list_first(list_dialogs);
	while (tmp != NULL) {
		t = (DIALOG_DATA *)tmp->data;
		if (!strcmp(t->mod_name, module)) {
			BackEnd *be = (BackEnd *)t->backend;
			if (t->mod_type == BOOK_TYPE) {
				t->offset = atoi(key);
				be->set_treekey(t->offset);
			} else {
				if (t->key)
					g_free(t->key);
				t->key = g_strdup(key);
				be->set_key(t->key);
			}
			if (t->mod_type == TEXT_TYPE || t->mod_type == COMMENTARY_TYPE)
				be->display_mod->display();
			gdk_window_raise(GDK_WINDOW(t->dialog));
			return;
		}
		tmp = g_list_next(tmp);
	}

	t = main_dialogs_open((gchar *)module, key, FALSE);
	BackEnd *be = (BackEnd *)t->backend;
	if (t->mod_type == BOOK_TYPE) {
		t->offset = atoi(key);
		be->set_treekey(t->offset);
	} else {
		if (t->key)
			g_free(t->key);
		t->key = g_strdup(key);
		be->set_key(t->key);
	}
	be->display_mod->display();
}

/******************************************************************************
 * Name
 *   main_dialogs_setup
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void main_dialogs_setup (GList *mods)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_dialogs_setup(void)
{
	list_dialogs = NULL;
	bible_freed = FALSE;
	dialog_freed = FALSE;
	bible_apply_change = FALSE;
	dlg_percom = NULL;
}

/******************************************************************************
 * Name
 *   main_keep_bibletext_dialog_in_sync
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_keep_bibletext_dialog_in_sync(gchar * key)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_keep_bibletext_dialog_in_sync(gchar *key)
{
	GList *tmp = NULL;
	gchar *url;
	tmp = g_list_first(list_dialogs);
	stop_window_sync++;
	while (tmp != NULL) {
		DIALOG_DATA *t = (DIALOG_DATA *)tmp->data;
		if (t->sync) {
			url = g_strdup_printf("sword://%s/%s", t->mod_name, key);
			main_dialogs_url_handler(t, url, TRUE);
			g_free(url);
		}
		tmp = g_list_next(tmp);
	}
	stop_window_sync--;
}

/******************************************************************************
 * Name
 *  main_dialogs_shutdown
 *
 * Synopsis
 *   #include ".h"
 *
 *  void main_dialogs_shutdown(void)
 *
 * Description
 *   shut down dialog support clean mem
 *
 * Return value
 *   void
 */

void main_dialogs_shutdown(void)
{
	GList *tmp = list_dialogs;
	XI_warning(("items = %d", g_list_length(tmp)));
	while (tmp != NULL) {
		DIALOG_DATA *t = (DIALOG_DATA *)tmp->data;
		bible_freed = TRUE;
		dialog_freed = TRUE;
		/*
		 *  destroy any dialogs created
		 */
		if (t->dialog)
			gtk_widget_destroy(t->dialog);
		/*
		 * free each DIALOG_DATA item created
		 */
		if ((BackEnd *)t->backend) {
			BackEnd *be = (BackEnd *)t->backend;
			delete be;
		}
		g_string_free(t->navbar.module_name, TRUE);
		g_string_free(t->navbar.key, TRUE);
		g_free(t->mod_name);
		g_free(t->key);
		g_free(t);
		tmp = g_list_next(tmp);
	}
	g_list_free(list_dialogs);
}

/******************************************************************************
 * Name
 *   show_note
 *
 * Synopsis
 *   #include ".h"
 *
 *   gint show_note(DIALOG_DATA * d,const gchar * module, const gchar * passage,
		const gchar * type, const gchar * value, gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_note(DIALOG_DATA *d, const gchar *module, const gchar *passage,
		      const gchar *type, const gchar *value, gboolean clicked)
{
	gchar *tmpbuf = NULL;
	gchar *buf = NULL;
	GString *str = g_string_new(NULL);
	GList *tmp = NULL;
	gint i = 0;
	BackEnd *be = (BackEnd *)d->backend;
	RESULTS *list_item;

	if (!be->is_module((gchar *)module))
		module = settings.MainWindowModule;

	if (passage && (strlen(passage) < 5))
		passage = d->key;

	if (strstr(type, "x") && clicked) {
		be->set_module_key((gchar *)module, (gchar *)passage);
		tmpbuf = be->get_entry_attribute("Footnote",
						 (gchar *)value,
						 "refList");
		if (tmpbuf) {
			main_display_verse_list_in_sidebar(d->key,
							   (gchar *)module,
							   tmpbuf);
			g_free(tmpbuf);
		}
	} else if (strstr(type, "n") && !clicked) {
		be->set_module_key((gchar *)module, (gchar *)passage);
		tmpbuf = be->get_entry_attribute("Footnote",
						 (gchar *)value,
						 "body");
		buf = be->render_this_text((gchar *)module, (gchar *)tmpbuf);
		if (tmpbuf)
			g_free(tmpbuf);
		if (buf) {
			main_dialog_information_viewer(module,
						       buf,
						       value,
						       "showNote",
						       type,
						       NULL,
						       NULL,
						       d);
			g_free(buf);
		}
	} else if (strstr(type, "x") && !clicked) {
		be->set_module_key((gchar *)module, (gchar *)passage);
		tmpbuf = be->get_entry_attribute("Footnote",
						 (gchar *)value,
						 "refList");

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

		tmp = be->parse_verse_list(d->mod_name, tmpbuf, d->key);
		while (tmp != NULL) {
			buf = g_strdup_printf(
			    "<a href=\"sword://%s/%s\">"
			    "<font color=\"%s\">%s,</font></a><br/>",
			    (gchar *)module,
			    (const char *)tmp->data,
			    settings.bible_text_color,
			    (const char *)tmp->data);
			str = g_string_append(str, buf);
			if (buf)
				g_free(buf);
			buf = NULL;
			++i;
			g_free((char *)tmp->data);
			tmp = g_list_next(tmp);
		}
		g_list_free(tmp);

		buf = g_strdup_printf(
		    "<a href=\"sword://%s/%s\">"
		    "<font color=\"%s\">%s%s</font></a><br/>",
		    (gchar *)module,
		    d->key,
		    settings.bible_text_color,
		    _("Back to "),
		    d->key);
		str = g_string_append(str, buf);
		if (buf)
			g_free(buf);

		if (tmpbuf)
			g_free(tmpbuf);
		if (str) {
			main_dialog_information_viewer(module,
						       str->str,
						       value,
						       "showNote",
						       type,
						       NULL,
						       NULL,
						       d);
		}
	}
	g_string_free(str, 1);
	return 1;
}

/******************************************************************************
 * Name
 *   show_morph
 *
 * Synopsis
 *   #include ".hh"
 *
 *   gint show_morph(DIALOG_DATA * d, const gchar * type,
			const gchar * value, gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_morph(DIALOG_DATA *d, const gchar *type,
		       const gchar *value, gboolean clicked)
{
	gchar *modbuf = NULL;

	BackEnd *be = (BackEnd *)d->backend;

	if (!strcmp(type, "Greek") || strstr(type, "x-Robinson") || strstr(type, "robinson")) {
		if (be->is_module("Robinson"))
			modbuf = (char *)"Robinson";
	}
	if (clicked) {
		static GtkWidget *dlg;

		if (!gsI_isrunning)
			dlg = gui_create_display_informtion_dialog();
		else
			gtk_widget_show(dlg);

		gui_display_mod_and_key(modbuf, value);
		gtk_window_set_title(GTK_WINDOW(dialog_display_info),
				     modbuf);
	} else {
		gchar *mybuf = main_get_rendered_text(modbuf, (gchar *)value);
		if (mybuf) {
			main_dialog_information_viewer(modbuf,
						       mybuf,
						       value,
						       "showMorph",
						       type,
						       NULL,
						       NULL,
						       d);
			g_free(mybuf);
		}
	}
	return 1;
}

/******************************************************************************
 * Name
 *   show_strongs
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_strongs(DIALOG_DATA * t, const gchar * type,
			const gchar * value, gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_strongs(DIALOG_DATA *t, const gchar *type,
			 const gchar *value, gboolean clicked)
{
	const gchar *modbuf = NULL;

	if (!strncmp(t->mod_name, "NASB", 4)) {
		if (!strcmp(type, "Greek"))
			modbuf = "NASGreek";
		else
			modbuf = "NASHebrew";

	} else {
		if (!strcmp(type, "Greek"))
			modbuf = settings.lex_greek;
		else if (!strcmp(type, "Hebrew"))
			modbuf = settings.lex_hebrew;
	}

	if (clicked) {
		static GtkWidget *dlg;

		if (!gsI_isrunning)
			dlg = gui_create_display_informtion_dialog();
		else
			gtk_widget_show(dlg);

		gui_display_mod_and_key(modbuf, value);
		gtk_window_set_title(GTK_WINDOW(dialog_display_info), modbuf);
	} else {
		gchar *mybuf = main_get_rendered_text((gchar *)modbuf, (gchar *)value);
		if (mybuf) {
			main_dialog_information_viewer(
			    modbuf,
			    mybuf,
			    value,
			    "showStrongs",
			    type,
			    NULL,
			    NULL,
			    t);
			g_free(mybuf);
		}
	}
	return 1;
}

/******************************************************************************
 * Name
 *   sword_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint sword_uri(const gchar * url)
 *
 * Description
 *   handle a sword uri in the form 'sword://KJV/1John5:8'
 *                                   'sword://MHC/Genesis1:1'
 *                                   'sword:///Romans8:28'
 *   and display in the appropriate pane
 *
 * Return value
 *   gint
 */

static gint sword_uri(DIALOG_DATA *t, const gchar *url, gboolean clicked)
{
	gchar *module = NULL;
	gchar *key = NULL;
	gchar **work_buf = NULL;

	BackEnd *be = (BackEnd *)t->backend;

	if (!clicked)
		return 0;

	work_buf = g_strsplit(url, "/", 4);
	if (!work_buf[MODULE] && !work_buf[KEY]) {
		g_strfreev(work_buf);
		return 0;
	}

	if (strlen(work_buf[MODULE]) < 2)
		module = g_strdup(t->mod_name);
	else
		module = g_strdup(work_buf[MODULE]);

	if (strlen(work_buf[KEY]) < 5)
		key = g_strdup(t->key);
	else
		key = g_strdup(work_buf[KEY]);

	if (t->key)
		g_free(t->key);
	t->key = be->get_valid_key(t->mod_name, key);

	if (t->mod_name)
		g_free(t->mod_name);
	t->mod_name = g_strdup(module);

	int mtype = be->module_type(t->mod_name);
	if ((mtype == TEXT_TYPE) || (mtype == COMMENTARY_TYPE)) {
		t->navbar.valid_key = valid_scripture_key =
		    main_is_Bible_key(t->mod_name, key);
	}
	be->set_module_key(t->mod_name, t->key);
	be->display_mod->display();
	valid_scripture_key = TRUE; // leave nice for future use.
	if (t->navbar.module_name)
		main_navbar_versekey_set(t->navbar, t->key);

	g_free(module);
	g_free(key);
	g_strfreev(work_buf);
	return 1;
}

static gint show_strongs_morph(DIALOG_DATA *d, const gchar *type, const gchar *value,
			       const gchar *morph, gboolean clicked)
{
	const gchar *modbuf = NULL;
	const gchar *morph_mod = NULL;

	if (!strncmp(d->mod_name, "NASB", 4)) {
		if (!strcmp(type, "Greek"))
			modbuf = "NASGreek";
		else
			modbuf = "NASHebrew";
	} else {
		if (!strcmp(type, "Greek")) {
			modbuf = settings.lex_greek;
			if (backend->is_module("Robinson"))
				morph_mod = "Robinson";
		} else
			modbuf = settings.lex_hebrew;
	}

	if (clicked) {
		static GtkWidget *dlg;

		if (!gsI_isrunning)
			dlg = gui_create_display_informtion_dialog();
		else
			gtk_widget_show(dlg);

		gui_display_mod_and_key(modbuf, (gchar *)value);
		gtk_window_set_title(GTK_WINDOW(dialog_display_info),
				     modbuf);
	} else {
		gchar *strongs_buf, *morph_buf;

		strongs_buf =
		    main_get_rendered_text((gchar *)modbuf, (gchar *)value);
		morph_buf =
		    main_get_rendered_text((gchar *)morph_mod, (gchar *)morph);
		if (strongs_buf) {
			main_dialog_information_viewer(
			    modbuf,
			    strongs_buf,
			    value,
			    "showStrongs",
			    type,
			    morph_buf,
			    morph,
			    d);
			g_free(strongs_buf);
			if (morph_buf)
				g_free(morph_buf);
		}
	}
	return 1;
}

/******************************************************************************
 * Name
 *   show_mod_info
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_mod_info(const gchar * module, const gchar * description,
 *						gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_mod_info(const gchar *module, const gchar *description,
			  gboolean clicked)
{
	if (clicked) {
		XI_print(("module = %s\n", module));
		gui_display_about_module_dialog((gchar *)module);
	} else {
		/* some mod descriptions contain fun(ny) characters */
		GString *desc_clean = hex_decode(description);
		XI_print(("description = %s\n", desc_clean->str));
		gui_set_statusbar(desc_clean->str);
		g_string_free(desc_clean, TRUE);
	}
	return 1;
}

/******************************************************************************
 * Name
 *   reference_uri
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint reference_uri(const gchar * url)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_ref(const gchar *module, const gchar *list, gboolean clicked)
{
	if (!clicked)
		return 1;

	if (!backend->is_module(module))
		module = settings.MainWindowModule;
	main_display_verse_list_in_sidebar(settings.currentverse,
					   (gchar *)module,
					   (gchar *)list);
	return 1;
}

static gint new_url_handler(DIALOG_DATA *t, const gchar *url, gboolean clicked)
{

	gchar *action = NULL;
	gchar *type = NULL;
	gchar *value = NULL;
	gchar *module = NULL;
	gchar *strongs = NULL;
	gchar *morph = NULL;
	URL *m_url;

	XI_message(("new_url_handler url = %s", url));
	m_url = new URL((const char *)url);
	action = g_strdup(m_url->getParameterValue("action"));
	type = g_strdup((gchar *)m_url->getParameterValue("type"));
	value = g_strdup((gchar *)m_url->getParameterValue("value"));
	morph = g_strdup((gchar *)m_url->getParameterValue("morph"));
	strongs = g_strdup((gchar *)m_url->getParameterValue("lemma"));

	XI_message(("action = %s", action));
	XI_message(("type = %s", type));
	XI_message(("value = %s", value));

	if (strlen(strongs) >= 1 && strlen(morph) >= 1) {
		show_strongs_morph(t, type, strongs, morph, clicked);
	} else if (strlen(strongs) >= 1 && strlen(morph) < 1) {
		show_strongs(t, type, strongs, clicked);
	}

	if (!strcmp(action, "showStrongs")) {
		show_strongs(t, type, value, clicked);
	}

	if (!strcmp(action, "showModInfo")) {
		module = g_strdup(m_url->getParameterValue("module"));
		show_mod_info(module, value, clicked);
		if (module)
			g_free(module);
	}

	if (!strcmp(action, "showMorph")) {
		show_morph(t, type, value, clicked);
	}

	if (!strcmp(action, "showNote")) {
		gchar *passage = g_strdup((gchar *)m_url->getParameterValue("passage"));
		module = g_strdup(m_url->getParameterValue("module"));
		show_note(t, module, passage, type, value, clicked);
		if (module)
			g_free(module);
		if (passage)
			g_free(passage);
	}

	if (!strcmp(action, "showRef")) {
		module = g_strdup(m_url->getParameterValue("module"));
		show_ref(module, value, clicked);
		if (module)
			g_free(module);
	}

	g_free(action);
	g_free(type);
	g_free(value);
	return 1;
}

/******************************************************************************
 * Name
 *   main_dialogs_url_handler
 *
 * Synopsis
 *   #include "main/module_dialogs.h"
 *
 *   gint main_dialogs_url_handler(DIALOG_DATA * t, const gchar * url, gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

gint main_dialogs_url_handler(DIALOG_DATA *t, const gchar *url, gboolean clicked)
{
	static int handling_url = FALSE;
	gint retval = 0;

	if (handling_url)
		return retval;
	handling_url = TRUE;

	XI_message(("main_dialogs_url_handler url = %s", url));

	if (strstr(url, "sword://") ||
	    strstr(url, "bible://")) {
		if ((settings.special_anchor = strchr((gchar *)url, '#')) || /* thml */
		    (settings.special_anchor = strchr((gchar *)url, '!')))   /* osisref */
			*(settings.special_anchor++) = '\0';

		GString *url_clean = hex_decode(url);
		XI_message(("url_clean = %s", url_clean->str));

		retval = sword_uri(t, url_clean->str, clicked);
		g_string_free(url_clean, TRUE);
	}
	if (strstr(url, "passagestudy.jsp") || strstr(url, "xiphos.url")) {
		handling_url = FALSE;
		return new_url_handler(t, url, clicked);
	}
	/*if (strstr(url,"sword://"))
		return sword_uri(t, url, clicked);*/

	handling_url = FALSE;
	return retval;
}

/******************************************************************************
 * Name
 *   main_dialogs_open
 *
 * Synopsis
 *   #include "modules_dialogs.h"
 *
 *   void main_dialogs_open(const gchar * mod_name, const gchar *key, const gboolean maximize)
 *
 * Description
 *   open a module as a separate dialog, w/optional key, with possibly maximized window.
 *
 * Return value
 *   void
 */

DIALOG_DATA *main_dialogs_open(const gchar *mod_name,
			       const gchar *key,
			       const gboolean maximize)
{
	BackEnd *be;
	DIALOG_DATA *t = NULL;
	gint type;
	gchar *direction = NULL;

	do_display = TRUE;
	if (!backend->is_module(mod_name))
		return NULL;
	type = backend->module_type(mod_name);

	t = g_new0(DIALOG_DATA, 1);
	t->backend = (BackEnd *)new BackEnd();
	be = (BackEnd *)t->backend;

	t->navbar.module_name = NULL;
	t->editor = NULL;
	t->search_string = NULL;
	t->dialog = NULL;
	t->key = NULL;
	t->is_dialog = TRUE;
	t->mod_name = g_strdup(mod_name);
	t->is_rtol = FALSE;
	if ((direction = be->get_config_entry(t->mod_name, (char *)"Direction")) != NULL) {
		if (!strcmp(direction, "RtoL"))
			t->is_rtol = TRUE;
	}
	t->sync = FALSE;

	switch (type) {
	case TEXT_TYPE:
		t->mod_type = TEXT_TYPE;
		gui_create_bibletext_dialog(t);
		be->chapDisplay = new DialogChapDisp(t->html, t, be);
		be->init_SWORD(1);
		if (key)
			t->key = g_strdup(key);
		else
			t->key = g_strdup(settings.currentverse);
		dlg_bible = t;
		t->navbar.module_name = g_string_new(mod_name);
		t->navbar.key = g_string_new(settings.currentverse);
		break;

	case COMMENTARY_TYPE:
	case PERCOM_TYPE:
		t->mod_type = COMMENTARY_TYPE;
		gui_create_commentary_dialog(t, FALSE);
		be->entryDisplay = new DialogEntryDisp(t->html, t, be);
		be->init_SWORD(1);
		if (key)
			t->key = g_strdup(key);
		else
			t->key = g_strdup(settings.currentverse);
		t->navbar.module_name = g_string_new(mod_name);
		t->navbar.key = g_string_new(settings.currentverse);
		break;

	case DICTIONARY_TYPE:
		t->mod_type = DICTIONARY_TYPE;
		gui_create_dictlex_dialog(t);
		be->entryDisplay = new DialogEntryDisp(t->html, t, be);
		be->init_SWORD(1);

		if (key)
			t->key = g_strdup(key);
		else
			t->key = g_strdup(settings.dictkey);

		// for devotional use.
		gchar buf[10];

		time_t curtime;
		char *feature;
		if ((feature = (char *)be->get_mgr()->getModule(mod_name)->getConfigEntry("Feature")) &&
		    !strcmp(feature, "DailyDevotion")) {
			if ((strlen(t->key) != 5) || // blunt tests.
			    (t->key[0] < '0') || (t->key[0] > '9') ||
			    (t->key[1] < '0') || (t->key[1] > '9') ||
			    (t->key[2] != '.') ||
			    (t->key[3] < '0') || (t->key[3] > '9') ||
			    (t->key[4] < '0') || (t->key[4] > '9')) { // not MM.DD
				struct tm *loctime;

				curtime = time(NULL);
				loctime = localtime(&curtime);
				strftime(buf, 10, "%m.%d", loctime);
				g_free(t->key);
				t->key = g_strdup(buf);
			}
		}
		break;

	case BOOK_TYPE:
	case PRAYERLIST_TYPE:
		t->mod_type = BOOK_TYPE;
		gui_create_gbs_dialog(t);
		be->entryDisplay = new DialogEntryDisp(t->html, t, be);
		be->init_SWORD(1);
		t->key = NULL;
		if (key)
			t->offset = atoi(key);
		else
			t->offset = 4;
		break;
	}

	gtk_widget_show(t->dialog);

	// F11-invoked "open maximally."
	// absurd dimensions are deliberate: just make it huge.
	if (maximize)
		gtk_window_resize(GTK_WINDOW(t->dialog), 10000, 10000);
	sync_windows();

	list_dialogs = g_list_append(list_dialogs, (DIALOG_DATA *)t);
	be->set_module(t->mod_name);
	if (type == BOOK_TYPE) {
		main_dialogs_add_book_to_tree(t->tree, t->mod_name,
					      TRUE, t);
		be->set_treekey(t->offset);
		main_setup_navbar_book_dialog(t);
		XI_message(("offset = %ld", t->offset));
	} else
		be->set_key(t->key);

	if (be->get_config_entry(t->mod_name, (char *)"CipherKey")) {
		t->cipher_old = be->get_config_entry(t->mod_name, (char *)"CipherKey");
		t->is_locked = strlen(t->cipher_old);
	} else {
		t->is_locked = 0;
		t->cipher_old = NULL;
	}

	if ((type != BOOK_TYPE) &&
	    (type != PRAYERLIST_TYPE) &&
	    (type != DICTIONARY_TYPE)) {
		valid_scripture_key = main_is_Bible_key(t->mod_name, t->key);
		be->display_mod->display();
		valid_scripture_key = TRUE; // leave nice for future use.
		bible_apply_change = TRUE;
	}
	if (type == TEXT_TYPE)
		main_dialogs_clear_viewer(t);

	if ((type == COMMENTARY_TYPE) ||
	    (type == PERCOM_TYPE) ||
	    (type == TEXT_TYPE))
		main_navbar_versekey_set(t->navbar, (char *)t->key);

	if (type == DICTIONARY_TYPE)
		gtk_entry_set_text(GTK_ENTRY(t->entry), t->key);
	return t;
}

/******   end of file   ******/
