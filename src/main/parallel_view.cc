/*
 * Xiphos Bible Study Tool
 * parallel_view.cc - support for displaying multiple modules
 *
 * Copyright (C) 2004-2011 Xiphos Developer Team
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

#include "backend/sword_main.hh"

#include "gui/parallel_view.h"
#include "gui/parallel_dialog.h"
#include "gui/parallel_tab.h"
#include "gui/dialog.h"
#include "gui/widgets.h"

#include "main/parallel_view.h"
#include "main/global_ops.hh"
#include "main/lists.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/url.hh"
#include "main/xml.h"
#include "main/display.hh"

#include "gui/debug_glib_null.h"

#define	HTML_START	"<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\"><!-- A { text-decoration:none } *[dir=rtl] { text-align: right; } --></style></head>"

extern GtkWidget *entrycbIntBook;
extern GtkWidget *sbIntChapter;
extern GtkWidget *sbIntVerse;
extern GtkWidget *entryIntLookup;

/******************************************************************************
 * static
 */

BackEnd *backend_p;

static const gchar *tf2of(int true_false)
{
	if (true_false)
		return "On";
	else
		return "Off";
}


/******************************************************************************
 * Name
 *   set_global_option
 *
 * Synopsis
 *   #include "main/parallel_view.h"
 *
 *   void set_global_option(char * option, gboolean choice)
 *
 * Description
 *   sets a sword global option and saves it to settings.xml
 *
 *
 *
 * Return value
 *   void
 */

static void set_global_option(char * option, gboolean choice)
{
	const char *on_off = tf2of(choice);
	SWMgr *mgr = backend_p->get_mgr();
	char *buf= g_strdup (option);

//	GS_message (("option://%s\n\n",option));

	mgr->setGlobalOption(buf, on_off);

	xml_set_value("Xiphos",
		      "parallel",
		      (char*)g_strdelimit (buf,"' ", '_'),
		      (char*)(choice ? "1" : "0"));
	g_free (buf);
}


static void set_global_textual_reading (const char * option, int choice)
{
	//gboolean primary = 0, secondary = 0, all = 0;
	char *buf= g_strdup (option);
	SWMgr *mgr = backend_p->get_mgr();

	xml_set_value("Xiphos",
		      "parallel",
		      (char*)g_strdelimit (buf,"' ", '_'),
		      (char*)(choice ? "1" : "0"));

	/*switch (choice)
	{
	case 0:
		primary = TRUE;  secondary = FALSE; all = FALSE;
		break;
	case 1:
		primary = FALSE; secondary = TRUE;  all = FALSE;
		break;
	case 2:
		primary = FALSE; secondary = FALSE; all = TRUE;
		break;
	default:
		g_message("invalid variant %d\n", choice);
		gui_generic_warning((char*)"Xiphos: invalid internal variant");
		break;
	}*/
	GS_message (("set_global_textual_reading\noption://%s",option));

	mgr->setGlobalOption("Textual Variants", option);

	g_free (buf);

}


/******************************************************************************
 * Name
 *   main_parallel_change_verse
 *
 * Synopsis
 *   #include ".h"
 *
 *   gchar *main_parallel_change_verse(void)
 *
 * Description
 *   this is for the parallel dialog only
 *
 *
 *
 * Return value
 *   gchar *
 */

gchar *main_parallel_change_verse(void)
{
	gchar *retval;
	const gchar *bookname;
	gchar buf[256];
	gint chapter, verse;
	char *newbook;
	char *numC, *numV;

	bookname = gtk_entry_get_text(GTK_ENTRY(entrycbIntBook));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (sbIntChapter));
	verse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (sbIntVerse));

	sprintf(buf, "%s %d:%d", bookname, chapter, verse);

	newbook = backend_p->key_get_book(buf);
	chapter = backend_p->key_get_chapter(buf);
	verse = backend_p->key_get_verse(buf);

	if (strcmp(bookname, newbook))
		gtk_entry_set_text(GTK_ENTRY(entrycbIntBook), newbook);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter),
				  chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), verse);

	numC = main_format_number(chapter);
	numV = main_format_number(verse);
	sprintf(buf, "%s %s:%s", newbook, numC, numV);
	g_free(numC);
	g_free(numV);

	gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
	retval = buf;
	g_free(newbook);
	return retval;
}


/******************************************************************************
 * Name
 *   gui_set_parallel_module_global_options
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void gui_set_parallel_module_global_options(gchar *option,
 *						gboolean choice)
 *
 * Description
 *   user checked or unchecked parallel option menu item
 *   set settings.parallel_<item> and call main_update_parallel_page()
 *   to display change.
 *
 * Return value
 *   void
 */

void main_set_parallel_module_global_options(GtkCheckMenuItem * menuitem,
					   gpointer user_data)
{
	gchar *option = (gchar*) user_data;
	gboolean choice = gtk_check_menu_item_get_active (menuitem);


	if (!strcmp(option, "Strong's Numbers")) {
		settings.parallel_strongs = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option, "Footnotes")) {
		settings.parallel_footnotes = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option, "Morphological Tags")) {
		settings.parallel_morphs = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option, "Hebrew Vowel Points")) {
		settings.parallel_hebrewpoints = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option, "Hebrew Cantillation")) {
		settings.parallel_cantillationmarks = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option, "Greek Accents")) {
		settings.parallel_greekaccents = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option,"Cross-references")) {
		settings.parallel_crossref = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option,"Transliteration")) {
		settings.parallel_transliteration = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option,"Words of Christ in Red")) {
		settings.parallel_red_words = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option,"Morpheme Segmentation")) {
		settings.parallel_segmentation = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option,"Headings")) {
		settings.parallel_headings = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option,"Lemmas")) {
		settings.parallel_lemmas = choice;
		set_global_option(option, choice);
	}

	if (!strcmp(option,"Primary Reading")) {
		settings.parallel_variants_primary = choice;
		set_global_textual_reading (option, choice);
	}
	if (!strcmp(option,"Secondary Reading")) {
		settings.parallel_variants_secondary = choice;
		set_global_textual_reading (option, choice);
	}
	if (!strcmp(option,"All Readings")) {
		settings.parallel_variants_all = choice;
		set_global_textual_reading (option, choice);
	}


	/* display change */
	if (settings.dockedInt) {
		main_update_parallel_page();
	} else {
		main_update_parallel_page_detached();
	}
}

/******************************************************************************
 * Name
 *   gui_set_parallel_options_at_start
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void gui_set_parallel_options_at_start(void)
 *
 * Description
 *   set sword global options on program start
 *
 * Return value
 *   void
 */

void main_set_parallel_options_at_start(void)
{
	char *value;
	SWMgr *mgr = backend_p->get_mgr();

	GList *tmp = backend->get_module_options();
	while (tmp) {
		char *option = g_strdup((char*)tmp->data);
		g_strdelimit (option,"' ", '_');
		//GS_message(("\n\n%s\n%s\n", (char*)tmp->data, option));
		value = xml_get_value("parallel", option);
		int choice = (value ? atoi(value) : 0);
		if (!strcmp((char*)tmp->data,"Textual Variants")) {
			if (atoi (xml_get_value("parallel", "Primary_Reading")))
				mgr->setGlobalOption ("Textual Variants",
						      "Primary Reading");
			else if (atoi (xml_get_value("parallel", "Secondary_Reading")))
				mgr->setGlobalOption ("Textual Variants",
						      "Secondary Reading");
			else
				mgr->setGlobalOption ("Textual Variants",
						      "All Readings");
		} else
			mgr->setGlobalOption((char*)tmp->data, choice ? "On" : "Off");
		g_free(option);
		tmp = g_list_next(tmp);
	}


}



/******************************************************************************
 * Name
 *   main_load_g_ops_parallel
 *
 * Synopsis
 *   #include "main/parallel_view.h"
 *
 *   void main_load_g_ops_parallel(GtkWidget *menu)
 *
 * Description
 *    create global options menu and set check marks
 *
 * Return value
 *   void
 */

void main_load_g_ops_parallel(GtkWidget *menu)
{
	GtkWidget * item;
	GtkWidget * variants_menu;
	GSList *group = NULL;

	item = gtk_check_menu_item_new_with_label(_("Strong's Numbers"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item),settings.parallel_strongs);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*) "Strong's Numbers");


	item = gtk_check_menu_item_new_with_label(_("Footnotes"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_footnotes);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Footnotes");


	item = gtk_check_menu_item_new_with_label(_("Morphological Tags"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_morphs);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*)  "Morphological Tags");


	item = gtk_check_menu_item_new_with_label(_("Hebrew Vowel Points"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_hebrewpoints);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*)  "Hebrew Vowel Points");


	item = gtk_check_menu_item_new_with_label(_("Hebrew Cantillation"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_cantillationmarks);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Hebrew Cantillation");


	item = gtk_check_menu_item_new_with_label(_("Greek Accents"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_greekaccents);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*)  "Greek Accents");


	item = gtk_check_menu_item_new_with_label(_("Cross-references"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_crossref);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Cross-references");


	item = gtk_check_menu_item_new_with_label(_("Lemmas"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_lemmas);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*) "Lemmas");


	item = gtk_check_menu_item_new_with_label(_("Headings"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_headings);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*)  "Headings");


	item = gtk_check_menu_item_new_with_label(_("Morpheme Segmentation"));
#if 0
	gtk_widget_show(item);
#else
	gtk_widget_hide(item);
#endif
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_segmentation);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*) "Morpheme Segmentation" );


	item = gtk_check_menu_item_new_with_label(_("Words of Christ in Red"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_red_words);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Words of Christ in Red");


	item = gtk_check_menu_item_new_with_label(_("Transliteration"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_transliteration);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Transliteration");


	item = gtk_menu_item_new_with_label(_("Textual Variants"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu), item);

	variants_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item),
				  variants_menu);

	item = gtk_radio_menu_item_new_with_mnemonic (group, _("Primary Reading" ));
  	group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (item));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(variants_menu), item);
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_variants_primary);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Primary Reading");

	item = gtk_radio_menu_item_new_with_mnemonic (group, _("Secondary Reading" ));
  	group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (item));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(variants_menu), item);
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), settings.parallel_variants_secondary);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Secondary Reading");

	item = gtk_radio_menu_item_new_with_mnemonic (group, _("All Readings" ));
  	group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (item));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(variants_menu), item);
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (item), settings.parallel_variants_all);
	g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "All Readings");
}



/******************************************************************************
 * Name
 *   gui_check_parallel_modules
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void gui_check_parallel_modules(void)
 *
 * Description
 *   check for parallel modules on program start
 *   we don't want to try to display modules we don't have
 *   it makes bad things happen
 *
 * Return value
 *   void
 */

void main_check_parallel_modules(void)
{
	/* i don't know what it's good for, but we'll keep it for now. */
	return;
}


void get_heading(SWBuf &text, BackEnd *p, gint modidx)
{
	const gchar *preverse, *preverse2, *buf;
	gchar heading[8];

	int x = 0;
	sprintf(heading, "%d", x);
	while ((preverse = p->get_entry_attribute("Heading", "Preverse",
						  heading)) != NULL) {
		preverse2 = p->render_this_text(
			       settings.parallel_list[modidx], preverse);
		buf = g_strdup_printf("<br/><b>%s</b><br/><br/>", preverse2);

		text += buf;

		g_free((gchar *)preverse2); 
		g_free((gchar *)preverse);
		g_free((gchar *)buf);
		++x;
		sprintf(heading, "%d", x);
	}
}

/******************************************************************************
 * Name
 *   gui_update_parallel_page
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void gui_update_parallel_page(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_update_parallel_page(void)
{
	gchar tmpBuf[256];
	const gchar *rowcolor;
	gchar *utf8str, *mod_name;
	gint modidx;
	gboolean is_rtol = FALSE;
	GString *data;
	MOD_FONT *mf;
	SWBuf text;

	settings.cvparallel = settings.currentverse;

	sprintf(tmpBuf, HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table>",
		settings.bible_bg_color,
		settings.bible_text_color, settings.link_color);
	data = g_string_new(tmpBuf);

	if (settings.parallel_list) {
		for (modidx = 0;
		     (mod_name = settings.parallel_list[modidx]);
		     modidx++) {
			mf = get_font(mod_name);

			is_rtol = main_is_mod_rtol(mod_name);

			if (modidx % 2 == 1)	/* alternating background color */
				rowcolor = "#c0c0c0";
			else
				rowcolor = settings.bible_bg_color;

			if (modidx == 0) {
				sprintf(tmpBuf,
					"<tr><td><i><font color=\"%s\" size=\"%d\">[%s]</font></i></td></tr>",
					settings.bible_verse_num_color,
					settings.verse_num_font_size + settings.base_font_size,
					settings.currentverse);
				g_string_append(data, tmpBuf);
			}

			sprintf(tmpBuf,
				"<tr bgcolor=\"%s\"><td><b><a href=\"passagestudy.jsp?action=showModInfo&value=%s&module=%s\"><font color=\"%s\" size=\"%+d\"> [%s]</font></a></b><br/>",
				rowcolor,
				main_get_module_description(mod_name),
				mod_name,
				settings.bible_verse_num_color,
				settings.verse_num_font_size + settings.base_font_size,
				mod_name);
			g_string_append(data, tmpBuf);

			int size = ((mf->old_font_size)
				    ? (atoi(mf->old_font_size) + settings.base_font_size)
				    : settings.base_font_size);
			sprintf(tmpBuf, (((strlen(mf->old_font) < 2) ||
					  !strncmp(mf->old_font, "none", 4))
					 ? "<font size=\"%+d\">"
					 : "<font size=\"%+d\" face=\"%s\">"),
				size, mf->old_font);
			free_font(mf);
			g_string_append(data, tmpBuf);

			if (is_rtol)
				g_string_append(data, "<br/><div align=right>");

			backend_p->set_module_key(settings.parallel_list[modidx], settings.cvparallel);
			text = "";
			get_heading(text, backend_p, modidx);
			g_string_append(data, text.c_str());

			utf8str = backend_p->get_render_text(mod_name, settings.currentverse);
			if (utf8str) {
				g_string_append(data, utf8str);
				g_free(utf8str);
			}

			if (is_rtol)
				g_string_append(data, "</div><br/>");

			sprintf(tmpBuf,
				"</font><small><br/>[<a href=\"passagestudy.jsp?action=showParallel&"
				"type=swap&value=%s\">%s</a>]</small></td></tr>",
				mod_name,_("view context"));
			g_string_append(data, tmpBuf);
		}
	}

	g_string_append(data, "</table></body></html>");
	HtmlOutput(data->str, widgets.html_parallel, NULL, NULL);
	g_string_free(data, TRUE);
}

/******************************************************************************
 * Name
 *   interpolate_parallel_display
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void interpolate_parallel_display(SWBuf& text, gchar *key)
 *
 * Description
 *   carry out the hard work of getting verses for parallel display.
 *
 * Return value
 *   void
 */

static void interpolate_parallel_display(SWBuf& text, gchar *key, gint parallel_count, gint fraction)
{
	gchar  	*utf8str,
		*textColor,
		*tmpkey,
		tmpbuf[256];
	const gchar *bgColor;
	gchar str[500];
	gint cur_verse, cur_chapter, verse, modidx;
	char *cur_book;
	MOD_FONT **mf;
	gboolean *is_rtol, *is_module;

	if (!gtk_widget_get_realized (GTK_WIDGET(widgets.notebook_bible_parallel))) return;

	is_module = g_new(gboolean, parallel_count);
	is_rtol   = g_new(gboolean, parallel_count);
	mf        = g_new(MOD_FONT *, parallel_count);

	// quick cache of fonts/rtol info.
	for (modidx = 0; modidx < parallel_count; ++modidx) {
		// determine module presence once each.
		is_module[modidx] = backend->is_module(settings.parallel_list[modidx]);

		if (is_module[modidx]) {
			is_rtol[modidx] = main_is_mod_rtol(settings.parallel_list[modidx]);
			mf[modidx] = get_font(settings.parallel_list[modidx]);
		}
	}

	// need #verses to process in this chapter.
	VerseKey vkey;
	int xverses;

	vkey.AutoNormalize(1);
	vkey = key;
	xverses = (vkey.getVerseMax());

	tmpkey = backend_p->get_valid_key(key);
	cur_verse = backend_p->key_get_verse(tmpkey);
	cur_chapter = backend_p->key_get_chapter(tmpkey);
	cur_book = backend_p->key_get_book(tmpkey);
	settings.intCurVerse = cur_verse;

	for (verse = 1; verse <= xverses; ++verse) {
		snprintf(tmpbuf, 255, "%s %d:%d", cur_book, cur_chapter, verse);
		free(tmpkey);
		tmpkey = backend_p->get_valid_key(tmpbuf);

		text += "<tr valign=\"top\">";

		// mark current verse properly.
		if (verse == cur_verse)
			textColor = settings.currentverse_color;
		else
			textColor = settings.bible_text_color;

		// alternate background colors.
		if (verse % 2 == 0)
			bgColor = "#c0c0c0";
		else
			bgColor = settings.bible_bg_color;

		for (modidx = 0; modidx < parallel_count; modidx++) {
			if (is_module[modidx]) {		
				
				const gchar *newurl = main_url_encode(tmpkey);
				gchar *num = main_format_number(verse);
				snprintf(str, 499,
					 "<td width=\"%d%%\" bgcolor=\"%s\">"
					 "<a href=\"passagestudy.jsp?action=showParallel&"
					 "type=verse&value=%s\" name=\"%d\">"
					 "<font color=\"%s\" size=\"%+d\">%s. </font></a>"
					 "<font face=\"%s\" size=\"%+d\" color=\"%s\">",
					 fraction,
					 bgColor,
					 newurl,
					 verse,
					 settings.bible_verse_num_color,
					 settings.verse_num_font_size + settings.base_font_size,
					 num,
					 mf[modidx]->old_font,
					 ((mf[modidx]->old_font_size)
					  ? (atoi(mf[modidx]->old_font_size) +
					     settings.base_font_size)
					  : settings.base_font_size),
					 textColor);
				g_free((gchar*)newurl);
				g_free(num);
				text += str;

				if (is_rtol[modidx])
					text += "<br/><div align=right>";

				backend_p->set_module_key(settings.parallel_list[modidx], tmpkey);
				get_heading(text, backend_p, modidx);

				utf8str = backend_p->get_render_text
						(settings.parallel_list[modidx], tmpkey);
				if (utf8str) {
					text += utf8str;
					g_free(utf8str);
				}

				if (is_rtol[modidx])
					text += "</div>";
			}

			text += "</font></td>";
		}

		text += "</tr>";
	}
	g_free(tmpkey);
	g_free(cur_book);

	/* clear bookkeeping data */
	for (modidx = 0; modidx < parallel_count; ++modidx)
		if (is_module[modidx])
			free_font(mf[modidx]);
	g_free(mf);
	g_free(is_rtol);
	g_free(is_module);
}


/******************************************************************************
 * Name
 *   gui_update_parallel_page_detached
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void gui_update_parallel_page_detached(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_update_parallel_page_detached(void)
{
	SWBuf text("");
	gchar buf[500];
	gint modidx, parallel_count, fraction;

	if (!widgets.html_parallel_dialog 
#ifdef USE_GTKMOZEMBED
	    || !gtk_widget_get_realized(GTK_WIDGET(widgets.html_parallel_dialog))
#endif
	    ) return;

	/* how big a pile of parallels have we got? */
	if (settings.parallel_list == NULL)
		return;
	for (parallel_count = 0; settings.parallel_list[parallel_count]; ++parallel_count)
		/* just count non-null string ptrs */;

	/* get the per-column percentage width. */
	/* 2 => 50, 4 => 25, 5 => 20, 10 => 10, ... */
	fraction = (parallel_count ? (100/parallel_count) : 100);

	snprintf(buf, 499, HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table align=\"left\" valign=\"top\"><tr valign=\"top\" >",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);
	text += buf;

	for (modidx = 0; settings.parallel_list[modidx]; ++modidx) {
		snprintf(buf, 499,
			 "<td valign=\"top\" width=\"%d%%\" bgcolor=\"#c0c0c0\"><font color=\"%s\" size=\"%+d\"><b>%s</b></font></td>",
			 fraction,
			 settings.bible_verse_num_color,
			 settings.verse_num_font_size + settings.base_font_size,
			 settings.parallel_list[modidx]);
		text += buf;
	}

	text += "</tr>";
	interpolate_parallel_display(text, settings.cvparallel, parallel_count, fraction);
	text += "</table></body></html>";

	snprintf(buf, 499, "%d", ((settings.intCurVerse > 1)
				  ? settings.intCurVerse - 1
				  : settings.intCurVerse));

	HtmlOutput((char *)text.c_str(), widgets.html_parallel_dialog, NULL, buf);
}

/******************************************************************************
 * Name
 *   gui_swap_parallel_with_main
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void gui_swap_parallel_with_main(char * intmod)
 *
 * Description
 *   swaps parallel mod with mod in main text window
 *
 * Return value
 *   void
 */

void main_swap_parallel_with_main(char *intmod)
{
	main_display_bible(intmod, settings.currentverse);
	main_update_parallel_page();
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                             0);
}


/******************************************************************************
 * Name
 *   load_menu_formmod_list
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void load_menu_formmod_list(GtkWidget *pmInt, GList *mods,
 *			gchar *label, GCallback mycallback)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_load_menu_form_mod_list(GtkWidget * pmInt, gchar * label,
				   GCallback mycallback)
{
	GList *tmp = get_list(TEXT_DESC_LIST);
	GtkWidget *item;
	GtkWidget *view_module;
	GtkWidget *view_module_menu;

	view_module = gtk_menu_item_new_with_label(label);
	gtk_widget_show(view_module);
	gtk_container_add(GTK_CONTAINER(pmInt), view_module);

	view_module_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_module),
				  view_module_menu);
/*	view_module_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (view_module_menu));*/
	while (tmp != NULL) {
		item =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_show(item);
		g_signal_connect(G_OBJECT(item), "activate",
				   G_CALLBACK(mycallback),
				   g_strdup((gchar *) tmp->data));

		gtk_container_add(GTK_CONTAINER(view_module_menu),
				  item);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}


/******************************************************************************
 * Name
 *   main_init_parallel_view
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void main_init_parallel_view(void)
 *
 * Description
 *   create a new sword backend for parallel use
 *   call gui_create_parallel_page to create a docked parallel pane
 *   call main_set_parallel_options_at_start to sword global opts
 *
 * Return value
 *   void
 */

void main_init_parallel_view(void)
{
	backend_p = new BackEnd();
}


/******************************************************************************
 * Name
 *   main_delete_parallel_view
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void main_delete_parallel_view(void)
 *
 * Description
 *   delete the sword backend for the parallel view
 *
 * Return value
 *   void
 */

void main_delete_parallel_view(void)
{
	delete backend_p;

}
