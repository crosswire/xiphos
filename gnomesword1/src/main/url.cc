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
#include <url.h>


#include <swbuf.h>
#include <gnome.h>

#include "gui/about_modules.h"
#include "gui/html.h"
#include "gui/history.h"
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

#include "main/url.hh"
#include "main/lists.h"
#include "main/settings.h"
#include "main/module_dialogs.h"
#include "main/parallel_view.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/xml.h"

#include "backend/sword_main.hh"


using namespace sword;


extern HISTORY history_list[];	/* sturcture for storing history items */
extern gint history_items;

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/url.hh"
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
 *   #include "main/url.hh"
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
	text = main_get_striptext(mod, key);
	str = remove_linefeeds(text);
	if (str) {
		gnome_appbar_set_status(GNOME_APPBAR(appbar), str);
		g_free(str);
	}
	g_free(text);
}


/******************************************************************************
 * Name
 *   show_mod_info
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_mod_info(const gchar * module, const gchar * discription,
 *						gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint show_mod_info(const gchar * module, const gchar * discription,
						gboolean clicked)
{
	if(clicked) {
		gui_display_about_module_dialog((gchar*)module, FALSE);		
	} else {
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
					discription);
	}
	return 1;
}


 /******************************************************************************
 * Name
 *   show_parallel
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_parallel(const gchar * value, const gchar * type,gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint show_parallel(const gchar * value, const gchar * type, 
							gboolean clicked)
{
	if(!strcmp(type,"swap")) {
		if(clicked) {
			main_swap_parallel_with_main((gchar *) value);		
		} else {
			gchar *buf = g_strdup_printf( 
				_("Show %s in main window"), value);
			gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
						buf);
			g_free(buf);
		}
	}
	if(!strcmp(type,"verse")) {
		if(clicked) {
		//g_warning(work_buf[3]);
			settings.cvparallel = 
				main_parallel_update_controls(value);
			main_update_parallel_page_detached();
		} 
	}
	return 1;
}


/******************************************************************************
 * Name
 *   morph_uri
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint morph_uri(const gchar * url, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint show_morph(const gchar * type, const gchar * value, 
				gboolean clicked)
{	
	gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	
	//g_warning("value = %s",value);
	if(!strcmp(type,"Greek") || strstr(type,"x-Robinson")) {
		if(backend->is_module("Robinson")) 
			modbuf = "Robinson";
	} 		
	if (clicked) {
		if (settings.inDictpane)
			main_display_dictionary(modbuf, (gchar*)value);
		if (settings.inViewer)
			main_sidebar_display_dictlex(modbuf, (gchar*)value);		
	} else {
		mybuf =
		    main_get_rendered_text(modbuf, (gchar*)value);
		if (mybuf) {
			main_information_viewer(modbuf, 
					mybuf, 
					(gchar*)value, 
					"showMorph",
					(gchar*)type,
					NULL,
					NULL);
			//show_in_appbar(widgets.appbar, (gchar*)value, modbuf);
			g_free(mybuf);
		}
	}
	return 1;
	
}



 /******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   
 *
 * Description
 *  
 *
 * Return value
 *   
 */ 
 
static gint show_strongs(const gchar * type, const gchar * value, 
			gboolean clicked)
{	
	gchar *modbuf_viewer = NULL;
	gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	guint delay;	
	guint i;	
	
	//g_warning("url = %s",url);
	if(!strcmp(type,"Greek")) {
		if(backend->is_module(settings.lex_greek_viewer)) 
			modbuf_viewer = settings.lex_greek_viewer;
		if(backend->is_module(settings.lex_greek)) 
			modbuf = settings.lex_greek;
	} else if(!strcmp(type,"Hebrew")) {
		if(backend->is_module(settings.lex_hebrew_viewer)) 
			modbuf_viewer = settings.lex_hebrew_viewer;
		if(backend->is_module(settings.lex_hebrew)) 
			modbuf = settings.lex_hebrew;
	}
	
	if (clicked) {
		if (settings.inDictpane)
			main_display_dictionary(modbuf, (gchar*)value);
		if (settings.inViewer)
			main_sidebar_display_dictlex(modbuf_viewer, (gchar*)value);		
	} else {
		mybuf =
		    main_get_rendered_text(modbuf, (gchar*)value);
		if (mybuf) {
			main_information_viewer(  
					modbuf, 
					mybuf, 
					(gchar*)value, 
					"showStrongs",
					(gchar*)type,
					NULL,
					NULL);
			g_free(mybuf);
		}
	}
	return 1;
	
}


 /******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   
 *
 * Description
 *  
 *
 * Return value
 *   
 */ 
 
static gint show_strongs_morph(const gchar * type, const gchar * value, 
			 const gchar * morph, gboolean clicked)
{	
	gchar *modbuf_viewer = NULL;
	gchar *modbuf = NULL;
	gchar *morph_mod = NULL;
	gchar *strongs_buf = NULL;
	gchar *morph_buf = NULL;
	guint delay;	
	guint i;	
	
	//g_warning("url = %s",url);
	if(!strcmp(type,"Greek")) {
		if(backend->is_module(settings.lex_greek_viewer)) 
			modbuf_viewer = settings.lex_greek_viewer;
		if(backend->is_module(settings.lex_greek)) 
			modbuf = settings.lex_greek;
		if(backend->is_module("Robinson")) 
			morph_mod = "Robinson";
	} else if(!strcmp(type,"Hebrew")) {
		if(backend->is_module(settings.lex_hebrew_viewer)) 
			modbuf_viewer = settings.lex_hebrew_viewer;
		if(backend->is_module(settings.lex_hebrew)) 
			modbuf = settings.lex_hebrew;
	}
	
	if (clicked) {
		if (settings.inDictpane)
			main_display_dictionary(modbuf, (gchar*)value);
		if (settings.inViewer)
			main_sidebar_display_dictlex(modbuf_viewer, (gchar*)value);		
	} else {
		strongs_buf =
		    main_get_rendered_text(modbuf, (gchar*)value);
		morph_buf =
		    main_get_rendered_text(morph_mod, (gchar*)morph);
		if (strongs_buf) {
			main_information_viewer(  
					modbuf, 
					strongs_buf, 
					(gchar*)value, 
					"showStrongsMorph",
					(gchar*)type,
					(gchar*)morph_buf,
					(gchar*)morph);
			g_free(strongs_buf);
			if(morph_buf) g_free(morph_buf);
		}
	}
	return 1;
	
}
 

/******************************************************************************
 * Name
 *   note_uri
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint note_uri(const gchar * url)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
static gint show_note(const gchar * module, const gchar * passage, 
		const gchar * type, const gchar * value, gboolean clicked)
{	
	gchar *tmpbuf = NULL;
	gchar *buf = NULL;
	GString *str = g_string_new(NULL);
	GList *tmp = NULL;
	gint i = 0;
	
	if(!in_url)
		return 1;	
/*	
	if (hint.in_popup) {
		gtk_widget_destroy(hint.hint_window);
		hint.in_popup = FALSE;
	}
*/	
	if(/*module && (strlen(module) > 2) && */!backend->is_module((gchar*)module)) 
		module = settings.MainWindowModule;
	
	if(passage && (strlen(passage) < 5)) 
		passage = settings.currentverse;
	
	if(strstr(type,"x") && clicked) {
		backend->set_module_key((gchar*)module, (gchar*)passage);
		tmpbuf = backend->get_entry_attribute("Footnote",
							 (gchar*)value,
							"refList");
		if (tmpbuf) {
			main_display_verse_list_in_sidebar(settings.
					  currentverse,
					  (gchar*)module,
					  tmpbuf);
			g_free(tmpbuf);
		}
	} else if(strstr(type,"n") && !clicked) {
		backend->set_module_key((gchar*)module, (gchar*)passage);		
		tmpbuf = backend->get_entry_attribute("Footnote",
						(gchar*)value,
						"body");		
		buf = backend->render_this_text((gchar*)module,(gchar*)tmpbuf);
		if(tmpbuf) g_free(tmpbuf);	
		if (buf) {

			main_information_viewer((gchar*)module, 
					buf, 
					(gchar*)value,
					"showNote",			
					(gchar*)type,
					NULL,
					NULL);
			//gui_display_in_hint_window(tmpbuf);
			if(buf) g_free(buf);	
		}			
	} else if(strstr(type,"x") && !clicked) {
		backend->set_module_key((gchar*)module, (gchar*)passage);		
		tmpbuf = backend->get_entry_attribute("Footnote",
						(gchar*)value,
						"refList");
		
		
		list_of_verses = g_list_first(list_of_verses);
		if(list_of_verses) {
			while(list_of_verses) {
				g_free(list_of_verses->data);
				list_of_verses = g_list_next(list_of_verses);
			}
			g_list_free(list_of_verses);
		}
		list_of_verses = NULL;
		
		tmp = backend->parse_verse_list(tmpbuf, settings.currentverse);  
		while (tmp != NULL) {
			buf = g_strdup_printf(
				"<a href=\"sword://%s/%s\">"
				"<font color=\"%s\">%s,</font></a><br>",
				(gchar*)module,
				(const char *) tmp->data,
				settings.bible_text_color,
				(const char *) tmp->data);
			str = g_string_append(str,buf);
			if(buf) g_free(buf);
			buf = NULL;
			++i;
			g_free((char *) tmp->data);
			tmp = g_list_next(tmp);
		}
		g_list_free(tmp);
		
		buf = g_strdup_printf(
				"<a href=\"sword://%s/%s\">"
				"<font color=\"%s\">%s%s</font></a><br>",
				(gchar*)module,
				settings.currentverse,
				settings.bible_text_color,
				_("Back to "),
				settings.currentverse);
		str = g_string_append(str,buf);
		if(buf) g_free(buf);
		
		//buf = backend->render_this_text((gchar*)module,(gchar*)tmpbuf);
		if(tmpbuf) g_free(tmpbuf);	
		if (str) {

			main_information_viewer((gchar*)module, 
					str->str, 
					(gchar*)value,
					"showNote",			
					(gchar*)type,
					NULL,
					NULL);
			//gui_display_in_hint_window(tmpbuf);
		}			
	}
	g_string_free(str, 1);
	return 1;
	
}


/******************************************************************************
 * Name
 *   reference_uri
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint reference_uri(const gchar * url)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
static gint show_ref(const gchar * module, const gchar * list, gboolean clicked)
{	
	if(!clicked)
		return 1;
	
	if(!backend->is_module(module)) 
		module = settings.MainWindowModule;
	main_display_verse_list_in_sidebar(settings.currentverse,
						  (gchar*)module,
						  (gchar*)list); 
	return 1;
}



/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint (const gchar * url)
 *
 * Description
 *   
 *   
 *
 * Return value
 *   gint
 */

static gint show_bookmark(const gchar * module, const gchar * key, 
						const gchar * type)//const gchar * url, gboolean use_dialog)
{
	gchar *buf = NULL;
	gchar *tmpkey = NULL;
	gint mod_type;
	gint verse_count;
	gboolean change_verse = FALSE;
	
	verse_count = backend->is_Bible_key(key, settings.currentverse);
	if(!verse_count){
		alert_url_not_found(key);
		return 0;
	}
	
	if(backend->is_module(module)) {
		if(!strcmp(type,"newTab")) {
			main_open_bookmark_in_new_tab((gchar*)module, 
					(gchar*)key);
			return 1;
		}
		
		mod_type = backend->module_type((gchar*)module);
		switch(mod_type) {
			case TEXT_TYPE:	
				if(!strcmp(type,"newDialog")) {
					main_dialog_goto_bookmark(
						(gchar*)module,
						(gchar*)key);					
				} else {
					tmpkey = main_update_nav_controls(key);
					main_display_bible(module, tmpkey);
					main_display_commentary(
							settings.CommWindowModule, tmpkey);
					main_keep_bibletext_dialog_in_sync((gchar*)tmpkey);
					if(tmpkey) g_free((gchar*)tmpkey);
				}
			break;
			case COMMENTARY_TYPE:
			case PERCOM_TYPE:
				if(!strcmp(type,"newDialog")) {
					main_dialog_goto_bookmark(
						(gchar*)module,
						(gchar*)key);
				} else {				
					tmpkey = main_update_nav_controls(key);
					main_display_bible(
							settings.MainWindowModule, tmpkey);
					main_display_commentary(module, tmpkey);
					if(tmpkey) g_free((gchar*)tmpkey);	
				}
			break;
			case DICTIONARY_TYPE:
				main_display_dictionary((gchar*)module,
							(gchar*)key);
			break;
			case BOOK_TYPE:
				main_display_book((gchar*)module, (gchar*)key); 
			break;
		}
	} 
	return 1;
}


/******************************************************************************
 * Name
 *   sword_uri
 *
 * Synopsis
 *   #include "main/url.hh"
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
        gsize bytes_read;
        gsize bytes_written;
        GError **error;
	char *mykey;
		
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
	
	mykey = g_convert(tmpkey,
                             -1,
                             OLD_CODESET,
                             UTF_8,
                             &bytes_read,
                             &bytes_written,
                             error);
	
	verse_count = backend->is_Bible_key(mykey, settings.currentverse);
	if(!work_buf[3] && !verse_count){
		alert_url_not_found(url);
		g_strfreev(work_buf);
		return 0;
	}
	if(backend->is_module(work_buf[MODULE])) {
		mod_type = backend->module_type(work_buf[MODULE]);
		switch(mod_type) {
			case TEXT_TYPE:	
				key = main_update_nav_controls(tmpkey);
				main_display_commentary(
						settings.CommWindowModule, key);
				main_display_bible(work_buf[MODULE], key);
				main_keep_bibletext_dialog_in_sync((gchar*)key);
				if(key) g_free((gchar*)key);
			break;
			case COMMENTARY_TYPE:				
				key = main_update_nav_controls(tmpkey);
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
			key = main_update_nav_controls(tmpkey);
			/* display in current Bible and Commentary */
			main_display_commentary(settings.CommWindowModule, key);
			main_display_bible(settings.MainWindowModule, key);
			main_keep_bibletext_dialog_in_sync((gchar*)key);
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
		main_update_parallel_page();
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
 *   #include "main/url.hh"
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
	gchar* action = NULL;
	gchar* type = NULL;
	gchar* value = NULL;
	gchar* module = NULL;
	gchar* passage = NULL;
	gchar* morph = NULL;
	gchar* strongs = NULL;
	gchar *buf = NULL;
	URL* m_url;
	
	if(strstr(url,"passagestudy.jsp") || strstr(url,"gnomesword.url")) {/* passagestudy.jsp?action=showStrongs&type= */
		m_url = new URL((const char*)url);	
		action = g_strdup(m_url->getParameterValue("action"));
		type = g_strdup((gchar*)m_url->getParameterValue("type"));
		value = g_strdup((gchar*)m_url->getParameterValue("value"));
		morph = g_strdup((gchar*)m_url->getParameterValue("morph"));
		strongs = g_strdup((gchar*)m_url->getParameterValue("lemma"));

#ifdef DEBUG
		g_warning("main_url_handler()");
		g_warning("url = %s",url);
		g_warning("action = %s",action);
		g_warning("type = %s",type);  
		g_warning("value = %s",value);
		g_warning("strongs = %s",strongs);  
		g_warning("morph = %s",morph);
#endif	
		
		if(strlen(strongs) > 2 && strlen(morph) > 2 ) {
			show_strongs_morph(type,strongs,morph,clicked);
		} else if(strlen(strongs) > 2 && strlen(morph) < 2) {
			show_strongs(type,strongs,clicked);			
		}
		
		if(!strcmp(action,"showStrongs")) 
			show_strongs(type,value,clicked);
		
		
		if(!strcmp(action,"showMorph"))
			show_morph(type, value,clicked);
		
		if(!strcmp(action,"showNote")) {	
			module = g_strdup(m_url->getParameterValue("module"));
			passage = g_strdup((gchar*)m_url->getParameterValue("passage"));
			show_note(module, passage, type, value, clicked);
			if(module) g_free(module);
			if(passage) g_free(passage);
		}
		
		if(!strcmp(action,"showRef")) {	
			module = g_strdup(m_url->getParameterValue("module"));
			if(!strcmp(type,"scripRef"))
				show_ref(module,value,clicked);
			if(!strcmp(type,"swordURL")) {
				
			}
			if(module) g_free(module);
		}
		
		if(!strcmp(action,"showBookmark")) {	
			module = g_strdup(m_url->getParameterValue("module"));
			show_bookmark(module, value, type);
			if(module) g_free(module);
		}		
		
		if(!strcmp(action,"showModInfo")) {	
			module = g_strdup(m_url->getParameterValue("module"));
			show_mod_info(module, value, clicked);
			if(module) g_free(module);
		}		
		
		if(!strcmp(action,"showParallel")) {
			show_parallel(value, type, clicked);
		}
		
		if(action) g_free(action);
		if(type) g_free(type);
		if(value) g_free(value);
		if(strongs) g_free(strongs);
		if(morph) g_free(morph);
		return 1;
	}
	if(strstr(url,"sword://"))
		return sword_uri(url,clicked);
	if(strstr(url,"book://"))
		return sword_uri(url,clicked);
	if(strstr(url,"chapter://"))
		return sword_uri(url,clicked);
	return 0;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   
 *
 * Description
 *  
 *
 * Return value
 *   
 */
 
const gchar *main_url_encode(const gchar * pram)
{
	SWBuf retval;
	retval = URL::encode(pram);
	//g_warning(retval.c_str());
	if(retval.length())
		return g_strdup(retval.c_str());
	else
		return g_strdup("");
}		

/******   end of file   ******/
