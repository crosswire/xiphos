/*
 * GnomeSword Bible Study Tool
 * bibletext.c - support for Bible text modules
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

/* frontend */
#include "_bibletext.h"
#include "shortcutbar_main.h"
#include "cipher_key_dialog.h"
#include "module_options.h"

/* main */
#include "settings.h"
#include "bibletext.h"
#include "gs_gnomesword.h"
#include "gs_html.h"

/* backend */
#include "bibletext_.h"
#include "bibletext_view_.h"
#include "sword.h"
#include "shortcutbar.h"

/******************************************************************************
 * externs
 */

extern gboolean isrunningVT;
extern gboolean gsI_isrunning;
extern GList *options;

/******************************************************************************
 * globals to this file only 
 */
static GList *text_list;


TEXT_DATA *cur_t;
gboolean display_change = TRUE;

/******************************************************************************
 * Name
 *  text_get_description
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   const char* text_get_description(int mod_num)	
 *
 * Description
 *    
 *
 * Return value
 *   const char*
 */
 
const char* text_get_description(int mod_num)
{
	return backend_get_text_module_description(mod_num);
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
 
void text_page_changed(gint page_num, TEXT_DATA *t)
{
	/*
	 * remember new module name
	 */
	sprintf(settings.MainWindowModule, "%s", t->mod_name);
	/*
	 * point TEXT_DATA *cur_t to t - cur_t is global to this file
	 */
	cur_t = t;
	/*
	 * remember page number
	 */
	settings.text_last_page = page_num;
	/*
	 * display new module with current verse
	 */
	if (display_change) {
		set_text_page_and_key(page_num,settings.currentverse);
	}
	/*
	 * set global options for current module 
	 */
	set_options_on_page_change(t);		
}

/******************************************************************************
 * Name
 *  get_module_global_options
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  static void get_module_global_options(TEXT_DATA * t)	
 *
 * Description
 *    get global options for a module
 *
 * Return value
 *   void
 */

static void get_module_global_options(TEXT_DATA * t)
{
	t->gbfstrongs =
	    backend_check_for_global_option(t->mod_num, "GBFStrongs");
	t->thmlstrongs =
	    backend_check_for_global_option(t->mod_num, "ThMLStrongs");
	t->gbfmorphs =
	    backend_check_for_global_option(t->mod_num, "GBFMorph");
	t->thmlmorphs =
	    backend_check_for_global_option(t->mod_num, "ThMLMorph");
	t->gbffootnotes =
	    backend_check_for_global_option(t->mod_num, "GBFFootnotes");
	t->thmlfootnotes =
	    backend_check_for_global_option(t->mod_num,"ThMLFootnotes");
	t->greekaccents =
	    backend_check_for_global_option(t->mod_num,
					    "UTF8GreekAccents");
	t->lemmas =
	    backend_check_for_global_option(t->mod_num, "ThMLLemma");
	t->scripturerefs =
	    backend_check_for_global_option(t->mod_num, "ThMLScripref");
	t->hebrewpoints =
	    backend_check_for_global_option(t->mod_num,
					    "UTF8HebrewPoints");
	t->hebrewcant =
	    backend_check_for_global_option(t->mod_num,
					    "UTF8Cantillation");
	t->headings =
	    backend_check_for_global_option(t->mod_num, "ThMLHeadings");
	t->variants =
	    backend_check_for_global_option(t->mod_num, "ThMLVariants");
}

/******************************************************************************
 * Name
 *  set_text_module_global_options
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void set_text_module_global_options(gchar * option, gboolean choice)	
 *
 * Description
 *   set sword global option 
 *
 * Return value
 *   void
 */

void set_text_module_global_option(gchar * option, gboolean choice)
{
	gchar *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}
	
	backend_save_module_options(settings.MainWindowModule,
				    option, on_off);
	backend_set_text_global_option(option, on_off);
	display_text(settings.currentverse);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void set_text_variant_global_option(gchar * option, gchar * choice)	
 *
 * Description
 *   set sword global option (THMLVariant)
 *
 * Return value
 *   void
 */

void set_text_variant_global_option(gchar * option, gchar * choice)
{
	backend_set_text_global_option(option, choice);
	display_text(settings.currentverse);
}

/******************************************************************************
 * Name
 *  set_options_on_page_change
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void set_options_on_page_change(TEXT_DATA * t)	
 *
 * Description
 *   set module global options on page change
 *
 * Return value
 *   void
 */

void set_options_on_page_change(TEXT_DATA * t)
{
	if (t->gbfstrongs || t->thmlstrongs)
		set_text_module_global_option("Strong's Numbers",
				GTK_TOGGLE_BUTTON(t->t_btn_strongs)->
					       active);
	if (t->gbfmorphs || t->thmlmorphs)
		set_text_module_global_option("Morphological Tags",
				GTK_TOGGLE_BUTTON(t->t_btn_morphs)->
						active);
	if (t->gbffootnotes || t->thmlfootnotes)
		set_text_module_global_option("Footnotes",
				GTK_TOGGLE_BUTTON(t->t_btn_footnotes)->
					       active);
	if (t->greekaccents)
		set_text_module_global_option("Greek Accents",
				GTK_TOGGLE_BUTTON(t->t_btn_accents)->
					       active);
	if (t->lemmas)
		set_text_module_global_option("Lemmas",
				GTK_TOGGLE_BUTTON(t->t_btn_lemmas)->
					       active);
	if (t->scripturerefs)
		set_text_module_global_option("Scripture Cross-references",
				GTK_TOGGLE_BUTTON(t->t_btn_scripturerefs)->
						active);
	if (t->hebrewpoints)
		set_text_module_global_option("Hebrew Vowel Points",
				GTK_TOGGLE_BUTTON(t->t_btn_points)->
					       active);
	if (t->hebrewcant)
		set_text_module_global_option("Hebrew Cantillation",
				GTK_TOGGLE_BUTTON(t->t_btn_cant)->
					       active);
	if (t->headings)
		set_text_module_global_option("Headings",
				GTK_TOGGLE_BUTTON(t->t_btn_headings)->
					       active);
	if (t->variants) {
		if(GTK_RADIO_MENU_ITEM(t->t_btn_primary)->check_menu_item.active)
			backend_set_text_global_option("Textual Variants", "Primary Reading");
		else if(GTK_RADIO_MENU_ITEM(t->t_btn_secondary)->check_menu_item.active)
			backend_set_text_global_option("Textual Variants", "Secondary Reading");
		else if(GTK_RADIO_MENU_ITEM(t->t_btn_all)->check_menu_item.active)
			backend_set_text_global_option("Textual Variants", "All Readings");
		display_text(settings.currentverse);
	}
}

/******************************************************************************
 * Name
 *  set_text_page_and_key
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void set_text_page_and_key(gint page_num, gchar * key)	
 *
 * Description
 *   change text module notebook page and display new key (reference)
 *
 * Return value
 *   void
 */

void set_text_page_and_key(gint page_num, gchar * key) 
{
	/*
	 * we don't want backend_dispaly_text to be
	 * called by on_notebook_text_switch_page
	 */
	display_change = FALSE;
	if (settings.text_last_page != page_num) {
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (settings.notebook_text),
				      page_num);
	}

	if (cur_t->is_locked) {
		GtkWidget *dlg;	
		dlg = gui_create_cipher_key_dialog(cur_t->mod_name);
		gtk_widget_show(dlg);

	} else
		backend_display_text(page_num, key);
	display_change = TRUE;
}

/******************************************************************************
 * Name
 *  set_page_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void set_page_text(gchar * modname, GList * text_list,SETTINGS * s)	
 *
 * Description
 *   change text module by finding page number from module name
 *
 * Return value
 *   void
 */

static void set_page_text(gchar * modname, GList * text_list,
			  SETTINGS * s) 
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
	cur_t = t;
	gtk_notebook_set_page(GTK_NOTEBOOK(s->notebook_text), page);
	s->text_last_page = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebook_text),
				   s->text_tabs);
}

/******************************************************************************
 * Name
 *  display_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *   void display_text(gchar * key)
 *  	
 *
 * Description
 *   call backend_display_text and pass module page num and key
 *
 * Return value
 *   void
 */

void display_text(gchar * key) 
{
	if (!cur_t->is_locked)
		backend_display_text(settings.text_last_page, key);
}

/******************************************************************************
 * Name
 *  setup_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void setup_text(SETTINGS * s, GList *mods)
 *
 * Description
 *   set up gui for sword text modules - return list of text module names
 *
 * Return value
 *   void
 */

void setup_text(SETTINGS * s, GList *mods) 
{
	GtkWidget *popupmenu;
	GList *tmp = NULL;
	gchar *modname;
	gchar *modbuf;
	TEXT_DATA *t;
	gint count = 0;

	text_list = NULL;

	//mods = backend_get_list_of_mods_by_type(TEXT_MODS);
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		t = g_new(TEXT_DATA, 1);
		t->mod_name = modname;
		t->mod_num = count;
		t->search_string = NULL;
		t->key = NULL;
		t->find_dialog = NULL;
		t->is_locked = backend_module_is_locked(t->mod_name);
		get_module_global_options(t);
		gui_create_text_pane(s, t);
		popupmenu = gui_create_pm_text(t);
		gnome_popup_menu_attach(popupmenu, t->html, NULL);
		backend_new_text_display(t->html, t->mod_name, s);
		text_list = g_list_append(text_list, (TEXT_DATA *) t);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(s->notebook_text),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebook_text_switch_page), text_list);

	modbuf = g_strdup(s->MainWindowModule);

	set_page_text(modbuf, text_list, s);

	g_free(modbuf);
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *  shutdown_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void shutdown_text(void)	
 *
 * Description
 *   shut down text module support clean mem
 *
 * Return value
 *   void
 */

void shutdown_text(void) 
{
	text_list = g_list_first(text_list);
	while (text_list != NULL) {
		TEXT_DATA *t = (TEXT_DATA *) text_list->data;
		/* 
		 * free any search dialogs created 
		 */
		if (t->find_dialog)
			 g_free(t->find_dialog);
		/* 
		 * free each TEXT_DATA item created 
		 */
		 g_free((TEXT_DATA *) text_list->data);
		 text_list = g_list_next(text_list);
	} g_list_free(text_list);
}

/******************************************************************************
 * Name
 *  get_book_viewtext 
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *    const char *get_book_viewtext(void)	
 *
 * Description
 *   get current book displayed in veiw text dialog
 *
 * Return value
 *   const char *
 */

const char *get_book_viewtext(void)
{
	return backend_get_book_viewtext();
}

/******************************************************************************
 * Name
 *  get_chapter_viewtext 
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   int get_chapter_viewtext(void)	
 *
 * Description
 *   get current chapter displayed in veiw text dialog
 *
 * Return value
 *   int
 */

int get_chapter_viewtext(void)
{
	return backend_get_chapter_viewtext();
}

/******************************************************************************
 * Name
 *   get_verse_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   int get_verse_viewtext(void)	
 *
 * Description
 *   get current verse displayed in veiw text dialog
 *
 * Return value
 *   int
 */

int get_verse_viewtext(void)
{	
	return backend_get_verse_viewtext();
}

/******************************************************************************
 * Name
 *   goto_verse_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void goto_verse_viewtext(gchar * verse)	
 *
 * Description
 *   find and display new verse for view text dialog
 *
 * Return value
 *   void
 */

void goto_verse_viewtext(char * verse)
{
	backend_goto_verse_viewtext(verse);
}

/******************************************************************************
 * Name
 *   load_module_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void load_module_viewtext(gchar * modName)	
 *
 * Description
 *   load a text module into the view text dialog
 *
 * Return value
 *   void
 */

void load_module_viewtext(char * module_name)
{
	backend_load_module_viewtext(module_name);
}

/******************************************************************************
 * Name
 *   shutdown_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void shutdown_viewtext(void)	
 *
 * Description
 *   shutdown down sword viewtext support
 *
 * Return value
 *   void
 */

void shutdown_viewtext(void)
{
	backend_shutdown_viewtext();
}

/******************************************************************************
 * Name
 *   set_global_options_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void set_global_options_viewtext(gchar *option, gboolean choice)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void set_global_options_viewtext(gchar *option, gboolean choice)
{
	if (choice) {	
		backend_set_global_options_viewtext(option,
						    "On");
	} else {		
		backend_set_global_options_viewtext(option,
						    "Off");
	}
}

/******************************************************************************
 * Name
 *   setup_viewtext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void setup_viewtext(GtkWidget * text)	
 *
 * Description
 *   setup the viewtext sword display
 *
 * Return value
 *   void
 */

void setup_viewtext(GtkWidget * text)
{
	backend_setup_viewtext(text);
}
