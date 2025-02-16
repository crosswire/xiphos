/*
 * Xiphos Bible Study Tool
 * preferences_dialog.h - get user preferences
 *
 * Copyright (C) 2000-2025 Xiphos Developer Team
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

#ifndef __PREFERENCES_DIALOG_H_
#define __PREFERENCES_DIALOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define NONE _("System Default")
extern char *locale_set[];
/* for selecting special locale */
/* autogenerated from po/LINGUAS */
/* see src/gtk/Makefile.am */

void on_checkbutton1_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void on_checkbutton2_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void on_checkbutton3_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void on_checkbutton4_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void on_checkbutton6_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void on_checkbutton7_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void on_checkbutton8_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void on_checkbutton9_toggled(GtkToggleButton *togglebutton,
			     gpointer user_data);
void on_justifybutton_toggled(GtkToggleButton *togglebutton,
			      gpointer user_data);

void on_folder_changed(GtkFileChooser *filechooser,
		       gpointer user_data);

void on_combobox1_changed(GtkComboBox *combobox,
			  gpointer user_data);
void on_combobox2_changed(GtkComboBox *combobox,
			  gpointer user_data);
void on_combobox4_changed(GtkComboBox *combobox,
			  gpointer user_data);
void on_combobox5_changed(GtkComboBox *combobox,
			  gpointer user_data);
void on_combobox6_changed(GtkComboBox *combobox,
			  gpointer user_data);
void on_combobox7_changed(GtkComboBox *combobox,
			  gpointer user_data);
void on_combobox8_changed(GtkComboBox *combobox,
			  gpointer user_data);
void on_combobox9_changed(GtkComboBox *combobox,
			  gpointer user_data);
void on_combobox10_changed(GtkComboBox *combobox,
			   gpointer user_data);
void on_combobox11_changed(GtkComboBox *combobox,
			   gpointer user_data);
void on_combobox12_changed(GtkComboBox *combobox,
			   gpointer user_data);
void on_combobox13_changed(GtkComboBox *combobox,
			   gpointer user_data);
void on_combobox14_changed(GtkComboBox *combobox,
			   gpointer user_data);
void on_combobox15_changed(GtkComboBox *combobox,
			   gpointer user_data);
void on_combobox16_changed(GtkComboBox *combobox,
			   gpointer user_data);
void on_combobox17_changed(GtkComboBox *combobox,
			   gpointer user_data);
void on_columncountvalue_changed(GtkComboBox *combobox,
				 gpointer user_data);

void on_colorbutton1_color_set(GtkColorButton *colorbutton,
			       gpointer user_data);
void on_colorbutton2_color_set(GtkColorButton *colorbutton,
			       gpointer user_data);
void on_colorbutton3_color_set(GtkColorButton *colorbutton,
			       gpointer user_data);
void on_colorbutton4_color_set(GtkColorButton *colorbutton,
			       gpointer user_data);
void on_colorbutton5_color_set(GtkColorButton *colorbutton,
			       gpointer user_data);
void on_colorbutton6_color_set(GtkColorButton *colorbutton,
			       gpointer user_data);
void on_colorbutton7_color_set(GtkColorButton *colorbutton,
			       gpointer user_data);
void setup_color_pickers(void);
void apply_color_settings(void);
void invert_colors(char **color1, char *label1, char **color2,
		   char *label2);
void on_invert(GtkWidget *button, gchar *user_data);

void on_dialog_prefs_response(GtkDialog *dialog, gint response_id,
			      gpointer user_data);

void gui_setup_preferences_dialog(void);
void setup_locale_combobox(void);
void setup_font_prefs_combobox(void);

void on_biblesync_kbd(int);
gchar *on_biblesync_obtain_passphrase(void);
extern GtkWidget *speaker_window, *speaker_list;

void on_checkbutton10_toggled(GtkToggleButton *togglebutton,
			      gpointer user_data);
void on_checkbutton11_toggled(GtkToggleButton *togglebutton,
			      gpointer user_data);
void on_checkbutton12_toggled(GtkToggleButton *togglebutton,
			      gpointer user_data);
void on_checkbutton_verse_num_bold_toggled(GtkToggleButton *togglebutton,
					   gpointer user_data);
void on_checkbutton_verse_num_bracket_toggled(GtkToggleButton *togglebutton,
					      gpointer user_data);
void on_checkbutton_verse_num_superscript_toggled(GtkToggleButton *togglebutton,
						  gpointer user_data);
void on_checkbutton_scroll_toggled(GtkToggleButton *togglebutton,
				   gpointer user_data);
void on_checkbutton_imageresize_toggled(GtkToggleButton *togglebutton,
					gpointer user_data);
void on_checkbutton_versehighlight_toggled(GtkToggleButton *togglebutton,
					   gpointer user_data);
void on_checkbutton_annotate_highlight_toggled(GtkToggleButton *togglebutton,
					       gpointer user_data);
void on_checkbutton_xrefs_in_verse_list_toggled(GtkToggleButton *togglebutton,
						gpointer user_data);
void on_checkbutton_prayerlist_toggled(GtkToggleButton *togglebutton,
				       gpointer user_data);
void on_checkbutton_statusbar_toggled(GtkToggleButton *togglebutton,
				      gpointer user_data);
void on_checkbutton_alternation_toggled(GtkToggleButton *togglebutton,
					gpointer user_data);
void on_basecombobox1_changed(GtkComboBox *combobox,
			      gpointer user_data);

void ps_close(GtkButton *button, gpointer user_data);
void ps_button_clear(GtkButton *button, gpointer user_data);
void ps_button_cut(GtkButton *button, gpointer user_data);
void ps_button_add(GtkButton *button, gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif
