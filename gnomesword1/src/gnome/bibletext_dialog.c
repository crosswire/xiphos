/*
 * GnomeSword Bible Study Tool
 * bibletext_dialog.c - view Bible text module in a dialog
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
#include <gtkhtml/gtkhtml.h>
#include <gal/widgets/e-unicode.h>

#include "gui/gtkhtml_display.h"
#include "gui/bibletext_dialog.h"
#include "gui/display_info.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/html.h"
#include "gui/gnomesword.h"
#include "gui/utilities.h"

#include "main/bibletext.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"


/******************************************************************************
 * static - global to this file only
 */
static GList * dialog_list;
static VIEW_TEXT * cur_vt;
static gboolean dialog_freed;

/******************************************************************************
 * externs
 *****************************************************************************/
extern gboolean gsI_isrunning; /* information dialog */



/******************************************************************************
 * Name
 *   show_in_statusbar
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *   void show_in_statusbar(GtkWidget * statusbar, gchar * key, 
 *							gchar * mod)
 *
 * Description
 *   display information (morph or strongs) in statusbar
 *
 * Return value
 *   void
 */

static void show_in_statusbar(GtkWidget * statusbar, gchar * key, 
							gchar * mod)
{
	gchar *str;
	gchar *text;
	gint context_id2;
	
	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
					 settings.program_title);
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);
	
	text = get_striptext(mod, key);
	str = remove_linefeeds(text);
	if(str) {
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2,
					str);
		g_free(str);
	}
	g_free(text);
}


/******************************************************************************
 * Name
 *   display
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void display(VIEW_TEXT * vt, gchar * key, gboolean show_key)	
 *
 * Description
 *   calls chapter_display to display module text
 *
 * Return value
 *   void
 */

static void display(VIEW_TEXT * vt, gchar * key, gboolean show_key)
{
	chapter_display(vt->t->html, 
			vt->t->mod_name, 
			vt->t->tgs, 
			key, 
			show_key);
}


/******************************************************************************
 * Name
 *   update_controls
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void update_controls(VIEW_TEXT * vt)	
 *
 * Description
 *   update the book, chapter and verse contorls
 *
 * Return value
 *   void
 */

static void update_controls(VIEW_TEXT * vt)
{
	const gchar *buf;
	cur_vt = vt;
	
	buf = get_book_from_key(vt->key);
	gtk_entry_set_text(GTK_ENTRY(vt->cbe_book), buf);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(vt->spb_chapter),
				  get_chapter_from_key(vt->key));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(vt->spb_verse),
				  get_verse_from_key(vt->key));
}

/******************************************************************************
 * Name
 *   link_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void link_clicked(GtkHTML * html, const gchar * url, 
 *							gpointer data)	
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

static void link_clicked(GtkHTML * html, const gchar * url, 
						VIEW_TEXT * vt)
{
	gchar *buf, *modName;
	gchar newref[80];
	static GtkWidget *dlg;
	cur_vt = vt;
	
	if (*url == '#') {

		if (!gsI_isrunning) {
			dlg = gui_create_display_informtion_dialog();
		}
		++url;		/* remove # */

		if (*url == 'T')
			++url;

		if (*url == 'G') {
			++url;
			modName = g_strdup(settings.lex_greek);
			buf = g_strdup(url);
			gui_display_mod_and_key(modName, buf);
			g_free(buf);
			g_free(modName);
		}

		if (*url == 'H') {
			++url;
			modName = g_strdup(settings.lex_hebrew);
			buf = g_strdup(url);
			gui_display_mod_and_key(modName, buf);
			g_free(buf);
			g_free(modName);
		}
		gtk_widget_show(dlg);
	}

	else if (*url == 'M') {
		if (!gsI_isrunning) {
			dlg = gui_create_display_informtion_dialog();
		}
		++url;		/* remove M */
		buf = g_strdup(url);
		gui_display_mod_and_key("Packard", buf);
		g_free(buf);
		gtk_widget_show(dlg);
	}

	else if (*url == '*') {
		++url;
//		g_warning(url);
		buf = g_strdup(url);
		display(vt, buf, TRUE);
		update_controls(vt);
		g_free(buf);
	}

	else if (!strncmp(url, "passage=", 7)) {/*** remove passage= verse list ***/
		gchar *mybuf = NULL;
		mybuf = strchr(url, '=');
		++mybuf;
		buf = g_strdup(mybuf);
		gui_display_verse_list(settings.MainWindowModule, buf);
		g_free(buf);

	}

	else if (!strncmp(url, "type=morph", 10)) {
		gchar *modbuf = NULL;
		gchar *mybuf = NULL;
		buf = g_strdup(url);
//		g_warning("mybuf = %s", url);
		mybuf = strstr(url, "class=");
		if (mybuf) {
			gint i;
			modbuf = strchr(mybuf, '=');
			++modbuf;
			for (i = 0; i < strlen(modbuf); i++) {
				if (modbuf[i] == ' ') {
					modbuf[i] = '\0';
					break;
				}
			}
		}


		mybuf = NULL;
		mybuf = strstr(buf, "value=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			buf = g_strdup(mybuf);
			if (!gsI_isrunning) {
				dlg = gui_create_display_informtion_dialog();
			}
			gui_display_mod_and_key(modbuf, buf);
		}
		g_free(buf);
		gtk_widget_show(dlg);
	}
	/*** thml strongs ***/
	else if (!strncmp(url, "type=Strongs", 12)) {
		gchar *modbuf = NULL;
		gchar *mybuf = NULL;
		gint type = 0;
		//buf = g_strdup(url);
		mybuf = NULL;
		mybuf = strstr(url, "value=");
		//i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			if (mybuf[0] == 'H')
				type = 0;
			if (mybuf[0] == 'G')
				type = 1;
			++mybuf;
			sprintf(newref, "%5.5d", atoi(mybuf));
		
			if (type)
				modbuf = settings.lex_greek;
			else
				modbuf = settings.lex_hebrew;
	
			if (!gsI_isrunning) {
				dlg = gui_create_display_informtion_dialog();
			}
			buf = g_strdup(newref);
			gui_display_mod_and_key(modbuf, buf);
			g_free(buf);
			gtk_widget_show(dlg);
		}

	}
}

/******************************************************************************
 * Name
 *   cbe_book_changed
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void cbe_book_changed(GtkEditable * editable,
 *					       gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void cbe_book_changed(GtkEditable * editable,
					       VIEW_TEXT * vt)
{

}

/******************************************************************************
 * Name
 *   btn_goto_verse_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void btn_goto_verse_clicked(GtkButton * button,
 *						gpointer user_data)
 *
 * Description
 *   change verse
 *
 * Return value
 *   void
 */

static void btn_goto_verse_clicked(GtkButton * button,
						VIEW_TEXT * vt)
{
	gchar *bookname, buf[120], *val_key;
	gint iChap, iVerse;
	cur_vt = vt;
	
	bookname = gtk_entry_get_text(GTK_ENTRY(vt->cbe_book));
	iChap =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (vt->spb_chapter));
	iVerse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (vt->spb_verse));
	sprintf(buf, "%s %d:%d", bookname, iChap, iVerse);
	val_key = get_valid_key(buf);
	//g_warning(buf);
	display(vt, val_key, TRUE);
	strcpy(vt->key,val_key);
	update_controls(vt);
	g_free(val_key);
}

/******************************************************************************
 * Name
 *   cbe_module_changed
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void (cbe_module_changedGtkEditable * editable, 
 *						gpointer user_data)	
 *
 * Description
 *   change to new module
 *
 * Return value
 *   void
 */

static void module_new_activate(GtkMenuItem * menuitem, 
					    gpointer user_data)
{	
	gui_open_bibletext_dialog((gchar *)user_data);
}

/******************************************************************************
 * Name
 *   btn_sync_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void btn_sync_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *   set text to main window current verse
 *
 * Return value
 *   void
 */

static void btn_sync_clicked(GtkButton * button, VIEW_TEXT * vt)
{
	cur_vt = vt;
	display(vt, settings.currentverse, TRUE);	
	strcpy(vt->key,settings.currentverse);
	update_controls(vt);
}


/******************************************************************************
 * Name
 *   free_on_destroy
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void free_on_destroy(VIEW_TEXT * vt)
 *
 * Description
 *   removes dialog from dialog_list when dialog is destroyed other than
 *   program shut down
 *
 * Return value
 *   void
 */

static void free_on_destroy(VIEW_TEXT * vt)
{	
	g_free(vt->t->tgs);
	g_free(vt->t);
	dialog_list = g_list_remove(dialog_list, (VIEW_TEXT *) vt);
//		g_warning("shuting down %s dialog",vt->t->mod_name);
	g_free(vt);
}


/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void dialog_destroy(GtkObject *object, DL_DIALOG * dlg)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void dialog_destroy(GtkObject *object, VIEW_TEXT * vt)
{	
	
	if(!dialog_freed)
		free_on_destroy(vt);
	dialog_freed = FALSE;
}


/******************************************************************************
 * Name
 *   btn_close_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void btn_close_clicked(GtkButton * button,VIEW_TEXT * vt)	
 *
 * Description
 *   call gtk_widget_destroy to destroy the bibletext dialog
 *   set vt->dialog to NULL
 *
 * Return value
 *   void
 */

static void btn_close_clicked(GtkButton * button, VIEW_TEXT * vt)
{
	if(vt->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(vt->dialog);
	}
}

/******************************************************************************
 * Name
 *   add_items_to_module_menu
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void add_items_to_module_menu(GtkWidget * shellmenu)	
 *
 * Description
 *   add bibletext module names to menu
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

	tmp = get_list(TEXT_LIST);
	while (tmp != NULL) {

		/* add module name items to menu */
		menuChoice =
		    gtk_check_menu_item_new_with_label((gchar *) 
				(gchar *)tmp->data);
		sprintf(menuName, "ModuleNum%d", view_number);
		gtk_object_set_data(GTK_OBJECT(widgets.app), menuName,
				    menuChoice);
		gtk_widget_show(menuChoice);
		gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
				   GTK_SIGNAL_FUNC(module_new_activate),
				   (gchar *)tmp->data);
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
				      GTK_WIDGET(menuChoice), view_number);
		++view_number;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}

static gboolean on_dialog_motion_notify_event(GtkWidget *widget,
                        GdkEventMotion  *event, VIEW_TEXT * vt)
{
	cur_vt = vt;
	return TRUE;
}


/******************************************************************************
 * Name
 *  morphs_toggled
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void morphs_toggled(GtkToggleButton *togglebutton, gchar *option)	
 *
 * Description
 *   called when Morphological Tags button is press
 *
 * Return value
 *   void
 */

static void morphs_toggled(GtkToggleButton * togglebutton,
						VIEW_TEXT * vt)
{
	cur_vt = vt;
	set_text_module_global_option("Morphological Tags",
				togglebutton->active);
	display(vt, vt->key, TRUE);
}


/******************************************************************************
 * Name
 *  strongs_toggled
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void strongs_toggled(GtkToggleButton *togglebutton, VIEW_TEXT * vt)	
 *
 * Description
 *   called when Strong's Numbers button is press
 *
 * Return value
 *   void
 */

static void strongs_toggled(GtkToggleButton * togglebutton, 
						VIEW_TEXT * vt)
{
	cur_vt = vt;
	set_text_module_global_option("Strong's Numbers",
				togglebutton->active);
	display(vt, vt->key, TRUE);
}


/******************************************************************************
 * Name
 *   footnotes_toggled
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void footnotes_toggled(GtkToggleButton *togglebutton, 
 *						VIEW_TEXT * vt)	
 *
 * Description
 *   called when Footnotes button is press
 *
 * Return value
 *   void
 */

static void footnotes_toggled(GtkToggleButton * togglebutton,
						VIEW_TEXT * vt)
{
	cur_vt = vt;
	set_text_module_global_option("Footnotes",
				togglebutton->active);
	display(vt, vt->key, TRUE);
}


/******************************************************************************
 * Name
 *  greek_accents_toggled
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void greek_accents_toggled(GtkToggleButton *togglebutton, 
 *						VIEW_TEXT * vt)	
 *
 * Description
 *   called when Greek Accents button is press
 *
 * Return value
 *   void
 */

static void greek_accents_toggled(GtkToggleButton * togglebutton,
						VIEW_TEXT * vt)
{
	cur_vt = vt;
	set_text_module_global_option("Greek Accents",
				togglebutton->active);
	display(vt, vt->key, TRUE);
}


/******************************************************************************
 * Name
 *  lemmas_toggled
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void lemmas_toggled(GtkToggleButton *togglebutton, 
 *					VIEW_TEXT * vt)	
 *
 * Description
 *   called when Lemmas button is press
 *
 * Return value
 *   void
 */

static void lemmas_toggled(GtkToggleButton * togglebutton, 
						VIEW_TEXT * vt)
{
	cur_vt = vt;
	set_text_module_global_option("Lemmas",
				togglebutton->active);
	display(vt, vt->key, TRUE);
}


/******************************************************************************
 * Name
 *  cross_references_toggled
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void cross_references_toggled(GtkToggleButton *togglebutton, 
						VIEW_TEXT * vt)	
 *
 * Description
 *   called when Scripture cross references button is press
 *
 * Return value
 *   void
 */

static void cross_references_toggled(GtkToggleButton * togglebutton,
						VIEW_TEXT * vt)
{
	cur_vt = vt;
	set_text_module_global_option("Scripture Cross-references",
				togglebutton->active);
	display(vt, vt->key, TRUE);
}


/******************************************************************************
 * Name
 *  vowel_points_toggled
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void vowel_points_toggled(GtkToggleButton *togglebutton, 
 *						VIEW_TEXT * vt)	
 *
 * Description
 *   called when Hebrew Vowel Points button is press
 *
 * Return value
 *   void
 */

static void vowel_points_toggled(GtkToggleButton * togglebutton,
						VIEW_TEXT * vt)
{
	cur_vt = vt;
	set_text_module_global_option("Hebrew Vowel Points",
				togglebutton->active);
	display(vt, vt->key, TRUE);
}


/******************************************************************************
 * Name
 *  cantillation_toggled
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void cantillation_toggled(GtkToggleButton *togglebutton, 
 *						VIEW_TEXT * vt)	
 *
 * Description
 *   called when Hebrew Cantillation button is press
 *
 * Return value
 *   void
 */

static void cantillation_toggled(GtkToggleButton * togglebutton,
						VIEW_TEXT * vt)
{
	cur_vt = vt;
	set_text_module_global_option("Hebrew Cantillation",
				togglebutton->active);
	display(vt, vt->key, TRUE);
}


/******************************************************************************
 * Name
 *  headings_toggled
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void headings_toggled(GtkToggleButton *togglebutton, 
 *						VIEW_TEXT * vt)	
 *
 * Description
 *   called when Headings button is press
 *
 * Return value
 *   void
 */

static void headings_toggled(GtkToggleButton * togglebutton,
						VIEW_TEXT * vt)
{
	cur_vt = vt;
	set_text_module_global_option("Headings",
				togglebutton->active);
	display(vt, vt->key, TRUE);
}


/******************************************************************************
 * Name
 *   on_primary_reading_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_primary_reading_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_primary_reading_activate(GtkMenuItem * menuitem,
						gpointer user_data)
{
	/*set_text_variant_global_option("Textual Variants",
				       "Primary Reading");*/
}

/******************************************************************************
 * Name
 *   on_secondary_reading_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_secondary_reading_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_secondary_reading_activate(GtkMenuItem * menuitem,
						gpointer user_data)
{
	/*set_text_variant_global_option("Textual Variants",
				       "Secondary Reading");*/
}

/******************************************************************************
 * Name
 *   on_all_readings_activate
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void on_all_readings_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_all_readings_activate(GtkMenuItem * menuitem,
						gpointer user_data)
{
	/*set_text_variant_global_option("Textual Variants",
				       "All Readings");*/
}

/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, VIEW_TEXT * vt)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_url(GtkHTML * html, const gchar * url, 
						VIEW_TEXT * vt)
{
	gchar buf[255], *buf1;
	gint context_id2;
	
	
	cur_vt = vt;
	
	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(vt->statusbar),
					 settings.program_title);
	gtk_statusbar_pop(GTK_STATUSBAR(vt->statusbar), context_id2);

	/***  moved out of url - clear appbar  ***/
	if (url == NULL) {		
		gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar), context_id2,
			   "");
		//in_url = FALSE;
	}
	/***  we are in an url  ***/
	else {
		
		if (*url == '#') {
			++url;		/* remove # */
			if (*url == 'T') {
				++url;	/* remove T */
				if (*url == 'G') {
					++url;	/* remove G */
					if (settings.havethayer) {
						buf1 = g_strdup(url);
						show_in_statusbar(vt->statusbar, buf1, "Thayer");
						g_free(buf1);
						return;
					}
	
					else
						return;
				}
	
				if (*url == 'H') {
					++url;	/* remove H */
					if (settings.havebdb) {
						buf1 = g_strdup(url);
						show_in_statusbar(vt->statusbar, buf1, "BDB");
						g_free(buf1);
						return;
					}
	
					else
						return;
				}
			}
	
			if (*url == 'G') {
				++url;	/* remove G */
				buf1 = g_strdup(url);
				if (atoi(buf1) > 5624) {
					if (settings.havethayer) {
						show_in_statusbar(vt->statusbar, buf1, "Thayer");
						g_free(buf1);						
						return;
					} else
						return;
	
				}
	
				else {
					show_in_statusbar(vt->statusbar, buf1, settings.lex_greek);
					g_free(buf1);		
					return;
				}
			}
	
			if (*url == 'H') {
				++url;	/* remove H */
				buf1 = g_strdup(url);
				if (atoi(buf1) > 8674) {
					if (settings.havebdb) {
						show_in_statusbar(vt->statusbar, buf1, "BDB");
						g_free(buf1);
						return;
					}
	
					else
						return;
				}
	
				else {
					show_in_statusbar(vt->statusbar, buf1, settings.lex_hebrew);
					g_free(buf1);
					return;
				}
			}
		}
		/***  thml morph tag  ***/
		else if (!strncmp(url, "type=morph", 10)) {
			gchar *modbuf = NULL;
			gchar *mybuf = NULL;
			buf1 = g_strdup(url);
			mybuf = strstr(url, "class=");
			if (mybuf) {
				gint i;
				modbuf = strchr(mybuf, '=');
				++modbuf;
				for (i = 0; i < strlen(modbuf); i++) {
					if (modbuf[i] == ' ') {
						modbuf[i] = '\0';
						break;
					}
				}
			}
	
			mybuf = NULL;
			mybuf = strstr(buf1, "value=");
			if (mybuf) {
				mybuf = strchr(mybuf, '=');
				++mybuf;
			}
			buf1 = g_strdup(mybuf);
			show_in_statusbar(vt->statusbar, buf1, modbuf);			
			g_free(buf1);
			return;
		}
		/*** thml strongs ***/
		else if (!strncmp(url, "type=Strongs", 12)) {
			gchar *modbuf = NULL;
			gchar *mybuf = NULL;
			gchar newref[80];
			gint type = 0;
			//buf = g_strdup(url);
			mybuf = NULL;
			mybuf = strstr(url, "value=");
			//i = 0;
			if (mybuf) {
				mybuf = strchr(mybuf, '=');
				++mybuf;
				if (mybuf[0] == 'H')
					type = 0;
				if (mybuf[0] == 'G')
					type = 1;
				++mybuf;
				sprintf(newref, "%5.5d", atoi(mybuf));
			}
			if (type)
				modbuf = settings.lex_greek;
			else
				modbuf = settings.lex_hebrew;
	
			buf1 = g_strdup(newref);
			show_in_statusbar(vt->statusbar, buf1, modbuf);				
			g_free(buf1);
			return;	
		}
		/***  any other link  ***/
		else
			sprintf(buf, _("Go to %s"), url);
		gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar), context_id2,
			  buf);
	}
}


static GnomeUIInfo primary_reading_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Primary Reading"),
	 NULL,
	 (gpointer) on_primary_reading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Secondary Reading"),
	 NULL,
	 (gpointer) on_secondary_reading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("All Readings"),
	 NULL,
	 (gpointer) on_all_readings_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo textual_variants_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_RADIOITEMS, NULL, NULL, primary_reading_uiinfo,
	 NULL, NULL, GNOME_APP_PIXMAP_NONE, NULL, 0,
	 (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo variant_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Textual Variants"),
	 NULL,
	 textual_variants_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};


/******************************************************************************
 * Name
 *   add_global_option_buttons 
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void add_global_option_buttons(GtkWidget * toolbar, 
 *				gchar * mod_name, 
 *				TEXT_GLOBALS * tgs, 
 *				GtkMenuCallback callback)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_global_option_buttons(GtkWidget * toolbar, 
				gchar * mod_name, VIEW_TEXT * vt)
{	
	gint active = 0;	
	GtkWidget *variant_menu;
	GtkWidget *tmp_toolbar_icon;
	
	if (vt->t->tgs->gbfstrongs || vt->t->tgs->thmlstrongs) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/s.xpm");
		vt->t->tgs->t_btn_strongs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("S"),
					       _
					       ("Toggle Strongs Numbers"),
					       NULL, tmp_toolbar_icon, 
					NULL, NULL);
		gtk_widget_ref(vt->t->tgs->t_btn_strongs);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vt->t->tgs->t_btn_strongs",
					 vt->t->tgs->t_btn_strongs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vt->t->tgs->t_btn_strongs);
		gtk_widget_set_usize(vt->t->tgs->t_btn_strongs, 24, 24);
		
		active = load_module_options(mod_name, "Strong's Numbers");
		
		gtk_toggle_button_set_active(
				GTK_TOGGLE_BUTTON(vt->t->tgs->t_btn_strongs),
                                        active);
		
		gtk_signal_connect(GTK_OBJECT(vt->t->tgs->t_btn_strongs),
				   "toggled",
				   GTK_SIGNAL_FUNC(strongs_toggled),
				   vt);
	}
	if (vt->t->tgs->gbfmorphs || vt->t->tgs->thmlmorphs) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/m.xpm");
		vt->t->tgs->t_btn_morphs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					NULL, _("M"),
					_("Toggle Morph Tags"),
					NULL, tmp_toolbar_icon, 
					NULL, NULL);
		gtk_widget_ref(vt->t->tgs->t_btn_morphs);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vt->t->tgs->t_btn_morphs",
					 vt->t->tgs->t_btn_morphs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vt->t->tgs->t_btn_morphs);
		gtk_widget_set_usize(vt->t->tgs->t_btn_morphs, 24, 24);
		
		active = load_module_options(mod_name, "Morphological Tags");
		
		gtk_toggle_button_set_active(
				GTK_TOGGLE_BUTTON(vt->t->tgs->t_btn_morphs),
                                        active);

		gtk_signal_connect(GTK_OBJECT(vt->t->tgs->t_btn_morphs),
				   "toggled",
				   GTK_SIGNAL_FUNC(morphs_toggled),
				   vt);
	}
	if (vt->t->tgs->gbffootnotes || vt->t->tgs->thmlfootnotes) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/f.xpm");
		vt->t->tgs->t_btn_footnotes =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("F"),
					       _("Toggle Footnotes"),
					       NULL, tmp_toolbar_icon, NULL, NULL);
		gtk_widget_ref(vt->t->tgs->t_btn_footnotes);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vt->t->tgs->t_btn_footnotes",
					 vt->t->tgs->t_btn_footnotes,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vt->t->tgs->t_btn_footnotes);
		gtk_widget_set_usize(vt->t->tgs->t_btn_footnotes, 24, 24);
		
		active = load_module_options(mod_name, "Footnotes");
		
		gtk_toggle_button_set_active(
				GTK_TOGGLE_BUTTON(vt->t->tgs->t_btn_footnotes),
                                        active);

		gtk_signal_connect(GTK_OBJECT(vt->t->tgs->t_btn_footnotes),
				   "toggled",
				   GTK_SIGNAL_FUNC(footnotes_toggled),
				   vt);
	}
	if (vt->t->tgs->greekaccents) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/a.xpm");
		vt->t->tgs->t_btn_accents =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("A"),
					       _
					       ("Toggle Greek Accents"),
					       NULL, tmp_toolbar_icon, NULL, NULL);
		gtk_widget_ref(vt->t->tgs->t_btn_accents);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vt->t->tgs->t_btn_accents",
					 vt->t->tgs->t_btn_accents,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vt->t->tgs->t_btn_accents);
		gtk_widget_set_usize(vt->t->tgs->t_btn_accents, 24, 24);
		
		active = load_module_options(mod_name, "Greek Accents");
		
		gtk_toggle_button_set_active(
				GTK_TOGGLE_BUTTON(vt->t->tgs->t_btn_accents),
                                        active);

		gtk_signal_connect(GTK_OBJECT(vt->t->tgs->t_btn_accents),
				   "toggled",
				   GTK_SIGNAL_FUNC(greek_accents_toggled),
				   vt);

	}
	if (vt->t->tgs->lemmas) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/l.xpm");
		vt->t->tgs->t_btn_lemmas =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("L"),
					       _("Toggle Lemmas"), NULL,
					       tmp_toolbar_icon, NULL, NULL);
		gtk_widget_ref(vt->t->tgs->t_btn_lemmas);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vt->t->tgs->t_btn_lemmas",
					 vt->t->tgs->t_btn_lemmas,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vt->t->tgs->t_btn_lemmas);
		gtk_widget_set_usize(vt->t->tgs->t_btn_lemmas, 24, 24);
		
		active = load_module_options(mod_name, "Lemmas");
		
		gtk_toggle_button_set_active(
				GTK_TOGGLE_BUTTON(vt->t->tgs->t_btn_lemmas),
                                        active);

		gtk_signal_connect(GTK_OBJECT(vt->t->tgs->t_btn_lemmas),
				   "toggled",
				   GTK_SIGNAL_FUNC(lemmas_toggled),
				   vt);

	}
	if (vt->t->tgs->scripturerefs) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/r.xpm");
		vt->t->tgs->t_btn_scripturerefs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("R"),
					       _
					       ("Toggle Scripture References"),
					       NULL, tmp_toolbar_icon, NULL, NULL);
		gtk_widget_ref(vt->t->tgs->t_btn_scripturerefs);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vt->t->tgs->t_btn_scripturerefs",
					 vt->t->tgs->t_btn_scripturerefs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vt->t->tgs->t_btn_scripturerefs);
		gtk_widget_set_usize(vt->t->tgs->t_btn_scripturerefs, 24, 24);
		
		active = load_module_options(mod_name, "Scripture Cross-references");
		
		gtk_toggle_button_set_active(
				GTK_TOGGLE_BUTTON(vt->t->tgs->t_btn_scripturerefs),
                                        active);


		gtk_signal_connect(GTK_OBJECT(vt->t->tgs->t_btn_scripturerefs),
				   "toggled",
				   GTK_SIGNAL_FUNC(cross_references_toggled),
				   vt);

	}
	if (vt->t->tgs->hebrewpoints) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/p.xpm");
		vt->t->tgs->t_btn_points =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("P"),
					       _
					       ("Toggle Hebrew Vowel Points"),
					       NULL, tmp_toolbar_icon, NULL, NULL);
		gtk_widget_ref(vt->t->tgs->t_btn_points);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vt->t->tgs->t_btn_points",
					 vt->t->tgs->t_btn_points,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vt->t->tgs->t_btn_points);
		gtk_widget_set_usize(vt->t->tgs->t_btn_points, 24, 24);
		
		active = load_module_options(mod_name, "Hebrew Vowel Points");
		
		gtk_toggle_button_set_active(
				GTK_TOGGLE_BUTTON(vt->t->tgs->t_btn_points),
                                        active);

		gtk_signal_connect(GTK_OBJECT(vt->t->tgs->t_btn_points),
				   "toggled",
				   GTK_SIGNAL_FUNC(vowel_points_toggled),
				   vt);

	}
	if (vt->t->tgs->hebrewcant) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/c.xpm");
		vt->t->tgs->t_btn_cant =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("C"),
					       _
					       ("Toggle Hebrew Cantillation"),
					       NULL, tmp_toolbar_icon, NULL, NULL);
		gtk_widget_ref(vt->t->tgs->t_btn_cant);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vt->t->tgs->t_btn_cant", vt->t->tgs->t_btn_cant,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vt->t->tgs->t_btn_cant);
		gtk_widget_set_usize(vt->t->tgs->t_btn_cant, 24, 24);
		
		active = load_module_options(mod_name, "Hebrew Cantillation");
		
		gtk_toggle_button_set_active(
				GTK_TOGGLE_BUTTON(vt->t->tgs->t_btn_cant),
                                        active);

		gtk_signal_connect(GTK_OBJECT(vt->t->tgs->t_btn_cant), "toggled",
				   GTK_SIGNAL_FUNC(cantillation_toggled),
				   vt);

	}
	if (vt->t->tgs->headings) {
		tmp_toolbar_icon =
		    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/h.xpm");
		vt->t->tgs->t_btn_headings =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("H"),
					       _("Toggle Headings"),
					       NULL, tmp_toolbar_icon, NULL, NULL);
		gtk_widget_ref(vt->t->tgs->t_btn_headings);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "vt->t->tgs->t_btn_headings",
					 vt->t->tgs->t_btn_headings,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(vt->t->tgs->t_btn_headings);
		gtk_widget_set_usize(vt->t->tgs->t_btn_headings, 24, 24);
		
		active = load_module_options(mod_name, "Headings");
		
		gtk_toggle_button_set_active(
				GTK_TOGGLE_BUTTON(vt->t->tgs->t_btn_headings),
                                        active);

		gtk_signal_connect(GTK_OBJECT(vt->t->tgs->t_btn_headings),
				   "toggled",
				   GTK_SIGNAL_FUNC(headings_toggled),
				   vt);
	}
	if (vt->t->tgs->variants) {
		variant_menu = gtk_menu_bar_new();
		gtk_widget_ref(variant_menu);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "variant_menu", variant_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(variant_menu);
		gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar),
					  variant_menu, NULL, NULL);
		gnome_app_fill_menu(GTK_MENU_SHELL(variant_menu),
				    variant_menu_uiinfo, NULL, FALSE,
				    0);

		gtk_widget_ref(variant_menu_uiinfo[0].widget);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "textual_variants",
					 variant_menu_uiinfo[0].widget,
					 (GtkDestroyNotify)
					 gtk_widget_unref);

		gtk_widget_ref(primary_reading_uiinfo[0].widget);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "primary_reading",
					 primary_reading_uiinfo[0].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
					       (primary_reading_uiinfo
						[0].widget), TRUE);

		vt->t->tgs->t_btn_primary = primary_reading_uiinfo[0].widget;
		

		gtk_widget_ref(primary_reading_uiinfo[1].widget);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "secondary_reading",
					 primary_reading_uiinfo[1].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);

		vt->t->tgs->t_btn_secondary = primary_reading_uiinfo[1].widget;

		gtk_widget_ref(primary_reading_uiinfo[2].widget);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app),
					 "all_readings",
					 primary_reading_uiinfo[2].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);

		vt->t->tgs->t_btn_all = primary_reading_uiinfo[2].widget;
	}

}

/******************************************************************************
 * Name
 *   create_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void create_bibletext_dialog(VIEW_TEXT * vt)	
 *
 * Description
 *   viewtext gui
 *
 * Return value
 *   void 
 */

static void create_bibletext_dialog(VIEW_TEXT * vt)
{

	GtkWidget *vbox33;
	GtkWidget *toolbar;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator16;
	GtkWidget *menubar;
	GtkWidget *module_new;
	GtkWidget *module_new_menu;
	GtkAccelGroup *module_new_menu_accels;
	GtkWidget *toolbar30;
	GtkWidget *combo11;
	GtkObject *spbVTChapter_adj;
	GtkObject *spbVTVerse_adj;
	GtkWidget *btnGotoVerse;
	GtkWidget *frame21;
	GtkWidget *swVText;
			      
	vt->dialog = gtk_window_new (GTK_WINDOW_DIALOG);
	    
	gtk_object_set_data(GTK_OBJECT(vt->dialog), "dlg->dialog",
			    vt->dialog);
	gtk_window_set_title (GTK_WINDOW (vt->dialog), get_module_description(vt->t->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(vt->dialog), 335, 400);
	gtk_window_set_policy(GTK_WINDOW(vt->dialog), TRUE, TRUE,
			      FALSE);

	vbox33 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox33);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vbox33",
				 vbox33,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox33);
	gtk_container_add(GTK_CONTAINER(vt->dialog), vbox33);
	

	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "toolbar",
				 toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_box_pack_start(GTK_BOX(vbox33), toolbar, FALSE, FALSE, 0);
	gtk_toolbar_set_space_size(GTK_TOOLBAR(toolbar), 3);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vt->dialog,
				      GNOME_STOCK_PIXMAP_REFRESH);
	vt->btn_sync =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Sync"),
				       _("Sync with main window"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(vt->btn_sync);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vt->btn_sync",
				 vt->btn_sync,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->btn_sync);

	vseparator16 = gtk_vseparator_new();
	gtk_widget_ref(vseparator16);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "vseparator16", vseparator16,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator16);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator16,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator16, 7, 13);
	

	/* add global option buttons to toolbar */			 
	add_global_option_buttons(toolbar, vt->t->mod_name, 
				vt);	
	


	vseparator16 = gtk_vseparator_new();
	gtk_widget_ref(vseparator16);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "vseparator16", vseparator16,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator16);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator16,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator16, 7, 13);

	menubar = gtk_menu_bar_new();
	gtk_widget_ref(menubar);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "menubar",
				 menubar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), menubar, NULL,
				  NULL);

	module_new =
	    gtk_menu_item_new_with_label(_("Open New Module "));
	gtk_widget_ref(module_new);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "module_new", module_new,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(module_new);
	gtk_container_add(GTK_CONTAINER(menubar), module_new);

	module_new_menu = gtk_menu_new();
	gtk_widget_ref(module_new_menu);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "module_new_menu",
				 module_new_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_new),
				  module_new_menu);
	module_new_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (module_new_menu));

	add_items_to_module_menu(module_new_menu);


	vseparator16 = gtk_vseparator_new();
	gtk_widget_ref(vseparator16);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "vseparator16", vseparator16,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator16);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator16,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator16, 7, 13);

	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vt->dialog,
				      GNOME_STOCK_PIXMAP_EXIT);
	vt->btn_close =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Close"), _("Close Dialog"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(vt->btn_close);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "vt->btn_close", vt->btn_close,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->btn_close);

	toolbar30 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar30);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "toolbar30",
				 toolbar30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar30);
	gtk_box_pack_start(GTK_BOX(vbox33), toolbar30, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar30),
				      GTK_RELIEF_NONE);

	combo11 = gtk_combo_new();
	gtk_widget_ref(combo11);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "combo11",
				 combo11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo11);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30), combo11, NULL,
				  NULL);

	vt->cbe_book = GTK_COMBO(combo11)->entry;
	gtk_widget_ref(vt->cbe_book);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vt->cbe_book",
				 vt->cbe_book,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->cbe_book);


	spbVTChapter_adj = gtk_adjustment_new(1, 0, 100, 1, 10, 10);
	vt->spb_chapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVTChapter_adj), 1, 0);
	gtk_widget_ref(vt->spb_chapter);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "vt->spb_chapter", vt->spb_chapter,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->spb_chapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30), vt->spb_chapter,
				  NULL, NULL);
	gtk_widget_set_usize(vt->spb_chapter, 53, -2);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(vt->spb_chapter),
				    TRUE);

	spbVTVerse_adj = gtk_adjustment_new(1, 0, 100, 1, 10, 10);
	vt->spb_verse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVTVerse_adj), 1, 0);
	gtk_widget_ref(vt->spb_verse);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vt->spb_verse",
				 vt->spb_verse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->spb_verse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30),
				  GTK_WIDGET(vt->spb_verse), NULL, NULL);
	gtk_widget_set_usize(vt->spb_verse, 51, -2);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vt->dialog,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	btnGotoVerse =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar30),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button7"), _("Go to verse"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnGotoVerse);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "btnGotoVerse", btnGotoVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnGotoVerse);

	frame21 = gtk_frame_new(NULL);
	gtk_widget_ref(frame21);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "frame21",
				 frame21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame21);
	gtk_box_pack_start(GTK_BOX(vbox33), frame21, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame21, -2, 400);

	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swVText);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "swVText",
				 swVText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swVText);
	gtk_container_add(GTK_CONTAINER(frame21), swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);

	vt->t->html = gtk_html_new();
	gtk_widget_ref(vt->t->html);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vt->t->html", vt->t->html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->t->html);
	gtk_container_add(GTK_CONTAINER(swVText), vt->t->html);
	gtk_html_load_empty(GTK_HTML(vt->t->html));
	
	vt->statusbar = gtk_statusbar_new ();
	gtk_widget_ref (vt->statusbar);
	gtk_object_set_data_full (GTK_OBJECT (vt->dialog), "vt->statusbar", vt->statusbar,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (vt->statusbar);
	gtk_box_pack_start (GTK_BOX (vbox33), vt->statusbar, FALSE, FALSE, 0);
	

	
	gtk_signal_connect (GTK_OBJECT (vt->dialog), "destroy",
                      GTK_SIGNAL_FUNC (dialog_destroy),
			   vt);

	gtk_signal_connect (GTK_OBJECT (vt->t->html), "motion_notify_event",
			   GTK_SIGNAL_FUNC (on_dialog_motion_notify_event),
			   vt);
	gtk_signal_connect (GTK_OBJECT (vt->dialog), "motion_notify_event",
			   GTK_SIGNAL_FUNC (on_dialog_motion_notify_event),
			   vt);
	gtk_signal_connect(GTK_OBJECT(vt->t->html), "on_url",
			   GTK_SIGNAL_FUNC(dialog_url), 
			   (gpointer) vt);
	gtk_signal_connect(GTK_OBJECT(vt->t->html), "link_clicked",
			   GTK_SIGNAL_FUNC(link_clicked), 
			   vt);
	gtk_signal_connect(GTK_OBJECT(vt->btn_sync), "clicked",
			   GTK_SIGNAL_FUNC(btn_sync_clicked), 
			   vt);
	gtk_signal_connect(GTK_OBJECT(vt->cbe_book), "changed",
			   GTK_SIGNAL_FUNC(cbe_book_changed), 
			   vt);
	gtk_signal_connect(GTK_OBJECT(btnGotoVerse), "clicked",
			   GTK_SIGNAL_FUNC(btn_goto_verse_clicked),
			   vt);
			   
	gtk_signal_connect(GTK_OBJECT(vt->btn_close), "clicked",
			   GTK_SIGNAL_FUNC(btn_close_clicked),
			   vt);	
			   
	
	gtk_combo_set_popdown_strings(GTK_COMBO(combo11), get_list(BOOKS_LIST));
	strcpy(vt->key, settings.currentverse);
	display(vt, settings.currentverse, TRUE);	
	update_controls(vt);
}

/******************************************************************************
 * Name
 *   gui_open_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_open_bibletext_dialog(gint mod_num)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_open_bibletext_dialog(gchar * mod_name)
{		
	VIEW_TEXT * vt;
	
	vt = g_new(VIEW_TEXT, 1);
	vt->t = g_new(TEXT_DATA, 1);
	vt->t->tgs = g_new(TEXT_GLOBALS, 1);
	vt->t->find_dialog = NULL;
	vt->t->mod_num = get_module_number(mod_name, TEXT_MODS);
	vt->t->search_string = NULL;
	vt->dialog = NULL;
	vt->t->mod_name = mod_name;
	gui_get_module_global_options(vt->t);
	create_bibletext_dialog(vt);
	gtk_widget_show(vt->dialog);
	dialog_list = g_list_append(dialog_list, (VIEW_TEXT *) vt);
	btn_sync_clicked(NULL, vt);
}

/******************************************************************************
 * Name
 *   gui_setup_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_setup_bibletext_dialog(GList *mods)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_setup_bibletext_dialog(GList *mods)
{
	dialog_list = NULL;
	dialog_freed = FALSE;
}

/******************************************************************************
 * Name
 *  gui_shutdown_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void gui_shutdown_bibletext_dialog(void)	
 *
 * Description
 *   shut down bibletext dialog support clean mem
 *
 * Return value
 *   void
 */

void gui_shutdown_bibletext_dialog(void) 
{
	dialog_list = g_list_first(dialog_list);
	while (dialog_list != NULL) {
		VIEW_TEXT *vt = (VIEW_TEXT *) dialog_list->data;
		dialog_freed = TRUE;
//		g_warning("shuting down %s dialog",vt->t->mod_name);
		/* 
		 *  destroy any dialogs created 
		 */
		if (vt->dialog)
			 gtk_widget_destroy(vt->dialog);
		/* 
		 * free any search dialogs created 
		 */
		if (vt->t->find_dialog)
			 g_free(vt->t->find_dialog);
		/* 
		 * free each TEXT_DATA item created 
		 */
		g_free(vt->t->tgs);
		g_free(vt->t);
		g_free((VIEW_TEXT *) dialog_list->data);
		dialog_list = g_list_next(dialog_list);
	} 
	g_list_free(dialog_list);
}

/******   end of file   ******/
