/*
 * GnomeSword Bible Study Tool
 * gtkhtml_dispaly.c - display Sword modules using GtkHTML 
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
#include "gui/gnomesword.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/bibletext.h"
#include "main/settings.h"
#include "main/key.h"


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
	set_text_module_global_option("Strong's Numbers", tgs->strongs);
	set_text_module_global_option("Morphological Tags",
				      tgs->morphs);
	set_text_module_global_option("Footnotes", tgs->footnotes);
	set_text_module_global_option("Greek Accents",
				      tgs->greekaccents);
	set_text_module_global_option("Lemmas", tgs->lemmas);
	set_text_module_global_option("Cross-references",
				      tgs->scripturerefs);
	set_text_module_global_option("Hebrew Vowel Points",
				      tgs->hebrewpoints);
	set_text_module_global_option("Hebrew Cantillation",
				      tgs->hebrewcant);
	set_text_module_global_option("Headings", tgs->headings);
	set_text_module_global_option("Words of Christ in Red",
				      tgs->words_in_red);
	if (tgs->variants_primary)
		set_text_global_option("Textual Variants",
				       "Primary Reading");
	else if (tgs->variants_secondary)
		set_text_global_option("Textual Variants",
				       "Secondary Reading");
	else
		set_text_global_option("Textual Variants",
				       "All Readings");
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
	gchar *use_font = FALSE;
	gchar *use_font_size = NULL;
	GString *str;
	GString *search_str;
	gboolean use_gtkhtml_font = FALSE;
	gboolean was_editable = FALSE;
	MOD_FONT *mf;
	GtkHTMLStreamStatus status1 = 0;
	GtkHTML *html;
	GtkHTMLStream *htmlstream;

	mf = get_font(mod_name);
	//if(mf->old_font)
	use_font = g_strdup(mf->old_font);
	//g_warning("mod = %s use_font = %s", mod_name,use_font);
	//g_warning("use_font = %s",use_font);
	if (use_font) {
		if (!strncmp(use_font, "none", 4))
			use_gtkhtml_font = TRUE;
		else
			use_gtkhtml_font = FALSE;

	} else {
		use_gtkhtml_font = TRUE;

	}
	use_font_size = g_strdup(mf->old_font_size);

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

	str = g_string_new(tmpBuf);
	/* show key in html widget  */
	if (show_key) {
		if ((settings.displaySearchResults)) {
			sprintf(tmpBuf,
				"<A HREF=\"version=%s passage=%s\">"
				"<FONT COLOR=\"%s\">[%s] %s </font></A>",
				mod_name,
				key,
				settings.bible_verse_num_color,
				mod_name, key);
		}

		else {
			sprintf(tmpBuf,
				"<A HREF=\"[%s] %s\">"
				"<FONT COLOR=\"%s\">[%s]</A></font>[%s] ",
				mod_name,
				get_module_description
				(mod_name),
				settings.bible_verse_num_color,
				mod_name, key);
		}
		str = g_string_append(str, tmpBuf);
	}


	if (use_gtkhtml_font)
		sprintf(tmpBuf, "<font size=\"%s\">", use_font_size);
	else
		sprintf(tmpBuf, "<font face=\"%s\" size=\"%s\">",
			use_font, use_font_size);
	str = g_string_append(str, tmpBuf);

	if (settings.displaySearchResults) {
		search_str = g_string_new(text);
		mark_search_words(search_str);
		str = g_string_append(str, search_str->str);
	} else {
		str = g_string_append(str, text);
	}

	sprintf(tmpBuf, " %s", "</font></body></html>");
	str = g_string_append(str, tmpBuf);

	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
	}
	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	if (use_font_size)
		free(use_font_size);
	if (use_font)
		free(use_font);
	//free_font(mf);
	free_font(mf);
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
	gchar *bgColor;
	gchar *textColor;
	gchar buf[500];
	gchar *tmpkey, tmpbuf[256], *use_font, *use_font_size;
	gchar *text_str = NULL;
	gchar *paragraphMark;
	gint count;
	gboolean was_editable = FALSE;
	gboolean newparagraph = FALSE;
	gboolean use_gtkhtml_font = FALSE;
	GString *str;
	gint cur_verse, cur_chapter, i = 1;
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



	mf = get_font(mod_name);

	use_font = g_strdup(mf->old_font);
	if (use_font) {
		if (!strncmp(use_font, "none", 4))
			use_gtkhtml_font = TRUE;
		else
			use_gtkhtml_font = FALSE;

	} else {
		use_gtkhtml_font = TRUE;

	}
	use_font_size = g_strdup(mf->old_font_size);

	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");

	/*
	 * set global options for current module 
	 */
	if (use_globals)
		set_global_options(tgs);

	sprintf(buf,
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);

	str = g_string_new(buf);
	tmpkey = get_valid_key(key);
	count = verse_count(tmpkey);
	for (i = 1; i <= count; i++) {
		sprintf(tmpbuf, "%s %d:%d", cur_book, cur_chapter, i);
		g_free(tmpkey);
		tmpkey = get_valid_key(tmpbuf);

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

		str = g_string_append(str, buf);

		if (!use_gtkhtml_font) {
			sprintf(tmpbuf,
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				use_font, use_font_size, textColor);
		} else {
			sprintf(tmpbuf,
				"<font size=\"%s\" color=\"%s\">",
				use_font_size, textColor);
		}
		str = g_string_append(str, tmpbuf);


		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			strcpy(tmpbuf, paragraphMark);
			str = g_string_append(str, tmpbuf);
		}

		/* get module text and prepare to display it */
		text_str = get_module_text(0, mod_name, tmpkey);
		str = g_string_append(str, text_str);
		/*
		   if (settings.displaySearchResults)
		   mark_search_words(str);
		   utf8str = str->str;
		 */

		if (settings.versestyle) {
			if ((strstr(text_str, "<BR>") == NULL) &&
			    (strstr(text_str, "<br />") == NULL) &&
			     (strstr(text_str, "<!P>") == NULL) &&
			     (strstr(text_str, "<p>") == NULL) &&
			     (strstr(text_str, "</p>") == NULL)  ) {
				sprintf(tmpbuf, " %s", "</font><br>");
			} else {
				sprintf(tmpbuf, " %s", "</font>");
			}
			if ((strstr(text_str, "<!P>") == NULL) &&
			     (strstr(text_str, "<p>") == NULL) ) {
				newparagraph = FALSE;
			} else {
				newparagraph = TRUE;
			}
		}

		else {
			if (strstr(text_str, "<!P>") == NULL)
				sprintf(tmpbuf, " %s", "</font>");
			else
				sprintf(tmpbuf, " %s", "</font><p>");
		}
		free(text_str);
		str = g_string_append(str, tmpbuf);
	}

	sprintf(buf, "%s", "</body></html>");
	str = g_string_append(str, buf);
	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	if(cur_verse > 1) {
		sprintf(buf, "%d", cur_verse - 1);
		gtk_html_jump_to_anchor(html, buf);
	}
	

	g_string_free(str, TRUE);
	if (use_font_size)
		g_free(use_font_size);
	if (use_font)
		g_free(use_font);
	free_font(mf);
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
 *   display Sword Bible texts a chapter at a time
 *
 * Return value
 *   void
 */


void chapter_display_textview(GtkWidget * textview, gchar * mod_name,
			     TEXT_GLOBALS * tgs, gchar * key,
			     gboolean use_globals)
{
	const gchar *cur_book;
	const gchar *mark_name = "CurrentVerse";
	gchar *tmpkey;
	gchar *text_str;
	gchar tmpbuf[256];
	GString *str;
	gint cur_verse, cur_chapter, i = 1;
	GtkTextMark   *mark = NULL;
	GtkTextIter iter,startiter, enditer;
	GtkTextBuffer *buffer;
	
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	tmpkey = get_valid_key(key);
	
	cur_verse = get_verse_from_key(tmpkey);
	cur_chapter = get_chapter_from_key(tmpkey);
	cur_book = get_book_from_key(tmpkey);
	
	/* clear the buffer */
	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_delete(buffer, &startiter, &enditer);
	
	str = g_string_new("");
	for (i = 1;; i++) {
		sprintf(tmpbuf, "%s %d:%d", cur_book, cur_chapter, i);
		g_free(tmpkey);
		tmpkey = get_valid_key(tmpbuf);
		if (cur_chapter != get_chapter_from_key(tmpkey))
			break;
		g_string_printf(str, "%d", i);
	        gtk_text_buffer_get_end_iter(buffer, &iter);
		gtk_text_buffer_insert_with_tags (buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                                               "verseNumber"),
                                        NULL);		
		text_str = get_module_text(0, mod_name, tmpkey);
		g_string_printf(str, ". %s", text_str);
		if(i == cur_verse) {
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			mark = gtk_text_buffer_create_mark (buffer,
						mark_name,
						&iter,
						TRUE);
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			gtk_text_buffer_insert_with_tags (buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_currentverse"),
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),			
                                        NULL);		
		} else {
			gtk_text_buffer_get_end_iter(buffer, &iter);			
			gtk_text_buffer_insert_with_tags (buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_verse"),
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),		
                                        NULL);		
		} 
		gtk_text_buffer_get_end_iter(buffer, &iter);
		gtk_text_buffer_insert(buffer, &iter, "\n", 1);
	}
	g_string_free(str,0);
	
	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_apply_tag_by_name(buffer, 
			"rtl_quote", 
			&startiter, 
			&enditer);
		
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(textview),
				mark,
				FALSE,
				TRUE,
				0.0,
				0.0);
	gtk_text_buffer_delete_mark(buffer,mark);
	g_free(tmpkey);	
}

/*
#ifdef ICU_SWORD
void chapter_display_icu(GtkWidget * html_widget, gchar * mod_name,
		TEXT_GLOBALS * tgs, gchar * key, gboolean use_globals)
{
	gchar
	    *bgColor,
	    *textColor,
	    buf[500],
	    *tmpkey, tmpbuf[256], *use_font, *use_font_size;
	gchar *text_str = NULL;
	gchar *paragraphMark;
	gchar *body;
	gint count;
	gboolean newparagraph = FALSE;
	gboolean use_gtkhtml_font = FALSE;
	gboolean was_editable = FALSE;
	GString *str;
	gint cur_verse, cur_chapter, i = 1;
	const char *cur_book;
	gboolean r2l = FALSE;
	MOD_FONT *mf;

	GtkHTML *html = GTK_HTML(html_widget);
	GtkHTMLStreamStatus status1 = 0;
	GtkHTMLStream *htmlstream;
	
	was_editable = gtk_html_get_editable(html);
	


	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");
		
	if(is_module_rtl(mod_name))
		r2l = TRUE;
	
	
	tmpkey = get_valid_key(key);

	paragraphMark = "&para;";

	bgColor = "#f1f1f1";
	cur_verse = get_verse_from_key(tmpkey);
	cur_chapter = get_chapter_from_key(tmpkey);
	cur_book = get_book_from_key(tmpkey);

	

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

	
	if (use_globals)
		set_global_options(tgs);

	if(r2l) {
		body = "body dir=\"rtl\"";
	}
	else {
		body = "body";
		
	}
	sprintf(buf,
		HTML_START
		"<%s bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		body,
		settings.bible_bg_color, 
		settings.bible_text_color,
		settings.link_color);
	
	str = g_string_new(buf);
	tmpkey = get_valid_key(key);
	count  = verse_count(tmpkey);
	for (i = 1 ;i <= count; i++) {
		sprintf(tmpbuf, "%s %d:%d", cur_book, cur_chapter, i);
		g_free(tmpkey);
		tmpkey = get_valid_key(tmpbuf);

		if (i == cur_verse)
			textColor = settings.currentverse_color;
		else
			textColor = settings.bible_text_color;
		
		if (use_gtkhtml_font) {
			sprintf(tmpbuf,
				"<font size=\"%s\" color=\"%s\">",
				use_font_size, textColor);
		} else {
			sprintf(tmpbuf,
			  "<font face=\"%s\" size=\"%s\" color=\"%s\">",
				use_font, use_font_size, textColor);
		}
		
		
		str = g_string_append(str,tmpbuf);
		
		
		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			strcpy(tmpbuf, paragraphMark);
			str = g_string_append(str,tmpbuf);
		}

		
		text_str = get_module_text(0, mod_name, tmpkey);
		
		str = g_string_append(str, text_str);

		if (settings.versestyle) {
			if ((strstr(text_str, "<BR>") == NULL)
			    && (strstr(text_str, "<!P>") == NULL)) {
				sprintf(tmpbuf, " %s", "</font><br>");
			} else {
				sprintf(tmpbuf, " %s", "</font>");
			}
			if (strstr(text_str, "<!P>") == NULL) {
				newparagraph = FALSE;
			} else {
				newparagraph = TRUE;
			}
		}

		else {
			if (strstr(text_str, "<!P>") == NULL)
				sprintf(tmpbuf, " %s", "</font>");
			else
				sprintf(tmpbuf, " %s", "</font><p>");
		}
		free(text_str);
		str = g_string_append(str,tmpbuf);
	}

	sprintf(buf, "%s", "</body></html>");
	str = g_string_append(str,buf);
	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	sprintf(buf, "%d", cur_verse);
	gtk_html_jump_to_anchor(html, buf);
	
	
	g_string_free(str, TRUE);
	if (use_font_size)
		g_free(use_font_size);
	if (use_font)
		g_free(use_font);
	free_font(mf);
	g_free(tmpkey);
}
#endif
*/

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
	gchar buf[500];
	GString *str;

	str = g_string_new("");
	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");

	g_string_printf(str, "%s", HTML_START "<body><br>");
	//utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
	//utf8len = strlen(utf8str);
	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
	}

	if (!cipher_key) {
		g_string_printf(str, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name, "</b>", _("module is locked."));
		//g_string_printf(str, "%s", buf);
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	}

	else {
		g_string_printf(str, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name,
			"</b>", _("module has been unlocked."));
		
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	}




	if (!cipher_key) {
		g_string_printf(str, "%s %s %s%s%s %s %s %s",
			"<br><br>",
			_("If you have the cipher key you can"),
			"<a href=\"U",
			mod_name,
			"\">",
			_("click here"),
			" </a>", _("to unlock the module"));
		
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}

		g_string_printf(str, "%s%s",
			"<br><br>",
			_
			("You will have to restart GnomeSword after you unlock it."));

		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	}

	else {
		g_string_printf(str, "%s%s",
			"<br><br>",
			_("You need to restart GnomeSword to view it"));
		
		if (str->len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       str->str, str->len);
		}
	}


	g_string_printf(str, "%s", "</body></html>");
	if (str->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	g_string_free(str,TRUE);

}

/******   end of file   ******/
