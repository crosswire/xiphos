/*
 * GnomeSword Bible Study Tool
 * setup_druid.h gnomesword fristrun druid
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
 
#ifndef __SETUP_DRUID_H__
#define __SETUP_DRUID_H__
 
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _settings_druid SETTINGS_DRUID;
struct _settings_druid {
	GtkWidget *combo_entry_text;
	GtkWidget *combo_entry_int1;
	GtkWidget *combo_entry_int2;
	GtkWidget *combo_entry_int3;
	GtkWidget *combo_entry_int4;
	GtkWidget *combo_entry_int5;
	GtkWidget *combo_entry_comm;
	GtkWidget *combo_entry_gbs;
	GtkWidget *combo_entry_personal;
	GtkWidget *combo_entry_dict;
	GtkWidget *combo_entry;
	GtkWidget *entry_greek_lex;
	GtkWidget *entry_hebrew_lex;
	GtkWidget *combo_entry_devotion;
	GtkWidget *radiobutton_use_default;
	GtkWidget *checkbutton_verse_style;
	GtkWidget *checkbutton_text_tabs;
	GtkWidget *checkbutton_text_window;
	GtkWidget *checkbutton_comm_tabs;
	GtkWidget *checkbutton_upper_workbook;
	GtkWidget *checkbutton_dict_tabs;
	GtkWidget *checkbutton_lower_workbook;
	GtkWidget *checkbutton_book_tabs;
};

gint gui_first_run(void);

#ifdef __cplusplus
}
#endif	

#endif

