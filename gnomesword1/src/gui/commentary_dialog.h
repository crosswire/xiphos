/*
 * GnomeSword Bible Study Tool
 * commentary_dialog.h - dialog for a commentary module
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

#ifndef __COMMENTARY_DIALOG_H_
#define __COMMENTARY_DIALOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gui/editor.h"
#include "gui/mod_global_ops.h"
//#include "gui/commentary.h"
	
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
	gboolean sync;
	
	GLOBAL_OPS *ops;
};
extern COMM_DATA *cur_c;
void gui_keep_comm_dialog_in_sync(gchar * key);	
void gui_commentary_dialog_goto_bookmark(gchar * mod_name, gchar * key);
void gui_on_lookup_commentary_dialog_selection
	(GtkMenuItem * menuitem, gchar * dict_mod_description);
void gui_close_comm_dialog(COMM_DATA * vc);
void gui_open_commentary_dialog(gchar * mod_name);
void gui_open_commentary_editor(gchar * mod_name);
void gui_setup_commentary_dialog(GList *mods);
void gui_shutdown_commentary_dialog(void);
void gui_display_commentary_in_dialog(gchar * key);

#ifdef __cplusplus
}
#endif

#endif
