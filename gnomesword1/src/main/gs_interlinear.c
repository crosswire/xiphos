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
#include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gal/widgets/e-unicode.h>

#include "main/gs_gnomesword.h"
#include "main/gs_interlinear.h"

#include "backend/sword.h"
#include "backend/interlinear.h"

extern gboolean havebible;

GtkHTMLStreamStatus status1;
GtkHTMLStream *htmlstream;

void update_interlinear_page()
{
	gchar tmpBuf[256], *rowcolor, *font_size;
	gchar *utf8str,*mod_name, *font_name = NULL;
	gint utf8len, i, j;
	gboolean was_editable, use_gtkhtml_font;
	
	if (havebible) {
		/* setup gtkhtml widget */
		GtkHTML *html = GTK_HTML(settings.htmlInterlinear);
		was_editable = gtk_html_get_editable(html);
		if (was_editable)
			gtk_html_set_editable(html, FALSE);
		htmlstream =
		    gtk_html_begin_content(html,
					   "text/html; charset=utf-8");
		sprintf(tmpBuf,
			"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table>",
			settings.bible_bg_color,
			settings.bible_text_color,
			settings.link_color);
		utf8str =
		    e_utf8_from_gtk_string(settings.htmlInterlinear,
					   tmpBuf);
		utf8len = strlen(utf8str);      
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
		
		for(i = 0, j = 0; i < 5; i++) {
			mod_name = NULL;
			switch(i) {
				case 0:
					mod_name = settings.Interlinear1Module;					
				break;
				case 1:
					mod_name = settings.Interlinear2Module;
				break;
				case 2:
					mod_name = settings.Interlinear3Module;
				break;
				case 3:
					mod_name = settings.Interlinear4Module;
				break;
				case 4:
					mod_name = settings.Interlinear5Module;
				break;
			}
			
			if(!*mod_name) continue;

			++j;
			
			if((font_name = get_module_font_name(mod_name)) != NULL)
				use_gtkhtml_font = FALSE;
			else
				use_gtkhtml_font = TRUE;
			
			font_size = settings.interlinear_font_size; 
			
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
				utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, tmpBuf);
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
				settings.bible_verse_num_color,
				settings.verse_num_font_size,
				mod_name);
			utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, tmpBuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}			
				
			if(use_gtkhtml_font)
				sprintf(tmpBuf, "<font size=\"%s\">", font_size);
			else
				sprintf(tmpBuf, "<font face=\"%s\"size=\"%s\">", font_name, font_size);
				
			utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, tmpBuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
				
			utf8str = backend_get_interlinear_module_text(mod_name, settings.currentverse);
			if (strlen(utf8str)) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, strlen(utf8str));
				g_free(utf8str);
			}
			
			sprintf(tmpBuf,
				"</font><small>[<A HREF=\"@%s\">view context</a>]</small></td></tr>",
				mod_name);
			utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, tmpBuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
		}		
		
		sprintf(tmpBuf, "</table></body></html>");
		utf8str =
		    e_utf8_from_gtk_string(settings.htmlInterlinear,
					   tmpBuf);
		utf8len = strlen(utf8str);       
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}

		gtk_html_end(GTK_HTML(html), htmlstream, status1);
		gtk_html_set_editable(html, was_editable);
	}
	gtk_frame_set_label(GTK_FRAME(settings.frameInt), settings.currentverse);
	if(font_name)
		free(font_name);
}

static void int_display(gchar *key)
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
	
	GtkHTML *html = GTK_HTML(settings.htmlInterlinear);
	
	tmpkey = backend_get_valid_key(key);
	
	bgColor = "#f1f1f1";
	cur_verse = backend_get_verse_from_key(tmpkey);
	settings.intCurVerse = cur_verse;
	cur_chapter = backend_get_chapter_from_key(tmpkey);
	cur_book = backend_get_book_from_key(tmpkey);
	
	for (i = 1; ; i++) {	
		sprintf(tmpbuf,"%s %d:%d",cur_book,cur_chapter,i);
		free(tmpkey);
		tmpkey = backend_get_valid_key(tmpbuf);
		if(cur_chapter != backend_get_chapter_from_key(tmpkey))
			break;
		sprintf(buf,"%s","<tr valign=\"top\">");		
		utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}		
				
		if(i == cur_verse)
			textColor = settings.currentverse_color;
		else 
			textColor = settings.bible_text_color;
		
		if(evenRow) {
			evenRow = FALSE;
			bgColor = settings.bible_bg_color;
		}
			
		else {
			evenRow = TRUE;
			bgColor = "#f1f1f1";				
		}
		
		for(j = 0; j < 5; j++) {	
			mod_name = NULL;
			switch(j) {
				case 0:
					mod_name = settings.Interlinear1Module;
				break;
				case 1:
					mod_name = settings.Interlinear2Module;
				break;
				case 2:
					mod_name = settings.Interlinear3Module;
				break;
				case 3:
					mod_name = settings.Interlinear4Module;
				break;
				case 4:
					mod_name = settings.Interlinear5Module;
				break;
			}
			
			use_font_size = settings.interlinear_font_size;
							
			sprintf(buf,
				"<td width=\"20%%\" bgcolor=\"%s\">"
				"<A HREF=\"I%s\" NAME=\"%d\">"
				"<font color=\"%s\">%d. </font></A>"
				"<font size=\"%s\" color=\"%s\">", 
				bgColor,
				tmpkey,
				i,				 
				settings.bible_verse_num_color, 
				i,
				use_font_size,
				textColor);	
			
			utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
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
			utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
			utf8len = strlen(utf8str);		
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
		}
				
		sprintf(buf,"%s","</tr>");		
		utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}
	}	
	free(tmpkey);
}

void update_interlinear_page_detached(void)
{
	gchar * utf8str, buf[500];
	gint utf8len;

	//-- setup gtkhtml widget
	GtkHTML *html = GTK_HTML(settings.htmlInterlinear);
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");


	sprintf(buf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table align=\"left\" valign=\"top\"><tr valign=\"top\" >",
		settings.bible_bg_color, settings.bible_text_color, settings.link_color);
	utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	if (settings.Interlinear1Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.Interlinear1Module);
		utf8str =
		    e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (settings.Interlinear2Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.Interlinear2Module);
		utf8str =
		    e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (settings.Interlinear3Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.Interlinear3Module);
		utf8str =
		    e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (settings.htmlInterlinear) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.Interlinear4Module);
		utf8str =
		    e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (settings.Interlinear5Module) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.Interlinear5Module);
		utf8str =
		    e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	sprintf(buf, "%s", "</tr>");
	utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}


	/******      ******/
	int_display(settings.cvInterlinear);

	sprintf(buf, "%s", "</table></body></html>");
	utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	sprintf(buf, "%d", settings.intCurVerse);
	gtk_html_jump_to_anchor(html, buf);	
}
/******************************************************************************
 * swaps interlinear mod with mod in main text window
 * intmod - interlinear mod name
 ******************************************************************************/
void swap_interlinear_with_main(char * intmod)
{
	char *modname;

	modname = settings.MainWindowModule;
	if (!strcmp(settings.Interlinear5Module, intmod)) {
		sprintf(settings.Interlinear5Module, "%s", modname);
	}
	if (!strcmp(settings.Interlinear4Module, intmod)) {
		sprintf(settings.Interlinear4Module, "%s", modname);
	}
	if (!strcmp(settings.Interlinear3Module, intmod)) {
		sprintf(settings.Interlinear3Module, "%s", modname);
	}
	if (!strcmp(settings.Interlinear2Module, intmod)) {
		sprintf(settings.Interlinear2Module, "%s", modname);
	}
	if (!strcmp(settings.Interlinear1Module, intmod)) {
		sprintf(settings.Interlinear1Module, "%s", modname);
	}
	change_module_and_key(intmod, settings.currentverse);
	update_interlinear_page();
}

void set_interlinear_module_global_options(gchar *option, gboolean choice)
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
	
	backend_set_interlinear_global_option(option, on_off);

	/* display change */
	if (settings.dockedInt)	{
		update_interlinear_page();
	}
	else {
		update_interlinear_page_detached();
	}
}

