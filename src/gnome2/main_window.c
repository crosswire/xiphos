/*
 * Xiphos Bible Study Tool
 * main_window.c - main window gui
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

#include "xiphos_html/xiphos_html.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/xml.h"
#include "main/search_dialog.h"
#include "main/url.hh"
#include "main/biblesync_glue.h"

#include "gui/xiphos.h"
#include "gui/main_window.h"
#include "gui/main_menu.h"
#include "gui/sidebar.h"
#include "gui/utilities.h"
#include "gui/bibletext.h"
#include "gui/parallel_view.h"
#include "gui/commentary.h"
#include "gui/gbs.h"
#include "gui/dialog.h"
#include "gui/find_dialog.h"
#include "gui/dictlex.h"
#include "gui/search_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/bibletext_dialog.h"
#include "gui/bookmark_dialog.h"
#include "gui/search_dialog.h"
#include "gui/navbar_versekey.h"
#include "gui/tabbed_browser.h"
#include "gui/widgets.h"
#include "gui/tabbed_browser.h"
#include "gui/menu_popup.h"
#include "gui/preferences_dialog.h"

#include "editor/slib-editor.h"

#include "gui/debug_glib_null.h"

/* X keyboard #definitions, to handle shortcuts */
/* we must define the categories of #definitions we need. */
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

WIDGETS widgets;

extern gboolean shift_key_pressed;

static GtkWidget *nav_toolbar;
static int main_window_created = FALSE;

/******************************************************************************
 * Name
 *  gui_show_hide_texts
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_hide_texts(gboolean choice)
 *
 * Description
 *    Show/hide bible texts
 *
 * Return value
 *   void
 */

void gui_show_hide_texts(gboolean choice)
{
	settings.showtexts = choice;
	gui_tab_set_showtexts(choice);
	gui_set_tab_label(settings.currentverse, TRUE);
	if (choice == FALSE) {
		if (main_window_created)
			gtk_widget_hide(widgets.vpaned);
		xml_set_value("Xiphos", "misc", "showtexts", "0");
	} else {
		if (main_window_created) {
			gtk_widget_show(widgets.vpaned);
			gtk_widget_show(nav_toolbar);
		}
		xml_set_value("Xiphos", "misc", "showtexts", "1");
	}
	/*if (main_window_created)
	   gui_set_bible_comm_layout(); */
}

/******************************************************************************
 * Name
 *  gui_show_hide_preview
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_hide_preview(gboolean choice)
 *
 * Description
 *    Show/hide bible texts
 *
 * Return value
 *   void
 */

void gui_show_hide_preview(gboolean choice)
{
	settings.showpreview = choice;
	gui_tab_set_showpreview(choice);
	if (choice == FALSE) {
		if (main_window_created)
			gtk_widget_hide(widgets.box_side_preview);
		gtk_widget_hide(widgets.vbox_previewer);
		xml_set_value("Xiphos", "misc", "showpreview", "0");
	} else {
		if (main_window_created) {
			if (settings.show_previewer_in_sidebar)
				gtk_widget_show(widgets.box_side_preview);
			else
				gtk_widget_show(widgets.vbox_previewer);
		}

		xml_set_value("Xiphos", "misc", "showpreview", "1");
	}
}

/******************************************************************************
 * Name
 *  gui_show_hide_comms
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_hide_comms(gboolean choice)
 *
 * Description
 *    Show/hide Commentaries
 *
 * Return value
 *   void
 */

void gui_show_hide_comms(gboolean choice)
{
	settings.showcomms = choice;
	gui_tab_set_showcomms(choice);
	gui_set_tab_label(settings.currentverse, TRUE);
	if (choice == FALSE) {
		if (main_window_created)
			gtk_widget_hide(widgets.notebook_comm_book);
		xml_set_value("Xiphos", "misc", "showcomms", "0");
	} else {
		if (main_window_created)
			gtk_widget_show(widgets.notebook_comm_book);
		xml_set_value("Xiphos", "misc", "showcomms", "1");
	}
	if (main_window_created)
		gui_set_bible_comm_layout();
}

/******************************************************************************
 * Name
 *  gui_show_hide_dicts
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_hide_dicts(gboolean choice)
 *
 * Description
 *    Show/hide Dictionaries-Lexicons
 *
 * Return value
 *   void
 */

void gui_show_hide_dicts(gboolean choice)
{
	settings.showdicts = choice;
	gui_tab_set_showdicts(choice);
	gui_set_tab_label(settings.currentverse, TRUE);
	if (choice == FALSE) {
		if (main_window_created)
			gtk_widget_hide(widgets.box_dict);
		xml_set_value("Xiphos", "misc", "showdicts", "0");
	} else {
		if (main_window_created)
			gtk_widget_show(widgets.box_dict);
		xml_set_value("Xiphos", "misc", "showdicts", "1");
	}
	if (main_window_created)
		gui_set_bible_comm_layout();
}

/******************************************************************************
 * Name
 *  gui_set_bible_comm_layout
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_set_bible_comm_layout(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_set_bible_comm_layout(void)
{
	gtk_paned_set_position(GTK_PANED(widgets.hpaned),
			       settings.biblepane_width);
	gtk_paned_set_position(GTK_PANED(widgets.vpaned),
			       settings.biblepane_height);
	gtk_paned_set_position(GTK_PANED(widgets.vpaned2),
			       settings.commpane_height);

	if ((settings.showcomms == TRUE) || (settings.showdicts == TRUE)) {
		gtk_widget_show(widgets.vpaned2);
	}

	gtk_paned_set_position(GTK_PANED(widgets.hpaned),
			       (settings.showtexts
				    ? settings.biblepane_width
				    : 0));

	gtk_paned_set_position(GTK_PANED(widgets.vpaned2),
			       (settings.showcomms
				    ? settings.commpane_height
				    : 0));

	gtk_paned_set_position(GTK_PANED(widgets.vpaned2),
			       (settings.showdicts
				    ? settings.commpane_height
				    : settings.gs_height));

	if ((settings.showcomms == FALSE) && (settings.showdicts == FALSE)) {
		gtk_widget_hide(widgets.vpaned2);
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       settings.gs_width);
	}

	if ((settings.showcomms == TRUE) || (settings.showdicts == TRUE)) {
		gtk_paned_set_position(GTK_PANED(widgets.hpaned),
				       settings.biblepane_width);
	}
	if (((settings.showcomms == FALSE) && (settings.showtexts == FALSE)) || ((settings.comm_showing == FALSE) && (settings.showtexts == FALSE)))
		gtk_widget_hide(nav_toolbar);
	else
		gtk_widget_show(nav_toolbar);

	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book),
				      (settings.comm_showing ? 0 : 1));
}

/******************************************************************************
 * Name
 *  gui_change_window_title
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_change_window_title(gchar * module_name)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_change_window_title(gchar *module_name)
{
	gchar *title;

	if (cur_passage_tab) {
		/* borrowed from tabbed_browser.c:pick_tab_label() */
		/* echo the current tab's module (full name) in title bar */
		if (cur_passage_tab->showtexts || cur_passage_tab->comm_showing) {
			title =
			    (cur_passage_tab->showtexts ? cur_passage_tab->text_mod : (cur_passage_tab->commentary_mod ? cur_passage_tab->commentary_mod : "[no commentary]"));
		} else {
			title = (cur_passage_tab->showcomms
				     ? (cur_passage_tab->book_mod
					    ? cur_passage_tab->book_mod
					    : "[no book]")
				     : (cur_passage_tab->dictlex_mod
					    ? cur_passage_tab->dictlex_mod
					    : "[no dict]"));
		}
	} else
		title = module_name;

	/*
	 * set program title to current module name
	 */
	title = g_strdup(main_get_module_description(title));
	if (!title)
		title =
		    g_strdup(main_get_module_description(settings.MainWindowModule));
	title =
	    g_strdup_printf("%s - %s", (title ? title : "[no title]"),
			    settings.program_title);
	gtk_window_set_title(GTK_WINDOW(widgets.app), title);
	g_free(title);
}

static gboolean delete_event(GtkWidget *widget,
			     GdkEvent *event, gpointer user_data)
{
	on_quit_activate(NULL, NULL);
	return TRUE;
}

/******************************************************************************
 * Name
 *   on_epaned_button_release_event
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   gboolean on_epaned_button_release_event(GtkWidget * widget,
 *			GdkEventButton * event, gpointer user_data)
 *
 * Description
 *    get and store pane sizes
 *
 * Return value
 *   gboolean
 */

static gboolean epaned_button_release_event(GtkWidget *widget,
					    GdkEventButton *event,
					    gpointer user_data)
{
	gint panesize;
	gchar layout[80];

	panesize = gtk_paned_get_position(GTK_PANED(widget));

	if (panesize > 15) {
		if (!strcmp((gchar *)user_data, "epaned")) {
			settings.sidebar_width = panesize;
			sprintf(layout, "%d", settings.sidebar_width);
			xml_set_value("Xiphos", "layout",
				      "shortcutbar", layout);
		}
		if (!strcmp((gchar *)user_data, "vpaned")) {
			settings.biblepane_height = panesize;
			sprintf(layout, "%d", settings.biblepane_height);
			xml_set_value("Xiphos", "layout",
				      "bibleheight", layout);
		}
		if (!strcmp((gchar *)user_data, "vpaned2")) {
			settings.commpane_height = panesize;
			sprintf(layout, "%d", settings.commpane_height);
			xml_set_value("Xiphos", "layout",
				      "commentaryheight", layout);
		}
		if (!strcmp((gchar *)user_data, "hpaned1")) {
			settings.biblepane_width = panesize;
			sprintf(layout, "%d", settings.biblepane_width);
			xml_set_value("Xiphos", "layout",
				      "textpane", layout);
		}
		return FALSE;
	}
	return TRUE;
}

/******************************************************************************
 * Name
 *   final_pane_sizes
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void final_pane_sizes()
 *
 * Description
 *   on quit, make a last query for pane sizes.
 *   this is needed because e.g. "maximize window" in the title bar does
 *   not call on_epaned_button_release_event for each newly-modified pane.
 *
 * Return value
 *   void
 */

void final_pane_sizes()
{
	epaned_button_release_event(GTK_WIDGET(widgets.epaned), NULL,
				    (gchar *)"epaned");
	epaned_button_release_event(GTK_WIDGET(widgets.vpaned), NULL,
				    (gchar *)"vpaned");
	epaned_button_release_event(GTK_WIDGET(widgets.vpaned2), NULL,
				    (gchar *)"vpaned2");
	epaned_button_release_event(GTK_WIDGET(widgets.hpaned), NULL,
				    (gchar *)"hpaned1");
}

/******************************************************************************
 * Name
 *   on_configure_event
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   gboolean on_configure_event(GtkWidget * widget,
 *				   GdkEventConfigure * event,
 *				   gpointer user_data)
 *
 * Description
 *   remember placement+size of main window.
 *
 * Return value
 *   gboolean
 */

static gboolean on_configure_event(GtkWidget *widget,
				   GdkEventConfigure *event,
				   gpointer user_data)
{
	gchar layout[80];
	gint x;
	gint y;

	gdk_window_get_root_origin(gtk_widget_get_window(widgets.app), &x, &y);

	settings.gs_width = event->width;
	settings.gs_height = event->height;
	settings.app_x = x;
	settings.app_y = y;

#if GTK_CHECK_VERSION(3, 12, 0)
	sprintf(layout, "%d", gtk_window_is_maximized(GTK_WINDOW(widgets.app)));
	xml_set_value("Xiphos", "layout", "maximized", layout);
#endif

	sprintf(layout, "%d", settings.gs_width);
	xml_set_value("Xiphos", "layout", "width", layout);

	sprintf(layout, "%d", settings.gs_height);
	xml_set_value("Xiphos", "layout", "height", layout);

	sprintf(layout, "%d", settings.app_x);
	xml_set_value("Xiphos", "layout", "app_x", layout);

	sprintf(layout, "%d", settings.app_y);
	xml_set_value("Xiphos", "layout", "app_y", layout);
	xml_save_settings_doc(settings.fnconfigure);

	return FALSE;
}

static void on_notebook_bible_parallel_switch_page(GtkNotebook *notebook,
						   gint page_num,
						   GList **tl)
{
#if 0
	if (page_num == 0)
		gui_set_drop_target(widgets.html_text);
	else
		gtk_drag_dest_unset(GTK_WIDGET(widgets.html_text));
#endif /* 0 */
}

#ifdef USE_GTK_3
static void on_notebook_comm_book_switch_page(GtkNotebook *notebook,
					      gpointer arg,
					      gint page_num, GList **tl)
#else
static void on_notebook_comm_book_switch_page(GtkNotebook *notebook,
					      GtkNotebookPage *page,
					      gint page_num, GList **tl)
#endif
{
	gchar *url = NULL;

	if (page_num == 0) {
#if 0
		gtk_drag_dest_unset(GTK_WIDGET(widgets.html_book));
		gui_set_drop_target(widgets.html_comm);
#endif /* 0 */
		settings.comm_showing = TRUE;
		gtk_widget_show(nav_toolbar);
	} else {
#if 0
		gtk_drag_dest_unset(GTK_WIDGET(widgets.html_comm));
		gui_set_drop_target(widgets.html_book);
#endif /* 0 */
		settings.comm_showing = FALSE;
		if (!settings.showtexts)
			gtk_widget_hide(nav_toolbar);
	}

	gui_update_tab_struct(NULL,
			      settings.CommWindowModule,
			      NULL,
			      NULL,
			      NULL,
			      NULL,
			      settings.comm_showing,
			      settings.showtexts,
			      settings.showpreview,
			      settings.showcomms, settings.showdicts);
	if (settings.comm_showing)
		main_display_commentary(settings.CommWindowModule,
					settings.currentverse);
	else if (settings.book_mod && *settings.book_mod) {
		url = g_strdup_printf("sword://%s/%ld", settings.book_mod,
				      settings.book_offset);
		main_url_handler(url, TRUE);
		g_free(url);
	}
	gui_set_tab_label(settings.currentverse, TRUE);
}

static void new_base_font_size(gboolean up)
{
	if (up) {
		settings.base_font_size++;
		if (settings.base_font_size > 5)
			settings.base_font_size = 5;
	} else {
		settings.base_font_size--;
		if (settings.base_font_size < -2)
			settings.base_font_size = -2;
	}

	if (settings.base_font_size_str)
		g_free(settings.base_font_size_str);
	settings.base_font_size_str =
	    g_strdup_printf("%+d", settings.base_font_size);

	xml_set_value("Xiphos", "fontsize", "basefontsize",
		      settings.base_font_size_str);
	redisplay_to_realign();
}

/* temporary shorthand for too-common use */
#define sM settings.MainWindowModule
#define sC settings.CommWindowModule
#define sD settings.DictWindowModule
#define sB settings.book_mod
#define sV settings.currentverse

static void kbd_toggle_option(gboolean cond, gchar *option)
{
	gchar *msg;

	if (cond) {
		int opt = !main_get_one_option(sM, option); // negate.
		main_save_module_options(sM, option, opt);
		gchar *url = g_strdup_printf("sword://%s/%s", sM, sV);
		main_url_handler(url, TRUE);
		g_free(url);
		msg =
		    g_strdup_printf("%s %s", option, (opt ? "on" : "off"));
		gui_set_statusbar(msg);
		g_free(msg);
	} else {
		msg = g_strdup_printf(_("Module has no support for %s."),
				      option);
		gui_generic_warning(msg);
		g_free(msg);
	}
}

static gboolean on_vbox1_key_press_event(GtkWidget *widget, GdkEventKey *event,
					 gpointer user_data)
{
	/* these are the mods we actually use for global keys, we always only check for these set */
	guint state =
	    event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK |
			    GDK_MOD1_MASK | GDK_MOD4_MASK);

	switch (event->keyval) {
	case XK_Shift_L: /* shift keys - we need this for locking strongs (and */
	case XK_Shift_R: /* other stuff) while moving mouse to previewer */
		shift_key_pressed = TRUE;
	/* no break? hm... */

	case XK_a:
	case XK_A:
		if (state == GDK_MOD1_MASK) { // Alt-A  annotation
			gui_mark_verse_dialog(sM, sV);
		} else if (state ==
			   (GDK_CONTROL_MASK | GDK_MOD1_MASK |
			    GDK_SHIFT_MASK))
			on_biblesync_kbd(3); // BSP audience
		break;

	case XK_b:
	case XK_B:
		if (state == GDK_MOD1_MASK) { // Alt-B  bookmark
			gchar *label = g_strdup_printf("%s, %s", sV, sM);
			gui_bookmark_dialog(label, sM, sV);
			g_free(label);
		}
		break;

	case XK_c:
	case XK_C:
		if (state == GDK_MOD1_MASK) { // Alt-C  commentary pane
			gtk_widget_grab_focus(navbar_versekey.lookup_entry);
			gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book),
						      0);
		}
		break;

	case XK_d:
	case XK_D:
		if (state == GDK_MOD1_MASK) // Alt-D  dictionary entry
			gtk_widget_grab_focus(widgets.entry_dict);
		break;

	case XK_f:
	case XK_F:
		if (state == GDK_CONTROL_MASK) { // Ctrl-F  find text
			if (settings.showtexts) {
				gui_find_dlg(widgets.html_text,
					     sM, FALSE, NULL);
			} else if (settings.showcomms) {
				if (settings.comm_showing) {
					gui_find_dlg(widgets.html_comm,
						     sC, FALSE, NULL);
				} else {
					gui_find_dlg(widgets.html_book,
						     sB, FALSE, NULL);
				}
			} else if (settings.showdicts) {
				gui_find_dlg(widgets.html_dict,
					     sD, FALSE, NULL);
			} else
				gui_generic_warning(_("Xiphos: No windows."));
		}
		break;

	case XK_g:
	case XK_G:
		if (state == GDK_MOD1_MASK) { // Alt-G  genbook entry
			gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book),
						      1);
			gtk_widget_grab_focus(navbar_book.lookup_entry);
		}
		break;

	case XK_j:
		if (state == 0) // J    "next verse"
			access_on_down_eventbox_button_release_event(VERSE_BUTTON);
		break;

	case XK_k:
		if (state == 0) // K    "previous verse"
			access_on_up_eventbox_button_release_event(VERSE_BUTTON);
		break;

	case XK_l:
	case XK_L:
		if (state == GDK_CONTROL_MASK) // Ctrl-L  verse entry
			gtk_widget_grab_focus(navbar_versekey.lookup_entry);
		else if (state == GDK_MOD1_MASK) // Alt-L  lemma
			kbd_toggle_option((main_check_for_global_option(sM, "ThMLLemma") ||
					   main_check_for_global_option(sM, "OSISLemma")),
					  "Lemmas");
		break;

	case XK_m:
	case XK_M:
		if (state == GDK_MOD1_MASK) // Alt-M morph
		{
			kbd_toggle_option((main_check_for_global_option(sM, "GBFMorph") ||
					   main_check_for_global_option(sM, "ThMLMorph") ||
					   main_check_for_global_option(sM, "OSISMorph")),
					  "Morphological Tags");
		}
		break;

	case XK_n:
	case XK_N:
		if (state == GDK_CONTROL_MASK) // Ctrl-N verse
			access_on_down_eventbox_button_release_event(VERSE_BUTTON);
		else if (state == 0) // n chapter
			access_on_down_eventbox_button_release_event(CHAPTER_BUTTON);
		else if (state == GDK_SHIFT_MASK) // N book
			access_on_down_eventbox_button_release_event(BOOK_BUTTON);
		else if (state == (GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_SHIFT_MASK)) {
			// BSP transient navigate
			if (biblesync_active_xmit_allowed()) {
				biblesync_prep_and_xmit(sM, sV);
				gui_set_statusbar(_("BibleSync: Current navigation sent."));
			} else {
				gui_generic_warning(_("BibleSync: Not speaking."));
			}
		}
		break;

	case XK_o:
	case XK_O:
		if (state ==
		    (GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_SHIFT_MASK))
			on_biblesync_kbd(0); // BSP off
		break;

	case XK_p:
	case XK_P:
		if (state == GDK_CONTROL_MASK) // Ctrl-P verse
			access_on_up_eventbox_button_release_event(VERSE_BUTTON);
		else if (state == 0) // p chapter
			access_on_up_eventbox_button_release_event(CHAPTER_BUTTON);
		else if (state == GDK_SHIFT_MASK) // P book
			access_on_up_eventbox_button_release_event(BOOK_BUTTON);
		else if (state == GDK_MOD1_MASK) // Alt-P  parallel detach
			on_undockInt_activate(NULL);
		else if (state ==
			 (GDK_CONTROL_MASK | GDK_MOD1_MASK |
			  GDK_SHIFT_MASK))
			on_biblesync_kbd(1); // BSP personal
		break;

	case XK_q:
	case XK_Q:
		if (state == GDK_CONTROL_MASK) // Ctrl-Q quit
			delete_event(NULL, NULL, NULL);
		break;

	case XK_r:
	case XK_R:
		if (state == GDK_MOD1_MASK) // Alt-R red words
		{
			kbd_toggle_option(((main_check_for_global_option(sM, "GBFRedLetterWords")) ||
					   (main_check_for_global_option(sM, "OSISRedLetterWords"))),
					  "Words of Christ in Red");
		}
		break;

	case XK_s:
	case XK_S:
		if (state == GDK_MOD1_MASK) // Alt-S strong's
		{
			kbd_toggle_option(((main_check_for_global_option(sM, "GBFStrongs")) ||
					   (main_check_for_global_option(sM, "ThMLStrongs")) ||
					   (main_check_for_global_option(sM, "OSISStrongs"))),
					  "Strong's Numbers");
		} else if (state ==
			   (GDK_CONTROL_MASK | GDK_MOD1_MASK |
			    GDK_SHIFT_MASK))
			on_biblesync_kbd(2); // BSP speaker
		break;

	case XK_t:
	case XK_T:
		if (state == GDK_CONTROL_MASK) // Ctrl-T open a new tab
			on_notebook_main_new_tab_clicked(NULL, NULL);
		else if (state == GDK_MOD1_MASK) // Alt-T transliteration
			kbd_toggle_option(true, "Transliteration");
		break;

	case XK_z:
	case XK_Z:
		if (state == GDK_MOD1_MASK) // Alt-Z  open personal commentary
			access_to_edit_percomm();
		break;

	case XK_plus: // Ctrl-Plus  Increase base font size
		if (state == (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
			new_base_font_size(TRUE);
		break;

	case XK_minus: // Ctrl-Minus  Decrease base font size
		if (state == GDK_CONTROL_MASK)
			new_base_font_size(FALSE);
		break;

	case XK_0: // Ctrl-0 (zero)  Neutralize base font size.
		if (state == GDK_CONTROL_MASK) {
			settings.base_font_size = 1;
			new_base_font_size(FALSE);
		}
		break;

	// ctrl-DIGIT [1-9] selects DIGIT-th tab.
	case XK_1:
	case XK_2:
	case XK_3:
	case XK_4:
	case XK_5:
	case XK_6:
	case XK_7:
	case XK_8:
	case XK_9:
		if (state == GDK_CONTROL_MASK)
			gui_select_nth_tab((event->keyval - XK_0) - 1); /* 0-based list */

	case XK_F1: // F1 help
		if (state == 0)
			on_help_contents_activate(NULL, NULL);
		break;

	case XK_F2: // F2 preferences
		if (state == 0)
			on_preferences_activate(NULL, NULL);
		break;

	case XK_F3: // F3 search
		if (state == 0)
			main_open_search_dialog();
		else if (state == GDK_CONTROL_MASK)
			gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
						      2);
		break;

	case XK_F4: // F4 module manager
		if (state == 0)
			on_module_manager_activate(NULL, NULL);
		else if (state == GDK_CONTROL_MASK)
			gui_close_passage_tab(gtk_notebook_page_num(GTK_NOTEBOOK(widgets.notebook_main),
								    ((PASSAGE_TAB_INFO *)
								     cur_passage_tab)->page_widget));
		break;

	case XK_F10: // Shift-F10 bible module right click
		if (state == GDK_SHIFT_MASK)
			gui_menu_popup(NULL, sM, NULL);
		/* FIXME: needs the html widget as first pram */
		break;

	case XK_F11: // F11 open current bible in separate window, maximized.
		if (state == 0)
			main_dialogs_open(sM, NULL, TRUE);
		break;
	}
	XI_message(("on_vbox1_key_press_event\nkeycode: %d, keysym: %0x, state: %d",
		    event->hardware_keycode, event->keyval, state));
	return FALSE;
}

static gboolean on_vbox1_key_release_event(GtkWidget *widget,
					   GdkEventKey *event,
					   gpointer user_data)
{
	switch (event->keyval) {
	case XK_Shift_L:
	case XK_Shift_R:
		shift_key_pressed = FALSE;
		break;
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *   create_mainwindow
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void create_mainwindow(void)
 *
 * Description
 *    create xiphos gui
 *
 * Return value
 *   void
 */

void create_mainwindow(void)
{
	char *imagename;
	GtkWidget *vbox_gs;
	GtkWidget *menu;
	GtkWidget *hbox25;
	GtkWidget *tab_button_icon;
	GtkWidget *label;
#ifndef USE_WEBKIT2
	GtkWidget *scrolledwindow;
#endif
	GtkWidget *box_book;
	GdkPixbuf *pixbuf;
	/*
	   GTK_SHADOW_NONE
	   GTK_SHADOW_IN
	   GTK_SHADOW_OUT
	   GTK_SHADOW_ETCHED_IN
	   GTK_SHADOW_ETCHED_OUT
	 */
	settings.shadow_type = GTK_SHADOW_IN;

	XI_print(("%s xiphos-%s\n", "Starting", VERSION));
	XI_print(("%s\n\n", "Building Xiphos interface"));

	widgets.studypad_dialog = NULL;

	/* A rough scektch of the main window (widgets.app) and it's children
	 *                widgets.app
	 *                     |
	 *                  vbox_gs
	 *                     |
	 *                  +--|----+
	 *                  |       |
	 *                 menu   hbox25
	 *                           |
	 *                     widgets.epaned
	 *                         |
	 *                 widgets.vboxMain
	 *                 |              |
	 *           widgets.hboxtb   widgets.page
	 *                            |         |
	 *                     widgets.hpaned  nav_toolbar
	 *                     |            |
	 *              widgets.vpaned---+  +----------------------------widgets.vpaned2---------------+
	 *               |               |                                      |                      |
	 * widgets.vbox_previewer   widgets.vbox_text              widgets.notebook_comm_book   widgets.box_dict
	 *                               |                         |                       |
	 *                widgets.notebook_bible_parallel   widgets.box_comm            box_book
	 *                               |
	 *                     widgets.notebook_text
	 *
	 */

	// The toplevel Xiphos window
	widgets.app = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(widgets.app), _("Xiphos - Bible Study Software"));
	g_object_set_data(G_OBJECT(widgets.app), "widgets.app", widgets.app);
	gtk_window_set_default_size(GTK_WINDOW(widgets.app), 680, 425);
	gtk_widget_set_can_focus(widgets.app, 1);
	gtk_window_set_resizable(GTK_WINDOW(widgets.app), TRUE);

	// The app icon.
	// FIXME:: This should be a big copy of the logo because GTK does the scaling (GTK 3.16?)
	imagename = image_locator("gs2-48x48.png");
	pixbuf = gdk_pixbuf_new_from_file(imagename, NULL);
	g_free(imagename);
	gtk_window_set_icon(GTK_WINDOW(widgets.app), pixbuf);

	// The main box for our toplevel window.
	UI_VBOX(vbox_gs, FALSE, 0);
	gtk_widget_show(vbox_gs);
	gtk_container_add(GTK_CONTAINER(widgets.app), vbox_gs);

	// Add the main menu.
	menu = gui_create_main_menu();
	gtk_box_pack_start(GTK_BOX(vbox_gs), menu, FALSE, TRUE, 0);

	// Another box
	UI_HBOX(hbox25, FALSE, 0);
	gtk_widget_show(hbox25);
	gtk_box_pack_start(GTK_BOX(vbox_gs), hbox25, TRUE, TRUE, 0);

	// widgets.epaned
	widgets.epaned = UI_HPANE();
	gtk_widget_show(widgets.epaned);
#if !GTK_CHECK_VERSION(3, 14, 0)
	gtk_container_set_border_width(GTK_CONTAINER(widgets.epaned), 4);
#endif
	gtk_box_pack_start(GTK_BOX(hbox25), widgets.epaned, TRUE, TRUE, 0);
	// Another box
	UI_VBOX(widgets.vboxMain, FALSE, 0);
	gtk_widget_show(widgets.vboxMain);
	gtk_paned_pack2(GTK_PANED(widgets.epaned), widgets.vboxMain, TRUE, TRUE);
#if !GTK_CHECK_VERSION(3, 14, 0)
	gtk_container_set_border_width(GTK_CONTAINER(widgets.vboxMain), 2);
#endif

	/*
	 * Notebook to have separate passages opened at once the passages are not
	 * actually open but are switched between similar to bookmarks
	 */
	UI_HBOX(widgets.hboxtb, FALSE, 0);
	if (settings.browsing)
		gtk_widget_show(widgets.hboxtb);
	gtk_box_pack_start(GTK_BOX(widgets.vboxMain), widgets.hboxtb, FALSE, FALSE, 0);

	widgets.button_new_tab = gtk_button_new();
	// Don't show button here in case !settings.browsing

#if GTK_CHECK_VERSION(3, 10, 0)
	tab_button_icon = gtk_image_new_from_icon_name("tab-new-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
#else
	tab_button_icon = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_SMALL_TOOLBAR);
#endif

	gtk_widget_show(tab_button_icon);
	gtk_container_add(GTK_CONTAINER(widgets.button_new_tab), tab_button_icon);
	gtk_button_set_relief(GTK_BUTTON(widgets.button_new_tab), GTK_RELIEF_NONE);
	gtk_box_pack_start(GTK_BOX(widgets.hboxtb), widgets.button_new_tab, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(widgets.button_new_tab, _("Open a new tab"));

	widgets.notebook_main = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_main);
	gtk_box_pack_start(GTK_BOX(widgets.hboxtb), widgets.notebook_main, TRUE, TRUE, 0);
	gtk_widget_set_size_request(widgets.notebook_main, -1, 25);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.notebook_main), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_main));
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.notebook_main), FALSE);
	// Main passage tabbed notebook end

	// Another box
	UI_VBOX(widgets.page, FALSE, 0);
	gtk_widget_show(widgets.page);
	gtk_box_pack_start(GTK_BOX(widgets.vboxMain), widgets.page, TRUE, TRUE, 0);

	//nav toolbar
	nav_toolbar = gui_navbar_versekey_new();
	gtk_box_pack_start(GTK_BOX(widgets.page), nav_toolbar, FALSE, FALSE, 0);

	// widgets.hpaned
	widgets.hpaned = UI_HPANE();
	gtk_widget_show(widgets.hpaned);
	gtk_box_pack_start(GTK_BOX(widgets.page), widgets.hpaned, TRUE, TRUE, 0);

	// widgets.vpaned
	widgets.vpaned = UI_VPANE();
	gtk_widget_show(widgets.vpaned);
	gtk_widget_set_size_request(widgets.vpaned, 50, -1);
	gtk_paned_pack1(GTK_PANED(widgets.hpaned), widgets.vpaned, TRUE, FALSE);

	// widgets.vpaned2
	widgets.vpaned2 = UI_VPANE();
	gtk_widget_set_size_request(widgets.vpaned2, 50, -1);

	// widgets.vbox_text
	UI_VBOX(widgets.vbox_text, FALSE, 0);
	gtk_widget_show(widgets.vbox_text);
	gtk_paned_pack1(GTK_PANED(widgets.vpaned), widgets.vbox_text, TRUE, TRUE);

	// Bible/parallel notebook
	widgets.notebook_bible_parallel = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_bible_parallel);
	gtk_box_pack_start(GTK_BOX(widgets.vbox_text), widgets.notebook_bible_parallel, TRUE, TRUE, 0);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(widgets.notebook_bible_parallel), GTK_POS_BOTTOM);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_bible_parallel), TRUE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.notebook_bible_parallel), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(widgets.notebook_bible_parallel), 1);

	g_signal_connect(G_OBJECT(widgets.notebook_bible_parallel), "change-current-page", G_CALLBACK(on_notebook_bible_parallel_switch_page), NULL);

	// Text notebook (The bible text show in the standard view)
	widgets.notebook_text = gui_create_bible_pane();
	gtk_container_add(GTK_CONTAINER(widgets.notebook_bible_parallel), widgets.notebook_text);

	label = gtk_label_new(_("Standard View"));
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_bible_parallel), gtk_notebook_get_nth_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel), 0), label);

	// Another box (For the previewer?)
	UI_VBOX(widgets.vbox_previewer, FALSE, 0);
	gtk_widget_show(widgets.vbox_previewer);
	gtk_paned_pack2(GTK_PANED(widgets.vpaned), widgets.vbox_previewer, TRUE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(widgets.vbox_previewer), 2);

#ifndef USE_WEBKIT2
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(widgets.vbox_previewer), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *) scrolledwindow, settings.shadow_type);
#endif
	widgets.html_previewer_text = GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, VIEWER_TYPE));
	gtk_widget_show(widgets.html_previewer_text);
#ifdef USE_WEBKIT2
	gtk_box_pack_start(GTK_BOX(widgets.vbox_previewer), widgets.html_previewer_text, TRUE, TRUE, 0);
#else
	gtk_container_add(GTK_CONTAINER(scrolledwindow), widgets.html_previewer_text);
#endif

	// Commentary/book notebook
	widgets.notebook_comm_book = gtk_notebook_new();
	gtk_widget_show(widgets.notebook_comm_book);

	gtk_paned_pack1(GTK_PANED(widgets.vpaned2), widgets.notebook_comm_book, TRUE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(widgets.notebook_comm_book), 1);

	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(widgets.notebook_comm_book), GTK_POS_BOTTOM);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_comm_book), TRUE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(widgets.notebook_comm_book), FALSE);

	// Commentary pane
	widgets.box_comm = gui_create_commentary_pane();
	gtk_container_add(GTK_CONTAINER(widgets.notebook_comm_book), widgets.box_comm);

	label = gtk_label_new(_("Commentary View"));
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_comm_book), gtk_notebook_get_nth_page(GTK_NOTEBOOK(widgets.notebook_comm_book), 0), label);

	// Book pane
	box_book = gui_create_book_pane();
	gtk_container_add(GTK_CONTAINER(widgets.notebook_comm_book), box_book);

	label = gtk_label_new(_("Book View"));
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_comm_book), gtk_notebook_get_nth_page(GTK_NOTEBOOK(widgets.notebook_comm_book), 1), label);

	// Dict/lex
	widgets.box_dict = gui_create_dictionary_pane();
	gtk_paned_pack2(GTK_PANED(widgets.vpaned2), widgets.box_dict, TRUE, TRUE);

	// Statusbar
	widgets.appbar = gtk_statusbar_new();
	if (settings.statusbar) {
		gtk_widget_show(widgets.appbar);
	}

#ifndef USE_GTK_3
	gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(widgets.appbar), TRUE);
#endif
	gtk_box_pack_start(GTK_BOX(vbox_gs), widgets.appbar, FALSE, TRUE, 0);
	gui_set_statusbar(_("Welcome to Xiphos"));

	gtk_paned_pack2(GTK_PANED(widgets.hpaned), widgets.vpaned2, TRUE, FALSE);
	gtk_widget_grab_focus(navbar_versekey.lookup_entry);

	gtk_window_set_default_size((GtkWindow *)widgets.app, settings.gs_width, settings.gs_height);
	gtk_widget_show_all(widgets.app);

	/* must connect signals *after* instantiating window above, */
	/* immediately above, otherwise window creation induces */
	/* configure_event, wiping out user's saved geometry specs. */
	/* *important*: drain gtk event queue first (i.e. sync). */

	sync_windows();
	g_signal_connect((gpointer)vbox_gs, "key_press_event", G_CALLBACK(on_vbox1_key_press_event), NULL);
	g_signal_connect((gpointer)vbox_gs, "key_release_event", G_CALLBACK(on_vbox1_key_release_event), NULL);

	g_signal_connect(G_OBJECT(widgets.notebook_comm_book), "switch_page", G_CALLBACK(on_notebook_comm_book_switch_page), NULL);

	g_signal_connect(G_OBJECT(widgets.app), "delete_event", G_CALLBACK(delete_event), NULL);

	g_signal_connect((gpointer)widgets.app, "configure_event", G_CALLBACK(on_configure_event), NULL);
	g_signal_connect(G_OBJECT(widgets.epaned), "button_release_event", G_CALLBACK(epaned_button_release_event), (gchar *)"epaned");
	g_signal_connect(G_OBJECT(widgets.vpaned), "button_release_event", G_CALLBACK(epaned_button_release_event), (gchar *)"vpaned");
	g_signal_connect(G_OBJECT(widgets.vpaned2), "button_release_event", G_CALLBACK(epaned_button_release_event), (gchar *)"vpaned2");
	g_signal_connect(G_OBJECT(widgets.hpaned), "button_release_event", G_CALLBACK(epaned_button_release_event), (gchar *)"hpaned1");

	main_window_created = TRUE;
}

/******************************************************************************
 * Name
 *   gui_show_main_window
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   void gui_show_main_window(void)
 *
 * Description
 *    display the app after all is created
 *
 * Return value
 *   void
 */

void gui_show_main_window(void)
{
	gtk_widget_show(widgets.app);
}
