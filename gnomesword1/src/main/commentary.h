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

void display_book_heading(int mod_num);
void display_chap_heading(int mod_num);
const char* navigate_commentary(gint modnum, gint direction);
void display_comm(gint page_num, gchar * key);
void new_display_commentary(GtkWidget * html, gchar * modName);
void shutdown_viewcomm(void);
void goto_verse_viewcomm(gchar *verse);
void load_module_viewcomm(gchar *module_name);
void nav_module_viewcomm(gint direction);
void setup_viewcomm(GtkWidget * text);

#ifdef __cplusplus
}
#endif

#endif

