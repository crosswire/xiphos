/*
 * GnomeSword Bible Study Tool
 * gnomesword.c - glue
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
#include <ctype.h>
#include <gal/widgets/e-unicode.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>

#include "gs_gnomesword.h"
#include "gs_interlinear.h"
#include "gs_history.h"
#include "gs_gui_cb.h"
#include "gs_gbs.h"
#include "gs_dictlex.h"
#include "support.h"
#include "gs_info_box.h"
#include "gs_html.h"
#include "gs_menu.h"
#include "gs_shortcutbar.h"

/*
 * gnome
 */
#include "main_menu.h"
#include "about_modules.h"
/*
 * main
 */ 
#include "percomm.h"
#include "bibletext.h"
#include "settings.h"
#include "commentary.h"
 
/*
 * backend
 */
#include "search_.h"
#include "sword.h"
#include "interlinear.h"
#include "bibletext_.h"
#include "percomm_.h"
#include "gbs.h"
#include "dictlex.h"
 
 
/*****************************************************************************
 * externs
 */

extern gboolean havebible, /* do we have at least one Bible text module */
 autoSave, addhistoryitem;	/* do we need to add item to history */
 
extern HISTORY historylist[];	/* sturcture for storing history items */
extern gint historyitems;

/******************************************************************************
 * globals
 */
gboolean changemain = TRUE; /* change verse of Bible text window */
gboolean ApplyChange;
gchar current_verse[80] = N_("Romans 8:28");
MOD_LISTS *mod_lists;
GList *sblist;			/* for saving search results to bookmarks  */
MOD_LISTS mods;

/******************************************************************************
 * static
 */
static gchar *update_nav_controls(gchar * key);

/******************************************************************************
 * initGnomeSword - sets up the interface
 *****************************************************************************/
void init_gnomesword(SETTINGS * s)
{	
	g_print("%s\n", "Initiating GnomeSWORD\n");
	mod_lists = &mods;

	/* set glist to null */
	mod_lists->biblemods = NULL;
	mod_lists->commentarymods = NULL;
	mod_lists->dictionarymods = NULL;
	mod_lists->percommods = NULL;
	mod_lists->bookmods = NULL;
	mod_lists->options = NULL;
	mod_lists->text_descriptions = NULL;
	mod_lists->comm_descriptions = NULL;
	mod_lists->dict_descriptions = NULL;
	mod_lists->book_descriptions = NULL;

	/* fill module lists */
	mod_lists->options = backend_get_global_options_list();
	mod_lists->text_descriptions = backend_get_mod_description_list_SWORD(TEXT_MODS);
	mod_lists->comm_descriptions = backend_get_mod_description_list_SWORD(COMM_MODS);
	mod_lists->dict_descriptions = backend_get_mod_description_list_SWORD(DICT_MODS);
	mod_lists->book_descriptions = backend_get_mod_description_list_SWORD(BOOK_MODS);
	
	/*
	 *  setup shortcut bar 
	 */
	setupSB(s);
	/*
	 *  create popup menus -- gs_menu.c 
	 */
	createpopupmenus(s, mod_lists->text_descriptions, mod_lists->options);
	/*
	 *  setup Bible text gui 
	 */
	mod_lists->biblemods = setup_text(s);

	/*
	 *  setup commentary gui support 
	 */
	mod_lists->commentarymods = setup_commentary(s);
	/*
	 *  setup personal comments gui support 
	 */
	mod_lists->percommods = setup_percomm(s);
	/*
	 *  setup general book gui support 
	 */
	mod_lists->bookmods = gui_setup_gbs(s);
	/*
	 *  setup Dict/Lex gui support 
	 */
	mod_lists->dictionarymods = gui_setup_dict(s);

	s->settingslist = NULL;
	s->displaySearchResults = FALSE;
	/*
	 *  add modules to about modules menus -- gs_menu.c 
	 */
	addmodstomenus(s,
		       mod_lists->biblemods,
		       mod_lists->commentarymods, 
		       mod_lists->dictionarymods, 
		       mod_lists->bookmods);
		
	s->versestyle_item =
	    additemtooptionmenu(s->app, _("_Settings/"),
				_("Verse Style"), (GtkMenuCallback)
				on_verse_style1_activate);

	g_print("done\n");

	/*
	 * Set toggle state of buttons and menu items.
	 */
	UpdateChecks(&settings);

	/* showing the devotional must come after the the app is shown or
	 *  it will mess up the shortcut bar display 
	 */
	/* 
	 * FIXME: maybe we need to move the devotional ? 
	 */
	if (s->showdevotional) {
		backend_display_devotional(s);
	}
}

void gnomesword_shutdown(SETTINGS * s)
{
	GtkWidget *msgbox;
	gint answer = 0;

	/* if study pad file has changed since last save */
	if (s->modifiedSP) {
		msgbox = create_InfoBox();
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer =
		    gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));

		switch (answer) {
		case 0:
			if (s->studypadfilename)
				save_file_program_end(s->htmlSP,
						      s->
						      studypadfilename);
			break;
		default:
			break;
		}
	}
	
	/* free lists */
	g_list_free(mod_lists->biblemods);
	g_list_free(mod_lists->commentarymods);
	g_list_free(mod_lists->dictionarymods);
	g_list_free(mod_lists->bookmods);
	g_list_free(mod_lists->percommods);
	g_list_free(mod_lists->text_descriptions);
	g_list_free(mod_lists->dict_descriptions);
	g_list_free(mod_lists->comm_descriptions);
	g_list_free(mod_lists->book_descriptions);
	g_list_free(mod_lists->options);
	g_list_free(s->settingslist);

	/* free dir and file stuff */
	g_free(settings.gSwordDir);
	g_free(settings.shortcutbarDir);
	g_free(settings.fnconfigure);
	g_free(settings.swbmDir);
	
	shutdown_text();
	gui_shutdownGBS();
	gui_shutdownDL();
	shutdown_commentary();
	shutdown_percomm();
	
	g_print("\nwe are done with Gnomesword\n");
}

/*****************************************************************************
 * UpdateChecks(GtkWidget *app) update chech menu items
 * and toggle buttons - called on start up
 *****************************************************************************/

void UpdateChecks(SETTINGS * s)
{
	/* does user want verses or paragraphs */
	GTK_CHECK_MENU_ITEM(s->versestyle_item)->active = s->versestyle;

	if (s->footnotesint)
		backend_set_global_option(INTERLINEAR_WINDOW, "Footnotes", "On");	/* keep footnotes in sync with menu */
	else
		backend_set_global_option(INTERLINEAR_WINDOW, "Footnotes", "Off");	/* keep footnotes in sync with menu */

	/*
	   set interlinear Strong's Numbers to last setting used 
	 */
	if (s->strongsint)
		backend_set_global_option(INTERLINEAR_WINDOW, "Strong's Numbers", "On");	/* keep Strongs in sync with menu */
	else
		backend_set_global_option(INTERLINEAR_WINDOW, "Strong's Numbers", "Off");	/* keep Strongs in sync with menu */

	/*
	   set interlinear morph tags to last setting used 
	 */
	if (s->morphsint)
		backend_set_global_option(INTERLINEAR_WINDOW, "Morphological Tags", "On");	/* keep Morph Tags in sync with menu */
	else
		backend_set_global_option(INTERLINEAR_WINDOW, "Morphological Tags", "Off");	/* keep Morph Tag in sync with menu */

	/*
	   set interlinear Hebrew Vowel Points to last setting used 
	 */
	if (s->hebrewpointsint)
		backend_set_global_option(INTERLINEAR_WINDOW, "Hebrew Vowel Points", "On");	/* keep Hebrew Vowel Points in sync with menu */
	else
		backend_set_global_option(INTERLINEAR_WINDOW, "Hebrew Vowel Points", "Off");	/* keep Hebrew Vowel Points in sync with menu */

	/*
	   set interlinear Hebrew Cantillation to last setting used 
	 */
	if (s->cantillationmarksint)
		backend_set_global_option(INTERLINEAR_WINDOW, "Hebrew Cantillation", "On");	/* keep Hebrew Cantillation in sync with menu */
	else
		backend_set_global_option(INTERLINEAR_WINDOW, "Hebrew Cantillation", "Off");	/* keep Hebrew Cantillation in sync with menu */

	/*
	   set interlinear Greek Accents to last setting used 
	 */
	if (s->greekaccentsint)
		backend_set_global_option(INTERLINEAR_WINDOW, "Greek Accents", "On");	/* keep Greek Accents in sync with menu */
	else
		backend_set_global_option(INTERLINEAR_WINDOW, "Greek Accents", "Off");	/* keep Greek Accents in sync with menu */

	/*
	   set auto save personal comments to last setting 
	 */
	autoSave = s->autosavepersonalcomments;
	/*
	   set auto save menu check item 
	 */
	//GTK_CHECK_MENU_ITEM (autosaveitem)->active = settings.autosavepersonalcomments;  

	/*
	   show hide shortcut bar - set to options setting 
	 */
	if (s->showshortcutbar) {
		gtk_widget_show(s->shortcut_bar);
		e_paned_set_position(E_PANED(s->epaned),
				     s->shortcutbar_width);
	}

	else if (!s->showshortcutbar && s->showdevotional) {
		gtk_widget_show(s->shortcut_bar);
		on_btnSB_clicked(NULL, &settings);
	}

	else {
		gtk_widget_hide(s->shortcut_bar);
		e_paned_set_position(E_PANED(s->epaned),
				     1);
	}

	/* set hight of bible and commentary pane */
	e_paned_set_position(E_PANED(lookup_widget(s->app, "vpaned1")),
			     s->upperpane_hight);

	/* set width of bible pane */
	e_paned_set_position(E_PANED(lookup_widget(s->app, "hpaned1")),
			     s->biblepane_width);

	if (!s->docked) {
		s->docked = TRUE;
		dock_undock(s);
	}
	gtk_widget_show(s->app);

	addhistoryitem = FALSE;
	change_verse(s->currentverse);
}

/******************************************************************************
 * setformatoption
 * button
 *****************************************************************************/

void setformatoption(GtkWidget * button)
{
	settings.formatpercom =
	    GTK_TOGGLE_BUTTON(GTK_BUTTON(button))->active;
}

/******************************************************************************
 * changepagenotebook - someone changed the page in the main notebook
 * notebook - notebook widget - main notebook
 * page_num - notebook page number
 *****************************************************************************/

void changepagenotebook(GtkNotebook * notebook, gint page_num)
{
	settings.notebook3page = page_num;	/* store the page number so we can open to it the next time we start */
	changemain = FALSE;	/* we don't want to cause the Bible text window to scrool */
}


/******************************************************************************
 * setautosave - someone clicked auto save personal  comments
 * choice
 *****************************************************************************/

void setautosave(gboolean choice)
{
	if (choice) {
		/* if choice was to autosave */
		autoSave = TRUE;
	} else {
		/* if choice was not to autosave */
		autoSave = FALSE;
	}

	/* remember our choice for next startup */
	settings.autosavepersonalcomments = choice;
}



void percent_update(char percent, void *userData)
{
	char maxHashes = *((char *) userData);
	float num;
	char buf[80];
	static char printed = 0;

	while (gtk_events_pending())
		gtk_main_iteration();
	while ((((float) percent) / 100) * maxHashes > printed) {
		sprintf(buf, "%f", (((float) percent) / 100));
		num = (float) percent / 100;
		gnome_appbar_set_progress((GnomeAppBar *) settings.appbar, num);
		printed++;
	}
	while (gtk_events_pending())
		gtk_main_iteration();
	printed = 0;
}

/******************************************************************************
 *   -string_is_color- this code is from bluefish-0.6
 *
 *****************************************************************************/

gint string_is_color(gchar * color)
{
	gint i;

	if (!color) {
		g_warning("string_is_color, pointer NULL\n");
		return 0;
	}
	if (strlen(color) != 7) {
		g_warning("string_is_color, strlen(%s) != 7\n", color);
		return 0;
	}
	if (color[0] != '#') {
		g_warning("string_is_color, 0 in %s is not #\n", color);
		return 0;
	}
	for (i = 1; i < 7; i++) {
		if ((color[i] > 102)
		    || (color[i] < 48)
		    || ((color[i] > 57) && (color[i] < 65))
		    || ((color[i] > 70) && (color[i] < 97))) {
			g_warning
			    ("string_is_color, %d in %s is not from a color, it is %d\n",
			     i, color, color[i]);
			return 0;
		}
	}
	//g_warning("string_is_color, %s is color\n", color);
	return 1;

}

/******************************************************************************
 * gdouble_arr_to_hex  -- this code is from bluefish-0.6
 *
 *****************************************************************************/

gchar *gdouble_arr_to_hex(gdouble * color, gint websafe)
{
	gchar *tmpstr;
	unsigned int red_int;
	unsigned int green_int;
	unsigned int blue_int;
	gdouble red;
	gdouble green;
	gdouble blue;

	red = color[0];
	green = color[1];
	blue = color[2];

	if (websafe) {
		red_int = 0x33 * ((unsigned int) (red * 255 / 0x33));
		green_int =
		    0x33 * ((unsigned int) (green * 255 / 0x33));
		blue_int = 0x33 * ((unsigned int) (blue * 255 / 0x33));
	} else {
		red_int = (unsigned int) (red * 255);
		green_int = (unsigned int) (green * 255);
		blue_int = (unsigned int) (blue * 255);
	}
	tmpstr = g_malloc(8 * sizeof(char));
	g_snprintf(tmpstr, 8, "#%.2X%.2X%.2X", red_int, green_int,
		   blue_int);
	return tmpstr;
}

/******************************************************************************
 *  hex_to_gdouble_arr -- this code is from bluefish-0.6
 *
 *****************************************************************************/

gdouble *hex_to_gdouble_arr(gchar * color)
{
	static gdouble tmpcol[4];
	gchar tmpstr[8];
	long tmpl;


	strncpy(tmpstr, &color[1], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[0] = (gdouble) tmpl;

	strncpy(tmpstr, &color[3], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[1] = (gdouble) tmpl;

	strncpy(tmpstr, &color[5], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[2] = (gdouble) tmpl;

	//g_warning("hex_to_gdouble_arr, R=%d, G=%d, B=%d\n", color[0], color[1], color[2]);

	tmpcol[3] = 0;

	return tmpcol;
}


/*
 * to display Sword module about information
 */
static void about_module_display(gchar * to, gchar * text)
{
	gint len, maxlen, i;
	gboolean center = FALSE;

	len = strlen(text);

	maxlen = len * 80;


	// -------------------------------
	for (i = 0; i < strlen(text) - 1; i++) {
		if (text[i] == '\\')	// a RTF command
		{
			if ((text[i + 1] == 'p') &&
			    (text[i + 2] == 'a') &&
			    (text[i + 3] == 'r') &&
			    (text[i + 4] == 'd')) {

				if (center) {
					*to++ = '<';
					*to++ = '/';
					*to++ = 'C';
					*to++ = 'E';
					*to++ = 'N';
					*to++ = 'T';
					*to++ = 'E';
					*to++ = 'R';
					*to++ = '>';
					center = FALSE;
				}
				i += 4;
				continue;
			}
			if ((text[i + 1] == 'p') && (text[i + 2] == 'a')
			    && (text[i + 3] == 'r')) {
				*to++ = '<';
				*to++ = 'b';
				*to++ = 'r';
				*to++ = '>';
				*to++ = '\n';
				i += 3;
				continue;
			}
			if (text[i + 1] == ' ') {
				i += 1;
				continue;
			}
			if (text[i + 1] == '\n') {
				i += 1;
				continue;
			}
			if ((text[i + 1] == 'q')
			    && (text[i + 2] == 'c')) {
				if (!center) {
					*to++ = '<';
					*to++ = 'C';
					*to++ = 'E';
					*to++ = 'N';
					*to++ = 'T';
					*to++ = 'E';
					*to++ = 'R';
					*to++ = '>';
					center = TRUE;
				}
				i += 2;
				continue;
			}
		}
		*to++ = text[i];
	}
	*to++ = 0;
}

void display_about_module_dialog(gchar * modname, gboolean isGBS)
{
	GtkWidget *aboutbox = NULL;	//-- pointer to about dialog        
	GtkWidget *text;	//-- pointer to text widget of dialog
	gchar *buf, *to = NULL,	//-- pointer to text buffer for label (mod name)
	*bufabout,		//-- pointer to text buffer for text widget (mod about)
	 discription[500];
	gint len, maxlen;

	bufabout = NULL;

	buf = backend_get_module_description(modname);
	bufabout = backend_get_mod_aboutSWORD(modname);

	sprintf(discription,
		"<FONT COLOR=\"#000FCF\"><center><b>%s</b></center></font><HR>",
		buf);
	if (!isGBS) {
		aboutbox = gui_create_about_modules();
		gtk_widget_show(aboutbox);
	}

	if (bufabout) {

		len = strlen(bufabout);
		maxlen = len * 8;

		if ((to = (gchar *) malloc(maxlen)) == NULL) {
			return;
		}

		if (!isGBS) {
			text = lookup_widget(aboutbox, "text");	/* get text widget */
		} else {
			text = settings.htmlBook;
		}

		about_module_display(to, bufabout);	/* send about info to display function */
		beginHTML(text, FALSE);
		displayHTML(text, "<html><body>",
			    strlen("<html><body>"));
		displayHTML(text, discription, strlen(discription));
		if (to)
			displayHTML(text, to, strlen(to));
		displayHTML(text, "</body></html>",
			    strlen("</body></html>"));
		endHTML(text);
	}

	else
		g_warning("oops");

	if (bufabout)
		g_free(bufabout);
	if (to)
		free(to);
}


/******************************************************************************
 *
 * num
 * returns module key
 *****************************************************************************/

gchar *get_module_key(SETTINGS * s)
{
	if (havebible) {
		switch (s->whichwindow) {
		case MAIN_TEXT_WINDOW:
			return (gchar *) s->currentverse;
			break;
		case COMMENTARY_WINDOW:
			return (gchar *) s->comm_key;
			break;
		case DICTIONARY_WINDOW:
			return (gchar *) s->dictkey;
			break;
		case INTERLINEAR_WINDOW:
			return (gchar *) s->cvInterlinear;
			break;
		case BOOK_WINDOW:
			return (gchar *) s->book_key;
			break;
		}
	}
	return NULL;
}

/******************************************************************************
 * num
 * returns module name
 *****************************************************************************/

gchar *get_module_name(SETTINGS * s)
{
	if (havebible) {
		switch (s->whichwindow) {
		case MAIN_TEXT_WINDOW:
			return (gchar *) s->MainWindowModule;
			break;
		case COMMENTARY_WINDOW:
			return (gchar *) s->CommWindowModule;
			break;
		case DICTIONARY_WINDOW:
			return (gchar *) s->DictWindowModule;
			break;
		case BOOK_WINDOW:
			return (gchar *) s->BookWindowModule;
			break;
		}
	}
	return NULL;
}

/******************************************************************************
 *
 * 
 * 
 */

gchar *get_module_name_from_description(gchar *description)
{
	gchar mod_name[16];

	memset(mod_name, 0, 16);
	backend_module_name_from_description(mod_name, description);
	if(mod_name)
		return g_strdup(mod_name);
	else 
		return NULL;
}

static gchar *update_nav_controls(gchar * key)
{
	char *val_key;
	gint cur_chapter = 8, cur_verse = 28;

	ApplyChange = FALSE;
	val_key = backend_get_valid_key(key);
	cur_chapter = backend_get_chapter_from_key(val_key);
	cur_verse = backend_get_verse_from_key(val_key);
	/* 
	 *  remember last verse 
	 */
	sprintf(settings.currentverse, "%s", val_key);
	/* 
	 *  set book, chapter,verse and freeform lookup entries to new verse 
	 */
	gtk_entry_set_text(GTK_ENTRY(settings.cbeBook),
			   backend_get_book_from_key(val_key));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (settings.spbChapter), cur_chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (settings.spbVerse), cur_verse);
	gtk_entry_set_text(GTK_ENTRY
			   (settings.cbeFreeformLookup), val_key);
	ApplyChange = TRUE;
	return val_key;
}

void change_module_and_key(gchar * module_name, gchar * key)
{
	gint mod_type;
	gint page_num;
	gchar *val_key = NULL;

	mod_type = backend_get_mod_type(module_name);

	switch (mod_type) {
	case TEXT_TYPE:
		page_num =
		    backend_get_module_page(module_name, TEXT_MODS);
		val_key = update_nav_controls(key);
		set_text_page_and_key(page_num, val_key);
		g_free(val_key);
		break;
	case COMMENTARY_TYPE:
		page_num =
		    backend_get_module_page(module_name, COMM_MODS);
		set_commentary_page_and_key(page_num, key);
		break;
	case DICTIONARY_TYPE:
		page_num =
		    backend_get_module_page(module_name, DICT_MODS);
		gui_set_dictionary_page_and_key(page_num, key);
		break;
	case BOOK_TYPE:
		page_num =
		    backend_get_module_page(module_name, BOOK_MODS);
		if(key)
			gui_set_book_page_and_key(page_num, key);
		else {
			gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebookGBS),
			      page_num);
		}
		break;
	}
}

void change_verse(gchar * key)
{
	gchar *val_key;

	val_key = update_nav_controls(key);
	sprintf(current_verse, "%s", val_key);

	ApplyChange = FALSE;
	
	if (havebible) {
		/* add item to history */
		if (addhistoryitem) {
			if (strcmp
			    (settings.currentverse,
			     historylist[historyitems - 1].verseref))
				addHistoryItem(settings.app,
					       GTK_WIDGET
					       (settings.shortcut_bar),
					       settings.currentverse);
		}
		addhistoryitem = TRUE;

		/* change main window */
		display_text(val_key);
	}

	/* 
	   change interlinear verses 
	 */
	if (settings.dockedInt)
		update_interlinear_page(&settings);

	/* 
	   change personal notes editor   if not in edit mode 
	 */
	if (settings.notefollow) {	                  
		if (!settings.editnote)
			display_percomm(val_key);
	}
	/* 
	   set commentary module to current verse 
	 */
	display_commentary(settings.currentverse);
	
	g_free(val_key);
	ApplyChange = TRUE;
}

void save_module_key(gchar * mod_name, gchar * key)
{
	backend_save_module_key(mod_name, key);

	/* FIXME: we need to display change */
}

/******************************************************************************
 * set verse style -- verses or paragraphs
 *****************************************************************************/

void set_verse_style(gboolean choice)
{
	/* remember our choice for the next program startup */
	settings.versestyle = choice;

	if (havebible) {
		/* show the change */
		display_text(settings.currentverse);
	}
}

const char *get_sword_version(void)
{
	return backend_get_sword_version();
}

void display_devotional(SETTINGS * s)
{
	backend_display_devotional(s);
}
