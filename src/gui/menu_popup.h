/*
 * Xiphos Bible Study Tool
 * menu_popup.h - creation of (and call backs) for xiphos popup menus
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

#ifndef __MENU_POPUP__H_
#define __MENU_POPUP__H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USE_GTKBUILDER
#include <glade/glade-xml.h>
#endif

#include "main/module_dialogs.h"

void gui_menu_popup(XiphosHtml *html, const gchar *mod_name,
		    DIALOG_DATA *d);

gint _get_type_mod_list(void);
gchar *_get_key(gchar *mod_name);
GtkWidget *_get_html(void);
void on_popup_about_activate(GtkMenuItem *menuitem,
			     gpointer user_data);
void on_popup_bookmark_activate(GtkMenuItem *menuitem,
				gpointer user_data);
void on_popup_export_passage_activate(GtkMenuItem *menuitem,
				      gpointer user_data);
void on_popup_print_activate(GtkMenuItem *menuitem,
			     gpointer user_data);
void on_close_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_popup_copy_activate(GtkMenuItem *menuitem,
			    gpointer user_data);
void on_popup_find_activate(GtkMenuItem *menuitem,
			    gpointer user_data);
void on_popup_font_activate(GtkMenuItem *menuitem,
			    gpointer user_data);
void on_verse_per_line_activate(GtkCheckMenuItem *menuitem,
				gpointer user_data);
void on_words_of_christ_in_red_activate(GtkCheckMenuItem *
					    menuitem,
					gpointer user_data);
void on_strong_s_numbers_activate(GtkCheckMenuItem *menuitem,
				  gpointer user_data);
void on_morphological_tags_activate(GtkCheckMenuItem *menuitem,
				    gpointer user_data);
void on_footnotes_activate(GtkCheckMenuItem *menuitem,
			   gpointer user_data);
void on_greek_accents_activate(GtkCheckMenuItem *menuitem,
			       gpointer user_data);
void on_lemmas_activate(GtkCheckMenuItem *menuitem,
			gpointer user_data);
void on_xlit_activate(GtkCheckMenuItem *menuitem,
		      gpointer user_data);
void on_enumerated_activate(GtkCheckMenuItem *menuitem,
			    gpointer user_data);
void on_glosses_activate(GtkCheckMenuItem *menuitem,
			 gpointer user_data);
void on_morphseg_activate(GtkCheckMenuItem *menuitem,
			  gpointer user_data);
void on_scripture_cross_references_activate(GtkCheckMenuItem *
						menuitem,
					    gpointer user_data);
void on_hebrew_vowel_points_activate(GtkCheckMenuItem *menuitem,
				     gpointer user_data);
void on_hebrew_cantillation_activate(GtkCheckMenuItem *menuitem,
				     gpointer user_data);
void on_headings_activate(GtkCheckMenuItem *menuitem,
			  gpointer user_data);
void on_transliteration_activate(GtkCheckMenuItem *menuitem,
				 gpointer user_data);
void on_commentary_by_chapter_activate(GtkCheckMenuItem *menuitem,
				       gpointer user_data);
void on_doublespace_activate(GtkCheckMenuItem *menuitem,
			     gpointer user_data);
void on_xrefnotenumbers_activate(GtkCheckMenuItem *menuitem,
				 gpointer user_data);
void on_primary_reading_activate(GtkCheckMenuItem *menuitem,
				 gpointer user_data);
void on_secondary_reading_activate(GtkCheckMenuItem *menuitem,
				   gpointer user_data);
void on_all_readings_activate(GtkCheckMenuItem *menuitem,
			      gpointer user_data);
void on_image_content_activate(GtkCheckMenuItem *menuitem,
			       gpointer user_data);
void on_respect_font_faces_activate(GtkCheckMenuItem *menuitem,
				    gpointer user_data);
void on_unlock_this_module_activate(GtkMenuItem *menuitem,
				    gpointer user_data);
void on_display_book_heading_activate(GtkMenuItem *menuitem,
				      gpointer user_data);
void on_display_chapter_heading_activate(GtkMenuItem *menuitem,
					 gpointer user_data);
void on_use_current_dictionary_activate(GtkMenuItem *menuitem,
					gpointer user_data);
void on_lookup_google_activate(GtkMenuItem *menuitem,
			       gpointer user_data);
void on_rename_perscomm_activate(GtkMenuItem *menuitem,
				 gpointer user_data);
void on_dump_perscomm_activate(GtkMenuItem *menuitem,
			       gpointer user_data);
void on_read_selection_aloud_activate(GtkMenuItem *menuitem,
				      gpointer user_data);
void on_mark_verse_activate(GtkMenuItem *menuitem,
			    gpointer user_data);

#ifdef USE_GTKBUILDER
void _add_and_check_global_opts(GtkBuilder *gxml,
				const gchar *mod_name,
				GtkWidget *submenu,
				DIALOG_DATA *d);
#else
void _add_and_check_global_opts(GladeXML *gxml,
				const gchar *mod_name,
				GtkWidget *submenu,
				DIALOG_DATA *d);
#endif

#ifdef __cplusplus
}
#endif
#endif /* __MENU_POPUP__H_ */
