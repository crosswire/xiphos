/*
 * Xiphos Bible Study Tool
 * about_modules.c - Sword modules about dialog
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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

#include "xiphos_html/xiphos_html.h"

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

static void about_modules_ok(GtkButton *button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

static void on_copy_activate(GtkMenuItem *menuitem, gpointer data)
{
	XI_message(("on_copy_activate"));
	XIPHOS_HTML_COPY_SELECTION(text_html);
}

static void create_menu1(void)
{
	GtkWidget *menu;
	GtkWidget *item;
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new();

	menu = gtk_menu_new();
	item =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_menu_item_new_with_mnemonic("_Copy");
#else
	    gtk_image_menu_item_new_from_stock("gtk-copy", accel_group);
#endif
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item), "activate",
			 G_CALLBACK(on_copy_activate), NULL);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_menu_set_accel_group(GTK_MENU(menu), accel_group);
#if GTK_CHECK_VERSION(3, 22, 0)
	gtk_menu_popup_at_pointer((GtkMenu *)menu, NULL);
#else
	gtk_menu_popup((GtkMenu *)menu, NULL, NULL, NULL, NULL, 2,
		gtk_get_current_event_time()); 
#endif
}

static void
_popupmenu_requested_cb(XiphosHtml *html, gchar *uri, gpointer user_data)
{
	create_menu1();
}

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

static GtkWidget *gui_create_about_modules(void)
{
	GtkWidget *dialog_about_mods;
	GtkWidget *dialog_vbox28;
	GtkWidget *vbox25;
	GtkWidget *hbox21;
	GtkWidget *pixmap;
	GtkWidget *frame73;
#ifndef USE_WEBKIT2
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
	gtk_window_set_default_size(GTK_WINDOW(dialog_about_mods), 500,
				    400);
	gtk_window_set_resizable(GTK_WINDOW(dialog_about_mods), TRUE);

	dialog_vbox28 =
	    gtk_dialog_get_content_area(GTK_DIALOG(dialog_about_mods));
	g_object_set_data(G_OBJECT(dialog_about_mods), "dialog_vbox28",
			  dialog_vbox28);
	gtk_widget_show(dialog_vbox28);

	UI_VBOX(vbox25, FALSE, 0);
	gtk_widget_show(vbox25);
	gtk_box_pack_start(GTK_BOX(dialog_vbox28), vbox25, TRUE, TRUE, 0);

	UI_HBOX(hbox21, FALSE, 0);
	gtk_widget_show(hbox21);
	gtk_box_pack_start(GTK_BOX(vbox25), hbox21, FALSE, TRUE, 0);

	pixmap = pixmap_finder("sword.png");
	gtk_widget_show(pixmap);
	gtk_box_pack_start(GTK_BOX(hbox21), pixmap, TRUE, TRUE, 0);

	frame73 = gtk_frame_new(NULL);
	gtk_widget_show(frame73);
	gtk_box_pack_start(GTK_BOX(vbox25), frame73, TRUE, TRUE, 0);

#ifndef USE_WEBKIT2
	scrolledwindow30 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow30);
	gtk_container_add(GTK_CONTAINER(frame73), scrolledwindow30);
	gtk_widget_set_size_request(scrolledwindow30, 304, 183);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow30),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow30,
					    settings.shadow_type);
#endif

	text_html = GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, 12));
	gtk_widget_show(text_html);
	g_signal_connect((gpointer)text_html,
			 "popupmenu_requested",
			 G_CALLBACK(_popupmenu_requested_cb), NULL);

#ifdef USE_WEBKIT2
	gtk_container_add(GTK_CONTAINER(frame73), text_html);
#else
	gtk_container_add(GTK_CONTAINER(scrolledwindow30), text_html);
#endif
	dialog_action_area28 =
#if GTK_CHECK_VERSION(3, 12, 0)
	    gtk_dialog_get_content_area(GTK_DIALOG(dialog_about_mods));
#else
	    gtk_dialog_get_action_area(GTK_DIALOG(dialog_about_mods));
#endif
	g_object_set_data(G_OBJECT(dialog_about_mods),
			  "dialog_action_area28", dialog_action_area28);
	gtk_widget_show(dialog_action_area28);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area28),
				       10);

#ifdef USE_GTK_3
	hbuttonbox7 = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
#else
	hbuttonbox7 = gtk_hbutton_box_new();
#endif
	gtk_widget_show(hbuttonbox7);
#if GTK_CHECK_VERSION(3, 12, 0)
	gtk_box_pack_start(GTK_BOX(dialog_action_area28), hbuttonbox7,
			   FALSE, TRUE, 3);
#else
	gtk_box_pack_start(GTK_BOX(dialog_action_area28), hbuttonbox7,
			   TRUE, TRUE, 0);
#endif
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox7),
				  GTK_BUTTONBOX_END);

	button =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_button_new_with_mnemonic(_("Close"));
#else
	    gtk_button_new_from_stock(GTK_STOCK_CLOSE);
#endif
	gtk_widget_show(button);
	gtk_container_add(GTK_CONTAINER(hbuttonbox7), button);
#if GTK_CHECK_VERSION(2, 18, 0)
	gtk_widget_set_can_default(button, TRUE);
#else
#ifdef USE_GTK_3
	gtk_widget_set_can_default(button, 1);
#else
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
#endif
#endif
	g_signal_connect(G_OBJECT(button), "clicked",
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
about_module_display(GString *str,
		     const gchar *text, const gboolean tooltip)
{
	gboolean center = FALSE;

	for (/* */; *text; ++text) {
		if (*text == '\\') { // a RTF command
			if ((text[1] == 'p') &&
			    (text[2] == 'a') &&
			    (text[3] == 'r') && (text[4] == 'd')) {
				if (center && !tooltip) {
					str =
					    g_string_append(str,
							    "</center>");
					center = FALSE;
				}
				text += 4;
				continue;
			}
			if ((text[1] == 'p') &&
			    (text[2] == 'a') && (text[3] == 'r')) {
				str =
				    g_string_append(str,
						    (tooltip ? "\n" : "<br/>\n"));
				text += 3;
				continue;
			}
			if ((text[1] == ' ') || (text[1] == '\n')) {
				text += 1;
				continue;
			}
			if ((text[1] == 'q') && (text[2] == 'c')) {
				if (!center && !tooltip) {
					str =
					    g_string_append(str,
							    "<center>");
					center = TRUE;
				}
				text += 2;
				continue;
			}
		}
		if (tooltip && (*text == '<')) { /* no htmlisms in tooltips */
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
gui_core_display_about_dialog(const gchar *desc,
			      const gchar *abouttext,
			      const gchar *modname)
{
	GtkWidget *aboutbox; //-- pointer to about dialog
	GString *str = g_string_new(NULL);
	GString *description = g_string_new(NULL);
	GString *text = g_string_new(NULL);
	GString *html_start = g_string_new(NULL);

	static const char *html_end = "</font></body></html>";
	MOD_FONT *mf = get_font(modname);
	g_string_printf(html_start,
			"<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head><body bgcolor=\"%s\" text=\"%s\"><font face=\"%s\" size=\"%+d\">",
			settings.bible_bg_color,
			settings.bible_text_color,
			((mf->old_font) ? mf->old_font : ""),
			mf->old_font_size_value - 1);
	free_font(mf);

	g_string_printf(description,
			"<center><font color=\"#000fcf\"><br/><b>%s</b></font></center><br/><hr width=\"80%%\"/><br/>",
			(desc ? desc : "No module present"));
	aboutbox = gui_create_about_modules();
	gtk_widget_show(aboutbox);

	about_module_display(str, abouttext, FALSE);

	g_string_append(text, html_start->str);
	g_string_append(text, description->str);
	g_string_append(text, str->str);
	g_string_append(text, html_end);
	HtmlOutput(text->str, text_html, NULL, NULL);

	g_string_free(text, TRUE);
	g_string_free(str, TRUE);
	g_string_free(description, TRUE);
	g_string_free(html_start, TRUE);
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

void gui_display_about_module_dialog(gchar *modname)
{
	const gchar *desc;
	gchar *about;
	const gchar *version, *language, *abbreviation,
	    *size, *promo, *dist, *langtoken, *companion,
	    *category, *feature;
	int feature_count = 0;
	GString *info = g_string_new("");

	desc = main_get_mod_config_entry(modname, "Description");
	about = main_get_mod_config_entry(modname, "About");
	version = main_get_mod_config_entry(modname, "Version");
	size = main_get_mod_config_entry(modname, "InstallSize");
	promo = main_get_mod_config_entry(modname, "ShortPromo");
	dist = main_get_mod_config_entry(modname, "DistributionLicense");
	companion = main_get_mod_config_entry(modname, "Companion");
	category = main_get_mod_config_entry(modname, "Category");
	feature = main_get_mod_config_entry(modname, "Feature");
	langtoken = main_get_mod_config_entry(modname, "Lang");
	language = main_get_module_language(modname);
	abbreviation = main_get_abbreviation(modname);

	info = g_string_append(info,
			       "<center><table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">");
	info = g_string_append(info, "<tr><td valign=\"top\">");

	info = g_string_append(info, _("<b>Module name:</b> "));
	info = g_string_append(info, modname);
	info = g_string_append(info, "<br/>");

	if (abbreviation && *abbreviation) {
		info =
		    g_string_append(info,
				    _("<b>Module abbreviation:</b> "));
		info = g_string_append(info, abbreviation);
		info = g_string_append(info, "<br/>");
	}

	info = g_string_append(info, _("<b>Version:</b> "));
	info = g_string_append(info, ((version && *version)
					  ? version
					  : _("No version stamp found")));
	info = g_string_append(info, "<br/>");

	info = g_string_append(info, _("<b>Type:</b> "));
	if (feature && !strcmp(feature, "DailyDevotion"))
		info = g_string_append(info, _("Daily Devotion"));
	else if (category && !strcmp(category, "Maps"))
		info = g_string_append(info, _("Maps"));
	else if (category && !strcmp(category, "Images"))
		info = g_string_append(info, _("Images"));
	else if (category && !strcmp(category, "Glossaries"))
		info = g_string_append(info, _("Glossary"));
	else if (category &&
		 !strcmp(category,
			 "Cults / Unorthodox / Questionable Material"))
		info = g_string_append(info, _("Cult"));
	else {
		switch (main_get_mod_type(modname)) {
		case TEXT_TYPE:
			info = g_string_append(info, _("Bible"));
			break;
		case COMMENTARY_TYPE:
			info = g_string_append(info, _("Commentary"));
			break;
		case DICTIONARY_TYPE:
			info = g_string_append(info, _("Dictionary"));
			break;
		case BOOK_TYPE:
			info = g_string_append(info, _("General Book"));
			break;
		case PERCOM_TYPE:
			info =
			    g_string_append(info,
					    _("Personal Commentary"));
			break;
		case PRAYERLIST_TYPE:
			info = g_string_append(info, _("Journal"));
			break;
		default:
			info = g_string_append(info, _("Unknown type"));
			break;
		}
	}
	info = g_string_append(info, "<br/>");

	info = g_string_append(info, _("<b>Language:</b> "));
	info = g_string_append(info, ((language && *language)
					  ? language
					  : _("Not specified")));
	info = g_string_append(info, "&nbsp;(");
	info = g_string_append(info, ((langtoken && *langtoken)
					  ? langtoken
					  : "?"));
	info = g_string_append(info, ")");
	info = g_string_append(info, "<br/>");

	if (size && *size) {
		info = g_string_append(info, _("<b>Installed size:</b> "));
		info = g_string_append(info, size);
		info = g_string_append(info, "<br/>");
	}

	if (companion && *companion) {
		info =
		    g_string_append(info,
				    _("<b>Companion module(s):</b></br>"));
		info = g_string_append(info, companion);
		info = g_string_append(info, "<br/>");
	}

	info = g_string_append(info,
			       "</td>"
			       "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
			       "<td valign=\"top\">");

	info = g_string_append(info, _("<b>Features:</b>"));
	info = g_string_append(info, "<br/>");
	if (main_check_for_global_option((gchar *)modname, "ThMLHeadings") || main_check_for_global_option((gchar *)modname,
													   "OSISHeadings")) {
		info = g_string_append(info, _("*&nbsp;Headings"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "GBFFootnotes") || main_check_for_global_option((gchar *)modname,
													   "ThMLFootnotes") ||
	    main_check_for_global_option((gchar *)modname,
					 "OSISFootnotes")) {
		info = g_string_append(info, _("*&nbsp;Footnotes"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "ThMLScripref") || main_check_for_global_option((gchar *)modname,
													   "OSISScripref")) {
		info = g_string_append(info, _("*&nbsp;Cross references"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if ((main_check_for_global_option((gchar *)modname, "GBFStrongs")) ||
	    (main_check_for_global_option((gchar *)modname, "ThMLStrongs")) ||
	    (main_check_for_global_option((gchar *)modname, "OSISStrongs"))) {
		info = g_string_append(info, _("*&nbsp;Strong's numbers"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "GBFMorph") ||
	    main_check_for_global_option((gchar *)modname, "ThMLMorph") ||
	    main_check_for_global_option((gchar *)modname, "OSISMorph")) {
		info =
		    g_string_append(info, _("*&nbsp;Morphological tags"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "ThMLLemma") ||
	    main_check_for_global_option((gchar *)modname, "OSISLemma")) {
		info = g_string_append(info, _("*&nbsp;Lemmas"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if ((main_check_for_global_option((gchar *)modname, "GBFRedLetterWords")) ||
	    (main_check_for_global_option((gchar *)modname, "OSISRedLetterWords"))) {
		info =
		    g_string_append(info,
				    _("*&nbsp;Words of Christ in red"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "UTF8GreekAccents")) {
		info = g_string_append(info, _("*&nbsp;Greek accents"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "UTF8HebrewPoints")) {
		info =
		    g_string_append(info, _("*&nbsp;Hebrew Vowel Points"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "UTF8Cantillation")) {
		info =
		    g_string_append(info, _("*&nbsp;Hebrew Cantillation"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "ThMLVariants") || main_check_for_global_option((gchar *)modname,
													   "OSISVariants")) {
		info = g_string_append(info, _("*&nbsp;Variant readings"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "OSISXlit")) {
		info =
		    g_string_append(info,
				    _("*&nbsp;Transliteration forms"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "OSISEnum")) {
		info = g_string_append(info, _("*&nbsp;Enumerations"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "OSISGlosses") || main_check_for_global_option((gchar *)modname, "OSISRuby")) {
		info = g_string_append(info, _("*&nbsp;Glosses"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (main_check_for_global_option((gchar *)modname, "OSISMorphSegmentation")) {
		info =
		    g_string_append(info,
				    _("*&nbsp;Morpheme Segmentation"));
		info = g_string_append(info, "<br/>");
		feature_count++;
	}
	if (feature_count == 0) {
		info =
		    g_string_append(info,
				    _("*&nbsp;<i>No features found</i>"));
		info = g_string_append(info, "<br/>");
	}

	info = g_string_append(info, "</td></tr></table></center>");

	if (dist && *dist) {
		info = g_string_append(info, "<br/><center>");
		info =
		    g_string_append(info,
				    _("<b>Distribution license:</b>"));
		info = g_string_append(info, "<br/>");
		info = g_string_append(info, dist);
		info = g_string_append(info, "</a></center>");
	}

	if (promo) {
		/* if promo looks like just an undecorated url, hotlink it. */
		gboolean just_url = (((strncmp(promo, "http", 4) == 0) ||
				      (strncmp(promo, "www.", 4) == 0)) &&
				     (strchr(promo, ' ') == 0));

		info = g_string_append(info, "<br/><center>");
		info = g_string_append(info, _("<b>Promotional:</b> "));
		info = g_string_append(info, "<br/>");
		if (just_url) {
			g_string_append(info, "<a href=\"");
			if (*promo == 'w')
				info = g_string_append(info, "http://");
			info = g_string_append(info, promo);
			g_string_append(info, "\">");
		}
		info = g_string_append(info, promo);
		if (just_url)
			g_string_append(info, "</a>");
		info = g_string_append(info, "</center>");
	}

	info = g_string_append(info, "<br/><hr width=\"80%%\"/><br/>");
	info = g_string_append(info, about);

	gui_core_display_about_dialog(desc, info->str, modname);
}
