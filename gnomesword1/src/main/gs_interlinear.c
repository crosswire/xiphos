/*
 * GnomeSword Bible Study Tool
 * gs_interlinear.c - support for displaying multiple modules
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
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gal/widgets/e-unicode.h>

#include "gs_gnomesword.h"
#include "gs_interlinear.h"
#include "sword.h"
#include "interlinear.h"

extern gboolean havebible;

GtkHTMLStreamStatus status1;
GtkHTMLStream *htmlstream;

void update_interlinear_page(SETTINGS * s)
{
	gchar tmpBuf[256], *rowcolor, *font_size;
	gchar *utf8str,*mod_name, *font_name;
	gint utf8len, i;
	gboolean was_editable, use_gtkhtml_font;
	
	if (havebible) {
		/* setup gtkhtml widget */
		GtkHTML *html = GTK_HTML(s->htmlInterlinear);
		was_editable = gtk_html_get_editable(html);
		if (was_editable)
			gtk_html_set_editable(html, FALSE);
		htmlstream =
		    gtk_html_begin_content(html,
					   "text/html; charset=utf-8");
		sprintf(tmpBuf,
			"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table>",
			s->bible_bg_color,
			s->bible_text_color,
			s->link_color);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear,
					   tmpBuf);
		utf8len = strlen(utf8str);      
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
		
		for(i = 0; i < 5; i++) {
			mod_name = NULL;
			switch(i) {
				case 0:
					mod_name = s->Interlinear1Module;					
				break;
				case 1:
					mod_name = s->Interlinear2Module;
				break;
				case 2:
					mod_name = s->Interlinear3Module;
				break;
				case 3:
					mod_name = s->Interlinear4Module;
				break;
				case 4:
					mod_name = s->Interlinear5Module;
				break;
			}
			
			if((font_name = backend_get_module_font_name(mod_name)) != NULL)
				use_gtkhtml_font = FALSE;
			else
				use_gtkhtml_font = TRUE;
			
			font_size = s->interlinear_font_size; 
			
			if (i == 0 || i == 2 || i == 4)
				rowcolor = "#F1F1F1";
			else
				rowcolor = s->bible_bg_color;
			
			if (i == 0) {
				sprintf(tmpBuf,
					"<tr><td><i><FONT COLOR=\"%s\" SIZE=\"%s\">[%s]</font></i></td></tr>",
					s->bible_verse_num_color,
					s->verse_num_font_size, 
					s->currentverse);
				utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, tmpBuf);
				utf8len = strlen(utf8str);
				if (utf8len) {
					gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
				}	
			}
			
			sprintf(tmpBuf,
				"<tr bgcolor=\"%s\"><td><B><A HREF=\"[%s]%s\"><FONT COLOR=\"%s\" SIZE=\"%s\"> [%s]</font></a></b>",
				rowcolor,
				mod_name,
				backend_get_module_description(mod_name),
				s->bible_verse_num_color,
				s->verse_num_font_size,
				mod_name);
			utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, tmpBuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}			
				
			if(use_gtkhtml_font)
				sprintf(tmpBuf, "<font size=\"%s\">", font_size);
			else
				sprintf(tmpBuf, "<font face=\"%s\"size=\"%s\">", font_name, font_size);
				
			utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, tmpBuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
				
			 utf8str = backend_get_interlinear_module_text(mod_name, s->currentverse);
			if (strlen(utf8str)) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, strlen(utf8str));
				g_free(utf8str);
			}
			
			sprintf(tmpBuf,
				"</font><small>[<A HREF=\"@%s\">view context</a>]</small></td></tr>",
				mod_name);
			utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, tmpBuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
		}		
		
		sprintf(tmpBuf, "</table></body></html>");
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear,
					   tmpBuf);
		utf8len = strlen(utf8str);       
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}

		gtk_html_end(GTK_HTML(html), htmlstream, status1);
		gtk_html_set_editable(html, was_editable);
	}
}

static void int_display(SETTINGS *s, gchar * key)
{
	gchar 
		*utf8str,
		*bgColor,
		*textColor,
		buf[500], 
		*tmpkey,
		tmpbuf[256],
		*mod_name,
		*use_font_size;
	
	gboolean 
		evenRow = FALSE;
	
	
	gint 
		utf8len,
		cur_verse,
		cur_chapter,
		i = 1,j;
	
	const char *cur_book;
	
	GtkHTML *html = GTK_HTML(s->htmlInterlinear);
	
	tmpkey = backend_get_valid_key(key);
	
	bgColor = "#f1f1f1";
	cur_verse = backend_get_verse_from_key(tmpkey);
	s->intCurVerse = cur_verse;
	cur_chapter = backend_get_chapter_from_key(tmpkey);
	cur_book = backend_get_book_from_key(tmpkey);
	
	for (i = 1; ; i++) {	
		sprintf(tmpbuf,"%s %d:%d",cur_book,cur_chapter,i);
		g_free(tmpkey);
		tmpkey = backend_get_valid_key(tmpbuf);
		if(cur_chapter != backend_get_chapter_from_key(tmpkey))
			break;
		sprintf(buf,"%s","<tr valign=\"top\">");		
		utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}		
				
		if(i == cur_verse)
			textColor = s->currentverse_color;
		else 
			textColor = s->bible_text_color;
		
		if(evenRow) {
			evenRow = FALSE;
			bgColor = s->bible_bg_color;
		}
			
		else {
			evenRow = TRUE;
			bgColor = "#f1f1f1";				
		}
		
		for(j = 0; j < 5; j++) {	
			mod_name = NULL;
			switch(j) {
				case 0:
					mod_name = s->Interlinear1Module;					
				break;
				case 1:
					mod_name = s->Interlinear2Module;
				break;
				case 2:
					mod_name = s->Interlinear3Module;
				break;
				case 3:
					mod_name = s->Interlinear4Module;
				break;
				case 4:
					mod_name = s->Interlinear5Module;
				break;
			}
			
			use_font_size = s->interlinear_font_size;
							
			sprintf(buf,
				"<td width=\"20%%\" bgcolor=\"%s\">"
				"<A HREF=\"I%s\" NAME=\"%d\">"
				"<font color=\"%s\">%d. </font></A>"
				"<font size=\"%s\" color=\"%s\">", 
				bgColor,
				tmpkey,
				i,				 
				s->bible_verse_num_color, 
				i,
				use_font_size,
				textColor);	
			
			utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
			utf8len = strlen(utf8str);		
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
			
			utf8str = backend_get_interlinear_module_text(mod_name, tmpkey);
			if (strlen(utf8str)) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, strlen(utf8str));
				g_free(utf8str);
			}
			
			sprintf(buf, "%s", "</font></td>");	
			utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
			utf8len = strlen(utf8str);		
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
		}
				
		sprintf(buf,"%s","</tr>");		
		utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}
	}	
	g_free(tmpkey);
}

void update_interlinear_page_detached(SETTINGS * s)
{
	gchar * utf8str, buf[500];
	gint utf8len;

	//-- setup gtkhtml widget
	GtkHTML *html = GTK_HTML(s->htmlInterlinear);
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");


	sprintf(buf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table align=\"left\" valign=\"top\"><tr valign=\"top\" >",
		s->bible_bg_color, s->bible_text_color, s->link_color);
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	if (s->Interlinear1Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear1Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (s->Interlinear2Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear2Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (s->Interlinear3Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear3Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (s->htmlInterlinear) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear4Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (s->Interlinear5Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear5Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	sprintf(buf, "%s", "</tr>");
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}


	/******      ******/
	int_display(s, s->cvInterlinear);

	sprintf(buf, "%s", "</table></body></html>");
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	sprintf(buf, "%d", s->intCurVerse);
	gtk_html_jump_to_anchor(html, buf);	
}
/******************************************************************************
 * swaps interlinear mod with mod in main text window
 * intmod - interlinear mod name
 ******************************************************************************/
void swap_interlinear_with_main(char * intmod, SETTINGS *s)
{
	char *modname;

	modname = s->MainWindowModule;
	if (!strcmp(s->Interlinear5Module, intmod)) {
		sprintf(s->Interlinear5Module, "%s", modname);
	}
	if (!strcmp(s->Interlinear4Module, intmod)) {
		sprintf(s->Interlinear4Module, "%s", modname);
	}
	if (!strcmp(s->Interlinear3Module, intmod)) {
		sprintf(s->Interlinear3Module, "%s", modname);
	}
	if (!strcmp(s->Interlinear2Module, intmod)) {
		sprintf(s->Interlinear2Module, "%s", modname);
	}
	if (!strcmp(s->Interlinear1Module, intmod)) {
		sprintf(s->Interlinear1Module, "%s", modname);
	}
	change_module_and_key(intmod, s->currentverse);
	update_interlinear_page(s);
}
void set_interlinear_module_global_options(gchar * option,
			       gboolean choice,
				SETTINGS *s)
{
	gchar *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}	

	if (!strcmp(option, "Strong's Numbers")) {
		s->strongsint = choice;
	}

	if (!strcmp(option, "Footnotes")) {
		s->footnotesint = choice;
	}

	if (!strcmp(option, "Morphological Tags")) {
		s->morphsint = choice;
	}

	if (!strcmp(option, "Hebrew Vowel Points")) {
		s->hebrewpointsint = choice;
	}

	if (!strcmp(option, "Hebrew Cantillation")) {
		s->cantillationmarksint = choice;
	}

	if (!strcmp(option, "Greek Accents")) {
		s->greekaccentsint = choice;
	}
	
	backend_set_interlinear_global_option(option, on_off);
	/* 
	   display change 
	 */
	if (s->dockedInt)	
		update_interlinear_page(s);
	else
		update_interlinear_page_detached(s);
}
/*****   end of file   ******/
