/*
 * GnomeSword Bible Study Tool
 * gs_parallel.c - support for displaying multiple modules
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

#include "gui/parallel_view.h"
#include "gui/parallel_dialog.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/bibletext.h"
#include "main/lists.h"
#include "main/sword.h"
#include "main/key.h"
#include "main/xml.h"



/******************************************************************************
 * static
 */
static GtkHTMLStreamStatus status1;
static GtkHTMLStream *htmlstream;
static GtkWidget *module_options_menu;
static gboolean parallel1;
static gboolean parallel2;
static gboolean parallel3;
static gboolean parallel4;
static gboolean parallel5;



/******************************************************************************
 * Name
 *   gui_check_parallel_modules
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_check_parallel_modules(void)
 *
 * Description
 *   check for parallel modules on program start
 *   we don't want to try to display modules we don't have
 *   it makes bad things happen
 *
 * Return value
 *   void
 */

void gui_check_parallel_modules(void)
{
	if (strlen(settings.parallel1Module) > 1)
		parallel1 =
		    check_for_module(settings.parallel1Module);
	else
		parallel1 = FALSE;

	if (strlen(settings.parallel2Module) > 1)
		parallel2 =
		    check_for_module(settings.parallel2Module);
	else
		parallel2 = FALSE;

	if (strlen(settings.parallel3Module) > 1)
		parallel3 =
		    check_for_module(settings.parallel3Module);
	else
		parallel3 = FALSE;

	if (strlen(settings.parallel4Module) > 1)
		parallel4 =
		    check_for_module(settings.parallel4Module);
	else
		parallel4 = FALSE;

	if (strlen(settings.parallel5Module) > 1)
		parallel5 =
		    check_for_module(settings.parallel5Module);
	else
		parallel5 = FALSE;

}

/******************************************************************************
 * Name
 *   gui_set_parallel_options_at_start
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_set_parallel_options_at_start(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_set_parallel_options_at_start(void)
{
	if (settings.footnotesint)
		set_parallel_global_option("Footnotes", "On");	/* keep footnotes in sync with menu */
	else
		set_parallel_global_option("Footnotes", "Off");	/* keep footnotes in sync with menu */

	/*
	   set parallel Strong's Numbers to last setting used
	 */
	if (settings.strongsint)
		set_parallel_global_option("Strong's Numbers", "On");	/* keep Strongs in sync with menu */
	else
		set_parallel_global_option("Strong's Numbers", "Off");	/* keep Strongs in sync with menu */

	/*
	   set parallel morph tags to last setting used
	 */
	if (settings.morphsint)
		set_parallel_global_option("Morphological Tags", "On");	/* keep Morph Tags in sync with menu */
	else
		set_parallel_global_option("Morphological Tags", "Off");	/* keep Morph Tag in sync with menu */

	/*
	   set parallel Hebrew Vowel Points to last setting used
	 */
	if (settings.hebrewpointsint)
		set_parallel_global_option("Hebrew Vowel Points", "On");	/* keep Hebrew Vowel Points in sync with menu */
	else
		set_parallel_global_option("Hebrew Vowel Points", "Off");	/* keep Hebrew Vowel Points in sync with menu */

	/*
	   set parallel Hebrew Cantillation to last setting used
	 */
	if (settings.cantillationmarksint)
		set_parallel_global_option("Hebrew Cantillation", "On");	/* keep Hebrew Cantillation in sync with menu */
	else
		set_parallel_global_option("Hebrew Cantillation", "Off");	/* keep Hebrew Cantillation in sync with menu */

	/*
	   set parallel Greek Accents to last setting used
	 */
	if (settings.greekaccentsint)
		set_parallel_global_option("Greek Accents", "On");	/* keep Greek Accents in sync with menu */
	else
		set_parallel_global_option("Greek Accents", "Off");	/* keep Greek Accents in sync with menu */
}


/******************************************************************************
 * Name
 *   on_undockInt_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_undockInt_activate(GtkMenuItem *menuitem)
 *
 * Description
 *   undock/dock parallel page
 *
 * Return value
 *   void
 */

static void on_undockInt_activate(GtkMenuItem * menuitem)
{
	if (settings.dockedInt) {
		settings.dockedInt = FALSE;
		gui_undock_parallel_page();

	} else {
		settings.dockedInt = TRUE;
		gui_btnDockInt_clicked(NULL, NULL);
	}
}



/******************************************************************************
 * Name
 *   on_changeint1mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint1mod_activate(GtkMenuItem * menuitem,
 *			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint1mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = module_name_from_description((gchar *) user_data);
	if (mod_name) {
		xml_set_value("GnomeSword", "modules", "int1",
			      mod_name);
		settings.parallel1Module =
		    xml_get_value("modules", "int1");
		parallel1 =
		    check_for_module(settings.parallel1Module);
		if (settings.dockedInt)
			gui_update_parallel_page();
		else
			gui_update_parallel_page_detached();
		g_free(mod_name);
	}
}



/******************************************************************************
 * Name
 *   on_changeint2mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint2mod_activate(GtkMenuItem * menuitem,
 *			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint2mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = module_name_from_description((gchar *) user_data);
	if (mod_name) {
		xml_set_value("GnomeSword", "modules", "int2",
			      mod_name);
		settings.parallel2Module =
		    xml_get_value("modules", "int2");
		parallel2 =
		    check_for_module(settings.parallel2Module);
		if (settings.dockedInt)
			gui_update_parallel_page();
		else
			gui_update_parallel_page_detached();
		g_free(mod_name);
	}
}


/******************************************************************************
 * Name
 *   on_changeint3mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint3mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint3mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = module_name_from_description((gchar *) user_data);
	if (mod_name) {
		xml_set_value("GnomeSword", "modules", "int3",
			      mod_name);
		settings.parallel3Module =
		    xml_get_value("modules", "int3");
		parallel3 =
		    check_for_module(settings.parallel3Module);
		if (settings.dockedInt)
			gui_update_parallel_page();
		else
			gui_update_parallel_page_detached();
		g_free(mod_name);
	}
}


/******************************************************************************
 * Name
 *   on_changeint4mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint4mod_activate(GtkMenuItem * menuitem,
 *			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint4mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = module_name_from_description((gchar *) user_data);
	if (mod_name) {
		xml_set_value("GnomeSword", "modules", "int4",
			      mod_name);
		settings.parallel4Module =
		    xml_get_value("modules", "int4");
		parallel4 =
		    check_for_module(settings.parallel4Module);
		if (settings.dockedInt)
			gui_update_parallel_page();
		else
			gui_update_parallel_page_detached();
		g_free(mod_name);
	}
}


/******************************************************************************
 * Name
 *   on_changeint5mod_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_changeint5mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void on_changeint5mod_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	gchar *mod_name = NULL;

	mod_name = module_name_from_description((gchar *) user_data);

	xml_set_value("GnomeSword", "modules", "int5", mod_name);
	settings.parallel5Module = xml_get_value("modules", "int5");
	parallel5 = check_for_module(settings.parallel5Module);
	if (settings.dockedInt)
		gui_update_parallel_page();
	else
		gui_update_parallel_page_detached();
	if (mod_name)
		g_free(mod_name);
}



/******************************************************************************
 * Name
 *   on_int_global_options_activate
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void on_int_global_options_activate(GtkMenuItem * menuitem,
 *				    gpointer user_data)
 *
 * Description
 *   toogle global options in parallel window
 *
 * Return value
 *   void
 */

static void on_int_global_options_activate(GtkMenuItem * menuitem,
					   gpointer user_data)
{
	gui_set_parallel_module_global_options((gchar *) user_data,
						  GTK_CHECK_MENU_ITEM
						  (menuitem)->active);
}


/******************************************************************************
 * Name
 *   gui_update_parallel_page
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_update_parallel_page(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_update_parallel_page(void)
{
	gchar tmpBuf[256], *rowcolor, *font_size = NULL;
	gchar *utf8str, *mod_name, *font_name = NULL;
	gint utf8len, i, j;
	gboolean was_editable, use_gtkhtml_font;

	if (settings.havebible) {
		/* setup gtkhtml widget */
		GtkHTML *html = GTK_HTML(widgets.html_parallel);
		was_editable = gtk_html_get_editable(html);
		if (was_editable)
			gtk_html_set_editable(html, FALSE);
		htmlstream =
		    gtk_html_begin_content(html,
					   "text/html; charset=utf-8");
		sprintf(tmpBuf,
			"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table>",
			settings.bible_bg_color,
			settings.bible_text_color, settings.link_color);
		/*utf8str =
		    e_utf8_from_gtk_string(widgets.html_parallel,
					   tmpBuf);*/
		utf8len = strlen(tmpBuf);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       tmpBuf, utf8len);
		}

		for (i = 0, j = 0; i < 5; i++) {
			mod_name = NULL;
			switch (i) {
			case 0:
				if (parallel1)
					mod_name =
					    settings.parallel1Module;
				else
					mod_name = NULL;
				break;
			case 1:
				if (parallel2)
					mod_name =
					    settings.parallel2Module;
				else
					mod_name = NULL;
				break;
			case 2:
				if (parallel3)
					mod_name =
					    settings.parallel3Module;
				else
					mod_name = NULL;
				break;
			case 3:
				if (parallel4)
					mod_name =
					    settings.parallel4Module;
				else
					mod_name = NULL;
				break;
			case 4:
				if (parallel5)
					mod_name =
					    settings.parallel5Module;
				else
					mod_name = NULL;
				break;
			}
//                      g_warning("mod_name = %s",mod_name);
			if (!mod_name)
				continue;

			++j;

			font_name = get_module_font_name(mod_name);
			if (strlen(font_name) < 2) {
				use_gtkhtml_font = TRUE;
//                                      g_warning("use_gtkhtml_font = TRUE");
			} else {
				if (!strncmp(font_name, "none", 4)) {
					use_gtkhtml_font = TRUE;
//                                      g_warning("use_gtkhtml_font = TRUE");
				} else {
					use_gtkhtml_font = FALSE;
//                                      g_warning("use_gtkhtml_font = FALSE");
				}
			}

			font_size = get_module_font_size(mod_name);
			if (strlen(font_size) < 2){
				free(font_size);
				font_size = g_strdup("+0");
			}

			if (j == 0 || j == 2 || j == 4)
				rowcolor = "#F1F1F1";
			else
				rowcolor = settings.bible_bg_color;

			if (j == 0) {
				sprintf(tmpBuf,
					"<tr><td><i><FONT COLOR=\"%s\" SIZE=\"%s\">[%s]</font></i></td></tr>",
					settings.bible_verse_num_color,
					settings.verse_num_font_size,
					settings.currentverse);

				utf8len = strlen(tmpBuf);
				if (utf8len) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream,
						       tmpBuf,
						       utf8len);
				}
			}

			sprintf(tmpBuf,
				"<tr bgcolor=\"%s\"><td><B><A HREF=\"[%s]%s\"><FONT COLOR=\"%s\" SIZE=\"%s\"> [%s]</font></a></b>",
				rowcolor,
				mod_name,
				get_module_description(mod_name),
				settings.bible_verse_num_color,
				settings.verse_num_font_size, mod_name);

			utf8len = strlen(tmpBuf);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, tmpBuf,
					       utf8len);
			}

			if (use_gtkhtml_font)
				sprintf(tmpBuf, "<font size=\"%s\">",
					font_size);
			else
				sprintf(tmpBuf,
					"<font face=\"%s\"size=\"%s\">",
					font_name, font_size);


			utf8len = strlen(tmpBuf);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, tmpBuf,
					       utf8len);
			}

			utf8str =
			    get_parallel_module_text(mod_name,
							settings.
							currentverse);
			//g_warning(utf8str);
			if (strlen(utf8str)) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, utf8str,
					       strlen(utf8str));
				free(utf8str);
			}

			sprintf(tmpBuf,
				"</font><small>[<A HREF=\"@%s\">view context</a>]</small></td></tr>",
				mod_name);

			utf8len = strlen(tmpBuf);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, tmpBuf,
					       utf8len);
			}
		}

		sprintf(tmpBuf, "</table></body></html>");

		utf8len = strlen(tmpBuf);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       tmpBuf, utf8len);
		}

		gtk_html_end(GTK_HTML(html), htmlstream, status1);
		gtk_html_set_editable(html, was_editable);
	}
	gtk_frame_set_label(GTK_FRAME(widgets.frame_parallel),
			    settings.currentverse);
	if (font_name)
		free(font_name);
	if (font_size)
		free(font_size);
}


/******************************************************************************
 * Name
 *   int_display
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void int_display(gchar *key)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void int_display(gchar * key)
{
	gchar
	    * utf8str,
	    *bgColor,
	    *textColor,
	    buf[500], *tmpkey, tmpbuf[256], *mod_name, *use_font_size;
	GString *str;

	gboolean evenRow = FALSE;


	gint utf8len, cur_verse, cur_chapter, i = 1, j;

	const char *cur_book;

	GtkHTML *html = GTK_HTML(widgets.html_parallel);

	str = g_string_new("");
	tmpkey = get_valid_key(key);

	bgColor = "#f1f1f1";
	cur_verse = get_verse_from_key(tmpkey);
	settings.intCurVerse = cur_verse;
	cur_chapter = get_chapter_from_key(tmpkey);
	cur_book = get_book_from_key(tmpkey);

	for (i = 1;; i++) {
		sprintf(tmpbuf, "%s %d:%d", cur_book, cur_chapter, i);
		free(tmpkey);
		tmpkey = get_valid_key(tmpbuf);
		if (cur_chapter != get_chapter_from_key(tmpkey))
			break;
		g_string_printf(str, "%s", "<tr valign=\"top\">");
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}

		if (i == cur_verse)
			textColor = settings.currentverse_color;
		else
			textColor = settings.bible_text_color;

		if (evenRow) {
			evenRow = FALSE;
			bgColor = settings.bible_bg_color;
		}

		else {
			evenRow = TRUE;
			bgColor = "#f1f1f1";
		}

		for (j = 0; j < 5; j++) {
			mod_name = NULL;
			switch (j) {
			case 0:
				if (parallel1)
					mod_name =
					    settings.parallel1Module;
				break;
			case 1:
				if (parallel2)
					mod_name =
					    settings.parallel2Module;
				break;
			case 2:
				if (parallel3)
					mod_name =
					    settings.parallel3Module;
				break;
			case 3:
				if (parallel4)
					mod_name =
					    settings.parallel4Module;
				break;
			case 4:
				if (parallel5)
					mod_name =
					    settings.parallel5Module;
				break;
			}

			use_font_size = get_module_font_size(mod_name);
			if (strlen(use_font_size) < 2) {
				free(use_font_size);
				use_font_size = g_strdup("+0");
			}

			g_string_printf(str,
				"<td width=\"20%%\" bgcolor=\"%s\">"
				"<A HREF=\"I%s\" NAME=\"%d\">"
				"<font color=\"%s\">%d. </font></A>"
				"<font size=\"%s\" color=\"%s\">",
				bgColor,
				tmpkey,
				i,
				settings.bible_verse_num_color,
				i, use_font_size, textColor);
			if (str->len) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, str->str,
					       str->len);
			}

			if (mod_name) {
				utf8str =
				    get_parallel_module_text
				    (mod_name, tmpkey);
				if (strlen(utf8str)) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream,
						       utf8str,
						       strlen(utf8str));
					free(utf8str);
				}
			}

			g_string_printf(str, "%s", "</font></td>");
			if (str->len) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, str->str,
					       str->len);
			}
			//if (use_font_size)
			g_free(use_font_size);
		}

		g_string_printf(str, "%s", "</tr>");
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	}
	free(tmpkey);
}


/******************************************************************************
 * Name
 *   gui_update_parallel_page_detached
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_update_parallel_page_detached(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_update_parallel_page_detached(void)
{
	gchar *utf8str, buf[500];
	gint utf8len;

	//-- setup gtkhtml widget
	GtkHTML *html = GTK_HTML(widgets.html_parallel);
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");


	sprintf(buf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table align=\"left\" valign=\"top\"><tr valign=\"top\" >",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);
//	utf8str = e_utf8_from_gtk_string(widgets.html_parallel, buf);
	utf8len = strlen(buf);	//g_utf8_strlen (utf8str , -1) ;
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, buf,
			       utf8len);
	}

	if (settings.parallel1Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.parallel1Module);

		utf8len = strlen(buf);	//g_utf8_strlen (utf8str , -1) ;
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       buf, utf8len);
		}
	}

	if (settings.parallel2Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.parallel2Module);

		utf8len = strlen(buf);	//g_utf8_strlen (utf8str , -1) ;
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       buf, utf8len);
		}
	}

	if (settings.parallel3Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.parallel3Module);

		utf8len = strlen(buf);	//g_utf8_strlen (utf8str , -1) ;
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       buf, utf8len);
		}
	}

	if (widgets.html_parallel) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.parallel4Module);

		utf8len = strlen(buf);	//g_utf8_strlen (utf8str , -1) ;
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       buf, utf8len);
		}
	}

	if (settings.parallel5Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.parallel5Module);

		utf8len = strlen(buf);	//g_utf8_strlen (utf8str , -1) ;
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       buf, utf8len);
		}
	}

	sprintf(buf, "%s", "</tr>");
	//utf8str = e_utf8_from_gtk_string(widgets.html_parallel, buf);
	utf8len = strlen(buf);	//g_utf8_strlen (utf8str , -1) ;
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, buf,
			       utf8len);
	}


	/******      ******/
	int_display(settings.cvparallel);

	sprintf(buf, "%s", "</table></body></html>");
	//utf8str = e_utf8_from_gtk_string(widgets.html_parallel, buf);
	utf8len = strlen(buf);	//g_utf8_strlen (utf8str , -1) ;
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, buf,
			       utf8len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	sprintf(buf, "%d", settings.intCurVerse);
	gtk_html_jump_to_anchor(html, buf);
}

/******************************************************************************
 * Name
 *   gui_swap_parallel_with_main
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_swap_parallel_with_main(char * intmod)
 *
 * Description
 *   swaps parallel mod with mod in main text window
 *
 * Return value
 *   void
 */

void gui_swap_parallel_with_main(char *intmod)
{
	if (!strcmp(settings.parallel5Module, intmod)) {
		settings.parallel5Module = xml_get_value("modules", "bible");
	}
	if (!strcmp(settings.parallel4Module, intmod)) {
		settings.parallel4Module = xml_get_value("modules", "bible");
	}
	if (!strcmp(settings.parallel3Module, intmod)) {
		settings.parallel3Module = xml_get_value("modules", "bible");
	}
	if (!strcmp(settings.parallel2Module, intmod)) {
		settings.parallel2Module = xml_get_value("modules", "bible");
	}
	if (!strcmp(settings.parallel1Module, intmod)) {
		settings.parallel1Module = xml_get_value("modules", "bible");
	}
	gui_change_module_and_key(intmod, settings.currentverse);
	gui_update_parallel_page();
}


/******************************************************************************
 * Name
 *   gui_set_parallel_module_global_options
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_set_parallel_module_global_options(gchar *option,
 *						gboolean choice)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_set_parallel_module_global_options(gchar * option,
					       gboolean choice)
{
	gchar *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}

	if (!strcmp(option, "Strong's Numbers")) {
		settings.strongsint = choice;
	}

	if (!strcmp(option, "Footnotes")) {
		settings.footnotesint = choice;
	}

	if (!strcmp(option, "Morphological Tags")) {
		settings.morphsint = choice;
	}

	if (!strcmp(option, "Hebrew Vowel Points")) {
		settings.hebrewpointsint = choice;
	}

	if (!strcmp(option, "Hebrew Cantillation")) {
		settings.cantillationmarksint = choice;
	}

	if (!strcmp(option, "Greek Accents")) {
		settings.greekaccentsint = choice;
	}

	set_parallel_global_option(option, on_off);

	/* display change */
	if (settings.dockedInt) {
		gui_update_parallel_page();
	} else {
		gui_update_parallel_page_detached();
	}
}


/******************************************************************************
 * Name
 *   add_items_to_options_menu
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void add_items_to_options_menu(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void add_items_to_options_menu(void)
{
	GtkWidget * menuChoice, *shellmenu;
	gchar menuName[64];
	int viewNumber = 0;
	GList *tmp;

	tmp = NULL;

	tmp = get_list(OPTIONS_LIST);	/* this is the only place this
					   list is used */
	while (tmp != NULL) {
		shellmenu = module_options_menu;

		/* add global option items to parallel popup menu */
		menuChoice =
		    gtk_check_menu_item_new_with_label((gchar *) tmp->
						       data);
		sprintf(menuName, "optionNum%d", viewNumber++);
		gtk_object_set_data(GTK_OBJECT(widgets.app), menuName,
				    menuChoice);
		gtk_widget_show(menuChoice);
		gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
				   G_CALLBACK
				   (on_int_global_options_activate),
				   (gchar *) tmp->data);
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
				      GTK_WIDGET(menuChoice), 1);

		if (!strcmp((gchar *) tmp->data, "Strong's Numbers")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
			    settings.strongsint;
		}

		if (!strcmp((gchar *) tmp->data, "Footnotes")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
			    settings.footnotesint;
		}

		if (!strcmp((gchar *) tmp->data, "Morphological Tags")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
			    settings.morphsint;
		}

		if (!strcmp((gchar *) tmp->data, "Hebrew Vowel Points")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
			    settings.hebrewpointsint;
		}

		if (!strcmp((gchar *) tmp->data, "Hebrew Cantillation")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
			    settings.cantillationmarksint;
		}

		if (!strcmp((gchar *) tmp->data, "Greek Accents")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
			    settings.greekaccentsint;
		}

		if (!strcmp
		    ((gchar *) tmp->data,
		     "Scripture Cross-references")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
			    settings.crossrefint;
		}

		if (!strcmp((gchar *) tmp->data, "Lemmas")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
			    settings.lemmasint;
		}

		if (!strcmp((gchar *) tmp->data, "Headings")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
			    settings.headingsint;
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}


/******************************************************************************
 * Name
 *   load_menu_formmod_list
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void load_menu_formmod_list(GtkWidget *pmInt, GList *mods,
 *			gchar *label, GtkMenuCallback mycallback)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void load_menu_formmod_list(GtkWidget * pmInt, GList * mods,
				   gchar * label,
				   GtkMenuCallback mycallback)
{
	GList *tmp;
	GtkWidget *item;
	GtkWidget *view_module;
	GtkWidget *view_module_menu;
	GtkAccelGroup *view_module_menu_accels;

	view_module = gtk_menu_item_new_with_label(label);
	gtk_widget_show(view_module);
	gtk_container_add(GTK_CONTAINER(pmInt), view_module);

	view_module_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_module),
				  view_module_menu);
/*	view_module_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (view_module_menu));*/
	tmp = mods;
	while (tmp != NULL) {
		item =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_show(item);
		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK(mycallback),
				   g_strdup((gchar *) tmp->data));

		gtk_container_add(GTK_CONTAINER(view_module_menu),
				  item);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}


/******************************************************************************
 * Name
 *   create_parallel_popup
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   GtkWidget * create_parallel_popup(GList * mods)
 *
 * Description
 *   create popup menu for parallel window
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_parallel_popup(GList * mods)
{
	GtkWidget *pmInt;
	GtkAccelGroup *pmInt_accels;
	GtkWidget *copy7;
	GtkWidget *undockInt;
	GtkWidget *module_options;
	GtkWidget *separator2;
	GtkTooltips *tooltips;


	tooltips = gtk_tooltips_new();
	pmInt = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmInt), "pmInt", pmInt);
/*	pmInt_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(pmInt));*/

	copy7 = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_show(copy7);
	gtk_container_add(GTK_CONTAINER(pmInt), copy7);

	separator2 = gtk_menu_item_new();
	gtk_widget_show(separator2);
	gtk_container_add(GTK_CONTAINER(pmInt), separator2);
	gtk_widget_set_sensitive(separator2, FALSE);

	undockInt = gtk_menu_item_new_with_label(_("Detach/Attach"));
	gtk_widget_show(undockInt);
	gtk_container_add(GTK_CONTAINER(pmInt), undockInt);

	module_options =
	    gtk_menu_item_new_with_label(_("Module Options"));
	gtk_widget_show(module_options);
	gtk_container_add(GTK_CONTAINER(pmInt), module_options);

	module_options_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_options),
				  module_options_menu);

	separator2 = gtk_menu_item_new();
	gtk_widget_show(separator2);
	gtk_container_add(GTK_CONTAINER(pmInt), separator2);
	gtk_widget_set_sensitive(separator2, FALSE);
	/* build change parallel modules submenu */
	load_menu_formmod_list(pmInt, mods, _("Change parallel 1"),
			       (GtkMenuCallback)
			       on_changeint1mod_activate);
	load_menu_formmod_list(pmInt, mods, _("Change parallel 2"),
			       (GtkMenuCallback)
			       on_changeint2mod_activate);
	load_menu_formmod_list(pmInt, mods, _("Change parallel 3"),
			       (GtkMenuCallback)
			       on_changeint3mod_activate);
	load_menu_formmod_list(pmInt, mods, _("Change parallel 4"),
			       (GtkMenuCallback)
			       on_changeint4mod_activate);
	load_menu_formmod_list(pmInt, mods, _("Change parallel 5"),
			       (GtkMenuCallback)
			       on_changeint5mod_activate);

	gtk_signal_connect(GTK_OBJECT(copy7), "activate",
			   G_CALLBACK(gui_copyhtml_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(undockInt), "activate",
			   G_CALLBACK(on_undockInt_activate),
			   &settings);

	return pmInt;
}


/******************************************************************************
 * Name
 *   gui_create_parallel_popup
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_create_parallel_popup(GList *bible_description)
 *
 * Description
 *   call create_parallel_popup() and attach menu to int html widget
 *
 * Return value
 *   void
 */

void gui_create_parallel_popup(GList * bible_description)
{
	/* create popup menu for parallel window */
	GtkWidget *menu_inter =
	    create_parallel_popup(bible_description);
	/* attach popup menus */
	gnome_popup_menu_attach(menu_inter,
				widgets.html_parallel,
				(gchar *) "1");
	add_items_to_options_menu();
}


/******************************************************************************
 * Name
 *   gui_create_parallel_page
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_create_parallel_page(guint page_num)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_create_parallel_page(guint page_num)
{
	GtkWidget *scrolled_window;
	GtkWidget *label;
	/*
	 * parallel page 
	 */
	widgets.frame_parallel = gtk_frame_new(NULL);
	gtk_widget_show(widgets.frame_parallel);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_text),
			  widgets.frame_parallel);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolled_window);
	gtk_container_add(GTK_CONTAINER(widgets.frame_parallel),
			  scrolled_window);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);

	settings.dockedInt = TRUE;

	widgets.html_parallel = gtk_html_new();
	gtk_widget_show(widgets.html_parallel);
	gtk_html_load_empty(GTK_HTML(widgets.html_parallel));
	gtk_container_add(GTK_CONTAINER(scrolled_window),
			  widgets.html_parallel);


	label = gtk_label_new(_("Parallel View"));
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_text),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.notebook_text), 
				    settings.parallel_page),
				   label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.notebook_text),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.notebook_text),
					  settings.parallel_page), 
					  _("Parallel View"));
	
	gtk_signal_connect(GTK_OBJECT(widgets.html_parallel),
			   "on_url", G_CALLBACK(gui_url),
			   (gpointer) widgets.app);
	gtk_signal_connect(GTK_OBJECT(widgets.html_parallel),
			   "link_clicked",
			   G_CALLBACK(gui_link_clicked), NULL);
			   /*
	gtk_signal_connect(GTK_OBJECT(widgets.html_parallel),
			   "button_release_event",
			   G_CALLBACK(button_release_event), NULL);
			   */
}
