/*
 * Xiphos Bible Study Tool
 * bibletext_dialog.c - view Bible text module in a dialog
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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

#include <gtk/gtk.h>
#ifdef USE_GTKMOZEMBED
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "webkit/wk-html.h"
#endif
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

#include "gui/bibletext_dialog.h"
#include "gui/display_info.h"
#include "gui/font_dialog.h"
#include "gui/sidebar.h"
#include "gui/main_window.h"
#include "gui/menu_popup.h"
#include "gui/navbar_versekey_dialog.h"
#include "gui/xiphos.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/navbar.h"
#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"
#include "main/display.hh"

#include "gui/debug_glib_null.h"


/******************************************************************************
 * static - global to this file only
 */
static DIALOG_DATA *cur_vt;

/******************************************************************************
 * externs
 */
extern gboolean bible_freed;

#ifndef USE_GTKMOZEMBED
/******************************************************************************
 * Name
 *   show_in_statusbar
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *   void show_in_statusbar(GtkWidget * statusbar, gchar * key, gchar * mod)
 *
 * Description
 *   display information (morph or strongs) in statusbar
 *
 * Return value
 *   void
 */

static void show_in_statusbar(GtkWidget *statusbar,
			      gchar *key,
			      gchar *mod)
{
	gchar *str;
	gchar *text;
	gint context_id2;

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
					 settings.program_title);
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);

	text = main_get_striptext(mod, key);
	str = remove_linefeeds(text);
	if (str) {
		gtk_statusbar_push(GTK_STATUSBAR(statusbar),
				   context_id2, str);
		g_free(str);
	}
	g_free(text);
}
#endif /* !USE_GTKMOZEMBED */


/******************************************************************************
 * Name
 *   link_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void link_clicked(GtkHTML * html, const gchar * url,
 *							gpointer data)
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

#ifndef USE_GTKMOZEMBED
static void link_clicked(GtkHTML *html,
			 const gchar *url,
			 DIALOG_DATA *vt)
{
	cur_vt = vt;
	main_dialogs_url_handler(vt, url, TRUE);
}
#endif /* !USE_GTKMOZEMBED */

/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void dialog_destroy(GtkObject *object, DL_DIALOG * dlg)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void dialog_destroy(GtkObject *object,
			   DIALOG_DATA *vt)
{
	if (!bible_freed)
		main_free_on_destroy(vt);
	bible_freed = FALSE;
}


static gboolean on_dialog_motion_notify_event(GtkWidget *widget,
					      GdkEventMotion *event,
					      DIALOG_DATA *vt)
{
	cur_vt = vt;
	return FALSE;
}


#ifndef USE_GTKMOZEMBED
/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, DIALOG_DATA * vt)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void dialog_url(GtkHTML *html,
		       const gchar *url,
		       DIALOG_DATA *vt)
{
	gchar buf[255], *buf1;
	gchar *url_buf = NULL;
	gint context_id2;

	cur_vt = vt;


	if (url) url_buf = g_strdup(url);
	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(vt->statusbar),
					 settings.program_title);
	gtk_statusbar_pop(GTK_STATUSBAR(vt->statusbar), context_id2);

	/***  moved out of url - clear appbar  ***/
	if (url == NULL) {
		gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar),
				   context_id2, "");
		in_url = FALSE;
	}
	/***  we are in an url  ***/
	else {
		in_url = TRUE;

		if (main_dialogs_url_handler(vt, url, FALSE))
			return;
		if (*url_buf == '#') {
			++url_buf;	/* remove # */
			if (*url_buf == 'T') {
				++url_buf;	/* remove T */
				if (*url_buf == 'G') {
					++url_buf;	/* remove G */
					return;
				}

				if (*url_buf == 'H') {
					++url_buf;	/* remove H */
					return;
				}
			}

			if (*url_buf == 'G') {
				++url_buf;	/* remove G */
				buf1 = g_strdup(url_buf);
				if (atoi(buf1) > 5624) {
					return;
				}
				else {
					show_in_statusbar(vt->statusbar,
							  buf1,
							  settings.
							  lex_greek);
					g_free(buf1);
					return;
				}
			}

			if (*url_buf == 'H') {
				++url_buf;	/* remove H */
				buf1 = g_strdup(url);
				if (atoi(buf1) > 8674) {
					return;
				}
				else {
					show_in_statusbar(vt->statusbar,
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
			buf1 = g_strdup(url_buf);
			modbuf = "Robinson";
			mybuf = NULL;
			mybuf = strstr(buf1, "value=");
			if (mybuf) {
				mybuf = strchr(mybuf, '=');
				++mybuf;
			}
			buf1 = g_strdup(mybuf);
			show_in_statusbar(vt->statusbar, buf1, modbuf);
			g_free(buf1);
			return;
		}
		/*** thml strongs ***/
		else if (!strncmp(url_buf, "type=Strongs", 12)) {
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
				modbuf = settings.lex_greek;
			else
				modbuf = settings.lex_hebrew;

			buf1 = g_strdup(newref);
			show_in_statusbar(vt->statusbar, buf1, modbuf);
			g_free(buf1);
			return;
		}
		/***  any other link  ***/
		else
			sprintf(buf, "%s %s", _("Go to "), url);
		gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar),
				   context_id2, buf);
	}
	if (url_buf)
		g_free(url_buf);
}
#endif /* !USE_GTKMOZEMBED */


/******************************************************************************
 * Name
 *   sync_with_main
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_with_main(DIALOG_DATA * c)
 *
 * Descriptionc->navbar.lookup_entry
 *   bring the the View Commentay Dialog module into sync with main window
 *
 * Return value
 *   void
 */

static void sync_with_main(DIALOG_DATA *c)
{
	gchar *url = g_strdup_printf("sword:///%s", settings.currentverse);
	main_dialogs_url_handler(c, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *   sync_toggled
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_toggled(GtkToggleButton * button, DIALOG_DATA * c)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_bible_dialog_sync_toggled(GtkToggleButton *button,
				   DIALOG_DATA *c)
{
	if (c == NULL)
		c = cur_vt;
	if (button->active) {
		sync_with_main(c);
		c->sync = TRUE;
	} else
		c->sync = FALSE;
}


/******************************************************************************
 * Name
 *   create_nav_toolbar
 *
 * Synopsis
 *   #include ".h"
 *
 *   GtkWidget *create_nav_toolbar(void)
 *
 * Description
 *    create navigation toolbar and
 *
 * Return value
 *   void
 */

static GtkWidget *create_nav_toolbar(DIALOG_DATA *c)
{
	c->navbar.type = NB_DIALOG;
	return gui_navbar_versekey_dialog_new(c);
}


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
static gboolean on_text_button_press_event(GtkWidget *widget,
					   GdkEventButton *event,
					   DIALOG_DATA *t)
{
	cur_vt = t;

	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		gui_menu_popup (NULL, t);
		//create_menu(t, event);
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
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

static gboolean on_button_release_event(GtkWidget *widget,
					GdkEventButton *event,
					DIALOG_DATA *t)
{

	gchar *key;
	cur_vt = t;

	//settings.whichwindow = MAIN_TEXT_WINDOW;
	/*
	 * set program title to current text module name
	 */
	//gui_change_window_title(t->mod_name);

	switch (event->button) {
	case 1:
		if (!in_url) {
			key = gui_button_press_lookup(t->html);
			if (key) {
				gchar *dict = NULL;
				if (settings.useDefaultDict)
					dict =
					    g_strdup(settings.
						     DefaultDict);
				else
					dict =
					    g_strdup(settings.
						     DictWindowModule);
				main_display_dictionary(dict,
								  key);
				g_free(key);
				if (dict)
					g_free(dict);
			}
		}
		break;
	case 2:
		break;
	case 3:
		break;
	}
	return FALSE;
}
#endif /* !USE_GTKMOZEMBED */

#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb (WkHtml *html,
			 gchar *uri,
			 gpointer user_data)
{
	DIALOG_DATA * d = (DIALOG_DATA *)user_data;
	gui_text_dialog_create_menu(d);
}
#endif /* USE_GTKMOZEMBED */

/******************************************************************************
 * Name
 *   create_bibletext_dialog
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *   void create_bibletext_dialog(DIALOG_DATA * vt)
 *
 * Description
 *   create a text dialog
 *
 * Return value
 *   void
 */

void gui_create_bibletext_dialog(DIALOG_DATA * vt)
{
	GtkWidget *vbox33;
	GtkWidget *paned;
	GtkWidget *frame;
#ifdef USE_GTKMOZEMBED
	GtkWidget *eventbox;
#else
	GtkWidget *swVText;
#endif

	vt->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_object_set_data(G_OBJECT(vt->dialog), "dlg->dialog",
			  vt->dialog);
	gtk_window_set_title(GTK_WINDOW(vt->dialog),
			     main_get_module_description(vt->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(vt->dialog), 200, 200);
	gtk_window_set_resizable(GTK_WINDOW(vt->dialog), TRUE);

	vbox33 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox33);
	gtk_container_add(GTK_CONTAINER(vt->dialog), vbox33);

	vt->toolbar_nav = create_nav_toolbar(vt);
	gtk_widget_show(vt->toolbar_nav);
	gtk_box_pack_start(GTK_BOX(vbox33), vt->toolbar_nav, FALSE, FALSE,
			   0);

	paned = gtk_vpaned_new();
	gtk_box_pack_start(GTK_BOX(vbox33), paned, TRUE, TRUE, 0);
	gtk_widget_show(paned);

	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_paned_add1((GtkPaned *)paned,frame);
	gtk_widget_set_size_request(frame, -1, 200);

#ifdef USE_GTKMOZEMBED
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);

	eventbox = gtk_event_box_new();
	gtk_widget_show(eventbox);
	gtk_container_add(GTK_CONTAINER(frame), eventbox);
	vt->html = GTK_WIDGET(wk_html_new());

	gtk_widget_show(vt->html);
	gtk_container_add(GTK_CONTAINER(eventbox), vt->html);
	g_signal_connect((gpointer)vt->html,
		      "popupmenu_requested",
		      G_CALLBACK(_popupmenu_requested_cb),
		      vt);

	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_paned_add2((GtkPaned *)paned,frame);
	gtk_widget_set_size_request(frame, -1, 100);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);

	eventbox = gtk_event_box_new();
	gtk_widget_show(eventbox);
	gtk_container_add(GTK_CONTAINER(frame), eventbox);

	vt->previewer = GTK_WIDGET(wk_html_new());
	gtk_widget_show(vt->previewer);
	gtk_container_add(GTK_CONTAINER(eventbox), vt->previewer);
#else
	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(swVText);
	gtk_container_add(GTK_CONTAINER(frame), swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)swVText,
					    settings.shadow_type);
	vt->html = gtk_html_new();
	gtk_widget_show(vt->html);
	gtk_container_add(GTK_CONTAINER(swVText), vt->html);
	gtk_html_load_empty(GTK_HTML(vt->html));
	g_signal_connect(G_OBJECT(vt->html), "on_url",
			   G_CALLBACK(dialog_url), (gpointer) vt);
	g_signal_connect(GTK_OBJECT(vt->html), "link_clicked",
			   G_CALLBACK(link_clicked), vt);
	g_signal_connect(GTK_OBJECT(vt->html),
			   "motion_notify_event",
			   G_CALLBACK
			   (on_dialog_motion_notify_event), vt);
	g_signal_connect(GTK_OBJECT(vt->html),
			   "button_release_event",
			   G_CALLBACK
			   (on_button_release_event),
			   (DIALOG_DATA *) vt);
	g_signal_connect(GTK_OBJECT(vt->html),
			   "button_press_event",
			   G_CALLBACK
			   (on_text_button_press_event),
			   (DIALOG_DATA *) vt);
	g_signal_connect(GTK_OBJECT(vt->html),
			   "url_requested",
			   G_CALLBACK
			   (url_requested),
			   (DIALOG_DATA *) vt);


	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_paned_add2((GtkPaned *)paned,frame);

	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(swVText);
	gtk_container_add(GTK_CONTAINER(frame), swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)swVText,
                                             settings.shadow_type);

	vt->previewer = gtk_html_new();
	gtk_widget_show(vt->previewer);
	gtk_container_add(GTK_CONTAINER(swVText), vt->previewer);
	gtk_html_load_empty(GTK_HTML(vt->previewer));

	g_signal_connect(GTK_OBJECT(vt->previewer), "link_clicked",
				   G_CALLBACK(link_clicked), vt);
#endif /* !USE_GTKMOZEMBED */

	vt->statusbar = gtk_statusbar_new();
	gtk_widget_show(vt->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox33), vt->statusbar, FALSE, FALSE,
			   0);

	g_signal_connect(GTK_OBJECT(vt->dialog),
			   "destroy",
			   G_CALLBACK(dialog_destroy), vt);


	g_signal_connect(GTK_OBJECT(vt->dialog),
			   "motion_notify_event",
			   G_CALLBACK
			   (on_dialog_motion_notify_event), vt);
}


void gui_text_dialog_create_menu(DIALOG_DATA * d)
{
	gui_menu_popup (NULL, d);
}

/******   end of file   ******/
