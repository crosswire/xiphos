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

typedef enum {
	SHOW_BIBLE_TABS,
	SHOW_COMMENTARY_TABS,
	SHOW_DICTIONARY_TABS,
	SHOW_BOOK_TABS,
	USE_DEFAULT_DICTIONARY,
	USE_VERSE_STYLE,
	SHOW_SPLASH_SCREEN,
	SHOW_BIBLE_PANE,
	SHOW_COMMENTARY_PANE,
	SHOW_LOWER_WORKBOOK,
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
	HREF_LINKS
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
	VERSE_NUMBER_SIZE
} which_entry;

typedef struct _preferences_entry ENTRY;
struct _preferences_entry {
	GtkWidget *text_module;	/*  Main Window Module  */
	GtkWidget *parallel_1_module;
	GtkWidget *parallel_2_module;
	GtkWidget *parallel_3_module;
	GtkWidget *parallel_4_module;
	GtkWidget *parallel_5_module;
	GtkWidget *commentary_module;
	GtkWidget *dictionary_module;
	GtkWidget *default_dictionary_module;
	GtkWidget *percomm_module;
	GtkWidget *devotion_module;
	GtkWidget *greek_lex__module;
	GtkWidget *hebrew_lex__module;
	GtkWidget *greek_lex_viewer_module;
	GtkWidget *hebrew_lex_viewer_module;
	GtkWidget *combo_entry_sp_dir;
	GtkWidget *verse_number_size;
};

typedef struct _preferences_color_pickers COLOR_PICKERS;
struct _preferences_color_pickers {
	GtkWidget *text;
	GtkWidget *text_background;
	GtkWidget *text_current_verse;
	GtkWidget *verse_numbers;
	GtkWidget *href_links;
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
	GtkWidget *show_splash_screen;

	GtkWidget *show_bible_pane;
	GtkWidget *show_commentary_pane;
	GtkWidget *show_lower_workbook;

	//GtkWidget *show_favorites;
	//GtkWidget *show_text_group;
	//GtkWidget *show_commentary_group;
	//GtkWidget *show_dictionary_group;
	//GtkWidget *show_book_group;
	//GtkWidget *show_history_group;
	//GtkWidget *dock_shortcut_bar;

	GtkWidget *show_in_viewer;
	GtkWidget *show_in_dictionary;
	GtkWidget *show_devotion;
	//GtkWidget *percomm_formatting;
	//GtkWidget *use_studypad;
	//GtkWidget *use_studypad_dialog;
	//GtkWidget *use_percomm_dialog;

	//GtkWidget *commentary_in_dialog;
	//GtkWidget *dictionary_in_dialog;
	//GtkWidget *book_in_dialog;
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
static GtkWidget *notebook7;
static GtkWidget *dlg;
static ENTRY entry;
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
 *  gdouble_arr_to_hex
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   gchar *gdouble_arr_to_hex(gdouble * color, gint websafe)	
 *
 * Description
 *    this code is from bluefish-0.6
 *
 * Return value
 *   gchar *
 */

static gchar *gdouble_arr_to_hex(gdouble * color, gint websafe)
{
	gchar *tmpstr;
	unsigned int red_int;
	unsigned int green_int;
	unsigned int blue_int;
	gdouble red;
	gdouble green;
	gdouble blue;

	red = color[0];
	green = color[1];
	blue = color[2];

	if (websafe) {
		red_int = 0x33 * ((unsigned int) (red * 255 / 0x33));
		green_int =
		    0x33 * ((unsigned int) (green * 255 / 0x33));
		blue_int = 0x33 * ((unsigned int) (blue * 255 / 0x33));
	} else {
		red_int = (unsigned int) (red * 255);
		green_int = (unsigned int) (green * 255);
		blue_int = (unsigned int) (blue * 255);
	}
	tmpstr = g_malloc(8 * sizeof(char));
	g_snprintf(tmpstr, 8, "#%.2X%.2X%.2X", red_int, green_int,
		   blue_int);
	return tmpstr;
}


/******************************************************************************
 * Name
 *  hex_to_gdouble_arr
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   gdouble *hex_to_gdouble_arr(gchar * color)	
 *
 * Description
 *    hex_to_gdouble_arr -- this code is from bluefish-0.6
 *
 * Return value
 *   gdouble *
 */

static gdouble *hex_to_gdouble_arr(gchar * color)
{
	static gdouble tmpcol[4];
	gchar tmpstr[8];
	long tmpl;


	strncpy(tmpstr, &color[1], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[0] = (gdouble) tmpl;

	strncpy(tmpstr, &color[3], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[1] = (gdouble) tmpl;

	strncpy(tmpstr, &color[5], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[2] = (gdouble) tmpl;

	tmpcol[3] = 0;

	return tmpcol;
}


/******************************************************************************
 * Name
 *   on_colorpicker_color_set
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_colorpicker_color_set(
 *				GnomeColorPicker * gnomecolorpicker,
 *				guint arg1, guint arg2,	guint arg3, 
 *				guint arg4, gpointer user_data)	
 *
 * Description
 *   a color picker has changed
 *   set ok - apply button sensitive
 *   set update html to true
 *
 * Return value
 *   void
 */

static void on_colorpicker_color_set(GnomeColorPicker *
				     gnomecolorpicker, guint arg1,
				     guint arg2, guint arg3, guint arg4,
				     gpointer user_data)
{
	gchar *buf2 = NULL;
	gdouble color[4];

	gnome_color_picker_get_d(gnomecolorpicker,
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf2 = gdouble_arr_to_hex(color, 1);

	switch (GPOINTER_TO_INT(user_data)) {
	case TEXT_FORGROUND:
		xml_set_value("GnomeSword", "HTMLcolors", "text_fg",
			      buf2);
		settings.bible_text_color =
		    xml_get_value("HTMLcolors", "text_fg");
		break;
	case TEXT_BACKGROUND:
		xml_set_value("GnomeSword", "HTMLcolors", "background",
			      buf2);
		settings.bible_bg_color =
		    xml_get_value("HTMLcolors", "background");
		break;
	case TEXT_CURRENT_VERSE:
		xml_set_value("GnomeSword", "HTMLcolors",
			      "currentverse", buf2);
		settings.currentverse_color =
		    xml_get_value("HTMLcolors", "currentverse");
		break;
	case VERSE_NUMBERS:
		xml_set_value("GnomeSword", "HTMLcolors", "versenum",
			      buf2);
		settings.bible_verse_num_color =
		    xml_get_value("HTMLcolors", "versenum");
		break;
	case HREF_LINKS:
		xml_set_value("GnomeSword", "HTMLcolors", "link", buf2);
		settings.link_color =
		    xml_get_value("HTMLcolors", "link");
		break;
	}
	if (buf2)
		g_free(buf2);


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
	xml_set_value("GnomeSword", "misc", "setup_canceled", "0");
}

/******************************************************************************
 * Name
 *   on_entry_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_entry_changed(GtkEditable * editable, gpointer user_data)	
 *
 * Description
 *   a combo entry has changed
 *   set ok - apply button sensitive
 *   set update html to true
 *
 * Return value
 *  void 
 */

static void on_entry_changed(GtkEditable * editable, gpointer user_data)
{
	gchar *buf = NULL;
	gchar *url = NULL;

	buf = gtk_editable_get_chars(editable, 0, -1);
	if (!buf)
		return;
	
	switch (GPOINTER_TO_INT(user_data)) {
	case TEXT_MODULE:
		url = 
		     g_strdup_printf("sword://%s/%s",buf,settings.currentverse);
		main_url_handler(url);
		g_free(url);
		break;
	case PARALLEL_1_MODULE:
		main_change_parallel_module(PARALLEL1, buf);
		break;
	case PARALLEL_2_MODULE:
		main_change_parallel_module(PARALLEL2, buf);
		break;
	case PARALLEL_3_MODULE:
		main_change_parallel_module(PARALLEL3, buf);
		break;
	case PARALLEL_4_MODULE:
		main_change_parallel_module(PARALLEL4, buf);
		break;
	case PARALLEL_5_MODULE:
		main_change_parallel_module(PARALLEL5, buf);
		break;
	case COMMENTARY_MODULE:
		url = g_strdup_printf("sword://%s/%s",buf,settings.currentverse);
		main_url_handler(url);
		g_free(url);
		break;
	case DICTIONARY_MODULE:
		url = g_strdup_printf("sword://%s/%s",buf,settings.dictkey);
		main_url_handler(url);
		g_free(url);
		break;
	case DEFAULT_DICTIONARY_MODULE:
		xml_set_value("GnomeSword", "lexicons",
			      "defaultdictionary", buf);
		settings.DefaultDict =
		    xml_get_value("lexicons", "defaultdictionary");
		return;
		break;
	case PERCOMM_MODULE:
		xml_set_value("GnomeSword", "modules", "percomm", buf);
		settings.personalcommentsmod =
		    xml_get_value("modules", "percomm");
		return;
		break;
	case DEVOTION_MODULE:
		xml_set_value("GnomeSword", "modules", "devotional",
			      buf);
		settings.devotionalmod =
		    xml_get_value("modules", "devotional");
		return;
		break;
	case GREEK_LEX__MODULE:
		xml_set_value("GnomeSword", "lexicons", "greek", buf);
		settings.lex_greek = xml_get_value("lexicons", "greek");
		return;
		break;
	case HEBREW_LEX__MODULE:
		xml_set_value("GnomeSword", "lexicons", "hebrew", buf);
		settings.lex_hebrew =
		    xml_get_value("lexicons", "hebrew");
		break;
	case GREEK_LEX_VIEWER_MODULE:
		xml_set_value("GnomeSword", "lexicons", "greekviewer",
			      buf);
		settings.lex_greek_viewer =
		    xml_get_value("lexicons", "greekviewer");
		return;
		break;
	case HEBREW_LEX_VIEWER_MODULE:
		xml_set_value("GnomeSword", "lexicons", "hebrewviewer",
			      buf);
		settings.lex_hebrew_viewer =
		    xml_get_value("lexicons", "hebrewviewer");
		return;
		break;
	case COMBO_ENTRY_SP_DIR:
		xml_set_value("GnomeSword", "studypad", "directory",
			      buf);
		settings.studypaddir =
		    xml_get_value("studypad", "directory");
		return;
		break;
	case VERSE_NUMBER_SIZE:
		xml_set_value("GnomeSword", "fontsize", "versenum",
			      buf);
		settings.verse_num_font_size =
		    xml_get_value("fontsize", "versenum");
		url = 
		     g_strdup_printf("sword://%s/%s",buf,settings.currentverse);
		main_url_handler(url);
		g_free(url);
		break;
	default:
		return;
		break;
	}
	xml_set_value("GnomeSword", "misc", "setup_canceled", "0");
}

/******************************************************************************
 * Name
 *   on_button_toggled
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_button_toggled(GtkToggleButton * togglebutton,
 *						gpointer user_data)	
 *
 * Description
 *   a toggle button has changed
 *   set ok - apply button sensitive
 *
 * Return value
 *   void
 */

static void on_button_toggled(GtkToggleButton * togglebutton,
			      gpointer user_data)
{
	switch (GPOINTER_TO_INT(user_data)) {
	case SHOW_BIBLE_TABS:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "tabs", "bible",
				      "1");
		else
			xml_set_value("GnomeSword", "tabs", "bible",
				      "0");
		settings.text_tabs =
		    atoi(xml_get_value("tabs", "bible"));
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK
					   (widgets.notebook_text),
					   settings.text_tabs);
/*		if (settings.havebible)
			gui_set_text_frame_label(cur_t);*/
		break;
	case SHOW_COMMENTARY_TABS:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "tabs", "comm",
				      "1");
		else
			xml_set_value("GnomeSword", "tabs", "comm",
				      "0");
		settings.comm_tabs =
		    atoi(xml_get_value("tabs", "comm"));
		/*gtk_notebook_set_show_tabs(GTK_NOTEBOOK
					   (widgets.notebook_comm),
					   settings.comm_tabs);*/
		/*if (settings.havecomm)
			gui_set_comm_frame_label(settings.CommWindowModule);*/
		break;
	case SHOW_DICTIONARY_TABS:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "tabs", "dict",
				      "1");
		else
			xml_set_value("GnomeSword", "tabs", "dict",
				      "0");
		settings.dict_tabs =
		    atoi(xml_get_value("tabs", "dict"));
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK
					   (widgets.notebook_dict),
					   settings.dict_tabs);
//		if (settings.havedict)
//			gui_set_dict_frame_label();
		break;
	case SHOW_BOOK_TABS:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "tabs", "book",
				      "1");
		else
			xml_set_value("GnomeSword", "tabs", "book",
				      "0");
		settings.book_tabs =
		    atoi(xml_get_value("tabs", "book"));
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK
					   (widgets.notebook_gbs),
					   settings.book_tabs);
//		if (settings.havebook)
//			gui_set_gbs_frame_label();
		break;
	case USE_DEFAULT_DICTIONARY:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "lexicons",
				      "usedefaultdict", "1");
		else
			xml_set_value("GnomeSword", "lexicons",
				      "usedefaultdict", "0");
		settings.useDefaultDict =
		    atoi(xml_get_value("lexicons", "usedefaultdict"));

		break;
	case USE_VERSE_STYLE:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "misc",
				      "versestyle", "1");
		else
			xml_set_value("GnomeSword", "misc",
				      "versestyle", "0");
		settings.versestyle =
		    atoi(xml_get_value("misc", "versestyle"));
		GTK_CHECK_MENU_ITEM(widgets.versestyle_item)->active =
		    settings.versestyle;
		break;
	case SHOW_SPLASH_SCREEN:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "misc", "splash",
				      "1");
		else
			xml_set_value("GnomeSword", "misc", "splash",
				      "0");
		settings.showsplash =
		    atoi(xml_get_value("misc", "splash"));
		break;
	case SHOW_BIBLE_PANE:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "misc", "showtexts",
				      "1");
		else
			xml_set_value("GnomeSword", "misc", "showtexts",
				      "0");
		settings.showtexts =
		    atoi(xml_get_value("misc", "showtexts"));
		GTK_CHECK_MENU_ITEM(widgets.viewtexts_item)->active =
		    settings.showtexts;
		gui_set_bible_comm_layout();
		break;
	case SHOW_COMMENTARY_PANE:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "misc", "showcomms",
				      "1");
		else
			xml_set_value("GnomeSword", "misc", "showcomms",
				      "0");
		settings.showcomms =
		    atoi(xml_get_value("misc", "showcomms"));
		GTK_CHECK_MENU_ITEM(widgets.viewcomms_item)->active =
		    settings.showcomms;
		gui_set_bible_comm_layout();
		break;
	case SHOW_LOWER_WORKBOOK:
		if (togglebutton->active)
			xml_set_value("GnomeSword", "misc", "showdicts",
				      "1");
		else
			xml_set_value("GnomeSword", "misc", "showdicts",
				      "0");
		settings.showdicts =
		    atoi(xml_get_value("misc", "showdicts"));
		GTK_CHECK_MENU_ITEM(widgets.viewdicts_item)->active =
		    settings.showdicts;
		gui_set_bible_comm_layout();
		break;
	case SHOW_IN_VIEWER:
		if (GTK_TOGGLE_BUTTON(check_button.show_in_viewer)->
		    active)
			xml_set_value("GnomeSword", "lexicons",
				      "inviewer", "1");
		else
			xml_set_value("GnomeSword", "lexicons",
				      "inviewer", "0");
		settings.inViewer =
		    atoi(xml_get_value("lexicons", "inviewer"));
		break;
	case SHOW_IN_DICTIONARY:
		if (GTK_TOGGLE_BUTTON(check_button.show_in_dictionary)->
		    active)
			xml_set_value("GnomeSword", "lexicons",
				      "indictpane", "1");
		else
			xml_set_value("GnomeSword", "lexicons",
				      "indictpane", "0");
		settings.inDictpane =
		    atoi(xml_get_value("lexicons", "indictpane"));
		break;
	case SHOW_DEVOTION:
		if (GTK_TOGGLE_BUTTON(check_button.show_devotion)->
		    active)
			xml_set_value("GnomeSword", "misc",
				      "dailydevotional", "1");
		else
			xml_set_value("GnomeSword", "misc",
				      "dailydevotional", "0");
		settings.showdevotional =
		    atoi(xml_get_value("misc", "dailydevotional"));
		break;
	case TABBED_BROWSING:
		if (GTK_TOGGLE_BUTTON
		    (check_button.enable_tabbed_browsing)->active) {
			xml_set_value("GnomeSword", "tabs", "browsing",
				      "1");
			settings.browsing = TRUE;
			gui_notebook_main_setup(NULL);
			gtk_widget_show(widgets.hboxtb);
		} else {
			xml_set_value("GnomeSword", "tabs", "browsing",
				      "0");
			gtk_widget_hide(widgets.hboxtb);
			settings.browsing = FALSE;
			gui_close_all_tabs();	//gui_notebook_main_shutdown();
		}
		settings.browsing =
		    atoi(xml_get_value("tabs", "browsing"));
		break;

		/*
		   case USE_STUDYPAD:   
		   if (GTK_TOGGLE_BUTTON(check_button.use_studypad)->active)
		   xml_set_value("GnomeSword", "editor", "UseStudyPad",
		   "1");
		   else
		   xml_set_value("GnomeSword", "editor", "UseStudyPad",
		   "0");
		   settings.use_studypad =
		   atoi(xml_get_value("editor", "UseStudyPad"));
		   break;
		   case USE_STUDYPAD_DIALOG:    
		   if (GTK_TOGGLE_BUTTON(check_button.use_studypad_dialog)->active)
		   xml_set_value("GnomeSword", "editor",
		   "UseStudypadDialog", "1");
		   else
		   xml_set_value("GnomeSword", "editor",
		   "UseStudypadDialog", "0");
		   settings.use_studypad_dialog =
		   atoi(xml_get_value("editor", "UseStudypadDialog"));
		   break;
		   case USE_PERCOMM_DIALOG:
		   if (GTK_TOGGLE_BUTTON(check_button.use_percomm_dialog)->active)
		   xml_set_value("GnomeSword", "editor",
		   "UsePercommDialog", "1");
		   else
		   xml_set_value("GnomeSword", "editor",
		   "UsePercommDialog", "0");
		   settings.use_percomm_dialog =
		   atoi(xml_get_value("editor", "UsePercommDialog"));
		   break; */
	}
	xml_set_value("GnomeSword", "editor", "UseStudyPad", "1");
	xml_set_value("GnomeSword", "editor", "UseStudypadDialog", "1");
	xml_set_value("GnomeSword", "editor", "UsePercommDialog", "1");
}


/******************************************************************************
 * Name
 *   setcolorpickersColor
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void setcolorpickersColor(GtkWidget * gcpTextBG,
 *				GtkWidget * gcpText,
 *				GtkWidget * gcpCurrentverse,
 *				GtkWidget * gcpTextVerseNums,
 *				GtkWidget * gcpTextLinks)	
 *
 * Description
 *   set dialog color picker to current settings
 *
 * Return value
 *   void
 */

static void setcolorpickersColor(GtkWidget * gcpTextBG,
				 GtkWidget * gcpText,
				 GtkWidget * gcpCurrentverse,
				 GtkWidget * gcpTextVerseNums,
				 GtkWidget * gcpTextLinks)
{
	gdouble *color;
	gushort a = 000000;

	if (string_is_color(settings.bible_bg_color)) {
		color = hex_to_gdouble_arr(settings.bible_bg_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextBG), color[0],
					  color[1], color[2], a);
	} else {
		color = hex_to_gdouble_arr("#FFFFFF");
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextBG), color[0],
					  color[1], color[2], a);
	}

	if (string_is_color(settings.bible_text_color)) {
		color = hex_to_gdouble_arr(settings.bible_text_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER(gcpText),
					  color[0], color[1], color[2],
					  a);
	} else {
		color = hex_to_gdouble_arr("#000000");
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpText), color[0],
					  color[1], color[2], a);
	}

	if (string_is_color(settings.currentverse_color)) {
		color = hex_to_gdouble_arr(settings.currentverse_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpCurrentverse), color[0],
					  color[1], color[2], a);
	} else {
		color = hex_to_gdouble_arr("#339766");
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpCurrentverse), color[0],
					  color[1], color[2], a);
	}

	if (string_is_color(settings.bible_verse_num_color)) {
		color =
		    hex_to_gdouble_arr(settings.bible_verse_num_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextVerseNums), color[0],
					  color[1], color[2], a);
	} else {
		color = hex_to_gdouble_arr("#0000CF");
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextVerseNums), color[0],
					  color[1], color[2], a);
	}

	if (string_is_color(settings.link_color)) {
		color = hex_to_gdouble_arr(settings.link_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextLinks), color[0],
					  color[1], color[2], a);
	} else {
		color = hex_to_gdouble_arr("#878787");
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextLinks), color[0],
					  color[1], color[2], a);
	}

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

static void tree_selection_changed(GtkTreeSelection * selection,
				   gpointer data)
{
	GtkTreeIter selected;
	gchar *name = NULL;
	gint page;
	GtkTreeModel *model;

	if (!gtk_tree_selection_get_selected
	    (selection, &model, &selected))
		return;

	gtk_tree_model_get(model, &selected, 1, &page, -1);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook7), page);
}

/******************************************************************************
 * Name
 *   on_dialog1_response
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_dialog_response(GtkDialog * dialog, gint response_id,
 *							GS_DIALOG * info)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_dialog_response(GtkDialog * dialog, gint response_id,
			       gpointer data)
{
	if (response_id == GTK_RESPONSE_CLOSE) {
		xml_save_settings_doc(settings.fnconfigure);
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
	xml_set_value("GnomeSword", "misc", "setup_canceled", "0");
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

static GtkWidget *gui_create_preferences_dialog(GList * biblelist,
						GList * commlist,
						GList * dictlist,
						GList * percomlist,
						GList * devotionlist)
{
	gint groupnum;
	gchar *pathname = NULL;
	GdkPixbuf *icon_pixbuf;
	GtkWidget *shortcut_bar;
	GtkWidget *dialog_prefs;
	GtkWidget *dialog_vbox24;
	GtkWidget *hbox22;
	GtkWidget *scrolledwindow56;
	GtkWidget *viewport9;
	GtkWidget *frame26;
	GtkWidget *vbox35;
	GtkWidget *table13;
	GtkWidget *label149;
	GtkWidget *label145;
	GtkWidget *label103;
	GtkWidget *label150;
	GtkWidget *label146;
	GtkWidget *label152;
	GtkWidget *label147;
	GtkWidget *hbox_default_dir;
	GtkWidget *fileentry1;
	GtkWidget *label;
	//GtkWidget *gcpCurrentverseBG;
	GtkWidget *cmbVerseNumSize;
	GList *cmbVerseNumSize_items = NULL;
	GtkWidget *label98;
	GtkWidget *scrolledwindow55;
	GtkWidget *viewport8;
	GtkWidget *vbox41;
	GtkWidget *hbox64;
	GtkWidget *vbox28;
	GtkWidget *frame21;
	GtkWidget *vbox36;
	GSList *vbox36_group = NULL;
	GtkWidget *rbtnNoDefaults;
	GtkWidget *frame42;
	GtkWidget *vbox54;
	GtkWidget *frame72;
	GtkWidget *vbox86;
	GtkWidget *frame43;
	GtkWidget *vbox55;
	GtkWidget *frame41;
	GtkWidget *vbox53;
	GtkWidget *frame74;
	GtkWidget *vbox89;
	GtkWidget *label123;
	GtkWidget *scrolledwindow54;
	GtkWidget *viewport7;
	GtkWidget *frame27;
	GtkWidget *table10;
	GtkWidget *label165;
	GtkWidget *label166;
	GtkWidget *label167;
	GtkObject *sbtnAppWidth_adj;
	GtkObject *sbtnSBWidth_adj;
	GtkObject *sbtnUpPaneHight_adj;
	GtkWidget *label169;
	GtkObject *sbtnTextWidth_adj;
	GtkWidget *label168;
	GtkObject *sbtnAppHight_adj;
	GtkWidget *label190;
	GtkWidget *scrolledwindow53;
	GtkWidget *viewport6;
	GtkWidget *vbox48;
	GtkWidget *frame24;
	GtkWidget *vbox29;
	GtkWidget *label189;
	GtkWidget *scrolledwindow52;
	GtkWidget *viewport5;
	GtkWidget *table9;
	GtkWidget *label156;
	GtkWidget *combo17;
	GtkWidget *combo18;
	GtkWidget *combo19;
	GtkWidget *combo20;
	GtkWidget *label157;
	GtkWidget *label158;
	GtkWidget *label159;
	GtkWidget *combo21;
	GtkWidget *combo22;
	GtkWidget *combo23;
	GtkWidget *combo25;
	GtkWidget *label160;
	GtkWidget *label161;
	GtkWidget *label162;
	GtkWidget *label163;
	GtkWidget *label164;
	GtkWidget *labelDevotional;
	GtkWidget *comboDevotion;
	GtkWidget *combo24;
	GtkWidget *combo26;
	GtkWidget *label192;
	GtkWidget *combo27;
	GtkWidget *label202;
	GtkWidget *label203;
	GtkWidget *comboGreekViewer;
	GtkWidget *comboHebViewer;
	GtkWidget *label191;
	GtkWidget *label253;
	GtkWidget *comboDefaultDict;
	GtkWidget *label155;
	GtkWidget *dialog_action_area24;
	GtkWidget *hbuttonbox3;
	GtkWidget *button_cancel;
	GtkWidget *scrolledwindow_treeview;
	GtkWidget *treeview;
	GtkTooltips *tooltips;
	GError *error = NULL;
	GtkCellRenderer *renderer;
	GtkTreeModel *model;
	GtkTreeStore *store;
	GObject *selection;
	GtkWidget *label800;
	GtkWidget *label801;
	GtkWidget *label802;
	GtkWidget *label803;
	GtkWidget *label804;
	GtkWidget *label805;
	GtkWidget *label806;
	GtkWidget *label807;
	GtkWidget *vbox;
	GtkWidget *image;
//      GtkWidget *label;
	GtkWidget *hbox;
	gchar *header;

	tooltips = gtk_tooltips_new();

	dialog_prefs = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog_prefs), "dialog_prefs",
			    dialog_prefs);
	gtk_window_set_title(GTK_WINDOW(dialog_prefs),
			     _("Preferences"));
	gtk_window_set_default_size(GTK_WINDOW(dialog_prefs), 600, 345);
	gtk_window_set_policy(GTK_WINDOW(dialog_prefs), TRUE, TRUE,
			      FALSE);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog_prefs), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_prefs), 12);

	dialog_vbox24 = GTK_DIALOG(dialog_prefs)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_prefs), "dialog_vbox24",
			    dialog_vbox24);
	gtk_widget_show(dialog_vbox24);

	hbox22 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox22);
	gtk_box_pack_start(GTK_BOX(dialog_vbox24), hbox22, TRUE, TRUE,
			   0);

/*************************************************** start sidebar */

	scrolledwindow_treeview = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_treeview);
	gtk_box_pack_start(GTK_BOX(hbox22), scrolledwindow_treeview,
			   FALSE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_treeview),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	model = create_model();
	treeview = gtk_tree_view_new_with_model(model);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview),
					  FALSE);
	gtk_widget_show(treeview);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_treeview),
			  treeview);
	gtk_widget_set_size_request(treeview, 130, -1);
	add_columns(treeview);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));

	selection =
	    G_OBJECT(gtk_tree_view_get_selection
		     (GTK_TREE_VIEW(treeview)));

	/* add icons to sidebar treeview */
/*	icon_pixbuf = NULL;
	icon_pixbuf = gtk_widget_render_icon(widgets.app,
                                             GTK_STOCK_SELECT_FONT,  
                                             GTK_ICON_SIZE_DND,
                                             NULL);
	if (!icon_pixbuf) {
		fprintf(stderr, "pixmap file error: %s\n",
			error->message);
		g_error_free(error);
		error = NULL;
	}
	add_row(_("Font Colors"), icon_pixbuf, store, 0);

	icon_pixbuf = NULL;
	icon_pixbuf =
	    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
				     "/gs2-48x48.png", &error);
	if (!icon_pixbuf) {
		fprintf(stderr, "pixmap file error: %s\n",
			error->message);
		g_error_free(error);
		error = NULL;
	}
	add_row(_("General Settings"), icon_pixbuf, store, 1);*/

/*	icon_pixbuf = NULL;
	icon_pixbuf =
	    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
				     "/gs2-48x48.png", &error);
	if (!icon_pixbuf) {
		fprintf(stderr, "pixmap file error: %s\n",
			error->message);
		g_error_free(error);
		error = NULL;
	}*/
/*	add_row("Layout", icon_pixbuf, store, 2);*/
/*
	icon_pixbuf = NULL;
	icon_pixbuf =
	    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/sword3.png",
				     &error);
	if (!icon_pixbuf) {
		fprintf(stderr, "pixmap file error: %s\n",
			error->message);
		g_error_free(error);
		error = NULL;
	}
	add_row(_("Sword Modules"), icon_pixbuf, store, 2);*/
/***************************************************** end sidebar */


/************************************************** start notebook */
	notebook7 = gtk_notebook_new();
	gtk_widget_show(notebook7);
	gtk_box_pack_start(GTK_BOX(hbox22), notebook7, TRUE, TRUE, 0);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook7), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(notebook7), 12);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook7), FALSE);

/* start cover page */

	vbox = gtk_vbox_new(TRUE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 6);

	image =
	    gtk_image_new_from_file(PACKAGE_PIXMAPS_DIR "/logo.png");
	gtk_widget_show(image);
	gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);

	label = gtk_label_new("Cover Page");	/* should never be seen by user */
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook7), 0), label);

/* end cover page */


/* start font color page */
	scrolledwindow56 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow56);
	gtk_container_add(GTK_CONTAINER(notebook7), scrolledwindow56);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow56),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	viewport9 = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport9);
	gtk_container_add(GTK_CONTAINER(scrolledwindow56), viewport9);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(viewport9),
				     GTK_SHADOW_NONE);

	//frame26 = gtk_frame_new(_("Font Colors"));
	frame26 = gtk_frame_new(NULL);

	gtk_frame_set_shadow_type(GTK_FRAME(frame26), GTK_SHADOW_NONE);

	label800 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Font Colors"));
	gtk_label_set_markup(GTK_LABEL(label800), header);
	g_free(header);

	gtk_widget_show(label800);
	gtk_frame_set_label_widget(GTK_FRAME(frame26), label800);
	gtk_misc_set_alignment(GTK_MISC(label800), 0, 0);

	gtk_widget_show(frame26);
	gtk_container_add(GTK_CONTAINER(viewport9), frame26);

	vbox35 = gtk_vbox_new(TRUE, 0);
	gtk_widget_show(vbox35);
	gtk_container_add(GTK_CONTAINER(frame26), vbox35);
	gtk_container_set_border_width(GTK_CONTAINER(vbox35), 6);



	/**************************************************************/

	hbox = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox, TRUE, TRUE, 0);


	label149 = gtk_label_new(_("Background"));
	gtk_widget_show(label149);
	gtk_box_pack_start(GTK_BOX(hbox), label149, TRUE, TRUE, 6);
	gtk_label_set_justify(GTK_LABEL(label149), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label149), 0.05, 0.5);


	color_picker.text_background = gnome_color_picker_new();
	gtk_widget_show(color_picker.text_background);
	gtk_box_pack_start(GTK_BOX(hbox), color_picker.text_background,
			   TRUE, TRUE, 6);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER
				     (color_picker.text_background),
				     _("Pick the background color"));

	hbox = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox, TRUE, TRUE, 0);


	label145 = gtk_label_new(_("Bible text"));
	gtk_widget_show(label145);
	gtk_box_pack_start(GTK_BOX(hbox), label145, TRUE, TRUE, 6);
	gtk_label_set_justify(GTK_LABEL(label145), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label145), 0.05, 0.5);


	color_picker.text = gnome_color_picker_new();
	gtk_widget_show(color_picker.text);
	gtk_box_pack_start(GTK_BOX(hbox), color_picker.text, TRUE, TRUE,
			   6);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER
				     (color_picker.text),
				     _("Pick the Bible text color"));


	hbox = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox, TRUE, TRUE, 0);

	label103 = gtk_label_new(_("Current Verse text"));
	gtk_widget_show(label103);
	gtk_box_pack_start(GTK_BOX(hbox), label103, TRUE, TRUE, 6);
	gtk_label_set_justify(GTK_LABEL(label103), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label103), 0.05, 0.5);

	color_picker.text_current_verse = gnome_color_picker_new();
	gtk_widget_show(color_picker.text_current_verse);
	gtk_box_pack_start(GTK_BOX(hbox),
			   color_picker.text_current_verse, TRUE, TRUE,
			   6);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER
				     (color_picker.text_current_verse),
				     _("Pick the current Verse color"));

/*
	label150 = gtk_label_new(_("Current Verse background"));
	//gtk_widget_show(label150);
	gtk_table_attach(GTK_TABLE(table13), label150, 0, 1, 2, 3,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_size_request(label150, 218, -1);
	gtk_label_set_justify(GTK_LABEL(label150), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label150), 0.01, 0.5);
*/


	hbox = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox, TRUE, TRUE, 0);

	label146 = gtk_label_new(_("Verse numbers"));
	gtk_widget_show(label146);
	gtk_box_pack_start(GTK_BOX(hbox), label146, TRUE, TRUE, 6);
	gtk_label_set_justify(GTK_LABEL(label146), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label146), 0.05, 0.5);


	color_picker.verse_numbers = gnome_color_picker_new();
	gtk_widget_show(color_picker.verse_numbers);
	gtk_box_pack_start(GTK_BOX(hbox), color_picker.verse_numbers,
			   TRUE, TRUE, 6);
	gtk_widget_set_size_request(color_picker.verse_numbers, 41, -1);


	hbox = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox, TRUE, TRUE, 0);

	label147 = gtk_label_new(_("Links"));
	gtk_widget_show(label147);
	gtk_box_pack_start(GTK_BOX(hbox), label147, TRUE, TRUE, 6);
	gtk_label_set_justify(GTK_LABEL(label147), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label147), 0.05, 0.5);



/*
	gcpCurrentverseBG = gnome_color_picker_new();
	//gtk_widget_show(gcpCurrentverseBG);
	gtk_table_attach(GTK_TABLE(table13), gcpCurrentverseBG, 1, 2, 2,
			 3, (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER
				     (gcpCurrentverseBG),
				     _
				     ("Pick the current Verse background color"));
*/



	color_picker.href_links = gnome_color_picker_new();
	gtk_widget_show(color_picker.href_links);
	gtk_box_pack_start(GTK_BOX(hbox), color_picker.href_links, TRUE,
			   TRUE, 6);
	gtk_tooltips_set_tip(tooltips, color_picker.href_links,
			     _("Strong's Numbers & Morphological Tags"),
			     NULL);


	button.gnomesword_defaults =
	    gtk_button_new_with_label(_("GnomeSword Defaults"));
	/*gtk_widget_show(button.gnomesword_defaults);
	   gtk_box_pack_start(GTK_BOX(vbox35), button.gnomesword_defaults,
	   TRUE, TRUE, 0);
	   gtk_widget_set_sensitive(button.gnomesword_defaults, FALSE);
	   gtk_tooltips_set_tip(tooltips, button.gnomesword_defaults,
	   _("Use GnomeSword defaults for all settings on this page"),
	   NULL);
	   gtk_button_set_relief(GTK_BUTTON(button.gnomesword_defaults),
	   GTK_RELIEF_HALF); */

	label98 = gtk_label_new(_("Bible Text Window"));
	gtk_widget_show(label98);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook7), 0),
				   label98);
/* end font color page */

/* start fonts misc page */

	frame26 = gtk_frame_new(NULL);

	gtk_frame_set_shadow_type(GTK_FRAME(frame26), GTK_SHADOW_NONE);

	label800 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Font Miscellaneous"));
	gtk_label_set_markup(GTK_LABEL(label800), header);
	g_free(header);

	gtk_widget_show(label800);
	gtk_frame_set_label_widget(GTK_FRAME(frame26), label800);
	gtk_misc_set_alignment(GTK_MISC(label800), 0, 0);

	gtk_widget_show(frame26);
	gtk_container_add(GTK_CONTAINER(notebook7), frame26);
	
	vbox = gtk_vbox_new(TRUE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(frame26), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 6);



	hbox = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);


	label152 = gtk_label_new(_("Verse number font size"));
	gtk_widget_show(label152);
	gtk_box_pack_start(GTK_BOX(hbox), label152, TRUE, TRUE, 6);
	gtk_label_set_justify(GTK_LABEL(label152), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label152), 0.05, 0.5);


	cmbVerseNumSize = gtk_combo_new();
	gtk_widget_show(cmbVerseNumSize);
	gtk_box_pack_start(GTK_BOX(hbox), cmbVerseNumSize, TRUE, TRUE,
			   6);
	gtk_combo_set_value_in_list(GTK_COMBO(cmbVerseNumSize), TRUE,
				    TRUE);
	cmbVerseNumSize_items =
	    g_list_append(cmbVerseNumSize_items, (gpointer) _("-2"));
	cmbVerseNumSize_items =
	    g_list_append(cmbVerseNumSize_items, (gpointer) _("-1"));
	cmbVerseNumSize_items =
	    g_list_append(cmbVerseNumSize_items, (gpointer) _("+0"));
	cmbVerseNumSize_items =
	    g_list_append(cmbVerseNumSize_items, (gpointer) _("+1"));
	cmbVerseNumSize_items =
	    g_list_append(cmbVerseNumSize_items, (gpointer) _("+2"));
	cmbVerseNumSize_items =
	    g_list_append(cmbVerseNumSize_items, (gpointer) _("+3"));
	cmbVerseNumSize_items =
	    g_list_append(cmbVerseNumSize_items, (gpointer) _("+4"));
	cmbVerseNumSize_items =
	    g_list_append(cmbVerseNumSize_items, (gpointer) _("+5"));
	gtk_combo_set_popdown_strings(GTK_COMBO(cmbVerseNumSize),
				      cmbVerseNumSize_items);
	g_list_free(cmbVerseNumSize_items);

	entry.verse_number_size = GTK_COMBO(cmbVerseNumSize)->entry;
	gtk_widget_show(entry.verse_number_size);
	gtk_tooltips_set_tip(tooltips, entry.verse_number_size,
			     _
			     ("Zero is the base font size. Increase or decrease the size."),
			     NULL);
	gtk_entry_set_text(GTK_ENTRY(entry.verse_number_size), _("+0"));
	   
/* end font misc page */


/* start tabs and panes page */

/*	scrolledwindow55 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow55);
	gtk_container_add(GTK_CONTAINER(notebook7), scrolledwindow55);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow55),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	viewport8 = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport8);
	gtk_container_add(GTK_CONTAINER(scrolledwindow55), viewport8);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(viewport8),
				     GTK_SHADOW_NONE);
	gtk_container_set_border_width(GTK_CONTAINER(viewport8), 6);*/

	vbox41 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox41);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox41);

	hbox64 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox64);
	gtk_box_pack_start(GTK_BOX(vbox41), hbox64, TRUE, TRUE, 0);

	vbox28 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox28);
	gtk_box_pack_start(GTK_BOX(hbox64), vbox28, TRUE, TRUE, 0);

	//frame21 = gtk_frame_new(_("Use Defaults When Starting GnomeSword"));
	frame21 = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame21), GTK_SHADOW_NONE);

	label801 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Use Defaults When Starting GnomeSword"));
	gtk_label_set_markup(GTK_LABEL(label801), header);
	g_free(header);

	gtk_widget_show(label801);
	gtk_frame_set_label_widget(GTK_FRAME(frame21), label801);

//      gtk_widget_show(frame21);
	gtk_box_pack_start(GTK_BOX(vbox28), frame21, TRUE, FALSE, 6);

	vbox36 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox36);
	gtk_container_add(GTK_CONTAINER(frame21), vbox36);
	gtk_container_set_border_width(GTK_CONTAINER(vbox36), 6);

	check_button.use_defaults =
	    gtk_radio_button_new_with_label(vbox36_group,
					    _("Yes, use defaults"));
	vbox36_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON
				       (check_button.use_defaults));
	gtk_widget_show(check_button.use_defaults);
	gtk_box_pack_start(GTK_BOX(vbox36), check_button.use_defaults,
			   FALSE, FALSE, 0);

	rbtnNoDefaults =
	    gtk_radio_button_new_with_label(vbox36_group,
					    _
					    ("No, use settings saved the last time I used GnomeSword"));
	vbox36_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON
				       (rbtnNoDefaults));
	gtk_widget_show(rbtnNoDefaults);
	gtk_box_pack_start(GTK_BOX(vbox36), rbtnNoDefaults, FALSE,
			   FALSE, 0);

	//frame42 = gtk_frame_new(_("Show Tabs"));
	frame42 = gtk_frame_new(NULL);

	gtk_frame_set_shadow_type(GTK_FRAME(frame42), GTK_SHADOW_NONE);

	label802 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Show Tabs"));
	gtk_label_set_markup(GTK_LABEL(label802), header);
	g_free(header);

	gtk_widget_show(label802);
	gtk_frame_set_label_widget(GTK_FRAME(frame42), label802);

	gtk_widget_show(frame42);
	gtk_box_pack_start(GTK_BOX(vbox28), frame42, TRUE, FALSE, 0);

	vbox54 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox54);
	gtk_container_add(GTK_CONTAINER(frame42), vbox54);
	gtk_container_set_border_width(GTK_CONTAINER(vbox54), 6);


	check_button.enable_tabbed_browsing =
	    gtk_check_button_new_with_label(_("Use tabbed browsing"));
	gtk_widget_show(check_button.enable_tabbed_browsing);
	gtk_box_pack_start(GTK_BOX(vbox54),
			   check_button.enable_tabbed_browsing, FALSE,
			   FALSE, 0);

	check_button.show_bible_tabs =
	    gtk_check_button_new_with_label(_("Bible texts"));
//	gtk_widget_show(check_button.show_bible_tabs);
	gtk_box_pack_start(GTK_BOX(vbox54),
			   check_button.show_bible_tabs, FALSE, FALSE,
			   0);

	check_button.show_commentary_tabs =
	    gtk_check_button_new_with_label(_("Commentary"));
//	gtk_widget_show(check_button.show_commentary_tabs);
	gtk_box_pack_start(GTK_BOX(vbox54),
			   check_button.show_commentary_tabs, FALSE,
			   FALSE, 0);

	check_button.show_dictionary_tabs =
	    gtk_check_button_new_with_label(_("Dictionary/Lexicon"));
//	gtk_widget_show(check_button.show_dictionary_tabs);
	gtk_box_pack_start(GTK_BOX(vbox54),
			   check_button.show_dictionary_tabs, FALSE,
			   FALSE, 0);

	check_button.show_book_tabs =
	    gtk_check_button_new_with_label(_("Books"));
//	gtk_widget_show(check_button.show_book_tabs);
	gtk_box_pack_start(GTK_BOX(vbox54), check_button.show_book_tabs,
			   FALSE, FALSE, 0);


	//frame72 = gtk_frame_new(_("Show"));
	frame72 = gtk_frame_new(NULL);

	gtk_frame_set_shadow_type(GTK_FRAME(frame72), GTK_SHADOW_NONE);

	label803 = gtk_label_new(NULL);
	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Show"));
	gtk_label_set_markup(GTK_LABEL(label803), header);
	g_free(header);

	gtk_widget_show(label803);

	gtk_frame_set_label_widget(GTK_FRAME(frame72), label803);

	gtk_widget_show(frame72);
	gtk_box_pack_start(GTK_BOX(vbox28), frame72, TRUE, FALSE, 0); //6);

	vbox86 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox86);
	gtk_container_add(GTK_CONTAINER(frame72), vbox86);
	gtk_container_set_border_width(GTK_CONTAINER(vbox86), 6);

	check_button.show_bible_pane =
	    gtk_check_button_new_with_label(_("Bible Texts"));
	gtk_widget_show(check_button.show_bible_pane);
	gtk_box_pack_start(GTK_BOX(vbox86),
			   check_button.show_bible_pane, FALSE, FALSE,
			   0);
	gtk_tooltips_set_tip(tooltips, check_button.show_bible_pane,
			     _("Display Bible text window"), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_bible_pane),
				     TRUE);

	check_button.show_commentary_pane =
	    gtk_check_button_new_with_label(_("Commentaries"));
	gtk_widget_show(check_button.show_commentary_pane);
	gtk_box_pack_start(GTK_BOX(vbox86),
			   check_button.show_commentary_pane, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips,
			     check_button.show_commentary_pane,
			     _
			     ("Commentaries, Personal Comments and StudyPad"),
			     NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.
				      show_commentary_pane), TRUE);

	check_button.show_lower_workbook =
	    gtk_check_button_new_with_label(_("Lower workbook"));
	gtk_widget_show(check_button.show_lower_workbook);
	gtk_box_pack_start(GTK_BOX(vbox86),
			   check_button.show_lower_workbook, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, check_button.show_lower_workbook,
			     _("Dictionary/Lexicon and generic books"),
			     NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_lower_workbook),
				     TRUE);

/* end tabs and panes page */


/* start misc page */


	vbox28 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox28);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox28);


	//frame43 = gtk_frame_new(_("General"));
	frame43 = gtk_frame_new(NULL);

	gtk_frame_set_shadow_type(GTK_FRAME(frame43), GTK_SHADOW_NONE);

	label804 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("General"));
	gtk_label_set_markup(GTK_LABEL(label804), header);
	g_free(header);

	gtk_widget_show(label804);

	gtk_frame_set_label_widget(GTK_FRAME(frame43), label804);

	gtk_widget_show(frame43);
	gtk_box_pack_start(GTK_BOX(vbox28), frame43, TRUE, FALSE, 0); //6);

	vbox55 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox55);
	gtk_container_add(GTK_CONTAINER(frame43), vbox55);
	gtk_container_set_border_width(GTK_CONTAINER(vbox55), 6);



	check_button.use_verse_style =
	    gtk_check_button_new_with_label(_("Use Verse style"));
	gtk_widget_show(check_button.use_verse_style);
	gtk_box_pack_start(GTK_BOX(vbox55),
			   check_button.use_verse_style, FALSE, FALSE,
			   0);
	gtk_widget_set_size_request(check_button.use_verse_style, 202,
				    -1);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_verse_style),
				     TRUE);

	check_button.use_default_dictionary =
	    gtk_check_button_new_with_label(_
					    ("Use default Dictionary"));
	gtk_widget_show(check_button.use_default_dictionary);
	gtk_box_pack_start(GTK_BOX(vbox55),
			   check_button.use_default_dictionary, FALSE,
			   FALSE, 0);

	check_button.show_devotion =
	    gtk_check_button_new_with_label(_
					    ("Show Daily Devotion at start up"));
	gtk_widget_show(check_button.show_devotion);
	gtk_box_pack_start(GTK_BOX(vbox55), check_button.show_devotion,
			   FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, check_button.show_devotion,
			     _
			     ("Show Daily Devotion if you have a Devotion module"),
			     NULL);

	check_button.show_splash_screen =
	    gtk_check_button_new_with_label(_
					    ("Show splash screen at start up"));
	gtk_widget_show(check_button.show_splash_screen);
	gtk_box_pack_start(GTK_BOX(vbox55),
			   check_button.show_splash_screen, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, check_button.show_splash_screen,
			     _
			     ("Show the splash screen when GnomeSword starts"),
			     NULL);


	//frame41 = gtk_frame_new(_("Where to View Dictionary/Lexicon"));
	frame41 = gtk_frame_new(NULL);

	gtk_frame_set_shadow_type(GTK_FRAME(frame41), GTK_SHADOW_NONE);

	label805 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Show Dictionary / Lexicon"));
	gtk_label_set_markup(GTK_LABEL(label805), header);
	g_free(header);

	gtk_widget_show(label805);

	gtk_frame_set_label_widget(GTK_FRAME(frame41), label805);

	gtk_widget_show(frame41);
	gtk_box_pack_start(GTK_BOX(vbox28), frame41, TRUE, FALSE, 6);

	vbox53 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox53);
	gtk_container_add(GTK_CONTAINER(frame41), vbox53);
	gtk_container_set_border_width(GTK_CONTAINER(vbox53), 6);



	check_button.show_in_viewer =
	    gtk_check_button_new_with_label(_("In Sidebar viewer"));
	gtk_widget_show(check_button.show_in_viewer);
	gtk_box_pack_start(GTK_BOX(vbox53), check_button.show_in_viewer,
			   FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_in_viewer),
				     TRUE);

	check_button.show_in_dictionary =
	    gtk_check_button_new_with_label(_
					    ("In Dictionary/Lexicon window"));
	gtk_widget_show(check_button.show_in_dictionary);
	gtk_box_pack_start(GTK_BOX(vbox53),
			   check_button.show_in_dictionary, FALSE,
			   FALSE, 0);
/*
	//frame74 = gtk_frame_new(_("Editors"));
	frame74 = gtk_frame_new(NULL);
	
	gtk_frame_set_shadow_type(GTK_FRAME(frame74),GTK_SHADOW_NONE);
  	 
  	label806 = gtk_label_new(NULL);
	
	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Editors"));
  	gtk_label_set_markup(GTK_LABEL(label806), header);
	g_free(header);
	
	
  	gtk_widget_show(label806);
  	gtk_frame_set_label_widget(GTK_FRAME(frame74),label806);

	gtk_widget_show(frame74);
	gtk_box_pack_start(GTK_BOX(vbox41), frame74, TRUE, FALSE, 6);

	vbox89 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox89);
	gtk_container_add(GTK_CONTAINER(frame74), vbox89);
	gtk_container_set_border_width(GTK_CONTAINER(vbox89), 6);



	check_button.use_studypad =
	    gtk_check_button_new_with_label(_("Use StudyPad"));
	gtk_widget_show(check_button.use_studypad);
	gtk_box_pack_start(GTK_BOX(vbox89), check_button.use_studypad,
			   FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_studypad), TRUE);

	check_button.use_studypad_dialog =
	    gtk_check_button_new_with_label(_
					    ("Open StudyPad in a dialog"));
	gtk_widget_show(check_button.use_studypad_dialog);
	gtk_box_pack_start(GTK_BOX(vbox89),
			   check_button.use_studypad_dialog, FALSE,
			   FALSE, 0);

	check_button.use_percomm_dialog =
	    gtk_check_button_new_with_label(
	    			_("Open Personal Comments editor in a dialog"));
	gtk_widget_show(check_button.use_percomm_dialog);
	gtk_box_pack_start(GTK_BOX(vbox89),
			   check_button.use_percomm_dialog, FALSE,
			   FALSE, 0);

	check_button.percomm_formatting =
	    gtk_check_button_new_with_label(_("Use formatting in Personal Comments"));
	//gtk_widget_show(check_button.percomm_formatting);
	gtk_box_pack_start(GTK_BOX(vbox89),
			   check_button.percomm_formatting, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, check_button.percomm_formatting,
			     _("Use HTML tags to format notes"), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.percomm_formatting),
				     TRUE);
*/
	hbox_default_dir = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox_default_dir);
	gtk_box_pack_start(GTK_BOX(vbox28), hbox_default_dir, TRUE,
			   TRUE, 0);

	label = gtk_label_new(NULL);
	header = g_strdup_printf("<span weight=\"bold\">%s</span>",
				 _("StudyPad default directory"));
	gtk_label_set_markup(GTK_LABEL(label), header);
	g_free(header);
	gtk_widget_show(label);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(GTK_BOX(hbox_default_dir), label, FALSE,
			   FALSE, 0);
	gtk_misc_set_padding(GTK_MISC(label), 0.01, .5);


	hbox_default_dir = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox_default_dir);
	gtk_box_pack_start(GTK_BOX(vbox28), hbox_default_dir, FALSE,
			   FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox_default_dir), 6);

	fileentry1 =
	    gnome_file_entry_new(NULL, _("StudyPad default directory"));
	gtk_widget_show(fileentry1);
	gtk_box_pack_start(GTK_BOX(hbox_default_dir), fileentry1, TRUE,
			   TRUE, 0);
	gnome_file_entry_set_directory_entry(GNOME_FILE_ENTRY
					     (fileentry1), TRUE);

	entry.combo_entry_sp_dir =
	    gnome_file_entry_gtk_entry(GNOME_FILE_ENTRY(fileentry1));
	gtk_widget_show(entry.combo_entry_sp_dir);


/* end misc page */


/* start module page */

	vbox = gtk_vbox_new(FALSE, 6);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 13);

	label = gtk_label_new(_("<b>Main Window Modules</b>"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Biblical Text"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo17 = gtk_combo_new();
	gtk_widget_show(combo17);
	gtk_box_pack_start(GTK_BOX(hbox), combo17, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo17, 209, -1);

	entry.text_module = GTK_COMBO(combo17)->entry;
	gtk_widget_show(entry.text_module);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Commentary"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo23 = gtk_combo_new();
	gtk_widget_show(combo23);
	gtk_box_pack_start(GTK_BOX(hbox), combo23, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo23, 209, -1);

	entry.commentary_module = GTK_COMBO(combo23)->entry;
	gtk_widget_show(entry.commentary_module);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Dictionary"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo24 = gtk_combo_new();
	gtk_widget_show(combo24);
	gtk_box_pack_start(GTK_BOX(hbox), combo24, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo24, 209, -1);

	entry.dictionary_module = GTK_COMBO(combo24)->entry;
	gtk_widget_show(entry.dictionary_module);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Default Dictionary"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	comboDefaultDict = gtk_combo_new();
	gtk_widget_show(comboDefaultDict);
	gtk_box_pack_start(GTK_BOX(hbox), comboDefaultDict, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(comboDefaultDict, 209, -1);

	entry.default_dictionary_module =
	    GTK_COMBO(comboDefaultDict)->entry;
	gtk_widget_show(entry.default_dictionary_module);


	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Personal Notes"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo25 = gtk_combo_new();
	gtk_widget_show(combo25);
	gtk_box_pack_start(GTK_BOX(hbox), combo25, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo25, 209, -1);

	entry.percomm_module = GTK_COMBO(combo25)->entry;
	gtk_widget_show(entry.percomm_module);

/* end module page */


/* start parallel module page */
	vbox = gtk_vbox_new(FALSE, 6);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 13);


	label = gtk_label_new(_("<b>Parallel View</b>"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Parallel 1"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo18 = gtk_combo_new();
	gtk_widget_show(combo18);
	gtk_box_pack_start(GTK_BOX(hbox), combo18, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo18, 209, -1);

	entry.parallel_1_module = GTK_COMBO(combo18)->entry;
	gtk_widget_show(entry.parallel_1_module);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Parallel 2"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo19 = gtk_combo_new();
	gtk_widget_show(combo19);
	gtk_box_pack_start(GTK_BOX(hbox), combo19, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo19, 209, -1);

	entry.parallel_2_module = GTK_COMBO(combo19)->entry;
	gtk_widget_show(entry.parallel_2_module);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Parallel 3"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo20 = gtk_combo_new();
	gtk_widget_show(combo20);
	gtk_box_pack_start(GTK_BOX(hbox), combo20, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo20, 209, -1);

	entry.parallel_3_module = GTK_COMBO(combo20)->entry;
	gtk_widget_show(entry.parallel_3_module);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Parallel 4"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo21 = gtk_combo_new();
	gtk_widget_show(combo21);
	gtk_box_pack_start(GTK_BOX(hbox), combo21, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo21, 209, -1);

	entry.parallel_4_module = GTK_COMBO(combo21)->entry;
	gtk_widget_show(entry.parallel_4_module);


	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Parallel 5"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo22 = gtk_combo_new();
	gtk_widget_show(combo22);
	gtk_box_pack_start(GTK_BOX(hbox), combo22, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo22, 209, -1);

	entry.parallel_5_module = GTK_COMBO(combo22)->entry;
	gtk_widget_show(entry.parallel_5_module);



/* end parallel module page */



/* start misc module page */
	vbox = gtk_vbox_new(FALSE, 6);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(notebook7), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 13);


	label = gtk_label_new(_("<b>Miscellaneous Modules</b>"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Daily Devotional"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	comboDevotion = gtk_combo_new();
	gtk_widget_show(comboDevotion);
	gtk_box_pack_start(GTK_BOX(hbox), comboDevotion, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(comboDevotion, 209, -1);

	entry.devotion_module = GTK_COMBO(comboDevotion)->entry;
	gtk_widget_show(entry.devotion_module);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Hebrew Lexicon"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo27 = gtk_combo_new();
	gtk_widget_show(combo27);
	gtk_box_pack_start(GTK_BOX(hbox), combo27, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo27, 209, -1);

	entry.hebrew_lex__module = GTK_COMBO(combo27)->entry;
	gtk_widget_show(entry.hebrew_lex__module);
	gtk_tooltips_set_tip(tooltips, entry.hebrew_lex__module,
			     _
			     ("Which Greek Lexicon to display in Dictionary/Lexicon window when a link or word is clicked"),
			     NULL);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Hebrew Lexicon Viewer"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	comboHebViewer = gtk_combo_new();
	gtk_widget_show(comboHebViewer);
	gtk_box_pack_start(GTK_BOX(hbox), comboHebViewer, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(comboHebViewer, 209, -1);

	entry.hebrew_lex_viewer_module =
	    GTK_COMBO(comboHebViewer)->entry;
	gtk_widget_show(entry.hebrew_lex_viewer_module);
	gtk_tooltips_set_tip(tooltips, entry.hebrew_lex_viewer_module,
			     _
			     ("Which Hebrew Lexicon to display in viewer when a link or word is clicked"),
			     NULL);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Greek Lexicon"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	combo26 = gtk_combo_new();
	gtk_widget_show(combo26);
	gtk_box_pack_start(GTK_BOX(hbox), combo26, FALSE, TRUE, 0);
	gtk_widget_set_size_request(combo26, 209, -1);

	entry.greek_lex__module = GTK_COMBO(combo26)->entry;
	gtk_widget_show(entry.greek_lex__module);
	gtk_tooltips_set_tip(tooltips, entry.greek_lex__module,
		_("Which Greek Lexicon to display in Dictionary/Lexicon window when a link or word is clicked"),
			     NULL);


	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new(_("Greek Lexicon Viewer"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_misc_set_padding(GTK_MISC(label), 12, 0);

	comboGreekViewer = gtk_combo_new();
	gtk_widget_show(comboGreekViewer);
	gtk_box_pack_start(GTK_BOX(hbox), comboGreekViewer, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(comboGreekViewer, 209, -1);

	entry.greek_lex_viewer_module =
	    GTK_COMBO(comboGreekViewer)->entry;
	gtk_widget_show(entry.greek_lex_viewer_module);
	gtk_tooltips_set_tip(tooltips, entry.greek_lex_viewer_module,
			     _
			     ("Which Greek Lexicon to display in viewer when a link or word is clicked"),
			     NULL);
/* end misc module page */



	/*  */

	dialog_action_area24 = GTK_DIALOG(dialog_prefs)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog_prefs),
			    "dialog_action_area24",
			    dialog_action_area24);
	gtk_widget_show(dialog_action_area24);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area24), 10);

	hbuttonbox3 = gtk_hbutton_box_new();
	gtk_widget_show(hbuttonbox3);
	gtk_box_pack_start(GTK_BOX(dialog_action_area24), hbuttonbox3,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox3),
				  GTK_BUTTONBOX_END);
	gtk_box_set_spacing(GTK_BOX(hbuttonbox3), 10);

	gtk_dialog_add_button(GTK_DIALOG(dialog_prefs),
			      GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);

/**************************************************** start settings */

	/** add module list to combo boxs **/
	if (biblelist) {
		gtk_combo_set_popdown_strings(GTK_COMBO(combo17),
					      biblelist);
		gtk_combo_set_popdown_strings(GTK_COMBO(combo18),
					      biblelist);
		gtk_combo_set_popdown_strings(GTK_COMBO(combo19),
					      biblelist);
		gtk_combo_set_popdown_strings(GTK_COMBO(combo20),
					      biblelist);
		gtk_combo_set_popdown_strings(GTK_COMBO(combo21),
					      biblelist);
		gtk_combo_set_popdown_strings(GTK_COMBO(combo22),
					      biblelist);
	}
	if (commlist)
		gtk_combo_set_popdown_strings(GTK_COMBO(combo23),
					      commlist);
	if (dictlist) {
		gtk_combo_set_popdown_strings(GTK_COMBO(combo24),
					      dictlist);
		gtk_combo_set_popdown_strings(GTK_COMBO
					      (comboDefaultDict),
					      dictlist);
		gtk_combo_set_popdown_strings(GTK_COMBO
					      (comboGreekViewer),
					      dictlist);
		gtk_combo_set_popdown_strings(GTK_COMBO(combo26),
					      dictlist);
		gtk_combo_set_popdown_strings(GTK_COMBO(comboHebViewer),
					      dictlist);
		gtk_combo_set_popdown_strings(GTK_COMBO(combo27),
					      dictlist);
	}
	if (percomlist)
		gtk_combo_set_popdown_strings(GTK_COMBO(combo25),
					      percomlist);
	if (devotionlist)
		gtk_combo_set_popdown_strings(GTK_COMBO(comboDevotion),
					      devotionlist);

	gtk_entry_set_text(GTK_ENTRY(entry.verse_number_size),
			   settings.verse_num_font_size);

	/* set toggle buttons */
	if (settings.usedefault)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (check_button.
					      use_defaults), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (rbtnNoDefaults), TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_bible_tabs),
				     settings.text_tabs);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.
				      show_commentary_tabs),
				     settings.comm_tabs);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.
				      show_dictionary_tabs),
				     settings.dict_tabs);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_book_tabs),
				     settings.book_tabs);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_verse_style),
				     settings.versestyle);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_devotion),
				     settings.showdevotional);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_bible_pane),
				     settings.showtexts);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.
				      show_commentary_pane),
				     settings.showcomms);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_lower_workbook),
				     settings.showdicts);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.
				      enable_tabbed_browsing),
				     settings.browsing);
	/*
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
	   (check_button.use_studypad),
	   settings.use_studypad);
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
	   (check_button.use_studypad_dialog),
	   settings.use_studypad_dialog);
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
	   (check_button.use_percomm_dialog),
	   settings.use_percomm_dialog);
	 */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_in_viewer),
				     settings.inViewer);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_in_dictionary),
				     settings.inDictpane);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.
				      use_default_dictionary),
				     settings.useDefaultDict);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_splash_screen),
				     settings.showsplash);

	if (settings.MainWindowModule)
		gtk_entry_set_text(GTK_ENTRY(entry.text_module),
				   settings.MainWindowModule);
	if (settings.parallel1Module)
		gtk_entry_set_text(GTK_ENTRY(entry.parallel_1_module),
				   settings.parallel1Module);
	if (settings.parallel2Module)
		gtk_entry_set_text(GTK_ENTRY(entry.parallel_2_module),
				   settings.parallel2Module);
	if (settings.parallel3Module)
		gtk_entry_set_text(GTK_ENTRY(entry.parallel_3_module),
				   settings.parallel3Module);
	if (settings.parallel4Module)
		gtk_entry_set_text(GTK_ENTRY(entry.parallel_4_module),
				   settings.parallel4Module);
	if (settings.parallel5Module)
		gtk_entry_set_text(GTK_ENTRY(entry.parallel_5_module),
				   settings.parallel5Module);

	if (settings.CommWindowModule)
		gtk_entry_set_text(GTK_ENTRY(entry.commentary_module),
				   settings.CommWindowModule);
	if (settings.DictWindowModule)
		gtk_entry_set_text(GTK_ENTRY(entry.dictionary_module),
				   settings.DictWindowModule);
	if (settings.DefaultDict)
		gtk_entry_set_text(GTK_ENTRY
				   (entry.default_dictionary_module),
				   settings.DefaultDict);
	if (settings.lex_greek_viewer)
		gtk_entry_set_text(GTK_ENTRY
				   (entry.greek_lex_viewer_module),
				   settings.lex_greek_viewer);
	if (settings.lex_hebrew_viewer)
		gtk_entry_set_text(GTK_ENTRY
				   (entry.hebrew_lex_viewer_module),
				   settings.lex_hebrew_viewer);
	if (settings.personalcommentsmod)
		gtk_entry_set_text(GTK_ENTRY(entry.percomm_module),
				   settings.personalcommentsmod);
	if (settings.lex_greek)
		gtk_entry_set_text(GTK_ENTRY(entry.greek_lex__module),
				   settings.lex_greek);
	if (settings.lex_hebrew)
		gtk_entry_set_text(GTK_ENTRY(entry.hebrew_lex__module),
				   settings.lex_hebrew);
	if (settings.devotionalmod)
		gtk_entry_set_text(GTK_ENTRY(entry.devotion_module),
				   settings.devotionalmod);
	//if(settings.) 
	gtk_entry_set_text(GTK_ENTRY(entry.combo_entry_sp_dir),
			   settings.studypaddir);

	/********************************************* end settings */

	g_signal_connect((gpointer) dialog_prefs, "response",
			 G_CALLBACK(on_dialog_response), NULL);
	/*** color pickers ***/
	g_signal_connect(GTK_OBJECT(color_picker.text_background),
			 "color_set",
			 G_CALLBACK(on_colorpicker_color_set),
			 GINT_TO_POINTER(TEXT_BACKGROUND));
	g_signal_connect(GTK_OBJECT(color_picker.text), "color_set",
			 G_CALLBACK(on_colorpicker_color_set),
			 GINT_TO_POINTER(TEXT_FORGROUND));
	/*g_signal_connect(GTK_OBJECT(gcpCurrentverseBG), "color_set",
	   G_CALLBACK(on_colorpicker_color_set), NULL); */
	g_signal_connect(GTK_OBJECT(color_picker.text_current_verse),
			 "color_set",
			 G_CALLBACK(on_colorpicker_color_set),
			 GINT_TO_POINTER(TEXT_CURRENT_VERSE));
	g_signal_connect(GTK_OBJECT(color_picker.verse_numbers),
			 "color_set",
			 G_CALLBACK(on_colorpicker_color_set),
			 GINT_TO_POINTER(VERSE_NUMBERS));
	g_signal_connect(GTK_OBJECT(color_picker.href_links),
			 "color_set",
			 G_CALLBACK(on_colorpicker_color_set),
			 GINT_TO_POINTER(HREF_LINKS));
	/*** combo entrys  font sizes ***/
	g_signal_connect(GTK_OBJECT(entry.verse_number_size),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(VERSE_NUMBER_SIZE));

	/*** toggle buttons ***//*
	   g_signal_connect(GTK_OBJECT(check_button.use_defaults),
	   "toggled",
	   G_CALLBACK(on_button_toggled),
	   GINT_TO_POINTER(0));
	   g_signal_connect(GTK_OBJECT(rbtnNoDefaults), "toggled",
	   G_CALLBACK(on_button_toggled),
	   GINT_TO_POINTER(0)); */
	/*
	   g_signal_connect(GTK_OBJECT(check_button.show_shortcut_bar),
	   "toggled",
	   G_CALLBACK(on_button_toggled),
	   GINT_TO_POINTER(2));
	 */
	g_signal_connect(GTK_OBJECT(check_button.show_bible_tabs),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_BIBLE_TABS));
	g_signal_connect(GTK_OBJECT
			 (check_button.show_commentary_tabs),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_COMMENTARY_TABS));
	g_signal_connect(GTK_OBJECT
			 (check_button.show_dictionary_tabs),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_DICTIONARY_TABS));
	g_signal_connect(GTK_OBJECT(check_button.show_book_tabs),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_BOOK_TABS));
	g_signal_connect(GTK_OBJECT(check_button.show_in_viewer),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_IN_VIEWER));
	g_signal_connect(GTK_OBJECT(check_button.show_in_dictionary),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_IN_DICTIONARY));

	g_signal_connect(GTK_OBJECT
			 (check_button.use_default_dictionary),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(USE_DEFAULT_DICTIONARY));
	g_signal_connect(GTK_OBJECT
			 (check_button.show_splash_screen),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_SPLASH_SCREEN));
	/*
	   g_signal_connect(GTK_OBJECT(check_button.percomm_formatting),
	   "toggled",
	   G_CALLBACK(on_button_toggled),
	   GINT_TO_POINTER());
	 */
	/*
	   g_signal_connect(GTK_OBJECT(check_button.dock_shortcut_bar),
	   "toggled", G_CALLBACK(on_dock_clicked),
	   NULL);
	 */
	g_signal_connect(GTK_OBJECT(check_button.use_verse_style),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(USE_VERSE_STYLE));
	g_signal_connect(GTK_OBJECT(check_button.show_devotion),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_DEVOTION));
	g_signal_connect(GTK_OBJECT
			 (check_button.enable_tabbed_browsing),
			 "toggled", G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(TABBED_BROWSING));

	g_signal_connect(GTK_OBJECT(check_button.show_bible_pane),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_BIBLE_PANE));
	g_signal_connect(GTK_OBJECT
			 (check_button.show_commentary_pane),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_COMMENTARY_PANE));
	g_signal_connect(GTK_OBJECT(check_button.show_lower_workbook),
			 "toggled",
			 G_CALLBACK(on_button_toggled),
			 GINT_TO_POINTER(SHOW_LOWER_WORKBOOK));
/*
	g_signal_connect(GTK_OBJECT(check_button.use_studypad),
			   "toggled",
			   G_CALLBACK(on_button_toggled), 
			   GINT_TO_POINTER(USE_STUDYPAD));
	g_signal_connect(GTK_OBJECT(check_button.use_studypad_dialog),
			   "toggled",
			   G_CALLBACK(on_button_toggled), 
			   GINT_TO_POINTER(USE_STUDYPAD_DIALOG));
	g_signal_connect(GTK_OBJECT(check_button.use_percomm_dialog),
			   "toggled",
			   G_CALLBACK(on_button_toggled), 
			   GINT_TO_POINTER(USE_PERCOMM_DIALOG));
*/
	/*** module combos ***/
	g_signal_connect(GTK_OBJECT(entry.text_module), "changed",
			 G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(TEXT_MODULE));
	g_signal_connect(GTK_OBJECT(entry.parallel_1_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(PARALLEL_1_MODULE));
	g_signal_connect(GTK_OBJECT(entry.parallel_2_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(PARALLEL_2_MODULE));
	g_signal_connect(GTK_OBJECT(entry.parallel_3_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(PARALLEL_3_MODULE));
	g_signal_connect(GTK_OBJECT(entry.parallel_4_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(PARALLEL_4_MODULE));
	g_signal_connect(GTK_OBJECT(entry.parallel_5_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(PARALLEL_5_MODULE));
	g_signal_connect(GTK_OBJECT(entry.commentary_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(COMMENTARY_MODULE));
	g_signal_connect(GTK_OBJECT(entry.dictionary_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(DICTIONARY_MODULE));
	g_signal_connect(GTK_OBJECT(entry.percomm_module), "changed",
			 G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(PERCOMM_MODULE));
	g_signal_connect(GTK_OBJECT(entry.greek_lex__module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(GREEK_LEX__MODULE));
	g_signal_connect(GTK_OBJECT(entry.hebrew_lex__module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(HEBREW_LEX__MODULE));
	g_signal_connect(GTK_OBJECT(entry.default_dictionary_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(DEFAULT_DICTIONARY_MODULE));
	g_signal_connect(GTK_OBJECT(entry.hebrew_lex_viewer_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(HEBREW_LEX_VIEWER_MODULE));
	g_signal_connect(GTK_OBJECT(entry.greek_lex_viewer_module),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(GREEK_LEX_VIEWER_MODULE));
	g_signal_connect(GTK_OBJECT(entry.devotion_module), "changed",
			 G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(DEVOTION_MODULE));
	g_signal_connect(GTK_OBJECT(entry.combo_entry_sp_dir),
			 "changed", G_CALLBACK(on_entry_changed),
			 GINT_TO_POINTER(COMBO_ENTRY_SP_DIR));

	/** set color pickers to current colors **/
	setcolorpickersColor(color_picker.text_background,
			     color_picker.text,
			     color_picker.text_current_verse,
			     color_picker.verse_numbers,
			     color_picker.href_links);

	g_signal_connect(selection, "changed",
			 G_CALLBACK(tree_selection_changed), model);

	return dialog_prefs;
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
	GList *text_modules = NULL;
	GList *comm_modules = NULL;
	GList *dict_modules = NULL;
	GList *book_modules = NULL;
	GList *percom_modules = NULL;
	GList *devotion_modules = NULL;

	text_modules = get_list(TEXT_LIST);
	comm_modules = get_list(COMM_LIST);
	dict_modules = get_list(DICT_LIST);
	book_modules = get_list(GBS_LIST);
	devotion_modules = get_list(DEVOTION_LIST);
	percom_modules = get_list(PERCOMM_LIST);

	/* create preferences dialog */
	dlg = gui_create_preferences_dialog(text_modules, comm_modules,
					    dict_modules,
					    percom_modules,
					    devotion_modules);
	gtk_widget_show(dlg);
}
