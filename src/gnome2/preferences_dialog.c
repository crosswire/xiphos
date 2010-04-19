/*
 * Xiphos Bible Study Tool
 * preferences_dialog.c - get user preferences
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

#include <gtk/gtk.h>
#include <gtk/gtk.h>
#include <glade/glade-xml.h>

#include "gui/bibletext.h"
#include "gui/dialog.h"
#include "gui/xiphos.h"
#include "gui/preferences_dialog.h"
#include "gui/utilities.h"
#include "gui/gbs.h"
#include "gui/commentary.h"
#include "gui/dictlex.h"
#include "gui/parallel_view.h"
#include "gui/main_window.h"
#include "gui/sidebar.h"
#include "gui/tabbed_browser.h"
#include "gui/widgets.h"
#include "gui/font_dialog.h"

#include "main/sword.h"
#include "main/lists.h"
#include "main/mod_mgr.h"
#include "main/parallel_view.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/xml.h"
#include "main/url.hh"

#include "gui/debug_glib_null.h"

typedef struct _preferences_combo COMBOBOXS;
struct _preferences_combo {
	GtkWidget *text_module;	/*  Main Window Module  */
	GtkWidget *commentary_module;
	GtkWidget *dictionary_module;
	GtkWidget *default_dictionary_module;
	GtkWidget *book_module;
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
struct _preferences_color_pickers {
	GtkWidget *text;
	GtkWidget *text_background;
	GtkWidget *text_current_verse;
	GtkWidget *verse_numbers;
	GtkWidget *href_links;
	GtkWidget *highlight_fg;
	GtkWidget *highlight_bg;
};

typedef struct _preferences_check_buttons CHECK_BUTTONS;
struct _preferences_check_buttons {
	GtkWidget *use_defaults;	/* radio button */

	GtkWidget *enable_tabbed_browsing;
	GtkWidget *show_bible_tabs;
	GtkWidget *show_commentary_tabs;
	GtkWidget *show_dictionary_tabs;
	GtkWidget *show_book_tabs;

	GtkWidget *use_default_dictionary;
	GtkWidget *use_linked_tabs;
	GtkWidget *use_chapter_scroll;
	GtkWidget *use_imageresize;
	GtkWidget *readaloud;
	GtkWidget *show_verse_num;
	GtkWidget *versehighlight;
	GtkWidget *annotate_highlight;
	GtkWidget *show_splash_screen;
	GtkWidget *prayerlist;

	GtkWidget *show_bible_pane;
	GtkWidget *show_preview_pane;
	GtkWidget *show_commentary_pane;
	GtkWidget *show_dictionary_pane;

	GtkWidget *show_in_viewer;
	GtkWidget *show_in_dictionary;
	GtkWidget *show_devotion;
	GtkWidget *show_paratab;
};

typedef struct _preferences_buttons BUTTONS;
struct _preferences_buttons {
	GtkWidget *xiphos_defaults;
};

typedef struct _parallel_select PARALLEL_SELECT;
struct _parallel_select {
	GtkWidget *button_clear;
	GtkWidget *button_cut;
	GtkWidget *button_add;
	GtkWidget *listview;
	GtkWidget *mod_sel_dialog;
	GtkWidget *mod_sel_close;
	GtkWidget *mod_sel_add;
	GtkWidget *mod_sel_treeview;
};


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
static PARALLEL_SELECT parallel_select;


/******************************************************************************
 * Name
 *  string_is_color
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   	gint string_is_color(gchar * color)
 *
 * Description
 *    this code is from bluefish-0.6
 *
 * Return value
 *   gint
 */

static gint
string_is_color(gchar * color)
{
	gint i;

	if (!color) {
		GS_warning(("string_is_color, pointer NULL\n"));
		return 0;
	}
	if (strlen(color) != 7) {
		GS_warning(("string_is_color, strlen(%s) != 7\n", color));
		return 0;
	}
	if (color[0] != '#') {
		GS_warning(("string_is_color, 0 in %s is not #\n", color));
		return 0;
	}
	for (i = 1; i < 7; i++) {
		if ((color[i] > 102)
		    || (color[i] < 48)
		    || ((color[i] > 57) && (color[i] < 65))
		    || ((color[i] > 70) && (color[i] < 97))) {
			GS_warning
			    (("string_is_color, %d in %s is not from a color, it is %d\n",
			     i, color, color[i]));
			return 0;
		}
	}
	GS_print(("string_is_color, %s is color\n", color));
	return 1;

}


/******************************************************************************
 * Name
 *  gdkcolor_to_hex
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   gchar *gdkcolor_to_hex(gdouble * color, gint websafe)
 *
 * Description
 *    this code is from bluefish-1.0.2
 *
 * Return value
 *   gchar *
 */

static gchar *
gdkcolor_to_hex(GdkColor color,
		gint websafe)
{
	gchar *tmpstr;

	tmpstr = g_malloc(8*sizeof(char));
	if (websafe) {
		g_snprintf (tmpstr, 8,"#%.2X%.2X%.2X",
					(0x33 * color.red/(256*0x33)),
					(0x33 * color.green/(256*0x33)),
					(0x33 * color.blue/(256*0x33)) );
	} else {
		g_snprintf (tmpstr, 8,"#%.2X%.2X%.2X",
					color.red/256,
					color.green/256,
					color.blue/256);
	}
	return tmpstr;
}


static void
apply_color_settings(void)
{
	if (settings.havebible)
	      main_display_bible(settings.MainWindowModule,
      			settings.currentverse);
	if (settings.havecomm)
		main_display_commentary(settings.CommWindowModule,
			settings.currentverse);
	if (settings.havedict)
		main_display_dictionary(settings.DictWindowModule,
			settings.dictkey);
	if (settings.havebible)
		main_update_parallel_page();
}


/******************************************************************************
 * Name
 *   on_colorbutton1_color_set
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_colorbutton1_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_colorbutton1_color_set(GtkColorButton  * colorbutton,
			  gpointer user_data)
{
	gchar *buf = NULL;
 	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf = gdkcolor_to_hex(color,1);
	xml_set_value("Xiphos", "HTMLcolors", "background", buf);
	settings.bible_bg_color = xml_get_value("HTMLcolors", "background");
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
 *
 *   void on_colorbutton2_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_colorbutton2_color_set(GtkColorButton * colorbutton,
			  gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1);
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
 *
 *   void on_colorbutton3_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_colorbutton3_color_set(GtkColorButton * colorbutton,
			  gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1);
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
 *
 *   void on_colorbutton4_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_colorbutton4_color_set(GtkColorButton * colorbutton,
			  gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1);
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
 *
 *   void on_colorbutton5_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_colorbutton5_color_set(GtkColorButton * colorbutton,
			  gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1);
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
 *
 *   void on_colorbutton6_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_colorbutton6_color_set(GtkColorButton * colorbutton,
			  gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1);
	xml_set_value("Xiphos", "HTMLcolors", "highlight_fg", buf2);
	settings.highlight_fg = xml_get_value("HTMLcolors", "highlight_fg");
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
 *
 *   void on_colorbutton7_color_set(GtkColorButton  * colorbutton,
 *							gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_colorbutton7_color_set(GtkColorButton * colorbutton,
			  gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;

	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1);
	xml_set_value("Xiphos", "HTMLcolors", "highlight_bg", buf2);
	settings.highlight_bg = xml_get_value("HTMLcolors", "highlight_bg");
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
 *
 *   void on_checkbutton1_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton1_toggled(GtkToggleButton * togglebutton,
			gpointer user_data)
{
	gui_tabs_on_off (togglebutton->active);
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
 *
 *   void on_checkbutton2_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton2_toggled(GtkToggleButton * togglebutton,
			gpointer user_data)
{
	if (togglebutton->active)
		xml_set_value("Xiphos", "misc", "showtexts", "1");
	else
		xml_set_value("Xiphos", "misc", "showtexts", "0");
	settings.showtexts = atoi(xml_get_value("misc", "showtexts"));
	GTK_CHECK_MENU_ITEM(widgets.viewtexts_item)->active =
	    settings.showtexts;
	gui_show_hide_texts(togglebutton->active);
}

/******************************************************************************
 * Name
 *   on_checkbutton3_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton3_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton3_toggled(GtkToggleButton * togglebutton,
			gpointer user_data)
{
	if (togglebutton->active)
		xml_set_value("Xiphos", "misc", "showcomms", "1");
	else
		xml_set_value("Xiphos", "misc", "showcomms", "0");
	settings.showcomms = atoi(xml_get_value("misc", "showcomms"));
	GTK_CHECK_MENU_ITEM(widgets.viewcomms_item)->active =
	    settings.showcomms;
	gui_show_hide_comms(togglebutton->active);
}

/******************************************************************************
 * Name
 *   on_checkbutton9_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton9_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton9_toggled(GtkToggleButton * togglebutton,
			gpointer user_data)
{
	if (togglebutton->active)
		xml_set_value("Xiphos", "misc", "showpreview", "1");
	else
		xml_set_value("Xiphos", "misc", "showpreview", "0");
	settings.showpreview = atoi(xml_get_value("misc", "showpreview"));
	GTK_CHECK_MENU_ITEM(widgets.viewpreview_item)->active =
	    settings.showpreview;
	gui_show_hide_preview(togglebutton->active);
}

/******************************************************************************
 * Name
 *   on_checkbutton4_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton4_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton4_toggled(GtkToggleButton *togglebutton,
			gpointer user_data)
{
	if (togglebutton->active)
		xml_set_value("Xiphos", "misc", "showdicts", "1");
	else
		xml_set_value("Xiphos", "misc", "showdicts", "0");
	settings.showdicts = atoi(xml_get_value("misc", "showdicts"));
	GTK_CHECK_MENU_ITEM(widgets.viewdicts_item)->active =
	    settings.showdicts;
	gui_show_hide_dicts(togglebutton->active);
}

/******************************************************************************
 * Name
 *   on_checkbutton10_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton10_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton10_toggled(GtkToggleButton * togglebutton,
			 gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "pinnedtabs",
		      (togglebutton->active ? "1" : "0"));
	settings.linkedtabs = atoi(xml_get_value("misc", "pinnedtabs"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
			       (widgets.linkedtabs_item),
			       settings.linkedtabs);
}


/******************************************************************************
 * Name
 *   on_checkbutton_showparatab_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton_showparatab_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

static void
on_checkbutton_showparatab_toggled(GtkToggleButton * togglebutton,
			 gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "showparatab",
		      (togglebutton->active ? "1" : "0"));
	if (togglebutton->active)
		gui_open_parallel_view_in_new_tab();
	else
		gui_close_passage_tab(1);


}


/******************************************************************************
 * Name
 *   on_checkbutton11_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton11_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton11_toggled(GtkToggleButton * togglebutton,
			 gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "readaloud",
		      (togglebutton->active ? "1" : "0"));
	settings.readaloud = atoi(xml_get_value("misc", "readaloud"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
			       (widgets.readaloud_item),
			       settings.readaloud);
}


/******************************************************************************
 * Name
 *   on_checkbutton12_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton12_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton12_toggled(GtkToggleButton * togglebutton,
			 gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "showversenum",
		      (togglebutton->active ? "1" : "0"));
	settings.showversenum = atoi(xml_get_value("misc", "showversenum"));

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
			       (widgets.showversenum_item),
			       settings.showversenum);
}


/******************************************************************************
 * Name
 *   on_checkbutton6_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton6_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton6_toggled(GtkToggleButton * togglebutton,
			gpointer user_data)
{
	xml_set_value("Xiphos", "lexicons", "usedefaultdict",
		      (togglebutton->active ? "1" : "0"));
	settings.useDefaultDict = togglebutton->active;
}


/******************************************************************************
 * Name
 *   on_checkbutton7_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton7_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton7_toggled(GtkToggleButton * togglebutton,
			gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "dailydevotional",
		      (togglebutton->active ? "1" : "0"));
	settings.showdevotional = togglebutton->active;
}


/******************************************************************************
 * Name
 *   on_checkbutton8_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton8_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton8_toggled(GtkToggleButton * togglebutton,
			gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "splash",
		      (togglebutton->active ? "1" : "0"));
	settings.showsplash = togglebutton->active;
}


/******************************************************************************
 * Name
 *   on_checkbutton_scroll_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton_scroll_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton_scroll_toggled(GtkToggleButton * togglebutton,
			      gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "chapter-scroll",
		      (togglebutton->active ? "1" : "0"));
	settings.chapter_scroll = togglebutton->active;
}

/******************************************************************************
 * Name
 *   on_checkbutton_imageresize_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton_imageresize_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton_imageresize_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "imageresize",
		      (togglebutton->active ? "1" : "0"));
	settings.imageresize =  togglebutton->active;
}

/******************************************************************************
 * Name
 *   on_checkbutton_versehighlight_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton_versehighlight_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton_versehighlight_toggled(GtkToggleButton * togglebutton,
				      gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "versehighlight",
		      (togglebutton->active ? "1" : "0"));
	settings.versehighlight = togglebutton->active;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
			       (widgets.versehighlight_item),
			       settings.versehighlight);
	main_display_bible(settings.MainWindowModule, settings.currentverse);
}

/******************************************************************************
 * Name
 *   on_checkbutton_annotate_highlight_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton_annotate_highlight_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton_annotate_highlight_toggled(GtkToggleButton * togglebutton,
				      gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "annotatehighlight",
		      (togglebutton->active ? "1" : "0"));
	settings.annotate_highlight = togglebutton->active;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
			       (widgets.annotate_highlight_item),
			       settings.annotate_highlight);
	main_display_bible(settings.MainWindowModule, settings.currentverse);
}

/******************************************************************************
 * Name
 *   on_checkbutton_prayerlist_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton_prayerlist_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_checkbutton_prayerlist_toggled(GtkToggleButton * togglebutton,
				  gpointer user_data)
{
	xml_set_value("Xiphos", "misc", "prayerlist",
		      (togglebutton->active ? "1" : "0"));
	settings.prayerlist = togglebutton->active;

	/* update module list to show choice */
	main_update_module_lists();
	main_load_module_tree(sidebar.module_list);
}


/******************************************************************************
 * Name
 *   on_folder_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_folder_changed(GtkFileChooser * filechooser, gpointer user_data)
 *
 * Description
 *
 *
 *
 * Return value
 *   void
 */

void
on_folder_changed(GtkFileChooser * filechooser,
		  gpointer user_data)
{
	gchar *directory = gtk_file_chooser_get_current_folder(filechooser);
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
 *
 *   void on_combobox1_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox1 (verse number size)
 *   has changed - update Biblical text pane
 *
 * Return value
 *  void
 */

void
on_combobox1_changed(GtkComboBox * combobox,
		     gpointer user_data)
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
	settings.verse_num_font_size_str = xml_get_value("fontsize", "versenum");
	settings.verse_num_font_size = atoi(settings.verse_num_font_size_str);
	url = g_strdup_printf("sword://%s/%s",settings.MainWindowModule,
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
 *
 *   void on_basecombobox1_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox1 (base font size)
 *   has changed - update Biblical text pane
 *
 * Return value
 *  void
 */

void
on_basecombobox1_changed(GtkComboBox * combobox,
			 gpointer user_data)
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
	settings.base_font_size_str = xml_get_value("fontsize", "basefontsize");

	settings.base_font_size = atoi(settings.base_font_size_str);
	url = g_strdup_printf("sword://%s/%s",
			      settings.MainWindowModule,
			      settings.currentverse);
	main_url_handler(url, TRUE);
	g_free(url);
	main_display_dictionary(settings.DictWindowModule, settings.dictkey);
}


/******************************************************************************
 * Name
 *   on_combobox2_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_combobox2_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox2 (Biblical text module) or combobox3 (Commentary module),
 *   has changed - update Biblical text and Commentary panes
 *
 * Return value
 *  void
 */

void
on_combobox2_changed(GtkComboBox * combobox,
		     gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
		return;

	url = g_strdup_printf("sword://%s/%s",buf,settings.currentverse);
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
 *
 *   void on_combobox4_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox4 (Dictionary module),
 *   has changed - update dictionary pane
 *
 * Return value
 *  void
 */

void
on_combobox4_changed(GtkComboBox * combobox,
		     gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
		return;

	url = g_strdup_printf("sword://%s/%s",buf,settings.dictkey);
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
 *
 *   void on_combobox5_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox5 (default dictionary module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void
on_combobox5_changed(GtkComboBox * combobox,
		     gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
		return;
	xml_set_value("Xiphos", "lexicons", "defaultdictionary", buf);
	settings.DefaultDict = xml_get_value("lexicons", "defaultdictionary");
	g_free(buf);
}


/******************************************************************************
 * Name
 *   on_combobox6_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_combobox6_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox6 (Personal comments module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void
on_combobox6_changed(GtkComboBox * combobox,
		     gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
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
 *
 *   void on_combobox12_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox12 (Daily Devotional module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void
on_combobox12_changed(GtkComboBox * combobox,
		      gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
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
 *
 *   void on_combobox13_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox13 (Hebrew lex module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void
on_combobox13_changed(GtkComboBox * combobox,
		      gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
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
 *
 *   void on_combobox14_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox14 (Greek lex module),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void
on_combobox14_changed(GtkComboBox * combobox,
		      gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
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
 *
 *   void on_combobox15_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox4 (Book module),
 *   has changed - update book pane
 *
 * Return value
 *  void
 */

void
on_combobox15_changed(GtkComboBox * combobox,
		      gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
		return;
	url = g_strdup_printf("sword://%s/%s",buf,"1");
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
 *
 *   void on_combobox16_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox16 (special locale),
 *   has changed - update settings
 *
 * Return value
 *  void
 */

void
on_combobox16_changed(GtkComboBox * combobox,
		      gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	gboolean clear, set;

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
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

	xml_set_value("Xiphos", "locale", "special", (clear ? NONE : buf));
	g_free(settings.special_locale);	/* dispose of old content */
	settings.special_locale = (clear ? g_strdup(NONE) : g_strdup(buf));
}


/******************************************************************************
 * Name
 *   on_combobox17_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_combobox17_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   combobox17 (language font preferences) => make a font selection.
 *
 * Return value
 *  void
 */

void
on_combobox17_changed(GtkComboBox * combobox,
		      gpointer user_data)
{
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	gchar *mod_name;

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf || !strcmp(buf, _("-- Select --")))	/* see fill_combobox */
		return;

	buf = strrchr(buf, '(') + 1;
	*strchr(buf, ')') = '\0';

	mod_name = g_strdup_printf("Language:%s", buf);
	gui_set_module_font(mod_name);
	g_free(mod_name);
}


/******************************************************************************
 * Name
 *   add_columns
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void add_columns(GtkWidget * listview)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
add_columns(GtkWidget * treeview)
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
 *
 *   void tree_selection_changed(GtkTreeSelection * selection,
 *		      GtkWidget * tree_widget)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
tree_selection_changed(GtkTreeSelection * selection,
		       gpointer data)
{
	GtkTreeIter selected;
	gint page;
	GtkTreeModel *model;

	if (!gtk_tree_selection_get_selected
	    (selection, &model, &selected))
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
 *
 *   void on_dialog_prefs_response(GtkDialog * dialog, gint response_id,
					gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void
on_dialog_prefs_response(GtkDialog * dialog,
			 gint response_id,
			 gpointer user_data)
{
	if (response_id == GTK_RESPONSE_CLOSE) {
		xml_save_settings_doc(settings.fnconfigure);
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
	main_update_parallel_page();
}


static GtkTreeModel *
create_model(void)
{
	GtkTreeStore *model;
	GtkTreeIter iter;
	GtkTreeIter child_iter;

	model = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);

	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("Fonts"), -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Color"), 1, 1, -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Sizes and Faces"), 1, 2, -1);


	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("General"), -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Tabs and Panes"), 1, 3, -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Options"), 1, 4, -1);


	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("Modules"), -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Main"), 1, 5, -1);

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
 *
 *   void setup_color_pickers(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
setup_color_pickers(void)
{
	GdkColor color;
	if (string_is_color(settings.bible_bg_color)) {
		gdk_color_parse(settings.bible_bg_color, &color);
		gtk_color_button_set_color(
				GTK_COLOR_BUTTON(color_picker.text_background),
                                &color);
	} else {
		gdk_color_parse("#FFFFFF", &color);
		gtk_color_button_set_color(
				GTK_COLOR_BUTTON(color_picker.text_background),
                                &color);
	}

	if (string_is_color(settings.bible_text_color)) {
		gdk_color_parse(settings.bible_text_color, &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.text),
                                        &color);
	} else {
		gdk_color_parse("#000000", &color);
		gtk_color_button_set_color(GTK_COLOR_BUTTON(color_picker.text),
                                        &color);
	}

	if (string_is_color(settings.currentverse_color)) {
		gdk_color_parse(settings.currentverse_color, &color);
		gtk_color_button_set_color(
			GTK_COLOR_BUTTON(color_picker.text_current_verse),
                                        &color);
	} else {
		gdk_color_parse("#339766", &color);
		gtk_color_button_set_color(
			GTK_COLOR_BUTTON(color_picker.text_current_verse),
                                        &color);
	}

	if (string_is_color(settings.bible_verse_num_color)) {
		gdk_color_parse(settings.bible_verse_num_color, &color);
		gtk_color_button_set_color(
			GTK_COLOR_BUTTON(color_picker.verse_numbers),
                                        &color);
	} else {
		gdk_color_parse("#0000CF", &color);
		gtk_color_button_set_color(
			GTK_COLOR_BUTTON(color_picker.verse_numbers),
                                        &color);
	}

	if (string_is_color(settings.link_color)) {
		gdk_color_parse(settings.link_color, &color);
		gtk_color_button_set_color(
			GTK_COLOR_BUTTON(color_picker.href_links),
                                        &color);
	} else {
		gdk_color_parse("#878787", &color);
		gtk_color_button_set_color(
			GTK_COLOR_BUTTON(color_picker.href_links),
                                        &color);
	}

	// contrasty highlighting -- foreground.
	if (string_is_color(settings.highlight_fg))
		gdk_color_parse(settings.highlight_fg, &color);
	else
		gdk_color_parse("#FFFF00", &color);
	gtk_color_button_set_color(
		GTK_COLOR_BUTTON(color_picker.highlight_fg),
				&color);

	// contrasty highlighting -- background.
	if (string_is_color(settings.highlight_bg))
		gdk_color_parse(settings.highlight_bg, &color);
	else
		gdk_color_parse("#060680", &color);
	gtk_color_button_set_color(
		GTK_COLOR_BUTTON(color_picker.highlight_bg),
				&color);
}


static void
setup_check_buttons(void)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.enable_tabbed_browsing),
				     settings.browsing);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_bible_pane),
				     settings.showtexts);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_preview_pane),
				     settings.showpreview);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_commentary_pane),
				     settings.showcomms);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_dictionary_pane),
				     settings.showdicts);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_default_dictionary),
				     settings.useDefaultDict);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_devotion),
				     settings.showdevotional);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_splash_screen),
				     settings.showsplash);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_chapter_scroll),
				     settings.chapter_scroll);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_imageresize),
				     settings.imageresize);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.versehighlight),
				     settings.versehighlight);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.annotate_highlight),
				     settings.annotate_highlight);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.prayerlist),
				     settings.prayerlist);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_paratab),
				     settings.showparatab);

	g_signal_connect(check_button.enable_tabbed_browsing, "toggled",
			 G_CALLBACK(on_checkbutton1_toggled), NULL);
	g_signal_connect(check_button.show_bible_pane, "toggled",
			 G_CALLBACK(on_checkbutton2_toggled), NULL);
	g_signal_connect(check_button.show_preview_pane, "toggled",
			 G_CALLBACK(on_checkbutton9_toggled), NULL);
	g_signal_connect(check_button.show_commentary_pane, "toggled",
			 G_CALLBACK(on_checkbutton3_toggled), NULL);
	g_signal_connect(check_button.show_dictionary_pane, "toggled",
			 G_CALLBACK(on_checkbutton4_toggled), NULL);
	g_signal_connect(check_button.use_default_dictionary, "toggled",
			 G_CALLBACK(on_checkbutton6_toggled), NULL);
	g_signal_connect(check_button.show_devotion, "toggled",
			 G_CALLBACK(on_checkbutton7_toggled), NULL);
	g_signal_connect(check_button.show_splash_screen, "toggled",
			 G_CALLBACK(on_checkbutton8_toggled), NULL);
	g_signal_connect(check_button.use_chapter_scroll, "toggled",
			 G_CALLBACK(on_checkbutton_scroll_toggled), NULL);
	g_signal_connect(check_button.use_imageresize, "toggled",
			 G_CALLBACK(on_checkbutton_imageresize_toggled), NULL);
	g_signal_connect(check_button.versehighlight, "toggled",
			 G_CALLBACK(on_checkbutton_versehighlight_toggled), NULL);
	g_signal_connect(check_button.annotate_highlight, "toggled",
			 G_CALLBACK(on_checkbutton_annotate_highlight_toggled), NULL);
	g_signal_connect(check_button.prayerlist, "toggled",
			 G_CALLBACK(on_checkbutton_prayerlist_toggled), NULL);
	g_signal_connect(check_button.show_paratab, "toggled",
			 G_CALLBACK(on_checkbutton_showparatab_toggled), NULL);
}

/*
 * comparator function to determine filling of true general dict lists.
 */
static gboolean
dict_no_image_map_dd(char *modname, char *feature)
{
	/* no features or categories */
	const char *f = main_get_mod_config_entry(modname, "Feature");
	const char *c = main_get_mod_config_entry(modname, "Category");
	return ((c == NULL) && (f == NULL));
}

/*
 * comparator function to determine filling of heb/grk dict lists.
 */
static gboolean
dict_match_feature(char *modname, char *feature)
{
	const char *f = main_get_mod_config_entry(modname, "Feature");
	return (f && (!strcmp(f, feature)));
}

static void
fill_combobox(GList * glist,
	      GtkComboBox * combo,
	      gchar * current_item,
	      gboolean (*eval)(char *, char *),	/* evaluator of feature */
	      char * feature)			/* feature sought or avoided */
{
	GtkTreeIter iter;
	GtkListStore *store;
	GtkCellRenderer *renderer;
	gint index = 0;
	gint i = 0;

	store = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_combo_box_set_model(combo,
				GTK_TREE_MODEL(store));

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (combo),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (combo),
				       renderer, "text", 0, NULL);

	gtk_list_store_clear(GTK_LIST_STORE(store));

	gtk_list_store_append(GTK_LIST_STORE(store), &iter);
	gtk_list_store_set(GTK_LIST_STORE(store),
			   &iter,
			   0,
			   _("-- Select --"),
			   -1);

	while (glist) {
		if ((eval == NULL) || (*eval)(glist->data, feature)) {
			/* "anything goes" or "acceptable match" */
			gtk_list_store_append(GTK_LIST_STORE(store), &iter);
			gtk_list_store_set(GTK_LIST_STORE(store),
					   &iter,
					   0,
					   (gchar*)glist->data,
					   -1);
			if (current_item && !strcmp((gchar*)glist->data,
						    current_item))
				index = i+1;
			++i;
		}
		glist = g_list_next(glist);
	}
	gtk_combo_box_set_active(combo, index);
}


static gint
get_font_size_index(const char *font)
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


static void
setup_module_comboboxes(void)
{
	/*
	 * Main Window Modules page
	 */
	fill_combobox(get_list(TEXT_LIST), GTK_COMBO_BOX(combo.text_module),
		      settings.MainWindowModule,
		      NULL, NULL);
	fill_combobox(get_list(COMM_LIST), GTK_COMBO_BOX(combo.commentary_module),
		      settings.CommWindowModule,
		      NULL, NULL);
	fill_combobox(get_list(DICT_LIST), GTK_COMBO_BOX(combo.dictionary_module),
		      settings.DictWindowModule,
		      dict_no_image_map_dd, NULL);
	fill_combobox(get_list(DICT_LIST), GTK_COMBO_BOX(combo.default_dictionary_module),
		      settings.DefaultDict,
		      dict_no_image_map_dd, NULL);
	fill_combobox(get_list(PERCOMM_LIST), GTK_COMBO_BOX(combo.percomm_module),
		      settings.personalcommentsmod,
		      NULL, NULL);
	fill_combobox(get_list(GBS_LIST), GTK_COMBO_BOX(combo.book_module),
		      settings.book_mod,
		      NULL, NULL);

	/*
	 * Miscellaneous Modules page
	 */
	fill_combobox(get_list(DEVOTION_LIST), GTK_COMBO_BOX(combo.devotion_module),
		      settings.devotionalmod,
		      NULL, NULL);
	fill_combobox(get_list(DICT_LIST), GTK_COMBO_BOX(combo.hebrew_lex__module),
		      settings.lex_hebrew,
		      dict_match_feature, "HebrewDef");
	fill_combobox(get_list(DICT_LIST), GTK_COMBO_BOX(combo.greek_lex__module),
		      settings.lex_greek,
		      dict_match_feature, "GreekDef");

	/*
	 * signal connectivity
	 */
	g_signal_connect(combo.text_module, "changed",
			 G_CALLBACK(on_combobox2_changed), NULL);
	g_signal_connect(combo.commentary_module, "changed",
			 G_CALLBACK(on_combobox2_changed), NULL);
	g_signal_connect(combo.dictionary_module, "changed",
			 G_CALLBACK(on_combobox4_changed), NULL);
	g_signal_connect(combo.default_dictionary_module, "changed",
			 G_CALLBACK(on_combobox5_changed), NULL);
	g_signal_connect(combo.percomm_module, "changed",
			 G_CALLBACK(on_combobox6_changed), NULL);
	g_signal_connect(combo.book_module, "changed",
			 G_CALLBACK(on_combobox15_changed), NULL);
	g_signal_connect(combo.devotion_module, "changed",
			 G_CALLBACK(on_combobox12_changed), NULL);
	g_signal_connect(combo.hebrew_lex__module, "changed",
			 G_CALLBACK(on_combobox13_changed), NULL);
	g_signal_connect(combo.greek_lex__module, "changed",
			 G_CALLBACK(on_combobox14_changed), NULL);
}


void
setup_locale_combobox(void)
{
	char **locale, brief_locale[3], *real_locale, *current_locale = NULL;
	GList *chase, *list = g_list_append(NULL, g_strdup(NONE));

	brief_locale[2] = '\0';
	for (locale = &locale_set[0]; *locale; ++locale) {
		brief_locale[0] = (*locale)[0];
		brief_locale[1] = (*locale)[1];
		real_locale = g_strdup_printf("%s (%s)",
					      main_get_language_map(brief_locale),
					      *locale);
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


void
setup_font_prefs_combobox(void)
{
	char **language_list = main_get_module_language_list();
	char *real_language, **language;
	GList *chase, *list = NULL;

	for (language = &language_list[0]; *language; ++language) {
		real_language = g_strdup_printf("%s (%s)",
						main_get_language_map(*language),
						*language);
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
 *
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

static gboolean button_release_event(GtkWidget * widget,
				     GdkEventButton * event,
				     gpointer data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeIter selected;
	GtkTreeModel *model;
	GtkTreePath *path;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	if ((!gtk_tree_selection_get_selected(selection, &model, &selected)) ||
	    (!gtk_tree_model_iter_has_child(model, &selected)))
		return FALSE;

	path = gtk_tree_model_get_path(model, &selected);
	if (gtk_tree_view_row_expanded(GTK_TREE_VIEW(widget), path))
	       gtk_tree_view_collapse_row(GTK_TREE_VIEW(widget), path);
        else
	       gtk_tree_view_expand_row(GTK_TREE_VIEW(widget), path, FALSE);
	gtk_tree_path_free(path);
	return FALSE;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   static void _setup_listview
 *
 * Description
 *   init the listing of currently-set parallel modules.
 *
 * Return value
 *
 */

static
void ps_setup_listview()
{
	GtkListStore *model;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeIter iter;
	GtkTreeModel *model_t;
	GtkListStore *list_store;
	int i;

	model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(parallel_select.listview), GTK_TREE_MODEL(model));

	for (i = 0; i < 2; ++i) {
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("Module",
								  renderer,
								  "text", i,
								  NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(parallel_select.listview), column);
	}
	gtk_tree_view_column_set_sort_column_id(column, 0);

	model_t = gtk_tree_view_get_model(GTK_TREE_VIEW(parallel_select.listview));
	list_store = GTK_LIST_STORE(model_t);
	gtk_list_store_clear(list_store);

	if (settings.parallel_list) {
		for (i = 0; settings.parallel_list[i]; ++i) {
			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter,
					   0, main_get_module_description
						(settings.parallel_list[i]),
					   1, (gchar *)settings.parallel_list[i],
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
 *
 *   void modules_lists_changed(GtkTreeSelection * selection,
 *		     					 gpointer data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static gchar *module_selected = NULL;

static void modules_lists_changed(GtkTreeSelection * selection,
				  GtkTreeView * tree_widget)
{
	gchar *mod = NULL;
	GtkTreeIter selected;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_widget));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	if (gtk_tree_model_iter_has_child(model, &selected)) {
		g_free(module_selected);
		module_selected = NULL;
		return;
	}

	gtk_tree_model_get(model, &selected, 0, &mod, -1);
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
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

static
void ps_setup_treeview(GtkWidget * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GObject *selection;

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Found",
							  renderer,
							  "text",
							  0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gui_load_module_tree(treeview, TRUE);

	selection = G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
	g_signal_connect_after(G_OBJECT(treeview), "button_release_event",
			       G_CALLBACK(button_release_event), GINT_TO_POINTER(0));
	g_signal_connect(selection, "changed", G_CALLBACK(modules_lists_changed), treeview);
}


static void on_mod_sel_add_clicked(GtkWidget * button, gchar * user_data)
{
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	char *parallels = g_strdup(""), *newhold;
	int count;

	if (!module_selected)
		return;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(parallel_select.listview));
	list_store = GTK_LIST_STORE(model);

	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, main_get_module_description(module_selected),
			   1, module_selected,
			   -1);

	if (settings.parallel_list == NULL) {
		settings.parallel_list = g_strsplit(module_selected, ",", -1);
	} else {
		for (count = 0; settings.parallel_list[count]; ++count)
			; /* count up the set */
		settings.parallel_list = g_renew(gchar *, settings.parallel_list, count + 2);
		settings.parallel_list[count] = g_strdup(module_selected);	/* new element */
		settings.parallel_list[count+1] = NULL;
	}

	for (count = 0; settings.parallel_list[count]; ++count) {
		newhold = g_strconcat(parallels, settings.parallel_list[count], ",", NULL);
		g_free(parallels);
		parallels = newhold;
	}

	if (*parallels) {
		*(parallels + strlen(parallels) - 1) = '\0';	/* end comma */
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
 *	ps_button_clear(GtkButton * button, gpointer user_data)
 *
 * Description
 *   clear all modules from the list. (parallel select)
 *
 * Return value
 *   void
 */
void ps_button_clear(GtkButton * button, gpointer user_data)
{
{
	GtkTreeModel *model;
	GtkListStore *list_store;
	gchar *str;

	str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
			      _("Clear List?"),
			      _("Are you sure you want to clear the module list?"));

	if (gui_yes_no_dialog(str, GTK_STOCK_DIALOG_WARNING)) {
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(parallel_select.listview));
		list_store = GTK_LIST_STORE(model);
		gtk_list_store_clear(list_store);
		if (settings.parallel_list)
			g_strfreev(settings.parallel_list);
		settings.parallel_list = NULL;
		xml_set_value("Xiphos", "modules", "parallels", "");
	}
	g_free(str);
}}

/******************************************************************************
 * Name
 *   ps_button_cut
 *
 * Synopsis
 *	ps_button_cut(GtkButton * button, gpointer user_data)
 *
 * Description
 *   cut one module from the list. (parallel select)
 *
 * Return value
 *   void
 */
void ps_button_cut(GtkButton * button, gpointer user_data)
{
	GList *mods = NULL;
	gchar *mod_list;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *str;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(parallel_select.listview));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(parallel_select.listview));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
			      _("Remove Module?"),
			      _("Are you sure you want to remove the selected module?"));

	if (gui_yes_no_dialog(str, (char *)GTK_STOCK_DIALOG_WARNING)) {
		gtk_list_store_remove(list_store, &selected);
		mods = get_current_list(GTK_TREE_VIEW(parallel_select.listview));
		mod_list = get_modlist_string(mods);
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
 *	ps_button_add(GtkButton * button, gpointer user_data)
 *
 * Description
 *   add one module from the list. (parallel select)
 *
 * Return value
 *   void
 */
void ps_button_add(GtkButton * button, gpointer user_data)
{
	gchar *glade_file;
	GladeXML *gxml;

	glade_file = gui_general_user_file("prefs.glade", FALSE);
	g_return_if_fail(glade_file != NULL);

	gxml = glade_xml_new(glade_file, "mod_sel_dialog", NULL);
	parallel_select.mod_sel_dialog = glade_xml_get_widget(gxml, "mod_sel_dialog");
	parallel_select.mod_sel_close  = glade_xml_get_widget(gxml, "mod_sel_button_close");
	parallel_select.mod_sel_add    = glade_xml_get_widget(gxml, "mod_sel_button_add");

	g_signal_connect((gpointer)parallel_select.mod_sel_close, "clicked",
			 G_CALLBACK(on_mod_sel_close_clicked), NULL);
	g_signal_connect((gpointer)parallel_select.mod_sel_add, "clicked",
			 G_CALLBACK(on_mod_sel_add_clicked), NULL);

	parallel_select.mod_sel_treeview = glade_xml_get_widget(gxml, "mod_sel_treeview");
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
 *
 *   	GtkWidget *gui_create_preferences_dialog(GList * biblelist,
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

static void
create_preferences_dialog(void)
{
	GladeXML *gxml;
	gchar *glade_file;
	GtkWidget *dialog_prefs;
	GtkWidget *treeview;
	GtkTreeModel *model;
	GObject *selection;
	GtkWidget *chooser;
	gint index = 0;

	glade_file = gui_general_user_file ("prefs.glade", TRUE);
	g_return_if_fail(glade_file != NULL);

	/* build the widget */
	gxml = glade_xml_new (glade_file, NULL, NULL);
	g_free (glade_file);
	g_return_if_fail (gxml != NULL);

	/* lookup the root widget */
	dialog_prefs = glade_xml_get_widget (gxml, "dialog_prefs");
	g_signal_connect(dialog_prefs, "response",
			 G_CALLBACK(on_dialog_prefs_response), NULL);
/*	g_signal_connect(, "",
			 G_CALLBACK(), NULL);
*/
	/* color pickers */
	color_picker.text_background = glade_xml_get_widget (gxml, "colorbutton1");
	g_signal_connect(color_picker.text_background, "color_set",
			 G_CALLBACK(on_colorbutton1_color_set), NULL);
	color_picker.text = glade_xml_get_widget (gxml, "colorbutton2");
	g_signal_connect(color_picker.text, "color_set",
			 G_CALLBACK(on_colorbutton2_color_set), NULL);
	color_picker.text_current_verse = glade_xml_get_widget (gxml, "colorbutton3");
	g_signal_connect(color_picker.text_current_verse, "color_set",
			 G_CALLBACK(on_colorbutton3_color_set), NULL);
	color_picker.verse_numbers = glade_xml_get_widget (gxml, "colorbutton4");
	g_signal_connect(color_picker.verse_numbers, "color_set",
			 G_CALLBACK(on_colorbutton4_color_set), NULL);
	color_picker.href_links = glade_xml_get_widget (gxml, "colorbutton5");
	g_signal_connect(color_picker.href_links, "color_set",
			 G_CALLBACK(on_colorbutton5_color_set), NULL);
	color_picker.highlight_fg = glade_xml_get_widget (gxml, "colorbutton6");
	g_signal_connect(color_picker.highlight_fg, "color_set",
			 G_CALLBACK(on_colorbutton6_color_set), NULL);
	color_picker.highlight_bg = glade_xml_get_widget (gxml, "colorbutton7");
	g_signal_connect(color_picker.highlight_bg, "color_set",
			 G_CALLBACK(on_colorbutton7_color_set), NULL);

	setup_color_pickers();
	/* check buttons */
	check_button.enable_tabbed_browsing = glade_xml_get_widget(gxml, "checkbutton1");
	check_button.show_bible_pane = glade_xml_get_widget(gxml,"checkbutton2");
	check_button.show_preview_pane = glade_xml_get_widget(gxml, "checkbutton9");
	check_button.show_commentary_pane = glade_xml_get_widget(gxml, "checkbutton3");
	check_button.show_dictionary_pane = glade_xml_get_widget(gxml, "checkbutton4");
	check_button.use_linked_tabs = glade_xml_get_widget(gxml, "checkbutton10");
	check_button.readaloud = glade_xml_get_widget(gxml, "checkbutton11");
	check_button.show_verse_num = glade_xml_get_widget(gxml, "checkbutton12");
	check_button.use_default_dictionary = glade_xml_get_widget(gxml, "checkbutton6");
	check_button.show_devotion = glade_xml_get_widget(gxml, "checkbutton7");
	check_button.show_splash_screen = glade_xml_get_widget(gxml, "checkbutton8");
	check_button.use_chapter_scroll = glade_xml_get_widget(gxml, "checkbutton_scroll");
#ifdef USE_GTKMOZEMBED
	gtk_widget_hide(check_button.use_chapter_scroll);
#endif
	check_button.use_imageresize = glade_xml_get_widget(gxml, "checkbutton_imageresize");
	check_button.versehighlight = glade_xml_get_widget(gxml, "checkbutton_versehighlight");
	check_button.annotate_highlight = glade_xml_get_widget(gxml, "checkbutton_annotate_highlight");
	check_button.prayerlist = glade_xml_get_widget(gxml, "checkbutton_prayerlist");

	check_button.show_paratab = glade_xml_get_widget(gxml, "checkbutton_paratab");

	gtk_widget_hide(check_button.show_paratab);

	setup_check_buttons();

	/* verse number size */
	index = get_font_size_index(settings.verse_num_font_size_str);
	combo.verse_number_size = glade_xml_get_widget(gxml, "combobox1");
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo.verse_number_size), index);
	g_signal_connect(combo.verse_number_size, "changed",
			 G_CALLBACK(on_combobox1_changed), NULL);

	/* base font size */
	index = get_font_size_index(settings.base_font_size_str);
	combo.base_font_size = glade_xml_get_widget(gxml, "basecombobox1");
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo.base_font_size), index);
	g_signal_connect(combo.base_font_size, "changed",
			 G_CALLBACK(on_basecombobox1_changed), NULL);

	/* module combos */
	combo.text_module = glade_xml_get_widget (gxml, "combobox2");
	combo.commentary_module = glade_xml_get_widget (gxml, "combobox3");
	combo.dictionary_module = glade_xml_get_widget (gxml, "combobox4");
	combo.default_dictionary_module = glade_xml_get_widget (gxml, "combobox5");
	combo.percomm_module = glade_xml_get_widget (gxml, "combobox6");
	combo.devotion_module = glade_xml_get_widget (gxml, "combobox12");
	combo.hebrew_lex__module = glade_xml_get_widget (gxml, "combobox13");
	combo.greek_lex__module = glade_xml_get_widget (gxml, "combobox14");
	combo.book_module = glade_xml_get_widget (gxml, "combobox15");
	setup_module_comboboxes();

	combo.special_locale = glade_xml_get_widget (gxml, "combobox16");
	setup_locale_combobox();
	combo.font_prefs = glade_xml_get_widget (gxml, "combobox17");
	setup_font_prefs_combobox();

	/* studypad directory chooserbutton */
	chooser = glade_xml_get_widget (gxml, "filechooserbutton1");
	gtk_file_chooser_set_current_folder((GtkFileChooser *)chooser,
                                             settings.studypaddir);
	g_signal_connect(chooser, "current_folder_changed",
			 G_CALLBACK(on_folder_changed), NULL);


	/* prefs notebook */
	notebook = glade_xml_get_widget (gxml, "notebook");
	/* setup treeview */
	model = create_model();
	treeview = glade_xml_get_widget (gxml, "treeview");
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
	gtk_widget_set_size_request(treeview, 130, -1);
	add_columns(treeview);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));
	selection = G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
	/* connect signals and data */
	glade_xml_signal_autoconnect_full
		(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func, NULL);

	g_signal_connect(selection, "changed",
			 G_CALLBACK(tree_selection_changed), model);

	/*
	 * parallel select dialog: chooser and button connectivity
	 */
	parallel_select.button_clear = glade_xml_get_widget(gxml, "ps_toolbutton_clear");
	parallel_select.button_cut   = glade_xml_get_widget(gxml, "ps_toolbutton_cut");
	parallel_select.button_add   = glade_xml_get_widget(gxml, "ps_toolbutton_add");
	parallel_select.listview     = glade_xml_get_widget(gxml, "ps_listview");

	g_signal_connect(parallel_select.button_clear, "clicked",
			 G_CALLBACK(ps_button_clear), NULL);
	g_signal_connect(parallel_select.button_cut, "clicked",
			 G_CALLBACK(ps_button_cut), NULL);
	g_signal_connect(parallel_select.button_add, "clicked",
			 G_CALLBACK(ps_button_add), NULL);

	ps_setup_listview();
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

void
gui_setup_preferences_dialog(void)
{
	/* create preferences dialog */
	create_preferences_dialog();
}
