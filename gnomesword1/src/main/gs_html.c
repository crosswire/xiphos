/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_html.c 
    * -------------------
    * Thu Feb 8 2001
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
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlobject.h>
#include <gtkhtml/htmlengine-edit-clueflowstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-rule.h>
#include <gtkhtml/htmlengine-edit-movement.h>
#include <gtkhtml/htmlengine-edit-cursor.h>
#include <gtkhtml/htmlengine-edit-text.h>
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-print.h>
#include <gtkhtml/htmlengine-save.h>
#include <gtkhtml/htmlsettings.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlcursor.h>
#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/gtkhtml-search.h>
#include <gal/widgets/e-unicode.h>
#include <libgnomeprint/gnome-printer.h>
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-printer-dialog.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>
#include <libgnomeprint/gnome-print-preview.h>

#include "gs_html.h"
#include "support.h"
#include "sword.h"
#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_editor.h"
#include "gs_interlinear.h"
#include "verselist.h"
#include "shortcutbar.h"

GtkHTMLStream *htmlstream;
GtkHTMLStreamStatus status1;

GtkWidget *htmlCommentaries;
GtkWidget *htmlTexts;
GtkWidget *htmlDict;
GtkWidget *textDict;
GtkWidget *htmlComments;
GtkWidget *usehtml;

extern GtkWidget *textDict;
extern SETTINGS *settings;
gboolean in_url;

GtkWidget *gs_new_html_widget(SETTINGS * s)
{
	GtkWidget *html;

	html = gtk_html_new();
	gtk_html_load_empty(GTK_HTML(html));
	gtk_signal_connect(GTK_OBJECT(html), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(html), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) s->app);
	return html;
}

/*
 *
 */
void on_url(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar buf[255];

	/***  moved out of url - clear appbar  ***/
	if (url == NULL) {
		gnome_appbar_set_status(GNOME_APPBAR(settings->appbar),
					"");
		in_url = FALSE;
	}
	/***  we are in an url  ***/
	else {
		in_url = TRUE;	/* we need this for html_button_released */
		/***  swap interlinear and main text mods link ***/
		if (*url == '@') {
			++url;
			sprintf(buf, _("Show %s in main window"), url);
		}
		/***  module name link  ***/
		else if (*url == '[') {
			++url;
			while (*url != ']') {
				++url;
			}
			++url;
			sprintf(buf, "%s", url);
		}
		/***  verse number link  ***/
		else if (*url == '*') {
			++url;
			sprintf(buf, "%s", url);
		}
		/***  any other link  ***/
		else
			sprintf(buf, _("Go to %s"), url);

		gnome_appbar_set_status(GNOME_APPBAR(settings->appbar),
					buf);
	}
}

/*
 *link in commentary module clicked
 */
void on_link_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar *buf = NULL, *modbuf = NULL, tmpbuf[255];
	gchar newmod[80], newref[80];
	gint i = 0, havemod = 0;


	if (*url == '@') {
		++url;
		swap_interlinear_with_main((gchar *) url, settings);
	}
	/***  verse numbers in Bible Text window  ***/
	else if (*url == '*') {
		++url;
		while (*url != ']') {
			++url;
		}
		++url;
		buf = g_strdup(url);
		change_verse(buf);
		g_free(buf);

	}

	else if (*url == 'I') {
		++url;
		buf = g_strdup(url);
		change_verse(buf);
		g_free(buf);
	}
	/***  module name  ***/
	else if (*url == '[') {
		++url;
		while (*url != ']') {
			tmpbuf[i++] = *url;
			tmpbuf[i + 1] = '\0';
			++url;
		}
		display_about_module_dialog(tmpbuf, FALSE);

	}
	/*** thml verse reference ***/
	else if (!strncmp(url, "version=", 7)
		 || !strncmp(url, "passage=", 7)) {
		gchar *mybuf = NULL;
		mybuf = strstr(url, "version=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			i = 0;
			while (mybuf[i] != ' ') {
				newmod[i] = mybuf[i];
				newmod[i + 1] = '\0';
				++i;
				++havemod;
			}
		}
		mybuf = NULL;
		mybuf = strstr(url, "passage=");
		i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			while (i < strlen(mybuf)) {
				newref[i] = mybuf[i];
				newref[i + 1] = '\0';
				++i;
			}
		}
		if (havemod > 2) {
			modbuf = newmod;
		} else {
			modbuf = settings->MainWindowModule;
		}
		buf = g_strdup(newref);
		sprintf(settings->groupName, "%s", "Verse List");

		if (backend_get_mod_type(modbuf) == DICTIONARY_TYPE) {
			/* we have a dict/lex module 
			   so we don't need to get a verse list */
			display_dictlex_in_viewer(modbuf, buf, settings);
		} else {
			display_verse_list(modbuf, buf, settings);
		}
		g_free(buf);

	}
	/***  thml morph tag  ***/
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
		}
		buf = g_strdup(mybuf);
		if (settings->inDictpane)
			change_module_and_key(modbuf, buf);
		if (settings->inViewer)
			display_dictlex_in_viewer(modbuf, buf, settings);
		g_free(buf);
	}
	/*** thml strongs ***/
	else if (!strncmp(url, "type=Strongs", 12)) {
		gchar *modbuf = NULL;
		gchar *mybuf = NULL;
		gint type = 0;
		//buf = g_strdup(url);
		mybuf = NULL;
		mybuf = strstr(url, "value=");
		i = 0;
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
			modbuf = settings->lex_greek;
		else
			modbuf = settings->lex_hebrew;

		buf = g_strdup(newref);
		if (settings->inDictpane)
			change_module_and_key(modbuf, buf);
		if (settings->inViewer)
			display_dictlex_in_viewer(modbuf, buf, settings);
		g_free(buf);

	}
	/***  gbf strongs  ***/
	else if (*url == '#') {
		++url;		/* remove # */
		if (*url == 'T') {
			++url;	/* remove T */
			if (*url == 'G') {
				++url;	/* remove G */
				if (settings->havethayer) {
					buf = g_strdup(url);
					if (settings->inDictpane)
						change_module_and_key("Thayer",
								  buf);
					if (settings->inViewer)
						display_dictlex_in_viewer
						    ("Thayer", buf,
						     settings);
					g_free(buf);
					return;
				}

				else
					return;
			}

			if (*url == 'H') {
				++url;	/* remove H */
				if (settings->havebdb) {
					buf = g_strdup(url);
					if (settings->inDictpane)
						change_module_and_key("BDB",
								  buf);
					if (settings->inViewer)
						display_dictlex_in_viewer("BDB",
								   buf,
								   settings);
					g_free(buf);
					return;
				}

				else
					return;
			}
		}

		if (*url == 'G') {
			++url;	/* remove G */
			buf = g_strdup(url);
			if (atoi(buf) > 5624) {
				if (settings->havethayer) {
					buf = g_strdup(url);
					if (settings->inDictpane)
						change_module_and_key("Thayer",
								  buf);
					if (settings->inViewer)
						display_dictlex_in_viewer
						    ("Thayer", buf,
						     settings);
					g_free(buf);
					return;
				} else
					return;

			}

			else {
				if (settings->inDictpane)
					change_module_and_key(settings->
							  lex_greek, buf);
				if (settings->inViewer)
					display_dictlex_in_viewer(settings->
							   lex_greek_viewer,
							   buf, settings);
				g_free(buf);
			}
		}

		if (*url == 'H') {
			++url;	/* remove H */
			buf = g_strdup(url);
			if (atoi(buf) > 8674) {
				if (settings->havebdb) {
					buf = g_strdup(url);
					if (settings->inDictpane)
						change_module_and_key("BDB",
								  buf);
					if (settings->inViewer)
						display_dictlex_in_viewer("BDB",
								   buf,
								   settings);
					g_free(buf);
					return;
				}

				else
					return;
			}

			else {
				if (settings->inDictpane)
					change_module_and_key(settings->
							  lex_hebrew, buf);
				if (settings->inViewer)
					display_dictlex_in_viewer(settings->
							   lex_hebrew_viewer,
							   buf, settings);
				g_free(buf);
			}
		}
	}
	/***  gbf morph tag  ***/
	else if (*url == 'M') {
		++url;		/* remove M */
		buf = g_strdup(url);
		if (settings->inDictpane)
			change_module_and_key("Packard", buf);
		if (settings->inViewer)
			display_dictlex_in_viewer("Packard", buf, settings);
		g_free(buf);
	}
}

/*
 * 
 */
static gint
html_button_pressed(GtkWidget * html, GdkEventButton * event,
		    gpointer * data)
{
	usehtml = html;
	settings->whichwindow = GPOINTER_TO_INT(data);

	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}
	return TRUE;

}/******************************************************************************

 * 
 ******************************************************************************/
static gint
html_button_released(GtkWidget * html, GdkEventButton * event,
		     gpointer * data)
{
	gchar *key;

	usehtml = html;
	settings->whichwindow = GPOINTER_TO_INT(data);

	switch (event->button) {
	case 1:
		if (!in_url) {
			key = buttonpresslookupGS_HTML(html);
			if (key) {
				gchar *dict = NULL;
				if (settings->useDefaultDict)
					dict =
					    g_strdup(settings->
						     DefaultDict);
				else
					dict =
					    g_strdup(settings->
						     DictWindowModule);
				if (settings->inViewer)
					display_dictlex_in_viewer(dict, key,
							   settings);
				if (settings->inDictpane)
					change_module_and_key(dict, key);
				g_free(key);
				if (dict)
					g_free(dict);
			}
			return TRUE;
		}
		break;
	case 2:
		break;
	case 3:		/*
				   gnome_popup_menu_do_popup(settings->menuBible,
				   NULL,
				   NULL,
				   event,
				   NULL); */
		break;
	}
	return TRUE;

}

/***************************************************************************************************
 *copy menu item clicked in gbs
 *html_widget - (GtkHTML widget) to copy from
 ***************************************************************************************************/
void copyGS_HTML(GtkWidget * html_widget)
{
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	gtk_html_copy(html);
}

/***************************************************************************************************
 *copy menu item clicked in any html window
 *user_data - window (GtkHTML widget) to copy from
 ***************************************************************************************************/
void on_copyhtml_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *widget;
	GtkHTML *html;

	if (!strcmp((gchar *) user_data, "textComp1"))
		widget = settings->htmlInterlinear;
	else
		widget = lookup_widget(settings->app, (gchar *) user_data);

	html = GTK_HTML(widget);
	gtk_html_copy(html);
}

/******************************************************************************
 * get word or selection from html widget
 */
gchar *get_word_or_selection(GtkWidget * html_widget, gboolean word)
{
	gchar *key = NULL;
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	if (word)
		gtk_html_select_word(html);

	if (html_engine_is_selection_active(html->engine)) {
		key = html_engine_get_selection_string(html->engine);
		key = g_strdelimit(key, ".,\"<>;:", ' ');
		key = g_strstrip(key);
		return g_strdup(key);	/* must be freed by calling function */
	}
	return key;
}

/***************************************************************************************************
 *lookup word in dict/lex module
 ***************************************************************************************************/
gchar *buttonpresslookupGS_HTML(GtkWidget * html_widget)
{
	gchar *key = NULL;
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	if (!html_engine_is_selection_active(html->engine)) {
		gtk_html_select_word(html);
		if (html_engine_is_selection_active(html->engine)) {
			key =
			    html_engine_get_selection_string(html->engine);
			key = g_strdelimit(key, ".,\"<>;:", ' ');
			key = g_strstrip(key);
			return g_strdup(key);	/* must be freed by calling function */
		}

	}
	return key;
}

/***************************************************************************************************
 *lookup word in dict/lex module
 ***************************************************************************************************/
void on_html_lookup_word_activate(GtkMenuItem * menuitem, gchar * modDesc)
{
	gchar *key = NULL, modName[16];

	if (strcmp(modDesc, "current")) {
				    /***  if menu choice was not 'use current dict' ***/
		memset(modName, 0, 16);
		backend_module_name_from_description(modName, modDesc);
	}

	else {
		sprintf(modName, "%s", settings->DictWindowModule);
	}

	key = get_word_or_selection(usehtml, TRUE);
					/***  usehtml is set when a button is pressed
						in a html window  ***/
	if (key) {
		if (settings->inViewer)
			display_dictlex_in_viewer(modName, key, settings);
		if (settings->inDictpane)
			change_module_and_key(modName, key);
		g_free(key);
	}

}

/***************************************************************************************************
 *lookup selection in current dict/lex module
 ***************************************************************************************************/
void on_html_lookup_selection_activate(GtkMenuItem * menuitem,
				       gchar * modDesc)
{
	gchar *key = NULL, modName[16];

	if (strcmp(modDesc, "current")) {
				    /***  if menu choice was not 'use current dict' ***/
		memset(modName, 0, 16);
		backend_module_name_from_description(modName, modDesc);
	}

	else {
		sprintf(modName, "%s", settings->DictWindowModule);
	}

	key = get_word_or_selection(usehtml, FALSE);
					 /***  usehtml is set when a button is pressed
						in a html window  ***/
	if (key) {
		if (settings->inViewer)
			display_dictlex_in_viewer(modName, key, settings);
		if (settings->inDictpane)
			change_module_and_key(modName, key);
		g_free(key);
	}

}

/***************************************************************************************************
 *goto selected Bible reference
 ***************************************************************************************************/
void on_html_goto_reference_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
{
	GtkWidget *widget, *entry;
	GtkHTML *html;
	gchar *buf, *modbuf;

	widget = lookup_widget(settings->app, (gchar *) user_data);
	html = GTK_HTML(widget);
	gtk_html_copy(html);
	entry = lookup_widget(settings->app, "cbeFreeformLookup");
	/* clear entry */
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	/* put selected ref in entry */
	gtk_editable_paste_clipboard(GTK_EDITABLE(GTK_ENTRY(entry)));
	buf = gtk_entry_get_text(GTK_ENTRY(entry));
	/* get name for current text module */
	settings->whichwindow = MAIN_TEXT_WINDOW;
	modbuf = get_module_name(settings);
	display_verse_list(modbuf, buf, settings);
}

/***************************************************************************************************
 *add_gtkhtml_widgets -- add the gthhtml widgets
 ***************************************************************************************************/
void add_gtkhtml_widgets(GtkWidget * app)
{
	htmlTexts = gtk_html_new();
	gtk_widget_ref(htmlTexts);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "htmlTexts", htmlTexts,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlTexts);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app, "swHtmlBible")),
			  htmlTexts);
	gtk_html_load_empty(GTK_HTML(htmlTexts));
/*
	htmlCommentaries = gtk_html_new();
	gtk_widget_ref(htmlCommentaries);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "htmlCommentaries", htmlCommentaries,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlCommentaries);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app, "swHtmlCom")),
			  htmlCommentaries);
*/
	usehtml = htmlTexts;


/*	
	settings->htmlInterlinear = gtk_html_new();
	gtk_widget_ref(settings->htmlInterlinear);
	gtk_object_set_data_full(GTK_OBJECT(app), "settings->htmlInterlinear",
				 settings->htmlInterlinear,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(settings->htmlInterlinear);
	
		gtk_container_add(GTK_CONTAINER
			  (settings->swIntDocked),
			  settings->htmlInterlinear);
	if(settings->dockedInt)
		gtk_container_add(GTK_CONTAINER
			  (settings->swIntDocked),
			  settings->htmlInterlinear);
*/
/*	htmlDict = gtk_html_new();
	gtk_widget_ref(htmlDict);
	gtk_object_set_data_full(GTK_OBJECT(app), "htmlDict",
				 htmlDict,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlDict);
	gtk_container_add(GTK_CONTAINER
			  (lookup_widget(app, "scrolledwindow8")),
			  htmlDict);
*/

	gtk_signal_connect(GTK_OBJECT(htmlTexts), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(htmlTexts), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) app);
	gtk_signal_connect(GTK_OBJECT(htmlTexts), "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(htmlTexts), "button_release_event",
			   GTK_SIGNAL_FUNC(html_button_released),
			   GINT_TO_POINTER(0));
/*
	gtk_signal_connect(GTK_OBJECT(htmlCommentaries), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(htmlCommentaries), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) app);
	gtk_signal_connect(GTK_OBJECT(htmlCommentaries),
			   "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed),
			   GINT_TO_POINTER(1));
	gtk_signal_connect(GTK_OBJECT(htmlCommentaries),
			   "button_release_event",
			   GTK_SIGNAL_FUNC(html_button_released),
			   GINT_TO_POINTER(0));
*/
/*
	gtk_signal_connect(GTK_OBJECT(htmlComments), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(htmlComments), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) app);
	gtk_signal_connect(GTK_OBJECT(htmlComments), "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), NULL);*/
/*
	gtk_signal_connect(GTK_OBJECT(settings->htmlInterlinear), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) app);
	gtk_signal_connect(GTK_OBJECT(settings->htmlInterlinear), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(settings->htmlInterlinear), "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), NULL);

	gtk_signal_connect(GTK_OBJECT(htmlDict), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) app);
	gtk_signal_connect(GTK_OBJECT(htmlDict), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(htmlDict), "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed),
			   GINT_TO_POINTER(2));
*/
	/*gtk_signal_connect(GTK_OBJECT(lookup_widget(settings->app,"htmlBooks")), "button_press_event",
	   GTK_SIGNAL_FUNC(html_button_pressed), NULL); */
}

/***************************************************************************************************
 *beginHTML - start loading html widget
 ***************************************************************************************************/
void beginHTML(GtkWidget * html_widget, gboolean isutf8)
{
	GtkHTML *html;
	gboolean was_editable;

	html = GTK_HTML(html_widget);
	was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	if (isutf8) {
		htmlstream =
		    gtk_html_begin_content(html,
					   "text/html; charset=utf-8");
	} else {
		htmlstream = gtk_html_begin(html);
	}
	gtk_html_set_editable(html, was_editable);
}

/***************************************************************************************************
 *endHTML
 ***************************************************************************************************/
void endHTML(GtkWidget * html)
{
	gtk_html_end(GTK_HTML(html), htmlstream, status1);
}

/***************************************************************************************************
 *displayHTML
 ***************************************************************************************************/
void displayHTML(GtkWidget * html, const gchar * txt, gint lentxt)
{
	if (strlen(txt)) {
		gtk_html_write(GTK_HTML(html), htmlstream, txt, lentxt);
	}
}

/***************************************************************************************************
 *gotoanchorHTML
 ***************************************************************************************************/
void gotoanchorHTML(GtkWidget * html_widget, gchar * verse)
{
	gtk_html_jump_to_anchor(GTK_HTML(html_widget), verse);
}

/***************************************************************************************************
 *sethtmltoeditHTML
 ***************************************************************************************************/
void sethtmltoeditHTML(gboolean choice)
{
	gtk_html_set_editable(GTK_HTML(htmlComments), choice);
}

/***************************************************************************************************
 * get html fontname from x fontname (gnome font picker name)
 ***************************************************************************************************/
gchar *gethtmlfontnameHTML(gchar * xfontname)
{
	gchar *token, *retval;
	++xfontname;
	token = strtok(xfontname, "-");
	token = strtok(NULL, "-");
	retval = token;
	return retval;
}

/***  printing using gtk_html_print_with_header_footer()  ***/
static gint page_num;
static GnomeFont *font;
static void
print_footer(GtkHTML * html, GnomePrintContext * context,
	     gdouble x, gdouble y, gdouble width, gdouble height,
	     gpointer user_data)
{
	gchar *text = g_strdup_printf("- %d -", page_num);
	gdouble tw = gnome_font_get_width_string(font, "text");

	if (font) {
		gnome_print_newpath(context);
		gnome_print_setrgbcolor(context, .0, .0, .0);
		gnome_print_moveto(context, x + (width - tw) / 2,
				   y - (height +
					gnome_font_get_ascender(font)) /
				   2);
		gnome_print_setfont(context, font);
		gnome_print_show(context, text);
	}

	g_free(text);
	page_num++;
}

void html_print(GtkWidget * htmlwidget)
{
	GnomePrintMaster *print_master;
	GnomePrintContext *print_context;
	GtkWidget *preview;
	GtkHTML *html;

	html = GTK_HTML(htmlwidget);

	print_master = gnome_print_master_new();
	print_context = gnome_print_master_get_context(print_master);

	page_num = 1;
	font =
	    gnome_font_new_closest("Helvetica", GNOME_FONT_BOOK, FALSE,
				   12);
	gtk_html_print_with_header_footer(html, print_context, .0, .03,
					  NULL, print_footer, NULL);
	if (font)
		gtk_object_unref(GTK_OBJECT(font));

	preview =
	    GTK_WIDGET(gnome_print_master_preview_new
		       (print_master, "GnomeSword Print Preview"));
	gtk_widget_show(preview);

	gtk_object_unref(GTK_OBJECT(print_master));
}

/*** toogle htmlwidget edit mode ***/
void set_html_edit(GtkWidget * htmlwidget)
{
	gboolean choice;

	if (gtk_html_get_editable(GTK_HTML(htmlwidget)))
		choice = FALSE;
	else
		choice = TRUE;
	gtk_html_set_editable(GTK_HTML(htmlwidget), choice);
}
