/*
 * Xiphos Bible Study Tool
 * bibletext.c - gui for Bible text modules
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>

#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif


#include "gui/xiphos.h"
#include "gui/bibletext.h"
#include "gui/bibletext_dialog.h"
#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/export_dialog.h"
#include "gui/shortcutbar_main.h" 
#include "gui/sidebar.h"
#include "gui/cipher_key_dialog.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/menu_popup.h"
#include "gui/shortcutbar_search.h"
#include "gui/dialog.h"
#include "gui/font_dialog.h"
#include "gui/dictlex.h"
#include "gui/tabbed_browser.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/global_ops.hh"
#include "main/display.hh"


gboolean shift_key_presed = FALSE;
guint scroll_adj_signal;
GtkAdjustment* adjustment;

#ifndef USE_GTKMOZEMBED
static GtkTextBuffer *text_buffer;
#endif


/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_popup_pm_text(void)
{
    gui_menu_popup (settings.MainWindowModule,
			NULL);	
}


#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb (GeckoHtml *html,
			     gchar *uri,
			     gpointer user_data)
{	
    gui_menu_popup (settings.MainWindowModule,
			NULL);	
	//gui_popup_pm_text();
}
#endif

#ifndef USE_GTKMOZEMBED
/******************************************************************************
 * Name
 *  on_text_button_press_event
 *
 * Synopsis
 *   #include ".h"
 *
 *  gboolean on_text_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */
static gboolean on_text_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	switch (event->button) {
	case 1:
		break;
	case 2:
		shift_key_presed = TRUE;
		break;
	case 3:
		gui_popup_pm_text();
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  gboolean on_button_release_event(GtkWidget * widget,
			    GdkEventButton * event, DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

static gboolean on_text_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer date)
{

#ifdef GTKHTML
	extern gboolean in_url;
	gchar *key;
	const gchar *url;

	settings.whichwindow = MAIN_TEXT_WINDOW;
	/*
	 * set program title to current text module name
	 */
	gui_change_window_title(settings.MainWindowModule);
	switch (event->button) {
	case 1:
		if (!in_url) {
			key = gui_button_press_lookup(widgets.html_text);
			if (key) {
				gchar *dict = NULL;
				if (settings.useDefaultDict)
					dict =
					    g_strdup(settings.
						     DefaultDict);
				else
					dict =
					    g_strdup(settings.DictWindowModule);
				main_display_dictionary(dict, key);
				g_free(key);
				if (dict)
					g_free(dict);
			}
		}
		break;
	case 2:
		if (shift_key_presed) {
			shift_key_presed = FALSE;
			break;
		}
		if (!in_url)
			break;
#ifdef USE_GTKHTML38
		url = gtk_html_get_url_at (GTK_HTML(widgets.html_text),		
								event->x,
								event->y);
#else
		url = html_engine_get_link_at (GTK_HTML(widgets.html_text)->engine,
					 event->x,
					 event->y);
#endif
		if (strstr(url,"sword://")) {
			gchar **work_buf = g_strsplit (url,"/",4);
			gui_open_passage_in_new_tab(work_buf[3]);
			g_strfreev(work_buf);
		}
		break;
	case 3:
		break;
	}
#endif /* GTKHTML */
	return FALSE;
}

static gboolean textview_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
//	extern gboolean in_url;
//	gchar *key;

	settings.whichwindow = MAIN_TEXT_WINDOW;
	/*
	 * set program title to current text module name
	 */
	gui_change_window_title(settings.MainWindowModule);
/*
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}
*/
	return FALSE;
}


static gboolean textview_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		gui_popup_pm_text();
		return TRUE;
		break;
	}
	return FALSE;
}


static gboolean
on_enter_notify_event        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
	//shift_key_presed = FALSE;
	//gtk_widget_grab_focus (widgets.html_text);
	settings.whichwindow = MAIN_TEXT_WINDOW;
	gui_change_window_title(settings.MainWindowModule);
  	return FALSE;
}
/*
static gboolean on_key_press_event           (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
	if (event->state == GDK_CONTROL_MASK)
		g_message("GDK_CONTROL_MASK");
	if (event->state == GDK_SHIFT_MASK)
		g_message("GDK_SHIFT_MASK");
	if (event->state == GDK_MOD1_MASK)
		g_message("GDK_MOD1_MASK");
	if (event->state == GDK_MODIFIER_MASK)
		g_message("GDK_MODIFIER_MASK");
	g_message("state: %d",event->state);
#endif
	switch(event->hardware_keycode) {
		case 50:
		case 62:
			shift_key_presed = TRUE;
  			return TRUE;
		break;
	}
	GS_message(("on_key_press_event\nkeycode: %d",event->hardware_keycode));
  	return FALSE;
}


static gboolean on_key_release_event         (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
	if (event->state == GDK_CONTROL_MASK)
		g_message("GDK_CONTROL_MASK");
	if (event->state == GDK_SHIFT_MASK)
		g_message("GDK_SHIFT_MASK");
	if (event->state == GDK_MOD1_MASK)
		g_message("GDK_MOD1_MASK");
#endif
	switch(event->hardware_keycode) {
		case 50:
		case 62:
			shift_key_presed = FALSE;
		break;
	}
	GS_message(("on_key_release_event\nkeycode: %d",event->hardware_keycode));
  	return FALSE;
}
*/

static gint tag_event_handler (GtkTextTag *tag, GtkWidget *widget,
	GdkEvent *event, const GtkTextIter *iter, gpointer user_data)
{
	gint char_index;

	char_index = gtk_text_iter_get_offset (iter);
	//printf ("offset = %d", char_index);
  switch (event->type)
    {
    case GDK_MOTION_NOTIFY:
      printf ("Motion event at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;

    case GDK_BUTTON_PRESS:
      printf ("Button press at char %d tag `%s'\n",
             char_index, tag->name);
	    switch(event->button.button){
		    case 1:
			//return do_something_with_tag(tag, iter, user_data);
		        break;
		    case 2:
		    case 3:
		        break;

	}

    	return TRUE;
      break;

    case GDK_2BUTTON_PRESS:
      printf ("Double click at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;

    case GDK_3BUTTON_PRESS:
      printf ("Triple click at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;

    case GDK_BUTTON_RELEASE:
      printf ("Button release at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;

    case GDK_KEY_PRESS:
    case GDK_KEY_RELEASE:
      printf ("Key event at char %d tag `%s'\n",
              char_index, tag->name);
    	return TRUE;
      break;

    case GDK_ENTER_NOTIFY:
      printf ("enter event at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;

    case GDK_LEAVE_NOTIFY:
      printf ("leave event at char %d tag `%s'\n",
             char_index, tag->name);
    	return TRUE;
      break;

    case GDK_PROPERTY_NOTIFY:
    case GDK_SELECTION_CLEAR:
    case GDK_SELECTION_REQUEST:
    case GDK_SELECTION_NOTIFY:
    case GDK_PROXIMITY_IN:
    case GDK_PROXIMITY_OUT:
#if 0
    case GDK_DRAG_ENTER:
    case GDK_DRAG_LEAVE:
    case GDK_DRAG_MOTION:
    case GDK_DRAG_STATUS:
    case GDK_DROP_START:
    case GDK_DROP_FINISHED:
#endif /* 0 */
    	return FALSE;
    default:
    	return FALSE;
      break;
    }
    return FALSE;
}

static void setup_tag (GtkTextTag *tag, gpointer user_data)
{
	g_signal_connect (G_OBJECT (tag),
		    "event",
		    G_CALLBACK (tag_event_handler),
		    user_data);
}

static void create_text_tags(GtkTextBuffer * buffer)
{
	GtkTextTag *tag;
	GdkColor color;
	//GdkColor color2;
	//GdkColor color_red;
	//GdkColor colorLink;
	//PangoFontDescription *font_desc;



	/* verse number tag verse style*/
	tag = gtk_text_buffer_create_tag (buffer, "verseNumber", NULL);
	setup_tag (tag, buffer);
	color.red = color.green = 0;
	color.blue = 0xffff;
	//	"scale", PANGO_SCALE_XX_SMALL,
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);

	/* verse number tag verse style*/
	/*tag = gtk_text_buffer_create_tag (buffer, "verse", NULL);
	setup_tag (tag, buffer);
	g_object_set (G_OBJECT (tag),
                NULL);	*/

	/* current verse color tag */
	tag = gtk_text_buffer_create_tag (buffer, "fg_currentverse", NULL);
	color.blue = 0;
	color.green = 0xbbbb;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);

	/*  verse color tag */
	tag = gtk_text_buffer_create_tag (buffer, "fg_verse", NULL);
	color.blue = 0;
	color.green = 0;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);

	/* right to left tag */
	tag = gtk_text_buffer_create_tag (buffer, "rtl_text", NULL);
        g_object_set (G_OBJECT (tag),
		//"font", rtl_font,
                "wrap_mode", GTK_WRAP_WORD,
                "direction", GTK_TEXT_DIR_RTL,
                "indent", 0,
                "left_margin", 0,
                "right_margin", 0,
                NULL);

	/* large tag */
	tag = gtk_text_buffer_create_tag (buffer, "large", NULL);
        g_object_set (G_OBJECT (tag),
		"scale", (double)1.928, //PANGO_SCALE_XX_LARGE,
                NULL);
}


static
void adj_changed(GtkAdjustment * adjustment1, gpointer user_data) 

{
	extern NAVBAR_VERSEKEY navbar_versekey;
	static int scroll = 1;
	if (!settings.chapter_scroll) return;
	if (scroll && (adjustment1->value <= adjustment1->lower)) {
		GS_message(("\ntop: %g\n",adjustment1->value));
#ifdef OLD_NAVBAR
		gui_navbar_handle_spinbutton_click(1, 0);
#else
		main_navbar_versekey_spin_chapter(navbar_versekey,0);
#endif
		scroll = 0;
	} else if (scroll && (adjustment1->value >= (adjustment1->upper - adjustment1->page_size))) {
		GS_message(("\nvalue + page_size: %g\n",adjustment1->value + adjustment1->page_size));
#ifdef OLD_NAVBAR
		gui_navbar_handle_spinbutton_click(1, 1);
#else
		main_navbar_versekey_spin_chapter(navbar_versekey,1);
#endif
		scroll = 0;
		gtk_adjustment_set_value(adjustment,2);
	} else 	scroll = 1;
}

#endif /* !USE_GTKMOZEMBED */

/******************************************************************************
 * Name
 *   gui_create_bible_pane
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   GtkWidget *gui_create_bible_pane(void)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_bible_pane(void)
{
	GtkWidget *notebook_text;
#ifndef USE_GTKMOZEMBED
	GtkWidget *scrolledwindow;
#endif
	
	notebook_text = gtk_notebook_new();
	gtk_widget_show(notebook_text);
#ifdef USE_GTKMOZEMBED
	widgets.html_text = GTK_WIDGET(gecko_html_new(NULL, FALSE, TEXT_TYPE)); 
	gtk_widget_show(widgets.html_text);
	gtk_container_add(GTK_CONTAINER(notebook_text), widgets.html_text);
	g_signal_connect((gpointer)widgets.html_text,
		      "popupmenu_requested",
		      G_CALLBACK (_popupmenu_requested_cb),
		      NULL);
#else	
	
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_container_add(GTK_CONTAINER(notebook_text),
			  scrolledwindow);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	adjustment = gtk_scrolled_window_get_vadjustment
                                            (GTK_SCROLLED_WINDOW(scrolledwindow));
	scroll_adj_signal = g_signal_connect(GTK_OBJECT(adjustment), "value-changed",
				G_CALLBACK(adj_changed),
				NULL);
	widgets.html_text = gtk_html_new();
	gtk_widget_show(widgets.html_text);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_text);
			  
	g_signal_connect(GTK_OBJECT(widgets.html_text), "link_clicked",
				G_CALLBACK(gui_link_clicked),
				NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_text), "on_url",
				G_CALLBACK(gui_url),
				GINT_TO_POINTER(TEXT_TYPE));		    
	g_signal_connect(GTK_OBJECT(widgets.html_text),"button_release_event",
				G_CALLBACK(on_text_button_release_event),
				NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_text), "button_press_event",
				G_CALLBACK(on_text_button_press_event),
				NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_text), "enter_notify_event",
		    		G_CALLBACK (on_enter_notify_event),
		       		NULL);
/*	g_signal_connect(GTK_OBJECT(widgets.html_text), "key_press_event",
		    		G_CALLBACK (on_key_press_event),
		    		NULL);  
	g_signal_connect(GTK_OBJECT(widgets.html_text), "key_release_event",
		    		G_CALLBACK (on_key_release_event),
		    		NULL);	*/			   
	g_signal_connect(GTK_OBJECT(widgets.html_text),
			 "url_requested",
			 G_CALLBACK(url_requested), NULL);
	
	/* gtktextview stuff */
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_container_add(GTK_CONTAINER(notebook_text),
			  scrolledwindow);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow,
                                             settings.shadow_type);
					     
	widgets.textview = gtk_text_view_new ();
	gtk_widget_show(widgets.textview);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.textview);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (widgets.textview), FALSE);
	text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (widgets.textview));

	create_text_tags(text_buffer);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW (widgets.textview),
						GTK_WRAP_WORD);

		    
	g_signal_connect(GTK_OBJECT(widgets.textview),
				   "button_release_event",
				   G_CALLBACK
				   (textview_button_release_event),
				  NULL);
	g_signal_connect(GTK_OBJECT(widgets.textview),
				   "button_press_event",
				   G_CALLBACK
				   (textview_button_press_event),
					 NULL);
#endif			 

	return 	notebook_text;			   
	
}
