/*
 * GnomeSword Bible Study Tool
 * bibletext.c - gui for Bible text modules
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
#include <config.h>
#endif

#include <gnome.h>

#ifdef USE_GTKEMBEDMOZ
#include <gtkmozembed.h>
#endif


#include "gui/gnomesword.h"
#include "gui/gtkhtml_display.h"
#include "gui/bibletext.h"
#include "gui/bibletext_menu.h"
#include "gui/bibletext_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/cipher_key_dialog.h"
#include "gui/html.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/dialog.h"
#include "gui/font_dialog.h"
#include "gui/dictlex.h"

#include "main/bibletext.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"

static void on_notebook_text_switch_page(GtkNotebook * notebook,
					 GtkNotebookPage * page,
					 gint page_num, GList * tl);
/******************************************************************************
 * externs
 */

extern gboolean gsI_isrunning;


TEXT_DATA *cur_t;

/******************************************************************************
 * globals to this file only 
 */
static GList *text_list;
static gboolean display_change = TRUE;



/******************************************************************************
 * Name
 *  gui_lookup_bibletext_selection
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 * void gui_lookup_bibletext_selection(GtkMenuItem * menuitem,
					 gchar * dict_mod_description)
 *
 * Description
 *   lookup seledtion in a dict/lex module
 *
 * Return value
 *   void
 */

void gui_lookup_bibletext_selection(GtkMenuItem * menuitem,
				    gchar * dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;

	mod_name = module_name_from_description(dict_mod_description);

	dict_key = gui_get_word_or_selection(cur_t->html, FALSE);
	if (dict_key && mod_name) {
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(mod_name,
						      dict_key);
		if (settings.inDictpane)
			gui_change_module_and_key(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
	}
}

/******************************************************************************
 * Name
 *  set_page_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void set_page_text(gchar * modname, GList * text_list)	
 *
 * Description
 *   change text module by finding page number from module name
 *
 * Return value
 *   void
 */

static void set_page_text(gchar * modname, GList * text_list)
{
	gint page = 0;
	TEXT_DATA *t = NULL;

	text_list = g_list_first(text_list);
	while (text_list != NULL) {
		t = (TEXT_DATA *) text_list->data;
		if (!strcmp(t->mod_name, modname))
			break;
		++page;
		text_list = g_list_next(text_list);
	}
	if (page)
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_text), page);
	else
		on_notebook_text_switch_page(GTK_NOTEBOOK
					     (widgets.notebook_text),
					     NULL, page, text_list);
	settings.text_last_page = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_text),
				   settings.text_tabs);
}

/******************************************************************************
 * Name
 *  text_page_changed
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void text_page_changed(gint page_num, TEXT_DATA *t)	
 *
 * Description
 *    take care of non gui stuff on notebook page change
 *
 * Return value
 *   void
 */

static void text_page_changed(gint page_num, TEXT_DATA * t)
{
	/*
	 * remember new module name
	 */
	settings.MainWindowModule = t->mod_name;
	xml_set_value("GnomeSword", "modules", "bible", t->mod_name);

	/*
	 * remember page number
	 */
	settings.text_last_page = page_num;
	/*
	 * display new module with current verse
	 */
	if (display_change) {
		gui_set_text_page_and_key(page_num,
					  settings.currentverse);
	}
}


/******************************************************************************
 * Name
 *  on_notebook_text_switch_page
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_notebook_text_switch_page(GtkNotebook * notebook,
 *				  GtkNotebookPage * page,
 *				  gint page_num, GList * tl)	
 *
 * Description
 *   sets gui to new sword module
 *
 * Return value
 *   void
 */

static void on_notebook_text_switch_page(GtkNotebook * notebook,
					 GtkNotebookPage * page,
					 gint page_num, GList * tl)
{
	TEXT_DATA *t;
	/*
	 * get data structure for new module 
	 */
	t = (TEXT_DATA *) g_list_nth_data(tl, page_num);
	/*
	 * create pane if it does not already exist
	 */
	if (!t->frame) {
		gui_add_new_text_pane(t);
	}
	/*
	 * point TEXT_DATA *cur_t to t - cur_t is global to this file
	 */
	cur_t = t;
	set_module(0, t->mod_name);
	/*
	 * do work that's non gui
	 */
	text_page_changed(page_num, t);
	/*
	 * set program title current text module name 
	 */
	gui_change_window_title(t->mod_name);
	/*
	 * set search label to current text module 
	 */
//	gui_set_search_label();
	/*
	 *  keep showtabs menu item current 
	 */
	GTK_CHECK_MENU_ITEM(t->showtabs)->active = settings.text_tabs;

	gui_set_text_frame_label(t);
	widgets.html_text = t->html;
/*
	if(!t->is_rtol)
		gtk_notebook_popup_enable(GTK_NOTEBOOK(widgets.notebook_text));
	else
		gtk_notebook_popup_disable(GTK_NOTEBOOK(widgets.notebook_text));
*/
}

/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  gboolean on_button_release_event(GtkWidget * widget,
			    GdkEventButton * event, TEXT_DATA * t)	
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

static gboolean on_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					TEXT_DATA * t)
{
	extern gboolean in_url;
	gchar *key;

	settings.whichwindow = MAIN_TEXT_WINDOW;
	/*
	 * set program title to current text module name 
	 */
	gui_change_window_title(t->mod_name);

	switch (event->button) {
	case 1:
		if (!in_url) {
			key = gui_button_press_lookup(t->html);
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
		break;
	case 2:
		break;
	case 3:
		break;
	}
	return FALSE;
}

#ifdef USE_GTKEMBEDMOZ
int mozilla_mouse_click(GtkMozEmbed * embed, gpointer dom_event)
{
/*
	GtkWidget *popupmenu;
	
	popupmenu = gui_create_pm_text(cur_t);
	gnome_popup_menu_do_popup(popupmenu,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL);
	g_warning("mozilla_mouse_click");
*/
	return 0;
}

/******************************************************************************
 * Name
 *  set_module_font_activate
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   void set_module_font_activate(GtkMenuItem * menuitem,
				     TEXT_DATA * t)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void set_module_font(GtkButton * button, TEXT_DATA * t)
{
	gui_set_module_font(t->mod_name);

	chapter_display_mozilla(t->html, t->mod_name,
				t->tgs, settings.currentverse, TRUE);
}

static void on_togglebutton_points_toggled(GtkToggleButton *
					   togglebutton, TEXT_DATA * t)
{
	t->tgs->hebrewpoints = togglebutton->active;
	save_module_options(t->mod_name, "Hebrew Vowel Points",
			    t->tgs->hebrewpoints);
	chapter_display_mozilla(t->html, t->mod_name,
				t->tgs, settings.currentverse, TRUE);
}


static void on_togglebutton_cant_toggled(GtkToggleButton * togglebutton,
					 TEXT_DATA * t)
{
	t->tgs->hebrewcant = togglebutton->active;
	save_module_options(t->mod_name, "Hebrew Cantillation",
			    t->tgs->hebrewcant);
	chapter_display_mozilla(t->html, t->mod_name,
				t->tgs, settings.currentverse, TRUE);
}


#endif

/******************************************************************************
 * Name
 *  create_pane
 *
 * Synopsis
 *   #include "bibletext.h"
 *   void create_pane(TEXT_DATA * t)
 *  	
 *
 * Description
 *   create a text pane(window) for each text module
 *
 * Return value
 *   void
 */

static void create_pane(TEXT_DATA * t)
{
	GtkWidget *vbox;
	GtkWidget *frame_text;
	GtkWidget *scrolledwindow;
#ifdef USE_GTKEMBEDMOZ
	GtkWidget *toolbar_moz;
	GtkWidget *button_set_font;
	GtkWidget *togglebutton_points;
	GtkWidget *togglebutton_cant;
#endif

	t->frame = gtk_frame_new(NULL);
	gtk_widget_show(t->frame);
	gtk_container_add(GTK_CONTAINER(t->vbox), t->frame);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(t->frame), vbox);


#ifdef USE_GTKEMBEDMOZ
	if (!t->is_rtol) {
		frame_text = gtk_frame_new(NULL);
		gtk_widget_show(frame_text);
		gtk_box_pack_start(GTK_BOX(vbox), frame_text, TRUE,
				   TRUE, 0);

		scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_show(scrolledwindow);
		gtk_container_add(GTK_CONTAINER(frame_text),
				  scrolledwindow);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
					       (scrolledwindow),
					       GTK_POLICY_AUTOMATIC,
					       GTK_POLICY_AUTOMATIC);
		t->html = gtk_html_new();
		gtk_widget_show(t->html);
		gtk_container_add(GTK_CONTAINER(scrolledwindow),
				  t->html);
		gtk_html_load_empty(GTK_HTML(t->html));
		gtk_signal_connect(GTK_OBJECT(t->html), "link_clicked",
				   G_CALLBACK(gui_link_clicked),
				   (TEXT_DATA *) t);
		gtk_signal_connect(GTK_OBJECT(t->html), "on_url",
				   G_CALLBACK(gui_url),
				   (TEXT_DATA *) t);
		gtk_signal_connect(GTK_OBJECT(t->html),
				   "button_release_event",
				   G_CALLBACK
				   (on_button_release_event),
				   (TEXT_DATA *) t);
	}

	else {

		toolbar_moz =
		    gtk_toolbar_new();
		gtk_toolbar_set_style (GTK_TOOLBAR (toolbar_moz), GTK_TOOLBAR_ICONS);
		gtk_widget_show(toolbar_moz);
		gtk_box_pack_start(GTK_BOX(vbox), toolbar_moz, FALSE,
				   FALSE, 0);
/*		gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar_moz),
					      GTK_RELIEF_NONE);*/

		button_set_font =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar_moz),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("Font"), NULL,
					       NULL, NULL, NULL, NULL);
		gtk_widget_show(button_set_font);

		gtk_signal_connect(GTK_OBJECT(button_set_font),
				   "clicked",
				   G_CALLBACK(set_module_font),
				   (TEXT_DATA *) t);


		if (check_for_global_option
		    (t->mod_name, "UTF8HebrewPoints")) {
			togglebutton_points =
			    gtk_toolbar_append_element(GTK_TOOLBAR
				       (toolbar_moz),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL,
				       _
				       ("Vowel Points"),
				       NULL, NULL, NULL,
				       NULL, NULL);
			gtk_widget_show(togglebutton_points);

			gtk_signal_connect(GTK_OBJECT
				   (togglebutton_points),
				   "toggled",
				   G_CALLBACK
				   (on_togglebutton_points_toggled),
				   (TEXT_DATA *) t);
		}

		if (check_for_global_option
		    (t->mod_name, "UTF8Cantillation")) {
			togglebutton_cant =
			    gtk_toolbar_append_element(GTK_TOOLBAR
				       (toolbar_moz),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL,
				       _("Cantilation"),
				       NULL, NULL, NULL,
				       NULL, NULL);
			gtk_widget_show(togglebutton_cant);

			gtk_signal_connect(GTK_OBJECT
				   (togglebutton_cant),
				   "toggled",
				   G_CALLBACK
				   (on_togglebutton_cant_toggled),
				   (TEXT_DATA *) t);
		}

		frame_text = gtk_frame_new(NULL);
		gtk_widget_show(frame_text);
		gtk_box_pack_start(GTK_BOX(vbox), frame_text, TRUE,
				   TRUE, 0);

		gtk_moz_embed_set_comp_path("usr/lib/mozilla-1.3.1");
		t->html = gtk_moz_embed_new();
		gtk_widget_show(t->html);
		gtk_container_add(GTK_CONTAINER(frame_text), t->html);
		gtk_widget_realize(t->html);


		gtk_signal_connect(GTK_OBJECT(t->html),
				   "dom_mouse_click",
				   G_CALLBACK(mozilla_mouse_click),
				   NULL);
	}

#else
	frame_text = gtk_frame_new(NULL);
	gtk_widget_show(frame_text);
	gtk_box_pack_start(GTK_BOX(vbox), frame_text, TRUE, TRUE, 0);

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_container_add(GTK_CONTAINER(frame_text), scrolledwindow);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	t->html = gtk_html_new();
	gtk_widget_show(t->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), t->html);
	gtk_html_load_empty(GTK_HTML(t->html));

	gtk_signal_connect(GTK_OBJECT(t->html), "link_clicked",
			   G_CALLBACK(gui_link_clicked),
			   (TEXT_DATA *) t);
	gtk_signal_connect(GTK_OBJECT(t->html), "on_url",
			   G_CALLBACK(gui_url), (TEXT_DATA *) t);
	gtk_signal_connect(GTK_OBJECT(t->html), "button_release_event",
			   G_CALLBACK(on_button_release_event),
			   (TEXT_DATA *) t);
#endif
}

/******************************************************************************
 * Name
 *  gui_set_text_frame_label
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void gui_set_text_frame_label(TEXT_DATA * t)	
 *
 * Description
 *   sets text frame label to module name or null
 *
 * Return value
 *   void
 */

void gui_set_text_frame_label(TEXT_DATA * t)
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module name
	 */
	if (settings.text_tabs)
		gtk_frame_set_label(GTK_FRAME(t->frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(t->frame), t->mod_name);
}

/******************************************************************************
 * Name
 *  gui_set_text_page_and_key
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void gui_set_text_page_and_key(gint page_num, gchar * key)	
 *
 * Description
 *   change text module notebook page and display new key (reference)
 *
 * Return value
 *   void
 */

void gui_set_text_page_and_key(gint page_num, gchar * key)
{
	/*
	 * we don't want backend_dispaly_text to be
	 * called by on_notebook_text_switch_page
	 */
	display_change = FALSE;
	if (settings.text_last_page != page_num) {
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_text),
				      page_num);
	}

	if (!cur_t->is_locked) {
#ifdef USE_GTKEMBEDMOZ
		if (!cur_t->is_rtol)
			chapter_display(cur_t->html,
					cur_t->mod_name,
					cur_t->tgs, key, TRUE);
		else
			chapter_display_mozilla(cur_t->html,
						cur_t->mod_name,
						cur_t->tgs, key, TRUE);

#else
		chapter_display(cur_t->html, cur_t->mod_name,
				cur_t->tgs, key, TRUE);
#endif
	}
	display_change = TRUE;
	cur_t->key = settings.currentverse;
}

/******************************************************************************
 * Name
 *  gui_display_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   void gui_display_text(gchar * key)
 *  	
 *
 * Description
 *   call chapter_display and pass html wigdet, module name and key
 *
 * Return value
 *   void
 */

void gui_display_text(gchar * key)
{
	if (!cur_t->is_locked)
#ifdef USE_GTKEMBEDMOZ
		if (!cur_t->is_rtol)
			chapter_display(cur_t->html,
					cur_t->mod_name,
					cur_t->tgs, key, TRUE);
		else
			chapter_display_mozilla(cur_t->html,
						cur_t->mod_name,
						cur_t->tgs, key, TRUE);

#else
		chapter_display(cur_t->html, cur_t->mod_name,
				cur_t->tgs, key, TRUE);
#endif
	else if (cur_t->cipher_key) {
		gui_module_is_locked_display(cur_t->html,
					     cur_t->mod_name,
					     cur_t->cipher_key);

	}
	cur_t->key = settings.currentverse;
}

/******************************************************************************
 * Name
 *  gui_add_new_pane
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void gui_add_new_pane(TEXT_DATA * t)
 *
 * Description
 *   creates a text pane when user selects a new text module
 *
 * Return value
 *   void
 */

void gui_add_new_text_pane(TEXT_DATA * t)
{
	GtkWidget *popupmenu;

	create_pane(t);
	if (t->is_locked)
		gui_module_is_locked_display(t->html, t->mod_name,
							t->cipher_key);

#ifdef USE_GTKEMBEDMOZ					    
	if (!t->is_rtol) {
		popupmenu = gui_create_pm_text(t);
		gnome_popup_menu_attach(popupmenu, t->html, NULL);
	}
#else
	popupmenu = gui_create_pm_text(t);
	gnome_popup_menu_attach(popupmenu, t->html, NULL);
#endif
}

/******************************************************************************
 * Name
 *  add_vbox_to_notebook
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void add_vbox_to_notebook(TEXT_DATA * t)
 *
 * Description
 *   adds a vbox and label to the text notebook for each text module
 *
 * Return value
 *   void
 */

static void add_vbox_to_notebook(TEXT_DATA * t)
{
	GtkWidget *label;

	t->vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(t->vbox);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_text),
			  t->vbox);


	label = gtk_label_new(t->mod_name);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_text),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(widgets.notebook_text),
				    t->mod_num), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.notebook_text),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.notebook_text),
					  t->mod_num),
					 (gchar *) t->mod_name);
}

/******************************************************************************
 * Name
 *  gui_setup_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void gui_setup_text(GList *mods)
 *
 * Description
 *   set up gui for sword text modules - return list of text module names
 *
 * Return value
 *   void
 */

void gui_setup_text(GList * mods)
{
	GList *tmp = NULL;
	gchar *modbuf;
	TEXT_DATA *t;
	gint count = 0;

	text_list = NULL;
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		t = g_new(TEXT_DATA, 1);
		t->tgs = g_new(TEXT_GLOBALS, 1);
		t->mod_name = (gchar *) tmp->data;
//		printf("mod_name = %s\n",t->mod_name);
		t->mod_num = count;
		t->search_string = NULL;
		t->key = NULL;
		t->cipher_key = NULL;
		t->is_dialog = FALSE;
		t->is_rtol = is_module_rtl(t->mod_name);
		if (has_cipher_tag(t->mod_name)) {
			t->is_locked = module_is_locked(t->mod_name);
			t->cipher_old = get_cipher_key(t->mod_name);
		}

		else {

			t->is_locked = 0;
			t->cipher_old = NULL;
		}
		t->frame = NULL;
		add_vbox_to_notebook(t);
		text_list = g_list_append(text_list, (TEXT_DATA *) t);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(widgets.notebook_text),
			   "switch_page",
			   G_CALLBACK
			   (on_notebook_text_switch_page), text_list);

	modbuf = g_strdup(xml_get_value("modules", "bible"));

	set_page_text(modbuf, text_list);

	g_free(modbuf);
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *  gui_shutdown_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void gui_shutdown_text(void)	
 *
 * Description
 *   shut down text module support clean mem
 *
 * Return value
 *   void
 */

void gui_shutdown_text(void)
{
	text_list = g_list_first(text_list);
	while (text_list != NULL) {
		TEXT_DATA *t = (TEXT_DATA *) text_list->data;
		/* 
		 * free any cipher keys 
		 */
		if (t->cipher_key)
			g_free(t->cipher_key);
		if (t->cipher_old)
			g_free(t->cipher_old);
		/* 
		 * free global options 
		 */
		g_free(t->tgs);
		/* 
		 * free each TEXT_DATA item created 
		 */
		g_free((TEXT_DATA *) text_list->data);
		text_list = g_list_next(text_list);
	}
	g_list_free(text_list);
}
