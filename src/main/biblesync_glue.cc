/*
 * Xiphos Bible Study Tool
 * biblesync_glue.cc
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
#include <glib.h>
#include <glib/gstdio.h>

#include "gui/main_window.h"
#include "gui/widgets.h"
#include "gui/dialog.h"
#include "gui/xiphos.h"
#include "gui/preferences_dialog.h"
#include "gui/tabbed_browser.h"
#include "gui/utilities.h"

#include "main/biblesync_glue.h"
#include "main/display.hh"
#include "main/previewer.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/url.hh"

#include "biblesync/biblesync.hh"

#include "gui/debug_glib_null.h"

BibleSync *biblesync;

BSP_SpeakerMap speakers;

#define BSP (string) "BibleSync: "

/******************************************************************************
 * Name
 *  biblesync_navigate
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_navigate(cmd, speaker, bible, ref, alt, group, domain, info, dump)
 *
 * Description
 *   navigates per incoming BibleSync request, or handles
 *   announcement or error instead.
 *   used as a callback out of class BibleSync.
 *
 * Return value
 *   void
 */
void
biblesync_navigate(char cmd, string speaker_uuid,
		   string bible, string ref, string alt,
		   string group, string domain,
		   string info, string dump)
{
	bool speaker_display_update = false;
	bool is_module;
	string message;
	const char *real_name = NULL;

	// parameter overload usage.
	string &presence = alt;
	string &user = bible, &ipaddr = ref, &app = group, &device = domain;

	// lockout: prevent re-xmit of what we're processing.
	settings.bs_receiving = TRUE;

	switch (cmd) {

	// error
	case 'E':
		if (settings.bs_debug) {
			// recv error occurred: explanation in info, pkt in dump.
			message = info + "\n\n" + dump;
			gui_generic_warning((char *)message.c_str());
		}
		break;

	// mismatch
	case 'M':
		if (settings.bs_mismatch) {
			message = BSP + (string)_("Mismatched packet\n\n")
				+ info
				+ ((info.substr(0, 8) == "announce")
				   ? ("\nMessage: " + presence)
				   : ("\nBible: " + bible
				      + "; Ref: " + ref
				      + "; Alt: " + alt
				      + "\nGroup: " + group
				      + "; Domain: " + domain))
				+ (settings.bs_debug
				   ? ("\n\n" + (string)dump)
				   : "");
			gui_generic_warning((char *)message.c_str());
		}
		break;

	// announce
	case 'A':
		if (settings.bs_presence) {
			if (settings.bs_debug) {
				message = _("Presence announcement:\n\n") + dump;
				gui_generic_warning((char *)message.c_str());
			}
			gui_generic_warning((char *)presence.c_str());
		}
		break;

	// navigation
	case 'N':
		if (settings.bs_debug) {
			message = _("Navigation packet:\n\n") + dump;
			gui_generic_warning((char *)message.c_str());
		}

		is_module = backend->is_module(bible.c_str());
		real_name = main_get_name(bible.c_str());

		// if the offered name isn't known,
		// maybe it's an abbreviation for something we do know.
		if (!is_module && real_name) {
			bible = real_name;
			is_module = backend->is_module(bible.c_str());
		}

		// direct navigation, or via verse list?
		if (is_module &&
		    settings.bs_navdirect &&
		    (strpbrk(ref.c_str(), "-;,") == NULL)) // not a multi-ref
		{
			speakers[speaker_uuid].direct = "D";

			// by default, Xiphos can interpret param "group" as tab#.
			if (settings.bs_group_tab) {
				char tab = group.c_str()[0];
				if ((group.length() == 1) && // 1-char string
				    (tab >= '1') && (tab <= '9')) {
					tab -= '1'; // 0-based list

					// select tab if not already current, avoid screen flash.
					if (cur_passage_tab &&
					    (tab != gtk_notebook_page_num(
							GTK_NOTEBOOK(widgets.notebook_main),
							cur_passage_tab->page_widget))) {
						gui_select_nth_tab(tab);
					}
					settings.showtexts = 1; // make panes visible
					settings.showcomms = 1;
					settings.comm_showing = 1;
					gui_recompute_shows(FALSE);
				}
			}

			main_url_handler(((string) "sword://" + bible + "/" + ref).c_str(), TRUE);
		} else // use verse list: unknown bible, user pref, or multi-ref.
		{
			speakers[speaker_uuid].direct = "I";

			main_display_verse_list_in_sidebar(settings.currentverse,
							   (gchar *)bible.c_str(), (gchar *)ref.c_str());

			if (!is_module) {
				message = BSP + _("Unknown module ") + bible + _(".\nNavigated indirectly using verse list.");
				gui_generic_warning((char *)message.c_str());
			}
		}

		speakers[speaker_uuid].ref = bible + " " + ref;
		speaker_display_update = true;

		gui_set_statusbar((BSP + speakers[speaker_uuid].user + ": " + speakers[speaker_uuid].ref).c_str());
		break;

	// new speaker discovery.
	case 'S':
		XI_message(("new speaker: key [%s], u [%s], ip [%s], a [%s], d [%s]",
			    speaker_uuid.c_str(), user.c_str(),
			    ipaddr.c_str(), app.c_str(), device.c_str()));

		unsigned int old_speakers_size, new_speakers_size;

		old_speakers_size = speakers.size();

		speakers[speaker_uuid].uuid = speaker_uuid;
		speakers[speaker_uuid].user = user;
		speakers[speaker_uuid].direct = "";
		speakers[speaker_uuid].ref = "";
		speakers[speaker_uuid].ipaddr = ipaddr;
		speakers[speaker_uuid].app = app;
		speakers[speaker_uuid].device = device;

		new_speakers_size = speakers.size();

		if (biblesync->getMode() == BSP_MODE_SPEAKER) {
			speakers[speaker_uuid].listen = false;
		} else {
			switch (settings.bs_listen_set) {
			case 0:
				// listen to the 1st, initially ignore later ones.
				if ((old_speakers_size == 0) && (new_speakers_size == 1)) {
					speakers[speaker_uuid].listen = true;
					gui_generic_warning(_((BSP + "Listening to " + user + ".").c_str()));
				} else {
					speakers[speaker_uuid].listen = false;
					if ((old_speakers_size == 1) && (new_speakers_size == 2)) {
						gui_generic_warning(_((BSP + "More than 1 speaker.").c_str()));
					}
					// but we stop announcing after the 2nd.
				}
				break;

			case 1:
				// listen to all.  announce only the first.
				speakers[speaker_uuid].listen = true;
				if ((old_speakers_size == 0) && (new_speakers_size == 1)) {
					gui_generic_warning(_((BSP + "Listening to " + user + ".").c_str()));
				}
				break;

			case 2:
				// listen to none.  announce none.
				speakers[speaker_uuid].listen = false;
				break;
			}
		}

		biblesync->listenToSpeaker(speakers[speaker_uuid].listen, speaker_uuid);
		speaker_display_update = true;
		break;

	// dead speaker -- timed out from lack of beacons.
	case 'D':
		XI_message(("dead speaker: key [%s]", speaker_uuid.c_str()));
		speakers.erase(speaker_uuid);
		speaker_display_update = true;
		break;

	default:
		message = _("ERROR: unknown BibleSync indicator: ")
			+ cmd
			+ (string) ".\n"
			+ (string)_("Other indications are:")
			+ "\nbible: " + bible
			+ "\nref: " + ref
			+ "\nalt: " + alt
			+ "\ngroup: " + group
			+ "\ndomain: " + domain
			+ "\ninfo: " + info
			+ "\ndump:\n" + dump;
		gui_generic_warning((char *)message.c_str());
		break;
	}

	// speakers change && preferences are open => re-display speakers.
	if (speaker_display_update && settings.display_prefs) {
		biblesync_update_speaker();
	}

	// unlock.
	settings.bs_receiving = FALSE;
}

/******************************************************************************
 * Name
 *  biblesync_compare_speaker
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   static int biblesync_compare_speaker(BSP_Speaker *left, BSP_Speaker *right)
 *
 * Description
 *   qsort comparator for deciding lexical ordering of 2 BSP_Speakers.
 *   dives down to c_str() because the function must return an integer.
 *   order of dominance: user, ipaddr, app+version, device, uuid.
 *
 * Return value
 *   void
 */
int biblesync_compare_speaker(const void *Lvoid, const void *Rvoid)
{
	const BSP_Speaker *L = *(const BSP_Speaker **)Lvoid;
	const BSP_Speaker *R = *(const BSP_Speaker **)Rvoid;
	int retval;

	if ((retval = L->user.compare(R->user)) == 0)
		if ((retval = L->ipaddr.compare(R->ipaddr)) == 0)
			if ((retval = L->app.compare(R->app)) == 0)
				if ((retval = L->device.compare(R->device)) == 0)
					retval = L->uuid.compare(R->uuid);
	return retval;
}

//
// enumerator for column identification in speaker list
//
enum {
	COLUMN_LISTEN,  // checkbox
	COLUMN_USER,    // user
	COLUMN_DIRECT,  // directly/indirectly navigated
	COLUMN_NAV,     // last navigation reference received
	COLUMN_ABOUT,   // invisible: other identifying info (tooltip)
	COLUMN_UUID,    // invisible: uuid
	COLUMN_VISIBLE, // gtk magic?
	NUM_COLUMNS
};

/******************************************************************************
 * Name
 *  listen_toggled
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   static void listen_toggled(GtkCellRendererToggle *cell,
 *				gchar *path_str,
 *				gpointer data)
 *
 * Description
 *   react to user's toggle of listen checkbox.
 *
 * Return value
 *   void
 */
static void
listen_toggled(GtkCellRendererToggle *cell,
	       gchar *path_str,
	       gpointer data)
{
	if (biblesync->getMode() == BSP_MODE_SPEAKER) {
		gui_generic_warning(_((BSP + "Speaker listens to none.").c_str()));
	} else if (settings.bs_listen_set == 0) // selective
	{
		GtkTreeView *treeview = (GtkTreeView *)data;
		GtkTreeModel *model = gtk_tree_view_get_model(treeview);
		GtkTreeIter iter;
		GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
		gboolean listen = 27;
		gchar *uuid = NULL;

		// get toggled iter
		gtk_tree_model_get_iter(model, &iter, path);
		gtk_tree_model_get(model, &iter,
				   COLUMN_LISTEN, &listen,
				   COLUMN_UUID, &uuid,
				   -1);

		// negate
		listen ^= 1;

		// set new value
		gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				   COLUMN_LISTEN, listen,
				   -1);
		speakers[(string)uuid].listen = listen;
		biblesync->listenToSpeaker(listen, (string)uuid);
		g_free(uuid);

		// clean up
		gtk_tree_path_free(path);
	} else // all or nothing
	{
		gui_generic_warning(_((BSP + "Not listening selectively.").c_str()));
	}
}

/******************************************************************************
 * Name
 *  query_tooltip
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void query_tooltip()
 *
 * Description
 *   find and show the identifying info for the selected user.
 *
 * Return value
 *   void
 */
static gboolean query_tooltip(GtkWidget *widget,
			      gint x,
			      gint y,
			      gboolean keyboard_mode,
			      GtkTooltip *tooltip,
			      gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeIter iter;
	gchar *about;

	if (!gtk_tree_view_get_tooltip_context((GtkTreeView *)widget,
					       &x, &y,
					       keyboard_mode,
					       &model, &path, &iter))
		return FALSE;

	if (gtk_tree_model_iter_has_child(model, &iter)) {
		gtk_tree_path_free(path);
		return FALSE;
	}

	gtk_tree_model_get(model, &iter, COLUMN_ABOUT, &about, -1);
	gtk_tooltip_set_text(tooltip, about);
	gtk_tree_view_set_tooltip_cell((GtkTreeView *)widget,
				       tooltip, path,
				       NULL, NULL);
	gtk_tree_path_free(path);
	g_free(about);
	return TRUE;
}

/******************************************************************************
 * Name
 *  biblesync_set_clear_all_listen
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_set_clear_all_listen(gboolean listen)
 *
 * Description
 *   turn on/off all listeners.
 *   calls biblesync_update_speaker() to refresh display.
 *
 * Return value
 *   void
 */
void biblesync_set_clear_all_listen(gboolean listen)
{
	BSP_SpeakerMapIterator object;

	for (object = speakers.begin(); object != speakers.end(); ++object) {
		// update our app status, and tell the underlying protocol class.
		object->second.listen = listen;
		biblesync->listenToSpeaker(listen, object->second.uuid);
	}

	biblesync_update_speaker();
}

/******************************************************************************
 * Name
 *  biblesync_update_speaker
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_update_speaker()
 *
 * Description
 *   push the updated speaker set into the preferences widget.
 *
 * Return value
 *   void
 */
void biblesync_update_speaker()
{
	// need the widget in which the list should appear.
	if (speaker_window == NULL)
		return;

	unsigned int size = speakers.size();
	BSP_Speaker **array = g_new(BSP_Speaker *, size);
	unsigned int i = 0;

	// extract map entries into a new array that's more accessible.
	// what comes from the std::map is probably UUID-sorted, yuck.
	// this provides re-sort in user|ipaddr|app|device order: human-useful.
	for (BSP_SpeakerMapIterator object = speakers.begin();
	     object != speakers.end();
	     ++object) {
		array[i++] = &(object->second);
	}
	qsort(array, size, sizeof(BSP_Speaker *), biblesync_compare_speaker);

	//
	// window setup borrowed/modified from sidebar search results creation.
	//

	// GtkTreeSelection *selection;
	GtkListStore *model_speakers =
	    gtk_list_store_new(NUM_COLUMNS,
			       // listen       user
			       G_TYPE_BOOLEAN, G_TYPE_STRING,
			       // direct       ref
			       G_TYPE_STRING, G_TYPE_STRING,
			       // information  uuid
			       G_TYPE_STRING, G_TYPE_STRING,
			       // visibility
			       G_TYPE_BOOLEAN);

	if (speaker_list)
		gtk_widget_destroy(speaker_list); // destroy old to create new.

	speaker_list =
	    gtk_tree_view_new_with_model(GTK_TREE_MODEL(model_speakers));
	gtk_widget_show(speaker_list);
	gtk_container_add(GTK_CONTAINER(speaker_window), speaker_list);
#if !GTK_CHECK_VERSION(3, 10, 0)
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(speaker_list), TRUE);
#endif
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(speaker_list), TRUE);

	//
	// column setup
	// borrowed/modified from mod.mgr module list creation.
	//
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkWidget *image;

	/* -- toggle choice -- */
	renderer = gtk_cell_renderer_toggle_new();
	g_signal_connect(renderer, "toggled", G_CALLBACK(listen_toggled), speaker_list);

	column = gtk_tree_view_column_new();
	image =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_image_new_from_icon_name("emblem-ok-symbolic", GTK_ICON_SIZE_MENU);
#else
	    gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_MENU);
#endif

	gtk_widget_show(image);
	gtk_widget_set_tooltip_text(image,
				    _("Check the box to listen to this Speaker"));
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "active", COLUMN_LISTEN,
					    "visible", COLUMN_VISIBLE, NULL);
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_min_width(GTK_TREE_VIEW_COLUMN(column), 25);
	gtk_tree_view_append_column(GTK_TREE_VIEW(speaker_list), column);

	/* -- column for user name -- */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Available Speakers"), renderer,
							  "text", COLUMN_USER, NULL);
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_GROW_ONLY);
	gtk_tree_view_column_set_min_width(GTK_TREE_VIEW_COLUMN(column), 200);
	gtk_tree_view_append_column(GTK_TREE_VIEW(speaker_list), column);

	/* -- column for direct/indirect -- */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("D/I"), renderer,
							  "text", COLUMN_DIRECT, NULL);
	image =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_image_new_from_icon_name("emblem-default", GTK_ICON_SIZE_MENU);
#else
	    gtk_image_new_from_stock("gtk-yes", GTK_ICON_SIZE_MENU); // XXX
#endif
	gtk_widget_show(image);
	gtk_widget_set_tooltip_text(image,
				    _("Last navigation was Direct or Indirect"));
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_min_width(GTK_TREE_VIEW_COLUMN(column), 25);
	gtk_tree_view_append_column(GTK_TREE_VIEW(speaker_list), column);

	/* -- column for recent nav -- */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Last Navigation"), renderer,
							  "text", COLUMN_NAV, NULL);
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_GROW_ONLY);
	gtk_tree_view_column_set_min_width(GTK_TREE_VIEW_COLUMN(column), 100);
	gtk_tree_view_append_column(GTK_TREE_VIEW(speaker_list), column);

	/* -- identifying info (invisible) -- */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("About"), renderer,
							  "text", COLUMN_ABOUT, NULL);
	gtk_tree_view_column_set_visible(column, FALSE); // not shown
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_min_width(GTK_TREE_VIEW_COLUMN(column), 2);
	gtk_tree_view_append_column(GTK_TREE_VIEW(speaker_list), column);

	/* -- uuid (invisible) -- */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("UUID"), renderer,
							  "text", COLUMN_UUID, NULL);
	gtk_tree_view_column_set_visible(column, FALSE); // not shown
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_min_width(GTK_TREE_VIEW_COLUMN(column), 2);
	gtk_tree_view_append_column(GTK_TREE_VIEW(speaker_list), column);
	// end of column setup.

	gtk_widget_set_has_tooltip(speaker_list, TRUE);
	g_signal_connect((gpointer)speaker_list,
			 "query-tooltip",
			 G_CALLBACK(query_tooltip), NULL);

	GtkTreeIter iter;
	string identifying_info;

	// fill it with the user list.
	for (i = 0; i < size; ++i) {
		BSP_Speaker *object = array[i];
		identifying_info = (string)_("IP address: ") + object->ipaddr + (string)_("\nApplication: ") + object->app + (string)_("\nDevice: ") + object->device + (string)_("\nUUID: ") + object->uuid;

		gtk_list_store_append(model_speakers, &iter);
		gtk_list_store_set(model_speakers, &iter,
				   COLUMN_LISTEN, object->listen,
				   COLUMN_USER, object->user.c_str(),
				   COLUMN_DIRECT, object->direct.c_str(),
				   COLUMN_NAV, object->ref.c_str(),
				   COLUMN_ABOUT, identifying_info.c_str(),
				   COLUMN_UUID, object->uuid.c_str(),
				   COLUMN_VISIBLE, TRUE,
				   -1);
	}

	g_free(array);
}

/******************************************************************************
 * Name
 *  biblesync_mode_select
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_mode_select(int m)
 *
 * Description
 *   checks incoming argument and assigns mode appropriately.
 *   starts receiver if we didn't disable.
 *
 * Return value
 *   void
 */
int
biblesync_mode_select(int m, char *p)
{
	BibleSync_mode mode, old_mode = biblesync->getMode();

	switch (m) {
	case 0:
		mode = BSP_MODE_DISABLE;
		break;
	case 1:
		mode = BSP_MODE_PERSONAL;
		break;
	case 2:
		mode = BSP_MODE_SPEAKER;
		break;
	case 3:
		mode = BSP_MODE_AUDIENCE;
		break;
	default:
		mode = N_BSP_MODE;
		break; // error
	}
	if (mode != N_BSP_MODE) {
		if (((mode = biblesync->setMode(mode, biblesync_navigate, p)) != BSP_MODE_DISABLE) &&
		    (old_mode == BSP_MODE_DISABLE)) {
			// whenever we move to any active mode, we must start receiver.
			// glib will stop it on its own any time we go back to disable.
			g_timeout_add(1000, (GSourceFunc)BibleSync::Receive, biblesync);
		}

		switch (mode) {
		case BSP_MODE_DISABLE:
			m = 0;
			break;
		case BSP_MODE_PERSONAL:
			m = 1;
			break;
		case BSP_MODE_SPEAKER:
			m = 2;
			break;
		case BSP_MODE_AUDIENCE:
			m = 3;
			break;
		default:
			m = 99;
			break;
		}
	}

	return m;
}

/******************************************************************************
 * Name
 *  biblesync_personal
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_personal()
 *
 * Description
 *   determines whether BibleSync is set in Personal mode.
 *
 * Return value
 *   int
 */
int biblesync_personal()
{
	return biblesync->getMode() == BSP_MODE_PERSONAL;
}

#if 0
// unneeded at this time. disabled to silence cppcheck.
/******************************************************************************
 * Name
 *  biblesync_active
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_active()
 *
 * Description
 *   determines whether BibleSync is active or disabled.
 *
 * Return value
 *   int
 */
int biblesync_active()
{
	return biblesync->getMode() != BSP_MODE_DISABLE;
}
#endif

/******************************************************************************
 * Name
 *  biblesync_active_xmit_allowed
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_active_xmit_allowed()
 *
 * Description
 *   determines whether BibleSync is alive and this user may send.
 *
 * Return value
 *   int
 */
int biblesync_active_xmit_allowed()
{
	return biblesync->getMode() == BSP_MODE_PERSONAL ||
	       biblesync->getMode() == BSP_MODE_SPEAKER;
}

/******************************************************************************
 * Name
 *  biblesync_get_passphrase
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   const char *biblesync_get_passphrase()
 *
 * Description
 *   retrieves the currently-set session passphrase.
 *
 * Return value
 *   void
 */
const char *biblesync_get_passphrase()
{
	static string phrase;

	phrase = biblesync->getPassphrase();
	return phrase.c_str();
}

/******************************************************************************
 * Name
 *  biblesync_transmit_verse_list
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   const char *biblesync_transmit_verse_list()
 *
 * Description
 *   ships out a multi-ref from verse list handler.
 *
 * Return value
 *   void
 */
void biblesync_transmit_verse_list(char *modname, char *vlist)
{
	// see comment below in biblesync_prep_and_xmit() on abbreviation use.
	const char *abbreviation = main_get_abbreviation(modname);

	biblesync->Transmit(BSP_SYNC,
			    (abbreviation
				 ? (string)abbreviation
				 : (string)modname),
			    (string)vlist);
	// alt, group, & domain defaulted.
}

/******************************************************************************
 * Name
 *  biblesync_privacy
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   const char *biblesync_privacy(gboolean)
 *
 * Description
 *   en/disables privacy in personal mode (TTL 0).
 *
 * Return value
 *   void
 */
void biblesync_privacy(gboolean privacy)
{
	biblesync->setPrivate(privacy);
}

/******************************************************************************
 * Name
 *   biblesync_prep_and_xmit()
 *
 * Synopsis
 *   #include "main/sword.h"
 *   void biblesync_prep_and_xmit(void)
 *
 * Description
 *   regular xmit of user's current nav point.
 *
 * Return value
 *   void
 */
void biblesync_prep_and_xmit(const char *mod_name, const char *key)
{
	if (!settings.bs_receiving && // no re-xmit of recv'd nav.
	    ((biblesync->getMode() == BSP_MODE_PERSONAL) ||
	     (biblesync->getMode() == BSP_MODE_SPEAKER)))
	// audience does not xmit nav.
	{
		string group = "1"; // default if determination fails.
		gint pagenum = (cur_passage_tab
				    ? gtk_notebook_page_num(
					  GTK_NOTEBOOK(widgets.notebook_main),
					  cur_passage_tab->page_widget)
				    : 1);

		if ((pagenum != -1) && (pagenum < 9)) {
			char pchar[2];
			pchar[0] = pagenum + '1'; // 0-based list.
			pchar[1] = '\0';
			group = (string)pchar;
		}

		// ** this key is not to be freed. **
		char *osis_key = (char *)main_get_osisref_from_key(mod_name, key);
		const char *abbreviation = main_get_abbreviation(mod_name);

		// => longstanding behavioral premise of the Internet:
		// - be conservative in what you xmit.
		// - be liberal in what you recv.

		// offer a common Bible abbreviation if it's available.
		// this is because we in Sword have very inconvenient
		// module names for what are, in non-English locales,
		// perfectly ordinary, locally common Bible names.
		// interoperability with non-Sword apps requires that
		// we attempt to be locally friendly in name use.

		biblesync->Transmit(BSP_SYNC,
				    (string)(abbreviation
						 ? abbreviation
						 : mod_name),
				    (string)osis_key,
				    (string) "",
				    group);
	}
}
