/*
 * GnomeSword Bible Study Tool
 * gs_bibletext.h - support for commentary modules
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

#ifndef __GS_BIBLETEXT_H_
#define __GS_BIBLETEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gs_gnomesword.h"

	typedef struct _text_find_dialog Text_Find_Dialog;
	struct _text_find_dialog {
		GnomeDialog *dialog;
		GtkWidget *htmlwidget;
		GtkWidget *entry;
		GtkWidget *backward;
		GtkWidget *case_sensitive;
		gboolean regular;
	};

	typedef struct _text_data TEXT_DATA;
	struct _text_data {
		GtkWidget *html;
		GtkWidget *showtabs;
		gchar *mod_name;
		gchar *mod_description;
		gchar *search_string;
		gchar *key;
		gint mod_num;
		gboolean is_locked;
		/*
		gboolean
		    strongs,
		    morphs,
		    footnotes,
		    greekaccents,
		    lemmas,
		    scripturerefs, 
		    hebrewpoints, 
		    hebrewcant, 
		    headings;
		*/
		Text_Find_Dialog *find_dialog;
	};
	void gui_set_text_page_and_key(gint page_num, gchar * key);
	void gui_setup_text(SETTINGS * s);
	void gui_shutdown_text(void);
	void gui_display_text(gchar * key);

#ifdef __cplusplus
}
#endif
#endif				/* __GS_BIBLETEXT_H_ */
