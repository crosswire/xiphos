/*
 * GnomeSword Bible Study Tool
 * editor.c - the html editor
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/gtkhtmlfontstyle.h>
#include <gtkhtml/htmlform.h>
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
#include <gal/widgets/e-unicode.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "gui/editor.h"
#include "gui/editor_menu.h"
#include "gui/editor_replace.h"
#include "gui/editor_search.h"
#include "gui/toolbar_style.h"
#include "gui/toolbar_edit.h"
#include "gui/fileselection.h"
#include "gui/studypad.h"
#include "gui/html.h"

#include "main/percomm.h"
#include "main/settings.h"

/******************************************************************************
 * much of this code taken form GtkHTML
 */ 

/******************************************************************************
 * Name
 *  gs_html_editor_control_data_new
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   GSHTMLEditorControlData *gs_html_editor_control_data_new(void)	
 *
 * Description
 *    
 *
 * Return value
 *   GSHTMLEditorControlData *
 */
 
GSHTMLEditorControlData *gs_html_editor_control_data_new(void)
{
	GSHTMLEditorControlData *necd =
	    g_new0(GSHTMLEditorControlData, 1);

	necd->paragraph_option = NULL;
	necd->properties_types = NULL;
	necd->block_font_style_change = FALSE;
	necd->gdk_painter = NULL;
	necd->plain_painter = NULL;
	necd->format_html = FALSE;
	necd->changed = FALSE;
	necd->gbs = FALSE;
	necd->personal_comments = FALSE;
	necd->studypad = FALSE;
	sprintf(necd->filename, "%s", settings.studypadfilename);
	return necd;
}

/******************************************************************************
 * Name
 *  gs_html_editor_control_data_destroy
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void gs_html_editor_control_data_destroy(GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
 
void gui_html_editor_control_data_destroy(GtkObject * object,
			GSHTMLEditorControlData * ecd)
{
	g_assert(ecd);

	if (ecd->plain_painter)
		gtk_object_unref(GTK_OBJECT(ecd->plain_painter));

	if (ecd->gdk_painter)
		gtk_object_unref(GTK_OBJECT(ecd->gdk_painter));

	g_free(ecd);
}

/******************************************************************************
 * Name
 *  gui_update_statusbar
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void gui_update_statusbar(GSHTMLEditorControlData * ecd)
 *
 * Description
 *    update information in editor status bar
 *
 * Return value
 *   void
 */ 

void gui_update_statusbar(GSHTMLEditorControlData * ecd)
{
	gint context_id2;
	gchar buf[255], buf3[255];
	gchar *buf2;

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(ecd->statusbar),
					 "GnomeSword");/* string not seen */
	gtk_statusbar_pop(GTK_STATUSBAR(ecd->statusbar), context_id2);

	if (ecd->personal_comments) {
		sprintf(buf3,"[%s] %s", ecd->filename, ecd->key);
		buf2 = buf3;
	}		
	else
		buf2 = ecd->filename;

	if (ecd->changed) {
		sprintf(buf, "%s - modified", buf2);
		if(!ecd->personal_comments && !ecd->gbs)
			settings.modifiedSP = TRUE;
		if(ecd->personal_comments)
			settings.modifiedPC = TRUE;
		if(ecd->gbs)
			settings.modifiedGBS = TRUE;
	} else {
		sprintf(buf, "%s", buf2);
		if(!ecd->personal_comments && !ecd->gbs)
			settings.modifiedSP = FALSE;
		if(ecd->personal_comments)
			settings.modifiedPC = FALSE;
		if(ecd->gbs)
			settings.modifiedGBS = FALSE;
	} 

	gtk_statusbar_push(GTK_STATUSBAR(ecd->statusbar), context_id2,
			   buf);
}

/******************************************************************************
 * Name
 *  run_dialog
 *
 * Synopsis
 *   #include "_editor.h"
 *
 *   void run_dialog(GnomeDialog *** dialog, GtkHTML * html, 
 *				DialogCtor ctor, const gchar * title)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
 
void run_dialog(GnomeDialog *** dialog, GtkHTML * html, 
				DialogCtor ctor, const gchar * title)
{
	if (*dialog) {
		gtk_window_set_title(GTK_WINDOW(**dialog), title);
		gtk_widget_show(GTK_WIDGET(**dialog));
		gdk_window_raise(GTK_WIDGET(**dialog)->window);
	} else {
		*dialog = ctor(html);
		gtk_window_set_title(GTK_WINDOW(**dialog), title);
		gtk_widget_show(GTK_WIDGET(**dialog));
	}
}

