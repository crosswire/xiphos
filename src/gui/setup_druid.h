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
	GtkWidget *combo_entry1;
	GtkWidget *combo_entry2;
	GtkWidget *combo_entry3;
	GtkWidget *combo_entry4;
	GtkWidget *combo_entry10;
	GtkWidget *combo_entry11;
	GtkWidget *combo_entry12;
	GtkWidget *combo_entry13;
	GtkWidget *combo_entry14;
	GtkWidget *radiobutton1;
	GtkWidget *checkbutton1;
	GtkWidget *checkbutton3;
	GtkWidget *checkbutton2;
	GtkWidget *checkbutton4;
	GtkWidget *checkbutton5;
	GtkWidget *checkbutton6;
};

gint gui_first_run(void);

#ifdef __cplusplus
}
#endif	

#endif

