/*
 * Xiphos Bible Study Tool
 * preferences_dialog.c - get user preferences
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
#include <gtk/gtk.h>
#ifndef USE_GTKBUILDER
#include <glade/glade-xml.h>
#endif

#include "gui/bibletext.h"
#include "gui/dialog.h"
#include "gui/xiphos.h"
#include "gui/preferences_dialog.h"
#include "gui/utilities.h"
#include "gui/gbs.h"
#include "gui/commentary.h"
#include "gui/dictlex.h"
#include "gui/parallel_view.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/sidebar.h"
#include "gui/tabbed_browser.h"
#include "gui/widgets.h"
#include "gui/font_dialog.h"

#include "main/sword.h"
#include "main/lists.h"
#include "main/mod_mgr.h"
#include "main/previewer.h"
#include "main/parallel_view.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/xml.h"
#include "main/url.hh"
#include "main/biblesync_glue.h"

#include "gui/debug_glib_null.h"

typedef struct _preferences_combo COMBOBOXS;
struct _preferences_combo
{
	GtkWidget *default_dictionary_module;
	GtkWidget *percomm_module;
	GtkWidget *devotion_module;
	GtkWidget *greek_lex__module;
	GtkWidget *hebrew_lex__module;
	GtkWidget *combo_entry_sp_dir;
	GtkWidget *base_font_size;
	GtkWidget *verse_number_size;
	GtkWidget *special_locale;
	GtkWidget *font_prefs;
};

typedef struct _preferences_color_pickers COLOR_PICKERS;
struct _preferences_color_pickers
{
	GtkWidget *text;
	GtkWidget *text_background;
	GtkWidget *text_current_verse;
	GtkWidget *verse_numbers;
	GtkWidget *href_links;
	GtkWidget *highlight_fg;
	GtkWidget *highlight_bg;
	GtkWidget *invert_normal;
	GtkWidget *invert_highlight;
};

typedef struct _preferences_check_buttons CHECK_BUTTONS;
struct _preferences_check_buttons
{
	GtkWidget *use_defaults; /* radio button */

	GtkWidget *use_default_dictionary;
	GtkWidget *use_linked_tabs;
	GtkWidget *use_chapter_scroll;
	GtkWidget *use_imageresize;
	GtkWidget *use_verse_num_bold;
	GtkWidget *use_verse_num_bracket;
	GtkWidget *use_verse_num_superscript;
	GtkWidget *readaloud;
	GtkWidget *show_verse_num;
	GtkWidget *versehighlight;
	GtkWidget *annotate_highlight;
	GtkWidget *xrefs_in_verse_list;
	GtkWidget *show_splash_screen;
	GtkWidget *prayerlist;
	GtkWidget *statusbar;
	GtkWidget *alternation;

	GtkWidget *show_in_viewer;
	GtkWidget *show_in_dictionary;
	GtkWidget *show_devotion;

	GtkWidget *bs_debug;
	GtkWidget *bs_presence;
	GtkWidget *bs_mismatch;
	GtkWidget *bs_group_tab;
	GtkWidget *bs_privacy;
};

typedef struct _preferences_radio_buttons RADIO_BUTTONS;
struct _preferences_radio_buttons
{
	/* BibleSync mode selection */
	GtkWidget *bs_mode_off;
	GtkWidget *bs_mode_personal;
	GtkWidget *bs_mode_speaker;
	GtkWidget *bs_mode_audience;

	/* BibleSync navigation choice */
	GtkWidget *bs_nav_direct;
	GtkWidget *bs_nav_verselist;

	/* BibleSync listener choice. */
	GtkWidget *bs_listen_some;
	GtkWidget *bs_listen_all;
	GtkWidget *bs_listen_none;
};

/* fixed callback values, for radio button assignment. */
static int rb_cb[] = {0, 1, 2, 3}; // userdata callback values.

typedef struct _preferences_buttons BUTTONS;
struct _preferences_buttons
{
	GtkWidget *xiphos_defaults;
};

typedef struct _parallel_select PARALLEL_SELECT;
struct _parallel_select
{
	GtkWidget *button_clear;
	GtkWidget *button_cut;
	GtkWidget *button_add;
	GtkWidget *listview;
	GtkWidget *mod_sel_dialog;
	GtkWidget *mod_sel_close;
	GtkWidget *mod_sel_add;
	GtkWidget *mod_sel_treeview;
};
static GtkWidget *dialog_prefs = NULL;

GtkWidget *speaker_window = NULL; // BibleSync
GtkWidget *speaker_list = NULL;   // BibleSync

/*****************************************************************************
 * externs
 */
extern gchar *tmpcolor;

/******************************************************************************
 * static - visible to this file only
 */
static GtkWidget *notebook;
static COMBOBOXS combo;
static COLOR_PICKERS color_picker;
static CHECK_BUTTONS check_button;
static RADIO_BUTTONS radio_button;
static PARALLEL_SELECT parallel_select;

/******************************************************************************
 * Name
 *   on_prefs_configure_event
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *   gboolean on_prefs_configure_event(GtkWidget * widget,
 *				   GdkEventConfigure * event,
 *				   gpointer user_data)
 *
 * Description
 *   remember placement+size of prefs window.
 *   cloned from on_parallel_configure_event
 *
 * Return value
 *   gboolean
 */

static gboolean on_prefs_configure_event(GtkWidget *widget,
					 GdkEventConfigure *event,
					 gpointer user_data)
{
	gchar layout[10];
	gint x;
	gint y;

	gdk_window_get_root_origin(GDK_WINDOW(gtk_widget_get_window(dialog_prefs)),
				   &x, &y);

	settings.prefs_width = event->width;
	settings.prefs_height = event->height;
	settings.prefs_x = x;
	settings.prefs_y = y;

	sprintf(layout, "%d", settings.prefs_width);
	xml_set_value("Xiphos", "layout", "prefs_width", layout);

	sprintf(layout, "%d", settings.prefs_height);
	xml_set_value("Xiphos", "layout", "prefs_height", layout);

	sprintf(layout, "%d", settings.prefs_x);
	xml_set_value("Xiphos", "layout", "prefs_x", layout);

	sprintf(layout, "%d", settings.prefs_y);
	xml_set_value("Xiphos", "layout", "prefs_y", layout);
	xml_save_settings_doc(settings.fnconfigure);

	return FALSE;
}

/******************************************************************************
 * Name
 *  string_is_color
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   gint string_is_color(gchar * color)
 *
 * Description
 *    this code is from bluefish-0.6
 *
 * Return value
 *   gint
 */
#if !GTK_CHECK_VERSION(3, 4, 0)
static gint string_is_color(gchar *color)
{
	gint i;

	if (!color) {
		XI_warning(("string_is_color, pointer NULL\n"));
		return 0;
	}
	if (strlen(color) != 7) {
		XI_warning(("string_is_color, strlen(%s) != 7\n", color));
		return 0;
	}
	if (color[0] != '#') {
		XI_warning(("string_is_color, 0 in %s is not #\n", color));
		return 0;
	}
	for (i = 1; i < 7; i++) {
		if ((color[i] > 102) || 
		    (color[i] < 48) || 
		    ((color[i] > 57) && (color[i] < 65)) || 
		    ((color[i] > 70) && (color[i] < 97))) {
			XI_warning(("string_is_color, %d in %s is not from a color, it is %d\n",
				    i, color, color[i]));
			return 0;
		}
	}
	XI_print(("string_is_color, %s is color\n", color));
	return 1;
}
#endif

#if GTK_CHECK_VERSION(3, 4, 0)
static gchar *gdkrgba_to_hex(GdkRGBA *color)
{
	gchar *tmpstr;

	tmpstr = g_malloc(8 * sizeof(char));
	g_snprintf(tmpstr, 8, "#%.2X%.2X%.2X",
		   (unsigned int)(color->red * 255),
		   (unsigned int)(color->green * 255),
		   (unsigned int)(color->blue * 255));
	return tmpstr;
}

#else
/******************************************************************************
 * Name
 *  gdkcolor_to_hex
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   gchar *gdkcolor_to_hex(gdouble * color, gint websafe)
 *
 * Description
 *    this code is from bluefish-1.0.2
 *
 * Return value
 *   gchar *
 */

static gchar *gdkcolor_to_hex(GdkColor color, gint websafe)
{
	gchar *tmpstr;

	tmpstr = g_malloc(8 * sizeof(char));
	if (websafe) {
		g_snprintf(tmpstr, 8, "#%.2X%.2X%.2X",
			   (0x33 * color.red / (256 * 0x33)),
			   (0x33 * color.green / (256 * 0x33)),
			   (0x33 * color.blue / (256 * 0x33)));
	} else {
		g_snprintf(tmpstr, 8, "#%.2X%.2X%.2X",
			   color.red / 256,
			   color.green / 256, color.blue / 256);
	}
	return tmpstr;
}

#endif

void apply_color_settings(void)
{
	if (settings.havebible) {
		main_display_bible(settings.MainWindowModule,
				   settings.currentverse);
		main_update_parallel_page();
	}
	if (settings.havecomm)
		main_display_commentary(settings.CommWindowModule,
					settings.currentverse);
	if (settings.havebook)
		main_display_book(settings.book_mod, settings.book_key);
	if (settings.havedict)
		main_display_dictionary(settings.DictWindowModule,
					settings.dictkey);
	main_init_previewer();
}

/******************************************************************************
 * Name
 *   invert_colors
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void invert_colors(char **color1, char *label1,
		        char **color2, char *label2)
 *
 * Description
 *   parameterized color selector inversion.  called from on_invert().
 *
 * Return value
 *   void
 */

void invert_colors(char **color1, char *label1,
		   char **color2, char *label2)
{
	char *hold = *color1;
	*color1 = *color2;
	*color2 = hold;

	xml_set_value("Xiphos", "HTMLcolors", label1, *color1);
	xml_set_value("Xiphos", "HTMLcolors", label2, *color2);

	setup_color_pickers();
	apply_color_settings();
}

/******************************************************************************
 * Name
 *   on_invert
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_invert(GtkWidget *button, gchar *user_data)
 *
 * Description
 *   invokes swap of normal or highlight foreground/background
 *
 * Return value
 *   void
 */

void on_invert(GtkWidget *button, gchar *user_data)
{
	if (user_data)
		invert_colors(&settings.bible_bg_color, "background",
			      &settings.bible_text_color, "text_fg");
	else
		invert_colors(&settings.highlight_fg, "highlight_fg",
			      &settings.highlight_bg, "highlight_bg");
}

/******************************************************************************
 * Name
 *   on_colorbutton1_color_set
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_colorbutton1_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_colorbutton1_color_set(GtkColorButton *colorbutton, gpointer user_data)
{
	gchar *buf = NULL;
#if GTK_CHECK_VERSION(3, 4, 0)
	GdkRGBA color;
	gtk_color_chooser_get_rgba((GtkColorChooser *)colorbutton,
				   &color);
	buf = gdkrgba_to_hex(&color); //gdk_rgba_to_string (&color);
#else
	GdkColor color;
	gtk_color_button_get_color(colorbutton, &color);
	buf = gdkcolor_to_hex(color, 1);
#endif
	xml_set_value("Xiphos", "HTMLcolors", "background", buf);
	settings.bible_bg_color =
	    xml_get_value("HTMLcolors", "background");
	if (buf)
		g_free(buf);
	apply_color_settings();
}

/******************************************************************************
 * Name
 *   on_colorbutton2_color_set
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_colorbutton2_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_colorbutton2_color_set(GtkColorButton *colorbutton, gpointer user_data)
{
	gchar *buf2 = NULL;
#if GTK_CHECK_VERSION(3, 4, 0)
	GdkRGBA color;

	gtk_color_chooser_get_rgba((GtkColorChooser *)colorbutton,
				   &color);
	buf2 = gdkrgba_to_hex(&color);
#else
	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color, 1);
#endif
	xml_set_value("Xiphos", "HTMLcolors", "text_fg", buf2);
	settings.bible_text_color = xml_get_value("HTMLcolors", "text_fg");
	if (buf2)
		g_free(buf2);
	apply_color_settings();
}

/******************************************************************************
 * Name
 *   on_colorbutton3_color_set
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_colorbutton3_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_colorbutton3_color_set(GtkColorButton *colorbutton, gpointer user_data)
{
	gchar *buf2 = NULL;
#if GTK_CHECK_VERSION(3, 4, 0)
	GdkRGBA color;

	gtk_color_chooser_get_rgba((GtkColorChooser *)colorbutton,
				   &color);
	buf2 = gdkrgba_to_hex(&color);
#else
	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color, 1);
#endif
	xml_set_value("Xiphos", "HTMLcolors", "currentverse", buf2);
	settings.currentverse_color =
	    xml_get_value("HTMLcolors", "currentverse");
	if (buf2)
		g_free(buf2);
	apply_color_settings();
}

/******************************************************************************
 * Name
 *   on_colorbutton4_color_set
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_colorbutton4_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_colorbutton4_color_set(GtkColorButton *colorbutton, gpointer user_data)
{
	gchar *buf2 = NULL;
#if GTK_CHECK_VERSION(3, 4, 0)
	GdkRGBA color;

	gtk_color_chooser_get_rgba((GtkColorChooser *)colorbutton,
				   &color);
	buf2 = gdkrgba_to_hex(&color);
#else
	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color, 1);
#endif
	xml_set_value("Xiphos", "HTMLcolors", "versenum", buf2);
	settings.bible_verse_num_color =
	    xml_get_value("HTMLcolors", "versenum");
	if (buf2)
		g_free(buf2);
	apply_color_settings();
}

/******************************************************************************
 * Name
 *   on_colorbutton5_color_set
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_colorbutton5_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_colorbutton5_color_set(GtkColorButton *colorbutton, gpointer user_data)
{
	gchar *buf2 = NULL;
#if GTK_CHECK_VERSION(3, 4, 0)
	GdkRGBA color;

	gtk_color_chooser_get_rgba((GtkColorChooser *)colorbutton,
				   &color);
	buf2 = gdkrgba_to_hex(&color);
#else
	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color, 1);
#endif
	xml_set_value("Xiphos", "HTMLcolors", "link", buf2);
	settings.link_color = xml_get_value("HTMLcolors", "link");
	if (buf2)
		g_free(buf2);
	apply_color_settings();
}

/******************************************************************************
 * Name
 *   on_colorbutton6_color_set
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_colorbutton6_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_colorbutton6_color_set(GtkColorButton *colorbutton, gpointer user_data)
{
	gchar *buf2 = NULL;
#if GTK_CHECK_VERSION(3, 4, 0)
	GdkRGBA color;

	gtk_color_chooser_get_rgba((GtkColorChooser *)colorbutton,
				   &color);
	buf2 = gdkrgba_to_hex(&color);
#else
	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color, 1);
#endif
	xml_set_value("Xiphos", "HTMLcolors", "highlight_fg", buf2);
	settings.highlight_fg =
	    xml_get_value("HTMLcolors", "highlight_fg");
	if (buf2)
		g_free(buf2);
	apply_color_settings();
}

/******************************************************************************
 * Name
 *   on_colorbutton7_color_set
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_colorbutton7_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_colorbutton7_color_set(GtkColorButton *colorbutton, gpointer user_data)
{
	gchar *buf2 = NULL;
#if GTK_CHECK_VERSION(3, 4, 0)
	GdkRGBA color;

	gtk_color_chooser_get_rgba((GtkColorChooser *)colorbutton,
				   &color);
	buf2 = gdkrgba_to_hex(&color);
#else
	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color, 1);
#endif
	xml_set_value("Xiphos", "HTMLcolors", "highlight_bg", buf2);
	settings.highlight_bg =
	    xml_get_value("HTMLcolors", "highlight_bg");
	if (buf2)
		g_free(buf2);
	apply_color_settings();
}

/******************************************************************************
 * Name
 *   on_checkbutton1_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton1_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton1_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	gui_tabs_on_off(gtk_toggle_button_get_active(togglebutton));
	/*	if (togglebutton->active) {
		xml_set_value("Xiphos", "tabs", "browsing", "1");
		settings.browsing = TRUE;
		gui_open_tabs();
		gtk_widget_show(widgets.hboxtb);
	} else {
		xml_set_value("Xiphos", "tabs", "browsing", "0");
		gtk_widget_hide(widgets.hboxtb);
		settings.browsing = FALSE;
		gui_close_all_tabs();
	}
	settings.browsing = atoi(xml_get_value("tabs", "browsing"));
	*/
}

/******************************************************************************
 * Name
 *   on_checkbutton2_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton2_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton2_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton))
		xml_set_value("Xiphos", "misc", "showtexts", "1");
	else
		xml_set_value("Xiphos", "misc", "showtexts", "0");
	settings.showtexts = atoi(xml_get_value("misc", "showtexts"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewtexts_item),
				       settings.showtexts);
	gui_show_hide_texts(gtk_toggle_button_get_active(togglebutton));
}

/******************************************************************************
 * Name
 *   on_checkbutton3_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton3_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton3_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton))
		xml_set_value("Xiphos", "misc", "showcomms", "1");
	else
		xml_set_value("Xiphos", "misc", "showcomms", "0");
	settings.showcomms = atoi(xml_get_value("misc", "showcomms"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewcomms_item),
				       settings.showcomms);
	gui_show_hide_comms(gtk_toggle_button_get_active(togglebutton));
}

/******************************************************************************
 * Name
 *   on_checkbutton9_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton9_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton9_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton))
		xml_set_value("Xiphos", "misc", "showpreview", "1");
	else
		xml_set_value("Xiphos", "misc", "showpreview", "0");
	settings.showpreview = atoi(xml_get_value("misc", "showpreview"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewpreview_item),
				       settings.showpreview);
	gui_show_hide_preview(gtk_toggle_button_get_active(togglebutton));
}

/******************************************************************************
 * Name
 *   on_checkbutton4_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton4_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton4_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton))
		xml_set_value("Xiphos", "misc", "showdicts", "1");
	else
		xml_set_value("Xiphos", "misc", "showdicts", "0");
	settings.showdicts = atoi(xml_get_value("misc", "showdicts"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.viewdicts_item),
				       settings.showdicts);
	gui_show_hide_dicts(gtk_toggle_button_get_active(togglebutton));
}

/******************************************************************************
 * Name
 *   on_checkbutton10_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton10_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton10_toggled(GtkToggleButton *togglebutton,
			 gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "pinnedtabs",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.linkedtabs = atoi(xml_get_value("misc", "pinnedtabs"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.linkedtabs_item),
				       settings.linkedtabs);
}

/******************************************************************************
 * Name
 *   on_biblesync_obtain_passphrase
 *
 * Synopsis
 *   #include "unlock_key_dialog.h"
 *   gchar *on_biblesync_obtain_passphrase()
 *
 * Description
 *   create dialog to get session passphrase
 *
 * Return value
 *   void
 */

gchar *on_biblesync_obtain_passphrase()
{
	gchar *retval;
	GS_DIALOG *info = gui_new_dialog();
#if GTK_CHECK_VERSION(3, 10, 0)
	info->stock_icon = g_strdup("dialog-warning");
#else
	info->stock_icon = g_strdup(GTK_STOCK_DIALOG_WARNING);
#endif
	info->label_top = g_strdup(_("BibleSync session passphrase"));
	info->text1 = g_strdup(biblesync_get_passphrase());
	info->label1 = _("Phrase:");
	info->ok = TRUE;

	(void)gui_gs_dialog(info);

	retval = g_strdup(info->text1);
	g_free(info->label_top);
	g_free(info->text1);
	g_free(info);
	return retval;
}

/******************************************************************************
 * Name
 *   on_checkbutton_biblesync_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_biblesync_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *   *((int*)user_data) is settings' bs_* bitflag.
 *
 * Description
 *   debug (packet dump), presence, mismatch, privacy, and group/tab.
 *
 * Return value
 *   void
 */

static void
on_checkbutton_biblesync_toggled(GtkToggleButton *togglebutton,
				 gpointer user_data)
{
	*((int *)user_data) = gtk_toggle_button_get_active(togglebutton);
	if (user_data == &settings.bs_privacy)
		biblesync_privacy(settings.bs_privacy);
}

/******************************************************************************
 * Name
 *   on_radiobutton_biblesync_mode
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_radiobutton_biblesync_mode(GtkToggleButton * togglebutton, gpointer user_data)
 *   *((int*)user_data) is settings.bs_mode's selection.
 *
 * Description
 *   BibleSync mode selection, 0-3.
 *
 * Return value
 *   void
 */

static void
on_radiobutton_biblesync_mode(GtkToggleButton *togglebutton,
			      gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
		settings.bs_mode = *((int *)user_data);

		if (settings.bs_mode != 0) {
			g_free(settings.bs_passphrase);
			settings.bs_passphrase =
			    on_biblesync_obtain_passphrase();
		}
		int new_mode = biblesync_mode_select(settings.bs_mode,
						     settings.bs_passphrase);
		/* selecting active mode enables polled receiver. */

		if (new_mode != settings.bs_mode) {
			/* mode selection failed? probably dead interface */
			gui_generic_warning(_("Mode selection failed.\n"
					      "Set \"Debug\" and try "
					      "again to see why."));
			settings.bs_mode = new_mode;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_mode_off),
						     (settings.bs_mode == 0));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_mode_personal),
						     (settings.bs_mode == 1));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_mode_speaker),
						     (settings.bs_mode == 2));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_mode_audience),
						     (settings.bs_mode == 3));
		}

		if (biblesync_personal()) {
			gtk_widget_set_sensitive(check_button.bs_privacy, TRUE);
			on_checkbutton_biblesync_toggled(GTK_TOGGLE_BUTTON(check_button.bs_privacy),
							 &settings.bs_privacy);
		} else {
			gtk_widget_set_sensitive(check_button.bs_privacy, FALSE);
		}
	}
}

/******************************************************************************
 * Name
 *   on_biblesync_kbd
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_biblesync_kbd(int mode)
 *
 * Description
 *   kbd-selected biblesync mode.
 *   as elsewhere: 0 = off; 1 = personal; 2 = speaker; 3 = audience.
 *
 * Return value
 *   void
 */

void on_biblesync_kbd(int mode)
{
	int new_mode;

	settings.bs_mode = mode;
	new_mode = biblesync_mode_select(settings.bs_mode,
					 settings.bs_passphrase);

	if (new_mode != settings.bs_mode) {
		gui_generic_warning(_("Mode selection failed.\n"));
		settings.bs_mode = new_mode;
	} else {
		gchar *msg = g_strdup_printf(_("BibleSync: %s (passphrase \"%s\")."),
					     ((mode == 0)
						  ? _("Disabled")
						  : ((mode == 1)
							 ? _("Personal")
							 : ((mode == 2)
								? _("Speaker")
								: _("Audience")))),
					     settings.bs_passphrase);
		gui_set_statusbar(msg);
		g_free(msg);
	}
}

/******************************************************************************
 * Name
 *   on_radiobutton_biblesync_nav
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_radiobutton_biblesync_nav(GtkToggleButton * togglebutton,
 *				       gpointer user_data)
 *   *((int*)user_data) is settings.bs_navdirect's T|F selection.
 *
 * Description
 *   BibleSync nav selection, direct (1) or verse list (0).
 *
 * Return value
 *   void
 */

static void
on_radiobutton_biblesync_nav(GtkToggleButton *togglebutton,
			     gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton))
		settings.bs_navdirect = *((int *)user_data);
}

/******************************************************************************
 * Name
 *   on_radiobutton_biblesync_listen
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_radiobutton_biblesync_listen(GtkToggleButton * togglebutton,
 *					  gpointer user_data)
 *   *((int*)user_data) is settings.bs_listen_set's 0|1|2 selection.
 *
 * Description
 *   BibleSync listen set choice.  calls biblesync glue if necessary,
 *   to set or clear all listen bits.
 *
 * Return value
 *   void
 */

static void
on_radiobutton_biblesync_listen(GtkToggleButton *togglebutton,
				gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
		settings.bs_listen_set = *((int *)user_data);

		if (settings.bs_listen_set != 0) // not selective
		{
			biblesync_set_clear_all_listen((settings.bs_listen_set == 1)
							   ? TRUE
							   : FALSE);
		}
	}
}

/******************************************************************************
 * Name
 *   on_checkbutton11_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton11_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton11_toggled(GtkToggleButton *togglebutton,
			 gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "readaloud",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.readaloud = atoi(xml_get_value("misc", "readaloud"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.readaloud_item),
				       settings.readaloud);
}

/******************************************************************************
 * Name
 *   on_checkbutton12_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton12_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton12_toggled(GtkToggleButton *togglebutton,
			 gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "showversenum",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.showversenum =
	    atoi(xml_get_value("misc", "showversenum"));

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.showversenum_item),
				       settings.showversenum);
}

/******************************************************************************
 * Name
 *   on_checkbutton6_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton6_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton6_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	xml_set_value("Xiphos", "lexicons", "usedefaultdict",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.useDefaultDict =
	    gtk_toggle_button_get_active(togglebutton);
}

/******************************************************************************
 * Name
 *   on_checkbutton7_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton7_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton7_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "dailydevotional",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.showdevotional =
	    gtk_toggle_button_get_active(togglebutton);
}

/******************************************************************************
 * Name
 *   on_checkbutton8_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton8_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton8_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "splash",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.showsplash = gtk_toggle_button_get_active(togglebutton);
}

/******************************************************************************
 * Name
 *   on_checkbutton_scroll_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_scroll_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton_scroll_toggled(GtkToggleButton *togglebutton,
			      gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "chapter-scroll",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.chapter_scroll =
	    gtk_toggle_button_get_active(togglebutton);
}

/******************************************************************************
 * Name
 *   on_checkbutton_imageresize_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_imageresize_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton_imageresize_toggled(GtkToggleButton *togglebutton,
				   gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "imageresize",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.imageresize = gtk_toggle_button_get_active(togglebutton);
}

/******************************************************************************
 * Name
 *   on_checkbutton_verse_num_bold_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_verse_num_bold_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton_verse_num_bold_toggled(GtkToggleButton *togglebutton,
				      gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "verse_num_bold",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.verse_num_bold =
	    atoi(xml_get_value("misc", "verse_num_bold"));
	char *url = g_strdup_printf("sword:///%s", settings.currentverse);
	main_url_handler(url, TRUE);
	g_free(url);
}

/******************************************************************************
 * Name
 *   on_checkbutton_verse_num_bracket_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_verse_num_bracket_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton_verse_num_bracket_toggled(GtkToggleButton *togglebutton,
					 gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "verse_num_bracket",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.verse_num_bracket =
	    atoi(xml_get_value("misc", "verse_num_bracket"));
	char *url = g_strdup_printf("sword:///%s", settings.currentverse);
	main_url_handler(url, TRUE);
	g_free(url);
}

/******************************************************************************
 * Name
 *   on_checkbutton_verse_num_superscript_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_verse_num_superscript_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton_verse_num_superscript_toggled(GtkToggleButton *
						 togglebutton,
					     gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "verse_num_superscript",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.verse_num_superscript =
	    atoi(xml_get_value("misc", "verse_num_superscript"));
	char *url = g_strdup_printf("sword:///%s", settings.currentverse);
	main_url_handler(url, TRUE);
	g_free(url);
}

/******************************************************************************
 * Name
 *   on_checkbutton_versehighlight_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_versehighlight_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton_versehighlight_toggled(GtkToggleButton *togglebutton,
				      gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "versehighlight",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.versehighlight =
	    gtk_toggle_button_get_active(togglebutton);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widgets.versehighlight_item),
				       settings.versehighlight);
	main_display_bible(settings.MainWindowModule,
			   settings.currentverse);
}

/******************************************************************************
 * Name
 *   on_checkbutton_annotate_highlight_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_annotate_highlight_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *   En/disable yellow highlight on user's verse annotations.
 *
 * Return value
 *   void
 */

void
on_checkbutton_annotate_highlight_toggled(GtkToggleButton *togglebutton,
					  gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "annotatehighlight",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.annotate_highlight =
	    gtk_toggle_button_get_active(togglebutton);
	main_display_bible(settings.MainWindowModule,
			   settings.currentverse);
}

/******************************************************************************
 * Name
 *   on_checkbutton_xrefs_in_verse_list_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_xrefs_in_verse_list_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *   En/disable use of verse list for Bible cross-refs.
 *
 * Return value
 *   void
 */

void
on_checkbutton_xrefs_in_verse_list_toggled(GtkToggleButton *togglebutton,
					   gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "xrefsinverselist",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.xrefs_in_verse_list =
	    gtk_toggle_button_get_active(togglebutton);
}

/******************************************************************************
 * Name
 *   on_checkbutton_prayerlist_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_prayerlist_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton_prayerlist_toggled(GtkToggleButton *togglebutton,
				  gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "prayerlist",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.prayerlist = gtk_toggle_button_get_active(togglebutton);

	/* update module list to show choice */
	if (settings.prayerlist)
		gtk_widget_show(widgets.new_journal_item);
	else
		gtk_widget_hide(widgets.new_journal_item);
	main_update_module_lists();
	main_load_module_tree(sidebar.module_list);
}

/******************************************************************************
 * Name
 *   on_checkbutton_statusbar_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_statusbar_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton_statusbar_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "statusbar",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.statusbar = gtk_toggle_button_get_active(togglebutton);

	if (settings.statusbar)
		gtk_widget_show(widgets.appbar);
	else
		gtk_widget_hide(widgets.appbar);
}

/******************************************************************************
 * Name
 *   on_checkbutton_alternation_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_checkbutton_alternation_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_checkbutton_alternation_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "alternation",
		      (gtk_toggle_button_get_active(togglebutton) ? "1" : "0"));
	settings.alternation = gtk_toggle_button_get_active(togglebutton);

	main_update_parallel_page();	/* 1 verse */
	if (settings.dockedInt) {	/* whole chapter */
		main_update_parallel_page();
	} else {
		main_update_parallel_page_detached();
	}
}

/******************************************************************************
 * Name
 *   on_folder_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_folder_changed(GtkFileChooser * filechooser, gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void on_folder_changed(GtkFileChooser *filechooser, gpointer user_data)
{
	gchar *directory =
	    gtk_file_chooser_get_current_folder(filechooser);
	xml_set_value("Xiphos", "studypad", "directory", directory);
	settings.studypaddir = xml_get_value("studypad", "directory");
	g_free(directory);
}

/******************************************************************************
 * Name
 *   on_combobox1_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox1_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox1 (verse number size)
 *   has changed - update Biblical text pane
 *
 * Return value
 *  void
 */

void on_combobox1_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	xml_set_value("Xiphos", "fontsize", "versenum", buf);
	settings.verse_num_font_size_str =
	    xml_get_value("fontsize", "versenum");
	settings.verse_num_font_size =
	    atoi(settings.verse_num_font_size_str);
	url =
	    g_strdup_printf("sword://%s/%s", settings.MainWindowModule,
			    settings.currentverse);
	main_url_handler(url, TRUE);
	g_free(url);
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_basecombobox1_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_basecombobox1_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox1 (base font size)
 *   has changed - update Biblical text pane
 *
 * Return value
 *  void
 */

void on_basecombobox1_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	xml_set_value("Xiphos", "fontsize", "basefontsize", buf);
	g_free(buf);

	if (settings.base_font_size_str)
		g_free(settings.base_font_size_str);
	settings.base_font_size_str =
	    xml_get_value("fontsize", "basefontsize");

	settings.base_font_size = atoi(settings.base_font_size_str);
	url = g_strdup_printf("sword://%s/%s",
			      settings.MainWindowModule,
			      settings.currentverse);
	main_url_handler(url, TRUE);
	g_free(url);
	main_display_dictionary(settings.DictWindowModule,
				settings.dictkey);
}

/******************************************************************************
 * Name
 *   on_combobox2_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox2_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox2 (Biblical text module) or combobox3 (Commentary module),
 *   has changed - update Biblical text and Commentary panes
 *
 * Return value
 *  void
 */

void on_combobox2_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;

	url = g_strdup_printf("sword://%s/%s", buf, settings.currentverse);
	main_url_handler(url, TRUE);
	g_free(url);
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox4_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox4_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox4 (Dictionary module),
 *   has changed - update dictionary pane
 *
 * Return value
 *  void
 */

void on_combobox4_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;

	url = g_strdup_printf("sword://%s/%s", buf, settings.dictkey);
	main_url_handler(url, TRUE);
	g_free(url);
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox5_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox5_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox5 (default dictionary module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void on_combobox5_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;
	xml_set_value("Xiphos", "lexicons", "defaultdictionary", buf);
	settings.DefaultDict =
	    xml_get_value("lexicons", "defaultdictionary");
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox6_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox6_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox6 (Personal comments module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void on_combobox6_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;
	xml_set_value("Xiphos", "modules", "percomm", buf);
	settings.personalcommentsmod = xml_get_value("modules", "percomm");
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox12_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox12_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox12 (Daily Devotional module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void on_combobox12_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;
	xml_set_value("Xiphos", "modules", "devotional", buf);
	settings.devotionalmod = xml_get_value("modules", "devotional");
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox13_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox13_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox13 (Hebrew lex module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void on_combobox13_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;
	xml_set_value("Xiphos", "lexicons", "hebrew", buf);
	settings.lex_hebrew = xml_get_value("lexicons", "hebrew");
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox14_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox14_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox14 (Greek lex module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void on_combobox14_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;
	xml_set_value("Xiphos", "lexicons", "greek", buf);
	settings.lex_greek = xml_get_value("lexicons", "greek");
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox15_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox15_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox4 (Book module),
 *   has changed - update book pane
 *
 * Return value
 *  void
 */

void on_combobox15_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;
	url = g_strdup_printf("sword://%s/%s", buf, "1");
	main_url_handler(url, TRUE);
	g_free(url);
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox16_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox16_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox16 (special locale),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void on_combobox16_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	gboolean clear, set;

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;

	clear = !strcmp(buf, NONE);
	if (!clear) {
		/* something new was chosen:
		   move forward to and isolate "(xx_YY)" locale spec. */
		buf = strrchr(buf, '(') + 1;
		*strchr(buf, ')') = '\0';
	}

	set = ((!settings.special_locale && strcmp(buf, NONE)) ||
	       (settings.special_locale && (strcmp(settings.special_locale, buf))));
	if (clear || set)
		gui_generic_warning(_("Locale will take effect after restart."));

	if (clear)
		*buf = '\0';

	/* NOTE: These 2 uses of "None" are NOT TRANSLATABLE. */
	xml_set_value("Xiphos", "locale", "special",
		      (clear ? "None" : buf));
	g_free(settings.special_locale); /* dispose of old content */
	settings.special_locale =
	    (clear ? g_strdup("None") : g_strdup(buf));
}

/******************************************************************************
 * Name
 *   on_combobox17_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void on_combobox17_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox17 (language font preferences) => make a font selection.
 *
 * Return value
 *  void
 */

void on_combobox17_changed(GtkComboBox *combobox, gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	gchar *mod_name;

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --"))) /* see fill_combobox */
		return;

	buf = strrchr(buf, '(') + 1;
	*strchr(buf, ')') = '\0';

	mod_name = g_strdup_printf("Language:%s", buf);
	gui_set_module_font(mod_name);
	redisplay_to_realign();
	g_free(mod_name);
}

/******************************************************************************
 * Name
 *   add_columns
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void add_columns(GtkWidget * listview)
 *
 * Description
 *
 * Return value
 *   void
 */

static void add_columns(GtkWidget *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Preferences"),
						     renderer,
						     "text", 0, NULL);

	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

/******************************************************************************
 * Name
 *   tree_selection_changed
 *
 * Synopsis
 *   #include "gui/.h"
 *   void tree_selection_changed(GtkTreeSelection * selection,
 *		      GtkWidget * tree_widget)
 *
 * Description
 *
 * Return value
 *   void
 */

static void
tree_selection_changed(GtkTreeSelection *selection, gpointer data)
{
	GtkTreeIter selected;
	gint page;
	GtkTreeModel *model;

	if (!gtk_tree_selection_get_selected(selection, &model, &selected))
		return;

	gtk_tree_model_get(model, &selected, 1, &page, -1);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page);
}

/******************************************************************************
 * Name
 *   on_dialog_prefs_response
 *
 * Synopsis
 *   #include "gui/.h"
 *   void on_dialog_prefs_response(GtkDialog * dialog, gint response_id,
 *					gpointer user_data)
 *
 * Description
 *
 * Return value
 *   void
 */

void
on_dialog_prefs_response(GtkDialog *dialog,
			 gint response_id, gpointer user_data)
{
	if (response_id == GTK_RESPONSE_CLOSE) {
		settings.display_prefs = 0;
		xml_set_value("Xiphos", "layout", "prefsopen", "0");

		xml_save_settings_doc(settings.fnconfigure);
		gtk_widget_destroy(GTK_WIDGET(dialog));

		dialog_prefs = NULL;
		speaker_window = NULL;
		speaker_list = NULL;
	}
	main_update_parallel_page();
}

static GtkTreeModel *create_model(void)
{
	GtkTreeStore *model;
	GtkTreeIter iter;
	GtkTreeIter child_iter;

	model = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);

	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("General"), -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Options"), 1, 4, -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("BibleSync"), 1, 3,
			   -1);

	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("Fonts"), -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Color"), 1, 1, -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Sizes and Faces"), 1,
			   2, -1);

	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("Modules"), -1);

	/* the former element "5" was previously here,
	   which was for the "main modules," now defunct.
	   we maintain the numbering because 6 & 7 are fixed refs. */

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Parallel"), 1, 6, -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Special"), 1, 7, -1);

	return GTK_TREE_MODEL(model);
}

/******************************************************************************
 * Name
 *   setup_color_pickers
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   void setup_color_pickers(void)
 *
 * Description
 *
 * Return value
 *   void
 */

void setup_color_pickers(void)
{
#if GTK_CHECK_VERSION(3, 4, 0)
	GdkRGBA rgba;
	if (gdk_rgba_parse(&rgba, settings.bible_bg_color))
		gtk_color_chooser_set_rgba((GtkColorChooser *)
					   color_picker.text_background,
					   &rgba);
	if (gdk_rgba_parse(&rgba, settings.bible_text_color))
		gtk_color_chooser_set_rgba((GtkColorChooser *)
					   color_picker.text,
					   &rgba);
	if (gdk_rgba_parse(&rgba, settings.currentverse_color))
		gtk_color_chooser_set_rgba((GtkColorChooser *)
					   color_picker.text_current_verse,
					   &rgba);
	if (gdk_rgba_parse(&rgba, settings.bible_verse_num_color))
		gtk_color_chooser_set_rgba((GtkColorChooser *)
					   color_picker.verse_numbers,
					   &rgba);
	if (gdk_rgba_parse(&rgba, settings.link_color))
		gtk_color_chooser_set_rgba((GtkColorChooser *)
					   color_picker.href_links,
					   &rgba);
	if (gdk_rgba_parse(&rgba, settings.highlight_fg))
		gtk_color_chooser_set_rgba((GtkColorChooser *)
					   color_picker.highlight_fg,
					   &rgba);
	if (gdk_rgba_parse(&rgba, settings.highlight_bg))
		gtk_color_chooser_set_rgba((GtkColorChooser *)
					   color_picker.highlight_bg,
					   &rgba);
#else
	GdkColor color;
	if (string_is_color(settings.bible_bg_color)) {
		gdk_color_parse(settings.bible_bg_color, &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.text_background),
					   &color);
	} else {
		gdk_color_parse("#FFFFFF", &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.text_background),
					   &color);
	}
	if (string_is_color(settings.bible_text_color)) {
		gdk_color_parse(settings.bible_text_color, &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.text), &color);
	} else {
		gdk_color_parse("#000000", &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.text), &color);
	}

	if (string_is_color(settings.currentverse_color)) {
		gdk_color_parse(settings.currentverse_color, &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.text_current_verse), &color);
	} else {
		gdk_color_parse("#339766", &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.text_current_verse), &color);
	}

	if (string_is_color(settings.bible_verse_num_color)) {
		gdk_color_parse(settings.bible_verse_num_color, &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.verse_numbers),
					   &color);
	} else {
		gdk_color_parse("#0000CF", &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.verse_numbers),
					   &color);
	}

	if (string_is_color(settings.link_color)) {
		gdk_color_parse(settings.link_color, &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.href_links),
					   &color);
	} else {
		gdk_color_parse("#878787", &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.href_links),
					   &color);
	}

	// contrasty highlighting -- foreground.
	if (string_is_color(settings.highlight_fg))
		gdk_color_parse(settings.highlight_fg, &color);
	else
		gdk_color_parse("#FFFF00", &color);
	gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.highlight_fg), &color);

	// contrasty highlighting -- background.
	if (string_is_color(settings.highlight_bg))
		gdk_color_parse(settings.highlight_bg, &color);
	else
		gdk_color_parse("#060680", &color);
	gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.highlight_bg), &color);
#endif
}

static void setup_check_buttons(void)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.use_default_dictionary),
				     settings.useDefaultDict);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.show_devotion),
				     settings.showdevotional);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.show_splash_screen),
				     settings.showsplash);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.use_chapter_scroll),
				     settings.chapter_scroll);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.use_imageresize),
				     settings.imageresize);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.use_verse_num_bold),
				     settings.verse_num_bold);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.use_verse_num_bracket),
				     settings.verse_num_bracket);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.use_verse_num_superscript),
				     settings.verse_num_superscript);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.versehighlight),
				     settings.versehighlight);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.annotate_highlight),
				     settings.annotate_highlight);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.xrefs_in_verse_list),
				     settings.xrefs_in_verse_list);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.prayerlist),
				     settings.prayerlist);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.statusbar),
				     settings.statusbar);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.alternation),
				     settings.alternation);

	/* v-- BibleSync --v */
	/* toggles */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.bs_debug),
				     settings.bs_debug);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.bs_presence),
				     settings.bs_presence);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.bs_mismatch),
				     settings.bs_mismatch);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.bs_group_tab),
				     settings.bs_group_tab);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button.bs_privacy),
				     settings.bs_privacy);
	gtk_widget_set_sensitive(check_button.bs_privacy,
				 biblesync_personal());

	/* mode */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_mode_off),
				     (settings.bs_mode == 0));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_mode_personal),
				     (settings.bs_mode == 1));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_mode_speaker),
				     (settings.bs_mode == 2));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_mode_audience),
				     (settings.bs_mode == 3));

	/* navigation method */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_nav_direct),
				     (settings.bs_navdirect == 1));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_nav_verselist),
				     (settings.bs_navdirect == 0));

	/* listening choice */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_listen_some),
				     (settings.bs_listen_set == 0));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_listen_all),
				     (settings.bs_listen_set == 1));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button.bs_listen_none),
				     (settings.bs_listen_set == 2));
	/* ^-- BibleSync --^ */

	g_signal_connect(check_button.use_default_dictionary, "toggled",
			 G_CALLBACK(on_checkbutton6_toggled), NULL);
	g_signal_connect(check_button.show_devotion, "toggled",
			 G_CALLBACK(on_checkbutton7_toggled), NULL);
	g_signal_connect(check_button.show_splash_screen, "toggled",
			 G_CALLBACK(on_checkbutton8_toggled), NULL);
	g_signal_connect(check_button.use_chapter_scroll, "toggled",
			 G_CALLBACK(on_checkbutton_scroll_toggled), NULL);
	g_signal_connect(check_button.use_imageresize, "toggled",
			 G_CALLBACK(on_checkbutton_imageresize_toggled),
			 NULL);
	g_signal_connect(check_button.use_verse_num_bold, "toggled",
			 G_CALLBACK(on_checkbutton_verse_num_bold_toggled),
			 NULL);
	g_signal_connect(check_button.use_verse_num_bracket, "toggled",
			 G_CALLBACK(on_checkbutton_verse_num_bracket_toggled), NULL);
	g_signal_connect(check_button.use_verse_num_superscript, "toggled",
			 G_CALLBACK(on_checkbutton_verse_num_superscript_toggled),
			 NULL);
	g_signal_connect(check_button.versehighlight, "toggled",
			 G_CALLBACK(on_checkbutton_versehighlight_toggled),
			 NULL);
	g_signal_connect(check_button.annotate_highlight, "toggled",
			 G_CALLBACK(on_checkbutton_annotate_highlight_toggled),
			 NULL);
	g_signal_connect(check_button.xrefs_in_verse_list, "toggled",
			 G_CALLBACK(on_checkbutton_xrefs_in_verse_list_toggled),
			 NULL);
	g_signal_connect(check_button.prayerlist, "toggled",
			 G_CALLBACK(on_checkbutton_prayerlist_toggled),
			 NULL);
	g_signal_connect(check_button.statusbar, "toggled",
			 G_CALLBACK(on_checkbutton_statusbar_toggled),
			 NULL);
	g_signal_connect(check_button.alternation, "toggled",
			 G_CALLBACK(on_checkbutton_alternation_toggled),
			 NULL);

	/* v-- BibleSync --v */
	g_signal_connect(check_button.bs_debug, "toggled",
			 G_CALLBACK(on_checkbutton_biblesync_toggled),
			 &settings.bs_debug);
	g_signal_connect(check_button.bs_presence, "toggled",
			 G_CALLBACK(on_checkbutton_biblesync_toggled),
			 &settings.bs_presence);
	g_signal_connect(check_button.bs_mismatch, "toggled",
			 G_CALLBACK(on_checkbutton_biblesync_toggled),
			 &settings.bs_mismatch);
	g_signal_connect(check_button.bs_group_tab, "toggled",
			 G_CALLBACK(on_checkbutton_biblesync_toggled),
			 &settings.bs_group_tab);
	g_signal_connect(check_button.bs_privacy, "toggled",
			 G_CALLBACK(on_checkbutton_biblesync_toggled),
			 &settings.bs_privacy);

	g_signal_connect(radio_button.bs_mode_off, "toggled",
			 G_CALLBACK(on_radiobutton_biblesync_mode),
			 &rb_cb[0]);
	g_signal_connect(radio_button.bs_mode_personal, "toggled",
			 G_CALLBACK(on_radiobutton_biblesync_mode),
			 &rb_cb[1]);
	g_signal_connect(radio_button.bs_mode_speaker, "toggled",
			 G_CALLBACK(on_radiobutton_biblesync_mode),
			 &rb_cb[2]);
	g_signal_connect(radio_button.bs_mode_audience, "toggled",
			 G_CALLBACK(on_radiobutton_biblesync_mode),
			 &rb_cb[3]);

	g_signal_connect(radio_button.bs_nav_direct, "toggled",
			 G_CALLBACK(on_radiobutton_biblesync_nav),
			 &rb_cb[1]);
	g_signal_connect(radio_button.bs_nav_verselist, "toggled",
			 G_CALLBACK(on_radiobutton_biblesync_nav),
			 &rb_cb[0]);

	g_signal_connect(radio_button.bs_listen_some, "toggled",
			 G_CALLBACK(on_radiobutton_biblesync_listen),
			 &rb_cb[0]);
	g_signal_connect(radio_button.bs_listen_all, "toggled",
			 G_CALLBACK(on_radiobutton_biblesync_listen),
			 &rb_cb[1]);
	g_signal_connect(radio_button.bs_listen_none, "toggled",
			 G_CALLBACK(on_radiobutton_biblesync_listen),
			 &rb_cb[2]);
	/* ^-- BibleSync --^ */
}

/*
 * comparator function to determine filling of true general dict lists.
 */
static gboolean dict_no_image_map_dd(char *modname, char *feature)
{
	/* no features or categories */
	const char *f = main_get_mod_config_entry(modname, "Feature");
	const char *c = main_get_mod_config_entry(modname, "Category");
	return ((c == NULL) && (f == NULL));
}

/*
 * comparator function to determine filling of heb/grk dict lists.
 */
static gboolean dict_match_feature(char *modname, char *feature)
{
	const char *f = main_get_mod_config_entry(modname, "Feature");
	return (f && (!strcmp(f, feature)));
}

static void fill_combobox(GList *glist, GtkComboBox *combo, gchar *current_item, gboolean (*eval)(char *, char *), /* evaluator of feature */
			  char *feature)
{ /* feature sought or avoided */
	GtkTreeIter iter;
	GtkListStore *store;
	GtkCellRenderer *renderer;
	gint index = 0;
	gint i = 0;

	store = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_combo_box_set_model(combo, GTK_TREE_MODEL(store));

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer, "text", 0, NULL);

	gtk_list_store_clear(GTK_LIST_STORE(store));

	gtk_list_store_append(GTK_LIST_STORE(store), &iter);
	gtk_list_store_set(GTK_LIST_STORE(store),
			   &iter, 0, _("-- Select --"), -1);

	while (glist) {
		if ((eval == NULL) || (*eval)(glist->data, feature)) {
			/* "anything goes" or "acceptable match" */
			gtk_list_store_append(GTK_LIST_STORE(store),
					      &iter);
			gtk_list_store_set(GTK_LIST_STORE(store), &iter, 0,
					   (gchar *)glist->data, -1);
			if (current_item && !strcmp((gchar *)glist->data, current_item))
				index = i + 1;
			++i;
		}
		glist = g_list_next(glist);
	}
	gtk_combo_box_set_active(combo, index);
}

static gint get_font_size_index(const char *font)
{
	if (!strcmp(font, "-2"))
		return 0;
	if (!strcmp(font, "-1"))
		return 1;
	if (!strcmp(font, "+0"))
		return 2;
	if (!strcmp(font, "+1"))
		return 3;
	if (!strcmp(font, "+2"))
		return 4;
	if (!strcmp(font, "+3"))
		return 5;
	if (!strcmp(font, "+4"))
		return 6;
	if (!strcmp(font, "+5"))
		return 7;
	return 0;
}

static void setup_module_comboboxes(void)
{
	/*
	 * Miscellaneous Modules page
	 */
	fill_combobox(get_list(DEVOTION_LIST),
		      GTK_COMBO_BOX(combo.devotion_module),
		      settings.devotionalmod, NULL, NULL);
	fill_combobox(get_list(DICT_LIST),
		      GTK_COMBO_BOX(combo.hebrew_lex__module),
		      settings.lex_hebrew, dict_match_feature,
		      "HebrewDef");
	fill_combobox(get_list(DICT_LIST),
		      GTK_COMBO_BOX(combo.greek_lex__module),
		      settings.lex_greek, dict_match_feature, "GreekDef");
	fill_combobox(get_list(DICT_LIST),
		      GTK_COMBO_BOX(combo.default_dictionary_module),
		      settings.DefaultDict, dict_no_image_map_dd, NULL);
	fill_combobox(get_list(PERCOMM_LIST),
		      GTK_COMBO_BOX(combo.percomm_module),
		      settings.personalcommentsmod, NULL, NULL);

	/*
	 * signal connectivity
	 */
	g_signal_connect(combo.devotion_module, "changed",
			 G_CALLBACK(on_combobox12_changed), NULL);
	g_signal_connect(combo.hebrew_lex__module, "changed",
			 G_CALLBACK(on_combobox13_changed), NULL);
	g_signal_connect(combo.greek_lex__module, "changed",
			 G_CALLBACK(on_combobox14_changed), NULL);
	g_signal_connect(combo.default_dictionary_module, "changed",
			 G_CALLBACK(on_combobox5_changed), NULL);
	g_signal_connect(combo.percomm_module, "changed",
			 G_CALLBACK(on_combobox6_changed), NULL);
}

void setup_locale_combobox(void)
{
	char **locale, brief_locale[3], *real_locale, *current_locale =
							  NULL;
	GList *chase, *list = g_list_append(NULL, g_strdup(NONE));

	brief_locale[2] = '\0';
	for (locale = &locale_set[0]; *locale; ++locale) {
		brief_locale[0] = (*locale)[0];
		brief_locale[1] = (*locale)[1];
		real_locale = g_strdup_printf("%s (%s)",
					      main_get_language_map(brief_locale), *locale);
		list = g_list_append(list, real_locale);
		if (settings.special_locale &&
		    !strcmp(settings.special_locale, *locale))
			current_locale = real_locale;
	}
	fill_combobox(list, GTK_COMBO_BOX(combo.special_locale),
		      (current_locale ? current_locale : NONE),
		      NULL, NULL);
	for (chase = list; chase; chase = g_list_next(chase))
		g_free(chase->data);
	g_list_free(list);

	g_signal_connect(combo.special_locale, "changed",
			 G_CALLBACK(on_combobox16_changed), NULL);
}

void setup_font_prefs_combobox(void)
{
	char **language_list = main_get_module_language_list();
	char **language;
	GList *chase, *list = NULL;

	for (language = &language_list[0]; *language; ++language) {
		char *real_language =
			g_strdup_printf("%s (%s)",
					main_get_language_map(*language), *language);
		list = g_list_append(list, real_language);
	}
	fill_combobox(list, GTK_COMBO_BOX(combo.font_prefs),
		      NULL, NULL, NULL);
	for (chase = list; chase; chase = g_list_next(chase))
		g_free(chase->data);
	g_list_free(list);
	g_strfreev(language_list);

	g_signal_connect(combo.font_prefs, "changed",
			 G_CALLBACK(on_combobox17_changed), NULL);
}

/******************************************************************************
 * Name
 *   button_release_event
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *   static gboolean button_release_event(GtkWidget * widget,
 *					  GdkEventButton * event,
 *					  gpointer data)
 *
 * Description
 *   click on treeview folder to expand or collapse it
 *
 * Return value
 *   gboolean
 */

static gboolean button_release_event(GtkWidget *widget,
				     GdkEventButton *event, gpointer data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeIter selected;
	GtkTreeModel *model;
	GtkTreePath *path;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	if ((!gtk_tree_selection_get_selected(selection, &model, &selected)) || (!gtk_tree_model_iter_has_child(model, &selected)))
		return FALSE;

	path = gtk_tree_model_get_path(model, &selected);
	if (gtk_tree_view_row_expanded(GTK_TREE_VIEW(widget), path))
		gtk_tree_view_collapse_row(GTK_TREE_VIEW(widget), path);
	else
		gtk_tree_view_expand_row(GTK_TREE_VIEW(widget), path,
					 FALSE);
	gtk_tree_path_free(path);
	return FALSE;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *   static void _setup_listview
 *
 * Description
 *   init the listing of currently-set parallel modules.
 *
 * Return value
 *
 */

static void ps_setup_listview()
{
	GtkListStore *model;
	GtkTreeViewColumn *column;
	GtkTreeIter iter;
	GtkTreeModel *model_t;
	GtkListStore *list_store;
	int i;

	model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(parallel_select.listview),
				GTK_TREE_MODEL(model));

	for (i = 0; i < 2; ++i) {
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("Module",
								  renderer,
								  "text",
								  i, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(parallel_select.listview),
					    column);
	}
	gtk_tree_view_column_set_sort_column_id(column, 0);

	model_t =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(parallel_select.listview));
	list_store = GTK_LIST_STORE(model_t);
	gtk_list_store_clear(list_store);

	if (settings.parallel_list) {
		for (i = 0; settings.parallel_list[i]; ++i) {
			const char *abbreviation =
			    main_get_abbreviation(settings.parallel_list
						      [i]);
			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter,
					   0, main_get_module_description(settings.parallel_list[i]),
					   1,
					   (abbreviation
						? abbreviation
						: (gchar *)
						  settings.parallel_list[i]),
					   -1);
		}
	}
}

/******************************************************************************
 * Name
 *   modules_lists_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *   void modules_lists_changed(GtkTreeSelection * selection,
 *		     					 gpointer data)
 *
 * Description
 *
 * Return value
 *   void
 */

static gchar *module_selected = NULL;

static void modules_lists_changed(GtkTreeSelection *selection,
				  GtkTreeView *tree_widget)
{
	gchar *mod = NULL;
	GtkTreeIter selected;
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(tree_widget));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	if (gtk_tree_model_iter_has_child(model, &selected)) {
		g_free(module_selected);
		module_selected = NULL;
		return;
	}

	gtk_tree_model_get(model, &selected, UTIL_COL_MODULE, &mod, -1);
	if (mod) {
		g_free(module_selected);
		module_selected = mod;
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *   void ps_setup_treeview(GtkWidget * treeview)
 *
 * Description
 *
 *
 * Return value
 *
 */

static void ps_setup_treeview(GtkWidget *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GObject *selection;

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Found",
							  renderer,
							  "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gui_load_module_tree(treeview, TRUE);

	selection =
	    G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
	g_signal_connect_after(G_OBJECT(treeview), "button_release_event",
			       G_CALLBACK(button_release_event),
			       GINT_TO_POINTER(0));
	g_signal_connect(selection, "changed",
			 G_CALLBACK(modules_lists_changed), treeview);
}

static void on_mod_sel_add_clicked(GtkWidget *button, gchar *user_data)
{
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	char *parallels = g_strdup(""), *newhold;
	const char *abbreviation;
	int count;

	if (!module_selected) {
		g_free(parallels);
		return;
	}
	abbreviation = main_get_abbreviation(module_selected);

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(parallel_select.listview));
	list_store = GTK_LIST_STORE(model);

	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, main_get_module_description(module_selected),
			   1,
			   (abbreviation ? abbreviation : module_selected),
			   -1);

	if (settings.parallel_list == NULL) {
		settings.parallel_list =
		    g_strsplit(module_selected, ",", -1);
	} else {
		for (count = 0; settings.parallel_list[count]; ++count)
			; /* count up the set */
		settings.parallel_list =
		    g_renew(gchar *, settings.parallel_list, count + 2);
		settings.parallel_list[count] = g_strdup(module_selected); /* new element */
		settings.parallel_list[count + 1] = NULL;
	}

	for (count = 0; settings.parallel_list[count]; ++count) {
		newhold =
		    g_strconcat(parallels, settings.parallel_list[count],
				",", NULL);
		g_free(parallels);
		parallels = newhold;
	}

	if (*parallels) {
		*(parallels + strlen(parallels) - 1) = '\0'; /* end comma */
		xml_set_value("Xiphos", "modules", "parallels", parallels);
	}
	g_free(parallels);
}

static void on_mod_sel_close_clicked(void)
{
	gtk_widget_destroy(GTK_WIDGET(parallel_select.mod_sel_dialog));
}

/******************************************************************************
 * Name
 *   ps_button_clear
 *
 * Synopsis
 *   void ps_button_clear(GtkButton * button, gpointer user_data)
 *
 * Description
 *   clear all modules from the list. (parallel select)
 *
 * Return value
 *   void
 */
void ps_button_clear(GtkButton *button, gpointer user_data)
{
	gchar *str =
		g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
				_("Clear List?"),
				_("Are you sure you want to clear the module list?"));

#if GTK_CHECK_VERSION(3, 10, 0)
	if (gui_yes_no_dialog(str, "dialog-warning")) {
#else
	if (gui_yes_no_dialog(str, GTK_STOCK_DIALOG_WARNING)) {
#endif

		GtkTreeModel *model =
			gtk_tree_view_get_model(GTK_TREE_VIEW(parallel_select.listview));
		GtkListStore *list_store = GTK_LIST_STORE(model);
		gtk_list_store_clear(list_store);
		if (settings.parallel_list)
			g_strfreev(settings.parallel_list);
		settings.parallel_list = NULL;
		xml_set_value("Xiphos", "modules", "parallels", "");
	}
	g_free(str);
}

/******************************************************************************
 * Name
 *   ps_button_cut
 *
 * Synopsis
 *   void ps_button_cut(GtkButton * button, gpointer user_data)
 *
 * Description
 *   cut one module from the list. (parallel select)
 *
 * Return value
 *   void
 */
void ps_button_cut(GtkButton *button, gpointer user_data)
{
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *str;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(parallel_select.listview));
	list_store = GTK_LIST_STORE(model);
	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(parallel_select.listview));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
			      _("Remove Module?"),
			      _("Are you sure you want to remove the selected module?"));

#if GTK_CHECK_VERSION(3, 10, 0)
	if (gui_yes_no_dialog(str, "dialog-warning")) {
#else
	if (gui_yes_no_dialog(str, GTK_STOCK_DIALOG_WARNING)) {
#endif
		gtk_list_store_remove(list_store, &selected);
		GList *mods =
		    get_current_list(GTK_TREE_VIEW(parallel_select.listview));
		gchar *mod_list = get_modlist_string(mods);
		if (settings.parallel_list)
			g_strfreev(settings.parallel_list);
		settings.parallel_list = g_strsplit(mod_list, ",", -1);
		xml_set_value("Xiphos", "modules", "parallels", mod_list);
		g_free(mod_list);
	}
	g_free(str);
}

/******************************************************************************
 * Name
 *   ps_button_add
 *
 * Synopsis
 *   void ps_button_add(GtkButton * button, gpointer user_data)
 *
 * Description
 *   add one module from the list. (parallel select)
 *
 * Return value
 *   void
 */
void ps_button_add(GtkButton *button, gpointer user_data)
{
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml;
#else
	GladeXML *gxml;
#endif
	gchar *glade_file =
	    gui_general_user_file("selector-prefs" UI_SUFFIX, FALSE);
	g_return_if_fail(glade_file != NULL);

#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	gxml = glade_xml_new(glade_file, "mod_sel_dialog", NULL);
#endif
	parallel_select.mod_sel_dialog =
	    UI_GET_ITEM(gxml, "mod_sel_dialog");
	parallel_select.mod_sel_close =
	    UI_GET_ITEM(gxml, "mod_sel_button_close");
	parallel_select.mod_sel_add =
	    UI_GET_ITEM(gxml, "mod_sel_button_add");

	g_signal_connect((gpointer)parallel_select.mod_sel_close,
			 "clicked", G_CALLBACK(on_mod_sel_close_clicked),
			 NULL);
	g_signal_connect((gpointer)parallel_select.mod_sel_add, "clicked",
			 G_CALLBACK(on_mod_sel_add_clicked), NULL);

	parallel_select.mod_sel_treeview =
	    UI_GET_ITEM(gxml, "mod_sel_treeview");
	ps_setup_treeview(parallel_select.mod_sel_treeview);

	gtk_widget_show(parallel_select.mod_sel_dialog);
	g_free(glade_file);
}

/******************************************************************************
 * Name
 *   gui_create_preferences_dialog
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *   GtkWidget *gui_create_preferences_dialog(GList * biblelist,
 *					 GList * commlist,
 *					 GList * dictlist,
 *					 GList * percomlist,
 *					 GList * devotionlist)
 *
 * Description
 *   create preferences dialog and set to current values
 *
 * Return value
 *   GtkWidget *
 */

static void create_preferences_dialog(void)
{
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml;
#else
	GladeXML *gxml;
#endif
	GtkWidget *treeview;
	GtkTreeModel *model;
	GObject *selection;
	GtkWidget *chooser;
	gint index = 0;
	gchar *glade_file = gui_general_user_file("prefs" UI_SUFFIX, TRUE);

	g_return_if_fail(glade_file != NULL);

/* build the widget */
#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	gxml = glade_xml_new(glade_file, NULL, NULL);
#endif
	g_free(glade_file);
	g_return_if_fail(gxml != NULL);

	/* lookup the root widget */
	dialog_prefs = UI_GET_ITEM(gxml, "dialog_prefs");
	gtk_window_resize(GTK_WINDOW(dialog_prefs),
			  settings.prefs_width, settings.prefs_height);
	g_signal_connect(dialog_prefs, "response",
			 G_CALLBACK(on_dialog_prefs_response), NULL);

	/* color pickers */
	color_picker.text_background = UI_GET_ITEM(gxml, "colorbutton1");
	color_picker.text = UI_GET_ITEM(gxml, "colorbutton2");
	color_picker.text_current_verse = UI_GET_ITEM(gxml, "colorbutton3");
	color_picker.verse_numbers = UI_GET_ITEM(gxml, "colorbutton4");
	color_picker.href_links    = UI_GET_ITEM(gxml, "colorbutton5");
	color_picker.highlight_fg  = UI_GET_ITEM(gxml, "colorbutton6");
	color_picker.highlight_bg  = UI_GET_ITEM(gxml, "colorbutton7");

	color_picker.invert_normal = UI_GET_ITEM(gxml, "invert_normal");
	color_picker.invert_highlight =
	    UI_GET_ITEM(gxml, "invert_highlight");

	check_button.use_linked_tabs        = UI_GET_ITEM(gxml, "checkbutton10");
	check_button.readaloud              = UI_GET_ITEM(gxml, "checkbutton11");
	check_button.show_verse_num         = UI_GET_ITEM(gxml, "checkbutton12");
	check_button.use_default_dictionary = UI_GET_ITEM(gxml, "checkbutton6");
	check_button.show_devotion          = UI_GET_ITEM(gxml, "checkbutton7");
	check_button.show_splash_screen     = UI_GET_ITEM(gxml, "checkbutton8");
	check_button.use_chapter_scroll     = UI_GET_ITEM(gxml, "checkbutton_scroll");

	check_button.use_imageresize = UI_GET_ITEM(gxml, "checkbutton_imageresize");
#ifdef WIN32
	/* webkit image hackery requires resize always be enabled. */
	gtk_widget_hide(check_button.use_imageresize);
#endif
	check_button.use_verse_num_bold        = UI_GET_ITEM(gxml, "checkbutton_verse_num_bold");
	check_button.use_verse_num_bracket     = UI_GET_ITEM(gxml, "checkbutton_verse_num_bracket");
	check_button.use_verse_num_superscript = UI_GET_ITEM(gxml, "checkbutton_verse_num_superscript");
	check_button.versehighlight      = UI_GET_ITEM(gxml, "checkbutton_versehighlight");
	check_button.annotate_highlight  = UI_GET_ITEM(gxml, "checkbutton_annotate_highlight");
	check_button.xrefs_in_verse_list = UI_GET_ITEM(gxml, "checkbutton_xrefs_in_verse_list");
	check_button.prayerlist          = UI_GET_ITEM(gxml, "checkbutton_prayerlist");
	check_button.statusbar           = UI_GET_ITEM(gxml, "checkbutton_statusbar");
	check_button.alternation         = UI_GET_ITEM(gxml, "checkbutton_alternation");

	/* v-- BibleSync --v */
	check_button.bs_debug     = UI_GET_ITEM(gxml, "checkbutton_BSP_nav_debug");
	check_button.bs_presence  = UI_GET_ITEM(gxml, "checkbutton_BSP_presence");
	check_button.bs_mismatch  = UI_GET_ITEM(gxml, "checkbutton_BSP_mismatch");
	check_button.bs_group_tab = UI_GET_ITEM(gxml, "checkbutton_BSP_group_tab");
	check_button.bs_privacy   = UI_GET_ITEM(gxml, "checkbutton_BSP_privacy");

	radio_button.bs_mode_off      = UI_GET_ITEM(gxml, "radiobutton_BSP_off");
	radio_button.bs_mode_personal = UI_GET_ITEM(gxml, "radiobutton_BSP_personal");
	radio_button.bs_mode_speaker  = UI_GET_ITEM(gxml, "radiobutton_BSP_speaker");
	radio_button.bs_mode_audience = UI_GET_ITEM(gxml, "radiobutton_BSP_audience");

	radio_button.bs_nav_direct    = UI_GET_ITEM(gxml, "radiobutton_BSP_nav_direct");
	radio_button.bs_nav_verselist = UI_GET_ITEM(gxml, "radiobutton_BSP_nav_verselist");

	radio_button.bs_listen_some = UI_GET_ITEM(gxml, "radiobutton_BSP_listen_some");
	radio_button.bs_listen_all  = UI_GET_ITEM(gxml, "radiobutton_BSP_listen_all");
	radio_button.bs_listen_none = UI_GET_ITEM(gxml, "radiobutton_BSP_listen_none");

	speaker_window = UI_GET_ITEM(gxml, "speakerwindow");
	biblesync_update_speaker();
	/* ^-- BibleSync --^ */

	g_signal_connect(color_picker.text_background, "color_set",
			 G_CALLBACK(on_colorbutton1_color_set), NULL);
	g_signal_connect(color_picker.text, "color_set",
			 G_CALLBACK(on_colorbutton2_color_set), NULL);
	g_signal_connect(color_picker.text_current_verse, "color_set",
			 G_CALLBACK(on_colorbutton3_color_set), NULL);
	g_signal_connect(color_picker.verse_numbers, "color_set",
			 G_CALLBACK(on_colorbutton4_color_set), NULL);
	g_signal_connect(color_picker.href_links, "color_set",
			 G_CALLBACK(on_colorbutton5_color_set), NULL);
	g_signal_connect(color_picker.highlight_fg, "color_set",
			 G_CALLBACK(on_colorbutton6_color_set), NULL);
	g_signal_connect(color_picker.highlight_bg, "color_set",
			 G_CALLBACK(on_colorbutton7_color_set), NULL);
	setup_color_pickers();

	/* color inverters */
	g_signal_connect((gpointer)color_picker.invert_normal, "clicked",
			 G_CALLBACK(on_invert), (void *)1);
	g_signal_connect((gpointer)color_picker.invert_highlight,
			 "clicked", G_CALLBACK(on_invert), (void *)0);

	/* check buttons */

	/* chapter scroll doesn't work in anything but gtkhtml3. */
	gtk_widget_hide(check_button.use_chapter_scroll);

	setup_check_buttons();

	/* verse number size */
	index = get_font_size_index(settings.verse_num_font_size_str);
	combo.verse_number_size = UI_GET_ITEM(gxml, "combobox1");
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo.verse_number_size),
				 index);
	g_signal_connect(combo.verse_number_size, "changed",
			 G_CALLBACK(on_combobox1_changed), NULL);

	/* base font size */
	index = get_font_size_index(settings.base_font_size_str);
	combo.base_font_size = UI_GET_ITEM(gxml, "basecombobox1");
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo.base_font_size),
				 index);
	g_signal_connect(combo.base_font_size, "changed",
			 G_CALLBACK(on_basecombobox1_changed), NULL);

	/* module combos */
	combo.default_dictionary_module = UI_GET_ITEM(gxml, "combobox5");
	combo.percomm_module = UI_GET_ITEM(gxml, "combobox6");
	combo.devotion_module = UI_GET_ITEM(gxml, "combobox12");
	combo.hebrew_lex__module = UI_GET_ITEM(gxml, "combobox13");
	combo.greek_lex__module = UI_GET_ITEM(gxml, "combobox14");
	setup_module_comboboxes();

	combo.special_locale = UI_GET_ITEM(gxml, "combobox16");
	setup_locale_combobox();

	combo.font_prefs = UI_GET_ITEM(gxml, "combobox17");
	setup_font_prefs_combobox();

	/* studypad directory chooserbutton */
	chooser = UI_GET_ITEM(gxml, "filechooserbutton1");
	gtk_file_chooser_set_current_folder((GtkFileChooser *)chooser,
					    settings.studypaddir);
	g_signal_connect(chooser, "current_folder_changed",
			 G_CALLBACK(on_folder_changed), NULL);

	/* prefs notebook */
	notebook = UI_GET_ITEM(gxml, "notebook");
	/* setup treeview */
	model = create_model();
	treeview = UI_GET_ITEM(gxml, "treeview");
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
	gtk_widget_set_size_request(treeview, 130, -1);
	add_columns(treeview);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));
	selection =
	    G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
/* connect signals and data */
#ifdef USE_GTKBUILDER
	gtk_builder_connect_signals_full(gxml, (GtkBuilderConnectFunc)gui_glade_signal_connect_func,
					 NULL);
#else
	glade_xml_signal_autoconnect_full(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func,
					  NULL);
#endif

	g_signal_connect(selection, "changed",
			 G_CALLBACK(tree_selection_changed), model);

	/*
	 * parallel select dialog: chooser and button connectivity
	 */
	parallel_select.button_clear =
	    UI_GET_ITEM(gxml, "ps_toolbutton_clear");
	parallel_select.button_cut =
	    UI_GET_ITEM(gxml, "ps_toolbutton_cut");
	parallel_select.button_add =
	    UI_GET_ITEM(gxml, "ps_toolbutton_add");
	parallel_select.listview = UI_GET_ITEM(gxml, "ps_listview");

	g_signal_connect(parallel_select.button_clear, "clicked",
			 G_CALLBACK(ps_button_clear), NULL);
	g_signal_connect(parallel_select.button_cut, "clicked",
			 G_CALLBACK(ps_button_cut), NULL);
	g_signal_connect(parallel_select.button_add, "clicked",
			 G_CALLBACK(ps_button_add), NULL);

	ps_setup_listview();

	/* geometry notifications */
	g_signal_connect((gpointer)dialog_prefs,
			 "configure_event",
			 G_CALLBACK(on_prefs_configure_event), NULL);

	settings.display_prefs = 1;
	xml_set_value("Xiphos", "layout", "prefsopen", "1");

	/*
	 * (from xiphos.c)
	 * a little paranoia:
	 * clamp geometry values to a reasonable bound.
	 * sometimes xiphos gets insane reconfig events as it dies,
	 * especially if it's due to just shutting linux down.
	 */
	if ((settings.prefs_x < 0) || (settings.prefs_x > 2000))
		settings.prefs_x = 40;
	if ((settings.prefs_y < 0) || (settings.prefs_y > 2000))
		settings.prefs_y = 40;

	gtk_window_move(GTK_WINDOW(dialog_prefs), settings.prefs_x,
			settings.prefs_y);
}

/******************************************************************************
 * Name
 *   gui_setup_preferences_dialog
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void gui_setup_preferences_dialog(void)
 *
 * Description
 *   get module list and setup preferences dialog call
 *   gui_create_preferences_dialog() to create dialog
 *
 * Return value
 *   void
 */

void gui_setup_preferences_dialog(void)
{
	if (dialog_prefs == NULL) {
		create_preferences_dialog();
	} else
		gdk_window_raise(gtk_widget_get_window(GTK_WIDGET(dialog_prefs)));
}
