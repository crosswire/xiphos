/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_preferences_dlg.c
    * -------------------
    * Fri July 27 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
  */

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

#include "gs_preferences_dlg.h"
#include "sword.h"
#include "gs_gnomesword.h"
#include "properties.h"
#include "support.h"
#include "gs_shortcutbar.h"

GtkWidget *notebook7;
extern gchar *tmpcolor;
static gboolean updatehtml, updateSB, updatelayout;

static EShortcutModel *shortcut_model;

/******************************************************************************
 * add_sb_group - add group to shourtcut bar
 * shortcut_bar - shortcut bar to add group to
 * group_name - name of the group to be added
 *****************************************************************************/
static gint add_sb_group2(EShortcutBar * shortcut_bar, gchar * group_name)
{
	gint group_num;

	group_num =
	    e_shortcut_model_add_group(shortcut_bar->model, -1,
				       group_name);
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_SMALL_ICONS);
	return group_num;
}

/*****************************************************************************
 *      for any shortcut bar item clicked
 *****************************************************************************/
static void
on_shortcut_bar_item_selected1(EShortcutBar * shortcut_bar,
			       GdkEvent * event,
			       gint group_num, gint item_num)
{
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook7), item_num);	/* set notebook page */
}


static void applyoptions(SETTINGS * s)
{
	GtkWidget *text, *dict, *comm;

	text = lookup_widget(s->app, "nbTextMods");
	dict = s->notebookDL;
	comm = s->notebookCOMM;
	/*  */
	if (updatelayout) {
		/* set the main window size */
		gtk_widget_set_usize(s->app, s->gs_width, s->gs_hight);

		if (s->showshortcutbar && s->docked) {
			e_paned_set_position(E_PANED
					     (lookup_widget
					      (s->app, "epaned")),
					     s->shortcutbar_width);
		} else {
			e_paned_set_position(E_PANED
					     (lookup_widget
					      (s->app, "epaned")), 1);
		}
		/* set height of bible and commentary pane */
		e_paned_set_position(E_PANED
				     (lookup_widget(s->app, "vpaned1")),
				     s->upperpane_hight);
		/* set width of bible pane */
		e_paned_set_position(E_PANED
				     (lookup_widget(s->app, "hpaned1")),
				     s->biblepane_width);
		updatelayout = FALSE;
	}
	
	if (s->text_tabs) {
		gtk_widget_show(text);
	} else {
		gtk_widget_hide(text);
	}
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookGBS), 
					s->book_tabs);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookDL), 
					s->dict_tabs);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookCOMM),
					s->comm_tabs);
	
	GTK_CHECK_MENU_ITEM(s->versestyle_item)->active = s->versestyle;
	if (updatehtml)
		backend_display_new_font_color_and_size(s);
	if (updateSB)
		update_shortcut_bar(s);
	updatehtml = FALSE;
	updateSB = FALSE;
}


static void get_preferences_from_dlg(GtkWidget * d, SETTINGS * s)
{
	/************************************************************
	we need to read all propertybox options here
	*************************************************************/
	gchar *buf, *font;
	gdouble color[4];

	/*** read modules ***/
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry3")));
	sprintf(s->MainWindowModule, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry4")));
	sprintf(s->Interlinear1Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry5")));
	sprintf(s->Interlinear2Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry6")));
	sprintf(s->Interlinear3Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry7")));
	sprintf(s->Interlinear4Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry8")));
	sprintf(s->Interlinear5Module, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry9")));
	sprintf(s->CommWindowModule, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry10")));
	sprintf(s->DictWindowModule, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entryDefaultDict")));
	sprintf(s->DefaultDict, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry11")));
	sprintf(s->personalcommentsmod, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entryDevotion")));
	sprintf(s->devotionalmod, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry13")));
	sprintf(s->lex_greek, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "entry14")));
	sprintf(s->lex_hebrew, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "combo_entryGreekViewer")));
	sprintf(s->lex_greek_viewer, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(d, "combo_entryHebViewer")));
	sprintf(s->lex_hebrew_viewer, "%s", buf);
	/*** read font pickers ***/
	font =
	    gnome_font_picker_get_font_name(GNOME_FONT_PICKER
					    (lookup_widget
					     (d, "fpDefaultFont")));
	sprintf(s->default_font, "%s", font);
	font =
	    gnome_font_picker_get_font_name(GNOME_FONT_PICKER
					    (lookup_widget
					     (d, "fpGreekFont")));
	sprintf(s->greek_font, "%s", font);
	font =
	    gnome_font_picker_get_font_name(GNOME_FONT_PICKER
					    (lookup_widget
					     (d, "fpHebrewFont")));
	sprintf(s->hebrew_font, "%s", font);
	font =
	    gnome_font_picker_get_font_name(GNOME_FONT_PICKER
					    (lookup_widget
					     (d, "fpUnicodeFont")));
	sprintf(s->unicode_font, "%s", font);
	/*** read html colors ***/
	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (lookup_widget(d, "gcpText")), &color[0],
				 &color[1], &color[2], &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(s->bible_text_color, "%s", buf);
	g_free(buf);
	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (lookup_widget(d, "gcpTextBG")),
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(s->bible_bg_color, "%s", buf);
	g_free(buf);
	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (lookup_widget(d, "gcpCurrentverse")),
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(s->currentverse_color, "%s", buf);
	g_free(buf);
	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (lookup_widget(d, "gcpTextVerseNums")),
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(s->bible_verse_num_color, "%s", buf);
	g_free(buf);
	gnome_color_picker_get_d(GNOME_COLOR_PICKER
				 (lookup_widget(d, "gcpTextLinks")),
				 &color[0], &color[1], &color[2],
				 &color[3]);
	buf = gdouble_arr_to_hex(color, 0);
	sprintf(s->link_color, "%s", buf);
	g_free(buf);
	/*** read font sizes ***/
	buf = gtk_entry_get_text(GTK_ENTRY
				 (lookup_widget(d, "cmbEntryTextSize")));
	sprintf(s->bible_font_size, "%s", buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				 (lookup_widget(d, "cmbEentryVNSize")));
	sprintf(s->verse_num_font_size, "%s", buf);
	/*** read radio buttons ***/
	s->usedefault = GTK_TOGGLE_BUTTON(lookup_widget(d,"rbtnUsedefaults"))->active;
	/*** read check buttons ***/
	s->showshortcutbar = GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowSCB"))->active;
	s->text_tabs =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowBibletabs"))->
	    active;
	s->comm_tabs =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowCOMtabs"))->active;
	s->dict_tabs =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowDLtabs"))->active;
	s->book_tabs =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowBooktabs"))->active;
	s->useDefaultDict =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnUseDefaultDict"))->active;
	s->versestyle =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "checkbutton9"))->active;
	s->showfavoritesgroup =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnFavoritesGroup"))->
	    active;
	s->showtextgroup =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowTextgroup"))->
	    active;
	s->showcomgroup =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowComGroup"))->
	    active;
	s->showdictgroup =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowDictGroup"))->
	    active;
	s->showbookgroup =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowBookGroup"))->
	    active;
	s->showhistorygroup =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowHistoryGroup"))->
	    active;
	s->docked =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "checkbuttonSBDock"))->
	    active;
	s->formatpercom =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnPNformat"))->active;
	s->inViewer =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnInViewer"))->active;
	s->inDictpane =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnInDictPane"))->active;
	/*
	   s->autosavepersonalcomments =
	   GTK_TOGGLE_BUTTON(lookup_widget(d, "checkbutton8"))->active;
	 */
	s->showdevotional =
	    GTK_TOGGLE_BUTTON(lookup_widget(d, "cbtnShowDevotion"))->active;
	    
	/*** read layout spin buttons ***/
	s->gs_width =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (lookup_widget
					      (d, "sbtnAppWidth")));
	s->gs_hight =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (lookup_widget
					      (d, "sbtnAppHight")));
	s->shortcutbar_width =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (lookup_widget
					      (d, "sbtnSBWidth")));
	s->biblepane_width =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (lookup_widget
					      (d, "sbtnTextWidth")));
	s->upperpane_hight =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (lookup_widget
					      (d, "sbtnUpPaneHight")));
	
	backend_save_properties(s, FALSE);
	applyoptions(s);

}


static void on_btnPropertyboxOK_clicked(GtkButton * button,
					gpointer user_data)
{
	GtkWidget *dlg;
	SETTINGS *s;

	s = (SETTINGS *) user_data;
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	get_preferences_from_dlg(dlg, s);
	gtk_widget_destroy(dlg);
}


static void on_btnPropertyboxApply_clicked(GtkButton * button,
					   gpointer user_data)
{
	GtkWidget *dlg;
	SETTINGS *s;

	s = (SETTINGS *) user_data;
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	get_preferences_from_dlg(dlg, s);
}

static void
on_btnPropertyboxCancel_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}


static void set_buttons_sensitive(GtkWidget * widget)
{
	GtkWidget *dlg, *btnok, *btnapply;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(widget));
	btnok = lookup_widget(dlg, "btnPropertyboxOK");
	btnapply = lookup_widget(dlg, "btnPropertyboxApply");
	gtk_widget_set_sensitive(btnok, TRUE);
	gtk_widget_set_sensitive(btnapply, TRUE);
}

static void
on_colorpicker_color_set(GnomeColorPicker * gnomecolorpicker,
			 guint arg1,
			 guint arg2,
			 guint arg3, guint arg4, gpointer user_data)
{
	set_buttons_sensitive(GTK_WIDGET(gnomecolorpicker));
	updatehtml = TRUE;
}

static void on_Entry_changed(GtkEditable * editable, gpointer user_data)
{
	set_buttons_sensitive(GTK_WIDGET(editable));
	updatehtml = TRUE;
}


static void
on_button_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	set_buttons_sensitive(GTK_WIDGET(togglebutton));
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


static void
on_spinbutton_changed(GtkEditable * editable, gpointer user_data)
{
	set_buttons_sensitive(GTK_WIDGET(editable));
	updatelayout = TRUE;
}

static void
on_font_set(GnomeFontPicker * gnomefontpicker,
	    gchar * arg1, gpointer user_data)
{
	set_buttons_sensitive(GTK_WIDGET(gnomefontpicker));
	updatehtml = TRUE;
}


static void
setcolorpickersColor(SETTINGS * s,
		     GtkWidget * gcpTextBG,
		     GtkWidget * gcpText,
		     GtkWidget * gcpCurrentverse,
		     GtkWidget * gcpTextVerseNums,
		     GtkWidget * gcpTextLinks)
{
	gdouble *color;
	gushort a = 000000;

	if (string_is_color(s->bible_bg_color)) {
		color = hex_to_gdouble_arr(s->bible_bg_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextBG), color[0],
					  color[1], color[2], a);
	}

	if (string_is_color(s->bible_text_color)) {
		color = hex_to_gdouble_arr(s->bible_text_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER(gcpText),
					  color[0], color[1], color[2], a);
	}

	if (string_is_color(s->currentverse_color)) {
		color = hex_to_gdouble_arr(s->currentverse_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpCurrentverse), color[0],
					  color[1], color[2], a);
	}

	if (string_is_color(s->bible_verse_num_color)) {
		color = hex_to_gdouble_arr(s->bible_verse_num_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextVerseNums), color[0],
					  color[1], color[2], a);
	}

	if (string_is_color(s->link_color)) {
		color = hex_to_gdouble_arr(s->link_color);
		gnome_color_picker_set_i8(GNOME_COLOR_PICKER
					  (gcpTextLinks), color[0],
					  color[1], color[2], a);
	}

}


/***  ***/
GtkWidget *create_dlgSettings(SETTINGS * s,
			      GList * biblelist,
			      GList * commlist,
			      GList * dictlist,
			      GList * percomlist, GList * devotionlist)
{
	gint groupnum;
	gchar *pathname;
	GdkPixbuf *icon_pixbuf;
	GtkWidget *shortcut_bar;

 GtkWidget *dlgSettings;
  GtkWidget *dialog_vbox9;
  GtkWidget *hbox22;
//  GtkWidget *frame30;
//  GtkWidget *notebook7;
  GtkWidget *scrolledwindow56;
  GtkWidget *viewport9;
  GtkWidget *frame26;
  GtkWidget *vbox35;
  GtkWidget *table13;
  GtkWidget *label151;
  GtkWidget *label149;
  GtkWidget *label145;
  GtkWidget *label103;
  GtkWidget *label150;
  GtkWidget *label146;
  GtkWidget *label152;
  GtkWidget *label147;
  GtkWidget *label174;
  GtkWidget *label175;
  GtkWidget *label176;
  GtkWidget *fpDefaultFont;
  GtkWidget *gcpTextBG;
  GtkWidget *gcpText;
  GtkWidget *cmbTextFontSize;
  GList *cmbTextFontSize_items = NULL;
  GtkWidget *cmbEntryTextSize;
  GtkWidget *gcpCurrentverseBG;
  GtkWidget *gcpCurrentverse;
  GtkWidget *gcpTextVerseNums;
  GtkWidget *gcpTextLinks;
  GtkWidget *fpGreekFont;
  GtkWidget *fpHebrewFont;
  GtkWidget *fpUnicodeFont;
  GtkWidget *cmbVerseNumSize;
  GList *cmbVerseNumSize_items = NULL;
  GtkWidget *cmbEentryVNSize;
  GtkWidget *label173;
  GtkWidget *btnSetToGSDefaults;
  GtkWidget *label98;
  GtkWidget *scrolledwindow55;
  GtkWidget *viewport8;
  GtkWidget *vbox41;
  GtkWidget *hbox64;
  GtkWidget *vbox28;
  GtkWidget *frame21;
  GtkWidget *vbox36;
  GSList *vbox36_group = NULL;
  GtkWidget *rbtnUsedefaults;
  GtkWidget *rbtnNoDefaults;
  GtkWidget *frame42;
  GtkWidget *vbox54;
  GtkWidget *cbtnShowBibletabs;
  GtkWidget *cbtnShowCOMtabs;
  GtkWidget *cbtnShowDLtabs;
  GtkWidget *cbtnShowBooktabs;  
  GtkWidget *frame43;
  GtkWidget *vbox55;
  GtkWidget *checkbutton9;
  GtkWidget *cbtnShowDevotion;
  GtkWidget *cbtnUseDefaultDict;
  GtkWidget *frame41;
  GtkWidget *vbox53;
  GtkWidget *cbtnInViewer;
  GtkWidget *cbtnInDictPane;
  GtkWidget *frame25;
  GtkWidget *vbox37;
  GtkWidget *cbtnPNformat;
  GtkWidget *checkbutton8;
  GtkWidget *label123;
  GtkWidget *scrolledwindow54;
  GtkWidget *viewport7;
  GtkWidget *frame27;
  GtkWidget *table10;
  GtkWidget *label165;
  GtkWidget *label166;
  GtkWidget *label167;
  GtkObject *sbtnAppWidth_adj;
  GtkWidget *sbtnAppWidth;
  GtkObject *sbtnSBWidth_adj;
  GtkWidget *sbtnSBWidth;
  GtkObject *sbtnUpPaneHight_adj;
  GtkWidget *sbtnUpPaneHight;
  GtkWidget *label169;
  GtkObject *sbtnTextWidth_adj;
  GtkWidget *sbtnTextWidth;
  GtkWidget *label168;
  GtkObject *sbtnAppHight_adj;
  GtkWidget *sbtnAppHight;
  GtkWidget *label190;
  GtkWidget *scrolledwindow53;
  GtkWidget *viewport6;
  GtkWidget *vbox48;
  GtkWidget *frame24;
  GtkWidget *vbox29;
  GtkWidget *cbtnShowSCB;
  GtkWidget *cbtnFavoritesGroup;
  GtkWidget *cbtnShowTextgroup;
  GtkWidget *cbtnShowComGroup;
  GtkWidget *cbtnShowDictGroup;
  GtkWidget *cbtnShowBookGroup;
  GtkWidget *cbtnShowHistoryGroup;
  GtkWidget *checkbuttonSBDock;
  GtkWidget *label189;
  GtkWidget *scrolledwindow52;
  GtkWidget *viewport5;
  GtkWidget *table9;
  GtkWidget *label156;
  GtkWidget *combo17;
  GtkWidget *entry3;
  GtkWidget *combo18;
  GtkWidget *entry4;
  GtkWidget *combo19;
  GtkWidget *entry5;
  GtkWidget *combo20;
  GtkWidget *entry6;
  GtkWidget *label157;
  GtkWidget *label158;
  GtkWidget *label159;
  GtkWidget *combo21;
  GtkWidget *entry7;
  GtkWidget *combo22;
  GtkWidget *entry8;
  GtkWidget *combo23;
  GtkWidget *entry9;
  GtkWidget *combo25;
  GtkWidget *entry11;
  GtkWidget *label160;
  GtkWidget *label161;
  GtkWidget *label162;
  GtkWidget *label163;
  GtkWidget *label164;
  GtkWidget *labelDevotional;
  GtkWidget *comboDevotion;
  GtkWidget *entryDevotion;
  GtkWidget *label191;
  GtkWidget *combo24;
  GtkWidget *entry10;
  GtkWidget *combo26;
  GtkWidget *entry13;
  GtkWidget *label192;
  GtkWidget *combo27;
  GtkWidget *entry14;
  GtkWidget *label202;
  GtkWidget *label203;
  GtkWidget *comboGreekViewer;
  GtkWidget *combo_entryGreekViewer;
  GtkWidget *comboHebViewer;
  GtkWidget *combo_entryHebViewer;
  GtkWidget *label204;
  GtkWidget *comboDefaultDict;
  GtkWidget *entryDefaultDict;
  GtkWidget *label155;
  GtkWidget *dialog_action_area9;
  GtkWidget *btnPropertyboxOK;
  GtkWidget *btnPropertyboxApply;
  GtkWidget *btnPropertyboxCancel;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();
  /*************************************************************/
	sbtnAppWidth_adj =
	    gtk_adjustment_new(s->gs_width, 640, 10000, 1, 10, 10);
	sbtnAppHight_adj =
	    gtk_adjustment_new(s->gs_hight, 480, 10000, 1, 10, 10);
	sbtnSBWidth_adj =
	    gtk_adjustment_new(s->shortcutbar_width, 0, 300, 1, 10, 10);
	sbtnTextWidth_adj =
	    gtk_adjustment_new(s->biblepane_width, 0, 500, 1, 10, 10);
	sbtnUpPaneHight_adj =
	    gtk_adjustment_new(s->upperpane_hight, 0, 10000, 1, 10, 10);
  /*************************************************************/	    
  dlgSettings = gnome_dialog_new (_("GnomeSword - Settings"), NULL);
  gtk_object_set_data (GTK_OBJECT (dlgSettings), "dlgSettings", dlgSettings);
  gtk_window_set_default_size (GTK_WINDOW (dlgSettings), 560, 371);
  gtk_window_set_policy (GTK_WINDOW (dlgSettings), TRUE, TRUE, TRUE);

  dialog_vbox9 = GNOME_DIALOG (dlgSettings)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlgSettings), "dialog_vbox9", dialog_vbox9);
  gtk_widget_show (dialog_vbox9);

  hbox22 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox22);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "hbox22", hbox22,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox22);
  gtk_box_pack_start (GTK_BOX (dialog_vbox9), hbox22, TRUE, TRUE, 0);

  notebook7 = gtk_notebook_new ();
  gtk_widget_ref (notebook7);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "notebook7", notebook7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (notebook7);
  gtk_box_pack_end(GTK_BOX(hbox22), notebook7, TRUE, TRUE, 0);
  GTK_WIDGET_UNSET_FLAGS (notebook7, GTK_CAN_FOCUS);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook7), FALSE);

  scrolledwindow56 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow56);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "scrolledwindow56", scrolledwindow56,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow56);
  gtk_container_add (GTK_CONTAINER (notebook7), scrolledwindow56);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow56), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport9 = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (viewport9);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "viewport9", viewport9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (viewport9);
  gtk_container_add (GTK_CONTAINER (scrolledwindow56), viewport9);

  frame26 = gtk_frame_new (_("Font Colors and Sizes"));
  gtk_widget_ref (frame26);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "frame26", frame26,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame26);
  gtk_container_add (GTK_CONTAINER (viewport9), frame26);

  vbox35 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox35);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox35", vbox35,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox35);
  gtk_container_add (GTK_CONTAINER (frame26), vbox35);

  table13 = gtk_table_new (12, 2, FALSE);
  gtk_widget_ref (table13);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "table13", table13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table13);
  gtk_box_pack_start (GTK_BOX (vbox35), table13, TRUE, TRUE, 0);

  label151 = gtk_label_new (_("Font Size"));
  gtk_widget_ref (label151);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label151", label151,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label151);
  gtk_table_attach (GTK_TABLE (table13), label151, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label151), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label151), 0, 0.5);

  label149 = gtk_label_new (_("Background Color"));
  gtk_widget_ref (label149);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label149", label149,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label149);
  gtk_table_attach (GTK_TABLE (table13), label149, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label149), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label149), 0, 0.5);

  label145 = gtk_label_new (_("Text Color"));
  gtk_widget_ref (label145);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label145", label145,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label145);
  gtk_table_attach (GTK_TABLE (table13), label145, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label145), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label145), 0, 0.5);

  label103 = gtk_label_new (_("Current Verse Color"));
  gtk_widget_ref (label103);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label103", label103,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label103);
  gtk_table_attach (GTK_TABLE (table13), label103, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label103), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label103), 0, 0.5);

  label150 = gtk_label_new (_("Current Verse Background Color"));
  gtk_widget_ref (label150);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label150", label150,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label150);
  gtk_table_attach (GTK_TABLE (table13), label150, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (label150, 218, -2);
  gtk_label_set_justify (GTK_LABEL (label150), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label150), 0, 0.5);

  label146 = gtk_label_new (_("Verse Numbers"));
  gtk_widget_ref (label146);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label146", label146,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label146);
  gtk_table_attach (GTK_TABLE (table13), label146, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label146), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label146), 0, 0.5);

  label152 = gtk_label_new (_("Verse Number Font Size"));
  gtk_widget_ref (label152);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label152", label152,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label152);
  gtk_table_attach (GTK_TABLE (table13), label152, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label152), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label152), 0, 0.5);

  label147 = gtk_label_new (_("Link Color"));
  gtk_widget_ref (label147);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label147", label147,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label147);
  gtk_table_attach (GTK_TABLE (table13), label147, 0, 1, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label147), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label147), 0, 0.5);

  label174 = gtk_label_new (_("Greek Font"));
  gtk_widget_ref (label174);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label174", label174,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label174);
  gtk_table_attach (GTK_TABLE (table13), label174, 0, 1, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label174), 0, 0.5);

  label175 = gtk_label_new (_("Hebrew Font"));
  gtk_widget_ref (label175);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label175", label175,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label175);
  gtk_table_attach (GTK_TABLE (table13), label175, 0, 1, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label175), 0, 0.5);

  label176 = gtk_label_new (_("Unicode Font"));
  gtk_widget_ref (label176);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label176", label176,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label176);
  gtk_table_attach (GTK_TABLE (table13), label176, 0, 1, 11, 12,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label176), 0, 0.5);

  fpDefaultFont = gnome_font_picker_new ();
  gtk_widget_ref (fpDefaultFont);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "fpDefaultFont", fpDefaultFont,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fpDefaultFont);
  gtk_table_attach (GTK_TABLE (table13), fpDefaultFont, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gnome_font_picker_set_mode (GNOME_FONT_PICKER (fpDefaultFont), GNOME_FONT_PICKER_MODE_FONT_INFO);

  gcpTextBG = gnome_color_picker_new ();
  gtk_widget_ref (gcpTextBG);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "gcpTextBG", gcpTextBG,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (gcpTextBG);
  gtk_table_attach (GTK_TABLE (table13), gcpTextBG, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gnome_color_picker_set_title (GNOME_COLOR_PICKER (gcpTextBG), _("Pick a color for Background"));

  gcpText = gnome_color_picker_new ();
  gtk_widget_ref (gcpText);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "gcpText", gcpText,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (gcpText);
  gtk_table_attach (GTK_TABLE (table13), gcpText, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gnome_color_picker_set_title (GNOME_COLOR_PICKER (gcpText), _("Pick a color for Bible Text"));

  cmbTextFontSize = gtk_combo_new ();
  gtk_widget_ref (cmbTextFontSize);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cmbTextFontSize", cmbTextFontSize,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cmbTextFontSize);
  gtk_table_attach (GTK_TABLE (table13), cmbTextFontSize, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gtk_combo_set_value_in_list (GTK_COMBO (cmbTextFontSize), TRUE, TRUE);
  cmbTextFontSize_items = g_list_append (cmbTextFontSize_items, (gpointer) _("-2"));
  cmbTextFontSize_items = g_list_append (cmbTextFontSize_items, (gpointer) _("-1"));
  cmbTextFontSize_items = g_list_append (cmbTextFontSize_items, (gpointer) _("+0"));
  cmbTextFontSize_items = g_list_append (cmbTextFontSize_items, (gpointer) _("+1"));
  cmbTextFontSize_items = g_list_append (cmbTextFontSize_items, (gpointer) _("+2"));
  cmbTextFontSize_items = g_list_append (cmbTextFontSize_items, (gpointer) _("+3"));
  cmbTextFontSize_items = g_list_append (cmbTextFontSize_items, (gpointer) _("+4"));
  cmbTextFontSize_items = g_list_append (cmbTextFontSize_items, (gpointer) _("+5\r"));
  gtk_combo_set_popdown_strings (GTK_COMBO (cmbTextFontSize), cmbTextFontSize_items);
  g_list_free (cmbTextFontSize_items);

  cmbEntryTextSize = GTK_COMBO (cmbTextFontSize)->entry;
  gtk_widget_ref (cmbEntryTextSize);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cmbEntryTextSize", cmbEntryTextSize,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cmbEntryTextSize);
  gtk_tooltips_set_tip (tooltips, cmbEntryTextSize, _("Zero is base font size Go up or down from there"), NULL);
  gtk_entry_set_text (GTK_ENTRY (cmbEntryTextSize), _("+0"));

  gcpCurrentverseBG = gnome_color_picker_new ();
  gtk_widget_ref (gcpCurrentverseBG);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "gcpCurrentverseBG", gcpCurrentverseBG,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (gcpCurrentverseBG);
  gtk_table_attach (GTK_TABLE (table13), gcpCurrentverseBG, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gnome_color_picker_set_title (GNOME_COLOR_PICKER (gcpCurrentverseBG), _("Pick a color - CurrentVerse BG"));

  gcpCurrentverse = gnome_color_picker_new ();
  gtk_widget_ref (gcpCurrentverse);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "gcpCurrentverse", gcpCurrentverse,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (gcpCurrentverse);
  gtk_table_attach (GTK_TABLE (table13), gcpCurrentverse, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gnome_color_picker_set_title (GNOME_COLOR_PICKER (gcpCurrentverse), _("Pick a color for Current Verse"));

  gcpTextVerseNums = gnome_color_picker_new ();
  gtk_widget_ref (gcpTextVerseNums);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "gcpTextVerseNums", gcpTextVerseNums,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (gcpTextVerseNums);
  gtk_table_attach (GTK_TABLE (table13), gcpTextVerseNums, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gtk_widget_set_usize (gcpTextVerseNums, 41, -2);

  gcpTextLinks = gnome_color_picker_new ();
  gtk_widget_ref (gcpTextLinks);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "gcpTextLinks", gcpTextLinks,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (gcpTextLinks);
  gtk_table_attach (GTK_TABLE (table13), gcpTextLinks, 1, 2, 8, 9,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gtk_tooltips_set_tip (tooltips, gcpTextLinks, _("Strongs Numbers & Morph Tags"), NULL);

  fpGreekFont = gnome_font_picker_new ();
  gtk_widget_ref (fpGreekFont);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "fpGreekFont", fpGreekFont,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fpGreekFont);
  gtk_table_attach (GTK_TABLE (table13), fpGreekFont, 1, 2, 9, 10,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gnome_font_picker_set_mode (GNOME_FONT_PICKER (fpGreekFont), GNOME_FONT_PICKER_MODE_FONT_INFO);

  fpHebrewFont = gnome_font_picker_new ();
  gtk_widget_ref (fpHebrewFont);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "fpHebrewFont", fpHebrewFont,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fpHebrewFont);
  gtk_table_attach (GTK_TABLE (table13), fpHebrewFont, 1, 2, 10, 11,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gnome_font_picker_set_mode (GNOME_FONT_PICKER (fpHebrewFont), GNOME_FONT_PICKER_MODE_FONT_INFO);

  fpUnicodeFont = gnome_font_picker_new ();
  gtk_widget_ref (fpUnicodeFont);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "fpUnicodeFont", fpUnicodeFont,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fpUnicodeFont);
  gtk_table_attach (GTK_TABLE (table13), fpUnicodeFont, 1, 2, 11, 12,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gnome_font_picker_set_mode (GNOME_FONT_PICKER (fpUnicodeFont), GNOME_FONT_PICKER_MODE_FONT_INFO);

  cmbVerseNumSize = gtk_combo_new ();
  gtk_widget_ref (cmbVerseNumSize);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cmbVerseNumSize", cmbVerseNumSize,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cmbVerseNumSize);
  gtk_table_attach (GTK_TABLE (table13), cmbVerseNumSize, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gtk_combo_set_value_in_list (GTK_COMBO (cmbVerseNumSize), TRUE, TRUE);
  cmbVerseNumSize_items = g_list_append (cmbVerseNumSize_items, (gpointer) _("-2"));
  cmbVerseNumSize_items = g_list_append (cmbVerseNumSize_items, (gpointer) _("-1"));
  cmbVerseNumSize_items = g_list_append (cmbVerseNumSize_items, (gpointer) _("+0"));
  cmbVerseNumSize_items = g_list_append (cmbVerseNumSize_items, (gpointer) _("+1"));
  cmbVerseNumSize_items = g_list_append (cmbVerseNumSize_items, (gpointer) _("+2"));
  cmbVerseNumSize_items = g_list_append (cmbVerseNumSize_items, (gpointer) _("+3"));
  cmbVerseNumSize_items = g_list_append (cmbVerseNumSize_items, (gpointer) _("+4"));
  cmbVerseNumSize_items = g_list_append (cmbVerseNumSize_items, (gpointer) _("+5\r"));
  gtk_combo_set_popdown_strings (GTK_COMBO (cmbVerseNumSize), cmbVerseNumSize_items);
  g_list_free (cmbVerseNumSize_items);

  cmbEentryVNSize = GTK_COMBO (cmbVerseNumSize)->entry;
  gtk_widget_ref (cmbEentryVNSize);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cmbEentryVNSize", cmbEentryVNSize,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cmbEentryVNSize);
  gtk_tooltips_set_tip (tooltips, cmbEentryVNSize, _("Zero is base font size Go up or down from there"), NULL);
  gtk_entry_set_text (GTK_ENTRY (cmbEentryVNSize), _("+0"));

  label173 = gtk_label_new (_("Default Font"));
  gtk_widget_ref (label173);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label173", label173,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label173);
  gtk_table_attach (GTK_TABLE (table13), label173, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label173), 0, 0.5);

  btnSetToGSDefaults = gtk_button_new_with_label (_("GnomeSword Defaults"));
  gtk_widget_ref (btnSetToGSDefaults);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "btnSetToGSDefaults", btnSetToGSDefaults,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnSetToGSDefaults);
  gtk_box_pack_start (GTK_BOX (vbox35), btnSetToGSDefaults, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, btnSetToGSDefaults, _("Use GnomeSword defaults for all settings on this page"), NULL);
  gtk_button_set_relief (GTK_BUTTON (btnSetToGSDefaults), GTK_RELIEF_HALF);

  label98 = gtk_label_new (_("Bible Text Window"));
  gtk_widget_ref (label98);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label98", label98,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label98);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook7), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook7), 0), label98);

  scrolledwindow55 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow55);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "scrolledwindow55", scrolledwindow55,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow55);
  gtk_container_add (GTK_CONTAINER (notebook7), scrolledwindow55);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow55), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport8 = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (viewport8);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "viewport8", viewport8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (viewport8);
  gtk_container_add (GTK_CONTAINER (scrolledwindow55), viewport8);

  vbox41 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox41);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox41", vbox41,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox41);
  gtk_container_add (GTK_CONTAINER (viewport8), vbox41);

  hbox64 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox64);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "hbox64", hbox64,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox64);
  gtk_box_pack_start (GTK_BOX (vbox41), hbox64, TRUE, TRUE, 0);

  vbox28 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox28);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox28", vbox28,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox28);
  gtk_box_pack_start (GTK_BOX (hbox64), vbox28, TRUE, TRUE, 0);

  frame21 = gtk_frame_new (_("Use Defaults When Opening GnomeSword"));
  gtk_widget_ref (frame21);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "frame21", frame21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame21);
  gtk_box_pack_start (GTK_BOX (vbox28), frame21, FALSE, TRUE, 0);

  vbox36 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox36);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox36", vbox36,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox36);
  gtk_container_add (GTK_CONTAINER (frame21), vbox36);

  rbtnUsedefaults = gtk_radio_button_new_with_label (vbox36_group, _("Yes, use defaults that I select"));
  vbox36_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbtnUsedefaults));
  gtk_widget_ref (rbtnUsedefaults);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "rbtnUsedefaults", rbtnUsedefaults,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbtnUsedefaults);
  gtk_box_pack_start (GTK_BOX (vbox36), rbtnUsedefaults, FALSE, FALSE, 0);

  rbtnNoDefaults = gtk_radio_button_new_with_label (vbox36_group, _("No, use settings saved on last GnomeSword run"));
  vbox36_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbtnNoDefaults));
  gtk_widget_ref (rbtnNoDefaults);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "rbtnNoDefaults", rbtnNoDefaults,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbtnNoDefaults);
  gtk_box_pack_start (GTK_BOX (vbox36), rbtnNoDefaults, FALSE, FALSE, 0);

  frame42 = gtk_frame_new (_("Show Tabs"));
  gtk_widget_ref (frame42);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "frame42", frame42,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame42);
  gtk_box_pack_start (GTK_BOX (vbox28), frame42, TRUE, TRUE, 0);

  vbox54 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox54);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox54", vbox54,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox54);
  gtk_container_add (GTK_CONTAINER (frame42), vbox54);

  cbtnShowBibletabs = gtk_check_button_new_with_label (_("Bible Texts"));
  gtk_widget_ref (cbtnShowBibletabs);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowBibletabs", cbtnShowBibletabs,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowBibletabs);
  gtk_box_pack_start (GTK_BOX (vbox54), cbtnShowBibletabs, FALSE, FALSE, 0);

  cbtnShowCOMtabs = gtk_check_button_new_with_label (_("Commentary"));
  gtk_widget_ref (cbtnShowCOMtabs);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowCOMtabs", cbtnShowCOMtabs,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowCOMtabs);
  gtk_box_pack_start (GTK_BOX (vbox54), cbtnShowCOMtabs, FALSE, FALSE, 0);

  cbtnShowDLtabs = gtk_check_button_new_with_label (_("Dict/Lex"));
  gtk_widget_ref (cbtnShowDLtabs);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowDLtabs", cbtnShowDLtabs,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowDLtabs);
  gtk_box_pack_start (GTK_BOX (vbox54), cbtnShowDLtabs, FALSE, FALSE, 0);

  cbtnShowBooktabs = gtk_check_button_new_with_label (_("Books"));
  gtk_widget_ref (cbtnShowBooktabs);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowBooktabs", cbtnShowBooktabs,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowBooktabs);
  gtk_box_pack_start (GTK_BOX (vbox54), cbtnShowBooktabs, FALSE, FALSE, 0);

  frame43 = gtk_frame_new (_("Misc"));
  gtk_widget_ref (frame43);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "frame43", frame43,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame43);
  gtk_box_pack_start (GTK_BOX (vbox28), frame43, FALSE, FALSE, 0);

  vbox55 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox55);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox55", vbox55,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox55);
  gtk_container_add (GTK_CONTAINER (frame43), vbox55);

  checkbutton9 = gtk_check_button_new_with_label (_("Use Verse Style"));
  gtk_widget_ref (checkbutton9);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "checkbutton9", checkbutton9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (checkbutton9);
  gtk_box_pack_start (GTK_BOX (vbox55), checkbutton9, FALSE, FALSE, 0);
  gtk_widget_set_usize (checkbutton9, 202, -2);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton9), TRUE);

  cbtnShowDevotion = gtk_check_button_new_with_label (_("Show Daily Devotion"));
  gtk_widget_ref (cbtnShowDevotion);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowDevotion", cbtnShowDevotion,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowDevotion);
  gtk_box_pack_start (GTK_BOX (vbox55), cbtnShowDevotion, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, cbtnShowDevotion, _("Show Daily Devotion if you have a Devotion module"), NULL);

  cbtnUseDefaultDict = gtk_check_button_new_with_label (_("Use Default Dictionary"));
  gtk_widget_ref (cbtnUseDefaultDict);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnUseDefaultDict", cbtnUseDefaultDict,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnUseDefaultDict);
  gtk_box_pack_start (GTK_BOX (vbox55), cbtnUseDefaultDict, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, cbtnUseDefaultDict, _("Use default dictionary when a word is left clicked"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cbtnUseDefaultDict), TRUE);

  frame41 = gtk_frame_new (_("where to View Dict/Lex When Link or Word Clicked"));
  gtk_widget_ref (frame41);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "frame41", frame41,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame41);
  gtk_box_pack_start (GTK_BOX (vbox28), frame41, FALSE, FALSE, 0);

  vbox53 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox53);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox53", vbox53,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox53);
  gtk_container_add (GTK_CONTAINER (frame41), vbox53);

  cbtnInViewer = gtk_check_button_new_with_label (_("In Shortcut bar Viewer"));
  gtk_widget_ref (cbtnInViewer);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnInViewer", cbtnInViewer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnInViewer);
  gtk_box_pack_start (GTK_BOX (vbox53), cbtnInViewer, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cbtnInViewer), TRUE);

  cbtnInDictPane = gtk_check_button_new_with_label (_("In Dict/Lex Window"));
  gtk_widget_ref (cbtnInDictPane);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnInDictPane", cbtnInDictPane,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnInDictPane);
  gtk_box_pack_start (GTK_BOX (vbox53), cbtnInDictPane, FALSE, FALSE, 0);

  frame25 = gtk_frame_new (_("Personal Comments"));
  gtk_widget_ref (frame25);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "frame25", frame25,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame25);
  gtk_box_pack_start (GTK_BOX (vbox28), frame25, TRUE, TRUE, 0);

  vbox37 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox37);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox37", vbox37,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox37);
  gtk_container_add (GTK_CONTAINER (frame25), vbox37);

  cbtnPNformat = gtk_check_button_new_with_label (_("Use Formatting"));
  gtk_widget_ref (cbtnPNformat);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnPNformat", cbtnPNformat,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnPNformat);
  gtk_box_pack_start (GTK_BOX (vbox37), cbtnPNformat, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, cbtnPNformat, _("Use HTML tags to format notes"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cbtnPNformat), TRUE);

  checkbutton8 = gtk_check_button_new_with_label (_("Auto Save"));
  gtk_widget_ref (checkbutton8);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "checkbutton8", checkbutton8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (checkbutton8);
  gtk_box_pack_start (GTK_BOX (vbox37), checkbutton8, FALSE, FALSE, 0);
  gtk_widget_set_usize (checkbutton8, 202, -2);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton8), TRUE);

  label123 = gtk_label_new (_("Interface"));
  gtk_widget_ref (label123);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label123", label123,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label123);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook7), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook7), 1), label123);

  scrolledwindow54 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow54);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "scrolledwindow54", scrolledwindow54,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow54);
  gtk_container_add (GTK_CONTAINER (notebook7), scrolledwindow54);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow54), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport7 = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (viewport7);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "viewport7", viewport7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (viewport7);
  gtk_container_add (GTK_CONTAINER (scrolledwindow54), viewport7);

  frame27 = gtk_frame_new (_("Layout"));
  gtk_widget_ref (frame27);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "frame27", frame27,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame27);
  gtk_container_add (GTK_CONTAINER (viewport7), frame27);

  table10 = gtk_table_new (5, 2, FALSE);
  gtk_widget_ref (table10);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "table10", table10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table10);
  gtk_container_add (GTK_CONTAINER (frame27), table10);

  label165 = gtk_label_new (_("App Width"));
  gtk_widget_ref (label165);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label165", label165,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label165);
  gtk_table_attach (GTK_TABLE (table10), label165, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label165), 0, 0.5);

  label166 = gtk_label_new (_("ShortCut Bar Width"));
  gtk_widget_ref (label166);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label166", label166,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label166);
  gtk_table_attach (GTK_TABLE (table10), label166, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label166), 0, 0.5);

  label167 = gtk_label_new (_("Upper Pane Height"));
  gtk_widget_ref (label167);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label167", label167,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label167);
  gtk_table_attach (GTK_TABLE (table10), label167, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label167), 0, 0.5);

  sbtnAppWidth = gtk_spin_button_new (GTK_ADJUSTMENT (sbtnAppWidth_adj), 1, 0);
  gtk_widget_ref (sbtnAppWidth);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "sbtnAppWidth", sbtnAppWidth,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sbtnAppWidth);
  gtk_table_attach (GTK_TABLE (table10), sbtnAppWidth, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  sbtnSBWidth = gtk_spin_button_new (GTK_ADJUSTMENT (sbtnSBWidth_adj), 1, 0);
  gtk_widget_ref (sbtnSBWidth);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "sbtnSBWidth", sbtnSBWidth,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sbtnSBWidth);
  gtk_table_attach (GTK_TABLE (table10), sbtnSBWidth, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  sbtnUpPaneHight = gtk_spin_button_new (GTK_ADJUSTMENT (sbtnUpPaneHight_adj), 1, 0);
  gtk_widget_ref (sbtnUpPaneHight);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "sbtnUpPaneHight", sbtnUpPaneHight,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sbtnUpPaneHight);
  gtk_table_attach (GTK_TABLE (table10), sbtnUpPaneHight, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label169 = gtk_label_new (_("Bible Pane Width"));
  gtk_widget_ref (label169);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label169", label169,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label169);
  gtk_table_attach (GTK_TABLE (table10), label169, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label169), 0, 0.5);

  sbtnTextWidth = gtk_spin_button_new (GTK_ADJUSTMENT (sbtnTextWidth_adj), 1, 0);
  gtk_widget_ref (sbtnTextWidth);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "sbtnTextWidth", sbtnTextWidth,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sbtnTextWidth);
  gtk_table_attach (GTK_TABLE (table10), sbtnTextWidth, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label168 = gtk_label_new (_("App Height"));
  gtk_widget_ref (label168);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label168", label168,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label168);
  gtk_table_attach (GTK_TABLE (table10), label168, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label168), 0, 0.5);

  sbtnAppHight = gtk_spin_button_new (GTK_ADJUSTMENT (sbtnAppHight_adj), 1, 0);
  gtk_widget_ref (sbtnAppHight);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "sbtnAppHight", sbtnAppHight,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sbtnAppHight);
  gtk_table_attach (GTK_TABLE (table10), sbtnAppHight, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label190 = gtk_label_new (_("label190"));
  gtk_widget_ref (label190);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label190", label190,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label190);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook7), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook7), 2), label190);

  scrolledwindow53 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow53);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "scrolledwindow53", scrolledwindow53,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow53);
  gtk_container_add (GTK_CONTAINER (notebook7), scrolledwindow53);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow53), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport6 = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (viewport6);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "viewport6", viewport6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (viewport6);
  gtk_container_add (GTK_CONTAINER (scrolledwindow53), viewport6);

  vbox48 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox48);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox48", vbox48,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox48);
  gtk_container_add (GTK_CONTAINER (viewport6), vbox48);

  frame24 = gtk_frame_new (_("Shortcut Bar"));
  gtk_widget_ref (frame24);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "frame24", frame24,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame24);
  gtk_box_pack_start (GTK_BOX (vbox48), frame24, TRUE, TRUE, 0);

  vbox29 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox29);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "vbox29", vbox29,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox29);
  gtk_container_add (GTK_CONTAINER (frame24), vbox29);

  cbtnShowSCB = gtk_check_button_new_with_label (_("Show Shortcut bar"));
  gtk_widget_ref (cbtnShowSCB);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowSCB", cbtnShowSCB,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowSCB);
  gtk_box_pack_start (GTK_BOX (vbox29), cbtnShowSCB, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cbtnShowSCB), TRUE);

  cbtnFavoritesGroup = gtk_check_button_new_with_label (_("Show Favorites Group"));
  gtk_widget_ref (cbtnFavoritesGroup);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnFavoritesGroup", cbtnFavoritesGroup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnFavoritesGroup);
  gtk_box_pack_start (GTK_BOX (vbox29), cbtnFavoritesGroup, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cbtnFavoritesGroup), TRUE);

  cbtnShowTextgroup = gtk_check_button_new_with_label (_("Show Bible Text Group"));
  gtk_widget_ref (cbtnShowTextgroup);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowTextgroup", cbtnShowTextgroup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowTextgroup);
  gtk_box_pack_start (GTK_BOX (vbox29), cbtnShowTextgroup, FALSE, FALSE, 0);

  cbtnShowComGroup = gtk_check_button_new_with_label (_("Show Commentary Group"));
  gtk_widget_ref (cbtnShowComGroup);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowComGroup", cbtnShowComGroup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowComGroup);
  gtk_box_pack_start (GTK_BOX (vbox29), cbtnShowComGroup, FALSE, FALSE, 0);

  cbtnShowDictGroup = gtk_check_button_new_with_label (_("Show Dict/Lex Group"));
  gtk_widget_ref (cbtnShowDictGroup);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowDictGroup", cbtnShowDictGroup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowDictGroup);
  gtk_box_pack_start (GTK_BOX (vbox29), cbtnShowDictGroup, FALSE, FALSE, 0);

  cbtnShowBookGroup = gtk_check_button_new_with_label (_("Show Books Group"));
  gtk_widget_ref (cbtnShowBookGroup);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowBookGroup", cbtnShowBookGroup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowBookGroup);
  gtk_box_pack_start (GTK_BOX (vbox29), cbtnShowBookGroup, FALSE, FALSE, 0);

  cbtnShowHistoryGroup = gtk_check_button_new_with_label (_("Show History Group"));
  gtk_widget_ref (cbtnShowHistoryGroup);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "cbtnShowHistoryGroup", cbtnShowHistoryGroup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbtnShowHistoryGroup);
  gtk_box_pack_start (GTK_BOX (vbox29), cbtnShowHistoryGroup, FALSE, FALSE, 0);

  checkbuttonSBDock = gtk_check_button_new_with_label (_("Dock Shortcut Bar"));
  gtk_widget_ref (checkbuttonSBDock);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "checkbuttonSBDock", checkbuttonSBDock,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (checkbuttonSBDock);
  gtk_box_pack_start (GTK_BOX (vbox29), checkbuttonSBDock, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonSBDock), TRUE);

  label189 = gtk_label_new (_("label189"));
  gtk_widget_ref (label189);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label189", label189,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label189);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook7), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook7), 3), label189);

  scrolledwindow52 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow52);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "scrolledwindow52", scrolledwindow52,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow52);
  gtk_container_add (GTK_CONTAINER (notebook7), scrolledwindow52);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow52), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport5 = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (viewport5);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "viewport5", viewport5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (viewport5);
  gtk_container_add (GTK_CONTAINER (scrolledwindow52), viewport5);

  table9 = gtk_table_new (14, 2, FALSE);
  gtk_widget_ref (table9);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "table9", table9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table9);
  gtk_container_add (GTK_CONTAINER (viewport5), table9);

  label156 = gtk_label_new (_("Main Text Module"));
  gtk_widget_ref (label156);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label156", label156,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label156);
  gtk_table_attach (GTK_TABLE (table9), label156, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  combo17 = gtk_combo_new ();
  gtk_widget_ref (combo17);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo17", combo17,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo17);
  gtk_table_attach (GTK_TABLE (table9), combo17, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry3 = GTK_COMBO (combo17)->entry;
  gtk_widget_ref (entry3);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry3", entry3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry3);

  combo18 = gtk_combo_new ();
  gtk_widget_ref (combo18);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo18", combo18,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo18);
  gtk_table_attach (GTK_TABLE (table9), combo18, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry4 = GTK_COMBO (combo18)->entry;
  gtk_widget_ref (entry4);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry4", entry4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry4);

  combo19 = gtk_combo_new ();
  gtk_widget_ref (combo19);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo19", combo19,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo19);
  gtk_table_attach (GTK_TABLE (table9), combo19, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry5 = GTK_COMBO (combo19)->entry;
  gtk_widget_ref (entry5);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry5", entry5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry5);

  combo20 = gtk_combo_new ();
  gtk_widget_ref (combo20);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo20", combo20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo20);
  gtk_table_attach (GTK_TABLE (table9), combo20, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry6 = GTK_COMBO (combo20)->entry;
  gtk_widget_ref (entry6);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry6", entry6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry6);

  label157 = gtk_label_new (_("Commentary"));
  gtk_widget_ref (label157);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label157", label157,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label157);
  gtk_table_attach (GTK_TABLE (table9), label157, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label157), 0, 0.5);

  label158 = gtk_label_new (_("Dict/Lex"));
  gtk_widget_ref (label158);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label158", label158,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label158);
  gtk_table_attach (GTK_TABLE (table9), label158, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label158), 0, 0.5);

  label159 = gtk_label_new (_("Personal"));
  gtk_widget_ref (label159);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label159", label159,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label159);
  gtk_table_attach (GTK_TABLE (table9), label159, 0, 1, 13, 14,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label159), 0, 0.5);

  combo21 = gtk_combo_new ();
  gtk_widget_ref (combo21);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo21", combo21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo21);
  gtk_table_attach (GTK_TABLE (table9), combo21, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry7 = GTK_COMBO (combo21)->entry;
  gtk_widget_ref (entry7);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry7", entry7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry7);

  combo22 = gtk_combo_new ();
  gtk_widget_ref (combo22);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo22", combo22,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo22);
  gtk_table_attach (GTK_TABLE (table9), combo22, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry8 = GTK_COMBO (combo22)->entry;
  gtk_widget_ref (entry8);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry8", entry8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry8);

  combo23 = gtk_combo_new ();
  gtk_widget_ref (combo23);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo23", combo23,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo23);
  gtk_table_attach (GTK_TABLE (table9), combo23, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry9 = GTK_COMBO (combo23)->entry;
  gtk_widget_ref (entry9);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry9", entry9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry9);

  combo25 = gtk_combo_new ();
  gtk_widget_ref (combo25);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo25", combo25,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo25);
  gtk_table_attach (GTK_TABLE (table9), combo25, 1, 2, 13, 14,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry11 = GTK_COMBO (combo25)->entry;
  gtk_widget_ref (entry11);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry11", entry11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry11);

  label160 = gtk_label_new (_("Interlinear 5"));
  gtk_widget_ref (label160);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label160", label160,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label160);
  gtk_table_attach (GTK_TABLE (table9), label160, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label160), 0, 0.5);

  label161 = gtk_label_new (_("Interlinear 4"));
  gtk_widget_ref (label161);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label161", label161,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label161);
  gtk_table_attach (GTK_TABLE (table9), label161, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label161), 0, 0.5);

  label162 = gtk_label_new (_("Interlinear 3"));
  gtk_widget_ref (label162);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label162", label162,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label162);
  gtk_table_attach (GTK_TABLE (table9), label162, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label162), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label162), 7.45058e-09, 7.45058e-09);

  label163 = gtk_label_new (_("Interlinear 2"));
  gtk_widget_ref (label163);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label163", label163,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label163);
  gtk_table_attach (GTK_TABLE (table9), label163, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label163), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label163), 7.45058e-09, 7.45058e-09);

  label164 = gtk_label_new (_("Interlinear 1"));
  gtk_widget_ref (label164);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label164", label164,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label164);
  gtk_table_attach (GTK_TABLE (table9), label164, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label164), 1.2666e-07, 7.45058e-09);

  labelDevotional = gtk_label_new (_("Daily Devotional"));
  gtk_widget_ref (labelDevotional);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "labelDevotional", labelDevotional,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (labelDevotional);
  gtk_table_attach (GTK_TABLE (table9), labelDevotional, 0, 1, 14, 15,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelDevotional), 0, 0.5);

  comboDevotion = gtk_combo_new ();
  gtk_widget_ref (comboDevotion);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "comboDevotion", comboDevotion,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (comboDevotion);
  gtk_table_attach (GTK_TABLE (table9), comboDevotion, 1, 2, 14, 15,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryDevotion = GTK_COMBO (comboDevotion)->entry;
  gtk_widget_ref (entryDevotion);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entryDevotion", entryDevotion,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryDevotion);

  label191 = gtk_label_new (_("Greek Lexicon"));
  gtk_widget_ref (label191);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label191", label191,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label191);
  gtk_table_attach (GTK_TABLE (table9), label191, 0, 1, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label191), 0, 0.5);

  combo24 = gtk_combo_new ();
  gtk_widget_ref (combo24);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo24", combo24,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo24);
  gtk_table_attach (GTK_TABLE (table9), combo24, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry10 = GTK_COMBO (combo24)->entry;
  gtk_widget_ref (entry10);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry10", entry10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry10);

  combo26 = gtk_combo_new ();
  gtk_widget_ref (combo26);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo26", combo26,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo26);
  gtk_table_attach (GTK_TABLE (table9), combo26, 1, 2, 9, 10,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry13 = GTK_COMBO (combo26)->entry;
  gtk_widget_ref (entry13);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry13", entry13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry13);
  gtk_tooltips_set_tip (tooltips, entry13, _("Which Greek Lexicon to display in Dict/Lex Window when a link or word is clicked"), NULL);

  label192 = gtk_label_new (_("Hebrew Lexicon"));
  gtk_widget_ref (label192);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label192", label192,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label192);
  gtk_table_attach (GTK_TABLE (table9), label192, 0, 1, 11, 12,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label192), 0, 0.5);

  combo27 = gtk_combo_new ();
  gtk_widget_ref (combo27);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo27", combo27,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo27);
  gtk_table_attach (GTK_TABLE (table9), combo27, 1, 2, 11, 12,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry14 = GTK_COMBO (combo27)->entry;
  gtk_widget_ref (entry14);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entry14", entry14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry14);
  gtk_tooltips_set_tip (tooltips, entry14, _("Which Hebrew Lexicon to display in Dict/Lex Window when a link or word is clicked"), NULL);

  label202 = gtk_label_new (_("Greek Lex Viewer"));
  gtk_widget_ref (label202);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label202", label202,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label202);
  gtk_table_attach (GTK_TABLE (table9), label202, 0, 1, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label202), 0, 0.5);

  label203 = gtk_label_new (_("Hebrew Lex Viewer"));
  gtk_widget_ref (label203);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label203", label203,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label203);
  gtk_table_attach (GTK_TABLE (table9), label203, 0, 1, 12, 13,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label203), 0, 0.5);

  comboGreekViewer = gtk_combo_new ();
  gtk_widget_ref (comboGreekViewer);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "comboGreekViewer", comboGreekViewer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (comboGreekViewer);
  gtk_table_attach (GTK_TABLE (table9), comboGreekViewer, 1, 2, 10, 11,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  combo_entryGreekViewer = GTK_COMBO (comboGreekViewer)->entry;
  gtk_widget_ref (combo_entryGreekViewer);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo_entryGreekViewer", combo_entryGreekViewer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entryGreekViewer);
  gtk_tooltips_set_tip (tooltips, combo_entryGreekViewer, _("Which Greek Lecicon to display in viewer when a link or word is clicked"), NULL);

  comboHebViewer = gtk_combo_new ();
  gtk_widget_ref (comboHebViewer);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "comboHebViewer", comboHebViewer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (comboHebViewer);
  gtk_table_attach (GTK_TABLE (table9), comboHebViewer, 1, 2, 12, 13,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  combo_entryHebViewer = GTK_COMBO (comboHebViewer)->entry;
  gtk_widget_ref (combo_entryHebViewer);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "combo_entryHebViewer", combo_entryHebViewer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entryHebViewer);
  gtk_tooltips_set_tip (tooltips, combo_entryHebViewer, _("Which Hebrew Lexicon to display in viewer when a link or word is clicked"), NULL);

  label204 = gtk_label_new (_("Default Dictionary"));
  gtk_widget_ref (label204);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label204", label204,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label204);
  gtk_table_attach (GTK_TABLE (table9), label204, 0, 1, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label204), 0, 0.5);

  comboDefaultDict = gtk_combo_new ();
  gtk_widget_ref (comboDefaultDict);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "comboDefaultDict", comboDefaultDict,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (comboDefaultDict);
  gtk_table_attach (GTK_TABLE (table9), comboDefaultDict, 1, 2, 8, 9,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryDefaultDict = GTK_COMBO (comboDefaultDict)->entry;
  gtk_widget_ref (entryDefaultDict);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "entryDefaultDict", entryDefaultDict,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryDefaultDict);
  gtk_tooltips_set_tip (tooltips, entryDefaultDict, _("Dictionary to use when words are selected by left clicking"), NULL);




  label155 = gtk_label_new (_("Sword Modules"));
  gtk_widget_ref (label155);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "label155", label155,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label155);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook7), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook7), 4), label155);

  dialog_action_area9 = GNOME_DIALOG (dlgSettings)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlgSettings), "dialog_action_area9", dialog_action_area9);
  gtk_widget_show (dialog_action_area9);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area9), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area9), 8);

  gnome_dialog_append_button (GNOME_DIALOG (dlgSettings), GNOME_STOCK_BUTTON_OK);
  btnPropertyboxOK = GTK_WIDGET (g_list_last (GNOME_DIALOG (dlgSettings)->buttons)->data);
  gtk_widget_ref (btnPropertyboxOK);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "btnPropertyboxOK", btnPropertyboxOK,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnPropertyboxOK);
  gtk_widget_set_sensitive (btnPropertyboxOK, FALSE);
  GTK_WIDGET_SET_FLAGS (btnPropertyboxOK, GTK_CAN_DEFAULT);

  gnome_dialog_append_button (GNOME_DIALOG (dlgSettings), GNOME_STOCK_BUTTON_APPLY);
  btnPropertyboxApply = GTK_WIDGET (g_list_last (GNOME_DIALOG (dlgSettings)->buttons)->data);
  gtk_widget_ref (btnPropertyboxApply);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "btnPropertyboxApply", btnPropertyboxApply,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnPropertyboxApply);
  gtk_widget_set_sensitive (btnPropertyboxApply, FALSE);
  GTK_WIDGET_SET_FLAGS (btnPropertyboxApply, GTK_CAN_DEFAULT);

  gnome_dialog_append_button (GNOME_DIALOG (dlgSettings), GNOME_STOCK_BUTTON_CANCEL);
  btnPropertyboxCancel = GTK_WIDGET (g_list_last (GNOME_DIALOG (dlgSettings)->buttons)->data);
  gtk_widget_ref (btnPropertyboxCancel);
  gtk_object_set_data_full (GTK_OBJECT (dlgSettings), "btnPropertyboxCancel", btnPropertyboxCancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnPropertyboxCancel);
  GTK_WIDGET_SET_FLAGS (btnPropertyboxCancel, GTK_CAN_DEFAULT);


/******************************************************************************** start shortcut bar */
	//gtk_box_pack_end(GTK_BOX(hbox22), notebook7, TRUE, TRUE, 0);

	shortcut_model = e_shortcut_model_new();

	shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(shortcut_bar),
				 shortcut_model);
	gtk_widget_set_usize(shortcut_bar, 150, 250);
	gtk_widget_show(shortcut_bar);
	gtk_box_pack_end(GTK_BOX(hbox22), shortcut_bar, FALSE, TRUE, 0);
	gtk_widget_set_usize(shortcut_bar, 162, -2);
/************************************************************************ end shortcut bar */

/************************************************************************** start settings */


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
	gtk_combo_set_popdown_strings(GTK_COMBO(comboDefaultDict), dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(comboGreekViewer), dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo26), dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(comboHebViewer), dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo27), dictlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo25), percomlist);
	gtk_combo_set_popdown_strings(GTK_COMBO(comboDevotion),
				      devotionlist);

	gtk_entry_set_text(GTK_ENTRY(cmbEentryVNSize),
			   s->verse_num_font_size);
	gnome_font_picker_set_font_name((GnomeFontPicker *) fpGreekFont,
					s->greek_font);
	gnome_font_picker_set_font_name((GnomeFontPicker *) fpHebrewFont,
					s->hebrew_font);
	gnome_font_picker_set_font_name((GnomeFontPicker *) fpUnicodeFont,
					s->unicode_font);
	gnome_font_picker_set_font_name((GnomeFontPicker *) fpDefaultFont,
					s->default_font);
	
	/* set toggle buttons */
	if(s->usedefault)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnUsedefaults), 
						TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnNoDefaults), 
						TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowSCB),
				     s->showshortcutbar);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowBibletabs),
				     s->text_tabs);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowCOMtabs),
				     s->comm_tabs);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowDLtabs),
				     s->dict_tabs);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowBooktabs),
				     s->book_tabs);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton9),
				     s->versestyle);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowDevotion),
				     s->showdevotional);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnFavoritesGroup),
				     s->showfavoritesgroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowTextgroup),
				     s->showtextgroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowComGroup),
				     s->showcomgroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowDictGroup),
				     s->showdictgroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowBookGroup),
				     s->showbookgroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowHistoryGroup),
				     s->showhistorygroup);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonSBDock),
				     s->docked);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnPNformat),
				     s->formatpercom);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnInViewer),
				     s->inViewer);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnInDictPane),
				     s->inDictpane);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnUseDefaultDict),
				     s->useDefaultDict);

	/******************************************************************************/
	/*
	   sbtnAppWidth_adj =
	   gtk_adjustment_new(s->gs_width, 640, 10000, 1, 10, 10);
	   sbtnAppHight_adj =
	   gtk_adjustment_new(s->gs_height, 480, 10000, 1, 10, 10);
	   sbtnSBWidth_adj =
	   gtk_adjustment_new(s->shortcutbar_width, 0, 300, 1, 10, 10);
	   sbtnTextWidth_adj =
	   gtk_adjustment_new(s->biblepane_width, 0, 500, 1, 10, 10);
	   sbtnUpPaneHight_adj =
	   gtk_adjustment_new(s->upperpane_hight, 0, 10000, 1, 10, 10);
	 */


	gtk_entry_set_text(GTK_ENTRY(cmbEntryTextSize),
			   s->bible_font_size);
	gtk_entry_set_text(GTK_ENTRY(entry3), s->MainWindowModule);
	gtk_entry_set_text(GTK_ENTRY(entry4), s->Interlinear1Module);
	gtk_entry_set_text(GTK_ENTRY(entry5), s->Interlinear2Module);
	gtk_entry_set_text(GTK_ENTRY(entry6), s->Interlinear3Module);
	gtk_entry_set_text(GTK_ENTRY(entry7), s->Interlinear4Module);
	gtk_entry_set_text(GTK_ENTRY(entry8), s->Interlinear5Module);
	gtk_entry_set_text(GTK_ENTRY(entry9), s->CommWindowModule);
	gtk_entry_set_text(GTK_ENTRY(entry10), s->DictWindowModule);
	gtk_entry_set_text(GTK_ENTRY(entryDefaultDict), s->DefaultDict);
	gtk_entry_set_text(GTK_ENTRY(combo_entryGreekViewer), s->lex_greek_viewer);
	gtk_entry_set_text(GTK_ENTRY(combo_entryHebViewer), s->lex_hebrew_viewer);
	gtk_entry_set_text(GTK_ENTRY(entry11), s->personalcommentsmod);
	gtk_entry_set_text(GTK_ENTRY(entry13), s->lex_greek);
	gtk_entry_set_text(GTK_ENTRY(entry14), s->lex_hebrew);
	gtk_entry_set_text(GTK_ENTRY(entryDevotion), s->devotionalmod);

	/********************************************************************* end settings */
	/*** font pickers ***/
	gtk_signal_connect(GTK_OBJECT(fpGreekFont), "font_set",
			   GTK_SIGNAL_FUNC(on_font_set), NULL);
	gtk_signal_connect(GTK_OBJECT(fpHebrewFont), "font_set",
			   GTK_SIGNAL_FUNC(on_font_set), s);
	gtk_signal_connect(GTK_OBJECT(fpUnicodeFont), "font_set",
			   GTK_SIGNAL_FUNC(on_font_set), s);
	gtk_signal_connect(GTK_OBJECT(fpDefaultFont), "font_set",
			   GTK_SIGNAL_FUNC(on_font_set), s);
	/*** color pickers ***/
	gtk_signal_connect(GTK_OBJECT(gcpTextBG), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(gcpText), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(gcpCurrentverseBG), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(gcpCurrentverse), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(gcpTextVerseNums), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(gcpTextLinks), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	/*** combo entrys  font sizes ***/
	gtk_signal_connect(GTK_OBJECT(cmbEntryTextSize), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(cmbEentryVNSize), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	/*gtk_signal_connect(GTK_OBJECT(btnSetToGSDefaults), "clicked",
	   GTK_SIGNAL_FUNC(on_btnSetToGSDefaults_clicked),
	   NULL); */
	/*** toggle buttons ***/
	gtk_signal_connect(GTK_OBJECT(rbtnUsedefaults), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(rbtnNoDefaults), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(cbtnShowSCB), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(2));
	gtk_signal_connect(GTK_OBJECT(cbtnShowBibletabs), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(cbtnShowCOMtabs), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(cbtnShowDLtabs), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(cbtnShowBooktabs), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(cbtnInViewer), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(cbtnInDictPane), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(cbtnFavoritesGroup), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(cbtnShowTextgroup), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(cbtnShowComGroup), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(cbtnShowDictGroup), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(cbtnShowHistoryGroup), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
  gtk_signal_connect (GTK_OBJECT (cbtnUseDefaultDict), "toggled",
                      GTK_SIGNAL_FUNC (on_button_toggled),
                      GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(cbtnPNformat), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(checkbuttonSBDock), "toggled",
			   GTK_SIGNAL_FUNC(on_btnSBDock_clicked), s);
	gtk_signal_connect(GTK_OBJECT(checkbutton9), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(cbtnShowDevotion), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   GINT_TO_POINTER(0));
	/*** spin buttons layout ***/
	gtk_signal_connect(GTK_OBJECT(sbtnAppWidth), "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(sbtnAppHight), "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(sbtnSBWidth), "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(sbtnTextWidth), "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(sbtnUpPaneHight), "changed",
			   GTK_SIGNAL_FUNC(on_spinbutton_changed), NULL);
	/*** module combos ***/
	gtk_signal_connect(GTK_OBJECT(entry3), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry4), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry5), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry6), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry7), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry8), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry9), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry10), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry11), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry13), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entry14), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entryDefaultDict), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(combo_entryHebViewer), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(combo_entryGreekViewer), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(entryDevotion), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed), NULL);
	/*** OK - Apply - Cancel ***/
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPropertyboxOK_clicked),
			   (SETTINGS *) s);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxApply), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPropertyboxApply_clicked),
			   (SETTINGS *) s);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxCancel), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btnPropertyboxCancel_clicked), NULL);

	gtk_object_set_data(GTK_OBJECT(dlgSettings), "tooltips", tooltips);

	/** add group and icons to shortcut bar **/
	groupnum =
	    add_sb_group2((EShortcutBar *) shortcut_bar, _("Preferences"));
	gtk_signal_connect(GTK_OBJECT(shortcut_bar), "item_selected",
			   GTK_SIGNAL_FUNC(on_shortcut_bar_item_selected1),
			   NULL);

	pathname = gnome_pixmap_file("gnome-ccwindowmanager.png");
	icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->model,
				  groupnum, -1, "fonts",
				  _("Font Colors and Sizes"), icon_pixbuf);
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
				  _("Shortcut Bar Settings"), icon_pixbuf);
	pathname = gnome_pixmap_file("gdict.png");
	icon_pixbuf = gdk_pixbuf_new_from_file(pathname);
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->model,
				  groupnum, -1, "Sword Mods",
				  _("Sword Modules"), icon_pixbuf);
	e_shortcut_bar_set_view_type((EShortcutBar *) shortcut_bar,
				     groupnum, E_ICON_BAR_LARGE_ICONS);
	/** set color pickers to current colors **/
	setcolorpickersColor(s,
			     gcpTextBG,
			     gcpText,
			     gcpCurrentverse,
			     gcpTextVerseNums, gcpTextLinks);

	return dlgSettings;
}
