/* 
 * added to gnomesword 2001-01-06 
*/

/* Bluefish HTML Editor
 * spell_gui.h - Spell Cheking GUI prototypes
 *
 * Copyright (C) 2000 Pablo De Napoli (for this module)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef __SPELL_H_
#define __SPELL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>
#include "_editor.h"


gboolean run_spell_checker(GSHTMLEditorControlData * ecd);
void load_text_from_spell(GtkWidget * text,
				 GSHTMLEditorControlData * ecd);
gboolean load_text_for_spell(GtkWidget * text,
				    GSHTMLEditorControlData * ecd);	
void ispell_terminate(void);

void destroy_spc_window (void);

extern gint spc_message;

/* messages to the spell checker */
enum {
        SPC_NONE,
        SPC_CLOSE,
        SPC_ACCEPT,
        SPC_IGNORE,
        SPC_INSERT,
        SPC_REPLACE
};

typedef struct {
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *scrollwindow;
  GtkWidget *text;	
  GtkWidget *hbuttonbox_top;
  GtkWidget *start_button;
  GtkWidget *close_button;
  GtkWidget *options_button;
  GtkWidget *word_frame;
  GtkWidget *word_entry;
  GtkWidget *replace_frame;
  GtkWidget *replace_entry;
  GtkWidget *progress_frame;
  GtkWidget *progress_bar;
  GtkWidget *button_table;
  GtkWidget *accept_button;
  GtkWidget *insert_button;
  GtkWidget *ignore_button;
  GtkWidget *replace_button;
  GtkWidget *near_misses_clist;
  GtkWidget *near_misses_scrolled_window;
  gint      status_bar_count;
} Tspc_gui;

extern Tspc_gui spc_gui;

#ifdef __cplusplus
}
#endif

#endif							/* __SPC_H_ */



