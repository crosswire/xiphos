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

#include "gui/interlinear.h"
#include "gui/interlinear_dialog.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"

//#include "main/gs_gnomesword.h"
#include "main/bibletext.h"
#include "main/lists.h"
#include "main/sword.h"



/******************************************************************************
 * static
 */
static GtkHTMLStreamStatus status1;
static GtkHTMLStream *htmlstream;
static GtkWidget *module_options_menu;


/******************************************************************************
 * Name
 *   gui_set_interlinear_options_at_start
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   void gui_set_interlinear_options_at_start(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_set_interlinear_options_at_start(void)
{
	
	if (settings.footnotesint)
		set_interlinear_global_option("Footnotes", "On");	/* keep footnotes in sync with menu */
	else
		set_interlinear_global_option("Footnotes", "Off");	/* keep footnotes in sync with menu */

	/*
	   set interlinear Strong's Numbers to last setting used 
	 */
	if (settings.strongsint)
		set_interlinear_global_option("Strong's Numbers", "On");	/* keep Strongs in sync with menu */
	else
		set_interlinear_global_option("Strong's Numbers", "Off");	/* keep Strongs in sync with menu */

	/*
	   set interlinear morph tags to last setting used 
	 */
	if (settings.morphsint)
		set_interlinear_global_option("Morphological Tags", "On");	/* keep Morph Tags in sync with menu */
	else
		set_interlinear_global_option("Morphological Tags", "Off");	/* keep Morph Tag in sync with menu */

	/*
	   set interlinear Hebrew Vowel Points to last setting used 
	 */
	if (settings.hebrewpointsint)
		set_interlinear_global_option("Hebrew Vowel Points", "On");	/* keep Hebrew Vowel Points in sync with menu */
	else
		set_interlinear_global_option("Hebrew Vowel Points", "Off");	/* keep Hebrew Vowel Points in sync with menu */

	/*
	   set interlinear Hebrew Cantillation to last setting used 
	 */
	if (settings.cantillationmarksint)
		set_interlinear_global_option("Hebrew Cantillation", "On");	/* keep Hebrew Cantillation in sync with menu */
	else
		set_interlinear_global_option("Hebrew Cantillation", "Off");	/* keep Hebrew Cantillation in sync with menu */

	/*
	   set interlinear Greek Accents to last setting used 
	 */
	if (settings.greekaccentsint)
		set_interlinear_global_option("Greek Accents", "On");	/* keep Greek Accents in sync with menu */
	else
		set_interlinear_global_option("Greek Accents", "Off");	/* keep Greek Accents in sync with menu */
	
	
}


/******************************************************************************
 * Name
 *   on_undockInt_activate
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   void on_undockInt_activate(GtkMenuItem *menuitem)
 *
 * Description
 *   undock/dock interlinear page
 *
 * Return value
 *   void
 */

static void on_undockInt_activate(GtkMenuItem *menuitem)
{
	if (settings.dockedInt) {
		settings.dockedInt = FALSE;
		gui_undock_interlinear_page();

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
 *   #include "gui/interlinear.h
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
	gchar modName[16];

	memset(modName, 0, 16);
	module_name_from_description(modName,
					     (gchar *) user_data);
	sprintf(settings.Interlinear1Module, "%s", modName);
	
	if (settings.dockedInt)
		gui_update_interlinear_page();
	else
		gui_update_interlinear_page_detached();
}



/******************************************************************************
 * Name
 *   on_changeint2mod_activate
 *
 * Synopsis
 *   #include "gui/interlinear.h
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
	gchar modName[16];

	memset(modName, 0, 16);
	module_name_from_description(modName,
					     (gchar *) user_data);
	sprintf(settings.Interlinear2Module, "%s", modName);
	
	if (settings.dockedInt)
		gui_update_interlinear_page();
	else
		gui_update_interlinear_page_detached();
}


/******************************************************************************
 * Name
 *   on_changeint3mod_activate
 *
 * Synopsis
 *   #include "gui/interlinear.h
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
	gchar modName[16];

	memset(modName, 0, 16);
	module_name_from_description(modName,
					     (gchar *) user_data);
	sprintf(settings.Interlinear3Module, "%s", modName);
	
	if (settings.dockedInt)
		gui_update_interlinear_page();
	else
		gui_update_interlinear_page_detached();
}


/******************************************************************************
 * Name
 *   on_changeint4mod_activate
 *
 * Synopsis
 *   #include "gui/interlinear.h
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
	gchar modName[16];

	memset(modName, 0, 16);
	module_name_from_description(modName,
					     (gchar *) user_data);
	sprintf(settings.Interlinear4Module, "%s", modName);
	
	if (settings.dockedInt)
		gui_update_interlinear_page();
	else
		gui_update_interlinear_page_detached();
}


/******************************************************************************
 * Name
 *   on_changeint5mod_activate
 *
 * Synopsis
 *   #include "gui/interlinear.h
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
	gchar modName[16];

	memset(modName, 0, 16);
	module_name_from_description(modName,
					     (gchar *) user_data);
	sprintf(settings.Interlinear5Module, "%s", modName);
	
	if (settings.dockedInt)
		gui_update_interlinear_page();
	else
		gui_update_interlinear_page_detached();
}



/******************************************************************************
 * Name
 *   on_int_global_options_activate
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   void on_int_global_options_activate(GtkMenuItem * menuitem,
 *				    gpointer user_data)
 *
 * Description
 *   toogle global options in interlinear window
 *
 * Return value
 *   void
 */

static void on_int_global_options_activate(GtkMenuItem * menuitem,
				    gpointer user_data)
{
	gui_set_interlinear_module_global_options((gchar *) user_data,
					      GTK_CHECK_MENU_ITEM
					      (menuitem)->active);
}


/******************************************************************************
 * Name
 *   gui_update_interlinear_page
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   void gui_update_interlinear_page(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_update_interlinear_page(void)
{
	gchar tmpBuf[256], *rowcolor, *font_size;
	gchar *utf8str,*mod_name, *font_name = NULL;
	gint utf8len, i, j;
	gboolean was_editable, use_gtkhtml_font;
	
	if (settings.havebible) {
		/* setup gtkhtml widget */
		GtkHTML *html = GTK_HTML(widgets.html_interlinear);
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
		    e_utf8_from_gtk_string(widgets.html_interlinear,
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
				utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, tmpBuf);
				utf8len = strlen(utf8str);
				if (utf8len) {
					gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
				}	
			}
			
			sprintf(tmpBuf,
				"<tr bgcolor=\"%s\"><td><B><A HREF=\"[%s]%s\"><FONT COLOR=\"%s\" SIZE=\"%s\"> [%s]</font></a></b>",
				rowcolor,
				mod_name,
				get_module_description(mod_name),
				settings.bible_verse_num_color,
				settings.verse_num_font_size,
				mod_name);
			utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, tmpBuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}			
				
			if(use_gtkhtml_font)
				sprintf(tmpBuf, "<font size=\"%s\">", font_size);
			else
				sprintf(tmpBuf, "<font face=\"%s\"size=\"%s\">", font_name, font_size);
				
			utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, tmpBuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
				
			utf8str = get_interlinear_module_text(mod_name, settings.currentverse);
			if (strlen(utf8str)) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, strlen(utf8str));
				free(utf8str);
			}
			
			sprintf(tmpBuf,
				"</font><small>[<A HREF=\"@%s\">view context</a>]</small></td></tr>",
				mod_name);
			utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, tmpBuf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
		}		
		
		sprintf(tmpBuf, "</table></body></html>");
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   tmpBuf);
		utf8len = strlen(utf8str);       
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}

		gtk_html_end(GTK_HTML(html), htmlstream, status1);
		gtk_html_set_editable(html, was_editable);
	}
	gtk_frame_set_label(GTK_FRAME(widgets.frame_interlinear), settings.currentverse);
	if(font_name)
		free(font_name);
}


/******************************************************************************
 * Name
 *   int_display
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   void int_display(gchar *key)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

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
	
	GtkHTML *html = GTK_HTML(widgets.html_interlinear);
	
	tmpkey = get_valid_key(key);
	
	bgColor = "#f1f1f1";
	cur_verse = get_verse_from_key(tmpkey);
	settings.intCurVerse = cur_verse;
	cur_chapter = get_chapter_from_key(tmpkey);
	cur_book = get_book_from_key(tmpkey);
	
	for (i = 1; ; i++) {	
		sprintf(tmpbuf,"%s %d:%d",cur_book,cur_chapter,i);
		free(tmpkey);
		tmpkey = get_valid_key(tmpbuf);
		if(cur_chapter != get_chapter_from_key(tmpkey))
			break;
		sprintf(buf,"%s","<tr valign=\"top\">");		
		utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
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
			
			utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
			utf8len = strlen(utf8str);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
			
			utf8str = get_interlinear_module_text(mod_name, tmpkey);
			if (strlen(utf8str)) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, strlen(utf8str));
				free(utf8str);
			}
			
			sprintf(buf, "%s", "</font></td>");	
			utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
			utf8len = strlen(utf8str);		
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}
		}
				
		sprintf(buf,"%s","</tr>");		
		utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}
	}	
	free(tmpkey);
}


/******************************************************************************
 * Name
 *   gui_update_interlinear_page_detached
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   void gui_update_interlinear_page_detached(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_update_interlinear_page_detached(void)
{
	gchar * utf8str, buf[500];
	gint utf8len;

	//-- setup gtkhtml widget
	GtkHTML *html = GTK_HTML(widgets.html_interlinear);
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");


	sprintf(buf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table align=\"left\" valign=\"top\"><tr valign=\"top\" >",
		settings.bible_bg_color, settings.bible_text_color, settings.link_color);
	utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
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
		    e_utf8_from_gtk_string(widgets.html_interlinear, buf);
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
		    e_utf8_from_gtk_string(widgets.html_interlinear, buf);
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
		    e_utf8_from_gtk_string(widgets.html_interlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (widgets.html_interlinear) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			settings.Interlinear4Module);
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear, buf);
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
		    e_utf8_from_gtk_string(widgets.html_interlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	sprintf(buf, "%s", "</tr>");
	utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}


	/******      ******/
	int_display(settings.cvInterlinear);

	sprintf(buf, "%s", "</table></body></html>");
	utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
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
 * Name
 *   gui_swap_interlinear_with_main
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   void gui_swap_interlinear_with_main(char * intmod)
 *
 * Description
 *   swaps interlinear mod with mod in main text window
 *
 * Return value
 *   void
 */

void gui_swap_interlinear_with_main(char * intmod)
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
	gui_change_module_and_key(intmod, settings.currentverse);
	gui_update_interlinear_page();
}


/******************************************************************************
 * Name
 *   gui_set_interlinear_module_global_options
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   void gui_set_interlinear_module_global_options(gchar *option, 
 *						gboolean choice)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_set_interlinear_module_global_options(gchar *option, 
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
	
	set_interlinear_global_option(option, on_off);

	/* display change */
	if (settings.dockedInt)	{
		gui_update_interlinear_page();
	}
	else {
		gui_update_interlinear_page_detached();
	}
}


/******************************************************************************
 * Name
 *   add_items_to_options_menu
 *
 * Synopsis
 *   #include "gui/interlinear.h
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
	GtkWidget 
		*menuChoice,
		*shellmenu;
	gchar menuName[64];
	int viewNumber = 0;
	GList *tmp;
	
	tmp = NULL;

	tmp = get_list(OPTIONS_LIST);
	while (tmp != NULL) {
		shellmenu =  module_options_menu;		
			
		/* add global option items to interlinear popup menu */
		menuChoice = gtk_check_menu_item_new_with_label((gchar *)(gchar *) tmp->data);	
		sprintf(menuName, "optionNum%d", viewNumber++);
		gtk_object_set_data(GTK_OBJECT(widgets.app), menuName, menuChoice);
		gtk_widget_show(menuChoice);
		gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
			   GTK_SIGNAL_FUNC(on_int_global_options_activate),
			  (gchar *)(gchar *) tmp->data);  
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
                                             GTK_WIDGET(menuChoice),
                                             1);  	      
				
		if(!strcmp((gchar *) tmp->data, "Strong's Numbers")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
				settings.strongsint;		
		}
		
		if(!strcmp((gchar *) tmp->data, "Footnotes")) {		
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
				settings.footnotesint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Morphological Tags")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
				settings.morphsint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Hebrew Vowel Points")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
				settings.hebrewpointsint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Hebrew Cantillation")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
				settings.cantillationmarksint;
		}
		
		if(!strcmp((gchar *) tmp->data, "Greek Accents")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
				settings.greekaccentsint;
		}	
		
		if(!strcmp((gchar *) tmp->data, "Scripture Cross-references")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
				settings.crossrefint;
		}	
		
		if(!strcmp((gchar *) tmp->data, "Lemmas")) {
			GTK_CHECK_MENU_ITEM(menuChoice)->active =
				settings.lemmasint;
		}		
		
		if(!strcmp((gchar *) tmp->data, "Headings")) {
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
 *   #include "gui/interlinear.h
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

static void load_menu_formmod_list(GtkWidget *pmInt, GList *mods,  
			gchar *label, GtkMenuCallback mycallback)
{
	GList *tmp;
	GtkWidget *item;
	GtkWidget *view_module;
	GtkWidget *view_module_menu;
	GtkAccelGroup *view_module_menu_accels;	
	
	view_module = gtk_menu_item_new_with_label (label);
  	gtk_widget_ref (view_module);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "view_module", view_module,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (view_module);
  	gtk_container_add (GTK_CONTAINER (pmInt), view_module);

  	view_module_menu = gtk_menu_new ();
  	gtk_widget_ref (view_module_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "view_module_menu", view_module_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (view_module), view_module_menu);
  	view_module_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (view_module_menu));
	tmp = mods;
	while (tmp != NULL) {
		item = gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item);
		gtk_object_set_data_full(GTK_OBJECT(pmInt), "item",
					 item,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item);		
		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC(mycallback),
				   g_strdup((gchar *)tmp->data ));

		gtk_container_add(GTK_CONTAINER(view_module_menu), item);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp); 
} 


/******************************************************************************
 * Name
 *   create_interlinear_popup
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   GtkWidget * create_interlinear_popup(GList * mods)
 *
 * Description
 *   create popup menu for interlinear window
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget * create_interlinear_popup(GList * mods)
{
	GtkWidget *pmInt;
	GtkAccelGroup *pmInt_accels;
	GtkWidget *copy7;
	GtkWidget *undockInt;
	GtkWidget *module_options;
	GtkWidget *separator2;
	GtkTooltips *tooltips;
	
	
	tooltips = gtk_tooltips_new();
	pmInt = gtk_menu_new ();
	gtk_object_set_data (GTK_OBJECT (pmInt), "pmInt", pmInt);
	pmInt_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (pmInt));

	copy7 = gtk_menu_item_new_with_label (_("Copy"));
	gtk_widget_ref (copy7);
	gtk_object_set_data_full (GTK_OBJECT (pmInt), "copy7", copy7,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (copy7);
	gtk_container_add (GTK_CONTAINER (pmInt), copy7);

	separator2 = gtk_menu_item_new ();
  	gtk_widget_ref (separator2);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator2);
  	gtk_container_add (GTK_CONTAINER (pmInt), separator2);
  	gtk_widget_set_sensitive (separator2, FALSE);
	
	undockInt = gtk_menu_item_new_with_label (_("Detach/Attach"));
	gtk_widget_ref (undockInt);
	gtk_object_set_data_full (GTK_OBJECT (pmInt), "undockInt",undockInt ,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (undockInt);
	gtk_container_add (GTK_CONTAINER (pmInt), undockInt);
	
	module_options = gtk_menu_item_new_with_label (_("Module Options"));
	gtk_widget_ref(module_options);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "module_options", module_options,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (module_options);
  	gtk_container_add (GTK_CONTAINER(pmInt), module_options);
	
	module_options_menu = gtk_menu_new ();
  	gtk_widget_ref(module_options_menu);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "module_options_menu",module_options_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (module_options), module_options_menu);
	
  	separator2 = gtk_menu_item_new ();
  	gtk_widget_ref (separator2);
  	gtk_object_set_data_full (GTK_OBJECT (pmInt), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (separator2);
  	gtk_container_add (GTK_CONTAINER (pmInt), separator2);
  	gtk_widget_set_sensitive (separator2, FALSE);
  	/* build change interlinear modules submenu */
	load_menu_formmod_list(pmInt, mods, _("Change Interlinear 1"), (GtkMenuCallback)on_changeint1mod_activate );  	
	load_menu_formmod_list(pmInt, mods, _("Change Interlinear 2"), (GtkMenuCallback)on_changeint2mod_activate );	
	load_menu_formmod_list(pmInt, mods, _("Change Interlinear 3"), (GtkMenuCallback)on_changeint3mod_activate );  	
	load_menu_formmod_list(pmInt, mods, _("Change Interlinear 4"), (GtkMenuCallback)on_changeint4mod_activate );
	load_menu_formmod_list(pmInt, mods, _("Change Interlinear 5"), (GtkMenuCallback)on_changeint5mod_activate );

  	gtk_signal_connect (GTK_OBJECT (copy7), "activate",
                      	GTK_SIGNAL_FUNC (gui_copyhtml_activate),
                      	NULL);
	gtk_signal_connect (GTK_OBJECT (undockInt), "activate",
                      	GTK_SIGNAL_FUNC (on_undockInt_activate), 
                      	&settings);
			
  return pmInt;
}


/******************************************************************************
 * Name
 *   gui_create_interlinear_popup
 *
 * Synopsis
 *   #include "gui/interlinear.h
 *
 *   void gui_create_interlinear_popup(GList *bible_description) 
 *
 * Description
 *   call create_interlinear_popup() and attach menu to int html widget
 *
 * Return value
 *   void
 */

void gui_create_interlinear_popup(GList *bible_description) 
{
	/* create popup menu for interlinear window */
	GtkWidget * menu_inter = create_interlinear_popup(bible_description);	
	/* attach popup menus */
	gnome_popup_menu_attach(menu_inter,
			widgets.html_interlinear, (gchar*)"1");
	add_items_to_options_menu();
}
