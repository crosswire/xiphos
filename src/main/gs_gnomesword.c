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
#include <time.h>
#include <gal/widgets/e-unicode.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>


/* frontend */
#include "main_menu.h"
#include "about_modules.h"
#include "shortcutbar_main.h"
#include "shortcutbar_dialog.h"
#include "shortcutbar_viewer.h"
#include "studypad.h"
#include "info_box.h"

/* main */ 
#include "gs_gnomesword.h"
#include "settings.h"
#include "lists.h"
#include "bibletext.h"
#include "commentary.h"
#include "percomm.h"
#include "dictlex.h"
#include "gbs.h"
#include "gs_interlinear.h"
#include "gs_history.h"
#include "support.h"
#include "gs_html.h"
#include "gs_menu.h"
#include "gs_gui.h"
 
/* backend */
#include "search_.h"
#include "sword.h"
#include "interlinear.h"
#include "bibletext_.h"
#include "commentary_.h"
#include "percomm_.h"
#include "gbs_.h"
#include "dictlex_.h"
#include "properties.h"
#include "verselist.h"
#include "shortcutbar.h"
 
 
/*****************************************************************************
 * externs
 */

extern gboolean 
 havebible, 
 havecomm, 
 havedict, 
 havebook, 
 havepercomm, 
 autoSave, 
 addhistoryitem;	/* do we need to add item to history */
 
extern HISTORY historylist[];	/* sturcture for storing history items */
extern gint historyitems;

/******************************************************************************
 * globals
 */
gboolean ApplyChange;

/******************************************************************************
 * static
 */
static gchar *update_nav_controls(gchar * key);

/******************************************************************************
 * initGnomeSword - sets up the interface
 *****************************************************************************/
void init_gnomesword(SETTINGS * s)
{	
	

	/*
	 *  setup shortcut bar 
	 */
	gui_setup_shortcut_bar(s);
	/*
	 *  create popup menus -- gs_menu.c 
	 */
	createpopupmenus(s, get_list(TEXT_DESC_LIST),get_list(OPTIONS_LIST));
	/*
	 *  setup Bible text gui 
	 */
	if(havebible)
		setup_text(s,get_list(TEXT_LIST));

	/*
	 *  setup commentary gui support 
	 */
	if(havecomm)
		setup_commentary(s,get_list(COMM_LIST));
	/*
	 *  setup personal comments gui support 
	 */
	if(havepercomm)
		setup_percomm(s,get_list(PERCOMM_LIST));
	/*
	 *  setup general book gui support 
	 */
	if(havebook)
		setup_gbs(s,get_list(GBS_LIST));
	/*
	 *  setup Dict/Lex gui support 
	 */
	if(havedict)
		setup_dictlex(s,get_list(DICT_LIST));
	
	g_print("%s\n", "Initiating GnomeSWORD\n");
	
	s->settingslist = NULL;
	s->displaySearchResults = FALSE;
	/*
	 *  add modules to about modules menus -- gs_menu.c 
	 */
	addmodstomenus(s,
		       get_list(TEXT_LIST),
		       get_list(COMM_LIST), 
		       get_list(DICT_LIST), 
		       get_list(GBS_LIST));
		
	s->versestyle_item =
	    additemtooptionmenu(s->app, _("_Settings/"),
				_("Verse Style"), (GtkMenuCallback)
				on_verse_style1_activate);



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
		display_devotional(s);
	}
	
	g_print("done\n");
}

void gnomesword_shutdown(SETTINGS * s)
{
	GtkWidget *msgbox;
	gint answer = 0;

	
	/* if study pad file has changed since last save */
	if (s->modifiedSP) {
		msgbox = gui_create_info_box();
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer =
		    gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));

		switch (answer) {
		case 0:
			if (s->studypadfilename)
				save_file_program_end(s->html_studypad,
						      s->
						      studypadfilename);
			break;
		default:
			break;
		}
	}
	
	/* shutdown the sword stuff */
	backend_shutdown(s);
	
	shutdown_list();
	g_list_free(s->settingslist);
	
	/* free dir and file stuff */
	g_free(settings.gSwordDir);
	g_free(settings.shortcutbarDir);
	g_free(settings.fnconfigure);
	g_free(settings.swbmDir);
	
	if(havebible)
		shutdown_text();
	if(havebook)
		shutdown_gbs();
	if(havedict)
		shutdown_dictlex();
	if(havecomm)
		shutdown_commentary();
	if(havepercomm)
		shutdown_percomm();
	
	g_print("\nGnomeSWORD is shutdown\n");
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
		gui_attach_detach_shortcutbar(s);
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
	settings.notebook3page = page_num;/* store the page number so we can 
					     open to it the next time we start */
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
			text = settings.html_book;
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

/*gchar *get_module_name_from_description(gchar *description)
{
	gchar mod_name[16];

	memset(mod_name, 0, 16);
	backend_module_name_from_description(mod_name, description);
	if(mod_name)
		return g_strdup(mod_name);
	else 
		return NULL;
}*/

void module_name_from_description(gchar *mod_name, gchar *description)
{
	backend_module_name_from_description(mod_name, description);
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
	strcpy(settings.currentverse, val_key);
	/* 
	 *  set book, chapter,verse and freeform lookup entries
	 *  to new verse - ApplyChange is set to false so we don't
	 *  start a loop
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
		if(havebible) {
			page_num =
			    backend_get_module_page(module_name, 
							TEXT_MODS);
			val_key = update_nav_controls(key);
			set_text_page_and_key(page_num, val_key);
			g_free(val_key);
		}
		break;
	case COMMENTARY_TYPE:
		if(havecomm) {
			page_num =
			    backend_get_module_page(module_name, 
							COMM_MODS);
			set_commentary_page_and_key(page_num, key);
		}
		break;
	case DICTIONARY_TYPE:
		if(havedict) {
			page_num =
			    backend_get_module_page(module_name, 
							DICT_MODS);
			set_dictionary_page_and_key(page_num, key);
		}
		break;
	case BOOK_TYPE:
		if(havebook) {
			page_num =
			    backend_get_module_page(module_name, 
							BOOK_MODS);
			if(key)
				set_book_page_and_key(page_num, key);
			else {
				gtk_notebook_set_page(GTK_NOTEBOOK(
					settings.notebook_gbs),
					page_num);
			}
		}
		break;
	}
}

void change_verse(gchar * key)
{
	gchar *val_key;

	val_key = update_nav_controls(key);

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
	 * change interlinear verses 
	 */
	if (settings.dockedInt)
		update_interlinear_page(&settings);

	/* 
	 * change personal notes editor   if not in edit mode 
	 */
	if (settings.notefollow) {	                  
		if (!settings.editnote)
			if (havepercomm)
				display_percomm(val_key);
	}
	/* 
	 * set commentary module to current verse 
	 */
	if (havecomm)
		display_commentary(val_key);
	
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
	gchar buf[80];
	time_t curtime;
	struct tm *loctime;

	/* 
	 * Get the current time. 
	 */
	curtime = time(NULL);

	/* 
	 * Convert it to local time representation. 
	 */
	loctime = localtime(&curtime);

	/* 
	 * Print it out in a nice format. 
	 */
	strftime(buf, 80, "%m.%d", loctime);

	gui_display_dictlex_in_viewer(s->devotionalmod, buf, s);
	set_sb_for_daily_devotion(s);
}
/*** the changes are already made we just need to show them ***/
void display_new_font_color_and_size(SETTINGS * s)
{
	display_text(settings.currentverse);
	display_commentary(s->currentverse);
	display_dictlex(s->dictkey);
	update_interlinear_page(&settings);
}

gchar *get_module_description(gchar * mod_name)
{
	return backend_get_module_description(mod_name);
}


const char *get_book_from_key(char *key)
{
	return backend_get_book_from_key(key);
}

int get_chapter_from_key(char *key)
{
	return backend_get_chapter_from_key(key);
}

int get_verse_from_key(char *key)
{
	return backend_get_verse_from_key(key);
}

/******************************************************************************
 * Name
 *  save_properties
 *
 * Synopsis
 *   #include "gs_gnomesword.h"
 *
 *   void save_properties(gboolean use_default)	
 *
 * Description
 *   save properties stored in SETTINGS structure
 *
 * Return value
 *   void
 */

void save_properties(gboolean use_default)
{
	backend_save_properties(&settings, use_default);
}
GList *do_search(gpointer *usr_data)
{
	return backend_do_search(&settings, usr_data);
}

void display_sb_dictlex(gchar *modName, gchar *key)
{
	backend_display_sb_dictlex(modName, key);
}

GList *get_verse_list(gchar* module_name, gchar *verse_list)
{
	return backend_get_verse_list(module_name, verse_list, &settings);
}

void verselist_change_verse(gchar * url)
{
	backend_verselist_change_verse(&settings, url);
}

void display_search_results_item(gchar * key)
{
	backend_search_results_item_display(key);
}

GList *load_sb_group(gchar *filename, gchar *group_name, 
						gchar *icon_size)
{	
	return backend_load_sb_group(filename, group_name, icon_size);
}

void save_sb_group(gchar *file_name, gchar *group_name, gint group_num,
						     char *large_icons)
{
	backend_save_sb_group(file_name, group_name, group_num, large_icons);
}

void save_sb_iconsize(gchar *file_name, char *icons)
{
	backend_save_sb_iconsize(file_name, icons);
}

gint get_sb_type_from_modname(gchar *modName)
{
	return backend_sb_type_from_modname(modName);
}

void setup_shortcutbar_backend(GtkWidget *html, GtkWidget *html2, 
						GtkWidget *html3)
{
	backend_setup_verselist(html,&settings);
	backend_setup_search_results_display(html2);
	backend_setup_viewer(html3);
}





/******************************************************************************
 * Name
 *    get_shortcut_item_info
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void get_shortcut_item_info(GtkWidget *shortcutbar_widget, 
 *    gint group_num, gint item_num, gchar **item_url, gchar **item_name)	
 *
 * Description
 *   get shortcut item information
 *
 * Return value
 *   void
 */

void get_shortcut_item_info(GtkWidget *shortcutbar_widget, 
     gint group_num, gint item_num, gchar **item_url, gchar **item_name)
{
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				       (shortcutbar_widget)->model,
				       group_num,
				       item_num,
				       item_url, item_name, NULL);


}

/******************************************************************************
 * Name
 *    get_num_shortcut_items
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   gint get_num_shortcut_items(GtkWidget * shortcutbar_widget,
						gint group_num)	
 *
 * Description
 *   returns the number of shortcut items in the current group
 *
 * Return value
 *   gint
 */

gint get_num_shortcut_items(GtkWidget * shortcutbar_widget,
						gint group_num)
{
	return e_shortcut_model_get_num_items(E_SHORTCUT_BAR
					      (shortcutbar_widget)->
					      model, group_num);

}
