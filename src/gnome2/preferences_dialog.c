/*
 * GnomeSword Bible Study Tool
 * preferences_dialog.c - get user preferences
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
#include <gtk/gtk.h>
#include <glade/glade-xml.h>

#include "gui/bibletext.h"
#include "gui/gnomesword.h"
#include "gui/preferences_dialog.h"
#include "gui/utilities.h"
#include "gui/gbs.h"
#include "gui/commentary.h"
#include "gui/dictlex.h"
#include "gui/parallel_view.h"
#include "gui/main_window.h"
#include "gui/widgets.h"
#include "gui/preferences_cell_renderer.h"

#include "main/sword.h"
#include "main/lists.h"
#include "main/parallel_view.h"
#include "main/settings.h"
#include "main/xml.h"
//gtk_toggle_button_set_active
typedef enum {
	SHOW_BIBLE_TABS,
	SHOW_COMMENTARY_TABS,
	SHOW_DICTIONARY_TABS,
	SHOW_BOOK_TABS,
	USE_DEFAULT_DICTIONARY,
	USE_VERSE_STYLE,
	USE_PINNED_TABS,
	READ_ALOUD,
	SHOW_VERSE_NUM,
	VERSE_HIGHLIGHT,
	DOUBLE_SPACE,
	SHOW_SPLASH_SCREEN,
	SHOW_BIBLE_PANE,
	SHOW_COMMENTARY_PANE,
	SHOW_DICTIONARY_PANE,
	SHOW_IN_VIEWER,
	SHOW_IN_DICTIONARY,
	SHOW_DEVOTION,
	USE_STUDYPAD,
	USE_STUDYPAD_DIALOG,
	USE_PERCOMM_DIALOG,
	TABBED_BROWSING
} which_check_button;

typedef enum {
	TEXT_FORGROUND,
	TEXT_BACKGROUND,
	TEXT_CURRENT_VERSE,
	VERSE_NUMBERS,
	HREF_LINKS,
	HIGHLIGHT_FG,
	HIGHLIGHT_BG
} which_color_combo;


typedef enum {
	TEXT_MODULE,
	PARALLEL_1_MODULE,
	PARALLEL_2_MODULE,
	PARALLEL_3_MODULE,
	PARALLEL_4_MODULE,
	PARALLEL_5_MODULE,
	COMMENTARY_MODULE,
	DICTIONARY_MODULE,
	DEFAULT_DICTIONARY_MODULE,
	PERCOMM_MODULE,
	DEVOTION_MODULE,
	GREEK_LEX__MODULE,
	HEBREW_LEX__MODULE,
	GREEK_LEX_VIEWER_MODULE,
	HEBREW_LEX_VIEWER_MODULE,
	COMBO_ENTRY_SP_DIR,
	BASE_FONT_SIZE,
	VERSE_NUMBER_SIZE
} which_entry;

typedef struct _preferences_combo COMBOBOXS;
struct _preferences_combo {
	GtkWidget *text_module;	/*  Main Window Module  */
	GtkWidget *parallel_1_module;
	GtkWidget *parallel_2_module;
	GtkWidget *parallel_3_module;
	GtkWidget *parallel_4_module;
	GtkWidget *parallel_5_module;
	GtkWidget *commentary_module;
	GtkWidget *dictionary_module;
	GtkWidget *default_dictionary_module;
	GtkWidget *book_module;
	GtkWidget *percomm_module;
	GtkWidget *devotion_module;
	GtkWidget *greek_lex__module;
	GtkWidget *hebrew_lex__module;
	GtkWidget *greek_lex_viewer_module;
	GtkWidget *hebrew_lex_viewer_module;
	GtkWidget *combo_entry_sp_dir;
	GtkWidget *base_font_size;
	GtkWidget *verse_number_size;
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
	GtkWidget *use_verse_style;
	GtkWidget *use_pinned_tabs;
	GtkWidget *use_chapter_scroll;
	GtkWidget *use_imageresize;
	GtkWidget *readaloud;
	GtkWidget *show_verse_num;
	GtkWidget *versehighlight;
	GtkWidget *doublespace;
	GtkWidget *show_splash_screen;

	GtkWidget *show_bible_pane;
	GtkWidget *show_preview_pane;
	GtkWidget *show_commentary_pane;
	GtkWidget *show_dictionary_pane;

	GtkWidget *show_in_viewer;
	GtkWidget *show_in_dictionary;
	GtkWidget *show_devotion;
};




typedef struct _preferences_buttons BUTTONS;
struct _preferences_buttons {
	GtkWidget *gnomesword_defaults;
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
static BUTTONS button;


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

static gint string_is_color(gchar * color)
{
	gint i;

	if (!color) {
		g_warning("string_is_color, pointer NULL\n");
		return 0;
	}
	if (strlen(color) != 7) {
		g_warning("string_is_color, strlen(%s) != 7\n", color);
		return 0;
	}
	if (color[0] != '#') {
		g_warning("string_is_color, 0 in %s is not #\n", color);
		return 0;
	}
	for (i = 1; i < 7; i++) {
		if ((color[i] > 102)
		    || (color[i] < 48)
		    || ((color[i] > 57) && (color[i] < 65))
		    || ((color[i] > 70) && (color[i] < 97))) {
			g_warning
			    ("string_is_color, %d in %s is not from a color, it is %d\n",
			     i, color, color[i]);
			return 0;
		}
	}
	/*g_print("string_is_color, %s is color\n", color); */
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

static gchar *gdkcolor_to_hex(GdkColor color, gint websafe)
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
#ifdef DEBUG
	g_message(tmpstr);
#endif
	return tmpstr;
}


static void apply_color_settings(void)
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

void on_colorbutton1_color_set(GtkColorButton  * colorbutton, 
							gpointer user_data)
{
	gchar *buf = NULL;
 	GdkColor color;
	
	gtk_color_button_get_color(colorbutton, &color);
	buf = gdkcolor_to_hex(color,1); 
	xml_set_value("GnomeSword", "HTMLcolors", "background", buf);
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

void on_colorbutton2_color_set(GtkColorButton * colorbutton,
                                        		gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;
	
	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1); 
	xml_set_value("GnomeSword", "HTMLcolors", "text_fg", buf2);
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

void on_colorbutton3_color_set(GtkColorButton * colorbutton,
                                       			gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;
	
	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1); 
	xml_set_value("GnomeSword", "HTMLcolors", "currentverse", buf2);
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

void on_colorbutton4_color_set(GtkColorButton * colorbutton,
                                    		    gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;
	
	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1); 
	xml_set_value("GnomeSword", "HTMLcolors", "versenum", buf2);
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

void on_colorbutton5_color_set(GtkColorButton * colorbutton,
                                       			 gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;
	
	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1); 
	xml_set_value("GnomeSword", "HTMLcolors", "link", buf2);
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

void on_colorbutton6_color_set(GtkColorButton * colorbutton,
                                       			 gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;
	
	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1); 
	xml_set_value("GnomeSword", "HTMLcolors", "highlight_fg", buf2);
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

void on_colorbutton7_color_set(GtkColorButton * colorbutton,
                                       			 gpointer user_data)
{
	gchar *buf2 = NULL;
 	GdkColor color;
	
	gtk_color_button_get_color(colorbutton, &color);
	buf2 = gdkcolor_to_hex(color,1); 
	xml_set_value("GnomeSword", "HTMLcolors", "highlight_bg", buf2);
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
 
void on_checkbutton1_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	if (togglebutton->active) {
		xml_set_value("GnomeSword", "tabs", "browsing", "1");
		settings.browsing = TRUE;
		gui_open_tabs();
		gtk_widget_show(widgets.hboxtb);
	} else {
		xml_set_value("GnomeSword", "tabs", "browsing", "0");
		gtk_widget_hide(widgets.hboxtb);
		settings.browsing = FALSE;
		gui_close_all_tabs();
	}
	settings.browsing = atoi(xml_get_value("tabs", "browsing"));
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

void on_checkbutton2_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	if (togglebutton->active)
		xml_set_value("GnomeSword", "misc", "showtexts", "1");
	else
		xml_set_value("GnomeSword", "misc", "showtexts", "0");
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

void on_checkbutton3_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	if (togglebutton->active)
		xml_set_value("GnomeSword", "misc", "showcomms", "1");
	else
		xml_set_value("GnomeSword", "misc", "showcomms", "0");
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

void on_checkbutton9_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{	
	if (togglebutton->active)
		xml_set_value("GnomeSword", "misc", "showpreview", "1");
	else
		xml_set_value("GnomeSword", "misc", "showpreview", "0");
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

void on_checkbutton4_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	if (togglebutton->active)
		xml_set_value("GnomeSword", "misc", "showdicts", "1");
	else
		xml_set_value("GnomeSword", "misc", "showdicts", "0");
	settings.showdicts = atoi(xml_get_value("misc", "showdicts"));
	GTK_CHECK_MENU_ITEM(widgets.viewdicts_item)->active =
	    settings.showdicts;
	gui_show_hide_dicts(togglebutton->active);
}

/******************************************************************************
 * Name
 *   on_checkbutton5_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton5_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *   
 *   
 *
 * Return value
 *   void
 */

void on_checkbutton5_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	extern gboolean style_display;
	xml_set_value("GnomeSword", "misc", "versestyle",
		      (togglebutton->active ? "1" : "0"));
	settings.versestyle = togglebutton->active;
	
	style_display = TRUE;	
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
			       (widgets.versestyle_item),
			       settings.versestyle);
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

void on_checkbutton10_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "misc", "pinnedtabs",
		      (togglebutton->active ? "1" : "0"));
	settings.pinnedtabs = atoi(xml_get_value("misc", "pinnedtabs"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
			       (widgets.pinnedtabs_item),
			       settings.pinnedtabs);
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

void on_checkbutton11_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "misc", "readaloud",
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

void on_checkbutton12_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "misc", "showversenum",
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

void on_checkbutton6_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "lexicons", "usedefaultdict",
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

void on_checkbutton7_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "misc", "dailydevotional",
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

void on_checkbutton8_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "misc", "splash",
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

void on_checkbutton_scroll_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "misc", "chapter-scroll",
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

void on_checkbutton_imageresize_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "misc", "imageresize",
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

void on_checkbutton_versehighlight_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "misc", "versehighlight",
		      (togglebutton->active ? "1" : "0"));
	settings.versehighlight = togglebutton->active;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
			       (widgets.versehighlight_item),
			       settings.versehighlight);
	main_display_bible(settings.MainWindowModule, settings.currentverse);
}

/******************************************************************************
 * Name
 *   on_checkbutton_doublespace_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_checkbutton_doublespace_toggled(GtkToggleButton * togglebutton, gpointer user_data)
 *
 * Description
 *   
 *   
 * Return value
 *   void
 */

void on_checkbutton_doublespace_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	xml_set_value("GnomeSword", "misc", "doublespace",
		      (togglebutton->active ? "1" : "0"));
	settings.doublespace = togglebutton->active;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
			       (widgets.doublespace_item),
			       settings.doublespace);
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
 
void on_folder_changed(GtkFileChooser * filechooser, gpointer user_data)
{
	gchar *directory = gtk_file_chooser_get_current_folder(filechooser);
	xml_set_value("GnomeSword", "studypad", "directory", directory);
	settings.studypaddir = xml_get_value("studypad", "directory");
	//g_message("on__folder_changed to %s", directory);
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

void on_combobox1_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;	
	xml_set_value("GnomeSword", "fontsize", "versenum", buf);
	settings.verse_num_font_size_str = xml_get_value("fontsize", "versenum");
	settings.verse_num_font_size = atoi(settings.verse_num_font_size_str);
	url = g_strdup_printf("sword://%s/%s",settings.MainWindowModule,
					      settings.currentverse);
	main_url_handler(url);
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

void on_basecombobox1_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;	
	xml_set_value("GnomeSword", "fontsize", "basefontsize", buf);
	settings.base_font_size_str = xml_get_value("fontsize", "basefontsize");
	settings.base_font_size = atoi(settings.base_font_size_str);
	url = g_strdup_printf("sword://%s/%s",settings.MainWindowModule,
					      settings.currentverse);
	main_url_handler(url);
	g_free(url);
	g_free(buf);
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

void on_combobox2_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	
	url = g_strdup_printf("sword://%s/%s",buf,settings.currentverse);
	main_url_handler(url);
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

void on_combobox4_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	
	url = g_strdup_printf("sword://%s/%s",buf,settings.dictkey);
	main_url_handler(url);
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

void on_combobox5_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	xml_set_value("GnomeSword", "lexicons", "defaultdictionary", buf);
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

void on_combobox6_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	xml_set_value("GnomeSword", "modules", "percomm", buf);
	settings.personalcommentsmod = xml_get_value("modules", "percomm");
	g_free(buf);
}


/******************************************************************************
 * Name
 *   on_combobox7_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_combobox7_changed(GtkEditable * editable, gpointer user_data)	
 *
 * Description
 *    
 *   has changed - update parallel pane
 *
 * Return value
 *  void 
 */

void on_combobox7_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	main_change_parallel_module(PARALLEL1, buf);
	g_free(buf);
}


/******************************************************************************
 * Name
 *   on_combobox8_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_combobox8_changed(GtkEditable * editable, gpointer user_data)	
 *
 * Description
 *    
 *   has changed - update parallel pane
 *
 * Return value
 *  void 
 */

void on_combobox8_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	main_change_parallel_module(PARALLEL2, buf);
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox9_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_combobox9_changed(GtkEditable * editable, gpointer user_data)	
 *
 * Description
 *    
 *   has changed - update parallel pane
 *
 * Return value
 *  void 
 */

void on_combobox9_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	main_change_parallel_module(PARALLEL3, buf);
	g_free(buf);
}


/******************************************************************************
 * Name
 *   on_combobox10_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_combobox10_changed(GtkEditable * editable, gpointer user_data)	
 *
 * Description
 *    
 *   has changed - update parallel pane
 *
 * Return value
 *  void 
 */

void on_combobox10_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	main_change_parallel_module(PARALLEL4, buf);
	g_free(buf);
}

/******************************************************************************
 * Name
 *   on_combobox11_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_combobox11_changed(GtkEditable * editable, gpointer user_data)	
 *
 * Description
 *    
 *   has changed - update parallel pane
 *
 * Return value
 *  void 
 */

void on_combobox11_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	main_change_parallel_module(PARALLEL5, buf);
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

void on_combobox12_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	xml_set_value("GnomeSword", "modules", "devotional", buf);
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

void on_combobox13_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	xml_set_value("GnomeSword", "lexicons", "hebrew", buf);
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

void on_combobox14_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;
	xml_set_value("GnomeSword", "lexicons", "greek", buf);
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

void on_combobox15_changed(GtkComboBox * combobox, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &buf, -1);
	if (!buf)
		return;	
	url = g_strdup_printf("sword://%s/%s",buf,"1");
	main_url_handler(url);
	g_free(url);
	g_free(buf);
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

static void add_columns(GtkWidget * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new();	//gui_preferences_cell_renderer_captioned_image_new();
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

static 
void tree_selection_changed(GtkTreeSelection * selection, gpointer data)
{
	GtkTreeIter selected;
	gchar *name = NULL;
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

void on_dialog_prefs_response(GtkDialog * dialog, gint response_id, 
					gpointer user_data)
{
	if (response_id == GTK_RESPONSE_CLOSE) {
		xml_save_settings_doc(settings.fnconfigure);
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
}


static GtkTreeModel *create_model(void)
{
	GtkTreeStore *model;
	GtkTreeIter iter;
	GtkTreeIter child_iter;

	model = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	//model = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);

	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("Fonts"), -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Color"), 1, 1, -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Misc"), 1, 2, -1);


	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("General"), -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Tabs and Panes"), 1,
			   3, -1);

/*	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, "Panes", 1, 3, -1);*/

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Misc"), 1, 4, -1);


	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("Modules"), -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Main"), 1, 5, -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Parallel"), 1, 6, -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Misc"), 1, 7, -1);

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

static void setup_color_pickers(void)
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

/*
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
*/

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


static void setup_check_buttons(void)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.enable_tabbed_browsing),
				     settings.browsing);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_verse_style),
				     settings.versestyle);
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
#ifdef DOUBLESPACE
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.doublespace),
				     settings.doublespace);
#endif /* DOUBLESPACE */

	g_signal_connect(check_button.enable_tabbed_browsing, "toggled",
			 G_CALLBACK(on_checkbutton1_toggled), NULL);
	g_signal_connect(check_button.use_verse_style, "toggled",
			 G_CALLBACK(on_checkbutton5_toggled), NULL);
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
#ifdef DOUBLESPACE
	g_signal_connect(check_button.doublespace, "toggled",
			 G_CALLBACK(on_checkbutton_doublespace_toggled), NULL);
#endif /* DOUBLESPACE */
}


static
void fill_combobox(GList * glist, GtkComboBox * combo, gchar * current_module)
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

	while(glist) {
		gtk_list_store_append(GTK_LIST_STORE(store), &iter);
		gtk_list_store_set(GTK_LIST_STORE(store), 
					&iter, 
					0, 
					(gchar*)glist->data, 
					-1);
		if(current_module && !strcmp((gchar*)glist->data, current_module))
			index = i;
		++i;
		glist = g_list_next(glist);
	}
	gtk_combo_box_set_active(combo, index);
}


static
gint get_font_size_index(const char *font)
{
	gint i = 0;
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
}


static
void setup_module_comboboxs(void)
{
	GList *glist = NULL;
	/* Main Window Modules page */
	glist = get_list(TEXT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.text_module), settings.MainWindowModule);
	glist = get_list(COMM_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.commentary_module), settings.CommWindowModule);
	glist = get_list(DICT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.dictionary_module), settings.DictWindowModule);
	glist = get_list(DICT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.default_dictionary_module), settings.DefaultDict);
	glist = get_list(PERCOMM_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.percomm_module), settings.personalcommentsmod);
	glist = get_list(GBS_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.book_module), settings.book_mod);
	/* parallel page */	
	glist = get_list(TEXT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.parallel_1_module), settings.parallel1Module);	
	glist = get_list(TEXT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.parallel_2_module), settings.parallel2Module);		
	glist = get_list(TEXT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.parallel_3_module), settings.parallel3Module);	
	glist = get_list(TEXT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.parallel_4_module), settings.parallel4Module);
	glist = get_list(TEXT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.parallel_5_module), settings.parallel5Module);
	/* Miscellaneous Modules page */
	glist = get_list(DEVOTION_LIST);
	if(glist)  {
		fill_combobox(glist,GTK_COMBO_BOX(combo.devotion_module), settings.devotionalmod);
		
	}
	glist = get_list(DICT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.hebrew_lex__module), settings.lex_hebrew);
	glist = get_list(DICT_LIST);
	if(glist) 
		fill_combobox(glist,GTK_COMBO_BOX(combo.greek_lex__module), settings.lex_greek);

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
	g_signal_connect(combo.parallel_1_module, "changed",
			 G_CALLBACK(on_combobox7_changed), NULL);
	g_signal_connect(combo.parallel_2_module, "changed",
			 G_CALLBACK(on_combobox8_changed), NULL);
	g_signal_connect(combo.parallel_3_module, "changed",
			 G_CALLBACK(on_combobox9_changed), NULL);
	g_signal_connect(combo.parallel_4_module, "changed",
			 G_CALLBACK(on_combobox10_changed), NULL);
	g_signal_connect(combo.parallel_5_module, "changed",
			 G_CALLBACK(on_combobox11_changed), NULL);
	g_signal_connect(combo.devotion_module, "changed",
			 G_CALLBACK(on_combobox12_changed), NULL);
	g_signal_connect(combo.hebrew_lex__module, "changed",
			 G_CALLBACK(on_combobox13_changed), NULL);
	g_signal_connect(combo.greek_lex__module, "changed",
			 G_CALLBACK(on_combobox14_changed), NULL);

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

static void create_preferences_dialog(void)
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
/*
	color_picker.verse_numbers = glade_xml_get_widget (gxml, "colorbutton4");
	g_signal_connect(color_picker.verse_numbers, "color_set",
			 G_CALLBACK(on_colorbutton4_color_set), NULL);
*/
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
	check_button.use_verse_style = glade_xml_get_widget(gxml, "checkbutton5");
	check_button.use_pinned_tabs = glade_xml_get_widget(gxml, "checkbutton10");
	check_button.readaloud = glade_xml_get_widget(gxml, "checkbutton11");
	check_button.show_verse_num = glade_xml_get_widget(gxml, "checkbutton12");
	check_button.use_default_dictionary = glade_xml_get_widget(gxml, "checkbutton6");
	check_button.show_devotion = glade_xml_get_widget(gxml, "checkbutton7");
	check_button.show_splash_screen = glade_xml_get_widget(gxml, "checkbutton8");
	check_button.use_chapter_scroll = glade_xml_get_widget(gxml, "checkbutton_scroll");
	check_button.use_imageresize = glade_xml_get_widget(gxml, "checkbutton_imageresize");
	check_button.versehighlight = glade_xml_get_widget(gxml, "checkbutton_versehighlight");
	check_button.doublespace = glade_xml_get_widget(gxml, "checkbutton_doublespace");
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
	combo.parallel_1_module = glade_xml_get_widget (gxml, "combobox7");
	combo.parallel_2_module = glade_xml_get_widget (gxml, "combobox8");
	combo.parallel_3_module = glade_xml_get_widget (gxml, "combobox9");
	combo.parallel_4_module = glade_xml_get_widget (gxml, "combobox10");
	combo.parallel_5_module = glade_xml_get_widget (gxml, "combobox11");
	combo.devotion_module = glade_xml_get_widget (gxml, "combobox12");
	combo.hebrew_lex__module = glade_xml_get_widget (gxml, "combobox13");
	combo.greek_lex__module = glade_xml_get_widget (gxml, "combobox14");
	combo.book_module = glade_xml_get_widget (gxml, "combobox15");
	setup_module_comboboxs();
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
	/* create preferences dialog */
	create_preferences_dialog();
}
