/*
 * GnomeSword Bible Study Tool
 * bibletext_dialog.h - view Bible text module in a dialog
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
 
#ifndef __MODULE_DIALOGS_H__
#define __MODULE_DIALOGS_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "gui/bibletext.h"

#include "main/global_ops.hh"
	


typedef struct _text_data TEXT_DATA;
struct _text_data {
	/* dialog widgets */
	GtkWidget *dialog;
	GtkWidget *toolbar_nav;
	GtkWidget *cbe_book;
	GtkWidget *spb_chapter;
	GtkWidget *spb_verse;
	GtkWidget *freeform_lookup;
	GtkWidget *statusbar;
	/* widgets for text panes */
	GtkWidget *vbox;
	GtkWidget *html;
	GtkWidget *text;
	GtkWidget *frame;
	GtkWidget *showtabs;
	GtkWidget *module_options_menu;
	
	gchar *mod_name;
	gchar *search_string;
	gchar *key;
	gchar *cipher_key;
	gchar *cipher_old;
	
	gint mod_num;
	
	gboolean is_locked;
	gboolean is_dialog;
	gboolean is_rtol;
	gboolean sync;
	
	gpointer backend;
	
	GLOBAL_OPS *ops;
};
extern TEXT_DATA *cur_t;	

void main_dialog_update_controls(TEXT_DATA * vt);
void main_bible_dialog_display(TEXT_DATA * t);
void main_bible_dialog_passage_changed(TEXT_DATA * t, gchar * passage);
void main_sync_bibletext_dialog_with_main(TEXT_DATA * vt);
//void gui_keep_bibletext_dialog_in_sync(gchar * key);
//void gui_bibletext_dialog_goto_bookmark(gchar * mod_name, gchar * key);
//void gui_sync_bibletext_dialog_with_main(TEXT_DATA * vt);
//void gui_close_text_dialog(TEXT_DATA * vt);
void main_setup_bibletext_dialog(GList *mods);
void main_shutdown_bibletext_dialog(void);
void main_free_on_destroy(TEXT_DATA * vt);
void main_open_bibletext_dialog(gchar * mod_name);
	
#ifdef __cplusplus
}
#endif	
	
#endif


/* end of file */
