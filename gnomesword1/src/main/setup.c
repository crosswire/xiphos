/*
 * GnomeSword Bible Study Tool
 * setup.c gnomesword fristrun stuff
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
#  include <config.h>
#endif

#include <gnome.h>

#include "sword.h"
#include "setup.h"
#include "properties.h"
#include "gs_gnomesword.h"

/******************************************************************************
 * Name
 *  save_frist_run_settings
 *
 * Synopsis
 *   #include "setup.h"
 *
 *   void save_frist_run_settings(SETTINGS_DRUID widgets)	
 *
 * Description
 *   gets settings from druid widgets and stores them in 
 *   the settings struct then calls backend_create_properties_from_setup
 *   to save them
 *
 * Return value
 *   void
 */ 

void save_frist_run_settings(SETTINGS_DRUID widgets)
{
	strcpy(settings.MainWindowModule,
		gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry1)));
	strcpy(settings.DictWindowModule, 
		gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry13)));
	strcpy(settings.CommWindowModule, 
		gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry12)));
	strcpy(settings.Interlinear1Module, 
		gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry2)));
	strcpy(settings.Interlinear2Module, 
		gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry3)));
	strcpy(settings.Interlinear3Module, 
		gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry4)));
	strcpy(settings.Interlinear4Module,
		gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry10)));
	strcpy(settings.Interlinear5Module, 
		gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry11)));
	strcpy(settings.personalcommentsmod,
		gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry14)));
	
	settings.usedefault = GTK_TOGGLE_BUTTON(widgets.radiobutton1)->active;
	settings.text_tabs = GTK_TOGGLE_BUTTON(widgets.checkbutton4)->active;
	settings.comm_tabs = GTK_TOGGLE_BUTTON(widgets.checkbutton5)->active;
	settings.dict_tabs = GTK_TOGGLE_BUTTON(widgets.checkbutton6)->active;
	settings.versestyle = GTK_TOGGLE_BUTTON(widgets.checkbutton2)->active;
	settings.autosavepersonalcomments = GTK_TOGGLE_BUTTON(widgets.checkbutton1)->active;
	settings.interlinearpage = GTK_TOGGLE_BUTTON(widgets.checkbutton3)->active;	
	
	backend_create_properties_from_setup();
}

/******************************************************************************
 * Name
 *  first_run
 *
 * Synopsis
 *   #include "setup.h"
 *
 *   void first_run(void)
 *
 * Description
 *    loads module list and calls 
 *    gui_create_setup_druid(biblemods, commmods, dictmods)
 *
 * Return value
 *   void
 */ 

void first_run(void)
{
	GtkWidget *dlg;
	GList *biblemods = NULL;
	GList *commmods = NULL;
	GList *dictmods = NULL;
	
	biblemods = backend_get_list_of_mods_by_type(TEXT_MODS);
	commmods = backend_get_list_of_mods_by_type(COMM_MODS);
	dictmods = backend_get_list_of_mods_by_type(DICT_MODS);
	
	dlg = gui_create_setup_druid(biblemods, commmods, dictmods);

	/*
	 * hold util we are done 
	 */
	gnome_dialog_set_default(GNOME_DIALOG(dlg), 2);
	gnome_dialog_run_and_close(GNOME_DIALOG(dlg));
	g_list_free(biblemods);
	g_list_free(commmods);
	g_list_free(dictmods);
}

/******************************************************************************
 * Name
 *  get_path_to_mods
 *
 * Synopsis
 *   #include "setup.h"
 *
 *   	gchar *get_path_to_mods(void)
 *
 * Description
 *    returns the path to the sword modules
 *
 * Return value
 *   get_path_to_mods
 */ 

gchar *get_path_to_mods(void)
{
	return backend_get_path_to_mods();
}
/******  end of file   ******/
