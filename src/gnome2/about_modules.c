/*
 * Xiphos Bible Study Tool
 * about_modules.c - Sword modules about dialog
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
#include "gecko/gecko-html.h"
#endif
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

#include "gui/about_modules.h"
#include "gui/utilities.h"
#include "gui/xiphos.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/display.hh"

#include "gui/debug_glib_null.h"

/******************************************************************************
 * static
 */

static GtkWidget *text_html;

/******************************************************************************
 * Name
 *   about_modules_ok
 *
 * Description
 *   Callback if about module button is clicked.
 *
 * Return value
 *   void
 */

static void
about_modules_ok(GtkButton * button,
		 gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}


static void
on_copy_activate(GtkMenuItem * menuitem,
		 gpointer data)
{
	GS_message(("on_copy_activate"));
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(text_html));
#else
	gui_copy_html(text_html);
#endif
}

static void
create_menu1(void)
{
  	GtkWidget *menu;
	GtkWidget *item;
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new ();

	menu = gtk_menu_new ();
	item =gtk_image_menu_item_new_from_stock  ("gtk-copy",
                                                        accel_group);
	gtk_widget_show(item);
	g_signal_connect(GTK_OBJECT(item), "clicked",
			   G_CALLBACK  (on_copy_activate),
			  NULL);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_menu_set_accel_group (GTK_MENU (menu), accel_group);
	gtk_menu_popup((GtkMenu*)menu, NULL, NULL, NULL, NULL, 2,
					gtk_get_current_event_time());
}

/******************************************************************************
 * Name
 *  on_button_press_event
 *
 * Synopsis
 *   #include "about_modules.h"
 *
 *  gboolean on_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer data)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */
#ifndef USE_GTKMOZEMBED
static gboolean
on_button_release_event(GtkWidget * widget,
			GdkEventButton * event,
			gpointer data)
{
	switch (event->button) {
	case 1:
		break;
	case 2:

		break;
	case 3:
		create_menu1();
		break;
	}
	return FALSE;
}
#endif /* 0 */

#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb(GeckoHtml *html,
			gchar *uri,
			gpointer user_data)
{
	create_menu1();
}
#endif

/******************************************************************************
 * Name
 *   gui_create_about_modules
 *
 * Synopsis
 *   #include "about_modules.h"
 *
 *   void gui_create_about_modules(void);
 *
 * Description
 *   Create Sword modules about dialog and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *
gui_create_about_modules(void)
{
	GtkWidget *dialog_about_mods;
	GtkWidget *dialog_vbox28;
	GtkWidget *vbox25;
	GtkWidget *hbox21;
	GtkWidget *pixmap;
	GtkWidget *frame73;
#ifndef USE_GTKMOZEMBED
	GtkWidget *scrolledwindow30;
#endif
	GtkWidget *dialog_action_area28;
	GtkWidget *hbuttonbox7;
	GtkWidget *button;

	dialog_about_mods = gtk_dialog_new();
	g_object_set_data(G_OBJECT(dialog_about_mods),
			  "dialog_about_mods", dialog_about_mods);
	gtk_window_set_title(GTK_WINDOW(dialog_about_mods),
			     _("About Sword Module"));
	gtk_window_set_default_size(GTK_WINDOW(dialog_about_mods), 324,
				    304);
	gtk_window_set_resizable(GTK_WINDOW(dialog_about_mods), TRUE);

	dialog_vbox28 = gtk_dialog_get_content_area(GTK_DIALOG(dialog_about_mods));
	g_object_set_data(G_OBJECT(dialog_about_mods),
			  "dialog_vbox28", dialog_vbox28);
	gtk_widget_show(dialog_vbox28);

	vbox25 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox25);
	gtk_box_pack_start(GTK_BOX(dialog_vbox28), vbox25, TRUE, TRUE,
			   0);

	hbox21 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox21);
	gtk_box_pack_start(GTK_BOX(vbox25), hbox21, FALSE, TRUE, 0);

	pixmap = pixmap_finder("sword.png");
	gtk_widget_show(pixmap);
	gtk_box_pack_start(GTK_BOX(hbox21), pixmap, TRUE, TRUE, 0);

	frame73 = gtk_frame_new(NULL);
	gtk_widget_show(frame73);
	gtk_box_pack_start(GTK_BOX(vbox25), frame73, TRUE, TRUE, 0);
#ifdef USE_GTKMOZEMBED
	text_html = GTK_WIDGET(gecko_html_new(NULL, FALSE, 12));
	gtk_widget_show(text_html);
	gtk_container_add(GTK_CONTAINER(frame73), text_html);
	//gtk_widget_set_sensitive(text_html,FALSE);
	g_signal_connect((gpointer)text_html,
		      "popupmenu_requested",
		      G_CALLBACK (_popupmenu_requested_cb),
		      NULL);
#else

	scrolledwindow30 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow30);
	gtk_container_add(GTK_CONTAINER(frame73), scrolledwindow30);
	gtk_widget_set_size_request(scrolledwindow30, 304, 183);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow30),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow30,
                                             settings.shadow_type);
	text_html = gtk_html_new();
	gtk_widget_show(text_html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow30), text_html);

	g_signal_connect(GTK_OBJECT(text_html),"button_release_event",
				G_CALLBACK(on_button_release_event),
				NULL);
#endif
	dialog_action_area28 =
                gtk_dialog_get_action_area(GTK_DIALOG(dialog_about_mods));
	g_object_set_data(G_OBJECT(dialog_about_mods),
			  "dialog_action_area28",
			  dialog_action_area28);
	gtk_widget_show(dialog_action_area28);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area28), 10);

	hbuttonbox7 = gtk_hbutton_box_new();
	gtk_widget_show(hbuttonbox7);
	gtk_box_pack_start(GTK_BOX(dialog_action_area28), hbuttonbox7,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox7),
				  GTK_BUTTONBOX_END);

	button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	gtk_widget_show(button);
	gtk_container_add(GTK_CONTAINER(hbuttonbox7), button);
#ifdef HAVE_GTK_218
        gtk_widget_set_can_default(button, TRUE);
#else
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
#endif
	g_signal_connect(GTK_OBJECT(button), "clicked",
			   G_CALLBACK(about_modules_ok), NULL);
	return dialog_about_mods;

}

/******************************************************************************
 * public
 *****************************************************************************/

/******************************************************************************
 * Name
 *   about_module_display
 *
 * Synopsis
 *   #include "about_modules.h"
 *
 *   void about_module_display(gchar * to, gchar * text)
 *
 * Description
 *   to filter rtf to html
 *
 * Return value
 *   void
 */

void
about_module_display(GString * str,
		     gchar * text,
		     gboolean tooltip)
{
	gboolean center = FALSE;

	for (/* */; *text; ++text) {
		if (*text == '\\') {	// a RTF command
			if ((text[1] == 'p') &&
			    (text[2] == 'a') &&
			    (text[3] == 'r') &&
			    (text[4] == 'd')) {
				if (center && !tooltip) {
					str = g_string_append(str, "</center>");
					center = FALSE;
				}
				text += 4;
				continue;
			}
			if ((text[1] == 'p') &&
			    (text[2] == 'a') &&
			    (text[3] == 'r')) {
				str = g_string_append(str, (tooltip ? "\n" : "<br/>\n"));
				text += 3;
				continue;
			}
			if ((text[1] == ' ') ||
			    (text[1] == '\n')) {
				text += 1;
				continue;
			}
			if ((text[1] == 'q') &&
			    (text[2] == 'c')) {
				if (!center && !tooltip) {
					str = g_string_append(str, "<center>");
					center = TRUE;
				}
				text += 2;
				continue;
			}
		}
		if (tooltip && (*text == '<')) {	/* no htmlisms in tooltips */
			while (*text && (*text != '>'))
				++text;
			continue;
		}
		str = g_string_append_c(str, *text);
	}
}

/******************************************************************************
 * Name
 *   gui_core_display_about_dialog
 *
 * Synopsis
 *   #include "about_modules.h"
 *
 *   void gui_core_display_about_dialog(gchar * desc, gchar * abouttext, gchar * version)
 *
 * Description
 *   internal routine to carry out About dialog manufacture.
 *
 * Return value
 *   void
 */

void
gui_core_display_about_dialog(gchar * desc,
			      gchar * abouttext,
			      const gchar * version)
{
	GtkWidget *aboutbox;	//-- pointer to about dialog
	GString *str = g_string_new(NULL);
	GString *description = g_string_new(NULL);
	GString *text = g_string_new(NULL);
	static const char *html_start =
	    "<html><head><meta http-equiv=\"content-type\" "
	    "content=\"text/html; charset=utf-8\"></head><body>";
	static const char *html_end = "</body></html>";

	g_string_printf(description,
			"<center><font color=\"#000fcf\"><b>%s</b></font><hr/>%s %s</center><br/>",
			(desc ? desc : "No module present"),
			(version ? "<br/>Sword module version" : ""),
			(version ? version : ""));
	aboutbox = gui_create_about_modules();
	gtk_widget_show(aboutbox);

	about_module_display(str,
			     ((abouttext && *abouttext)
			      ? abouttext
			      : _("The module has no About information.")),
			     FALSE);

	g_string_append_len(text, html_start, strlen(html_start));
	g_string_append_len(text, description->str, strlen(description->str));
	g_string_append_len(text, str->str, strlen(str->str));
	g_string_append_len(text, html_end, strlen(html_end));
	HtmlOutput(text->str, text_html, NULL, NULL);

	g_string_free(text, TRUE);
	g_string_free(str, TRUE);
	g_string_free(description, TRUE);
}

/******************************************************************************
 * Name
 *   gui_display_about_module_dialog
 *
 * Synopsis
 *   #include "about_modules.h"
 *
 *   void gui_display_about_module_dialog(gchar * modname)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void
gui_display_about_module_dialog(gchar *modname)
{
	gchar *buf;		//-- pointer to text buffer for label (mod name)
	gchar *bufabout;	//-- pointer to text buffer for text widget (mod about)
	const gchar * version;

	buf = main_get_module_description(modname);
	bufabout = main_get_mod_about_info(modname);
	version = main_get_mod_config_entry(modname, "Version");

	gui_core_display_about_dialog(buf, bufabout, version);

	if (bufabout)
		g_free(bufabout);
}
