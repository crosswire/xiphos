/*
 * Xiphos Bible Study Tool
 * menu_popup.c - main window panes and dialogs popup menus
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
#include <config.h>
#endif

#include <gnome.h>
#include <glade/glade-xml.h>
#include <ctype.h>

#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif


#include "gui/menu_popup.h"
#include "gui/cipher_key_dialog.h"
#include "gui/dialog.h"
#include "gui/utilities.h"
#include "gui/widgets.h"
#include "gui/bookmark_dialog.h"
#include "gui/export_dialog.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/about_modules.h"

#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/display.hh"
#include "main/search_sidebar.h"
#include "main/mod_mgr.h"
#include "main/url.hh"

#ifdef USE_GTKHTML3_14_23
#include "editor/slib-editor.h"
#else
#include "editor/bonobo-editor.h"
#endif


static gchar *menu_mod_name = NULL;
static DIALOG_DATA * dialog = NULL;
static int is_dialog = 0;


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

gint  _get_type_mod_list (void)
{
    	switch (main_get_mod_type(menu_mod_name)) {
		case TEXT_TYPE:	
			return TEXT_DESC_LIST;
			break;
		case COMMENTARY_TYPE:	
			return COMM_DESC_LIST;			
			break;
		case DICTIONARY_TYPE:	
			return DICT_DESC_LIST;
			break;	
		case BOOK_TYPE:	
			return GBS_DESC_LIST;
			break;	
		default:
		    	return -1;
			break;
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

gchar * _get_key (gchar * mod_name) 
{
    	gchar *key = NULL;
    
    	if (is_dialog) return g_strdup (dialog->key);
    
	switch (main_get_mod_type(mod_name)) {
		case TEXT_TYPE:
		case COMMENTARY_TYPE:
		case PERCOM_TYPE:
			key = g_strdup (settings.currentverse);			
			break;
		case DICTIONARY_TYPE:
			key = g_strdup (settings.dictkey);
			break;	
		case BOOK_TYPE:
		case PRAYERLIST_TYPE:
			key = g_strdup (settings.book_key);
			break;		
	}
    	return key;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

GtkWidget * _get_html (void)
{
    	if (is_dialog)
		return dialog->html;
    	else
		switch (main_get_mod_type(menu_mod_name)) {
		case TEXT_TYPE:	
			return widgets.html_text;
			break;
		case COMMENTARY_TYPE:
		case PERCOM_TYPE:	
			return widgets.html_comm;			
			break;
		case DICTIONARY_TYPE:	
			return widgets.html_dict;
			break;	
		case BOOK_TYPE:	
		case PRAYERLIST_TYPE:
			return widgets.html_book;
			break;		
		}
    	return NULL;
}


/******************************************************************************
 * Name
 *   _global_option_main_pane
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   void _global_option_main_pane(GtkMenuItem * menuitem,
			   GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static 
void _global_option_main_pane(GtkMenuItem * menuitem, const gchar * option)
{
	gchar *key = _get_key (menu_mod_name);
	gchar *mod = (gchar*) (is_dialog ? dialog->mod_name : menu_mod_name);
    	
    	if (key) {
		gchar *url = g_strdup_printf ("sword://%s/%s",
					     mod,
					     key);
		main_save_module_options (mod, (gchar*) option,
					 GTK_CHECK_MENU_ITEM (menuitem)->active, 
					 is_dialog);
		if (is_dialog) {
			/* show the change */
			main_dialogs_url_handler(dialog, url, TRUE);
		} else {
			/* show the change */
			main_url_handler (url, TRUE);
		}
		
		g_free (url);
		g_free (key);
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

static void on_edit_percomm_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *key = NULL;
    	if (is_dialog)
		key = g_strdup (dialog->key);
    	else
    		key = _get_key (menu_mod_name);
    	if (key ) {
		GS_message(("\n\npercomm key: %s\n\n",key));
		    
		editor_create_new((gchar *)user_data, (gchar *) key, NOTE_EDITOR);
		    
		g_free (key);
	 }
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

static void on_edit_prayerlist_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    	GS_message(("settings.book_key: %s",(char*)(is_dialog ?
						  dialog->key :
						  settings.book_key)));
	editor_create_new((gchar *)user_data,
				  (gchar*) (is_dialog ?
				  dialog->key :
				  settings.book_key),
				  BOOK_EDITOR);
    
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_popup_about_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	gui_display_about_module_dialog((is_dialog ?
					dialog->mod_name :
					menu_mod_name));
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_popup_bookmark_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gchar *key = NULL;
    	
	if (is_dialog) return;
    
	key = _get_key (menu_mod_name);
	
    	if (key) {
		gchar *label = g_strdup_printf("%s, %s",
					       key,
					       menu_mod_name);
		gui_bookmark_dialog(label,
				    menu_mod_name,
				    key);
		g_free (label);	
		g_free (key);
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_popup_export_passage_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{	
    	if (is_dialog) return;
    
	gui_export_dialog();
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_popup_print_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (is_dialog) {
#ifdef USE_GTKMOZEMBED
		gecko_html_print_document (GTK_WINDOW (widgets.app), 
					   dialog->mod_name, 
					   dialog);
	
#else
		gui_html_print (dialog->html, FALSE, dialog->mod_name);
#endif	    
	} else {
#ifdef USE_GTKMOZEMBED
		gecko_html_print_document (GTK_WINDOW (widgets.app), 
					   menu_mod_name, 
					   NULL);
	
#else
		gui_html_print (_get_html(), FALSE, menu_mod_name);
#endif
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_close_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/* FIXME */    
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_popup_copy_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (is_dialog) {
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection (GECKO_HTML (dialog->html));
#else
	gui_copy_html (dialog->html);
#endif
	} else {
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection (GECKO_HTML (_get_html ()));
#else
	gui_copy_html (_get_html ());
#endif	    
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_popup_find_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    	if (is_dialog)
		gui_find_dlg (dialog->html, dialog->mod_name, FALSE, NULL);
    	else
		gui_find_dlg (_get_html (), menu_mod_name, FALSE, NULL);    
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_popup_font_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	if (is_dialog) {
		gchar *url = g_strdup_printf("sword://%s/%s",
					     dialog->mod_name,
					     dialog->key);
		gui_set_module_font(dialog->mod_name);
		/* show the change */
		main_dialogs_url_handler(dialog, url, TRUE);
		g_free(url);
	} else {
	    	gchar *key = _get_key (menu_mod_name);
	    	if (key) {
			gchar *url = g_strdup_printf (	"sword://%s/%s",
							menu_mod_name,
							key);
			gui_set_module_font (menu_mod_name);
			/* show the change */
			main_url_handler (url, TRUE);		
			g_free (url); 		
			g_free (key); 
		 }
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_words_of_christ_in_red_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
   	_global_option_main_pane((GtkMenuItem*)menuitem, 
					 "Words of Christ in Red"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_strong_s_numbers_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Strong's Numbers"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_morphological_tags_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Morphological Tags"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_footnotes_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Footnotes"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_greek_accents_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Greek Accents"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_lemmas_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Lemmas"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_scripture_cross_references_activate (GtkCheckMenuItem * menuitem,
                                        	gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Scripture Cross-references"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_hebrew_vowel_points_activate (GtkCheckMenuItem * menuitem,
                                        gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Hebrew Vowel Points"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_hebrew_cantillation_activate (GtkCheckMenuItem * menuitem,
                                        gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Hebrew Cantillation"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_headings_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Headings"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_transliteration_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Transliteration"); /* string not seen by user */
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_commentary_by_chapter_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
   	_global_option_main_pane((GtkMenuItem*)menuitem, "Commentary by Chapter"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_primary_reading_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	gchar *key = NULL;
    
	if (is_dialog) {
		reading_selector(dialog->mod_name,
				 dialog->key,
				 dialog,
				 (GtkMenuItem*)menuitem,
				 GINT_TO_POINTER(0));
	} else {
		key = _get_key(menu_mod_name);
		if (key) {
			reading_selector(menu_mod_name,
					 key,
					 NULL,
					 (GtkMenuItem*)menuitem,
					 GINT_TO_POINTER(0));	
			g_free (key);
		}
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_secondary_reading_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{	
    	gchar *key = NULL;
    
	if (is_dialog) {
		reading_selector(dialog->mod_name,
				 dialog->key,
				 dialog,
				 (GtkMenuItem*)menuitem,
				 GINT_TO_POINTER(1));
	} else {
		key = _get_key(menu_mod_name);
		if (key) {
			reading_selector(menu_mod_name,
					 key,
					 NULL,
					 (GtkMenuItem*)menuitem,
					 GINT_TO_POINTER(1));	
			g_free (key);
		}
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_all_readings_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{	
    	gchar *key = NULL;
    
	if (is_dialog) {
		reading_selector(dialog->mod_name,
				 dialog->key,
				 dialog,
				 (GtkMenuItem*)menuitem,
				 GINT_TO_POINTER(2));
	} else {
		key = _get_key(menu_mod_name);
		if (key) {
			reading_selector(menu_mod_name,
					 key,
					 NULL,
					 (GtkMenuItem*)menuitem,
					 GINT_TO_POINTER(2));	
			g_free (key);
		}
	}

}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_image_content_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Image Content"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_respect_font_faces_activate (GtkCheckMenuItem * menuitem, gpointer user_data)
{
	_global_option_main_pane((GtkMenuItem*)menuitem, "Respect Font Faces"); /* string not seen by user */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_unlock_this_module_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *cipher_key;
	gchar *cipher_old;
    	gchar *key = NULL;
    
    	if (is_dialog) return;

	cipher_old = main_get_mod_config_entry (menu_mod_name, "CipherKey");
	cipher_key = gui_add_cipher_key (menu_mod_name, cipher_old);
	if (cipher_key) {
	    	key = _get_key (menu_mod_name);
	    	if (key) {
			main_display_bible (menu_mod_name, key);
			g_free (key);
		}
		g_free (cipher_key);
	}
	if (cipher_old)
		g_free (cipher_old);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_display_book_heading_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	if (is_dialog)
		main_dialogs_book_heading (dialog);
	else
		main_book_heading (menu_mod_name);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_display_chapter_heading_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	if (is_dialog)
		main_dialogs_chapter_heading (dialog);
	else
		main_chapter_heading (menu_mod_name);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_use_current_dictionary_activate (GtkMenuItem * menuitem, gpointer user_data)
{			
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(_get_html ()));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);
#else
	gchar *dict_key = gui_get_word_or_selection(_get_html (), FALSE);
	if (dict_key) {
		gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), dict_key);
		gtk_widget_activate(widgets.entry_dict);
		g_free(dict_key);
	}
#endif
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_rename_perscomm_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    	if (is_dialog) return;
    
#if defined(__CYGWIN__) || defined(WIN32)
	gui_generic_warning(_("Renaming is not available in Windows.\n\n"
			      "Xiphos is limited by Windows' filesystem,\n"
			      "because it disallows the renaming of filename\n"
			      "components of currently-open files,\n"
			      "such as the contents of this commentary.\n"
			      "Therefore, personal commentary renaming is\n"
			      "not available in the Windows environment."));
#else
	GS_DIALOG *info;
	GString *workstr;
	char *s;
	char *datapath_old, *datapath_new;
	const char *conf_old;
	char *conf_new;
	char *sworddir, *modsdir;
	FILE *result;

	// get a new name for the module.
	info = gui_new_dialog();
	info->title = _("Rename Commentary");
	workstr = g_string_new("");
	g_string_printf(workstr, "<span weight=\"bold\">%s</span>",
			_("Choose Commentary Name"));
	info->label_top = workstr->str;
	info->text1 = g_strdup(_("New Name"));
	info->label1 = N_("Name: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	if (gui_gs_dialog(info) != GS_OK)
		goto out1;

	for (s = info->text1; *s; ++s) {
		if (!isalnum(*s) && (*s != '_')) {
			gui_generic_warning
			    (_("Module names must contain [A-Za-z0-9_] only."));
			goto out1;
		}
	}

	if (main_is_module(info->text1)) {
		gui_generic_warning
		    (_("Xiphos already knows a module by that name."));
		goto out1;
	}

	sworddir = g_new(char, strlen(settings.homedir) + 8);
	sprintf(sworddir, "%s/" DOTSWORD, settings.homedir);
	modsdir  = g_new(char, strlen(sworddir) + 8);
	sprintf(modsdir,  "%s/mods.d", sworddir);

	conf_old =
	    main_get_mod_config_file(settings.CommWindowModule, sworddir);

	conf_new = g_strdup(info->text1);	// dirname is lowercase.
	for (s = conf_new; *s; ++s)
		if (isupper(*s))
			*s = tolower(*s);

	datapath_old =
	    main_get_mod_config_entry(settings.CommWindowModule, "DataPath");
	datapath_new = g_strdup(datapath_old);
	if ((s = strstr(datapath_new, "rawfiles/")) == NULL) {
	    gui_generic_warning("Malformed datapath in old configuration!");
	    goto out2;
	}

	*(s+9) = '\0';		// skip past "rawfiles/".
	s = g_strdup_printf("%s%s", datapath_new, conf_new);
	g_free(datapath_new);	// out with the old...
	datapath_new = s;	// ..and in with the new.

	// move old data directory to new.
	if ((chdir(sworddir) != 0) ||
	    (rename(datapath_old, datapath_new) != 0)) {
		gui_generic_warning("Failed to rename directory.");
		goto out2;
	}

	// manufacture new .conf from old.
	g_string_printf(workstr,
			"( cd \"%s\" && sed -e '/^\\[/s|^.*$|[%s]|' -e '/^DataPath=/s|rawfiles/.*$|rawfiles/%s/|' < \"%s\" > \"%s.conf\" ) 2>&1",
			modsdir, info->text1, conf_new, conf_old, conf_new);
	if ((result = popen(workstr->str, "r")) == NULL) {
		g_string_printf(workstr,
				_("Failed to create new configuration:\n%s"),
				strerror(errno));
		gui_generic_warning(workstr->str);
		goto out2;
	} else {
		gchar output[258];
		if (fgets(output, 256, result) != NULL) {
			g_string_truncate(workstr, 0);
			g_string_append(workstr,
					_("Configuration build error:\n\n"));
			g_string_append(workstr, output);
			gui_generic_warning(workstr->str);
			goto out2;	// necessary?  advisable?
		}
		pclose(result);
	}

	// unlink old conf.
	g_string_printf(workstr, "%s/%s", modsdir, conf_old);
	if (unlink(workstr->str) != 0) {
		g_string_printf(workstr,
				"Unlink of old configuration failed:\n%s",
				strerror(errno));
		gui_generic_warning(workstr->str);
		goto out2;
	}
	main_update_module_lists();
	settings.CommWindowModule = g_strdup(info->text1);
	main_display_commentary(info->text1, settings.currentverse);

out2:
	g_free(conf_new);
	g_free((char*)conf_old);
	g_free(datapath_old);
	g_free(datapath_new);
	g_free(modsdir);
	g_free(sworddir);
out1:
	g_free(info->text1);
	g_free(info);
	g_string_free(workstr, TRUE);
#endif /* !__CYGWIN__ */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_dump_perscomm_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	main_sidebar_perscomm_dump ();
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   
 *
 * Description
 *
 *
 * Return value
 *   
 */

G_MODULE_EXPORT void on_read_selection_aloud_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *dict_key;
	int len;
	GtkWidget *html_widget = _get_html ();
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(html_widget));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	dict_key =
		g_strdup(gtk_editable_get_chars(
			(GtkEditable *)widgets.entry_dict,0,-1));
	len = (dict_key ? strlen(dict_key) : 0);
#else
	GtkHTML *html = GTK_HTML(html_widget);
	dict_key = gtk_html_get_selection_html(html, &len);
#endif /* !USE_GTKMOZEMBED */

	if (dict_key && len && *dict_key) {
		ReadAloud(0, dict_key);
		g_free(dict_key);
	} else
		gui_generic_warning("No selection made");
}

/******************************************************************************
 * Name
 *   _add_global_opts
 *
 * Synopsis
 *   gui/menu_popup.h
 *
 *   GtkWidget * _add_global_opts(const gcahr * module_name)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget*
 */

G_MODULE_EXPORT void _add_and_check_global_opts (GladeXML *gxml,
				 const gchar * mod_name, 
				 GtkWidget * submenu, 
				 DIALOG_DATA * d)
{
	GtkWidget * item;    
	GLOBAL_OPS *ops = NULL;
    
	if (is_dialog) 
		ops = main_new_globals((gchar*) mod_name, 1);
	else
		ops = main_new_globals((gchar*) mod_name, 0);
    
    
    	item = glade_xml_get_widget (gxml, "words_of_christ_in_red");
    	gtk_widget_hide (item);
    
	if ((main_check_for_global_option ((gchar*) mod_name, "GBFRedLetterWords")) ||
	    (main_check_for_global_option ((gchar*) mod_name, "OSISRedLetterWords"))) {		
		gtk_widget_show(item);
		GTK_CHECK_MENU_ITEM(item)->active = ops->words_in_red;
	}
    
    	item = glade_xml_get_widget (gxml, "strong's_numbers");
    	gtk_widget_hide (item);
    
	if ((main_check_for_global_option((gchar*) mod_name, "GBFStrongs")) ||
	    (main_check_for_global_option((gchar*) mod_name, "ThMLStrongs")) ||
	    (main_check_for_global_option((gchar*) mod_name, "OSISStrongs"))) {		
		gtk_widget_show(item);
		GTK_CHECK_MENU_ITEM(item)->active = ops->strongs;
	}
    
    	item = glade_xml_get_widget (gxml, "morphological_tags");
    	gtk_widget_hide (item);
    
	if (main_check_for_global_option((gchar*) mod_name, "GBFMorph") ||
	    main_check_for_global_option((gchar*) mod_name, "ThMLMorph") ||
	    main_check_for_global_option((gchar*) mod_name, "OSISMorph")) {		
		gtk_widget_show(item);
		GTK_CHECK_MENU_ITEM(item)->active = ops->morphs;
	}
    
    	item = glade_xml_get_widget (gxml, "footnotes");
    	gtk_widget_hide (item);
    
	if (main_check_for_global_option((gchar*) mod_name, "GBFFootnotes") ||
	    main_check_for_global_option((gchar*) mod_name, "ThMLFootnotes") ||
	    main_check_for_global_option((gchar*) mod_name, "OSISFootnotes")) {		
		gtk_widget_show(item);
		GTK_CHECK_MENU_ITEM(item)->active = ops->footnotes;
	}
    
    	item = glade_xml_get_widget (gxml, "greek_accents");
    	gtk_widget_hide (item);
    
	if (main_check_for_global_option((gchar*) mod_name, "UTF8GreekAccents")) {		
		gtk_widget_show(item);
		GTK_CHECK_MENU_ITEM(item)->active = ops->greekaccents;
	}
    
    	item = glade_xml_get_widget (gxml, "lemmas");
    	gtk_widget_hide (item);
    
	if (main_check_for_global_option((gchar*) mod_name, "ThMLLemma") ||
	    main_check_for_global_option((gchar*) mod_name, "OSISLemma")) {		
		gtk_widget_show(item);
		GTK_CHECK_MENU_ITEM(item)->active = ops->lemmas;
	}
    
    	item = glade_xml_get_widget (gxml, "scripture_cross-references");
    	gtk_widget_hide (item);
    
	if (main_check_for_global_option((gchar*) mod_name, "ThMLScripref") ||
	    main_check_for_global_option((gchar*) mod_name, "OSISScripref")) {		
		gtk_widget_show(item);
		GTK_CHECK_MENU_ITEM(item)->active = ops->scripturerefs;
	}
    
    	item = glade_xml_get_widget (gxml, "hebrew_vowel_points");
    	gtk_widget_hide (item);
    
	if (main_check_for_global_option((gchar*) mod_name, "UTF8HebrewPoints")) {		
		gtk_widget_show(item);
		GTK_CHECK_MENU_ITEM(item)->active = ops->hebrewpoints;
	}
    
    	item = glade_xml_get_widget (gxml, "hebrew_cantillation");
    	gtk_widget_hide (item);
    
	if (main_check_for_global_option ((gchar*) mod_name, "UTF8Cantillation")) {		
		gtk_widget_show (item);
		GTK_CHECK_MENU_ITEM (item)->active = ops->hebrewcant;
	}
    
    	item = glade_xml_get_widget (gxml, "headings");
    	gtk_widget_hide (item);
    
	if (main_check_for_global_option ((gchar*) mod_name, "ThMLHeadings") ||
	    main_check_for_global_option ((gchar*) mod_name, "OSISHeadings")) {		
		gtk_widget_show (item);
		GTK_CHECK_MENU_ITEM (item)->active = ops->headings;
	} 
    
    	item = glade_xml_get_widget (gxml, "transliteration");
	gtk_widget_hide (item);
#ifndef WIN32	   
	gtk_widget_show (item);	
	GTK_CHECK_MENU_ITEM (item)->active = ops->transliteration;
#endif 
    	item = glade_xml_get_widget (gxml, "variants");
    	gtk_widget_hide (item);
    
	if (main_check_for_global_option ((gchar*) mod_name, "ThMLVariants")) {		
		gtk_widget_show (item);
		item = glade_xml_get_widget (gxml, "primary_reading");
			GTK_CHECK_MENU_ITEM(item)->active = ops->variants_primary;
		item = glade_xml_get_widget (gxml, "secondary_reading");
			GTK_CHECK_MENU_ITEM(item)->active = ops->variants_secondary;
		item = glade_xml_get_widget (gxml, "all_readings");
			GTK_CHECK_MENU_ITEM (item)->active = ops->variants_all;
	}
    
    	item = glade_xml_get_widget (gxml, "image_content");
    	gtk_widget_hide (item);
    
	if (ops->image_content != -1) {		
		gtk_widget_show (item);
		GTK_CHECK_MENU_ITEM (item)->active = ops->image_content;
	}
    
    	item = glade_xml_get_widget (gxml, "respect_font_faces");
    	gtk_widget_hide (item);
    
	if (ops->respect_font_faces != -1) {		
		gtk_widget_show(item);
		GTK_CHECK_MENU_ITEM (item)->active = ops->respect_font_faces;
	}
    
    	item = glade_xml_get_widget (gxml, "commentary_by_chapter");
    	gtk_widget_hide (item);
    
    	if (main_get_mod_type((gchar*) (is_dialog ? d->mod_name : mod_name)) == COMMENTARY_TYPE) {
    		gtk_widget_show (item);
   		GTK_CHECK_MENU_ITEM (item)->active = ops->commentary_by_chapter;  
	}
    
	g_free(ops);
    
}

/******************************************************************************
 * Name
 *  on_view_mod_activate
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   void on_view_mod_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   show a different module
 *
 * Return value
 *   void
 */

static void on_view_mod_activate(GtkMenuItem *menuitem,
				 gpointer user_data)
{
	if (is_dialog) return;
    
    	gchar *module_name = main_module_name_from_description ((gchar *)user_data);
	gchar *url = NULL;
    	gchar *key = NULL;
    
    	key = _get_key (menu_mod_name);
	if (module_name && key) {
		url = g_strdup_printf ("sword://%s/%s",
					module_name,
					key);
		main_url_handler (url, TRUE);
		g_free (url);
		g_free (module_name);
		g_free (key);
	}
}


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
 *   lookup selection in a dict/lex module
 *
 * Return value
 *   void
 */

static void _lookup_selection(GtkMenuItem *menuitem,
				       gchar *dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;
	GtkWidget *html = _get_html ();
    	if (!html) return;
	mod_name = main_module_name_from_description (dict_mod_description);
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection (GECKO_HTML(html));
	gtk_editable_select_region ((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard ((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate (widgets.entry_dict);
	dict_key =
		g_strdup(gtk_editable_get_chars (
			(GtkEditable *)widgets.entry_dict,0,-1));

#else
	dict_key = gui_get_word_or_selection (html, FALSE);
#endif
	if (dict_key && mod_name) {
		main_display_dictionary (mod_name, dict_key);
	}
	if (dict_key)
		g_free (dict_key);
	if (mod_name)
		g_free (mod_name);
}


/******************************************************************************
 * Name
 *   _create_popup_menu
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   GtkWidget * _create_popup_menu ( const gchar * mod_name, DIALOG_DATA * d)
 *
 * Description
 *   creates the needed popup and returns it to gui_menu_popup ()
 *
 * Return value
 *   GtkWidget*
 */

static 
GtkWidget * _create_popup_menu ( const gchar * mod_name, DIALOG_DATA * d)
{
	gchar *glade_file;
	GladeXML *gxml;
    	const gchar *mname = NULL;
	
	glade_file = gui_general_user_file ("xi-menus.glade", FALSE);
	g_return_val_if_fail ((glade_file != NULL), NULL);
	
	gxml = glade_xml_new (glade_file, "menu_popup", NULL);
		
	g_free (glade_file);
	g_return_val_if_fail ((gxml != NULL), NULL);
	
	GtkWidget *menu 	= glade_xml_get_widget (gxml, "menu_popup");
    
	GtkWidget *bookmark	= glade_xml_get_widget (gxml, "bookmark");
	GtkWidget *open 	= glade_xml_get_widget (gxml, "open_module2"); /*  */
	GtkWidget *export_	= glade_xml_get_widget (gxml, "export_passage");
	GtkWidget *close 	= glade_xml_get_widget (gxml, "close");
	GtkWidget *note 	= glade_xml_get_widget (gxml, "note"); /*  */
	GtkWidget *open_edit	= glade_xml_get_widget (gxml, "open_in_editor");
	
    	GtkWidget *mod_opt_sub 	= GTK_WIDGET (glade_xml_get_widget (gxml, "module_options1_menu")); /*  */
	
	GtkWidget *lookup 	= glade_xml_get_widget (gxml, "lookup_selection1"); /*  */
	GtkWidget *lookup_sub 	= glade_xml_get_widget (gxml, "lookup_selection1_menu");
	GtkWidget *unlock 	= glade_xml_get_widget (gxml, "unlock_this_module"); 
	GtkWidget *book_heading = glade_xml_get_widget (gxml, "display_book_heading");
	GtkWidget *chapter_heading = glade_xml_get_widget (gxml, "display_chapter_heading");
	GtkWidget *rename_percomm = glade_xml_get_widget (gxml, "rename_perscomm");
    	GtkWidget *dump_percomm = glade_xml_get_widget (gxml, "dump_perscomm");
	
#if defined(__CYGWIN__)
	GtkWidget *read_aloud	= glade_xml_get_widget (gxml, "read_selection_aloud");
#endif /* __CYGWIN__ */	 
#if 0
	GtkWidget *print 	= glade_xml_get_widget (gxml, "print");
#endif

    	GtkWidget *open_sub 	= gtk_menu_new ();
	GtkWidget *note_sub 	= gtk_menu_new ();
	
	gtk_widget_hide (unlock);
	gtk_widget_hide (book_heading);
	gtk_widget_hide (export_);
	gtk_widget_hide (chapter_heading);
	gtk_widget_hide (rename_percomm);
	gtk_widget_hide (dump_percomm);
	gtk_widget_hide (note);
   	gtk_widget_hide (open_edit);
	gtk_widget_hide (close); /* FIXME: hide until connected to dialog close */
    
    	if (is_dialog) {
		mname = dialog->mod_name;
	    	gtk_widget_hide (open);
	    	gtk_widget_hide (bookmark);
	    	gtk_widget_hide (export_);
	} else {
		mname = mod_name;
	    	gtk_widget_hide (close);	    
    		gtk_menu_item_set_submenu (GTK_MENU_ITEM(open),
				  	open_sub);
    		gui_add_mods_2_gtk_menu (_get_type_mod_list (), open_sub,
				(GCallback) on_view_mod_activate);
	    
	    	if (main_has_cipher_tag((gchar*) mod_name)) /* only unlock from main window */
			gtk_widget_show(unlock);
	}
    
    	switch (main_get_mod_type((gchar*) mname)) {
	case TEXT_TYPE:	
		gtk_widget_show (export_);
		if (is_dialog ) break;
		gtk_widget_show (note);		
		gtk_menu_item_set_submenu (GTK_MENU_ITEM(note), note_sub);		    
		gui_add_mods_2_gtk_menu (PERCOMM_LIST, note_sub,
				(GCallback) on_edit_percomm_activate);
		break;
	case COMMENTARY_TYPE:	
		gtk_widget_show (book_heading);
		gtk_widget_show (chapter_heading);			
		break;
	case PERCOM_TYPE:
   		gtk_widget_show (open_edit);
		g_signal_connect (GTK_OBJECT(open_edit),
			 	"activate",
			 	G_CALLBACK (on_edit_percomm_activate),
			 	(gchar*) (is_dialog ? d->mod_name : mod_name));	
		
		gtk_widget_show (rename_percomm); 
		gtk_widget_show (dump_percomm);   
		break;
	case DICTIONARY_TYPE:	
		
		break;	
	case BOOK_TYPE:	
		
		break;	
	case PRAYERLIST_TYPE:
   		gtk_widget_show (open_edit);
		g_signal_connect (GTK_OBJECT(open_edit),
			 	"activate",
			 	G_CALLBACK (on_edit_prayerlist_activate),
			 	(gchar*) (is_dialog ? d->mod_name : mod_name));	
		
		break;
	}	
	
#if defined(__CYGWIN__)
	gtk_widget_hide(read_aloud);	// read aloud
#endif /* __CYGWIN__ */	 
    
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lookup),
				  lookup_sub);
    
    	gui_add_mods_2_gtk_menu (DICT_DESC_LIST, lookup_sub,
				(GCallback) _lookup_selection);

	/* = glade_xml_get_widget (gxml, ""); */
	_add_and_check_global_opts (gxml,
				    (char*)(is_dialog ? d->mod_name : mod_name), 
				    mod_opt_sub, 
				    d);
    	/* connect signals and data */
	glade_xml_signal_autoconnect_full
		(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func, NULL);

	return menu;
}

/******************************************************************************
 * Name
 *   gui_menu_popup
 *
 * Synopsis
 *   #include "gui/menu_popup.h"
 *
 *   void gui_menu_popup (const gchar * mod_name, DIALOG_DATA * d)
 *
 * Description
 *   determens if a main window or dialog popup is needed
 *   and calls _create_popup_menu() to create the menu and
 *   then pops the menu up
 *
 * Return value
 *   void
 */

void gui_menu_popup (const gchar * mod_name, DIALOG_DATA * d)
{
	GtkWidget *menu;
    
	if (d) {
		dialog = d;
		menu_mod_name = NULL;
	    	is_dialog = TRUE;
	} else if (mod_name) {
		menu_mod_name = (gchar*) mod_name;
		dialog = NULL;
	    	is_dialog = FALSE;
	} else
		return;
    
	menu = _create_popup_menu (mod_name, d);
	gtk_menu_popup ((GtkMenu*)menu, NULL, NULL, NULL, NULL, 2,
		     			gtk_get_current_event_time());
	
}
