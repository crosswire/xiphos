/*
 * GnomeSword Bible Study Tool
 * parallel_view.cc - support for displaying multiple modules
 *
 * Copyright (C) 2004 GnomeSword Developer Team
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

#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>
#ifdef __cplusplus
}
#endif

#include <gal/widgets/e-unicode.h>

#include "gui/parallel_view.h"
#include "gui/parallel_dialog.h"
#include "gui/html.h"
#include "gui/widgets.h"

#include "main/parallel_view.h"
#include "main/bibletext.h"
#include "main/global_ops.hh"
#include "main/lists.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/key.h"
#include "main/xml.h"


#include "backend/sword_main.hh"

extern GtkWidget *entrycbIntBook;
extern GtkWidget *sbIntChapter;
extern GtkWidget *sbIntVerse;
extern GtkWidget *entryIntLookup;

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
static GLOBAL_OPS *ops;

BackEnd *backend_p;

static gchar *tf2of(int true_false)
{
	if (true_false)
		return "On";
	else
		return "Off";
}


static void set_global_option(char * option, gboolean choice)
{
	char *on_off = tf2of(choice);;
	SWMgr *mgr = backend_p->get_display_mgr();

	mgr->setGlobalOption(option, on_off);
}


/******************************************************************************
 * Name
 *   main_parallel_change_verse
 *
 * Synopsis
 *   #include ".h"
 *   
 *   gchar *main_parallel_change_verse(void)
 *
 * Description
 *   this is for the parallel dialog only
 *
 *   
 *
 * Return value
 *   gchar *
 */

gchar *main_parallel_change_verse(void)
{
	gchar *retval;
	const gchar *bookname;
	gchar buf[256];
	gint chapter, verse;
	char *newbook;

	bookname = gtk_entry_get_text(GTK_ENTRY(entrycbIntBook));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (sbIntChapter));
	verse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (sbIntVerse));

	sprintf(buf, "%s %d:%d", bookname, chapter, verse);

	newbook = backend_p->key_get_book(buf);
	chapter = backend_p->key_get_chapter(buf);
	verse = backend_p->key_get_verse(buf);

	if (strcmp(bookname, newbook))
		gtk_entry_set_text(GTK_ENTRY(entrycbIntBook), newbook);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter),
				  chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), verse);
	sprintf(buf, "%s %d:%d", newbook, chapter, verse);
	gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
	retval = buf;
	g_free(newbook);
	return retval;
}


/******************************************************************************
 * Name
 *   main_parallel_update_controls
 *
 * Synopsis
 *   #include ".h"
 *   
 *   gchar *main_parallel_update_controls(gchar * ref)
 *
 * Description
 *   
 *
 *
 *   
 *
 * Return value
 *   gchar*
 */

gchar *main_parallel_update_controls(const gchar * ref)
{
	const gchar *bookname;
	gchar buf[256];
	gint chapter, verse;
	char *newbook;

	newbook = backend_p->key_get_book(ref); 
	chapter = backend_p->key_get_chapter(ref);
	verse = backend_p->key_get_verse(ref);

	bookname = gtk_entry_get_text(GTK_ENTRY(entrycbIntBook));
	if (strcmp(bookname, newbook))
		gtk_entry_set_text(GTK_ENTRY(entrycbIntBook), newbook);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter),
				  chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), verse);
	sprintf(buf, "%s %d:%d", newbook, chapter, verse);
	gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
	g_free(newbook);
	return g_strdup(buf);
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

void main_set_parallel_module_global_options(GtkCheckMenuItem * menuitem,
					   gpointer user_data)
{
	gchar *option = (gchar*) user_data;
	gboolean choice = menuitem->active;
	
	
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

	set_global_option(option, choice);

	/* display change */
	if (settings.dockedInt) {
		main_update_parallel_page();
	} else {
		main_update_parallel_page_detached();
	}
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

void main_set_parallel_options_at_start(void)
{
/*	if (settings.footnotesint)
		set_parallel_global_option("Footnotes", "On");	
	else
		set_parallel_global_option("Footnotes", "Off");	

	
	if (settings.strongsint)
		set_parallel_global_option("Strong's Numbers", "On");	
	else
		set_parallel_global_option("Strong's Numbers", "Off");	

	
	if (settings.morphsint)
		set_parallel_global_option("Morphological Tags", "On");	
	else
		set_parallel_global_option("Morphological Tags", "Off");	

	
	if (settings.hebrewpointsint)
		set_parallel_global_option("Hebrew Vowel Points", "On");	
	else
		set_parallel_global_option("Hebrew Vowel Points", "Off");	

	
	if (settings.cantillationmarksint)
		set_parallel_global_option("Hebrew Cantillation", "On");	
	else
		set_parallel_global_option("Hebrew Cantillation", "Off");	

	
	if (settings.greekaccentsint)
		set_parallel_global_option("Greek Accents", "On");	
	else
		set_parallel_global_option("Greek Accents", "Off");	
*/
}



/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/mod_global_ops.h"
 *
 *   	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

	  
void main_load_g_ops_parallel(GtkWidget *menu)
{
	GList *tmp = NULL;	
	GtkWidget * item;

	tmp = backend->get_module_options();
	while(tmp) {
		item =
		       gtk_check_menu_item_new_with_label(
					(gchar *) tmp->data);
		gtk_widget_show(item);
		
		gtk_container_add(GTK_CONTAINER(menu), item); 
		
		if (!strcmp((gchar *) tmp->data, "Strong's Numbers")) {
			GTK_CHECK_MENU_ITEM(item)->active =
			    settings.strongsint;
		}

		if (!strcmp((gchar *) tmp->data, "Footnotes")) {
			GTK_CHECK_MENU_ITEM(item)->active =
			    settings.footnotesint;
		}

		if (!strcmp((gchar *) tmp->data, "Morphological Tags")) {
			GTK_CHECK_MENU_ITEM(item)->active =
			    settings.morphsint;
		}

		if (!strcmp((gchar *) tmp->data, "Hebrew Vowel Points")) {
			GTK_CHECK_MENU_ITEM(item)->active =
			    settings.hebrewpointsint;
		}

		if (!strcmp((gchar *) tmp->data, "Hebrew Cantillation")) {
			GTK_CHECK_MENU_ITEM(item)->active =
			    settings.cantillationmarksint;
		}

		if (!strcmp((gchar *) tmp->data, "Greek Accents")) {
			GTK_CHECK_MENU_ITEM(item)->active =
			    settings.greekaccentsint;
		}

		if (!strcmp((gchar *) tmp->data,
		     "Scripture Cross-references")) {
			GTK_CHECK_MENU_ITEM(item)->active =
			    settings.crossrefint;
		}

		if (!strcmp((gchar *) tmp->data, "Lemmas")) {
			GTK_CHECK_MENU_ITEM(item)->active =
			    settings.lemmasint;
		}

		if (!strcmp((gchar *) tmp->data, "Headings")) {
			GTK_CHECK_MENU_ITEM(item)->active =
			    settings.headingsint;
		}		
		
		g_signal_connect(GTK_OBJECT(item), "activate",
		    G_CALLBACK(main_set_parallel_module_global_options),
				   (gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}
}



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

void main_check_parallel_modules(void)
{
	if(settings.parallel1Module) 
		parallel1 =
		    main_is_module(settings.parallel1Module);
	else
		parallel1 = FALSE;

	if (settings.parallel2Module)
		parallel2 =
		    main_is_module(settings.parallel2Module);
	else
		parallel2 = FALSE;

	if (settings.parallel3Module)
		parallel3 =
		    main_is_module(settings.parallel3Module);
	else
		parallel3 = FALSE;

	if (settings.parallel4Module)
		parallel4 =
		    main_is_module(settings.parallel4Module);
	else
		parallel4 = FALSE;

	if (settings.parallel5Module)
		parallel5 =
		    main_is_module(settings.parallel5Module);
	else
		parallel5 = FALSE;

}


/******************************************************************************
 * Name
 *   gui_change_int1_mod
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void gui_changeint1_mod(gchar * mod_name)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_change_parallel_module(GSParallel parallel, gchar * mod_name)
{
	if(!mod_name) 
		return;
	switch(parallel) {
	case PARALLEL1:	
		xml_set_value("GnomeSword", "modules", "int1",
			      mod_name);
		settings.parallel1Module =
		    xml_get_value("modules", "int1");
		parallel1 =
		    main_is_module(settings.parallel1Module);
		break;
	case PARALLEL2:	
		xml_set_value("GnomeSword", "modules", "int2",
			      mod_name);
		settings.parallel2Module =
		    xml_get_value("modules", "int2");
		parallel2 =
		    main_is_module(settings.parallel2Module);
		break;
	case PARALLEL3:	
		xml_set_value("GnomeSword", "modules", "int3",
			      mod_name);
		settings.parallel3Module =
		    xml_get_value("modules", "int3");
		parallel3 =
		    main_is_module(settings.parallel3Module);
		break;
	case PARALLEL4:	
		xml_set_value("GnomeSword", "modules", "int4",
			      mod_name);
		settings.parallel4Module =
		    xml_get_value("modules", "int4");
		parallel4 =
		    main_is_module(settings.parallel4Module);
		break;
	case PARALLEL5:	
		xml_set_value("GnomeSword", "modules", "int5",
			      mod_name);
		settings.parallel5Module =
		    xml_get_value("modules", "int5");
		parallel5 =
		    main_is_module(settings.parallel5Module);
		break;
	default:
		return;
		break;
	}
	if (settings.dockedInt)
		main_update_parallel_page();
	else
		main_update_parallel_page_detached();
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

void main_update_parallel_page(void)
{
	gchar tmpBuf[256], *rowcolor, *font_size = NULL;
	gchar *utf8str, *mod_name, *font_name = NULL;
	gint utf8len, i, j;
	gboolean was_editable, use_gtkhtml_font;
	gboolean is_rtol = FALSE;
	gchar *buf;

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
			is_rtol = main_is_mod_rtol(mod_name);
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
					"<tr><td><i><font color=\"%s\" size=\"%s\">[%s]</font></i></td></tr>",
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
				"<tr bgcolor=\"%s\"><td><b><a href=\"about://%s/%s\"><font color=\"%s\" size=\"%s\"> [%s]</font></a></b>",
				rowcolor,
				mod_name,
				main_get_module_description(mod_name),
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
			    backend_p->get_render_text(mod_name,
							settings.
							currentverse);
			//g_warning(utf8str);
			if(is_rtol) {
				buf = g_strdup_printf(
					"%s","<br><DIV ALIGN=right>");			
				if (strlen(buf)) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream, buf,
						       strlen(buf));
					free(buf);
				}
			}	
			if (strlen(utf8str)) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, utf8str,
					       strlen(utf8str));
				free(utf8str);
			}

			if(is_rtol) {
				buf = g_strdup_printf(
					"%s","</DIV><br>");			
				if (strlen(buf)) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream, buf,
						       strlen(buf));
					free(buf);
				}
			}	
			sprintf(tmpBuf,
				"</font><small>[<a href=\"parallel://swap/%s\">view context</a>]</small></td></tr>",
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
/*	gtk_frame_set_label(GTK_FRAME(widgets.frame_parallel),
			    settings.currentverse);*/
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
	gboolean is_rtol = FALSE;
	gchar *buf2;
	gint utf8len, cur_verse, cur_chapter, i = 1, j;
	char *cur_book;
	GtkHTML *html = GTK_HTML(widgets.html_parallel);

	str = g_string_new("");
	tmpkey = backend_p->get_valid_key(key);

	bgColor = "#f1f1f1";
	cur_verse = backend_p->key_get_verse(tmpkey);
	settings.intCurVerse = cur_verse;
	cur_chapter = backend_p->key_get_chapter(tmpkey);
	cur_book = backend_p->key_get_book(tmpkey);

	for (i = 1;; i++) {
		sprintf(tmpbuf, "%s %d:%d", cur_book, cur_chapter, i);
		free(tmpkey);
		tmpkey = backend_p->get_valid_key(tmpbuf);
		if (cur_chapter != backend_p->key_get_chapter(tmpkey))
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

			is_rtol = main_is_mod_rtol(mod_name);
			
			use_font_size = get_module_font_size(mod_name);
			if (strlen(use_font_size) < 2) {
				free(use_font_size);
				use_font_size = g_strdup("+0");
			}

			g_string_printf(str,
				"<td width=\"20%%\" bgcolor=\"%s\">"
				"<a href=\"parallel://verse/%s\" name=\"%d\">"
				"<font color=\"%s\">%d. </font></a>"
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
			if(is_rtol) {
				buf2 = g_strdup_printf(
					"%s","<br><DIV ALIGN=right>");			
				if (strlen(buf2)) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream, buf2,
						       strlen(buf2));
					free(buf2);
				}
			}
			if (mod_name) {
				
				utf8str =
				    main_get_rendered_text
				    (mod_name, tmpkey);
				if (strlen(utf8str)) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream,
						       utf8str,
						       strlen(utf8str));
					free(utf8str);
				}
			}
			if(is_rtol) {
				buf2 = g_strdup_printf(
					"%s","</DIV>");			
				if (strlen(buf2)) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream, buf2,
						       strlen(buf2));
					free(buf2);
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
	g_free(tmpkey);
	g_free(cur_book);
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

void main_update_parallel_page_detached(void)
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

void main_swap_parallel_with_main(char *intmod)
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
	main_display_bible(intmod, settings.currentverse);
	main_update_parallel_page();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets.
						   button_parallel_view),FALSE);
}



/******************************************************************************
 * Name
 *   load_menu_formmod_list
 *
 * Synopsis
 *   #include "gui/parallel.h
 *
 *   void load_menu_formmod_list(GtkWidget *pmInt, GList *mods,
 *			gchar *label, GCallback mycallback)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_load_menu_form_mod_list(GtkWidget * pmInt, gchar * label,
				   GCallback mycallback)
{
	GList *tmp = get_list(TEXT_DESC_LIST);
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
	while (tmp != NULL) {
		item =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_show(item);
		g_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK(mycallback),
				   g_strdup((gchar *) tmp->data));

		gtk_container_add(GTK_CONTAINER(view_module_menu),
				  item);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}


void main_init_paraellel_view(void)
{
	backend_p = new BackEnd();
	gui_create_parallel_page();
	gui_create_parallel_popup();
}

void main_delete_paraellel_view(void)
{
	delete backend_p;
	
}
