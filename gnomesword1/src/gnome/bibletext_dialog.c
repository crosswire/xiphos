/*
 * GnomeSword Bible Study Tool
 * bibletext_dialog.c - view Bible text module in a dialog
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
#include <gtkhtml/gtkhtml.h>
#include <gal/widgets/e-unicode.h>

#include "gui/gtkhtml_display.h"
#include "gui/bibletext_dialog.h"
#include "gui/bibletext_menu.h"
#include "gui/display_info.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"
#include "gui/utilities.h"

#include "main/bibletext.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/key.h"



/******************************************************************************
 * static - global to this file only
 */
static GList *dialog_list;
static TEXT_DATA *cur_vt;
static gboolean dialog_freed;
static gboolean apply_change;

/******************************************************************************
 * externs
 */
extern gboolean gsI_isrunning;	/* information dialog */




/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   void  gui_on_lookup_bibletext_dialog_selection
		(GtkMenuItem * menuitem, gchar * dict_mod_description)	
 *
 * Description
 *   lookup seledtion in a dict/lex module
 *
 * Return value
 *   void
 */

void gui_on_lookup_bibletext_dialog_selection
		(GtkMenuItem * menuitem, gchar * dict_mod_description)
{
	gchar *dict_key, mod_name[16];

	memset(mod_name, 0, 16);
	module_name_from_description(mod_name, dict_mod_description);

	dict_key = gui_get_word_or_selection(cur_vt->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(mod_name,
						      dict_key);
		if (settings.inDictpane)
			gui_change_module_and_key(mod_name, dict_key);
		g_free(dict_key);
	}
}

/******************************************************************************
 * Name
 *   gui_close_text_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void btn_close_clicked(GtkButton * button,TEXT_DATA * vt)	
 *
 * Description
 *   call gtk_widget_destroy to destroy the bibletext dialog
 *   set vt->dialog to NULL
 *
 * Return value
 *   void
 */

void gui_close_text_dialog(TEXT_DATA * vt)
{
	if (vt->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(vt->dialog);
	}
}


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
	if (str) {
		gtk_statusbar_push(GTK_STATUSBAR(statusbar),
				   context_id2, str);
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
 *   void display(TEXT_DATA * vt, gchar * key, gboolean show_key)	
 *
 * Description
 *   calls chapter_display to display module text
 *
 * Return value
 *   void
 */

static void display(TEXT_DATA * vt, gchar * key, gboolean show_key)
{
	chapter_display(vt->html,
			vt->mod_name, vt->tgs, key, show_key);
}


/******************************************************************************
 * Name
 *   update_controls
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void update_controls(TEXT_DATA * vt)	
 *
 * Description
 *   update the book, chapter and verse contorls
 *
 * Return value
 *   void
 */

static void update_controls(TEXT_DATA * vt)
{
	gchar *val_key;
	gint cur_chapter, cur_verse;

	cur_vt = vt;
	apply_change = FALSE;
	val_key = get_valid_key(vt->key);
	cur_chapter = get_chapter_from_key(val_key);
	cur_verse = get_verse_from_key(val_key);
	/* 
	 *  set book, chapter,verse and freeform lookup entries
	 *  to new verse - settings.apply_change is set to false so we don't
	 *  start a loop
	 */
	gtk_entry_set_text(GTK_ENTRY(vt->cbe_book),
			   get_book_from_key(val_key));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (vt->spb_chapter), cur_chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (vt->spb_verse), cur_verse);
	gtk_entry_set_text(GTK_ENTRY(vt->freeform_lookup), val_key);
	g_free(val_key);

	apply_change = TRUE;
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
			 TEXT_DATA * vt)
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
//              g_warning(url);
		buf = g_strdup(url);
		strcpy(vt->key, buf);
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
				dlg =
				    gui_create_display_informtion_dialog
				    ();
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
		mybuf = NULL;
		mybuf = strstr(url, "value=");
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
				dlg =
				    gui_create_display_informtion_dialog
				    ();
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
 *   book_changed
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void book_changed(GtkEditable * editable, gpointer user_data)	
 *
 * Description
 *   change book 
 *
 * Return value
 *   void
 */

static void book_changed(GtkEditable * editable, TEXT_DATA * vt)
{
	if (apply_change) {
		gchar buf[256], *val_key;
		gchar *bookname =
		    gtk_entry_get_text(GTK_ENTRY(vt->cbe_book));
		sprintf(buf, "%s 1:1", bookname);
		val_key = get_valid_key(buf);
		display(vt, val_key, TRUE);
		strcpy(vt->key, val_key);
		update_controls(vt);
		g_free(val_key);
	}
}


/******************************************************************************
 * Name
 *   chapter_button_release_event
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   gboolean chapter_button_release_event(GtkWidget * widget,
 *			GdkEventButton * event,   gpointer user_data)	
 *
 * Description
 *    change chapter 
 *
 * Return value
 *   gboolean
 */

static gboolean chapter_button_release_event(GtkWidget * widget,
					     GdkEventButton * event,
					     TEXT_DATA * vt)
{
	if (apply_change) {
		gchar *bookname, *val_key;
		gchar buf[256];
		gint chapter;

		bookname = gtk_entry_get_text(GTK_ENTRY(vt->cbe_book));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (vt->spb_chapter));
		sprintf(buf, "%s %d:1", bookname, chapter);
		val_key = get_valid_key(buf);
		display(vt, val_key, TRUE);
		strcpy(vt->key, val_key);
		update_controls(vt);
		g_free(val_key);
		return TRUE;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   gboolean (GtkWidget * widget, 
 *			GdkEventButton * event,  gpointer user_data)	
 *
 * Description
 *    change verse
 *
 * Return value
 *   gboolean
 */

static gboolean verse_button_release_event(GtkWidget * widget,
					   GdkEventButton * event,
					   TEXT_DATA * vt)
{
	if (apply_change) {
		gchar *bookname, buf[256], *val_key;
		gint chapter, verse;

		bookname = gtk_entry_get_text(GTK_ENTRY(vt->cbe_book));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (vt->spb_chapter));
		verse =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (vt->spb_verse));
		sprintf(buf, "%s %d:%d", bookname, chapter, verse);

		val_key = get_valid_key(buf);
		display(vt, val_key, TRUE);
		strcpy(vt->key, val_key);
		update_controls(vt);
		g_free(val_key);
		return TRUE;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   entry_key_press_event
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   gboolean entry_key_press_event(GtkWidget * widget,
 *				GdkEventKey * event, gpointer user_data)	
 *
 * Description
 *   go to verse in free form entry if user hit <enter>
 *
 * Return value
 *   gboolean
 */

static gboolean entry_key_press_event(GtkWidget * widget,
				      GdkEventKey * event,
				      TEXT_DATA * vt)
{
	/* if <enter> key */
	if (event->keyval == 65293 || event->keyval == 65421) {
		gchar *buf, *val_key;
		buf = gtk_entry_get_text(GTK_ENTRY(widget));
		val_key = get_valid_key(buf);
		display(vt, val_key, TRUE);
		strcpy(vt->key, val_key);
		update_controls(vt);
		g_free(val_key);
	}
	return TRUE;
}

/******************************************************************************
 * Name
 *   gui_sync_bibletext_dialog
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *   gui_sync_bibletext_dialog(TEXT_DATA * vt)	
 *
 * Description
 *   set bibletext dialog to main window current verse
 *
 * Return value
 *   void
 */

void gui_sync_bibletext_dialog(TEXT_DATA * vt)
{
	display(cur_vt, settings.currentverse, TRUE);
	cur_vt->key = settings.currentverse;
	update_controls(cur_vt);
}


/******************************************************************************
 * Name
 *   free_on_destroy
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void free_on_destroy(TEXT_DATA * vt)
 *
 * Description
 *   removes dialog from dialog_list when dialog is destroyed other than
 *   program shut down
 *
 * Return value
 *   void
 */

static void free_on_destroy(TEXT_DATA * vt)
{
	g_free(vt->tgs);
	dialog_list = g_list_remove(dialog_list, (TEXT_DATA *) vt);
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

static void dialog_destroy(GtkObject * object, TEXT_DATA * vt)
{
	if (!dialog_freed)
		free_on_destroy(vt);
	dialog_freed = FALSE;
}


static gboolean on_dialog_motion_notify_event(GtkWidget * widget,
					      GdkEventMotion * event,
					      TEXT_DATA * vt)
{
	cur_vt = vt;
	return TRUE;
}

/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, TEXT_DATA * vt)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_url(GtkHTML * html, const gchar * url,
		       TEXT_DATA * vt)
{
	gchar buf[255], *buf1;
	gchar *url_buf = NULL;
	gint context_id2;

	
	cur_vt = vt;
	if(url) url_buf = g_strdup(url);
	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(vt->statusbar),
					 settings.program_title);
	gtk_statusbar_pop(GTK_STATUSBAR(vt->statusbar), context_id2);

	/***  moved out of url - clear appbar  ***/
	if (url == NULL) {
		gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar),
				   context_id2, "");
		//in_url = FALSE;
	}
	/***  we are in an url  ***/
	else {

		if (*url_buf == '#') {
			++url_buf;	/* remove # */
			if (*url_buf == 'T') {
				++url_buf;	/* remove T */
				if (*url_buf == 'G') {
					++url_buf;	/* remove G */
					if (settings.havethayer) {
						buf1 = g_strdup(url);
						show_in_statusbar(vt->
								  statusbar,
								  buf1,
								  "Thayer");
						g_free(buf1);
						return;
					}

					else
						return;
				}

				if (*url_buf == 'H') {
					++url_buf;	/* remove H */
					if (settings.havebdb) {
						buf1 = g_strdup(url);
						show_in_statusbar(vt->
								  statusbar,
								  buf1,
								  "BDB");
						g_free(buf1);
						return;
					}

					else
						return;
				}
			}

			if (*url_buf == 'G') {
				++url_buf;	/* remove G */
				buf1 = g_strdup(url_buf);
				if (atoi(buf1) > 5624) {
					if (settings.havethayer) {
						show_in_statusbar(vt->
								  statusbar,
								  buf1,
								  "Thayer");
						g_free(buf1);
						return;
					} else
						return;

				}

				else {
					show_in_statusbar(vt->statusbar,
							  buf1,
							  settings.
							  lex_greek);
					g_free(buf1);
					return;
				}
			}

			if (*url_buf == 'H') {
				++url_buf;	/* remove H */
				buf1 = g_strdup(url);
				if (atoi(buf1) > 8674) {
					if (settings.havebdb) {
						show_in_statusbar(vt->
								  statusbar,
								  buf1,
								  "BDB");
						g_free(buf1);
						return;
					}

					else
						return;
				}

				else {
					show_in_statusbar(vt->statusbar,
							  buf1,
							  settings.
							  lex_hebrew);
					g_free(buf1);
					return;
				}
			}
		}
		/***  thml morph tag  ***/
		else if (!strncmp(url, "type=morph", 10)) {
			gchar *modbuf = NULL;
			gchar *mybuf = NULL;
			buf1 = g_strdup(url_buf);
			mybuf = strstr(url_buf, "class=");
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
		else if (!strncmp(url_buf, "type=Strongs", 12)) {
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
			sprintf(buf, "%s %s", _("Go to "), url);
		gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar),
				   context_id2, buf);
	}
	if(url_buf) 
		g_free(url_buf);
}


/******************************************************************************
 * Name
 *   create_nav_toolbar
 *
 * Synopsis
 *   #include ".h"
 *
 *   GtkWidget *create_nav_toolbar(void)	
 *
 * Description
 *    create navigation toolbar and 
 *
 * Return value
 *   void
 */

static GtkWidget *create_nav_toolbar(TEXT_DATA * vt)
{
	GtkWidget *toolbar_nav;
	GtkWidget *cbBook;
	GtkObject *spbChapter_adj;
	GtkObject *spbVerse_adj;

	toolbar_nav =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar_nav);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "toolbar_nav",
				 toolbar_nav,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar_nav);
	gtk_widget_set_usize(toolbar_nav, -2, 34);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar_nav),
				      GTK_RELIEF_NONE);

	cbBook = gtk_combo_new();
	gtk_widget_show(cbBook);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav), cbBook,
				  NULL, NULL);

	/*
	 * get and load books of the Bible 
	 */
	gtk_combo_set_popdown_strings(GTK_COMBO(cbBook),
				      get_list(BOOKS_LIST));

	vt->cbe_book = GTK_COMBO(cbBook)->entry;
	gtk_widget_set_usize(vt->cbe_book, 100, -2);
	gtk_widget_show(vt->cbe_book);
	gtk_entry_set_text(GTK_ENTRY(vt->cbe_book), _("Romans"));

	spbChapter_adj = gtk_adjustment_new(8, -1, 151, 1, 10, 10);
	vt->spb_chapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbChapter_adj), 1, 0);
	gtk_widget_show(vt->spb_chapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vt->spb_chapter, NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON
				    (vt->spb_chapter), TRUE);

	spbVerse_adj = gtk_adjustment_new(28, -1, 180, 1, 10, 10);
	vt->spb_verse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVerse_adj), 1, 0);
	gtk_widget_show(vt->spb_verse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vt->spb_verse, NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(vt->spb_verse),
				    TRUE);

	vt->freeform_lookup = gtk_entry_new();;
	gtk_widget_show(vt->freeform_lookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vt->freeform_lookup, NULL, NULL);
	gtk_entry_set_text(GTK_ENTRY(vt->freeform_lookup),
			   _("Romans 8:28"));

	gtk_signal_connect(GTK_OBJECT(vt->cbe_book),
			   "changed",
			   GTK_SIGNAL_FUNC(book_changed), vt);
	gtk_signal_connect(GTK_OBJECT(vt->spb_chapter),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (chapter_button_release_event), vt);
	gtk_signal_connect(GTK_OBJECT(vt->spb_verse),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (verse_button_release_event), vt);
	gtk_signal_connect(GTK_OBJECT(vt->freeform_lookup),
			   "key_press_event",
			   GTK_SIGNAL_FUNC(entry_key_press_event), vt);
	return toolbar_nav;
}



/******************************************************************************
 * Name
 *   create_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void create_bibletext_dialog(TEXT_DATA * vt)	
 *
 * Description
 *   viewtext gui
 *
 * Return value
 *   void 
 */

static void create_bibletext_dialog(TEXT_DATA * vt)
{

	GtkWidget *vbox33;
	GtkWidget *toolbar_nav;
	GtkWidget *frame21;
	GtkWidget *swVText; 

	vt->dialog = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_object_set_data(GTK_OBJECT(vt->dialog), "dlg->dialog",
			    vt->dialog);
	gtk_window_set_title(GTK_WINDOW(vt->dialog),
			     get_module_description(vt->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(vt->dialog), 380, 400);
	gtk_window_set_policy(GTK_WINDOW(vt->dialog), TRUE, TRUE,
			      FALSE);

	vbox33 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox33);
	gtk_container_add(GTK_CONTAINER(vt->dialog), vbox33);


	toolbar_nav = create_nav_toolbar(vt);
	gtk_widget_show(toolbar_nav);
	gtk_box_pack_start(GTK_BOX(vbox33), toolbar_nav, FALSE, FALSE,
			   0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar_nav),
				      GTK_RELIEF_NONE);

	frame21 = gtk_frame_new(NULL);
	gtk_widget_show(frame21);
	gtk_box_pack_start(GTK_BOX(vbox33), frame21, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame21, -2, 400);

	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(swVText);
	gtk_container_add(GTK_CONTAINER(frame21), swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);

	vt->html = gtk_html_new();
	gtk_widget_show(vt->html);
	gtk_container_add(GTK_CONTAINER(swVText), vt->html);
	gtk_html_load_empty(GTK_HTML(vt->html));

	vt->statusbar = gtk_statusbar_new();
	gtk_widget_show(vt->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox33), vt->statusbar, FALSE, FALSE,
			   0);



	gtk_signal_connect(GTK_OBJECT(vt->dialog),
			   "destroy",
			   GTK_SIGNAL_FUNC(dialog_destroy), vt);
	gtk_signal_connect(GTK_OBJECT(vt->html),
			   "motion_notify_event",
			   GTK_SIGNAL_FUNC
			   (on_dialog_motion_notify_event), vt);
	gtk_signal_connect(GTK_OBJECT(vt->dialog),
			   "motion_notify_event",
			   GTK_SIGNAL_FUNC
			   (on_dialog_motion_notify_event), vt);
	gtk_signal_connect(GTK_OBJECT(vt->html), "on_url",
			   GTK_SIGNAL_FUNC(dialog_url), (gpointer) vt);			   
	gtk_signal_connect(GTK_OBJECT(vt->html), "link_clicked",
			   GTK_SIGNAL_FUNC(link_clicked), vt);
}


/******************************************************************************
 * Name
 *   gui_bibletext_dialog_goto_bookmark
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_bibletext_dialog_goto_bookmark(gchar * mod_name, gchar * key)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_bibletext_dialog_goto_bookmark(gchar * mod_name, gchar * key)
{
	GList *tmp = NULL;
	tmp = g_list_first(dialog_list);
	while (tmp != NULL) {
		TEXT_DATA *vt = (TEXT_DATA *) tmp->data;
		if(!strcmp(vt->mod_name, mod_name)) {
			vt->key = key;
			display(vt, vt->key, TRUE);
			update_controls(vt);
			gdk_window_raise(vt->dialog->window);
			return;
		}		
		tmp = g_list_next(tmp);
	}
	gui_open_bibletext_dialog(mod_name);
	strcpy(cur_vt->key, key);
	display(cur_vt, cur_vt->key, TRUE);
	update_controls(cur_vt);
}

/******************************************************************************
 * Name
 *   gui_open_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_open_bibletext_dialog(gchar * mod_name)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_open_bibletext_dialog(gchar * mod_name)
{
	TEXT_DATA *vt;
	GtkWidget *popupmenu;

	vt = g_new(TEXT_DATA, 1);
	vt->tgs = g_new(TEXT_GLOBALS, 1);
	vt->mod_num = get_module_number(mod_name, TEXT_MODS);
	vt->search_string = NULL;
	vt->dialog = NULL;
	vt->is_dialog = TRUE;
	vt->mod_name = g_strdup(mod_name);
	create_bibletext_dialog(vt);
	if (has_cipher_tag(vt->mod_name)) {
		vt->is_locked = module_is_locked(vt->mod_name);
		vt->cipher_old = get_cipher_key(vt->mod_name);
	}

	else {
		vt->is_locked = 0;
		vt->cipher_old = NULL;
	}
	popupmenu = gui_create_pm_text(vt);
	gnome_popup_menu_attach(popupmenu, vt->html, NULL);
	gtk_widget_show(vt->dialog);
	dialog_list = g_list_append(dialog_list, (TEXT_DATA *) vt);
	cur_vt = vt;
	gui_sync_bibletext_dialog(vt);
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

void gui_setup_bibletext_dialog(GList * mods)
{
	dialog_list = NULL;
	dialog_freed = FALSE;
	apply_change = FALSE;
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
		TEXT_DATA *vt = (TEXT_DATA *) dialog_list->data;
		dialog_freed = TRUE;
//              g_warning("shuting down %s dialog",vt->mod_name);
		/* 
		 *  destroy any dialogs created 
		 */
		if (vt->dialog)
			gtk_widget_destroy(vt->dialog);
		/* 
		 * free each TEXT_DATA item created 
		 */
		g_free(vt->mod_name);
		g_free(vt->tgs);
		g_free((TEXT_DATA *) dialog_list->data);
		dialog_list = g_list_next(dialog_list);
	}
	g_list_free(dialog_list);
}

/******   end of file   ******/
