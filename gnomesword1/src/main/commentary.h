/*
 * GnomeSword Bible Study Tool
 * commentary.h - glue for commentary modules and commentary dialog
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

#ifndef __GS_COMMENTARY_H_
#define __GS_COMMENTARY_H_

#ifdef __cplusplus
extern "C" {
#endif
	
/* main */
#include "main/settings.h"	

typedef struct _commdata COMM_DATA;

#include "gui/commentary_find.h"

struct _commdata {
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *frame_toolbar;
	GtkWidget *btnCOMMSync;
	GtkWidget *btnCOMMBack;
	GtkWidget *btnCOMMForward;
	GtkWidget *btnCOMMPrint;
	GtkWidget *btn_book_heading;
	GtkWidget *btn_chap_heading;
	GtkWidget *showtabs;
	GtkWidget *showtoolbar;
	gchar *modName;
	gchar *searchstring;
	gchar key[80];
	gint modnum;
	gboolean has_key;
	COMMFindDialog *find_dialog;
};	

void display_book_heading(int mod_num);
void display_chap_heading(int mod_num);
const char* navigate_commentary(gint modnum, gint direction);
void set_commentary_page_and_key(gint page_num,
				     gchar * key);
void setup_commentary(GList *mods);
void shutdown_commentary(void);
void display_commentary(gchar * key);
void shutdown_viewcomm(void);
void goto_verse_viewcomm(gchar *verse);
void load_module_viewcomm(gchar *module_name);
void nav_module_viewcomm(gint direction);
void setup_viewcomm(GtkWidget * text);

#ifdef __cplusplus
}
#endif

#endif

