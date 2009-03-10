/*
 * Xiphos Bible Study Tool
 * url.cc - support functions
 *
 * Copyright (C) 2004-2008 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */
 
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>	
#include <ctype.h>
#include <url.h>

#include <swbuf.h>

#include <swmgr.h>
#include <swmodule.h>

#include <gnome.h>

#ifdef USE_GTKHTML3_14_23
#include "editor/slib-editor.h"
#else
#include "editor/bonobo-editor.h"
#endif

#include "gui/about_modules.h"
#include "gui/xiphos.h"
#include "gui/widgets.h"
#include "gui/utilities.h"
#include "gui/sidebar.h"
#include "gui/dialog.h"
#include "gui/parallel_dialog.h"
#include "gui/parallel_view.h"
#include "gui/main_window.h"
#include "gui/studypad.h"
#include "gui/toolbar_nav.h"

#include "main/url.hh"
#include "main/lists.h"
#include "main/previewer.h"
#include "main/settings.h"
#include "main/module_dialogs.h"
#include "main/parallel_view.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/xml.h"

#include "backend/sword_main.hh"

gint in_url;

using namespace sword;


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
	dialog->stock_icon = (gchar*) GTK_STOCK_DIALOG_INFO;
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
 *   
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint (const gchar * filename, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint show_studypad(const gchar * filename, gboolean clicked)
{
#ifdef USE_GTKHTML38
	editor_create_new(filename, NULL, FALSE);
#else
	if (settings.studypad_dialog_exist) {
		gtk_widget_show(widgets.studypad_dialog);
		load_file((gchar*) filename, editor_cd);
		gdk_window_raise(GTK_WIDGET(widgets.studypad_dialog)->
				 window);
	} else {
		settings.studypad_dialog_exist =
			  gui_open_studypad((gchar*)filename);
	}
#endif
	return 1;
}


/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint (const gchar * filename, gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

const char *display_progs[] = {
#ifndef __CYGWIN__
	"/usr/bin/gvfs-open",
	"/usr/bin/gnome-open",
#endif /* !__CYGWIN__ */
	"/usr/bin/display",
	NULL
};

static gint show_separate_image(const gchar * filename, gboolean clicked)
{
	if (clicked) {
#ifdef WIN32
		// as of october 2008, gnome_url_show() doesn't behave
		// properly on linux, because it does not respect the user's
		// preferences properly.  so this is used on windows only.
		// it kicks off the standard windows fax/pic viewer.

		gboolean result;
		gchar *file_as_url = g_strdup_printf("file://%s", filename);

		GS_print(("file = %s\n", file_as_url));
		result = gnome_url_show(file_as_url, NULL);
		g_free(file_as_url);
		if (result == FALSE) {
			gui_generic_warning((char *)"Could not display that image");
		}
#else
		FILE *result;
		GString *cmd = g_string_new(NULL);
		int i;

		for (i = 0; display_progs[i]; i++) {
			if (access(display_progs[i], X_OK) == 0)
				break;
		}
		if (display_progs[i] == NULL) {
			gui_generic_warning((char *)"Xiphos cannot find\nan image display program.");
			return 0;
		}

		GS_print(("file = %s\n", filename));
		g_string_printf(cmd, "%s \"%s\" < /dev/null > /dev/null 2>&1 &",
				display_progs[i], filename);

		if ((result = popen(cmd->str, "r")) == NULL) {
			g_string_printf(cmd,
					_("Xiphos could not execute %s"),
					display_progs[i]);
			gui_generic_warning(cmd->str);
		} else {
			gchar output[258];
			if (fgets(output, 256, result) != NULL) {
				g_string_truncate(cmd, 0);
				g_string_append(cmd,
						_("Viewer error:\n"));
				g_string_append(cmd, output);
				gui_generic_warning(cmd->str);
			}
			pclose(result);
		}
		g_string_free(cmd, TRUE);
#endif
	} else {
		gui_set_statusbar (filename);
		//gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),filename);
	}
	return 1;
}


/******************************************************************************
 * Name
 *   show_mod_info
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_mod_info(const gchar * module, const gchar * description,
 *						gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint show_mod_info(const gchar * module, const gchar * description,
						gboolean clicked)
{
	if(clicked) {
		GS_print(("module = %s\n",module));
		gui_display_about_module_dialog((gchar*)module, FALSE);		
	} else {
		/* some mod descriptions contain fun(ny) characters */
		GString *desc_clean = hex_decode(description);
		GS_print(("description = %s\n", desc_clean->str));
		gui_set_statusbar (desc_clean->str);
		/*gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
					desc_clean->str);*/
		g_string_free(desc_clean, TRUE);
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
 
 static gint show_parallel(const gchar * svalue, const gchar * stype, 
							gboolean clicked)
{
	if(!strcmp(stype,"swap")) {
		if(clicked) {
			main_swap_parallel_with_main((gchar *) svalue);		
		} else {
			gchar *buf = g_strdup_printf( 
				_("Show %s in main window"), svalue);
			gui_set_statusbar (buf);
			//gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), buf);
			g_free(buf);
		}
	}
	if(!strcmp(stype,"verse")) {
		if(clicked) {
#ifdef OLD_NAVBAR
			settings.cvparallel = backend->get_valid_key(svalue);
			main_update_parallel_page_detached();
			gui_set_parallel_navbar((char*)svalue);
#else
			gtk_entry_set_text(GTK_ENTRY(navbar_parallel.lookup_entry),svalue);
			gtk_widget_activate(navbar_parallel.lookup_entry);
#endif
		} 
	}
	return 1;
}


/******************************************************************************
 * Name
 *   show_morph
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_morph(const gchar * type, const gchar * value, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint show_morph(const gchar * stype, const gchar * svalue, 
				gboolean clicked)
{	
	const gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	
	if (!strcmp(stype,"Greek") ||
	     strstr(stype,"x-Robinson") ||
	     strstr(stype,"robinson") ||
	     strstr(stype,"Robinson")) {
		if (backend->get_key_testament(settings.currentverse) == 2) {
			if (backend->is_module("Robinson")) 
				modbuf = "Robinson";
		} else {
			if (backend->is_module("Packard")) 
				modbuf = "Packard";
		}
	}
	//GS_message(("modbuf = %s", modbuf));
	if (clicked) {
		main_display_dictionary(modbuf, (gchar*)svalue);		
	} else {
		mybuf = main_get_rendered_text(modbuf, (gchar*)svalue);
		//GS_message(("mybuf = %s", mybuf));
		if (mybuf) {
			main_information_viewer(modbuf,
					mybuf,
					(gchar*)svalue,
					"showMorph",
					(gchar*)stype,
					NULL,
					NULL);
			g_free(mybuf);
		}
	}
	return 1;
}


 /******************************************************************************
 * Name
 *   show_strongs
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_strongs(const gchar * type, const gchar * value, 
			gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */ 
 
static gint show_strongs(const gchar * stype, const gchar * svalue, 
			gboolean clicked)
{	
	if ((stype == NULL) || (*stype == '\0'))
		return 1;	// it's a lemma only - no lexdict reference.

	gchar *modbuf_viewer = NULL;
	const gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	gchar *val = NULL;
	gchar *val1 = NULL;
	gchar *val2 = NULL;
	gchar *buf = g_new(gchar,strlen(svalue));
	guint delay;	
	guint i;	

	val = g_strdup(svalue);
	GS_message(("buf len = %d",strlen(buf)));
	/*if((val1 = strchar(val,'|')) != NULL)	{
		val1 = (val1) ? (val1 + 1) : val;
		
	}*/
	
	if(!strcmp(settings.MainWindowModule,"NASB")) {
		if(!strcmp(stype,"Greek")) 
			modbuf = "NASGreek";
		else 
			modbuf = "NASHebrew";
		//if(strstr(svalue,"!"))
			//g_strdelimit((gchar*)svalue,"!",'\0');
	} else {
		if(!strcmp(stype,"Greek")) 
			modbuf = settings.lex_greek;
		else  
			modbuf = settings.lex_hebrew;
	}
	
	if (clicked) {
		main_display_dictionary(modbuf, (gchar*)svalue);		
	} else {
		mybuf = main_get_rendered_text(modbuf, (gchar*)svalue);
		if (mybuf) {
			main_information_viewer(  
					modbuf, 
					mybuf, 
					(gchar*)svalue, 
					"showStrongs",
					(gchar*)stype,
					NULL,
					NULL);
			g_free(mybuf);
		}
	}
	g_free(val);
	return 1;
}


 /******************************************************************************
 * Name
 *   show_strongs_morph
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_strongs_morph(const gchar * type, const gchar * value, 
			 const gchar * morph, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */ 
 
static gint show_strongs_morph(const gchar * stype, const gchar * svalue, 
			 const gchar * morph, gboolean clicked)
{	
	gchar *modbuf_viewer = NULL;
	const gchar *modbuf = NULL;
	const gchar *morph_mod = NULL;
	gchar *strongs_buf = NULL;
	gchar *strongs_buf2 = NULL;
	gchar *morph_buf = NULL;
	gchar *morph_buf2 = NULL;
	guint delay;	
	guint i;	
	gchar *buf = NULL;
	gchar *val = g_new(gchar,strlen(svalue));
	gchar *val2 = NULL;
	//gboolean realstrongs;
	
	
	
	if(!strcmp(settings.MainWindowModule,"NASB")) {
		if(!strcmp(stype,"Greek")) 
			modbuf = "NASGreek";
		else 
			modbuf = "NASHebrew";
	} else {
		if(!strcmp(stype,"Greek")) {
			modbuf = settings.lex_greek;
			if(backend->is_module("Robinson")) 
				morph_mod = "Robinson";
			//realstrongs = (!strcmp(modbuf,"StrongsRealGreek"));
		} else {
			modbuf = settings.lex_hebrew;
			//realstrongs = (!strcmp(modbuf,"StrongsRealHebrew"));
		}
	}
	buf = g_strdup(svalue);
	//g_message("buf len = %d",strlen(buf));
	if(strchr(buf,'|')) {
		//g_message("buf = %s", buf);
		gint valuelen = strlen(svalue);
		for (i = 0; i < valuelen; i++) {
			if(svalue[i] == '|') {
				val[i] = '\0';
				break;
			}
			val[i] = svalue[i];
		}
		val2 = strchr(buf,'|');
		++val2;
		strongs_buf = g_strdup_printf("%s<br /><br />%s",
					main_get_rendered_text(modbuf, (gchar*)val),
					main_get_rendered_text(modbuf, (gchar*)val2));
	} else { 
		strongs_buf = main_get_rendered_text(modbuf, (gchar*)svalue);
	}
	
	// morph stuff
	g_free(buf);	
	g_free(val);	
	buf = g_strdup(morph);			
	val = g_new(gchar,strlen(morph));
	if(morph_mod)  {		
		if(strchr(buf,'|')) {
			gint morphlen = strlen(morph);
			GS_message(("buf = %s", buf));
			for (i = 0; i < morphlen; i++) {
				if(morph[i] == '|') {
					val[i] = '\0';
					break;
				}
				val[i] = morph[i];
			}
			val2 = strchr(buf,'|');
			++val2;
			
			GS_message(("val = %s", val));
			GS_message(("val2 = %s", val2));
			morph_buf = g_strdup_printf("%s<br />%s<br /><br />%s<br />%s",
					val,
					main_get_rendered_text(morph_mod, (gchar*)val),
					val2,
					main_get_rendered_text(morph_mod, (gchar*)val2));		
		} else { 
			morph_buf = main_get_rendered_text(morph_mod, (gchar*)morph);
		}
	}
	
	if (clicked) {
		main_display_dictionary(modbuf, (gchar*)svalue);		
	} else {
		//morph_buf = main_get_rendered_text(morph_mod, (gchar*)morph);
		if (strongs_buf) {
			main_information_viewer(  
					modbuf, 
					strongs_buf, 
					(gchar*)svalue, 
					"showStrongsMorph",
					(gchar*)stype,
					(gchar*)morph_buf,
					(gchar*)morph);
		}
	}
	if(morph_buf) g_free(morph_buf);
	g_free(buf);
	g_free(val);
	g_free(strongs_buf);
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
		const gchar * stype, const gchar * svalue, gboolean clicked)
{	
	gchar *tmpbuf = NULL;
	gchar *buf = NULL;
	gchar *work_buf = NULL;
	GString *str = g_string_new(NULL);
	GList *tmp = NULL;
	gint i = 0, j = 0;
	RESULTS *list_item;
	
	if(!in_url)
		return 1;
	
	if(!backend->is_module((gchar*)module)) 
		module = settings.MainWindowModule;
	
	if(passage && (strlen(passage) < 5))
		passage = settings.currentverse;
	
	//
	// if we are asking for a note/xref in n:0,
	// we must stop autonormalization for a moment.
	//
	int stop_autonorm = ((strstr(passage, ":0")) != NULL);
	VerseKey *vkey;
	char oldAutoNorm;
	if (stop_autonorm) {
		SWMgr *mgr = backend->get_display_mgr();
		backend->display_mod = mgr->Modules[module];
		backend->display_mod->setKey(passage);
		vkey = (VerseKey*)(SWKey*)(*backend->display_mod);
		oldAutoNorm = vkey->AutoNormalize();
		vkey->AutoNormalize(0);
		vkey->Chapter(vkey->Chapter() + 1);
		vkey->Verse(0);
	} else
		backend->set_module_key((gchar*)module, (gchar*)passage);

	if(strchr(stype,'x') && clicked) {
		tmpbuf = backend->get_entry_attribute("Footnote",
						      (gchar*)svalue,
						      "refList");
		if (tmpbuf) {
			main_display_verse_list_in_sidebar(settings.
							   currentverse,
							   (gchar*)module,
							   tmpbuf);
			g_free(tmpbuf);
		}
	} else if(strchr(stype,'n') && !clicked) {
		tmpbuf = backend->get_entry_attribute("Footnote",
						      (gchar*)svalue,
						      "body");
		buf = backend->render_this_text((gchar*)module,(gchar*)tmpbuf);
		if(tmpbuf) g_free(tmpbuf);
		if (buf) {
			main_information_viewer((gchar*)module,
						buf,
						(gchar*)svalue,
						"showNote",
						(gchar*)stype,
						NULL,
						NULL);
			if(buf) g_free(buf);
		}
	} else if(strchr(stype,'x') && !clicked) {
		tmpbuf = backend->get_entry_attribute("Footnote",
						      (gchar*)svalue,
						      "refList");
		list_of_verses = g_list_first(list_of_verses);
		if(list_of_verses) {
			while(list_of_verses) {
				list_item = (RESULTS*)list_of_verses->data;
				g_free(list_item->module);
				g_free(list_item->key);
				g_free(list_item);
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
			//++i;
			g_free((char *) tmp->data);
			tmp = g_list_next(tmp);
		}
		g_list_free(tmp);

		buf = g_strdup_printf("<a href=\"sword://%s/%s\">"
				      "<font color=\"%s\">%s%s</font></a><br>",
				      (gchar*)module,
				      settings.currentverse,
				      settings.bible_text_color,
				      _("Back to "),
				      settings.currentverse);
		str = g_string_append(str,buf);
		if(buf) g_free(buf);

		if(tmpbuf) g_free(tmpbuf);
		if (str) {
			main_information_viewer((gchar*)module,
						str->str,
						(gchar*)svalue,
						"showNote",
						(gchar*)stype,
						NULL,
						NULL);
		}
	}

	if (stop_autonorm)
		vkey->AutoNormalize(oldAutoNorm);
	if(work_buf)
		g_free(work_buf);
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
 *   show_module_and_key
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_module_and_key(const gchar * module, const gchar * key, 
					const gchar * type, gboolean clicked)
 *
 * Description
 *   
 *   
 *
 * Return value
 *   gint
 */

static int show_module_and_key(const char * module, const char * key, 
					const char * stype, gboolean clicked)
{
	gchar *buf = NULL;
	gchar *tmpkey = NULL;
	gint mod_type;
	gint verse_count;
	gboolean change_verse = FALSE;
	
	if(module && (strlen((char*)module) < 3) && 
		backend->is_Bible_key(key, settings.currentverse)) {
		module = settings.MainWindowModule;
	}
	if(!clicked) {
		//gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), url);
		return 1;
	}
	
	if(backend->is_module(module)) {
		if(!strcmp(stype,"newTab")) {
			main_open_bookmark_in_new_tab((gchar*)module, 
					(gchar*)key);
			return 1;
		}
		if(!strcmp(stype,"newDialog"))  {
#ifdef USE_GTKHTML38
			if(module && (main_get_mod_type((gchar*)module) == PERCOM_TYPE)) {			
				editor_create_new(module,key,TRUE);
				return 1;
			}
#endif
			main_dialog_goto_bookmark((gchar*)module,
						(gchar*)key);
			return 1;
		}		
		mod_type = backend->module_type((gchar*)module);
		switch(mod_type) {
			case TEXT_TYPE:	
				tmpkey = main_update_nav_controls(key);
				main_display_bible(module, tmpkey);
				main_display_commentary(NULL, tmpkey);
				main_keep_bibletext_dialog_in_sync((gchar*)tmpkey);
#ifdef USE_GTKHTML38
				editor_sync_with_main();
#endif
				if(tmpkey) g_free((gchar*)tmpkey);
			break;
			case COMMENTARY_TYPE:
			case PERCOM_TYPE:				
				tmpkey = main_update_nav_controls(key);
				main_display_bible(NULL, tmpkey);
				main_display_commentary(module, tmpkey);
				if(tmpkey) g_free((gchar*)tmpkey);
				if(gtk_notebook_get_current_page (GTK_NOTEBOOK
						(widgets.notebook_comm_book)) 
				   		!= 0)
					gtk_notebook_set_current_page(
			 				GTK_NOTEBOOK (widgets.
							notebook_comm_book),
							0);
			break;
			case DICTIONARY_TYPE:
				main_display_dictionary((gchar*)module,
							(gchar*)key);
			break;
			case BOOK_TYPE:
			case PRAYERLIST_TYPE:
				main_display_book((gchar*)module, (gchar*)key); 
				if(gtk_notebook_get_current_page (GTK_NOTEBOOK 
						(widgets.notebook_comm_book)) 
				   		!= 1)
					gtk_notebook_set_current_page(
							GTK_NOTEBOOK (widgets.
							notebook_comm_book),
							1);
			break;
		}
	}
	settings.addhistoryitem = TRUE;
	return 1;
}


 /******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint (const gchar * type, const gchar * value, 
			gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */ 
 
static gint show_in_previewer(const gchar * url)
{	
	
	gchar **work_buf = NULL;       
	gchar *modbuf_viewer = NULL;
	const gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	guint delay;	
	guint i;	

	work_buf = g_strsplit (url,"/",4);
	//GS_message(("work_buf :%s, %s",work_buf[MODULE],work_buf[KEY]));	
	
	mybuf = main_get_rendered_text(work_buf[MODULE], work_buf[KEY]);
	
	if (mybuf) {
		main_information_viewer(  
				(gchar*)work_buf[MODULE], 
				mybuf, 
				(gchar*)work_buf[KEY], 
				NULL,
				NULL,
				NULL,
				NULL);
		g_free(mybuf);
	}
	g_strfreev(work_buf);
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
		
	if (!clicked) {
		gchar *name = g_strstr_len(url, 10, "://");
		if (!name)
			gui_set_statusbar (url);
			//gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), url);
		else {
			name += 3;		// at name beginning.
			gchar *slash = g_strstr_len(name, 20, "/");
			if (slash)
				*slash = '\0';	// limit to name end.
			int mod_type = backend->module_type(name);
			if (slash)
				*slash = '/';
			
			if (mod_type == DICTIONARY_TYPE)
				show_in_previewer(url);			
			else
				gui_set_statusbar (url);
				//gnome_appbar_set_status (GNOME_APPBAR(widgets.appbar), url);
		}
		return 1;
	}
	
	work_buf = g_strsplit (url,"/",4);
	//if(work_buf[KEY][0] == '/' ) ++work_buf[KEY];
	GS_message(("work_buf: %s, %s",work_buf[MODULE],work_buf[KEY]));
	if (!work_buf[MODULE] && !work_buf[KEY]) {
		alert_url_not_found(url);
		g_strfreev(work_buf);
		return 0;
	}
	if(!work_buf[KEY]) {
		tmpkey = work_buf[MODULE];		
	} else
		tmpkey = work_buf[KEY];
	
/*	mykey = g_convert(tmpkey,
                             -1,
                             OLD_CODESET,
                             UTF_8,
                             &bytes_read,
                             &bytes_written,
                             error);
*/	
	verse_count = 1; //backend->is_Bible_key(mykey, settings.currentverse);
	/*if(!work_buf[3] && !verse_count){
		alert_url_not_found(url);
		g_strfreev(work_buf);
		return 0;
	}*/
	if(backend->is_module(work_buf[MODULE])) {
		mod_type = backend->module_type(work_buf[MODULE]);
		switch(mod_type) {
			case TEXT_TYPE:
				key = main_update_nav_controls(tmpkey);
				main_display_commentary(NULL, key);
				main_display_bible(work_buf[MODULE], key);
				main_keep_bibletext_dialog_in_sync((gchar*)key);
#ifdef USE_GTKHTML38
				editor_sync_with_main();
#endif
				if(key) g_free((gchar*)key);
			break;				
			case COMMENTARY_TYPE:	
				//settings.comm_showing = TRUE;
				key = main_update_nav_controls(tmpkey);
				main_display_commentary(work_buf[MODULE],key);
				main_display_bible(NULL, key);
				main_keep_bibletext_dialog_in_sync((gchar*)key);
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
			main_display_commentary(NULL, key);
			main_display_bible(NULL, key);
			main_keep_bibletext_dialog_in_sync((gchar*)key);
#ifdef USE_GTKHTML38
			editor_sync_with_main();
#endif
			if(key) g_free((gchar*)key);
		} else {
			alert_url_not_found(url);
		}
	} 
	g_strfreev(work_buf);
	settings.addhistoryitem = TRUE;
	return 1;
}


/******************************************************************************
 * Name
 *   main_url_handler
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint main_url_handler(const gchar * url, gboolean clicked)
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
	gchar* stype = NULL;
	gchar* svalue = NULL;
	gchar* module = NULL;
	gchar* passage = NULL;
	gchar* morph = NULL;
	gchar* strongs = NULL;
	gchar *buf = NULL;
	URL* m_url;
	int retval = 0;		// assume failure.

	GS_message(("main_url_handler()"));
	GS_message(("url = %s", url));

	if (strstr(url, "sword://") ||
	    strstr(url, "bible://") ||
	    strstr(url, "book://")  ||
	    strstr(url, "chapter://")) {
		GString *url_clean = hex_decode(url);
		GS_message(("url_clean = %s", url_clean->str));
		
		retval = sword_uri(url_clean->str, clicked);
		g_string_free(url_clean, TRUE);
	}
	else if (strstr(url, "passagestudy.jsp") ||
		 strstr(url, "xiphos.url")) {

		// another minor nightmare: re-encode / and : in hex.
		gchar *place;
		gchar tmpbuf[1023];
		GString *tmpstr = g_string_new(NULL);

		place = (char*)strchr(url, '?');		// url's beginning, as-is.
		strncpy(tmpbuf, url, (++place)-url);
		tmpbuf[place-url] = '\0';
		tmpstr = g_string_append(tmpstr, tmpbuf);
		for (/* */ ; *place; ++place) {
			switch (*place) {
			case '/':
				tmpstr = g_string_append(tmpstr, "%2F"); break;
			case ':':
				tmpstr = g_string_append(tmpstr, "%3A"); break;
			case ' ':
				tmpstr = g_string_append(tmpstr, "%20"); break;
			default:
				tmpstr = g_string_append_c(tmpstr, *place);
			}
		}

		/* passagestudy.jsp?action=showStrongs&type= */
		m_url = new URL((const char*)tmpstr->str);
		action = g_strdup(m_url->getParameterValue("action"));
		morph = g_strdup((gchar*)m_url->getParameterValue("morph"));
		strongs = g_strdup((gchar*)m_url->getParameterValue("lemma"));
		stype = g_strdup((gchar*)m_url->getParameterValue("type"));
		svalue = g_strdup((gchar*)m_url->getParameterValue("value"));
		
		// XXX gross hack-fix
		// AraSVD is named "Smith & Van Dyke", using a literal '&'.
		// this is technically a Sword bug: Sword should encode it.
		// we work around it here: replace '&' with '+'.  *sigh*
		//if (svalue = strstr(, " & "))
		//	*(svalue+1) = '-';
			 
		GS_message(("action = %s", action));
		GS_message(("type = %s", stype));
		GS_message(("value = %s", svalue));
		GS_message(("strongs = %s", strongs));
		GS_message(("morph = %s", morph));

		if (!strcmp(action, "showStrongs")) {
			//stype = g_strdup((gchar*)m_url->getParameterValue("type"));
			//svalue = g_strdup((gchar*)m_url->getParameterValue("value"));
			show_strongs(stype, svalue, clicked);
		}

		else if (!strcmp(action, "showMorph")) {
			//stype = g_strdup((gchar*)m_url->getParameterValue("type"));
			//svalue = g_strdup((gchar*)m_url->getParameterValue("value"));
			show_morph(stype, svalue, clicked);
		}

		else if (!strcmp(action, "showNote")) {
			module = g_strdup(m_url->getParameterValue("module"));
			passage = g_strdup((gchar*)m_url->getParameterValue("passage"));
			show_note(module, passage, stype, svalue, clicked);
			if (module) g_free(module);
			if (passage) g_free(passage);
		}

		else if (!strcmp(action, "showRef")) {
			module = g_strdup(m_url->getParameterValue("module"));
			if (!strcmp(stype, "scripRef"))
				show_ref(module, svalue, clicked);
			if (!strcmp(stype, "swordURL")) {
				// do nothing?
			}
			if (module)
				g_free(module);
		}

		else if (!strcmp(action, "showBookmark")) {
			module = g_strdup(m_url->getParameterValue("module"));
			show_module_and_key(module, svalue, stype, clicked);
			if (module) g_free(module);
		}

		else if (!strcmp(action, "showModInfo")) {
			//svalue = g_strdup((gchar*)m_url->getParameterValue("value"));
			module = g_strdup(m_url->getParameterValue("module"));
			show_mod_info(module, svalue, clicked);
			if (module) g_free(module);
		}

		else if (!strcmp(action, "showParallel")) {
			show_parallel(svalue, stype, clicked);
		}

		else if (!strcmp(action, "showStudypad")) {
			show_studypad(svalue, clicked);
		}

		else if (!strcmp(action, "showImage")) {
			show_separate_image(svalue+5, clicked);	// skip "file:"
		}

		if (action) g_free(action);
		if (stype) g_free(stype);
		if (svalue) g_free(svalue);
		if (strongs) g_free(strongs);
		if (morph) g_free(morph);
		g_string_free(tmpstr, TRUE);
		delete m_url;
		retval = 1;
	} else if (clicked)
		gnome_url_show(url, NULL);

	return retval;
}


/******************************************************************************
 * Name
 *	main_url_handler_gecko
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *	gint main_url_handler_gecko(const gchar * url)
 *
 * Description
 *	
 *
 * Return value
 *	gint
 */
/*
gint main_url_handler_gecko(const gchar * url)
{
	gchar* url_work = g_strdup(url);;
	int retval = 0;		// assume failure.
	
	if (strstr(url, "sword://")) {
		GString *url_clean = hex_decode(url);
		GS_message(("main_url_handler_gecko: url_clean = %s", url_clean->str));
		
		retval = sword_uri(url_clean->str, 0);
		g_string_free(url_clean, TRUE);
		return retval;
	}
	
	if (strstr(url_work, "passagestudy.jsp") ||
	    strstr(url_work, "xiphos.url")) {
		gchar* action = NULL;
		const gchar* stype = NULL;
		gchar* svalue = NULL;
		gchar* module = NULL;
		gchar* passage = NULL;
		gchar **work_buf = NULL;

		// XXX gross hack-fix
		// AraSVD is named "Smith & Van Dyke", using a literal '&'.
		// this is technically a Sword bug: Sword should encode it.
		// we work around it here: replace '&' with '+'.  *sigh*
		if (svalue = strstr(url_work, " & "))
			*(svalue+1) = '-';

		if((action = g_strstr_len(url_work, 40, "action")) == NULL)
			return 0;
		 
		work_buf = g_strsplit (action,"&",6);	
			
		GS_message(("action = %s", work_buf[0]));
		if (strstr(action, "showStrongs")) {
			if (strstr(work_buf[1],"Greek")) 
				stype = "Greek";
			else if (strstr(work_buf[1],"Hebrew")) 
				stype = "Hebrew";
			else
				stype = "";
			svalue = strchr(work_buf[2],'=');
			++svalue;
			GS_message(("type = %s", stype));
			GS_message(("value = %s", svalue));
			show_strongs(stype, svalue, FALSE);
		}
		else if (strstr(action, "showMorph")) {
			stype = strchr(work_buf[1],'=');
			++stype;
			svalue = strchr(work_buf[2],'=');
			++svalue;
			GS_message(("type = %s", stype));
			GS_message(("value = %s", svalue));
			show_morph(stype, svalue, FALSE);
		}
		else if (strstr(action, "showModInfo")) {
			svalue = strchr(work_buf[1],'=');
			++svalue;
			module = strchr(work_buf[2],'=');
			++module;
			GS_message(("module = %s", module));
			GS_message(("svalue = %s", svalue));
			show_mod_info(module, svalue, FALSE);
		}
		else if (strstr(action, "showNote")) {
			stype = strchr(work_buf[1],'=');
			++stype;
			svalue = strchr(work_buf[2],'=');
			++svalue;
			module = strchr(work_buf[3],'=');
			++module;
			passage = strchr(work_buf[4],'=');
			++passage;
			int plen = strlen(passage);
			for (int i = 0; i < plen; i++) {
				if (passage[i] == '+') passage[i] = ' ';
				else if (!strncmp(&(passage[i]), "%2F", 3)) {
					// repair undecoded "%2F" -> "/"
					char *lead, *follow;
					passage[i] = '/';
					for (follow = &(passage[i+1]), lead = follow + 2;
					     *lead;
					     ++follow, ++lead)
					    *follow = *lead;
					*follow = '\0';
					plen -= 2;
				}
			}
			GS_message(("stype = %s", stype));
			GS_message(("svalue = %s", svalue));
			show_note(module, passage, stype, svalue, FALSE);
		}
		else if (strstr(action, "showRef")) {
			stype = strchr(work_buf[1],'=');
			++stype;
			svalue = strchr(work_buf[2],'=');
			++svalue;
			int slen = strlen(svalue);
			for (int i = 0; i < slen; i++) {
				if (svalue[i] == '+') svalue[i] = ' ';
				else if (!strncmp(&(svalue[i]), "%2F", 3)) {
					// repair undecoded "%2F" -> "/"
					char *lead, *follow;
					svalue[i] = '/';
					for (follow = &(svalue[i+1]), lead = follow + 2;
					     *lead;
					     ++follow, ++lead)
					    *follow = *lead;
					*follow = '\0';
					slen -= 2;
				}
			}
			module = strchr(work_buf[3],'=');
			if (module) ++module;
			if (!strcmp(stype, "scripRef"))
				show_ref(module, svalue, FALSE);
		}
		g_strfreev (work_buf);
		g_free(url_work);
	}
}
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

const gchar *main_url_encode(const gchar * pram)
{
	SWBuf retval;
	retval = URL::encode(pram);
/*#ifdef DEBUG
	g_warning(pram);
	g_warning(retval.c_str());
#endif*/
	if (retval.length())
		return g_strdup(retval.c_str());
	else
		return g_strdup("");
}
 
/******************************************************************************
 * Name
 *   hex_decode
 *
 * Synopsis
 *   hex_decode(const gchar *url)
 *
 * Description
 *   extricates `+' and "%xx" encodings.
 *
 * Return value
 *   GString *
 */

GString *
hex_decode(const gchar *url)
{
	// handle `+' space substitutions and %xx encodings.
	GString *url_clean = g_string_new(NULL);
	const gchar *url_chase;
	char hex_template[] = { '0', '0', '\0' };
	unsigned long from_hex;

	for (url_chase = url; *url_chase; ++url_chase) {
		switch (*url_chase) {
		case '+':
			g_string_append_c(url_clean, ' ');
			break;
		case '%':
			if (isxdigit(*(url_chase+1)) &&
			    isxdigit(*(url_chase+2))) {
				hex_template[0] = *(url_chase+1);
				hex_template[1] = *(url_chase+2);
				from_hex = strtol(hex_template, NULL, 16);
				g_string_append_c(url_clean,
						  (gchar) from_hex);
				url_chase += 2;
			} else {
				// failed %xx enconding; normal character.
				// should we instead do nothing with this '%'?
				g_string_append_c(url_clean, '%');
			}
			break;
		default:
			g_string_append_c(url_clean, *url_chase);
			break;
		}
	}
	return url_clean;
}

/******   end of file   ******/
