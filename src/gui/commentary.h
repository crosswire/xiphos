/*
 * GnomeSword Bible Study Tool
 * commentary.h - gui for commentary modules
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#ifndef ___COMMENTARY_H_
#define ___COMMENTARY_H_

#include "gui/editor.h"

typedef struct _comm_global_ops COMM_GLOBALS;
struct  _comm_global_ops {
	gboolean
	    words_in_red,
	    strongs,
	    morphs,
	    footnotes,
	    greekaccents,
	    lemmas,
	    scripturerefs,
	    hebrewpoints, 
	    hebrewcant, 
	    headings, 
	    variants_all, 
	    variants_primary, 
	    variants_secondary;
};

typedef struct _commdata COMM_DATA;
struct _commdata {
	/* for commentary dialogs */
	GtkWidget *dialog;	
	GtkWidget *cbe_book;
	GtkWidget *spb_chapter;
	GtkWidget *spb_verse;
	GtkWidget *freeform_lookup;
	GtkWidget *statusbar;
	/* for personal comments editor */
	GSHTMLEditorControlData *ec;
	/* for commentary panes */
	GtkWidget *vbox;
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *btnCOMMSync;
	GtkWidget *btnCOMMBack;
	GtkWidget *btnCOMMForward;
	GtkWidget *btnCOMMPrint;
	GtkWidget *btn_book_heading;
	GtkWidget *btn_chap_heading;
	GtkWidget *showtabs;
	GtkWidget *module_options_menu;
	
	gchar *mod_name;
	gchar *search_string;
	gchar *cipher_key;
	gchar *cipher_old;
	gchar key[80];
	
	gint modnum;
	
	gboolean is_locked;
	gboolean is_percomm;
	gboolean is_dialog;
	gboolean book_heading;
	gboolean chapter_heading;
	
	COMM_GLOBALS *cgs;
};
extern COMM_DATA *cur_c;

void gui_set_comm_frame_label(COMM_DATA * c);
void gui_set_commentary_page_and_key(gint page_num,
				     gchar * key);
void gui_display_commentary(gchar * key);
void gui_display_commentary_with_struct(COMM_DATA *c, gchar * key);
void gui_add_new_comm_pane(COMM_DATA *c);
void gui_setup_commentary(GList *mods);
void gui_shutdown_commentary(void);
#endif

