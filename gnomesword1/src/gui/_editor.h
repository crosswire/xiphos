/*
 * GnomeSword Bible Study Tool
 * _editor.h - the html editor
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

#ifndef ___EDITOR_H_
#define ___EDITOR_H_

#include <gnome.h>
#include "main/settings.h"

typedef struct _GSHTMLEditorControlData GSHTMLEditorControlData;
#include "gui/editor_replace.h"
#include "gui/editor_search.h"

typedef GnomeDialog ** (*DialogCtor)(GtkHTML *html);

#define RUN_DIALOG(name,title) run_dialog ((GnomeDialog ***)&ecd-> name ## _dialog, ecd->html, (DialogCtor) gs_editor_ ## name ## _dialog_new, title)

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
	GSHTMLSearchDialog  *search_dialog;
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
		*handlebox_toolbar,
		*toolbar_style;
	GtkWidget *frame_toolbar;
	/* menu items */
	 GtkWidget
	    *editnote,
	    *show_tabs,
	    *file,
	    *edit2,
	    *link;
	/* 
	   toolbar_commands buttons 
	 */	
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
	gboolean gbs; /** gen book support **/
	gboolean studypad; 
	gchar filename[256];

};

void update_statusbar(GSHTMLEditorControlData * ecd);
GSHTMLEditorControlData *gs_html_editor_control_data_new(SETTINGS * s);
void gs_html_editor_control_data_destroy(GSHTMLEditorControlData * cd);
void on_editor_destroy(GtkObject * object, GSHTMLEditorControlData * ecd);
void run_dialog (GnomeDialog ***dialog, GtkHTML *html, DialogCtor ctor, const gchar *title);

#endif

