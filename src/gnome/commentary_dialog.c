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
#include "gui/html.h"
#include "gui/gnomesword.h"
#include "gui/editor_menu.h"
#include "gui/toolbar_style.h"
#include "gui/toolbar_edit.h"

#include "main/commentary.h"
#include "main/percomm.h"
#include "main/sword.h"
#include "main/lists.h"
#include "main/settings.h"


/****************************************************************************************
 * static - global to this file only
 */
static GList *dialog_list;
static VIEW_COMM *cur_vc;
static gboolean dialog_freed;


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

static void display(VIEW_COMM * c, gchar * key)
{
	gchar *text_str = NULL;
	gchar *strkey;

	strkey = get_valid_key(key);
	strcpy(c->key, strkey);

	if (c->book_heading) {
		c->book_heading = FALSE;
		text_str = get_book_heading(c->mod_name, strkey);
	}

	else if (c->chapter_heading) {
		c->chapter_heading = FALSE;
		text_str = get_chapter_heading(c->mod_name, strkey);
	}

	else {
		if (c->ec) {
			settings.percomverse = key;
			change_percomm_module(c->mod_name);
			strcpy(c->ec->key, key);
			text_str = get_percomm_text(key);
			if (text_str) {
				entry_display(c->ec->htmlwidget,
					      c->mod_name,
					      text_str, key, FALSE);
				free(text_str);
				gui_update_statusbar(c->ec);
				return;
			}
		} else {
			text_str =
			    get_commentary_text(c->mod_name, strkey);
		}
	}

	if (text_str) {
		entry_display(c->html, c->mod_name,
			      text_str, key, TRUE);
		free(text_str);
	}
}


/******************************************************************************
 * Name
 *   free_on_destroy
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void free_on_destroy(VIEW_COMM * vc)
 *
 * Description
 *   removes dialog from dialog_list when dialog is destroyed other than
 *   program shut down
 *
 * Return value
 *   void
 */

static void free_on_destroy(VIEW_COMM * vc)
{
	dialog_list = g_list_remove(dialog_list, (VIEW_COMM *) vc);
//      g_warning("shuting down %s dialog",vc->mod_name);
	g_free(vc);
}


/******************************************************************************
 * Name
 *   on_dialog_destroy
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_dialog_destroy(GtkObject * object, 
 *						VIEW_COMM * vc)	
 *
 * Description
 *   shut down the View Commentay Dialog
 *
 * Return value
 *   void
 */

static void on_dialog_destroy(GtkObject * object, VIEW_COMM * vc)
{
	if (!dialog_freed) {
		if (vc->ec)
			gui_html_editor_control_data_destroy(NULL,
							     vc->ec);
		free_on_destroy(vc);
	}
	dialog_freed = FALSE;
	settings.percomm_dialog_exist = FALSE;
}


/******************************************************************************
 * Name
 *   on_btn_close_clicked
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_btn_close_clicked(GtkButton * button, 
 *						VIEW_COMM * vc)	
 *
 * Description
 *   hide the dialog but do not destroy
 *
 * Return value
 *   void
 */

static void on_btn_close_clicked(GtkButton * button, VIEW_COMM * vc)
{
	if (vc->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(vc->dialog);
	}
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
	gui_open_commentary_dialog((gchar *) user_data);
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
	if (key) {
		strcpy(vc->key, key);
		display(vc, key);
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
	if (key) {
		strcpy(vc->key, key);
		display(vc, key);
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

static gboolean on_dialog_motion_notify_event(GtkWidget * widget,
					      GdkEventMotion * event,
					      VIEW_COMM * vc)
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
		gtk_object_set_data(GTK_OBJECT(widgets.app), menuName,
				    menuChoice);
		gtk_widget_show(menuChoice);
		gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
				   GTK_SIGNAL_FUNC(module_new_activate),
				   (gchar *) tmp->data);
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
				      GTK_WIDGET(menuChoice),
				      view_number);
		++view_number;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}


/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, VIEW_COMM * vc)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_url(GtkHTML * html, const gchar * url,
		       VIEW_COMM * vc)
{

	cur_vc = vc;

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

static void create_commentary_dialog(VIEW_COMM * vc, gboolean do_edit)
{
	GtkWidget *vbox30;
	GtkWidget *vbox_toolbars;
	GtkWidget *toolbar;
	GtkWidget *vseparator;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *menubar;
	GtkWidget *module_new;
	GtkWidget *module_new_menu;
	GtkAccelGroup *module_new_menu_accels;
	GtkWidget *frame19;
	GtkWidget *scrolledwindow38;

	vc->dialog = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_object_set_data(GTK_OBJECT(vc->dialog), "vc->dialog",
			    vc->dialog);
	gtk_window_set_title(GTK_WINDOW(vc->dialog),
			     get_module_description(vc->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(vc->dialog), 462, 280);
	gtk_window_set_policy(GTK_WINDOW(vc->dialog), TRUE, TRUE,
			      FALSE);

	vbox30 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox30);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "vbox30",
				 vbox30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox30);

	gtk_container_add(GTK_CONTAINER(vc->dialog), vbox30);

	vbox_toolbars = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_toolbars);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
				 "vbox_toolbars", vbox_toolbars,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_toolbars);
	gtk_box_pack_start(GTK_BOX(vbox30), vbox_toolbars, FALSE, FALSE,
			   0);

	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog), "toolbar",
				 toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_box_pack_start(GTK_BOX(vbox_toolbars), toolbar, FALSE,
			   FALSE, 0);
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

	if (!do_edit) {
		menubar = gtk_menu_bar_new();
		gtk_widget_ref(menubar);
		gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
					 "menubar", menubar,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(menubar);
		gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), menubar,
					  NULL, NULL);

		module_new =
		    gtk_menu_item_new_with_label(_("Open New Module "));
		gtk_widget_ref(module_new);
		gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
					 "module_new", module_new,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(module_new);
		gtk_container_add(GTK_CONTAINER(menubar), module_new);

		module_new_menu = gtk_menu_new();
		gtk_widget_ref(module_new_menu);
		gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
					 "module_new_menu",
					 module_new_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_new),
					  module_new_menu);
		module_new_menu_accels =
		    gtk_menu_ensure_uline_accel_group(GTK_MENU
						      (module_new_menu));

		add_items_to_module_menu(module_new_menu);
	}

	vc->entry_key = gtk_entry_new();
	gtk_widget_ref(vc->entry_key);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
				 "vc->entry_key", vc->entry_key,
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
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
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
	    gnome_stock_pixmap_widget(vc->dialog,
				      GNOME_STOCK_PIXMAP_UP);
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

	vseparator = gtk_vseparator_new();
	gtk_widget_ref(vseparator);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
				 "vseparator", vseparator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator, 7, 13);


	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vc->dialog,
				      GNOME_STOCK_PIXMAP_EXIT);
	vc->btn_close =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Close"), _("Close Dialog"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(vc->btn_close);
	gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
				 "vc->btn_close", vc->btn_close,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vc->btn_close);


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


	if (do_edit) {
		vc->ec->htmlwidget = gtk_html_new();
		vc->ec->html = GTK_HTML(vc->ec->htmlwidget);
		gtk_widget_ref(vc->ec->htmlwidget);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vc->ec->htmlwidget",
					 vc->ec->htmlwidget,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vc->ec->htmlwidget);
		gtk_container_add(GTK_CONTAINER(scrolledwindow38),
				  vc->ec->htmlwidget);
		gtk_html_load_empty(vc->ec->html);

		vc->ec->vbox = vbox30;

		vc->ec->pm = gui_create_editor_popup(vc->ec);
		gnome_popup_menu_attach(vc->ec->pm, vc->ec->htmlwidget,
					NULL);

		vc->ec->statusbar = gtk_statusbar_new();
		gtk_widget_ref(vc->ec->statusbar);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vc->ec->statusbar",
					 vc->ec->statusbar,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vc->ec->statusbar);
		gtk_box_pack_start(GTK_BOX(vbox30), vc->ec->statusbar,
				   FALSE, TRUE, 0);
		/*
		   gtk_signal_connect(GTK_OBJECT(vc->ec->html), "submit",
		   GTK_SIGNAL_FUNC(on_submit), vc->ec);
		   gtk_signal_connect(GTK_OBJECT
		   (vc->ec->htmlwidget),
		   "load_done",
		   GTK_SIGNAL_FUNC(html_load_done), vc->ec);
		   gtk_signal_connect(GTK_OBJECT
		   (vc->ec->htmlwidget),
		   "key_press_event",
		   GTK_SIGNAL_FUNC(html_key_pressed), vc->ec);
		   gtk_signal_connect(GTK_OBJECT
		   (vc->ec->htmlwidget),
		   "button_press_event",
		   GTK_SIGNAL_FUNC(html_button_pressed), vc->ec);
		   gtk_signal_connect(GTK_OBJECT(vc->ec->htmlwidget),
		   "enter_notify_event",
		   GTK_SIGNAL_FUNC(on_html_enter_notify_event),
		   vc->ec);
		 */
		/* gs_html.c */
		gtk_signal_connect(GTK_OBJECT(vc->ec->htmlwidget),
				   "link_clicked",
				   GTK_SIGNAL_FUNC(gui_link_clicked),
				   NULL);
		/* gs_html.c */
		gtk_signal_connect(GTK_OBJECT(vc->ec->htmlwidget),
				   "on_url", GTK_SIGNAL_FUNC(gui_url),
				   NULL);

		gui_toolbar_style(vc->ec);
		gtk_box_pack_start(GTK_BOX(vbox_toolbars),
				   vc->ec->toolbar_style, FALSE, FALSE,
				   0);
		gtk_widget_hide(vc->ec->toolbar_style);
		gui_toolbar_edit(vc->ec);
		gtk_box_pack_start(GTK_BOX(vbox_toolbars),
				   vc->ec->toolbar_edit, FALSE, FALSE,
				   0);
		gtk_widget_hide(vc->ec->toolbar_edit);

	} else {
		vc->html = gtk_html_new();
		gtk_widget_ref(vc->html);
		gtk_object_set_data_full(GTK_OBJECT(vc->dialog),
					 "vc->html", vc->html,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vc->html);
		gtk_container_add(GTK_CONTAINER(scrolledwindow38),
				  vc->html);
		gtk_html_load_empty(GTK_HTML(vc->html));

		gtk_signal_connect(GTK_OBJECT(vc->html), "link_clicked",
				   GTK_SIGNAL_FUNC(gui_link_clicked),
				   vc);
		gtk_signal_connect(GTK_OBJECT(vc->html), "on_url",
				   GTK_SIGNAL_FUNC(dialog_url), vc);
	}


	gtk_signal_connect(GTK_OBJECT(vc->dialog), "destroy",
			   GTK_SIGNAL_FUNC(on_dialog_destroy), vc);
	gtk_signal_connect(GTK_OBJECT(vc->dialog),
			   "motion_notify_event",
			   GTK_SIGNAL_FUNC
			   (on_dialog_motion_notify_event), vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_sync), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_sync_clicked), vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_goto), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_goto_clicked), vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_prev), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_prev_clicked), vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_next), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_next_clicked), vc);
	gtk_signal_connect(GTK_OBJECT(vc->btn_close), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_close_clicked), vc);

	gtk_entry_set_text(GTK_ENTRY(vc->entry_key),
			   settings.currentverse);
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

void gui_open_commentary_dialog(gchar * mod_name)
{	
	VIEW_COMM *vc;

	vc = g_new(VIEW_COMM, 1);
	vc->ec = NULL;
	vc->dialog = NULL;
	vc->mod_name = mod_name;
	vc->book_heading = FALSE;
	vc->chapter_heading = FALSE;
	dialog_list = g_list_append(dialog_list, (VIEW_COMM *) vc);
	create_commentary_dialog(vc, FALSE);
	gtk_widget_show(vc->dialog);
}


/******************************************************************************
 * Name
 *   gui_open_commentary_editor
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_open_commentary_editor(gint mod_num)	
 *
 * Description
 *   create a commentary editor by calling create_commentary_dialog(vc,true)
 *   or bring an already existing editor to the top
 *
 * Return value
 *   void
 */

void gui_open_commentary_editor(gchar * mod_name)
{	
	VIEW_COMM *vc;	
	
	GList *tmp = g_list_first(dialog_list);
	while (tmp != NULL) {
		vc = (VIEW_COMM *) tmp->data;
		if(!strcmp(vc->mod_name, mod_name)) {
			gtk_entry_set_text(GTK_ENTRY(vc->entry_key),
			   settings.currentverse);
			display(vc, settings.currentverse);
			gdk_window_raise(GTK_WIDGET(vc->dialog)->
				 window);
			return;
		}		
		tmp = g_list_next(tmp);
	}
	
	vc = g_new(VIEW_COMM, 1);
	vc->ec = gs_html_editor_control_data_new();
	vc->dialog = NULL;
	vc->mod_name = mod_name;
	
	vc->ec->stylebar =
	    load_percomm_options(vc->mod_name, "Style bar");
	vc->ec->editbar =
	    load_percomm_options(vc->mod_name, "Edit bar");
	vc->ec->personal_comments = TRUE;
	strcpy(vc->ec->filename, vc->mod_name);
	vc->book_heading = FALSE;
	vc->chapter_heading = FALSE;
	dialog_list = g_list_append(dialog_list, (VIEW_COMM *) vc);
	create_commentary_dialog(vc, TRUE);
	gtk_widget_show(vc->dialog);
	
	if (vc->ec->stylebar)
		gtk_widget_show(vc->ec->toolbar_style);
	if (vc->ec->editbar)
		gtk_widget_show(vc->ec->toolbar_edit);
	gtk_widget_set_sensitive(vc->ec->toolbars, TRUE);
	gtk_html_set_editable(GTK_HTML(vc->ec->html), TRUE);
	settings.percomm_dialog_exist = TRUE;	
	
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

void gui_setup_commentary_dialog(GList * mods)
{
	dialog_list = NULL;
	dialog_freed = FALSE;
	settings.percomm_dialog_exist = FALSE;
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
		dialog_freed = TRUE;
//              g_warning("shuting down %s dialog",vc->mod_name);
		if (vc->ec)
			gui_html_editor_control_data_destroy(NULL,
							     vc->ec);
		/* 
		 *  destroy any dialogs created 
		 */
		if (vc->dialog)
			gtk_widget_destroy(vc->dialog);

		g_free((VIEW_COMM *) dialog_list->data);
		dialog_list = g_list_next(dialog_list);
	}
	g_list_free(dialog_list);
}
