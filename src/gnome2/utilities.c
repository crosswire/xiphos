/*
 * Xiphos Bible Study Tool
 * utilities.c - support functions
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

#define _GNU_SOURCE 1

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gtk/gtk.h>

#include "gui/utilities.h"
#include "gui/preferences_dialog.h"
#include "gui/xiphos.h"
#include "gui/mod_mgr.h"
#include "gui/widgets.h"
#include "gui/dialog.h"

#include "main/lists.h"
#include "main/mod_mgr.h"
#include "main/settings.h"
#include "main/configs.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"

#ifdef WIN32
#undef DATADIR
#include <winsock2.h>
#include <windows.h>
#include <shellapi.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif /* !WIN32 */
#include <errno.h>

#include "xiphos_html/xiphos_html.h"

#include <gsf/gsf-utils.h>
#include <gsf/gsf-outfile-zip.h>
#include <gsf/gsf-output-stdio.h>
#include <gsf/gsf-output.h>
#include <gsf/gsf-input.h>
#include <gsf/gsf-outfile.h>
#include <gsf/gsf-input-stdio.h>

#include "gui/debug_glib_null.h"

/*
 * month names and lengths, for use in constructing journals and devotional references.
 */
int month_day_counts[] =
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char *(month_names[]) = {
    N_("January"),
    N_("February"),
    N_("March"),
    N_("April"),
    N_("May"),
    N_("June"),
    N_("July"),
    N_("August"),
    N_("September"), N_("October"), N_("November"), N_("December")};

/******************************************************************************
 * Name
 *   sync_windows
 *
 * Synopsis
 *   void sync_windows()
 *
 * Description
 *   forces GTK window updates iff the block flag is unset.
 *   this flag is only checked here; set/unset by others.
 *
 * Return value
 *   void
 */

gint stop_window_sync = 0;

void sync_windows()
{
	if (stop_window_sync == 0) {
		while (gtk_events_pending())
			gtk_main_iteration();
	}
}

/******************************************************************************
 * Name
 *  utilities_parse_treeview
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void utilities_parse_treeview(xmlNodePtr parent, GtkTreeIter * tree_parent)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void utilities_parse_treeview(xmlNodePtr parent, GtkTreeIter *tree_parent,
			      GtkTreeModel *model)
{
	static xmlNodePtr cur_node;
	GtkTreeIter child;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;

	gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &child,
				     tree_parent);

	do {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &child,
				   2, &caption,
				   3, &key,
				   4, &module,
				   5, &mod_desc, 6, &description, -1);
		if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model),
						  &child)) {
			cur_node = xml_add_folder_to_parent(parent,
							    caption);
			utilities_parse_treeview(cur_node, &child, model);

		} else
			xml_add_bookmark_to_parent(parent,
						   description,
						   key, module, mod_desc);

		g_free(caption);
		g_free(key);
		g_free(module);
		g_free(mod_desc);
		g_free(description);
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &child));
}

gint gui_of2tf(const gchar *on_off)
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

void gui_set_progressbar_text(GtkWidget *pb, gchar *text)
{
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pb), text);
	sync_windows();
}

void gui_set_statusbar(const gchar *message)
{
	static guint context_id = 0;
	gtk_statusbar_pop(GTK_STATUSBAR(widgets.appbar), context_id);
	context_id =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(widgets.appbar),
					 message);

	context_id =
	    gtk_statusbar_push(GTK_STATUSBAR(widgets.appbar),
			       context_id, message);
	XI_message(("context_id: %d\nmessage: %s", context_id, message));
}

void gui_set_progressbar_fraction(GtkWidget *pb, gdouble fraction)
{
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb), fraction);
	sync_windows();
}

void gui_set_combo_index(GtkWidget *combo, gint index)
{
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), index);
}

void gui_clear_combo(GtkWidget *combo)
{
	GtkTreeModel *model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
	gtk_list_store_clear(GTK_LIST_STORE(model));
}

void gui_add_item_to_combo(GtkWidget *combo, gchar *item)
{
	GtkTreeIter iter;
	GtkTreeModel *model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
	gtk_list_store_append(GTK_LIST_STORE(model), &iter);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, (gchar *)item,
			   -1);
}

/*
 * taken from galeon
 * glade_signal_connect_func: used by glade_xml_signal_autoconnect_full
 */
void gui_glade_signal_connect_func(const gchar *cb_name, GObject *obj,
				   const gchar *signal_name,
				   const gchar *signal_data,
				   GObject *conn_obj, gboolean conn_after,
				   gpointer user_data)
{
	/** Module with all the symbols of the program */
	static GModule *mod_self = NULL;
	gpointer handler_func;

	/* initialize gmodule */
	if (mod_self == NULL) {
		mod_self = g_module_open(NULL, 0);
		g_assert(mod_self != NULL);
	}

	/*g_print( "glade_signal_connect_func: cb_name = '%s', signal_name = '%s', signal_data = '%s'\n",
	   cb_name, signal_name, signal_data ); */

	if (g_module_symbol(mod_self, cb_name, &handler_func)) {
		/* found callback */
		if (conn_obj) {
			if (conn_after) {
				g_signal_connect_object(obj, signal_name,
							handler_func, conn_obj,
							G_CONNECT_AFTER);
			} else {
				g_signal_connect_object(obj, signal_name,
							handler_func, conn_obj,
							G_CONNECT_SWAPPED);
			}
		} else {
			/* no conn_obj; use standard connect */
			gpointer data = NULL;

			data = user_data;

			if (conn_after) {
				g_signal_connect_after(obj, signal_name, handler_func, data);
			} else {
				g_signal_connect(obj, signal_name, handler_func, data);
			}
		}
	} else {
		XI_warning(("callback function not found: %s", cb_name));
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
gchar *gui_general_user_file(const char *fname, gboolean critical)
{
	static GHashTable *already_found = NULL;
	gchar *alternative[10];
	gchar *file;
	gint i;

	/* create cache hash table if it doesnt already exist */
	if (already_found == NULL) {
		already_found =
		    g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
					  g_free);
	}

	/* Have we already found this? */
	file = g_hash_table_lookup(already_found, fname);
	if (file != NULL) {
		if (g_file_test(file, G_FILE_TEST_EXISTS)) {
			return g_strdup(file);
		} else {
			g_hash_table_remove(already_found, fname);
		}
	}

	/* try the default */
	file = g_build_filename(settings.gSwordDir, fname, NULL);

	/* success? */
	if (g_file_test(file, G_FILE_TEST_EXISTS)) {
		/* add it to the set of found files */
		g_hash_table_insert(already_found, g_strdup(fname),
				    g_strdup(file));
		return file;
	}
	g_free(file);

	/* specify alternate locations in order of precedence */
	i = 0;
	alternative[i++] = g_strdup(fname);
#ifdef DEBUG
	XI_message(("MAINTAINER_MODE"));
	/* generally only developers have any use for these */
	alternative[i++] = g_build_filename("..", fname, NULL);
	alternative[i++] = g_build_filename("ui", fname, NULL);
	alternative[i++] = g_build_filename("..", "ui", fname, NULL);
	alternative[i++] = g_build_filename("..", "..", "ui", fname, NULL);
#endif
#ifdef WIN32
	alternative[i++] =
	    g_build_filename(xiphos_win32_get_subdir("share"), "xiphos",
			     fname, NULL);
#else
	alternative[i++] =
	    g_build_filename("..", "share", "xiphos", fname, NULL);
#endif
	alternative[i++] = g_build_filename(SHARE_DIR, fname, NULL);
	alternative[i++] = NULL; /* NULL terminator needed */

	/* select one of the alternatives */
	file = NULL;

	for (i = 0; alternative[i] != NULL; i++) {
		if (file == NULL && g_file_test(alternative[i],
						G_FILE_TEST_IS_REGULAR)) {
			file = alternative[i];
		} else {
			/* free unused string */
			g_free(alternative[i]);
		}
	}

	/* check for success */
	if (file != NULL) {
		/* warn if we're using other than the install default */
		if (!g_path_is_absolute(file)) {
			gchar *cwd, *file1;
			XI_message(("Using %s (usually OK)", file));

			/* Make it absolute */
			cwd = g_get_current_dir();
			file1 = g_build_filename(cwd, file, NULL);
			g_free(file);
			g_free(cwd);
			file = file1;
		}

		/* add it to the set of found files */
		g_hash_table_insert(already_found, g_strdup(fname),
				    g_strdup(file));
	}
	/* if nothing then theres an error */
	else if (critical) {
		XI_error(("%s not found", fname));
	}

	/* return result */
	return file;
}

/******************************************************************************
 * Name
 *   language_add_folders
 *
 * Synopsis
 *   #include "main/sidebar.h"
 *
 *   void language_add_folders(GtkTreeModel * model, GtkTreeIter iter,
 *			       gchar ** languages)
 *
 * Description
 *   insert a block of languages into a tree model.
 *
 * Return value
 *   void
 */

static void
language_add_folders(GtkTreeModel *model,
		     GtkTreeIter iter, gchar **languages)
{
	GtkTreeIter iter_iter;
	GtkTreeIter child_iter;
	int j;

	(void)gtk_tree_model_iter_children(model, &iter_iter, &iter);
	for (j = 0; languages[j]; ++j) {
		gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter,
				      &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter, 0,
				   ((g_utf8_validate(languages[j], -1, NULL))
					? languages[j]
					: _("Unknown")),
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

	gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter, &iter);
	gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
			   0, module_name, -1);
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

static void add_module_to_language_folder(GtkTreeModel *model,
					  GtkTreeIter iter,
					  const gchar *language,
					  gchar *module_name,
					  gchar *description)
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

		gtk_tree_model_get(model, &iter_iter, 0, &str_data, -1);
		if (!strcmp(language, str_data)) {
			gchar *content;
			const gchar *abbreviation =
			    main_get_abbreviation(module_name);

			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter_iter);
			content = g_strdup_printf("%s: %s",
						  (abbreviation
						       ? abbreviation
						       : module_name),
						  description);
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, UTIL_COL_CAPTION,
					   (gchar *)content,
					   UTIL_COL_MODULE,
					   (gchar *)module_name, -1);
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

void gui_load_module_tree(GtkWidget *tree, gboolean limited)
{
	// GtkTreeIter should be an opaque type.
	// but because cppcheck(1) complains of uninit'd use,
	// we cheat & peek & assign the structs (perhaps pointlessly).
	GtkTreeIter text = { -1, NULL, NULL, NULL };
	GtkTreeIter commentary = { -1, NULL, NULL, NULL };
	GtkTreeIter dictionary = { -1, NULL, NULL, NULL };
	GtkTreeIter glossary = { -1, NULL, NULL, NULL };
	GtkTreeIter devotional = { -1, NULL, NULL, NULL };
	GtkTreeIter book = { -1, NULL, NULL, NULL };
	GtkTreeIter map = { -1, NULL, NULL, NULL };
	GtkTreeIter image = { -1, NULL, NULL, NULL };
	GtkTreeIter cult = { -1, NULL, NULL, NULL };
	GtkTreeIter prayerlist = { -1, NULL, NULL, NULL };
	GtkTreeStore *store;
	GList *tmp = NULL;
	GList *tmp2 = NULL;

	store =
	    gtk_tree_store_new(UTIL_N_COLUMNS, G_TYPE_STRING,
			       G_TYPE_STRING);
	gtk_tree_store_clear(store);

	/*  Biblical Texts folders */
	gtk_tree_store_append(store, &text, NULL);
	gtk_tree_store_set(store, &text,
			   UTIL_COL_CAPTION, _("Biblical Texts"),
			   UTIL_COL_MODULE, NULL, -1);

	/*  Commentaries folders */
	gtk_tree_store_append(store, &commentary, NULL);
	gtk_tree_store_set(store, &commentary,
			   UTIL_COL_CAPTION, _("Commentaries"),
			   UTIL_COL_MODULE, NULL, -1);

	if (!limited) {
		/*  Dictionaries folders */
		gtk_tree_store_append(store, &dictionary, NULL);
		gtk_tree_store_set(store, &dictionary,
				   UTIL_COL_CAPTION, _("Dictionaries"),
				   UTIL_COL_MODULE, NULL, -1);

		/*  Glossaries folders */
		gtk_tree_store_append(store, &glossary, NULL);
		gtk_tree_store_set(store, &glossary,
				   UTIL_COL_CAPTION, _("Glossaries"),
				   UTIL_COL_MODULE, NULL, -1);

		/*  Devotionals folders */
		gtk_tree_store_append(store, &devotional, NULL);
		gtk_tree_store_set(store, &devotional,
				   UTIL_COL_CAPTION,
				   _("Daily Devotionals"), UTIL_COL_MODULE,
				   NULL, -1);

		/*  General Books folders */
		gtk_tree_store_append(store, &book, NULL);
		gtk_tree_store_set(store, &book,
				   UTIL_COL_CAPTION, _("General Books"),
				   UTIL_COL_MODULE, NULL, -1);

		/*  Maps folders */
		gtk_tree_store_append(store, &map, NULL);
		gtk_tree_store_set(store, &map,
				   UTIL_COL_CAPTION, _("Maps"),
				   UTIL_COL_MODULE, NULL, -1);

		/*  Images folders */
		gtk_tree_store_append(store, &image, NULL);
		gtk_tree_store_set(store, &image,
				   UTIL_COL_CAPTION, _("Images"),
				   UTIL_COL_MODULE, NULL, -1);

		/*  Cult folders */
		gtk_tree_store_append(store, &cult, NULL);
		gtk_tree_store_set(store, &cult,
				   UTIL_COL_CAPTION, _("Cult/Unorthodox"),
				   UTIL_COL_MODULE, NULL, -1);

		/*  Prayer lists folder */
		if (settings.prayerlist) {
			gtk_tree_store_append(store, &prayerlist, NULL);
			gtk_tree_store_set(store, &prayerlist,
					   UTIL_COL_CAPTION,
					   _("Prayer List/Journal"),
					   UTIL_COL_MODULE, NULL, -1);
		}
	}

	tmp = mod_mgr_list_local_modules(settings.path_to_mods, TRUE);

	language_make_list(tmp, store,
			   text, commentary, map, image,
			   devotional, dictionary, glossary, book, cult,
			   NULL, NULL, language_add_folders, limited);

	tmp2 = tmp;
	while (tmp2 != NULL) {
		MOD_MGR *info = (MOD_MGR *)tmp2->data;

		/* see comment on similar code in src/main/sidebar.cc. */

		if ((info->type[0] == 'B') && !info->is_cult) {
			add_module_to_language_folder(GTK_TREE_MODEL(store), text,
						      info->language,
						      info->name,
						      info->description);
		} else if ((info->type[0] == 'C') && !info->is_cult) {
			add_module_to_language_folder(GTK_TREE_MODEL(store), commentary,
						      info->language,
						      info->name,
						      info->description);
		} else if (!limited) {
			if (info->is_cult) {
				add_module_to_language_folder(GTK_TREE_MODEL(store), cult,
							      info->language, info->name,
							      info->description);
			} else if (info->is_maps) {
				add_module_to_language_folder(GTK_TREE_MODEL(store), map,
							      info->language, info->name,
							      info->description);
			} else if (info->is_images) {
				add_module_to_language_folder(GTK_TREE_MODEL(store), image,
							      info->language, info->name,
							      info->description);
			} else if (info->is_devotional) {
				add_module_to_language_folder(GTK_TREE_MODEL(store), devotional,
							      info->language, info->name,
							      info->description);
			} else if (info->is_glossary) {
				add_module_to_language_folder(GTK_TREE_MODEL(store), glossary,
							      info->language, info->name,
							      info->description);
			} else if (info->type[0] == 'L') {
				add_module_to_language_folder(GTK_TREE_MODEL(store), dictionary,
							      info->language, info->name,
							      info->description);
			} else if (info->type[0] == 'G') {
				gchar *gstype =
				    main_get_mod_config_entry(info->name,
							      "GSType");
				if ((gstype == NULL) || strcmp(gstype, "PrayerList")) {
					add_module_to_language_folder(GTK_TREE_MODEL(store), book,
								      info->language, info->name,
								      info->description);
				}
			} else {
				XI_warning(("mod `%s' unknown type `%s'",
					    info->name, info->type));
			}
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
	if (!limited && settings.prayerlist) {
		tmp = get_list(PRAYER_LIST);
		while (tmp != NULL) {
			add_module_to_prayerlist_folder(GTK_TREE_MODEL(store),
							prayerlist,
							(gchar *)
							tmp->data);
			tmp = g_list_next(tmp);
		}
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

MOD_FONT *get_font(const gchar *mod_name)
{
	MOD_FONT *mf;
	static gchar *file = NULL;

	if (file == NULL)
		file = g_strdup_printf("%s/fonts.conf", settings.gSwordDir); /* not freed */

	mf = g_new0(MOD_FONT, 1);
	mf->mod_name = mod_name;

	mf->old_font = get_conf_file_item(file, mod_name, "Font");
	mf->old_font_size = get_conf_file_item(file, mod_name, "Fontsize");

	/* 1st try: module pref */
	if ((mf->old_font == NULL) || !strcmp(mf->old_font, "none")) {
		/* in absence of module pref, user can specify language pref */
		gchar *lang = main_get_mod_config_entry(mod_name, "Lang");
		gchar *lang_lang =
		    g_strdup_printf("Language:%s", (lang ? lang : ""));
		g_free(lang);
		gchar *lang_font =
		    get_conf_file_item(file, lang_lang, "Font");
		gchar *lang_size =
		    get_conf_file_item(file, lang_lang, "Fontsize");
		g_free(lang_lang);

		/* in absence of any pref, module can name its pref */
		gchar *preferred_font =
		    main_get_mod_config_entry(mod_name, "Font");
		gchar *preferred_size =
		    main_get_mod_config_entry(mod_name, "Fontsize");

		g_free(mf->old_font);

		/* 2nd try: per-language pref */
		if (lang_font && (*lang_font != '\0')) {
			g_free(preferred_font);
			mf->old_font = lang_font;
		} else {
			g_free(lang_font);
			/* 3rd try: module config pref */
			if (preferred_font && (*preferred_font != '\0'))
				mf->old_font = preferred_font;
			else {
				/* nothing ever specified: utter default */
				g_free(preferred_font);
				mf->old_font = g_strdup("none");
			}
		}

		/* 1st try */
		if ((mf->old_font_size == NULL) ||
		    !strcmp(mf->old_font_size, "+0")) {
			g_free(mf->old_font_size);

			/* 2nd try */
			if (lang_size && (*lang_size != '\0')) {
				g_free(preferred_size);
				mf->old_font_size = lang_size;
			} else {
				/* 3rd try */
				g_free(lang_size);
				if (preferred_size && (*preferred_size != '\0'))
					mf->old_font_size = preferred_size;
				else {
					/* utter default */
					g_free(preferred_size);
					mf->old_font_size = g_strdup("+0");
				}
			}
		}
	}

	/* convert string -> int, here, once for all. includes base bias. */
	mf->old_font_size_value = ((mf->old_font_size)
				       ? atoi(mf->old_font_size) +
					     settings.base_font_size
				       : settings.base_font_size);

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

	if (mf->old_font)
		g_free(mf->old_font);
	if (mf->old_font_size)
		g_free(mf->old_font_size);
	g_free(mf);
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
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

gchar *remove_linefeeds(gchar *buf)
{
	gchar *key = NULL;

	key = g_strdelimit(buf, "\n", ' ');

	return (key ? g_strdup(key) : NULL);
}
#endif

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

void gui_add_mods_2_gtk_menu(gint mod_type, GtkWidget *menu,
			     GCallback callback)
{
	GList *tmp = NULL;

	if (mod_type == -1)
		return;

	tmp = get_list(mod_type);
	while (tmp != NULL) {
		GtkWidget *item = gtk_menu_item_new_with_label((gchar *)tmp->data);
		gtk_widget_show(item);
		g_signal_connect(G_OBJECT(item), "activate",
				 G_CALLBACK(callback), (gchar *)tmp->data);
		gtk_container_add(GTK_CONTAINER(menu), item);
		tmp = g_list_next(tmp);
	}
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
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

gchar *ncr_to_utf8(gchar *text)
{
	gchar *ncr;
	gunichar unicode;
	gchar utf8[7];
	guint len;
	GString *newtext = g_string_new(NULL);

	for (ncr = strstr(text, "&#");
	     ncr; text = ncr, ncr = strstr(text, "&#")) {
		newtext = g_string_append_len(newtext, text, ncr - text);

		// convert ncr value (string) to unicode (guint32)
		unicode = 0;
		for (ncr = ncr + 2; // just past "&#"
		     (*ncr != '\0') && (*ncr >= '0') && (*ncr <= '9');
		     ncr++)
			unicode = (unicode * 10) + ((*ncr) - '0');

		// converts unicode char to utf8
		// need proper terminator + validation of content
		if ((*ncr == ';') && g_unichar_validate(unicode)) {
			ncr++; // step past ';'
			len = g_unichar_to_utf8(unicode, utf8);
			utf8[len] = '\0';
			newtext = g_string_append(newtext, utf8);
		} else {
			g_string_append_printf(newtext, "&#%d;", unicode);
			XI_message(("ncr2utf8: invalid unicode &#%d;\n",
				    unicode));
		}
	}

	if (*text != '\0') // residual text?  paste it on.
		newtext = g_string_append(newtext, text);

	return g_string_free(newtext, FALSE);
}
#endif

//
// for choosing variants, primary/secondary/all.
//
void reading_selector(char *modname,
		      char *key,
		      DIALOG_DATA *dialog,
		      GtkMenuItem *menuitem, gpointer user_data)
{
	gchar *url;
	gboolean primary = 0, secondary = 0, all = 0;
	gboolean is_dialog = (dialog ? 1 : 0);

	switch ((int)GPOINTER_TO_INT(user_data)) {
	case 0:
		primary = TRUE;
		secondary = FALSE;
		all = FALSE;
		break;
	case 1:
		primary = FALSE;
		secondary = TRUE;
		all = FALSE;
		break;
	case 2:
		primary = FALSE;
		secondary = FALSE;
		all = TRUE;
		break;
	default:
		g_message("invalid variant %d\n",
			  (int)GPOINTER_TO_INT(user_data));
		gui_generic_warning("Xiphos: invalid internal variant");
		break;
	}

	url = g_strdup_printf("sword://%s/%s", modname, key);
	main_save_module_options(modname, "Primary Reading", primary);
	main_save_module_options(modname, "Secondary Reading", secondary);
	main_save_module_options(modname, "All Readings", all);
	if (is_dialog)
		main_dialogs_url_handler(dialog, url, TRUE);
	else
		main_url_handler(url, TRUE);

	g_free(url);
}

/* **************************************************************** */
/* language sorting functions, for better module list displays.     */
/* **************************************************************** */

struct language_set
{
	gchar **ptr;
	int count;
	int max;
} language_set[N_LANGSET_MODTYPES];

/* one-shot setup during initialization. */
void language_init(void)
{
	int i;
	for (i = 0; i < N_LANGSET_MODTYPES; ++i) {
		language_set[i].ptr =
		    calloc(LANGSET_STRIDE + 1, sizeof(char *));
		language_set[i].count = 0;
		language_set[i].max = LANGSET_STRIDE;	/* initial limit */
	}
}

/* per-usage clear of old content before a new collection is made. */
static void language_clear()
{
	int i, j;

	for (i = 0; i < N_LANGSET_MODTYPES; ++i) {
		char **s = language_set[i].ptr;
		for (j = 0; j < language_set[i].count; ++j) {
			g_free(s[j]);
			s[j] = NULL;
		}
		language_set[i].count = 0;
	}
}

/* used during module set analysis: add one language to a per-type set. */
static void language_add(const char *language, int module_type)
{
	int j;
	char **s;

	if ((language == NULL) || (*language == '\0'))
		language = _("Unknown");
	assert((module_type >= 0) && (module_type < N_LANGSET_MODTYPES));

	s = language_set[module_type].ptr;
	for (j = 0; j < language_set[module_type].count; ++j) {
		if (!strcmp(s[j], language))
			return; /* found -- duplicate. */
	}

	// grow the set dynamically as we run out of space.
	if (j >= language_set[module_type].max) {
		int old_max = language_set[module_type].max;

		language_set[module_type].max += LANGSET_STRIDE;
		s = language_set[module_type].ptr =
			realloc(language_set[module_type].ptr,
				(language_set[module_type].max + 1) *
				sizeof(char*));
		// realloc() does not set newly-alloc'd mem to zeroes.
		memset(s + old_max, '\0', (LANGSET_STRIDE + 1) * sizeof(char*));
	}

	s[j] = g_strdup(language);
	++(language_set[module_type].count);
}

/* comparator function pulled verbatim from qsort(3) */
/* but with function changed to ucol_strcollUTF8(3). */
/* http://icu-project.org/apiref/icu4c/ucol_8h.html  */
static int cmpstringp(const void *p1, const void *p2)
{
	/* The actual arguments to this function are "pointers to
	   pointers to char", but strcmp(3) arguments are "pointers
	   to char", hence the following cast plus dereference */

	return ucol_strcollUTF8(collator,
				*(char *const *)p1, -1,
				*(char *const *)p2, -1,
				&collator_status);
}

/* retrieve the language set specific to the module type requested */
static gchar **language_get_type(int module_type)
{
	assert((module_type >= 0) && (module_type < N_LANGSET_MODTYPES));
	return language_set[module_type].ptr;
}

/* push a list of modules' sorted languages into the specified tree store. */
/* other than init, this is the only public function. */
void
language_make_list(GList *modlist,
		   GtkTreeStore *store,
		   GtkTreeIter text,
		   GtkTreeIter commentary,
		   GtkTreeIter map,
		   GtkTreeIter image,
		   GtkTreeIter devotional,
		   GtkTreeIter dictionary,
		   GtkTreeIter glossary,
		   GtkTreeIter book,
		   GtkTreeIter cult,
		   GtkTreeIter *update,
		   GtkTreeIter *uninstalled,
		   void (*add)(GtkTreeModel *, GtkTreeIter, gchar **),
		   gboolean limited)
{
	int i;

	language_clear();

	/* append */
	while (modlist != NULL) {
		MOD_MGR *info = (MOD_MGR *)modlist->data;

		/* mod.mgr: special extra lists */
		if ((update != NULL) && (uninstalled != NULL)) {
			if (!info->installed) {
				language_add(info->language,
					     LANGSET_UNINSTALLED);
			} else if ((!info->old_version && info->new_version && strcmp(info->new_version, " ")) || (info->old_version && !info->new_version) || (info->old_version && info->new_version && strcmp(info->new_version, info->old_version) > 0)) {
				language_add(info->language,
					     LANGSET_UPDATE);
			}
		}

		/* modtype analysis identical to add_to_folder calls. */
		if (info->is_cult)
			language_add(info->language, LANGSET_CULT);
		else if (info->type[0] == 'B')
			language_add(info->language, LANGSET_BIBLE);
		else if (info->type[0] == 'C')
			language_add(info->language, LANGSET_COMMENTARY);
		else if (info->is_maps)
			language_add(info->language, LANGSET_MAP);
		else if (info->is_images)
			language_add(info->language, LANGSET_IMAGE);
		else if (info->is_devotional)
			language_add(info->language, LANGSET_DEVOTIONAL);
		else if (info->is_glossary)
			language_add(info->language, LANGSET_GLOSSARY);
		else if (info->type[0] == 'L')
			language_add(info->language, LANGSET_DICTIONARY);
		else if ((info->type[0] == 'G') &&
			 (!main_get_mod_config_entry(info->name, "GSType")))
			language_add(info->language, LANGSET_GENBOOK);

		modlist = g_list_next(modlist);
	}

	/* self-defense, prior to qsort(3). */
	/* if this is 1st run, then we have not yet init'd the sword engine,
	 * in turn we do not have sword_locale or collator. we need collator
	 * in order to use qsort(3). it's a gross, unreliable hack, but we
	 * will simply use $LANG in this one instance, un-massaged by the
	 * more correct set_sword_locale().
	 */
	if (!collator) {
		char *locale = getenv("LANG");
		collator = ucol_open((locale ? locale : ""), &collator_status);
	}

	/* sort */
	for (i = 0; i < N_LANGSET_MODTYPES; ++i) {
		qsort(language_set[i].ptr,
		      language_set[i].count, sizeof(char *), cmpstringp);
	}

	/* generate tree. */
	(*add)(GTK_TREE_MODEL(store), text,
	       language_get_type(LANGSET_BIBLE));
	(*add)(GTK_TREE_MODEL(store), commentary,
	       language_get_type(LANGSET_COMMENTARY));
	if (!limited) {
		(*add)(GTK_TREE_MODEL(store), map,
		       language_get_type(LANGSET_MAP));
		(*add)(GTK_TREE_MODEL(store), image,
		       language_get_type(LANGSET_IMAGE));
		(*add)(GTK_TREE_MODEL(store), cult,
		       language_get_type(LANGSET_CULT));
		(*add)(GTK_TREE_MODEL(store), devotional,
		       language_get_type(LANGSET_DEVOTIONAL));
		(*add)(GTK_TREE_MODEL(store), glossary,
		       language_get_type(LANGSET_GLOSSARY));
		(*add)(GTK_TREE_MODEL(store), dictionary,
		       language_get_type(LANGSET_DICTIONARY));
		(*add)(GTK_TREE_MODEL(store), book,
		       language_get_type(LANGSET_GENBOOK));
		if ((update != NULL) && (uninstalled != NULL)) {
			(*add)(GTK_TREE_MODEL(store), *update,
			       language_get_type(LANGSET_UPDATE));
			(*add)(GTK_TREE_MODEL(store), *uninstalled,
			       language_get_type(LANGSET_UNINSTALLED));
		}
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

gchar *get_modlist_string(GList *mods)
{
	gchar *rv;
	GString *str = g_string_new("");
	GList *orig_mods = mods;

	while (mods != NULL) {
		str = g_string_append(str, (gchar *)mods->data);
		g_free(mods->data);
		mods = g_list_next(mods);
		if (mods != NULL)
			str = g_string_append_c(str, ',');
	}
	g_list_free(orig_mods);

	rv = g_strdup(str->str);
	g_string_free(str, TRUE);
	return rv;
}

/******************************************************************************
 * Name
 *   get_current_list
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   GList *get_current_list(GtkTreeView *treeview)
 *
 * Description
 *
 *
 * Return value
 *   GList *
 */

GList *get_current_list(GtkTreeView *treeview)
{
	GList *items = NULL;
	gchar *buf;
	GtkTreeModel *model;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model(treeview);
	if (gtk_tree_model_get_iter_first(model, &iter)) {
		do {
			gtk_tree_model_get(model, &iter, 1, &buf, -1);
			items =
			    g_list_append(items, (gchar *)g_strdup(buf));

		} while (gtk_tree_model_iter_next(model, &iter));
	}
	return items;
}

/*
 * caller must free the returned string.
 */
char *image_locator(const char *image)
{
#ifndef WIN32
	return g_strdup_printf("%s/%s", PACKAGE_PIXMAPS_DIR, image);
#else
	return g_build_filename(xiphos_win32_get_subdir("share"),
				"xiphos", image, NULL);
#endif /* WIN32 */
}

/*
 * get a pixmap widget from specified file.
 */
GtkWidget *pixmap_finder(char *image)
{
	GtkWidget *w;
	char *image_file;

	image_file = image_locator(image);
	w = gtk_image_new_from_file(image_file);
	g_free(image_file);
	return w;
}

/*
 * get a pixbuf from specified file.
 */
GdkPixbuf *pixbuf_finder(const char *image, int size, GError **error)
{
	GdkPixbuf *p;
	char *image_file;

	image_file = image_locator(image);
	if (size == 0)
		p = gdk_pixbuf_new_from_file(image_file, error);
	else
		p = gdk_pixbuf_new_from_file_at_scale(image_file,
						      size, size,
						      TRUE, error);
	g_free(image_file);
	return p;
}

//
// utility function to write out HTML.
//
#ifdef min
#undef min
#endif
#define min(x, y) ((x) < (y) ? (x) : (y))

void
HtmlOutput(char *text, GtkWidget *gtkText, MOD_FONT *mf, char *anchor)
{
	int len = strlen(text);

	XiphosHtml *html = XIPHOS_HTML(gtkText);
	XIPHOS_HTML_OPEN_STREAM(html, "text/html");

	// EVIL EVIL EVIL EVIL.
	// crazy nonsense with xulrunner 1.9.2.3, failure to jump to anchor.
	// force the issue by stuffing a javascript snippet inside <head></head>.
	// there are forms of evil so dark that they should not be contemplated.
	if (anchor || settings.special_anchor) {
		gchar *buf;

		// first, scribble out everything up to the closing </head>.
		buf = strstr(text, "</head>"); // yes, lowercase.
		assert(buf != NULL);	   // don't be so stupid as not to include <head></head>.
		int offset = buf - text;
		XIPHOS_HTML_WRITE(html, text, offset);
		len -= offset;

		// now write the javascript snippet.
		buf =
		    g_strdup_printf("<script type=\"text/javascript\" language=\"javascript\">"
				    " window.onload = function () { window.location.hash = \"%s\"; }"
				    " </script>",
				    (settings.special_anchor ? settings.special_anchor : anchor));
		XIPHOS_HTML_WRITE(html, buf, strlen(buf));
		g_free(buf);
	}

	if (!anchor)
		XIPHOS_HTML_WRITE(html, text, len);

	/* use anchor if asked, but if so, special anchor takes priority. */
	XIPHOS_HTML_CLOSE(html);
	if (anchor || settings.special_anchor)
		XIPHOS_HTML_JUMP_TO_ANCHOR(html, (settings.special_anchor
						      ? settings.special_anchor
						      : anchor));

	/* the special anchor gets exactly one use. */
	settings.special_anchor = NULL;
}

void set_window_icon(GtkWindow *window)
{
	gchar *imagename;
	GdkPixbuf *pixbuf;

	imagename = image_locator("xiphos-x-16.png");
	pixbuf = gdk_pixbuf_new_from_file(imagename, NULL);
	g_free(imagename);
	gtk_window_set_icon(window, pixbuf);
}

/**************************************************************************
 * Name
 *  xiphos_open_default
 *
 * Synopsis
 *  #include "glib.h"
 *  #include <gnome.h>
 *  #include <windows.h>
 *  #include <shellapi.h>
 *
 * xiphos_open_default (gchar *file)
 *
 * Description
 *  this is a generic "open anything with the default program" function;
 *  on Windows, it uses the ShellExecuteW API (more robust than the gnome
 *  or gtk implementations on Windows)
 *  on *nix, it uses gnome_url_show; in the future, this could be changed
 *  to gtk_show_uri as gnome_url_show is deprecated
 *
 * Return value
 *   void
 */
gboolean xiphos_open_default(const gchar *file)
{
#ifdef WIN32
	gunichar2 *w_file;
	gint rt;
	w_file = g_utf8_to_utf16(file, -1, NULL, NULL, NULL);
	XI_message(("opening file %ls", w_file));
	rt = (gint)ShellExecuteW(NULL, L"open", w_file, NULL, NULL,
				 SW_SHOWDEFAULT);
	return rt > 32;

#else
	GError *error = NULL;
#if GTK_CHECK_VERSION(3, 22, 0)
	gtk_show_uri_on_window(NULL, file, gtk_get_current_event_time(), &error);
#else
	gtk_show_uri(NULL, file, gtk_get_current_event_time(), &error);*/
#endif
	if (error != NULL) {
		XI_warning(("%s", error->message));
		g_error_free(error);
		return FALSE;
	} else
		return TRUE;
#endif
}

/**
 * xiphos_win32_get_subdir
 * @subdir: a #gchar, the name of the subdir desired
 *
 * get the full path of the subdir; for instance, 'bin'
 * would return something like C:/Program Files/Crosswire/Xiphos/bin
 *
 * Since 3.1.2
 */
#ifdef WIN32
gchar *xiphos_win32_get_subdir(const gchar *subdir)
{
	gchar *ret_dir =
	    g_win32_get_package_installation_directory_of_module(NULL);
	ret_dir = g_strconcat(ret_dir, "\0", NULL);
	ret_dir = g_build_filename(ret_dir, subdir, NULL);
	return ret_dir;
}
#endif

/**
 * archive_addfile
 * @output: a #GsfOutfile, the parent of the desired child
 * @file: a #gchar, the on disk path of the file to add
 * @name: a #gchar, the name (without path) of the file to add
 *
 * add a single file to the #GsfOutfile
 *
 * Since 3.1.2
 */
void archive_addfile(GsfOutfile *output, const gchar *file,
		     const gchar *name)
{
	GsfInput *inputchild = NULL;
	GsfOutput *outputchild = NULL;

	inputchild = GSF_INPUT(gsf_input_stdio_new(file, NULL));
	outputchild = gsf_outfile_new_child(output, name, FALSE);
	gsf_input_copy(inputchild, outputchild);
	gsf_output_close(outputchild);
	g_object_unref(inputchild);
	g_object_unref(outputchild);
}

/**
 * archive_adddir
 * @output: a #GsfOutfile, the parent of the desired child
 * @path: a #gchar, the on disk path of the desired child directory
 * @name: a #gchar, the name (without path) of the desired child dir
 *
 * add a child directory and all subdirs and files to the GsfOutfile;
 * recursive
 *
 * Since 3.1.2
 */
void archive_adddir(GsfOutfile *output, gchar *path, const gchar *name)
{
	GDir *dir;
	const gchar *file;
	GsfOutfile *child;

	child = GSF_OUTFILE(gsf_outfile_new_child(output, name, TRUE));
	dir = g_dir_open(path, 0, NULL);
	while ((file = g_dir_read_name(dir))) {
		gchar *complete_file = g_build_filename(path, file, NULL);
		if (g_file_test(complete_file, G_FILE_TEST_IS_DIR))
			archive_adddir(GSF_OUTFILE(child), complete_file,
				       file);
		else
			archive_addfile(GSF_OUTFILE(child), complete_file,
					file);
		g_free(complete_file);
	}
	g_dir_close(dir);
}

/**
 * xiphos_create_archive
 * @conf_file: a #gchar with the location of the module's .conf
 * @datapath: a #gchar with the location of the module's data
 * @zip: a #gchar with the name of the zip file
 * @destination: #gchar with the location to store the zip
 *
 * archive the specified module in the specified location
 *
 * Since 3.1.2
 */
void xiphos_create_archive(gchar *conf_file, gchar *datapath,
			   gchar *zip, const gchar *destination)
{
	GsfOutfile *outputfile;
	GsfOutput *output;
	GsfOutfile *tmp;
	gchar **path;
	gchar *moddirname;
	GString *modpath;
	int i;

	/* don't confuse gsf by init'ing more than once. */
	static gboolean have_init_gsf = FALSE;
	if (!have_init_gsf) {
		gsf_init();
		have_init_gsf = TRUE;
	}

	/* the module dir is the last part of the path */
	path = g_strsplit(datapath, "/", -1);
	moddirname = g_strdup(path[g_strv_length(path) - 1]);

	/* create a relative path without preceding "." */
	/* or the trailing dir name */
	modpath = g_string_new(NULL);
	for (i = 1; i <= (g_strv_length(path) - 2); i++) {
		modpath = g_string_append(modpath, path[i]);
		if (i < g_strv_length(path) - 2) /* avoid a trailing / */
			modpath = g_string_append(modpath, "/");
	}

	/* open zip file for writing */
	output = gsf_output_stdio_new(zip, NULL);
	outputfile = gsf_outfile_zip_new(output, NULL);

	/* add the module directory */
	tmp =
	    GSF_OUTFILE(gsf_outfile_new_child(outputfile, modpath->str, TRUE));

	/* add all data files */
	archive_adddir(tmp,
		       g_build_filename(destination, datapath, NULL),
		       moddirname);
	gsf_output_close(GSF_OUTPUT(tmp));
	g_object_unref(tmp);

	/* add conf file */
	tmp =
	    GSF_OUTFILE(gsf_outfile_new_child(outputfile, "mods.d", TRUE));
	archive_addfile(tmp,
			g_build_filename(destination, "mods.d", conf_file,
					 NULL),
			conf_file);

	/* cleanup */
	gsf_output_close(GSF_OUTPUT(outputfile));
	gsf_output_close(output);
	g_object_unref(tmp);
	g_object_unref(outputfile);
	g_object_unref(output);
	g_strfreev(path);
	g_free(moddirname);
}

//
// Read aloud some text, i.e. the current verse.
// Text is cleaned of '"', <tokens>, "&symbols;", and *n/*x strings,
// then scribbled out the local static socket with (SayText "...").
// Non-zero verse param is prefixed onto supplied text.
//

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#else
#ifdef WIN32
#undef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif
#endif

void ReadAloud(unsigned int verse, const char *suppliedtext)
{
	static int tts_socket = INVALID_SOCKET; // no initial connection.
	static int use_counter = -2;		// to shortcircuit early uses.

	if (settings.readaloud || // read anything, or
	    (verse == 0)) {       // read what's handed us.
		gchar *s, *t;

		// setup for communication.
		if (tts_socket < 0) {
			struct sockaddr_in service;

			if ((tts_socket =
				 socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				char msg[256];
				sprintf(msg,
					"ReadAloud disabled:\nsocket failed, %s",
					strerror(errno));
				settings.readaloud = 0;
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.readaloud_item),
							       settings.readaloud);
				gui_generic_warning(msg);
				return;
			}
			// festival's port (1314) on localhost (127.0.0.1).
			memset(&service, 0, sizeof service);
			service.sin_family = AF_INET;
			service.sin_port = htons(1314);
			service.sin_addr.s_addr = htonl(0x7f000001);
			if (connect(tts_socket,
				    (const struct sockaddr *)&service,
				    sizeof(service)) != 0) {
				StartFestival();
#ifdef WIN32
				Sleep(2); // give festival a moment to init.
#else
				sleep(2); // give festival a moment to init.
#endif
				if (connect(tts_socket,
					    (const struct sockaddr *)&service,
					    sizeof(service)) != 0) {
					// it still didn't work -- missing.
					char msg[256];
					sprintf(msg, "%s\n%s, %s",
						"TTS \"festival\" not started -- perhaps not installed",
						"TTS connect failed",
						strerror(errno));
					StopFestival(&tts_socket);
					settings.readaloud = 0;
					gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.readaloud_item),
								       settings.readaloud);
					gui_generic_warning(msg);
					return;
				}
			}
		}
		// avoid speaking the first *2* times.
		// (2 Display() calls are made during startup.)
		// though speaking may be intended, startup speech is annoying.
		if (verse && (++use_counter < 1))
			return;

		GString *text = g_string_new(NULL);
		if ((settings.showversenum) && (verse != 0))
			g_string_printf(text, "%d. ...  %s", verse,
					suppliedtext);
		// use of ". ..." is to induce proper pauses.
		else
			g_string_printf(text, "%s", suppliedtext);
		XI_message(("ReadAloud: dirty: %s\n", text->str));

		// clean: no <span> surrounding strongs/morph.
		// i wish there was a regexp form of strstr().
		for (s = strstr(text->str, "<span class=\"strongs\">");
		     s; s = strstr(s, "<span class=\"strongs\">")) {
			if ((t = strstr(s, "</span>"))) {
				t += 6;
				while (s <= t)
					*(s++) = ' ';
			} else {
				XI_message(("ReadAloud: Unmatched <span strong></span> in %s\n", s));
				goto out;
			}
		}
		for (s = strstr(text->str, "<span class=\"morph\">");
		     s; s = strstr(s, "<span class=\"morph\">")) {
			if ((t = strstr(s, "</span>"))) {
				t += 6;
				while (s <= t)
					*(s++) = ' ';
			} else {
				XI_message(("ReadAloud: Unmatched <span morph></span> in %s\n", s));
				goto out;
			}
		}

		// clean: no quotes (conflict w/festival syntax).
		for (s = strchr(text->str, '"'); s; s = strchr(s, '"'))
			*s = ' ';

		// clean: no <tokens>.
		for (s = strchr(text->str, '<'); s; s = strchr(s, '<')) {
			if ((t = strchr(s, '>'))) {
				while (s <= t)
					*(s++) = ' ';
			} else {
				XI_message(("ReadAloud: Unmatched <> in %s\n", s));
				goto out;
			}
		}

		// clean: no &lt;...&gt; sequences.  (Strong's ref, "<1234>".)
		for (s = strstr(text->str, "&lt;"); s;
		     s = strstr(s, "&lt;")) {
			if ((t = strstr(s, "&gt;"))) {
				t += 3;
				while (s <= t)
					*(s++) = ' ';
			} else {
				XI_message(("ReadAloud: Unmatched &lt;&gt; in %s\n", s));
				goto out;
			}
		}

		// clean: no unicode apostrophes and quotation marks.
		for (s = strstr(text->str, "\342\200\231"); s;
		     s = strstr(s, "\342\200\231")) {
			*(s++) = '\'';
			*(s++) = '\'';
			*(s++) = '\'';
		}
		for (s = strstr(text->str, "\342\200\234"); s;
		     s = strstr(s, "\342\200\234")) {
			*(s++) = ' ';
			*(s++) = ' ';
			*(s++) = ' ';
		}
		for (s = strstr(text->str, "\342\200\235"); s;
		     s = strstr(s, "\342\200\235")) {
			*(s++) = ' ';
			*(s++) = ' ';
			*(s++) = ' ';
		}

		// clean: no other &symbols;.
		for (s = strchr(text->str, '&'); s; s = strchr(s, '&')) {
			if ((t = strchr(s, ';'))) {
				while (s <= t)
					*(s++) = ' ';
			} else {
				XI_message(("ReadAloud: Incomplete &xxx; in %s\n", s));
				goto out;
			}
		}

		// clean: no note/xref strings.
		for (s = strstr(text->str, "*n"); s; s = strstr(s, "*n")) {
			*(s++) = ' ';
			*(s++) = ' ';
			while (isdigit(*s))
				*(s++) = ' ';
		}
		for (s = strstr(text->str, "*u"); s; s = strstr(s, "*u")) {
			*(s++) = ' ';
			*(s++) = ' ';
		}
		for (s = strstr(text->str, "*x"); s; s = strstr(s, "*x")) {
			*(s++) = ' ';
			*(s++) = ' ';
			while (isdigit(*s))
				*(s++) = ' ';
		}

		// festival *pronounces* brackets and asterisks -- idiots.
		for (s = strchr(text->str, '['); s; s = strchr(s, '['))
			*s = ' ';
		for (s = strchr(text->str, ']'); s; s = strchr(s, ']'))
			*s = ' ';
		for (s = strchr(text->str, '*'); s; s = strchr(s, '*'))
			*s = ' ';
		// in case it isn't obvious, i'd really like a  standard
		// function that walks a string for multiple individual chars.

		// walk the string, looking for dislocated "LORD" as "L<spaces>ORD".
		// this occurs in "smallcaps" use in many bibles.
		for (s = strchr(text->str, 'L'); s; s = strchr(s + 1, 'L')) {
			gchar *begin = s++;
			while (*s == ' ')
				++s;
			if (!strncmp(s, "ORD", 3)) {
				*begin = ' ';
				*(s - 1) = 'L';
			}
		}
		// same song, second verse: G<spaces>OD.
		for (s = strchr(text->str, 'G'); s; s = strchr(s + 1, 'G')) {
			gchar *begin = s++;
			while (*s == ' ')
				++s;
			if (!strncmp(s, "OD", 2)) {
				*begin = ' ';
				*(s - 1) = 'G';
			}
		}

		XI_message(("ReadAloud: clean: %s\n", text->str));
		// scribble clean text to the socket.
		if (FestivalSpeak(text->str, strlen(text->str), tts_socket) == FALSE) {
			char msg[256];
			sprintf(msg,
				"TTS disappeared?\nTTS write failed: %s",
				strerror(errno));
			StopFestival(&tts_socket);
			settings.readaloud = 0;
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.readaloud_item),
						       settings.readaloud);
			gui_generic_warning(msg);
		}

	out:
		g_string_free(text, TRUE);
		return;

	} else {

		// Reading aloud is disabled.
		// If we had been reading, shut it down.
		if (tts_socket >= 0) {
			StopFestival(&tts_socket);
		}
		use_counter++;
		return;
	}
}

//
// starts festival in a async process
//
void StartFestival(void)
{
#ifdef WIN32
	//on windows, we will ship festival directly under Xiphos
	gchar *festival_args[5];
	gchar *festival_com =
	    g_win32_get_package_installation_directory_of_module(NULL);
	festival_com = g_strconcat(festival_com, "\0", NULL);
	gchar *festival_lib =
	    g_build_filename(festival_com, "festival\\lib\0");
	festival_com =
	    g_build_filename(festival_com, "festival\\festival.exe\0");
	festival_args[0] = festival_com;
	festival_args[1] = g_strdup("--libdir");
	festival_args[2] = festival_lib;
	festival_args[3] = g_strdup("--server");
	festival_args[4] = NULL;
#else
	gchar *festival_args[3];
	festival_args[0] = g_strdup("festival");
	festival_args[1] = g_strdup("--server");
	festival_args[2] = NULL;
#endif
	g_spawn_async(NULL,
		      festival_args,
		      NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL);
}

//
// shuts down Festival
//
void StopFestival(int *tts_socket)
{
#ifdef WIN32
	closesocket(*tts_socket);
#else
	shutdown(*tts_socket, SHUT_RDWR);
	close(*tts_socket);
#endif
	*tts_socket = INVALID_SOCKET;
}

//
// tells Festival to say the given text
//
gboolean FestivalSpeak(gchar *text, int length, int tts_socket)
{
#ifdef WIN32
	if ((send(tts_socket, "(SayText \"", 10, MSG_DONTROUTE) ==
	     INVALID_SOCKET) ||
	    (send(tts_socket, text, length, MSG_DONTROUTE) ==
	     INVALID_SOCKET) ||
	    (send(tts_socket, "\")\r\n", 4, MSG_DONTROUTE) ==
	     INVALID_SOCKET))
		return FALSE;
#else
	if ((write(tts_socket, "(SayText \"", 10) < 0) ||
	    (write(tts_socket, text, length) < 0) ||
	    (write(tts_socket, "\")\r\n", 4) < 0))
		return FALSE;
#endif
	return TRUE;
}

#ifndef HAVE_STRCASESTR
/*
 * strcasestr() turns out to be nonstandard extension, but we need it.
 */
const char *strcasestr(const char *haystack, const char *needle)
{
	char *lower_haystack = g_strdup(haystack);
	char *lower_needle = g_strdup(needle);
	char *s;

	for (s = lower_haystack; *s; ++s)
		if (isupper(*s))
			*s = tolower(*s);
	for (s = lower_needle; *s; ++s)
		if (isupper(*s))
			*s = tolower(*s);

	s = strstr(lower_haystack, lower_needle);
	if (s)
		s = (char *)haystack + (s - lower_haystack);

	g_free(lower_haystack);
	g_free(lower_needle);
	return s;
}
#endif /* !HAVE_STRCASESTR */

int ImageDimensions(const char *path, int *x, int *y)
{

	if (gdk_pixbuf_get_file_info(path, x, y))
		return 0;
	else
		return -1;
}

#define IMGSRC_STRING "<img src=\""
#define IMGSRC_LENGTH 10 // strlen(IMGSRC_STRING)
#ifndef HAVE_STRCASESTR
const char *strcasestr(const char *haystack, const char *needle);
#endif

const char *AnalyzeForImageSize(const char *origtext, GdkWindow *window)
{
	static GString *resized;
	static gint resized_init = FALSE;

	const char *trail; // "trail" trails behind ...
	char *path;	// ... the current "path".
	char *end;	 // "end" is the path's end.
	char buf[32];      // for preparing new width+height spec.
	gint image_x, image_y, window_x = -999, window_y = -999;
	int image_retval;
	gboolean warned_once = FALSE;

	if (!resized_init) {
		resized = g_string_new("");
		resized_init = TRUE;
	}
	// performance tweak:
	// image content is by no means common. therefore, spend an extra
	// search call to determine whether any of the rest is needed,
	// most especially to stop copying large blocks of text w/no images.
	if ((path = (char *)strcasestr(origtext, IMGSRC_STRING)) == NULL)
		return origtext;

	for (resized = g_string_assign(resized, ""), trail = origtext
						     /* and path was initialized just above */;
	     path; path = (char *)strcasestr(path, IMGSRC_STRING)) {

		if (window_y == -999) {
/* we have images, but we don't know bounds yet */

#ifdef USE_GTK_3
			window_x = gdk_window_get_width(window);
			window_y = gdk_window_get_height(window);
#else
			gdk_drawable_get_size(window, &window_x,
					      &window_y);
#endif
			if ((window_x > 200) || (window_y > 200)) {
				window_x -= 23;
				window_y -= 23;
			} else {
				window_x = (window_x * 93) / 100;
				window_y = (window_y * 93) / 100;
			}
		}

		/* add the working segment, with annotation added */
		/* to keep us from matching IMGSRC_STRING again. */
		path += IMGSRC_LENGTH;
		resized =
		    g_string_append_len(resized, trail, path - trail - 5);
		resized = g_string_append(resized, "resized=\"yes\" ");
		resized = g_string_append_len(resized, path - 5, 5);

		// some modules play fast-n-loose with proper file spec.
		if (strncmp(path, "file://", 7) == 0) {
#ifdef WIN32
			/* due to need for local soup server */
			path += 5;
			resized =
			    g_string_append(resized,
					    "http://127.0.0.1:7878/");
#else
			path += 7;
			resized = g_string_append(resized, "file://");
#endif
		} else if (strncmp(path, "file:", 5) == 0) {
			path += 5;
#ifdef WIN32
			resized =
			    g_string_append(resized,
					    "http://127.0.0.1:7878/");
#else
			resized = g_string_append(resized, "file:");
#endif
		}
		// else we have an odd case of a src="..." which does not
		// begin with "file:"...which may become the WIN32 norm because
		// webkit is broken as of 2012 jan 12.
		// we will just take "path" as it stands.

		// getting this far means we have a valid img src and file.
		// find closing '"' to determine pathname end.
		if ((end = strchr(path, '"')) == 0)
			continue;

		*end = '\0';
#ifdef WIN32
		resized =
		    g_string_append(resized,
				    g_strdelimit(path, "\\", '/'));
#else
		resized = g_string_append(resized, path);
#endif
		image_retval = ImageDimensions(path, &image_x, &image_y);
		*end = '"';

		resized = g_string_append_c(resized, '"');
		path = end + 1;
		trail = path;

		if (image_retval != 0) {
			if (!warned_once) {
				gui_generic_warning(_("An image file's size could not be determined.\n"
						      "Xiphos cannot resize images to fit window."));
				// settings.imageresize = 0;
				warned_once = TRUE;
			}
			continue;
		}
		// knowing image size & window size, adjust to fit.
		if (image_x > window_x) {
			float proportion =
			    (float)window_x / (float)image_x;
			image_x = window_x;
			image_y = (int)((float)image_y * proportion);
		}
		if (image_y > window_y) {
			float proportion =
			    (float)window_y / (float)image_y;
			image_y = window_y;
			image_x = (int)((float)image_x * proportion);
		}
		sprintf(buf, " width=\"%d\" height=\"%d\"", image_x,
			image_y);
		resized = g_string_append(resized, buf);
	}

	resized = g_string_append(resized, trail); // remainder of text appended.
	return resized->str;
}

/******************************************************************************
 * Name
 *   inhale_text_from_file
 *
 * Synopsis
 *   char *inhale_text_from_file(const char *filename)
 *
 * Description
 *   reads content from a file using stdio, for use anywhere.
 *
 * Return value
 *   char *
 *   to be freed by caller.
 */

char *inhale_text_from_file(const char *filename)
{
	FILE *handle;
	size_t length;
	gchar *blob = NULL;

	if ((handle = g_fopen(filename, "r")) == NULL)
		return NULL;

	(void)fseek(handle, 0L, SEEK_END);
	length = ftell(handle);
	rewind(handle);

	if ((length != 0) &&
	    ((blob = (gchar *)g_malloc(length + 2)) != NULL) &&
	    (fread(blob, 1, length, handle) == length)) {
		/* success. bound the text as a C string. */
		*(blob + length) = '\0';
	} else {
		if (blob)
			g_free(blob);
		blob = NULL;
	}

	fclose(handle);
	return blob;
}

/******   end of file   ******/
