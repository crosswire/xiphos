/*
 * GnomeSword Bible Study Tool
 * commentary_dialog.c - dialog for a commentary module
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
#include <gtkhtml/gtkhtml.h>
#include <gal/widgets/e-unicode.h>

#include "gui/gtkhtml_display.h"
#include "gui/commentary_dialog.h"

#include "main/commentary.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"
#include "main/lists.h"
#include "main/settings.h"


/****************************************************************************************
 * static - global to this file only
 */
static GList * dialog_list;
static VIEW_COMM * cur_vc;

 

/******************************************************************************
 * Name
 *  display
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void display(VIEW_COMM *c, gchar * key)	
 *
 * Description
 *   calls entry_display to display a commentary entry 
 *
 * Return value
 *   void
 */
 
static void display(VIEW_COMM *c, gchar * key)
{
	gchar *text_str = NULL;
	gchar *strkey;
	
	strkey = get_valid_key(key);
	strcpy(c->key, strkey);
	
	if(c->book_heading){
		c->book_heading = FALSE;
		text_str = get_book_heading(c->mod_name, strkey);
	}
	
	else if(c->chapter_heading){
		c->chapter_heading = FALSE;
		text_str = get_chapter_heading(c->mod_name, strkey);
	}
	
	else {
		text_str = get_commentary_text(c->mod_name, strkey);
	}
	entry_display(c->html, c->mod_name, text_str, key);
	free(text_str);
	
}

/******************************************************************************
 * Name
 *   on_btnVCClose_clicked
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_btnVCClose_clicked(GtkButton * button, 
 *						VIEW_COMM * vc)	
 *
 * Description
 *   hide the dialog but do not destroy
 *
 * Return value
 *   void
 */

static void on_btn_close_clicked(GtkButton * button, 
						VIEW_COMM * vc)
{
	gtk_widget_destroy(vc->dialog);
	vc->dialog = NULL;
}

/******************************************************************************
 * Name
 *   on_dlgViewComm_destroy
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_dlgViewComm_destroy(GtkObject * object, 
 *						VIEW_COMM * vc)	
 *
 * Description
 *   shut down the View Commentay Dialog
 *
 * Return value
 *   void
 */

static void on_dialog_destroy(GtkObject * object, 
						VIEW_COMM * vc)
{
	//shutdown_viewcomm();
}

/******************************************************************************
 * Name
 *   on_btn_sync_clicked
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_btn_sync_clicked(GtkButton * button, VIEW_COMM * vc)	
 *
 * Description
 *   bring the the View Commentay Dialog module into sync with main window
 *
 * Return value
 *   void
 */

static void on_btn_sync_clicked(GtkButton * button, VIEW_COMM * vc)
{
	display(vc, settings.currentverse);
}

/******************************************************************************
 * Name
 *   module_new_activate
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void module_new_activate(GtkEditable * editable, 
 *					gpointer user_data)	
 *
 * Description
 *   load new commentary module
 *
 * Return value
 *   void
 */

static void module_new_activate(GtkEditable * editable, 
					gpointer user_data)
{
	gui_open_commentary_dialog(GPOINTER_TO_INT(user_data));
}

/******************************************************************************
 * Name
 *   on_btn_goto_clicked
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_btn_goto_clicked(GtkButton * button, VIEW_COMM * vc)	
 *
 * Description
 *   goto new versekey
 *
 * Return value
 *   void
 */

static void on_btn_goto_clicked(GtkButton * button, VIEW_COMM * vc)
{
	gchar *buf;

	buf = gtk_entry_get_text(GTK_ENTRY(vc->entry_key));
	display(vc, buf);
}

/******************************************************************************
 * Name
 *   on_btn_prev_clicked
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_btn_prev_clicked(GtkButton * button, VIEW_COMM * vc)
 *
 * Description
 *   goto previous module key
 *
 * Return value
 *   void
 */

static void on_btn_prev_clicked(GtkButton * button, VIEW_COMM * vc)
{
	gchar *key;
	
	set_commentary_key(vc->mod_name, vc->key);
	key = navigate_commentary(vc->mod_name, 0);
	if(key) {		
		strcpy(vc->key, key);
		display(vc,key);
		free(key);
	}
}

/******************************************************************************
 * Name
 *   on_btn_next_clicked
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_btn_next_clicked(GtkButton * button, VIEW_COMM * vc)	
 *
 * Description
 *   goto next module key
 *
 * Return value
 *   void
 */

static void on_btn_next_clicked(GtkButton * button, VIEW_COMM * vc)
{
	gchar *key;
	
	set_commentary_key(vc->mod_name, vc->key);
	key = navigate_commentary(vc->mod_name, 1);
	if(key) {		
		strcpy(vc->key, key);
		display(vc,key);
		free(key);
	}
}

/******************************************************************************
 * Name
 *   on_dialog_motion_notify_event
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   gboolean on_dialog_motion_notify_event(GtkWidget *widget,
                      GdkEventMotion  *event, VIEW_COMM * vc)	
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean on_dialog_motion_notify_event(GtkWidget *widget,
                      GdkEventMotion  *event, VIEW_COMM * vc)
{
	cur_vc = vc;
	return TRUE;
}

/******************************************************************************
 * Name
 *   add_items_to_module_menu
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void add_items_to_module_menu(GtkWidget * shellmenu)	
 *
 * Description
 *   add commentary module names to new module menu
 *
 * Return value
 *   void
 */

static void add_items_to_module_menu(GtkWidget * shellmenu)
{
	GtkWidget *menuChoice;
	gchar menuName[64];
	int view_number = 0;
	GList *tmp;

	tmp = NULL;

	tmp = get_list(COMM_LIST);
	while (tmp != NULL) {

		/* add module name items to menu */
		menuChoice =
		    gtk_check_menu_item_new_with_label((gchar *) (gchar
								  *)
						       tmp->data);
		sprintf(menuName, "ModuleNum%d", view_number);
		gtk_object_set_data(GTK_OBJECT(settings.app), menuName,
				    menuChoice);
		gtk_widget_show(menuChoice);
		gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
				   GTK_SIGNAL_FUNC(module_new_activate),
				   GINT_TO_POINTER(view_number));
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
				      GTK_WIDGET(menuChoice), view_number);
		++view_number;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *   gui_create_commentary_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_create_commentary_dialog(void)	
 *
 * Description
 *   create a Commentary Dialog
 *
 * Return value
 *   void
 */

void create_commentary_dialog(VIEW_COMM * vc)
{
	GtkWidget *dialog_vbox11;
	GtkWidget *vbox30;
	GtkWidget *toolbar;
	GtkWidget *vseparator;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *menubar;
	GtkWidget *module_new;
	GtkWidget *module_new_menu;
	GtkAccelGroup *module_new_menu_accels;
	GtkWidget *frame19;
	GtkWidget *scrolledwindow38;
	GtkWidget *dialog_action_area11;

	vc->dialog =
	    gnome_dialog_new(_("GnomeSword - View Commentary"), NULL);
	gtk_object_set_data(GTK_OBJECT(vc->dialog), "vc->dialog",
			    vc->dialog);
	GTK_WINDOW(vc->dialog)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_default_size(GTK_WINDOW(vc->dialog), 462, 280);
	gtk_window_set_policy(GTK_WINDOW(vc->dialog), TRUE, TRUE, FALSE);

	dialog_vbox11 = GNOME_DIALOG(vc->dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(vc->dialog), "dialog_vbox11",
			    dialog_vbox11);
	gtk_widget_show(dialog_vbox11);

	vbox30 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox30);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "vbox30", vbox30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox30);
	gtk_box_pack_start(GTK_BOX(dialog_vbox11), vbox30, TRUE, TRUE, 0);

	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "toolbar",
				 toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_box_pack_start(GTK_BOX(vbox30), toolbar, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vc->dialog,
				      GNOME_STOCK_PIXMAP_REFRESH);
	vc->btn_sync =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button7"),
				       _("Sync with main window"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(vc->btn_sync);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "vc->btn_sync",
				 vc->btn_sync,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vc->btn_sync);

	vseparator = gtk_vseparator_new();
	gtk_widget_ref(vseparator);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
				 "vseparator", vseparator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator, 7, 13);

	menubar = gtk_menu_bar_new();
	gtk_widget_ref(menubar);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "menubar",
				 menubar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), menubar, NULL,
				  NULL);

	module_new =
	    gtk_menu_item_new_with_label(_("Open New Module "));
	gtk_widget_ref(module_new);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
				 "module_new", module_new,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(module_new);
	gtk_container_add(GTK_CONTAINER(menubar), module_new);

	module_new_menu = gtk_menu_new();
	gtk_widget_ref(module_new_menu);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
				 "module_new_menu",
				 module_new_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_new),
				  module_new_menu);
	module_new_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (module_new_menu));

	add_items_to_module_menu(module_new_menu);

	vc->entry_key = gtk_entry_new();
	gtk_widget_ref(vc->entry_key);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "vc->entry_key", vc->entry_key,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vc->entry_key);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vc->entry_key,
				  _("Enter a verse"), NULL);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vc->dialog,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	vc->btn_goto =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button10"), _("Goto Verse"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(vc->btn_goto);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "vc->btn_goto",
				 vc->btn_goto,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vc->btn_goto);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vc->dialog,
				      GNOME_STOCK_PIXMAP_DOWN);
	vc->btn_prev =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button9"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(vc->btn_prev);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "vc->btn_prev",
				 vc->btn_prev,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vc->btn_prev);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vc->dialog, GNOME_STOCK_PIXMAP_UP);
	vc->btn_next =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button8"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(vc->btn_next);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "vc->btn_next",
				 vc->btn_next,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vc->btn_next);

	frame19 = gtk_frame_new(NULL);
	gtk_widget_ref(frame19);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "frame19",
				 frame19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame19);
	gtk_box_pack_start(GTK_BOX(vbox30), frame19, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame19), GTK_SHADOW_IN);

	scrolledwindow38 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow38);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
				 "scrolledwindow38", scrolledwindow38,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow38);
	gtk_container_add(GTK_CONTAINER(frame19), scrolledwindow38);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow38),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	vc->html = gtk_html_new();
	gtk_widget_ref(vc->html);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "vc->html", vc->html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vc->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow38), vc->html);
	gtk_html_load_empty(GTK_HTML(vc->html));

	dialog_action_area11 = GNOME_DIALOG(vc->dialog)->action_area;
	gtk_object_set_data(GTK_OBJECT(vc->dialog),
			    "dialog_action_area11", dialog_action_area11);
	gtk_widget_show(dialog_action_area11);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area11),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area11),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(vc->dialog),
				   GNOME_STOCK_BUTTON_CLOSE);
	vc->btn_close = g_list_last(GNOME_DIALOG(vc->dialog)->buttons)->data;
	gtk_widget_ref(vc->btn_close);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "vc->btn_close",
				 vc->btn_close,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vc->btn_close);
	GTK_WIDGET_SET_FLAGS(vc->btn_close, GTK_CAN_DEFAULT);
	
	gtk_signal_connect(GTK_OBJECT(vc->dialog), "destroy",
			   GTK_SIGNAL_FUNC(on_dialog_destroy), 
			   vc);
	gtk_signal_connect (GTK_OBJECT (vc->dialog), "motion_notify_event",
			   GTK_SIGNAL_FUNC (on_dialog_motion_notify_event),
			   vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_sync), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_sync_clicked), 
			   vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_goto), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_goto_clicked), 
			   vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_prev), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_prev_clicked), 
			   vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_next), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_next_clicked), 
			   vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_close), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_close_clicked), 
			   vc);
	gtk_signal_connect(GTK_OBJECT(vc->html), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), 
			   vc);			   
	gtk_signal_connect (GTK_OBJECT (vc->html), "on_url",
			    GTK_SIGNAL_FUNC (on_url), 
			    (gpointer)settings.app);
	
	gtk_entry_set_text(GTK_ENTRY(vc->entry_key), settings.currentverse);
	display(vc, settings.currentverse);
}

/******************************************************************************
 * Name
 *   gui_open_commentary_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_open_commentary_dialog(gint mod_num)	
 *
 * Description
 *   create a commentary dialog by calling create_commentary_dialog()
 *   or bring an already existing dialog to the top
 *
 * Return value
 *   void
 */

void gui_open_commentary_dialog(gint mod_num)
{		
	VIEW_COMM * vc; 
	
	vc = (VIEW_COMM *) g_list_nth_data(dialog_list, mod_num);	
	
	g_warning(vc->mod_name);
	if(vc->dialog == NULL){
		create_commentary_dialog(vc);
		gtk_widget_show(vc->dialog);
	}
	else {
		gtk_widget_show(vc->dialog);
		gdk_window_raise(GTK_WIDGET(vc->dialog)->window);
	}
}

/******************************************************************************
 * Name
 *   gui_setup_commentary_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_setup_commentary_dialog(GList *mods)	
 *
 * Description
 *   setup support for commentary dialogs.
 *   create a list of structures that will allow user to display a
 *   dialog for each commentary installed.
 *
 * Return value
 *   void
 */

void gui_setup_commentary_dialog(GList *mods)
{
	VIEW_COMM * vc;
	GList *tmp = NULL;
	gint count = 0;
	
	dialog_list = NULL;
	
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		vc = g_new(VIEW_COMM, 1);
		vc->dialog = NULL;
		vc->dialog_number = count;
		vc->mod_name = (gchar *) tmp->data;
		vc->book_heading = FALSE;
		vc->chapter_heading = FALSE;
		++count;
		dialog_list = g_list_append(dialog_list, (VIEW_COMM *) vc);
		tmp = g_list_next(tmp);
	}
}

/******************************************************************************
 * Name
 *  gui_shutdown_commentary_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *  void gui_shutdown_commentary_dialog(void)	
 *
 * Description
 *   shut down commentary dialog support and clean mem
 *
 * Return value
 *   void
 */

void gui_shutdown_commentary_dialog(void) 
{
	dialog_list = g_list_first(dialog_list);
	while (dialog_list != NULL) {
		VIEW_COMM *vc = (VIEW_COMM *) dialog_list->data;
		/* 
		 *  destroy any dialogs created 
		 */
		if (vc->dialog)
			 gtk_widget_destroy(vc->dialog);
		
		
		g_free((VIEW_COMM *) dialog_list->data);
		dialog_list = g_list_next(dialog_list);
	} g_list_free(dialog_list);
}



