/*
 * GnomeSword Bible Study Tool
 * gtkhtml_dispaly.c - display Sword modules using GtkHTML 
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
#include "gui/gnomesword.h"

#include "main/sword.h"
#include "main/bibletext.h"
#include "main/settings.h"

//"<font color=\"#FF0000\">"
/******************************************************************************
 * Name
 *  get_font
 *
 * Synopsis
 *   #include ".h"
 *
 *   MOD_FONT * get_font(gchar *mod_name)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static MOD_FONT *get_font(gchar * mod_name)
{
	MOD_FONT *mf;

	mf = g_new(MOD_FONT, 1);
	mf->mod_name = mod_name;
	//g_warning(mf->mod_name);
	mf->old_font = NULL;
	mf->old_gdk_font = NULL;
	mf->old_font_size = NULL;
	mf->new_font = NULL;
	mf->new_gdk_font = NULL;
	mf->new_font_size = NULL;
	mf->no_font = 0;
	get_font_info(mf);
	if (strlen(mf->old_font) < 2)
		mf->old_font = "none";
	//g_warning("mf->old_font = %s",mf->old_font);
	return mf;
}

/******************************************************************************
 * Name
 *  set_gobal_options
 *
 * Synopsis
 *   #include ".h"
 *
 *  void (TEXT_DATA * t)	
 *
 * Description
 *   set module global options on page change
 *
 * Return value
 *   void
 */

static void set_global_options(TEXT_GLOBALS * tgs)
{
	if (tgs->strongs)
		set_text_module_global_option("Strong's Numbers",
					      GTK_TOGGLE_BUTTON(tgs->
								t_btn_strongs)->
					      active);
	if (tgs->morphs)
		set_text_module_global_option("Morphological Tags",
					      GTK_TOGGLE_BUTTON(tgs->
								t_btn_morphs)->
					      active);
	if (tgs->gbffootnotes || tgs->thmlfootnotes)
		set_text_module_global_option("Footnotes",
					      GTK_TOGGLE_BUTTON(tgs->
								t_btn_footnotes)->
					      active);
	if (tgs->greekaccents)
		set_text_module_global_option("Greek Accents",
					      GTK_TOGGLE_BUTTON(tgs->
								t_btn_accents)->
					      active);
	if (tgs->lemmas)
		set_text_module_global_option("Lemmas",
					      GTK_TOGGLE_BUTTON(tgs->
								t_btn_lemmas)->
					      active);
	if (tgs->scripturerefs)
		set_text_module_global_option
		    ("Scripture Cross-references",
		     GTK_TOGGLE_BUTTON(tgs->t_btn_scripturerefs)->
		     active);
	if (tgs->hebrewpoints)
		set_text_module_global_option("Hebrew Vowel Points",
					      GTK_TOGGLE_BUTTON(tgs->
								t_btn_points)->
					      active);
	if (tgs->hebrewcant)
		set_text_module_global_option("Hebrew Cantillation",
					      GTK_TOGGLE_BUTTON(tgs->
								t_btn_cant)->
					      active);
	if (tgs->headings)
		set_text_module_global_option("Headings",
					      GTK_TOGGLE_BUTTON(tgs->
								t_btn_headings)->
					      active);
	if (tgs->words_in_red)
		set_text_module_global_option("Red letter words",
					      GTK_TOGGLE_BUTTON(tgs->
								t_btn_redwords)->
					      active);
	if (tgs->variants) {
		if (GTK_RADIO_MENU_ITEM(tgs->t_btn_primary)->
		    check_menu_item.active)
			set_text_global_option("Textual Variants",
					       "Primary Reading");
		else if (GTK_RADIO_MENU_ITEM(tgs->t_btn_secondary)->
			 check_menu_item.active)
			set_text_global_option("Textual Variants",
					       "Secondary Reading");
		else if (GTK_RADIO_MENU_ITEM(tgs->t_btn_all)->
			 check_menu_item.active)
			set_text_global_option("Textual Variants",
					       "All Readings");
	}
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include ".h"
 *
 *   void ( GString *str )	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void strip_words_in_red(GString * str)
{
	gchar *tmpbuf, *buf, color_tag[26];
	gint pos, len, len1, len2;

	strcpy(color_tag, "<font color=\"#FF0000\">");
	/* point buf to found verse */
	buf = str->str;

	len = strlen(color_tag);
	len1 = strlen(buf);
	while ((tmpbuf = strstr(buf, color_tag)) != NULL) {
		len2 = strlen(tmpbuf);
		pos = len1 - len2;
		/* remove color tag */
		str = g_string_erase(str, pos, len);
	}
}


/******************************************************************************
 * Name
 *   mark_search_words
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void mark_search_words( GString *str )	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void mark_search_words(GString * str)
{
	gchar *tmpbuf, *buf, *searchbuf;
	gint len1, len2, len3, len4;
	gchar closestr[40], openstr[40];

	/* regular expression search results         **fixme** */
	if (settings.searchType == 0) {
		return;
	}
	/* close tags */
	sprintf(closestr, "</b></font>");
	/* open tags */
	sprintf(openstr, "<font color=\"%s\"><b>",
		settings.found_color);
	/* point buf to found verse */
	buf = str->str;
	searchbuf = g_strdup(settings.searchText);

	/* if we have a muti word search go here */
	if (settings.searchType == -2) {
		char *token;
		GList *list;
		gint count = 0, i = 0;

		list = NULL;
		/* seperate the search words and add them to a glist */
		if ((token = strtok(searchbuf, " ")) != NULL) {
			list = g_list_append(list, token);
			++count;
			while ((token = strtok(NULL, " ")) != NULL) {
				list = g_list_append(list, token);
				++count;
			}
			/* if we have only one word */
		} else {
			list = g_list_append(list, searchbuf);
			count = 1;
		}
		list = g_list_first(list);
		/* find each word in the list and mark it */
		for (i = 0; i < count; i++) {
			/* set len1 to length of verse */
			len1 = strlen(buf);
			/* set len2 to length of search word */
			len2 = strlen((gchar *) list->data);
			/* find search word in verse */
			if ((tmpbuf =
			     strstr(buf,
				    (gchar *) list->data)) != NULL) {
				/* set len3 to length of tmpbuf 
				   (tmpbuf points to first occurance of 
				   search word in verse) */
				len3 = strlen(tmpbuf);
				//-- set len4 to diff between len1 and len3
				len4 = len1 - len3;
				/* add end tags first 
				   (position to add tag to is len4 + len2) */
				str =
				    g_string_insert(str, (len4 + len2),
						    closestr);
				/* then add start tags 
				   (position to add tag to is len4) */
				str =
				    g_string_insert(str, len4, openstr);
			}
			/* point buf to changed str */
			buf = str->str;
			list = g_list_next(list);
		}
		g_list_free(list);

		/* else we have a phrase and only need to mark it */
	} else {
		len1 = strlen(buf);
		len2 = strlen(searchbuf);
		tmpbuf = strstr(buf, searchbuf);
		if (tmpbuf) {
			len3 = strlen(tmpbuf);
			len4 = len1 - len3;
			/* place end tag first */
			str =
			    g_string_insert(str, (len4 + len2),
					    closestr);
			/* then place start tag */
			str = g_string_insert(str, len4, openstr);
		}
	}
	/* free searchbuf */
	g_free(searchbuf);
}

/******************************************************************************
 * Name
 *   entry_display
 *
 * Synopsis
 *   #include "gui/gtkhtml_display.h"
 *
 *   void entry_display(GtkWidget * html_widget, gchar * mod_name, 
 *					      gchar * text, gchar *key)
 *
 * Description
 *   display Sword modules a verse (entry) at a time
 *
 * Return value
 *   void
 */

void entry_display(GtkWidget * html_widget, gchar * mod_name,
		   gchar * text, gchar * key, gboolean show_key)
{
	gchar tmpBuf[500];
	gchar *use_font;
	gchar *use_font_size = NULL;
	gchar *utf8str;
	gint utf8len;
	GString *str;
	gboolean use_gtkhtml_font = FALSE;
	gboolean was_editable = FALSE;
	MOD_FONT *mf;
	GtkHTMLStreamStatus status1 = 0;
	GtkHTML *html;
	GtkHTMLStream *htmlstream;

	mf = get_font(mod_name);

	use_font = g_strdup(mf->old_font);
	//g_warning("use_font = %s",use_font);
	if (use_font) {
		if (!strncmp(use_font, "none", 4))
			use_gtkhtml_font = TRUE;
		else
			use_gtkhtml_font = FALSE;

	} else {
		use_gtkhtml_font = TRUE;

	}

	if ((mf->old_font_size[0] == '-')
	    || (mf->old_font_size[0] == '+'))
		use_font_size = g_strdup(mf->old_font_size);
	else
		use_font_size = g_strdup("+1");

	/* setup gtkhtml widget */
	html = GTK_HTML(html_widget);
	was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");


	sprintf(tmpBuf,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);
	utf8str = e_utf8_from_gtk_string(html_widget, tmpBuf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	/* show key in html widget  */
	if (show_key) {
		str = g_string_new("");
		if ((settings.displaySearchResults)) {
			g_string_sprintf(str,
					 "<A HREF=\"version=%s passage=%s\">"
					 "<FONT COLOR=\"%s\">[%s] %s </font></A>",
					 mod_name,
					 key,
					 settings.bible_verse_num_color,
					 mod_name, key);
		}

		else {
			g_string_sprintf(str,
					 "<A HREF=\"[%s] %s\">"
					 "<FONT COLOR=\"%s\">[%s]</A></font>[%s] ",
					 mod_name,
					 get_module_description
					 (mod_name),
					 settings.bible_verse_num_color,
					 mod_name, key);
		}

		utf8str = e_utf8_from_gtk_string(html_widget, str->str);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
		g_string_free(str, 0);
	}


	if (use_gtkhtml_font)
		sprintf(tmpBuf, "<font size=\"%s\">", use_font_size);
	else
		sprintf(tmpBuf, "<font face=\"%s\" size=\"%s\">",
			use_font, use_font_size);

	utf8str = e_utf8_from_gtk_string(html_widget, tmpBuf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	if (settings.displaySearchResults) {
		str = g_string_new(text);
		mark_search_words(str);
		utf8str = str->str;
	} else {
		str = g_string_new(text);
		utf8str = str->str;
	}
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	sprintf(tmpBuf, " %s", "</font></body></html>");
	utf8str = e_utf8_from_gtk_string(html_widget, tmpBuf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	if (use_font_size)
		free(use_font_size);
	if (use_font)
		free(use_font);
	g_free(mf);
	g_string_free(str, TRUE);
}

/******************************************************************************
 * Name
 *   chapter_display
 *
 * Synopsis
 *   #include "gui/gtkhtml_display.h"
 *
 *   void chapter_display(GtkWidget * html_widget, gchar * module_name, 
 *					      		 gchar *key)
 *
 * Description
 *   display Sword Bible texts a chapter at a time
 *
 * Return value
 *   void
 */

void chapter_display(GtkWidget * html_widget, gchar * mod_name,
		     TEXT_GLOBALS * tgs, gchar * key,
		     gboolean use_globals)
{
	gchar
	    * utf8str,
	    *bgColor,
	    *textColor,
	    buf[500],
	    *tmpkey, tmpbuf[256], *use_font, *use_font_size, *text_str;
	gchar *paragraphMark;

	gboolean was_editable = FALSE;
	gboolean newparagraph = FALSE;
	gboolean use_gtkhtml_font = FALSE;
	GString *str;
	gint utf8len, cur_verse, cur_chapter, i = 1;
	const char *cur_book;
	MOD_FONT *mf;

	GtkHTML *html = GTK_HTML(html_widget);
	GtkHTMLStreamStatus status1 = 0;
	GtkHTMLStream *htmlstream;

	was_editable = gtk_html_get_editable(html);
	tmpkey = get_valid_key(key);

	paragraphMark = "&para;";

	bgColor = "#f1f1f1";
	cur_verse = get_verse_from_key(tmpkey);
	cur_chapter = get_chapter_from_key(tmpkey);
	cur_book = get_book_from_key(tmpkey);

	str = g_string_new("");

	mf = get_font(mod_name);

	use_font = g_strdup(mf->old_font);
	//g_warning("use_font = %s",use_font);
	if (use_font) {
		if (!strncmp(use_font, "none", 4))
			use_gtkhtml_font = TRUE;
		else
			use_gtkhtml_font = FALSE;

	} else {
		use_gtkhtml_font = TRUE;

	}

	if ((mf->old_font_size[0] == '-')
	    || (mf->old_font_size[0] == '+'))
		use_font_size = g_strdup(mf->old_font_size);
	else
		use_font_size = g_strdup("+1");

	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");

	/*
	 * set global options for current module 
	 */
	if (use_globals)
		set_global_options(tgs);

	sprintf(buf,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);
	utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	for (i = 1;; i++) {
		sprintf(tmpbuf, "%s %d:%d", cur_book, cur_chapter, i);
		g_free(tmpkey);
		tmpkey = get_valid_key(tmpbuf);
		if (cur_chapter != get_chapter_from_key(tmpkey))
			break;

		if (i == cur_verse)
			textColor = settings.currentverse_color;
		else
			textColor = settings.bible_text_color;

		sprintf(buf,
			"&nbsp; <A HREF=\"*%s\" NAME=\"%d\">"
			"<font size=\"%s\" color=\"%s\">%d</font></A> ",
			tmpkey,
			i,
			settings.verse_num_font_size,
			settings.bible_verse_num_color, i);

		utf8str = e_utf8_from_gtk_string(html_widget, buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}

		if (use_gtkhtml_font) {
			sprintf(tmpbuf,
				"<font size=\"%s\" color=\"%s\">",
				use_font_size, textColor);
		} else {
			sprintf(tmpbuf,
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				use_font, use_font_size, textColor);
		}

		utf8str = e_utf8_from_gtk_string(html_widget, tmpbuf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			g_string_free(str, TRUE);
			str = g_string_new(utf8str);
		}

		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			strcpy(tmpbuf, paragraphMark);
			utf8str =
			    e_utf8_from_gtk_string(html_widget, tmpbuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				str = g_string_append(str, utf8str);
			}
		}

		/* get module text and prepare to display it */
		text_str = get_bibletext_text(mod_name, tmpkey);
		str = g_string_append(str, text_str);
		if (settings.displaySearchResults)
			mark_search_words(str);
		utf8str = str->str;
		free(text_str);

		if (settings.versestyle) {
			if ((strstr(utf8str, "<BR>") == NULL)
			    && (strstr(utf8str, "<!P>") == NULL)) {
				sprintf(tmpbuf, " %s", "</font><br>");
			} else {
				sprintf(tmpbuf, " %s", "</font>");
			}
			if (strstr(utf8str, "<!P>") == NULL) {
				newparagraph = FALSE;
			} else {
				newparagraph = TRUE;
			}
		}

		else {
			if (strstr(utf8str, "<!P>") == NULL)
				sprintf(tmpbuf, " %s", "</font>");
			else
				sprintf(tmpbuf, " %s", "</font><p>");
		}
		utf8str = e_utf8_from_gtk_string(html_widget, tmpbuf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			str = g_string_append(str, utf8str);
			gtk_html_write(html, htmlstream, str->str,
				       str->len);
		}

	}

	sprintf(buf, "%s", "</body></html>");
	utf8str = e_utf8_from_gtk_string(html_widget, buf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	sprintf(buf, "%d", cur_verse);
	gtk_html_jump_to_anchor(html, buf);

	if (use_font_size)
		free(use_font_size);
	if (use_font)
		free(use_font);
	g_free(mf);
	g_free(tmpkey);
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/gtkhtml_display.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_module_is_locked_display(GtkWidget * html_widget,
				  gchar * mod_name, gchar * cipher_key)
{
	GtkHTML *html = GTK_HTML(html_widget);
	GtkHTMLStreamStatus status1 = 0;
	GtkHTMLStream *htmlstream;
	gchar buf[500], *utf8str;
	gint utf8len;


	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");

	sprintf(buf, "%s", HTML_START "<body><br>");
	utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	if (!cipher_key) {
		sprintf(buf, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name, "</b>", _("module is locked."));
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	else {
		sprintf(buf, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name,
			"</b>", _("module has been unlocked."));
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}




	if (!cipher_key) {
		sprintf(buf, "%s %s %s%s%s %s %s %s",
			"<br><br>",
			_("If you have the cipher key you can"),
			"<a href=\"U",
			mod_name,
			"\">",
			_("click here"),
			" </a>", _("to unlock the module"));
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}

		sprintf(buf, "%s%s",
			"<br><br>",
			_
			("You will have to restart GnomeSword after you unlock it."));

		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	else {
		sprintf(buf, "%s%s",
			"<br><br>",
			_("You need to restart GnomeSword to view it"));
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}


	strcpy(buf, "</body></html>");
	utf8str = e_utf8_from_gtk_string(html_widget, buf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);

}

/******   end of file   ******/
