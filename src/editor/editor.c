/*
 * Xiphos Bible Study Tool
 * editor.c - webkit stuff
 *
 * Copyright (C) 2005-2017 Xiphos Developer Team
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

#ifdef USE_WEBKIT_EDITOR

/* X keyboard #definitions, to handle shortcuts */
/* we must define the categories of #definitions we need. */
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

#include <gtk/gtk.h>
#include <webkit/webkit.h>

#include "editor/webkit_editor.h"
#include "editor/editor.h"

#include "main/sword.h"
#include "main/settings.h"

#include "gui/utilities.h"

#define html_start "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"><html><head>"

BUTTONS_STATE buttons_state;

glong mouse_x;
glong mouse_y;

//WebKitDOMElement * current_element;

/******************************************************************************
 * Name
 *   editor_insert_new_outline_level
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   gint editor_insert_new_outline_level (gint level, EDITOR * e)
 *
 * Description
 *  use to create an outline:
 * I. 
 *  	A.
 *  		1.
 *  			a.  
 *
 * Return value
 *   gint
 */

gint editor_insert_new_outline_level(gint level, EDITOR *e)
{
	WebKitDOMDocument *doc;
	WebKitDOMElement *element_anchor = NULL;
	WebKitDOMElement *element = NULL;
	WebKitDOMElement *parent_ol_element = NULL;
	WebKitDOMDOMWindow *window;
	WebKitDOMDOMSelection *selection = NULL;
	GError *error = NULL;
	gchar *name = NULL;
	gchar *class = NULL;
	gchar *level_str = NULL;

	XI_message(("\n%s\n", "editor_insert_new_outline_level"));

	doc = webkit_web_view_get_dom_document((WebKitWebView *)
					       e->html_widget);

	if (!doc)
		return 0;

	window = webkit_dom_document_get_default_view(doc);
	selection = webkit_dom_dom_window_get_selection(window);
	if (selection == NULL) {
		XI_message(("\n%s\n", "failed to get selection"));
		return 0;
	}

	element_anchor = (WebKitDOMElement *)
	    webkit_dom_dom_selection_get_anchor_node(selection);
	if (element_anchor == NULL) {
		XI_message(("\n%s\n", "failed to get anchor"));
		return 0;
	}

	name = webkit_dom_element_get_tag_name(element_anchor);
	if (!g_strcmp0("LI", name) || !g_strcmp0("li", name)) {
		XI_message(("current element: %s", name));
		element =
		    webkit_dom_node_get_parent_element((WebKitDOMNode *)
						       element_anchor);
	} else
		return 1;

	parent_ol_element =
	    webkit_dom_node_get_parent_element((WebKitDOMNode *)element);
	name = webkit_dom_element_get_tag_name(parent_ol_element);
	class =
	    webkit_dom_element_get_attribute(parent_ol_element, "class");
	XI_message(("\nparent_ol_element: %s\nclass: %s\n", name, class));

	if (class) {
		if (!g_strcmp0("L1", class))
			level_str = g_strdup("L2");
		if (!g_strcmp0("L2", class))
			level_str = g_strdup("L3");
		if (!g_strcmp0("L3", class))
			level_str = g_strdup("L4");
		if (!g_strcmp0("L4", class))
			level_str = g_strdup("L4");
		if (!level_str) {
			return 0;
		}
	}

	webkit_dom_element_set_attribute(element, "class", level_str,
					 &error);
	if (error) {
		fprintf(stderr, "Failed to create new attribute: %s\n",
			error->message);
		g_error_free(error);
		error = NULL;
		return 0;
	}

	return 1;
}

/******************************************************************************
 * Name
 *   editor_get_document_content
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void editor_get_document_content (GString * data, EDITOR * e)
 *
 * Description
 *   fills the GString with the contents of the html document.
 *
 * Return value
 *   void
 */

void editor_get_document_content(GString *data, EDITOR *e)
{
	WebKitDOMHTMLElement *html;
	WebKitDOMHTMLHeadElement *header;
	WebKitDOMDocument *dom_document = NULL;
	gchar *body = NULL;
	gchar *head = NULL;

	dom_document = webkit_web_view_get_dom_document((WebKitWebView *)
							e->html_widget);
	if (!dom_document)
		return;

	/* get document <head> info */
	header = webkit_dom_document_get_head(dom_document);
	head =
	    webkit_dom_html_element_get_inner_html((WebKitDOMHTMLElement *)
						   header);

	/* get document <body> info */
	html = webkit_dom_document_get_body(dom_document);
	body = webkit_dom_html_element_get_inner_html(html);

	g_string_printf(data, "%s%s</head><body>%s</body>\n</html>",
			html_start, head, body);
}

/******************************************************************************
 * Name
 *   editor_get_selected_text
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   gchar * editor_get_selected_text (EDITOR * e)
 *
 * Description
 *   returns the selected text and must be freed by calling function
 *
 * Return value
 *   gchar *
 */

gchar *editor_get_selected_text(EDITOR *e)
{
	WebKitDOMDocument *dom_document;
	WebKitDOMDOMWindow *window = NULL;
	WebKitDOMDOMSelection *selection = NULL;
	gchar *text = NULL;
	GError *error = NULL;

	dom_document = webkit_web_view_get_dom_document((WebKitWebView *)
							e->html_widget);
	if (!dom_document)
		return NULL;
	window = webkit_dom_document_get_default_view(dom_document);
	selection = webkit_dom_dom_window_get_selection(window);
	if (selection) {
		WebKitDOMRange *range =
		    webkit_dom_dom_selection_get_range_at(selection, 0,
							  &error);
		if (error) {
			fprintf(stderr, "Failed to get range: %s\n",
				error->message);
			g_error_free(error);
			error = NULL;
			//return;
		}
		text = webkit_dom_range_to_string(range, &error);
		if (error) {
			fprintf(stderr, "Failed range text: %s\n",
				error->message);
			g_error_free(error);
			error = NULL;
			return NULL;
		}
	}
	return g_strdup(text);
}

/******************************************************************************
 * Name
 *   editor_find_string
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void editor_find_string (gchar * needle, EDITOR * e)
 *
 * Description
 *   search document for needle
 *
 * Return value
 *   void
 */

void editor_find_string(gchar *needle, EDITOR *e)
{
	webkit_web_view_search_text((WebKitWebView *)e->html_widget,
				    needle, FALSE, TRUE, TRUE);
}

/******************************************************************************
 * Name
 *   editor_replace_string
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *  void editor_replace_string (gchar * old_string, gchar * new_string, EDITOR * e) 
 *
 * Description
 *   search document for old_string and replace with new_string
 *   **** needs work ****
 *
 * Return value
 *   void
 */

void editor_replace_string(gchar *old_string, gchar *new_string,
			   EDITOR *e)
{
	WebKitWebFrame *frame = NULL;

	frame = webkit_web_view_get_main_frame((WebKitWebView *)
					       e->html_widget);
	webkit_web_view_search_text((WebKitWebView *)e->html_widget,
				    old_string, FALSE, TRUE, FALSE);
	webkit_web_frame_replace_selection(frame, new_string);
}

/******************************************************************************
 * Name
 *   editor_execute_script
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void editor_execute_script(gchar * script, EDITOR * e)
 *
 * Description
 *   executes scripts ie "document.execCommand('insertOrderedList', null, \"\");"
 *	 
 *
 * Return value
 *   void
 */

void editor_execute_script(gchar *script, EDITOR *e)
{
	if (script) {
		webkit_web_view_execute_script(WEBKIT_WEB_VIEW(e->html_widget), script);
		XI_message(("script: %s", script));
	}
}

/******************************************************************************
 * Name
 *   editor_insert_html
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void editor_insert_html(const gchar * html, EDITOR * e)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void editor_insert_html(const gchar *html, EDITOR *e)
{
	gchar *str = g_strdup_printf("document.execCommand('insertHTML', null, \'%s\');", html);
	XI_message(("script: %s", str));
	editor_execute_script(str, e);
	g_free(str);
}

/*
void editor_open_recent (const gchar * uri, EDITOR * e)
{
	webkit_web_view_load_uri (WEBKIT_WEB_VIEW(e->html_widget),uri); 

}
*/

gboolean editor_cut(EDITOR *e)
{
	if (webkit_web_view_can_cut_clipboard(WEBKIT_WEB_VIEW(e->html_widget))) {
		webkit_web_view_cut_clipboard(WEBKIT_WEB_VIEW(e->html_widget));
		return 1;
	} else
		return 0;
}

gboolean editor_copy(EDITOR *e)
{
	if (webkit_web_view_can_copy_clipboard(WEBKIT_WEB_VIEW(e->html_widget))) {
		webkit_web_view_copy_clipboard(WEBKIT_WEB_VIEW(e->html_widget));
		return 1;
	} else
		return 0;
}

gboolean editor_paste(EDITOR *e)
{
	if (webkit_web_view_can_paste_clipboard(WEBKIT_WEB_VIEW(e->html_widget))) {
		webkit_web_view_paste_clipboard(WEBKIT_WEB_VIEW(e->html_widget));
		return 1;
	} else
		return 0;
}

/*
GtkWidget * entry_module;
GtkWidget * entry_verse;
GtkWidget * entry_text;
GtkWidget * linkage_verse_list;
GtkWidget * hbox_sword_link;
GtkWidget * hbox_rbuttons; 
GtkWidget * uri_entry; 
gboolean sword_link;

void editor_insert_link_ok (void)
{
	const gchar *mod_str = NULL;
	const gchar *verse_str = NULL;
	const gchar *text_str = NULL;
	const gchar *uri_str = NULL;
	//const gchar *encoded_mod = NULL;
	//const gchar *encoded_verse = NULL;
	gchar *str = NULL;
	//gint type = -1;
	if(sword_link) {
		//XI_message(("%s","sword_link"));
		mod_str = gtk_entry_get_text (GTK_ENTRY (entry_module));
		verse_str = gtk_entry_get_text (GTK_ENTRY (entry_verse));
		text_str = gtk_entry_get_text (GTK_ENTRY (entry_text));
		//type = main_get_mod_type((gchar*)mod_str);
		str = g_strdup_printf(" <a href=\"sword://%s/%s\">%s</a>",
				mod_str ,verse_str,text_str);
	} else {
				
		uri_str = gtk_entry_get_text (GTK_ENTRY (uri_entry));		
		str = g_strdup_printf(" <a href=\"%s\">%s</a>",
				uri_str, uri_str);
	}
	 
	 editor_insert_html(str, e);
	if(str)
		g_free(str);
}

gboolean editor_insert_sword_link(void)
{
    GtkBuilder * builder;
	GtkWidget * window;        
	GtkWidget * hbox_sword_link;
	GtkWidget * hbox_rbuttons; 	
	GError *error = NULL; 
	sword_link = TRUE;
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, LINK_DIALOG_UI_FILE, &error);
	if (error) {
	      	fprintf(stderr, "Failed to load ui file: %s\n", error->message);
	      	g_error_free(error);
	      	error = NULL;
	}	
    
    window = GTK_WIDGET (gtk_builder_get_object (builder, "dialog1"));
    gtk_builder_connect_signals (builder,window);      
	
	hbox_sword_link  = GTK_WIDGET (gtk_builder_get_object (builder, "hbox_sword_link"));
	hbox_rbuttons  = GTK_WIDGET (gtk_builder_get_object (builder, "hbox6"));
	gtk_widget_show (hbox_sword_link);  
	gtk_widget_show (hbox_rbuttons);

	entry_module       = GTK_WIDGET (gtk_builder_get_object (builder, "entry2"));
	entry_verse        = GTK_WIDGET (gtk_builder_get_object (builder, "entry1"));
	entry_text         = GTK_WIDGET (gtk_builder_get_object (builder, "entry3"));
	linkage_verse_list = GTK_WIDGET (gtk_builder_get_object (builder, "radiobutton2"));

    g_object_unref (G_OBJECT (builder));
    gtk_widget_show (window);
	return 0;
}
   
gboolean editor_insert_link(void)
{
     	GtkBuilder * builder;
	//GtkWidget * entry_module;
	//GtkWidget * entry_verse;
	//GtkWidget * entry_text;
	GtkWidget * window;
	 GtkWidget * hbox_url_link;
	sword_link = FALSE;
	
        builder = gtk_builder_new ();
        gtk_builder_add_from_file (builder, LINK_DIALOG_UI_FILE, NULL);
        
        window = GTK_WIDGET (gtk_builder_get_object (builder, "dialog1"));
        gtk_builder_connect_signals (builder,window); 
	 hbox_url_link = GTK_WIDGET (gtk_builder_get_object (builder, "hbox_url_link"));
	gtk_widget_show (hbox_url_link);  

	uri_entry = GTK_WIDGET (gtk_builder_get_object (builder, "entry4"));
	//entry_verse        = GTK_WIDGET (gtk_builder_get_object (builder, "entry1"));
	//entry_text         = GTK_WIDGET (gtk_builder_get_object (builder, "entry3"));
	//linkage_verse_list = GTK_WIDGET (gtk_builder_get_object (builder, "radiobutton2"));

        g_object_unref (G_OBJECT (builder));
        gtk_widget_show (window);
	return 0;
}

*/

/******************************************************************************
 * Name
 *   user_changed_contents_cb
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void user_changed_contents_cb (WebKitWebView * web_view, EDITOR * e)
 *
 * Description
 *   callback - when user makes a change to a document
 *
 * Return value
 *   void
 */
static void user_changed_contents_cb(WebKitWebView *web_view, EDITOR *e)
{
	XI_message(("%s", "user_changed_contents_cb"));
	e->is_changed = TRUE;
}

/******************************************************************************
 * Name
 *   on_navigation_requested
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   WebKitNavigationResponse on_navigation_requested (WebKitWebView * web_view,
 *                                                     WebKitWebFrame * frame,
 *                                                     WebKitNetworkRequest * request,
 *                                             		   EDITOR * e)
 *
 * Description
 *   allows only a uri that contains 'file:'
 *
 * Return value
 *   WebKitNavigationResponse
 */
static WebKitNavigationResponse on_navigation_requested(WebKitWebView *web_view,
							WebKitWebFrame *frame,
							WebKitNetworkRequest *
							    request,
							EDITOR *e)
{
	const gchar *uri = NULL;

	if (e->is_changed) {
		ask_about_saving(e);
	}

	uri = webkit_network_request_get_uri(request);
	XI_message(("on_navigation_requested uri: %s", uri));
	if (g_strstr_len(uri, 6, "file:"))
		return FALSE;
	else
		return TRUE;
}

/******************************************************************************
 * Name
 *   link_handler
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void link_handler (GtkWidget *widget, 
 *			      gchar     *title,
 *			      gchar     *uri,
 *			      EDITOR * e)
 *
 * Description
 *   hovering over a link - does nothing at present
 *
 * Return value
 *   void
 */

static void link_handler(GtkWidget *widget,
			 gchar *title, gchar *uri, EDITOR *e)
{
	XI_message(("link_handler"));
}

/******************************************************************************
 * Name
 *   _has_element
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   gint _has_element(gchar * name, gchar * class, EDITOR * e)
 *
 * Description
 *   checks for html element - for button status ie bold on or off
 *
 * Return value
 *   gint
 */

static gint _has_element(gchar *name, gchar *class, EDITOR *e)
{
	if (!name)
		return 0;
	if (!g_strcmp0("BODY", name)) {
		return 0;
	}
	if (!g_strcmp0("B", name)) {
		buttons_state.bold = 1;
		return 1;
	} else if (!g_strcmp0("I", name)) {
		buttons_state.italic = 1;
		return 1;
	} else if (!g_strcmp0("U", name)) {
		buttons_state.underline = 1;
		return 1;
	} else if (!g_strcmp0("STRIKE", name)) {
		buttons_state.strike = 1;
		return 1;
	} else if (!g_strcmp0("DIV", name)) {
		buttons_state.style = 0;
		return 1;
	} else if (!g_strcmp0("H1", name)) {
		buttons_state.style = 1;
		return 1;
	} else if (!g_strcmp0("H2", name)) {
		buttons_state.style = 2;
		return 1;
	} else if (!g_strcmp0("H3", name)) {
		buttons_state.style = 3;
		return 1;
	} else if (!g_strcmp0("H4", name)) {
		buttons_state.style = 4;
		return 1;
	} else if (!g_strcmp0("H5", name)) {
		buttons_state.style = 5;
		return 1;
	} else if (!g_strcmp0("H6", name)) {
		buttons_state.style = 6;
		return 1;
	} else if (!g_strcmp0("ADDRESS", name)) {
		buttons_state.style = 7;
		return 1;
	} else if (!g_strcmp0("PRE", name)) {
		buttons_state.style = 8;
		return 1;
	} else if (!g_strcmp0("UL", name)) {
		buttons_state.style = 9;
		return 1;
	} else if (!g_strcmp0("FONT", name)) {
		//buttons_state.style = 9;
		return 1;
	}
	if (!g_strcmp0("LI", name)) {
		return 1;
	} else if (!g_strcmp0("OL", name)) {
		buttons_state.style = 10;
		if (class) {
			XI_message(("\nclass: %s\n", class));
			if (!g_strcmp0("L1", class)) {
				e->toolitems.outline_level = 1;
				buttons_state.style = 10;
			}
			if (!g_strcmp0("L2", class)) {
				e->toolitems.outline_level = 2;
				buttons_state.style = 12;
			}
			if (!g_strcmp0("L3", class)) {
				e->toolitems.outline_level = 3;
				buttons_state.style = 11;
			}
			if (!g_strcmp0("L4", class)) {
				e->toolitems.outline_level = 4;
				buttons_state.style = 12;
			}
		}
		return 0;
	} else
		return 0;
	return 1;
}

/******************************************************************************
 * Name
 *   key_handler
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   gboolean key_handler (GtkWidget *widget,
 *                         GdkEvent  *event,
 *                         EDITOR * e)
 *
 * Description
 *   key release handler - does nothing at present
 *
 * Return value
 *   gboolean
 */

static gboolean key_handler(GtkWidget *widget, GdkEventKey *event, EDITOR *e)
{
	/* these are the mods we actually use for global keys, we always only check for these set */
	guint state =
	    event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK |
			    GDK_MOD1_MASK | GDK_MOD4_MASK);

	switch (event->keyval) {
	case XK_s: // Ctrl-L  verse entry
		if (state == GDK_CONTROL_MASK)
			action_save_activate_cb(e->html_widget, e);
		break;
	}

	return 0;
}

/******************************************************************************
 * Name
 *   menu_spell_item_activated
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void  menu_spell_item_activated (GtkWidget *menuitem, EDITOR * e)
 *
 * Description
 *   user clicked on spelling word guess - get word form menu item label
 *   and replace misspelled word
 *
 * Return value
 *   void
 */

static void menu_spell_item_activated(GtkWidget *menuitem, EDITOR *e)
{
	WebKitWebFrame *frame;
	const gchar *label;

	label = gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));
	frame = webkit_web_view_get_main_frame((WebKitWebView *)
					       e->html_widget);

	webkit_web_frame_replace_selection(frame, label);
}

/******************************************************************************
 * Name
 *   menu_spell_add_item_activated
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void  menu_spell_add_item_activated (GtkWidget *menuitem, gpointer user_data)
 *
 * Description
 *   add word to dictionary
 *
 * Return value
 *   void
 */

static void menu_spell_add_item_activated(GtkWidget *menuitem,
					  gpointer user_data)
{
	WebKitSpellChecker *checker = NULL;
	checker = (WebKitSpellChecker *)webkit_get_text_checker();

	webkit_spell_checker_learn_word(checker, (gchar *)user_data);
	if ((gchar *)user_data)
		g_free((gchar *)user_data);
}

/******************************************************************************
 * Name
 *  menu_spell_ignore_item_activated 
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void  menu_spell_ignore_item_activated (GtkWidget *menuitem, gpointer user_data)
 *
 * Description
 *   ignore word for this document
 *
 * Return value
 *   void
 */

static void menu_spell_ignore_item_activated(GtkWidget *menuitem,
					     gpointer user_data)
{
	WebKitSpellChecker *checker = NULL;
	checker = (WebKitSpellChecker *)webkit_get_text_checker();

	webkit_spell_checker_ignore_word(checker, (gchar *)user_data);
	if ((gchar *)user_data)
		g_free((gchar *)user_data);
}

/******************************************************************************
 * Name
 *   _fill_spell_menu
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   gint _fill_spell_menu(GtkWidget * menu, gchar * word, EDITOR * e)
 *
 * Description
 *   add spelling guesses to context menu - word must be selected in
 *   for this to work - it's webkit thing
 *
 * Return value
 *   gint
 */

static gint _fill_spell_menu(GtkWidget *menu, gchar *word, EDITOR *e)
{
	WebKitSpellChecker *checker = NULL;
	int misspelling_location;
	int misspelling_length;
	GtkWidget *item;
	char **word_list = NULL;

	XI_message(("\nword: %s\n", word));
	checker = (WebKitSpellChecker *)webkit_get_text_checker();

	webkit_spell_checker_check_spelling_of_string(checker,
						      word,
						      &misspelling_location,
						      &misspelling_length);
	if (!misspelling_length)
		return 0;

	word_list = webkit_spell_checker_get_guesses_for_word(checker,
							      word, NULL);
	/* add guesses to menu */
	if (word_list) {
		int i = 0;
		while (word_list[i]) {
			item = gtk_menu_item_new_with_label(word_list[i]);
			gtk_widget_show(item);
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK(menu_spell_item_activated), e);
			gtk_menu_shell_append((GtkMenuShell *)menu,
					      (GtkWidget *)item);
			i++;
		}
	}

	/* separator */
	item = gtk_separator_menu_item_new();
	gtk_widget_show(item);
	gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

	/* add word */
	item = gtk_menu_item_new_with_label("Add word");
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item), "activate",
			 G_CALLBACK(menu_spell_add_item_activated),
			 g_strdup((gchar *)word));
	gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

	/* ignore word */
	item = gtk_menu_item_new_with_label("Ignore word");
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item), "activate",
			 G_CALLBACK(menu_spell_ignore_item_activated),
			 g_strdup((gchar *)word));
	gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

	/* separator */
	item = gtk_separator_menu_item_new();
	gtk_widget_show(item);
	gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

	if (word_list)
		g_strfreev(word_list);

	return misspelling_length;
}

/******************************************************************************
 * Name
 *   _create_context_menu
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void _create_context_menu (WebKitDOMDocument * dom_document, guint32 time, EDITOR * e)
 *
 * Description
 *   called when right button is clicked - 
 *
 * Return value
 *   void
 */

static void _create_context_menu(WebKitDOMDocument *dom_document, guint32 time,
				 EDITOR *e)
{
	WebKitDOMDOMWindow *window = NULL;
	WebKitDOMDOMSelection *selection = NULL;
	GError *error = NULL;
	GtkWidget *menu = NULL;
	GtkWidget *item = NULL;
	gboolean have_selection = FALSE;

	menu = gtk_menu_new();
	window = webkit_dom_document_get_default_view(dom_document);
	selection = webkit_dom_dom_window_get_selection(window);
	if (selection) {
		WebKitDOMRange *range =
		    webkit_dom_dom_selection_get_range_at(selection, 0,
							  &error);
		if (error) {
			fprintf(stderr, "Failed to get range: %s\n",
				error->message);
			g_error_free(error);
			error = NULL;
		}
		gchar *text = webkit_dom_range_to_string(range, &error);
		if (error) {
			fprintf(stderr, "Failed range text: %s\n",
				error->message);
			g_error_free(error);
			error = NULL;
			return;
		}
		if (text && (g_strcmp0(text, "") != 0)) {
			_fill_spell_menu(menu, text, e);
			have_selection = TRUE;
		} else
			have_selection = FALSE;
	}

	/* cut */
	item =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_menu_item_new_with_label("Cut");
#else
	    gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT, NULL);
#endif
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item), "activate",
			 G_CALLBACK(action_cut_activate_cb), e);
	gtk_widget_set_sensitive(item, have_selection);
	gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

	/* copy */
	item =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_menu_item_new_with_label("Copy");
#else
	    gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY, NULL);
#endif
	gtk_widget_show(item);
	gtk_widget_set_sensitive(item, have_selection);
	g_signal_connect(G_OBJECT(item), "activate",
			 G_CALLBACK(action_copy_activate_cb), e);
	gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

	/* paste */
	item =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_menu_item_new_with_label("Paste");
#else
	    gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE, NULL);
#endif
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item), "activate",
			 G_CALLBACK(action_paste_activate_cb), e);
	gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

	/* separator */
	item = gtk_separator_menu_item_new();
	gtk_widget_show(item);
	gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

	/* select all */
	item =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_menu_item_new_with_label("Select All");
#else
	    gtk_image_menu_item_new_from_stock(GTK_STOCK_SELECT_ALL, NULL);

#endif
	gtk_widget_show(item);
	/*g_signal_connect (G_OBJECT (item), "activate",
	   G_CALLBACK (action_paste_activate_cb),
	   NULL); */
	gtk_menu_shell_append((GtkMenuShell *)menu, (GtkWidget *)item);

#if GTK_CHECK_VERSION(3, 22, 0)
	gtk_menu_popup_at_pointer((GtkMenu *)menu, NULL);
#else
	gtk_menu_popup((GtkMenu *)menu, NULL, NULL, NULL, NULL, 3, time);
#endif
}

/******************************************************************************
 * Name
 *   button_handler
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   gboolean button_handler (GtkWidget *widget,
 *                              GdkEvent  *event,
 *                              EDITOR * e)
 *
 * Description
 *   user pressed a button - set button status 
 *   if right button call _create_context_menu()
 *
 * Return value
 *   gboolean
 */

static gboolean button_handler(GtkWidget *widget, GdkEvent *event, EDITOR *e)
{
	WebKitDOMDocument *dom_document;
	WebKitDOMElement *element = NULL;
	buttons_state.bold = 0;
	buttons_state.italic = 0;
	buttons_state.underline = 0;
	buttons_state.strike = 0;
	buttons_state.style = 0;
	buttons_state.nochange = 1;
	gchar *name = NULL;
	gchar *class = NULL;
	gchar *color = NULL;
	gint i = 1;

	//current_element = NULL;
	e->toolitems.outline_level = 0;
	mouse_x = event->button.x;
	mouse_y = event->button.y;

	dom_document = webkit_web_view_get_dom_document((WebKitWebView *)
							e->html_widget);
	if (!dom_document)
		return 0;

	if (event->button.button == 3) {
		_create_context_menu(dom_document, event->button.time, e);
		return 1; // return true so we don't get the webkit context menu
	}

	element =
	    webkit_dom_document_element_from_point(dom_document, mouse_x,
						   mouse_y);
	if (!element)
		return 0;

	name = webkit_dom_element_get_tag_name(element);
	if (!name)
		return 0;

	/* set buttons.color to font color element */
	if (buttons_state.color)
		g_free(buttons_state.color);
	buttons_state.color = g_strdup("#000000"); /* start with black */

	/* we have to set it here in case the color element is the only element */
	color = webkit_dom_element_get_attribute(element, "color");
	if (color[0] == '#') {
		if (buttons_state.color)
			g_free(buttons_state.color);
		buttons_state.color = g_strdup(color);
	}

	/*      
	   if(!g_strcmp0 ("LI",name)){
	   current_element = element;
	   }
	 */
	i = _has_element(name, class, e);
	while (i) {
		if (name) {
			XI_message(("\nelement name:  %s\nclass:     %s\ncolor: %s\n", name, class, color));
			element =
			    webkit_dom_node_get_parent_element((WebKitDOMNode *)element);
			if (element) {
				class =
				    webkit_dom_element_get_attribute(element, "class");
				color =
				    webkit_dom_element_get_attribute(element, "color");
				name =
				    webkit_dom_element_get_tag_name(element);

				if (color[0] == '#') {
					if (buttons_state.color)
						g_free(buttons_state.color);
					buttons_state.color =
					    g_strdup(color);
				}
			}
		}
		i = _has_element(name, class, e);
	}
	set_button_state(buttons_state, e);
	buttons_state.nochange = 0;
	return 0;
}

/******************************************************************************
 * Name
 *   create_editor_window
 *
 * Synopsis
 *   #include "editor/editor.h"
 *
 *   void create_editor_window (GtkWidget * scrollwindow, EDITOR * e)
 *
 * Description
 *   create and setup webkit wigdet for editing
 *
 * Return value
 *   void
 */

void create_editor_window(GtkWidget *scrollwindow, EDITOR *e)
{
	WebKitWebSettings *setting;
	GtkWidget *webview;
	gchar *text = NULL, *fname = NULL;

	webview = webkit_web_view_new();
	e->html_widget = webview;
	gtk_widget_show(webview);

	/* Turn on editing */
	webkit_web_view_set_editable((WebKitWebView *)webview, TRUE);

	/* Create a new websettings and enable spell checking */
	setting = webkit_web_settings_new();
	g_object_set(G_OBJECT(setting), "enable-spell_checking", TRUE,
		     NULL);

	/* Apply the result */
	webkit_web_view_set_settings(WEBKIT_WEB_VIEW(webview), setting);

	/* new empty document from template */
	fname = g_build_filename(settings.gSwordDir, "studypad.spt", NULL);
	XI_message(("action delete item [%s]", fname));
	text = inhale_text_from_file(fname);
	g_free(fname);

	if (text && strlen(text)) {
		webkit_web_view_load_string((WebKitWebView *)
					    e->html_widget,
					    text,
					    "text/html", "utf_8",
					    "file://");
	}
	if (text)
		g_free(text);

	e->is_changed = FALSE;

	gtk_container_add(GTK_CONTAINER(scrollwindow), webview);

	g_signal_connect(G_OBJECT(webview), "navigation-requested",
			 G_CALLBACK(on_navigation_requested), e);
	g_signal_connect(G_OBJECT(webview), "hovering-over-link",
			 G_CALLBACK(link_handler), e);
	g_signal_connect(G_OBJECT(webview), "user-changed-contents",
			 G_CALLBACK(user_changed_contents_cb), e);
	g_signal_connect(G_OBJECT(webview), "button-press-event",
			 G_CALLBACK(button_handler), e);
	g_signal_connect(G_OBJECT(webview), "key-press-event",
			 G_CALLBACK(key_handler), e);
}

#endif /* USE_WEBKIT_EDITOR */
