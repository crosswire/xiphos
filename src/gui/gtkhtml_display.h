/*
 * GnomeSword Bible Study Tool
 * gtkhtml_dispaly.h - display Sword modules using GtkHTML
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

#ifndef _GTKHTML_DISPALY_H_
#define _GTKHTML_DISPALY_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#include "gui/bibletext.h"
	
#define HTML_START "<html><head><meta http-equiv='content-type' content='text/html; charset=utf-8'></head>"
	
void entry_display(GtkWidget * html_widget, gchar * mod_name, 
 				        gchar * text, gchar * key, 
					gboolean show_key);
void chapter_display(GtkWidget * html_widget, gchar * module_name, 
		TEXT_GLOBALS * tgs, gchar * key, gboolean use_globals);
void chapter_display_mozilla(GtkWidget * html_widget, gchar * mod_name,
		TEXT_GLOBALS * tgs, gchar * key, gboolean use_globals);
void gui_module_is_locked_display(GtkWidget * html_widget, 
				gchar * mod_name, gchar * cipher_key);
	
#ifdef __cplusplus
}
#endif

#endif /* _GTKHTML_DISPALY_H_ */
