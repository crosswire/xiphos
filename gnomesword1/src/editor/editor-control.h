/***************************************************************************
 *            editor-control.h
 *
 *  Thu Feb 10 09:07:42 2005
 *  Copyright  2005  terry
 *  tbiggs@users.sf.net
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _EDITOR_CONTROL_H
#define _EDITOR_CONTROL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <config.h>	
#ifdef USE_GTKHTML38

typedef struct _editor EDITOR;
struct _editor {
	GtkWidget *window;
	GtkWidget *toolbar;
	GtkWidget *html_widget;
	GtkWidget *statusbar;
	
	gboolean studypad;
	gboolean is_changed;
	
	gchar *filename;
	gchar *module;
	gchar *key;
};

gint 
editor_create_new(const gchar * filename, const gchar * key, gint note);
gint
load_file (const gchar *fname);	
		
#else
	
#include "editor/editor.h"	
enum {
	STUDYPAD,
	PERCOM
};

GSHTMLEditorControlData *editor_new(GtkWidget * container, int type, char * filename);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _EDITOR_CONTROL_H */
