/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                               gs_preferences_dlg.c
                             -------------------
    begin                : Fri July 27 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

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
 *  You should have received a copy of the GNU General Public License
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

#include <gnome.h>
#include <gtk/gtk.h>

#include "gs_preferences_dlg.h"

#include "callback.h"
#include "gs_studypad.h"
#include "gs_gnomesword.h"
#include "sw_properties.h"
#include "support.h"

GtkWidget *notebook7;
extern gchar *tmpcolor;

#define NUM_SHORTCUT_TYPES1 3
static gchar *shortcut_types[NUM_SHORTCUT_TYPES1] = {
	"font:", "misc:", "modules:"
};
static gchar *icon_filenames[NUM_SHORTCUT_TYPES1] = {
	"gnome-ccwindowmanager.png",
	"gnome-settings.png",
	"gdict.png"
};

static GdkPixbuf *icon_pixbufs[NUM_SHORTCUT_TYPES1];
//static GtkWidget *main_label;
static EShortcutModel *shortcut_model;
static GdkPixbuf *icon_callback1(EShortcutBar * shortcut_bar,
				const gchar * url, gpointer data);

static GdkPixbuf *icon_callback1(EShortcutBar * shortcut_bar,
				 const gchar * url, gpointer data)
{
	gint i;

	for (i = 0; i < NUM_SHORTCUT_TYPES1; i++) {
		if (!strncmp(url, shortcut_types[i],
			     strlen(shortcut_types[i]))) {
			gdk_pixbuf_ref(icon_pixbufs[i]);
			return icon_pixbufs[i];
		}
	}
	return NULL;
}

/******************************************************************************
 * add_sb_group - add group to shourtcut bar
 * shortcut_bar - shortcut bar to add group to
 * group_name - name of the group to be added
 *****************************************************************************/
static gint
add_sb_group(EShortcutBar *shortcut_bar,gchar *group_name)
{
	gint group_num;
	
	group_num = e_shortcut_model_add_group (shortcut_bar->model, -1, group_name); 	
	e_shortcut_bar_set_view_type (shortcut_bar, group_num, E_ICON_BAR_SMALL_ICONS);
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
/*
static void set_preferences_to_current(SETTINGS * s)
{

}
*/
static void get_preferences_from_dlg(GtkWidget *d ,SETTINGS * s)
{
	/************************************************************
	we need to read all propertybox options here
	*************************************************************/
	gchar *buf;
	gdouble color[4];
	
	/*** read modules ***/
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "entry3")));
	sprintf(s->MainWindowModule, "%s",buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "entry4")));
	sprintf(s->Interlinear1Module, "%s",buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "entry5")));
	sprintf(s->Interlinear2Module, "%s",buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "entry6")));
	sprintf(s->Interlinear3Module, "%s",buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "entry7")));
	sprintf(s->Interlinear4Module, "%s",buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "entry8")));
	sprintf(s->Interlinear5Module, "%s",buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "entry9")));
	sprintf(s->CommWindowModule, "%s",buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "entry10")));
	sprintf(s->DictWindowModule, "%s",buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "entry11")));
	sprintf(s->personalcommentsmod, "%s",buf);
	/*** read html colors ***/
	gnome_color_picker_get_d(GNOME_COLOR_PICKER(lookup_widget(d,"gcpText")), 
			&color[0] , &color[1], &color[2], &color[3]);
        buf = gdouble_arr_to_hex(color, 0);
	sprintf(s->bible_text_color, "%s",buf);
	gnome_color_picker_get_d(GNOME_COLOR_PICKER(lookup_widget(d,"gcpTextBG")), 
			&color[0] , &color[1], &color[2], &color[3]);
        buf = gdouble_arr_to_hex(color, 0);
	sprintf(s->bible_bg_color, "%s",buf);
	gnome_color_picker_get_d(GNOME_COLOR_PICKER(lookup_widget(d,"gcpCurrentverse")), 
			&color[0] , &color[1], &color[2], &color[3]);
        buf = gdouble_arr_to_hex(color, 0);
	sprintf(s->currentverse_color, "%s",buf);
	gnome_color_picker_get_d(GNOME_COLOR_PICKER(lookup_widget(d,"gcpTextLinks")), 
			&color[0] , &color[1], &color[2], &color[3]);
        buf = gdouble_arr_to_hex(color, 0);
	sprintf(s->link_color, "%s",buf);
	/*** read font sizes ***/	
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "cmbEntryTextSize")));
	sprintf(s->bible_font_size, "%s",buf);
	buf = gtk_entry_get_text(GTK_ENTRY
				       (lookup_widget(d,
					 "cmbEentryVNSize")));
	sprintf(s->verse_num_font_size, "%s",buf);
	/*** read radio buttons ***/
	s->usedefault = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "rbtnUsedefaults"))->active;
	/*** read check buttons ***/
	s->showshortcutbar = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "cbtnShowSCB"))->active;
	s->text_tabs = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "cbtnShowBibletabs"))->active;
	s->comm_tabs = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "cbtnShowCOMtabs"))->active;
	s->dict_tabs = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "cbtnShowDLtabs"))->active;	
	s->versestyle = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "checkbutton9"))->active; 
	s->showtextgroup = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "cbtnShowTextgroup"))->active;
	s->showcomgroup = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "cbtnShowComGroup"))->active;
	s-> showdictgroup= GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "cbtnShowDictGroup"))->active;
	s->showhistorygroup = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "cbtnShowHistoryGroup"))->active;	
	s->formatpercom = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "cbtnPNformat"))->active;		
	s->autosavepersonalcomments = GTK_TOGGLE_BUTTON(lookup_widget(d,
					 "checkbutton8"))->active;
	
	saveconfig();
	applyoptions();
}
 

static void on_btnPropertyboxOK_clicked(GtkButton * button,
					gpointer user_data)
{
	GtkWidget *dlg;
	SETTINGS *s;
	
	s = (SETTINGS *)user_data;
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	get_preferences_from_dlg(dlg,s);
	gtk_widget_destroy(dlg);
}


static void on_btnPropertyboxApply_clicked(GtkButton * button,
					   gpointer user_data)
{
	GtkWidget *dlg;
	SETTINGS *s;
	
	s = (SETTINGS *)user_data;
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	get_preferences_from_dlg(dlg,s);
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
}

static void on_Entry_changed(GtkEditable * editable, gpointer user_data)
{
	set_buttons_sensitive(GTK_WIDGET(editable));
}


static void
on_button_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	set_buttons_sensitive(GTK_WIDGET(togglebutton));
}				
/***  ***/
GtkWidget *create_dlgSettings(SETTINGS * s,
				GList *biblelist,
				GList *commlist,
				GList *dictlist,
				GList *percomlist)
{
	gint i, groupnum;
	gchar *pathname;
//	GdkPixbuf *icon_pixbuf;
	gdouble *color;
	gushort a;
	GtkWidget *shortcut_bar;
	GtkWidget *dlgSettings;
	GtkWidget *dialog_vbox9;
	GtkWidget *hbox22;
	GtkWidget *frame26;
	GtkWidget *vbox35;
	GtkWidget *hbox34;
	GtkWidget *label149;
	GtkWidget *gcpTextBG;
	GtkWidget *hbox30;
	GtkWidget *label145;
	GtkWidget *gcpText;
	GtkWidget *hbox36;
	GtkWidget *label151;
	GtkWidget *cmbTextFontSize;
	GList *cmbTextFontSize_items = NULL;
	GtkWidget *cmbEntryTextSize;
	GtkWidget *hbox35;
	GtkWidget *label150;
	GtkWidget *gcpCurrentverseBG;
	GtkWidget *hbox23;
	GtkWidget *label103;
	GtkWidget *gcpCurrentverse;
	GtkWidget *hbox31;
	GtkWidget *label146;
	GtkWidget *gcpTextVerseNums;
	GtkWidget *hbox37;
	GtkWidget *label152;
	GtkWidget *cmbVerseNumSize;
	GList *cmbVerseNumSize_items = NULL;
	GtkWidget *cmbEentryVNSize;
	GtkWidget *hbox32;
	GtkWidget *label147;
	GtkWidget *gcpTextLinks;
	GtkWidget *btnSetToGSDefaults;
	GtkWidget *label98;
	GtkWidget *hbox64;
	GtkWidget *vbox28;
	GtkWidget *frame21;
	GtkWidget *vbox36;
	GSList *vbox36_group = NULL;
	GtkWidget *rbtnUsedefaults;
	GtkWidget *rbtnNoDefaults;
	GtkWidget *cbtnShowSCB;
	GtkWidget *cbtnShowBibletabs;
	GtkWidget *cbtnShowCOMtabs;
	GtkWidget *cbtnShowDLtabs;
	GtkWidget *checkbutton9;
	GtkWidget *vbox41;
	GtkWidget *frame24;
	GtkWidget *vbox29;
	GtkWidget *cbtnShowTextgroup;
	GtkWidget *cbtnShowComGroup;
	GtkWidget *cbtnShowDictGroup;
	GtkWidget *cbtnShowHistoryGroup;
	GtkWidget *frame25;
	GtkWidget *vbox37;
	GtkWidget *cbtnPNformat;
	/****************************************************************************/
	GtkWidget *checkbutton8;
	GtkWidget *label123;
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
	GtkWidget *combo24;
	GtkWidget *entry10;
	GtkWidget *combo25;
	GtkWidget *entry11;
	GtkWidget *label160;
	GtkWidget *label161;
	GtkWidget *label162;
	GtkWidget *label163;
	GtkWidget *label164;
	GtkWidget *label155;
	GtkWidget *dialog_action_area9;
	GtkWidget *btnPropertyboxOK;
	GtkWidget *btnPropertyboxApply;
	GtkWidget *btnPropertyboxCancel;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();

	a=000000;
	
	dlgSettings = gnome_dialog_new(_("GnomeSword - Preferences"), NULL);
	gtk_object_set_data(GTK_OBJECT(dlgSettings), "dlgSettings",
			    dlgSettings);
	gtk_window_set_policy(GTK_WINDOW(dlgSettings), FALSE, FALSE,
			      FALSE);

	dialog_vbox9 = GNOME_DIALOG(dlgSettings)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgSettings), "dialog_vbox9",
			    dialog_vbox9);
	gtk_widget_show(dialog_vbox9);

	hbox22 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox22);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox22", hbox22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox22);
	gtk_box_pack_start(GTK_BOX(dialog_vbox9), hbox22, TRUE, TRUE, 0);

	shortcut_model = e_shortcut_model_new();
	
	shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(shortcut_bar),
				shortcut_model);
	gtk_widget_set_usize(shortcut_bar, 150, 250);
	gtk_widget_show(shortcut_bar);
	gtk_box_pack_start(GTK_BOX(hbox22), shortcut_bar, TRUE, TRUE, 0);

	e_shortcut_bar_set_icon_callback(E_SHORTCUT_BAR(shortcut_bar),
				icon_callback1, NULL);
				
	notebook7 = gtk_notebook_new();
	gtk_widget_ref(notebook7);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "notebook7",
				 notebook7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(notebook7);
	gtk_box_pack_start(GTK_BOX(hbox22), notebook7, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(notebook7, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook7), FALSE);

	frame26 = gtk_frame_new(_("Font Colors and Sizes"));
	gtk_widget_ref(frame26);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "frame26",
				 frame26,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame26);
	gtk_container_add(GTK_CONTAINER(notebook7), frame26);

	vbox35 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox35);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox35", vbox35,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox35);
	gtk_container_add(GTK_CONTAINER(frame26), vbox35);

	hbox34 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox34);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox34", hbox34,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox34);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox34, FALSE, TRUE, 0);

	label149 = gtk_label_new(_("Background Color"));
	gtk_widget_ref(label149);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label149",
				 label149,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label149);
	gtk_box_pack_start(GTK_BOX(hbox34), label149, TRUE, TRUE, 0);
	gtk_widget_set_usize(label149, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label149), GTK_JUSTIFY_LEFT);

	gcpTextBG = gnome_color_picker_new();
	gtk_widget_ref(gcpTextBG);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "gcpTextBG",
				 gcpTextBG,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(gcpTextBG);
	gtk_box_pack_start(GTK_BOX(hbox34), gcpTextBG, TRUE, FALSE, 0);
	gtk_widget_set_usize(gcpTextBG, 41, -2);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER(gcpTextBG),
				     _("Pick a color for Background"));
	if(string_is_color(s->bible_bg_color)){
		color = hex_to_gdouble_arr(s->bible_bg_color); 
		gnome_color_picker_set_i8 (GNOME_COLOR_PICKER(gcpTextBG),color[0] ,color[1] , color[2], a); 	
	}

	hbox30 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox30);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox30", hbox30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox30);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox30, FALSE, TRUE, 0);

	label145 = gtk_label_new(_("Text Color"));
	gtk_widget_ref(label145);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label145",
				 label145,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label145);
	gtk_box_pack_start(GTK_BOX(hbox30), label145, TRUE, TRUE, 0);
	gtk_widget_set_usize(label145, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label145), GTK_JUSTIFY_LEFT);

	gcpText = gnome_color_picker_new();
	gtk_widget_ref(gcpText);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "gcpText",
				 gcpText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(gcpText);
	gtk_box_pack_start(GTK_BOX(hbox30), gcpText, TRUE, FALSE, 0);
	gtk_widget_set_usize(gcpText, 41, -2);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER(gcpText),
				     _("Pick a color for Bible Text"));
	if(string_is_color(s->bible_text_color)){
		color = hex_to_gdouble_arr(s->bible_text_color); 
		gnome_color_picker_set_i8 (GNOME_COLOR_PICKER(gcpText),color[0] ,color[1] , color[2], a); 	
	}
	
	hbox36 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox36);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox36", hbox36,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox36);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox36, FALSE, TRUE, 0);

	label151 = gtk_label_new(_("Font Size"));
	gtk_widget_ref(label151);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label151",
				 label151,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label151);
	gtk_box_pack_start(GTK_BOX(hbox36), label151, TRUE, TRUE, 0);
	gtk_widget_set_usize(label151, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label151), GTK_JUSTIFY_LEFT);

	cmbTextFontSize = gtk_combo_new();
	gtk_widget_ref(cmbTextFontSize);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cmbTextFontSize", cmbTextFontSize,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cmbTextFontSize);
	gtk_box_pack_start(GTK_BOX(hbox36), cmbTextFontSize, TRUE, FALSE,
			   0);
	gtk_widget_set_usize(cmbTextFontSize, 41, -2);
	gtk_combo_set_value_in_list(GTK_COMBO(cmbTextFontSize), TRUE,
				    TRUE);
	cmbTextFontSize_items =
	    g_list_append(cmbTextFontSize_items, (gpointer) _("-2"));
	cmbTextFontSize_items =
	    g_list_append(cmbTextFontSize_items, (gpointer) _("-1"));
	cmbTextFontSize_items =
	    g_list_append(cmbTextFontSize_items, (gpointer) _("+0"));
	cmbTextFontSize_items =
	    g_list_append(cmbTextFontSize_items, (gpointer) _("+1"));
	cmbTextFontSize_items =
	    g_list_append(cmbTextFontSize_items, (gpointer) _("+2"));
	cmbTextFontSize_items =
	    g_list_append(cmbTextFontSize_items, (gpointer) _("+3"));
	cmbTextFontSize_items =
	    g_list_append(cmbTextFontSize_items, (gpointer) _("+4"));
	cmbTextFontSize_items =
	    g_list_append(cmbTextFontSize_items, (gpointer) _("+5\r"));
	gtk_combo_set_popdown_strings(GTK_COMBO(cmbTextFontSize),
				      cmbTextFontSize_items);
	g_list_free(cmbTextFontSize_items);

	cmbEntryTextSize = GTK_COMBO(cmbTextFontSize)->entry;
	gtk_widget_ref(cmbEntryTextSize);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cmbEntryTextSize", cmbEntryTextSize,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cmbEntryTextSize);
	gtk_tooltips_set_tip(tooltips, cmbEntryTextSize,
			     _
			     ("Zero is base font size Go up or down from there"),
			     NULL);
	gtk_entry_set_text(GTK_ENTRY(cmbEntryTextSize), _(s->bible_font_size));

	hbox35 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox35);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox35", hbox35,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox35);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox35, FALSE, TRUE, 0);

	label150 = gtk_label_new(_("Current Verse Background Color"));
	gtk_widget_ref(label150);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label150",
				 label150,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label150);
	gtk_box_pack_start(GTK_BOX(hbox35), label150, TRUE, TRUE, 0);
	gtk_widget_set_usize(label150, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label150), GTK_JUSTIFY_LEFT);

	gcpCurrentverseBG = gnome_color_picker_new();
	gtk_widget_ref(gcpCurrentverseBG);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "gcpCurrentverseBG", gcpCurrentverseBG,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(gcpCurrentverseBG);
	gtk_box_pack_start(GTK_BOX(hbox35), gcpCurrentverseBG, TRUE, FALSE,
			   0);
	gtk_widget_set_usize(gcpCurrentverseBG, 41, -2);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER(gcpCurrentverseBG),
				     _("Pick a color - CurrentVerse BG"));

	hbox23 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox23);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox23", hbox23,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox23);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox23, FALSE, TRUE, 0);

	label103 = gtk_label_new(_("Current Verse Color"));
	gtk_widget_ref(label103);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label103",
				 label103,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label103);
	gtk_box_pack_start(GTK_BOX(hbox23), label103, TRUE, TRUE, 0);
	gtk_widget_set_usize(label103, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label103), GTK_JUSTIFY_LEFT);

	gcpCurrentverse = gnome_color_picker_new();
	gtk_widget_ref(gcpCurrentverse);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "gcpCurrentverse", gcpCurrentverse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(gcpCurrentverse);
	gtk_box_pack_start(GTK_BOX(hbox23), gcpCurrentverse, TRUE, FALSE,
			   0);
	gtk_widget_set_usize(gcpCurrentverse, 41, -2);
	gnome_color_picker_set_title(GNOME_COLOR_PICKER(gcpCurrentverse),
				     _("Pick a color for Current Verse"));
	if(string_is_color(s->currentverse_color)){
		color = hex_to_gdouble_arr(s->currentverse_color); 
		gnome_color_picker_set_i8 (GNOME_COLOR_PICKER(gcpCurrentverse),color[0] ,color[1] , color[2], a); 	
	}
	hbox31 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox31);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox31", hbox31,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox31);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox31, FALSE, TRUE, 0);

	label146 = gtk_label_new(_("Verse Numbers"));
	gtk_widget_ref(label146);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label146",
				 label146,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label146);
	gtk_box_pack_start(GTK_BOX(hbox31), label146, TRUE, TRUE, 0);
	gtk_widget_set_usize(label146, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label146), GTK_JUSTIFY_LEFT);

	gcpTextVerseNums = gnome_color_picker_new();
	gtk_widget_ref(gcpTextVerseNums);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "gcpTextVerseNums", gcpTextVerseNums,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(gcpTextVerseNums);
	gtk_box_pack_start(GTK_BOX(hbox31), gcpTextVerseNums, TRUE, FALSE,
			   0);
	gtk_widget_set_usize(gcpTextVerseNums, 41, -2);
	if(string_is_color(s->bible_verse_num_color)){
		color = hex_to_gdouble_arr(s->bible_verse_num_color); 
		gnome_color_picker_set_i8 (GNOME_COLOR_PICKER(gcpTextVerseNums),color[0] ,color[1] , color[2], a); 	
	}

	hbox37 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox37);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox37", hbox37,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox37);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox37, FALSE, TRUE, 0);

	label152 = gtk_label_new(_("Verse Number Font Size"));
	gtk_widget_ref(label152);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label152",
				 label152,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label152);
	gtk_box_pack_start(GTK_BOX(hbox37), label152, TRUE, TRUE, 0);
	gtk_widget_set_usize(label152, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label152), GTK_JUSTIFY_LEFT);

	cmbVerseNumSize = gtk_combo_new();
	gtk_widget_ref(cmbVerseNumSize);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cmbVerseNumSize", cmbVerseNumSize,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cmbVerseNumSize);
	gtk_box_pack_start(GTK_BOX(hbox37), cmbVerseNumSize, TRUE, FALSE,
			   0);
	gtk_widget_set_usize(cmbVerseNumSize, 41, -2);
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
	    g_list_append(cmbVerseNumSize_items, (gpointer) _("+5\r"));
	gtk_combo_set_popdown_strings(GTK_COMBO(cmbVerseNumSize),
				      cmbVerseNumSize_items);
	g_list_free(cmbVerseNumSize_items);

	cmbEentryVNSize = GTK_COMBO(cmbVerseNumSize)->entry;
	gtk_widget_ref(cmbEentryVNSize);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cmbEentryVNSize", cmbEentryVNSize,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cmbEentryVNSize);
	gtk_tooltips_set_tip(tooltips, cmbEentryVNSize,
			     _
			     ("Zero is base font size Go up or down from there"),
			     NULL);
	gtk_entry_set_text(GTK_ENTRY(cmbEentryVNSize), _("+0"));

	hbox32 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox32);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox32", hbox32,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox32);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox32, FALSE, TRUE, 0);

	label147 = gtk_label_new(_("Link Color"));
	gtk_widget_ref(label147);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label147",
				 label147,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label147);
	gtk_box_pack_start(GTK_BOX(hbox32), label147, TRUE, TRUE, 0);
	gtk_widget_set_usize(label147, 216, -2);
	gtk_label_set_justify(GTK_LABEL(label147), GTK_JUSTIFY_LEFT);

	gcpTextLinks = gnome_color_picker_new();
	gtk_widget_ref(gcpTextLinks);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "gcpTextLinks",
				 gcpTextLinks,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(gcpTextLinks);
	gtk_box_pack_start(GTK_BOX(hbox32), gcpTextLinks, TRUE, FALSE, 0);
	gtk_widget_set_usize(gcpTextLinks, 41, -2);
	gtk_tooltips_set_tip(tooltips, gcpTextLinks,
			     _("Strongs Numbers & Morph Tags"), NULL);
	if(string_is_color(s->link_color)){
		color = hex_to_gdouble_arr(s->link_color); 
		gnome_color_picker_set_i8 (GNOME_COLOR_PICKER(gcpTextLinks),color[0] ,color[1] , color[2], a); 	
	}

	btnSetToGSDefaults =
	    gtk_button_new_with_label(_("GnomeSword Defaults"));
	gtk_widget_ref(btnSetToGSDefaults);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnSetToGSDefaults", btnSetToGSDefaults,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSetToGSDefaults);
	gtk_box_pack_start(GTK_BOX(vbox35), btnSetToGSDefaults, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, btnSetToGSDefaults,
			     _("Use GnomeSword defaults for all settings"),
			     NULL);
	gtk_button_set_relief(GTK_BUTTON(btnSetToGSDefaults),
			      GTK_RELIEF_NORMAL);

	label98 = gtk_label_new(_("Bible Text Window"));
	gtk_widget_ref(label98);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label98",
				 label98,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label98);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     0), label98);

	hbox64 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox64);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "hbox64", hbox64,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox64);
	gtk_container_add(GTK_CONTAINER(notebook7), hbox64);

	vbox28 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox28);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox28", vbox28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox28);
	gtk_box_pack_start(GTK_BOX(hbox64), vbox28, TRUE, TRUE, 0);

	frame21 = gtk_frame_new(_("Use Defaults When Opening GnomeSword"));
	gtk_widget_ref(frame21);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "frame21",
				 frame21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame21);
	gtk_box_pack_start(GTK_BOX(vbox28), frame21, FALSE, TRUE, 0);

	vbox36 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox36);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox36", vbox36,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox36);
	gtk_container_add(GTK_CONTAINER(frame21), vbox36);
 
	rbtnUsedefaults =
	    gtk_radio_button_new_with_label(vbox36_group,
					    _
					    ("Yes, use defaults that I select"));
	vbox36_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbtnUsedefaults));
	gtk_widget_ref(rbtnUsedefaults);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "rbtnUsedefaults", rbtnUsedefaults,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rbtnUsedefaults);
	gtk_box_pack_start(GTK_BOX(vbox36), rbtnUsedefaults, FALSE, FALSE,
			   0);
	

	rbtnNoDefaults =
	    gtk_radio_button_new_with_label(vbox36_group,
					    _
					    ("No, use settings saved\non last GnomeSword run"));
	vbox36_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbtnNoDefaults));
	gtk_widget_ref(rbtnNoDefaults);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "rbtnNoDefaults",
				 rbtnNoDefaults,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rbtnNoDefaults);
	gtk_box_pack_start(GTK_BOX(vbox36), rbtnNoDefaults, FALSE, FALSE,
			   0);
	if(s->usedefault)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnUsedefaults), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnNoDefaults), TRUE);
	
	cbtnShowSCB =
	    gtk_check_button_new_with_label(_("Show Shortcut bar"));
	gtk_widget_ref(cbtnShowSCB);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "cbtnShowSCB",
				 cbtnShowSCB,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowSCB);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowSCB, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowSCB), s->showshortcutbar);

	cbtnShowBibletabs =
	    gtk_check_button_new_with_label(_
					    ("Show Bible Texts notebook tabs"));
	gtk_widget_ref(cbtnShowBibletabs);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowBibletabs", cbtnShowBibletabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowBibletabs);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowBibletabs, FALSE,
			   FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowBibletabs), s->text_tabs);

	cbtnShowCOMtabs =
	    gtk_check_button_new_with_label(_
					    ("Show Commentary notebook tabs"));
	gtk_widget_ref(cbtnShowCOMtabs);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowCOMtabs", cbtnShowCOMtabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowCOMtabs);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowCOMtabs, FALSE, FALSE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowCOMtabs), s->comm_tabs);

	cbtnShowDLtabs =
	    gtk_check_button_new_with_label(_
					    ("Show Dict/Lex notebook tabs"));
	gtk_widget_ref(cbtnShowDLtabs);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "cbtnShowDLtabs",
				 cbtnShowDLtabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowDLtabs);
	gtk_box_pack_start(GTK_BOX(vbox28), cbtnShowDLtabs, FALSE, FALSE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowDLtabs), s->dict_tabs);

	checkbutton9 =
	    gtk_check_button_new_with_label(_("Use Verse Style"));
	gtk_widget_ref(checkbutton9);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "checkbutton9",
				 checkbutton9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(checkbutton9);
	gtk_box_pack_start(GTK_BOX(vbox28), checkbutton9, FALSE, FALSE, 0);
	gtk_widget_set_usize(checkbutton9, 202, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton9),
				    s->versestyle );

	vbox41 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox41);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox41", vbox41,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox41);
	gtk_box_pack_start(GTK_BOX(hbox64), vbox41, TRUE, TRUE, 0);

	frame24 = gtk_frame_new(_("Shortcut Bar Groups"));
	gtk_widget_ref(frame24);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "frame24",
				 frame24,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame24);
	gtk_box_pack_start(GTK_BOX(vbox41), frame24, FALSE, TRUE, 0);

	vbox29 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox29);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox29", vbox29,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox29);
	gtk_container_add(GTK_CONTAINER(frame24), vbox29);

	cbtnShowTextgroup =
	    gtk_check_button_new_with_label(_("Show Bible Text Group"));
	gtk_widget_ref(cbtnShowTextgroup);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowTextgroup", cbtnShowTextgroup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowTextgroup);
	gtk_box_pack_start(GTK_BOX(vbox29), cbtnShowTextgroup, FALSE,
			   FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowTextgroup),
				    s->showtextgroup );

	cbtnShowComGroup =
	    gtk_check_button_new_with_label(_("Show Commentary Group"));
	gtk_widget_ref(cbtnShowComGroup);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowComGroup", cbtnShowComGroup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowComGroup);
	gtk_box_pack_start(GTK_BOX(vbox29), cbtnShowComGroup, FALSE, FALSE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowComGroup),
				    s->showcomgroup );

	cbtnShowDictGroup =
	    gtk_check_button_new_with_label(_("Show Dict/Lex Group"));
	gtk_widget_ref(cbtnShowDictGroup);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowDictGroup", cbtnShowDictGroup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowDictGroup);
	gtk_box_pack_start(GTK_BOX(vbox29), cbtnShowDictGroup, FALSE,
			   FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowDictGroup),
				    s->showdictgroup );

	cbtnShowHistoryGroup =
	    gtk_check_button_new_with_label(_("Show History Group"));
	gtk_widget_ref(cbtnShowHistoryGroup);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "cbtnShowHistoryGroup",
				 cbtnShowHistoryGroup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnShowHistoryGroup);
	gtk_box_pack_start(GTK_BOX(vbox29), cbtnShowHistoryGroup, FALSE,
			   FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnShowHistoryGroup),
				    s->showhistorygroup );

	frame25 = gtk_frame_new(_("Personal Comments"));
	gtk_widget_ref(frame25);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "frame25",
				 frame25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame25);
	gtk_box_pack_start(GTK_BOX(vbox41), frame25, FALSE, TRUE, 0);

	vbox37 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox37);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "vbox37", vbox37,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox37);
	gtk_container_add(GTK_CONTAINER(frame25), vbox37);

	cbtnPNformat =
	    gtk_check_button_new_with_label(_("Use Formatting"));
	gtk_widget_ref(cbtnPNformat);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "cbtnPNformat",
				 cbtnPNformat,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbtnPNformat);
	gtk_box_pack_start(GTK_BOX(vbox37), cbtnPNformat, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtnPNformat),
				    s->formatpercom );


	checkbutton8 = gtk_check_button_new_with_label(_("Auto Save"));
	gtk_widget_ref(checkbutton8);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "checkbutton8",
				 checkbutton8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(checkbutton8);
	gtk_box_pack_start(GTK_BOX(vbox37), checkbutton8, FALSE, FALSE, 0);
	gtk_widget_set_usize(checkbutton8, 202, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton8),
				    s->autosavepersonalcomments );

	label123 = gtk_label_new(_("Interface"));
	gtk_widget_ref(label123);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label123",
				 label123,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label123);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     1), label123);

	table9 = gtk_table_new(9, 2, FALSE);
	gtk_widget_ref(table9);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "table9", table9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table9);
	gtk_container_add(GTK_CONTAINER(notebook7), table9);

	label156 = gtk_label_new(_("Main Text Module"));
	gtk_widget_ref(label156);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label156",
				 label156,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label156);
	gtk_table_attach(GTK_TABLE(table9), label156, 0, 1, 0, 1,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);
 
	combo17 = gtk_combo_new();
	gtk_widget_ref(combo17);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "combo17",
				 combo17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo17);
	gtk_table_attach(GTK_TABLE(table9), combo17, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo17), biblelist);
	
	entry3 = GTK_COMBO(combo17)->entry;
	gtk_widget_ref(entry3);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "entry3", entry3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry3);
	gtk_entry_set_text(GTK_ENTRY(entry3), s->MainWindowModule);
	
	combo18 = gtk_combo_new();
	gtk_widget_ref(combo18);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "combo18",
				 combo18,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo18);
	gtk_table_attach(GTK_TABLE(table9), combo18, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo18), biblelist);
	
	entry4 = GTK_COMBO(combo18)->entry;
	gtk_widget_ref(entry4);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "entry4", entry4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry4);
	gtk_entry_set_text(GTK_ENTRY(entry4), s->Interlinear1Module);
	
	combo19 = gtk_combo_new();
	gtk_widget_ref(combo19);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "combo19",
				 combo19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo19);
	gtk_table_attach(GTK_TABLE(table9), combo19, 1, 2, 2, 3,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo19), biblelist);

	entry5 = GTK_COMBO(combo19)->entry;
	gtk_widget_ref(entry5);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "entry5", entry5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry5);
	gtk_entry_set_text(GTK_ENTRY(entry5), s->Interlinear2Module);

	combo20 = gtk_combo_new();
	gtk_widget_ref(combo20);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "combo20",
				 combo20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo20);
	gtk_table_attach(GTK_TABLE(table9), combo20, 1, 2, 3, 4,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo20), biblelist);

	entry6 = GTK_COMBO(combo20)->entry;
	gtk_widget_ref(entry6);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "entry6", entry6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry6);
	gtk_entry_set_text(GTK_ENTRY(entry6), s->Interlinear3Module);

	label157 = gtk_label_new(_("Commentary"));
	gtk_widget_ref(label157);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label157",
				 label157,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label157);
	gtk_table_attach(GTK_TABLE(table9), label157, 0, 1, 6, 7,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label157), 0, 0.5);

	label158 = gtk_label_new(_("Dict/Lex"));
	gtk_widget_ref(label158);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label158",
				 label158,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label158);
	gtk_table_attach(GTK_TABLE(table9), label158, 0, 1, 7, 8,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label158), 0, 0.5);

	label159 = gtk_label_new(_("Personal"));
	gtk_widget_ref(label159);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label159",
				 label159,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label159);
	gtk_table_attach(GTK_TABLE(table9), label159, 0, 1, 8, 9,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label159), 0, 0.5);

	combo21 = gtk_combo_new();
	gtk_widget_ref(combo21);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "combo21",
				 combo21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo21);
	gtk_table_attach(GTK_TABLE(table9), combo21, 1, 2, 4, 5,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo21), biblelist);

	entry7 = GTK_COMBO(combo21)->entry;
	gtk_widget_ref(entry7);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "entry7", entry7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry7);
	gtk_entry_set_text(GTK_ENTRY(entry7), s->Interlinear4Module);

	combo22 = gtk_combo_new();
	gtk_widget_ref(combo22);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "combo22",
				 combo22,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo22);
	gtk_table_attach(GTK_TABLE(table9), combo22, 1, 2, 5, 6,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo22), biblelist);

	entry8 = GTK_COMBO(combo22)->entry;
	gtk_widget_ref(entry8);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "entry8", entry8,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry8);
	gtk_entry_set_text(GTK_ENTRY(entry8), s->Interlinear5Module);

	combo23 = gtk_combo_new();
	gtk_widget_ref(combo23);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "combo23",
				 combo23,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo23);
	gtk_table_attach(GTK_TABLE(table9), combo23, 1, 2, 6, 7,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo23), commlist);

	entry9 = GTK_COMBO(combo23)->entry;
	gtk_widget_ref(entry9);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "entry9", entry9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry9);
	gtk_entry_set_text(GTK_ENTRY(entry9), s->CommWindowModule);

	combo24 = gtk_combo_new();
	gtk_widget_ref(combo24);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "combo24",
				 combo24,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo24);
	gtk_table_attach(GTK_TABLE(table9), combo24, 1, 2, 7, 8,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo24), dictlist);

	entry10 = GTK_COMBO(combo24)->entry;
	gtk_widget_ref(entry10);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "entry10",
				 entry10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry10);
	gtk_entry_set_text(GTK_ENTRY(entry10), s->DictWindowModule);

	combo25 = gtk_combo_new();
	gtk_widget_ref(combo25);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "combo25",
				 combo25,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo25);
	gtk_table_attach(GTK_TABLE(table9), combo25, 1, 2, 8, 9,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo25), percomlist);

	entry11 = GTK_COMBO(combo25)->entry;
	gtk_widget_ref(entry11);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "entry11",
				 entry11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry11);
	gtk_entry_set_text(GTK_ENTRY(entry11), s->personalcommentsmod);

	label160 = gtk_label_new(_("Interlinear 5"));
	gtk_widget_ref(label160);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label160",
				 label160,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label160);
	gtk_table_attach(GTK_TABLE(table9), label160, 0, 1, 5, 6,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label160), 0, 0.5);

	label161 = gtk_label_new(_("Interlinear 4"));
	gtk_widget_ref(label161);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label161",
				 label161,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label161);
	gtk_table_attach(GTK_TABLE(table9), label161, 0, 1, 4, 5,
			 (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label161), 0, 0.5);

	label162 = gtk_label_new(_("Interlinear 3"));
	gtk_widget_ref(label162);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label162",
				 label162,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label162);
	gtk_table_attach(GTK_TABLE(table9), label162, 0, 1, 3, 4,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label162), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label162), 7.45058e-09,
			       7.45058e-09);

	label163 = gtk_label_new(_("Interlinear 2"));
	gtk_widget_ref(label163);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label163",
				 label163,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label163);
	gtk_table_attach(GTK_TABLE(table9), label163, 0, 1, 2, 3,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label163), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label163), 7.45058e-09,
			       7.45058e-09);

	label164 = gtk_label_new(_("Interlinear 1"));
	gtk_widget_ref(label164);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label164",
				 label164,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label164);
	gtk_table_attach(GTK_TABLE(table9), label164, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label164), 1.2666e-07,
			       7.45058e-09);

	label155 = gtk_label_new(_("Sword Modules"));
	gtk_widget_ref(label155);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings), "label155",
				 label155,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label155);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook7),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook7),
							     2), label155);

	dialog_action_area9 = GNOME_DIALOG(dlgSettings)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgSettings), "dialog_action_area9",
			    dialog_action_area9);
	gtk_widget_show(dialog_action_area9);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area9),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area9), 8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSettings),
				   GNOME_STOCK_BUTTON_OK);
	btnPropertyboxOK =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgSettings)->buttons)->
		       data);
	gtk_widget_ref(btnPropertyboxOK);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnPropertyboxOK", btnPropertyboxOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPropertyboxOK);
	gtk_widget_set_sensitive(btnPropertyboxOK, FALSE);
	GTK_WIDGET_SET_FLAGS(btnPropertyboxOK, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSettings),
				   GNOME_STOCK_BUTTON_APPLY);
	btnPropertyboxApply =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgSettings)->buttons)->
		       data);
	gtk_widget_ref(btnPropertyboxApply);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnPropertyboxApply",
				 btnPropertyboxApply,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPropertyboxApply);
	gtk_widget_set_sensitive(btnPropertyboxApply, FALSE);
	GTK_WIDGET_SET_FLAGS(btnPropertyboxApply, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgSettings),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnPropertyboxCancel =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgSettings)->buttons)->
		       data);
	gtk_widget_ref(btnPropertyboxCancel);
	gtk_object_set_data_full(GTK_OBJECT(dlgSettings),
				 "btnPropertyboxCancel",
				 btnPropertyboxCancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnPropertyboxCancel);
	GTK_WIDGET_SET_FLAGS(btnPropertyboxCancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(gcpTextBG), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(gcpText), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cmbEntryTextSize), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed),
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
	gtk_signal_connect(GTK_OBJECT(cmbEentryVNSize), "changed",
			   GTK_SIGNAL_FUNC(on_Entry_changed),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(gcpTextLinks), "color_set",
			   GTK_SIGNAL_FUNC(on_colorpicker_color_set),
			   NULL);
	/*gtk_signal_connect(GTK_OBJECT(btnSetToGSDefaults), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSetToGSDefaults_clicked),
			   NULL);*/
	gtk_signal_connect(GTK_OBJECT(rbtnUsedefaults), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(rbtnNoDefaults), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowSCB), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowBibletabs), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowCOMtabs), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowDLtabs), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowTextgroup), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowComGroup), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowDictGroup), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnShowHistoryGroup), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbtnPNformat), "toggled",
			   GTK_SIGNAL_FUNC(on_button_toggled), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPropertyboxOK_clicked),
			   (SETTINGS*) s);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxApply), "clicked",
			   GTK_SIGNAL_FUNC(on_btnPropertyboxApply_clicked),
			   (SETTINGS*) s);
	gtk_signal_connect(GTK_OBJECT(btnPropertyboxCancel), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btnPropertyboxCancel_clicked), NULL);

	gtk_object_set_data(GTK_OBJECT(dlgSettings), "tooltips", tooltips);

	for (i = 0; i < NUM_SHORTCUT_TYPES1; i++) {
		pathname = gnome_pixmap_file(icon_filenames[i]);
		if (pathname)
			icon_pixbufs[i] =
			    gdk_pixbuf_new_from_file(pathname);
		else
			icon_pixbufs[i] = NULL;
	}
	gtk_signal_connect(GTK_OBJECT(shortcut_bar), "item_selected",
			   GTK_SIGNAL_FUNC(on_shortcut_bar_item_selected1),
			   NULL);
	groupnum = add_sb_group((EShortcutBar *)shortcut_bar, "Preferences");
	e_shortcut_bar_set_view_type((EShortcutBar*)shortcut_bar,
						      groupnum,
						      E_ICON_BAR_LARGE_ICONS);
	e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum, -1,
					shortcut_types[0],
					"Font Colors and Sizes");
	e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum, -1,
					shortcut_types[1],
					"Misc Interface Settings");
	e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum, -1,
					shortcut_types[2],
					"Sword Modules");
	return dlgSettings;
}



