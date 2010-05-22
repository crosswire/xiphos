/*
 * Xiphos Bible Study Tool
 * parallel_view.cc - support for displaying multiple modules
 *
 * Copyright (C) 2004-2009 Xiphos Developer Team
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
#ifdef __cplusplus
extern "C" {
#include <gtkhtml/gtkhtml.h>
}
#endif  /* __cplusplus */
#include "gui/html.h"
#endif  /* USE_GTKMOZEMBED */

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

#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><STYLE type=\"text/css\"><!--A { text-decoration:none }--></STYLE></head>"

extern GtkWidget *entrycbIntBook;
extern GtkWidget *sbIntChapter;
extern GtkWidget *sbIntVerse;
extern GtkWidget *entryIntLookup;

/******************************************************************************
 * static
 */
#ifndef USE_GTKMOZEMBED
static GtkHTMLStreamStatus status1;
static GtkHTMLStream *htmlstream;
#endif
static gboolean parallel1;
static gboolean parallel2;
static gboolean parallel3;
static gboolean parallel4;
static gboolean parallel5;

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
	gboolean choice = menuitem->active;
	
	
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

	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_strongs;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*) "Strong's Numbers");
		
	
	item = gtk_check_menu_item_new_with_label(_("Footnotes"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_footnotes;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Footnotes");
	
	
	item = gtk_check_menu_item_new_with_label(_("Morphological Tags"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 

	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_morphs;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*)  "Morphological Tags");
	
	
	item = gtk_check_menu_item_new_with_label(_("Hebrew Vowel Points"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_hebrewpoints;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*)  "Hebrew Vowel Points");
	
	
	item = gtk_check_menu_item_new_with_label(_("Hebrew Cantillation"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_cantillationmarks;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Hebrew Cantillation");
	
	
	item = gtk_check_menu_item_new_with_label(_("Greek Accents"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_greekaccents;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*)  "Greek Accents");
	
	
	item = gtk_check_menu_item_new_with_label(_("Cross-references"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_crossref;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Cross-references");
	
	
	item = gtk_check_menu_item_new_with_label(_("Lemmas"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_lemmas;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*) "Lemmas");
	
	
	item = gtk_check_menu_item_new_with_label(_("Headings"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_headings;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*)  "Headings");
	
	
	item = gtk_check_menu_item_new_with_label(_("Morpheme Segmentation"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_segmentation;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			  (char*) "Morpheme Segmentation" );
	
	
	item = gtk_check_menu_item_new_with_label(_("Words of Christ in Red"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_red_words;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Words of Christ in Red");
	
	
	item = gtk_check_menu_item_new_with_label(_("Transliteration"));
	gtk_widget_show(item);		
	gtk_container_add(GTK_CONTAINER(menu), item); 
	
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_transliteration;
	g_signal_connect(GTK_OBJECT(item), "activate",
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
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_variants_primary;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Primary Reading");
	
	item = gtk_radio_menu_item_new_with_mnemonic (group, _("Secondary Reading" ));
  	group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (item));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(variants_menu), item); 
	GTK_CHECK_MENU_ITEM(item)->active = settings.parallel_variants_secondary;
	g_signal_connect(GTK_OBJECT(item), "activate",
	    G_CALLBACK(main_set_parallel_module_global_options),
			   (char*) "Secondary Reading");
	
	item = gtk_radio_menu_item_new_with_mnemonic (group, _("All Readings" ));
  	group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (item));
	gtk_widget_show(item);	
	gtk_container_add(GTK_CONTAINER(variants_menu), item); 		
	GTK_CHECK_MENU_ITEM (item)->active = settings.parallel_variants_all;
	g_signal_connect(GTK_OBJECT(item), "activate",
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
	if(settings.parallel1Module) 
		parallel1 =
		    main_is_module(settings.parallel1Module);
	else
		parallel1 = FALSE;

	if (settings.parallel2Module)
		parallel2 =
		    main_is_module(settings.parallel2Module);
	else
		parallel2 = FALSE;

	if (settings.parallel3Module)
		parallel3 =
		    main_is_module(settings.parallel3Module);
	else
		parallel3 = FALSE;

	if (settings.parallel4Module)
		parallel4 =
		    main_is_module(settings.parallel4Module);
	else
		parallel4 = FALSE;

	if (settings.parallel5Module)
		parallel5 =
		    main_is_module(settings.parallel5Module);
	else
		parallel5 = FALSE;

}


/******************************************************************************
 * Name
 *   gui_change_int1_mod
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void gui_changeint1_mod(gchar * mod_name)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_change_parallel_module(GSParallel parallel, gchar * mod_name)
{
	if(!mod_name) 
		return;
	switch(parallel) {
	case PARALLEL1:	
		xml_set_value("Xiphos", "modules", "int1",
			      mod_name);
		settings.parallel1Module =
		    xml_get_value("modules", "int1");
		parallel1 =
		    main_is_module(settings.parallel1Module);
		break;
	case PARALLEL2:	
		xml_set_value("Xiphos", "modules", "int2",
			      mod_name);
		settings.parallel2Module =
		    xml_get_value("modules", "int2");
		parallel2 =
		    main_is_module(settings.parallel2Module);
		break;
	case PARALLEL3:	
		xml_set_value("Xiphos", "modules", "int3",
			      mod_name);
		settings.parallel3Module =
		    xml_get_value("modules", "int3");
		parallel3 =
		    main_is_module(settings.parallel3Module);
		break;
	case PARALLEL4:	
		xml_set_value("Xiphos", "modules", "int4",
			      mod_name);
		settings.parallel4Module =
		    xml_get_value("modules", "int4");
		parallel4 =
		    main_is_module(settings.parallel4Module);
		break;
	case PARALLEL5:	
		xml_set_value("Xiphos", "modules", "int5",
			      mod_name);
		settings.parallel5Module =
		    xml_get_value("modules", "int5");
		parallel5 =
		    main_is_module(settings.parallel5Module);
		break;
	default:
		return;
		break;
	}
	if (settings.dockedInt)
		main_update_parallel_page();
	else
		main_update_parallel_page_detached();
}

#ifdef USE_GTKMOZEMBED
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
	gchar *font_size_tmp = NULL, *font_size = NULL;
	gchar *utf8str, *mod_name, *font_name = NULL;
	gint utf8len, i, j;
	gboolean use_gtkhtml_font;
	gboolean is_rtol = FALSE;
	gchar *buf;
	gchar *file = NULL;
	gchar *data = NULL;
	
	
	if (!GTK_WIDGET_REALIZED(GTK_WIDGET(widgets.html_parallel))) return ;
	GeckoHtml *html = GECKO_HTML(widgets.html_parallel);
	gecko_html_open_stream(html,"text/html");
	
	settings.cvparallel = settings.currentverse;
	
	if (settings.havebible) {	
		sprintf(tmpBuf, HTML_START
			"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table>",
			settings.bible_bg_color,
			settings.bible_text_color, settings.link_color);
		
		utf8len = strlen(tmpBuf);
		if (utf8len) {	
			data = g_strconcat(tmpBuf,NULL);
		}

		for (i = 0, j = 0; i < 5; i++) {
			mod_name = NULL;
			switch (i) {
			case 0:
				if (parallel1)
					mod_name =
					    settings.parallel1Module;
				else
					mod_name = NULL;
				break;
			case 1:
				if (parallel2)
					mod_name =
					    settings.parallel2Module;
				else
					mod_name = NULL;
				break;
			case 2:
				if (parallel3)
					mod_name =
					    settings.parallel3Module;
				else
					mod_name = NULL;
				break;
			case 3:
				if (parallel4)
					mod_name =
					    settings.parallel4Module;
				else
					mod_name = NULL;
				break;
			case 4:
				if (parallel5)
					mod_name =
					    settings.parallel5Module;
				else
					mod_name = NULL;
				break;
			}
			GS_message(("mod_name = %s",mod_name));
			if (!mod_name)
				continue;

			++j;
			is_rtol = main_is_mod_rtol(mod_name);
			
			file = g_strdup_printf("%s/fonts.conf", settings.gSwordDir);
			font_name = get_conf_file_item(file, mod_name, "Font");
			if (!font_name || !strcmp(font_name, "none")) {
				font_name =
				    main_get_mod_config_entry(mod_name, "Font");
				if (!font_name)
					font_name = g_strdup("none");
			}
			if (strlen(font_name) < 2) {
				use_gtkhtml_font = TRUE;
			} else {
				if (!strncmp(font_name, "none", 4)) {
					use_gtkhtml_font = TRUE;
				} else {
					use_gtkhtml_font = FALSE;
				}
			}

			font_size_tmp = get_conf_file_item(file, mod_name, "Fontsize");
			g_free(file);
			if (!font_size_tmp) {
				font_size_tmp =
				    main_get_mod_config_entry(mod_name, "Fontsize");
			}

			font_size =
			    g_strdup_printf("%+d",
					    (font_size_tmp ? atoi(font_size_tmp) : 0)
					     + settings.base_font_size);

			if (j == 0 || j == 2 || j == 4)
				rowcolor = "#F1F1F1";
			else
				rowcolor = settings.bible_bg_color;

			if (j == 0) {
				sprintf(tmpBuf,
					"<tr><td><i><font color=\"%s\" size=\"%d\">[%s]</font></i></td></tr>",
					settings.bible_verse_num_color,
					settings.verse_num_font_size + settings.base_font_size,
					settings.currentverse);

				utf8len = strlen(tmpBuf);
				if (utf8len) {
					data = g_strconcat(data, tmpBuf, NULL);	
				}
			}

			sprintf(tmpBuf,
				"<tr bgcolor=\"%s\"><td><b><a href=\"xiphos.url?action=showModInfo&value=%s&module=%s\"><font color=\"%s\" size=\"%+d\"> [%s]</font></a></b>",
				rowcolor,
				main_get_module_description(mod_name),
				mod_name,
				settings.bible_verse_num_color,
				settings.verse_num_font_size + settings.base_font_size,
				mod_name);

			utf8len = strlen(tmpBuf);
			if (utf8len) {
					data = g_strconcat(data, tmpBuf, NULL);	
			}

			if (use_gtkhtml_font)
				sprintf(tmpBuf, "<font size=\"%s\">",
					font_size);
			else
				sprintf(tmpBuf,
					"<font face=\"%s\" size=\"%s\">",
					font_name, font_size);


			utf8len = strlen(tmpBuf);
			if (utf8len) {
				data = g_strconcat(data, tmpBuf, NULL);	
			}

			utf8str =
			    backend_p->get_render_text(mod_name,
							settings.
							currentverse);
			if (is_rtol) {
				buf = g_strdup_printf(
					"%s","<br><DIV ALIGN=right>");			
				if (strlen(buf)) {
					data = g_strconcat(data, buf, NULL);
					free(buf);
				}
			}	
			if (strlen(utf8str)) {
				data = g_strconcat(data, utf8str, NULL);
				free(utf8str);
			}

			if (is_rtol) {
				buf = g_strdup_printf(
					"%s","</DIV><br>");			
				if (strlen(buf)) {
					data = g_strconcat(data, buf, NULL);
					free(buf);
				}
			}	
			sprintf(tmpBuf,
				"</font><small>[<a href=\"xiphos.url?action=showParallel&"
				"type=swap&value=%s\">%s</a>]</small></td></tr>",
				mod_name,_("view context"));

			utf8len = strlen(tmpBuf);
			if (utf8len) {
				data = g_strconcat(data, tmpBuf, NULL);	
			}
		}

		sprintf(tmpBuf, "</table></body></html>");

		utf8len = strlen(tmpBuf);
		if (utf8len) {
			data = g_strconcat(data, tmpBuf, NULL);	
			gecko_html_write(html,data,-1);
		}	
	}
	gecko_html_close(html);
	if (data)
		g_free(data);		
	if (font_name)
		free(font_name);
	if (font_size)
		free(font_size);
	if (font_size_tmp)
		free(font_size_tmp);
}

#else
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
	gchar tmpBuf[256], *rowcolor;
	gchar *font_size_tmp = NULL, *font_size = NULL;
	gchar *utf8str, *mod_name, *font_name = NULL;
	gint utf8len, i, j;
	gboolean was_editable, use_gtkhtml_font;
	gboolean is_rtol = FALSE;
	gchar *buf;
	gchar *file = NULL;

	settings.cvparallel = settings.currentverse;
	
	if (settings.havebible) {
		/* setup gtkhtml widget */
		GtkHTML *html = GTK_HTML(widgets.html_parallel);
		was_editable = gtk_html_get_editable(html);
		if (was_editable)
			gtk_html_set_editable(html, FALSE);
		htmlstream =
		    gtk_html_begin_content(html,
					   (gchar *)"text/html; charset=utf-8");
		sprintf(tmpBuf,
			"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table>",
			settings.bible_bg_color,
			settings.bible_text_color, settings.link_color);
		/*utf8str =
		    e_utf8_from_gtk_string(widgets.html_parallel,
					   tmpBuf);*/
		utf8len = strlen(tmpBuf);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       tmpBuf, utf8len);
		}

		for (i = 0, j = 0; i < 5; i++) {
			mod_name = NULL;
			switch (i) {
			case 0:
				if (parallel1)
					mod_name =
					    settings.parallel1Module;
				else
					mod_name = NULL;
				break;
			case 1:
				if (parallel2)
					mod_name =
					    settings.parallel2Module;
				else
					mod_name = NULL;
				break;
			case 2:
				if (parallel3)
					mod_name =
					    settings.parallel3Module;
				else
					mod_name = NULL;
				break;
			case 3:
				if (parallel4)
					mod_name =
					    settings.parallel4Module;
				else
					mod_name = NULL;
				break;
			case 4:
				if (parallel5)
					mod_name =
					    settings.parallel5Module;
				else
					mod_name = NULL;
				break;
			}
			if (!mod_name)
				continue;
			GS_message(("mod_name = %s", mod_name));

			++j;
			is_rtol = main_is_mod_rtol(mod_name);
			//font_name = get_module_font_name(mod_name);
			file = g_strdup_printf("%s/fonts.conf", settings.gSwordDir);
			font_name = get_conf_file_item(file, mod_name, "Font");
			if (!font_name || !strcmp(font_name, "none")) {
				font_name =
				    main_get_mod_config_entry(mod_name, "Font");
				if (!font_name)
					font_name = g_strdup("none");
			}
			if (strlen(font_name) < 2) {
				use_gtkhtml_font = TRUE;
				GS_message(("use_gtkhtml_font = TRUE"));
			} else {
				if (!strncmp(font_name, "none", 4)) {
					use_gtkhtml_font = TRUE;
					GS_message(("use_gtkhtml_font = TRUE"));
				} else {
					use_gtkhtml_font = FALSE;
					GS_message(("use_gtkhtml_font = FALSE"));
				}
			}
						
			font_size_tmp = get_conf_file_item(file, mod_name, "Fontsize");
			g_free(file);
			if (!font_size_tmp) {
				font_size_tmp =
				    main_get_mod_config_entry(mod_name, "Fontsize");
			}

			font_size =
			    g_strdup_printf("%+d",
					    (font_size_tmp ? atoi(font_size_tmp) : 0)
					     + settings.base_font_size);
		
			if (j == 0 || j == 2 || j == 4)
				rowcolor = (gchar *)"#F1F1F1";
			else
				rowcolor = settings.bible_bg_color;

			if (j == 0) {
				sprintf(tmpBuf,
					"<tr><td><i><font color=\"%s\" size=\"%d\">[%s]</font></i></td></tr>",
					settings.bible_verse_num_color,
					settings.verse_num_font_size + settings.base_font_size,
					settings.currentverse);

				utf8len = strlen(tmpBuf);
				if (utf8len) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream,
						       tmpBuf,
						       utf8len);
				}
			}

			sprintf(tmpBuf,
				"<tr bgcolor=\"%s\"><td><b><a href=\"xiphos.url?action=showModInfo&value=%s&module=%s\"><font color=\"%s\" size=\"%+d\"> [%s]</font></a></b>",
				rowcolor,
				main_get_module_description(mod_name),
				mod_name,
				settings.bible_verse_num_color,
				settings.verse_num_font_size + settings.base_font_size,
				mod_name);

			utf8len = strlen(tmpBuf);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, tmpBuf,
					       utf8len);
			}

			if (use_gtkhtml_font)
				sprintf(tmpBuf, "<font size=\"%s\">",
					font_size);
			else
				sprintf(tmpBuf,
					"<font face=\"%s\"size=\"%s\">",
					font_name, font_size);


			utf8len = strlen(tmpBuf);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, tmpBuf,
					       utf8len);
			}

			utf8str =
			    backend_p->get_render_text(mod_name,
							settings.
							currentverse);
/*#ifdef  DEBUG			
			if(!strcmp(mod_name,"KJV2006"))
				g_message("\nmod: %s\nRAW: %s\nRENDERED: %s", mod_name,
						backend_p->get_raw_text(mod_name,
								settings.currentverse),utf8str);
#endif
*/			
			if(is_rtol) {
				buf = g_strdup_printf(
					"%s","<br><DIV ALIGN=right>");			
				if (strlen(buf)) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream, buf,
						       strlen(buf));
					free(buf);
				}
			}	
			if (strlen(utf8str)) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, utf8str,
					       strlen(utf8str));
				free(utf8str);
			}

			if(is_rtol) {
				buf = g_strdup_printf(
					"%s","</DIV><br>");			
				if (strlen(buf)) {
					gtk_html_write(GTK_HTML(html),
						       htmlstream, buf,
						       strlen(buf));
					free(buf);
				}
			}	
			sprintf(tmpBuf,
				"</font><small>[<a href=\"xiphos.url?action=showParallel&"
				"type=swap&value=%s\">%s</a>]</small></td></tr>",
				mod_name,_("view context"));

			utf8len = strlen(tmpBuf);
			if (utf8len) {
				gtk_html_write(GTK_HTML(html),
					       htmlstream, tmpBuf,
					       utf8len);
			}
			if (font_name)
				free(font_name);
			if (font_size)
				free(font_size);
			if (font_size_tmp)
				free(font_size_tmp);
			
		}

		sprintf(tmpBuf, "</table></body></html>");

		utf8len = strlen(tmpBuf);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       tmpBuf, utf8len);
		}

		gtk_html_end(GTK_HTML(html), htmlstream, status1);
		gtk_html_set_editable(html, was_editable);
	}
}

#endif

/******************************************************************************
 * Name
 *   int_display
 *
 * Synopsis
 *   #include "main/parallel_view.h
 *
 *   void int_display(SWBuf& text, gchar *key, char *mod_name[])
 *
 * Description
 *   carry out the hard work of getting verses for parallel display.
 *
 * Return value
 *   void
 */

static void int_display(SWBuf& text, gchar *key, char *mod_name[])
{
	gchar  	*utf8str,
		*textColor,
		*tmpkey,
		tmpbuf[256],
		*use_font_size[5],
		*font_size_tmp[5],
		*use_font_name[5];
	const gchar *bgColor;
	gchar str[500];
	gboolean evenRow = FALSE;
	gboolean is_rtol = FALSE;
	gchar *file = NULL;
	gint cur_verse, cur_chapter, i = 1, j;
	char *cur_book;
	gboolean is_module[5] = { FALSE, FALSE, FALSE, FALSE, FALSE };

	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(widgets.notebook_bible_parallel))) return;

	// need #verses to process in this book.
	VerseKey vkey;
	int xverses;

	vkey.AutoNormalize(1);
	vkey = key;

#ifdef SWORD_MULTIVERSE
	xverses = (vkey.getVerseMax());
#else
	xverses = (vkey.books[vkey.Testament()-1]
				 [vkey.Book()-1].
				 versemax[vkey.Chapter()-1]);
#endif

	// quick cache of fonts.  (mod_name was passed in.)
	file = g_strdup_printf("%s/fonts.conf", settings.gSwordDir);
	for (j = 0; j < 5; ++j) {
		// determine module presence just once each for this routine.
		is_module[j] = (mod_name[j] &&
				*(mod_name[j]) &&
				backend->is_module(mod_name[j]));

		// collect decorations.
		font_size_tmp[j] = get_conf_file_item(file, mod_name[j], "Fontsize");
		if ((font_size_tmp[j] == NULL) ||
		    !strcmp(font_size_tmp[j], "+0")) {
			g_free(font_size_tmp[j]);
			font_size_tmp[j] =
			    main_get_mod_config_entry(mod_name[j], "Fontsize");
			if ((font_size_tmp[j] == NULL) || (*font_size_tmp[j] == '\0'))
				font_size_tmp[j] = g_strdup("+0");
		}

		use_font_size[j] =
		    g_strdup_printf("%+d",
				    (font_size_tmp[j] ? atoi(font_size_tmp[j]) : 0)
				    + settings.base_font_size);

		use_font_name[j] = get_conf_file_item(file, mod_name[j], "Font");
		if ((use_font_name[j] == NULL) ||
		    !strcmp(use_font_name[j], "none")) {
			use_font_name[j] =
			    main_get_mod_config_entry(mod_name[j], "Font");
			if (!use_font_name[j])
				use_font_name[j] = g_strdup("none");
		}
	}
	g_free(file);

	bgColor = "#f1f1f1";

	tmpkey = backend_p->get_valid_key(key);
	cur_verse = backend_p->key_get_verse(tmpkey);
	settings.intCurVerse = cur_verse;
	cur_chapter = backend_p->key_get_chapter(tmpkey);
	cur_book = backend_p->key_get_book(tmpkey);

	for (i = 1; i <= xverses; ++i) {
		snprintf(tmpbuf, 255, "%s %d:%d", cur_book, cur_chapter, i);
		free(tmpkey);
		tmpkey = backend_p->get_valid_key(tmpbuf);

		text += "<tr valign=\"top\">";

		// mark current verse properly.
		if (i == cur_verse)
			textColor = settings.currentverse_color;
		else
			textColor = settings.bible_text_color;

		// alternate background colors.
		if (evenRow) {
			evenRow = FALSE;
			bgColor = "#f1f1f1";
		} else {
			evenRow = TRUE;
			bgColor = settings.bible_bg_color;
		}

		for (j = 0; j < 5; j++) {
			is_rtol = main_is_mod_rtol(mod_name[j]);

			char *num = main_format_number(i);
			snprintf(str, 499,
				"<td width=\"20%%\" bgcolor=\"%s\">"
				"<a href=\"xiphos.url?action=showParallel&"
				"type=verse&value=%s\" name=\"%d\">"
				"<font color=\"%s\">%s. </font></a>"
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				bgColor,
				main_url_encode(tmpkey),
				i,
				settings.bible_verse_num_color,
				num,
				use_font_name[j],
				use_font_size[j],
				textColor);
			g_free(num);
			text += str;

			if (is_module[j]) {
				if (is_rtol)
					text += "<br><DIV ALIGN=right>";

				utf8str = backend_p->get_render_text
				    (mod_name[j], tmpkey);
				if (strlen(utf8str)) {
					text += utf8str;
					free(utf8str);
				}

				if (is_rtol)
					text += "</DIV>";
			}

			text += "</font></td>";
		}

		text += "</tr>";
	}

	for (j = 0; j < 5; ++j) {
		g_free(use_font_size[j]);
		g_free(font_size_tmp[j]);
		g_free(use_font_name[j]);
	}
	g_free(tmpkey);
	g_free(cur_book);
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
	gchar buf[500], *mod_name[5];
	gint j;
	gchar space[2];  //there is surely a better way?
	space[0] = ' ';
	space[1] = '\0';
	mod_name[0] = (parallel1 ? settings.parallel1Module : space);
	mod_name[1] = (parallel2 ? settings.parallel2Module : space);
	mod_name[2] = (parallel3 ? settings.parallel3Module : space);
	mod_name[3] = (parallel4 ? settings.parallel4Module : space);
	mod_name[4] = (parallel5 ? settings.parallel5Module : space);
    
    	snprintf(buf, 499, "<span color='blue' weight='bold'>%s</span>", mod_name[0]);
    	gtk_label_set_markup(GTK_LABEL(plabels.label_1), buf);
    
    	snprintf(buf, 499, "<span color='blue' weight='bold'>%s</span>", mod_name[1]);
    	gtk_label_set_markup(GTK_LABEL(plabels.label_2), buf);
    
    	snprintf(buf, 499, "<span color='blue' weight='bold'>%s</span>", mod_name[2]);
    	gtk_label_set_markup(GTK_LABEL(plabels.label_3), buf);
    	
    	snprintf(buf, 499, "<span color='blue' weight='bold'>%s</span>", mod_name[3]);
    	gtk_label_set_markup(GTK_LABEL(plabels.label_4), buf);
    
    	snprintf(buf, 499, "<span color='blue' weight='bold'>%s</span>", mod_name[4]);
    	gtk_label_set_markup(GTK_LABEL(plabels.label_5), buf);
    
#ifdef USE_GTKMOZEMBED
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(widgets.html_parallel_dialog))) return;
#endif

	snprintf(buf, 499, HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table align=\"left\" valign=\"top\"><tr valign=\"top\" >",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);
	text += buf;
	
	for (j = 0; j < 5; ++j){
		snprintf(buf, 499,
			"<td valign=\"top\" width=\"20%%\" bgcolor=\"#f1f1f1\"><font color=\"%s\" size=\"%+d\"><b>%s</b></td>",
			settings.bible_verse_num_color, 
			settings.verse_num_font_size + settings.base_font_size,
			mod_name[j]);
		text += buf;
	}

	text += "</tr>";
	int_display(text, settings.cvparallel, mod_name);	
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
#if 0
	if ((settings.parallel5Module) && !strcmp(settings.parallel5Module, intmod)) {
		settings.parallel5Module = xml_get_value("modules", "bible");
	}
	if ((settings.parallel4Module) && !strcmp(settings.parallel4Module, intmod)) {
		settings.parallel4Module = xml_get_value("modules", "bible");
	}
	if ((settings.parallel3Module) && !strcmp(settings.parallel3Module, intmod)) {
		settings.parallel3Module = xml_get_value("modules", "bible");
	}
	if ((settings.parallel2Module) && !strcmp(settings.parallel2Module, intmod)) {
		settings.parallel2Module = xml_get_value("modules", "bible");
	}
	if ((settings.parallel1Module) && !strcmp(settings.parallel1Module, intmod)) {
		settings.parallel1Module = xml_get_value("modules", "bible");
	}
#endif
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
		g_signal_connect(GTK_OBJECT(item), "activate",
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
	gui_create_parallel_page();
	gtk_widget_realize(widgets.html_parallel);
	main_set_parallel_options_at_start();
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
