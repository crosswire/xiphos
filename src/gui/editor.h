/*
 * GnomeSword Bible Study Tool
 * editor.h - the html editor
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

#ifndef __EDITOR_H_
#define __EDITOR_H_

#include <gnome.h>
#ifdef USE_GNOME_SPELL	
#include <bonobo.h>
#include "gnome2/Spell.h"
#endif

typedef struct _GSHTMLEditorControlData GSHTMLEditorControlData;
#include "gui/editor_replace.h"

#ifdef USE_GNOME2
typedef GtkWidget ** (*DialogCtor)(GtkHTML *html);
#else
typedef GnomeDialog ** (*DialogCtor)(GtkHTML *html);
#endif

struct _GSHTMLEditorControlData {
	GtkHTML *html;
	GtkWidget *htmlwidget;
	GtkWidget *vbox;
	GtkWidget *statusbar;
	GtkWidget *frame;

	GtkWidget *cpicker;
	GtkWidget *combo;
	GtkWidget *paragraph_option;

	GList *properties_types;
	GSList *paragraph_group;

	/* search & replace dialogs */
	GtkHTMLReplaceDialog *replace_dialog;

	/* html/plain mode settings */
	gboolean format_html;
	HTMLGdkPainter *gdk_painter;
	HTMLGdkPainter *plain_painter;

	/* object from last button press event */
	HTMLObject *obj;

	/* button release signal id */
	guint releaseId;

	/* toolbars */
	GtkWidget 
		*toolbar_commands, 
		*toolbar_edit, 
		*toolbar_style;
	/* menu items */
	 GtkWidget
	    *editnote,
	    *toolbars,
	    *show_tabs,
	    *file,
	    *edit2,
	    *link;
	/* 
	   toolbar_commands buttons 
	 */	
	GtkWidget *btn_new;
	GtkWidget *btn_open;
	GtkWidget *btn_save;
	GtkWidget *btn_delete;
	GtkWidget *btn_print;
	GtkWidget *btn_cut;
	GtkWidget *btn_copy;
	GtkWidget *btn_paste;
	GtkWidget *btn_undo;
	GtkWidget *btn_Find;
	GtkWidget *btn_replace;
	GtkWidget *btn_spell;
	/* 
	   toolbar_style buttons 
	 */    
	GtkWidget
	    * tt_button,
	    *bold_button,
	    *italic_button,
	    *underline_button,
	    *strikeout_button,
	    *left_align_button,
	    *center_button,
	    *right_align_button, 
	    *unindent_button, 
	    *indent_button;
	GtkWidget *bold;
	GtkWidget *italic;
	GtkWidget *underline;
	GtkWidget *strikeout;

	GtkWidget *left_align;
	GtkWidget *center;
	GtkWidget *right_align;

	//GtkWidget *indent;
	//GtkWidget *unindent;

	GtkWidget *font_size_menu;
	GtkWidget *pm;

	guint font_style_changed_connection_id;
	gboolean block_font_style_change;
	gboolean changed;
	gboolean personal_comments;
	gboolean stylebar;
	gboolean editbar;
	gboolean gbs; /** gen book support **/
	gboolean studypad; 
	gchar filename[256];
	gchar key[256];
#ifdef USE_GNOME_SPELL	
	CORBA_sequence_GNOME_Spell_Language *languages;
	GNOME_Spell_Dictionary  dict;
	BonoboObject           *persist_stream;
        BonoboObject           *persist_file;
	BonoboControl          *control;
	Bonobo_PropertyBag spell_control_pb;
	gboolean has_spell_control;
	gboolean has_spell_control_set;
	gboolean spell_check_next;
	
#endif
	gboolean                block_language_changes;
	gchar                  *language;
//	EditorEngine           *editor_bonobo_engine;

	GtkWidget *spell_dialog;

};

void gui_update_statusbar(GSHTMLEditorControlData *ecd);
GSHTMLEditorControlData *gs_html_editor_control_data_new(void);
void gui_html_editor_control_data_destroy(GtkObject *object, 
					GSHTMLEditorControlData *cd);
/*void gui_editor_destroy(GtkObject *object, 
					GSHTMLEditorControlData *ecd);
void run_dialog (GnomeDialog ***dialog, GtkHTML *html, DialogCtor ctor,
						const gchar *title);*/

#endif
