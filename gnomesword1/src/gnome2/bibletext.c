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


#include "gui/gnomesword.h"
#include "gui/gtkhtml_display.h"
#include "gui/bibletext.h"
#include "gui/bibletext_menu.h"
#include "gui/bibletext_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/cipher_key_dialog.h"
#include "gui/html.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/dialog.h"
#include "gui/font_dialog.h"
#include "gui/dictlex.h"
#include "gui/widgets.h"

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

GtkTextBuffer *text_buffer;
/******************************************************************************
 * globals to this file only 
 */
static GList *text_list;
static gboolean display_change = TRUE;
static gint text_last_page;



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
	
	if(!cur_t->html) return;
	
	mod_name = module_name_from_description(dict_mod_description);
	dict_key = gui_get_word_or_selection(cur_t->html, FALSE);
	if (dict_key && mod_name) {
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(mod_name,
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
	if(modname) {
		while (text_list != NULL) {
			t = (TEXT_DATA *) text_list->data;
			if (!strcmp(t->mod_name, modname))
				break;
			++page;
			text_list = g_list_next(text_list);
		}
	}
	else {
		t = (TEXT_DATA *) text_list->data;
		page = 0;
	}
	
	if (page)
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_text), page);
	else
		on_notebook_text_switch_page(GTK_NOTEBOOK
					     (widgets.notebook_text),
					     NULL, page, text_list);
	text_last_page = page;
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
	text_last_page = page_num;
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
	
	if(page_num == settings.parallel_page)
		return;
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
	if(!t->is_rtol)
	GTK_CHECK_MENU_ITEM(t->showtabs)->active = settings.text_tabs;

	gui_set_text_frame_label(t);
	if(!t->is_rtol)
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
					gui_display_dictlex_in_sidebar
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

static gboolean textview_button_release_event(GtkWidget * widget,
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
		break;
	case 2:
		break;
	case 3:
		break;
	}
	return FALSE;
}



static gint tag_event_handler (GtkTextTag *tag, GtkWidget *widget,
	GdkEvent *event, const GtkTextIter *iter, gpointer user_data)
{
	gint char_index;
	
	char_index = gtk_text_iter_get_offset (iter);
	//printf ("offset = %d", char_index);
  switch (event->type)
    {
    case GDK_MOTION_NOTIFY:
      printf ("Motion event at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;
        
    case GDK_BUTTON_PRESS:
      printf ("Button press at char %d tag `%s'\n",
             char_index, tag->name);
	    switch(event->button.button){
		    case 1:
			//return do_something_with_tag(tag, iter, user_data);
		        break;
		    case 2:
		    case 3:
		        break;
	    
	}
	
    	return TRUE;	
      break;
        
    case GDK_2BUTTON_PRESS:
      printf ("Double click at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;
        
    case GDK_3BUTTON_PRESS:
      printf ("Triple click at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;
        
    case GDK_BUTTON_RELEASE:
      printf ("Button release at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;
        
    case GDK_KEY_PRESS:
    case GDK_KEY_RELEASE:
      printf ("Key event at char %d tag `%s'\n",
              char_index, tag->name);
    	return TRUE;
      break;
      
    case GDK_ENTER_NOTIFY:
      printf ("enter event at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;
	    
    case GDK_LEAVE_NOTIFY:
      printf ("leave event at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;
	    
    case GDK_PROPERTY_NOTIFY:
    case GDK_SELECTION_CLEAR:
    case GDK_SELECTION_REQUEST:
    case GDK_SELECTION_NOTIFY:
    case GDK_PROXIMITY_IN:
    case GDK_PROXIMITY_OUT:
    case GDK_DRAG_ENTER:
    case GDK_DRAG_LEAVE:
    case GDK_DRAG_MOTION:
    case GDK_DRAG_STATUS:
    case GDK_DROP_START:
    case GDK_DROP_FINISHED:
    	return FALSE;
    default:
    	return FALSE;
      break;
    }    
    return FALSE;
}

static void setup_tag (GtkTextTag *tag, gpointer user_data)
{
	g_signal_connect (G_OBJECT (tag),
		    "event",
		    G_CALLBACK (tag_event_handler),
		    user_data);
}

static void create_text_tags(GtkTextBuffer * buffer, gchar * rtl_font)
{
	GtkTextTag *tag;
	GdkColor color;
	GdkColor color2;
	GdkColor color_red;
	GdkColor colorLink;
	PangoFontDescription *font_desc;
	
		
		
	/* verse number tag verse style*/
	tag = gtk_text_buffer_create_tag (buffer, "verseNumber", NULL);
	setup_tag (tag, buffer);  
	color.red = color.green = 0;
	color.blue = 0xffff;
		"scale", PANGO_SCALE_XX_SMALL,
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);	
		
	/* current verse color tag */	
	tag = gtk_text_buffer_create_tag (buffer, "fg_currentverse", NULL);
	color.blue = 0;
	color.green = 0xbbbb;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);
		
	/*  verse color tag */	
	tag = gtk_text_buffer_create_tag (buffer, "fg_verse", NULL);
	color.blue = 0;
	color.green = 0;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);
		
	/* right to left tag */
	tag = gtk_text_buffer_create_tag (buffer, "rtl_text", NULL);
        g_object_set (G_OBJECT (tag),
		//"font", rtl_font,
                "wrap_mode", GTK_WRAP_WORD,
                "direction", GTK_TEXT_DIR_RTL,
                "indent", 0,
                "left_margin", 0,
                "right_margin", 0,
                NULL);	
		
	/* right to left font */
	tag = gtk_text_buffer_create_tag (buffer, "rtl_font", NULL);
        g_object_set (G_OBJECT (tag),
		"font", rtl_font,
                NULL);	
		
	/* large tag */
	tag = gtk_text_buffer_create_tag (buffer, "large", NULL);
        g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_XX_LARGE,
                NULL);			
}
	

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
	gchar *gdk_font = NULL;
	gchar file[250];

	
	
	t->frame = gtk_frame_new(NULL);
	gtk_widget_show(t->frame);
	gtk_box_pack_start(GTK_BOX(t->vbox), t->frame, TRUE,
			   TRUE, 0); 
	gtk_frame_set_shadow_type(GTK_FRAME(t->frame),GTK_SHADOW_NONE);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(t->frame), vbox);

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

	if (!t->is_rtol) {
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

	else {  /* use gtktextview for right to left text */
		sprintf(file, "%s/fonts.conf", settings.gSwordDir);
		t->text = gtk_text_view_new ();
		gtk_widget_show (t->text);
		gtk_container_add (GTK_CONTAINER (scrolledwindow), t->text);
		gtk_text_view_set_editable (GTK_TEXT_VIEW (t->text), FALSE);
		text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (t->text));
		gdk_font = get_conf_file_item(file, t->mod_name, "GdkFont");
		//g_warning("font = %s",gdk_font);
		create_text_tags(text_buffer,gdk_font);
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW (t->text), GTK_WRAP_WORD);
		
		gtk_signal_connect(GTK_OBJECT(t->text),
				   "button_release_event",
				   G_CALLBACK
				   (textview_button_release_event),
				   (TEXT_DATA *) t);
		if(gdk_font) g_free(gdk_font);
	}
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
	page_num;
	display_change = FALSE;
	if (text_last_page != page_num) {
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_text),
				      page_num);
	}
	
	if(settings.parallel_page == gtk_notebook_get_current_page(GTK_NOTEBOOK
				      (widgets.notebook_text))) {
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_text),
				      page_num);					      
	}
	
	if (!cur_t->is_locked) {
		if (!cur_t->is_rtol)
			chapter_display(cur_t->html,
					cur_t->mod_name,
					cur_t->tgs, key, TRUE);
		else
			chapter_display_textview(cur_t->text,
						cur_t->mod_name,
						cur_t->tgs, key, TRUE);
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
		if (!cur_t->is_rtol)
			chapter_display(cur_t->html,
					cur_t->mod_name,
					cur_t->tgs, key, TRUE);
		else {
			/* right to left text */
			chapter_display_textview(cur_t->text,
				cur_t->mod_name,
				cur_t->tgs, key, TRUE);
		}
			
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
		gui_module_is_locked_display(t->html, t->mod_name,t->cipher_key);
		
	popupmenu = gui_create_pm_text(t);
	if (!t->is_rtol) 
		gnome_popup_menu_attach(popupmenu, t->html, NULL);
	else
		gnome_popup_menu_attach(popupmenu, t->text, NULL);
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
	
	settings.parallel_page = g_list_length(mods);
	text_list = NULL;
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		t = g_new0(TEXT_DATA, 1);
		t->tgs = g_new0(TEXT_GLOBALS, 1);
		t->tgs->words_in_red = FALSE;
		t->tgs->strongs = FALSE;
		t->tgs->morphs = FALSE;
		t->tgs->footnotes = FALSE;
		t->tgs->greekaccents = FALSE;
		t->tgs->lemmas = FALSE;
		t->tgs->scripturerefs = FALSE;
		t->tgs->hebrewpoints = FALSE;
		t->tgs->hebrewcant = FALSE;
		t->tgs->headings = FALSE;
		t->tgs->variants_all = FALSE;
		t->tgs->variants_primary = FALSE;
		t->tgs->variants_secondary = FALSE;
		t->mod_name = (gchar *) tmp->data;
		t->mod_num = count;
		t->search_string = NULL;
		t->key = NULL;
		t->cipher_key = NULL;
		t->is_dialog = FALSE;
		t->html = NULL;
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

	cur_t = t;
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
