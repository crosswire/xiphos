/*
 * GnomeSword Bible Study Tool
 * sword.c - glue 
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
#include <swmgr.h>
#include <swmodule.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>
#ifdef __cplusplus
}
#endif

#include <ctype.h>
#include <time.h>

#include "gui/main_window.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"
#include "gui/commentary.h"
#include "gui/parallel_view.h"
#include "gui/tabbed_browser.h"
#include "gui/gnomesword.h"
#include "gui/utilities.h"

#include "main/display.hh"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"
 
#include "backend/sword.h"
#include "backend/sword_defs.h"
#include "backend/sword_main.hh"
#include "backend/mgr.hh"

	
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"

gboolean style_display = TRUE;

/*void delete_module_mgr(void)
{
	backend_delete_module_mgr();
}
*/
/******************************************************************************
 * Name
 *  module_name_from_description
 *
 * Synopsis
 *   #include ".h"
 *
 *   void module_name_from_description(gchar *mod_name, gchar *description)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

char *main_module_name_from_description(char *description)
{
	return backend->module_name_from_description(description);
}

/******************************************************************************
 * Name
 *  get_sword_version
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   const char *get_sword_version(void)	
 *
 * Description
 *    
 *
 * Return value
 *   const char *
 */ 

const char *get_sword_version(void)
{
	return backend_get_sword_version();
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *main_get_crossref(char * mod_name, char * key, char * note_number)
{
	
		backend->set_module_key(mod_name, key);
		return g_strdup(backend->get_entry_attribute("Footnote",
							note_number,
							"refList"));
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *main_get_footnote_body(char * mod_name, char * key, char * note_number)
{
	
	backend->set_module_key(mod_name, key);
	return g_strdup(backend->get_entry_attribute("Footnote",
							note_number,
							"body"));
}


/******************************************************************************
 * Name
 *   get_search_results_text
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *get_search_results_text(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *main_get_search_results_text(char * mod_name, char * key)	
{
	return g_strdup(backend->get_render_text((char *)mod_name,(char *)key));
}



/******************************************************************************
 * Name
 *  get_path_to_mods
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   	gchar *get_path_to_mods(void)
 *
 * Description
 *    returns the path to the sword modules
 *
 * Return value
 *   gchar *
 */ 

char *get_path_to_mods(void)
{
	return backend_get_path_to_mods();
}


void init_sword(void)
{
	backend_init();
}
/******************************************************************************
 * Name
 *   shutdown_sword
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void shutdown_sword(void)	
 *
 * Description
 *   close down sword by calling backend_shutdown();
 *
 * Return value
 *   void
 */

void shutdown_backend(void)
{
	backend_shutdown(TRUE);
}

/******************************************************************************
 * Name
 *   main_locked_module_display
 *
 * Synopsis
 *   #include ".h"
 *
 *   main_locked_module_display(GtkWidget * html_widget,
				  gchar * mod_name, gchar * cipher_key)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_locked_module_display(GtkWidget * html_widget,
				  gchar * mod_name, gchar * cipher_key)
{
	GtkHTML *html = GTK_HTML(html_widget);
	GtkHTMLStreamStatus status1;
	GtkHTMLStream *htmlstream;
	gchar buf[500];
	GString *str;

	str = g_string_new("");
	htmlstream =
	    gtk_html_begin(html);

	g_string_printf(str, "%s", HTML_START "<body><br>");
	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
	}

	if (!cipher_key) {
		g_string_printf(str, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name, "</b>", _("module is locked."));
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	} else {
		g_string_printf(str, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name,
			"</b>", _("module has been unlocked."));
		
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	}

	if (!cipher_key) {
		g_string_printf(str, "%s %s %s%s%s %s %s %s",
			"<br><br>",
			_("If you have the cipher key you can"),
			"<a href=\"U",
			mod_name,
			"\">",
			_("click here"),
			" </a>", _("to unlock the module"));
		
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}

		g_string_printf(str, "%s%s",
			"<br><br>",
			_
			("You will need to restart GnomeSword after you unlock it."));

		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	} else {
		g_string_printf(str, "%s%s",
			"<br><br>",
			_("You need to restart GnomeSword to view it"));
		
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	}

	g_string_printf(str, "%s", "</body></html>");
	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	g_string_free(str,TRUE);

}


/******************************************************************************
 * Name
 *   mark_search_words
 *
 * Synopsis
 *   #include ".h"
 *
 *   void mark_search_words( GString *str )	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void mark_search_words(GString * str)
{
	gchar *tmpbuf, *buf, *searchbuf;
	gint len1, len2, len3, len4;
	gchar closestr[40], openstr[40];

	/* regular expression search results         **fixme** */
	if (settings.searchType == 0) {
		return;
	}
	/* close tags */
	sprintf(closestr, "</b></font>");
	/* open tags */
	sprintf(openstr, "<font color=\"%s\"><b>",
		settings.found_color);
	/* point buf to found verse */
	buf = str->str;
	searchbuf = g_strdup(settings.searchText);

	/* if we have a muti word search go here */
	if (settings.searchType == -2) {
		char *token;
		GList *list;
		gint count = 0, i = 0;

		list = NULL;
		/* seperate the search words and add them to a glist */
		if ((token = strtok(searchbuf, " ")) != NULL) {
			list = g_list_append(list, token);
			++count;
			while ((token = strtok(NULL, " ")) != NULL) {
				list = g_list_append(list, token);
				++count;
			}
			/* if we have only one word */
		} else {
			list = g_list_append(list, searchbuf);
			count = 1;
		}
		list = g_list_first(list);
		/* find each word in the list and mark it */
		for (i = 0; i < count; i++) {
			/* set len1 to length of verse */
			len1 = strlen(buf);
			/* set len2 to length of search word */
			len2 = strlen((gchar *) list->data);
			/* find search word in verse */
			if ((tmpbuf =
			     strstr(buf,
				    (gchar *) list->data)) != NULL) {
				/* set len3 to length of tmpbuf 
				   (tmpbuf points to first occurance of 
				   search word in verse) */
				len3 = strlen(tmpbuf);
				//-- set len4 to diff between len1 and len3
				len4 = len1 - len3;
				/* add end tags first 
				   (position to add tag to is len4 + len2) */
				str =
				    g_string_insert(str, (len4 + len2),
						    closestr);
				/* then add start tags 
				   (position to add tag to is len4) */
				str =
				    g_string_insert(str, len4, openstr);
			}
			/* point buf to changed str */
			buf = str->str;
			list = g_list_next(list);
		}
		g_list_free(list);

		/* else we have a phrase and only need to mark it */
	} else {
		len1 = strlen(buf);
		len2 = strlen(searchbuf);
		tmpbuf = strstr(buf, searchbuf);
		if (tmpbuf) {
			len3 = strlen(tmpbuf);
			len4 = len1 - len3;
			/* place end tag first */
			str =
			    g_string_insert(str, (len4 + len2),
					    closestr);
			/* then place start tag */
			str = g_string_insert(str, len4, openstr);
		}
	}
	/* free searchbuf */
	g_free(searchbuf);
}


/******************************************************************************
 * Name
 *   main_entry_display
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_entry_display(GtkWidget * html_widget, gchar * mod_name, 
 *					      gchar * text, gchar *key)
 *
 * Description
 *   display Sword modules one verse (entry) at a time
 *
 * Return value
 *   void
 */

void main_entry_display(GtkWidget * html_widget, gchar * mod_name,
		   gchar * text, gchar * key, gboolean show_key)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	GString *search_str;
	gboolean was_editable = FALSE;
	MOD_FONT *mf = get_font(mod_name);
	GtkHTML *html = GTK_HTML(html_widget);

	/* setup gtkhtml widget */
	was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);

	g_string_printf(tmp_str,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);

	str = g_string_new(tmp_str->str);
	/* show key in html widget  */
	if (show_key) {
		if ((settings.displaySearchResults)) {
			g_string_printf(tmp_str,
				"<a href=\"sword://%s/%s\">"
				"<font color=\"%s\">[%s] %s </font></A>",
				mod_name,
				key,
				settings.bible_verse_num_color,
				mod_name, key);
		} else {
			g_string_printf(tmp_str,
				"<a href=\"about://%s/%s\">"
				"<font color=\"%s\">[%s]</a></font>[%s] ",
				mod_name,
				backend->module_description(mod_name),
				settings.bible_verse_num_color,
				mod_name, 
				key);
		}
		str = g_string_append(str, tmp_str->str);
	}
	
	g_string_printf(tmp_str, 
			"<font face=\"%s\" size=\"%s\">",
			(mf->old_font)?mf->old_font:"none", 
			(mf->old_font_size)?mf->old_font_size:"+0");
	str = g_string_append(str, tmp_str->str);
	
	if (settings.displaySearchResults) {
		search_str = g_string_new(text);
		mark_search_words(search_str);
		str = g_string_append(str, search_str->str);
	} else {
		str = g_string_append(str, text);
	}

	g_string_printf(tmp_str, " %s", "</font></body></html>");
	str = g_string_append(str, tmp_str->str);

	if (str->len) {
		gtk_html_load_from_string(html,str->str,str->len);
	}
	gtk_html_set_editable(html, was_editable);

	/* andyp - inserted for debugging, remove */
	//g_print(str->str); 
	
	free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}

void main_dictionary_entery_changed(char * mod_name)
{
	gint count = 7, i;
	gchar *new_key, *text = NULL;
	gchar *key = NULL;
	static gboolean firsttime = TRUE;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	gint height;
	
	key = (gchar*)gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict));
	backend->set_module_key(mod_name, key);
	//g_warning("mod = %s key = %s",mod_name, key);
	key = backend->get_module_key();
	//g_warning(key);
	xml_set_value("GnomeSword", "keys", "dictionary", key);
	settings.dictkey = xml_get_value("keys", "dictionary");
	
	backend->set_module_key(mod_name, key);
	backend->display_mod->Display();
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets.listview_dict));
	list_store = GTK_LIST_STORE(model);
	
	if (!firsttime) {
		gdk_drawable_get_size ((GdkDrawable *)widgets.listview_dict->window,
                                             NULL,
                                             &height);
		count = height / settings.cell_height;
	}	 

	if (count) {
		gtk_list_store_clear(list_store);
		new_key = backend->navigate_module(-1);

		for (i = 0; i < (count / 2); i++) {
			free(new_key);
			new_key = backend->navigate_module(0);
		}

		for (i = 0; i < count; i++) {
			free(new_key);
			new_key = backend->navigate_module(1);
			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, 0,
					   new_key, -1);
		}
		free(new_key);
	}
	firsttime = FALSE;
} 


void main_display_book(const char * mod_name, char * key)
{
	if(!settings.havebook || !mod_name)
		return;
	if(!settings.book_mod)
		settings.book_mod = (char*)mod_name;
	
	//settings.comm_showing = FALSE;
	settings.whichwindow = BOOK_WINDOW;
	gtk_label_set_text (GTK_LABEL(widgets.label_comm),mod_name);
	gui_change_window_title(settings.book_mod);
	
	xml_set_value("GnomeSword", "keys", "offset", key);
	settings.book_offset = atol(xml_get_value( "keys", "offset"));
	
	if(strcmp(settings.book_mod,mod_name)) {
		xml_set_value("GnomeSword", "modules", "book", mod_name);
		settings.book_mod = xml_get_value( "modules", "book");
	}
	backend->set_module(mod_name);
	backend->set_treekey(settings.book_offset);
	backend->display_mod->Display();
	
	if(settings.browsing)
		gui_update_tab_struct(NULL,
				      NULL,
				      NULL,
				      mod_name,
				      NULL,
				      key?key:NULL,
				      FALSE);

}

void main_display_commentary(const char * mod_name, const char * key)
{
	
	if(!settings.havecomm || !mod_name || !settings.comm_showing)
		return;
	if(!settings.CommWindowModule)
		settings.CommWindowModule = (char*)mod_name;
	
	//settings.comm_showing = TRUE;
	settings.whichwindow = COMMENTARY_WINDOW;
	gtk_label_set_text (GTK_LABEL(widgets.label_comm),mod_name);
	gui_change_window_title(settings.CommWindowModule);
	
	if(strcmp(settings.CommWindowModule,mod_name)) {
		xml_set_value("GnomeSword", "modules", "comm", mod_name);
		settings.CommWindowModule = xml_get_value( "modules", "comm");
	}
	
	backend->set_module_key(mod_name, key);
	backend->display_mod->Display();
	
	if(settings.browsing)
		gui_update_tab_struct(NULL,
				      mod_name,
				      NULL,
				      NULL,
				      NULL,
				      NULL,
				      TRUE);
}

void main_display_dictionary(char * mod_name, char * key)
{
	const gchar *old_key;
	
	if(!settings.havedict || !mod_name)
		return;
	if(!settings.DictWindowModule)
		settings.DictWindowModule = (char*)mod_name;
	if(strcmp(settings.DictWindowModule,mod_name)) {
		xml_set_value("GnomeSword", "modules", "dict",
					mod_name);
		settings.DictWindowModule = xml_get_value(
					"modules", "dict");
	}
	gtk_label_set_text (GTK_LABEL(widgets.label_dict),mod_name);
	
	if(key == NULL)
		key = "Grace";
	
	old_key = gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict));
	if(!strcmp(old_key, key))
		main_dictionary_entery_changed(settings.DictWindowModule);
	else
		gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);
	
	if(settings.browsing)
		gui_update_tab_struct(NULL,
				      NULL,
				      mod_name,
				      NULL,
				      key,
				      NULL,
				      settings.comm_showing);
} 


void main_display_bible(const char * mod_name, const char * key)
{
	gchar *file = NULL;
	gchar *style = NULL;
	
	if(!settings.havebible || !mod_name)
		return;

	file = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	if(!settings.MainWindowModule)
		settings.MainWindowModule = (char*)mod_name;
	if(strcmp(settings.MainWindowModule, mod_name)) {
		xml_set_value("GnomeSword", "modules", "bible",
					mod_name);
		settings.MainWindowModule = xml_get_value(
					"modules", "bible");
	}
	
	if(strcmp(settings.currentverse, key)) {
		xml_set_value("GnomeSword", "keys", "verse",
					key);
		settings.currentverse = xml_get_value(
					"keys", "verse");
	}
	style = get_conf_file_item(file, mod_name, "style");
	if((style) && strcmp(style,"verse"))
		settings.versestyle = FALSE;
	else	
		settings.versestyle = TRUE;
	
	style_display = FALSE;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.versestyle_item),
				       settings.versestyle);
	style_display = TRUE;
	
	backend->set_module_key(mod_name, key);
	backend->display_mod->Display();
	
	if(settings.browsing) {
		gui_update_tab_struct(mod_name,
				      NULL,
				      NULL,
				      NULL,
				      NULL,
				      NULL,
				      settings.comm_showing);
		gui_set_tab_label(settings.currentverse);
	}

	/*
	 * change parallel verses
	 */
	if (settings.dockedInt) {
		gui_update_parallel_page();
	}

}


void main_change_verse(const char * bible, const char * commentary, 
						const char * key)
{
	
}


void main_setup_displays(void)
{
	sw.entryDisplay = new GTKEntryDisp(widgets.html_comm,backend);
	sw.dictDisplay = new GTKEntryDisp(widgets.html_dict,backend);
}

void main_setup_new_displays(void)
{
	backend->commDisplay = new GTKEntryDisp(widgets.html_comm,backend);
	backend->dictDisplay = new GTKEntryDisp(widgets.html_dict,backend);
	backend->textDisplay = new GTKChapDisp(widgets.html_text,backend);
	backend->RTOLDisplay = new GTKTextviewChapDisp(widgets.textview,backend);
}

const char *main_get_module_language(const char *module_name)
{
	return backend->module_get_language(module_name);
}



/******************************************************************************
 * Name
 *  main_check_for_global_option
 *
 * Synopsis
 *   #include ".h"
 *
 *  	gint main_check_for_global_option(gchar * mod_name, gchar * option)
 *
 * Description
 *    get global options for a module
 *
 * Return value
 *   gint
 */

gint main_check_for_global_option(gchar * mod_name, gchar * option)
{
	return backend->has_global_option(mod_name, option);
}

/******************************************************************************
 * Name
 *   main_is_module
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   int main_is_module(char * mod_name)
 *
 * Description
 *    check for presents of a module by name
 *
 * Return value
 *   int
 */
 
int main_is_module(char * mod_name)
{
	return backend->is_module(mod_name);
}

/******************************************************************************
 * Name
 *  get_mod_about_info
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   gchar *get_mod_about_info(char * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   gchar *
 */ 

char *main_get_mod_about_info(char * mod_name)
{
	return backend->get_config_entry(mod_name, "About");
}


const char *main_get_mod_config_entry(const char * module_name, const char * entry)
{
	return backend->get_config_entry((char*)module_name, (char*)entry);
}


/******************************************************************************
 * Name
 *  main_has_cipher_tag
 *
 * Synopsis
 *   #include "main/.h"
 *
 *   int main_has_cipher_tag(char *mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int main_has_cipher_tag(char *mod_name)
{
	gchar *tmpbuf = backend->get_config_entry(mod_name, "CipherKey");
	if(tmpbuf) {
		return true;
	}
	return false;
}



/******************************************************************************
 * Name
 *   main_get_striptext
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   char *main_get_striptext(char *module_name, char *key)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *main_get_striptext(char *module_name, char *key)
{
	return backend->get_strip_text(module_name, key);
}


/******************************************************************************
 * Name
 *   main_get_rendered_text
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   char *main_get_rendered_text(char *module_name, char *key)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *main_get_rendered_text(char *module_name, char *key)
{
	return backend->get_render_text(module_name, key);
}
/******************************************************************************
 * Name
 *  main_get_mod_type
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   int main_get_mod_type(char * mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int main_get_mod_type(char * mod_name)
{
	
	return backend->module_type(mod_name);
}


/******************************************************************************
 * Name
 *  main_get_module_description
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   gchar *main_get_module_description(gchar * module_name)	
 *
 * Description
 *    
 *
 * Return value
 *   gchar *
 */ 

char *main_get_module_description(char * module_name)
{
	return backend->module_description(module_name);
}
