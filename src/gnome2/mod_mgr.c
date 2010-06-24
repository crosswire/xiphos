/*
 * Xiphos Bible Study Tool
 * gnome2/mod_mgr.c
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <gtk/gtk.h>
#include <glade/glade-xml.h>

#include "gui/mod_mgr.h"
#include "gui/dialog.h"
#include "gui/sidebar.h"
#include "gui/utilities.h"
#include "gui/widgets.h"
#include "gui/about_modules.h"

#include "main/lists.h"
#include "main/mod_mgr.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/modulecache.hh"

#include "gui/debug_glib_null.h"

#include <glib/gstdio.h>

/******************************************************************************
 * defines
 */
#define	XI_GENERAL_INTRO	\
_("<b>Overview of the Module Manager.</b>\n\nThis is Xiphos' mechanism to get new and updated content.\nIf you have never been here before, please take a moment to look it over.\n\nModules come from different <u>repositories</u>.  <b>Module Sources: Add/Remove</b> will show you what repositories are currently known.\n\n<b>Module Sources: Choose</b> is for deciding from where modules should come, that is, from which repository Xiphos should obtain them, as well as where they should be placed on your system. Set <i>Install Source</i> and <i>Install Destination</i>, then click <i>Refresh</i>.\n\n<b>Modules: Install/Update</b> is for selecting and obtaining modules after choosing source and destination.\n\n<b>Modules: Maintenance</b> is for archive and index creation.\n\nSee section 5 of our manual for Module Manager detail, or ask for help via Live Chat, or (if no one is responsive in chat) send mail to our users' mailing list.\n")

#define XI_FIRST_INSTALL	\
_("<b>Welcome to Xiphos.</b>\n\nThere are no Bibles installed. In order to initialize, Xiphos needs at least one Bible module. To facilitate this, the Module Manager has been opened so that you may install one or more Bibles, either from a local module set (cdrom, flash drive) or over the network from CrossWire Bible Society. Please refer to these step-by-step instructions, and to the general module manager overview that has also been opened.\n\n<u>For local install:</u>\n- In <i>Module Sources: Add/Remove</i>, add a new local folder name where modules can be found.\n  (This is where folders exist named <i>mods.d</i> and <i>modules</i>.)\n- In <i>Module Sources: Choose</i>, click the \"Local\" button, and select your folder from the pulldown.\n\n<u>For network install from CrossWire:</u>\n- In <i>Module Sources: Choose</i>, click the \"Remote\" button and select CrossWire from the pulldown.\n- Click the \"Refresh\" button at the bottom.\n\n<u>In either case:</u>\n- In <i>Modules: Install/Update</i>, select Bibles and other modules of your preference.\n- Click \"Install\".\n- Close the Module Manager when you are done.\n\n<u>Warning</u>: If you live in a persecuted country, use with care.\n\nBoth this step-by-step instruction dialog and the general introduction dialog may be closed at any time.")

#define GTK_RESPONSE_REFRESH 301
#define GTK_RESPONSE_REMOVE  302
#define GTK_RESPONSE_INSTALL 303
#define GTK_RESPONSE_ARCHIVE 304
#define GTK_RESPONSE_FASTMOD 305
#define GTK_RESPONSE_DELFAST 306
#define GTK_RESPONSE_SOURCES 307
#define GTK_RESPONSE_INTRO   308
/* see these codes' use in ui/module-manager.glade. */

/* activity codes */
enum {
	REMOVE  = 0,
	INSTALL = 1,
	ARCHIVE = 2,
	FASTMOD = 3,
	DELFAST = 4,
};

/* dialog & progress bar phrases */
enum {
	PHRASE_INQUIRY  = 0,
	PHRASE_PREPARE  = 1,
	PHRASE_DOING    = 2,
	PHRASE_COMPLETE = 3,
};

/* treeview columns */
enum {
	COLUMN_NAME,
	COLUMN_INSTALLED,
	COLUMN_FIXED,
	COLUMN_INSTALLED_VERSION,
	COLUMN_FASTREADY,
	COLUMN_LOCKED,
	COLUMN_ABOUT,
	COLUMN_DIFFERENT,
	COLUMN_AVAILABLE_VERSION,
	COLUMN_INSTALLSIZE,
	COLUMN_DESC,
	COLUMN_VISIBLE,
	NUM_COLUMNS
};

/* new install source dialog fields */
enum {
	COLUMN_TYPE,
	COLUMN_CAPTION,
	COLUMN_SOURCE,
	COLUMN_DIRECTORY,
	COLUMN_USER,
	COLUMN_PASS,
	COLUMN_UID,
	NUM_REMOTE_COLUMNS
};

static GtkWidget *treeview;
static GtkWidget *treeview1;
static GtkWidget *treeview2;
static GtkWidget *notebook1;
static GtkWidget *button_close;
static GtkWidget *button_cancel;
static GtkWidget *button_refresh;
static GtkWidget *button_install;
static GtkWidget *button_remove;
static GtkWidget *button_add_local;
static GtkWidget *button_remove_local;
static GtkWidget *button_add_remote;
static GtkWidget *button_remove_remote;
static GtkWidget *button_arch;
static GtkWidget *button_idx;
static GtkWidget *button_delidx;
static GtkWidget *button_load_sources;
static GtkWidget *button_intro;
static GtkWidget *label_home;
static GtkWidget *label_system;
//static GtkWidget *progressbar;
//static GtkWidget *progressbar1;
static GtkWidget *progressbar_refresh;
static GtkWidget *radiobutton_source;
static GtkWidget *radiobutton2;
static GtkWidget *radiobutton_dest;
static GtkWidget *radiobutton4;
static GtkWidget *combo_entry1;
static GtkWidget *combo_entry2;
static GtkWidget *dialog;
static GtkWidget *treeview_local;
static GtkWidget *treeview_remote;

static gboolean local;
static const gchar *source;
static const gchar *destination;
static gboolean have_configs;
static gboolean have_changes;
static gint current_page;
static GdkPixbuf *INSTALLED;
static GdkPixbuf *FASTICON;
static GdkPixbuf *NO_INDEX;
static GdkPixbuf *LOCKED;
static GdkPixbuf *REFRESH;
static GdkPixbuf *BLANK;
static gchar *current_mod;
static gchar *remote_source;
static gboolean first_time_user = FALSE;
static gboolean working = FALSE;
static gboolean is_running = FALSE;

GladeXML *gxml;

static void load_module_tree(GtkTreeView * treeview, gboolean install);
static void set_controls_to_last_use(void);
static int load_source_treeviews(void);

/* indexed by REMOVE/INSTALL/ARCHIVE/FASTMOD/DELFAST and PHRASE_* */
char *verbs[5][4] = {
    {
	N_("Remove these modules?"),
	N_("Preparing to remove"),
	N_("Removing"),
	N_("Remove")
    },
    {
	N_("Install these modules?"),
	N_("Preparing to install"),
	N_("Installing"),
	N_("Install")
    },
    {
	N_("Archive these modules?"),
	N_("Preparing to archive"),
	N_("Archiving"),
	N_("Archive")
    },
    {
	N_("Build fast-search index for these\nmodules (may take minutes/module)?"),
	N_("Preparing to index"),
	N_("Indexing"),
	N_("Index")
    },
    {
	N_("Delete fast-search index for these modules?"),
	N_("Preparing to delete index"),
	N_("Deleting index"),
	N_("Deletion")
    },
};

static
gboolean query_tooltip (GtkWidget  *widget,
			gint        x,
			gint        y,
			gboolean    keyboard_mode,
			GtkTooltip *tooltip,
			gpointer    user_data)
{
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeIter iter;
	GdkPixbuf *pixbuf;
	gchar *about;
	gchar *version;
	gchar *desc;
	GString *str  = g_string_new (NULL);
	GString *text = g_string_new(NULL);
	GString *description = g_string_new(NULL);

	if (!gtk_tree_view_get_tooltip_context ((GtkTreeView *)widget,
						 &x,
						 &y,
						 keyboard_mode,
						 &model,
						 &path,
						 &iter)) {
		return FALSE;
	}

	if (gtk_tree_model_iter_has_child (model ,&iter)) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	gtk_tree_model_get(model, &iter, COLUMN_DESC, &desc, -1);
	if (!desc)
		return FALSE;
	gtk_tree_model_get(model, &iter, COLUMN_ABOUT, &about, -1);
	gtk_tree_model_get(model, &iter, COLUMN_AVAILABLE_VERSION, &version, -1);

	if (about)
		g_strdelimit (about, "&", '+');
	g_string_printf(description,
			"%s\n%s %s\n\n",
			desc,
			(version) ? "Sword module version" : "",
			(version) ? version : "");

	about_module_display(str,
			     ((about && *about)
			      ? about
			      : _("The module has no About information.")),
			     TRUE);

	text = g_string_append_len(text, description->str, description->len);
	text = g_string_append_len(text, str->str, str->len);
	if (text->len > 1200) {
		text = g_string_truncate (text, 1200);
		text = g_string_append_len(text, " ...", strlen (" ..."));
	}
	pixbuf = pixbuf_finder("sword3.png", 0, NULL);
	gtk_tooltip_set_icon (tooltip, pixbuf);
	gtk_tooltip_set_text (tooltip, text->str);

	gtk_tree_view_set_tooltip_cell((GtkTreeView *)widget,
				       tooltip,
				       path,
				       NULL,
				       NULL);
	gtk_tree_path_free (path);
	g_free (about);
	g_free (desc);
	g_free (version);
	g_string_free (str, TRUE);
	g_string_free (text, TRUE);
	g_string_free (description, TRUE);
	return TRUE;
}

/******************************************************************************
 * Name
 *   create_pixbufs
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void create_pixbufs(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
create_pixbufs(void)
{
	INSTALLED = gtk_widget_render_icon(dialog,
					   GTK_STOCK_APPLY,
					   GTK_ICON_SIZE_MENU, NULL);
	FASTICON  = pixbuf_finder("indexed-16.png", 0, NULL);
	NO_INDEX  = gtk_widget_render_icon(dialog,
					   GTK_STOCK_CANCEL,
					   GTK_ICON_SIZE_MENU, NULL);
	LOCKED    = pixbuf_finder("epiphany-secure.png", 0, NULL);
	REFRESH   = gtk_widget_render_icon(dialog,
					   GTK_STOCK_REFRESH,
					   GTK_ICON_SIZE_MENU, NULL);
	BLANK     = gtk_widget_render_icon(dialog,
					   "gnome-stock-blank",
					   GTK_ICON_SIZE_MENU, NULL);
}

/******************************************************************************
 * Name
 *   create_model
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   GtkTreeModel *create_model (void)
 *
 * Description
 *
 *
 * Return value
 *   GtkTreeModel *
 */

static GtkTreeModel *
create_model(void)
{
	GtkTreeStore *store;

	/* create list store */
	store = gtk_tree_store_new(NUM_COLUMNS,
				   G_TYPE_STRING,	/* module name */
				   GDK_TYPE_PIXBUF,	/* installed */
				   G_TYPE_BOOLEAN,	/* checkbox */
				   G_TYPE_STRING,	/* installed verssion */
				   GDK_TYPE_PIXBUF,	/* fastready */
				   GDK_TYPE_PIXBUF,	/* locked */
				   G_TYPE_STRING,	/* about */
				   GDK_TYPE_PIXBUF,	/* refresh */
				   G_TYPE_STRING,	/* available version */
				   G_TYPE_STRING,	/* size */
				   G_TYPE_STRING,	/* description */
				   G_TYPE_BOOLEAN);	/* visibility */
	return GTK_TREE_MODEL(store);
}

/******************************************************************************
 * Name
 *   fixed_toggled
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void fixed_toggled (GtkCellRendererToggle *cell, gchar *path_str,
 *							gpointer data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
fixed_toggled(GtkCellRendererToggle *cell,
	      gchar *path_str,
	      gpointer data)
{
	GtkTreeView *treeview = (GtkTreeView *) data;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
	gboolean fixed;

	/* get toggled iter */
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter, COLUMN_FIXED, &fixed, -1);

	/* do something with the value */
	fixed ^= 1;

	/* set new value */
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, COLUMN_FIXED,
			   fixed, -1);
	/* clean up */
	gtk_tree_path_free(path);
}

/******************************************************************************
 * Name
 *   add_columns
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void add_columns (GtkTreeView *treeview)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
add_columns(GtkTreeView * treeview,
	    gboolean remove)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkWidget *image;

	/* -- column for sword module name -- */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Module Name"), renderer,
						     "text", COLUMN_NAME,
						     NULL);
					/* fixed sizing (200 pixels) */
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN
					     (column), 200);
	gtk_tree_view_append_column(treeview, column);

	/* -- installed -- */
	column = gtk_tree_view_column_new();
	image = (remove
		 ? gtk_image_new_from_stock("gnome-stock-blank",
					    GTK_ICON_SIZE_MENU)
		 : gtk_image_new_from_stock(GTK_STOCK_APPLY,
					    GTK_ICON_SIZE_MENU));
	gtk_widget_show(image);
	gtk_widget_set_tooltip_text(image,
				    (remove
				     ? ""
				     : _("A checkmark means this module is already installed")));
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COLUMN_INSTALLED,
					    NULL);
	gtk_tree_view_append_column(treeview, column);

	/* -- toggle choice -- */
	renderer = gtk_cell_renderer_toggle_new();
	g_signal_connect(renderer, "toggled",
			 G_CALLBACK(fixed_toggled), treeview);

	column = gtk_tree_view_column_new();
	image = gtk_image_new_from_stock((remove
					  ? "gtk-yes" //GTK_STOCK_REMOVE
					  : GTK_STOCK_ADD),
					 GTK_ICON_SIZE_MENU);
	gtk_widget_show(image);
	gtk_widget_set_tooltip_text(image,
				    (remove
				     ? _("Click the box to work on this module")
				     : _("Click the box to select this module for install/update")));
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "active",
					    COLUMN_FIXED,
					    "visible",
					    COLUMN_VISIBLE, NULL);

					/* fixed sizing (25 pixels) */
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN
					     (column), 25);
	gtk_tree_view_append_column(treeview, column);

	/* -- installed version -- */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Installed"),
						     renderer, "text",
						     COLUMN_INSTALLED_VERSION,
						     NULL);
	gtk_tree_view_append_column(treeview, column);

	/* -- fast index ready -- */
	column = gtk_tree_view_column_new();
	image = pixmap_finder("indexed-16.png");
	gtk_widget_show(image);
	gtk_widget_set_tooltip_text(image, _("The index icon means you have built an optimized ('lucene') index for this module for fast searching (see the Maintenance pane for this function)"));
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COLUMN_FASTREADY,
					    NULL);
					/* fixed sizing (25 pixels) */
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN
					     (column), 25);
	gtk_tree_view_append_column(treeview, column);

	/* -- locked -- */
	column = gtk_tree_view_column_new();
	image = pixmap_finder("epiphany-secure.png");
	gtk_widget_show(image);
	gtk_widget_set_tooltip_text(image, _("The lock icon means this module is encrypted, and requires that you purchase an unlock key from the content owner"));
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COLUMN_LOCKED,
					    NULL);
					/* fixed sizing (25 pixels) */
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN
					     (column), 25);
	gtk_tree_view_append_column(treeview, column);

	/* -- About content (invisible) -- */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("About"), renderer,
						     "text", COLUMN_ABOUT,
						     NULL);
	/* column not shown */
	gtk_tree_view_column_set_visible(column, FALSE);
					/* fixed sizing (2 pixels) */
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN
					     (column), 2);
	gtk_tree_view_append_column(treeview, column);

	if (remove)
		return;	/* no more fields needed */

	/* -- refresh/update -- */
	column = gtk_tree_view_column_new();
	image = gtk_image_new_from_stock(GTK_STOCK_REFRESH,
					 GTK_ICON_SIZE_MENU);
	gtk_widget_show(image);
	gtk_widget_set_tooltip_text(image, _("The refresh icon means the Installed module is older than the newer Available module: You should update the module"));
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COLUMN_DIFFERENT,
					    NULL);
	gtk_tree_view_append_column(treeview, column);

	/* -- available version -- */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Available"),
						     renderer, "text",
						     COLUMN_AVAILABLE_VERSION,
						     NULL);
	gtk_tree_view_append_column(treeview, column);

	/* -- install size -- */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Size"),
						     renderer, "text",
						     COLUMN_INSTALLSIZE,
						     NULL);
	gtk_tree_view_append_column(treeview, column);

	/* -- description -- */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Description"),
						     renderer, "text",
						     COLUMN_DESC,
						     NULL);
	gtk_tree_view_append_column(treeview, column);
}

static void
setup_treeview_install(GtkTreeView * install)
{
	GtkTreeModel *model;
	model = create_model();
	gtk_tree_view_set_model(install, NULL);
	gtk_tree_view_set_model(install, model);
	add_columns(install, FALSE);
}

static void
setup_treeview_maintenance(GtkTreeView * maintenance)
{
	GtkTreeModel *model;
	model =  create_model();
	gtk_tree_view_set_model(maintenance, NULL);
	gtk_tree_view_set_model(maintenance, model);
	add_columns(maintenance, TRUE);
}

/******************************************************************************
 * Name
 *   mod_mgr_check_for_file
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   gboolean mod_mgr_check_for_file(const gchar * filename)
 *
 * Description
 *
 *
 * Return value
 *   gboolean
 */

static gboolean mod_mgr_check_for_file(const gchar * filename)
{
	return g_file_test(filename, G_FILE_TEST_EXISTS);
}


/******************************************************************************
 * Name
 *   remove_install_modules
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *    void remove_install_modules(GList * modules, int activity)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

#define ZIP_DIR	DOTSWORD "/zip"

static void
remove_install_modules(GList * modules,
		       int activity)
{
	GList *tmp;
	gchar *buf;
	const gchar *new_dest;
	gint failed = 1;
	GString *mods;
	gchar *dialog_text;

	if (!modules)
		return;

	mods = g_string_new("");
	tmp = modules;
	while (tmp) {
		buf = (gchar *) tmp->data;
		if (buf[0] == '*')
			++buf;
		if (*(mods->str))	// not the first
			mods = g_string_append(mods, ", ");
		mods = g_string_append(mods, buf);
		tmp = g_list_next(tmp);
	}

	dialog_text = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
				      gettext (verbs[activity][PHRASE_INQUIRY]), mods->str);

	if (!gui_yes_no_dialog(dialog_text, NULL)) {
		tmp = modules; /* free list data */
		while (tmp) {
			g_free((gchar*)tmp->data);
			tmp = g_list_next(tmp);
		}
		g_list_free(modules);
		g_free(dialog_text);
		return;
	}
	g_free(dialog_text);

	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh),
				  gettext (verbs[activity][PHRASE_PREPARE]));
	gtk_widget_show(progressbar_refresh);
	gtk_widget_queue_draw(dialog);
	gtk_widget_hide(button_close);
	gtk_widget_hide(button_refresh);
	gtk_widget_hide(button_install);
	gtk_widget_hide(button_remove);
	gtk_widget_hide(button_arch);
	gtk_widget_hide(button_idx);
	gtk_widget_hide(button_delidx);
	gtk_widget_hide(button_load_sources);
	gtk_widget_show(button_cancel);

	tmp = modules;
	while (tmp) {
		buf = (gchar *) tmp->data;
		current_mod = buf;
		g_string_printf(mods, "%s: %s",
				gettext (verbs[activity][PHRASE_DOING]), buf);
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR
					  (progressbar_refresh),
					  mods->str);
		sync_windows();

		if (activity == ARCHIVE) {
			GString *cmd = g_string_new(NULL);
			gchar *dir;
			gchar *zipfile;
			char *datapath, *conf_file;

			GS_print(("archive %s in %s\n", buf,
				  (destination
				   ? destination
				   : settings.path_to_mods)));
			dir = g_strdup_printf("%s/%s", settings.homedir, ZIP_DIR);
			if ((g_access(dir, F_OK) == -1) &&
			    (g_mkdir(dir, S_IRWXU) != 0)) {
				char msg[300];
				sprintf(msg, _("`mkdir %s' failed:\n%s."),
					dir, strerror(errno));
				g_free(dir);
				gui_generic_warning(msg);
				return;
			}

			zipfile = g_strdup_printf("%s/%s.zip", dir, buf);
			datapath = main_get_mod_config_entry(buf, "DataPath");
			if (g_access(datapath, F_OK) == -1)
			    *(strrchr(datapath, '/')) = '\0';

			conf_file = main_get_mod_config_file
					    (buf, (destination
						   ? destination
						   : settings.path_to_mods));
			g_remove(zipfile);

			xiphos_create_archive(conf_file, datapath, zipfile,
					      destination ?
					      destination : settings.path_to_mods);
			g_string_append(cmd, buf);
			g_string_append(cmd, _(" archived in: \n"));
			g_string_append(cmd, zipfile);
			gui_generic_warning(cmd->str);
			g_free(conf_file);
			g_free(datapath);
			g_free(zipfile);
			g_free(dir);
			g_string_free(cmd, TRUE);
			failed = 0;
		}

		if ((activity == REMOVE) ||	// just delete it
		    (!first_time_user &&	// don't trip on "no modules".
		     (activity == INSTALL) &&
		     main_is_module(buf))) {	// delete before re-install
			GS_print(("remove %s from %s\n", buf,
				  (destination
				   ? destination
				   : settings.path_to_mods)));
			failed = mod_mgr_uninstall(destination, buf);
			if (failed == -1) {
				//mod_mgr_shut_down();
				if (destination) {
					new_dest = NULL;
				} else {
					new_dest = gtk_label_get_text
					    (GTK_LABEL(label_home));
					GS_warning(("%s",new_dest));
				}
				GS_print(("removing %s from %s\n",
					  buf,
					  (new_dest
					   ? new_dest
					   : settings.path_to_mods)));
				failed =
				    mod_mgr_uninstall(new_dest, buf);
			}

			// annihilate cache of removed module.
			ModuleCacheErase((const char *)buf);
		}

		if (activity == INSTALL) {
			GS_print(("install %s, source=%s\n", buf, source));
			failed = ((local)
				  ? mod_mgr_local_install_module(destination, source, buf)
				  : mod_mgr_remote_install(destination, source, buf));
		}

		if (activity == FASTMOD) {
			GS_print(("index %s\n", buf));
#if 0
			// why did we ever preclude indexing these?
			if (main_get_mod_config_entry(buf, "GSType"))
				gui_generic_warning
				    (_("Journals and prayer lists cannot be indexed."));
			else
#endif
				failed = ((main_module_mgr_index_mod(buf))
					  ? 0 : 1);
		}

		if (activity == DELFAST) {
			GS_print(("deleting index %s\n", buf));
			failed = ((main_module_mgr_delete_index_mod(buf))
				  ? 0 : 1);
		}

		g_free(tmp->data);
		tmp = g_list_next(tmp);
	}
	have_changes = TRUE;
	g_list_free(modules);

	if (failed) {
		g_string_printf(mods, _("%s failed"),
				gettext(verbs[activity][PHRASE_COMPLETE]));
	} else {
		g_string_printf(mods, "%s", _("Finished"));
	}
	if (!failed &&
	    ((activity == REMOVE) ||
	     (activity == FASTMOD) ||
	     (activity == DELFAST)))
		load_module_tree(GTK_TREE_VIEW(treeview2), 0);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), mods->str);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh), 0);
	g_string_free(mods, TRUE);
	gtk_widget_hide(button_cancel);
	gtk_widget_show(button_close);
	switch (current_page) {
		case 3:
			gtk_widget_show(button_refresh);
			gtk_widget_show(button_install);
			gtk_widget_hide(button_arch);
			gtk_widget_hide(button_idx);
			gtk_widget_hide(button_delidx);
			gtk_widget_hide(button_load_sources);
		break;
		case 4:
			gtk_widget_show(button_remove);
			gtk_widget_show(button_arch);
			gtk_widget_show(button_idx);
			gtk_widget_show(button_delidx);
			gtk_widget_hide(button_load_sources);
		break;
	}
	sync_windows();
}


/******************************************************************************
 * Name
 *   parse_treeview
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *    GList *parse_treeview(GtkTreeModel * model, GtkTreeIter * tree_parent)
 *
 * Description
 *
 *
 * Return value
 *   GList *
 */

static GList *
parse_treeview(GList *list,
	       GtkTreeModel *model,
	       GtkTreeIter *tree_parent)
{
	GtkTreeIter child;
	gchar *name = NULL;
	gboolean fixed;

	gtk_tree_model_iter_children(model, &child, tree_parent);

	do {
		gtk_tree_model_get(model, &child,
				   COLUMN_FIXED, &fixed, COLUMN_NAME,
				   &name, -1);
		if (gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &child)) {
			list = parse_treeview(list, model, &child);
		} else {
			if (fixed) {
				list = g_list_append(list, (gchar *) name);
			}
		}
	} while (gtk_tree_model_iter_next(model, &child));

	return list;
}


/******************************************************************************
 * Name
 *   get_list_mods_to_remove_install
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void get_list_mods_to_remove_install(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static GList *
get_list_mods_to_remove_install(int activity)
{
	GList *retval = NULL;
	GtkTreeIter root;
	GtkTreeModel *model;
	gchar *name;
	gboolean fixed;
	if (activity == INSTALL)
		model =
		    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	else
		model =
		    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview2));
	if (!gtk_tree_model_get_iter_first
	    (GTK_TREE_MODEL(model), &root))
		return retval;

	do {
		gtk_tree_model_get(model, &root,
				   COLUMN_FIXED, &fixed, COLUMN_NAME,
				   &name, -1);
		if (gtk_tree_model_iter_has_child(model, &root)) {
			retval = parse_treeview(retval, model, &root);
		} else {
			if (fixed) {
				retval = g_list_append(retval, (gchar *)name);
			}

		}
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &root));

	return retval;
}


/******************************************************************************
 * Name
 *   add_module_to_language_folder
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void add_module_to_language_folder(GtkTreeModel * model,
 *		      GtkTreeIter iter, MOD_MGR *info)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
add_module_to_language_folder(GtkTreeView * tree,
			      GtkTreeModel *model,
			      GtkTreeIter iter,
			      MOD_MGR *info,
			      gboolean checkmark)
{
	GtkTreeIter iter_iter;
	GtkTreeIter child_iter;
	//GtkTreePath *path;
	gboolean valid;
	GdkPixbuf *installed;
	GdkPixbuf *fasticon;
	GdkPixbuf *locked;
	GdkPixbuf *refresh;
	gchar *description = NULL;
//	GtkTooltip *tooltip;

	/* Check language */
	const gchar *buf = info->language;
	if (!g_utf8_validate(buf,-1,NULL))
		info->language = _("Unknown");
	if (!g_unichar_isalnum(g_utf8_get_char(buf)) || (info->language == NULL))
		info->language = _("Unknown");

	description = info->description;

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;

		/*gtk_tooltip_set_markup ( tooltip,
                                         "<b>a little</b><br>tooltip");*/
		gtk_tree_model_get(model, &iter_iter, COLUMN_NAME,
				   &str_data, -1);
		if (!strcmp(info->language, str_data)) {
			installed = ((info->installed) ? INSTALLED : BLANK);

			locked    = ((info->locked)    ? LOCKED    : BLANK);

			if ((info->installed &&
			     (!info->old_version && info->new_version &&
			      strcmp(info->new_version, " "))) ||
			    ((info->old_version && !info->new_version)) ||
			    ((info->old_version && info->new_version &&
			      strcmp(info->new_version, info->old_version) > 0)))
				refresh = REFRESH;
			else
				refresh = BLANK;

			if (!first_time_user &&
			    info->installed &&
			    main_has_search_framework(info->name)) {
				if (main_optimal_search(info->name))
					fasticon = FASTICON;
				else
					fasticon = NO_INDEX;
			} else
				fasticon = BLANK;

			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter_iter);
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
					   COLUMN_NAME, info->name,
					   COLUMN_INSTALLED,
					   (checkmark ? installed : BLANK),
					   COLUMN_FIXED, FALSE,
					   COLUMN_INSTALLED_VERSION,
					   info->old_version,
					   COLUMN_FASTREADY, fasticon,
					   COLUMN_LOCKED, locked,
					   COLUMN_ABOUT, info->about,
					   COLUMN_DIFFERENT, refresh,
					   COLUMN_AVAILABLE_VERSION,
					   info->new_version,
					   COLUMN_INSTALLSIZE,
					   info->installsize,
					   COLUMN_DESC, description,
					   COLUMN_VISIBLE, TRUE, -1);
			g_free(str_data);
			return;
		}
		g_free(str_data);
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
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
language_add_folders(GtkTreeModel * model,
		     GtkTreeIter iter,
		     gchar ** languages)
{
	GtkTreeIter iter_iter;
	GtkTreeIter child_iter;
	int j;

	(void) gtk_tree_model_iter_children(model, &iter_iter, &iter);
	for (j = 0; languages[j]; ++j) {
		gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(model),
				   &child_iter,
				   COLUMN_VISIBLE,
				   FALSE,
				   COLUMN_NAME,
				   ((g_utf8_validate(languages[j], -1, NULL))
				    ? languages[j]
				    : _("Unknown")),
				   -1);
	}
}


/******************************************************************************
 * Name
 *   add_language_folder
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void add_language_folder(GtkTreeModel * model, GtkTreeIter iter,
 *			      gchar * language)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
add_language_folder(GtkTreeModel *model,
		    GtkTreeIter iter,
		    const gchar *language)
{
	GtkTreeIter iter_iter;
	GtkTreeIter child_iter;
	gboolean valid;

	/* Check language */
	const gchar *buf = language;
	if (!g_utf8_validate(buf,-1,NULL))
		language = _("Unknown");
	if (!g_unichar_isalnum(g_utf8_get_char(buf)) || (language == NULL))
		language = _("Unknown");

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;

		gtk_tree_model_get(model,
				   &iter_iter,
				   COLUMN_NAME,
				   &str_data,
				   -1);

		if (!g_utf8_collate(g_utf8_casefold(language, -1),
				    g_utf8_casefold(str_data, -1))) {
			g_free(str_data);
			return;
		}
		g_free(str_data);
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
	gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter, &iter);
	gtk_tree_store_set(GTK_TREE_STORE(model),
			&child_iter,
			COLUMN_VISIBLE,
			FALSE,
			COLUMN_NAME,
			language,
			-1);
}

static gboolean
on_modules_list_button_release(GtkWidget * widget,
			     GdkEventButton * event,
			     gpointer data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter selected;
	GtkTreePath *path;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(data));

	if (!gtk_tree_selection_get_selected (selection, &model, &selected))
		return FALSE;

	if (!gtk_tree_model_iter_has_child (model ,&selected))
		return FALSE;

	path = gtk_tree_model_get_path (model, &selected);
	if (gtk_tree_view_row_expanded (GTK_TREE_VIEW(data), path))
	       gtk_tree_view_collapse_row ( GTK_TREE_VIEW(data), path );
        else
	       gtk_tree_view_expand_row ( GTK_TREE_VIEW(data), path, FALSE );
	gtk_tree_path_free ( path );
	return FALSE;
}



/******************************************************************************
 * Name
 *   load_module_tree
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void load_module_tree(GtkTreeView * treeview, gboolean install)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
load_module_tree(GtkTreeView * treeview,
		 gboolean install)
{
	GtkTreeStore *store;
	GtkTreeIter repository_name;
	gchar *repository_identifier;
	GtkTreeIter category_type;
	GtkTreeIter category_avail;
	GtkTreeIter text;
	GtkTreeIter commentary;
	GtkTreeIter dictionary;
	GtkTreeIter devotional;
	GtkTreeIter book;
	GtkTreeIter map;
	GtkTreeIter image;
	GtkTreeIter cult;
	GtkTreeIter glossary;
	GtkTreeIter separator;
	GtkTreeIter update;
	GtkTreeIter uninstalled;
	GtkTreeIter prayerlist;
	GList *tmp = NULL;
	GList *tmp2 = NULL;
	MOD_MGR *info;

	if (install) {
		if (GTK_TOGGLE_BUTTON(radiobutton_source)->active) {
			local = TRUE;
			source =
			    gtk_entry_get_text(GTK_ENTRY(GTK_BIN(combo_entry1)->child));

			// must find the directory attached to the name.
			// they may (and normally will) be the same,
			// but it's not certain.
			tmp = tmp2 = mod_mgr_list_local_sources();
			while (tmp) {
				MOD_MGR_SOURCE *mms =
				    (MOD_MGR_SOURCE *)tmp->data;

				if (!strcmp(source, mms->caption))
					source = g_strdup(mms->directory);

				g_free((gchar*)mms->type);
				g_free((gchar*)mms->caption);
				g_free((gchar*)mms->source);
				g_free((gchar*)mms->directory);
				g_free((gchar*)mms->user);
				g_free((gchar*)mms->pass);
				g_free((gchar*)mms->uid);
				g_free(mms);
				tmp = g_list_next(tmp);
			}
			g_list_free(tmp2);

			tmp = mod_mgr_list_local_modules(source, FALSE);
			// false -> tell installmgr not to mess with ~/.sword content.
		} else {
			local = FALSE;
			source =
			    gtk_entry_get_text(GTK_ENTRY(GTK_BIN(combo_entry2)->child));
			tmp = mod_mgr_remote_list_modules(source);
		}
	} else {
		// we are doing maintenance on all modules.
		tmp = mod_mgr_list_local_modules(settings.path_to_mods, TRUE);
		// true -> we must have all modules available, incl. ~/.sword content.
	}

	gtk_tree_view_set_model(treeview, NULL);
	store = GTK_TREE_STORE(create_model());
	gtk_tree_store_clear(store);

	if (!g_list_length(tmp))
		return;

	if (install && !first_time_user) {
		/* note the repository that is active */
		if ((local == FALSE) && (remote_source == NULL))
			remote_source = g_strdup(gtk_combo_box_get_active_text(
						     GTK_COMBO_BOX(combo_entry2)));

		repository_identifier =
		    g_strdup_printf(_("Repository:\n%s"),
				    (local ? source : remote_source));
		gtk_tree_store_append(store, &repository_name, NULL);
		gtk_tree_store_set(store, &repository_name, 0,
				   repository_identifier, -1);
		g_free(repository_identifier);

		gtk_tree_store_append(store, &separator, NULL);
		gtk_tree_store_set(store, &separator, 0, "------------------------", -1);

		gtk_tree_store_append(store, &category_type, NULL);
		gtk_tree_store_set(store, &category_type, 0,
				   _("Categorized by\nModule Type"), -1);
	}

	/*  add Biblical Texts folder */
	gtk_tree_store_append(store, &text, NULL);
	gtk_tree_store_set(store, &text, 0, _("Biblical Texts"), -1);

	/*  add Commentaries folder */
	gtk_tree_store_append(store, &commentary, NULL);
	gtk_tree_store_set(store, &commentary, 0, _("Commentaries"), -1);

	/*  add Dictionaries folder */
	gtk_tree_store_append(store, &dictionary, NULL);
	gtk_tree_store_set(store, &dictionary, 0, _("Dictionaries"), -1);

	/*  add Glossaries folder */
	gtk_tree_store_append(store, &glossary, NULL);
	gtk_tree_store_set(store, &glossary, 0, _("Glossaries"), -1);

	/*  add Devotionals folder */
	gtk_tree_store_append(store, &devotional, NULL);
	gtk_tree_store_set(store, &devotional, 0, _("Daily Devotionals"), -1);

	/*  add Books folder */
	gtk_tree_store_append(store, &book, NULL);
	gtk_tree_store_set(store, &book, 0, _("General Books"), -1);

	/*  add Maps folder */
	gtk_tree_store_append(store, &map, NULL);
	gtk_tree_store_set(store, &map, 0, _("Maps"), -1);

	/*  add Images folder */
	gtk_tree_store_append(store, &image, NULL);
	gtk_tree_store_set(store, &image, 0, _("Images"), -1);

	/*  add Cult folder */
	gtk_tree_store_append(store, &cult, NULL);
	gtk_tree_store_set(store, &cult, 0, _("Cult/Unorthodox"), -1);

	if (install && !first_time_user) {
		gtk_tree_store_append(store, &separator, NULL);
		gtk_tree_store_set(store, &separator, 0, "------------------------", -1);

		gtk_tree_store_append(store, &category_avail, NULL);
		gtk_tree_store_set(store, &category_avail, 0,
				   _("Categorized by\nAvailability"), -1);


		/*  add Updates folder */
		gtk_tree_store_append(store, &update, NULL);
		gtk_tree_store_set(store, &update, 0, _("Updates"), -1);

		/*  add Uninstalled folder */
		gtk_tree_store_append(store, &uninstalled, NULL);
		gtk_tree_store_set(store, &uninstalled, 0, _("Uninstalled"), -1);
	} else {
		if (settings.prayerlist) {
			gtk_tree_store_append(store, &prayerlist, NULL);
			gtk_tree_store_set(store, &prayerlist, 0, _("Prayer List/Journal"), -1);
		}
	}

	language_make_list(tmp, store,
			   text, commentary, map, image,
			   devotional, dictionary, glossary, book, cult,
			   ((install && !first_time_user) ? &update : NULL),
			   ((install && !first_time_user) ? &uninstalled : NULL),
			   language_add_folders, FALSE);

	tmp2 = tmp;
	while (tmp2) {
		info = (MOD_MGR *) tmp2->data;

		if (install && !first_time_user) {
			// special lists: updated and uninstalled modules.
			if (!info->installed) {
				add_module_to_language_folder(treeview,
							      GTK_TREE_MODEL
							      (store), uninstalled,
							      info, install);
			} else if ((!info->old_version && info->new_version &&
				    strcmp(info->new_version, " ")) ||
				   (info->old_version && !info->new_version) ||
				   (info->old_version && info->new_version &&
				    strcmp(info->new_version, info->old_version) > 0)) {
				add_module_to_language_folder(treeview,
							      GTK_TREE_MODEL
							      (store), update,
							      info, install);
			}
		}

		// see comment on similar code in src/main/sidebar.cc.

		if (info->is_cult) {
			add_module_to_language_folder(treeview,
						      GTK_TREE_MODEL
						      (store), cult,
						      info, install);
		}
		else if (info->type[0] == 'B') {
			add_module_to_language_folder(treeview,
						      GTK_TREE_MODEL
						      (store), text,
						      info, install);
		}
		else if (info->type[0] == 'C') {
			add_module_to_language_folder(treeview,
						      GTK_TREE_MODEL
						      (store), commentary,
						      info, install);
		}
		else if (info->is_maps) {
			add_module_to_language_folder(treeview,
						      GTK_TREE_MODEL
						      (store), map,
						      info, install);
		}
		else if (info->is_images) {
			add_module_to_language_folder(treeview,
						      GTK_TREE_MODEL
						      (store), image,
						      info, install);
		}
		else if (info->is_devotional) {
			add_module_to_language_folder(treeview,
						      GTK_TREE_MODEL
						      (store), devotional,
						      info, install);
		}
		else if (info->is_glossary) {
			add_module_to_language_folder(treeview,
						      GTK_TREE_MODEL
						      (store), glossary,
						      info, install);
		}
		else if (info->type[0] == 'L') {
			add_module_to_language_folder(treeview,
						      GTK_TREE_MODEL
						      (store), dictionary,
						      info, install);
		}
		else if (info->type[0] == 'G') {
			gchar *gstype;
			if (first_time_user ||
			    ((gstype = main_get_mod_config_entry(info->name, "GSType"))
			     == NULL) ||
			    strcmp(gstype, "PrayerList")) {
				add_module_to_language_folder
				    (treeview,GTK_TREE_MODEL(store), book, info, install);
			} else if (settings.prayerlist) {
				add_language_folder
				    (GTK_TREE_MODEL(store), prayerlist, info->language);
				add_module_to_language_folder
				    (treeview,GTK_TREE_MODEL(store), prayerlist, info, install);
			}
		}
		else {
			GS_warning(("mod `%s' unknown type `%s'",
				    info->name, info->type));
		}

		g_free(info->name);
		g_free(info->about);
		g_free(info->type);
		g_free(info->new_version);
		g_free(info->old_version);
		g_free(info->installsize);
		g_free(info);
		tmp2 = g_list_next(tmp2);
	}
	g_list_free(tmp);

	gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));

	g_signal_connect_after((gpointer) treeview,
			 "button_release_event",
			 G_CALLBACK
			 (on_modules_list_button_release), treeview);

}


/******************************************************************************
 * Name
 *   remove_install_wrapper
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *    void remove_install_wrapper(int activity)
 *
 * Description
 *   generalization of install/remove/archive/index methodology.
 *
 * Return value
 *   void
 */

static void
remove_install_wrapper(int activity)
{
	GList *modules = NULL;

	if (working) return;
	working = TRUE;

	modules = get_list_mods_to_remove_install(activity);

	sync_windows();

	remove_install_modules(modules, activity);
	mod_mgr_shut_down();
	mod_mgr_init(destination, FALSE, TRUE);
	load_module_tree(GTK_TREE_VIEW(treeview), (activity == INSTALL));

	sync_windows();

	working = FALSE;
}

/******************************************************************************
 * Name
 *   response_refresh
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void response_refresh(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
response_refresh(void)
{
	gint failed;
	gchar *buf;

	if (working) return;
	working = TRUE;

	if (remote_source == NULL)
		remote_source = g_strdup(gtk_combo_box_get_active_text(
					     GTK_COMBO_BOX(combo_entry2)));

	buf = g_strdup_printf("%s: %s", _("Refreshing from remote source"), remote_source);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), buf);
	g_free(buf);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh), 0);
	gtk_widget_show(progressbar_refresh);
	sync_windows();
	failed = mod_mgr_refresh_remote_source(remote_source);

	if (failed) {
		gtk_progress_bar_set_text(
			GTK_PROGRESS_BAR(progressbar_refresh), _("Remote source not found"));
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh), 0);
		working = FALSE;
		return;
	}
	load_module_tree(GTK_TREE_VIEW(treeview), TRUE);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), _("Finished"));
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh), 0);

	working = FALSE;
}

/******************************************************************************
 * Name
 *   check_sync_repos
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void check_sync_repos(void)
 *
 * Description
 *   verifies enough remote sources are known, and gets more if needed.
 *
 * Return value
 *   void
 */

static void
check_sync_repos(void)
{
	GList *tmp = mod_mgr_list_remote_sources();
	/*
	 * if too few remote sources, synchronize repos from crosswire.
	 */
	if (g_list_length(tmp) <= 1) {
		char index[10];
		mod_mgr_init_config_extras();
		settings.mod_mgr_remote_source_index = load_source_treeviews();
		g_snprintf(index, 10, "%d", settings.mod_mgr_remote_source_index);
		xml_set_value("Xiphos", "modmgr", "mod_mgr_remote_source_index", index);
		set_controls_to_last_use();
	}
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *   add_columns_to_first
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void add_columns (GtkTreeView *treeview)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
add_columns_to_first(GtkTreeView * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Configure"),
						     renderer, "text",
						     COLUMN_NAME, NULL);
	gtk_tree_view_append_column(treeview, column);
}


/******************************************************************************
 * Name
 *   add_columns_to_remote_treeview
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void  add_columns_to_remote_treeview(GtkTreeView *treeview)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
add_columns_to_remote_treeview(GtkTreeView * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Type"),
						     renderer, "text",
						     COLUMN_TYPE, NULL);
	gtk_tree_view_append_column(treeview, column);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Caption"),
						     renderer, "text",
						     COLUMN_CAPTION,
						     NULL);
	gtk_tree_view_append_column(treeview, column);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Source"),
						     renderer, "text",
						     COLUMN_SOURCE,
						     NULL);
	gtk_tree_view_append_column(treeview, column);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Directory"),
						     renderer, "text",
						     COLUMN_DIRECTORY,
						     NULL);
	gtk_tree_view_append_column(treeview, column);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("User"),
						     renderer, "text",
						     COLUMN_USER,
						     NULL);
	gtk_tree_view_append_column(treeview, column);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Password"),
						     renderer, "text",
						     COLUMN_PASS,
						     NULL);
	gtk_tree_view_append_column(treeview, column);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("UID"),
						     renderer, "text",
						     COLUMN_UID,
						     NULL);
	gtk_tree_view_append_column(treeview, column);
}

static GtkTreeModel *
create_model_to_first(void)
{
	GtkTreeStore *model;
	GtkTreeIter iter;
	GtkTreeIter child_iter;

	model = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);

	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("Module Sources"), -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Add/Remove"), 1, 1, -1);
	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Choose"), 1, 2, -1);


	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("Modules"), -1);
	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Install/Update"), 1, 3, -1);
	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Maintenance"), 1, 4, -1);

	return GTK_TREE_MODEL(model);
}




static GtkTreeModel *
create_remote_source_treeview_model(void)
{
	GtkListStore *store;

	/* create list store */
	store = gtk_list_store_new(NUM_REMOTE_COLUMNS,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING);

	return GTK_TREE_MODEL(store);
}


/******************************************************************************
 * Name
 *   load_source_treeviews
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *   int load_source_treeviews(void)
 *
 * Description
 *   inhale InstallMgr.conf content.  return CrossWire's position.
 *
 * Return value
 *   int
 */

static int
load_source_treeviews(void)
{
	int crosswire_index = 1 /* guess */, crosswire_tracker = 0 /* start */;
	GList *tmp = NULL;
	GList *tmp2 = NULL;
	GtkTreeIter iter;
	GtkTreeIter combo_iter;
	MOD_MGR_SOURCE *mms;
	GtkTreeModel *remote_model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_remote));
	GtkTreeModel *local_model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_local));
	GtkTreeModel* module_box_local =
		gtk_combo_box_get_model(GTK_COMBO_BOX(combo_entry1));
	GtkTreeModel* module_box_remote =
		gtk_combo_box_get_model(GTK_COMBO_BOX(combo_entry2));

	/* remote */
	gtk_list_store_clear(GTK_LIST_STORE(remote_model));
	gtk_list_store_clear(GTK_LIST_STORE(module_box_remote));
	tmp = tmp2 = mod_mgr_list_remote_sources();
	while (tmp) {
		mms = (MOD_MGR_SOURCE *) tmp->data;
		if (!strcmp(mms->caption, "CrossWire"))
			crosswire_index = crosswire_tracker;
		gtk_list_store_append(GTK_LIST_STORE(remote_model),
				      &iter);
		gtk_list_store_set(GTK_LIST_STORE(remote_model), &iter,
				   COLUMN_TYPE,      mms->type,
				   COLUMN_CAPTION,   mms->caption,
				   COLUMN_SOURCE,    mms->source,
				   COLUMN_DIRECTORY, mms->directory,
				   COLUMN_USER,      mms->user,
				   COLUMN_PASS,      mms->pass,
				   COLUMN_UID,       mms->uid,
				   -1);
		gtk_list_store_append(GTK_LIST_STORE(module_box_remote), &combo_iter);
		gtk_list_store_set(GTK_LIST_STORE(module_box_remote),
					&combo_iter,
					0,
					(gchar*)mms->caption,
					-1);
		g_free((gchar*)mms->type);
		g_free((gchar*)mms->caption);
		g_free((gchar*)mms->source);
		g_free((gchar*)mms->directory);
		g_free((gchar*)mms->user);
		g_free((gchar*)mms->pass);
		g_free((gchar*)mms->uid);
		g_free(mms);
		tmp = g_list_next(tmp);
		crosswire_tracker++;
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_entry2), 0);
	g_list_free(tmp2);

	/* local */
	gtk_list_store_clear(GTK_LIST_STORE(local_model));
	gtk_list_store_clear(GTK_LIST_STORE(module_box_local));
	tmp = tmp2 = mod_mgr_list_local_sources();
	while (tmp) {
		mms = (MOD_MGR_SOURCE *) tmp->data;
		gtk_list_store_append(GTK_LIST_STORE(local_model),
				      &iter);
		gtk_list_store_set(GTK_LIST_STORE(local_model), &iter,
				   COLUMN_TYPE,      mms->type,
				   COLUMN_CAPTION,   mms->caption,
				   COLUMN_SOURCE,    " ", // mms->source - eh.
				   COLUMN_DIRECTORY, mms->directory,
				   COLUMN_USER,      "",
				   COLUMN_PASS,      "",
				   COLUMN_UID,       "",
				   -1);
		gtk_list_store_append(GTK_LIST_STORE(module_box_local), &combo_iter);
		gtk_list_store_set(GTK_LIST_STORE(module_box_local),
					&combo_iter,
					0,
					(gchar*)mms->caption,
					-1);
		g_free((gchar*)mms->type);
		g_free((gchar*)mms->caption);
		g_free((gchar*)mms->source);
		g_free((gchar*)mms->directory);
		g_free((gchar*)mms->user);
		g_free((gchar*)mms->pass);
		g_free((gchar*)mms->uid);
		g_free(mms);
		tmp = g_list_next(tmp);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_entry1), 0);
	g_list_free(tmp2);

	return crosswire_index;
}


/******************************************************************************
 * Name
 *   clear_and_hide_progress_bar
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void clear_and_hide_progress_bar(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void
clear_and_hide_progress_bar(void)
{
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), "");
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh),0);
	gtk_widget_hide(progressbar_refresh);
}


/******************************************************************************
 * Name
 *   on_notebook1_switch_page
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_notebook1_switch_page(GtkNotebook * notebook,
 *				     GtkNotebookPage * page,
 *				     guint page_num, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void
on_notebook1_switch_page(GtkNotebook * notebook,
			 GtkNotebookPage * page,
			 guint page_num, gpointer user_data)
{
	gchar *str;
	GdkCursor *cursor;
	GdkDisplay *display;
	GdkWindow *window;
	gint x, y;
	GTimer *total;
	double d;

	total = g_timer_new();

	cursor = gdk_cursor_new(GDK_WATCH);
	display = gdk_display_get_default();
	window = gdk_display_get_window_at_pointer(display, &x, &y);

	gdk_window_set_cursor(window, cursor);
	gdk_display_sync(display);
	gdk_cursor_unref(cursor);

	current_page = page_num;
	clear_and_hide_progress_bar();

	switch (page_num) {
	case 0:
		break;
	case 1:
		mod_mgr_shut_down();
		mod_mgr_init(destination, FALSE, TRUE);
		break;
	case 2:
		/* we already cleared progress bar */
		break;
	case 3:
		if (!have_configs) {
			str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
					      _("Please Refresh"),
					      _("Your module list is not up to date!"));
			gui_generic_warning(str);
			g_free(str);
		}
		if (GTK_TOGGLE_BUTTON(radiobutton_dest)->active) {
			destination =
			    gtk_label_get_text(GTK_LABEL(label_home));
		} else {
			destination = settings.path_to_mods;
			// local = FALSE;
		}
		mod_mgr_shut_down();
		mod_mgr_init(destination, FALSE, TRUE);
		load_module_tree(GTK_TREE_VIEW(treeview), TRUE);
		break;
	case 4:
		if (GTK_TOGGLE_BUTTON(radiobutton_dest)->active) {
			destination =
			    gtk_label_get_text(GTK_LABEL(label_home));
		} else {
			destination = settings.path_to_mods;
			// local = FALSE;
		}
		mod_mgr_shut_down();
		main_update_module_lists();
		mod_mgr_init(destination, FALSE, TRUE);
		load_module_tree(GTK_TREE_VIEW(treeview2), FALSE);
		break;
	}
	gdk_window_set_cursor(window, NULL);

	g_timer_stop(total);
	d = g_timer_elapsed(total, NULL);
	GS_message(("total time is %f", d));
}


/******************************************************************************
 * Name
 *   on_radiobutton2_toggled
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_radiobutton2_toggled(GtkToggleButton * togglebutton,
 *			     gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void
on_radiobutton2_toggled(GtkToggleButton * togglebutton,
			gpointer user_data)
{
	if (togglebutton->active) {
		gtk_widget_show(button_refresh);
		if (remote_source)
		        g_free(remote_source);
		remote_source = g_strdup(gtk_combo_box_get_active_text(
					     GTK_COMBO_BOX(combo_entry2)));
		xml_set_value("Xiphos", "modmgr", "mod_mgr_source", "1");

	} else {
		gtk_widget_hide(button_refresh);
		gtk_widget_hide(progressbar_refresh);
		xml_set_value("Xiphos", "modmgr", "mod_mgr_source", "0");
	}
	settings.mod_mgr_source = togglebutton->active;
	xml_save_settings_doc(settings.fnconfigure);
}


void
on_radiobutton4_toggled(GtkToggleButton * togglebutton,
			gpointer user_data)
{
	xml_set_value("Xiphos", "modmgr", "mod_mgr_source",
		      (togglebutton->active ? "1" : "0"));
	settings.mod_mgr_source = togglebutton->active;
	xml_save_settings_doc(settings.fnconfigure);
}

/******************************************************************************
 * Name
 *   save_sources
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void save_sources(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void
save_sources(void)
{
	gchar *type = NULL;
	gchar *caption = NULL;
	gchar *source = NULL;
	gchar *directory = NULL;
	gchar *user = NULL;
	gchar *pass = NULL;
	gchar *uid = NULL;
	gboolean valid;
	GtkTreeIter iter;
	GtkTreeModel *remote_model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_remote));
	GtkTreeModel *local_model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_local));

	mod_mgr_clear_config();

	valid = gtk_tree_model_get_iter_first(remote_model, &iter);
	while (valid) {
		gtk_tree_model_get(remote_model, &iter,
				   COLUMN_TYPE, &type,
				   COLUMN_CAPTION, &caption,
				   COLUMN_SOURCE, &source,
				   COLUMN_DIRECTORY, &directory,
				   COLUMN_USER, &user,
				   COLUMN_PASS, &pass,
				   COLUMN_UID, &uid,
				   -1);

		mod_mgr_add_source("FTPSource",
				   type, caption, source, directory,
				   user, pass, uid);
		g_free(type);
		g_free(caption);
		g_free(source);
		g_free(directory);
		g_free(user);
		g_free(pass);
		g_free(uid);
		valid = gtk_tree_model_iter_next(remote_model, &iter);
	}

	valid = gtk_tree_model_get_iter_first(local_model, &iter);
	while (valid) {
		gtk_tree_model_get(local_model, &iter,
				   COLUMN_TYPE, &type,
				   COLUMN_CAPTION, &caption,
				   COLUMN_SOURCE, &source,
				   COLUMN_DIRECTORY, &directory,
				   COLUMN_USER, &user,
				   COLUMN_PASS, &pass,
				   COLUMN_UID, &uid,
				   -1);

		mod_mgr_add_source("DIRSource",
				   type, caption, "[local]", directory,
				   "", "", "");
		g_free(type);
		g_free(caption);
		g_free(source);
		g_free(directory);
		g_free(user);
		g_free(pass);
		g_free(uid);
		valid = gtk_tree_model_iter_next(local_model, &iter);
	}

	mod_mgr_reread_config();
	load_source_treeviews();
}


/******************************************************************************
 * Name
 *   create_fileselection_local_source
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void create_fileselection_local_source (void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
create_fileselection_local_source(void)
{
	GtkWidget *dialog;
    	gchar *filename;
	GtkTreeIter iter;
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_local));

	dialog =
		gtk_file_chooser_dialog_new ("Open File",
				      NULL,
				      GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
				      NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		GS_message(("%s",filename));
		gtk_list_store_append(GTK_LIST_STORE(model), &iter);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				   COLUMN_TYPE, "DIR",
				   COLUMN_CAPTION, filename,
				   COLUMN_SOURCE, "[local]",
				   COLUMN_DIRECTORY, filename,
				   COLUMN_USER, "",
				   COLUMN_PASS, "",
				   COLUMN_UID,  "",
				   -1);
		save_sources();
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}


/******************************************************************************
 * Name
 *   on_dialog_destroy
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_dialog_destroy(GtkObject * object, gpointer user_data)
 *
 * Description
 *   when the dialog is destroyed and changes have been made
 *   Sword is shut down and restarted
 *   module lists are cleared and refilled with current modules
 *   the sidebar module tree is reloaded
 *
 * Return value
 *   void
 */

static void
on_dialog_destroy(GtkObject * object, gpointer user_data)
{
	GList *tmp;
	gchar *str;

	if (working) return;
	working = TRUE;

	GS_message(("on_destroy"));
	if (remote_source) {
	        g_free(remote_source);
		remote_source = NULL;
	}

	mod_mgr_shut_down();
	sync_windows();
	if (have_changes && !first_time_user) {
		main_update_module_lists();
		main_load_module_tree(sidebar.module_list);
	}

	/* little bits of nonsense left behind by the engine. */
	str = g_strdup_printf("%s/dirlist", settings.homedir);
	if (mod_mgr_check_for_file(str))
		unlink(str);
	g_free(str);

	check_sync_repos();

	working = FALSE;
	is_running = FALSE;
	if (first_time_user) {
		/* no deeper analysis, first time around. */
		gtk_main_quit();
		return;
	}

	/*
	 * if we uninstalled a current module, substitute a live one
	 */
	if (!main_is_module(settings.MainWindowModule)) {
		if ((tmp = get_list(TEXT_LIST)))
			main_display_bible((char *)tmp->data, settings.currentverse);
		else {
			/* Zero Bibles is just not workable in Xiphos. */
			gui_generic_warning(_("You have uninstalled your last Bible.\n"
					      "Xiphos requires at least one."));
			main_shutdown_list();
			gui_open_mod_mgr_initial_run();
			main_init_lists();
			if (settings.havebible == 0) {
				gui_generic_warning(_("There are still no Bibles installed.\n"
						      "Xiphos cannot continue without one."));
				exit(1);
			}
		}
	}
	if (!main_is_module(settings.CommWindowModule)) {
		if ((tmp = get_list(COMM_LIST)))
			main_display_commentary((char *)tmp->data, settings.currentverse);
	}
	if (!main_is_module(settings.DictWindowModule)) {
		if ((tmp = get_list(DICT_LIST)))
			main_display_commentary((char *)tmp->data, settings.dictkey);
	}
	if (!main_is_module(settings.book_mod)) {
		if ((tmp = get_list(GBS_LIST)))
			main_display_book((char *)tmp->data, "/");	/* blank key */
	}
}


/******************************************************************************
 * Name
 *   response_close
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void response_close(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
response_close(void)
{
	gtk_widget_destroy(GTK_WIDGET(dialog));
	on_dialog_destroy(NULL, NULL);
}


/*
 * click callbacks, most using a wrapper.
 */
void
on_refresh_clicked(GtkButton * button, gpointer  user_data)
{
	response_refresh();
}

void
on_install_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(INSTALL);
}

void
on_remove_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(REMOVE);
}

void
on_archive_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(ARCHIVE);
}

void
on_index_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(FASTMOD);
}

void
on_delete_index_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(DELFAST);
}

void
on_load_sources_clicked(GtkButton * button, gpointer  user_data)
{
	if (mod_mgr_init_config_extras() == 0) {
		/* not quite identical to check_sync_repos(). */
		char index[10];
		gui_generic_warning(_("Standard remote sources have been loaded."));
		settings.mod_mgr_remote_source_index = load_source_treeviews();
		g_snprintf(index, 10, "%d", settings.mod_mgr_remote_source_index);
		xml_set_value("Xiphos", "modmgr", "mod_mgr_remote_source_index", index);
		set_controls_to_last_use();
	} else
		gui_generic_warning(_("Could not load standard sources from CrossWire."));
}

void
on_mod_mgr_intro_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new_with_markup
	    (NULL,	/* no need for a parent window */
	     GTK_DIALOG_DESTROY_WITH_PARENT,
	     GTK_MESSAGE_INFO,
	     GTK_BUTTONS_OK,
	     (user_data ? XI_FIRST_INSTALL : XI_GENERAL_INTRO));
	g_signal_connect_swapped (dialog, "response",
				  G_CALLBACK (gtk_widget_destroy),
				  dialog);
	gtk_widget_show(dialog);
}

void
on_cancel_clicked(GtkButton * button, gpointer  user_data)
{
	mod_mgr_terminate();
	sync_windows();
}


/******************************************************************************
 * Name
 *   on_mod_mgr_response
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_mod_mgr_response(GtkDialog * dialog, gint response_id, gpointer user_data)
 *
 * Description
 *   these are local defines at the top of this file.
 *
 * Return value
 *   void
 */

void
on_mod_mgr_response(GtkDialog * dialog,
		    gint response_id,
		    gpointer user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_CANCEL:
		mod_mgr_terminate();
		sync_windows();
		break;
	case GTK_RESPONSE_REFRESH:
		response_refresh();
		break;
	case GTK_RESPONSE_CLOSE:
		response_close();
		break;
	case GTK_RESPONSE_INSTALL:
		remove_install_wrapper(INSTALL);
		break;
	case GTK_RESPONSE_REMOVE:
		remove_install_wrapper(REMOVE);
		break;
	case GTK_RESPONSE_ARCHIVE:
		remove_install_wrapper(ARCHIVE);
		break;
	case GTK_RESPONSE_FASTMOD:
		remove_install_wrapper(FASTMOD);
		break;
	case GTK_RESPONSE_DELFAST:
		remove_install_wrapper(DELFAST);
		break;
	case GTK_RESPONSE_SOURCES:
		on_load_sources_clicked(NULL, NULL);
		break;
	case GTK_RESPONSE_INTRO:
		on_mod_mgr_intro_clicked(NULL, NULL);
		break;
	}
}


/******************************************************************************
 * Name
 *   on_button_add_local_clicked
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_button_add_local_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   add local source
 *
 * Return value
 *   void
 */

void
on_button_add_local_clicked(GtkButton * button,
		   gpointer user_data)
{
	if (!working)
	{
		working = TRUE;
		create_fileselection_local_source();
		working = FALSE;
	}
}


/******************************************************************************
 * Name
 *   on_button_remove_local_clicked
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_button_remove_local_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   remove local source
 *
 * Return value
 *   void
 */

void
on_button_remove_local_clicked(GtkButton * button,
		   gpointer user_data)
{
	gchar *name_string;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *caption = NULL;
	gchar *type = NULL;
	gchar *source = NULL;
	gchar *directory = NULL;
	gchar *user = NULL;
	gchar *pass = NULL;
	gchar *uid = NULL;
	gchar *str;
	GtkTreeModel *model;

	if (working) return;
	working = TRUE;

	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_local));
	if (!gtk_tree_selection_get_selected
		(selection, &model, &selected)){
			working = FALSE;
			return;
	}
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
			   COLUMN_TYPE, &type,
			   COLUMN_CAPTION, &caption,
			   COLUMN_SOURCE, &source,
			   COLUMN_DIRECTORY, &directory,
			   COLUMN_USER, &user,
			   COLUMN_PASS, &pass,
			   COLUMN_UID, &uid,
			   -1);
	name_string = caption;

	str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s|%s|%s|%s",
			      _("Remove the selected source"),
			      caption, type, source, directory);

	if (gui_yes_no_dialog(str, GTK_STOCK_DIALOG_WARNING)) {
		gtk_list_store_remove(GTK_LIST_STORE(model), &selected);
		save_sources();
	}
	g_free(type);
	g_free(caption);
	g_free(source);
	g_free(directory);
	g_free(user);
	g_free(pass);
	g_free(uid);
	g_free(str);

	working = FALSE;
}


/******************************************************************************
 * Name
 *   on_button_add_remote_clicked
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_button_add_remote_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   add remote source
 *
 * Return value
 *   void
 */

void
on_button_add_remote_clicked(GtkButton * button,
		   gpointer user_data)
{
	gint test;
	GS_DIALOG *dialog;
	GtkTreeIter iter;
	GString *str;
	GList *tmp, *tmp2;
	gboolean name_conflict = FALSE;
	MOD_MGR_SOURCE *mms;

	if (working) return;
	working = TRUE;

	str = g_string_new(NULL);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2),TRUE);
	gtk_widget_hide(button_refresh);

	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_remote));
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>",
			_("Enter a remote source"));
	dialog = gui_new_dialog();
	dialog->stock_icon = GTK_STOCK_DIALOG_INFO;
	dialog->label_top = str->str;
	dialog->label1 = _("Caption:");
	dialog->label2 = _("Type:");
	dialog->label3 = _("Host:");
	dialog->label4 = _("Directory:");
	dialog->label5 = _("User (optional):");
	dialog->label6 = _("Password (optional):");
	dialog->text1 = g_strdup("CrossWire");
	dialog->text2 = g_strdup("FTP");
	dialog->text3 = g_strdup("ftp.crosswire.org");
	dialog->text4 = g_strdup("/pub/sword/raw");
	dialog->cancel = TRUE;
	dialog->ok = TRUE;

	test = gui_gs_dialog(dialog);
	if (test != GS_OK) {
		g_free(dialog->text1);
		g_free(dialog->text2);
		g_free(dialog->text3);
		g_free(dialog->text4);
		g_free(dialog->text5);
		g_free(dialog->text6);
		g_free(dialog);
		g_string_free(str, TRUE);
		goto out;
	}

	for (tmp = tmp2 = mod_mgr_list_remote_sources();
	     tmp;
	     tmp = g_list_next(tmp)) {
		mms = (MOD_MGR_SOURCE *) tmp->data;
		if (!strcmp(mms->caption, dialog->text1)) {
			/* this can happen at most once */
			gui_generic_warning(_("A source by that name already exists."));
			name_conflict = TRUE;
		}
		g_free((gchar*)mms->type);
		g_free((gchar*)mms->caption);
		g_free((gchar*)mms->source);
		g_free((gchar*)mms->directory);
		g_free((gchar*)mms->user);
		g_free((gchar*)mms->pass);
		g_free((gchar*)mms->uid);
		g_free(mms);
	}
	g_list_free(tmp2);

	if (!name_conflict) {
		/* timestamped UID field */
		time_t now = time(NULL);
		struct tm *local = localtime(&now);
		/*
		 * who is the psychotic moron who gave us tm_year as "# yrs
		 * since 1900" and tm_mon as "# months since january, [0-11]"?
		 * flensing, trepanation, and hari-kari all apply.
		 */
		gchar *uid = g_strdup_printf("%d%02d%02d%02d%02d%02d",
					     local->tm_year+1900,
					     local->tm_mon+1, local->tm_mday,
					     local->tm_hour, local->tm_min,
					     local->tm_sec);

		gtk_list_store_append(GTK_LIST_STORE(model), &iter);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				   COLUMN_TYPE,      dialog->text2,
				   COLUMN_CAPTION,   dialog->text1,
				   COLUMN_SOURCE,    dialog->text3,
				   COLUMN_DIRECTORY, dialog->text4,
				   COLUMN_USER,      dialog->text5,
				   COLUMN_PASS,      dialog->text6,
				   COLUMN_UID,       uid,
				   -1);
		g_free(uid);
		save_sources();

		/* set the new item's index as active */
		for (test = 0, tmp = tmp2 = mod_mgr_list_remote_sources();
		     tmp;
		     tmp = g_list_next(tmp), ++test) {
			mms = (MOD_MGR_SOURCE *) tmp->data;
			if (!strcmp(mms->caption, dialog->text1)) {
				gtk_combo_box_set_active(
				    GTK_COMBO_BOX(combo_entry2), test);
			}
			g_free((gchar*)mms->type);
			g_free((gchar*)mms->caption);
			g_free((gchar*)mms->source);
			g_free((gchar*)mms->directory);
			g_free((gchar*)mms->user);
			g_free((gchar*)mms->pass);
			g_free((gchar*)mms->uid);
			g_free(mms);
		}
		g_list_free(tmp2);
	}

	if (remote_source)
	        g_free(remote_source);
	remote_source = g_strdup(dialog->text1);

	g_free(dialog->text1);
	g_free(dialog->text2);
	g_free(dialog->text3);
	g_free(dialog->text4);
	g_free(dialog->text5);
	g_free(dialog->text6);
	g_free(dialog);
	g_string_free(str, TRUE);

	sync_windows();

	mod_mgr_shut_down();
	mod_mgr_init(destination, FALSE, TRUE);

out:
	gtk_widget_show(button_refresh);
	working = FALSE;
}


/******************************************************************************
 * Name
 *   on_button_remove_remote_clicked
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_button_remove_remote_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   remove remote source
 *
 * Return value
 *   void
 */

void
on_button_remove_remote_clicked(GtkButton * button,
		   gpointer user_data)
{
	gint test;
	GS_DIALOG *yes_no_dialog;
	gchar *name_string;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *caption = NULL;
	gchar *type = NULL;
	gchar *source = NULL;
	gchar *directory = NULL;
	gchar *user = NULL;
	gchar *pass = NULL;
	gchar *uid = NULL;
	GtkTreeModel *model;
	GString *str;

	if (working) return;
	working = TRUE;

	str = g_string_new(NULL);

	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_remote));
	if (!gtk_tree_selection_get_selected
	    (selection, &model, &selected)){
	    	working = FALSE;
		return;
	}
	gtk_tree_model_get(model, &selected,
			   COLUMN_TYPE, &type,
			   COLUMN_CAPTION, &caption,
			   COLUMN_SOURCE, &source,
			   COLUMN_DIRECTORY, &directory,
			   COLUMN_USER, &user,
			   COLUMN_PASS, &pass,
			   COLUMN_UID, &uid,
			   -1);
	name_string = caption;

	yes_no_dialog = gui_new_dialog();
	yes_no_dialog->stock_icon = GTK_STOCK_DIALOG_WARNING;
	yes_no_dialog->title = _("Delete a remote source");
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s|%s|%s|%s",
			_("Remove the selected source"),
			caption, type, source, directory);
	yes_no_dialog->label_top = str->str;
	yes_no_dialog->yes = TRUE;
	yes_no_dialog->no = TRUE;

	test = gui_alert_dialog(yes_no_dialog);
	if (test == GS_YES) {
		gtk_list_store_remove(GTK_LIST_STORE(model), &selected);
		save_sources();
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_entry2), 0);
	}
	g_free(yes_no_dialog);
	g_free(type);
	g_free(caption);
	g_free(source);
	g_free(directory);
	g_free(user);
	g_free(pass);
	g_free(uid);
	g_string_free(str, TRUE);

	working = FALSE;
}


/******************************************************************************
 * Name
 *   on_treeview1_button_release_event
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   gboolean on_treeview1_button_release_event(GtkWidget * widget,
 *                           GdkEventButton * event, gpointer user_data)
 *
 * Description
 *   button release in main treeview
 *   change notebook page to 'sel' returned from selection
 *   show/hide dialog response buttons as needed
 *
 * Return value
 *   void
 */

gboolean
on_treeview1_button_release_event(GtkWidget * widget,
				  GdkEventButton * event,
				  gpointer user_data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeIter selected;
	gint sel;
	GtkTreeModel *model;

	if (working) return 0;
	working = TRUE;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview1));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));

	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   1, &sel, -1);
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected)) {
			gtk_notebook_set_current_page(GTK_NOTEBOOK
						      (notebook1), sel);
			switch (sel) {
			case 1:
				gtk_widget_hide(button_refresh);
				gtk_widget_hide(button_install);
				gtk_widget_hide(button_remove);
				gtk_widget_hide(button_arch);
				gtk_widget_hide(button_idx);
				gtk_widget_hide(button_delidx);
				if (first_time_user)
					gtk_widget_hide(button_load_sources);
				else
					gtk_widget_show(button_load_sources);
				break;
			case 2:
				if (GTK_TOGGLE_BUTTON(radiobutton2)->
				    active)
					gtk_widget_show(button_refresh);
				else
					gtk_widget_hide(button_refresh);
				gtk_widget_hide(button_install);
				gtk_widget_hide(button_remove);
				gtk_widget_hide(button_arch);
				gtk_widget_hide(button_idx);
				gtk_widget_hide(button_delidx);
				gtk_widget_hide(button_load_sources);
				break;
			case 3:
				if (GTK_TOGGLE_BUTTON(radiobutton2)->
				    active)
					gtk_widget_show(button_refresh);
				else
					gtk_widget_hide(button_refresh);
				gtk_widget_show(button_install);
				gtk_widget_hide(button_remove);
				gtk_widget_hide(button_arch);
				gtk_widget_hide(button_idx);
				gtk_widget_hide(button_delidx);
				gtk_widget_hide(button_load_sources);
				break;
			case 4:
				gtk_widget_show(button_remove);
				gtk_widget_show(button_arch);
				gtk_widget_show(button_idx);
				gtk_widget_show(button_delidx);
				gtk_widget_hide(button_load_sources);
				gtk_widget_hide(button_refresh);
				gtk_widget_hide(button_install);
				break;
			}

		}
	}

	working = FALSE;
	return FALSE;
}


static void
setup_treeview_main(GtkTreeView * tree_view)
{
	GtkTreeModel *model;

	model = create_model_to_first();
	gtk_tree_view_set_model(tree_view, model);
	add_columns_to_first(tree_view);
	gtk_tree_view_expand_all(tree_view);

	g_signal_connect(tree_view, "button_release_event",
			 G_CALLBACK(on_treeview1_button_release_event), NULL);
}

static void
setup_treeviews_local_remote(GtkTreeView * local, GtkTreeView * remote)
{
	GtkTreeModel *model;

	model = create_remote_source_treeview_model();
	gtk_tree_view_set_model(local, model);
	add_columns_to_remote_treeview(local);
	model = create_remote_source_treeview_model();
	gtk_tree_view_set_model(remote, model);
	add_columns_to_remote_treeview(remote);
}


static void
set_combobox(GtkComboBox * combo)
{
	GtkListStore *store;

	store = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_combo_box_set_model(combo, GTK_TREE_MODEL(store));
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo), 0);
}


static void
setup_dialog_action_area(GtkDialog * dialog)
{
	GtkWidget *dialog_action_area1 = GTK_DIALOG (dialog)->action_area;

	gtk_widget_show (dialog_action_area1);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

	/*
	 * response buttons
	 */
	g_signal_connect(button_close, "clicked", G_CALLBACK(response_close), NULL);
	g_signal_connect(button_refresh, "clicked", G_CALLBACK(on_refresh_clicked), NULL);
	g_signal_connect(button_install, "clicked", G_CALLBACK(on_install_clicked), NULL);
	g_signal_connect(button_remove, "clicked", G_CALLBACK(on_remove_clicked), NULL);
	g_signal_connect(button_arch, "clicked", G_CALLBACK(on_archive_clicked), NULL);
	g_signal_connect(button_idx, "clicked", G_CALLBACK(on_index_clicked), NULL);
	g_signal_connect(button_delidx, "clicked", G_CALLBACK(on_delete_index_clicked), NULL);
	g_signal_connect(button_load_sources, "clicked", G_CALLBACK(on_load_sources_clicked), NULL);
	g_signal_connect(button_intro, "clicked", G_CALLBACK(on_mod_mgr_intro_clicked), NULL);
	g_signal_connect(button_cancel, "clicked", G_CALLBACK(on_cancel_clicked), NULL);

	gtk_widget_show(button_close);
	on_mod_mgr_intro_clicked(NULL, NULL);
	sync_windows();
	sleep(1);
	on_mod_mgr_intro_clicked(NULL, (gpointer) 1);
}

static void
set_controls_to_last_use(void)
{
	/* local or remote source */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2),
				     settings.mod_mgr_source);
	/* local source */
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_entry1),
				 settings.mod_mgr_local_source_index);
	/* remote source */
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_entry2),
				 settings.mod_mgr_remote_source_index);
	/* destination */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton4),
				     settings.mod_mgr_destination);
}

static void
setup_ui_labels()
{
	gchar *str;

	have_changes = FALSE;
	str = g_strdup_printf("%s/%s", settings.homedir,
			      DOTSWORD "/InstallMgr/InstallMgr.conf");
	if (!mod_mgr_check_for_file(str)) {
		have_configs = FALSE;
		mod_mgr_init_config();
	}
	g_free(str);

	have_configs = TRUE;
	mod_mgr_init(NULL, TRUE, TRUE);
	str = g_strdup_printf("%s/%s", settings.homedir, DOTSWORD);
	gtk_label_set_text(GTK_LABEL(label_home), str);
	g_free(str);

	gtk_label_set_text(GTK_LABEL(label_system), settings.path_to_mods);
	if (g_access(settings.path_to_mods, W_OK) == -1) {
		GS_print(("%s is write protected\n",
			  (settings.path_to_mods
			   ? settings.path_to_mods
			   : "<null>")));
		gtk_widget_set_sensitive(label_system, FALSE);
		gtk_widget_set_sensitive(radiobutton4, FALSE);
	} else {
		gtk_widget_set_sensitive(label_system, TRUE);
		gtk_widget_set_sensitive(radiobutton4, TRUE);
	}
	create_pixbufs();
	load_source_treeviews();
}


static void
on_comboboxentry_local_changed(GtkComboBox *combobox, gpointer user_data)
{
	gint index = gtk_combo_box_get_active (GTK_COMBO_BOX(combo_entry1));
	settings.mod_mgr_local_source_index = index;
	gchar *index_str = g_strdup_printf("%d",index);
	xml_set_value("Xiphos", "modmgr", "mod_mgr_local_source_index", index_str);
	xml_save_settings_doc(settings.fnconfigure);
	g_free(index_str);
}

static void
on_comboboxentry_remote_changed(GtkComboBox *combobox, gpointer user_data)
{
	gint index = gtk_combo_box_get_active (GTK_COMBO_BOX(combo_entry2));
	settings.mod_mgr_remote_source_index = index;
	gchar *index_str = g_strdup_printf("%d",index);
	GS_message(("index = %d index_str = %s",index,index_str));
	xml_set_value("Xiphos", "modmgr", "mod_mgr_remote_source_index", index_str);
	xml_save_settings_doc(settings.fnconfigure);
	g_free(index_str);

	if (remote_source)
		g_free(remote_source);
	remote_source = g_strdup(gtk_entry_get_text(GTK_ENTRY(GTK_BIN(combobox)->child)));
}

static GtkWidget *
create_module_manager_dialog(gboolean first_run)
{
	gchar *glade_file;

	glade_file = gui_general_user_file ("module-manager.glade", FALSE);
	g_return_val_if_fail((glade_file != NULL), NULL);
	GS_message(("%s",glade_file));

	gxml = glade_xml_new (glade_file, "dialog", NULL);
	g_free (glade_file);
	g_return_val_if_fail ((gxml != NULL), NULL);
	dialog =  glade_xml_get_widget (gxml, "dialog");

	/* response buttons */
	button_close = glade_xml_get_widget (gxml, "button_close");
	GTK_WIDGET_SET_FLAGS (button_close, GTK_CAN_DEFAULT);
	button_cancel = glade_xml_get_widget (gxml, "button_cancel");
	button_refresh = glade_xml_get_widget (gxml, "button_refresh");
	button_install = glade_xml_get_widget (gxml, "button_install");
	button_remove = glade_xml_get_widget (gxml, "button_remove");
	button_arch = glade_xml_get_widget (gxml, "button_archive");
	button_idx = glade_xml_get_widget (gxml, "button_index");
	button_delidx = glade_xml_get_widget (gxml, "button_delete_index");
	button_load_sources = glade_xml_get_widget (gxml, "button_load_sources"); /* load sources */
	button_intro = glade_xml_get_widget (gxml, "button_view_intro");

	g_signal_connect(dialog, "destroy",
			 G_CALLBACK(on_dialog_destroy), NULL);
	if (first_run)
		setup_dialog_action_area(GTK_DIALOG (dialog));
	else
		g_signal_connect(dialog, "response",
				 G_CALLBACK(on_mod_mgr_response), NULL);

	/* progress bars */
	progressbar_refresh = glade_xml_get_widget (gxml, "progressbar1"); /* refresh */

	/* treeviews */
	treeview1 = glade_xml_get_widget (gxml, "treeview1");
	setup_treeview_main(GTK_TREE_VIEW(treeview1));

	treeview_local = glade_xml_get_widget (gxml, "treeview2");
	treeview_remote = glade_xml_get_widget (gxml, "treeview3");
	setup_treeviews_local_remote(GTK_TREE_VIEW(treeview_local), GTK_TREE_VIEW(treeview_remote));

	treeview = glade_xml_get_widget (gxml, "treeview4");

	treeview2 = glade_xml_get_widget (gxml, "treeview5");
	setup_treeview_install(GTK_TREE_VIEW(treeview));
	setup_treeview_maintenance(GTK_TREE_VIEW(treeview2));

	gtk_widget_set_has_tooltip (treeview, TRUE);
	g_signal_connect((gpointer) treeview,
			 "query-tooltip",
			 G_CALLBACK(query_tooltip), NULL);

	gtk_widget_set_has_tooltip (treeview2, TRUE);
	g_signal_connect((gpointer) treeview2,
			 "query-tooltip",
			 G_CALLBACK(query_tooltip), NULL);

	/* notebook */
	notebook1 = glade_xml_get_widget (gxml, "notebook1");
	g_signal_connect(notebook1, "switch_page",
			 G_CALLBACK(on_notebook1_switch_page), NULL);
	/* labels */
	label_home = glade_xml_get_widget (gxml, "label_home");
	label_system = glade_xml_get_widget (gxml, "label_sword_sys");

	/* sources buttons */
	button_add_local = glade_xml_get_widget (gxml, "button_add_local"); /* close */
	button_remove_local = glade_xml_get_widget (gxml, "button_remove_local"); /* close */
	button_add_remote = glade_xml_get_widget (gxml, "button_add_remote"); /* refresh */
	button_remove_remote = glade_xml_get_widget (gxml, "button_remove_remote"); /* install */
	g_signal_connect(button_add_local, "clicked",
			 G_CALLBACK(on_button_add_local_clicked), NULL);
	g_signal_connect(button_remove_local, "clicked",
			 G_CALLBACK(on_button_remove_local_clicked), NULL);
	g_signal_connect(button_add_remote, "clicked",
			 G_CALLBACK(on_button_add_remote_clicked), NULL);
	g_signal_connect(button_remove_remote, "clicked",
			 G_CALLBACK(on_button_remove_remote_clicked), NULL);

	/* combo box entrys */
	combo_entry1 = glade_xml_get_widget (gxml, "comboboxentry1"); /* local source */
	set_combobox(GTK_COMBO_BOX(combo_entry1));
	combo_entry2 = glade_xml_get_widget (gxml, "comboboxentry2"); /* remote source */
	set_combobox(GTK_COMBO_BOX(combo_entry2));

	/* radio buttons */
	radiobutton_source = glade_xml_get_widget (gxml, "radiobutton1"); /* local */
	radiobutton2 = glade_xml_get_widget (gxml, "radiobutton2"); /* remote */
	radiobutton_dest = glade_xml_get_widget (gxml, "radiobutton3"); /* homedir */
	radiobutton4 = glade_xml_get_widget (gxml, "radiobutton4"); /* homedir */
	setup_ui_labels();
	set_controls_to_last_use();
	g_signal_connect(radiobutton2, "toggled",
			 G_CALLBACK(on_radiobutton2_toggled), NULL);
	g_signal_connect(radiobutton4, "toggled",
			 G_CALLBACK(on_radiobutton4_toggled), NULL);
	g_signal_connect ((gpointer) combo_entry1, "changed",
                    G_CALLBACK (on_comboboxentry_local_changed),
                    NULL);
	g_signal_connect ((gpointer) combo_entry2, "changed",
                    G_CALLBACK (on_comboboxentry_remote_changed),
                    NULL);
	if (first_run)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2),TRUE);

	gtk_widget_hide(button_refresh);

	return dialog;
}


/******************************************************************************
 * Name
 *   gui_open_mod_mgr
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void gui_open_mod_mgr(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_open_mod_mgr(void)
{
	if (!is_running) {
		GtkWidget *dlg;
		dlg = create_module_manager_dialog(FALSE);
		set_window_icon(GTK_WINDOW(dlg));
		is_running = TRUE;
	} else
		gdk_window_raise(GTK_WIDGET(dialog)->window);
}


/******************************************************************************
 * Name
 *   gui_open_mod_mgr_initial_run
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void gui_open_mod_mgr_initial_run(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_open_mod_mgr_initial_run(void)
{
	GtkWidget *dlg;
	first_time_user = TRUE;
	dlg = create_module_manager_dialog(TRUE);
	set_window_icon(GTK_WINDOW(dlg));
	gtk_main();
	first_time_user = FALSE;
}

/******************************************************************************
 * Name
 *   gui_update_install_status
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *  void gui_update_install_status(glong total, glong done, const gchar * message)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_update_install_status(glong total, glong done,
			       const gchar * message)
{
	gchar *buf;

	buf = g_strdup_printf("%s: %s",current_mod,message);
	gui_set_progressbar_text(progressbar_refresh,buf);
	g_free(buf);
}


/******************************************************************************
 * Name
 *   gui_update_install_progressbar
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void gui_update_install_progressbar(gdouble fraction)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_update_install_progressbar(gdouble fraction)
{
	gui_set_progressbar_fraction(progressbar_refresh, fraction);
}
