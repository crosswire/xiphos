/*
 * GnomeSword Bible Study Tool
 * url.cc - support functions
 *
 * Copyright (C) 2004 GnomeSword Developer Team
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>

#include "gui/about_modules.h"
#include "gui/html.h"
#include "gui/history.h"
#include "gui/gtkhtml_display.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"
#include "gui/hints.h"
#include "gui/utilities.h"
#include "gui/sidebar.h"
#include "gui/dialog.h"
#include "gui/parallel_dialog.h"
#include "gui/parallel_view.h"
#include "gui/main_window.h"
#include "gui/toolbar_nav.h"

#include "main/url.h"
#include "main/lists.h"
#include "main/settings.h"
#include "main/configs.h"
#include "main/module.h"
#include "main/sword.h"
#include "main/xml.h"

#include "backend/sword_main.hh"



extern HISTORY history_list[];	/* sturcture for storing history items */
extern gint history_items;

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   
 *
 * Description
 *  
 *
 * Return value
 *   
 */
 
 
/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   
 *
 * Description
 *  
 *
 * Return value
 *   
 */
 
static void alert_url_not_found(const gchar * url)
{
	GS_DIALOG *dialog;
	GString *dialog_text = g_string_new(NULL);

	dialog = gui_new_dialog();
	dialog->stock_icon = GTK_STOCK_DIALOG_INFO;
	g_string_printf(dialog_text,
				"<span weight=\"bold\">%s</span>\n\n%s",
				_("URL not found:"),
				url);

	dialog->label_top = dialog_text->str;
	dialog->ok = TRUE;

	gui_alert_dialog(dialog);
	g_free(dialog);
	g_string_free(dialog_text, TRUE);
}
 
/******************************************************************************
 * Name
 *   show_in_appbar
 *
 * Synopsis
 *   #include "gui/url.h"
 *
 *   void show_in_appbar(GtkWidget * appbar, gchar * key, 
 *							gchar * mod)
 *
 * Description
 *   display information (morph or strongs) in appbar
 *
 * Return value
 *   void
 */

static void show_in_appbar(GtkWidget * appbar, gchar * key, gchar * mod)
{
	gchar *str;
	gchar *text;
	text = get_striptext(4, mod, key);
	str = remove_linefeeds(text);
	if (str) {
		gnome_appbar_set_status(GNOME_APPBAR(appbar), str);
		g_free(str);
	}
	g_free(text);
}


/******************************************************************************
 * Name
 *   about_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint about_uri(const gchar * url, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint about_uri(const gchar * url, gboolean clicked)
{
	gchar **work_buf = g_strsplit (url,"/",4);
	if(clicked) {
		gui_display_about_module_dialog(work_buf[2], FALSE);		
	} else {
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
					work_buf[3]);
	}
	g_strfreev(work_buf);
	return 1;
}

 /******************************************************************************
 * Name
 *   parallel_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint parallel_uri(const gchar * url, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint parallel_uri(const gchar * url, gboolean clicked)
{
	gchar **work_buf = g_strsplit (url,"/",4);
	if(!strcmp(work_buf[2],"swap")) {
		if(clicked) {
			gui_swap_parallel_with_main((gchar *) work_buf[3]);		
		} else {
			gchar *buf = g_strdup_printf( 
				_("Show %s in main window"), work_buf[3]);
			gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
						buf);
			g_free(buf);
		}
	}
	if(!strcmp(work_buf[2],"verse")) {
		if(clicked) {
		g_warning(work_buf[3]);
			settings.cvparallel = 
				gui_update_controls_parallel(work_buf[3]);
			gui_update_parallel_page_detached();
		} 
	}
	g_strfreev(work_buf);
	return 1;
}

/******************************************************************************
 * Name
 *   morph_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint morph_uri(const gchar * url, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint morph_uri(const gchar * url, gboolean clicked)
{	
	gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	gchar **work_buf = g_strsplit (url,"/",4);
	
	if(!strcmp(work_buf[2],"Greek") || strstr(work_buf[2],"x-Robinson")) {
		if(backend->is_module("Robinson")) 
			modbuf = "Robinson";
	} 		
	if (clicked) {
		if (settings.inDictpane)
			main_display_dictionary(modbuf, work_buf[3]);
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(modbuf, work_buf[3]);		
	} else {
		mybuf =
		    get_module_text(get_mod_type(modbuf), modbuf, work_buf[3]);
		if (mybuf) {
			show_in_appbar(widgets.appbar, work_buf[3], modbuf);
			//gui_display_in_hint_window(mybuf);
			g_free(mybuf);
		}
	}
	g_strfreev(work_buf);
	return 1;
	
}
 

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   
 *
 * Description
 *  
 *
 * Return value
 *   
 */
 
 
static gint strongs_uri(const gchar * url, gboolean clicked)
{	
	gchar *modbuf_viewer = NULL;
	gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	guint delay;	
	guint i;	
	gchar **work_buf = g_strsplit (url,"/",4);
	
	if(!strcmp(work_buf[2],"Greek")) {
		if(backend->is_module(settings.lex_greek_viewer)) 
			modbuf_viewer = settings.lex_greek_viewer;
		if(backend->is_module(settings.lex_greek)) 
			modbuf = settings.lex_greek;
	} else if(!strcmp(work_buf[2],"Hebrew")) {
		if(backend->is_module(settings.lex_hebrew_viewer)) 
			modbuf_viewer = settings.lex_hebrew_viewer;
		if(backend->is_module(settings.lex_hebrew)) 
			modbuf = settings.lex_hebrew;
	}
		
	if (clicked) {
		if (settings.inDictpane)
			main_display_dictionary(modbuf, work_buf[3]);
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(modbuf_viewer, work_buf[3]);		
	} else {
		mybuf =
		    get_module_text(get_mod_type(modbuf), modbuf, work_buf[3]);
		if (mybuf) {
			entry_display(sidebar.html_viewer_widget,  
					modbuf, 
					mybuf, 
					work_buf[3], 
					TRUE);
			//show_in_appbar(widgets.appbar, work_buf[3], modbuf);
			//gui_display_in_hint_window(mybuf);
			g_free(mybuf);
		}
	}
	g_strfreev(work_buf);
	return 1;
	
}
 
 
/******************************************************************************
 * Name
 *   note_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint note_uri(const gchar * url)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
static gint note_uri(const gchar * url, gboolean clicked)
{	
	gchar **work_buf = NULL;
	gchar *module = NULL;
	gchar *tmpbuf = NULL;
	
	if(!in_url)
		return 1;
	
	work_buf = g_strsplit (url,"/",6);
	
	if (hint.in_popup) {
		gtk_widget_destroy(hint.hint_window);
		hint.in_popup = FALSE;
	}
	
	if(backend->is_module(work_buf[2])) 
		module = work_buf[2];
	else
		module = settings.MainWindowModule;
	if(strstr(work_buf[4],"x") && clicked) {
		tmpbuf = get_crossref(module,work_buf[3],work_buf[5]);
		if (tmpbuf) {
			gui_display_verse_list_in_sidebar(settings.
					  currentverse,
					  module,
					  tmpbuf);
			g_free(tmpbuf);
		}
	} else if(!clicked) {
		
		tmpbuf = get_footnote_body(module,work_buf[3],work_buf[5]);
		if (tmpbuf) {
			gui_display_in_hint_window(tmpbuf);
			g_free(tmpbuf);	
		}			
	}
	g_strfreev(work_buf);
	return 1;
	
}
 

/******************************************************************************
 * Name
 *   reference_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint reference_uri(const gchar * url)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
static gint reference_uri(const gchar * url, gboolean clicked)
{	
	static gchar **work_buf = NULL;
	gchar *module = NULL;
	
	if(!clicked)
		return 1;
	work_buf = g_strsplit (url,"/",4);
	
	if(backend->is_module(work_buf[2])) 
		module = work_buf[2];
	else
		module = settings.MainWindowModule;
	
	gui_display_verse_list_in_sidebar(settings.currentverse,
						  module,
						  work_buf[3]); 
	g_strfreev(work_buf);
	return 1;
}

/******************************************************************************
 * Name
 *   sword_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint sword_uri(const gchar * url)
 *
 * Description
 *   handle a sword uri in the form 'sword://KJV/1John5:8'
 *                                   'sword://MHC/Genesis1:1'
 *                                   'sword:///Romans8:28'
 *   and display in the appropriate pane
 *
 * Return value
 *   gint
 */

static gint sword_uri(const gchar * url, gboolean clicked)
{
	gchar *buf = NULL;
	gchar *module = NULL;
	const gchar *key = NULL;
	gchar *tmpkey = NULL;
	gint mod_type;
	gint verse_count;
	gboolean change_verse = FALSE;
	gchar **work_buf = NULL;
		
	if(!clicked) {
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), url);
		return 1;
	}
	
	work_buf = g_strsplit (url,"/",4);
	if(!work_buf[2] && !work_buf[KEY]) {
		alert_url_not_found(url);
		g_strfreev(work_buf);
		return 0;
	}
	if(!work_buf[3]) {
		tmpkey = work_buf[MODULE];		
	} else
		tmpkey = work_buf[KEY];
	
	verse_count = backend->is_Bible_key(tmpkey, settings.currentverse);
	if(!work_buf[3] && !verse_count){
		alert_url_not_found(url);
		g_strfreev(work_buf);
		return 0;
	}
	if(backend->is_module(work_buf[MODULE])) {
		mod_type = backend->module_type(work_buf[MODULE]);
		switch(mod_type) {
			case TEXT_TYPE:	
				key = gui_update_nav_controls(tmpkey);
				main_display_bible(work_buf[MODULE], key);
				main_display_commentary(
						settings.CommWindowModule, key);
				if(key) g_free((gchar*)key);
			break;
			case COMMENTARY_TYPE:				
				key = gui_update_nav_controls(tmpkey);
				main_display_bible(
						settings.MainWindowModule, key);
				main_display_commentary(work_buf[MODULE], key);
				if(key) g_free((gchar*)key);	
			break;
			case DICTIONARY_TYPE:
				main_display_dictionary(work_buf[MODULE],
							tmpkey);
			break;
			case BOOK_TYPE:
				main_display_book(work_buf[MODULE], tmpkey); 
			break;
		}
	} else { /* module name not found or not given */
		if(verse_count) { 
			key = gui_update_nav_controls(tmpkey);
			/* display in current Bible and Commentary */
			main_display_bible(settings.MainWindowModule, key);
			main_display_commentary(settings.CommWindowModule, key);
			if(key) g_free((gchar*)key);
		} else {
			alert_url_not_found(url);
		}
	} 
	g_strfreev(work_buf);
	
	/*
	 * change parallel verses
	 */
	if (settings.dockedInt) {
		gui_update_parallel_page();
	}
	
	/* 
	 * add item to history 
	 */
	if (settings.addhistoryitem) {
		if (strcmp(settings.currentverse, history_list[history_items - 1].verseref))
			//g_warning("currentverse = %s",settings.currentverse);
			gui_add_history_Item(widgets.app,
				       GTK_WIDGET
				       (widgets.shortcutbar),
				       settings.currentverse);
	}
	settings.addhistoryitem = TRUE;
	
	return 1;
	
}

/******************************************************************************
 * Name
 *   gui_url_handler
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint gui_url_handler(const gchar * url)
 *
 * Description
 *   
 *
 * Return value
 *   gint
 */

gint main_url_handler(const gchar * url, gboolean clicked)
{		
	//g_warning(url);
	if(strstr(url,"sword://"))
		return sword_uri(url,clicked);
	if(strstr(url,"book://"))
		return sword_uri(url,clicked);
	if(strstr(url,"chapter://"))
		return sword_uri(url,clicked);
	if(strstr(url,"reference://"))
		return reference_uri(url,clicked);
	if(strstr(url,"noteID://"))
		return note_uri(url,clicked);
	if(strstr(url,"strongs://"))
		return strongs_uri(url,clicked);
	if(strstr(url,"morph://"))
		return morph_uri(url,clicked);
	if(strstr(url,"about://"))
		return about_uri(url,clicked);
	if(strstr(url,"parallel://"))
		return parallel_uri(url,clicked);
	/*if(clicked)
		alert_url_not_found(url);*/
	return 0;
}

/******   end of file   ******/
