/*
 * GnomeSword Bible Study Tool
 * gs_parallel.c - support for displaying multiple modules
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
#include <gal/widgets/e-unicode.h>

#include "gui/parallel_view.h"
#include "gui/parallel_dialog.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/parallel_view.h"
#include "main/bibletext.h"
#include "main/global_ops.hh"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"



/******************************************************************************
 * static
 */

/******************************************************************************
 * Name
 *   on_undockInt_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_undockInt_activate(GtkMenuItem *menuitem)
 *
 * Description
 *   undock/dock parallel page
 *
 * Return value
 *   void
 */

static void on_undockInt_activate(GtkMenuItem * menuitem)
{
	if (settings.dockedInt) {
		settings.dockedInt = FALSE;
		gui_undock_parallel_page();

	} else {
		settings.dockedInt = TRUE;
		gui_btnDockInt_clicked(NULL, NULL);
	}
}


/******************************************************************************
 * Name
 *   on_changeint1mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint1mod_activate(GtkMenuItem * menuitem,
 *			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint1mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description((gchar *) user_data);
	if(!mod_name) 
		return;
	main_change_parallel_module(PARALLEL1, mod_name);
}



/******************************************************************************
 * Name
 *   on_changeint2mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint2mod_activate(GtkMenuItem * menuitem,
 *			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint2mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description((gchar *) user_data);
	if(!mod_name) 
		return;
	main_change_parallel_module(PARALLEL2, mod_name);
}


/******************************************************************************
 * Name
 *   on_changeint3mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint3mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint3mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description((gchar *) user_data);
	if(!mod_name) 
		return;
	main_change_parallel_module(PARALLEL3, mod_name);
}


/******************************************************************************
 * Name
 *   on_changeint4mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint4mod_activate(GtkMenuItem * menuitem,
 *			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint4mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description((gchar *) user_data);
	if(!mod_name) 
		return;
	main_change_parallel_module(PARALLEL4, mod_name);
}


/******************************************************************************
 * Name
 *   on_changeint5mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint5mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint5mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description((gchar *) user_data);

	if(!mod_name) 
		return;
	main_change_parallel_module(PARALLEL5, mod_name);
}


/******************************************************************************
 * Name
 *   create_parallel_popup
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   GtkWidget * create_parallel_popup(GList * mods)
 *
 * Description
 *   create popup menu for parallel window
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_parallel_popup(void)
{
	GtkWidget *pmInt;
	GtkAccelGroup *pmInt_accels;
	GtkWidget *copy7;
	GtkWidget *undockInt;
	GtkWidget *module_options;
	GtkWidget *separator2;
	GtkTooltips *tooltips;	
	GtkWidget *module_options_menu;
	
	tooltips = gtk_tooltips_new();
	pmInt = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmInt), "pmInt", pmInt);

	copy7 = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_show(copy7);
	gtk_container_add(GTK_CONTAINER(pmInt), copy7);

	separator2 = gtk_menu_item_new();
	gtk_widget_show(separator2);
	gtk_container_add(GTK_CONTAINER(pmInt), separator2);
	gtk_widget_set_sensitive(separator2, FALSE);

	undockInt = gtk_menu_item_new_with_label(_("Detach/Attach"));
	gtk_widget_show(undockInt);
	gtk_container_add(GTK_CONTAINER(pmInt), undockInt);

	module_options =
	    gtk_menu_item_new_with_label(_("Module Options"));
	gtk_widget_show(module_options);
	gtk_container_add(GTK_CONTAINER(pmInt), module_options);

	module_options_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_options),
				  module_options_menu);
				  
	main_load_g_ops_parallel(module_options_menu);

	separator2 = gtk_menu_item_new();
	gtk_widget_show(separator2);
	gtk_container_add(GTK_CONTAINER(pmInt), separator2);
	gtk_widget_set_sensitive(separator2, FALSE);
	/* build change parallel modules submenu */
	main_load_menu_form_mod_list(pmInt, _("Change parallel 1"),
			       (GCallback)
			       on_changeint1mod_activate);
	main_load_menu_form_mod_list(pmInt, _("Change parallel 2"),
			       (GCallback)
			       on_changeint2mod_activate);
	main_load_menu_form_mod_list(pmInt, _("Change parallel 3"),
			       (GCallback)
			       on_changeint3mod_activate);
	main_load_menu_form_mod_list(pmInt, _("Change parallel 4"),
			       (GCallback)
			       on_changeint4mod_activate);
	main_load_menu_form_mod_list(pmInt, _("Change parallel 5"),
			       (GCallback)
			       on_changeint5mod_activate);

	g_signal_connect(GTK_OBJECT(copy7), "activate",
			   G_CALLBACK(gui_copyhtml_activate),
			   NULL);
	g_signal_connect(GTK_OBJECT(undockInt), "activate",
			   G_CALLBACK(on_undockInt_activate),
			   &settings);

	return pmInt;
}


/******************************************************************************
 * Name
 *   gui_create_parallel_popup
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_create_parallel_popup(GList *bible_description)
 *
 * Description
 *   call create_parallel_popup() and attach menu to int html widget
 *
 * Return value
 *   void
 */

void gui_create_parallel_popup(void)
{
	/* create popup menu for parallel window */
	GtkWidget *menu_inter =
	    create_parallel_popup();
	/* attach popup menus */
	gnome_popup_menu_attach(menu_inter,
				widgets.html_parallel,
				(gchar *) "1");
}


/******************************************************************************
 * Name
 *   gui_create_parallel_page
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_create_parallel_page(guint page_num)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_create_parallel_page(void)
{
	GtkWidget *scrolled_window;
	GtkWidget *label;
	/*
	 * parallel page 
	 */
	widgets.frame_parallel = gtk_frame_new(_("Parallel View"));
	gtk_widget_show(widgets.frame_parallel);
	gtk_frame_set_shadow_type(GTK_FRAME(widgets.frame_parallel),GTK_SHADOW_NONE);
	
	
	gtk_container_add(GTK_CONTAINER(widgets.notebook_parallel_text),
			  widgets.frame_parallel);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolled_window);
	gtk_container_add(GTK_CONTAINER(widgets.frame_parallel),
			  scrolled_window);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);

	settings.dockedInt = TRUE;

	widgets.html_parallel = gtk_html_new();
	gtk_widget_show(widgets.html_parallel);
	gtk_html_load_empty(GTK_HTML(widgets.html_parallel));
	gtk_container_add(GTK_CONTAINER(scrolled_window),
			  widgets.html_parallel);


	label = gtk_label_new(_("Parallel View"));
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_parallel_text),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.notebook_parallel_text), 
				    1),
				   label);
	
	g_signal_connect(GTK_OBJECT(widgets.html_parallel),
			   "on_url", G_CALLBACK(gui_url),
			   (gpointer) widgets.app);
	g_signal_connect(GTK_OBJECT(widgets.html_parallel),
			   "link_clicked",
			   G_CALLBACK(gui_link_clicked), NULL);
			   /*
	g_signal_connect(GTK_OBJECT(widgets.html_parallel),
			   "button_release_event",
			   G_CALLBACK(button_release_event), NULL);
			   */
}
