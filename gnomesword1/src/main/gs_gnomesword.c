/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_gnomesword.c
    * -------------------
    * Tue Dec 05 2000
    * copyright (C) 2001 by tbiggs
    * tbiggs@users.sourceforge.net
    *
  */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include <ctype.h>
#include <gal/widgets/e-unicode.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>

#include "gs_gnomesword.h"
#include "gs_interlinear.h"
#include "gs_history.h"
#include "sword.h"
#include "gs_gui_cb.h"
#include "gs_commentary.h"
#include "gs_gbs.h"
#include "gs_dictlex.h"
#include "gs_mainmenu_cb.h"
#include "support.h"
#include "gs_file.h"
#include "gs_info_box.h"
#include "gs_html.h"
#include "gs_menu.h"
#include "gs_shortcutbar.h"
#include "gs_bibletext.h"
#include "about_modules.h"
#include "search.h"
#include "interlinear.h"


/*****************************************************************************
* globals
*****************************************************************************/
GtkWidget *notepage,		/* widget to access toggle menu - for interlinear notebook page */
*autosaveitem,			/* widget to access toggle menu - for personal comments auto save */
*notes;				/* notes text widget */

gboolean file_changed = FALSE,	/* set to true if text is study pad has changed - and file is not saved  */
 changemain = TRUE,		/* change verse of Bible text window */
   ApplyChange = TRUE;
gchar *current_filename = NULL;	/* filename for open file in study pad window */

gchar current_verse[80] = N_("Romans 8:28");	/* current verse showing in main window - 1st - 2nd - 3rd interlinear window - commentary window */

gint dictpages, compages, textpages, bookpages;

SETTINGS *settings;
GList *sblist, /* for saving search results to bookmarks  */
    *biblemods,
    *commentarymods,
    *dictionarymods,
    *bookmods,
    *percommods,
    *sbfavoritesmods,
    *sbbiblemods, 
    *sbdictmods, 
    *sbcommods, 
    *sbbookmods, 
    *options;			
/*****************************************************************************
* externs
*****************************************************************************/
extern gchar *mydictmod, *shortcut_types[], rememberlastitem[];

extern gboolean havedict,	/* let us know if we have at least one lex-dict module */
 havecomm,			/* let us know if we have at least one commentary module */
 havebible,			/* let us know if we have at least one Bible text module */
 usepersonalcomments,		/* do we setup for personal comments - default is FALSE */
 autoSave, addhistoryitem,	/* do we need to add item to history */
 ApplyChange;
extern gint groupnum4, iquickmarks;	/* number of items in bookmark menu -- declared in gs_file.c */
extern HISTORY historylist[];	/* sturcture for storing history items */
extern gint historyitems;
extern GtkWidget *shortcut_bar;

/******************************************************************************
 * initGnomeSword - sets up the interface
 *****************************************************************************/
void
initGnomeSword(SETTINGS * s)
{
	GtkWidget *notebook;

	gint biblepage;


	g_print("%s\n", "Initiating GnomeSWORD\n");
	/*
	   set glist to null 
	 */
	biblemods = NULL;
	commentarymods = NULL;
	dictionarymods = NULL;
	percommods = NULL;
	sbfavoritesmods = NULL;
	sbbiblemods = NULL;
	sbcommods = NULL;
	sbdictmods = NULL;
	bookmods = NULL;
	sbbookmods = NULL;
	
	/*
	   fill module lists
	 */
	biblemods = backend_get_list_of_mods_by_type(TEXT_MODS);
	sbbiblemods = backend_get_mod_description_list_SWORD(TEXT_MODS);
	
	commentarymods = backend_get_list_of_mods_by_type(COMM_MODS);
	sbcommods = backend_get_mod_description_list_SWORD(COMM_MODS);
	
	dictionarymods = backend_get_list_of_mods_by_type(DICT_MODS);	
	sbdictmods = backend_get_mod_description_list_SWORD(DICT_MODS);
	
	bookmods = backend_get_list_of_mods_by_type(BOOK_MODS);
	sbbookmods = backend_get_mod_description_list_SWORD(BOOK_MODS);
	
	percommods = backend_get_list_of_percom_modules();
	if (percommods) {
		/*
		   show personal comments page 
		 */
		gtk_widget_show(lookup_widget(s->app, "vbox2"));
		/*
		   change personal comments module 
		 */
		backend_change_percom_module(s->personalcommentsmod);
	}
	/*
	   setup Bible text gui 
	 */
	gui_setup_text(s);
	/*
	   setup shortcut bar 
	 */
	setupSB(s);
	/*
	   setup commentary gui support 
	 */
	gui_setupCOMM(s);
	/*
	   setup general book gui support 
	 */
	gui_setupGBS(s);
	/*
	   setup Dict/Lex gui support 
	 */
	gui_setupDL(s);
	s->settingslist = NULL;
	s->displaySearchResults = FALSE;
	/*
	   add modules to menus -- gs_menu.c 
	 */
	addmodstomenus(s,
		       biblemods,
		       commentarymods, dictionarymods, bookmods,
		       percommods);
	/*
	   create popup menus -- gs_menu.c 
	 */
	createpopupmenus(s, sbbiblemods, sbcommods, sbdictmods,
			 options);
	additemstooptionsmenu(options, s);
	/*
	   add pages to module notebooks 
	 */
	 /*
	biblepage =
	    addnotebookpages(lookup_widget(s->app, "nbTextMods"),
			     biblemods, s->MainWindowModule);
	*/
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(s->app, "nbPerCom")), 0);
	/*
	   set text windows to word warp 
	 */
	gtk_text_set_word_wrap(GTK_TEXT
			       (lookup_widget(s->app, "textComments")),
			       TRUE);
	/*
	   store text widgets for spell checker 
	 */
	notes = lookup_widget(s->app, "textComments");

	s->versestyle_item =
	    additemtooptionmenu(s->app, _("_Settings/"),
				_("Verse Style"), (GtkMenuCallback)
				on_verse_style1_activate);

	/*
	   set Bible module to open notebook page 
	 */
	/*
	   let's don't do this if we don't have at least one text module 
	 */
	 /*
	if (havebible) {
		if (biblepage == 0)
			backend_change_text_module(s->MainWindowModule,
						   TRUE);
		
		notebook = lookup_widget(s->app, "nbTextMods");
		gtk_signal_connect(GTK_OBJECT(notebook), "switch_page",
				   GTK_SIGNAL_FUNC
				   (on_nbTextMods_switch_page), NULL);
		
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),
				      biblepage);
		if (settings->text_tabs)
			gtk_widget_show(notebook);
		else
			gtk_widget_hide(notebook);
	}*/

	//if (usepersonalcomments) {
		

	/*
	   free module lists 
	 */
	g_list_free(biblemods);
	g_list_free(commentarymods);
	g_list_free(dictionarymods);
	g_list_free(percommods);
	g_list_free(sbbiblemods);
	g_list_free(sbcommods);
	g_list_free(sbdictmods);
	g_list_free(bookmods);
	g_list_free(sbbookmods);
	/* 
	   options list freed on exit 
	 */

	g_print("done\n");

	/*
	 * Set toggle state of buttons and menu items.
	 */
	UpdateChecks(s);

	/* showing the devotional must come after the the app is shown or
	   it will mess up the shortcut bar display */
	/* FIXME: maybe we need to move the devotional ? */
	if (s->showdevotional) {
		backend_display_devotional(s);
	}
}

void gnomesword_shutdown(SETTINGS * s)
{
	GtkWidget *msgbox;
	extern gchar
	    * gSwordDir, *shortcutbarDir, *fnconfigure, *swbmDir;
	gint answer = 0;

	if (s->modifiedSP) {	//-- if study pad file has changed since last save  
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
	//-- free dir and file stuff 
	g_free(gSwordDir);
	g_free(shortcutbarDir);
	g_free(fnconfigure);
	g_free(swbmDir);

	//-- free glist
	g_list_free(options);
	g_list_free(s->settingslist);
	gui_shutdown_text();
	gui_shutdownGBS();
	gui_shutdownDL();
	gui_shutdownCOMM();
	g_print("\nwe are done with Gnomesword\n");
}

/*****************************************************************************
 * addnotebookpages - add pages to commentary and dictionary notebooks
 * notebook - notebook to add the pages to
 * list - list of modules - add one page per module
 *********************************************************************************************/
gint
addnotebookpages(GtkWidget * notebook, GList * modlist, gchar * modName)
{
	GList *tmp;
	gint pg = 0, retVal = 0;
	GtkWidget *empty_notebook_page,	/* used to create new pages */
	*label;
	GtkLabel *mylabel;

	tmp = modlist;
	while (tmp != NULL) {
		empty_notebook_page = gtk_vbox_new(FALSE, 0);
		gtk_widget_show(empty_notebook_page);
		gtk_container_add(GTK_CONTAINER(notebook),
				  empty_notebook_page);
		label = gtk_label_new((gchar *) tmp->data);
		mylabel = GTK_LABEL(label);
		if (!strcmp((gchar *) mylabel->label, modName))	/* set retVal to saved mod's page number */
			retVal = pg;
		gtk_widget_show(label);
		gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook),
					   gtk_notebook_get_nth_page
					   (GTK_NOTEBOOK(notebook), pg),
					   label);
		gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(notebook),
						 gtk_notebook_get_nth_page
						 (GTK_NOTEBOOK
						  (notebook), pg),
						 (gchar *) tmp->data);
		++pg;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return retVal;
}

/*****************************************************************************
 * UpdateChecks(GtkWidget *app) update chech menu items
 * and toggle buttons - called on start up
 *****************************************************************************/
void UpdateChecks(SETTINGS * s)
{
	/*
	   does user want verses or paragraphs 
	 */
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
	//GTK_CHECK_MENU_ITEM (autosaveitem)->active = settings->autosavepersonalcomments;  

	/*
	   show hide shortcut bar - set to options setting 
	 */
	if (s->showshortcutbar) {
		gtk_widget_show(s->shortcut_bar);
		e_paned_set_position(E_PANED
				     (lookup_widget(s->app, "epaned")),
				     s->shortcutbar_width);
	}

	else if (!s->showshortcutbar && s->showdevotional) {
		gtk_widget_show(s->shortcut_bar);
		on_btnSB_clicked(NULL, settings);
	}

	else {
		gtk_widget_hide(s->shortcut_bar);
		e_paned_set_position(E_PANED
				     (lookup_widget(s->app, "epaned")),
				     1);
	}

	/*
	   set hight of bible and commentary pane 
	 */
	e_paned_set_position(E_PANED(lookup_widget(s->app, "vpaned1")),
			     s->upperpane_hight);
	/*
	   set width of bible pane 
	 */
	e_paned_set_position(E_PANED(lookup_widget(s->app, "hpaned1")),
			     s->biblepane_width);

	if (!s->docked) {
		s->docked = TRUE;
		on_btnSBDock_clicked(NULL, s);
	}
	gtk_widget_show(s->app); /** display the whole thing **/

	
	addhistoryitem = FALSE;
	change_verse(s->currentverse);
}

/*****************************************************************************
 *setformatoption
 *button
*****************************************************************************/
void setformatoption(GtkWidget * button)
{
	settings->formatpercom =
	    GTK_TOGGLE_BUTTON(GTK_BUTTON(button))->active;
}

/*****************************************************************************
 *changepagenotebook - someone changed the page in the main notebook
 *notebook - notebook widget - main notebook
 *page_num - notebook page number
*****************************************************************************/
void changepagenotebook(GtkNotebook * notebook, gint page_num)
{
	settings->notebook3page = page_num;	/* store the page number so we can open to it the next time we start */
	changemain = FALSE;	/* we don't want to cause the Bible text window to scrool */
	//if(page_num == 4) change_verse(current_verse); /* if we changed to page 0, 1 or 2 */
}


/*****************************************************************************
 * showIntPage - do we want to see interlinear page?
 * choice
*****************************************************************************/
void showIntPage(GtkWidget * app, gboolean choice)
{
	GtkWidget *intpage, *frame;	/* pointer to interlinear notebook page */

	intpage = lookup_widget(app, "vboxInt");	/* set pointer to page */
	frame = lookup_widget(app, "frame2");	/* set pointer to page */
	if (choice) {
		gtk_widget_show(intpage);	/* show page */
		gtk_widget_show(frame);	/* show page */
	} else {
		gtk_widget_hide(intpage);	/* hide page */
		gtk_widget_hide(frame);
	}
	settings->interlinearpage = choice;	/* remember choice for next program startup */
}


/*****************************************************************************
 * setautosave - someone clicked auto save personal  comments
 * choice
*****************************************************************************/
void setautosave(gboolean choice)
{
	if (choice) {		/* if choice was to autosave */
		autoSave = TRUE;
	} else {		/* if choice was not to autosave    */
		autoSave = FALSE;
	}
	settings->autosavepersonalcomments = choice;	/* remember our choice for next startup */
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
		gnome_appbar_set_progress((GnomeAppBar *) settings->
					  appbar, num);
		printed++;
	}
	while (gtk_events_pending())
		gtk_main_iteration();
	printed = 0;
}

/*****************************************************************************
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

/*****************************************************************************
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

/*****************************************************************************
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
			text = settings->htmlBook;
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

void search_module(SETTINGS * s, SEARCH_OPT * so)
{
	if (sblist)
		g_list_free(sblist);
	sblist = NULL;
	sblist = backend_do_search(s, so);
	fill_search_results_clist(sblist, so, s);
}

/******************************************************************************
 *
 * num
 * returns module key
******************************************************************************/
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
 *
 * num
 * returns module name
******************************************************************************/
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

void gui_set_text_mod_page(gint page)
{
	GtkWidget *notebook = lookup_widget(settings->app,
					    "nbTextMods");
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), page);
	backend_text_module_change_verse(settings->currentverse);
}

void change_module_and_key(gchar * module_name, gchar * key)
{
	gint mod_type;
	gint page_num;
	GtkWidget *notebook;

	mod_type = backend_get_mod_type(module_name);

	switch (mod_type) {
	case TEXT_TYPE:
		page_num = backend_get_module_page(module_name, TEXT_MODS);
		gui_set_text_page_and_key(page_num, key);
		break;
	case COMMENTARY_TYPE:
		page_num =
		    backend_get_module_page(module_name, COMM_MODS);
		gui_set_commentary_page_and_key(page_num, key);
		break;
	case DICTIONARY_TYPE:
		page_num =
		    backend_get_module_page(module_name, DICT_MODS);
		gui_set_dictionary_page_and_key(page_num, key);
		break;
	case BOOK_TYPE:
		page_num =
		    backend_get_module_page(module_name, BOOK_MODS);
		gui_set_book_page_and_key(page_num, key);
		break;
	}
}

void set_module_global_options(gchar * option, gint window,
			       gboolean choice, gboolean showchange)
{
	gchar *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}

	

	switch (window) {
	case MAIN_TEXT_WINDOW:
		backend_save_module_options(settings->MainWindowModule,
					    option, on_off);

		if (!strcmp(option, "Strong's Numbers")) {
			GTK_CHECK_MENU_ITEM(settings->strongsnum)->
			    active = choice;
			/* set strongs toogle button */
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (lookup_widget
						      (settings->app,
						       "btnStrongs")),
						     choice);
		}

		if (!strcmp(option, "Footnotes")) {
			GTK_CHECK_MENU_ITEM(settings->footnotes)->
			    active = choice;
			/* set footnotes toogle button */
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (lookup_widget
						      (settings->app,
						       "btnFootnotes")),
						     choice);
		}

		if (!strcmp(option, "Morphological Tags")) {
			GTK_CHECK_MENU_ITEM(settings->morphs)->active =
			    choice;
			/* set morphs toogle button */
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (lookup_widget
						      (settings->app,
						       "btnMorphs")),
						     choice);
		}

		if (!strcmp(option, "Hebrew Vowel Points")) {
			GTK_CHECK_MENU_ITEM(settings->hebrewpoints)->
			    active = choice;
			//settings->hebrewpoints = choice;
		}

		if (!strcmp(option, "Hebrew Cantillation")) {
			GTK_CHECK_MENU_ITEM(settings->
					    cantillationmarks)->active =
			    choice;
			//settings->cantillationmarks = choice;
		}

		if (!strcmp(option, "Greek Accents")) {
			GTK_CHECK_MENU_ITEM(settings->greekaccents)->
			    active = choice;
			//settings->greekaccents = choice;
		}

		if (havebible && showchange) {	/* display change */
			backend_text_module_change_verse(settings->
							 currentverse);
		}
		backend_set_text_global_option(option, on_off);
		break;

	case INTERLINEAR_WINDOW:

		if (!strcmp(option, "Strong's Numbers")) {
			settings->strongsint = choice;
		}

		if (!strcmp(option, "Footnotes")) {
			settings->footnotesint = choice;
		}

		if (!strcmp(option, "Morphological Tags")) {
			settings->morphsint = choice;
		}

		if (!strcmp(option, "Hebrew Vowel Points")) {
			settings->hebrewpointsint = choice;
		}

		if (!strcmp(option, "Hebrew Cantillation")) {
			settings->cantillationmarksint = choice;
		}

		if (!strcmp(option, "Greek Accents")) {
			settings->greekaccentsint = choice;
		}
		
		backend_set_interlinear_global_option(option, on_off);
		/* 
		   display change 
		 */
		if (settings->dockedInt && havebible)	
			update_interlinear_page(settings);
		else
			update_interlinear_page_detached(settings);

		break;
	}

}

void change_verse(gchar * key)
{
	gchar *val_key;
	gint cur_chapter = 8,	
	 cur_verse = 28;	
	gchar s1[255];

	val_key = backend_get_valid_key(key);
	sprintf(current_verse, "%s", val_key);
	g_warning(val_key);
	ApplyChange = FALSE;
	/* 
	   change main window 
	 */
	if (havebible) {
		if (addhistoryitem) {
			if (strcmp
			    (settings->currentverse,
			     historylist[historyitems - 1].verseref))
				addHistoryItem(settings->app,
					       GTK_WIDGET
					       (shortcut_bar),
					       settings->currentverse);
		}
		addhistoryitem = TRUE;
		/* remember last verse */
		sprintf(settings->currentverse, "%s", val_key);
		cur_chapter = backend_get_chapter_from_key(val_key);
		cur_verse = backend_get_verse_from_key(val_key);
		sprintf(s1, "%s", val_key);
		/* set book, chapter,verse and freeform lookup entries to new verse */
		gtk_entry_set_text(GTK_ENTRY(lookup_widget
					     (settings->app,
					      "cbeBook")),
				   backend_get_book_from_key(val_key));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON
					  (lookup_widget
					   (settings->app,
					    "spbChapter")),
					  cur_chapter);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON
					  (lookup_widget
					   (settings->app,
					    "spbVerse")), cur_verse);
		gtk_entry_set_text(GTK_ENTRY
				   (lookup_widget
				    (settings->app,
				     "cbeFreeformLookup")), val_key);
		gui_display_text(val_key);
	}
	changemain = TRUE;

	/* change interlinear verses */
	if (settings->dockedInt)
		update_interlinear_page(settings);

	/* change personal notes editor   if not in edit mode */
	
	/* set commentary module to current verse */
	gui_displayCOMM(settings->currentverse);
	g_free(val_key);
	ApplyChange = TRUE;
}

void save_module_key(gchar *mod_name, gchar * key)
{		
	backend_save_module_key(mod_name, key);
	
	/*
	   FIXME: we need to display change
	 */

}
/*****   end of file   ******/
