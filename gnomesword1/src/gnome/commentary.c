/*
 * GnomeSword Bible Study Tool
 * commentary.c - gui for commentary modules
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
#include "gui/commentary.h"
#include "gui/commentary_menu.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"

#include "main/commentary.h"
#include "main/percomm.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"


COMM_DATA *cur_c;

/******************************************************************************
 * global to this file only 
 */
static GList *comm_list;
static gboolean comm_display_change;


static void on_notebook_comm_switch_page(GtkNotebook * notebook,
					 GtkNotebookPage * page,
					 gint page_num, GList * cl);

/******************************************************************************
 * Name
 *  display
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void display(COMM_DATA *c, gchar * key)	
 *
 * Description
 *   calls entry_display to display a commentary entry 
 *
 * Return value
 *   void
 */

static void display(COMM_DATA * c, gchar * key)
{
	gchar *text_str = NULL;
	gchar *strkey;

	if (c->is_locked) {
		return;
	} else if (c->cipher_key) {
		gui_module_is_locked_display(c->html,
					     c->mod_name,
					     c->cipher_key);
		return;
	}


	strkey = get_valid_key(key);
	if (c->book_heading) {
		c->book_heading = FALSE;
		text_str = get_book_heading(c->mod_name, strkey);
	}

	else if (c->chapter_heading) {
		c->chapter_heading = FALSE;
		text_str = get_chapter_heading(c->mod_name, strkey);
	}

	else {
		text_str = get_commentary_text(c->mod_name, strkey);
	}
	entry_display(c->html, c->mod_name, text_str, key, TRUE);
	free(text_str);

}

/******************************************************************************
 * Name
 *  set_commentary_page
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void set_commentary_page(gchar * modname, GList * comm_list)	
 *
 * Description
 *    change commentary page without changing key
 *
 * Return value
 *   void
 */

static void set_commentary_page(gchar * modname, GList * comm_list)
{
	gint page = 0;
	COMM_DATA *c = NULL;

	comm_list = g_list_first(comm_list);
	while (comm_list != NULL) {
		c = (COMM_DATA *) comm_list->data;
		if (!strcmp(c->mod_name, modname))
			break;
		++page;
		comm_list = g_list_next(comm_list);
	}
	if (page)
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_comm), page);
	else
		on_notebook_comm_switch_page(GTK_NOTEBOOK
					     (widgets.notebook_comm),
					     NULL, page, comm_list);
	settings.comm_last_page = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_comm),
				   settings.comm_tabs);
}



/******************************************************************************
 * Name
 *  gui_set_comm_frame_label
 *
 * Synopsis
 *   #include "gui/commentary.h"
 *
 *   void gui_set_comm_frame_label(void)	
 *
 * Description
 *   sets frame label to module name or null
 *
 * Return value
 *   void
 */

void gui_set_comm_frame_label(void)
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module name
	 */
	if (settings.comm_tabs)
		gtk_frame_set_label(GTK_FRAME(cur_c->frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(cur_c->frame),
				    cur_c->mod_name);

}

/******************************************************************************
 * Name
 *  on_notebook_comm_switch_page
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_notebook_comm_switch_page(GtkNotebook * notebook,
 *				  GtkNotebookPage * page,
 *				  gint page_num, GList * cl)	
 *
 * Description
 *   sets commentary gui to new sword module
 *
 * Return value
 *   void
 */

void on_notebook_comm_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, GList * cl)
{
	COMM_DATA *c, *c_old;
	c_old = (COMM_DATA *) g_list_nth_data(cl,
					      settings.comm_last_page);
	c = (COMM_DATA *) g_list_nth_data(cl, page_num);
	cur_c = c;

	strcpy(settings.CommWindowModule, c->mod_name);

	if (!c->frame)
		gui_add_new_comm_pane(c);

	GTK_CHECK_MENU_ITEM(c->showtabs)->active = settings.comm_tabs;
	gui_set_comm_frame_label();

	gui_change_window_title(c->mod_name);
	/*
	 * set search module to current text module 
	 */
	strcpy(settings.sb_search_mod, c->mod_name);
	/*
	 * set search frame label to current commentary module 
	 */
	gui_set_search_label(c->mod_name);

	settings.comm_last_page = page_num;
	/*
	 * set settings.comm_key to current module key
	 */
	if (c->key)
		strcpy(settings.comm_key, c->key);

	if (comm_display_change) {
		if ((c->key[0] == '\0')
		    && (settings.currentverse != NULL)) {
			display(c, settings.currentverse);
			strcpy(settings.comm_key,
			       settings.currentverse);
			strcpy(c->key, settings.comm_key);
		}
	}
	widgets.html_comm = c->html;
}


/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   gboolean on_button_release_event(GtkWidget * widget,
 *			GdkEventButton * event, COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static gboolean on_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					COMM_DATA * c)
{
	extern gboolean in_url;
	gchar *key;

	settings.whichwindow = COMMENTARY_WINDOW;

	gui_change_window_title(c->mod_name);
	/*
	 * set search module to current commentary module 
	 */
	strcpy(settings.sb_search_mod, c->mod_name);
	/*
	 * set search frame label to current commentary module 
	 */
	gui_set_search_label(c->mod_name);

	switch (event->button) {
	case 1:
		if (!in_url) {
			key = gui_button_press_lookup(c->html);
			if (key) {
				gchar *dict = NULL;
				if (settings.useDefaultDict)
					dict =
					    g_strdup(settings.
						     DefaultDict);
				else
					dict =
					    g_strdup(settings.
						     DictWindowModule);
				if (settings.inViewer)
					gui_display_dictlex_in_viewer
					    (dict, key);
				if (settings.inDictpane)
					gui_change_module_and_key(dict,
								  key);
				g_free(key);
				if (dict)
					g_free(dict);
			}
		}
		return TRUE;
		break;
	case 2:
		return TRUE;
		break;
	case 3:
		return TRUE;
		break;
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *    gui_create_commentary_pane
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void gui_create_commentary_pane(COMM_DATA * c)
 *
 * Description
 *   create a commentary pane (window) for a sword commentary module
 *
 * Return value
 *   void
 */

static void create_commentary_pane(COMM_DATA * c)
{
	GtkWidget *vbox57;
	GtkWidget *frame_comm;
	GtkWidget *scrolledwindowCOMMhtml;


	c->frame = gtk_frame_new(NULL);
	gtk_widget_ref(c->frame);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "c->frame",
				 c->frame, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->frame);
	gtk_container_add(GTK_CONTAINER(c->vbox), c->frame);

	vbox57 = gtk_vbox_new(FALSE, 0);;
	gtk_widget_show(vbox57);
	gtk_container_add(GTK_CONTAINER(c->frame), vbox57);
/*
	c->frame_toolbar = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (c->frame_toolbar), GTK_SHADOW_NONE);
	gtk_box_pack_start(GTK_BOX(vbox57), 
				c->frame_toolbar, FALSE, TRUE, 0);
*/
	frame_comm = gtk_frame_new(NULL);
	gtk_widget_show(frame_comm);
	gtk_box_pack_start(GTK_BOX(vbox57), frame_comm, TRUE, TRUE, 0);



	scrolledwindowCOMMhtml = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindowCOMMhtml);
	gtk_container_add(GTK_CONTAINER(frame_comm),
			  scrolledwindowCOMMhtml);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowCOMMhtml),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);


	c->html = gtk_html_new();
	gtk_widget_show(c->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowCOMMhtml),
			  c->html);
	gtk_html_load_empty(GTK_HTML(c->html));


	gtk_signal_connect(GTK_OBJECT(c->html), "link_clicked",
			   GTK_SIGNAL_FUNC(gui_link_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->html), "on_url",
			   GTK_SIGNAL_FUNC(gui_url),
			   (gpointer) widgets.app);
	gtk_signal_connect(GTK_OBJECT(c->html), "button_release_event",
			   GTK_SIGNAL_FUNC(on_button_release_event),
			   (COMM_DATA *) c);
}

/******************************************************************************
 * Name
 *  gui_set_commentary_page_and_key
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_set_commentary_page_and_key(gint page_num, gchar *key)	
 *
 * Description
 *   change commentary page and key 
 *
 * Return value
 *   void
 */

void gui_set_commentary_page_and_key(gint page_num, gchar * key)
{
	comm_display_change = FALSE;
	strcpy(settings.comm_key, key);
	strcpy(cur_c->key, key);
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_comm),
			      page_num);
	display(cur_c, key);
	comm_display_change = TRUE;
}

/******************************************************************************
 * Name
 *   gui_display_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_display_commentary(gchar * key)	
 *
 * Description
 *    display new key in current commentary
 *
 * Return value
 *   void
 */

void gui_display_commentary(gchar * key)
{
	if (!cur_c)
		return;
	strcpy(settings.comm_key, key);
	strcpy(cur_c->key, key);
	display(cur_c, key);
}


/******************************************************************************
 * Name
 *   gui_display_commentary_with_struct
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_display_commentary_with_struct(gchar * key)	
 *
 * Description
 *    display new key in current commentary
 *
 * Return value
 *   void
 */

void gui_display_commentary_with_struct(COMM_DATA * c, gchar * key)
{
	strcpy(settings.comm_key, key);
	strcpy(cur_c->key, key);
	display(c, key);
}


/******************************************************************************
 * Name
 *  gui_add_new_comm_pane
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_add_new_comm_pane(COMM_DATA *c)
 *
 * Description
 *   creates a commentary pane when user selects a new commentary module
 *
 * Return value
 *   void
 */

void gui_add_new_comm_pane(COMM_DATA * c)
{
	GtkWidget *popupmenu;

	popupmenu = gui_create_pm_comm(c);
	create_commentary_pane(c);
	if (c->is_locked)
		gui_module_is_locked_display(c->html, c->mod_name,
					     c->cipher_key);
	gnome_popup_menu_attach(popupmenu, c->html, NULL);
	display(c, settings.currentverse);
}


/******************************************************************************
 * Name
 *  add_vbox_to_notebook
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void add_vbox_to_notebook(COMM_DATA *c)
 *
 * Description
 *   adds a vbox and label to the commentary notebook for each comm module
 *
 * Return value
 *   void
 */

static void add_vbox_to_notebook(COMM_DATA * c)
{
	GtkWidget *label;

	c->vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(c->vbox);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "c->vbox", c->vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(c->vbox);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_comm),
			  c->vbox);

	label = gtk_label_new(c->mod_name);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label",
				 label, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_comm),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(widgets.notebook_comm),
				    c->modnum), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.notebook_comm),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.notebook_comm),
					  c->modnum),
					 (gchar *) c->mod_name);
}


/******************************************************************************
 * Name
 *  gui_setup_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_setup_commentary(GList *mods)	
 *
 * Description
 *    setup gui commentary support and return list on commentary names
 *
 * Return value
 *   void
 */

void gui_setup_commentary(GList * mods)
{
	GList *tmp = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	COMM_DATA *c;
	gint count = 0;

	comm_list = NULL;

	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		c = g_new(COMM_DATA, 1);
		c->cgs = g_new(COMM_GLOBALS, 1);
		c->frame = NULL;
		c->mod_name = modname;
		c->modnum = count;
		c->search_string = NULL;
		c->is_percomm = is_personal_comment(c->mod_name);
		c->is_dialog = FALSE;
		c->key[0] = '\0';
		c->book_heading = FALSE;
		c->chapter_heading = FALSE;
		c->cipher_key = NULL;

		if (has_cipher_tag(c->mod_name)) {
			c->is_locked = module_is_locked(c->mod_name);
			c->cipher_old = get_cipher_key(c->mod_name);
			//g_warning("cipher key for %s is %s", c->mod_name,c->cipher_old);
		}

		else {

			c->is_locked = 0;
			c->cipher_old = NULL;
		}

		add_vbox_to_notebook(c);
		comm_list = g_list_append(comm_list, (COMM_DATA *) c);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(widgets.notebook_comm),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebook_comm_switch_page), comm_list);

	modbuf = g_strdup(settings.CommWindowModule);
	keybuf = g_strdup(settings.currentverse);

	set_commentary_page(modbuf, comm_list);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *   shutdown_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void shutdown_commentary(void)	
 *
 * Description
 *   shut down and cleanup 
 *
 * Return value
 *   void
 */

void gui_shutdown_commentary(void)
{
	comm_list = g_list_first(comm_list);
	while (comm_list != NULL) {
		COMM_DATA *c = (COMM_DATA *) comm_list->data;
		/* 
		 * free any cipher keys 
		 */
		if (c->cipher_key)
			g_free(c->cipher_key);
		if (c->cipher_old)
			g_free(c->cipher_old);
		/* 
		 * free global options 
		 */
		g_free(c->cgs);
		g_free((COMM_DATA *) comm_list->data);
		comm_list = g_list_next(comm_list);
	}
	g_list_free(comm_list);
}


//******  end of file  ******/
