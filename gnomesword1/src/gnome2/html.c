/*
 * GnomeSword Bible Study Tool
 * html.c - GtkHtml gui stuff
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
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine-print.h>
#include <gtkhtml/htmlselection.h>
#include <gal/widgets/e-unicode.h>
#include <libgnomeprint/gnome-print.h>
#include <fcntl.h>

#include "gui/html.h"
#include "gui/gnomesword.h"
#include "gui/editor.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/shortcutbar_main.h"
#include "gui/parallel_view.h"
#include "gui/utilities.h"
#include "gui/about_modules.h"
#include "gui/main_window.h"
#include "gui/cipher_key_dialog.h"
#include "gui/dialog.h"
#include "gui/bibletext.h"
#include "gui/commentary_menu.h"
#include "gui/hints.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/xml.h"

static GtkHTMLStream *htmlstream;
static GtkHTMLStreamStatus status1;
gboolean in_url;



/******************************************************************************
 * Name
 *   url_requested
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void url_requested (GtkHTML *html, const gchar *url, 
 *					GtkHTMLStream *handle)
 *
 * Description
 *   needed for displaying images
 *
 * Return value
 *   void
 */

void url_requested (GtkHTML *html, const gchar *url, 
					GtkHTMLStream *handle)
{
	GtkHTMLStreamStatus status;
	gint fd;

	if (!strncmp (url, "file:", 5))
		url += 5;

	fd = open (url, O_RDONLY);
	status = GTK_HTML_STREAM_OK;
	if (fd != -1) {
		ssize_t size;
		void *buf = alloca (1 << 7);
		while ((size = read (fd, buf, 1 << 7))) {
			if (size == -1) {
				status = GTK_HTML_STREAM_ERROR;
				break;
			} else
				gtk_html_write (html, handle, (const gchar *) buf, size);
		}
	} else
		status = GTK_HTML_STREAM_ERROR;
	gtk_html_end (html, handle, status);
}

/******************************************************************************
 * Name
 *   show_in_appbar
 *
 * Synopsis
 *   #include "gui/html.h"
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
	text = get_striptext(4, mod, key);
	str = remove_linefeeds(text);
	if (str) {
		gnome_appbar_set_status(GNOME_APPBAR(appbar), str);
		g_free(str);
	}
	g_free(text);
}


/******************************************************************************
 * Name
 *   gui_url
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_url(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Description
 *   called when mouse moves over an url (link)
 *
 * Return value
 *   void
 */

void gui_url(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar buf[255], *buf1;
	gchar *tmpbuf;
	gboolean is_strongsmorph = FALSE;

	/***  moved out of url - clear appbar  ***/
	if (url == NULL) {
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
					"");
		in_url = FALSE;
		if(hint.in_popup) {
			gtk_widget_destroy(hint.hint_window);
			hint.in_popup = FALSE;
		}
	}
	/***  we are in an url  ***/
	else {
		in_url = TRUE;	/* we need this for html_button_released */
		/***  swap parallel and main text mods link ***/
		if (*url == '@') {
			++url;
			sprintf(buf, _("Show %s in main window"), url);
		}
		/***  osis footnote  ***/
		else if (!strncmp(url, "noteID=", 7)) {
			//g_warning(url);
			buf1 = strchr(url,'=');
			++buf1;
			tmpbuf = get_footnote_body(buf1);
			if(tmpbuf == NULL)
				return;	
			else {
				gnome_appbar_set_status(GNOME_APPBAR(
					widgets.appbar),tmpbuf);
				if(hint.use_hints) {
					//gui_display_hint_in_viewer(tmpbuf);
					gui_display_in_hint_window(tmpbuf);
				}
				
				g_free(tmpbuf);				
				return;				
			}					
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
		} /***  gbf strongs  ***/
		else if (*url == '#') {
			++url;	/* remove # */
			if (*url == 'T') {
				++url;	/* remove T */
				if (*url == 'G') {
					++url;	/* remove G */
					if (settings.havethayer) {
						buf1 = g_strdup(url);
						show_in_appbar(widgets.
							       appbar,
							       buf1,
							       "Thayer");
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
						show_in_appbar(widgets.
							       appbar,
							       buf1,
							       "BDB");
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
						show_in_appbar(widgets.
							       appbar,
							       buf1,
							       "Thayer");
						g_free(buf1);
						return;
					} else
						return;

				}

				else {
					show_in_appbar(widgets.appbar,
						       buf1,
						       settings.
						       lex_greek);
					g_free(buf1);
					return;
				}
			}

			if (*url == 'H') {
				++url;	/* remove H */
				buf1 = g_strdup(url);
				if (atoi(buf1) > 8674) {
					if (settings.havebdb) {
						show_in_appbar(widgets.
							       appbar,
							       buf1,
							       "BDB");
						g_free(buf1);
						return;
					}

					else
						return;
				}

				else {
					show_in_appbar(widgets.appbar,
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
			gchar *myurl = NULL;
			gchar *oldnew = NULL;
			
			oldnew = g_strdup(url);
			myurl = g_strdup(url);
			buf1 = g_strdup(myurl);

			mybuf = strstr(myurl, "class=");
			if (mybuf) {
				modbuf = strchr(mybuf, '=');
				++modbuf;
				if(modbuf[0] == 'x' && modbuf[1] == '-')
					modbuf += 2;
				if (!strncmp(modbuf, "Robinson", 7)) {	
					modbuf = "Robinson";
				} 
				else if(!strncmp(modbuf, "StrongsMorph", 11)) {
					is_strongsmorph = TRUE;
					if(strstr(oldnew,"value=TH"))
						modbuf = settings.lex_hebrew;
					else
						modbuf = settings.lex_greek;
				}
			}
			else 
				modbuf = "Robinson";

			mybuf = NULL;
			mybuf = strstr(buf1, "value=");
			if (mybuf) {
				mybuf = strchr(mybuf, '=');
				++mybuf;
			}
			if(is_strongsmorph) {				
				++mybuf;
				++mybuf;
			} 
			buf1 = g_strdup(mybuf);
			g_warning("show %s in %s",buf1,modbuf);
			mybuf = NULL;
			if(hint.use_hints) {
				mybuf = get_module_text(get_mod_type(modbuf), modbuf, buf1);
				if(mybuf) {
					gui_display_in_hint_window(mybuf);
					//gui_display_hint_in_viewer(mybuf);
					g_free(mybuf);
				}
			}
			show_in_appbar(widgets.appbar, buf1, modbuf);
			g_free(buf1);
			g_free(myurl);
			g_free(oldnew);
			return;
		}
		/*** thml strongs ***/
		else if (!strncmp(url, "type=Strongs", 12)) {
			//g_warning(url);
			//return;
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
				if ((atoi(mybuf) > 5624)
				    && (settings.havethayer))
					modbuf = "Thayer";
				else
					modbuf = settings.lex_greek;
			else
				modbuf = settings.lex_hebrew;

			buf1 = g_strdup(newref);
			mybuf = NULL;
			if(hint.use_hints) {
				mybuf = get_module_text(get_mod_type(modbuf), modbuf, buf1);
				if(mybuf) {
					//gui_display_hint_in_viewer(mybuf);
					gui_display_in_hint_window(mybuf);
					g_free(mybuf);
				}
			}
			show_in_appbar(widgets.appbar, buf1, modbuf);
			g_free(buf1);
			return;
		}

		else if (*url == 'U') {
			++url;
			sprintf(buf, "%s %s", _("Unlock "), url);
		}
		/***  any other link  ***/
		else
			sprintf(buf, "%s %s", _("Go to "), url);

		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
					buf);

	}
}


/******************************************************************************
 * Name
 *   gui_link_clicked
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_link_clicked(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

void gui_link_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar *buf = NULL, *modbuf = NULL, tmpbuf[255];
	gchar newmod[80], newref[80];
	gchar *oldnew = NULL;
	gint i = 0;
	gboolean is_strongsmorph = FALSE;

	if (*url == '@') {
		++url;
		gui_swap_parallel_with_main((gchar *) url);
	}
	/***  verse numbers in Bible Text window  ***/
	else if (*url == '*') {
		++url;
		buf = g_strdup(url);
		gui_change_verse(buf);
		g_free(buf);
	}

	else if (*url == 'I') {
		++url;
		buf = g_strdup(url);
		gui_change_verse(buf);
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
		gui_display_about_module_dialog(tmpbuf, FALSE);

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
		if (check_for_module(newmod)) {
			modbuf = newmod;
		} else {
			modbuf = xml_get_value("modules", "bible");	//settings.MainWindowModule;
		}
		buf = g_strdup(newref);
		sprintf(settings.groupName, "%s", "Verse List");

		if (get_mod_type(modbuf) == DICTIONARY_TYPE) {
			/* we have a dict/lex module 
			   so we don't need to get a verse list */
			gui_display_dictlex_in_viewer(modbuf, buf);
		} else {
			gui_display_verse_list(settings.currentverse, modbuf, buf);
		}
		g_free(buf);

	}
	/***  thml morph tag  ***/
	else if (!strncmp(url, "type=morph", 10)) {
		gchar *modbuf = NULL;
		gchar *mybuf = NULL;

		oldnew = g_strdup(url);
		buf = g_strdup(url);
		mybuf = strstr(url, "class=");
		if (mybuf) {
			modbuf = strchr(mybuf, '=');
			++modbuf;
			if(modbuf[0] == 'x' && modbuf[1] == '-')
				modbuf += 2;
			if (!strncmp(modbuf, "Robinson", 7)) {	
				modbuf = "Robinson";
			} 
			else if(!strncmp(modbuf, "StrongsMorph", 11)) {
				is_strongsmorph = TRUE;
				if(strstr(oldnew,"value=TH"))
					modbuf = settings.lex_hebrew;
				else
					modbuf = settings.lex_greek;
			}
		}
		else
			modbuf = "Robinson";
		mybuf = NULL;
		mybuf = strstr(buf, "value=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
		}
		if(is_strongsmorph) {				
			++mybuf;
			++mybuf;
		} 
		buf = g_strdup(mybuf);
		if (settings.inDictpane)
			gui_change_module_and_key(modbuf, buf);
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(modbuf, buf);
		g_free(buf);
		g_free(oldnew);
	}
	/*** thml strongs ***/
	else if (!strncmp(url, "type=Strongs", 12)) {
		gchar *modbuf = NULL;
		gchar *modbuf_viewer = NULL;
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
		if (type) {
			if ((atoi(mybuf) > 5624)
			    && (settings.havethayer)) {
				modbuf = "Thayer";
				modbuf_viewer = "Thayer";
			} else {
				modbuf_viewer =
				    settings.lex_greek_viewer;
				modbuf = settings.lex_greek;
			}
		} else {
			modbuf = settings.lex_hebrew;
			modbuf_viewer = settings.lex_hebrew_viewer;
		}

		buf = g_strdup(newref);
		if (settings.inDictpane)
			gui_change_module_and_key(modbuf, buf);
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(modbuf_viewer,
						      buf);
		g_free(buf);
	}
	/***  gbf strongs  ***/
	else if (*url == '#') {
		++url;		/* remove # */
		if (*url == 'T') {
			++url;	/* remove T */
			if (*url == 'G') {
				++url;	/* remove G */
				if (settings.havethayer) {
					buf = g_strdup(url);
					if (settings.inDictpane)
						gui_change_module_and_key
						    ("Thayer", buf);
					if (settings.inViewer)
						gui_display_dictlex_in_viewer
						    ("Thayer", buf);
					g_free(buf);
					return;
				}

				else
					return;
			}

			if (*url == 'H') {
				++url;	/* remove H */
				if (settings.havebdb) {
					buf = g_strdup(url);
					if (settings.inDictpane)
						gui_change_module_and_key
						    ("BDB", buf);
					if (settings.inViewer)
						gui_display_dictlex_in_viewer
						    ("BDB", buf);
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
				if (settings.havethayer) {
					buf = g_strdup(url);
					if (settings.inDictpane)
						gui_change_module_and_key
						    ("Thayer", buf);
					if (settings.inViewer)
						gui_display_dictlex_in_viewer
						    ("Thayer", buf);
					g_free(buf);
					return;
				} else
					return;

			}

			else {
				if (settings.inDictpane)
					gui_change_module_and_key
					    (settings.lex_greek, buf);
				if (settings.inViewer)
					gui_display_dictlex_in_viewer
					    (settings.lex_greek_viewer,
					     buf);
				g_free(buf);
			}
		}

		if (*url == 'H') {
			++url;	/* remove H */
			buf = g_strdup(url);
			if (atoi(buf) > 8674) {
				if (settings.havebdb) {
					buf = g_strdup(url);
					if (settings.inDictpane)
						gui_change_module_and_key
						    ("BDB", buf);
					if (settings.inViewer)
						gui_display_dictlex_in_viewer
						    ("BDB", buf);
					g_free(buf);
					return;
				}

				else
					return;
			}

			else {
				if (settings.inDictpane)
					gui_change_module_and_key
					    (settings.lex_hebrew, buf);
				if (settings.inViewer)
					gui_display_dictlex_in_viewer
					    (settings.lex_hebrew_viewer,
					     buf);
				g_free(buf);
			}
		}
	}
	/***  gbf morph tag  ***/
	else if (*url == 'M') {
		++url;		/* remove M */
		buf = g_strdup(url);
		if (settings.inDictpane)
			gui_change_module_and_key("Packard", buf);
		if (settings.inViewer)
			gui_display_dictlex_in_viewer("Packard", buf);
		g_free(buf);
	}

	else if (*url == 'U') {
		++url;		/* remove U */
		buf = g_strdup(url);
		if (get_mod_type(buf) == TEXT_TYPE) {
			gui_unlock_bibletext(NULL, (TEXT_DATA *) data);
		} else if (get_mod_type(buf) == COMMENTARY_TYPE) {
			gui_unlock_commentary(NULL, (COMM_DATA *) data);
		}
		g_free(buf);
	}
}


/******************************************************************************
 * Name
 *   gui_copy_html
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_copy_html(GtkWidget * html_widget)
 *
 * Description
 *   copy menu item clicked in gbs
 *   html_widget - (GtkHTML widget) to copy from
 *
 * Return value
 *   
 */

void gui_copy_html(GtkWidget * html_widget)
{
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	gtk_html_copy(html);
}


/******************************************************************************
 * Name
 *   gui_copyhtml_activate
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_copyhtml_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   copy menu item clicked in any html window
 *   user_data - window (GtkHTML widget) to copy from   
 *
 * Return value
 *   void
 */

void gui_copyhtml_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkHTML *html;

	switch (settings.whichwindow) {
	case MAIN_TEXT_WINDOW:
		html = GTK_HTML(widgets.html_text);
		break;
	case parallel_WINDOW:
		html = GTK_HTML(widgets.html_parallel);
		break;
	case COMMENTARY_WINDOW:
		html = GTK_HTML(widgets.html_comm);
		break;
	case DICTIONARY_WINDOW:
		html = GTK_HTML(widgets.html_dict);
		break;
	case BOOK_WINDOW:
		html = GTK_HTML(widgets.html_book);
		break;
	case PERCOMM_WINDOW:
		html = GTK_HTML(widgets.html_percomm);
		break;
	case STUDYPAD_WINDOW:
		html = GTK_HTML(widgets.html_studypad);
		break;
	default:
		html = GTK_HTML(widgets.html_text);
	}

	gtk_html_copy(html);
}


/******************************************************************************
 * Name
 *   gui_get_word_or_selection
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   gchar *gui_get_word_or_selection(GtkWidget * html_widget, gboolean word)
 *
 * Description
 *   get word or selection from html widget
 *
 * Return value
 *   gchar *
 */

gchar *gui_get_word_or_selection(GtkWidget * html_widget, gboolean word)
{
	gchar *key = NULL;
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	if (word)
		gtk_html_select_word(html);

	if (html_engine_is_selection_active(html->engine)) {
		key = html_engine_get_selection_string(html->engine);
		key = g_strdelimit(key, ".,\"<>;:?", ' ');
		key = g_strstrip(key);
		return g_strdup(key);	/* must be freed by calling function */
	}
	return key;
}


/******************************************************************************
 * Name
 *   gui_button_press_lookup
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   gchar *gui_button_press_lookup(GtkWidget * html_widget)
 *
 * Description
 *   lookup word in dict/lex module
 *
 * Return value
 *   gchar *
 */

gchar *gui_button_press_lookup(GtkWidget * html_widget)
{
	gchar *key = NULL;
	GtkHTML *html;
	gint len;

	html = GTK_HTML(html_widget);
	if (!html_engine_is_selection_active(html->engine)) {
		gtk_html_select_word(html);
		if (html_engine_is_selection_active(html->engine)) {
			key =
			    html_engine_get_selection_string(html->
							     engine);
			key = g_strdelimit(key, ".,\"<>;:?", ' ');
			key = g_strstrip(key);
			len = strlen(key);
			g_warning("len = %d",len);
			if (key[len-1] == 's' || key[len-1] == 'd') 
				key[len-1] = '\0';
			if (key[len-1] == 'h' && key[len-2] == 't' 
				&& key[len-3] == 'e') 
					key[len-3] = '\0';
			return g_strdup(key);	/* must be freed by calling function */
		}

	}
	return key;
}


/******************************************************************************
 * Name
 *   gui_begin_html
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_begin_html(GtkWidget * html_widget, gboolean isutf8)
 *
 * Description
 *   start loading html widget
 *
 * Return value
 *   void
 */

void gui_begin_html(GtkWidget * html_widget, gboolean isutf8)
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


/******************************************************************************
 * Name
 *   gui_end_html
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_end_html(GtkWidget * html)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_end_html(GtkWidget * html)
{
	gtk_html_end(GTK_HTML(html), htmlstream, status1);
}


/******************************************************************************
 * Name
 *   gui_display_html
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_display_html(GtkWidget * html, const gchar * txt, gint lentxt)
 *
 * Description
 *   display text using gtk_html_write()
 *
 * Return value
 *   void
 */

void gui_display_html(GtkWidget * html, const gchar * txt, gint lentxt)
{
	if (strlen(txt)) {
		gtk_html_write(GTK_HTML(html), htmlstream, txt, lentxt);
	}
}


/******************************************************************************
 * Name
 *   print_footer
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void print_footer(GtkHTML * html, GnomePrintContext * context,
 *		gdouble x, gdouble y, gdouble width, gdouble height,
 *						gpointer user_data)
 *
 * Description
 *   printing stuff
 *
 * Return value
 *   void
 */

static gint page_num;
static GnomeFont *font;
static void print_footer(GtkHTML * html, GnomePrintContext * context,
			 gdouble x, gdouble y, gdouble width,
			 gdouble height, gpointer user_data)
{
	//gchar *text = g_strdup_printf("- %d -", page_num);
	/*gdouble tw = gnome_font_get_width_string(font, "text");

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
	page_num++;*/
}


/******************************************************************************
 * Name
 *   gui_html_print
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_html_print(GtkWidget * htmlwidget)
 *
 * Description
 *   printing using gtk_html_print_with_header_footer()
 *
 * Return value
 *   void
 */

void gui_html_print(GtkWidget * htmlwidget)
{
/*	GnomePrintMaster *print_master;
	GnomePrintContext *print_context;
	GtkWidget *preview;
	GtkWidget *print_dialog;
	GtkHTML *html;
	gint ret;

	html = GTK_HTML(htmlwidget);

	print_master = gnome_print_master_new();
	print_context = gnome_print_master_get_context(print_master);

	print_dialog = gnome_print_dialog_new(_("Print"), 0);

	page_num = 1;
	font =
	    gnome_font_new_closest("Times", GNOME_FONT_BOOK, FALSE, 12);
	ret = gnome_dialog_run(GNOME_DIALOG(print_dialog));
	print_master =
	    gnome_print_master_new_from_dialog(GNOME_PRINT_DIALOG
					       (print_dialog));
	print_context = gnome_print_master_get_context(print_master);
	gtk_html_print_with_header_footer(html, print_context, .0,
					  .03, NULL, print_footer,
					  NULL);

	switch (ret) {
	case GNOME_PRINT_PRINT:
		gnome_print_master_print(print_master);
		break;
	case GNOME_PRINT_PREVIEW:
		preview = GTK_WIDGET(gnome_print_master_preview_new
				     (print_master,
				      _("GnomeSword Print Preview")));
		gtk_widget_show(preview);
		break;
	case GNOME_PRINT_CANCEL:
		break;
	}
	gtk_widget_destroy(print_dialog);
	
	if (font)
		gtk_object_unref(GTK_OBJECT(font));
	gtk_object_unref(GTK_OBJECT(print_master));*/
}
