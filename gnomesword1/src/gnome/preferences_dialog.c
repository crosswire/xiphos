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
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>

#include <gnome.h>
#include <gtk/gtk.h>

#include "gui/bibletext.h"
#include "gui/gnomesword.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_dialog.h"
#include "gui/preferences_dialog.h"
#include "gui/utilities.h"
#include "gui/gbs.h"
#include "gui/commentary.h"
#include "gui/dictlex.h"
#include "gui/interlinear.h"
#include "gui/main_window.h"

#include "main/sword.h"
#include "main/lists.h"
#include "main/settings.h"



typedef struct _preferences_entry ENTRY;
struct _preferences_entry {
	GtkWidget *text_module;	/*  Main Window Module  */
	GtkWidget *interlinear_1_module;
	GtkWidget *interlinear_2_module;
	GtkWidget *interlinear_3_module;
	GtkWidget *interlinear_4_module;
	GtkWidget *interlinear_5_module;
	GtkWidget *commentary_module;
	GtkWidget *dictionary_module;
	GtkWidget *default_dictionary_module;
	GtkWidget *percomm_module;
	GtkWidget *devotion_module;
	GtkWidget *greek_lex__module;
	GtkWidget *hebrew_lex__module;
	GtkWidget *greek_lex_viewer_module;
	GtkWidget *hebrew_lex_viewer_module;

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

	GtkWidget *show_shortcut_bar;
	GtkWidget *show_bible_tabs;
	GtkWidget *show_commentary_tabs;
	GtkWidget *show_dictionary_tabs;
	GtkWidget *show_book_tabs;

	GtkWidget *use_default_dictionary;
	GtkWidget *use_verse_style;

	GtkWidget *show_bible_pane;
	GtkWidget *show_commentary_pane;
	GtkWidget *show_lower_workbook;

	GtkWidget *show_favorites;
	GtkWidget *show_text_group;
	GtkWidget *show_commentary_group;
	GtkWidget *show_dictionary_group;
	GtkWidget *show_book_group;
	GtkWidget *show_history_group;
	GtkWidget *dock_shortcut_bar;

	GtkWidget *show_in_viewer;
	GtkWidget *show_in_dictionary;
	GtkWidget *show_devotion;
	GtkWidget *percomm_formatting;
	GtkWidget *use_studypad;
	GtkWidget *use_studypad_dialog;
	GtkWidget *use_percomm_dialog;
};


typedef struct _preferences_spin_buttons SPIN_BUTTONS;
struct _preferences_spin_buttons {
	GtkWidget *app_width;
	GtkWidget *app_height;
	GtkWidget *shortcut_bar_width;
	GtkWidget *bible_pane_width;
	GtkWidget *bible_pane_height;
};


typedef struct _preferences_buttons BUTTONS;
struct _preferences_buttons {
	GtkWidget *gnomesword_defaults;
	GtkWidget *ok;
	GtkWidget *apply;
};


/*****************************************************************************
 * externs
 */
extern gchar *tmpcolor;

/******************************************************************************
 * static - visible to this file only
 */
static gboolean updatehtml, updateSB, updatelayout;
static EShortcutModel *shortcut_model;
static GtkWidget *notebook7;
static GtkWidget *dlg;
static ENTRY entry;
static COLOR_PICKERS color_picker;
static CHECK_BUTTONS check_button;
static SPIN_BUTTONS spin_button;
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
	//g_warning("string_is_color, %s is color\n", color);
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
 *   add_sb_group2
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   gint add_sb_group2(EShortcutBar * shortcut_bar,
					gchar * group_name)	
 *
 * Description
 *   add group to perferences dialog shourtcut bar
 *
 * Return value
 *   gint
 */

static gint add_sb_group2(EShortcutBar * shortcut_bar,
			  gchar * group_name)
{
	gint group_num;

	group_num =
	    e_shortcut_model_add_group(shortcut_bar->model, -1,
				       group_name);
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_SMALL_ICONS);
	return group_num;
}

/******************************************************************************
 * Name
 *   on_shortcut_bar_item_selected1
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_shortcut_bar_item_selected1(EShortcutBar * shortcut_bar,
			GdkEvent * event, gint group_num, gint item_num)	
 *
 * Description
 *   display notebook page for item clicked
 *
 * Return value
 *   void
 */

static void on_shortcut_bar_item_selected1(EShortcutBar * shortcut_bar,
					   GdkEvent * event,
					   gint group_num,
					   gint item_num)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook7), item_num);
}

/******************************************************************************
 * Name
 *   applyoptions
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void applyoptions(void)	
 *
 * Description
 *   apply changes
 *
 * Return value
 *   void
 */

static void applyoptions(void)
{
	GtkWidget *dict;
	GtkWidget *comm;

	dict = widgets.notebook_dict;
	comm = widgets.notebook_comm;

	/*  */
	if (updatelayout) {
		/* set the main window size */
		gtk_widget_set_usize(widgets.app, settings.gs_width,
				     settings.gs_hight);

		if (settings.showshortcutbar && settings.docked) {
			e_paned_set_position(E_PANED(widgets.epaned),
					     settings.
					     shortcutbar_width);
		} else {
			e_paned_set_position(E_PANED(widgets.epaned),
					     1);
		}
		updatelayout = FALSE;
	}
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_text),
				   settings.text_tabs);
	gui_set_text_frame_label();

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_gbs),
				   settings.book_tabs);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_dict),
				   settings.dict_tabs);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_comm),
				   settings.comm_tabs);

	GTK_CHECK_MENU_ITEM(widgets.versestyle_item)->active =
	    settings.versestyle;
	GTK_CHECK_MENU_ITEM(widgets.viewtexts_item)->active =
	    settings.showtexts;
	GTK_CHECK_MENU_ITEM(widgets.viewcomms_item)->active =
	    settings.showcomms;
	GTK_CHECK_MENU_ITEM(widgets.viewdicts_item)->active =
	    settings.showdicts;

	if (updatehtml) {
		gui_display_text(settings.currentverse);
		gui_display_commentary(settings.currentverse);
		gui_display_dictlex(settings.dictkey);
		gui_update_interlinear_page();
		//display_new_font_color_and_size();
	}

	if (updateSB) {
		gui_update_shortcut_bar();
	}

	gui_set_bible_comm_layout();
	updatehtml = FALSE;
	updateSB = FALSE;
}

/******************************************************************************
 * Name
 *   on_dock_clicked
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_dock_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   attach or detach shortcut bar
 *
 * Return value
 *   void
 */

static void on_dock_clicked(GtkButton * button, gpointer user_data)
{
	gui_attach_detach_shortcutbar();
}

/******************************************************************************
 * Name
 *   get_preferences_from_dlg
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void get_preferences_from_dlg(GtkWidget * d)	
 *
 * Description
 *   read all preferences dialog options
 *
 * Return value
 *   void
 */

static void get_preferences_from_dlg(GtkWidget * d)
{
	/************************************************************
	we need to read all propertybox options here
	*************************************************************/
	gchar *buf;
	gdouble color[4];

	/*** read modules ***/
	buf = gtk_entry_get_text(GTK_ENTRY(entry.text_module));
	sprintf(settings.MainWindowModule, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.interlinear_1_module));
	sprintf(settings.Interlinear1Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.interlinear_2_module));
	sprintf(settings.Interlinear2Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.interlinear_3_module));
	sprintf(settings.Interlinear3Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.interlinear_4_module));
	sprintf(settings.Interlinear4Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.interlinear_5_module));
	sprintf(settings.Interlinear5Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.commentary_module));
	sprintf(settings.CommWindowModule, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.dictionary_module));
	sprintf(settings.DictWindowModule, "%s", buf);
	buf =
	    gtk_entry_get_text(GTK_ENTRY
			       (entry.default_dictionary_module));
	sprintf(settings.DefaultDict, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.percomm_module));
	sprintf(settings.personalcommentsmod, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.devotion_module));
	sprintf(settings.devotionalmod, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.greek_lex__module));
	sprintf(settings.lex_greek, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(entry.hebrew_lex__module));
	sprintf(settings.lex_hebrew, "%s", buf);
	buf =
	    gtk_entry_get_text(GTK_ENTRY
			       (entry.greek_lex_viewer_module));
	sprintf(settings.lex_greek_viewer, "%s", buf);
	buf =
	    gtk_entry_get_text(GTK_ENTRY
			       (entry.hebrew_lex_viewer_module));
	sprintf(settings.lex_hebrew_viewer, "%s", buf);


	/*** read html colors ***/
	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (color_picker.text),
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(settings.bible_text_color, "%s", buf);
	g_free(buf);

	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (color_picker.text_background),
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(settings.bible_bg_color, "%s", buf);
	g_free(buf);

	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (color_picker.text_current_verse),
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(settings.currentverse_color, "%s", buf);
	g_free(buf);

	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (color_picker.verse_numbers),
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(settings.bible_verse_num_color, "%s", buf);
	g_free(buf);

	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (color_picker.href_links),
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(settings.link_color, "%s", buf);
	g_free(buf);

	/*** read font sizes ***/
	buf = gtk_entry_get_text(GTK_ENTRY(entry.verse_number_size));
	sprintf(settings.verse_num_font_size, "%s", buf);

	/*** read radio buttons ***/
	settings.usedefault =
	    GTK_TOGGLE_BUTTON(check_button.use_defaults)->active;

	/*** read check buttons ***/
	settings.showshortcutbar =
	    GTK_TOGGLE_BUTTON(check_button.show_shortcut_bar)->active;
	settings.text_tabs =
	    GTK_TOGGLE_BUTTON(check_button.show_bible_tabs)->active;
	settings.comm_tabs =
	    GTK_TOGGLE_BUTTON(check_button.show_commentary_tabs)->
	    active;
	settings.dict_tabs =
	    GTK_TOGGLE_BUTTON(check_button.show_dictionary_tabs)->
	    active;
	settings.book_tabs =
	    GTK_TOGGLE_BUTTON(check_button.show_book_tabs)->active;
	settings.useDefaultDict =
	    GTK_TOGGLE_BUTTON(check_button.use_default_dictionary)->
	    active;
	settings.versestyle =
	    GTK_TOGGLE_BUTTON(check_button.use_verse_style)->active;

	settings.showtexts =
	    GTK_TOGGLE_BUTTON(check_button.show_bible_pane)->active;
	settings.showcomms =
	    GTK_TOGGLE_BUTTON(check_button.show_commentary_pane)->
	    active;
	settings.showdicts =
	    GTK_TOGGLE_BUTTON(check_button.show_lower_workbook)->active;

	settings.showfavoritesgroup =
	    GTK_TOGGLE_BUTTON(check_button.show_favorites)->active;
	settings.showtextgroup =
	    GTK_TOGGLE_BUTTON(check_button.show_text_group)->active;
	settings.showcomgroup =
	    GTK_TOGGLE_BUTTON(check_button.show_commentary_group)->
	    active;
	settings.showdictgroup =
	    GTK_TOGGLE_BUTTON(check_button.show_dictionary_group)->
	    active;
	settings.showbookgroup =
	    GTK_TOGGLE_BUTTON(check_button.show_book_group)->active;
	settings.showhistorygroup =
	    GTK_TOGGLE_BUTTON(check_button.show_history_group)->active;
	settings.docked =
	    GTK_TOGGLE_BUTTON(check_button.dock_shortcut_bar)->active;

	settings.inViewer =
	    GTK_TOGGLE_BUTTON(check_button.show_in_viewer)->active;
	settings.inDictpane =
	    GTK_TOGGLE_BUTTON(check_button.show_in_dictionary)->active;
	settings.showdevotional =
	    GTK_TOGGLE_BUTTON(check_button.show_devotion)->active;

	settings.formatpercom =
	    GTK_TOGGLE_BUTTON(check_button.percomm_formatting)->active;
	settings.use_studypad =
	    GTK_TOGGLE_BUTTON(check_button.use_studypad)->active;
	settings.use_studypad_dialog =
	    GTK_TOGGLE_BUTTON(check_button.use_studypad_dialog)->active;
	settings.use_percomm_dialog =
	    GTK_TOGGLE_BUTTON(check_button.use_percomm_dialog)->active;

	/*** read layout spin buttons ***/
	settings.gs_width =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (spin_button.app_width));
	settings.gs_hight =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (spin_button.app_height));
	settings.shortcutbar_width =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (spin_button.
					      shortcut_bar_width));
	settings.biblepane_width =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (spin_button.
					      bible_pane_width));
	settings.upperpane_hight =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (spin_button.
					      bible_pane_height));

	save_properties(FALSE);
	applyoptions();

}

/******************************************************************************
 * Name
 *   on_btnPropertyboxOK_clicked
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_btnPropertyboxOK_clicked(GtkButton * button,
 *					gpointer user_data)	
 *
 * Description
 *   apply user's changes and close dialog
 *
 * Return value
 *   void
 */

static void on_btnPropertyboxOK_clicked(GtkButton * button,
					gpointer user_data)
{
	get_preferences_from_dlg(dlg);
	gtk_widget_destroy(dlg);
}

/******************************************************************************
 * Name
 *   on_btnPropertyboxApply_clicked
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_btnPropertyboxApply_clicked(GtkButton * button,
 *					   gpointer user_data)	
 *
 * Description
 *   apply user's changes
 *
 * Return value
 *   void
 */

static void on_btnPropertyboxApply_clicked(GtkButton * button,
					   gpointer user_data)
{
	get_preferences_from_dlg(dlg);
}

/******************************************************************************
 * Name
 *   on_btnPropertyboxCancel_clicked
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_btnPropertyboxCancel_clicked(GtkButton * button,
 *						gpointer user_data)	
 *
 * Description
 *   cancel button clicked - close dialog and do nothing else
 *
 * Return value
 *   void
 */

static void on_btnPropertyboxCancel_clicked(GtkButton * button,
					    gpointer user_data)
{
	gtk_widget_destroy(dlg);
}

/******************************************************************************
 * Name
 *   set_buttons_sensitive
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void set_buttons_sensitive(GtkWidget * widget)	
 *
 * Description
 *   set ok and apply buttons to sensitive
 *
 * Return value
 *   void
 */

static void set_buttons_sensitive(GtkWidget * widget,
				  gboolean sensitive)
{
	gtk_widget_set_sensitive(button.ok, sensitive);
	gtk_widget_set_sensitive(button.apply, sensitive);
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
	set_buttons_sensitive(GTK_WIDGET(gnomecolorpicker), TRUE);
	updatehtml = TRUE;
}

/******************************************************************************
 * Name
 *   on_Entry_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_Entry_changed(GtkEditable * editable, gpointer user_data)	
 *
 * Description
 *   a combo entry has changed
 *   set ok - apply button sensitive
 *   set update html to true
 *
 * Return value
 *  void 
 */

static void on_Entry_changed(GtkEditable * editable, gpointer user_data)
{
	set_buttons_sensitive(GTK_WIDGET(editable), TRUE);
	updatehtml = TRUE;
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
	set_buttons_sensitive(NULL, TRUE);
	switch (GPOINTER_TO_INT(user_data)) {
	case 0:
		/*  */
		break;
	case 1:
		updateSB = TRUE;
		break;
	case 2:
		updatelayout = TRUE;
		break;
	}
}

/******************************************************************************
 * Name
 *   on_spinbutton_changed
 *
 * Synopsis
 *   #include "preferences_dialog.h"
 *
 *   void on_spinbutton_changed(GtkEditable * editable,
					gpointer user_data)	
 *
 * Description
 *   one of the spin button has changed
 *   set ok - apply button sensitive
 *   set updatelayout to true
 *
 * Return value
 *   void
 */

static void on_spinbutton_changed(GtkEditable * editable,
				  gpointer user_data)
{
	set_buttons_sensitive(GTK_WIDGET(editable), TRUE);
	updatelayout = TRUE;
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
	}

	if (string_is_color(settings.bible_text_color)) {
		color = hex_to_gdouble_arr(settings.bible_text_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER(gcpText),
					  color[0], color[1], color[2],
					  a);
	}

	if (string_is_color(settings.currentverse_color)) {
		color = hex_to_gdouble_arr(settings.currentverse_color);
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
	}

	if (string_is_color(settings.link_color)) {
		color = hex_to_gdouble_arr(settings.link_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextLinks), color[0],
					  color[1], color[2], a);
	}

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
	gchar *pathname;
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
	GtkWidget *gcpCurrentverseBG;
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
	GtkTooltips *tooltips;


	tooltips = gtk_tooltips_new();
  /*************************************************************/
	sbtnAppWidth_adj =
	    gtk_adjustment_new(settings.gs_width, 640, 10000, 1, 10,
			       10);
	sbtnAppHight_adj =
	    gtk_adjustment_new(settings.gs_hight, 480, 10000, 1, 10,
			       10);
	sbtnSBWidth_adj =
	    gtk_adjustment_new(settings.shortcutbar_width, 0, 300, 1,
			       10, 10);
	sbtnTextWidth_adj =
	    gtk_adjustment_new(settings.biblepane_width, 0, 500, 1, 10,
			       10);
	sbtnUpPaneHight_adj =
	    gtk_adjustment_new(settings.upperpane_hight, 0, 10000, 1,
			       10, 10);
  /*************************************************************/


	dialog_prefs = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog_prefs), "dialog_prefs",
			    dialog_prefs);
	gtk_window_set_title(GTK_WINDOW(dialog_prefs), _("dialog3"));
	gtk_window_set_default_size(GTK_WINDOW(dialog_prefs), 611, 408);
	gtk_window_set_policy(GTK_WINDOW(dialog_prefs), TRUE, TRUE,
			      FALSE);

	dialog_vbox24 = GTK_DIALOG(dialog_prefs)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_prefs), "dialog_vbox24",
			    dialog_vbox24);
	gtk_widget_show(dialog_vbox24);

	hbox22 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox22);
	gtk_box_pack_start(GTK_BOX(dialog_vbox24), hbox22, TRUE, TRUE,
			   0);


	notebook7 = gtk_notebook_new();
	gtk_widget_show(notebook7);
	gtk_box_pack_end(GTK_BOX(hbox22), notebook7, TRUE, TRUE, 0);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook7), FALSE);

	scrolledwindow56 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow56);
	gtk_container_add(GTK_CONTAINER(notebook7), scrolledwindow56);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow56),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	viewport9 = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport9);
	gtk_container_add(GTK_CONTAINER(scrolledwindow56), viewport9);

	frame26 = gtk_frame_new(_("Font Colors and Sizes"));
	gtk_widget_show(frame26);
	gtk_container_add(GTK_CONTAINER(viewport9), frame26);

	vbox35 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox35);
	gtk_container_add(GTK_CONTAINER(frame26), vbox35);

/***************************************************************************/
	table13 = gtk_table_new(7, 2, FALSE);
	gtk_widget_show(table13);
	gtk_box_pack_start(GTK_BOX(vbox35), table13, TRUE, TRUE, 0);

	label149 = gtk_label_new(_("Background Color"));
	gtk_widget_show(label149);
	gtk_table_attach(GTK_TABLE(table13), label149, 0, 1, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label149), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label149), 0, 0.5);

	label145 = gtk_label_new(_("Text Color"));
	gtk_widget_show(label145);
	gtk_table_attach(GTK_TABLE(table13), label145, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label145), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label145), 0, 0.5);

	label103 = gtk_label_new(_("Current Verse Color"));
	gtk_widget_show(label103);
	gtk_table_attach(GTK_TABLE(table13), label103, 0, 1, 3, 4,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label103), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label103), 0, 0.5);

	label150 = gtk_label_new(_("Current Verse Background Color"));
	gtk_widget_show(label150);
	gtk_table_attach(GTK_TABLE(table13), label150, 0, 1, 2, 3,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(label150, 218, -2);
	gtk_label_set_justify(GTK_LABEL(label150), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label150), 0, 0.5);

	label146 = gtk_label_new(_("Verse Numbers"));
	gtk_widget_show(label146);
	gtk_table_attach(GTK_TABLE(table13), label146, 0, 1, 4, 5,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label146), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label146), 0, 0.5);

	label152 = gtk_label_new(_("Verse Number Font Size"));
	gtk_widget_show(label152);
	gtk_table_attach(GTK_TABLE(table13), label152, 0, 1, 5, 6,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label152), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label152), 0, 0.5);

	label147 = gtk_label_new(_("Link Color"));
	gtk_widget_show(label147);
	gtk_table_attach(GTK_TABLE(table13), label147, 0, 1, 6, 7,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label147), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label147), 0, 0.5);

	color_picker.text_background = gnome_color_picker_new();
	gtk_widget_show(color_picker.text_background);
	gtk_table_attach(GTK_TABLE(table13),
			 color_picker.text_background, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER
				     (color_picker.text_background),
				     _("Pick a color for Background"));

	color_picker.text = gnome_color_picker_new();
	gtk_widget_show(color_picker.text);
	gtk_table_attach(GTK_TABLE(table13), color_picker.text, 1, 2, 1,
			 2, (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER
				     (color_picker.text),
				     _("Pick a color for Bible Text"));

	gcpCurrentverseBG = gnome_color_picker_new();
	gtk_widget_show(gcpCurrentverseBG);
	gtk_table_attach(GTK_TABLE(table13), gcpCurrentverseBG, 1, 2, 2,
			 3, (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER
				     (gcpCurrentverseBG),
				     _
				     ("Pick a color - CurrentVerse BG"));

	color_picker.text_current_verse = gnome_color_picker_new();
	gtk_widget_show(color_picker.text_current_verse);
	gtk_table_attach(GTK_TABLE(table13),
			 color_picker.text_current_verse, 1, 2, 3, 4,
			 (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER
				     (color_picker.text_current_verse),
				     _
				     ("Pick a color for Current Verse"));

	color_picker.verse_numbers = gnome_color_picker_new();
	gtk_widget_show(color_picker.verse_numbers);
	gtk_table_attach(GTK_TABLE(table13), color_picker.verse_numbers,
			 1, 2, 4, 5,
			 (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_widget_set_usize(color_picker.verse_numbers, 41, -2);

	color_picker.href_links = gnome_color_picker_new();
	gtk_widget_show(color_picker.href_links);
	gtk_table_attach(GTK_TABLE(table13), color_picker.href_links, 1,
			 2, 6, 7,
			 (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_tooltips_set_tip(tooltips, color_picker.href_links,
			     _("Strongs Numbers & Morph Tags"), NULL);

	cmbVerseNumSize = gtk_combo_new();
	gtk_widget_show(cmbVerseNumSize);
	gtk_table_attach(GTK_TABLE(table13), cmbVerseNumSize, 1, 2, 5,
			 6, (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
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
			     ("Zero is base font size Go up or down from there"),
			     NULL);
	gtk_entry_set_text(GTK_ENTRY(entry.verse_number_size), _("+0"));

	button.gnomesword_defaults =
	    gtk_button_new_with_label(_("GnomeSword Defaults"));
	gtk_widget_show(button.gnomesword_defaults);
	gtk_box_pack_start(GTK_BOX(vbox35), button.gnomesword_defaults,
		 	   TRUE,
			   TRUE, 0);
	gtk_widget_set_sensitive(button.gnomesword_defaults, FALSE);
	gtk_tooltips_set_tip(tooltips, button.gnomesword_defaults,
		_("Use GnomeSword defaults for all settings on this page"),
			     NULL);
	gtk_button_set_relief(GTK_BUTTON(button.gnomesword_defaults),
			      GTK_RELIEF_HALF);

	label98 = gtk_label_new(_("Bible Text Window"));
	gtk_widget_show(label98);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook7), 0),
				   label98);

	scrolledwindow55 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow55);
	gtk_container_add(GTK_CONTAINER(notebook7), scrolledwindow55);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow55),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	viewport8 = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport8);
	gtk_container_add(GTK_CONTAINER(scrolledwindow55), viewport8);

	vbox41 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox41);
	gtk_container_add(GTK_CONTAINER(viewport8), vbox41);

	hbox64 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox64);
	gtk_box_pack_start(GTK_BOX(vbox41), hbox64, TRUE, TRUE, 0);

	vbox28 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox28);
	gtk_box_pack_start(GTK_BOX(hbox64), vbox28, TRUE, TRUE, 0);

	frame21 =
	    gtk_frame_new(_("Use Defaults When Opening GnomeSword"));
	gtk_widget_show(frame21);
	gtk_box_pack_start(GTK_BOX(vbox28), frame21, FALSE, TRUE, 0);

	vbox36 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox36);
	gtk_container_add(GTK_CONTAINER(frame21), vbox36);

	check_button.use_defaults =
	    gtk_radio_button_new_with_label(vbox36_group,
		_("Yes, use defaults that I select"));
	vbox36_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON
				   (check_button.use_defaults));
	gtk_widget_show(check_button.use_defaults);
	gtk_box_pack_start(GTK_BOX(vbox36), check_button.use_defaults,
			   FALSE, FALSE, 0);

	rbtnNoDefaults =
	    gtk_radio_button_new_with_label(vbox36_group,
		_("No, use settings saved on last GnomeSword run"));
	vbox36_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbtnNoDefaults));
	gtk_widget_show(rbtnNoDefaults);
	gtk_box_pack_start(GTK_BOX(vbox36), rbtnNoDefaults, FALSE,
			   FALSE, 0);

	frame42 = gtk_frame_new(_("Show Tabs"));
	gtk_widget_show(frame42);
	gtk_box_pack_start(GTK_BOX(vbox28), frame42, TRUE, TRUE, 0);

	vbox54 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox54);
	gtk_container_add(GTK_CONTAINER(frame42), vbox54);

	check_button.show_bible_tabs =
	    gtk_check_button_new_with_label(_("Bible Texts"));
	gtk_widget_show(check_button.show_bible_tabs);
	gtk_box_pack_start(GTK_BOX(vbox54),
			   check_button.show_bible_tabs, FALSE, FALSE,
			   0);

	check_button.show_commentary_tabs =
	    gtk_check_button_new_with_label(_("Commentary"));
	gtk_widget_show(check_button.show_commentary_tabs);
	gtk_box_pack_start(GTK_BOX(vbox54),
			   check_button.show_commentary_tabs, FALSE,
			   FALSE, 0);

	check_button.show_dictionary_tabs =
	    gtk_check_button_new_with_label(_("Dict/Lex "));
	gtk_widget_show(check_button.show_dictionary_tabs);
	gtk_box_pack_start(GTK_BOX(vbox54),
			   check_button.show_dictionary_tabs, FALSE,
			   FALSE, 0);

	check_button.show_book_tabs =
	    gtk_check_button_new_with_label(_("Books"));
	gtk_widget_show(check_button.show_book_tabs);
	gtk_box_pack_start(GTK_BOX(vbox54), check_button.show_book_tabs,
			   FALSE, FALSE, 0);

	frame72 = gtk_frame_new(_("Show"));
	gtk_widget_show(frame72);
	gtk_box_pack_start(GTK_BOX(vbox28), frame72, TRUE, TRUE, 0);

	vbox86 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox86);
	gtk_container_add(GTK_CONTAINER(frame72), vbox86);

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
		_("Commentaries, Personal Comments and StudyPad"),
			     NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.
				      show_commentary_pane), TRUE);

	check_button.show_lower_workbook =
	    gtk_check_button_new_with_label(_("Lower Workbook"));
	gtk_widget_show(check_button.show_lower_workbook);
	gtk_box_pack_start(GTK_BOX(vbox86),
			   check_button.show_lower_workbook, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, check_button.show_lower_workbook,
			     _
			     ("Dict/Lex, Generic Books and Interlinear Page"),
			     NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_lower_workbook),
				     TRUE);

	frame43 = gtk_frame_new(_("Misc"));
	gtk_widget_show(frame43);
	gtk_box_pack_start(GTK_BOX(vbox28), frame43, FALSE, FALSE, 0);

	vbox55 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox55);
	gtk_container_add(GTK_CONTAINER(frame43), vbox55);

	check_button.use_verse_style =
	    gtk_check_button_new_with_label(_("Use Verse Style"));
	gtk_widget_show(check_button.use_verse_style);
	gtk_box_pack_start(GTK_BOX(vbox55),
			   check_button.use_verse_style, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(check_button.use_verse_style, 202, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_verse_style),
				     TRUE);

	check_button.use_default_dictionary =
	    gtk_check_button_new_with_label(_
				    ("Use Default Dictionary"));
	gtk_widget_show(check_button.use_default_dictionary);
	gtk_box_pack_start(GTK_BOX(vbox55),
			   check_button.use_default_dictionary, FALSE,
			   FALSE, 0);

	check_button.show_devotion =
	    gtk_check_button_new_with_label(_("Show Daily Devotion"));
	gtk_widget_show(check_button.show_devotion);
	gtk_box_pack_start(GTK_BOX(vbox55), check_button.show_devotion,
			   FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, check_button.show_devotion,
		 _("Show Daily Devotion if you have a Devotion module"),
			     NULL);

	frame41 = gtk_frame_new(
		_("where to View Dict/Lex When Link or Word Clicked"));
	gtk_widget_show(frame41);
	gtk_box_pack_start(GTK_BOX(vbox28), frame41, FALSE, FALSE, 0);

	vbox53 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox53);
	gtk_container_add(GTK_CONTAINER(frame41), vbox53);

	check_button.show_in_viewer =
	    gtk_check_button_new_with_label(_
					    ("In Shortcut bar Viewer"));
	gtk_widget_show(check_button.show_in_viewer);
	gtk_box_pack_start(GTK_BOX(vbox53), check_button.show_in_viewer,
			   FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_in_viewer),
				     TRUE);

	check_button.show_in_dictionary =
	    gtk_check_button_new_with_label(_("In Dict/Lex Window"));
	gtk_widget_show(check_button.show_in_dictionary);
	gtk_box_pack_start(GTK_BOX(vbox53),
			   check_button.show_in_dictionary, FALSE,
			   FALSE, 0);
			   
	frame74 = gtk_frame_new(_("Editors"));
	gtk_widget_show(frame74);
	gtk_box_pack_start(GTK_BOX(vbox41), frame74, TRUE, TRUE, 0);

	vbox89 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox89);
	gtk_container_add(GTK_CONTAINER(frame74), vbox89);

	check_button.use_studypad =
	    gtk_check_button_new_with_label(_("Use StudyPad"));
	gtk_widget_show(check_button.use_studypad);
	gtk_box_pack_start(GTK_BOX(vbox89), check_button.use_studypad,
			   FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_studypad), TRUE);

	check_button.use_studypad_dialog =
	    gtk_check_button_new_with_label(_
				    ("Open StudyPad in a Dialog"));
	gtk_widget_show(check_button.use_studypad_dialog);
	gtk_box_pack_start(GTK_BOX(vbox89),
			   check_button.use_studypad_dialog, FALSE,
			   FALSE, 0);

	check_button.use_percomm_dialog =
	    gtk_check_button_new_with_label(_
				    ("Open Personal Comments Editor in a Dialog"));
	gtk_widget_show(check_button.use_percomm_dialog);
	gtk_box_pack_start(GTK_BOX(vbox89),
			   check_button.use_percomm_dialog, FALSE,
			   FALSE, 0);

	check_button.percomm_formatting =
	    gtk_check_button_new_with_label(_
				    ("Use Formatting in Personal Comments"));
	gtk_widget_show(check_button.percomm_formatting);
	gtk_box_pack_start(GTK_BOX(vbox89),
			   check_button.percomm_formatting, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, check_button.percomm_formatting,
			     _("Use HTML tags to format notes"), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.percomm_formatting),
				     TRUE);

	label123 = gtk_label_new(_("Interface"));
	gtk_widget_show(label123);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook7), 1),
				   label123);

	scrolledwindow54 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow54);
	gtk_container_add(GTK_CONTAINER(notebook7), scrolledwindow54);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow54),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	viewport7 = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport7);
	gtk_container_add(GTK_CONTAINER(scrolledwindow54), viewport7);

	frame27 = gtk_frame_new(_("Layout"));
	gtk_widget_show(frame27);
	gtk_container_add(GTK_CONTAINER(viewport7), frame27);

	table10 = gtk_table_new(5, 2, FALSE);;
	gtk_widget_show(table10);
	gtk_container_add(GTK_CONTAINER(frame27), table10);

	label165 = gtk_label_new(_("App Width"));
	gtk_widget_show(label165);
	gtk_table_attach(GTK_TABLE(table10), label165, 0, 1, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label165), 0, 0.5);

	label166 = gtk_label_new(_("ShortCut Bar Width"));
	gtk_widget_show(label166);
	gtk_table_attach(GTK_TABLE(table10), label166, 0, 1, 2, 3,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label166), 0, 0.5);

	label167 = gtk_label_new(_("Upper Pane Height"));
	gtk_widget_show(label167);
	gtk_table_attach(GTK_TABLE(table10), label167, 0, 1, 3, 4,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label167), 0, 0.5);

	spin_button.app_width =
	    gtk_spin_button_new(GTK_ADJUSTMENT(sbtnAppWidth_adj), 1, 0);
	gtk_widget_show(spin_button.app_width);
	gtk_table_attach(GTK_TABLE(table10), spin_button.app_width, 1,
			 2, 0, 1,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	spin_button.shortcut_bar_width =
	    gtk_spin_button_new(GTK_ADJUSTMENT(sbtnSBWidth_adj), 1, 0);
	gtk_widget_show(spin_button.shortcut_bar_width);
	gtk_table_attach(GTK_TABLE(table10),
			 spin_button.shortcut_bar_width, 1, 2, 2, 3,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	spin_button.bible_pane_height =
	    gtk_spin_button_new(GTK_ADJUSTMENT(sbtnUpPaneHight_adj), 1,
				0);
	gtk_widget_show(spin_button.bible_pane_height);
	gtk_table_attach(GTK_TABLE(table10),
			 spin_button.bible_pane_height, 1, 2, 3, 4,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	label169 = gtk_label_new(_("Bible Pane Width"));
	gtk_widget_show(label169);
	gtk_table_attach(GTK_TABLE(table10), label169, 0, 1, 4, 5,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label169), 0, 0.5);

	spin_button.bible_pane_width =
	    gtk_spin_button_new(GTK_ADJUSTMENT(sbtnTextWidth_adj), 1,
				0);
	gtk_widget_show(spin_button.bible_pane_width);
	gtk_table_attach(GTK_TABLE(table10),
			 spin_button.bible_pane_width, 1, 2, 4, 5,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	label168 = gtk_label_new(_("App Height"));
	gtk_widget_show(label168);
	gtk_table_attach(GTK_TABLE(table10), label168, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label168), 0, 0.5);

	spin_button.app_height =
	    gtk_spin_button_new(GTK_ADJUSTMENT(sbtnAppHight_adj), 1, 0);
	gtk_widget_show(spin_button.app_height);
	gtk_table_attach(GTK_TABLE(table10), spin_button.app_height, 1,
			 2, 1, 2,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	label190 = gtk_label_new(_("label190"));
	gtk_widget_show(label190);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook7), 2),
				   label190);

	scrolledwindow53 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow53);
	gtk_container_add(GTK_CONTAINER(notebook7), scrolledwindow53);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow53),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	viewport6 = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport6);
	gtk_container_add(GTK_CONTAINER(scrolledwindow53), viewport6);

	vbox48 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox48);
	gtk_container_add(GTK_CONTAINER(viewport6), vbox48);

	frame24 = gtk_frame_new(_("Shortcut Bar"));
	gtk_widget_show(frame24);
	gtk_box_pack_start(GTK_BOX(vbox48), frame24, TRUE, TRUE, 0);

	vbox29 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox29);
	gtk_container_add(GTK_CONTAINER(frame24), vbox29);

	check_button.show_shortcut_bar =
	    gtk_check_button_new_with_label(_("Show Shortcut bar"));
	gtk_widget_show(check_button.show_shortcut_bar);
	gtk_box_pack_start(GTK_BOX(vbox29),
			   check_button.show_shortcut_bar, FALSE, FALSE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_shortcut_bar),
				     TRUE);

	check_button.show_favorites =
	    gtk_check_button_new_with_label(_("Show Favorites Group"));
	gtk_widget_show(check_button.show_favorites);
	gtk_box_pack_start(GTK_BOX(vbox29), check_button.show_favorites,
			   FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_favorites),
				     TRUE);

	check_button.show_text_group =
	    gtk_check_button_new_with_label(_("Show Bible Text Group"));
	gtk_widget_show(check_button.show_text_group);
	gtk_box_pack_start(GTK_BOX(vbox29),
			   check_button.show_text_group, FALSE, FALSE,
			   0);

	check_button.show_commentary_group =
	    gtk_check_button_new_with_label(_("Show Commentary Group"));
	gtk_widget_show(check_button.show_commentary_group);
	gtk_box_pack_start(GTK_BOX(vbox29),
			   check_button.show_commentary_group, FALSE,
			   FALSE, 0);

	check_button.show_dictionary_group =
	    gtk_check_button_new_with_label(_("Show Dict/Lex Group"));
	gtk_widget_show(check_button.show_dictionary_group);
	gtk_box_pack_start(GTK_BOX(vbox29),
			   check_button.show_dictionary_group, FALSE,
			   FALSE, 0);

	check_button.show_book_group =
	    gtk_check_button_new_with_label(_("Show Books Group"));
	gtk_widget_show(check_button.show_book_group);
	gtk_box_pack_start(GTK_BOX(vbox29),
			   check_button.show_book_group, FALSE, FALSE,
			   0);

	check_button.show_history_group =
	    gtk_check_button_new_with_label(_("Show History Group"));
	gtk_widget_show(check_button.show_history_group);
	gtk_box_pack_start(GTK_BOX(vbox29),
			   check_button.show_history_group, FALSE,
			   FALSE, 0);

	check_button.dock_shortcut_bar =
	    gtk_check_button_new_with_label(_("Dock Shortcut Bar"));
	gtk_widget_show(check_button.dock_shortcut_bar);
	gtk_box_pack_start(GTK_BOX(vbox29),
			   check_button.dock_shortcut_bar, FALSE, FALSE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.dock_shortcut_bar),
				     TRUE);

	label189 = gtk_label_new(_(""));
	gtk_widget_show(label189);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook7), 3),
				   label189);

	scrolledwindow52 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow52);
	gtk_container_add(GTK_CONTAINER(notebook7), scrolledwindow52);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow52),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	viewport5 = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport5);
	gtk_container_add(GTK_CONTAINER(scrolledwindow52), viewport5);

	table9 = gtk_table_new(15, 2, FALSE);
	gtk_widget_show(table9);
	gtk_container_add(GTK_CONTAINER(viewport5), table9);

	label156 = gtk_label_new(_("Main Text Module"));
	gtk_widget_show(label156);
	gtk_table_attach(GTK_TABLE(table9), label156, 0, 1, 0, 1,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	combo17 = gtk_combo_new();
	gtk_widget_show(combo17);
	gtk_table_attach(GTK_TABLE(table9), combo17, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.text_module = GTK_COMBO(combo17)->entry;
	gtk_widget_show(entry.text_module);

	combo18 = gtk_combo_new();
	gtk_widget_show(combo18);
	gtk_table_attach(GTK_TABLE(table9), combo18, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.interlinear_1_module = GTK_COMBO(combo18)->entry;
	gtk_widget_show(entry.interlinear_1_module);

	combo19 = gtk_combo_new();
	gtk_widget_show(combo19);
	gtk_table_attach(GTK_TABLE(table9), combo19, 1, 2, 2, 3,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.interlinear_2_module = GTK_COMBO(combo19)->entry;
	gtk_widget_show(entry.interlinear_2_module);

	combo20 = gtk_combo_new();
	gtk_widget_show(combo20);
	gtk_table_attach(GTK_TABLE(table9), combo20, 1, 2, 3, 4,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.interlinear_3_module = GTK_COMBO(combo20)->entry;
	gtk_widget_show(entry.interlinear_3_module);

	label157 = gtk_label_new(_("Commentary"));
	gtk_widget_show(label157);
	gtk_table_attach(GTK_TABLE(table9), label157, 0, 1, 6, 7,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label157), 0, 0.5);

	label158 = gtk_label_new(_("Dict/Lex"));
	gtk_widget_show(label158);
	gtk_table_attach(GTK_TABLE(table9), label158, 0, 1, 7, 8,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label158), 0, 0.5);

	label159 = gtk_label_new(_("Personal"));
	gtk_widget_show(label159);
	gtk_table_attach(GTK_TABLE(table9), label159, 0, 1, 13, 14,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label159), 0, 0.5);

	combo21 = gtk_combo_new();
	gtk_widget_show(combo21);
	gtk_table_attach(GTK_TABLE(table9), combo21, 1, 2, 4, 5,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.interlinear_4_module = GTK_COMBO(combo21)->entry;
	gtk_widget_show(entry.interlinear_4_module);

	combo22 = gtk_combo_new();
	gtk_widget_show(combo22);
	gtk_table_attach(GTK_TABLE(table9), combo22, 1, 2, 5, 6,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.interlinear_5_module = GTK_COMBO(combo22)->entry;
	gtk_widget_show(entry.interlinear_5_module);

	combo23 = gtk_combo_new();
	gtk_widget_show(combo23);
	gtk_table_attach(GTK_TABLE(table9), combo23, 1, 2, 6, 7,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.commentary_module = GTK_COMBO(combo23)->entry;
	gtk_widget_show(entry.commentary_module);

	combo25 = gtk_combo_new();
	gtk_widget_show(combo25);
	gtk_table_attach(GTK_TABLE(table9), combo25, 1, 2, 13, 14,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.percomm_module = GTK_COMBO(combo25)->entry;
	gtk_widget_show(entry.percomm_module);

	label160 = gtk_label_new(_("Interlinear 5"));
	gtk_widget_show(label160);
	gtk_table_attach(GTK_TABLE(table9), label160, 0, 1, 5, 6,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label160), 0, 0.5);

	label161 = gtk_label_new(_("Interlinear 4"));
	gtk_widget_show(label161);
	gtk_table_attach(GTK_TABLE(table9), label161, 0, 1, 4, 5,
			 (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label161), 0, 0.5);

	label162 = gtk_label_new(_("Interlinear 3"));
	gtk_widget_show(label162);
	gtk_table_attach(GTK_TABLE(table9), label162, 0, 1, 3, 4,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label162), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label162), 7.45058e-09,
			       7.45058e-09);

	label163 = gtk_label_new(_("Interlinear 2"));
	gtk_widget_show(label163);
	gtk_table_attach(GTK_TABLE(table9), label163, 0, 1, 2, 3,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label163), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label163), 7.45058e-09,
			       7.45058e-09);

	label164 = gtk_label_new(_("Interlinear 1"));
	gtk_widget_show(label164);
	gtk_table_attach(GTK_TABLE(table9), label164, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label164), 1.2666e-07,
			       7.45058e-09);

	labelDevotional = gtk_label_new(_("Daily Devotional"));
	gtk_widget_show(labelDevotional);
	gtk_table_attach(GTK_TABLE(table9), labelDevotional, 0, 1, 14,
			 15, (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(labelDevotional), 0, 0.5);

	comboDevotion = gtk_combo_new();
	gtk_widget_show(comboDevotion);
	gtk_table_attach(GTK_TABLE(table9), comboDevotion, 1, 2, 14, 15,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.devotion_module = GTK_COMBO(comboDevotion)->entry;
	gtk_widget_show(entry.devotion_module);

	combo24 = gtk_combo_new();
	gtk_widget_show(combo24);
	gtk_table_attach(GTK_TABLE(table9), combo24, 1, 2, 7, 8,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.dictionary_module = GTK_COMBO(combo24)->entry;
	gtk_widget_show(entry.dictionary_module);

	combo26 = gtk_combo_new();
	gtk_widget_show(combo26);
	gtk_table_attach(GTK_TABLE(table9), combo26, 1, 2, 9, 10,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.greek_lex__module = GTK_COMBO(combo26)->entry;
	gtk_widget_show(entry.greek_lex__module);
	gtk_tooltips_set_tip(tooltips, entry.greek_lex__module,
	  _("Which Greek Lexicon to display in Dict/Lex Window when a link or word is clicked"),
			     NULL);

	label192 = gtk_label_new(_("Hebrew Lexicon"));
	gtk_widget_show(label192);
	gtk_table_attach(GTK_TABLE(table9), label192, 0, 1, 11, 12,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label192), 0, 0.5);

	combo27 = gtk_combo_new();
	gtk_widget_show(combo27);
	gtk_table_attach(GTK_TABLE(table9), combo27, 1, 2, 11, 12,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.hebrew_lex__module = GTK_COMBO(combo27)->entry;
	gtk_widget_show(entry.hebrew_lex__module);
	gtk_tooltips_set_tip(tooltips, entry.hebrew_lex__module,
	  _("Which Hebrew Lexicon to display in Dict/Lex Window when a link or word is clicked"),
			     NULL);

	label202 = gtk_label_new(_("Greek Lex Viewer"));
	gtk_widget_show(label202);
	gtk_table_attach(GTK_TABLE(table9), label202, 0, 1, 10, 11,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label202), 0, 0.5);

	label203 = gtk_label_new(_("Hebrew Lex Viewer"));
	gtk_widget_show(label203);
	gtk_table_attach(GTK_TABLE(table9), label203, 0, 1, 12, 13,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label203), 0, 0.5);

	comboGreekViewer = gtk_combo_new();
	gtk_widget_show(comboGreekViewer);
	gtk_table_attach(GTK_TABLE(table9), comboGreekViewer, 1, 2, 10,
			 11, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.greek_lex_viewer_module =
	    GTK_COMBO(comboGreekViewer)->entry;
	gtk_widget_show(entry.greek_lex_viewer_module);
	gtk_tooltips_set_tip(tooltips, entry.greek_lex_viewer_module,
	  _("Which Greek Lecicon to display in viewer when a link or word is clicked"),
			     NULL);

	comboHebViewer = gtk_combo_new();
	gtk_widget_show(comboHebViewer);
	gtk_table_attach(GTK_TABLE(table9), comboHebViewer, 1, 2, 12,
			 13, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.hebrew_lex_viewer_module =
	    GTK_COMBO(comboHebViewer)->entry;
	gtk_widget_show(entry.hebrew_lex_viewer_module);
	gtk_tooltips_set_tip(tooltips, entry.hebrew_lex_viewer_module,
	   _("Which Hebrew Lexicon to display in viewer when a link or word is clicked"),
			     NULL);

	label191 = gtk_label_new(_("Greek Lexicon"));
	gtk_widget_show(label191);
	gtk_table_attach(GTK_TABLE(table9), label191, 0, 1, 9, 10,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label191), 0, 0.5);

	label253 = gtk_label_new(_("Default Dict"));
	gtk_widget_show(label253);
	gtk_table_attach(GTK_TABLE(table9), label253, 0, 1, 8, 9,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label253), 0, 0.5);

	comboDefaultDict = gtk_combo_new();
	gtk_widget_show(comboDefaultDict);
	gtk_table_attach(GTK_TABLE(table9), comboDefaultDict, 1, 2, 8,
			 9, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	entry.default_dictionary_module =
	    GTK_COMBO(comboDefaultDict)->entry;
	gtk_widget_show(entry.default_dictionary_module);

	label155 = gtk_label_new(_("Sword Modules"));
	gtk_widget_show(label155);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook7), 4),
				   label155);

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
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(hbuttonbox3), 10);

	button.ok = gnome_stock_button(GNOME_STOCK_BUTTON_OK);
	gtk_widget_show(button.ok);
	gtk_container_add(GTK_CONTAINER(hbuttonbox3), button.ok);
	GTK_WIDGET_SET_FLAGS(button.ok, GTK_CAN_DEFAULT);

	button.apply = gnome_stock_button(GNOME_STOCK_BUTTON_APPLY);
	gtk_widget_show(button.apply);
	gtk_container_add(GTK_CONTAINER(hbuttonbox3), button.apply);
	GTK_WIDGET_SET_FLAGS(button.apply, GTK_CAN_DEFAULT);

	button_cancel = gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	gtk_widget_show(button_cancel);
	gtk_container_add(GTK_CONTAINER(hbuttonbox3), button_cancel);
	GTK_WIDGET_SET_FLAGS(button_cancel, GTK_CAN_DEFAULT);

/*********************************************** start shortcut bar */
	shortcut_model = e_shortcut_model_new();

	shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(shortcut_bar),
				 shortcut_model);
	gtk_widget_set_usize(shortcut_bar, 150, 250);
	gtk_widget_show(shortcut_bar);
	gtk_box_pack_end(GTK_BOX(hbox22), shortcut_bar, FALSE, TRUE, 0);
	gtk_widget_set_usize(shortcut_bar, 162, -2);
/************************************************** end shortcut bar */


/**************************************************** start settings */
	updatehtml = FALSE;
	updateSB = FALSE;
	updatelayout = FALSE;

	/** add module list to combo boxs **/
	gtk_combo_set_popdown_strings(GTK_COMBO(combo17), biblelist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo18), biblelist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo19), biblelist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo20), biblelist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo21), biblelist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo22), biblelist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo23), commlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo24), dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(comboDefaultDict),
				      dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(comboGreekViewer),
				      dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo26), dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(comboHebViewer),
				      dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo27), dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo25), percomlist);
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
				     (check_button.show_shortcut_bar),
				     settings.showshortcutbar);
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
				     (check_button.show_favorites),
				     settings.showfavoritesgroup);
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
				     (check_button.show_text_group),
				     settings.showtextgroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.
				      show_commentary_group),
				     settings.showcomgroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.
				      show_dictionary_group),
				     settings.showdictgroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_book_group),
				     settings.showbookgroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.show_history_group),
				     settings.showhistorygroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.dock_shortcut_bar),
				     settings.docked);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.percomm_formatting),
				     settings.formatpercom);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_studypad),
				     settings.use_studypad);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_studypad_dialog),
				     settings.use_studypad_dialog);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (check_button.use_percomm_dialog),
				     settings.use_percomm_dialog);
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

	gtk_entry_set_text(GTK_ENTRY(entry.text_module),
			   settings.MainWindowModule);
	gtk_entry_set_text(GTK_ENTRY(entry.interlinear_1_module),
			   settings.Interlinear1Module);
	gtk_entry_set_text(GTK_ENTRY(entry.interlinear_2_module),
			   settings.Interlinear2Module);
	gtk_entry_set_text(GTK_ENTRY(entry.interlinear_3_module),
			   settings.Interlinear3Module);
	gtk_entry_set_text(GTK_ENTRY(entry.interlinear_4_module),
			   settings.Interlinear4Module);
	gtk_entry_set_text(GTK_ENTRY(entry.interlinear_5_module),
			   settings.Interlinear5Module);
	gtk_entry_set_text(GTK_ENTRY(entry.commentary_module),
			   settings.CommWindowModule);
	gtk_entry_set_text(GTK_ENTRY(entry.dictionary_module),
			   settings.DictWindowModule);
	gtk_entry_set_text(GTK_ENTRY(entry.default_dictionary_module),
			   settings.DefaultDict);
	gtk_entry_set_text(GTK_ENTRY(entry.greek_lex_viewer_module),
			   settings.lex_greek_viewer);
	gtk_entry_set_text(GTK_ENTRY(entry.hebrew_lex_viewer_module),
			   settings.lex_hebrew_viewer);
	gtk_entry_set_text(GTK_ENTRY(entry.percomm_module),
			   settings.personalcommentsmod);
	gtk_entry_set_text(GTK_ENTRY(entry.greek_lex__module),
			   settings.lex_greek);
	gtk_entry_set_text(GTK_ENTRY(entry.hebrew_lex__module),
			   settings.lex_hebrew);
	gtk_entry_set_text(GTK_ENTRY(entry.devotion_module),
			   settings.devotionalmod);

	/********************************************* end settings */

	/*** color pickers ***/
	gtk_signal_connect(GTK_OBJECT(color_picker.text_background),
			   "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(color_picker.text), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(gcpCurrentverseBG), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(color_picker.text_current_verse),
			   "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(color_picker.verse_numbers),
			   "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(color_picker.href_links),
			   "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	/*** combo entrys  font sizes ***/
	gtk_signal_connect(GTK_OBJECT(entry.verse_number_size),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);

	/*** toggle buttons ***/
	gtk_signal_connect(GTK_OBJECT(check_button.use_defaults),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(rbtnNoDefaults), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(check_button.show_shortcut_bar),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(2));
	gtk_signal_connect(GTK_OBJECT(check_button.show_bible_tabs),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT
			   (check_button.show_commentary_tabs),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT
			   (check_button.show_dictionary_tabs),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(check_button.show_book_tabs),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(check_button.show_in_viewer),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(check_button.show_in_dictionary),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(check_button.show_favorites),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(check_button.show_text_group),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT
			   (check_button.show_commentary_group),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT
			   (check_button.show_dictionary_group),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(check_button.show_history_group),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT
			   (check_button.use_default_dictionary),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(check_button.percomm_formatting),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(check_button.dock_shortcut_bar),
			   "toggled", GTK_SIGNAL_FUNC(on_dock_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(check_button.use_verse_style),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(check_button.show_devotion),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));

	gtk_signal_connect(GTK_OBJECT(check_button.show_bible_pane),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT
			   (check_button.show_commentary_pane),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(check_button.show_lower_workbook),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));


	gtk_signal_connect(GTK_OBJECT(check_button.use_studypad),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(check_button.use_studypad_dialog),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(check_button.use_percomm_dialog),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled), NULL);

	/*** spin buttons layout ***/
	gtk_signal_connect(GTK_OBJECT(spin_button.app_width), "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spin_button.app_height),
			   "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spin_button.shortcut_bar_width),
			   "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spin_button.bible_pane_width),
			   "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spin_button.bible_pane_height),
			   "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed),
			   NULL);
			   
	/*** module combos ***/
	gtk_signal_connect(GTK_OBJECT(entry.text_module), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry.interlinear_1_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.interlinear_2_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.interlinear_3_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.interlinear_4_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.interlinear_5_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.commentary_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.dictionary_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.percomm_module), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry.greek_lex__module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.hebrew_lex__module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.default_dictionary_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.hebrew_lex_viewer_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.greek_lex_viewer_module),
			   "changed", GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(entry.devotion_module), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	/*** OK - Apply - Cancel ***/
	gtk_signal_connect(GTK_OBJECT(button.ok), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPropertyboxOK_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(button.apply), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btnPropertyboxApply_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(button_cancel), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btnPropertyboxCancel_clicked), NULL);
	gtk_object_set_data(GTK_OBJECT(dialog_prefs), "tooltips",
			    tooltips);

	/** add group and icons to shortcut bar **/
	groupnum =
	    add_sb_group2((EShortcutBar *) shortcut_bar,
			  _("Preferences"));
	gtk_signal_connect(GTK_OBJECT(shortcut_bar), "item_selected",
			   GTK_SIGNAL_FUNC
			   (on_shortcut_bar_item_selected1), NULL);

	pathname = gnome_pixmap_file("gnome-ccwindowmanager.png");
	icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->model,
				  groupnum, -1, "fonts",
				  _("Font Colors and Sizes"),
				  icon_pixbuf);
	pathname = gnome_pixmap_file("gnome-settings.png");
	icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->model,
				  groupnum, -1, "misc interface",
				  _("Misc Interface Settings"),
				  icon_pixbuf);
	pathname = gnome_pixmap_file("gnome-settings.png");
	icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->model,
				  groupnum, -1, "misc interface",
				  _("Layout"), icon_pixbuf);
	pathname = gnome_pixmap_file("gnome-settings.png");
	icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->model,
				  groupnum, -1, "misc interface",
				  _("Shortcut Bar Settings"),
				  icon_pixbuf);
	pathname = gnome_pixmap_file("gdict.png");
	icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->model,
				  groupnum, -1, "Sword Mods",
				  _("Sword Modules"), icon_pixbuf);
	e_shortcut_bar_set_view_type((EShortcutBar *) shortcut_bar,
				     groupnum, E_ICON_BAR_LARGE_ICONS);
	/** set color pickers to current colors **/
	setcolorpickersColor(color_picker.text_background,
			     color_picker.text,
			     color_picker.text_current_verse,
			     color_picker.verse_numbers,
			     color_picker.href_links);

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
	set_buttons_sensitive(GTK_WIDGET(dlg), FALSE);
	gtk_widget_show(dlg);
}
