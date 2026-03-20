/*
 * Xiphos Bible Study Tool
 * bookmarks_treeview.c - gui for bookmarks using treeview
 *
 * Copyright (C) 2003-2025 Xiphos Developer Team
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

#include <glib.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>

#include <math.h>
#include <cairo.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gui/xiphos.h"
#include "gui/bookmarks_treeview.h"
#include "gui/bookmarks_menu.h"
#include "gui/bookmark_dialog.h"
#include "gui/utilities.h"
#include "gui/main_window.h"
#include "gui/dialog.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/sidebar.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/module_dialogs.h"
#include "main/url.hh"

#include "gui/debug_glib_null.h"

GtkTreeStore *model;
GtkTreeModel *tree_model;
gboolean button_one;
gboolean use_dialog;
GtkTreeSelection *current_selection;
GtkTreeView *bookmark_tree;

BookMarksPixbufs *bm_pixbufs;

/******************************************************************************
 * Name
 *   gui_verselist_to_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void gui_verselist_to_bookmarks(GList * list)
 *
 * Description
 *   add results of search to tree as a root node with children
 *
 * Return value
 *   void
 */

void gui_verselist_to_bookmarks(GList *verses, gint save_as_single)
{
	gint test;
	gchar *module_name = NULL;
	GtkTreeIter parent;
	GtkTreeIter iter;
	GS_DIALOG *info;
	RESULTS *list_item;

	/*gchar *dlg = g_strdup_printf("<span weight=\"bold\">%s</span>\n%s",
	   _("Save these results as a single bookmark?"),
	   _("(rather than as a series of bookmarks)")); */
	if (save_as_single) {
		GString *name = g_string_new(NULL);
		GString *verse_string = g_string_new("");
		gboolean first_entry = TRUE;

		g_string_printf(name, _("Search result: %s"),
				settings.searchText);
		while (verses) {
			list_item = (RESULTS *)verses->data;
			if (main_is_Bible_key(list_item->module, list_item->key)) {
				if (first_entry) {
					module_name =
					    g_strdup(list_item->module);
					first_entry = FALSE;
				} else {
					verse_string =
					    g_string_append(verse_string,
							    "; ");
				}
				verse_string =
				    g_string_append(verse_string,
						    list_item->key);
			}
			verses = g_list_next(verses);
		}
		gui_bookmark_dialog(name->str, module_name,
				    verse_string->str);
		g_string_free(name, TRUE);
		g_string_free(verse_string, TRUE);
		g_free(module_name);
		return;
	}

	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &iter);
	info = gui_new_dialog();
#if GTK_CHECK_VERSION(3, 10, 0)
	info->stock_icon = "document-open";
#else
	info->stock_icon = GTK_STOCK_OPEN;
#endif

	info->title = _("Bookmark");
	info->label_top = _("Enter Folder Name");
	info->text1 = g_strdup(settings.searchText);
	info->label1 = _("Folder: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	/*** open dialog to get name for root node ***/

	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		gtk_tree_store_append(GTK_TREE_STORE(model), &parent,
				      &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &parent,
				   COL_OPEN_PIXBUF,
				   bm_pixbufs->pixbuf_opened,
				   COL_CLOSED_PIXBUF,
				   bm_pixbufs->pixbuf_closed,
				   COL_CAPTION, info->text1,
				   COL_KEY, NULL, COL_MODULE, NULL, -1);
		//              set_results_position((char) 1); // TOP
		GString *str = g_string_new(" ");
		while (verses) {
			list_item = (RESULTS *)verses->data;
			module_name = list_item->module;
			gchar *tmpbuf = list_item->key;
			g_string_printf(str, "%s, %s", tmpbuf,
					module_name);
			XI_message(("bookmark: %s", str->str));
			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &iter, &parent);
			gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
					   COL_OPEN_PIXBUF,
					   bm_pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION, str->str,
					   COL_KEY, tmpbuf,
					   COL_MODULE, module_name, -1);
			verses = g_list_next(verses);
		}
		g_string_free(str, TRUE);
		bookmarks_changed = TRUE;
		gui_save_bookmarks_treeview();
	}
	g_free(info->text1);
	g_free(info);
}

/******************************************************************************
 * Name
 *   get_xml_folder_data
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void get_xml_folder_data(xmlNodePtr cur, BOOKMARK_DATA * data)
 *
 * Description
 *    get date from xml bookmark folder and put into
 *    BOOKMARK_DATA structure
 *
 * Return value
 *   void
 */

static void get_xml_folder_data(xmlNodePtr cur, BOOKMARK_DATA *data)
{
	xmlChar *folder;
	gchar *color;

	folder = xmlGetProp(cur, (const xmlChar *)"caption");
	data->caption = g_strdup((char *)folder);
	data->key = NULL;
	data->module = NULL;
	data->module_desc = NULL;
	data->description = NULL;
	data->is_leaf = FALSE;
	data->opened = bm_pixbufs->pixbuf_opened;
	data->closed = bm_pixbufs->pixbuf_closed;

	/* color is optional — NULL when the folder has no tag color assigned */
	color = xml_get_folder_color(cur);
	data->color = color ? g_strdup(color) : NULL;
	if (color)
		xmlFree((xmlChar *)color);
}

/******************************************************************************
 * Name
 *    get_xml_bookmark_data
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void get_xml_bookmark_data(xmlNodePtr cur, BOOKMARK_DATA * data)
 *
 * Description
 *    get date from xml bookmark and put into BOOKMARK_DATA structure
 *
 * Return value
 *   void
 */

static void get_xml_bookmark_data(xmlNodePtr cur, BOOKMARK_DATA *data)
{
	xmlChar *mod1;
	xmlChar *key;
	xmlChar *caption;
	xmlChar *mod_desc;
	xmlChar *description;
	gchar buf[500];

	data->opened = bm_pixbufs->pixbuf_helpdoc;
	data->closed = NULL;
	mod1 = xmlGetProp(cur, (const xmlChar *)"modulename");
	key = xmlGetProp(cur, (const xmlChar *)"key");
	mod_desc = xmlGetProp(cur, (const xmlChar *)"moduledescription");
	description = xmlGetProp(cur, (const xmlChar *)"description");
	caption = xmlGetProp(cur, (const xmlChar *)"description");
	if ((char *)caption) {
		if (strlen((char *)caption) > 0) {
			data->caption = g_strdup((char *)caption);
		} else {
			sprintf(buf, "%s, %s", key, mod1);
			data->caption = g_strdup((char *)buf);
		}
	} else
		data->caption = g_strdup((char *)key);
	data->key = g_strdup((char *)key);
	data->module = g_strdup((char *)mod1);
	data->description = g_strdup((char *)description);
	data->module_desc = g_strdup((char *)mod_desc);
	data->is_leaf = TRUE;
	data->color = NULL;  /* leaves (bookmarks) never carry a color */
}

/******************************************************************************
 * Name
 *  free_bookmark_data
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void free_bookmark_data(BOOKMARK_DATA *data)
 *
 * Description
 *    frees the BOOKMARK_DATA structure
 *
 * Return value
 *   void
 */

static void free_bookmark_data(BOOKMARK_DATA *data)
{
	if (data->caption)
		g_free(data->caption);
	if (data->key)
		g_free(data->key);
	if (data->module)
		g_free(data->module);
	if (data->module_desc)
		g_free(data->module_desc);
	if (data->description)
		g_free(data->description);
	if (data->color)
		g_free(data->color);
}

/******************************************************************************
 * Name
 *  gui_add_node_to_ctree
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   GtkCTreeNode *gui_add_node_to_ctree(GtkCTree * ctree,
 *			GtkCTreeNode *node, BOOKMARK_DATA * data)
 *
 * Description
 *    actually add the GtkCTreeNode to the bookmark ctree
 *
 * Return value
 *   GtkCTreeNode
 */

void gui_add_item_to_tree(GtkTreeIter *iter, GtkTreeIter *parent,
			  BOOKMARK_DATA *data)
{
	gtk_tree_store_append(GTK_TREE_STORE(model), iter, parent);

	gtk_tree_store_set(GTK_TREE_STORE(model), iter,
			   COL_OPEN_PIXBUF, data->opened,
			   COL_CLOSED_PIXBUF, data->closed,
			   COL_CAPTION, data->caption,
			   COL_KEY, data->key,
			   COL_MODULE, data->module,
			   COL_MODULE_DESC, data->module_desc,
			   COL_DESCRIPTION, data->description,
			   COL_COLOR, data->color,
			   -1);
}

/******************************************************************************
 * Name
 *  add_node
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void add_node(xmlDocPtr doc, xmlNodePtr cur, GtkCTree * ctree,
						GtkCTreeNode *node)
 *
 * Description
 *    parse the xml bookmarks and add to bookmark ctree
 *
 * Return value
 *   void
 */

static void add_node(xmlNodePtr cur, GtkTreeIter *parent)
{
	GtkTreeIter iter;
	BOOKMARK_DATA data, *p = NULL;
	xmlNodePtr work = NULL;

	p = &data;
	if (cur == NULL)
		return;

	for (work = cur->xmlChildrenNode; work; work = work->next) {
		if (!xmlStrcmp(work->name, (const xmlChar *)"Bookmark")) {
			get_xml_bookmark_data(work, p);
			gui_add_item_to_tree(&iter, parent, p);
			free_bookmark_data(p);
		} else if (!xmlStrcmp(work->name, (const xmlChar *)"Folder")) {
			get_xml_folder_data(work, p);
			gui_add_item_to_tree(&iter, parent, p);
			free_bookmark_data(p);
			add_node(work, &iter);
		}
	}
}

/******************************************************************************
 * Name
 *  gui_parse_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void gui_parse_bookmarks(GtkCTree * ctree)
 *
 * Description
 *    load a xml bookmark file
 *
 * Return value
 *   void
 */

void gui_parse_bookmarks(GtkTreeView *tree, const xmlChar *file,
			 GtkTreeIter *parent)
{
	xmlNodePtr cur = NULL;
	BOOKMARK_DATA data, *p = NULL;
	GtkTreeIter iter;
	GtkTreePath *path;

	p = &data;
	cur = xml_load_bookmark_file(file);
	//cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Bookmark")) {
			get_xml_bookmark_data(cur, p);
			gui_add_item_to_tree(&iter, parent, p);
			free_bookmark_data(p);
		} else {
			get_xml_folder_data(cur, p);
			if (p->caption) {
				gui_add_item_to_tree(&iter, parent, p);
			}
			free_bookmark_data(p);
			add_node(cur, &iter);
		}

		if (cur->next)
			cur = cur->next;
		else
			break;
	}
	xml_free_bookmark_doc();
	path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), parent);
	gtk_tree_view_expand_to_path(tree, path);
	gtk_tree_path_free(path);
}

/******************************************************************************
 * Name
 *  load_xml_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void load_xml_bookmarks(GtkCTree * ctree)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void load_xml_bookmarks(GtkTreeView *tree, GtkTreeIter *iter)
{
	GString *str;
	const xmlChar *file;

	str = g_string_new(NULL);
	g_string_printf(str, "%s/bookmarks.xml", settings.swbmDir);
	file = (const xmlChar *)str->str;
	gui_parse_bookmarks(tree, file, iter);
	g_string_free(str, TRUE);
}

/******************************************************************************
 * Name
 *   row_changed
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void row_changed(GtkTreeModel *treemodel, GtkTreePath *arg1,
 *                             GtkTreeIter *arg2, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void row_changed(GtkTreeModel *treemodel, GtkTreePath *arg1,
			GtkTreeIter *arg2, gpointer user_data)
{
	bookmarks_changed = TRUE;
	gui_save_bookmarks_treeview();
}

/******************************************************************************
 * Name
 *   row_changed
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void row_deleted(GtkTreeModel * treemodel, GtkTreePath * arg1,
 *							gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void row_deleted(GtkTreeModel *treemodel, GtkTreePath *arg1,
			gpointer user_data)
{
	bookmarks_changed = TRUE;
	gui_save_bookmarks_treeview();
}

/******************************************************************************
 * Name
 *   create_pixbufs
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void create_pixbufs(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void create_pixbufs(void)
{
	GtkTextDirection dir =
	    gtk_widget_get_direction(GTK_WIDGET(widgets.app));

	bm_pixbufs = g_new0(BookMarksPixbufs, 1);

	if (dir == GTK_TEXT_DIR_LTR) {
		/* leave this for if we make custom reversable icons
		   if (!bm_pixbufs->pixbuf_closed)
		   bm_pixbufs->pixbuf_closed =
		   pixbuf_finder("book_closed.png", 16, NULL);
		 */

		bm_pixbufs->pixbuf_opened =
		    pixbuf_finder("book_open.png", 16, NULL);
	} else {
		/* leave this for if we make custom reversable icons
		   if (!bm_pixbufs->pixbuf_closed)
		   bm_pixbufs->pixbuf_closed =
		   pixbuf_finder("book_closed_rtol.png", 16, NULL);
		 */

		bm_pixbufs->pixbuf_opened =
		    pixbuf_finder("book_open_rtol.png", 16, NULL);
	}

	bm_pixbufs->pixbuf_closed =
	    pixbuf_finder("epiphany-bookmarks.png", 0, NULL);

	bm_pixbufs->pixbuf_helpdoc =
	    pixbuf_finder("epiphany-bookmark-page.png", 0, NULL);

	if (!bm_pixbufs->pixbuf_helpdoc)
#ifdef USE_GTK_3
		bm_pixbufs->pixbuf_helpdoc =
#if GTK_CHECK_VERSION(3, 10, 0)
		    GDK_PIXBUF(gtk_image_new_from_icon_name("gtk-dnd",
							    GTK_ICON_SIZE_BUTTON));
#else
		    gtk_widget_render_icon_pixbuf(widgets.app,
						  GTK_STOCK_DND,
						  GTK_ICON_SIZE_MENU);
#endif

#else
		bm_pixbufs->pixbuf_helpdoc = gtk_widget_render_icon(widgets.app,
								    GTK_STOCK_DND,
								    GTK_ICON_SIZE_MENU, NULL);
#endif
}

/******************************************************************************
 * Name
 *   add_columns
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   void add_columns(GtkTreeView * tree)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

/* Creates a round color swatch pixbuf using Cairo (14x14 px) */
#ifdef USE_GTK_3
static GdkPixbuf *make_color_dot(const gchar *hex_color)
{
	const gint SIZE = 14;
	cairo_surface_t *surface;
	cairo_t *cr;
	GdkPixbuf *pixbuf;
	GdkRGBA rgba;

	if (!hex_color || !*hex_color)
		return NULL;
	if (!gdk_rgba_parse(&rgba, hex_color))
		return NULL;

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, SIZE, SIZE);
	cr = cairo_create(surface);

	/* transparent background */
	cairo_set_source_rgba(cr, 0, 0, 0, 0);
	cairo_paint(cr);

	/* filled circle */
	cairo_set_source_rgba(cr, rgba.red, rgba.green, rgba.blue, 1.0);
	cairo_arc(cr, SIZE/2.0, SIZE/2.0, SIZE/2.0 - 1, 0, 2 * G_PI);
	cairo_fill_preserve(cr);

	/* thin dark border */
	cairo_set_source_rgba(cr, 0, 0, 0, 0.35);
	cairo_set_line_width(cr, 1.0);
	cairo_stroke(cr);

	cairo_destroy(cr);
	pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, SIZE, SIZE);
	cairo_surface_destroy(surface);
	return pixbuf;
}

/* Cell data function: generates the color dot pixbuf on the fly */
static void color_dot_cell_func(GtkTreeViewColumn *col,
                                GtkCellRenderer   *renderer,
                                GtkTreeModel      *tree_model,
                                GtkTreeIter       *iter,
                                gpointer           data)
{
	gchar *color = NULL;
	GdkPixbuf *dot = NULL;
	gtk_tree_model_get(tree_model, iter, COL_COLOR, &color, -1);
	dot = make_color_dot(color);
	g_object_set(renderer, "pixbuf", dot, NULL);
	if (dot) g_object_unref(dot);
	g_free(color);
}
#endif
void gui_add_columns(GtkTreeView *tree)
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

	column = gtk_tree_view_column_new();

	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COL_OPEN_PIXBUF,
					    "pixbuf-expander-open", COL_OPEN_PIXBUF,
					    "pixbuf-expander-closed", COL_CLOSED_PIXBUF, NULL);


#ifdef USE_GTK_3
	/* Color dot renderer — round swatch drawn with Cairo */
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_cell_data_func(column, renderer,
						color_dot_cell_func,
						NULL, NULL);
#endif
	/* Caption renderer */
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_CAPTION, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_sort_column_id(column, COL_CAPTION);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_KEY, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);

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
					    "text", COL_MODULE_DESC, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);

	column = gtk_tree_view_column_new();
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "text", COL_DESCRIPTION, NULL);
	gtk_tree_view_append_column(tree, column);
	gtk_tree_view_column_set_visible(column, FALSE);
}

/******************************************************************************
 * Name
 *   create_model
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   GtkTreeModel *create_model(void)
 *
 *
 * Description
 *
 *
 * Return value
 *   GtkTreeModel *
 */

static GtkTreeModel *create_model(void)
{
	/* create tree store */
	model = gtk_tree_store_new(N_COLUMNS,
				   GDK_TYPE_PIXBUF,  /* COL_OPEN_PIXBUF  */
				   GDK_TYPE_PIXBUF,  /* COL_CLOSED_PIXBUF */
				   G_TYPE_STRING,    /* COL_CAPTION      */
				   G_TYPE_STRING,    /* COL_KEY          */
				   G_TYPE_STRING,    /* COL_MODULE       */
				   G_TYPE_STRING,    /* COL_MODULE_DESC  */
				   G_TYPE_STRING,    /* COL_DESCRIPTION  */
				   G_TYPE_STRING);   /* COL_COLOR        */
	return GTK_TREE_MODEL(model);
}

/******************************************************************************
 * Name
 *   button_release_event
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   gboolean button_release_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
 *
 * Description
 *   catch button 3 and select the row the pointer is over
 *   but does not display the bookmark
 *
 * Return value
 *   void
 */

static void lambda_open_url(GtkMenuItem *item, gpointer data)
{
	const gchar *url = (const gchar *)g_object_get_data(G_OBJECT(item), "url");
	if (url)
		main_url_handler(url, TRUE);
}

static gboolean button_release_event(GtkWidget *widget,
				     GdkEventButton *event, gpointer data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeIter selected;
	gboolean is_selected = FALSE;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *range_start = NULL;
	gchar *description = NULL;
	button_one = FALSE;

	selection = gtk_tree_view_get_selection(bookmark_tree);
	current_selection = selection;
	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption,
				   3, &key,
				   4, &module,
				   5, &mod_desc, 6, &description, -1);
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected) && key != NULL) {
			gboolean multi = (strpbrk(key, "-;,") != NULL);
			gtk_widget_set_sensitive(menu.in_tab, !multi);
			gtk_widget_set_sensitive(menu.in_dialog, !multi);
			gtk_widget_set_sensitive(menu.new, FALSE);
			gtk_widget_set_sensitive(menu.insert, FALSE);
			gtk_widget_set_sensitive(menu.remove, FALSE);
		} else {
			/* click on treeview folder to expand or collapse it */
			GtkTreePath *path =
			    gtk_tree_model_get_path(GTK_TREE_MODEL(model),
						    &selected);
			if (gtk_tree_view_row_expanded(bookmark_tree, path))
				gtk_tree_view_collapse_row(bookmark_tree,
							   path);
			else
				gtk_tree_view_expand_row(bookmark_tree,
							 path, FALSE);
			gtk_tree_path_free(path);

			gtk_widget_set_sensitive(menu.in_tab, FALSE);
			gtk_widget_set_sensitive(menu.in_dialog, FALSE);
			gtk_widget_set_sensitive(menu.new, TRUE);
			gtk_widget_set_sensitive(menu.insert, TRUE);
			gtk_widget_set_sensitive(menu.remove, TRUE);
		}

		gtk_widget_set_sensitive(menu.bibletime, TRUE);
		gtk_widget_set_sensitive(menu.edit, TRUE);
		gtk_widget_set_sensitive(menu.delete, TRUE);
		is_selected = TRUE;
	}

	if (!module || !strcmp(module, "")) {
		g_free(module);
		module = g_strdup(settings.MainWindowModule);
	}

	switch (event->button) {
	case 1:
		button_one = TRUE;
		break;
	case 2:
		gui_generic_warning(_("Opening a multi-reference bookmark in\n"
				      "separate tabs is not supported."));
		break;
	case 3:
		g_free(caption);
		g_free(key);
		g_free(module);
#if GTK_CHECK_VERSION(3, 22, 0)
		gtk_menu_popup_at_pointer(GTK_MENU(menu.menu), NULL);
#else
		gtk_menu_popup(GTK_MENU(menu.menu),
			       NULL, NULL, NULL, NULL,
			       event->button, event->time);
#endif

		if (!settings.browsing)
			gtk_widget_hide(menu.in_tab);

		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu.reorder))) {
			gtk_widget_set_sensitive(menu.in_tab, FALSE);
			gtk_widget_set_sensitive(menu.in_dialog, FALSE);
			gtk_widget_set_sensitive(menu.new, FALSE);
			gtk_widget_set_sensitive(menu.insert, FALSE);
			gtk_widget_set_sensitive(menu.edit, FALSE);
			gtk_widget_set_sensitive(menu.delete, FALSE);
			gtk_widget_set_sensitive(menu.bibletime, TRUE);
			gtk_widget_set_sensitive(menu.remove, FALSE);
			//gtk_widget_set_sensitive(menu.remove, FALSE);
			return TRUE;
		} else {
			gtk_widget_set_sensitive(menu.bibletime, TRUE);
			return FALSE;
		}
		break;
	}
	if (is_selected) {
		if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected) && key != NULL) {
			const gchar *real_mod = main_abbrev_to_name(module);
			/* multi if contains ";", "-" (range) or comma between verse numbers */
			gboolean multi = (strchr(key, ';') != NULL);
			gboolean is_range = FALSE;
			if (strchr(key, '-')) {
				/* verse range: navigate to first verse directly */
				const gchar *colon = strchr(key, ':');
				const gchar *dash  = strchr(key, '-');
				if (colon && dash && dash > colon) {
					is_range = TRUE;
					range_start = g_strndup(key, dash - key);
				}
			}
			if (!multi && strchr(key, ',')) {
				/* check if comma separates verses: "Eph 2:8,9" */
				const gchar *colon = strchr(key, ':');
				const gchar *comma = strchr(key, ',');
				if (colon && comma && comma > colon)
					multi = TRUE;
			}

			if (is_range && button_one && range_start) {
				/* navigate directly to first verse of range */
				gchar *url = g_strdup_printf(
					"passagestudy.jsp?action=showBookmark&"
					"type=%s&value=%s&module=%s",
					"currentTab",
					main_url_encode(range_start),
					main_url_encode((real_mod ? real_mod : module)));
				main_url_handler(url, TRUE);
				g_free(url);
				g_free(range_start);
				range_start = NULL;
			} else if (multi && button_one) {
				/* split on ";" first, then expand "book ch:v1,v2" */
				GList *refs = NULL;
				gchar **semis = g_strsplit(key, ";", -1);
				gchar *last_book = NULL;
				for (gint si = 0; semis[si]; si++) {
					gchar *part = g_strstrip(semis[si]);
					if (!*part) continue;
					gchar *full_part;
					/* if no space in part but has colon, prepend last book */
					if (last_book && strchr(part, ':') && !strchr(part, ' '))
						full_part = g_strdup_printf("%s %s", last_book, part);
					else {
						full_part = g_strdup(part);
						/* extract book name: before last space before colon */
						const gchar *col = strchr(full_part, ':');
						if (col) {
							const gchar *sp = col;
							while (sp > full_part && *sp != ' ') sp--;
							if (sp > full_part) {
								g_free(last_book);
								last_book = g_strndup(full_part, sp - full_part);
							}
						}
					}
					/* handle comma-separated verses */
					const gchar *colon = strchr(full_part, ':');
					const gchar *comma = strchr(full_part, ',');
					if (colon && comma && comma > colon) {
						gchar *prefix = g_strndup(full_part, colon - full_part + 1);
						gchar **vnums = g_strsplit(colon + 1, ",", -1);
						for (gint vi = 0; vnums[vi]; vi++) {
							gchar *vn = g_strstrip(vnums[vi]);
							if (*vn)
								refs = g_list_append(refs,
									g_strdup_printf("%s%s", prefix, vn));
						}
						g_strfreev(vnums);
						g_free(prefix);
					} else {
						refs = g_list_append(refs, g_strdup(full_part));
					}
					g_free(full_part);
				}
				g_free(last_book);
				g_strfreev(semis);
				GtkWidget *popup = gtk_menu_new();
				for (GList *l = refs; l; l = l->next) {
					gchar *ref = (gchar *)l->data;
					GtkWidget *item = gtk_menu_item_new_with_label(ref);
					gchar *url = g_strdup_printf(
						"passagestudy.jsp?action=showBookmark&"
						"type=%s&value=%s&module=%s",
						"currentTab",
						main_url_encode(ref),
						main_url_encode((real_mod ? real_mod : module)));
					g_object_set_data_full(G_OBJECT(item), "url", url, g_free);
					g_signal_connect(item, "activate",
						G_CALLBACK(lambda_open_url), NULL);
					gtk_menu_shell_append(GTK_MENU_SHELL(popup), item);
				}
				g_list_free_full(refs, g_free);
				gtk_widget_show_all(popup);
#if GTK_CHECK_VERSION(3, 22, 0)
				gtk_menu_popup_at_pointer(GTK_MENU(popup), NULL);
#else
				gtk_menu_popup(GTK_MENU(popup), NULL, NULL, NULL, NULL, 1,
					      gtk_get_current_event_time());
#endif
			} else {
				gchar *url = NULL;
				if (!strcmp(module, "studypad"))
					url = g_strdup_printf(
						"passagestudy.jsp?action=showStudypad&"
						"type=9&value=%s&module=%s",
						main_url_encode(key),
						main_url_encode((real_mod ? real_mod : module)));
				else if (button_one)
					url = g_strdup_printf(
						"passagestudy.jsp?action=showBookmark&"
						"type=%s&value=%s&module=%s",
						"currentTab",
						main_url_encode(key),
						main_url_encode((real_mod ? real_mod : module)));
				if (url) {
					main_url_handler(url, TRUE);
					g_free(url);
				}
			}
		}
		g_free(caption);
		g_free(range_start);
		g_free(key);
		g_free(module);
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *   gui_create_bookmark_tree
 *
 * Synopsis
 *   #include "gui/bookmarks_treeview.h"
 *
 *   GtkWidget *gui_create_bookmark_tree(void)
 *
 * Description
 *
 *
 * Return value
 *   GtkWidget*
 */

/**
 * bookmark_get_tag_color_for_key:
 * @osiskey: a verse key string, e.g. "Gen 1:1"
 *
 * Walks the bookmark GtkTreeStore and returns the color of the
 * first tag-group folder that contains a bookmark matching @osiskey.
 * Returns NULL if none found.  Caller must g_free() the result.
 */
static void debug_dump_recursive(GtkTreeIter *parent, int depth)
{
	GtkTreeIter child;
	if (!gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &child, parent))
		return;
	do {
		gchar *caption = NULL, *color = NULL;
		gtk_tree_model_get(GTK_TREE_MODEL(model), &child,
				   COL_CAPTION, &caption, COL_COLOR, &color, -1);
		debug_dump_recursive(&child, depth + 1);
		g_free(caption); g_free(color);
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &child));
}

void bookmark_debug_dump_colors(void)
{
	GtkTreeIter root;
	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &root)) {
	}
	debug_dump_recursive(&root, 0);
}


gchar *bookmark_get_tag_color_for_key(const gchar *osiskey)
{
	GtkTreeIter folder, child;
	gchar *color = NULL;

	if (!osiskey || !model)
		return NULL;

	/* iterate top-level folders */
	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &folder))
		return NULL;

	do {
		gchar *folder_color = NULL;
		gtk_tree_model_get(GTK_TREE_MODEL(model), &folder,
				   COL_COLOR, &folder_color, -1);
		if (!folder_color || !*folder_color) {
			g_free(folder_color);
			continue;
		}
		/* scan children of this colored folder */
		if (gtk_tree_model_iter_children(GTK_TREE_MODEL(model),
						 &child, &folder)) {
			do {
				gchar *key = NULL;
				gtk_tree_model_get(GTK_TREE_MODEL(model),
						   &child,
						   COL_KEY, &key, -1);
				/* compare case-insensitively;
				 * also try stripping trailing spaces */
				gchar *k = key ? g_strstrip(g_strdup(key)) : NULL;
				gchar *q = osiskey ? g_strstrip(g_strdup(osiskey)) : NULL;
				gboolean match = (k && q && !g_ascii_strcasecmp(k, q));
				g_free(k); g_free(q);
				if (match) {
					color = g_strdup(folder_color);
					g_free(key);
					g_free(folder_color);
					return color;
				}
				g_free(key);
			} while (gtk_tree_model_iter_next(
					GTK_TREE_MODEL(model), &child));
		}
		g_free(folder_color);
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &folder));

	return NULL;
}

GtkWidget *gui_create_bookmark_tree(void)
{
	GtkWidget *tree;
	GtkTreeIter iter;

	gui_create_bookmark_menu();
	create_pixbufs();
	tree_model = create_model();
	tree = gtk_tree_view_new_with_model(tree_model);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

	gui_add_columns(GTK_TREE_VIEW(tree));
	gtk_tree_store_append(GTK_TREE_STORE(tree_model), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(tree_model), &iter,
			   COL_OPEN_PIXBUF, bm_pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, bm_pixbufs->pixbuf_closed,
			   COL_CAPTION, _("Bookmarks"),
			   COL_KEY, NULL, COL_MODULE, NULL, -1);

	load_xml_bookmarks(GTK_TREE_VIEW(tree), &iter);

	g_signal_connect_after(G_OBJECT(tree),
			       "button_release_event",
			       G_CALLBACK(button_release_event),
			       GINT_TO_POINTER(0));
	use_dialog = FALSE;
	bookmark_tree = GTK_TREE_VIEW(tree);
	gtk_tree_view_set_reorderable(bookmark_tree, TRUE);

	g_signal_connect(model, "row-changed",
			 G_CALLBACK(row_changed), NULL);
	g_signal_connect(model, "row-deleted",
			 G_CALLBACK(row_deleted), NULL);

	return tree;
}
